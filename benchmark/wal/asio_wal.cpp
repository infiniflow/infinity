// POC of asio usage on file IO and timer

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "crc.hpp"

using namespace std;

std::shared_ptr<spdlog::logger> logger;

void
initializeLogger() {
    spdlog::sink_ptr stdoutSink =
            std::make_shared<spdlog::sinks::stdout_sink_mt>();
    logger = std::make_shared<spdlog::logger>("console", stdoutSink);
    spdlog::register_logger(logger);
}

enum class WALType : uint8_t {
    INVALID = 0,
    // -----------------------------
    // Data
    // -----------------------------
    INSERT_TUPLE = 26,
    DELETE_TUPLE = 27,
    UPDATE_TUPLE = 28,
    // -----------------------------
    // Flush
    // -----------------------------
    CHECKPOINT = 99,
    WAL_FLUSH = 100
};

struct WalEntry {
    int64_t lsn;   // each entry's lsn(Log Sequence Number) is strictly
    // increasing by one.
    int32_t size;  // size of payload, excluding the header, round to multi
    // of 4. There's 4 bytes pad just after the payload storing
    // the same value to assist backward iterating.
    uint32_t
            checksum;  // crc32 of the entry, including the header and the payload.
    // User shall populate it before writing to wal.
    WALType type;
    int32_t pad: 24;
    union {
        int64_t txn_id;
        int64_t max_lsn;
    };

    virtual ~WalEntry() = default;
    virtual int32_t
    GetSize() = 0;  // size of the entry, excluding the 4 bytes pad.
};

struct WALInsertTuple : WalEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t len;
    char data[];
    virtual int32_t
    GetSize() { return sizeof(WALInsertTuple) + size; }
};

struct WALDeleteTuple : WalEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t count;
    virtual int32_t
    GetSize() { return sizeof(WALDeleteTuple); }
};

struct WALUpdateTuple : WalEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t len;
    char data[];
    virtual int32_t
    GetSize() { return sizeof(WALUpdateTuple) + size; }
};

struct CheckpointEntry : WalEntry {
    virtual int32_t
    GetSize() { return sizeof(CheckpointEntry); }
};

class Txn {
public:
    int64_t txn_id;
    int64_t begin_ts;
    int64_t commit_ts;
    int64_t lsn;

public:
    void
    Commit() {
        commit_ts = std::chrono::steady_clock::now().time_since_epoch().count();
    };
};

class SeqGenerator {
public:
    // Begin with 1 to avoid distinguish uninitialized value and the minimal
    // valid value.
    SeqGenerator(int64_t begin = 1) : next_seq_(begin) {}
    int64_t
    Generate() { return next_seq_.fetch_add(1); }
    int64_t
    GetLast() { return next_seq_.load() - 1; }

private:
    atomic<int64_t> next_seq_;
};

class TxnManager {
public:
    static TxnManager*
    GetInstance() {
        if(instance_ == nullptr) {
            instance_ = new TxnManager();
        }
        return instance_;
    }
    void
    AddActiveTxn(std::shared_ptr<Txn> txn) {
        std::lock_guard guard(mutex_);
        active_trans_[txn->txn_id] = txn;
    }
    int64_t
    AllocTxnId() { return txn_id_gen_.Generate(); }
    void
    CommitTxn(int64_t txn_id, int64_t lsn) {
        std::shared_ptr<Txn> txn = nullptr;
        {
            std::lock_guard guard(mutex_);
            auto ent = active_trans_.find(txn_id);
            if(ent != active_trans_.end()) {
                txn = ent->second;
            }
        }
        active_trans_.erase(txn_id);
        txn->lsn = lsn;
        txn->Commit();
        logger->info("TxnManager::Commit done for transaction {}", txn_id);
    }

private:
    static TxnManager* instance_;
    std::mutex mutex_;
    std::map<int64_t, std::shared_ptr<Txn>> active_trans_;
    SeqGenerator txn_id_gen_;
};

class WalManager {
public:
    WalManager(boost::asio::io_context& ioc, const std::string& wal_path)
            : running_(true),
              seq_(0),
              ioc_(ioc),
              stream_file_(
                      boost::asio::stream_file(ioc, wal_path,
                                               boost::asio::stream_file::read_write |
                                               boost::asio::stream_file::append |
                                               boost::asio::stream_file::create)) {
        // schedule an immediate Flush()
        ioc.post([this] { Flush(); });
    }

    void
    Stop() { running_.store(false); }

    // Session request to persist an entry. Assuming txn_id of the entry has
    // been initialized.
    void
    WriteEntry(std::shared_ptr<WalEntry> entry) {
        if(running_.load()) {
            mutex_.lock();
            que_.push(entry);
            mutex_.unlock();
        }
    }

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void
    Flush() {
        int seq = seq_.fetch_add(1);
        logger->info("WalManager::Flush {} enter", seq);
        boost::system::error_code ec;
        int written = 0;
        int32_t size_pad = 0;
        int64_t max_lsn = lsn_gen_.GetLast();
        if(!running_.load()) {
            goto QUIT;
        }
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        while(!que2_.empty()) {
            std::shared_ptr<WalEntry> entry = que2_.front();
            size_pad = entry->GetSize();
            entry->lsn = lsn_gen_.Generate();
            max_lsn = entry->lsn;
            logger->info(
                    "WalManager::Flush {} begin writing wal for transaction {}",
                    seq, entry->txn_id);
            boost::asio::write(
                    stream_file_,
                    boost::asio::buffer(entry.get(), entry->GetSize()), ec);
            boost::asio::write(stream_file_, boost::asio::buffer(&size_pad, 4),
                               ec);
            if(ec.failed()) {
                logger->error(
                        "WalManager::Flush {} failed to write wal for transaction "
                        "{}, async_write error: {}",
                        seq, entry->txn_id, ec.to_string());
            } else {
                logger->info(
                        "WalManager::Flush {} done writing wal for transaction {}",
                        seq, entry->txn_id);
            }
            que2_.pop();
            que3_.push(entry);
            written++;
        }
        if(written > 0) {
            logger->info(
                    "WalManager::Flush {} begin syncing wal for {} transactions",
                    seq, written);
            stream_file_.sync_all();
            logger->info(
                    "WalManager::Flush {} done syncing wal for {} transactions",
                    seq, written);
            while(!que3_.empty()) {
                std::shared_ptr<WalEntry> entry = que3_.front();
                // Commit sequently so they get visible in the same order with
                // wal.
                TxnManager::GetInstance()->CommitTxn(entry->txn_id, entry->lsn);
                que3_.pop();
            }
            pending_checkpoint_ += written;
        }
        // Fuzzy checkpoint for every 10 transactions or 20s.
        if(pending_checkpoint_ >= 10 ||
           (checkpoint_ts_ > 0 &&
            std::chrono::steady_clock::now().time_since_epoch().count() -
            checkpoint_ts_ >=
            20000000000)) {
            ioc_.post([this, max_lsn] { Checkpoint(max_lsn); });
            pending_checkpoint_ = 0;
        }
QUIT:
        // Need to ensure all above async tasks be done at this point. However
        // it's false. asio requires to wrap the remained code into a function,
        // and call it from the callback.
        if(written == 0) {
            // async_wait is an initiating function for an @ref
            // asynchronous_operation, and always returns immediately. Wait
            // asynchronously for the sleep duration since no work to do
            logger->info("WalManager::Flush {} schedule a delayed run", seq);
            auto timer = std::make_shared<boost::asio::steady_timer>(ioc_);
            timer->expires_from_now(std::chrono::milliseconds(1000));
            timer->async_wait([this, timer, seq](boost::system::error_code ec) {
                if(ec == boost::asio::error::operation_aborted) {
                    logger->error(
                            "WalManager::Flush {} async_wait has been cancelled.",
                            seq);
                } else if(ec.failed()) {
                    logger->error(
                            "WalManager::Flush {} failed to async_wait. error: {}",
                            seq, ec.to_string());
                } else {
                    Flush();
                }
            });
        } else {
            logger->info("WalManager::Flush {} schedule an immediate run", seq);
            ioc_.post([this] { Flush(); });
        }
        logger->info("WalManager::Flush {} quit", seq);
    }

    // Do checkpoint for transactions which's lsn no larger than the given one.
    void
    Checkpoint(int64_t lsn) {
        // Ensure that at most one instance of a particular async task is
        // running at any given time.
        if(!checkpoint_mutex_.try_lock()) {
            return;
        }
        // Checkponit is heavy and infrequent operation.
        logger->info("WalManager::Checkpoint enter for transactions' lsn <= {}",
                     lsn);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        logger->info("WalManager::Checkpoint quit", lsn);
        checkpoint_ts_ =
                std::chrono::steady_clock::now().time_since_epoch().count();
        checkpoint_mutex_.unlock();
    }

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    atomic<bool> running_;
    atomic<int> seq_;
    boost::asio::io_context& ioc_;
    mutex mutex_;
    queue<std::shared_ptr<WalEntry>> que_, que2_, que3_;
    boost::asio::stream_file stream_file_;
    SeqGenerator lsn_gen_;
    int pending_checkpoint_;
    int64_t checkpoint_ts_;
    mutex checkpoint_mutex_;
};

TxnManager* TxnManager::instance_ = nullptr;
std::shared_ptr<WalManager> wal_manager = nullptr;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(size_t seq) : seq_(seq) {}

    void
    Start() {
        bool expected = false;
        bool changed = running_.compare_exchange_strong(expected, true);
        if(!changed)
            return;
        std::unique_ptr<std::thread> th(
                new std::thread([this] { Generating(); }));
        t_ = std::move(th);
    }

    void
    Stop() {
        bool expected = true;
        bool changed = running_.compare_exchange_strong(expected, false);
        if(!changed)
            return;
        t_->join();
    }

    void
    Generating() {
        while(running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if(wal_manager != nullptr) {
                auto txn = std::make_shared<Txn>();
                txn->txn_id = TxnManager::GetInstance()->AllocTxnId();
                txn->begin_ts =
                        std::chrono::steady_clock::now().time_since_epoch().count();
                txn->commit_ts = 0;
                TxnManager::GetInstance()->AddActiveTxn(txn);
                logger->info("session {} generated transaction {}", seq_,
                             txn->txn_id);
                auto ent = std::make_shared<WALInsertTuple>();
                ent->type = WALType::INSERT_TUPLE;
                ent->txn_id = txn->txn_id;
                ent->checksum = 0;
                // CRC32IEEE is equivalent to boost::crc_32_type on
                // little-endian machine.
                ent->checksum = infinity::CRC32IEEE::makeCRC(
                        reinterpret_cast<const unsigned char*>(ent.get()),
                        ent->GetSize());
                wal_manager->WriteEntry(ent);
            }
        }
    }

private:
    size_t seq_;
    std::unique_ptr<std::thread> t_;
    atomic<bool> running_;
};

int
main(int argc, char* argv[]) {
    if(argc != 4) {
        std::cout << "Usage: " << argv[0]
                  << " <sessions> <ioc_threads> <wal_path>" << std::endl;
        return 1;
    }

    std::size_t sessions = std::atoi(argv[1]);
    std::size_t ioc_threads = std::atoi(argv[2]);
    std::string wal_path(argv[3]);

    initializeLogger();

    boost::asio::io_context ioc;
    wal_manager = std::make_shared<WalManager>(ioc, wal_path);

    std::vector<std::unique_ptr<std::thread>> m_threads;
    // All threads from the pool will be used to call the corresponding
    // asynchronous operation completion callbacks.
    for (int i = 0; i < (int)ioc_threads; i++) {
        std::unique_ptr<std::thread> th(new std::thread([&ioc]() {
            while(!ioc.stopped()) {
                // Process asynchronous operations till all are completed.
                ioc.run();
                // Sleep for a short duration to avoid busy waiting.
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }));
        m_threads.push_back(std::move(th));
    }

    std::vector<Session*> sesses;
    for (int i = 0; i != (int)sessions; i++) {
        Session* sess = new Session(i);
        sesses.emplace_back(sess);
        sess->Start();
    }

    std::this_thread::sleep_for(std::chrono::seconds(30));

    // Stop generating transactions. This allows the I/O threads to exit the
    // event loop when there are no more pending asynchronous operations.
    for (int i = 0; i != (int)sessions; ++i) {
        sesses[i]->Stop();
        delete sesses[i];
    }
    // Waiting for the I/O threads to exit.
    for (int i = 0; i < (int)ioc_threads; i++) {
        m_threads[i]->join();
    }

    return 0;
}

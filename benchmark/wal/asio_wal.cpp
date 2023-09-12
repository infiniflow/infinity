// POC of asio usage on file IO and timer

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

using namespace std;

std::shared_ptr<spdlog::logger> logger;

void initializeLogger()
{
    spdlog::sink_ptr stdoutSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
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

struct WALEntry {
    WALType type;
    int64_t lsn;
    virtual int64_t GetSize() = 0;
};

struct WALInsertTuple : WALEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t size;
    char data[];
    virtual int64_t GetSize() { return sizeof(WALInsertTuple) + size; }
};

struct WALDeleteTuple : WALEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t count;
    virtual int64_t GetSize() { return sizeof(WALDeleteTuple); }
};

struct WALUpdateTuple : WALEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t size;
    char data[];
    virtual int64_t GetSize() { return sizeof(WALUpdateTuple) + size; }
};

class Txn {
public:
    int64_t lsn;
    int64_t begin_ts;
    int64_t commit_ts;

public:
    void Commit()
    {
        commit_ts = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    };
};

class TxnManager {
public:
    static TxnManager* GetInstance()
    {
        if (instance_ == nullptr) {
            instance_ = new TxnManager();
        }
        return instance_;
    }
    void AddActiveTxn(std::shared_ptr<Txn> txn)
    {
        std::lock_guard guard(mutex_);
        active_trans_[txn->lsn] = txn;
    }
    void CommitTxn(int64_t lsn)
    {
        std::shared_ptr<Txn> txn = nullptr;
        {
            std::lock_guard guard(mutex_);
            auto ent = active_trans_.find(lsn);
            if (ent != active_trans_.end()) {
                txn = ent->second;
            }
        }
        active_trans_.erase(lsn);
        txn->Commit();
        logger->info("TxnManager::CommitTxn done for transaction {}", lsn);
    }

private:
    static TxnManager* instance_;
    std::mutex mutex_;
    std::map<int64_t, std::shared_ptr<Txn>> active_trans_;
};

class LsnGenerator {
public:
    static int64_t GetNextLsn()
    {
        return next_lsn_++;
    }

private:
    static int64_t next_lsn_;
};

class WalManager {
public:
    WalManager(boost::asio::io_context& ioc, const std::string& wal_path)
        : running_(true)
        , seq_(0)
        , ioc_(ioc)
        , stream_file_(boost::asio::stream_file(ioc, wal_path, boost::asio::stream_file::read_write | boost::asio::stream_file::append | boost::asio::stream_file::create))
    {
        // schedule an immediate Flush()
        ioc.post([this] { Flush(); });
    }

    void Stop()
    {
        running_.store(false);
    }

    void WriteEntry(std::shared_ptr<WALEntry> entry)
    {
        if (running_.load()) {
            mutex_.lock();
            que_.push(entry);
            mutex_.unlock();
        }
    }

    // Flush is scheduled regularly. It collects a batch of transactions, sync wal and do parallel committing.
    // Each sync cost ~1s. Each checkpoint cost ~10s.
    // So it's necessary to sync for a batch of transactions, and to checkpoint for a batch of sync.
    void Flush()
    {
        int seq = seq_.fetch_add(1);
        logger->info("WalManager::Flush {} enter", seq);
        boost::system::error_code ec;
        int written = 0;
        if (!running_.load()) {
            goto QUIT;
        }
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        while (!que2_.empty()) {
            std::shared_ptr<WALEntry> entry = que2_.front();
            logger->info("WalManager::Flush {}  begin writing wal for transaction {}", seq, entry->lsn);
            boost::asio::write(stream_file_, boost::asio::buffer(entry.get(), entry->GetSize()), ec);
            if (ec.failed()) {
                logger->error("WalManager::Flush {} failed to write wal for transaction {}, async_write error: {}", seq, entry->lsn, ec.to_string());
            } else {
                logger->info("WalManager::Flush {} done writing wal for transaction {}", seq, entry->lsn);
                // schedule a commit
                ioc_.post([entry] { TxnManager::GetInstance()->CommitTxn(entry->lsn); });
            }
            que2_.pop();
            written++;
        }
        if (written > 0) {
            logger->info("WalManager::Flush {} begin syncing wal for {} transactions", seq, written);
            stream_file_.sync_all();
            logger->info("WalManager::Flush {} done syncing wal for {} transactions", seq, written);
            if (written >= 10) {
                // TODO: do checkpoint?
            }
        }
    QUIT:
        // Need to ensure all above async tasks be done at this point. However it's false.
        // asio requires to wrap the remained code into a function, and call it from the callback.
        if (written == 0) {
            // async_wait is an initiating function for an @ref asynchronous_operation, and always returns immediately.
            // Wait asynchronously for the sleep duration since no work to do
            logger->info("WalManager::Flush {} schedule a delayed run", seq);
            auto timer = std::make_shared<boost::asio::steady_timer>(ioc_);
            timer->expires_from_now(std::chrono::milliseconds(1000));
            timer->async_wait([this, timer, seq](boost::system::error_code ec) {
                if (ec == boost::asio::error::operation_aborted) {
                    logger->error("WalManager::Flush {} async_wait has been cancelled.", seq);
                } else if (ec.failed()) {
                    logger->error("WalManager::Flush {} failed to async_wait. error: {}", seq, ec.to_string());
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

    void
    Sync()
    {
    }

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue and do serial writing.
    atomic<bool> running_;
    atomic<int> seq_;
    boost::asio::io_context& ioc_;
    mutex mutex_;
    queue<std::shared_ptr<WALEntry>> que_, que2_;
    boost::asio::stream_file stream_file_;
};

int64_t LsnGenerator::next_lsn_ = 0;
TxnManager* TxnManager::instance_ = nullptr;
std::shared_ptr<WalManager> wal_manager = nullptr;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(size_t seq)
        : seq_(seq)
    {
    }

    void Start()
    {
        bool expected = false;
        bool changed = running_.compare_exchange_strong(expected, true);
        if (!changed)
            return;
        std::unique_ptr<std::thread> th(new std::thread([this] { Generating(); }));
        t_ = std::move(th);
    }

    void Stop()
    {
        bool expected = true;
        bool changed = running_.compare_exchange_strong(expected, false);
        if (!changed)
            return;
        t_->join();
    }

    void Generating()
    {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if (wal_manager != nullptr) {
                auto txn = std::make_shared<Txn>();
                txn->lsn = LsnGenerator::GetNextLsn();
                txn->begin_ts = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                txn->commit_ts = 0;
                TxnManager::GetInstance()->AddActiveTxn(txn);
                logger->info("session {} generated transaction {}", seq_, txn->lsn);
                auto ent = std::make_shared<WALInsertTuple>();
                ent->type = WALType::INSERT_TUPLE;
                ent->lsn = txn->lsn;
                wal_manager->WriteEntry(ent);
            }
        }
    }

private:
    size_t seq_;
    std::unique_ptr<std::thread> t_;
    atomic<bool> running_;
};

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <sessions> <ioc_threads> <wal_path>" << std::endl;
        return 1;
    }

    std::int32_t sessions = std::atoi(argv[1]);
    std::int32_t ioc_threads = std::atoi(argv[2]);
    std::string wal_path(argv[3]);

    initializeLogger();

    boost::asio::io_context ioc;
    wal_manager = std::make_shared<WalManager>(ioc, wal_path);

    std::vector<std::unique_ptr<std::thread>> m_threads;
    // All threads from the pool will be used to call the corresponding asynchronous operation completion callbacks.
    for (std::size_t i = 0; i < ioc_threads; i++) {
        std::unique_ptr<std::thread> th(
            new std::thread([&ioc]() {
                while (!ioc.stopped()) {
                    ioc.run();
                }
            }));
        m_threads.push_back(std::move(th));
    }

    std::vector<Session*> sesses;
    for (std::size_t i = 0; i != sessions; i++) {
        Session* sess = new Session(i);
        sesses.emplace_back(sess);
        sess->Start();
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop generating transactions. This allows the I/O threads to exit the event loop when there are no more pending asynchronous operations.
    for (std::size_t i = 0; i != sessions; ++i) {
        sesses[i]->Stop();
        delete sesses[i];
    }
    // Waiting for the I/O threads to exit.
    for (std::size_t i = 0; i < ioc_threads; i++) {
        m_threads[i]->join();
    }

    return 0;
}

// POC of asio usage on file IO and timer

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <queue>
#include <set>
#include <map>
#include <atomic>
#include <memory>
#include <boost/asio.hpp>

using namespace std;

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
    virtual int64_t GetSize()=0;
};

struct WALInsertTuple : WALEntry {
    int16_t columnId;
    int64_t row_start;
    int64_t size;
    char data[];
    virtual int64_t GetSize() { return sizeof(WALInsertTuple) + size; }
};

struct WALDeleteTuple : WALEntry{
    int16_t columnId;
    int64_t row_start;
    int64_t count;
    virtual int64_t GetSize() { return sizeof(WALDeleteTuple); }
};

struct WALUpdateTuple : WALEntry{
    int16_t columnId;
    int64_t row_start;
    int64_t size;
    char data[];
    virtual int64_t GetSize() { return sizeof(WALUpdateTuple) + size; }
};


class TxnManager;

class Txn{
public:
    int64_t lsn;
    int64_t begin_ts;
    int64_t commit_ts;
public:
    void Commit(){
        commit_ts = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    };
};

class TxnManager{
public:
    static TxnManager * GetInstance(){
        if(instance_ == nullptr){
            instance_ = new TxnManager();
        }
        return instance_;
    }
    std::shared_ptr<Txn> GetActiveTxn(int64_t lsn){
        std::lock_guard guard(mutex_);
        auto ent = active_trans_.find(lsn);
        if(ent != active_trans_.end()){
            return ent->second;
        }
        return nullptr;
    }
    void AddActiveTxn(std::shared_ptr<Txn> txn){
        std::lock_guard guard(mutex_);
        active_trans_[txn->lsn] = txn;
    }
    void CommitTxn(int64_t lsn){
        std::shared_ptr<Txn> txn = nullptr;
        {
            std::lock_guard guard(mutex_);
            auto ent = active_trans_.find(lsn);
            if(ent != active_trans_.end()){
                txn = ent->second;
            }
        }
        active_trans_.erase(lsn);
        txn->Commit();
    }
private:
    static TxnManager* instance_;
    std::mutex mutex_;
    std::map<int64_t, std::shared_ptr<Txn>> active_trans_;
};


class LsnGenerator{
public:
    static int64_t GetNextLsn(){
        return next_lsn_++;
    }
private:
    static int64_t next_lsn_;
};


int64_t next_lsn_;
map<int64_t, std::shared_ptr<Txn>> TxnManager::active_trans_;
TxnManager::instance_ = nullptr;
class WalManager;
WalManager * wal_manager = nullptr;


class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(size_t seq): seq_(seq), t_([this] { Generating(); }) { }

    void Start() {
        running_ = true;
    }

    void Stop() {
        running_ = false;
        t_.join();
    }

    void Generating()
    {
        while(running_){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if(wal_manager != nullptr){
                auto txn = new Txn{
                    .lsn = LsnGenerator::GetNextLsn(),
                    .begin_ts = std::chrono::high_resolution_clock::now().time_since_epoch().count(),
                    .commit_ts = 0
                };
                TxnManager::active_trans_[txn->lsn] = txn;
                auto ent = std::make_shard<WALInsertTuple>();
                ent->type = WALType::INSERT_TUPLE;
                ent->lsn = txn.lsn;
                wal_manager->WriteEntry(ent);
            }
        }
    }

private:
    size_t seq_;
    std::thread t_;
    bool running_;
};


class WalManager
{
public:
    WalManager(boost::asio::io_context & ioc, const std::string& wal_path)
    : running_(true), ioc_(ioc), stream_file_(boost::asio::stream_file(ioc, wal_path, boost::asio::stream_file::read_write)){
        // schedule an immediate Flush()
        ioc.post([this] { Flush(); });
    }

    void WriteEntry(std::shared_ptr<WALEntry> entry) {
        if(running_.load()){
            mutex_.lock();
            que_.push(entry);
            mutex_.unlock();
        }
    }

    // Flush is scheduled only once. It collects a batch of transactions, sync wal and do parallel committing.
    // Each sync cost ~1s. Each checkpoint cost ~10s.
    // So it's necessary to sync for a batch of transactions, and to checkpoint for a batch of sync.
    void Flush() {
        while(running_.load()){
            mutex_.lock();
            que_.swap(que2_);
            mutex_.unlock();
            int written = 0;
            while (!que2_.empty()){
                std::shared_ptr<WALEntry> entry = que2_.front();
                boost::asio::async_write(
                    stream_file_, boost::asio::buffer(entry, entry->GetSize()), [this, entry](boost::system::error_code ec, std::size_t length) {
                    if (!ec)
                    {
                        std::cout << "async_write error: " << ec << std::endl;
                    } else {
                        std::shared_ptr<Txn> txn = TxnManager::GetInstance()->GetActiveTxn(entry->lsn);
                        if(txn != nullptr) {
                            // schedule a commit
                            ioc_.post([txn] { txn->Commit(); });
                        }
                    }
                });
                que2_.pop();
                written++;
            }
            if (written == 0) {
                // Wait asynchronously for the sleep duration since no work to do
                boost::asio::steady_timer timer(ioc_, std::chrono::milliseconds(100));
                timer.async_wait([this](boost::system::error_code ec) {});
            } else {
                stream_file_.sync_all();
                if(written >= 10){
                    //TODO: do checkpoint?
                }
            }
        }
    }


private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue and do serial writing.
    atomic<bool> running_;
    boost::asio::io_context& ioc_;
    mutex mutex_;
    queue<std::shared_ptr<WALEntry>> que_, que2_;
    boost::asio::stream_file stream_file_;
};

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << "<sessions>" << " <wal_path>" << std::endl;
        return 1;
    }

    std::int32_t sessions = std::atoi(argv[1]);
    std::string wal_path(argv[2]);
    
    std::vector<Session*> sesses;
    for (std::size_t i = 0; i != sessions; ++i){
        Session *sess = new Session(i);
		sesses.emplace_back(sess);
        sess->Start();
	}

    boost::asio::io_context ioc;
    WalManager wal_manager(ioc, wal_path);
    ioc.run();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    ioc.stop();
    return 0;
}

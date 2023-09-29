//
// Created by jinhai on 23-5-16.
//

#include "wal_manager.h"
#include "common/utility/exception.h"
#include "main/logger.h"
#include "storage/storage.h"
#include <filesystem>
#include <vector>

namespace infinity {

using namespace std;
namespace fs = std::filesystem;

WalManager::WalManager(Storage* storage, const std::string& wal_path)
        : storage_(storage), wal_path_(wal_path), running_(false) {}

WalManager::~WalManager() {
    Stop();
    while(!que_.empty()) {
        que_.pop();
    }
    while(!que2_.empty()) {
        que2_.pop();
    }
    while(!que3_.empty()) {
        que3_.pop();
    }
}

void
WalManager::Start() {
    bool expected = false;
    bool changed = running_.compare_exchange_strong(expected, true);
    if(!changed)
        return;
    fs::path wal_dir = fs::path(wal_path_).parent_path();
    if(!fs::exists(wal_dir)) {
        fs::create_directory(wal_dir);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if(!ofs_.is_open()) {
        throw Exception("Failed to open wal file: " + wal_path_);
    }
    flush_thread_ = std::thread([this] { Flush(); });
    checkpoint_thread_ = std::thread([this] { Checkpoint(); });
}

void
WalManager::Stop() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if(!changed)
        return;
    flush_thread_.join();
    checkpoint_thread_.join();
    ofs_.close();
}

// Session request to persist an entry. Assuming txn_id of the entry has
// been initialized.
int
WalManager::PutEntry(std::shared_ptr<WalEntry> entry) {
    if(!running_.load()) {
        return -1;
    }
    mutex_.lock();
    que_.push(entry);
    mutex_.unlock();
    return 0;
}

// Flush is scheduled regularly. It collects a batch of transactions, sync
// wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
// ~10s. So it's necessary to sync for a batch of transactions, and to
// checkpoint for a batch of sync.
void
WalManager::Flush() {
    while(running_.load()) {
        int written = 0;
        int32_t size_pad = 0;
        int64_t max_commit_ts = 0;
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        while(!que2_.empty()) {
            std::shared_ptr<WalEntry> entry = que2_.front();
            max_commit_ts = entry->commit_ts;
            LOG_TRACE("WalManager::Flush begin writing wal for transaction {}",
                      entry->txn_id);
            int32_t exp_size = entry->GetSizeInBytes();
            vector<char> buf(exp_size);
            char* ptr = buf.data();
            entry->WriteAdv(ptr);
            int32_t act_size = ptr - buf.data();
            if(exp_size != act_size)
                LOG_ERROR("WalManager::Flush WalEntry estimated size {} differ "
                          "with the actual one {}",
                          exp_size, act_size);
            ofs_.write(buf.data(), ptr - buf.data());
            LOG_TRACE("WalManager::Flush done writing wal for transaction {}",
                      entry->txn_id);
            que2_.pop();
            que3_.push(entry);
            written++;
        }
        if(written == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            LOG_TRACE("WalManager::Flush begin syncing wal for {} transactions",
                      written);
            ofs_.flush();
            LOG_TRACE("WalManager::Flush done syncing wal for {} transactions",
                      written);
            TxnManager* txn_mgr = storage_->txn_manager();
            while(!que3_.empty()) {
                std::shared_ptr<WalEntry> entry = que3_.front();
                // Commit sequently so they get visible in the same order
                // with wal.
                Txn* txn = txn_mgr->GetTxn(entry->txn_id);
                if(txn != nullptr) {
                    txn->CommitTxnBottom();
                }
                que3_.pop();
            }
            // Check if the wal file is too large.
            auto file_size = fs::file_size(wal_path_);
            if(file_size > kWALFileSizeThreshold) {
                this->SwapWALFile(max_commit_ts);
            }
            lsn_pend_chk_.store(max_commit_ts);
        }
    }
}

// Do checkpoint for transactions which's lsn no larger than the given one.
void
WalManager::Checkpoint() {
    // Fuzzy checkpoint for every 10 transactions or 20s.
    while(running_.load()) {
        int64_t lsn_pend_chk = lsn_pend_chk_.load();
        if(lsn_pend_chk - lsn_done_chk_ < 10 ||
           (checkpoint_ts_ > 0 &&
            std::chrono::steady_clock::now().time_since_epoch().count() -
            checkpoint_ts_ <
            20000000000)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Checkponit is heavy and infrequent operation.
        LOG_INFO("WalManager::Checkpoint enter for transactions' lsn <= {}",
                 lsn_pend_chk);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        LOG_INFO("WalManager::Checkpoint quit", lsn_pend_chk);
        checkpoint_ts_ =
                std::chrono::steady_clock::now().time_since_epoch().count();
    }
}


/**
 * @brief Swap the wal file to a new one.
 * We will swap a new wal file when the current wal file is too large.
 * Just rename the current wal file to a new one, and create a new wal file with the original name.
 * So we only focus on the current wal file: wal.log
 * When replaying the wal file, we will just start with the wal.log file.
 * @param max_commit_ts The max commit timestamp of the transactions in the current wal file.
 */
void
WalManager::SwapWALFile(const int64_t max_commit_ts) {
    if(ofs_.is_open()) {
        ofs_.close();
    }

    fs::path old_file_path = fs::path(wal_path_);

    String new_file_path = old_file_path.string() + '.' + std::to_string(max_commit_ts);
    LOG_INFO("WAL NEW PATH: {}", new_file_path.c_str());

    // Rename the current wal file to a new one.
    fs::rename(wal_path_, new_file_path);

    // Create a new wal file with the original name.
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if(!ofs_.is_open()) {
        throw StorageException("Failed to open wal file: " + wal_path_);
    }
}
/**
 * @brief Replay the wal file.
 *
 * @return int64_t The max commit timestamp of the transactions in the wal file.
 *
 */
int64_t
WalManager::ReplayWALFile() {
    // if the wal directory does not exist, just return 0.
    if(!fs::exists(wal_path_)) {
        return 0;
    }
    // if the wal file is empty, just return 0.
    if(fs::file_size(wal_path_) == 0) {
        return 0;
    }
    // if the wal file is not empty, we will replay the wal file.
    // 1. First , we will get all wal files store to wal_list_.
    // wal_list_ is a vector of string, each string is a wal file path.
    // wal_list_ is sorted by the suffix of the wal file path.
    // e.g. wal_list_ = {wal.log.1, wal.log.2, wal.log.3}
    LOG_INFO("WAL PATH: {}", wal_path_.c_str());
    for(const auto& entry: fs::directory_iterator(fs::path(wal_path_).parent_path())) {
        if(entry.is_regular_file()) {
            wal_list_.push_back(entry.path().string());
        }
    }
    std::sort(wal_list_.begin(), wal_list_.end(), [](const String& a, const String& b) {
        return a.substr(a.find_last_of('.') + 1) > b.substr(b.find_last_of('.') + 1);
    });

    // log the wal files.
    for(const auto& wal_file: wal_list_) {
        LOG_INFO("WAL FILE: {}", wal_file.c_str());
    }

    // 2. Replay the wal files one by one.
    // We will replay the wal files in the order of wal_list_.
    // First we check the wal.log file, if it exists, we will replay it first.
    // Then we check the wal.log.1 file, if it exists, we will replay it.

    // 2.1 Get the max commit timestamp of the transactions in the wal files.
    int64_t max_commit_ts = 0;
    // 2.2 Replay the wal files one by one.
    for(const auto& wal_file: wal_list_) {
        // 2.2.1 Open the wal file.

        std::ifstream ifs(wal_file, std::ios::binary | std::ios::ate);
        if(!ifs.is_open()) {
            throw StorageException("Failed to open wal file: " + wal_file);
        }
        std::streamsize size = ifs.tellg();
        ifs.seekg(size - 4, std::ios::beg);
        int32_t entry_size;

        ifs.read(reinterpret_cast<char*>(&entry_size), 4);

        ifs.seekg(size - 4 - (entry_size - 4), std::ios::beg);

        std::vector<char> buf(entry_size);

        ifs.read(buf.data(), entry_size);

        ifs.close();

        char* ptr = buf.data();

        auto entry = WalEntry::ReadAdv(ptr, entry_size);

        max_commit_ts = entry->commit_ts;

        auto commands = entry->cmds;


        return max_commit_ts;
    }

    // 2.2.2 Read the wal file.
    // Get the last entry of the wal file.
    // the entry's tail is 4B, which is the size of the entry.
    // So we can get the entry's size.

    // Get the entry's content.
    // The entry's content is entry_size bytes.

    // So we can get the entry's content.
    // Deserialize the entry.

}

} // namespace infinity
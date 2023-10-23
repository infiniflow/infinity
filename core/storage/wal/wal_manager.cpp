//
// Created by jinhai on 23-5-16.
//

module;

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <thread>

import stl;
import logger;
import third_party;
import txn_manager;
import txn;
import storage;
import wal_entry;
import infinity_exception;
import infinity_assert;

module wal_manager;

namespace infinity {

// using namespace std;
// namespace std::filesystem = std::filesystem;

WalManager::WalManager(Storage *storage,
                       const String &wal_path,
                       u64 wal_size_threshold,
                       u64 full_checkpoint_time_interval,
                       u64 full_checkpoint_txn_interval,
                       u64 delta_checkpoint_time_interval,
                       u64 delta_checkpoint_txn_interval)
    : storage_(storage), wal_path_(wal_path), wal_size_threshold_(wal_size_threshold), full_checkpoint_time_interval_(full_checkpoint_time_interval),
      full_checkpoint_txn_interval_(full_checkpoint_txn_interval), delta_checkpoint_time_interval_(delta_checkpoint_time_interval),
      delta_checkpoint_txn_interval_(delta_checkpoint_txn_interval), running_(false) {}

WalManager::~WalManager() {
    Stop();
    while (!que_.empty()) {
        que_.pop();
    }
    while (!que2_.empty()) {
        que2_.pop();
    }
    while (!que3_.empty()) {
        que3_.pop();
    }
}

void WalManager::Start() {
    bool expected = false;
    bool changed = running_.compare_exchange_strong(expected, true);
    if (!changed)
        return;
    Path wal_dir = Path(wal_path_).parent_path();
    if (!std::filesystem::exists(wal_dir)) {
        std::filesystem::create_directory(wal_dir);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        Error<StorageException>(Format("Failed to open wal file: {}", wal_path_), __FILE_NAME__, __LINE__);
    }
    flush_thread_ = Thread([this] { Flush(); });
    checkpoint_thread_ = Thread([this] { Checkpoint(); });
}

void WalManager::Stop() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("WalManager::Stop already stopped");
        return;
    }

    LOG_INFO("WalManager::Stop begin to stop txn manager");
    // Notify txn manager to stop.
    TxnManager *txn_mgr = storage_->txn_manager();
    txn_mgr->Stop();

    // pop all the entries in the queue. and notify the condition variable.
    LockGuard<Mutex> guard(mutex_);
    while (!que_.empty()) {
        auto wal_entry = que_.front();
        Txn *txn = txn_mgr->GetTxn(wal_entry->txn_id);
        if (txn != nullptr) {
            txn->CancelCommitTxnBottom();
        }
        que_.pop();
    }

    // Wait for checkpoint thread to stop.
    LOG_INFO("WalManager::Stop checkpoint thread join");
    checkpoint_thread_.join();

    // Wait for flush thread to stop
    LOG_INFO("WalManager::Stop flush thread join");
    flush_thread_.join();

    ofs_.close();
}

// Session request to persist an entry. Assuming txn_id of the entry has
// been initialized.
int WalManager::PutEntry(SharedPtr<WalEntry> entry) {
    if (!running_.load()) {
        return -1;
    }
    int rc = 0;
    mutex_.lock();
    if (running_.load()) {
        que_.push(entry);
        rc = -1;
    }
    mutex_.unlock();
    return rc;
}

// Flush is scheduled regularly. It collects a batch of transactions, sync
// wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
// ~10s. So it's necessary to sync for a batch of transactions, and to
// checkpoint for a batch of sync.
void WalManager::Flush() {
    while (running_.load()) {
        int written = 0;
        int total = 0;
        int32_t size_pad = 0;
        int64_t max_commit_ts = 0;
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        while (!que2_.empty()) {
            SharedPtr<WalEntry> entry = que2_.front();
            max_commit_ts = entry->commit_ts;
            if (!entry->cmds.empty()) {
                // Don't need to write empty WalEntry (read-only transactions).
                LOG_TRACE(Format("WalManager::Flush begin writing wal for transaction {}", entry->txn_id));
                int32_t exp_size = entry->GetSizeInBytes();
                Vector<char> buf(exp_size);
                char *ptr = buf.data();
                entry->WriteAdv(ptr);
                int32_t act_size = ptr - buf.data();
                if (exp_size != act_size)
                    LOG_ERROR(Format("WalManager::Flush WalEntry estimated size {} differ "
                                     "with the actual one {}",
                                     exp_size,
                                     act_size));
                ofs_.write(buf.data(), ptr - buf.data());
                LOG_TRACE(Format("WalManager::Flush done writing wal for transaction {}", entry->txn_id));
                written++;
            }
            que2_.pop();
            que3_.push(entry);
            total++;
        }
        if (total == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            if (written > 0) {
                LOG_TRACE(Format("WalManager::Flush begin syncing wal for {} transactions", written));
                ofs_.flush();
                LOG_TRACE(Format("WalManager::Flush done syncing wal for {} transactions", written));
            }
            TxnManager *txn_mgr = storage_->txn_manager();
            while (!que3_.empty()) {
                SharedPtr<WalEntry> entry = que3_.front();
                // Commit sequentially so they get visible in the same order
                // with wal.
                Txn *txn = txn_mgr->GetTxn(entry->txn_id);
                if (txn != nullptr) {
                    txn->CommitTxnBottom();
                }
                que3_.pop();
            }

            // Check if the wal file is too large.
            try {
                auto file_size = std::filesystem::file_size(wal_path_);
                if (file_size > wal_size_threshold_) {
                    this->SwapWalFile(max_commit_ts);
                }

            } catch (std::exception &e) {
                LOG_WARN(e.what());
            } catch (...) {
                LOG_WARN("WalManager::Flush threads get exception");
                return;
            }
            commit_ts_pend_.store(max_commit_ts);
        }
    }
}

// Do checkpoint for transactions which's lsn no larger than the given one.
void WalManager::Checkpoint() {
    // Fuzzy checkpoint for every 10 transactions or 20s.
    while (running_.load()) {
        TxnTimeStamp commit_ts_pend = commit_ts_pend_.load();
        if (commit_ts_pend - commit_ts_done_ < full_checkpoint_txn_interval_ ||
            (checkpoint_ts_ > 0 && std::chrono::steady_clock::now().time_since_epoch().count() - checkpoint_ts_ < full_checkpoint_time_interval_)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Checkponit is heavy and infrequent operation.
        LOG_INFO(Format("WalManager::Checkpoint enter for transactions' lsn <= {}", commit_ts_pend));

        TxnManager *txn_mgr = nullptr;
        Txn *txn = nullptr;
        try {
            txn_mgr = storage_->txn_manager();
            txn = txn_mgr->CreateTxn();
            txn->BeginTxn();
            txn->Checkpoint(commit_ts_pend);
            txn->CommitTxn();
        } catch (std::exception &e) {
            LOG_WARN(e.what());
            return;
        } catch (...) {
            LOG_WARN("WalManager::Checkpoint failed to commit checkpoint txn");
            return;
        }

        commit_ts_done_ = commit_ts_pend;

        LOG_INFO(Format("WalManager::Checkpoint quit, {}", commit_ts_done_));
        checkpoint_ts_ = std::chrono::steady_clock::now().time_since_epoch().count();

        RecycleWalFile();
    }
}

/**
 * @brief Swap the wal file to a new one.
 * We will swap a new wal file when the current wal file is too large.
 * Just rename the current wal file to a new one, and create a new wal file with
 * the original name. So we only focus on the current wal file: wal.log When
 * replaying the wal file, we will just start with the wal.log file.
 * @param max_commit_ts The max commit timestamp of the transactions in the
 * current wal file.
 */
void WalManager::SwapWalFile(const TxnTimeStamp max_commit_ts) {
    if (ofs_.is_open()) {
        ofs_.close();
    }

    Path old_file_path = Path(wal_path_);

    String new_file_path = old_file_path.string() + '.' + std::to_string(max_commit_ts);
    LOG_INFO(Format("WAL NEW PATH: {}", new_file_path.c_str()));

    // Rename the current wal file to a new one.
    std::filesystem::rename(wal_path_, new_file_path);

    // Create a new wal file with the original name.
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        Error<StorageException>(Format("Failed to open wal file: {}", wal_path_), __FILE_NAME__, __LINE__);
    }
}
/**
 * @brief Replay the wal file.
 *
 * @return int64_t The max commit timestamp of the transactions in the wal file.
 *
 */
int64_t WalManager::ReplayWalFile() {
    // if the wal directory does not exist, just return 0.
    if (!std::filesystem::exists(wal_path_)) {
        return 0;
    }
    // if the wal file is empty, just return 0.
    if (std::filesystem::file_size(wal_path_) == 0) {
        return 0;
    }
    // if the wal file is not empty, we will replay the wal file.
    // 1. First , we will get all wal files store to wal_list_.
    // wal_list_ is a Vector of String, each String is a wal file path.
    // wal_list_ is sorted by the suffix of the wal file path.
    // e.g. wal_list_ = {wal.log.1, wal.log.2, wal.log.3}
    LOG_INFO(Format("WAL PATH: {}", wal_path_.c_str()));
    for (const auto &entry : std::filesystem::directory_iterator(Path(wal_path_).parent_path())) {
        if (entry.is_regular_file()) {
            wal_list_.push_back(entry.path().string());
        }
    }
    std::sort(wal_list_.begin(), wal_list_.end(), [](const String &a, const String &b) {
        return a.substr(a.find_last_of('.') + 1) > b.substr(b.find_last_of('.') + 1);
    });

    // log the wal files.
    for (const auto &wal_file : wal_list_) {
        LOG_INFO(Format("WAL FILE: {}", wal_file.c_str()));
    }

    // 2. Replay the wal files one by one.
    // We will replay the wal files in the order of wal_list_.
    // First we check the wal.log file, if it exists, we will replay it first.
    // Then we check the wal.log.1 file, if it exists, we will replay it.

    // 2.1 Get the max commit timestamp of the transactions in the wal files.
    int64_t max_commit_ts = 0;
    // 2.2 Replay the wal files one by one.
    for (const auto &wal_file : wal_list_) {
        // 2.2.1 Open the wal file.

        std::ifstream ifs(wal_file, std::ios::binary | std::ios::ate);
        if (!ifs.is_open()) {
            Error<StorageException>(Format("Failed to open wal file: {}", wal_path_), __FILE_NAME__, __LINE__);
        }
        std::streamsize size = ifs.tellg();
        ifs.seekg(size - 4, std::ios::beg);
        int32_t entry_size;

        ifs.read(reinterpret_cast<char *>(&entry_size), 4);

        ifs.seekg(size - 4 - (entry_size - 4), std::ios::beg);

        Vector<char> buf(entry_size);

        ifs.read(buf.data(), entry_size);

        ifs.close();

        char *ptr = buf.data();

        auto entry = WalEntry::ReadAdv(ptr, entry_size);

        max_commit_ts = entry->commit_ts;

        auto commands = entry->cmds;

        break;
    }
    return max_commit_ts;

    // 2.2.2 Read the wal file.
    // Get the last entry of the wal file.
    // the entry's tail is 4B, which is the size of the entry.
    // So we can get the entry's size.

    // Get the entry's content.
    // The entry's content is entry_size bytes.

    // So we can get the entry's content.
    // Deserialize the entry.
}

/**
 * @brief Gc the old wal files.
 * Only delete the wal.log.* files. the wal.log file is current wal file.
 * Check if the wal.log.* files are too old.
 * if * is little than the max_commit_ts, we will delete it.
 */
void WalManager::RecycleWalFile() {
    // Gc old wal files.
    LOG_INFO("WalManager::Checkpoint begin to gc wal files");
    if (std::filesystem::exists(wal_path_)) {
        for (const auto &entry : std::filesystem::directory_iterator(Path(wal_path_).parent_path())) {
            if (entry.is_regular_file() && entry.path().string().find("wal.log.") != std::string::npos) {
                auto suffix = entry.path().string().substr(entry.path().string().find_last_of('.') + 1);
                if (std::stoll(suffix) < commit_ts_done_) {
                    std::filesystem::remove(entry.path());
                    LOG_TRACE(Format("WalManager::Checkpoint delete wal file: {}", entry.path().string().c_str()));
                }
            }
        }
    }
    LOG_INFO("WalManager::Checkpoint end to gc wal files");
}

} // namespace infinity
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

WalManager::WalManager(Storage *storage, const std::string &wal_path)
    : storage_(storage), wal_path_(wal_path), running_(false) {}

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
    fs::path wal_dir = fs::path(wal_path_).parent_path();
    if (!fs::exists(wal_dir)) {
        fs::create_directory(wal_dir);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        throw Exception("Failed to open wal file: " + wal_path_);
    }
    flush_thread_ = std::thread([this] { Flush(); });
    checkpoint_thread_ = std::thread([this] { Checkpoint(); });
}

void WalManager::Stop() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed)
        return;
    flush_thread_.join();
    checkpoint_thread_.join();
    ofs_.close();
}

// Session request to persist an entry. Assuming txn_id of the entry has
// been initialized.
int WalManager::PutEntry(std::shared_ptr<WalEntry> entry) {
    if (!running_.load()) {
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
void WalManager::Flush() {
    int64_t seq = 0;
    while (running_.load()) {
        int written = 0;
        int32_t size_pad = 0;
        int64_t max_lsn = lsn_gen_.GetLast();
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        while (!que2_.empty()) {
            std::shared_ptr<WalEntry> entry = que2_.front();
            entry->lsn = lsn_gen_.Generate();
            max_lsn = entry->lsn;
            LOG_TRACE("WalManager::Flush begin writing wal for transaction {}",
                      entry->txn_id);
            int32_t exp_size = entry->GetSizeInBytes();
            vector<char> buf(exp_size);
            char *ptr = buf.data();
            entry->WriteAdv(ptr);
            int32_t act_size = ptr - buf.data();
            if (exp_size != act_size)
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
        if (written == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            LOG_TRACE("WalManager::Flush begin syncing wal for {} transactions",
                      written);
            ofs_.flush();
            LOG_TRACE("WalManager::Flush done syncing wal for {} transactions",
                      seq, written);
            TxnManager *txn_mgr = storage_->txn_manager();
            while (!que3_.empty()) {
                std::shared_ptr<WalEntry> entry = que3_.front();
                // Commit sequently so they get visible in the same order
                // with wal.
                Txn *txn = txn_mgr->GetTxn(entry->txn_id);
                if (txn != nullptr) {
                    txn->CommitTxnBottom();
                }
                que3_.pop();
            }
            lsn_pend_chk_.store(max_lsn);
        }
    }
}

// Do checkpoint for transactions which's lsn no larger than the given one.
void WalManager::Checkpoint() {
    // Fuzzy checkpoint for every 10 transactions or 20s.
    while (running_.load()) {
        int64_t lsn_pend_chk = lsn_pend_chk_.load();
        if (lsn_pend_chk - lsn_done_chk_ < 10 ||
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

} // namespace infinity
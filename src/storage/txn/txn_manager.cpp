//
// Created by jinhai on 23-6-4.
//

#include "txn_manager.h"
#include "storage/meta/catalog.h"

namespace infinity {

Txn *TxnManager::CreateTxn() {
    rw_locker_.lock();
    u64 new_txn_id = GetNewTxnID();
    UniquePtr<Txn> new_txn = MakeUnique<Txn>(this, catalog_, new_txn_id);
    Txn *res = new_txn.get();
    txn_map_[new_txn_id] = std::move(new_txn);
    rw_locker_.unlock();
    return res;
}

void TxnManager::DestroyTxn(u64 txn_id) {
    rw_locker_.lock();
    txn_map_.erase(txn_id);
    rw_locker_.unlock();
}

Txn *TxnManager::GetTxn(u64 txn_id) {
    rw_locker_.lock_shared();
    Txn *res = txn_map_[txn_id].get();
    rw_locker_.unlock_shared();
    return res;
}

TxnState TxnManager::GetTxnState(u64 txn_id) {
    std::shared_lock<RWMutex> r_locker(rw_locker_);
    Txn *txn_ptr = txn_map_[txn_id].get();
    TxnState res = txn_ptr->GetTxnState();
    return res;
}

u64 TxnManager::GetNewTxnID() {
    u64 new_txn_id = ++catalog_->next_txn_id_;
    return new_txn_id;
}

TxnTimeStamp TxnManager::GetTimestamp(bool prepare_wal) {
    std::lock_guard guard(mutex_);
    TxnTimeStamp ts = txn_ts_++;
    if (prepare_wal && put_wal_entry_ != nullptr) {
        priority_que_[ts] = nullptr;
    }
    return ts;
}

void TxnManager::Invalidate(TxnTimeStamp commit_ts) {
    std::lock_guard guard(mutex_);
    size_t cnt = priority_que_.erase(commit_ts);
    if (cnt > 0 && !priority_que_.empty()) {
        auto it = priority_que_.begin();
        while (it != priority_que_.end() && it->second != nullptr) {
            put_wal_entry_(it->second);
            it = priority_que_.erase(it);
        }
    }
}

void TxnManager::PutWalEntry(std::shared_ptr<WalEntry> entry) {
    if (put_wal_entry_ == nullptr)
        return;
    std::unique_lock lk(mutex_);
    priority_que_[entry->commit_ts] = entry;
    auto it = priority_que_.begin();
    while (it != priority_que_.end() && it->second != nullptr) {
        put_wal_entry_(it->second);
        it = priority_que_.erase(it);
    }
}

} // namespace infinity

// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <functional>

import txn;
import txn_state;
import stl;
import catalog;

import infinity_exception;
import wal_entry;
import logger;
import buffer_manager;

module txn_manager;

namespace infinity {

TxnManager::TxnManager(NewCatalog *catalog, BufferManager *buffer_mgr, PutWalEntryFn put_wal_entry_fn, u64 start_txn_id, TxnTimeStamp start_ts)
    : catalog_(catalog), txn_id_(start_txn_id), buffer_mgr_(buffer_mgr), put_wal_entry_(put_wal_entry_fn), txn_ts_(start_ts), is_running_(false) {}

Txn *TxnManager::CreateTxn() {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        Error<TransactionException>("TxnManager is not running, cannot create txn");
    }
    rw_locker_.lock();
    u64 new_txn_id = GetNewTxnID();
    UniquePtr<Txn> new_txn = MakeUnique<Txn>(this, catalog_, new_txn_id);
    Txn *res = new_txn.get();
    txn_map_[new_txn_id] = Move(new_txn);
    rw_locker_.unlock();
    return res;
}

Txn *TxnManager::GetTxn(u64 txn_id) {
    rw_locker_.lock_shared();
    Txn *res = txn_map_[txn_id].get();
    rw_locker_.unlock_shared();
    return res;
}

TxnState TxnManager::GetTxnState(u64 txn_id) {
    SharedLock<RWMutex> r_locker(rw_locker_);
    Txn *txn_ptr = txn_map_[txn_id].get();
    TxnState res = txn_ptr->GetTxnState();
    return res;
}

u64 TxnManager::GetNewTxnID() {
    u64 new_txn_id = ++catalog_->next_txn_id_;
    return new_txn_id;
}

TxnTimeStamp TxnManager::GetTimestamp(bool prepare_wal) {
    LockGuard<Mutex> guard(mutex_);
    TxnTimeStamp ts = txn_ts_++;
    if (prepare_wal && put_wal_entry_ != nullptr) {
        priority_que_[ts] = nullptr;
    }
    return ts;
}

void TxnManager::Invalidate(TxnTimeStamp commit_ts) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        Error<TransactionException>("TxnManager is not running, cannot invalidate");
    }
    LockGuard<Mutex> guard(mutex_);
    SizeT cnt = priority_que_.erase(commit_ts);
    if (cnt > 0 && !priority_que_.empty()) {
        auto it = priority_que_.begin();
        while (it != priority_que_.end() && it->second.get() != nullptr) {
            put_wal_entry_(it->second);
            it = priority_que_.erase(it);
        }
    }
}

void TxnManager::PutWalEntry(SharedPtr<WalEntry> entry) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        Error<TransactionException>("TxnManager is not running, cannot put wal entry");
    }
    if (put_wal_entry_ == nullptr)
        return;
    UniqueLock<Mutex> lk(mutex_);
    priority_que_[entry->commit_ts] = entry;
    auto it = priority_que_.begin();
    while (it != priority_que_.end() && it->second.get() != nullptr) {
        put_wal_entry_(it->second);
        it = priority_que_.erase(it);
    }
    return;
}

void TxnManager::Start() { is_running_.store(true, MemoryOrderRelax); }

void TxnManager::Stop() {
    bool expected = true;
    bool changed = is_running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("TxnManager::Stop already stopped");
        return;
    }

    LOG_INFO("TxnManager is stopping...");
    LockGuard<Mutex> guard(mutex_);
    auto it = priority_que_.begin();
    while (it != priority_que_.end()) {
        // remove and notify the wal manager condition variable
        auto txn = GetTxn(it->first);
        if (txn != nullptr) {
            txn->CancelCommitBottom();
        }
        ++it;
    }
    priority_que_.clear();
    LOG_INFO("TxnManager is stopped");
}

bool TxnManager::Stopped() { return !is_running_.load(); }

TxnTimeStamp TxnManager::CommitTxn(Txn* txn) {
    TxnTimeStamp txn_ts = txn->Commit();
    rw_locker_.lock();
    txn_map_.erase(txn->TxnID());
    rw_locker_.unlock();
    return txn_ts;
}

void TxnManager::RollBackTxn(Txn* txn) {
    txn->Rollback();
    rw_locker_.lock();
    txn_map_.erase(txn->TxnID());
    rw_locker_.unlock();
}

} // namespace infinity

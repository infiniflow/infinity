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
#include <memory>

module txn_manager;

import txn;
import txn_state;
import stl;
import third_party;

import infinity_exception;
import logger;
import buffer_manager;
import background_process;
import catalog_delta_entry;
import default_values;
import wal_manager;
import bg_task;

namespace infinity {

TxnManager::TxnManager(Catalog *catalog,
                       BufferManager *buffer_mgr,
                       BGTaskProcessor *bg_task_processor,
                       WalManager *wal_mgr,
                       TransactionID start_txn_id,
                       TxnTimeStamp start_ts,
                       bool enable_compaction)
    : catalog_(catalog), buffer_mgr_(buffer_mgr), bg_task_processor_(bg_task_processor), wal_mgr_(wal_mgr), start_txn_id_(start_txn_id),
      start_ts_(start_ts), is_running_(false), enable_compaction_(enable_compaction) {
    catalog_->SetTxnMgr(this);
}

Txn *TxnManager::BeginTxn() {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("TxnManager is not running, cannot create txn");
    }

    rw_locker_.lock();

    // Assign a new txn id
    u64 new_txn_id = ++catalog_->next_txn_id_;

    // Record the start ts of the txn
    TxnTimeStamp ts = ++start_ts_;

    // Create txn instance
    UniquePtr<Txn> new_txn = MakeUnique<Txn>(this, buffer_mgr_, catalog_, bg_task_processor_, new_txn_id, ts);

    // Storage txn in txn manager
    Txn *res = new_txn.get();
    txn_map_[new_txn_id] = std::move(new_txn);
    ts_map_.emplace(ts, new_txn_id);
    rw_locker_.unlock();

    LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", new_txn_id, ts));
    return res;
}

Txn *TxnManager::GetTxn(TransactionID txn_id) {
    rw_locker_.lock_shared();
    Txn *res = txn_map_.at(txn_id).get();
    rw_locker_.unlock_shared();
    return res;
}

SharedPtr<Txn> TxnManager::GetTxnPtr(TransactionID txn_id) {
    rw_locker_.lock_shared();
    SharedPtr<Txn> res = txn_map_.at(txn_id);
    rw_locker_.unlock_shared();
    return res;
}

TxnState TxnManager::GetTxnState(TransactionID txn_id) { return GetTxn(txn_id)->GetTxnState(); }

u64 TxnManager::GetNewTxnID() {
    u64 new_txn_id = ++catalog_->next_txn_id_;
    return new_txn_id;
}

TxnTimeStamp TxnManager::GetTimestamp() {
    TxnTimeStamp ts = ++start_ts_;
    return ts;
}

void TxnManager::SendToWAL(Txn *txn) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("TxnManager is not running, cannot put wal entry");
    }
    if (wal_mgr_ == nullptr) {
        UnrecoverableError("TxnManager is null");
    }

    wal_mgr_->PutEntry(txn->GetWALEntry(), txn);
}

void TxnManager::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("TxnManager is not running, cannot add delta entry");
    }
    i64 wal_size = wal_mgr_->WalSize();
    const auto &txn_ids = delta_entry->txn_ids();
    this->AddWaitFlushTxn(txn_ids);
    bg_task_processor_->Submit(MakeShared<AddDeltaEntryTask>(std::move(delta_entry), wal_size));
}

void TxnManager::Start() {
    is_running_.store(true, std::memory_order::relaxed);
    LOG_INFO("TxnManager is started.");
}

void TxnManager::Stop() {
    bool expected = true;
    bool changed = is_running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("TxnManager::Stop already stopped");
        return;
    }

    LOG_INFO("Txn manager is stopping...");
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    auto it = txn_map_.begin();
    while (it != txn_map_.end()) {
        // remove and notify the wal manager condition variable
        Txn *txn_ptr = it->second.get();
        if (txn_ptr != nullptr) {
            txn_ptr->CancelCommitBottom();
        }
        ++it;
    }
    txn_map_.clear();
    LOG_INFO("TxnManager is stopped");
}

bool TxnManager::Stopped() { return !is_running_.load(); }

TxnTimeStamp TxnManager::CommitTxn(Txn *txn) {
    TxnTimeStamp txn_ts = txn->Commit();
    rw_locker_.lock();
    txn_map_.erase(txn->TxnID());
    rw_locker_.unlock();
    return txn_ts;
}

void TxnManager::RollBackTxn(Txn *txn) {
    TransactionID txn_id = txn->TxnID();
    txn->Rollback();
    rw_locker_.lock();
    txn_map_.erase(txn_id);
    rw_locker_.unlock();
}

void TxnManager::AddWaitFlushTxn(const Vector<TransactionID> &txn_ids) {
    // std::stringstream ss;
    // for (auto txn_id : txn_ids) {
    //     ss << txn_id << " ";
    // }
    // LOG_INFO(fmt::format("Add txns: {} to wait flush set", ss.str()));
    std::unique_lock w_lock(rw_locker_);
    wait_flush_txns_.insert(txn_ids.begin(), txn_ids.end());
}

void TxnManager::RemoveWaitFlushTxns(const Vector<TransactionID> &txn_ids) {
    // std::stringstream ss2;
    // for (auto txn_id : txn_ids) {
    //     ss2 << txn_id << " ";
    // }
    // LOG_INFO(fmt::format("Remove txn: {} from wait flush set", ss2.str()));
    std::unique_lock w_lock(rw_locker_);
    for (auto txn_id : txn_ids) {
        if (!wait_flush_txns_.erase(txn_id)) {
            UnrecoverableError(fmt::format("Txn: {} not found in wait flush set", txn_id));
        }
    }
}

TxnTimeStamp TxnManager::GetMinUnflushedTS() {
    std::unique_lock w_locker(rw_locker_);
    for (auto iter = ts_map_.begin(); iter != ts_map_.end();) {
        auto &[ts, txn_id] = *iter;
        if (txn_map_.find(txn_id) != txn_map_.end()) {
            LOG_TRACE(fmt::format("Txn: {} found in txn map", txn_id));
            return ts;
        }
        if (wait_flush_txns_.find(txn_id) != wait_flush_txns_.end()) {
            LOG_TRACE(fmt::format("Txn: {} wait flush", txn_id));
            return ts;
        }
        iter = ts_map_.erase(iter);
    }
    LOG_TRACE(fmt::format("No txn is active, return the next ts {}", start_ts_));
    return start_ts_;
}

} // namespace infinity

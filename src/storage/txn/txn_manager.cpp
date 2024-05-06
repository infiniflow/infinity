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
    : catalog_(catalog), buffer_mgr_(buffer_mgr), bg_task_processor_(bg_task_processor), wal_mgr_(wal_mgr), start_ts_(start_ts), is_running_(false),
      enable_compaction_(enable_compaction) {
    catalog_->SetTxnMgr(this);
}

Txn *TxnManager::BeginTxn(UniquePtr<String> txn_text) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("TxnManager is not running, cannot create txn");
    }

    rw_locker_.lock();

    // Assign a new txn id
    u64 new_txn_id = ++catalog_->next_txn_id_;

    // Record the start ts of the txn
    TxnTimeStamp ts = ++start_ts_;
    // LOG_INFO(fmt::format("Txn {} begin ts: {}", new_txn_id, ts));

    // Create txn instance
    auto new_txn = SharedPtr<Txn>(new Txn(this, buffer_mgr_, catalog_, bg_task_processor_, new_txn_id, ts, std::move(txn_text)));

    // Storage txn in txn manager
    txn_map_[new_txn_id] = new_txn;
    ts_map_.emplace(ts, new_txn_id);
    beginned_txns_.emplace_back(new_txn);
    rw_locker_.unlock();

    LOG_TRACE(fmt::format("Txn: {} is Begin. begin ts: {}", new_txn_id, ts));
    return new_txn.get();
}

Txn *TxnManager::GetTxn(TransactionID txn_id) {
    std::lock_guard guard(rw_locker_);
    Txn *res = txn_map_.at(txn_id).get();
    return res;
}

SharedPtr<Txn> TxnManager::GetTxnPtr(TransactionID txn_id) {
    std::lock_guard guard(rw_locker_);
    SharedPtr<Txn> res = txn_map_.at(txn_id);
    return res;
}

TxnState TxnManager::GetTxnState(TransactionID txn_id) { return GetTxn(txn_id)->GetTxnState(); }

TxnTimeStamp TxnManager::GetCommitTimeStampR(Txn *txn) {
    std::lock_guard guard(rw_locker_);
    TxnTimeStamp commit_ts = ++start_ts_;
    txn->SetTxnRead();
    return commit_ts;
}

TxnTimeStamp TxnManager::GetCommitTimeStampW(Txn *txn) {
    std::lock_guard guard(rw_locker_);
    TxnTimeStamp commit_ts = ++start_ts_;
    wait_conflict_ck_.emplace(commit_ts, nullptr);
    finished_txns_.emplace_back(txn);
    txn->SetTxnWrite();
    return commit_ts;
}

bool TxnManager::CheckConflict(Txn *txn) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    TxnTimeStamp commit_ts = txn->CommitTS();
    Vector<Txn *> candidate_txns;
    {
        std::lock_guard guard(rw_locker_);
        // use binary search find the first txn whose commit ts is greater than `begin_ts`
        auto iter =
            std::lower_bound(finished_txns_.begin(), finished_txns_.end(), begin_ts, [](Txn *txn, TxnTimeStamp ts) { return txn->CommitTS() <= ts; });
        for (; iter != finished_txns_.end(); ++iter) {
            if ((*iter)->CommitTS() >= commit_ts) {
                break; // not include itself
            }
            auto state = (*iter)->GetTxnState();
            if (state == TxnState::kRollbacking || state == TxnState::kRollbacked) {
                continue;
            }
            candidate_txns.push_back(*iter);
        }
    }
    for (auto *candidate_txn : candidate_txns) {
        if (txn->CheckConflict(candidate_txn)) {
            return true;
        }
    }
    return false;
}

void TxnManager::SendToWAL(Txn *txn) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("TxnManager is not running, cannot put wal entry");
    }
    if (wal_mgr_ == nullptr) {
        UnrecoverableError("TxnManager is null");
    }

    TxnTimeStamp commit_ts = txn->CommitTS();
    WalEntry *wal_entry = txn->GetWALEntry();

    std::lock_guard guard(rw_locker_);
    if (wait_conflict_ck_.empty()) {
        UnrecoverableError(fmt::format("WalManager::PutEntry wait_conflict_ck_ is empty, txn->CommitTS() {}", txn->CommitTS()));
    }
    if (wait_conflict_ck_.begin()->first > commit_ts) {
        UnrecoverableError(fmt::format("WalManager::PutEntry wait_conflict_ck_.begin()->first {} > txn->CommitTS() {}",
                                       wait_conflict_ck_.begin()->first,
                                       txn->CommitTS()));
    }
    if (wal_entry) {
        wait_conflict_ck_.at(commit_ts) = wal_entry;
    } else {
        wait_conflict_ck_.erase(commit_ts); // rollback
    }
    if (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr) {
        Vector<WalEntry *> wal_entries;
        do {
            wal_entries.push_back(wait_conflict_ck_.begin()->second);
            wait_conflict_ck_.erase(wait_conflict_ck_.begin());
        } while (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr);
        wal_mgr_->PutEntries(wal_entries);
    }
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
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp commit_ts = txn->Commit();
    this->FinishTxn(txn_id);
    return commit_ts;
}

void TxnManager::RollBackTxn(Txn *txn) {
    TransactionID txn_id = txn->TxnID();
    txn->Rollback();
    this->FinishTxn(txn_id);
}

SizeT TxnManager::ActiveTxnCount() {
    std::unique_lock w_lock(rw_locker_);
    return txn_map_.size();
}

TxnTimeStamp TxnManager::CurrentTS() const { return start_ts_; }

// A Txn can be deleted when there is no uncommitted txn whose begin is less than the commit ts of the txn
// So maintain the least uncommitted begin ts
void TxnManager::FinishTxn(TransactionID txn_id) {
    std::lock_guard guard(rw_locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return;
    }
    auto *txn = iter->second.get();
    if (txn->GetTxnType() == TxnType::kInvalid) {
        UnrecoverableError("Txn type is invalid");
    }
    if (txn->GetTxnType() == TxnType::kRead) {
        txn_map_.erase(txn_id);
        // LOG_INFO(fmt::format("Erase txn 1: {}", txn_id));
        return;
    }

    TxnTimeStamp least_uncommitted_begin_ts = txn->CommitTS() + 1;
    while (!beginned_txns_.empty()) {
        auto first_txn = beginned_txns_.front().lock();
        if (first_txn.get() == nullptr) {
            beginned_txns_.pop_front();
            continue;
        }
        auto status = first_txn->GetTxnState();
        if (status == TxnState::kCommitted || status == TxnState::kRollbacked) {
            beginned_txns_.pop_front();
        } else {
            least_uncommitted_begin_ts = first_txn->BeginTS();
            break;
        }
    }

    while (!finished_txns_.empty()) {
        auto *finished_txn = finished_txns_.front();
        auto finished_state = finished_txn->GetTxnState();
        if (finished_state != TxnState::kCommitted && finished_state != TxnState::kRollbacked) {
            break;
        }
        TxnTimeStamp finished_commit_ts = finished_txn->CommitTS();
        if (finished_commit_ts > least_uncommitted_begin_ts) {
            break;
        }
        auto finished_txn_id = finished_txn->TxnID();
        // LOG_INFO(fmt::format("Txn: {} is erased from txn map", finished_txn_id));
        SizeT remove_n = txn_map_.erase(finished_txn_id);
        // LOG_INFO(fmt::format("Erase txn 2: {}", finished_txn_id));
        if (remove_n == 0) {
            UnrecoverableError(fmt::format("Txn: {} not found in txn map", finished_txn_id));
        }
        finished_txns_.pop_front();
    }
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

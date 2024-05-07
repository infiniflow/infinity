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

    // LOG_INFO(fmt::format("Txn: {} is Begin. begin ts: {}", new_txn_id, ts));
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
    finishing_txns_.emplace(txn);
    txn->SetTxnWrite();
    return commit_ts;
}

bool TxnManager::CheckConflict(Txn *txn) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    TxnTimeStamp commit_ts = txn->CommitTS();
    Vector<Txn *> candidate_txns;
    {
        std::lock_guard guard(rw_locker_);
        // LOG_INFO(fmt::format("Txn {} check conflict", txn->TxnID()));
        for (auto *finishing_txn : finishing_txns_) {
            // LOG_INFO(fmt::format("Txn {} tries to test txn {}", txn->TxnID(), finishing_txn->TxnID()));
            const auto &finishing_state = finishing_txn->GetTxnState();
            bool add = false;
            if (finishing_state == TxnState::kCommitted) {
                TxnTimeStamp committed_ts = finishing_txn->CommittedTS();
                if (begin_ts < committed_ts) {
                    add = true;
                }
            } else if (finishing_state == TxnState::kCommitting) {
                TxnTimeStamp finishing_commit_ts = finishing_txn->CommitTS();
                if (commit_ts > finishing_commit_ts) {
                    add = true;
                }
            }
            if (add) {
                // LOG_INFO(fmt::format("Txn {} tests txn {}", txn->TxnID(), finishing_txn->TxnID()));
                candidate_txns.push_back(finishing_txn);
            }
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
    TxnTimeStamp commit_ts = txn->Commit();
    this->FinishTxn(txn);
    return commit_ts;
}

void TxnManager::RollBackTxn(Txn *txn) {
    txn->Rollback();
    this->FinishTxn(txn);
}

SizeT TxnManager::ActiveTxnCount() {
    std::unique_lock w_lock(rw_locker_);
    return txn_map_.size();
}

TxnTimeStamp TxnManager::CurrentTS() const { return start_ts_; }

// A Txn can be deleted when there is no uncommitted txn whose begin is less than the commit ts of the txn
// So maintain the least uncommitted begin ts
void TxnManager::FinishTxn(Txn *txn) {
    std::lock_guard guard(rw_locker_);

    if (txn->GetTxnType() == TxnType::kInvalid) {
        UnrecoverableError("Txn type is invalid");
    } else if (txn->GetTxnType() == TxnType::kRead) {
        txn_map_.erase(txn->TxnID());
        return;
    }

    TxnTimeStamp finished_ts = ++start_ts_;
    finished_txns_.emplace_back(finished_ts, txn);
    auto state = txn->GetTxnState();
    if (state == TxnState::kCommitting) {
        txn->SetTxnCommitted(finished_ts);
    } else if (state == TxnState::kRollbacking) {
        txn->SetTxnRollbacked();
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
        const auto &[finished_ts, finished_txn] = finished_txns_.front();
        auto finished_state = finished_txn->GetTxnState();
        if (finished_state != TxnState::kCommitted && finished_state != TxnState::kRollbacked) {
            break;
        }
        if (finished_ts > least_uncommitted_begin_ts) {
            break;
        }
        auto finished_txn_id = finished_txn->TxnID();
        finishing_txns_.erase(finished_txn);
        // LOG_INFO(fmt::format("Txn: {} is erased", finished_txn_id));
        SizeT remove_n = txn_map_.erase(finished_txn_id);
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

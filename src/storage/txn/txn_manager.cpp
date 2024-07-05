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
      enable_compaction_(enable_compaction) {}

Txn *TxnManager::BeginTxn(UniquePtr<String> txn_text) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "TxnManager is not running, cannot create txn";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    locker_.lock();

    // Assign a new txn id
    u64 new_txn_id = ++catalog_->next_txn_id_;

    // Record the start ts of the txn
    TxnTimeStamp ts = ++start_ts_;

    // Create txn instance
    auto new_txn = SharedPtr<Txn>(new Txn(this, buffer_mgr_, catalog_, bg_task_processor_, new_txn_id, ts, std::move(txn_text)));

    // Storage txn in txn manager
    txn_map_[new_txn_id] = new_txn;
    beginned_txns_.emplace_back(new_txn);
    locker_.unlock();

    // LOG_INFO(fmt::format("Txn: {} is Begin. begin ts: {}", new_txn_id, ts));
    return new_txn.get();
}

Txn *TxnManager::GetTxn(TransactionID txn_id) {
    std::lock_guard guard(locker_);
    Txn *res = txn_map_.at(txn_id).get();
    return res;
}

SharedPtr<Txn> TxnManager::GetTxnPtr(TransactionID txn_id) {
    std::lock_guard guard(locker_);
    SharedPtr<Txn> res = txn_map_.at(txn_id);
    return res;
}

TxnState TxnManager::GetTxnState(TransactionID txn_id) {
    std::lock_guard guard(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return TxnState::kCommitted;
    }
    Txn *txn = iter->second.get();
    return txn->GetTxnState();
}

bool TxnManager::CheckIfCommitting(TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::lock_guard guard(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return true; // Txn is already committed
    }
    Txn *txn = iter->second.get();
    auto state = txn->GetTxnState();
    if (state != TxnState::kCommitting && state != TxnState::kCommitted) {
        return false;
    }
    return txn->CommitTS() < begin_ts;
}

TxnTimeStamp TxnManager::GetCommitTimeStampR(Txn *txn) {
    std::lock_guard guard(locker_);
    TxnTimeStamp commit_ts = ++start_ts_;
    txn->SetTxnRead();
    return commit_ts;
}

TxnTimeStamp TxnManager::GetCommitTimeStampW(Txn *txn) {
    std::lock_guard guard(locker_);
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
        std::lock_guard guard(locker_);
        // LOG_INFO(fmt::format("Txn {} check conflict", txn->TxnID()));
        for (auto *finishing_txn : finishing_txns_) {
            // LOG_INFO(fmt::format("Txn {} tries to test txn {}", txn->TxnID(), finishing_txn->TxnID()));
            auto finishing_state = finishing_txn->GetTxnState();
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
        String error_message = "TxnManager is not running, cannot put wal entry";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    if (wal_mgr_ == nullptr) {
        String error_message = "TxnManager is null";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    TxnTimeStamp commit_ts = txn->CommitTS();
    WalEntry *wal_entry = txn->GetWALEntry();

    std::lock_guard guard(locker_);
    if (wait_conflict_ck_.empty()) {
        String error_message = fmt::format("WalManager::PutEntry wait_conflict_ck_ is empty, txn->CommitTS() {}", txn->CommitTS());
        LOG_ERROR(error_message);
        UnrecoverableError(error_message);
    }
    if (wait_conflict_ck_.begin()->first > commit_ts) {
        String error_message = fmt::format("WalManager::PutEntry wait_conflict_ck_.begin()->first {} > txn->CommitTS() {}",
                                           wait_conflict_ck_.begin()->first,
                                           txn->CommitTS());
        LOG_ERROR(error_message);
        UnrecoverableError(error_message);
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
        String error_message = "TxnManager is not running, cannot add delta entry";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    i64 wal_size = wal_mgr_->WalSize();
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
    std::unique_lock<std::mutex> w_locker(locker_);
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
    std::unique_lock w_lock(locker_);
    return txn_map_.size();
}

Vector<TxnInfo> TxnManager::GetTxnInfoArray() const {
    Vector<TxnInfo> res;
    res.reserve(txn_map_.size());

    std::unique_lock w_lock(locker_);
    for(const auto& txn_pair: txn_map_) {
        TxnInfo txn_info;
        txn_info.txn_id_ = txn_pair.first;
        txn_info.txn_text_ = txn_pair.second->GetTxnText();
        res.emplace_back(txn_info);
    }
    return res;
}

UniquePtr<TxnInfo> TxnManager::GetTxnInfoByID(TransactionID txn_id) const {
    std::unique_lock w_lock(locker_);
    auto iter = txn_map_.find(txn_id);
    if(iter == txn_map_.end()) {
        return nullptr;
    }
    return MakeUnique<TxnInfo>(iter->first, iter->second->GetTxnText());
}

TxnTimeStamp TxnManager::CurrentTS() const { return start_ts_; }

TxnTimeStamp TxnManager::GetCleanupScanTS() {
    std::lock_guard guard(locker_);
    TxnTimeStamp first_uncommitted_begin_ts = start_ts_;
    while (!beginned_txns_.empty()) {
        auto first_txn = beginned_txns_.front().lock();
        if (first_txn.get() != nullptr) {
            first_uncommitted_begin_ts = first_txn->BeginTS();
            break;
        }
        beginned_txns_.pop_front();
    }
    TxnTimeStamp checkpointed_ts = wal_mgr_->GetCheckpointedTS();
    return std::min(first_uncommitted_begin_ts, checkpointed_ts);
}

// A Txn can be deleted when there is no uncommitted txn whose begin is less than the commit ts of the txn
// So maintain the least uncommitted begin ts
void TxnManager::FinishTxn(Txn *txn) {
    std::lock_guard guard(locker_);

    if (txn->GetTxnType() == TxnType::kInvalid) {
        String error_message = "Txn type is invalid";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
    } else {
        String error_message = fmt::format("Invalid transaction status: {}", ToString(state));
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
            String error_message = fmt::format("Txn: {} not found in txn map", finished_txn_id);
            LOG_ERROR(error_message);
            UnrecoverableError(error_message);
        }
        finished_txns_.pop_front();
    }
}

} // namespace infinity

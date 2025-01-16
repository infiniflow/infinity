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
import catalog_delta_entry;
import catalog;
import default_values;
import wal_manager;
import defer_op;
import infinity_context;
import global_resource_usage;
import bg_task;

namespace infinity {

TxnManager::TxnManager(BufferManager *buffer_mgr, WalManager *wal_mgr, TxnTimeStamp start_ts)
    : buffer_mgr_(buffer_mgr), wal_mgr_(wal_mgr), current_ts_(start_ts), max_committed_ts_(start_ts), is_running_(false) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("TxnManager");
#endif
}

TxnManager::~TxnManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("TxnManager");
#endif
}

Txn *TxnManager::BeginTxn(UniquePtr<String> txn_text, TransactionType txn_type) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "TxnManager is not running, cannot create txn";
        UnrecoverableError(error_message);
    }

    Catalog *catalog_ptr = InfinityContext::instance().storage()->catalog();

    std::lock_guard guard(locker_);

    // Assign a new txn id
    u64 new_txn_id = ++catalog_ptr->next_txn_id_;

    // Record the start ts of the txn
    TxnTimeStamp begin_ts = current_ts_ + 1;

    if (txn_type == TransactionType::kCheckpoint) {
        if (ckp_begin_ts_ != UNCOMMIT_TS) {
            // not set ckp_begin_ts_ may not truncate the wal file.
            LOG_WARN(fmt::format("Another checkpoint txn is started in {}, new checkpoint {} will do nothing", ckp_begin_ts_, begin_ts));
        } else {
            LOG_DEBUG(fmt::format("Checkpoint txn is started in {}", begin_ts));
            ckp_begin_ts_ = begin_ts;
        }
    }

    // Create txn instance
    auto new_txn = SharedPtr<Txn>(new Txn(this, buffer_mgr_, new_txn_id, begin_ts, std::move(txn_text), txn_type));

    // Storage txn in txn manager
    txn_map_[new_txn_id] = new_txn;
    beginned_txns_.emplace_back(new_txn);

    // LOG_INFO(fmt::format("Txn: {} is Begin. begin ts: {}", new_txn_id, begin_ts));
    return new_txn.get();
}

Txn *TxnManager::GetTxn(TransactionID txn_id) const {
    std::lock_guard guard(locker_);
    Txn *res = txn_map_.at(txn_id).get();
    return res;
}

TxnState TxnManager::GetTxnState(TransactionID txn_id) const {
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

// Prepare to commit ReadTxn
TxnTimeStamp TxnManager::GetReadCommitTS(Txn *txn) {
    std::lock_guard guard(locker_);
    TxnTimeStamp commit_ts = current_ts_ + 1;
    txn->SetTxnRead();
    return commit_ts;
}

// Prepare to commit WriteTxn
TxnTimeStamp TxnManager::GetWriteCommitTS(Txn *txn) {
    std::lock_guard guard(locker_);
    current_ts_ += 2;
    TxnTimeStamp commit_ts = current_ts_;
    wait_conflict_ck_.emplace(commit_ts, nullptr);
    committing_txns_.emplace(commit_ts, txn);
    txn->SetTxnWrite();
    return commit_ts;
}

Optional<String> TxnManager::CheckTxnConflict(Txn *txn) {
    TxnTimeStamp commit_ts = txn->CommitTS();
    Vector<SharedPtr<Txn>> candidate_txns;
    TxnTimeStamp min_checking_ts = UNCOMMIT_TS;
    {
        std::lock_guard guard(locker_);
        // LOG_INFO(fmt::format("Txn {}(commit_ts:{}) check conflict", txn->TxnID(), txn->CommitTS()));
        for (auto &[committing_ts, committing_txn] : committing_txns_) {
            if (commit_ts > committing_ts) {
                candidate_txns.push_back(committing_txn->shared_from_this());
                min_checking_ts = std::min(min_checking_ts, committing_txn->BeginTS());
            }
        }
        if (min_checking_ts != UNCOMMIT_TS) {
            checking_ts_.insert(min_checking_ts);
        }
    }
    DeferFn defer_fn([&] {
        if (min_checking_ts != UNCOMMIT_TS) {
            std::lock_guard guard(locker_);
            checking_ts_.erase(min_checking_ts);
        }
    });
    if (txn->CheckConflict()) {
        return "Conflict in txn->CheckConflict()";
    }
    for (const auto &candidate_txn : candidate_txns) {
        // LOG_INFO(fmt::format("Txn {}(commit_ts: {}) check conflict with txn {}(commit_ts: {})",
        //                      txn->TxnID(),
        //                      txn->CommitTS(),
        //                      candidate_txn->TxnID(),
        //                      candidate_txn->CommitTS()));
        if (const auto conflict_reason = txn->CheckConflict(candidate_txn.get()); conflict_reason) {
            return fmt::format("Conflict with candidate_txn {}: {}", candidate_txn->TxnID(), *conflict_reason);
        }
    }
    return None;
}

void TxnManager::SendToWAL(Txn *txn) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "TxnManager is not running, cannot put wal entry";
        UnrecoverableError(error_message);
    }
    if (wal_mgr_ == nullptr) {
        String error_message = "TxnManager is null";
        UnrecoverableError(error_message);
    }

    TxnTimeStamp commit_ts = txn->CommitTS();
    WalEntry *wal_entry = txn->GetWALEntry();

    std::lock_guard guard(locker_);
    if (wait_conflict_ck_.empty()) {
        String error_message = fmt::format("TxnManager::SendToWAL wait_conflict_ck_ is empty, txn->CommitTS() {}", txn->CommitTS());
        UnrecoverableError(error_message);
    }
    if (wait_conflict_ck_.begin()->first > commit_ts) {
        String error_message = fmt::format("TxnManager::SendToWAL wait_conflict_ck_.begin()->first {} > txn->CommitTS() {}",
                                           wait_conflict_ck_.begin()->first,
                                           txn->CommitTS());
        UnrecoverableError(error_message);
    }
    if (wal_entry) {
        wait_conflict_ck_.at(commit_ts) = txn;
    } else {
        wait_conflict_ck_.erase(commit_ts); // rollback
    }
    if (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr) {
        Vector<Txn *> txn_array;
        do {
            txn_array.push_back(wait_conflict_ck_.begin()->second);
            wait_conflict_ck_.erase(wait_conflict_ck_.begin());
        } while (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr);
        wal_mgr_->SubmitTxn(txn_array);
    }
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
    LOG_INFO("Txn manager is stopped");
}

bool TxnManager::Stopped() { return !is_running_.load(); }

TxnTimeStamp TxnManager::CommitTxn(Txn *txn) {
    TxnTimeStamp commit_ts = txn->Commit();
    this->CleanupTxn(txn, true);
    return commit_ts;
}

void TxnManager::RollBackTxn(Txn *txn) {
    txn->Rollback();
    this->CleanupTxn(txn, false);
}

SizeT TxnManager::ActiveTxnCount() {
    std::unique_lock w_lock(locker_);
    return txn_map_.size();
}

Vector<TxnInfo> TxnManager::GetTxnInfoArray() const {
    Vector<TxnInfo> res;

    std::unique_lock w_lock(locker_);
    res.reserve(txn_map_.size());

    for (const auto &txn_pair : txn_map_) {
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
    if (iter == txn_map_.end()) {
        return nullptr;
    }
    return MakeUnique<TxnInfo>(iter->first, iter->second->GetTxnText());
}

Vector<SharedPtr<TxnContext>> TxnManager::GetTxnContextHistories() const {
    std::unique_lock w_lock(locker_);
    Vector<SharedPtr<TxnContext>> txn_context_histories;
    txn_context_histories.reserve(txn_context_histories_.size());

    for (const auto &context_ptr : txn_context_histories_) {
        txn_context_histories.emplace_back(context_ptr);
    }

    for (const auto& ongoing_txn_pair: txn_map_) {
        txn_context_histories.push_back(ongoing_txn_pair.second->txn_context());
    }

    return txn_context_histories;
}

TxnTimeStamp TxnManager::CurrentTS() const { return current_ts_; }

TxnTimeStamp TxnManager::GetNewTimeStamp() { return current_ts_ + 1; }

TxnTimeStamp TxnManager::GetCleanupScanTS() {
    std::lock_guard guard(locker_);
    TxnTimeStamp first_uncommitted_begin_ts = current_ts_;
    // Get earliest txn of the ongoing transactions
    while (!beginned_txns_.empty()) {
        auto first_txn = beginned_txns_.front().lock();
        if (first_txn.get() != nullptr) {
            first_uncommitted_begin_ts = first_txn->BeginTS();
            break;
        }
        beginned_txns_.pop_front();
    }

    // Get the earlier txn ts between current ongoing txn and last checkpoint ts
    TxnTimeStamp last_checkpoint_ts = wal_mgr_->LastCheckpointTS();
    TxnTimeStamp least_ts = std::min(first_uncommitted_begin_ts, last_checkpoint_ts);

    // Get the earlier txn ts between current least txn and checking conflict TS
    if (!checking_ts_.empty()) {
        least_ts = std::min(least_ts, *checking_ts_.begin());
    }

    LOG_INFO(fmt::format("Cleanup scan ts: {}, checkpoint ts: {}", least_ts, last_checkpoint_ts));
    return least_ts;
}

void TxnManager::CleanupTxn(Txn *txn, bool commit) {
    bool is_write_transaction = txn->IsWriteTransaction();
    TransactionID txn_id = txn->TxnID();
    if(is_write_transaction) {
        // For write txn, we need to update the state: committing->committed, rollbacking->rollbacked
        TxnState txn_state = txn->GetTxnState();
        switch (txn_state) {
            case TxnState::kCommitting: {
                txn->SetTxnCommitted();
                break;
            }
            case TxnState::kRollbacking: {
                txn->SetTxnRollbacked();
                break;
            }
            default: {
                String error_message = fmt::format("Invalid transaction status: {}", TxnState2Str(txn_state));
                UnrecoverableError(error_message);
            }
        }
        SharedPtr<AddDeltaEntryTask> add_delta_entry_task = txn->MakeAddDeltaEntryTask();
        {
            // cleanup the txn from committing_txn and txm_map
            auto commit_ts = txn->CommitTS();
            std::lock_guard guard(locker_);
            SizeT remove_n = committing_txns_.erase(commit_ts);
            if (remove_n == 0) {
                UnrecoverableError("Txn not found in committing_txns_");
            }
            SharedPtr<Txn> txn_ptr = txn_map_[txn_id];
            if (txn_context_histories_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
                txn_context_histories_.pop_front();
            }
            txn_context_histories_.push_back(txn_ptr->txn_context());
            remove_n = txn_map_.erase(txn_id);
            if (remove_n == 0) {
                String error_message = fmt::format("Txn: {} not found in txn map", txn_id);
                UnrecoverableError(error_message);
            }
            if (committing_txns_.empty() || committing_txns_.begin()->first > commit_ts) {
                max_committed_ts_ = commit_ts;
            }
        }
        if (commit && add_delta_entry_task) {
            InfinityContext::instance().storage()->bg_processor()->Submit(std::move(add_delta_entry_task));
        }
    } else {
        // For read-only Txn only remove txn from txn_map
        std::lock_guard guard(locker_);
        //            SharedPtr<Txn> txn_ptr = txn_map_[txn_id];
        //            if (txn_contexts_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        //                txn_contexts_.pop_front();
        //            }
        //            txn_contexts_.push_back(txn_ptr);
        txn_map_.erase(txn_id);
    }
}

bool TxnManager::InCheckpointProcess(TxnTimeStamp commit_ts) {
    std::lock_guard guard(locker_);
    if (commit_ts > ckp_begin_ts_) {
        LOG_TRACE(fmt::format("Full/Delta checkpoint begin at {}, cur txn commit_ts: {}, swap to new wal file", ckp_begin_ts_, commit_ts));
        ckp_begin_ts_ = UNCOMMIT_TS;
        return true;
    }
    return false;
}

} // namespace infinity

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

module new_txn_manager;

import new_txn;
import txn_state;
import stl;
import third_party;
import wal_entry;
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
import kv_store;
import new_catalog;
import txn_allocator;
import txn_allocator_task;
import txn_committer;
import txn_committer_task;
import storage;
import catalog_cache;
import base_txn_store;

namespace infinity {

NewTxnManager::NewTxnManager(Storage *storage, KVStore *kv_store, TxnTimeStamp start_ts)
    : storage_(storage), buffer_mgr_(storage->buffer_manager()), wal_mgr_(storage->wal_manager()), kv_store_(kv_store), current_ts_(start_ts),
      prepare_commit_ts_(start_ts), is_running_(false) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("NewTxnManager");
#endif

    // auto kv_instance = kv_store_->GetInstance();
    // String db_string_id;
    // Status status = kv_instance->Get(NEXT_DATABASE_ID.data(), db_string_id);
    // if (!status.ok()) {
    //     kv_instance->Put(NEXT_DATABASE_ID.data(), "0");
    // }
    // String table_string_id;
    // status = kv_instance->Get(NEXT_TABLE_ID.data(), table_string_id);
    // if (!status.ok()) {
    //     kv_instance->Put(NEXT_TABLE_ID.data(), "0");
    // }
    // String index_string_id;
    // status = kv_instance->Get(NEXT_INDEX_ID.data(), index_string_id);
    // if (!status.ok()) {
    //     kv_instance->Put(NEXT_INDEX_ID.data(), "0");
    // }
    // status = kv_instance->Commit();
    // if (!status.ok()) {
    //     UnrecoverableError("Can't initialize latest ID");
    // }
    NewCatalog::Init(kv_store_);
}

NewTxnManager::~NewTxnManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("NewTxnManager");
#endif
}

void NewTxnManager::Start() {
    txn_allocator_ = MakeShared<TxnAllocator>(storage_);
    txn_allocator_->Start();

    txn_committer_ = MakeShared<TxnCommitter>(storage_);
    txn_committer_->Start();

    is_running_.store(true, std::memory_order::relaxed);
    LOG_INFO("NewTxnManager is started.");
}

void NewTxnManager::Stop() {
    if (!is_running_) {
        // FIXME: protect the double stop, the double stop need to be fixed.
        return;
    }

    txn_committer_->Stop();
    txn_committer_.reset();

    txn_allocator_->Stop();
    txn_allocator_.reset();

    bool expected = true;
    bool changed = is_running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("NewTxnManager::Stop already stopped");
        return;
    }

    LOG_INFO("NewTxn manager is stopping...");
    std::unique_lock<std::mutex> w_locker(locker_);
    auto it = txn_map_.begin();
    while (it != txn_map_.end()) {
        // remove and notify the wal manager condition variable
        NewTxn *txn_ptr = it->second.get();
        if (txn_ptr != nullptr) {
            txn_ptr->CancelCommitBottom();
        }
        ++it;
    }
    txn_map_.clear();
    LOG_INFO("NewTxn manager is stopped");
}

bool NewTxnManager::Stopped() { return !is_running_.load(); }

SharedPtr<NewTxn> NewTxnManager::BeginTxnShared(UniquePtr<String> txn_text, TransactionType txn_type) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot create txn";
        UnrecoverableError(error_message);
    }

    Catalog *catalog_ptr = InfinityContext::instance().storage()->catalog();

    std::lock_guard guard(locker_);

    // Assign a new txn id
    u64 new_txn_id = ++catalog_ptr->next_txn_id_;

    // Record the start ts of the txn
    TxnTimeStamp begin_ts = current_ts_ + 1;

    if (txn_type == TransactionType::kNewCheckpoint) {
        if (ckp_begin_ts_ == UNCOMMIT_TS) {
            LOG_DEBUG(fmt::format("Checkpoint txn is started in {}", begin_ts));
            ckp_begin_ts_ = begin_ts;
        } else {
            LOG_WARN(fmt::format("Another checkpoint txn is started in {}, new checkpoint {} will do nothing, not start this txn",
                                 ckp_begin_ts_,
                                 begin_ts));
            return nullptr;
        }
    }

    if (txn_text == nullptr) {
        LOG_DEBUG(fmt::format("NewTxn: {} is Begin. begin ts: {}, No command text", new_txn_id, begin_ts));
    } else {
        LOG_DEBUG(fmt::format("NewTxn: {} is Begin. begin ts: {}, Command: {}", new_txn_id, begin_ts, *txn_text));
    }

    // Create txn instance
    auto new_txn = MakeShared<NewTxn>(this, new_txn_id, begin_ts, kv_store_->GetInstance(), std::move(txn_text), txn_type);

    // Storage txn in txn manager
    txn_map_[new_txn_id] = new_txn;
    begin_txns_.emplace(begin_ts, new_txn_id);

    return new_txn;
}

NewTxn *NewTxnManager::BeginTxn(UniquePtr<String> txn_text, TransactionType txn_type) { return BeginTxnShared(std::move(txn_text), txn_type).get(); }

UniquePtr<NewTxn> NewTxnManager::BeginReplayTxn(const SharedPtr<WalEntry> &wal_entry) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot replay txn";
        UnrecoverableError(error_message);
    }

    TxnTimeStamp txn_id = wal_entry->txn_id_;
    TxnTimeStamp commit_ts = wal_entry->commit_ts_;
    TxnTimeStamp begin_ts = commit_ts - 1;

    // Create txn instance
    UniquePtr<NewTxn> replay_txn = NewTxn::NewReplayTxn(this, txn_id, begin_ts, commit_ts, kv_store_->GetInstance());
    return replay_txn;
}

UniquePtr<NewTxn> NewTxnManager::BeginRecoveryTxn() {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot replay txn";
        UnrecoverableError(error_message);
    }

    //    current_ts_ += 2;
    prepare_commit_ts_ = current_ts_ + 2;
    TxnTimeStamp commit_ts = current_ts_ + 2; // Will not be used, actually.
    TxnTimeStamp begin_ts = current_ts_ + 1;  // current_ts_ > 0

    // Create txn instance
    UniquePtr<NewTxn> recovery_txn = NewTxn::NewRecoveryTxn(this, begin_ts, commit_ts);
    return recovery_txn;
}

bool NewTxnManager::SetTxnCheckpoint(NewTxn *txn) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    std::lock_guard guard(locker_);

    if (ckp_begin_ts_ == UNCOMMIT_TS) {
        LOG_DEBUG(fmt::format("Checkpoint txn is started in {}", begin_ts));
        ckp_begin_ts_ = begin_ts;
    } else {
        LOG_WARN(
            fmt::format("Another checkpoint txn is started in {}, new checkpoint {} will do nothing, not start this txn", ckp_begin_ts_, begin_ts));
        return false;
    }
    txn->txn_context()->txn_type_ = TransactionType::kNewCheckpoint;

    return true;
}

NewTxn *NewTxnManager::GetTxn(TransactionID txn_id) const {
    std::lock_guard guard(locker_);
    NewTxn *res = txn_map_.at(txn_id).get();
    return res;
}

TxnState NewTxnManager::GetTxnState(TransactionID txn_id) const {
    std::lock_guard guard(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return TxnState::kCommitted;
    }
    NewTxn *txn = iter->second.get();
    return txn->GetTxnState();
}

bool NewTxnManager::CheckIfCommitting(TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::lock_guard guard(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return true; // NewTxn is already committed
    }
    NewTxn *txn = iter->second.get();
    auto state = txn->GetTxnState();
    if (state != TxnState::kCommitting && state != TxnState::kCommitted) {
        return false;
    }
    return txn->CommitTS() < begin_ts;
}

// Prepare to commit ReadTxn
TxnTimeStamp NewTxnManager::GetReadCommitTS(NewTxn *txn) {
    std::lock_guard guard(locker_);
    TxnTimeStamp commit_ts = current_ts_ + 1;
    txn->SetTxnRead();
    return commit_ts;
}

// Prepare to commit WriteTxn
TxnTimeStamp NewTxnManager::GetWriteCommitTS(SharedPtr<NewTxn> txn) {
    std::lock_guard guard(locker_);
    prepare_commit_ts_ += 2;
    TxnTimeStamp commit_ts = prepare_commit_ts_;
    wait_conflict_ck_.emplace(commit_ts, nullptr);
    check_txns_.emplace_back(txn);
    if (txn->NeedToAllocate()) {
        allocator_map_.emplace(commit_ts, nullptr);
    }
    txn->SetTxnWrite();
    return commit_ts;
}

TxnTimeStamp NewTxnManager::GetReplayWriteCommitTS(NewTxn *txn) {
    std::lock_guard guard(locker_);
    prepare_commit_ts_ += 2;
    TxnTimeStamp commit_ts = prepare_commit_ts_;
    txn->SetTxnWrite();
    return commit_ts;
}

bool NewTxnManager::CheckConflict1(NewTxn *txn, String &conflict_reason, bool &retry_query) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    TxnTimeStamp commit_ts = txn->CommitTS();

    Vector<SharedPtr<NewTxn>> check_txns = GetCheckTxns(begin_ts, commit_ts);
    for (SharedPtr<NewTxn> &check_txn : check_txns) {
        if (txn->CheckConflict1(check_txn, conflict_reason, retry_query)) {
            return true;
        }
    }
    return false;
}

void NewTxnManager::SendToWAL(NewTxn *txn) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot put wal entry";
        UnrecoverableError(error_message);
    }
    if (wal_mgr_ == nullptr) {
        String error_message = "NewTxnManager is null";
        UnrecoverableError(error_message);
    }

    TxnTimeStamp commit_ts = txn->CommitTS();

    std::lock_guard guard(locker_);
    if (wait_conflict_ck_.empty()) {
        String error_message = fmt::format("NewTxnManager::SendToWAL wait_conflict_ck_ is empty, txn->CommitTS() {}", txn->CommitTS());
        UnrecoverableError(error_message);
    }
    if (wait_conflict_ck_.begin()->first > commit_ts) {
        String error_message = fmt::format("NewTxnManager::SendToWAL wait_conflict_ck_.begin()->first {} > txn->CommitTS() {}",
                                           wait_conflict_ck_.begin()->first,
                                           txn->CommitTS());
        UnrecoverableError(error_message);
    }
    wait_conflict_ck_.at(commit_ts) = txn;
    if (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr) {
        Vector<NewTxn *> txn_array;
        do {
            txn_array.push_back(wait_conflict_ck_.begin()->second);
            wait_conflict_ck_.erase(wait_conflict_ck_.begin());
        } while (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr);
        wal_mgr_->SubmitTxn(txn_array);
    }
}

Status NewTxnManager::CommitTxn(NewTxn *txn, TxnTimeStamp *commit_ts_ptr) {
    Status status = txn->Commit();
    if (commit_ts_ptr != nullptr) {
        *commit_ts_ptr = txn->CommitTS();
    }
    if (status.ok()) {
        switch (txn->GetTxnType()) {
            case TransactionType::kNewCheckpoint: {
                std::lock_guard guard(locker_);
                ckp_begin_ts_ = UNCOMMIT_TS;
                break;
            }
            case TransactionType::kCreateDB: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                if (base_txn_store != nullptr) {
                    // base_txn_store means the creation with ignore if exists
                    CreateDBTxnStore *txn_store = static_cast<CreateDBTxnStore *>(base_txn_store);
                    system_cache_->AddNewDbCache(txn_store->db_name_, txn_store->db_id_);
                }

                break;
            }
            case TransactionType::kDropDB: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                // base_txn_store means the drop with ignore
                if (base_txn_store != nullptr) {
                    DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(base_txn_store);
                    system_cache_->DropDbCache(drop_db_txn_store->db_id_);
                }

                break;
            }
            case TransactionType::kCreateTable: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                if (base_txn_store != nullptr) {
                    // base_txn_store means the creation with ignore if exists
                    CreateTableTxnStore *txn_store = static_cast<CreateTableTxnStore *>(base_txn_store);
                    system_cache_->AddNewTableCache(txn_store->db_id_, txn_store->table_id_, txn_store->table_name_);
                }
                break;
            }
            case TransactionType::kDropTable: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                // base_txn_store means the drop with ignore
                if (base_txn_store != nullptr) {
                    DropTableTxnStore *txn_store = static_cast<DropTableTxnStore *>(base_txn_store);
                    system_cache_->DropTableCache(txn_store->db_id_, txn_store->table_id_);
                }
                break;
            }
            case TransactionType::kCreateIndex: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                if (base_txn_store != nullptr) {
                    // base_txn_store means the creation with ignore if exists
                    CreateIndexTxnStore *txn_store = static_cast<CreateIndexTxnStore *>(base_txn_store);
                    system_cache_->AddNewIndexCache(txn_store->db_id_, txn_store->table_id_, txn_store->table_name_);
                }
                break;
            }
            case TransactionType::kDropIndex: {
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                // base_txn_store means the drop with ignore
                if (base_txn_store != nullptr) {
                    DropIndexTxnStore *txn_store = static_cast<DropIndexTxnStore *>(base_txn_store);
                    system_cache_->DropIndexCache(txn_store->db_id_, txn_store->table_id_, txn_store->index_id_);
                }
                break;
            }
            default: {
                break;
            }
        }
        this->CleanupTxn(txn, true);
    } else {
        this->CleanupTxn(txn, false);
    }
    return status;
}

void NewTxnManager::CommitReplayTxn(NewTxn *txn) {
    Status status = txn->CommitReplay();
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Fail to commit replay txn: {}", status.message()));
    }

    current_ts_ = txn->CommitTS();
    prepare_commit_ts_ = txn->CommitTS();
}

Status NewTxnManager::RollBackTxn(NewTxn *txn) {
    Status status = txn->Rollback();
    if (status.ok()) {
        if (txn->GetTxnType() == TransactionType::kNewCheckpoint) {
            std::lock_guard guard(locker_);
            ckp_begin_ts_ = UNCOMMIT_TS;
        }
        this->CleanupTxn(txn, false);
    }
    return status;
}

SizeT NewTxnManager::ActiveTxnCount() {
    std::unique_lock w_lock(locker_);
    return txn_map_.size();
}

Vector<TxnInfo> NewTxnManager::GetTxnInfoArray() const {
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

UniquePtr<TxnInfo> NewTxnManager::GetTxnInfoByID(TransactionID txn_id) const {
    std::unique_lock w_lock(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return nullptr;
    }
    return MakeUnique<TxnInfo>(iter->first, iter->second->GetTxnText());
}

Vector<SharedPtr<TxnContext>> NewTxnManager::GetTxnContextHistories() const {
    std::unique_lock w_lock(locker_);
    Vector<SharedPtr<TxnContext>> txn_context_histories;
    txn_context_histories.reserve(txn_context_histories_.size());

    for (const auto &context_ptr : txn_context_histories_) {
        txn_context_histories.emplace_back(context_ptr);
    }

    for (const auto &ongoing_txn_pair : txn_map_) {
        txn_context_histories.push_back(ongoing_txn_pair.second->txn_context());
    }

    return txn_context_histories;
}

void NewTxnManager::SetNewSystemTS(TxnTimeStamp new_system_ts) {
    current_ts_ = new_system_ts;
    prepare_commit_ts_ = new_system_ts;
}

TxnTimeStamp NewTxnManager::GetNewTimeStamp() { return current_ts_ + 1; }

TxnTimeStamp NewTxnManager::GetCleanupScanTS1() {
    std::lock_guard guard(locker_);

    return begin_txns_.empty() ? current_ts_ + 1 : begin_txns_.begin()->first;
}

void NewTxnManager::CommitBottom(TxnTimeStamp commit_ts, TransactionID txn_id) {
    std::lock_guard guard(locker_);
    if (current_ts_ > commit_ts || commit_ts > prepare_commit_ts_) {
        UnrecoverableError(fmt::format("Commit ts error: {}, {}, {}", current_ts_, commit_ts, prepare_commit_ts_));
    }
    current_ts_ = commit_ts;
}

void NewTxnManager::CleanupTxn(NewTxn *txn, bool commit) {
    bool is_write_transaction = txn->IsWriteTransaction();
    TxnTimeStamp begin_ts = txn->BeginTS();
    TransactionID txn_id = txn->TxnID();
    LOG_DEBUG(fmt::format("Cleanup txn, id: {}, begin_ts: {}", txn_id, begin_ts));
    if (is_write_transaction) {
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
        {
            std::lock_guard guard(locker_);
            SharedPtr<NewTxn> txn_ptr = txn_map_[txn_id];
            if (txn_context_histories_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
                txn_context_histories_.pop_front();
            }
            txn_context_histories_.push_back(txn_ptr->txn_context());
            SizeT remove_n = txn_map_.erase(txn_id);
            if (remove_n == 0) {
                String error_message = fmt::format("NewTxn: {} not found in txn map", txn_id);
                UnrecoverableError(error_message);
            }
        }
    } else {
        // For read-only NewTxn only remove txn from txn_map
        std::lock_guard guard(locker_);
        //            SharedPtr<NewTxn> txn_ptr = txn_map_[txn_id];
        //            if (txn_contexts_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        //                txn_contexts_.pop_front();
        //            }
        //            txn_contexts_.push_back(txn_ptr);
        txn_map_.erase(txn_id);
    }
    {
        std::lock_guard guard(locker_);

        SizeT remove_n = begin_txns_.erase({begin_ts, txn_id});
        if (remove_n == 0) {
            String error_message = fmt::format("NewTxn: {} not found in begin_txns_", txn_id);
            UnrecoverableError(error_message);
        }

        TxnTimeStamp first_begin_ts = begin_txns_.empty() ? UNCOMMIT_TS : begin_txns_.begin()->first;

        while (!check_txns_.empty() && check_txns_.front()->CommitTS() < first_begin_ts) {
            check_txns_.pop_front();
        }
    }
}

bool NewTxnManager::InCheckpointProcess(TxnTimeStamp commit_ts) {
    std::lock_guard guard(locker_);
    if (commit_ts > ckp_begin_ts_) {
        LOG_TRACE(fmt::format("Full/Delta checkpoint begin at {}, cur txn commit_ts: {}, swap to new wal file", ckp_begin_ts_, commit_ts));
        ckp_begin_ts_ = UNCOMMIT_TS;
        return true;
    }
    return false;
}

void NewTxnManager::PrintAllKeyValue() const {
    std::cout << String("All store key and value: ") << std::endl;
    std::cout << kv_store_->ToString() << std::endl;
    std::cout << String(" -------------- ") << std::endl;
}

SizeT NewTxnManager::KeyValueNum() const { return kv_store_->KeyValueNum(); }

Status NewTxnManager::Cleanup(TxnTimeStamp last_cleanup_ts, TxnTimeStamp *cur_cleanup_ts) {
    TxnTimeStamp cleanup_ts = this->GetCleanupScanTS1();
    if (cur_cleanup_ts) {
        *cur_cleanup_ts = cleanup_ts;
    }
    if (last_cleanup_ts >= cleanup_ts) {
        return Status::OK();
    }

    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();

    Status status;

    // this->PrintAllKeyValue();

    status = NewTxn::Cleanup(cleanup_ts, kv_instance.get());
    if (!status.ok()) {
        return status;
    }

    status = kv_instance->Commit();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Vector<SharedPtr<NewTxn>> NewTxnManager::GetCheckTxns(TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    Vector<SharedPtr<NewTxn>> res;
    {
        std::lock_guard guard(locker_);
        res.reserve(check_txns_.size());
        for (SizeT i = 0; i < check_txns_.size(); ++i) {
            SharedPtr<NewTxn> &check_txn = check_txns_[i];
            TxnTimeStamp check_commit_ts = check_txn->CommitTS();
            TxnState check_txn_state = check_txn->GetTxnState();
            bool is_rollback = (check_txn_state == TxnState::kRollbacking) || (check_txn_state == TxnState::kRollbacked);
            if (check_commit_ts < begin_ts || is_rollback) {
                continue;
            }
            if (check_commit_ts >= commit_ts) {
                break;
            }
            res.push_back(check_txn);
        }
    }
    return res;
}

void NewTxnManager::SubmitForAllocation(SharedPtr<TxnAllocatorTask> txn_allocator_task) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot put wal entry";
        UnrecoverableError(error_message);
    }
    if (wal_mgr_ == nullptr) {
        String error_message = "NewTxnManager is null";
        UnrecoverableError(error_message);
    }
    NewTxn *txn = txn_allocator_task->txn_ptr();
    TxnTimeStamp commit_ts = txn->CommitTS();

    std::lock_guard guard(locker_);
    if (allocator_map_.empty()) {
        String error_message = fmt::format("NewTxnManager::SubmitForAllocation allocator_map_ is empty, txn->CommitTS() {}", txn->CommitTS());
        UnrecoverableError(error_message);
    }
    if (allocator_map_.begin()->first > commit_ts) {
        String error_message = fmt::format("NewTxnManager::SubmitForAllocation allocator_map_.begin()->first {} > txn->CommitTS() {}",
                                           allocator_map_.begin()->first,
                                           txn->CommitTS());
        UnrecoverableError(error_message);
    }

    allocator_map_.at(commit_ts) = txn_allocator_task;
    //    for (const auto &pair : allocator_map_) {
    //        if (pair.second == nullptr) {
    //            LOG_INFO(fmt::format("Suspended null task: commit_ts: {}", pair.first));
    //        } else {
    //            NewTxn *txn_ptr = pair.second->txn_ptr();
    //            LOG_INFO(
    //                fmt::format("Suspended task: {}, transaction id: {}, commit_ts: {}", *txn_ptr->GetTxnText(), txn_ptr->TxnID(),
    //                txn_ptr->CommitTS()));
    //        }
    //    }
    while (!allocator_map_.empty() && allocator_map_.begin()->second != nullptr) {
        //        NewTxn *txn_ptr = allocator_map_.begin()->second->txn_ptr();
        //        LOG_INFO(
        //            fmt::format("Before submit task: {}, transaction id: {}, commit_ts: {}", *txn_ptr->GetTxnText(), txn_ptr->TxnID(),
        //            txn_ptr->CommitTS()));
        txn_allocator_->Submit(allocator_map_.begin()->second);
        allocator_map_.erase(allocator_map_.begin());
    }
}

void NewTxnManager::RemoveFromAllocation(TxnTimeStamp commit_ts) {
    std::lock_guard guard(locker_);
    auto iter = allocator_map_.find(commit_ts);
    if (iter == allocator_map_.end()) {
        String error_message = fmt::format("NewTxnManager::RemoveFromAllocation commit_ts {} not found in allocator_map_", commit_ts);
        UnrecoverableError(error_message);
    }
    allocator_map_.erase(commit_ts);
    return;
}

void NewTxnManager::SubmitForCommit(const SharedPtr<TxnCommitterTask> &txn_committer_task) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        String error_message = "NewTxnManager is not running, cannot put wal entry";
        UnrecoverableError(error_message);
    }
    if (wal_mgr_ == nullptr) {
        String error_message = "NewTxnManager is null";
        UnrecoverableError(error_message);
    }

    txn_committer_->Submit(txn_committer_task);
}

void NewTxnManager::SetSystemCache() {
    system_cache_ = storage_->new_catalog()->GetSystemCache();
    txn_allocator_->SetSystemCache(system_cache_);
}

} // namespace infinity

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

module infinity_core:new_txn_manager.impl;

import :new_txn_manager;
import :new_txn;
import :txn_state;
import :wal_entry;
import :infinity_exception;
import :logger;
import :buffer_manager;
import :default_values;
import :wal_manager;
import :defer_op;
import :infinity_context;
import :bg_task;
import :kv_store;
import :new_catalog;
import :txn_allocator;
import :txn_allocator_task;
import :storage;
import :catalog_cache;
import :base_txn_store;
import :meta_cache;
import :txn_info;

import std;
import third_party;

import global_resource_usage;

namespace infinity {

NewTxnManager::NewTxnManager(Storage *storage, KVStore *kv_store, TxnTimeStamp start_ts)
    : storage_(storage), buffer_mgr_(storage->buffer_manager()), wal_mgr_(storage->wal_manager()), kv_store_(kv_store), current_ts_(start_ts),
      prepare_commit_ts_(start_ts), is_running_(false) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("NewTxnManager");
#endif
    NewCatalog::Init(kv_store_);
}

NewTxnManager::~NewTxnManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("NewTxnManager");
#endif
}

void NewTxnManager::Start() {
    txn_allocator_ = std::make_shared<TxnAllocator>(storage_);
    txn_allocator_->Start();

    is_running_.store(true, std::memory_order::relaxed);
    LOG_INFO("NewTxnManager is started.");
}

void NewTxnManager::Stop() {
    if (!is_running_) {
        // FIXME: protect the double stop, the double stop need to be fixed.
        LOG_INFO("WAL manager was stopped...");
        return;
    }

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

std::shared_ptr<NewTxn> NewTxnManager::BeginTxnShared(std::unique_ptr<std::string> txn_text, TransactionType txn_type) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("NewTxnManager is not running, cannot create txn");
    }

    std::lock_guard guard(locker_);

    // Assign a new txn id
    u64 new_txn_id = ++current_transaction_id_;

    // Record the start ts of the txn
    TxnTimeStamp begin_ts = current_ts_ + 1;

    if (txn_type == TransactionType::kNewCheckpoint) {
        if (ckp_begin_ts_ == UNCOMMIT_TS) {
            LOG_DEBUG(fmt::format("Checkpoint txn is started in {}", begin_ts));
            ckp_begin_ts_ = begin_ts;
        } else {
            LOG_WARN(fmt::format("Another checkpoint txn is started in {}, new txn: {} checkpoint start at {} will do nothing, not start this txn",
                                 ckp_begin_ts_,
                                 new_txn_id,
                                 begin_ts));
            return nullptr;
        }
    }

    if (txn_text == nullptr) {
        LOG_DEBUG(fmt::format("Begin new txn: {}, begin ts: {}, No command text", new_txn_id, begin_ts));
    } else {
        LOG_DEBUG(fmt::format("Begin new txn: {}. begin ts: {}, Command: {}", new_txn_id, begin_ts, *txn_text));
    }

    // Create txn instance
    auto new_txn = std::make_shared<NewTxn>(this,
                                            new_txn_id,
                                            begin_ts,
                                            last_kv_commit_ts_,
                                            last_commit_ts_,
                                            kv_store_->GetInstance(),
                                            std::move(txn_text),
                                            txn_type);

    // Storage txn in txn manager
    txn_map_[new_txn_id] = new_txn;
    ++begin_txn_map_[begin_ts];
    //    begin_txns_.emplace(begin_ts, new_txn_id);

    return new_txn;
}

NewTxn *NewTxnManager::BeginTxn(std::unique_ptr<std::string> txn_text, TransactionType txn_type) {
    return BeginTxnShared(std::move(txn_text), txn_type).get();
}

std::unique_ptr<NewTxn> NewTxnManager::BeginReplayTxn(const std::shared_ptr<WalEntry> &wal_entry) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("NewTxnManager is not running, cannot replay txn");
    }

    TxnTimeStamp txn_id = wal_entry->txn_id_;
    TxnTimeStamp commit_ts = wal_entry->commit_ts_;
    TxnTimeStamp begin_ts = commit_ts - 1;

    // Create txn instance
    std::unique_ptr<NewTxn> replay_txn = NewTxn::NewReplayTxn(this, txn_id, begin_ts, commit_ts, kv_store_->GetInstance());
    return replay_txn;
}

std::unique_ptr<NewTxn> NewTxnManager::BeginRecoveryTxn() {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("NewTxnManager is not running, cannot replay txn");
    }

    std::lock_guard guard(locker_);
    //    current_ts_ += 2;
    prepare_commit_ts_ = current_ts_ + 2;
    TxnTimeStamp commit_ts = current_ts_ + 2; // Will not be used
    TxnTimeStamp begin_ts = current_ts_ + 1;  // current_ts_ > 0

    // Create txn instance
    std::unique_ptr<NewTxn> recovery_txn = NewTxn::NewRecoveryTxn(this, begin_ts, commit_ts);
    return recovery_txn;
}

// Prepare to commit ReadTxn
TxnTimeStamp NewTxnManager::GetReadCommitTS(NewTxn *txn) {
    std::lock_guard guard(locker_);
    TxnTimeStamp commit_ts = current_ts_ + 1;
    txn->SetTxnRead();
    return commit_ts;
}

// TxnTimeStamp NewTxnManager::GetCurrentTS() {
//     std::lock_guard guard(locker_);
//     return current_ts_;
// }

// Prepare to commit WriteTxn
TxnTimeStamp NewTxnManager::GetWriteCommitTS(std::shared_ptr<NewTxn> txn) {
    std::lock_guard guard(locker_);
    prepare_commit_ts_ += 2;
    TxnTimeStamp commit_ts = prepare_commit_ts_;
    wait_conflict_ck_.emplace(commit_ts, nullptr);
    check_txn_map_.emplace(commit_ts, txn);
    bottom_txns_.emplace(commit_ts, txn);
    if (txn->NeedToAllocate()) {
        // LOG_INFO(fmt::format("allocate map add: {}, ts: {}", txn->TxnID(), commit_ts));
        allocator_map_.emplace(commit_ts, nullptr);
    }
    txn->SetTxnWrite();
    return commit_ts;
}

bool NewTxnManager::CheckConflict1(NewTxn *txn, std::string &conflict_reason, bool &retry_query) {

    std::vector<std::shared_ptr<NewTxn>> check_txns = GetCheckCandidateTxns(txn);
    LOG_DEBUG(fmt::format("CheckConflict1:: Txn {} check conflict with check_txns {}", txn->TxnID(), check_txns.size()));

    for (std::shared_ptr<NewTxn> &check_txn : check_txns) {
        if (txn->CheckConflictTxnStores(check_txn, conflict_reason, retry_query)) {
            return true;
        }
    }
    return false;
}

void NewTxnManager::SaveOrResetMetaCacheForReadTxn(NewTxn *txn) {

    if (txn->GetTxnStore() != nullptr) {
        UnrecoverableError("Txn store isn't empty, not read-only transaction");
    }

    if (txn->IsReplay()) {
        UnrecoverableError("Replay transaction can't be read-only.");
    }

    if (!txn->readonly()) {
        // For non-readonly, don't save meta cache
        return;
    }

    // For read-only txn check if previous txn is writable txn. If so, remove the items to cache.
    std::vector<std::shared_ptr<NewTxn>> check_txns = GetCheckCandidateTxns(txn);
    bool all_read_txns = true;
    for (const auto &check_txn : check_txns) {
        if (check_txn->GetTxnStore() == nullptr or check_txn->readonly()) {
            if (check_txn->GetTxnStore() != nullptr) {
                UnrecoverableError("Check txn store isn't empty, not read-only transaction");
            }

            if (check_txn->IsReplay()) {
                UnrecoverableError("Check txn shouldn't be replay txn.");
            }
            // Read-only txn
        } else {
            // Writable Txn
            LOG_DEBUG(fmt::format("Reset meta cache and cache info for read txn {}", txn->TxnID()));
            txn->ResetMetaCacheAndCacheInfo();
            all_read_txns = false;
            break;
        }
    }

    // Check if current txn is started before any write-able txn. If no, the meta should be inserted into cache. Otherwise, the read meta could be
    // deprecated, shouldn't be inserted into cache.
    if (all_read_txns) {
        LOG_DEBUG(fmt::format("Save meta cache and cache info for read txn {}", txn->TxnID()));
        txn->SaveMetaCacheAndCacheInfo();
    }
}

void NewTxnManager::SendToWAL(NewTxn *txn) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("NewTxnManager is not running, cannot put wal entry");
    }
    if (wal_mgr_ == nullptr) {
        UnrecoverableError("NewTxnManager is null");
    }

    const TxnTimeStamp commit_ts = txn->CommitTS();

    std::lock_guard guard(locker_);
    if (wait_conflict_ck_.empty()) {
        UnrecoverableError(fmt::format("NewTxnManager::SendToWAL wait_conflict_ck_ is empty, txn->CommitTS() {}", txn->CommitTS()));
    }
    if (wait_conflict_ck_.begin()->first > commit_ts) {
        UnrecoverableError(fmt::format("NewTxnManager::SendToWAL wait_conflict_ck_.begin()->first {} > txn->CommitTS() {}",
                                       wait_conflict_ck_.begin()->first,
                                       txn->CommitTS()));
    }

    if (txn->GetTxnState() == TxnState::kRollbacking) {
        this->RemoveMapElementForRollbackNoLock(txn->CommitTS(), txn);
    } else {
        wait_conflict_ck_.at(commit_ts) = txn;
    }
    if (!wait_conflict_ck_.empty() && wait_conflict_ck_.begin()->second != nullptr) {
        std::vector<NewTxn *> txn_array;
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
        TransactionType txn_type = txn->GetTxnType();
        if (txn_type == TransactionType::kNewCheckpoint or txn_type == TransactionType::kSkippedCheckpoint) {
            std::lock_guard guard(locker_);
            ckp_begin_ts_ = UNCOMMIT_TS;
        }
    }
    CollectInfo(txn);
    CleanupTxn(txn);
    return status;
}

void NewTxnManager::CommitReplayTxn(NewTxn *txn) {
    Status status = txn->CommitReplay();
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Fail to commit replay txn: {}", status.message()));
    }

    std::lock_guard guard(locker_);
    current_ts_ = txn->CommitTS();
    prepare_commit_ts_ = txn->CommitTS();
}

Status NewTxnManager::RollBackTxn(NewTxn *txn) {
    Status status = txn->Rollback();
    if (status.ok()) {
        TransactionType txn_type = txn->GetTxnType();
        if (txn_type == TransactionType::kNewCheckpoint or txn_type == TransactionType::kSkippedCheckpoint) {
            std::lock_guard guard(locker_);
            ckp_begin_ts_ = UNCOMMIT_TS;
        }
        CollectInfo(txn);
        CleanupTxn(txn);
    }
    return status;
}

size_t NewTxnManager::ActiveTxnCount() {
    std::unique_lock w_lock(locker_);
    return txn_map_.size();
}

std::vector<TxnInfo> NewTxnManager::GetTxnInfoArray() const {
    std::vector<TxnInfo> res;

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

std::unique_ptr<TxnInfo> NewTxnManager::GetTxnInfoByID(TransactionID txn_id) const {
    std::unique_lock w_lock(locker_);
    auto iter = txn_map_.find(txn_id);
    if (iter == txn_map_.end()) {
        return nullptr;
    }
    return std::make_unique<TxnInfo>(iter->first, iter->second->GetTxnText());
}

std::vector<std::shared_ptr<TxnContext>> NewTxnManager::GetTxnContextHistoriesNoLock() const {

    std::vector<std::shared_ptr<TxnContext>> txn_context_histories;
    txn_context_histories.reserve(txn_context_histories_.size() + txn_map_.size());

    for (const auto &context_ptr : txn_context_histories_) {
        txn_context_histories.emplace_back(context_ptr);
    }

    for (const auto &ongoing_txn_pair : txn_map_) {
        txn_context_histories.push_back(ongoing_txn_pair.second->txn_context());
    }

    return txn_context_histories;
}

std::vector<std::shared_ptr<TxnContext>> NewTxnManager::GetTxnContextHistories() const {
    std::unique_lock w_lock(locker_);
    return GetTxnContextHistoriesNoLock();
}

void NewTxnManager::SetNewSystemTS(TxnTimeStamp new_system_ts) {
    std::lock_guard guard(locker_);
    current_ts_ = new_system_ts;
    prepare_commit_ts_ = new_system_ts;
}

void NewTxnManager::SetCurrentTransactionID(TransactionID current_transaction_id) {
    std::lock_guard guard(locker_);
    current_transaction_id_ = current_transaction_id;
}

TxnTimeStamp NewTxnManager::GetOldestAliveTS() {
    std::lock_guard guard(locker_);
    return begin_txn_map_.empty() ? current_ts_ + 1 : begin_txn_map_.begin()->first;
}

void NewTxnManager::CommitBottom(NewTxn *txn) {
    if (txn->IsReplay()) {
        txn->SetTxnBottomDone();

        // We do not update catalog cache during replay as system_cache_ is still nullptr right now.
        return;
    }
    TxnTimeStamp commit_ts = txn->CommitTS();
    TransactionID txn_id = txn->TxnID();

    std::vector<std::shared_ptr<NewTxn>> txns_to_update;
    {
        std::lock_guard guard(locker_);
        auto iter = bottom_txns_.find(commit_ts);
        if (iter == bottom_txns_.end()) {
            UnrecoverableError(fmt::format("NewTxn: {} not found in bottom txn", txn_id));
        }
        if (iter->second == nullptr) {
            UnrecoverableError(fmt::format("NewTxn {} has already done bottom", txn_id));
        }
        if (iter->second->TxnID() != txn_id) {
            UnrecoverableError(fmt::format("NewTxn {} and {} have the same commit ts {}", iter->second->TxnID(), txn_id, commit_ts));
        }
        iter->second->SetTxnBottomDone();

        // ensure notify top half orderly per commit_ts
        while (!bottom_txns_.empty()) {
            iter = bottom_txns_.begin();
            TxnTimeStamp it_ts = iter->first;
            std::shared_ptr<NewTxn> it_txn = iter->second;
            if (current_ts_ > it_ts || it_ts > prepare_commit_ts_) {
                UnrecoverableError(fmt::format("Commit ts error: {}, {}, {}", current_ts_, it_ts, prepare_commit_ts_));
            }
            if (it_txn->GetTxnBottomDone() == false) {
                break;
            }
            bottom_txns_.erase(iter);
            current_ts_ = it_ts;
            txns_to_update.push_back(it_txn);
        }
    }

    // Update catalog cache outside of locker_ to avoid lock-order-inversion
    for (auto &it_txn : txns_to_update) {
        UpdateCatalogCache(it_txn.get());
        it_txn->NotifyTopHalf();
    }
}

void NewTxnManager::CommitKVInstance(NewTxn *txn) {
    // Generate meta cache items
    txn->GetTxnStore();

    TxnTimeStamp commit_ts = txn->CommitTS();
    // Put meta cache items with kv_instance
    WalEntry *wal_entry = txn->GetWALEntry();
    std::vector<std::shared_ptr<EraseBaseCache>> items_to_erase;
    for (const auto &cmd : wal_entry->cmds_) {
        std::vector<std::shared_ptr<EraseBaseCache>> items_to_erase_part = cmd->ToCachedMeta(commit_ts);
        items_to_erase.insert(items_to_erase.end(), items_to_erase_part.begin(), items_to_erase_part.end());
    }

    MetaCache *meta_cache_ptr = this->storage_->meta_cache();
    Status status = meta_cache_ptr->EraseAndCommitKV(items_to_erase, txn->kv_instance_.get(), commit_ts);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Put cache: {}", status.message()));
    }

    //    BaseTxnStore *base_txn_store = txn->GetTxnStore();
    //    if (base_txn_store != nullptr) {
    //        std::vector<std::shared_ptr<EraseBaseCache>> items_to_erase = txn->GetTxnStore()->ToCachedMeta(commit_ts);
    //        MetaCache *meta_cache_ptr = this->storage_->meta_cache();
    //        Status status = meta_cache_ptr->Erase(items_to_erase, txn->kv_instance_.get());
    //        if (!status.ok()) {
    //            UnrecoverableError(fmt::format("Put cache: {}", status.message()));
    //        }
    //    } else {
    //        Status status = txn->kv_instance_->Commit();
    //        if (!status.ok()) {
    //            UnrecoverableError(fmt::format("Commit kv_instance: {}", status.message()));
    //        }
    //    }

    TxnTimeStamp kv_commit_ts;
    {
        std::lock_guard guard(locker_);
        kv_commit_ts = current_ts_ + 1;
        last_kv_commit_ts_ = kv_commit_ts;
        last_commit_ts_ = commit_ts;
    }
    txn->SetTxnKVCommitTS(kv_commit_ts);
}

void NewTxnManager::UpdateCatalogCache(NewTxn *txn) {
    switch (txn->GetTxnType()) {
        case TransactionType::kCreateDB: {
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                // base_txn_store means the creation with ignore if exists
                CreateDBTxnStore *txn_store = static_cast<CreateDBTxnStore *>(base_txn_store);
                system_cache_->AddNewDbCache(txn_store->db_name_, txn_store->db_id_);
            }

            break;
        }
            //        case TransactionType::kDropDB: {
            //            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            //            // base_txn_store means the drop with ignore
            //            if (base_txn_store != nullptr) {
            //                DropDBTxnStore *drop_db_txn_store = static_cast<DropDBTxnStore *>(base_txn_store);
            //                system_cache_->DropDbCache(drop_db_txn_store->db_id_);
            //            }
            //
            //            break;
            //        }
        case TransactionType::kCreateTable: {
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                // base_txn_store means the creation with ignore if exists
                CreateTableTxnStore *txn_store = static_cast<CreateTableTxnStore *>(base_txn_store);
                system_cache_->AddNewTableCache(txn_store->db_id_, txn_store->table_id_, txn_store->table_name_);
            }
            break;
        }
        case TransactionType::kRestoreTable: {
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                RestoreTableTxnStore *txn_store = static_cast<RestoreTableTxnStore *>(base_txn_store);
                system_cache_->AddNewTableCache(txn_store->db_id_, txn_store->table_id_, txn_store->table_name_);
                if (!txn_store->segment_infos_.empty()) {
                    system_cache_->ApplySegmentIDs(txn_store->db_id_, txn_store->table_id_, txn_store->segment_infos_.back().segment_id_ + 1);
                }
                for (const auto &index_cmd : txn_store->index_cmds_) {
                    system_cache_->AddNewIndexCache(txn_store->db_id_, txn_store->table_id_, index_cmd.index_id_);
                }
            }
            break;
        }
        case TransactionType::kRestoreDatabase: {
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                RestoreDatabaseTxnStore *txn_store = static_cast<RestoreDatabaseTxnStore *>(base_txn_store);
                u64 db_id = std::stoull(txn_store->db_id_str_);
                system_cache_->AddNewDbCache(txn_store->db_name_, db_id);
                for (const auto &restore_table_txn_store : txn_store->restore_table_txn_stores_) {
                    system_cache_->AddNewTableCache(db_id, restore_table_txn_store->table_id_, restore_table_txn_store->table_name_);
                    if (!restore_table_txn_store->segment_infos_.empty()) {
                        system_cache_->ApplySegmentIDs(db_id,
                                                       restore_table_txn_store->table_id_,
                                                       restore_table_txn_store->segment_infos_[-1].segment_id_ + 1);
                    }
                    for (const auto &index_cmd : restore_table_txn_store->index_cmds_) {
                        system_cache_->AddNewIndexCache(db_id, restore_table_txn_store->table_id_, index_cmd.index_id_);
                    }
                }
            }
            break;
        }
            //        case TransactionType::kDropTable: {
            //            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            //            // base_txn_store means the drop with ignore
            //            if (base_txn_store != nullptr) {
            //                DropTableTxnStore *txn_store = static_cast<DropTableTxnStore *>(base_txn_store);
            //                system_cache_->DropTableCache(txn_store->db_id_, txn_store->table_id_);
            //            }
            //            break;
            //        }
        case TransactionType::kCreateIndex: {
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                // base_txn_store means the creation with ignore if exists
                CreateIndexTxnStore *txn_store = static_cast<CreateIndexTxnStore *>(base_txn_store);
                system_cache_->AddNewIndexCache(txn_store->db_id_, txn_store->table_id_, *txn_store->index_base_->index_name_);
            }
            break;
        }
            //        case TransactionType::kDropIndex: {
            //            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            //            // base_txn_store means the drop with ignore
            //            if (base_txn_store != nullptr) {
            //                DropIndexTxnStore *txn_store = static_cast<DropIndexTxnStore *>(base_txn_store);
            //                system_cache_->DropIndexCache(txn_store->db_id_, txn_store->table_id_, txn_store->index_id_);
            //            }
            //            break;
            //        }
        default: {
            break;
        }
    }
}

void NewTxnManager::CleanupTxn(NewTxn *txn) {
    bool is_write_transaction = txn->IsWriteTransaction();
    TxnTimeStamp begin_ts = txn->BeginTS();
    TransactionID txn_id = txn->TxnID();
    LOG_DEBUG(fmt::format("Cleanup txn, id: {}, begin_ts: {}", txn_id, begin_ts));
    if (is_write_transaction) {
        TxnState txn_state = txn->GetTxnState();
        switch (txn_state) {
            case TxnState::kCommitted:
            case TxnState::kRollbacked: {
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Invalid transaction status: {}", TxnState2Str(txn_state)));
            }
        }
        {
            std::lock_guard guard(locker_);
            std::shared_ptr<NewTxn> txn_ptr = txn_map_[txn_id];
            if (txn_context_histories_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
                txn_context_histories_.pop_front();
            }
            txn_context_histories_.push_back(txn_ptr->txn_context());
            size_t remove_n = txn_map_.erase(txn_id);
            if (remove_n == 0) {
                UnrecoverableError(fmt::format("NewTxn: {} not found in txn map", txn_id));
            }

            CleanupTxnBottomNolock(txn_id, begin_ts);
        }
    } else {
        // For read-only NewTxn only remove txn from txn_map
        std::lock_guard guard(locker_);
        //            std::shared_ptr<NewTxn> txn_ptr = txn_map_[txn_id];
        //            if (txn_contexts_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        //                txn_contexts_.pop_front();
        //            }
        //            txn_contexts_.push_back(txn_ptr);
        txn_map_.erase(txn_id);

        CleanupTxnBottomNolock(txn_id, begin_ts);
    }
    LOG_FLUSH();
}

void NewTxnManager::CleanupTxnBottomNolock(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto begin_txn_iter = begin_txn_map_.find(begin_ts);
    if (begin_txn_iter == begin_txn_map_.end()) {
        UnrecoverableError(fmt::format("CleanupTxnBottomNolock: NewTxn: {} with begin ts: {} not found in begin_txn_map_", txn_id, begin_ts));
    }
    if (begin_txn_iter->second == 0) {
        UnrecoverableError(fmt::format("CleanupTxnBottomNolock: Txn count with begin ts: {} in begin_txn_map_ is 0", begin_ts));
    }
    if (--begin_txn_iter->second == 0) {
        begin_txn_map_.erase(begin_txn_iter);
    }

    if (begin_txn_map_.empty()) {
        check_txn_map_.clear();
        return;
    }

    TxnTimeStamp first_begin_ts = begin_txn_map_.begin()->first;
    while (!check_txn_map_.empty() && check_txn_map_.begin()->second->KVCommitTS() < first_begin_ts) {
        LOG_TRACE(fmt::format("Pop check txn, id: {}, with commit_ts: {} < begin_ts: {}",
                              check_txn_map_.begin()->second->TxnID(),
                              check_txn_map_.begin()->first,
                              first_begin_ts));
        check_txn_map_.erase(check_txn_map_.begin());
    }
}

void NewTxnManager::PrintAllKeyValue() const {
    std::stringstream ss;
    ss << "All store key and value: " << std::endl;
    ss << kv_store_->ToString();
    LOG_INFO(ss.str());
}

void NewTxnManager::PrintPMKeyValue() const {
    std::stringstream ss;
    ss << "Persistence Manager keys and values: " << std::endl;
    std::vector<std::pair<std::string, std::string>> all_key_values = kv_store_->GetAllKeyValue();
    for (const auto &[key, value] : all_key_values) {
        // Check if the key is a PM key by looking for "pm|" prefix
        if (key.find("pm|") == 0) {
            ss << fmt::format("PM key: {}, value: {}", key, value) << std::endl;
        }
    }
    LOG_INFO(ss.str());
}

void NewTxnManager::PrintAllDroppedKeys() const {
    std::stringstream ss;
    ss << "All dropped keys: " << std::endl;
    // Get all key-value pairs from the KV store
    std::vector<std::pair<std::string, std::string>> all_key_values = kv_store_->GetAllKeyValue();

    for (const auto &[key, value] : all_key_values) {
        // Check if the key is a dropped key by looking for "drop|" prefix
        if (key.find("drop|") == 0) {
            ss << fmt::format("Dropped key: {}, value: {}", key, value) << std::endl;
        }
    }
    LOG_INFO(ss.str());
}

size_t NewTxnManager::KeyValueNum() const { return kv_store_->KeyValueNum(); }

std::vector<std::shared_ptr<NewTxn>> NewTxnManager::GetCheckCandidateTxns(NewTxn *this_txn) {

    TxnTimeStamp this_begin_ts = this_txn->BeginTS();
    TxnTimeStamp this_commit_ts = this_txn->CommitTS(); // Already got commit ts, but without kv_commit ts.
    TransactionID this_txn_id = this_txn->TxnID();
    TxnTimeStamp this_last_system_kv_commit_ts = this_txn->LastSystemKVCommitTS();
    TxnTimeStamp this_last_system_commit_ts = this_txn->LastSystemCommitTS();

    std::vector<std::shared_ptr<NewTxn>> res;
    {
        std::lock_guard guard(locker_);
        res.reserve(check_txn_map_.size());
        for (const auto &other_txn_pair : check_txn_map_) {
            const std::shared_ptr<NewTxn> &other_txn = other_txn_pair.second;
            //            LOG_TRACE(fmt::format("This txn: {}, check txn: {}, begin_ts: {}, commit_ts: {}",
            //                                 this_txn_id,
            //                                 other_txn->TxnID(),
            //                                 other_txn->BeginTS(),
            //                                 other_txn->CommitTS()));
            if (this_txn_id == other_txn->TxnID()) {
                // Same txn, SKIP
                continue;
            }
            TxnTimeStamp other_kv_commit_ts = other_txn->KVCommitTS();
            TxnTimeStamp other_commit_ts = other_txn->CommitTS();
            TxnTimeStamp other_begin_ts = other_txn->BeginTS();
            TxnState other_txn_state = other_txn->GetTxnState();
            bool is_rollback = (other_txn_state == TxnState::kRollbacking) || (other_txn_state == TxnState::kRollbacked);
            if (other_kv_commit_ts < this_begin_ts || is_rollback) {
                // SKIP, other txn is committed before this txn begin
                // SKIP, other txn is rolled back.
                continue;
            }

            if (other_kv_commit_ts == this_begin_ts) {
                if (other_kv_commit_ts < this_last_system_kv_commit_ts) {
                    // other txn kv commit before this txn begin
                    // case 1
                    //  t1               commit     kv commit (509) -- no conflicts
                    //  |-------------------|----------|
                    //                                  |---------------------------------|------------|
                    //                              t3 begin (begin_ts 511) (last_kv_commit 511)
                    // t2 must begin after t1 kv commit, no conflict
                    // SKIP
                    continue;
                } else if (other_kv_commit_ts == this_last_system_kv_commit_ts) {
                    // other txn kv commit ts same as this txn begin ts
                    // case 2
                    //        t2        commit (512)  kv commit(513) -- conflicts
                    //        |-----------|--------------|
                    //  t1            commit (510)  kv commit (513) -- no conflicts
                    //  |---------------|---------------|
                    //                                  |---------------------------------|------------|
                    //                              t3 begin (begin_ts 513) (last_kv_commit 513) (last_commit_ts 510)
                    if (other_commit_ts <= this_last_system_commit_ts) {
                        // not conflicts, SKIP
                        continue;
                    } else {
                        // conflicts
                        ;
                    }
                } else {
                    // other_kv_commit_ts > this_last_system_kv_commit_ts
                    // case 3
                    //  t1               commit        kv commit (515) -- conflicts
                    //  |-------------------|------------|
                    //                                  |---------------------------------|------------|
                    //                              t3 begin (begin_ts 513) (last_kv_commit 513)
                    // t2 must begin before t1 kv commit, no conflict
                    // conflicts
                    ;
                }
            }

            if (other_begin_ts > this_commit_ts) {
                // SKIP, other txn is started after this txn commit
                // Obviously, other txn after this txn commit might also conflict with this txn, due to the kv commit ts reason. But when commit other
                // txn, the conflict check must be done again.
                break;
            }
            res.push_back(other_txn);
        }
    }
    return res;
}

void NewTxnManager::SubmitForAllocation(std::shared_ptr<TxnAllocatorTask> txn_allocator_task) {
    // Check if the is_running_ is true
    if (is_running_.load() == false) {
        UnrecoverableError("NewTxnManager is not running, cannot put wal entry");
    }
    if (wal_mgr_ == nullptr) {
        UnrecoverableError("NewTxnManager is null");
    }
    NewTxn *txn = txn_allocator_task->txn_ptr();
    TxnTimeStamp commit_ts = txn->CommitTS();

    std::lock_guard guard(locker_);
    if (allocator_map_.empty()) {
        UnrecoverableError(fmt::format("NewTxnManager::SubmitForAllocation allocator_map_ is empty, txn->CommitTS() {}", txn->CommitTS()));
    }
    if (allocator_map_.begin()->first > commit_ts) {
        UnrecoverableError(fmt::format("NewTxnManager::SubmitForAllocation allocator_map_.begin()->first {} > txn->CommitTS() {}",
                                       allocator_map_.begin()->first,
                                       txn->CommitTS()));
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
    // LOG_INFO(fmt::format("TxnAllocator: txn: {} ts: {} SubmitForAllocation", txn_allocator_task->txn_ptr()->TxnID(), commit_ts));
    while (!allocator_map_.empty() && allocator_map_.begin()->second != nullptr) {
        //        NewTxn *txn_ptr = allocator_map_.begin()->second->txn_ptr();
        //        LOG_INFO(
        //            fmt::format("Before submit task: {}, transaction id: {}, commit_ts: {}", *txn_ptr->GetTxnText(), txn_ptr->TxnID(),
        //            txn_ptr->CommitTS()));
        txn_allocator_->Submit(allocator_map_.begin()->second);
        // LOG_INFO(fmt::format("SubmitForAllocation:: remove allocation ts: {}", allocator_map_.begin()->first));
        allocator_map_.erase(allocator_map_.begin());
    }
}

void NewTxnManager::SetSystemCache(std::unique_ptr<SystemCache> system_cache) {
    system_cache_ = std::move(system_cache);
    txn_allocator_->SetSystemCache(system_cache_.get());
}

void NewTxnManager::RemoveMapElementForRollbackNoLock(TxnTimeStamp commit_ts, NewTxn *txn_ptr) {
    if (!wait_conflict_ck_.erase(commit_ts)) {
        UnrecoverableError(fmt::format("Key: {} is not exists.", commit_ts));
    }
    if (!bottom_txns_.erase(commit_ts)) {
        UnrecoverableError(fmt::format("Key: {} is not exists.", commit_ts));
    }
    check_txn_map_.erase(commit_ts);

    if (txn_ptr->NeedToAllocate()) {
        allocator_map_.erase(commit_ts);
        // LOG_INFO(fmt::format("RemoveMapElementForRollbackNoLock ts: {}", commit_ts));
        // Re-trigger submit allocation.
        while (!allocator_map_.empty() && allocator_map_.begin()->second != nullptr) {
            txn_allocator_->Submit(allocator_map_.begin()->second);
            allocator_map_.erase(allocator_map_.begin());
        }
    }
}

SystemCache *NewTxnManager::GetSystemCachePtr() const { return system_cache_.get(); }

void NewTxnManager::UpdateTxnBeginTSAndKVInstance(NewTxn *txn) {
    std::lock_guard guard(locker_);
    TxnTimeStamp old_begin_ts = txn->BeginTS();
    TxnTimeStamp new_begin_ts = current_ts_ + 1;
    if (new_begin_ts == old_begin_ts) {
        return;
    }
    txn->SetBeginTS(new_begin_ts);
    ++begin_txn_map_[new_begin_ts];

    auto old_it = begin_txn_map_.find(old_begin_ts);
    if (old_it == begin_txn_map_.end()) {
        UnrecoverableError(
            fmt::format("UpdateTxnBeginTSAndKVInstance: NewTxn: {} with begin ts: {} not found in begin_txn_map_", txn->TxnID(), old_begin_ts));
    }
    if (old_it->second == 0) {
        UnrecoverableError(fmt::format("UpdateTxnBeginTSAndKVInstance: Txn count with begin ts: {} in begin_txn_map_ is 0", old_begin_ts));
    }
    if (--old_it->second == 0) {
        begin_txn_map_.erase(old_it);
    }

    txn->UpdateKVInstance(kv_store_->GetInstance());
}

void NewTxnManager::CollectInfo(NewTxn *txn) {
    switch (txn->GetTxnType()) {
        case TransactionType::kNewCheckpoint: {
            std::shared_ptr<TxnCheckpointInfo> ckp_info = std::make_shared<TxnCheckpointInfo>();
            ckp_info->txn_id_ = txn->TxnID();
            ckp_info->begin_ts_ = txn->BeginTS();
            ckp_info->commit_ts_ = txn->CommitTS();

            if (txn->GetTxnState() == TxnState::kCommitted) {
                ckp_info->committed_ = true;
            }

            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            CheckpointTxnStore *checkpoint_txn_store = static_cast<CheckpointTxnStore *>(base_txn_store);
            ckp_info->auto_flush_ = checkpoint_txn_store->auto_check_point_;
            ckp_info->checkpoint_ts_ = checkpoint_txn_store->max_commit_ts_;
            if (!checkpoint_txn_store->entries_.empty()) {
                ckp_info->entries_ = checkpoint_txn_store->entries_;
            }

            this->AddCheckpointInfo(ckp_info);
            break;
        }
        case TransactionType::kCompact: {
            std::shared_ptr<TxnCompactInfo> compact_info = std::make_shared<TxnCompactInfo>();
            compact_info->txn_id_ = txn->TxnID();
            compact_info->begin_ts_ = txn->BeginTS();
            compact_info->commit_ts_ = txn->CommitTS();
            if (txn->GetTxnState() == TxnState::kCommitted) {
                compact_info->committed_ = true;
            }

            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store);
                compact_info->deprecated_segment_ids_ = compact_txn_store->deprecated_segment_ids_;
                compact_info->new_segment_id_ = compact_txn_store->new_segment_id_;
                compact_info->table_id_ = compact_txn_store->table_id_;
                compact_info->table_name_ = compact_txn_store->table_name_;
                compact_info->table_id_ = compact_txn_store->db_id_;
                compact_info->db_name_ = compact_info->db_name_;
            }

            this->AddCompactInfo(compact_info);
            break;
        }
        case TransactionType::kOptimizeIndex: {

            auto txn_id = txn->TxnID();
            auto begin_ts = txn->BeginTS();
            auto commit_ts = txn->CommitTS();

            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store == nullptr) {
                std::shared_ptr<TxnOptimizeInfo> optimize_info = std::make_shared<TxnOptimizeInfo>();
                optimize_info->txn_id_ = txn_id;
                optimize_info->begin_ts_ = begin_ts;
                optimize_info->commit_ts_ = commit_ts;
                if (txn->GetTxnState() == TxnState::kCommitted) {
                    optimize_info->committed_ = true;
                }
                this->AddOptimizeInfo(optimize_info);
            } else {
                OptimizeIndexTxnStore *optimize_index_store = static_cast<OptimizeIndexTxnStore *>(base_txn_store);
                for (const auto &optimize_index_entry : optimize_index_store->entries_) {
                    std::shared_ptr<TxnOptimizeInfo> optimize_info = std::make_shared<TxnOptimizeInfo>();
                    optimize_info->txn_id_ = txn_id;
                    optimize_info->begin_ts_ = begin_ts;
                    if (txn->GetTxnState() == TxnState::kCommitted) {
                        optimize_info->committed_ = true;
                    }
                    optimize_info->db_id_ = optimize_index_entry.db_id_;
                    optimize_info->db_name_ = optimize_index_entry.db_name_;
                    optimize_info->table_id_ = optimize_index_entry.table_id_;
                    optimize_info->table_name_ = optimize_index_entry.table_name_;
                    optimize_info->segment_id_ = optimize_index_entry.segment_id_;
                    optimize_info->deprecated_chunk_ids_ = optimize_index_entry.deprecate_chunks_;
                    optimize_info->new_chunk_id_ = optimize_index_entry.new_chunk_infos_[0].chunk_id_;
                    optimize_info->index_name_ = optimize_index_entry.index_name_;
                    optimize_info->index_id_ = optimize_index_entry.index_id_;
                    this->AddOptimizeInfo(optimize_info);
                }
            }
            break;
        }
        case TransactionType::kImport: {
            std::shared_ptr<TxnImportInfo> import_info = std::make_shared<TxnImportInfo>();
            import_info->txn_id_ = txn->TxnID();
            import_info->begin_ts_ = txn->BeginTS();
            import_info->commit_ts_ = txn->CommitTS();
            if (txn->GetTxnState() == TxnState::kCommitted) {
                import_info->committed_ = true;
            }

            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store);
                import_info->db_name_ = import_txn_store->db_name_;
                import_info->db_id_ = import_txn_store->db_id_;
                import_info->table_name_ = import_txn_store->table_name_;
                import_info->table_id_ = import_txn_store->table_id_;
                import_info->segment_ids_ = import_txn_store->segment_ids_;
                import_info->row_count_ = import_txn_store->row_count_;
            }

            this->AddImportInfo(import_info);
            break;
        }
        case TransactionType::kCleanup: {
            std::shared_ptr<TxnCleanInfo> clean_info = std::make_shared<TxnCleanInfo>();
            clean_info->txn_id_ = txn->TxnID();
            clean_info->begin_ts_ = txn->BeginTS();
            clean_info->commit_ts_ = txn->CommitTS();
            if (txn->GetTxnState() == TxnState::kCommitted) {
                clean_info->committed_ = true;
            }
            BaseTxnStore *base_txn_store = txn->GetTxnStore();
            if (base_txn_store != nullptr) {
                CleanupTxnStore *cleanup_txn_store = static_cast<CleanupTxnStore *>(base_txn_store);
                clean_info->dropped_keys_ = cleanup_txn_store->dropped_keys_;
                clean_info->metas_ = cleanup_txn_store->metas_;
            }
            this->AddCleanInfo(clean_info);
            break;
        }
        default: {
            break;
        }
    }
}

void NewTxnManager::AddTaskInfo(std::shared_ptr<BGTaskInfo> task_info) {
    std::lock_guard<std::mutex> lock(task_lock_);
    if (task_info_list_.size() >= DEFAULT_TASK_HISTORY_SIZE) {
        task_info_list_.pop_front(); // Remove the oldest task info if the list exceeds the size limit
    }
    task_info_list_.push_back(std::move(task_info));
}

std::vector<std::shared_ptr<BGTaskInfo>> NewTxnManager::GetTaskInfoList() const {
    std::lock_guard<std::mutex> lock(task_lock_);
    std::vector<std::shared_ptr<BGTaskInfo>> task_info_list;
    task_info_list.reserve(task_info_list_.size());
    for (const auto &task_info : task_info_list_) {
        task_info_list.push_back(task_info);
    }
    return task_info_list;
}

void NewTxnManager::AddCheckpointInfo(std::shared_ptr<TxnCheckpointInfo> ckp_info) {
    std::lock_guard<std::mutex> lock(checkpoint_info_lock_);
    if (ckp_info_list_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        ckp_info_list_.pop_front();
    }
    ckp_info_list_.push_back(std::move(ckp_info));
}

std::vector<std::shared_ptr<TxnCheckpointInfo>> NewTxnManager::GetCheckpointInfoList() const {
    std::vector<std::shared_ptr<TxnCheckpointInfo>> ckp_info_list;
    std::lock_guard<std::mutex> lock(checkpoint_info_lock_);
    ckp_info_list.reserve(ckp_info_list_.size());
    for (const auto &ckp_info : ckp_info_list_) {
        ckp_info_list.push_back(ckp_info);
    }
    return ckp_info_list;
}

void NewTxnManager::AddCompactInfo(std::shared_ptr<TxnCompactInfo> compact_info) {
    std::lock_guard<std::mutex> lock(compact_info_lock_);
    if (compact_info_list_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        compact_info_list_.pop_front();
    }
    compact_info_list_.push_back(std::move(compact_info));
}

std::vector<std::shared_ptr<TxnCompactInfo>> NewTxnManager::GetCompactInfoList() const {
    std::vector<std::shared_ptr<TxnCompactInfo>> compact_info_list;
    std::lock_guard<std::mutex> lock(compact_info_lock_);
    compact_info_list.reserve(compact_info_list_.size());
    for (const auto &compact_info : compact_info_list_) {
        compact_info_list.push_back(compact_info);
    }
    return compact_info_list;
}

void NewTxnManager::AddOptimizeInfo(std::shared_ptr<TxnOptimizeInfo> optimize_info) {
    std::lock_guard<std::mutex> lock(optimize_info_lock_);
    if (optimize_info_list_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        optimize_info_list_.pop_front();
    }
    optimize_info_list_.push_back(std::move(optimize_info));
}

std::vector<std::shared_ptr<TxnOptimizeInfo>> NewTxnManager::GetOptimizeInfoList() const {
    std::vector<std::shared_ptr<TxnOptimizeInfo>> optimize_info_list;
    std::lock_guard<std::mutex> lock(optimize_info_lock_);
    optimize_info_list.reserve(optimize_info_list_.size());
    for (const auto &optimize_info : optimize_info_list_) {
        optimize_info_list.push_back(optimize_info);
    }
    return optimize_info_list;
}

void NewTxnManager::AddImportInfo(std::shared_ptr<TxnImportInfo> import_info) {
    std::lock_guard<std::mutex> lock(import_info_lock_);
    if (import_info_list_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        import_info_list_.pop_front();
    }
    import_info_list_.push_back(std::move(import_info));
}

std::vector<std::shared_ptr<TxnImportInfo>> NewTxnManager::GetImportInfoList() const {
    std::vector<std::shared_ptr<TxnImportInfo>> import_info_list;
    std::lock_guard<std::mutex> lock(import_info_lock_);
    import_info_list.reserve(import_info_list_.size());
    for (const auto &import_info : import_info_list_) {
        import_info_list.push_back(import_info);
    }
    return import_info_list;
}

void NewTxnManager::AddCleanInfo(std::shared_ptr<TxnCleanInfo> clean_info) {
    std::lock_guard<std::mutex> lock(clean_info_lock_);
    if (clean_info_list_.size() >= DEFAULT_TXN_HISTORY_SIZE) {
        clean_info_list_.pop_front();
    }
    clean_info_list_.push_back(std::move(clean_info));
}

std::vector<std::shared_ptr<TxnCleanInfo>> NewTxnManager::GetCleanInfoList() const {
    std::vector<std::shared_ptr<TxnCleanInfo>> clean_info_list;
    std::lock_guard<std::mutex> lock(clean_info_lock_);
    clean_info_list.reserve(clean_info_list_.size());
    for (const auto &clean_info : clean_info_list_) {
        clean_info_list.push_back(clean_info);
    }
    return clean_info_list;
}

} // namespace infinity

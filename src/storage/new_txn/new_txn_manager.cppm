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

export module infinity_core:new_txn_manager;

import :buffer_manager;
import :txn_state;
import :default_values;
import :status;

namespace infinity {

class TxnAllocator;
class TxnAllocatorTask;
class WalManager;
class Storage;
class NewTxn;
class KVStore;
struct WalEntry;
class SystemCache;
struct TxnContext;
struct BGTaskInfo;
struct TxnInfo;
struct TxnCheckpointInfo;
struct TxnCompactInfo;
struct TxnOptimizeInfo;
struct TxnImportInfo;
struct TxnCleanInfo;

export class NewTxnManager {
public:
    explicit NewTxnManager(Storage *storage, KVStore *kv_store, TxnTimeStamp start_ts);

    ~NewTxnManager();

    void Start();

    void Stop();

    std::shared_ptr<NewTxn> BeginTxnShared(std::unique_ptr<std::string> txn_text, TransactionType txn_type);
    NewTxn *BeginTxn(std::unique_ptr<std::string> txn_text, TransactionType txn_type);
    std::unique_ptr<NewTxn> BeginReplayTxn(const std::shared_ptr<WalEntry> &replay_entries);
    std::unique_ptr<NewTxn> BeginRecoveryTxn();

    inline void Lock() { locker_.lock(); }

    inline void UnLock() { locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    TxnTimeStamp GetReadCommitTS(NewTxn *txn);

    TxnTimeStamp GetWriteCommitTS(std::shared_ptr<NewTxn> txn);

    // TxnTimeStamp GetCurrentTS();

    bool CheckConflict1(NewTxn *txn, std::string &conflict_reason, bool &retry_query);
    void SaveOrResetMetaCacheForReadTxn(NewTxn *txn);

    void SendToWAL(NewTxn *txn);

    //[[nodiscard]]
    Status CommitTxn(NewTxn *txn, TxnTimeStamp *commit_ts_ptr = nullptr);
    void CommitReplayTxn(NewTxn *txn);

    Status RollBackTxn(NewTxn *txn);

    size_t ActiveTxnCount();

    std::vector<TxnInfo> GetTxnInfoArray() const;

    std::unique_ptr<TxnInfo> GetTxnInfoByID(TransactionID txn_id) const;

    std::vector<std::shared_ptr<TxnContext>> GetTxnContextHistoriesNoLock() const;
    std::vector<std::shared_ptr<TxnContext>> GetTxnContextHistories() const;

    // This function is only used for unit test.
    void SetNewSystemTS(TxnTimeStamp new_system_ts);

    void SetCurrentTransactionID(TransactionID latest_transaction_id);

    TransactionID current_transaction_id() const {
        std::lock_guard guard(locker_);
        return current_transaction_id_;
    }

    TxnTimeStamp CurrentTS() const {
        std::lock_guard guard(locker_);
        return current_ts_;
    }

    TxnTimeStamp PrepareCommitTS() const {
        std::lock_guard guard(locker_);
        return prepare_commit_ts_;
    }

    TxnTimeStamp GetOldestAliveTS();

    void IncreaseCommittedTxnCount() { ++total_committed_txn_count_; }

    u64 total_committed_txn_count() const { return total_committed_txn_count_; }

    void IncreaseRollbackedTxnCount() { ++total_rollbacked_txn_count_; }

    u64 total_rollbacked_txn_count() const { return total_rollbacked_txn_count_; }

    WalManager *wal_manager() const { return wal_mgr_; }
    Storage *storage() const { return storage_; }

    void CommitBottom(NewTxn *txn);
    void CommitKVInstance(NewTxn *txn);

private:
    void UpdateCatalogCache(NewTxn *txn);

    void CleanupTxn(NewTxn *txn);

    void CleanupTxnBottomNolock(TransactionID txn_id, TxnTimeStamp begin_ts);

public:
    // Only used by follower and learner when received the replicated log from leader
    void SetStartTS(TxnTimeStamp new_start_ts) {
        std::lock_guard guard(locker_);
        current_ts_ = new_start_ts;
    }

    void PrintAllKeyValue() const;

    void PrintPMKeyValue() const;

    void PrintAllDroppedKeys() const;

    size_t KeyValueNum() const;

    KVStore *kv_store() const { return kv_store_; }

    std::vector<std::shared_ptr<NewTxn>> GetCheckCandidateTxns(NewTxn *this_txn);

    void SubmitForAllocation(std::shared_ptr<TxnAllocatorTask> txn_allocator_task);

    void SetSystemCache(std::unique_ptr<SystemCache> system_cache);

    void RemoveMapElementForRollbackNoLock(TxnTimeStamp commit_ts, NewTxn *txn_ptr);

    SystemCache *GetSystemCachePtr() const;

    TxnTimeStamp LastKVCommitTS() const {
        std::lock_guard guard(locker_);
        return last_kv_commit_ts_;
    }

    void UpdateTxnBeginTSAndKVInstance(NewTxn *txn);

private:
    mutable std::mutex locker_{};
    Storage *storage_{};
    BufferManager *buffer_mgr_{};
    WalManager *wal_mgr_{};

    std::unordered_map<TransactionID, std::shared_ptr<NewTxn>> txn_map_{};
    std::deque<std::shared_ptr<TxnContext>> txn_context_histories_{};

    KVStore *kv_store_;

    std::map<TxnTimeStamp, u64> begin_txn_map_{};                     // Used for clean up TS and txn conflict check txns
    std::map<TxnTimeStamp, std::shared_ptr<NewTxn>> check_txn_map_{}; // sorted by commit ts
    std::map<TxnTimeStamp, std::shared_ptr<NewTxn>> bottom_txns_;     // sorted by commit ts

    std::map<TxnTimeStamp, NewTxn *> wait_conflict_ck_{}; // sorted by commit ts

    TransactionID current_transaction_id_{0}; // The current transaction id, used for new txn
    TxnTimeStamp current_ts_{};               // The next txn ts
    TxnTimeStamp prepare_commit_ts_{};
    TxnTimeStamp last_kv_commit_ts_{};        // record last kv commit ts, used for conflict check
    TxnTimeStamp last_commit_ts_{};           // record last commit ts, used for conflict check
    TxnTimeStamp ckp_begin_ts_ = UNCOMMIT_TS; // current ckp begin ts, UNCOMMIT_TS if no ckp is happening, UNCOMMIT_TS is a maximum u64 integer

    // For stop the txn manager
    std::atomic_bool is_running_{false};

    std::atomic_uint64_t total_committed_txn_count_{0};
    std::atomic_uint64_t total_rollbacked_txn_count_{0};

    std::shared_ptr<SystemCache> system_cache_{};

private:
    // Also protected by locker_, to contain append / import / create index / dump mem index txn.
    std::map<TxnTimeStamp, std::shared_ptr<TxnAllocatorTask>> allocator_map_{};
    std::shared_ptr<TxnAllocator> txn_allocator_{};

public:
    void CollectInfo(NewTxn *txn);

    // Background task info list
    void AddTaskInfo(std::shared_ptr<BGTaskInfo> task_info);
    std::vector<std::shared_ptr<BGTaskInfo>> GetTaskInfoList() const;

    void AddCheckpointInfo(std::shared_ptr<TxnCheckpointInfo> ckp_info);
    std::vector<std::shared_ptr<TxnCheckpointInfo>> GetCheckpointInfoList() const;

    void AddCompactInfo(std::shared_ptr<TxnCompactInfo> ckp_info);
    std::vector<std::shared_ptr<TxnCompactInfo>> GetCompactInfoList() const;

    void AddOptimizeInfo(std::shared_ptr<TxnOptimizeInfo> ckp_info);
    std::vector<std::shared_ptr<TxnOptimizeInfo>> GetOptimizeInfoList() const;

    void AddImportInfo(std::shared_ptr<TxnImportInfo> ckp_info);
    std::vector<std::shared_ptr<TxnImportInfo>> GetImportInfoList() const;

    void AddCleanInfo(std::shared_ptr<TxnCleanInfo> ckp_info);
    std::vector<std::shared_ptr<TxnCleanInfo>> GetCleanInfoList() const;

private:
    mutable std::mutex task_lock_{};
    std::deque<std::shared_ptr<BGTaskInfo>> task_info_list_{};

    mutable std::mutex checkpoint_info_lock_{};
    std::deque<std::shared_ptr<TxnCheckpointInfo>> ckp_info_list_{};

    mutable std::mutex compact_info_lock_{};
    std::deque<std::shared_ptr<TxnCompactInfo>> compact_info_list_{};

    mutable std::mutex optimize_info_lock_{};
    std::deque<std::shared_ptr<TxnOptimizeInfo>> optimize_info_list_{};

    mutable std::mutex import_info_lock_{};
    std::deque<std::shared_ptr<TxnImportInfo>> import_info_list_{};

    mutable std::mutex clean_info_lock_{};
    std::deque<std::shared_ptr<TxnCleanInfo>> clean_info_list_{};
};

} // namespace infinity

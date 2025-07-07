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

export module infinity_core:new_txn_manager;

import :stl;
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

export struct TxnInfo {
    TransactionID txn_id_;
    SharedPtr<String> txn_text_;
};

export class NewTxnManager {
public:
    explicit NewTxnManager(Storage *storage, KVStore *kv_store, TxnTimeStamp start_ts);

    ~NewTxnManager();

    void Start();

    void Stop();

    SharedPtr<NewTxn> BeginTxnShared(UniquePtr<String> txn_text, TransactionType txn_type);
    NewTxn *BeginTxn(UniquePtr<String> txn_text, TransactionType txn_type);
    UniquePtr<NewTxn> BeginReplayTxn(const SharedPtr<WalEntry> &replay_entries);
    UniquePtr<NewTxn> BeginRecoveryTxn();

    NewTxn *GetTxn(TransactionID txn_id) const;

    TxnState GetTxnState(TransactionID txn_id) const;

    inline void Lock() { locker_.lock(); }

    inline void UnLock() { locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    TxnTimeStamp GetReadCommitTS(NewTxn *txn);

    TxnTimeStamp GetWriteCommitTS(SharedPtr<NewTxn> txn);

    // Optional<String> CheckTxnConflict(NewTxn *txn);

    bool CheckConflict1(NewTxn *txn, String &conflict_reason, bool &retry_query);

    void SendToWAL(NewTxn *txn);

    //[[nodiscard]]
    Status CommitTxn(NewTxn *txn, TxnTimeStamp *commit_ts_ptr = nullptr);
    void CommitReplayTxn(NewTxn *txn);

    Status RollBackTxn(NewTxn *txn);

    SizeT ActiveTxnCount();

    Vector<TxnInfo> GetTxnInfoArray() const;

    UniquePtr<TxnInfo> GetTxnInfoByID(TransactionID txn_id) const;

    Vector<SharedPtr<TxnContext>> GetTxnContextHistories() const;

    // This function is only used for unit test.
    void SetNewSystemTS(TxnTimeStamp new_system_ts);

    void SetCurrentTransactionID(TransactionID latest_transaction_id);

    TransactionID current_transaction_id() const { return current_transaction_id_; }

    TxnTimeStamp CurrentTS() const { return current_ts_; }

    TxnTimeStamp PrepareCommitTS() const { return prepare_commit_ts_; }

    TxnTimeStamp GetOldestAliveTS();

    void IncreaseCommittedTxnCount() { ++total_committed_txn_count_; }

    u64 total_committed_txn_count() const { return total_committed_txn_count_; }

    void IncreaseRollbackedTxnCount() { ++total_rollbacked_txn_count_; }

    u64 total_rollbacked_txn_count() const { return total_rollbacked_txn_count_; }

    WalManager *wal_manager() const { return wal_mgr_; }
    Storage *storage() const { return storage_; }

    void CommitBottom(NewTxn *txn);

private:
    void UpdateCatalogCache(NewTxn *txn);

    void CleanupTxn(NewTxn *txn);

    void CleanupTxnBottomNolock(TransactionID txn_id, TxnTimeStamp begin_ts);

public:
    // Only used by follower and learner when received the replicated log from leader
    void SetStartTS(TxnTimeStamp new_start_ts) { current_ts_ = new_start_ts; }

    void PrintAllKeyValue() const;

    SizeT KeyValueNum() const;

    KVStore *kv_store() const { return kv_store_; }

    Vector<SharedPtr<NewTxn>> GetCheckTxns(TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    void SubmitForAllocation(SharedPtr<TxnAllocatorTask> txn_allocator_task);

    void RemoveFromAllocation(TxnTimeStamp commit_ts);

    void SetSystemCache();

    void RemoveMapElementForRollbackNoLock(TxnTimeStamp commit_ts, NewTxn *txn_ptr);

private:
    mutable std::mutex locker_{};
    Storage *storage_{};
    BufferManager *buffer_mgr_{};
    WalManager *wal_mgr_{};

    HashMap<TransactionID, SharedPtr<NewTxn>> txn_map_{};
    Deque<SharedPtr<TxnContext>> txn_context_histories_{};

    KVStore *kv_store_;

    Map<TxnTimeStamp, u64> begin_txn_map_{};               // Used for clean up TS and txn conflict check txns
    Map<TxnTimeStamp, SharedPtr<NewTxn>> check_txn_map_{}; // sorted by commit ts
    Map<TxnTimeStamp, SharedPtr<NewTxn>> bottom_txns_;     // sorted by commit ts

    Map<TxnTimeStamp, NewTxn *> wait_conflict_ck_{}; // sorted by commit ts

    TransactionID current_transaction_id_{0}; // The current transaction id, used for new txn
    TxnTimeStamp current_ts_{};               // The next txn ts
    TxnTimeStamp prepare_commit_ts_{};
    TxnTimeStamp ckp_begin_ts_ = UNCOMMIT_TS; // current ckp begin ts, UNCOMMIT_TS if no ckp is happening, UNCOMMIT_TS is a maximum u64 integer

    // For stop the txn manager
    atomic_bool is_running_{false};

    Atomic<u64> total_committed_txn_count_{0};
    Atomic<u64> total_rollbacked_txn_count_{0};

    SharedPtr<SystemCache> system_cache_{};

private:
    // Also protected by locker_, to contain append / import / create index / dump mem index txn.
    Map<TxnTimeStamp, SharedPtr<TxnAllocatorTask>> allocator_map_{};
    SharedPtr<TxnAllocator> txn_allocator_{};

public:
    // Background task info list
    void AddTaskInfo(SharedPtr<BGTaskInfo> task_info);
    Vector<SharedPtr<BGTaskInfo>> GetTaskInfoList() const;

private:
    mutable std::mutex task_lock_{};
    Deque<SharedPtr<BGTaskInfo>> task_info_list_{};
};

} // namespace infinity

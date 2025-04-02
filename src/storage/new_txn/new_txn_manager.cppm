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

export module new_txn_manager;

import stl;
import buffer_manager;
import txn_state;
import default_values;
import txn_context;
import txn_manager;
import status;

namespace infinity {

class BGTaskProcessor;
class WalManager;
class CatalogDeltaEntry;
class NewTxn;
class KVStore;

export class NewTxnManager {
public:
    explicit NewTxnManager(BufferManager *buffer_mgr, WalManager *wal_mgr, KVStore *kv_store, TxnTimeStamp start_ts);

    ~NewTxnManager();

    NewTxn *BeginTxn(UniquePtr<String> txn_text, TransactionType txn_type);

    NewTxn *GetTxn(TransactionID txn_id) const;

    TxnState GetTxnState(TransactionID txn_id) const;

    bool CheckIfCommitting(TransactionID txn_id, TxnTimeStamp begin_ts);

    inline void Lock() { locker_.lock(); }

    inline void UnLock() { locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    TxnTimeStamp GetReadCommitTS(NewTxn *txn);

    TxnTimeStamp GetWriteCommitTS(SharedPtr<NewTxn> txn);

    TxnTimeStamp GetReplayWriteCommitTS(NewTxn *txn);

    // Optional<String> CheckTxnConflict(NewTxn *txn);

    bool CheckConflict1(NewTxn *txn, String &conflict_reason);

    void SendToWAL(NewTxn *txn);

    void Start();

    void Stop();

    bool Stopped();

    Status CommitTxn(NewTxn *txn, TxnTimeStamp *commit_ts_ptr = nullptr);

    Status RollBackTxn(NewTxn *txn);

    SizeT ActiveTxnCount();

    Vector<TxnInfo> GetTxnInfoArray() const;

    UniquePtr<TxnInfo> GetTxnInfoByID(TransactionID txn_id) const;

    Vector<SharedPtr<TxnContext>> GetTxnContextHistories() const;

    TxnTimeStamp CurrentTS() const { return current_ts_; }

    TxnTimeStamp PrepareCommitTS() const { return prepare_commit_ts_; }

    TxnTimeStamp GetNewTimeStamp();

    TxnTimeStamp GetCleanupScanTS1();

    void IncreaseCommittedTxnCount() { ++total_committed_txn_count_; }

    u64 total_committed_txn_count() const { return total_committed_txn_count_; }

    void IncreaseRollbackedTxnCount() { ++total_rollbacked_txn_count_; }

    u64 total_rollbacked_txn_count() const { return total_rollbacked_txn_count_; }

    WalManager *wal_manager() const { return wal_mgr_; }

    void CommitBottom(TxnTimeStamp commit_ts, TransactionID txn_id);

private:
    void CleanupTxn(NewTxn *txn, bool commit);

public:
    u64 NextSequence() { return ++sequence_; }

    bool InCheckpointProcess(TxnTimeStamp commit_ts);

    // Only used by follower and learner when received the replicated log from leader
    void SetStartTS(TxnTimeStamp new_start_ts) { current_ts_ = new_start_ts; }

    void PrintAllKeyValue() const;

    SizeT KeyValueNum() const;

    KVStore *kv_store() const { return kv_store_; }

    Status Cleanup();

    Vector<SharedPtr<NewTxn>> GetCheckTxns(TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

private:
    mutable std::mutex locker_{};
    BufferManager *buffer_mgr_{};

    HashMap<TransactionID, SharedPtr<NewTxn>> txn_map_{};
    Deque<SharedPtr<TxnContext>> txn_context_histories_{};

    WalManager *wal_mgr_;
    KVStore *kv_store_;

    Set<Pair<TxnTimeStamp, TransactionID>> begin_txns_;
    Deque<SharedPtr<NewTxn>> check_txns_; //

    Map<TxnTimeStamp, NewTxn *> wait_conflict_ck_{}; // sorted by commit ts

    Atomic<TxnTimeStamp> current_ts_{}; // The next txn ts
    TxnTimeStamp prepare_commit_ts_{};
    Atomic<TxnTimeStamp> ckp_begin_ts_ =
        UNCOMMIT_TS; // current ckp begin ts, UNCOMMIT_TS if no ckp is happening, UNCOMMIT_TS is a maximum u64 integer

    // For stop the txn manager
    atomic_bool is_running_{false};
    bool enable_compaction_{};

    u64 sequence_{};

    Atomic<u64> total_committed_txn_count_{0};
    Atomic<u64> total_rollbacked_txn_count_{0};
};

} // namespace infinity

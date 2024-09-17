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

export module txn_manager;

import stl;
import txn;
import buffer_manager;
import txn_state;
import wal_entry;
import default_values;

namespace infinity {

class BGTaskProcessor;
struct Catalog;
class WalManager;
class CatalogDeltaEntry;

export struct TxnInfo {
    TransactionID txn_id_;
    SharedPtr<String> txn_text_;
};

export class TxnManager {
public:
    explicit TxnManager(Catalog *catalog,
                        BufferManager *buffer_mgr,
                        BGTaskProcessor *task_processor,
                        WalManager *wal_mgr,
                        TransactionID start_txn_id,
                        TxnTimeStamp start_ts);

    ~TxnManager() = default;

    Txn *BeginTxn(UniquePtr<String> txn_text, bool ckp_txn = false);

    Txn *GetTxn(TransactionID txn_id) const;

    TxnState GetTxnState(TransactionID txn_id) const;

    bool CheckIfCommitting(TransactionID txn_id, TxnTimeStamp begin_ts);

    inline void Lock() { locker_.lock(); }

    inline void UnLock() { locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    Catalog *GetCatalog() const { return catalog_; }

    BGTaskProcessor *bg_task_processor() const { return bg_task_processor_; }

    TxnTimeStamp GetCommitTimeStampR(Txn *txn);

    TxnTimeStamp GetCommitTimeStampW(Txn *txn);

    bool CheckConflict(Txn *txn);

    void SendToWAL(Txn *txn);

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    void Start();

    void Stop();

    bool Stopped();

    TxnTimeStamp CommitTxn(Txn *txn);

    void RollBackTxn(Txn *txn);

    SizeT ActiveTxnCount();

    Vector<TxnInfo> GetTxnInfoArray() const;

    UniquePtr<TxnInfo> GetTxnInfoByID(TransactionID txn_id) const;

    TxnTimeStamp CurrentTS() const;

    TxnTimeStamp GetNewTimeStamp();

    TxnTimeStamp GetCleanupScanTS();

    void IncreaseCommittedTxnCount() { ++total_committed_txn_count_; }

    u64 total_committed_txn_count() const { return total_committed_txn_count_; }

    void IncreaseRollbackedTxnCount() { ++total_rollbacked_txn_count_; }

    u64 total_rollbacked_txn_count() const { return total_rollbacked_txn_count_; }

    WalManager *wal_manager() const { return wal_mgr_; }

private:
    void FinishTxn(Txn *txn);

public:
    u64 NextSequence() { return ++sequence_; }

    bool InCheckpointProcess(TxnTimeStamp commit_ts);

private:
    Catalog *catalog_{};
    mutable std::mutex locker_{};
    BufferManager *buffer_mgr_{};
    BGTaskProcessor *bg_task_processor_{};
    HashMap<TransactionID, SharedPtr<Txn>> txn_map_{};
    WalManager *wal_mgr_;

    Deque<WeakPtr<Txn>> beginned_txns_; // sorted by begin ts
    HashSet<Txn *> finishing_txns_;     // the txns in committing stage, can use flat_map
    Deque<Txn *> finished_txns_;        // the txns that committed_ts

    Map<TxnTimeStamp, WalEntry *> wait_conflict_ck_{}; // sorted by commit ts

    Atomic<TxnTimeStamp> start_ts_{};         // The next txn ts
    TxnTimeStamp ckp_begin_ts_ = UNCOMMIT_TS; // cur ckp begin ts, 0 if no ckp is happening

    // For stop the txn manager
    atomic_bool is_running_{false};
    bool enable_compaction_{};

    u64 sequence_{};

    Atomic<u64> total_committed_txn_count_{0};
    Atomic<u64> total_rollbacked_txn_count_{0};
};

} // namespace infinity

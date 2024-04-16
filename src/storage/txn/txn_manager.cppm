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

namespace infinity {

class BGTaskProcessor;
struct Catalog;
class WalManager;
class CatalogDeltaEntry;

export class TxnManager {
public:
    explicit TxnManager(Catalog *catalog,
                        BufferManager *buffer_mgr,
                        BGTaskProcessor *task_processor,
                        WalManager *wal_mgr,
                        TransactionID start_txn_id,
                        TxnTimeStamp start_ts,
                        bool enable_compaction);

    ~TxnManager() { Stop(); }

    Txn *BeginTxn();

    Txn *GetTxn(TransactionID txn_id);

    SharedPtr<Txn> GetTxnPtr(TransactionID txn_id);

    TxnState GetTxnState(TransactionID txn_id);

    inline void Lock() { rw_locker_.lock(); }

    inline void UnLock() { rw_locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    Catalog *GetCatalog() const { return catalog_; }

    BGTaskProcessor *bg_task_processor() const { return bg_task_processor_; }

    TxnTimeStamp GetTimestamp();

    TxnTimeStamp GetBeginTimestamp(TransactionID txn_id);

    void Invalidate(TxnTimeStamp commit_ts);

    void SendToWAL(Txn *txn);

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    void Start();

    void Stop();

    bool Stopped();

    TxnTimeStamp CommitTxn(Txn *txn);

    void RollBackTxn(Txn *txn);

    void AddWaitFlushTxn(TransactionID txn_id);

    void RemoveWaitFlushTxns(const Vector<TransactionID> &txn_ids);

    TxnTimeStamp GetMinUnflushedTS();

    bool enable_compaction() const { return enable_compaction_; }

    u64 NextSequence() { return ++sequence_; }

private:
    TransactionID GetNewTxnID();

private:
    Catalog *catalog_{};
    std::shared_mutex rw_locker_{};
    BufferManager *buffer_mgr_{};
    BGTaskProcessor *bg_task_processor_{};
    HashMap<TransactionID, SharedPtr<Txn>> txn_map_{};
    WalManager *wal_mgr_;

    TransactionID start_txn_id_{};
    // Use a variant of priority queue to ensure entries are putted to WalManager in the same order as commit_ts allocation.
    //    std::mutex mutex_;
    Atomic<TxnTimeStamp> start_ts_{}; // The next txn ts
    // Deque<TxnTimeStamp> ts_queue_{}; // the ts queue
    Map<TxnTimeStamp, TransactionID> ts_map_{};
    HashSet<TransactionID> wait_flush_txns_{};

    //    Map<TxnTimeStamp, SharedPtr<WalEntry>> priority_que_; // TODO: use C++23 std::flat_map?
    // For stop the txn manager
    atomic_bool is_running_{false};
    bool enable_compaction_{};

    u64 sequence_{};
};

} // namespace infinity

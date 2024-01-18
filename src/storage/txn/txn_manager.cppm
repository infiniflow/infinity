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
import wal;

namespace infinity {

using PutWalEntryFn = StdFunction<void(SharedPtr<WalEntry>)>;

struct NewCatalog;

export class TxnManager {
public:
    explicit TxnManager(NewCatalog *catalog,
                        BufferManager *buffer_mgr,
                        PutWalEntryFn put_wal_entry_fn,
                        TransactionID start_txn_id = 0,
                        TxnTimeStamp start_ts = 1);

    ~TxnManager() { Stop(); }

    Txn *CreateTxn();

    Txn *GetTxn(TransactionID txn_id);

    TxnState GetTxnState(TransactionID txn_id);

    inline void Lock() { rw_locker_.lock(); }

    inline void UnLock() { rw_locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    TxnTimeStamp GetTimestamp(bool prepare_wal = false);

    void Invalidate(TxnTimeStamp commit_ts);

    void PutWalEntry(SharedPtr<WalEntry> entry);

    void Start();

    void Stop();

    bool Stopped();

    TxnTimeStamp CommitTxn(Txn *txn);

    void RollBackTxn(Txn *txn);

private:
    u64 GetNewTxnID();

private:
    NewCatalog *catalog_{};
    std::shared_mutex rw_locker_{};
    TransactionID txn_id_{};
    BufferManager *buffer_mgr_{};
    HashMap<TransactionID, UniquePtr<Txn>> txn_map_{};
    // PutWalEntry function
    PutWalEntryFn put_wal_entry_{};

    // Use a variant of priority queue to ensure entries are putted to WalManager in the same order as commit_ts allocation.
    std::mutex mutex_;
    TxnTimeStamp txn_ts_{};
    Map<TxnTimeStamp, SharedPtr<WalEntry>> priority_que_; // TODO: use C++23 std::flat_map?
    // For stop the txn manager
    atomic_bool is_running_{false};
};

} // namespace infinity

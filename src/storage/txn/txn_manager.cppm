//
// Created by jinhai on 23-6-4.
//

module;

export module txn_manager;

import stl;
import txn;
import buffer_manager;
import txn_state;
import wal_entry;

namespace infinity {

using PutWalEntryFn = StdFunction<void(SharedPtr<WalEntry>)>;

// class WalEntry;
// class BufferManager;
class NewCatalog;

export class TxnManager {
public:
    explicit TxnManager(NewCatalog *catalog,
                        BufferManager *buffer_mgr,
                        PutWalEntryFn put_wal_entry_fn,
                        u64 start_txn_id = 0,
                        TxnTimeStamp start_ts = 1);

    ~TxnManager() { Stop(); }

    Txn *CreateTxn();

    void DestroyTxn(u64 txn_id);

    Txn *GetTxn(u64 txn_id);

    TxnState GetTxnState(u64 txn_id);

    inline void Lock() { rw_locker_.lock(); }

    inline void UnLock() { rw_locker_.unlock(); }

    BufferManager *GetBufferMgr() const { return buffer_mgr_; }

    TxnTimeStamp GetTimestamp(bool prepare_wal = false);

    void Invalidate(TxnTimeStamp commit_ts);

    void PutWalEntry(SharedPtr<WalEntry> entry);

    void Start();

    void Stop();

    bool Stopped();

private:
    u64 GetNewTxnID();

private:
    NewCatalog *catalog_{};
    RWMutex rw_locker_{};
    u64 txn_id_{};
    BufferManager *buffer_mgr_{};
    HashMap<u64, UniquePtr<Txn>> txn_map_{};
    // PutWalEntry function
    PutWalEntryFn put_wal_entry_{};

    // Use a variant of priority queue to ensure entries are putted to WalManager in the same order as commit_ts allocation.
    Mutex mutex_;
    TxnTimeStamp txn_ts_{};
    Map<TxnTimeStamp, SharedPtr<WalEntry>> priority_que_; // TODO: use C++23 std::flat_map?
    // For stop the txn manager
    atomic_bool is_running_{false};
};

} // namespace infinity

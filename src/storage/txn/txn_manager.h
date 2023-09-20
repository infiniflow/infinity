//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "txn.h"
#include "storage/wal/wal_entry.h"

namespace infinity {

using PutWalEntryFn = std::function<void(std::shared_ptr<WalEntry>)>;

class BufferManager;
class TxnManager {
public:
    explicit
    TxnManager(NewCatalog* catalog, BufferManager* buffer_mgr, PutWalEntryFn put_wal_entry_fn,  u64 start_txn_id = 0)
        : catalog_(catalog),
        buffer_mgr_(buffer_mgr),
        put_wal_entry_(put_wal_entry_fn),
        txn_id_(start_txn_id)
        {}

    Txn*
    CreateTxn();

    void
    DestroyTxn(u64 txn_id);

    Txn*
    GetTxn(u64 txn_id);

    TxnState
    GetTxnState(u64 txn_id);

    inline void
    Lock() {
        rw_locker_.lock();
    }

    inline void
    UnLock() {
        rw_locker_.unlock();
    }

    BufferManager*
    GetBufferMgr() const {
        return buffer_mgr_;
    }

    static inline TxnTimeStamp
    GetTimestamp() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }

    void PutWalEntry(std::shared_ptr<WalEntry> entry) {
        if (put_wal_entry_ != nullptr)
            put_wal_entry_(entry);
    }

private:
    u64
    GetNewTxnID();

private:
    NewCatalog* catalog_{};
    RWMutex rw_locker_{};
    u64 txn_id_{};
    BufferManager* buffer_mgr_{};
    HashMap<u64, UniquePtr<Txn>> txn_map_{};
    PutWalEntryFn put_wal_entry_{};
};

}

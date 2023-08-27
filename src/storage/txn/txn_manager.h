//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "txn.h"

namespace infinity {

class BufferManager;
class TxnManager {
public:
    explicit
    TxnManager(NewCatalog* catalog, BufferManager* buffer_mgr, u64 start_txn_id = 0)
        : catalog_(catalog),
        buffer_mgr_(buffer_mgr),
        txn_id_(start_txn_id)
        {}

    Txn*
    CreateTxn();

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

private:
    u64
    GetNewTxnID();

private:
    NewCatalog* catalog_{};
    RWMutex rw_locker_{};
    u64 txn_id_{};
    BufferManager* buffer_mgr_{};
    HashMap<u64, UniquePtr<Txn>> txn_map_{};
};

}

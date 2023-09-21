//
// Created by jinhai on 23-6-4.
//

#include "txn_manager.h"
#include "storage/meta/catalog.h"

namespace infinity {

Txn*
TxnManager::CreateTxn() {
    rw_locker_.lock();
    u64 new_txn_id = GetNewTxnID();
    UniquePtr<Txn> new_txn = MakeUnique<Txn>(this, catalog_, new_txn_id);
    Txn* res = new_txn.get();
    txn_map_[new_txn_id] = std::move(new_txn);
    rw_locker_.unlock();
    return res;
}

void
TxnManager::DestroyTxn(u64 txn_id) {
    rw_locker_.lock();
    txn_map_.erase(txn_id);
    rw_locker_.unlock();
}

Txn*
TxnManager::GetTxn(u64 txn_id) {
    rw_locker_.lock_shared();
    Txn* res = txn_map_[txn_id].get();
    rw_locker_.unlock_shared();
    return res;
}

TxnState
TxnManager::GetTxnState(u64 txn_id) {
    std::shared_lock<RWMutex> r_locker(rw_locker_);
    Txn* txn_ptr = txn_map_[txn_id].get();
    TxnState res = txn_ptr->GetTxnState();
    return res;
}

u64
TxnManager::GetNewTxnID() {
    u64 new_txn_id = ++ catalog_->next_txn_id_;
    return new_txn_id;
}

}

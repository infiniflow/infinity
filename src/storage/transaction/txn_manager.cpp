//
// Created by jinhai on 23-6-4.
//

#include "txn_manager.h"

namespace infinity {

UniquePtr<Txn>
TxnManager::CreateTxn() {
    rw_locker_.lock();
    u64 new_txn_id = GetNewTxnID();
    rw_locker_.unlock();
    UniquePtr<Txn> new_txn = MakeUnique<Txn>(catalog_, new_txn_id);
    return new_txn;
}

u64
TxnManager::GetNewTxnID() {
    ++ txn_id_;
    return txn_id_;
}

}

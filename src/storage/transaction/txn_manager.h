//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "txn.h"

namespace infinity {

class TxnManager {
public:
    explicit
    TxnManager(NewCatalog* catalog, u64 start_txn_id = 0)
        : txn_id_(start_txn_id),
          catalog_(catalog) {}

    UniquePtr<Txn>
    CreateTxn();

private:
    u64
    GetNewTxnID();

private:
    NewCatalog* catalog_{};
    RWMutex rw_locker_{};
    u64 txn_id_{};
};

}

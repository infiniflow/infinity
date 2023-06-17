//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <shared_mutex>
#include "common/types/internal_types.h"
#include "storage/common/async_batch_processor.h"
#include "storage/meta/entry/db_entry.h"
#include "db_meta.h"
//#include "storage/transaction/txn.h"

namespace infinity {

class Txn;
class NewCatalog {
public:
    NewCatalog(UniquePtr<String> dir,
            UniquePtr<AsyncBatchProcessor> scheduler);

    EntryResult
    CreateDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts);

    EntryResult
    DropDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts);

    EntryResult
    GetDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts);

    Vector<DBEntry*>
    Databases(Txn* txn);

private:
    UniquePtr<String> dir_{nullptr};
    UniquePtr<AsyncBatchProcessor> scheduler_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    RWMutex rw_lock_;
};

}

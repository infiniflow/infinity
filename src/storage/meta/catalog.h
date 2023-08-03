//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <shared_mutex>
#include "common/types/internal_types.h"
#include "storage/common/async_batch_processor.h"
#include "storage/meta/entry/db_entry.h"
#include "db_meta.h"
#include "storage/txn/txn_context.h"
//#include "storage/txn/txn.h"

namespace infinity {

class Txn;
class NewCatalog {
public:
    NewCatalog(UniquePtr<String> dir,
               void* buffer_mgr,
               UniquePtr<AsyncBatchProcessor> scheduler);

    EntryResult
    CreateDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    EntryResult
    DropDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    EntryResult
    GetDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts);

    void
    RemoveDBEntry(const String& db_name, u64 txn_id, TxnContext* txn_context);

    Vector<DBEntry*>
    Databases(Txn* txn);

private:
    UniquePtr<String> dir_{nullptr};
    void* buffer_mgr_{};
    UniquePtr<AsyncBatchProcessor> scheduler_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    RWMutex rw_locker_;
};

}

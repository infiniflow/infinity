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
struct NewCatalog {
public:
    NewCatalog(SharedPtr<String> dir,
               UniquePtr<AsyncBatchProcessor> scheduler);

public:
    static EntryResult
    CreateDatabase(NewCatalog* catalog,
                   const String& db_name,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnContext* txn_context);

    static EntryResult
    DropDatabase(NewCatalog* catalog,
                 const String& db_name,
                 u64 txn_id,
                 TxnTimeStamp begin_ts,
                 TxnContext* txn_context);

    static EntryResult
    GetDatabase(NewCatalog* catalog,
                const String& db_name,
                u64 txn_id,
                TxnTimeStamp begin_ts);

    static void
    RemoveDBEntry(NewCatalog* catalog,
                  const String& db_name,
                  u64 txn_id,
                  TxnContext* txn_context);

    static Vector<DBEntry*>
    Databases(NewCatalog* catalog, Txn* txn);


    static nlohmann::json
    Serialize(const NewCatalog* catalog);

    static SharedPtr<NewCatalog>
    Deserialize(const nlohmann::json& catalog_json);
public:
    SharedPtr<String> current_dir_{nullptr};
    UniquePtr<AsyncBatchProcessor> scheduler_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    RWMutex rw_locker_;
};

}

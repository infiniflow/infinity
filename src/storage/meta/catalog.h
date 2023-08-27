//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <shared_mutex>
#include "common/types/internal_types.h"
#include "storage/common/async_batch_processor.h"
#include "storage/meta/entry/db_entry.h"
#include "db_meta.h"
//#include "storage/txn/txn.h"

namespace infinity {

class Txn;
class TxnManager;
struct NewCatalog {
public:
    explicit
    NewCatalog(SharedPtr<String> dir);

public:
    static EntryResult
    CreateDatabase(NewCatalog* catalog,
                   const String& db_name,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnManager* txn_mgr);

    static EntryResult
    DropDatabase(NewCatalog* catalog,
                 const String& db_name,
                 u64 txn_id,
                 TxnTimeStamp begin_ts,
                 TxnManager* txn_mgr);

    static EntryResult
    GetDatabase(NewCatalog* catalog,
                const String& db_name,
                u64 txn_id,
                TxnTimeStamp begin_ts);

    static void
    RemoveDBEntry(NewCatalog* catalog,
                  const String& db_name,
                  u64 txn_id,
                  TxnManager* txn_mgr);

    static Vector<DBEntry*>
    Databases(NewCatalog* catalog, Txn* txn);


    static nlohmann::json
    Serialize(const NewCatalog* catalog);

    static void
    Deserialize(const nlohmann::json& catalog_json,
                BufferManager* buffer_mgr,
                UniquePtr<NewCatalog>& catalog);
public:
    SharedPtr<String> current_dir_{nullptr};
    HashMap<String, UniquePtr<DBMeta>> databases_{};
    u64 next_txn_id_{};
    RWMutex rw_locker_;
};

}

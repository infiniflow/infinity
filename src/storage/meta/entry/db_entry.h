//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "base_entry.h"
#include "common/types/internal_types.h"
#include "parser/statement/extra/extra_ddl_info.h"
#include "table_collection_entry.h"
#include "storage/meta/table_collection_meta.h"
#include "storage/common/table_collection_detail.h"

namespace infinity {

class TxnManager;

class DBEntry : public BaseEntry {
public:
    explicit
    DBEntry(const SharedPtr<String>& base_dir,
            SharedPtr<String> db_name,
            u64 txn_id,
            TxnTimeStamp begin_ts)
            : BaseEntry(EntryType::kDatabase), base_dir_(base_dir), db_name_(std::move(db_name)) {
        // current_dir_ = MakeShared<String>(*base_dir + "/txn_" + std::to_string(txn_id));
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

public:
    static EntryResult
    CreateTableCollection(DBEntry* db_entry,
                          TableCollectionType table_collection_type,
                          const SharedPtr<String>& table_collection_name,
                          const Vector<SharedPtr<ColumnDef>>& columns,
                          u64 txn_id,
                          TxnTimeStamp begin_ts,
                          TxnManager* txn_mgr);

    static EntryResult
    DropTableCollection(DBEntry* db_entry,
                        const String& table_collection_name,
                        ConflictType conflict_type,
                        u64 txn_id,
                        TxnTimeStamp begin_ts,
                        TxnManager* txn_mgr);

    static EntryResult
    GetTableCollection(DBEntry* db_entry,
                       const String& table_collection_name,
                       u64 txn_id,
                       TxnTimeStamp begin_ts);

    static void
    RemoveTableCollectionEntry(DBEntry* db_entry,
                               const String& table_collection_name,
                               u64 txn_id,
                               TxnManager* txn_mgr);

    static Vector<TableCollectionEntry*>
    TableCollections(DBEntry* db_entry,
                     u64 txn_id,
                     TxnTimeStamp begin_ts);

    static Vector<TableCollectionDetail>
    GetTableCollectionsDetail(DBEntry* db_entry,
                              u64 txn_id,
                              TxnTimeStamp);

    static SharedPtr<String>
    ToString(DBEntry* db_entry);

    static nlohmann::json
    Serialize(const DBEntry* db_entry);

    static UniquePtr<DBEntry>
    Deserialize(const nlohmann::json& db_entry_json,
                BufferManager* buffer_mgr);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> base_dir_{};
    SharedPtr<String> db_name_{};
    HashMap<String, UniquePtr<TableCollectionMeta>> tables_{};
};

}

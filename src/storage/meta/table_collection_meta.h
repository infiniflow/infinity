//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>


#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "parser/statement/extra/create_table_info.h"

namespace infinity {

class TxnManager;

class DBEntry;

class BufferManager;

struct TableCollectionMeta {
public:
    explicit
    TableCollectionMeta(const SharedPtr<String>& base_dir,
                        SharedPtr<String> name,
                        DBEntry* db_entry)
            : base_dir_(base_dir), table_collection_name_(std::move(name)), db_entry_(db_entry) {
    }

public:
    static EntryResult
    CreateNewEntry(TableCollectionMeta* table_meta,
                   TableCollectionType table_collection_type,
                   const SharedPtr<String>& table_collection_name,
                   const Vector<SharedPtr<ColumnDef>>& columns,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnManager* txn_mgr);

    static EntryResult
    DropNewEntry(TableCollectionMeta* table_meta,
                 u64 txn_id,
                 TxnTimeStamp begin_ts,
                 TxnManager* txn_mgr,
                 const String& table_name,
                 ConflictType conflict_type);

    static void
    DeleteNewEntry(TableCollectionMeta* table_meta,
                   u64 txn_id,
                   TxnManager* txn_mgr);

    static EntryResult
    GetEntry(TableCollectionMeta* table_meta,
             u64 txn_id,
             TxnTimeStamp begin_ts);

    static SharedPtr<String>
    ToString(TableCollectionMeta* table_meta);

    static inline DBEntry*
    GetDBEntry(TableCollectionMeta* table_meta) {
        return table_meta->db_entry_;
    }

    static nlohmann::json
    Serialize(const TableCollectionMeta* table_meta);

    static UniquePtr<TableCollectionMeta>
    Deserialize(const nlohmann::json& table_meta_json,
                DBEntry* db_entry,
                BufferManager* buffer_mgr);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> table_collection_name_{};
    SharedPtr<String> base_dir_{};

    DBEntry* db_entry_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}


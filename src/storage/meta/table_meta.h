//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/txn/txn_context.h"
#include "storage/table_def.h"

namespace infinity {

struct TableMeta {
public:
    explicit
    TableMeta(const SharedPtr<String>& base_dir,
              String name,
              void* db_entry) : base_dir_(base_dir), table_name_(std::move(name)), db_entry_(db_entry) {
    }

public:
    static EntryResult
    CreateNewEntry(TableMeta* table_meta,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnContext* txn_context,
                   const SharedPtr<TableDef>& table_def);

    static EntryResult
    DropNewEntry(TableMeta* table_meta,
                 u64 txn_id,
                 TxnTimeStamp begin_ts,
                 TxnContext* txn_context,
                 const String& table_name);

    static void
    DeleteNewEntry(TableMeta* table_meta,
                   u64 txn_id,
                   TxnContext* txn_context);

    static EntryResult
    GetEntry(TableMeta* table_meta,
             u64 txn_id,
             TxnTimeStamp begin_ts);

    static SharedPtr<String>
    ToString(TableMeta* table_meta);

    static inline void*
    GetDBEntry(TableMeta* table_meta) {
        return table_meta->db_entry_;
    }

public:
    RWMutex rw_locker_{};
    String table_name_{};
    SharedPtr<String> base_dir_{};

    void* db_entry_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}


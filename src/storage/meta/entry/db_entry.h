//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "base_entry.h"
#include "common/types/internal_types.h"
#include "table_entry.h"
#include "storage/meta/table_meta.h"

namespace infinity {

class DBEntry : public BaseEntry {
public:
    explicit
    DBEntry(const SharedPtr<String>& base_dir,
            String db_name,
            u64 txn_id,
            TxnTimeStamp begin_ts,
            TxnContext* txn_context)
        : BaseEntry(EntryType::kDatabase, txn_context), base_dir_(base_dir), db_name_(std::move(db_name)) {
        // current_dir_ = MakeShared<String>(*base_dir + "/txn_" + std::to_string(txn_id));
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

public:
    static EntryResult
    CreateTable(DBEntry* db_entry,
                const SharedPtr<TableDef>& table_def,
                u64 txn_id,
                TxnTimeStamp begin_ts,
                TxnContext* txn_context);

    static EntryResult
    DropTable(DBEntry* db_entry,
              const String& table_name,
              u64 txn_id,
              TxnTimeStamp begin_ts,
              TxnContext* txn_context);

    static EntryResult
    GetTable(DBEntry* db_entry,
             const String& table_name,
             u64 txn_id,
             TxnTimeStamp begin_ts);

    static void
    RemoveTableEntry(DBEntry* db_entry,
                     const String& table_name,
                     u64 txn_id,
                     TxnContext* txn_context);

    static SharedPtr<String>
    ToString(DBEntry* db_entry);

    static nlohmann::json
    Serialize(const DBEntry* db_entry);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> base_dir_{};
    String db_name_{};
    HashMap<String, UniquePtr<TableMeta>> tables_{};
};

}

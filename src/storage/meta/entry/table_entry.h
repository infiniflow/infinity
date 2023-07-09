//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "storage/table_def.h"

namespace infinity {

class TableEntry : public BaseEntry {
public:
    explicit
    TableEntry(UniquePtr<TableDef> table_def, void* db_entry, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context)
            : BaseEntry(EntryType::kTable, txn_context), table_def_(std::move(table_def)), db_entry_(db_entry) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    inline void*
    GetDBEntry() const {
        return db_entry_;
    }

    inline TableDef*
    GetTableDesc() const {
        return table_def_.get();
    }

private:
    RWMutex rw_locker_{};
    UniquePtr<TableDef> table_def_{};
    void* db_entry_{};
};

}

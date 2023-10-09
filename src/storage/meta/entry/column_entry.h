//
// Created by jinhai on 23-8-9.
//

#pragma once

#include <utility>

#include "db_entry.h"

#include "storage/table_def.h"

namespace infinity {

class ColumnEntry : public BaseEntry {
public:
    explicit ColumnEntry(SharedPtr<ColumnDef> column_def, void *table_entry, u64 txn_id, TxnTimeStamp begin_ts, void *buffer_mgr)
        : BaseEntry(EntryType::kTable), column_def_(std::move(column_def)), table_entry_(table_entry) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    inline void *GetTableEntry() const { return table_entry_; }

    inline ColumnDef *GetColumnDesc() const { return column_def_.get(); }

private:
    RWMutex rw_locker_{};
    SharedPtr<ColumnDef> column_def_{};
    void *table_entry_{};
};

} // namespace infinity

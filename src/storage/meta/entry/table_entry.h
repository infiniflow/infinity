//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "storage/table_def.h"
#include "storage/table/data_table.h"

namespace infinity {

class TableEntry : public BaseEntry {
public:
    explicit
    TableEntry(const SharedPtr<String>& dir,
                SharedPtr<TableDef> table_def,
                void* db_entry,
                u64 txn_id,
                TxnTimeStamp begin_ts,
                TxnContext* txn_context,
                void* buffer_mgr)
            : BaseEntry(EntryType::kTable, txn_context),
            dir_(dir), table_def_(std::move(table_def)), db_entry_(db_entry), buffer_mgr_(buffer_mgr) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
        if(table_def_ == nullptr) {
            data_table_ = nullptr;
        } else {
            data_table_ = DataTable::Make(dir_, table_def_, buffer_mgr_);
        }
    }

    inline void*
    GetDBEntry() const {
        return db_entry_;
    }

    inline TableDef*
    GetTableDesc() const {
        return table_def_.get();
    }

    inline DataTable*
    GetDataTable() const {
        return data_table_.get();
    }

private:
    const SharedPtr<String>& dir_{};
    RWMutex rw_locker_{};
    SharedPtr<TableDef> table_def_{};
    void* db_entry_{};
    SharedPtr<DataTable> data_table_{};
    void* buffer_mgr_{};
};

}

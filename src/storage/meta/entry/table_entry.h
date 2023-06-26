//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "storage/meta/table_desc.h"

namespace infinity {

class TableEntry : public BaseEntry {
public:
    explicit
    TableEntry(UniquePtr<TableDesc> table_desc, void* db_entry, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context)
            : BaseEntry(EntryType::kTable, txn_context), table_desc_(std::move(table_desc)), db_entry_(db_entry) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    inline void*
    GetDBEntry() const {
        return db_entry_;
    }

    inline TableDesc*
    GetTableDesc() const {
        return table_desc_.get();
    }

private:
    RWMutex rw_locker_{};
    UniquePtr<TableDesc> table_desc_{};
    void* db_entry_{};
};

}

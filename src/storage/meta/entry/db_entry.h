//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "base_entry.h"
#include "common/types/internal_types.h"

namespace infinity {

class DBEntry : public BaseEntry {
public:
    explicit
    DBEntry(String db_name, u64 txn_id, TxnTimeStamp begin_ts)
        : BaseEntry(EntryType::kDatabase), db_name_(std::move(db_name)) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

private:
    RWMutex rw_locker_{};
    String db_name_{};
};

}

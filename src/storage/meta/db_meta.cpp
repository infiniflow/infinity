//
// Created by jinhai on 23-6-4.
//

#include "db_meta.h"

namespace infinity {

DBEntry*
DBMeta::CreateNewEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_name_, txn_id, begin_ts);
    DBEntry* res = db_entry.get();
    entry_list_.emplace_back(std::move(db_entry));
    return res;
}

}

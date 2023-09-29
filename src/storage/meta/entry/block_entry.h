//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "db_entry.h"
#include "common/types/internal_types.h"

namespace infinity {

enum class BlockEntryType {
    kBlock,
    kDeleteFile,
};

class BlockEntry : public BaseEntry {
public:
    explicit
    BlockEntry(BlockEntryType entry_type, u64 txn_id, TxnTimeStamp begin_ts)
            : BaseEntry(EntryType::kBlock), entry_type_(entry_type) {}

private:
    BlockEntryType entry_type_{BlockEntryType::kBlock};
};

}

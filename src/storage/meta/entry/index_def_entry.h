#pragma once

#include "common/types/alias/concurrency.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "json.hpp"
#include "storage/index_def/index_def.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {

class IndexDefMeta;

struct IndexDefEntry : public BaseEntry {
public:
    explicit IndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts);

public:
    static nlohmann::json Serialize(const IndexDefEntry *index_def_entry);

    static UniquePtr<IndexDefEntry> Deserialize(const nlohmann::json &index_def_entry_json, IndexDefMeta *index_def_meta);

public:
    RWMutex rw_locker_{};

    IndexDefMeta *index_def_meta_{};

    SharedPtr<IndexDef> index_def_{};
};
} // namespace infinity

// TODO!!!!!!!!!:: rename
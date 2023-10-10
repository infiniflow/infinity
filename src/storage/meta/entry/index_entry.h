#pragma once

#include "common/types/alias/concurrency.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/object_handle.h"
#include "storage/index_def/index_def.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {

class IndexMeta;

struct IndexEntry : public BaseEntry {
public:
    explicit IndexEntry(SharedPtr<IndexDef> index_def, IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts);

public:
    static IndexObjectHandle GetIndexObjectHandle(IndexEntry *index_entry);

public:
    RWMutex rw_locker_{};

    IndexMeta *index_meta_{};

    SharedPtr<IndexDef> index_def_{};

    SharedPtr<String> file_name_{};
    BufferHandle *index_buffer_handle_{};
};
} // namespace infinity

#include "index_entry.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {
IndexEntry::IndexEntry(SharedPtr<IndexDef> index_def, IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIndex), index_meta_(index_meta), index_def_(index_def) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

IndexObjectHandle IndexEntry::GetIndexObjectHandle(IndexEntry *index_entry) {
    IndexObjectHandle object_handle(index_entry->index_buffer_handle_);
    return object_handle;
}

} // namespace infinity

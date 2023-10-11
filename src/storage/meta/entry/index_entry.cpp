#include "index_entry.h"

#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/segment_entry.h"

namespace infinity {
UniquePtr<IndexEntry> IndexEntry::MakeNewIndexEntry(SharedPtr<String> segment_dir, SharedPtr<String> index_name, BufferManager *buffer_manager) {
    // TODO shenyushi: estimate index size.
    auto buffer_handle = buffer_manager->AllocateBufferHandle(segment_dir, index_name, 0);
    return MakeUnique<IndexEntry>(std::move(segment_dir), std::move(index_name), buffer_handle);
}

[[nodiscard]] ObjectHandle IndexEntry::GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr) {
    if (index_entry->buffer_handle_ == nullptr) {
        auto &&index_dir = IndexEntry::IndexDirName(*index_entry->segment_entry_dir_);
        auto &&index_file = IndexEntry::IndexFileName(*index_entry->index_name_);
        index_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(MakeShared<String>(index_dir), MakeShared<String>(index_dir), BufferType::kFile);
    }
    ObjectHandle index_handle(index_entry->buffer_handle_);
    return index_handle;
}

void IndexEntry::Flush(IndexEntry *index_entry) {
    if (index_entry->buffer_handle_) {
        index_entry->buffer_handle_->WriteFile(0);
        index_entry->buffer_handle_->SyncFile();
        index_entry->buffer_handle_->CloseFile();
    }
}

} // namespace infinity
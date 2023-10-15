#include "index_entry.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/segment_entry.h"

namespace infinity {
SharedPtr<IndexEntry>
IndexEntry::NewIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager, FaissIndexPtr *index) {
    // FIXME shenyushi: estimate index size.
    auto buffer_handle = buffer_manager->AllocateBufferHandle(segment_entry->segment_dir_, index_name, 0, BufferType::kTempFaissIndex);
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = std::shared_ptr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer_handle));
    // auto index_entry = MakeShared<IndexEntry>(segment_entry, std::move(index_name), buffer_handle);

    auto object_handle = IndexEntry::GetIndex(index_entry.get(), buffer_manager);
    object_handle.WriteFaissIndex(index);
    return index_entry;
}

SharedPtr<IndexEntry> IndexEntry::LoadIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager) {
    auto buffer_handle = buffer_manager->GetBufferHandle(segment_entry->segment_dir_, index_name, BufferType::kFaissIndex);
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = std::shared_ptr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer_handle));

    return index_entry;
}

[[nodiscard]] ObjectHandle IndexEntry::GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr) {
    if (index_entry->buffer_handle_ == nullptr) {
        auto &&index_dir = IndexEntry::IndexDirName(*index_entry->segment_entry_->segment_dir_);
        auto &&index_file = IndexEntry::IndexFileName(*index_entry->index_name_);
        index_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(MakeShared<String>(index_dir), MakeShared<String>(index_dir), BufferType::kFile);
    }
    ObjectHandle index_handle(index_entry->buffer_handle_);
    return index_handle;
}

void IndexEntry::Flush(const IndexEntry *index_entry) {
    if (index_entry->buffer_handle_) {
        index_entry->buffer_handle_->WriteFile(0);
        index_entry->buffer_handle_->SyncFile();
        index_entry->buffer_handle_->CloseFile();
    }
}

nlohmann::json IndexEntry::Serialize(const IndexEntry *index_entry) {
    nlohmann::json index_entry_json;
    index_entry_json["index_name"] = *index_entry->index_name_;
    return index_entry_json;
}

SharedPtr<IndexEntry> IndexEntry::Deserialize(const nlohmann::json &index_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    auto index_name = MakeShared<String>(index_entry_json["index_name"].get<String>());

    auto index_entry = LoadIndexEntry(segment_entry, std::move(index_name), buffer_mgr);
    return index_entry;
}

} // namespace infinity
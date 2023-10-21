#include "index_entry.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/segment_entry.h"

namespace infinity {
SharedPtr<IndexEntry> IndexEntry::NewIndexEntry(SegmentEntry *segment_entry,
                                                SharedPtr<String> index_name,
                                                TxnTimeStamp create_ts,
                                                BufferManager *buffer_manager,
                                                FaissIndexPtr *index) {
    // FIXME shenyushi: estimate index size.
    auto buffer_handle = buffer_manager->AllocateBufferHandle(segment_entry->segment_dir_, index_name, 0, BufferType::kTempFaissIndex);
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = std::shared_ptr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer_handle));
    index_entry->min_ts_ = create_ts;
    index_entry->max_ts_ = create_ts;
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

void IndexEntry::UpdateIndex(IndexEntry *index_entry, TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr) {
    NotImplementError("Not tested. TODO shenyushi0");

    index_entry->max_ts_ = commit_ts;
    auto object_handle = IndexEntry::GetIndex(index_entry, buffer_mgr);
    object_handle.WriteFaissIndex(index);
}

bool IndexEntry::Flush(IndexEntry *index_entry, TxnTimeStamp checkpoint_ts) {
    LOG_TRACE("Segment: {}, Index: {} is being flushing", index_entry->segment_entry_->segment_id_, *index_entry->index_name_);
    if (index_entry->max_ts_ <= index_entry->checkpoint_ts_ || index_entry->min_ts_ > checkpoint_ts) {
        LOG_TRACE("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                  index_entry->segment_entry_->segment_id_,
                  *index_entry->index_name_);
        return false;
    }
    if (index_entry->buffer_handle_ == nullptr) {
        LOG_WARN("Index entry is not initialized");
        return false;
    }

    index_entry->buffer_handle_->WriteFile(0);
    index_entry->buffer_handle_->SyncFile();
    index_entry->buffer_handle_->CloseFile();

    index_entry->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE("Segment: {}, Index: {} is flushed", index_entry->segment_entry_->segment_id_, *index_entry->index_name_);
    return true;
}

nlohmann::json IndexEntry::Serialize(const IndexEntry *index_entry) {
    nlohmann::json index_entry_json;
    index_entry_json["index_name"] = *index_entry->index_name_;
    index_entry_json["min_ts"] = index_entry->min_ts_;
    index_entry_json["max_ts"] = index_entry->max_ts_;
    index_entry_json["checkpoint_ts"] = index_entry->checkpoint_ts_;
    return index_entry_json;
}

SharedPtr<IndexEntry> IndexEntry::Deserialize(const nlohmann::json &index_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    auto index_name = MakeShared<String>(index_entry_json["index_name"].get<String>());

    auto index_entry = LoadIndexEntry(segment_entry, std::move(index_name), buffer_mgr);
    index_entry->min_ts_ = index_entry_json["min_ts"];
    index_entry->max_ts_ = index_entry_json["max_ts"];
    index_entry->checkpoint_ts_ = index_entry_json["checkpoint_ts"];
    return index_entry;
}

void IndexEntry::MergeFrom(BaseEntry &other) {
    auto &other_index_entry = dynamic_cast<IndexEntry &>(other);
    if (other_index_entry.min_ts_ < min_ts_) {
        min_ts_ = other_index_entry.min_ts_;
    }
    if (other_index_entry.max_ts_ > max_ts_) {
        max_ts_ = other_index_entry.max_ts_;
    }
    if (other_index_entry.checkpoint_ts_ > checkpoint_ts_) {
        checkpoint_ts_ = other_index_entry.checkpoint_ts_;
    }
}

} // namespace infinity
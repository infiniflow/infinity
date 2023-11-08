// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <string>

import stl;
import segment_entry;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import logger;
import third_party;

import infinity_exception;
import base_entry;
import faiss_index_file_worker;
import ann_ivf_index_file_worker;

module index_entry;

namespace infinity {

IndexEntry::IndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferObj *buffer)
    : BaseEntry(EntryType::kIndex), segment_entry_(segment_entry), index_name_(Move(index_name)), buffer_(buffer){};

SharedPtr<IndexEntry> IndexEntry::NewIndexEntry(SegmentEntry *segment_entry,
                                                SharedPtr<String> index_name,
                                                TxnTimeStamp create_ts,
                                                BufferManager *buffer_manager,
                                                FaissIndexPtr *index) {
    // FIXME shenyushi: estimate index size.
    auto file_worker = MakeUnique<FaissIndexFileWorker>(segment_entry->segment_dir_, index_name, 0);
    auto buffer = buffer_manager->Allocate(std::move(file_worker));
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = SharedPtr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer));
    index_entry->min_ts_ = create_ts;
    index_entry->max_ts_ = create_ts;
    auto buffer_handle = IndexEntry::GetIndex(index_entry.get(), buffer_manager);
    auto dest = static_cast<FaissIndexPtr *>(buffer_handle.GetDataMut());
    *dest = *index;
    return index_entry;
}

SharedPtr<IndexEntry> IndexEntry::NewAnnIVFFlatIndexEntry(SegmentEntry *segment_entry,
                                                          SharedPtr<String> index_name,
                                                          TxnTimeStamp create_ts,
                                                          BufferManager *buffer_manager,
                                                          AnnIVFFlatIndexPtr *index_ptr) {
    // FIXME shenyushi: estimate index size.
    auto file_worker = MakeUnique<FaissIndexFileWorker>(segment_entry->segment_dir_, index_name, 0);
    auto buffer = buffer_manager->Allocate(std::move(file_worker));
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = SharedPtr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer));
    index_entry->min_ts_ = create_ts;
    index_entry->max_ts_ = create_ts;
    auto buffer_handle = IndexEntry::GetIndex(index_entry.get(), buffer_manager);
    auto dest = static_cast<AnnIVFFlatIndexPtr *>(buffer_handle.GetDataMut());
    *dest = *index_ptr;
    return index_entry;
}

SharedPtr<IndexEntry> IndexEntry::LoadIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager) {
    auto file_worker = MakeUnique<FaissIndexFileWorker>(segment_entry->segment_dir_, index_name, 0);
    auto buffer = buffer_manager->Get(std::move(file_worker));
    // FIXME shenyushi: Should use make_shared instead. One heap allocate
    auto index_entry = SharedPtr<IndexEntry>(new IndexEntry(segment_entry, std::move(index_name), buffer));

    return index_entry;
}

BufferHandle IndexEntry::GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr) { return index_entry->buffer_->Load(); }

void IndexEntry::UpdateIndex(IndexEntry *index_entry, TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr) {
    Error<NotImplementException>("Not tested. TODO shenyushi0");

    index_entry->max_ts_ = commit_ts;
    auto buffer_handle = IndexEntry::GetIndex(index_entry, buffer_mgr);
    auto dest = static_cast<FaissIndexPtr *>(buffer_handle.GetDataMut());
    *dest = *index;
}

bool IndexEntry::Flush(IndexEntry *index_entry, TxnTimeStamp checkpoint_ts) {
    LOG_TRACE(Format("Segment: {}, Index: {} is being flushing", index_entry->segment_entry_->segment_id_, *index_entry->index_name_));
    if (index_entry->max_ts_ <= index_entry->checkpoint_ts_ || index_entry->min_ts_ > checkpoint_ts) {
        LOG_TRACE(Format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                         index_entry->segment_entry_->segment_id_,
                         *index_entry->index_name_));
        return false;
    }
    if (index_entry->buffer_ == nullptr) {
        LOG_WARN("Index entry is not initialized");
        return false;
    }
    if (index_entry->buffer_->Save()) {
        index_entry->buffer_->Sync();
        index_entry->buffer_->CloseFile();
    }

    index_entry->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(Format("Segment: {}, Index: {} is flushed", index_entry->segment_entry_->segment_id_, *index_entry->index_name_));
    return true;
}

Json IndexEntry::Serialize(const IndexEntry *index_entry) {
    Json index_entry_json;
    index_entry_json["index_name"] = *index_entry->index_name_;
    index_entry_json["min_ts"] = index_entry->min_ts_;
    index_entry_json["max_ts"] = index_entry->max_ts_;
    index_entry_json["checkpoint_ts"] = index_entry->checkpoint_ts_;
    return index_entry_json;
}

SharedPtr<IndexEntry> IndexEntry::Deserialize(const Json &index_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {
    auto index_name = MakeShared<String>(index_entry_json["index_name"].get<String>());

    auto index_entry = LoadIndexEntry(segment_entry, Move(index_name), buffer_mgr);
    Assert<StorageException>(index_entry.get() != nullptr, "Failed to load index entry");
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

String IndexEntry::IndexFileName(const String &index_name) { return index_name + ".idx"; }

String IndexEntry::IndexDirName(const String &segment_entry_dir) { return segment_entry_dir + "/index"; }

} // namespace infinity
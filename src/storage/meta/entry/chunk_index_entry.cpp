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

#include "type/complex/row_id.h"
#include <cassert>

module chunk_index_entry;

import stl;
import third_party;
import base_entry;
import meta_entry_interface;
import cleanup_scanner;
import segment_index_entry;
import index_file_worker;
import logger;
import create_index_info;
import third_party;
import hnsw_file_worker;
import raw_file_worker;
import index_base;
import buffer_manager;
import buffer_obj;
import buffer_handle;
import infinity_exception;
import index_defines;

namespace infinity {

String ChunkIndexEntry::EncodeIndex(const ChunkID chunk_id, const SegmentIndexEntry *segment_index_entry) {
    return fmt::format("{}#{}", segment_index_entry->encode(), chunk_id);
}

ChunkIndexEntry::ChunkIndexEntry(ChunkID chunk_id, SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count)
    : BaseEntry(EntryType::kChunkIndex, false, ChunkIndexEntry::EncodeIndex(chunk_id, segment_index_entry)), chunk_id_(chunk_id),
      segment_index_entry_(segment_index_entry), base_name_(base_name), base_rowid_(base_rowid), row_count_(row_count){};

UniquePtr<IndexFileWorker> ChunkIndexEntry::CreateFileWorker(const IndexBase *index_base,
                                                             const SharedPtr<String> &index_dir,
                                                             CreateIndexParam *param,
                                                             SegmentID segment_id,
                                                             ChunkID chunk_id) {
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            auto *create_hnsw_param = static_cast<CreateHnswParam *>(param);
            auto index_file_name = MakeShared<String>(ChunkIndexEntry::IndexFileName(segment_id, chunk_id));
            return MakeUnique<HnswFileWorker>(index_dir, index_file_name, create_hnsw_param);
        }
        default: {
            LOG_TRACE(fmt::format("index {} not store in index chunk entry by buffer obj", (u8)index_base->index_type_));
            return nullptr;
        }
    }
}

String ChunkIndexEntry::IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewChunkIndexEntry(ChunkID chunk_id,
                                                               SegmentIndexEntry *segment_index_entry,
                                                               CreateIndexParam *param,
                                                               RowID base_rowid,
                                                               BufferManager *buffer_mgr) {
    auto chunk_index_entry = SharedPtr<ChunkIndexEntry>(new ChunkIndexEntry(chunk_id, segment_index_entry, "", base_rowid, -1));

    const auto &index_dir = segment_index_entry->index_dir();
    const auto &index_base = param->index_base_;
    SegmentID segment_id = segment_index_entry->segment_id();

    auto file_worker = ChunkIndexEntry::CreateFileWorker(index_base.get(), index_dir, param, segment_id, chunk_id);
    chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewFtChunkIndexEntry(SegmentIndexEntry *segment_index_entry,
                                                                 const String &base_name,
                                                                 RowID base_rowid,
                                                                 u32 row_count,
                                                                 BufferManager *buffer_mgr) {
    auto chunk_index_entry = SharedPtr<ChunkIndexEntry>(new ChunkIndexEntry(0, segment_index_entry, base_name, base_rowid, row_count));
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
        auto file_worker = MakeUnique<RawFileWorker>(index_dir, column_length_file_name);
        chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewReplayChunkIndexEntry(ChunkID chunk_id,
                                                                     SegmentIndexEntry *segment_index_entry,
                                                                     CreateIndexParam *param,
                                                                     const String &base_name,
                                                                     RowID base_rowid,
                                                                     u32 row_count,
                                                                     BufferManager *buffer_mgr) {
    auto chunk_index_entry = SharedPtr<ChunkIndexEntry>(new ChunkIndexEntry(chunk_id, segment_index_entry, base_name, base_rowid, row_count));
    if (param->index_base_->index_type_ == IndexType::kFullText) {
        const auto &index_dir = segment_index_entry->index_dir();
        auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
        auto file_worker = MakeUnique<RawFileWorker>(index_dir, column_length_file_name);
        chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
    } else {
        const auto &index_dir = segment_index_entry->index_dir();
        const auto &index_base = param->index_base_;
        SegmentID segment_id = segment_index_entry->segment_id();

        auto file_worker = ChunkIndexEntry::CreateFileWorker(index_base.get(), index_dir, param, segment_id, chunk_id);
        chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
    }
    return chunk_index_entry;
}

BufferHandle ChunkIndexEntry::GetIndex() { return buffer_obj_->Load(); }

nlohmann::json ChunkIndexEntry::Serialize() {
    nlohmann::json index_entry_json;
    index_entry_json["chunk_id"] = this->chunk_id_;
    index_entry_json["base_name"] = this->base_name_;
    index_entry_json["base_rowid"] = this->base_rowid_.ToUint64();
    index_entry_json["row_count"] = this->row_count_;
    index_entry_json["commit_ts"] = this->commit_ts_.load();
    return index_entry_json;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::Deserialize(const nlohmann::json &index_entry_json,
                                                        SegmentIndexEntry *segment_index_entry,
                                                        CreateIndexParam *param,
                                                        BufferManager *buffer_mgr) {
    ChunkID chunk_id = index_entry_json["chunk_id"];
    String base_name = index_entry_json["base_name"];
    RowID base_rowid = RowID::FromUint64(index_entry_json["base_rowid"]);
    u32 row_count = index_entry_json["row_count"];
    auto ret = NewReplayChunkIndexEntry(chunk_id, segment_index_entry, param, base_name, base_rowid, row_count, buffer_mgr);
    ret->commit_ts_.store(index_entry_json["commit_ts"]);
    return ret;
}

void ChunkIndexEntry::Cleanup() {
    if (buffer_obj_) {
        buffer_obj_->PickForCleanup();
    }
}

void ChunkIndexEntry::SaveIndexFile() {
    if (buffer_obj_ == nullptr) {
        return;
    }
    buffer_obj_->Save();
}

} // namespace infinity

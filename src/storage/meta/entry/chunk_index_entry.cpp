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
import table_index_entry;
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
import local_file_system;
import secondary_index_file_worker;
import emvb_index_file_worker;
import bmp_index_file_worker;
import column_def;
import infinity_context;
import persistence_manager;

namespace infinity {

Vector<std::string_view> ChunkIndexEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid chunk index entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = SegmentIndexEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String ChunkIndexEntry::EncodeIndex(const ChunkID chunk_id, const SegmentIndexEntry *segment_index_entry) {
    return fmt::format("{}#{}", segment_index_entry->encode(), chunk_id);
}

ChunkIndexEntry::ChunkIndexEntry(ChunkID chunk_id, SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count)
    : BaseEntry(EntryType::kChunkIndex, false, segment_index_entry->base_dir_, ChunkIndexEntry::EncodeIndex(chunk_id, segment_index_entry)),
      chunk_id_(chunk_id), segment_index_entry_(segment_index_entry), base_name_(base_name), base_rowid_(base_rowid), row_count_(row_count){};

String ChunkIndexEntry::IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewHnswIndexChunkIndexEntry(ChunkID chunk_id,
                                                                        SegmentIndexEntry *segment_index_entry,
                                                                        const String &base_name,
                                                                        RowID base_rowid,
                                                                        u32 row_count,
                                                                        BufferManager *buffer_mgr,
                                                                        SizeT index_size) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        SegmentID segment_id = segment_index_entry->segment_id();
        auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
        auto file_worker = MakeUnique<HnswFileWorker>(full_dir, hnsw_index_file_name, index_base, column_def, index_size);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    }
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
        SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
        auto file_worker = MakeUnique<RawFileWorker>(full_dir, column_length_file_name, row_count * sizeof(u32));
        chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewSecondaryIndexChunkIndexEntry(ChunkID chunk_id,
                                                                             SegmentIndexEntry *segment_index_entry,
                                                                             const String &base_name,
                                                                             RowID base_rowid,
                                                                             u32 row_count,
                                                                             BufferManager *buffer_mgr) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        SegmentID segment_id = segment_index_entry->segment_id();
        auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
        auto file_worker = MakeUnique<SecondaryIndexFileWorker>(full_dir, secondary_index_file_name, index_base, column_def, row_count);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        const u32 part_cnt = (row_count + 8191) / 8192;
        for (u32 i = 0; i < part_cnt; ++i) {
            auto part_name = MakeShared<String>(fmt::format("{}_part{}", *secondary_index_file_name, i));
            auto part_file_worker = MakeUnique<SecondaryIndexFileWorkerParts>(full_dir, part_name, index_base, column_def, row_count, i);
            BufferObj *part_ptr = buffer_mgr->AllocateBufferObject(std::move(part_file_worker));
            chunk_index_entry->part_buffer_objs_.push_back(part_ptr);
        }
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewEMVBIndexChunkIndexEntry(ChunkID chunk_id,
                                                                        SegmentIndexEntry *segment_index_entry,
                                                                        const String &base_name,
                                                                        RowID base_rowid,
                                                                        u32 row_count,
                                                                        BufferManager *buffer_mgr) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        SegmentID segment_id = segment_index_entry->segment_id();
        auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        const auto segment_start_offset = base_rowid.segment_offset_;
        SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
        auto file_worker = MakeUnique<EMVBIndexFileWorker>(full_dir, emvb_index_file_name, index_base, column_def, segment_start_offset);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewBMPIndexChunkIndexEntry(ChunkID chunk_id,
                                                                       SegmentIndexEntry *segment_index_entry,
                                                                       const String &base_name,
                                                                       RowID base_rowid,
                                                                       u32 row_count,
                                                                       BufferManager *buffer_mgr,
                                                                       SizeT index_size) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        SegmentID segment_id = segment_index_entry->segment_id();
        auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
        auto file_worker = MakeUnique<BMPIndexFileWorker>(full_dir, bmp_index_file_name, index_base, column_def, index_size);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewReplayChunkIndexEntry(ChunkID chunk_id,
                                                                     SegmentIndexEntry *segment_index_entry,
                                                                     CreateIndexParam *param,
                                                                     const String &base_name,
                                                                     RowID base_rowid,
                                                                     u32 row_count,
                                                                     TxnTimeStamp commit_ts,
                                                                     TxnTimeStamp deprecate_ts,
                                                                     BufferManager *buffer_mgr) {
    auto chunk_index_entry = SharedPtr<ChunkIndexEntry>(new ChunkIndexEntry(chunk_id, segment_index_entry, base_name, base_rowid, row_count));
    const auto &column_def = segment_index_entry->table_index_entry()->column_def();
    const auto &index_dir = segment_index_entry->index_dir();
    SharedPtr<String> full_dir = MakeShared<String>(fmt::format("{}/{}", *chunk_index_entry->base_dir_, *index_dir));
    switch (param->index_base_->index_type_) {
        case IndexType::kHnsw: {
            SegmentID segment_id = segment_index_entry->segment_id();

            auto index_file_name = MakeShared<String>(ChunkIndexEntry::IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<HnswFileWorker>(full_dir, index_file_name, param->index_base_, column_def);
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kFullText: {
            auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
            String obj_path = pm->GetObjCache(column_length_file_name);
            auto file_worker = MakeUnique<RawFileWorker>(full_dir, column_length_file_name, row_count * sizeof(u32));
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kSecondary: {
            SegmentID segment_id = segment_index_entry->segment_id();
            auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
            auto file_worker = MakeUnique<SecondaryIndexFileWorker>(full_dir, secondary_index_file_name, index_base, column_def, row_count);
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            chunk_index_entry->LoadPartsReader(buffer_mgr);
            break;
        }
        case IndexType::kEMVB: {
            SegmentID segment_id = segment_index_entry->segment_id();
            auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
            const auto segment_start_offset = base_rowid.segment_offset_;
            auto file_worker = MakeUnique<EMVBIndexFileWorker>(full_dir, emvb_index_file_name, index_base, column_def, segment_start_offset);
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kBMP: {
            const auto &index_base = param->index_base_;
            SegmentID segment_id = segment_index_entry->segment_id();

            auto index_file_name = MakeShared<String>(ChunkIndexEntry::IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<BMPIndexFileWorker>(full_dir, index_file_name, index_base, column_def);
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unsupported index type: {}", (u8)param->index_base_->index_type_));
        }
    }
    chunk_index_entry->commit_ts_ = commit_ts;
    chunk_index_entry->deprecate_ts_ = deprecate_ts;
    return chunk_index_entry;
}

u64 ChunkIndexEntry::GetColumnLengthSum() const {
    assert(segment_index_entry_->table_index_entry()->index_base()->index_type_ == IndexType::kFullText);
    // Read the length from buffer object and sum up
    u64 column_length_sum = 0UL;
    BufferHandle buffer_handle = buffer_obj_->Load();
    const u32 *column_lengths = (const u32 *)buffer_handle.GetData();
    for (SizeT i = 0; i < row_count_; i++) {
        column_length_sum += column_lengths[i];
    }
    return column_length_sum;
}

BufferHandle ChunkIndexEntry::GetIndex() { return buffer_obj_->Load(); }

IndexType ChunkIndexEntry::GetIndexType() { return segment_index_entry_->table_index_entry()->index_base()->index_type_; }

nlohmann::json ChunkIndexEntry::Serialize() {
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;

    nlohmann::json index_entry_json;
    index_entry_json["chunk_id"] = this->chunk_id_;
    index_entry_json["base_name"] = this->base_name_;
    index_entry_json["base_rowid"] = this->base_rowid_.ToUint64();
    index_entry_json["row_count"] = this->row_count_;
    index_entry_json["commit_ts"] = this->commit_ts_.load();
    index_entry_json["deprecate_ts"] = this->deprecate_ts_.load();

    if (use_object_cache) {
        IndexType index_type = GetIndexType();
        switch (index_type) {
            case IndexType::kFullText: {
                String full_path = fmt::format("{}/{}", *this->base_dir_, *(segment_index_entry_->index_dir()));
                String posting_file_name = base_name_ + POSTING_SUFFIX;
                String dict_file_name = base_name_ + DICT_SUFFIX;
                String column_length_file_name = base_name_ + LENGTH_SUFFIX;
                index_entry_json["obj_addrs"].emplace(pm->GetObjLocalCache(full_path + posting_file_name).Serialize());
                index_entry_json["obj_addrs"].emplace(pm->GetObjLocalCache(full_path + dict_file_name).Serialize());
                index_entry_json["obj_addrs"].emplace(pm->GetObjLocalCache(full_path + column_length_file_name).Serialize());
            }
            default: {
                break;
            }
        }
    }
    return index_entry_json;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::Deserialize(const nlohmann::json &index_entry_json,
                                                        SegmentIndexEntry *segment_index_entry,
                                                        CreateIndexParam *param,
                                                        BufferManager *buffer_mgr) {
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;

    ChunkID chunk_id = index_entry_json["chunk_id"];
    String base_name = index_entry_json["base_name"];
    RowID base_rowid = RowID::FromUint64(index_entry_json["base_rowid"]);
    u32 row_count = index_entry_json["row_count"];
    TxnTimeStamp commit_ts = index_entry_json["commit_ts"];
    TxnTimeStamp deprecate_ts = index_entry_json["deprecate_ts"];
    auto ret = NewReplayChunkIndexEntry(chunk_id, segment_index_entry, param, base_name, base_rowid, row_count, commit_ts, deprecate_ts, buffer_mgr);

    // Deserialize obj_addrs
    if (use_object_cache) {
        ObjAddr obj_addr;
        switch (param->index_base_->index_type_) {
            case IndexType::kFullText: {
                String full_path = fmt::format("{}/{}", *ret->base_dir_, *segment_index_entry->index_dir());
                String posting_file_name = ret->base_name_ + POSTING_SUFFIX;
                String dict_file_name = ret->base_name_ + DICT_SUFFIX;
                String column_length_file_name = ret->base_name_ + LENGTH_SUFFIX;
                obj_addr.Deserialize(index_entry_json["obj_addrs"][0]);
                pm->PutObjLocalCache(full_path + posting_file_name, obj_addr);
                obj_addr.Deserialize(index_entry_json["obj_addrs"][1]);
                pm->PutObjLocalCache(full_path + dict_file_name, obj_addr);
                obj_addr.Deserialize(index_entry_json["obj_addrs"][2]);
                pm->PutObjLocalCache(full_path + column_length_file_name, obj_addr);
            }
            default: {
                break;
            }
        }
    }

    return ret;
}

void ChunkIndexEntry::Cleanup() {
    if (buffer_obj_) {
        buffer_obj_->PickForCleanup();
    }
    for (BufferObj *part_buffer_obj : part_buffer_objs_) {
        part_buffer_obj->PickForCleanup();
    }
    TableIndexEntry *table_index_entry = segment_index_entry_->table_index_entry();
    const auto &index_dir = segment_index_entry_->index_dir();
    const IndexBase *index_base = table_index_entry->index_base();
    if (index_base->index_type_ == IndexType::kFullText) {
        if (InfinityContext::instance().persistence_manager() != nullptr) {
            // TODO cleanup fulltext file obj,
            Path path = Path(*index_dir) / base_name_;
            String index_prefix = path.string();
            String posting_file = index_prefix + POSTING_SUFFIX;
            String dict_file = index_prefix + DICT_SUFFIX;

            LocalFileSystem fs;
            fs.DeleteFile(posting_file);
            fs.DeleteFile(dict_file);
            LOG_DEBUG(fmt::format("Cleaned chunk index entry {}, posting: {}, dictionary file: {}", index_prefix, posting_file, dict_file));
        } else {
            Path path = Path(*index_dir) / base_name_;
            String index_prefix = path.string();
            String posting_file = index_prefix + POSTING_SUFFIX;
            String dict_file = index_prefix + DICT_SUFFIX;

            LocalFileSystem fs;
            fs.DeleteFile(posting_file);
            fs.DeleteFile(dict_file);
            LOG_DEBUG(fmt::format("Cleaned chunk index entry {}, posting: {}, dictionary file: {}", index_prefix, posting_file, dict_file));
        }
    } else {
        LOG_DEBUG(fmt::format("Cleaned chunk index entry {}/{}", *index_dir, chunk_id_));
    }
}

void ChunkIndexEntry::SaveIndexFile() {
    if (buffer_obj_ == nullptr) {
        return;
    }
    buffer_obj_->Save();
    for (BufferObj *part_buffer_obj : part_buffer_objs_) {
        part_buffer_obj->Save();
    }
}

void ChunkIndexEntry::LoadPartsReader(BufferManager *buffer_mgr) {
    const auto &index_dir = segment_index_entry_->index_dir();
    SegmentID segment_id = segment_index_entry_->segment_id();
    String secondary_index_file_name = IndexFileName(segment_id, chunk_id_);
    const auto &index_base = segment_index_entry_->table_index_entry()->table_index_def();
    const auto &column_def = segment_index_entry_->table_index_entry()->column_def();
    const u32 part_cnt = (row_count_ + 8191) / 8192;
    part_buffer_objs_.clear();
    part_buffer_objs_.reserve(part_cnt);
    for (u32 i = 0; i < part_cnt; ++i) {
        auto part_name = MakeShared<String>(fmt::format("{}_part{}", secondary_index_file_name, i));
        auto part_file_worker = MakeUnique<SecondaryIndexFileWorkerParts>(index_dir, std::move(part_name), index_base, column_def, row_count_, i);
        BufferObj *part_ptr = buffer_mgr->GetBufferObject(std::move(part_file_worker));
        part_buffer_objs_.push_back(part_ptr);
    }
}

BufferHandle ChunkIndexEntry::GetIndexPartAt(u32 i) { return part_buffer_objs_.at(i)->Load(); }

void ChunkIndexEntry::Save() {
    if (buffer_obj_) {
        buffer_obj_->Save();
    }
}

} // namespace infinity

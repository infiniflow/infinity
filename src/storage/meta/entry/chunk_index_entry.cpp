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

#include <cassert>
#include <vector>

module chunk_index_entry;

import stl;
import third_party;
import base_entry;
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
import virtual_store;
import secondary_index_file_worker;
import ivf_index_file_worker;
import emvb_index_file_worker;
import bmp_index_file_worker;
import column_def;
import internal_types;
import infinity_context;
import persistence_manager;
import persist_result_handler;
import snapshot_info;

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

String ChunkIndexEntry::EncodeIndex(const ChunkID chunk_id, const String &base_name, const SegmentIndexEntry *segment_index_entry) {
    return fmt::format("{}#{}", segment_index_entry->encode(), chunk_id);
}

ChunkIndexEntry::ChunkIndexEntry(ChunkID chunk_id, SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count)
    : BaseEntry(EntryType::kChunkIndex, false, ChunkIndexEntry::EncodeIndex(chunk_id, base_name, segment_index_entry)), chunk_id_(chunk_id),
      segment_index_entry_(segment_index_entry), base_name_(base_name), base_rowid_(base_rowid), row_count_(row_count) {};

ChunkIndexEntry::~ChunkIndexEntry() {}

ChunkIndexEntry::ChunkIndexEntry(const ChunkIndexEntry &other)
    : BaseEntry(other), chunk_id_(other.chunk_id_), segment_index_entry_(other.segment_index_entry_), base_name_(other.base_name_),
      base_rowid_(other.base_rowid_), row_count_(other.row_count_), deprecate_ts_(other.deprecate_ts_.load()), buffer_obj_(other.buffer_obj_) {
    if (buffer_obj_) {
        buffer_obj_->AddObjRc();
    }
}

UniquePtr<ChunkIndexEntry> ChunkIndexEntry::Clone(SegmentIndexEntry *segment_index_entry) const {
    auto ret = UniquePtr<ChunkIndexEntry>(new ChunkIndexEntry(*this));
    ret->segment_index_entry_ = segment_index_entry;
    return ret;
}

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
        const SegmentID segment_id = segment_index_entry->segment_id();
        auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        auto file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      index_dir,
                                                      std::move(hnsw_index_file_name),
                                                      index_base,
                                                      column_def,
                                                      buffer_mgr->persistence_manager(),
                                                      index_size);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewFtChunkIndexEntry(SegmentIndexEntry *segment_index_entry,
                                                                 ChunkID chunk_id,
                                                                 const String &base_name,
                                                                 RowID base_rowid,
                                                                 u32 row_count,
                                                                 BufferManager *buffer_mgr) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
        auto file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                     MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                     index_dir,
                                                     std::move(column_length_file_name),
                                                     row_count * sizeof(u32),
                                                     buffer_mgr->persistence_manager());
        chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
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
        auto file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                index_dir,
                                                                secondary_index_file_name,
                                                                index_base,
                                                                column_def,
                                                                row_count,
                                                                buffer_mgr->persistence_manager());
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewIVFIndexChunkIndexEntry(ChunkID chunk_id,
                                                                       SegmentIndexEntry *segment_index_entry,
                                                                       const String &base_name,
                                                                       RowID base_rowid,
                                                                       u32 row_count,
                                                                       BufferManager *buffer_mgr) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_dir = segment_index_entry->index_dir();
    assert(index_dir.get() != nullptr);
    if (buffer_mgr != nullptr) {
        const SegmentID segment_id = segment_index_entry->segment_id();
        auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        auto file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                          MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                          index_dir,
                                                          ivf_index_file_name,
                                                          index_base,
                                                          column_def,
                                                          buffer_mgr->persistence_manager());
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
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
        auto file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                           MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                           index_dir,
                                                           std::move(emvb_index_file_name),
                                                           index_base,
                                                           column_def,
                                                           segment_start_offset,
                                                           buffer_mgr->persistence_manager());
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
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
        const SegmentID segment_id = segment_index_entry->segment_id();
        auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
        const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
        const auto &column_def = segment_index_entry->table_index_entry()->column_def();
        auto file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                          MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                          index_dir,
                                                          std::move(bmp_index_file_name),
                                                          index_base,
                                                          column_def,
                                                          buffer_mgr->persistence_manager(),
                                                          index_size);
        chunk_index_entry->buffer_obj_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        chunk_index_entry->buffer_obj_->AddObjRc();
    }
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::NewReplayChunkIndexEntry(ChunkID chunk_id,
                                                                     SegmentIndexEntry *segment_index_entry,
                                                                     const String &base_name,
                                                                     RowID base_rowid,
                                                                     u32 row_count,
                                                                     TxnTimeStamp commit_ts,
                                                                     TxnTimeStamp deprecate_ts,
                                                                     BufferManager *buffer_mgr) {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(chunk_id, segment_index_entry, base_name, base_rowid, row_count);
    const auto &index_base = segment_index_entry->table_index_entry()->table_index_def();
    const auto &column_def = segment_index_entry->table_index_entry()->column_def();
    const auto &index_dir = segment_index_entry->index_dir();
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            const SegmentID segment_id = segment_index_entry->segment_id();
            auto index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                          MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                          index_dir,
                                                          std::move(index_file_name),
                                                          index_base,
                                                          column_def,
                                                          buffer_mgr->persistence_manager());
            BufferObj *buffer_obj = buffer_mgr->GetBufferObject(std::move(file_worker));
            buffer_obj->ToMmap();
            chunk_index_entry->buffer_obj_ = buffer_obj;
            break;
        }
        case IndexType::kFullText: {
            auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
            auto file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                         MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                         index_dir,
                                                         std::move(column_length_file_name),
                                                         row_count * sizeof(u32),
                                                         buffer_mgr->persistence_manager());
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kSecondary: {
            const SegmentID segment_id = segment_index_entry->segment_id();
            auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                    MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                    index_dir,
                                                                    std::move(secondary_index_file_name),
                                                                    index_base,
                                                                    column_def,
                                                                    row_count,
                                                                    buffer_mgr->persistence_manager());
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kIVF: {
            const SegmentID segment_id = segment_index_entry->segment_id();
            auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                              index_dir,
                                                              std::move(ivf_index_file_name),
                                                              index_base,
                                                              column_def,
                                                              buffer_mgr->persistence_manager());
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kEMVB: {
            const SegmentID segment_id = segment_index_entry->segment_id();
            auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            const auto segment_start_offset = base_rowid.segment_offset_;
            auto file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                               MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                               index_dir,
                                                               std::move(emvb_index_file_name),
                                                               index_base,
                                                               column_def,
                                                               segment_start_offset,
                                                               buffer_mgr->persistence_manager());
            chunk_index_entry->buffer_obj_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kBMP: {
            const SegmentID segment_id = segment_index_entry->segment_id();
            auto index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
            auto file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                              index_dir,
                                                              std::move(index_file_name),
                                                              index_base,
                                                              column_def,
                                                              buffer_mgr->persistence_manager());
            BufferObj *buffer_obj = buffer_mgr->GetBufferObject(std::move(file_worker));
            buffer_obj->ToMmap();
            chunk_index_entry->buffer_obj_ = buffer_obj;
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unsupported index type: {}", index_base->ToString()));
        }
    }
    if (chunk_index_entry->buffer_obj_) {
        chunk_index_entry->buffer_obj_->AddObjRc();
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

nlohmann::json ChunkIndexEntry::Serialize() {
    nlohmann::json index_entry_json;
    index_entry_json["chunk_id"] = this->chunk_id_;
    index_entry_json["base_name"] = this->base_name_;
    index_entry_json["base_rowid"] = this->base_rowid_.ToUint64();
    index_entry_json["row_count"] = this->row_count_;
    index_entry_json["commit_ts"] = this->commit_ts_.load();
    index_entry_json["deprecate_ts"] = this->deprecate_ts_.load();
    return index_entry_json;
}

SharedPtr<ChunkIndexEntry>
ChunkIndexEntry::Deserialize(const nlohmann::json &index_entry_json, SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    ChunkID chunk_id = index_entry_json["chunk_id"];
    String base_name = index_entry_json["base_name"];
    RowID base_rowid = RowID::FromUint64(index_entry_json["base_rowid"]);
    u32 row_count = index_entry_json["row_count"];
    TxnTimeStamp commit_ts = index_entry_json["commit_ts"];
    TxnTimeStamp deprecate_ts = index_entry_json["deprecate_ts"];
    auto ret = NewReplayChunkIndexEntry(chunk_id, segment_index_entry, base_name, base_rowid, row_count, commit_ts, deprecate_ts, buffer_mgr);
    return ret;
}

void ChunkIndexEntry::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    if (buffer_obj_ != nullptr) {
        buffer_obj_->PickForCleanup();
        if (info_tracer) {
            info_tracer->AddCleanupInfo(buffer_obj_->GetFilename());
        }
    }
    if (!dropped) {
        return;
    }

    TableIndexEntry *table_index_entry = segment_index_entry_->table_index_entry();
    const auto &index_dir = segment_index_entry_->index_dir();
    const IndexBase *index_base = table_index_entry->index_base();
    if (index_base->index_type_ == IndexType::kFullText) {
        PersistenceManager *pm = InfinityContext::instance().persistence_manager();
        Path path = Path(*index_dir) / base_name_;
        String index_prefix = path.string();
        String posting_file = index_prefix + POSTING_SUFFIX;
        String dict_file = index_prefix + DICT_SUFFIX;
        if (pm != nullptr) {
            LOG_INFO(fmt::format("Cleaned chunk index entry {}, posting: {}, dictionary file: {}", index_prefix, posting_file, dict_file));

            PersistResultHandler handler(pm);
            PersistWriteResult result1 = pm->Cleanup(posting_file);
            PersistWriteResult result2 = pm->Cleanup(dict_file);
            handler.HandleWriteResult(result1);
            handler.HandleWriteResult(result2);

        } else {
            String absolute_posting_file = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), posting_file);
            String absolute_dict_file = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), dict_file);

            LOG_INFO(fmt::format("Clean chunk index entry {}, posting: {}, dictionary file: {}",
                                  index_prefix,
                                  absolute_posting_file,
                                  absolute_dict_file));

            VirtualStore::DeleteFile(absolute_posting_file);
            VirtualStore::DeleteFile(absolute_dict_file);
        }
        if (info_tracer) {
            info_tracer->AddCleanupInfo(std::move(posting_file));
            info_tracer->AddCleanupInfo(std::move(dict_file));
        }
    } else {
        LOG_DEBUG(fmt::format("Cleaned chunk index entry {}/{}", *index_dir, chunk_id_));
    }
}

Vector<String> ChunkIndexEntry::GetFilePath(Txn* txn) const {
    Vector<String> res;
    res.emplace_back(buffer_obj_->GetFilename());
    return res;
}

void ChunkIndexEntry::SaveIndexFile() {
    if (buffer_obj_ == nullptr) {
        return;
    }
    if (buffer_obj_->type() == BufferType::kMmap) {
        return;
    }
    buffer_obj_->Save();
    switch (segment_index_entry_->table_index_entry()->index_base()->index_type_) {
        case IndexType::kHnsw:
        case IndexType::kBMP: {
            buffer_obj_->ToMmap();
            break;
        }
        default: {
            break;
        }
    }
}

void ChunkIndexEntry::DeprecateChunk(TxnTimeStamp commit_ts) {
    assert(commit_ts_.load() < commit_ts);
    deprecate_ts_.store(commit_ts);
    LOG_INFO(fmt::format("Deprecate chunk {}, ts: {}", encode(), commit_ts));
}

bool ChunkIndexEntry::CheckVisible(Txn *txn) const {
    if (txn == nullptr) {
        return deprecate_ts_.load() == UNCOMMIT_TS;
    }
    TxnTimeStamp begin_ts = txn->BeginTS();
    return begin_ts < deprecate_ts_.load() && BaseEntry::CheckVisible(txn);
}

SharedPtr<ChunkIndexSnapshotInfo> ChunkIndexEntry::GetSnapshotInfo(Txn *txn_ptr) const {
    SharedPtr<ChunkIndexSnapshotInfo> chunk_index_snapshot_info = MakeShared<ChunkIndexSnapshotInfo>();
    chunk_index_snapshot_info->chunk_id_ = chunk_id_;
    chunk_index_snapshot_info->base_name_ = base_name_;
    chunk_index_snapshot_info->base_rowid_ = base_rowid_;
    chunk_index_snapshot_info->row_count_ = row_count_;

    TableIndexEntry *table_index_entry = segment_index_entry_->table_index_entry();
    const auto &index_dir = segment_index_entry_->index_dir();
    const IndexBase *index_base = table_index_entry->index_base();
    if (index_base->index_type_ == IndexType::kFullText) {
        Path path = Path(*index_dir) / base_name_;
        String index_prefix = path.string();
        String posting_file = index_prefix + POSTING_SUFFIX;
        String dict_file = index_prefix + DICT_SUFFIX;
        String len_file = index_prefix + LENGTH_SUFFIX;
        chunk_index_snapshot_info->files_.push_back(posting_file);
        chunk_index_snapshot_info->files_.push_back(dict_file);
        chunk_index_snapshot_info->files_.push_back(len_file);
    }
    chunk_index_snapshot_info->index_filename_ = IndexFileName(segment_index_entry_->segment_id(),chunk_id_);
    return chunk_index_snapshot_info;
}

SharedPtr<ChunkIndexEntry> ChunkIndexEntry::ApplySnapshotInfo(SegmentIndexEntry * segment_index_entry,ChunkIndexSnapshotInfo *chunk_index_snapshot_info,TransactionID txn_id,
                                                                TxnTimeStamp begin_ts)  {
    auto chunk_index_entry = MakeShared<ChunkIndexEntry>(
        chunk_index_snapshot_info->chunk_id_,
        segment_index_entry,
        chunk_index_snapshot_info->base_name_,
        chunk_index_snapshot_info->base_rowid_,
        chunk_index_snapshot_info->row_count_
    );

    chunk_index_entry->index_filename_ = chunk_index_snapshot_info->index_filename_;
    return chunk_index_entry;
}

} // namespace infinity

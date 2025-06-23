// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <vector>
#include <string>

module chunk_index_meta;

import kv_code;
import kv_store;
import table_meeta;
import table_index_meeta;
import segment_index_meta;

import index_base;
import column_def;
import index_defines;
import create_index_info;
import infinity_context;
import buffer_manager;
import secondary_index_file_worker;
import ivf_index_file_worker;
import raw_file_worker;
import hnsw_file_worker;
import bmp_index_file_worker;
import emvb_index_file_worker;
import infinity_exception;

import persistence_manager;
import persist_result_handler;
import virtual_store;
import logger;
import file_worker;

namespace infinity {

namespace {

String IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

} // namespace

void ChunkIndexMetaInfo::ToJson(nlohmann::json &json) const {
    json["base_name"] = base_name_;
    json["base_row_id"] = base_row_id_.ToUint64();
    json["row_count"] = row_cnt_;
    json["index_size"] = index_size_;
}

void ChunkIndexMetaInfo::FromJson(const nlohmann::json &json) {
    base_name_ = json["base_name"].get<String>();
    base_row_id_ = RowID::FromUint64(json["base_row_id"].get<u64>());
    row_cnt_ = json["row_count"].get<u64>();
    index_size_ = json["index_size"].get<u64>();
}

ChunkIndexMeta::ChunkIndexMeta(ChunkID chunk_id, SegmentIndexMeta &segment_index_meta)
    : kv_instance_(segment_index_meta.kv_instance()), segment_index_meta_(segment_index_meta), chunk_id_(chunk_id) {}

Status ChunkIndexMeta::GetIndexBuffer(BufferObj *&index_buffer) {
    if (!index_buffer_) {
        Status status = LoadIndexBuffer();
        if (!status.ok()) {
            return status;
        }
    }
    index_buffer = index_buffer_;
    return Status::OK();
}

Status ChunkIndexMeta::InitSet(const ChunkIndexMetaInfo &chunk_info) {
    chunk_info_ = chunk_info;
    {
        String chunk_info_key = GetChunkIndexTag("chunk_info");
        nlohmann::json chunk_info_json;
        chunk_info_->ToJson(chunk_info_json);
        Status status = kv_instance_.Put(chunk_info_key, chunk_info_json.dump());
        if (!status.ok()) {
            return status;
        }
    }

    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }

    SharedPtr<ColumnDef> column_def;
    {
        auto [col_def, status] = table_index_meta.GetColumnDef();
        if (!status.ok()) {
            return status;
        }
        column_def = std::move(col_def);
    }

    SegmentID segment_id = segment_index_meta_.segment_id();
    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();
    {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        switch (index_base->index_type_) {
            case IndexType::kSecondary: {
                auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
                auto index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                              index_dir,
                                                                              std::move(secondary_index_file_name),
                                                                              index_base,
                                                                              column_def,
                                                                              chunk_info.row_cnt_,
                                                                              buffer_mgr->persistence_manager());
                index_buffer_ = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kFullText: {
                auto column_length_file_name = MakeShared<String>(chunk_info.base_name_ + LENGTH_SUFFIX);
                auto index_file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                   MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                   index_dir,
                                                                   std::move(column_length_file_name),
                                                                   chunk_info.row_cnt_ * sizeof(u32),
                                                                   buffer_mgr->persistence_manager());
                index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kIVF: {
                auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
                auto index_file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                        MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                        index_dir,
                                                                        std::move(ivf_index_file_name),
                                                                        index_base,
                                                                        column_def,
                                                                        buffer_mgr->persistence_manager());
                index_buffer_ = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kHnsw: {
                auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
                auto index_file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                    MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                    index_dir,
                                                                    std::move(hnsw_index_file_name),
                                                                    index_base,
                                                                    column_def,
                                                                    buffer_mgr->persistence_manager(),
                                                                    chunk_info.index_size_);
                index_buffer_ = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kBMP: {
                auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
                auto file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                  MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                  index_dir,
                                                                  std::move(bmp_index_file_name),
                                                                  index_base,
                                                                  column_def,
                                                                  buffer_mgr->persistence_manager(),
                                                                  chunk_info.index_size_);
                index_buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
                break;
            }
            case IndexType::kEMVB: {
                auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
                const auto segment_start_offset = chunk_info.base_row_id_.segment_offset_;
                auto file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                   MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                   index_dir,
                                                                   std::move(emvb_index_file_name),
                                                                   index_base,
                                                                   column_def,
                                                                   segment_start_offset,
                                                                   buffer_mgr->persistence_manager());
                index_buffer_ = buffer_mgr->AllocateBufferObject(std::move(file_worker));
                break;
            }
            case IndexType::kDiskAnn: {
                LOG_WARN("Not implemented");
                break;
            }
            default: {
                UnrecoverableError("Not implemented yet");
            }
        }
        if (index_buffer_ == nullptr) {
            return Status::BufferManagerError("AllocateBufferObject failed");
        }
        index_buffer_->AddObjRc();
    }

    return Status::OK();
}

Status ChunkIndexMeta::LoadSet() {
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    SegmentID segment_id = segment_index_meta_.segment_id();

    ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
    Status status = this->GetChunkInfo(chunk_info_ptr);
    if (!status.ok()) {
        return status;
    }
    RowID base_row_id = chunk_info_ptr->base_row_id_;
    SizeT row_count = chunk_info_ptr->row_cnt_;
    const String &base_name = chunk_info_ptr->base_name_;
    SizeT index_size = chunk_info_ptr->index_size_;

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    auto [column_def, col_status] = table_index_meta.GetColumnDef();
    if (!col_status.ok()) {
        return status;
    }
    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();

    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            auto index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                          MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                          index_dir,
                                                                          std::move(secondary_index_file_name),
                                                                          index_base,
                                                                          column_def,
                                                                          row_count,
                                                                          buffer_mgr->persistence_manager());
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
            break;
        }
        case IndexType::kFullText: {
            auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
            auto index_file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                               MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                               index_dir,
                                                               std::move(column_length_file_name),
                                                               row_count * sizeof(u32),
                                                               buffer_mgr->persistence_manager());
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
            break;
        }
        case IndexType::kIVF: {
            auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            auto index_file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                    MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                    index_dir,
                                                                    std::move(ivf_index_file_name),
                                                                    index_base,
                                                                    column_def,
                                                                    buffer_mgr->persistence_manager());
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
            break;
        }
        case IndexType::kHnsw: {
            auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            auto index_file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                index_dir,
                                                                std::move(hnsw_index_file_name),
                                                                index_base,
                                                                column_def,
                                                                buffer_mgr->persistence_manager(),
                                                                index_size);
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
            break;
        }
        case IndexType::kBMP: {
            auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            auto file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                              index_dir,
                                                              std::move(bmp_index_file_name),
                                                              index_base,
                                                              column_def,
                                                              buffer_mgr->persistence_manager(),
                                                              index_size);
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        case IndexType::kEMVB: {
            auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            const auto segment_start_offset = base_row_id.segment_offset_;
            auto file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                               MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                               index_dir,
                                                               std::move(emvb_index_file_name),
                                                               index_base,
                                                               column_def,
                                                               segment_start_offset,
                                                               buffer_mgr->persistence_manager());
            index_buffer_ = buffer_mgr->GetBufferObject(std::move(file_worker));
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    if (index_buffer_ == nullptr) {
        return Status::BufferManagerError("GetBufferObject failed");
    }
    index_buffer_->AddObjRc();
    return Status::OK();
}

Status ChunkIndexMeta::RestoreSet() {
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    SegmentID segment_id = segment_index_meta_.segment_id();

    ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
    Status status = this->GetChunkInfo(chunk_info_ptr);
    if (!status.ok()) {
        return status;
    }
    RowID base_row_id = chunk_info_ptr->base_row_id_;
    SizeT row_count = chunk_info_ptr->row_cnt_;
    const String &base_name = chunk_info_ptr->base_name_;
    SizeT index_size = chunk_info_ptr->index_size_;

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    auto [column_def, col_status] = table_index_meta.GetColumnDef();
    if (!col_status.ok()) {
        return status;
    }
    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();
    UniquePtr<FileWorker> index_file_worker;
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                     MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                     index_dir,
                                                                     std::move(secondary_index_file_name),
                                                                     index_base,
                                                                     column_def,
                                                                     row_count,
                                                                     buffer_mgr->persistence_manager());
            break;
        }
        case IndexType::kFullText: {
            auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
            index_file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                          MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                          index_dir,
                                                          std::move(column_length_file_name),
                                                          row_count * sizeof(u32),
                                                          buffer_mgr->persistence_manager());
            break;
        }
        case IndexType::kIVF: {
            auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            index_file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                               MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                               index_dir,
                                                               std::move(ivf_index_file_name),
                                                               index_base,
                                                               column_def,
                                                               buffer_mgr->persistence_manager());
            break;
        }
        case IndexType::kHnsw: {
            auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            index_file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                           MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                           index_dir,
                                                           std::move(hnsw_index_file_name),
                                                           index_base,
                                                           column_def,
                                                           buffer_mgr->persistence_manager(),
                                                           index_size);
            break;
        }
        case IndexType::kBMP: {
            auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            index_file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                               MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                               index_dir,
                                                               std::move(bmp_index_file_name),
                                                               index_base,
                                                               column_def,
                                                               buffer_mgr->persistence_manager(),
                                                               index_size);
            break;
        }
        case IndexType::kEMVB: {
            auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id_));
            const auto segment_start_offset = base_row_id.segment_offset_;
            index_file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                index_dir,
                                                                std::move(emvb_index_file_name),
                                                                index_base,
                                                                column_def,
                                                                segment_start_offset,
                                                                buffer_mgr->persistence_manager());

            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    auto *buffer_obj = buffer_mgr->GetBufferObject(index_file_worker->GetFilePath());
    if (buffer_obj == nullptr) {
        index_buffer_ = buffer_mgr->GetBufferObject(std::move(index_file_worker));
        index_buffer_->AddObjRc();
    }
    if (index_buffer_ == nullptr) {
        return Status::BufferManagerError("GetBufferObject failed");
    }

    return Status::OK();
}

Status ChunkIndexMeta::UninitSet(UsageFlag usage_flag) {
    auto *kv_store = InfinityContext::instance().storage()->kv_store();
    Status status = this->GetIndexBuffer(index_buffer_);
    if (!status.ok()) {
        return status;
    }
    index_buffer_->PickForCleanup();

    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    auto [index_def, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    if (usage_flag == UsageFlag::kOther) {
        if (index_def->index_type_ == IndexType::kFullText) {
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            status = this->GetChunkInfo(chunk_info_ptr);
            if (!status.ok()) {
                return status;
            }
            SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();

            String posting_file = fmt::format("{}/{}", *index_dir, chunk_info_ptr->base_name_ + POSTING_SUFFIX);
            String dict_file = fmt::format("{}/{}", *index_dir, chunk_info_ptr->base_name_ + DICT_SUFFIX);

            PersistenceManager *pm = InfinityContext::instance().persistence_manager();
            if (pm != nullptr) {
                LOG_INFO(fmt::format("Cleaned chunk index entry, posting: {}, dictionary file: {}", posting_file, dict_file));

                PersistResultHandler handler(pm);
                PersistWriteResult result1 = pm->Cleanup(posting_file);
                PersistWriteResult result2 = pm->Cleanup(dict_file);

                handler.HandleWriteResult(result1);
                handler.HandleWriteResult(result2);

                kv_store->Delete(KeyEncode::PMObjectKey(posting_file));
                kv_store->Delete(KeyEncode::PMObjectKey(dict_file));

            } else {
                String absolute_posting_file = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), posting_file);
                String absolute_dict_file = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), dict_file);
                LOG_INFO(fmt::format("Clean chunk index entry , posting: {}, dictionary file: {}", absolute_posting_file, absolute_dict_file));

                VirtualStore::DeleteFile(absolute_posting_file);
                VirtualStore::DeleteFile(absolute_dict_file);
            }
        }
    }
    {
        String chunk_info_key = GetChunkIndexTag("chunk_info");
        status = kv_instance_.Delete(chunk_info_key);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status ChunkIndexMeta::SetChunkInfo(const ChunkIndexMetaInfo &chunk_info) {
    chunk_info_ = chunk_info;
    {
        String chunk_info_key = GetChunkIndexTag("chunk_info");
        nlohmann::json chunk_info_json;
        chunk_info_->ToJson(chunk_info_json);
        Status status = kv_instance_.Put(chunk_info_key, chunk_info_json.dump());
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status ChunkIndexMeta::SetChunkInfoNoPutKV(const ChunkIndexMetaInfo &chunk_info) {
    chunk_info_ = chunk_info;
    return Status::OK();
}

Status ChunkIndexMeta::FilePaths(Vector<String> &paths) {
    Status status;
    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    auto [index_def, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
    status = this->GetChunkInfo(chunk_info_ptr);
    if (!status.ok()) {
        return status;
    }
    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();
    switch (index_def->index_type_) {
        case IndexType::kFullText: {
            paths.push_back(fmt::format("{}/{}", *index_dir, chunk_info_ptr->base_name_ + POSTING_SUFFIX));
            paths.push_back(fmt::format("{}/{}", *index_dir, chunk_info_ptr->base_name_ + DICT_SUFFIX));
            paths.push_back(fmt::format("{}/{}", *index_dir, chunk_info_ptr->base_name_ + LENGTH_SUFFIX));
            break;
        }
        case IndexType::kHnsw:
        case IndexType::kEMVB:
        case IndexType::kIVF:
        case IndexType::kSecondary:
        case IndexType::kBMP: {
            String file_name = IndexFileName(segment_index_meta_.segment_id(), chunk_id_);
            String file_path = fmt::format("{}/{}", *index_dir, file_name);
            paths.push_back(file_path);
            break;
        }
        default: {
            String error_message = "Unsupported index type when add wal.";
            UnrecoverableError(error_message);
        }
    }
    return Status::OK();
}

Status ChunkIndexMeta::LoadChunkInfo() {
    String chunk_info_key = GetChunkIndexTag("chunk_info");
    String chunk_info_str;
    Status s = kv_instance_.Get(chunk_info_key, chunk_info_str);
    if (!s.ok()) {
        return s;
    }
    chunk_info_ = ChunkIndexMetaInfo();
    chunk_info_->FromJson(nlohmann::json::parse(chunk_info_str));
    return Status::OK();
}

Status ChunkIndexMeta::LoadIndexBuffer() {
    TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();

    String index_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), table_index_meta.GetTableIndexDir()->c_str());
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    auto [index_def, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SegmentID segment_id = segment_index_meta_.segment_id();
    switch (index_def->index_type_) {
        case IndexType::kSecondary:
        case IndexType::kIVF:
        case IndexType::kHnsw:
        case IndexType::kBMP:
        case IndexType::kEMVB: {
            String index_file_name = IndexFileName(segment_id, chunk_id_);
            String index_filepath = fmt::format("{}/{}", index_dir, index_file_name);
            index_buffer_ = buffer_mgr->GetBufferObject(index_filepath);
            if (index_buffer_ == nullptr) {
                return Status::BufferManagerError(fmt::format("GetBufferObject failed: {}", index_filepath));
            }
            break;
        }
        case IndexType::kFullText: {
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            {
                Status status = this->GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            auto column_length_file_name = chunk_info_ptr->base_name_ + LENGTH_SUFFIX;
            String index_filepath = fmt::format("{}/{}", index_dir, column_length_file_name);
            index_buffer_ = buffer_mgr->GetBufferObject(index_filepath);
            if (index_buffer_ == nullptr) {
                return Status::BufferManagerError(fmt::format("GetBufferObject failed: {}", index_filepath));
            }
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

String ChunkIndexMeta::GetChunkIndexTag(const String &tag) const {
    const TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    const TableMeeta &table_meta = table_index_meta.table_meta();
    return KeyEncode::CatalogIdxChunkTagKey(table_meta.db_id_str(),
                                            table_meta.table_id_str(),
                                            table_index_meta.index_id_str(),
                                            segment_index_meta_.segment_id(),
                                            chunk_id_,
                                            tag);
}

} // namespace infinity

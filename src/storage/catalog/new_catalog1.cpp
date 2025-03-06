// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module new_catalog;

import stl;
import third_party;
import vector_buffer;
import buffer_manager;
import infinity_context;
import block_version;
import column_def;
import infinity_exception;
import index_defines;
import create_index_info;

import secondary_index_file_worker;
import ivf_index_file_worker;
import raw_file_worker;
import hnsw_file_worker;
import bmp_index_file_worker;
import emvb_index_file_worker;
import version_file_worker;
import data_file_worker;
import var_file_worker;
import file_worker;

import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;

namespace infinity {

namespace {

String IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

} // namespace

void NewTxnGetVisibleRangeState::Init(SharedPtr<BlockLock> block_lock, BufferHandle version_buffer_handle, TxnTimeStamp begin_ts) {
    block_lock_ = std::move(block_lock);
    version_buffer_handle_ = std::move(version_buffer_handle);
    begin_ts_ = begin_ts;
    {
        std::shared_lock<std::shared_mutex> lock(block_lock_->mtx_);
        const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());
        block_offset_end_ = block_version->GetRowCount(begin_ts_);
    }
}

bool NewTxnGetVisibleRangeState::Next(BlockOffset block_offset_begin, Pair<BlockOffset, BlockOffset> &visible_range) {
    if (block_offset_begin == block_offset_end_) {
        return false;
    }
    [[maybe_unused]] const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());

    std::shared_lock<std::shared_mutex> lock(block_lock_->mtx_);
    while (block_offset_begin < block_offset_end_ && block_version->CheckDelete(block_offset_begin, begin_ts_)) {
        ++block_offset_begin;
    }
    BlockOffset row_idx;
    for (row_idx = block_offset_begin; row_idx < block_offset_end_; ++row_idx) {
        if (block_version->CheckDelete(row_idx, begin_ts_)) {
            break;
        }
    }
    visible_range = {block_offset_begin, row_idx};
    return block_offset_begin < row_idx;
}

Status NewCatalog::CleanTable(TableMeeta &table_meta) {
    Status status;

    SharedPtr<Vector<SegmentID>> segment_ids_ptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
        status = NewCatalog::CleanSegment(segment_meta);
        if (!status.ok()) {
            return status;
        }
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());
        status = NewCatalog::CleanTableIndex(table_index_meta);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::CleanTableIndex(TableIndexMeeta &table_index_meta) {
    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());
        status = NewCatalog::CleanSegmentIndex(segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta) {
    {
        Status status = table_meta.AddSegmentID(segment_id);
        if (!status.ok()) {
            return status;
        }
    }
    segment_meta.emplace(segment_id, table_meta, table_meta.kv_instance());
    {
        Status status = segment_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanSegment(SegmentMeta &segment_meta) {
    Status status;

    Vector<BlockID> *block_ids_ptr = nullptr;
    status = segment_meta.GetBlockIDs(block_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        status = NewCatalog::CleanBlock(block_meta);
        if (!status.ok()) {
            return status;
        }
    }
    segment_meta.UninitSet();

    return Status::OK();
}

Status NewCatalog::AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta) {
    {
        Status status = segment_meta.AddBlockID(block_id);
        if (!status.ok()) {
            return status;
        }
    }
    block_meta.emplace(block_id, segment_meta, segment_meta.kv_instance());
    {
        Status status = block_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> block_dir_ptr = block_meta->GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    BufferObj *version_buffer = nullptr;
    {
        auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                 MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                 block_dir_ptr,
                                                                 BlockVersion::FileName(),
                                                                 block_meta->block_capacity(),
                                                                 buffer_mgr->persistence_manager());
        version_buffer = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
        version_buffer->AddObjRc();
    }
    SizeT column_num = 0;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        column_num = column_defs_ptr->size();
    }
    for (SizeT column_idx = 0; column_idx < column_num; ++column_idx) {
        [[maybe_unused]] Optional<ColumnMeta> column_meta;
        Status status = NewCatalog::AddNewBlockColumn(*block_meta, column_idx, column_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanBlock(BlockMeta &block_meta) {
    Status status;

    BufferObj *version_buffer = nullptr;
    status = NewCatalog::GetVersionBufferObj(block_meta, version_buffer);
    if (!status.ok()) {
        return status;
    }
    version_buffer->PickForCleanup();

    SizeT column_num = 0;
    {
        TableMeeta &table_meta = block_meta.segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        column_num = column_defs_ptr->size();
    }
    for (SizeT column_idx = 0; column_idx < column_num; ++column_idx) {
        ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
        Status status = NewCatalog::CleanBlockColumn(column_meta);
        if (!status.ok()) {
            return status;
        }
    }
    block_meta.UninitSet();

    return Status::OK();
}

Status NewCatalog::AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta) {
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = block_meta.segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx].get();
    }

    column_meta.emplace(column_idx, block_meta, block_meta.kv_instance());
    {
        Status status = column_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    BufferObj *buffer = nullptr;
    {
        auto filename = MakeShared<String>(fmt::format("{}.col", column_idx));
        SizeT total_data_size = block_meta.block_capacity() * col_def->type()->Size();
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_dir_ptr,
                                                      filename,
                                                      total_data_size,
                                                      buffer_mgr->persistence_manager());
        buffer = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        buffer->AddObjRc();
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_idx));
        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             filename,
                                                             0, /*buffer_size*/
                                                             buffer_mgr->persistence_manager());
        BufferObj *outline_buffer = buffer_mgr->AllocateBufferObject(std::move(outline_file_worker));
        outline_buffer->AddObjRc();
    }
    return Status::OK();
}

Status NewCatalog::CleanBlockColumn(ColumnMeta &column_meta) {
    Status status;

    BufferObj *buffer_obj = nullptr;
    BufferObj *outline_buffer_obj = nullptr;
    status = NewCatalog::GetColumnBufferObj(column_meta, buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    buffer_obj->PickForCleanup();
    if (outline_buffer_obj) {
        outline_buffer_obj->PickForCleanup();
    }
    status = column_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta) {
    {
        Status status = table_index_meta.AddSegmentID(segment_id);
        if (!status.ok()) {
            return status;
        }
    }
    {
        segment_index_meta.emplace(segment_id, table_index_meta, table_index_meta.kv_instance());
        Status status = segment_index_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanSegmentIndex(SegmentIndexMeta &segment_index_meta) {
    Status status;

    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        status = NewCatalog::CleanChunkIndex(chunk_index_meta);
        if (!status.ok()) {
            return status;
        }
    }
    status = segment_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                    ChunkID chunk_id,
                                    RowID base_row_id,
                                    SizeT row_count,
                                    const String &base_name,
                                    SizeT index_size,
                                    Optional<ChunkIndexMeta> &chunk_index_meta,
                                    BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    SegmentID segment_id = segment_index_meta.segment_id();

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

    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();
    ChunkIndexMetaInfo chunk_info;
    {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        switch (index_base->index_type_) {
            case IndexType::kSecondary: {
                auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                              index_dir,
                                                                              std::move(secondary_index_file_name),
                                                                              index_base,
                                                                              column_def,
                                                                              row_count,
                                                                              buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
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
                buffer_obj = buffer_mgr->GetBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kIVF: {
                auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto index_file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                        MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                        index_dir,
                                                                        std::move(ivf_index_file_name),
                                                                        index_base,
                                                                        column_def,
                                                                        buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kHnsw: {
                auto hnsw_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto index_file_worker = MakeUnique<HnswFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                    MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                    index_dir,
                                                                    std::move(hnsw_index_file_name),
                                                                    index_base,
                                                                    column_def,
                                                                    buffer_mgr->persistence_manager(),
                                                                    index_size);
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kBMP: {
                auto bmp_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto file_worker = MakeUnique<BMPIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                  MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                  index_dir,
                                                                  std::move(bmp_index_file_name),
                                                                  index_base,
                                                                  column_def,
                                                                  buffer_mgr->persistence_manager(),
                                                                  index_size);
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(file_worker));
                break;
            }
            case IndexType::kEMVB: {
                auto emvb_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                const auto segment_start_offset = base_row_id.segment_offset_;
                auto file_worker = MakeUnique<EMVBIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                   MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                   index_dir,
                                                                   std::move(emvb_index_file_name),
                                                                   index_base,
                                                                   column_def,
                                                                   segment_start_offset,
                                                                   buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(file_worker));
                break;
            }
            default: {
                UnrecoverableError("Not implemented yet");
            }
        }
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError("AllocateBufferObject failed");
        }
        buffer_obj->AddObjRc();
    }
    chunk_info.base_name_ = base_name;
    chunk_info.base_row_id_ = base_row_id;
    chunk_info.row_cnt_ = row_count;
    {
        chunk_index_meta.emplace(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        Status status = chunk_index_meta->InitSet(chunk_info);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = segment_index_meta.AddChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanChunkIndex(ChunkIndexMeta &chunk_index_meta) {
    Status status;

    BufferObj *buffer_obj = nullptr;
    status = NewCatalog::GetChunkIndex(chunk_index_meta, buffer_obj);
    if (!status.ok()) {
        return status;
    }
    buffer_obj->PickForCleanup();

    status = chunk_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::GetColumnVector(ColumnMeta &column_meta, SizeT row_count, const ColumnVectorTipe &tipe, ColumnVector &column_vector) {
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = column_meta.block_meta().segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_meta.column_idx()].get();
    }

    BufferObj *buffer_obj = nullptr;
    BufferObj *outline_buffer_obj = nullptr;
    Status status = NewCatalog::GetColumnBufferObj(column_meta, buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    column_vector = ColumnVector(col_def->type());
    column_vector.Initialize(buffer_obj, outline_buffer_obj, row_count, tipe);
    return Status::OK();
}

Status NewCatalog::GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, NewTxnGetVisibleRangeState &state) {
    BufferObj *version_buffer = nullptr;
    Status status = NewCatalog::GetVersionBufferObj(block_meta, version_buffer);
    if (!status.ok()) {
        return status;
    }

    BufferHandle buffer_handle = version_buffer->Load();
    SharedPtr<BlockLock> block_lock;
    {
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
    }
    state.Init(std::move(block_lock), std::move(buffer_handle), begin_ts);
    return Status::OK();
}

Status NewCatalog::GetChunkIndex(ChunkIndexMeta &chunk_index_meta, BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = chunk_index_meta.segment_index_meta().table_index_meta();

    String index_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), table_index_meta.GetTableIndexDir()->c_str());
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SegmentID segment_id = chunk_index_meta.segment_index_meta().segment_id();
    ChunkID chunk_id = chunk_index_meta.chunk_id();
    switch (index_base->index_type_) {
        case IndexType::kSecondary:
        case IndexType::kIVF:
        case IndexType::kHnsw:
        case IndexType::kBMP:
        case IndexType::kEMVB: {
            String index_file_name = IndexFileName(segment_id, chunk_id);
            String index_filepath = fmt::format("{}/{}", index_dir, index_file_name);
            buffer_obj = buffer_mgr->GetBufferObject(index_filepath);
            if (buffer_obj == nullptr) {
                return Status::BufferManagerError("GetBufferObject failed");
            }
            break;
        }
        case IndexType::kFullText: {
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            {
                Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            auto column_length_file_name = chunk_info_ptr->base_name_ + LENGTH_SUFFIX;
            String index_filepath = fmt::format("{}/{}", index_dir, column_length_file_name);
            buffer_obj = buffer_mgr->GetBufferObject(index_filepath);
            if (buffer_obj == nullptr) {
                return Status::BufferManagerError("GetBufferObject failed");
            }
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    return Status::OK();
}

Status NewCatalog::GetColumnBufferObj(ColumnMeta &column_meta, BufferObj *&buffer_obj, BufferObj *&outline_buffer_obj) {
    SharedPtr<String> block_dir_ptr = column_meta.block_meta().GetBlockDir();
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = column_meta.block_meta().segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_meta.column_idx()].get();
    }

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    {
        String col_filename = std::to_string(column_meta.column_idx()) + ".col";
        String col_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + col_filename;
        buffer_obj = buffer_mgr->GetBufferObject(col_filepath);
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_filepath));
        }
    }
    [[maybe_unused]] SizeT chunk_offset = 0;
    {
        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            String outline_filename = fmt::format("col_{}_out_0", column_meta.column_idx());
            String outline_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + outline_filename;
            BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
            outline_buffer_obj = buffer_mgr->GetBufferObject(outline_filepath);
            if (outline_buffer_obj == nullptr) {
                return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_filepath));
            }
            {
                Status status = column_meta.GetChunkOffset(chunk_offset);
                if (!status.ok()) {
                    return status;
                }
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::GetVersionBufferObj(BlockMeta &block_meta, BufferObj *&version_buffer) {
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
    version_buffer = buffer_mgr->GetBufferObject(version_filepath);
    if (version_buffer == nullptr) {
        return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
    }

    return Status::OK();
}

} // namespace infinity
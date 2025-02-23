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

module new_txn;

import chunk_index_meta;
import segment_index_meta;
import table_index_meeta;
import table_meeta;
import infinity_context;
import buffer_manager;
import third_party;
import infinity_exception;
import secondary_index_file_worker;
import column_def;
import internal_types;
import index_file_worker;
import create_index_info;
import buffer_obj;

namespace infinity {

namespace {

String IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

} // namespace

Status NewTxn::AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta) {
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

Status NewTxn::AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                ChunkID chunk_id,
                                RowID base_row_id,
                                SizeT row_count,
                                Optional<ChunkIndexMeta> &chunk_index_meta) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    SegmentID segment_id = segment_index_meta.segment_id();

    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<ColumnDef> column_def;
    {
        Status status = table_index_meta.GetColumnDef(column_def);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> index_dir;
    {
        Status status = table_index_meta.GetIndexDir(index_dir);
        if (!status.ok()) {
            return status;
        }
    }
    ChunkIndexMetaInfo chunk_info;
    {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        UniquePtr<IndexFileWorker> index_file_worker;
        switch (index_base->index_type_) {
            case IndexType::kSecondary: {
                auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
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
            default: {
                UnrecoverableError("Not implemented yet");
            }
        }
        BufferObj *buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError("AllocateBufferObject failed");
        }
        buffer_obj->AddObjRc();
    }
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

Status NewTxn::GetChunkIndex(ChunkIndexMeta &chunk_index_meta, BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = chunk_index_meta.segment_index_meta().table_index_meta();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    SegmentID segment_id = chunk_index_meta.segment_index_meta().segment_id();
    ChunkID chunk_id = chunk_index_meta.chunk_id();
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            String secondary_index_file_name = IndexFileName(segment_id, chunk_id);
            buffer_obj = buffer_mgr->GetBufferObject(secondary_index_file_name);
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

} // namespace infinity

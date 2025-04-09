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

module new_txn;

import new_txn_manager;
import kv_store;
import new_catalog;
import chunk_index_meta;
import segment_index_meta;
import table_index_meeta;
import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import logger;
import infinity_context;
import buffer_manager;
import third_party;
import infinity_exception;
import column_def;
import internal_types;
import create_index_info;
import buffer_obj;
import mem_index;
import wal_entry;
import secondary_index_in_mem;
import secondary_index_data;
import default_values;
import ivf_index_data_in_mem;
import ivf_index_data;
import memory_indexer;
import index_full_text;
import column_index_reader;
import column_index_merger;
import abstract_hnsw;
import index_hnsw;
import abstract_bmp;
import index_bmp;
import emvb_index_in_mem;
import emvb_index;
import meta_key;
import data_access_state;

namespace infinity {

Status NewTxn::DumpMemIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    Status status;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    if (!status.ok()) {
        return status;
    }
    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

    status = new_catalog_->SetMemIndexDump(table_key);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->IncreaseTableWriteCount(this, table_key);
    if (!status.ok()) {
        Status mem_index_status = new_catalog_->UnsetMemIndexDump(table_key);
        if (!mem_index_status.ok()) {
            UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", table_name, mem_index_status.message()));
        }
        return status;
    }

    ChunkID new_chunk_id = 0;
    status = this->DumpMemIndexInner(segment_index_meta, new_chunk_id);
    if (!status.ok()) {
        Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
        if (!ref_cnt_status.ok()) {
            UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
        }
        Status mem_index_status = new_catalog_->UnsetMemIndexDump(table_key);
        if (!mem_index_status.ok()) {
            UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", table_name, mem_index_status.message()));
        }
        return status;
    }

    ChunkIndexMeta chunk_index_meta(new_chunk_id, segment_index_meta);
    status = this->AddChunkWal(db_name, table_name, index_name, table_key, chunk_index_meta, {}, DumpIndexCause::kDumpMemIndex);
    if (!status.ok()) {
        Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
        if (!ref_cnt_status.ok()) {
            UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
        }
        Status mem_index_status = new_catalog_->UnsetMemIndexDump(table_key);
        if (!mem_index_status.ok()) {
            UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", table_name, mem_index_status.message()));
        }
        return status;
    }

    return Status::OK();
}

Status NewTxn::OptimizeIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    Status status = Status::OK();

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    Optional<TableIndexMeeta> table_index_meta_opt;
    String table_key;
    String index_key;
    status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta_opt, table_index_meta_opt, &table_key, &index_key);
    if (!status.ok()) {
        return status;
    }

    status = new_catalog_->IncreaseTableWriteCount(this, table_key);
    if (!status.ok()) {
        return status;
    }

    TableMeeta &table_meta = *table_meta_opt;
    TableIndexMeeta &table_index_meta = *table_index_meta_opt;
    SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);

    Vector<ChunkID> *old_chunk_ids_ptr = nullptr;
    {
        status = segment_index_meta.GetChunkIDs(old_chunk_ids_ptr);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
        if (old_chunk_ids_ptr->size() <= 1) {
            return Status::OK();
        }
    }
    RowID base_rowid;
    u32 row_cnt = 0;
    Vector<SizeT> row_cnts;

    String base_name;

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
        if (!ref_cnt_status.ok()) {
            UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
        }
        return index_status;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        status = this->OptimizeFtIndex(index_base, segment_index_meta, base_rowid, row_cnt, base_name);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
    } else {
        base_rowid = RowID(segment_id, 0);
        RowID last_rowid = base_rowid;
        ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
        for (ChunkID old_chunk_id : *old_chunk_ids_ptr) {
            ChunkIndexMeta old_chunk_meta(old_chunk_id, segment_index_meta);
            {
                status = old_chunk_meta.GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                    if (!ref_cnt_status.ok()) {
                        UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                    }
                    return status;
                }
            }
            if (last_rowid != chunk_info_ptr->base_row_id_) {
                UnrecoverableError("OptimizeIndex: base_row_id is not continuous");
            }
            last_rowid.segment_offset_ += chunk_info_ptr->row_cnt_;
            row_cnt += chunk_info_ptr->row_cnt_;
            row_cnts.push_back(chunk_info_ptr->row_cnt_);
        }
    }
    Vector<ChunkID> deprecate_ids = *old_chunk_ids_ptr;
    ChunkID chunk_id = 0;
    {
        status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        status = NewCatalog::AddNewChunkIndex(segment_index_meta, chunk_id, base_rowid, row_cnt, base_name, 0 /*index_size*/, chunk_index_meta);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
        status = chunk_index_meta->GetIndexBuffer(buffer_obj);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            Vector<Pair<u32, BufferObj *>> old_buffers;
            for (SizeT i = 0; i < deprecate_ids.size(); ++i) {
                ChunkID old_chunk_id = deprecate_ids[i];
                ChunkIndexMeta old_chunk_meta(old_chunk_id, segment_index_meta);

                BufferObj *buffer_obj = nullptr;
                {
                    // Status status = NewCatalog::GetChunkIndex(old_chunk_meta, buffer_obj);
                    status = old_chunk_meta.GetIndexBuffer(buffer_obj);
                    if (!status.ok()) {
                        Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                        if (!ref_cnt_status.ok()) {
                            UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                        }
                        return status;
                    }
                }
                old_buffers.emplace_back(row_cnts[i], buffer_obj);
            }

            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<SecondaryIndexData *>(buffer_handle.GetDataMut());
            data_ptr->InsertMergeData(old_buffers);
            break;
        }
        case IndexType::kFullText: {
            // Skip here. merge finished in `OptimizeFtIndex`
            break;
        }
        case IndexType::kIVF: {
            SegmentMeta segment_meta(segment_id, table_meta);
            SharedPtr<ColumnDef> column_def;
            {
                auto [col_def, status] = table_index_meta.GetColumnDef();
                if (!status.ok()) {
                    Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                    if (!ref_cnt_status.ok()) {
                        UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                    }
                    return status;
                }
                column_def = std::move(col_def);
            }

            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<IVFIndexInChunk *>(buffer_handle.GetDataMut());
            data_ptr->BuildIVFIndex(segment_meta, row_cnt, column_def);
            break;
        }
        case IndexType::kHnsw:
        case IndexType::kBMP: {
            SegmentMeta segment_meta(segment_id, table_meta);
            SharedPtr<ColumnDef> column_def;
            {
                auto [col_def, status] = table_index_meta.GetColumnDef();
                if (!status.ok()) {
                    Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                    if (!ref_cnt_status.ok()) {
                        UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                    }
                    return status;
                }
                column_def = std::move(col_def);
            }
            status = OptimizeVecIndex(index_base, column_def, segment_meta, base_rowid, row_cnt, buffer_obj);
            if (!status.ok()) {
                Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                if (!ref_cnt_status.ok()) {
                    UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                }
                return status;
            }
            break;
        }
        case IndexType::kEMVB: {
            SegmentMeta segment_meta(segment_id, table_meta);
            SharedPtr<ColumnDef> column_def;
            {
                auto [col_def, status] = table_index_meta.GetColumnDef();
                if (!status.ok()) {
                    Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
                    if (!ref_cnt_status.ok()) {
                        UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
                    }
                    return status;
                }
                column_def = std::move(col_def);
            }

            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<EMVBIndex *>(buffer_handle.GetDataMut());
            data_ptr->BuildEMVBIndex(base_rowid, row_cnt, segment_meta, column_def);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    {
        Vector<ChunkID> chunk_ids = {chunk_id};
        status = segment_index_meta.SetChunkIDs(chunk_ids);
        if (!status.ok()) {
            Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
            if (!ref_cnt_status.ok()) {
                UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
            }
            return status;
        }
    }

    buffer_obj->Save();
    if (index_base->index_type_ == IndexType::kHnsw || index_base->index_type_ == IndexType::kBMP) {
        if (buffer_obj->type() != BufferType::kMmap) {
            buffer_obj->ToMmap();
        }
    }

    status = this->AddChunkWal(db_name, table_name, index_name, table_key, *chunk_index_meta, deprecate_ids, DumpIndexCause::kOptimizeIndex);
    if (!status.ok()) {
        Status ref_cnt_status = new_catalog_->DecreaseTableWriteCount(this, table_key);
        if (!ref_cnt_status.ok()) {
            UnrecoverableError(fmt::format("Can't decrease table reference count: {}, cause: {}", table_name, status.message()));
        }
        return status;
    }
    return Status::OK();
}

Status NewTxn::ListIndex(const String &db_name, const String &table_name, Vector<String> &index_names) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }
    index_names = *index_names_ptr;
    return Status::OK();
}

Status NewTxn::AppendIndex(TableIndexMeeta &table_index_meta, const Vector<AppendRange> &append_ranges) {
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    ColumnID column_id = 0;
    {
        auto [column_def, status] = table_index_meta.table_meta().GetColumnDefByColumnName(index_base->column_name());
        if (!status.ok()) {
            return status;
        }
        column_id = column_def->id();
    }
    Optional<SegmentMeta> segment_meta;
    Optional<SegmentIndexMeta> segment_index_meta;
    Optional<BlockMeta> block_meta;
    Optional<ColumnMeta> column_meta;
    BlockOffset cur_offset = 0;
    BlockOffset cur_row_cnt = 0;

    auto append_in_column = [&]() {
        ColumnVector col;
        {
            Status status = NewCatalog::GetColumnVector(*column_meta, cur_offset + cur_row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        SegmentOffset block_offset = block_meta->block_capacity() * block_meta->block_id() + cur_offset;
        RowID base_row_id(segment_meta->segment_id(), block_offset);
        {
            Status status = this->AppendMemIndex(*segment_index_meta, base_row_id, col, cur_offset, cur_row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };

    for (const AppendRange &range : append_ranges) {
        if (!segment_meta || segment_meta->segment_id() != range.segment_id_) {
            segment_meta.emplace(range.segment_id_, table_index_meta.table_meta());

            {
                Vector<SegmentID> *segment_ids_ptr = nullptr;
                Status status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
                if (!status.ok()) {
                    return status;
                }
                auto iter = std::find(segment_ids_ptr->begin(), segment_ids_ptr->end(), range.segment_id_);
                if (iter == segment_ids_ptr->end()) {
                    status = NewCatalog::AddNewSegmentIndex(table_index_meta, range.segment_id_, segment_index_meta);
                    if (!status.ok()) {
                        return status;
                    }
                } else {
                    segment_index_meta.emplace(range.segment_id_, table_index_meta);
                }
            }
            block_meta.reset();
        }
        if (!block_meta || block_meta->block_id() != range.block_id_) {
            if (block_meta) {
                append_in_column();
            }
            block_meta.emplace(range.block_id_, segment_meta.value());
            column_meta.emplace(column_id, *block_meta);
            cur_offset = range.start_offset_;
            cur_row_cnt = range.row_count_;
        } else {
            if (cur_offset + cur_row_cnt != range.start_offset_) {
                UnrecoverableError("AppendIndex: append range is not continuous");
            }
            cur_row_cnt += range.row_count_;
        }
    }
    if (cur_row_cnt > 0) {
        Status status = append_in_column();
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status
NewTxn::AppendMemIndex(SegmentIndexMeta &segment_index_meta, RowID base_row_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    auto [index_base, index_status] = segment_index_meta.table_index_meta().GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta.GetAndWriteMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            SharedPtr<SecondaryIndexInMem> memory_secondary_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_secondary_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, nullptr, base_row_id);
                }
                memory_secondary_index = mem_index->memory_secondary_index_;
            }
            memory_secondary_index->InsertBlockData(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        case IndexType::kFullText: {
            const auto *index_fulltext = static_cast<const IndexFullText *>(index_base.get());
            SharedPtr<MemoryIndexer> memory_indexer;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_indexer_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }

                    SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
                    String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
                    String full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
                    mem_index->memory_indexer_ =
                        MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_, nullptr);
                    // table_index_entry_->UpdateFulltextSegmentTs(commit_ts);
                } else {
                    RowID exp_begin_row_id = mem_index->memory_indexer_->GetBaseRowId() + mem_index->memory_indexer_->GetDocCount();
                    assert(base_row_id >= exp_begin_row_id);
                    if (base_row_id > exp_begin_row_id) {
                        LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                             base_row_id.ToUint64(),
                                             exp_begin_row_id.ToUint64(),
                                             base_row_id - exp_begin_row_id));
                        mem_index->memory_indexer_->InsertGap(base_row_id - exp_begin_row_id);
                    }
                }
                if (index_fulltext->IsRealtime()) {
                    UnrecoverableError("Not implemented yet");
                } else {
                    auto col_ptr = MakeShared<ColumnVector>(std::move(col));
                    mem_index->memory_indexer_->Insert(col_ptr, offset, row_cnt, false);
                }
            }
            break;
        }
        case IndexType::kIVF: {
            SharedPtr<IVFIndexInMem> memory_ivf_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_ivf_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_ivf_index_ = IVFIndexInMem::NewIVFIndexInMem(column_def.get(), index_base.get(), base_row_id, nullptr);
                }
                memory_ivf_index = mem_index->memory_ivf_index_;
            }
            memory_ivf_index->InsertBlockData(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        case IndexType::kHnsw: {
            SharedPtr<HnswIndexInMem> memory_hnsw_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_hnsw_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_hnsw_index_ = HnswIndexInMem::Make(base_row_id, index_base.get(), column_def.get(), nullptr, true /*trace*/);
                }
                memory_hnsw_index = mem_index->memory_hnsw_index_;
            }
            memory_hnsw_index->InsertVecs(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        case IndexType::kBMP: {
            SharedPtr<BMPIndexInMem> memory_bmp_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_bmp_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_bmp_index_ = MakeShared<BMPIndexInMem>(base_row_id, index_base.get(), column_def.get(), nullptr);
                }
                memory_bmp_index = mem_index->memory_bmp_index_;
            }
            memory_bmp_index->AddDocs(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        case IndexType::kEMVB: {
            SharedPtr<EMVBIndexInMem> memory_emvb_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_emvb_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_emvb_index_ = EMVBIndexInMem::NewEMVBIndexInMem(index_base, column_def, base_row_id);
                    TableMeeta &table_meta = segment_index_meta.table_index_meta().table_meta();
                    mem_index->memory_emvb_index_->SetSegmentID(table_meta.db_id_str(), table_meta.table_id_str(), segment_index_meta.segment_id());
                }
                memory_emvb_index = mem_index->memory_emvb_index_;
            }
            memory_emvb_index->Insert(col, offset, row_cnt, segment_index_meta.kv_instance(), begin_ts);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

Status NewTxn::PopulateIndex(const String &db_name,
                             const String &table_name,
                             const String &index_name,
                             const String &table_key,
                             TableIndexMeeta &table_index_meta,
                             SegmentMeta &segment_meta,
                             SizeT segment_row_cnt,
                             DumpIndexCause dump_index_cause,
                             WalCmdCreateIndex *create_index_cmd_ptr) {
    Optional<SegmentIndexMeta> segment_index_meta;
    {
        Status status = NewCatalog::AddNewSegmentIndex(table_index_meta, segment_meta.segment_id(), segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SharedPtr<ColumnDef> column_def;
    ColumnID column_id = 0;
    {
        auto [column_def_, status] = table_index_meta.table_meta().GetColumnDefByColumnName(index_base->column_name());
        if (!status.ok()) {
            return status;
        }
        column_def = column_def_;
        column_id = column_def->id();
    }
    Vector<ChunkID> old_chunk_ids;
    {
        Vector<ChunkID> *old_chunk_ids_ptr = nullptr;
        Status status = segment_index_meta->GetChunkIDs(old_chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        old_chunk_ids = *old_chunk_ids_ptr;
    }
    ChunkID new_chunk_id = 0;
    if (index_base->index_type_ == IndexType::kIVF) {
        Status status = this->PopulateIvfIndexInner(index_base, *segment_index_meta, segment_meta, column_def, new_chunk_id);
        if (!status.ok()) {
            return status;
        }
    } else if (index_base->index_type_ == IndexType::kEMVB) {
        Status status = this->PopulateEmvbIndexInner(index_base, *segment_index_meta, segment_meta, column_def, new_chunk_id);
        if (!status.ok()) {
            return status;
        }
    } else {
        switch (index_base->index_type_) {
            case IndexType::kSecondary:
            case IndexType::kHnsw:
            case IndexType::kBMP: {
                Status status = this->PopulateIndexToMem(*segment_index_meta, segment_meta, column_id, segment_row_cnt);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case IndexType::kFullText: {
                Status status = this->PopulateFtIndexInner(index_base, *segment_index_meta, segment_meta, column_id, segment_row_cnt);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            default: {
                UnrecoverableError("Invalid index type");
                return Status::OK();
            }
        }
        {
            Status status = DumpMemIndexInner(*segment_index_meta, new_chunk_id);
            if (!status.ok()) {
                return status;
            }
        }
    }

    ChunkIndexMeta chunk_index_meta(new_chunk_id, *segment_index_meta);
    if (create_index_cmd_ptr) {
        WalCmdCreateIndex &create_index_cmd = *create_index_cmd_ptr;
        Vector<WalChunkIndexInfo> chunk_infos;
        chunk_infos.emplace_back(chunk_index_meta);

        create_index_cmd.segment_index_infos_.emplace_back(segment_meta.segment_id(), std::move(chunk_infos));
    } else {
        Status status = this->AddChunkWal(db_name, table_name, index_name, table_key, chunk_index_meta, old_chunk_ids, dump_index_cause);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::ReplayDumpIndex(WalCmdDumpIndex *dump_index_cmd) {
    Status status;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    status = GetTableIndexMeta(dump_index_cmd->db_name_,
                               dump_index_cmd->table_name_,
                               dump_index_cmd->index_name_,
                               db_meta,
                               table_meta,
                               table_index_meta,
                               &table_key,
                               &index_key);
    if (!status.ok()) {
        return status;
    }
    dump_index_cmd->db_id_str_ = db_meta->db_id_str();
    dump_index_cmd->table_id_str_ = table_meta->table_id_str();
    dump_index_cmd->index_id_str_ = table_index_meta->index_id_str();

    SegmentID segment_id = dump_index_cmd->segment_id_;
    Optional<SegmentIndexMeta> segment_index_meta_opt;
    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        Status status = table_index_meta->GetSegmentIDs(segment_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        auto iter = std::find(segment_ids_ptr->begin(), segment_ids_ptr->end(), segment_id);
        if (iter == segment_ids_ptr->end()) {
            status = NewCatalog::AddNewSegmentIndex(*table_index_meta, segment_id, segment_index_meta_opt);
            if (!status.ok()) {
                return status;
            }
        } else {
            segment_index_meta_opt.emplace(segment_id, *table_index_meta);
        }
    }
    SegmentIndexMeta &segment_index_meta = *segment_index_meta_opt;

    Vector<ChunkID> new_chunk_ids;
    {
        HashSet<ChunkID> deprecate_chunk_ids(dump_index_cmd->deprecate_ids_.begin(), dump_index_cmd->deprecate_ids_.end());
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        for (ChunkID chunk_id : *chunk_ids_ptr) {
            if (!deprecate_chunk_ids.contains(chunk_id)) {
                new_chunk_ids.push_back(chunk_id);
            }
        }
    }
    for (const WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
        new_chunk_ids.push_back(chunk_info.chunk_id_);

        status = NewCatalog::LoadFlushedChunkIndex(segment_index_meta, chunk_info);
        if (!status.ok()) {
            return status;
        }
    }
    status = segment_index_meta.SetChunkIDs(new_chunk_ids);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::PopulateIndexToMem(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, ColumnID column_id, SizeT segment_row_cnt) {
    auto [block_ids, status] = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }
    SizeT block_capacity = DEFAULT_BLOCK_CAPACITY;
    for (BlockID block_id : *block_ids) {
        BlockMeta block_meta(block_id, segment_meta);
        ColumnMeta column_meta(column_id, block_meta);

        SizeT row_cnt = block_id == block_ids->back() ? segment_row_cnt - block_capacity * (block_ids->size() - 1) : block_capacity;

        ColumnVector col;
        status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
        if (!status.ok()) {
            return status;
        }
        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id();
        RowID begin_row_id(segment_meta.segment_id(), block_offset);
        u32 offset = 0;
        status = this->AppendMemIndex(segment_index_meta, begin_row_id, col, offset, row_cnt);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::PopulateFtIndexInner(SharedPtr<IndexBase> index_base,
                                    SegmentIndexMeta &segment_index_meta,
                                    SegmentMeta &segment_meta,
                                    ColumnID column_id,
                                    SizeT segment_row_cnt) {
    if (index_base->index_type_ != IndexType::kFullText) {
        UnrecoverableError("Invalid index type");
    }
    const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base.get());
    RowID base_row_id(segment_index_meta.segment_id(), 0);
    String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
    String full_path;
    {
        SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
        full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
    }
    Status status;
    SharedPtr<MemIndex> mem_index;
    status = segment_index_meta.GetAndWriteMemIndex(mem_index);
    if (!status.ok()) {
        return status;
    }
    mem_index->memory_indexer_ =
        MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_, nullptr);
    MemoryIndexer *memory_indexer = mem_index->memory_indexer_.get();

    Vector<BlockID> *block_ids_ptr = nullptr;

    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }
    SizeT block_capacity = DEFAULT_BLOCK_CAPACITY;

    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        ColumnMeta column_meta(column_id, block_meta);

        SizeT row_cnt = block_id == block_ids_ptr->back() ? segment_row_cnt - block_capacity * (block_ids_ptr->size() - 1) : block_capacity;

        ColumnVector col;
        status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
        if (!status.ok()) {
            return status;
        }
        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id();
        RowID begin_row_id(segment_meta.segment_id(), block_offset);

        RowID exp_begin_row_id = memory_indexer->GetBaseRowId() + memory_indexer->GetDocCount();
        assert(begin_row_id >= exp_begin_row_id);
        if (begin_row_id > exp_begin_row_id) {
            LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                 begin_row_id.ToUint64(),
                                 exp_begin_row_id.ToUint64(),
                                 begin_row_id - exp_begin_row_id));
            memory_indexer->InsertGap(begin_row_id - exp_begin_row_id);
        }

        auto col_ptr = MakeShared<ColumnVector>(std::move(col));
        memory_indexer->Insert(col_ptr, 0, row_cnt, true);
        memory_indexer->Commit(true);
    }
    return Status::OK();
}

Status NewTxn::PopulateIvfIndexInner(SharedPtr<IndexBase> index_base,
                                     SegmentIndexMeta &segment_index_meta,
                                     SegmentMeta &segment_meta,
                                     SharedPtr<ColumnDef> column_def,
                                     ChunkID &new_chunk_id) {
    RowID base_row_id(segment_index_meta.segment_id(), 0);
    u32 row_count = 0;
    {
        auto [rc, status] = segment_meta.GetRowCnt1();
        if (!status.ok()) {
            return status;
        }
        row_count = rc;
    }
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    new_chunk_id = chunk_id;
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status =
            NewCatalog::AddNewChunkIndex(segment_index_meta, chunk_id, base_row_id, row_count, "" /*base_name*/, 0 /*index_size*/, chunk_index_meta);
        if (!status.ok()) {
            return status;
        }
        status = chunk_index_meta->GetIndexBuffer(buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    {
        BufferHandle buffer_handle = buffer_obj->Load();
        auto *data_ptr = static_cast<IVFIndexInChunk *>(buffer_handle.GetDataMut());
        data_ptr->BuildIVFIndex(segment_meta, row_count, column_def);
    }
    buffer_obj->Save();
    return Status::OK();
}

Status NewTxn::PopulateEmvbIndexInner(SharedPtr<IndexBase> index_base,
                                      SegmentIndexMeta &segment_index_meta,
                                      SegmentMeta &segment_meta,
                                      SharedPtr<ColumnDef> column_def,
                                      ChunkID &new_chunk_id) {
    RowID base_row_id(segment_index_meta.segment_id(), 0);
    u32 row_count = 0;
    {
        auto [rc, status] = segment_meta.GetRowCnt1();
        if (!status.ok()) {
            return status;
        }
        row_count = rc;
    }
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    new_chunk_id = chunk_id;
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status =
            NewCatalog::AddNewChunkIndex(segment_index_meta, chunk_id, base_row_id, row_count, "" /*base_name*/, 0 /*index_size*/, chunk_index_meta);
        status = chunk_index_meta->GetIndexBuffer(buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    {
        BufferHandle buffer_handle = buffer_obj->Load();
        auto *data_ptr = static_cast<EMVBIndex *>(buffer_handle.GetDataMut());
        data_ptr->BuildEMVBIndex(base_row_id, row_count, segment_meta, column_def);
    }
    buffer_obj->Save();
    return Status::OK();
}

Status NewTxn::OptimizeFtIndex(SharedPtr<IndexBase> index_base,
                               SegmentIndexMeta &segment_index_meta,
                               RowID &base_rowid_out,
                               u32 &row_cnt_out,
                               String &base_name_out) {
    const auto *index_fulltext = static_cast<const IndexFullText *>(index_base.get());

    Vector<ChunkID> chunk_ids;
    {
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        Status status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        chunk_ids = *chunk_ids_ptr;
    }
    if (chunk_ids.size() <= 1) {
        return Status::OK();
    }
    String msg = fmt::format("merging {}", *index_fulltext->index_name_);
    Vector<String> base_names;
    Vector<RowID> base_rowids;
    RowID base_rowid;
    u32 total_row_count = 0;
    Vector<ChunkID> old_ids;

    for (auto iter = chunk_ids.begin(); iter != chunk_ids.end(); ++iter) {
        ChunkID chunk_id = *iter;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
        {
            Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        if (chunk_id == 0) {
            base_rowid = chunk_info_ptr->base_row_id_;
        }
        if (const RowID expect_base_row_id = base_rowid + total_row_count; chunk_info_ptr->base_row_id_ > expect_base_row_id) {
            msg += fmt::format(" stop at gap to chunk {}, expect_base_row_id: {:016x}, base_row_id: {:016x}",
                               chunk_info_ptr->base_name_,
                               expect_base_row_id.ToUint64(),
                               chunk_info_ptr->base_row_id_.ToUint64());
            chunk_ids.erase(iter, chunk_ids.end());
            break;
        } else if (chunk_info_ptr->base_row_id_ < expect_base_row_id) {
            msg += fmt::format(" found overlap to chunk {}, expect_base_row_id: {:016x}, base_row_id: {:016x}",
                               chunk_info_ptr->base_name_,
                               expect_base_row_id.ToUint64(),
                               chunk_info_ptr->base_row_id_.ToUint64());
            UnrecoverableError(msg);
            return Status::OK();
        }
        msg += " " + chunk_info_ptr->base_name_;
        base_names.push_back(chunk_info_ptr->base_name_);
        base_rowids.push_back(chunk_info_ptr->base_row_id_);
        total_row_count += chunk_info_ptr->row_cnt_;
    }

    if (chunk_ids.size() <= 1) {
        msg += fmt::format(" skip merge due to only {} chunk", chunk_ids.size());
        LOG_INFO(msg);
        return Status::OK();
    }

    String dst_base_name = fmt::format("ft_{:016x}_{:x}", base_rowid.ToUint64(), total_row_count);
    msg += " -> " + dst_base_name;
    LOG_INFO(msg);

    SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
    ColumnIndexMerger column_index_merger(*index_dir, index_fulltext->flag_);
    column_index_merger.Merge(base_names, base_rowids, dst_base_name);
    {
        base_rowid_out = base_rowid;
        row_cnt_out = total_row_count;
        base_name_out = dst_base_name;
    }

    // // OPTIMIZE invoke this func at which the txn hasn't been commited yet.
    // TxnTimeStamp ts = std::max(txn->BeginTS(), txn->CommitTS());
    // table_index_entry->UpdateFulltextSegmentTs(ts);
    LOG_INFO(fmt::format("done merging {} {}", *index_fulltext->index_name_, dst_base_name));

    return Status::OK();
}

Status NewTxn::OptimizeVecIndex(SharedPtr<IndexBase> index_base,
                                SharedPtr<ColumnDef> column_def,
                                SegmentMeta &segment_meta,
                                RowID base_rowid,
                                u32 total_row_cnt,
                                BufferObj *buffer_obj) {
    auto [block_ids, status] = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }

    if (index_base->index_type_ == IndexType::kHnsw) {
        const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base.get());
        if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
            UnrecoverableError("Not implemented yet");
        }

        UniquePtr<HnswIndexInMem> memory_hnsw_index = HnswIndexInMem::Make(base_rowid, index_base.get(), column_def.get(), nullptr);
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            SizeT block_row_cnt = 0;
            // std::tie(block_row_cnt, status) = block_meta.GetRowCnt();
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            if (!status.ok()) {
                return status;
            }
            ColumnMeta column_meta(column_def->id(), block_meta);
            SizeT row_cnt = std::min(block_row_cnt, SizeT(total_row_cnt));
            total_row_cnt -= row_cnt;
            ColumnVector col;
            status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
            u32 offset = 0;
            memory_hnsw_index->InsertVecs(base_rowid.segment_offset_, col, offset, row_cnt);
        }

        memory_hnsw_index->Dump(buffer_obj);
    } else if (index_base->index_type_ == IndexType::kBMP) {
        auto memory_bmp_index = MakeShared<BMPIndexInMem>(base_rowid, index_base.get(), column_def.get(), nullptr);

        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            SizeT block_row_cnt = 0;
            // std::tie(block_row_cnt, status) = block_meta.GetRowCnt();
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            if (!status.ok()) {
                return status;
            }
            ColumnMeta column_meta(column_def->id(), block_meta);
            SizeT row_cnt = std::min(block_row_cnt, SizeT(total_row_cnt));
            total_row_cnt -= row_cnt;
            ColumnVector col;
            status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
            u32 offset = 0;
            memory_bmp_index->AddDocs(base_rowid.segment_offset_, col, offset, row_cnt);
        }
        memory_bmp_index->Dump(buffer_obj);
    } else {
        UnrecoverableError("Not implemented yet");
    }
    return Status::OK();
}

Status NewTxn::DumpMemIndexInner(SegmentIndexMeta &segment_index_meta, ChunkID &new_chunk_id) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta.GetMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNoMemIndex();
        if (!status.ok()) {
            return status;
        }
    }
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    new_chunk_id = chunk_id;
    RowID row_id{};
    u32 row_count = 0;
    String base_name;
    SizeT index_size = 0;

    // dump mem index only happens in parallel with read, not write, so no lock is needed.
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            if (mem_index->memory_secondary_index_ == nullptr) {
                return Status::DumpingMemIndex("No secondary mem index on segment");
            }
            row_id = mem_index->memory_secondary_index_->GetBeginRowID();
            row_count = mem_index->memory_secondary_index_->GetRowCount();
            break;
        }
        case IndexType::kFullText: {
            if (mem_index->memory_indexer_ == nullptr) {
                return Status::DumpingMemIndex("No full text mem index on segment");
            }
            base_name = mem_index->memory_indexer_->GetBaseName();
            row_id = mem_index->memory_indexer_->GetBaseRowId();
            row_count = mem_index->memory_indexer_->GetDocCount();
            break;
        }
        case IndexType::kIVF: {
            if (mem_index->memory_ivf_index_ == nullptr) {
                return Status::DumpingMemIndex("No IVF index on segment");
            }
            row_id = mem_index->memory_ivf_index_->GetBeginRowID();
            row_count = mem_index->memory_ivf_index_->GetRowCount();
            break;
        }
        case IndexType::kHnsw: {
            if (mem_index->memory_hnsw_index_ == nullptr) {
                return Status::DumpingMemIndex("No HNSW index on segment");
            }
            row_id = mem_index->memory_hnsw_index_->GetBeginRowID();
            row_count = mem_index->memory_hnsw_index_->GetRowCount();
            index_size = mem_index->memory_hnsw_index_->GetSizeInBytes();
            break;
        }
        case IndexType::kBMP: {
            if (mem_index->memory_bmp_index_ == nullptr) {
                return Status::DumpingMemIndex("No BMP index on segment");
            }
            row_id = mem_index->memory_bmp_index_->GetBeginRowID();
            row_count = mem_index->memory_bmp_index_->GetRowCount();
            index_size = mem_index->memory_bmp_index_->GetSizeInBytes();
            break;
        }
        case IndexType::kEMVB: {
            if (mem_index->memory_emvb_index_ == nullptr) {
                return Status::DumpingMemIndex("No EMVB index on segment");
            }
            row_id = mem_index->memory_emvb_index_->GetBeginRowID();
            row_count = mem_index->memory_emvb_index_->GetRowCount();
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = NewCatalog::AddNewChunkIndex(segment_index_meta, chunk_id, row_id, row_count, base_name, index_size, chunk_index_meta);
        if (!status.ok()) {
            return status;
        }
        status = chunk_index_meta->GetIndexBuffer(buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            mem_index->memory_secondary_index_->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        case IndexType::kFullText: {
            mem_index->memory_indexer_->Dump(false /*offline*/, false /*spill*/);
            u64 len_sum = mem_index->memory_indexer_->GetColumnLengthSum();
            u32 len_cnt = mem_index->memory_indexer_->GetDocCount();
            Status status = segment_index_meta.UpdateFtInfo(len_sum, len_cnt);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case IndexType::kIVF: {
            mem_index->memory_ivf_index_->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        case IndexType::kHnsw: {
            mem_index->memory_hnsw_index_->Dump(buffer_obj);
            buffer_obj->Save();
            if (buffer_obj->type() != BufferType::kMmap) {
                buffer_obj->ToMmap();
            }
            break;
        }
        case IndexType::kBMP: {
            mem_index->memory_bmp_index_->Dump(buffer_obj);
            buffer_obj->Save();
            if (buffer_obj->type() != BufferType::kMmap) {
                buffer_obj->ToMmap();
            }
            break;
        }
        case IndexType::kEMVB: {
            mem_index->memory_emvb_index_->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

Status NewTxn::AddChunkWal(const String &db_name,
                           const String &table_name,
                           const String &index_name,
                           const String &table_key,
                           ChunkIndexMeta &chunk_index_meta,
                           const Vector<ChunkID> &deprecate_ids,
                           DumpIndexCause dump_index_cause) {
    SegmentIndexMeta &segment_index_meta = chunk_index_meta.segment_index_meta();
    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(chunk_index_meta);
    SegmentID segment_id = segment_index_meta.segment_id();
    auto dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id, std::move(chunk_infos), deprecate_ids);
    if (dump_index_cause == DumpIndexCause::kDumpMemIndex) {
        dump_cmd->clear_mem_index_ = true;
        dump_cmd->db_id_str_ = segment_index_meta.table_index_meta().table_meta().db_id_str();
        dump_cmd->table_id_str_ = segment_index_meta.table_index_meta().table_meta().table_id_str();
        dump_cmd->index_id_str_ = segment_index_meta.table_index_meta().index_id_str();
    }
    dump_cmd->table_key_ = table_key;
    dump_cmd->dump_cause_ = dump_index_cause;

    wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(dump_cmd));
    txn_context_ptr_->AddOperation(MakeShared<String>(dump_cmd->ToString()));

    return Status::OK();
}

Status NewTxn::CountMemIndexGapInSegment(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, Vector<AppendRange> &append_ranges) {
    Status status;
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    Vector<ChunkIndexMetaInfo> chunk_index_meta_infos;
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        ChunkIndexMetaInfo *chunk_index_meta_info_ptr = nullptr;
        status = chunk_index_meta.GetChunkInfo(chunk_index_meta_info_ptr);
        if (!status.ok()) {
            return status;
        }
        chunk_index_meta_infos.push_back(*chunk_index_meta_info_ptr);
    }
    std::sort(chunk_index_meta_infos.begin(), chunk_index_meta_infos.end(), [](const ChunkIndexMetaInfo &lhs, const ChunkIndexMetaInfo &rhs) {
        return lhs.base_row_id_ < rhs.base_row_id_;
    });
    SegmentID segment_id = segment_meta.segment_id();

    RowID start_row_id(segment_id, 0);
    for (const ChunkIndexMetaInfo &chunk_index_meta_info : chunk_index_meta_infos) {
        if (chunk_index_meta_info.base_row_id_ < start_row_id) {
            UnrecoverableError("chunk index range overlap");
        } else {
            if (chunk_index_meta_info.base_row_id_ > start_row_id) {
                UnrecoverableError("Not implemented");
            }
            start_row_id = chunk_index_meta_info.base_row_id_ + chunk_index_meta_info.row_cnt_;
        }
    }

    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }
    SizeT block_capacity = DEFAULT_BLOCK_CAPACITY;
    Vector<BlockID> block_ids = *block_ids_ptr;
    sort(block_ids.begin(), block_ids.end());
    BlockID start_block_id = start_row_id.segment_offset_ / block_capacity;
    BlockOffset block_offset = start_row_id.segment_offset_ % block_capacity;
    {
        SizeT i = 0;
        while (i < block_ids.size() && block_ids[i] != start_block_id) {
            ++i;
        }
        if (i >= block_ids.size()) {
            return Status::OK();
        }
        if (block_ids[i] != start_block_id) {
            UnrecoverableError(fmt::format("block id {} not found in segment {}", start_block_id, segment_id));
        }
        for (; i < block_ids.size(); ++i) {
            BlockID block_id = block_ids[i];
            BlockMeta block_meta(block_id, segment_meta);
            SizeT block_row_cnt = 0;
            // std::tie(block_row_cnt, status) = block_meta.GetRowCnt();
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            if (!status.ok()) {
                return status;
            }
            append_ranges.emplace_back(segment_id, block_id, block_offset, block_row_cnt);
            block_offset = 0;
        }
    }
    return Status::OK();
}

Status NewTxn::RecoverMemIndex(TableIndexMeeta &table_index_meta) {
    Status status;
    TableMeeta &table_meta = table_index_meta.table_meta();

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    Vector<SegmentID> *index_segment_ids_ptr = nullptr;
    status = table_index_meta.GetSegmentIDs(index_segment_ids_ptr);
    if (!status.ok()) {
        return status;
    }

    Vector<AppendRange> append_ranges;
    HashSet<SegmentID> index_segment_ids_set(index_segment_ids_ptr->begin(), index_segment_ids_ptr->end());
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        if (!index_segment_ids_set.contains(segment_id)) {
            LOG_WARN(fmt::format("Segment {} not in index {}", segment_id, table_index_meta.index_id_str()));
            Optional<SegmentIndexMeta> segment_index_meta;
            status = NewCatalog::AddNewSegmentIndex(table_index_meta, segment_id, segment_index_meta);
            if (!status.ok()) {
                return status;
            }
            status = CountMemIndexGapInSegment(*segment_index_meta, segment_meta, append_ranges);
            if (!status.ok()) {
                return status;
            }
        } else {
            SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
            status = CountMemIndexGapInSegment(segment_index_meta, segment_meta, append_ranges);
            if (!status.ok()) {
                return status;
            }
        }
    }
    status = this->AppendIndex(table_index_meta, append_ranges);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::CommitMemIndex(TableIndexMeeta &table_index_meta) {
    Status status;

    SharedPtr<IndexBase> index_base;
    std::tie(index_base, status) = table_index_meta.GetIndexBase();
    if (!status.ok()) {
        return status;
    }

    if (index_base->index_type_ != IndexType::kFullText) {
        return Status::OK();
    }

    Vector<SegmentID> *index_segment_ids_ptr = nullptr;
    status = table_index_meta.GetSegmentIDs(index_segment_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *index_segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);

        SharedPtr<MemIndex> mem_index;
        Status status = segment_index_meta.GetMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }

        SharedPtr<MemoryIndexer> memory_indexer = mem_index->memory_indexer_;
        if (memory_indexer) {
            memory_indexer->Commit();
        }
    }

    return Status::OK();
}

Status NewTxn::GetFullTextIndexReader(const String &db_name, const String &table_name, SharedPtr<IndexReader> &index_reader) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    SharedPtr<TableIndexReaderCache> ft_index_cache;
    status = table_meta->GetFtIndexCache(ft_index_cache);
    if (!status.ok()) {
        return status;
    }
    index_reader = ft_index_cache->GetIndexReader(this);
    return Status::OK();
}

Status NewTxn::CommitCreateIndex(WalCmdCreateIndex *create_index_cmd) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_index_cmd->db_name_;
    const String &table_name = create_index_cmd->table_name_;
    const String &index_name = *create_index_cmd->index_base_->index_name_;
    const String &db_id_str = create_index_cmd->db_id_;
    const String &table_id_str = create_index_cmd->table_id_;
    const String &table_key = create_index_cmd->table_key_;

    // Get latest index id and lock the id
    String index_id_str;
    Status status = IncrLatestID(index_id_str, LATEST_INDEX_ID);
    if (!status.ok()) {
        return status;
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_, begin_ts);
    Optional<TableIndexMeeta> table_index_meta_opt;
    status = NewCatalog::AddNewTableIndex(table_meta, index_id_str, commit_ts, create_index_cmd->index_base_, table_index_meta_opt);
    if (!status.ok()) {
        return status;
    }
    TableIndexMeeta &table_index_meta = *table_index_meta_opt;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    if (this->IsReplay()) {
        WalCmdDumpIndex dump_index_cmd(db_name, table_name, index_name, 0);
        dump_index_cmd.dump_cause_ = DumpIndexCause::kReplayCreateIndex;
        for (const WalSegmentIndexInfo &segment_index_info : create_index_cmd->segment_index_infos_) {
            dump_index_cmd.segment_id_ = segment_index_info.segment_id_;
            dump_index_cmd.chunk_infos_ = segment_index_info.chunk_infos_;

            status = this->ReplayDumpIndex(&dump_index_cmd);
            if (!status.ok()) {
                return status;
            }
        }
    } else {
        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            auto [segment_row_cnt, status] = segment_meta.GetRowCnt1();
            if (!status.ok()) {
                return status;
            }
            status = this->PopulateIndex(db_name,
                                         table_name,
                                         index_name,
                                         table_key,
                                         table_index_meta,
                                         segment_meta,
                                         segment_row_cnt,
                                         DumpIndexCause::kCreateIndex,
                                         create_index_cmd);
            if (!status.ok()) {
                return status;
            }
        }
    }
    if (create_index_cmd->index_base_->index_type_ == IndexType::kFullText) {
        auto ft_cache = MakeShared<TableIndexReaderCache>(db_id_str, table_id_str);
        Status status = table_meta.AddFtIndexCache(ft_cache);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::PostCommitDumpIndex(const WalCmdDumpIndex *dump_index_cmd, KVInstance *kv_instance) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    const String &db_id_str = dump_index_cmd->db_id_str_;
    const String &table_id_str = dump_index_cmd->table_id_str_;
    const String &index_id_str = dump_index_cmd->index_id_str_;
    SegmentID segment_id = dump_index_cmd->segment_id_;

//    const String &table_key = dump_index_cmd->table_key_;

//    if (dump_index_cmd->dump_cause_ == DumpIndexCause::kDumpMemIndex) {
//        Status mem_index_status = new_catalog_->UnsetMemIndexDump(table_key);
//        if (!mem_index_status.ok()) {
//            UnrecoverableError(fmt::format("Can't unset mem index dump: {}, cause: {}", dump_index_cmd->table_name_, mem_index_status.message()));
//        }
//    }

    if (dump_index_cmd->clear_mem_index_) {

        TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);

        const String &index_id_str_ = dump_index_cmd->index_id_str_;
        TableIndexMeeta table_index_meta(index_id_str_, table_meta);

        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        SharedPtr<MemIndex> mem_index;
        Status status = segment_index_meta.GetMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
        mem_index->ClearMemIndex();
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    for (ChunkID deprecate_id : dump_index_cmd->deprecate_ids_) {
        new_catalog->AddCleanedMeta(commit_ts, MakeUnique<ChunkIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id, deprecate_id));
    }
    return Status::OK();
}

} // namespace infinity

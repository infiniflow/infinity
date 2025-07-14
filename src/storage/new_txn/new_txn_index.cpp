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
#include <memory>

module infinity_core;

import :new_txn_manager;
import :kv_store;
import :chunk_index_meta;
import :segment_index_meta;
import :table_index_meeta;
import :catalog_meta;
import :db_meeta;
import :table_meeta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :logger;
import :infinity_context;
import :buffer_manager;
import :third_party;
import :infinity_exception;
import column_def;
import internal_types;
import create_index_info;
import :buffer_obj;
import :mem_index;
import :wal_entry;
import :secondary_index_in_mem;
import :secondary_index_data;
import :default_values;
import :ivf_index_data_in_mem;
import :ivf_index_data;
import :memory_indexer;
import :index_full_text;
import :column_index_reader;
import :column_index_merger;
#ifdef INDEX_HANDLER
import :hnsw_handler;
import :bmp_handler;
#else
import :abstract_hnsw;
import :abstract_bmp;
#endif
import :index_hnsw;
import :index_bmp;
import :emvb_index_in_mem;
import :emvb_index;
import :meta_key;
import :data_access_state;
import :hnsw_util;
import :bmp_util;
import :defer_op;
import :base_txn_store;
import :kv_code;
import :buffer_handle;
import :bg_task;
import :mem_index_appender;
import :txn_context;
import :kv_utility;
import statement_common;
import :dump_index_process;
import :persistence_manager;
import :persist_result_handler;

namespace infinity {

Status NewTxn::DumpMemIndex(const String &db_name, const String &table_name, const String &index_name) {
    this->SetTxnType(TransactionType::kDumpMemIndex);

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
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_index_meta->GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }
    base_txn_store_ = MakeShared<DumpMemIndexTxnStore>();
    DumpMemIndexTxnStore *txn_store = static_cast<DumpMemIndexTxnStore *>(base_txn_store_.get());
    txn_store->db_name_ = db_name;
    txn_store->db_id_str_ = table_index_meta->table_meta().db_id_str();
    txn_store->table_name_ = table_name;
    txn_store->table_id_str_ = table_index_meta->table_meta().table_id_str();
    txn_store->index_name_ = index_name;
    txn_store->index_id_str_ = table_index_meta->index_id_str();
    txn_store->index_id_ = std::stoull(txn_store->index_id_str_);
    txn_store->segment_ids_ = *segment_ids_ptr;
    txn_store->table_key_ = table_key;

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

        SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        if (mem_index == nullptr || (mem_index->GetBaseMemIndex() == nullptr && mem_index->GetEMVBIndex() == nullptr)) {
            continue;
        }

        ChunkID chunk_id = 0;
        {
            status = segment_index_meta.GetNextChunkID(chunk_id);
            if (!status.ok()) {
                return status;
            }
            status = segment_index_meta.SetNextChunkID(chunk_id + 1);
            if (!status.ok()) {
                return status;
            }
        }

        ChunkIndexMetaInfo chunk_index_meta_info;
        if (mem_index->GetBaseMemIndex() != nullptr) {
            chunk_index_meta_info = mem_index->GetBaseMemIndex()->GetChunkIndexMetaInfo();
        } else if (mem_index->GetEMVBIndex() != nullptr) {
            chunk_index_meta_info = mem_index->GetEMVBIndex()->GetChunkIndexMetaInfo();
        } else {
            return Status::UnexpectedError("Invalid mem index.");
        }
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        chunk_index_meta.SetChunkInfoNoPutKV(chunk_index_meta_info);
        Vector<WalChunkIndexInfo> chunk_infos;
        chunk_infos.emplace_back(chunk_index_meta);

        txn_store->chunk_infos_in_segments_.emplace(segment_id, chunk_infos);
    }

    if (txn_store->chunk_infos_in_segments_.empty()) {
        base_txn_store_ = nullptr; // No mem index to dump.
    }

    return Status::OK();
}

Status NewTxn::DumpMemIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, RowID begin_row_id) {
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
    SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();

    // Return when there is no mem index to dump.
    if (mem_index == nullptr || (mem_index->GetBaseMemIndex() == nullptr && mem_index->GetEMVBIndex() == nullptr) ||
        (begin_row_id != RowID() && mem_index->GetBaseMemIndex() != nullptr && begin_row_id != mem_index->GetBaseMemIndex()->GetBeginRowID())) {
        LOG_INFO(fmt::format("NewTxn::DumpMemIndex skipped dumping MemIndex {}.{}.{}.{}.{} since it doesn't exist.",
                             db_name,
                             table_name,
                             index_name,
                             segment_id,
                             begin_row_id.ToUint64()));
        return Status::OK();
    }

    // Get chunk id of the chunk index to dump mem index to.
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

    // Get chunk index info of the mem index and put it to chunk index meta.
    ChunkIndexMetaInfo chunk_index_meta_info;
    if (mem_index->GetBaseMemIndex() != nullptr) {
        chunk_index_meta_info = mem_index->GetBaseMemIndex()->GetChunkIndexMetaInfo();
    } else if (mem_index->GetEMVBIndex() != nullptr) {
        chunk_index_meta_info = mem_index->GetEMVBIndex()->GetChunkIndexMetaInfo();
    } else {
        return Status::UnexpectedError("Invalid mem index.");
    }
    ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
    chunk_index_meta.SetChunkInfoNoPutKV(chunk_index_meta_info);
    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(chunk_index_meta);

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = MakeShared<DumpMemIndexTxnStore>();
        DumpMemIndexTxnStore *txn_store = static_cast<DumpMemIndexTxnStore *>(base_txn_store_.get());
        txn_store->db_name_ = db_name;
        txn_store->db_id_str_ = segment_index_meta.table_index_meta().table_meta().db_id_str();
        txn_store->table_name_ = table_name;
        txn_store->table_id_str_ = segment_index_meta.table_index_meta().table_meta().table_id_str();
        txn_store->index_name_ = index_name;
        txn_store->index_id_str_ = segment_index_meta.table_index_meta().index_id_str();
        txn_store->index_id_ = std::stoull(txn_store->index_id_str_);
        txn_store->table_key_ = table_key;
        txn_store->segment_ids_ = {segment_id};
        txn_store->chunk_infos_in_segments_.emplace(segment_id, chunk_infos);
    } else {
        DumpMemIndexTxnStore *txn_store = static_cast<DumpMemIndexTxnStore *>(base_txn_store_.get());
        txn_store->segment_ids_.emplace_back(segment_id);
        txn_store->chunk_infos_in_segments_.emplace(segment_id, chunk_infos);
    }

    return Status::OK();
}

Status NewTxn::CommitBottomDumpMemIndex(WalCmdDumpIndexV2 *dump_index_cmd) {
    Status status;
    const String &db_name = dump_index_cmd->db_name_;
    const String &table_name = dump_index_cmd->table_name_;
    const String &index_name = dump_index_cmd->index_name_;
    const SegmentID &segment_id = dump_index_cmd->segment_id_;

    ChunkID chunk_id = dump_index_cmd->chunk_infos_[0].chunk_id_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Optional<TableIndexMeeta> table_index_meta;
    String table_key;
    String index_key;
    status = GetTableIndexMeta(db_name, table_name, index_name, db_meta, table_meta, table_index_meta, &table_key, &index_key);
    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta);

    // Dump Mem Index
    status = this->DumpSegmentMemIndex(segment_index_meta, chunk_id);
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto [index_base, status2] = table_index_meta->GetIndexBase();
    if (!status2.ok()) {
        return status2;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        table_index_meta->UpdateFulltextSegmentTS(commit_ts);
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        // When all data and index is write to disk, try to finalize the
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result);
    }

    return Status::OK();
}

Status NewTxn::OptimizeAllIndexes() {
    // TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    CatalogMeta catalog_meta(this);
    Vector<String> *db_id_strs_ptr = nullptr;
    Vector<String> *db_names_ptr = nullptr;
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SizeT i = 0; i < db_id_strs_ptr->size(); ++i) {
        const String &db_id_str = (*db_id_strs_ptr)[i];
        const String &db_name = (*db_names_ptr)[i];

        DBMeeta db_meta(db_id_str, this);
        Vector<String> *table_id_strs_ptr = nullptr;
        Vector<String> *table_names_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr, &table_names_ptr);
        if (!status.ok()) {
            return status;
        }
        for (SizeT i = 0; i < table_id_strs_ptr->size(); ++i) {
            // const String &table_id_str = (*table_id_strs_ptr)[i];
            const String &table_name = (*table_names_ptr)[i];

            status = this->OptimizeTableIndexes(db_name, table_name);
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::OptimizeTableIndexes(const String &db_name, const String &table_name) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }
    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }

    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_names_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, *table_meta);

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
            status = this->OptimizeIndexInner(segment_index_meta, index_name, table_name, db_name, table_key);
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::OptimizeIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    this->SetTxnType(TransactionType::kOptimizeIndex);

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

    SegmentIndexMeta segment_index_meta(segment_id, *table_index_meta_opt);

    return OptimizeIndexInner(segment_index_meta, index_name, table_name, db_name, table_key);
}

Status NewTxn::OptimizeIndexInner(SegmentIndexMeta &segment_index_meta,
                                  const String &index_name,
                                  const String &table_name,
                                  const String &db_name,
                                  const String &table_key) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    TableMeeta &table_meta = table_index_meta.table_meta();
    SegmentID segment_id = segment_index_meta.segment_id();

    auto [old_chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
    if (!status.ok()) {
        return status;
    }
    if (old_chunk_ids_ptr->size() <= 1) {
        return Status::OK();
    }
    RowID base_rowid;
    u32 row_cnt = 0;
    Vector<SizeT> row_cnts;

    String base_name;

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        status = this->OptimizeFtIndex(index_base, segment_index_meta, base_rowid, row_cnt, base_name);
        if (!status.ok()) {
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
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        status =
            NewCatalog::AddNewChunkIndex1(segment_index_meta, this, chunk_id, base_rowid, row_cnt, base_name, 0 /*index_size*/, chunk_index_meta);
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
            Vector<Pair<u32, BufferObj *>> old_buffers;
            for (SizeT i = 0; i < deprecate_ids.size(); ++i) {
                ChunkID old_chunk_id = deprecate_ids[i];
                ChunkIndexMeta old_chunk_meta(old_chunk_id, segment_index_meta);

                BufferObj *buffer_obj = nullptr;
                {
                    // Status status = NewCatalog::GetChunkIndex(old_chunk_meta, buffer_obj);
                    status = old_chunk_meta.GetIndexBuffer(buffer_obj);
                    if (!status.ok()) {
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
                    return status;
                }
                column_def = std::move(col_def);
            }
            status = OptimizeVecIndex(index_base, column_def, segment_meta, base_rowid, row_cnt, buffer_obj);
            if (!status.ok()) {
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
        // To delete deprecated chunk ids
        status = segment_index_meta.RemoveChunkIDs(deprecate_ids);
        //        Vector<ChunkID> chunk_ids = {chunk_id};
        //        status = segment_index_meta.SetChunkIDs(chunk_ids);
        if (!status.ok()) {
            return status;
        }
    }

    buffer_obj->Save();
    if (index_base->index_type_ == IndexType::kHnsw || index_base->index_type_ == IndexType::kBMP) {
        if (buffer_obj->type() != BufferType::kMmap) {
            buffer_obj->ToMmap();
        }
    }

    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(*chunk_index_meta);

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = MakeShared<OptimizeIndexTxnStore>();
    }

    OptimizeIndexTxnStore *optimize_index_txn_store = static_cast<OptimizeIndexTxnStore *>(base_txn_store_.get());
    if (std::find(optimize_index_txn_store->db_names_.begin(), optimize_index_txn_store->db_names_.end(), db_name) ==
        optimize_index_txn_store->db_names_.end()) {
        optimize_index_txn_store->db_names_.emplace_back(db_name);
    }
    if (std::find(optimize_index_txn_store->table_names_in_db_[db_name].begin(),
                  optimize_index_txn_store->table_names_in_db_[db_name].end(),
                  table_name) == optimize_index_txn_store->table_names_in_db_[db_name].end()) {
        optimize_index_txn_store->table_names_in_db_[db_name].emplace_back(table_name);
    }

    optimize_index_txn_store->entries_.emplace_back(db_name,
                                                    table_meta.db_id_str(),
                                                    std::stoull(table_meta.db_id_str()),
                                                    table_name,
                                                    table_meta.table_id_str(),
                                                    std::stoull(table_meta.table_id_str()),
                                                    table_key,
                                                    index_name,
                                                    table_index_meta.index_id_str(),
                                                    std::stoull(table_index_meta.index_id_str()),
                                                    segment_id,
                                                    std::move(chunk_infos),
                                                    std::move(deprecate_ids));

    return Status::OK();
}

Status NewTxn::OptimizeIndexByParams(const String &db_name,
                                     const String &table_name,
                                     const String &index_name,
                                     Vector<UniquePtr<InitParameter>> raw_params) {
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
    TableIndexMeeta &table_index_meta = *table_index_meta_opt;

    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }

    bool opt = false;
    SharedPtr<IndexBase> new_index_base;
    switch (index_base->index_type_) {
        case IndexType::kBMP: {
            opt = true;
            break;
        }
        case IndexType::kHnsw: {
            auto params = HnswUtil::ParseOptimizeOptions(raw_params);
            if (!params) {
                break;
            }
            opt = true;
            if (params->compress_to_lvq) {
                auto *hnsw_index = static_cast<IndexHnsw *>(index_base.get());
                if (hnsw_index->encode_type_ != HnswEncodeType::kPlain) {
                    LOG_WARN("Not implemented");
                    break;
                }
                auto new_index_hnsw = MakeShared<IndexHnsw>(*hnsw_index);
                // IndexHnsw old_index_hnsw = *hnsw_index;
                new_index_hnsw->encode_type_ = HnswEncodeType::kLVQ;
                if (new_index_hnsw->build_type_ == HnswBuildType::kLSG) {
                    new_index_hnsw->build_type_ = HnswBuildType::kPlain;
                }
                new_index_base = std::move(new_index_hnsw);
            }
            break;
        }
        default: {
        }
    }
    if (!opt) {
        return Status::OK();
    }
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        Status status = OptimizeSegmentIndexByParams(segment_index_meta, raw_params);
        if (!status.ok()) {
            return status;
        }
    }

    if (new_index_base) {
        Status status = table_index_meta.SetIndexBase(new_index_base);
        if (!status.ok()) {
            return status;
        }
    }

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdOptimizeV2>(db_name,
                                                                 db_meta->db_id_str(),
                                                                 table_name,
                                                                 table_meta_opt->table_id_str(),
                                                                 index_name,
                                                                 table_index_meta_opt->index_id_str(),
                                                                 std::move(raw_params));
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(wal_command->ToString()));

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        // When all data and index is write to disk, try to finalize the
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result);
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

Status NewTxn::AppendIndex(TableIndexMeeta &table_index_meta, const Pair<RowID, u64> &append_range) {
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SizeT column_idx = 0;
    {
        auto [column_def, status] = table_index_meta.table_meta().GetColumnDefByColumnName(index_base->column_name(), &column_idx);
        if (!status.ok()) {
            return status;
        }
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
            Status status = NewCatalog::GetColumnVector(*column_meta, cur_offset + cur_row_cnt, ColumnVectorMode::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        BlockID block_id = block_meta->block_id();
        {
            Status status = this->AppendMemIndex(*segment_index_meta, block_id, col, cur_offset, cur_row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };

    SegmentID segment_id = append_range.first.segment_id_;
    BlockID block_id = append_range.first.segment_offset_ >> BLOCK_OFFSET_SHIFT;
    cur_offset = append_range.first.segment_offset_ & BLOCK_OFFSET_MASK;
    cur_row_cnt = append_range.second;
    if (!segment_meta || segment_meta->segment_id() != segment_id) {
        segment_meta.emplace(segment_id, table_index_meta.table_meta());
        if (!table_index_meta.HasSegmentIndexID(segment_id) && append_range.first.segment_offset_ == 0) {
            Status status = NewCatalog::AddNewSegmentIndex1(table_index_meta, this, segment_id, segment_index_meta);
            if (!status.ok()) {
                return status;
            }
        } else {
            segment_index_meta.emplace(segment_id, table_index_meta);
        }
    }
    if (!block_meta || block_meta->block_id() != block_id) {
        block_meta.emplace(block_id, segment_meta.value());
        column_meta.emplace(column_idx, block_meta.value());
    }
    Status status = append_in_column();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status
NewTxn::AppendMemIndex(SegmentIndexMeta &segment_index_meta, BlockID block_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt) {
    SegmentOffset block_offset = block_id * DEFAULT_BLOCK_CAPACITY;
    RowID base_row_id = RowID(segment_index_meta.segment_id(), block_offset + offset);

    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto [index_base, index_status] = segment_index_meta.table_index_meta().GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }
    SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
    bool is_null = mem_index->IsNull();
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            SharedPtr<SecondaryIndexInMem> memory_secondary_index;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                memory_secondary_index = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, base_row_id);

                mem_index->SetSecondaryIndex(memory_secondary_index);
            } else {
                memory_secondary_index = mem_index->GetSecondaryIndex();
            }
            memory_secondary_index->InsertBlockData(block_offset, col, offset, row_cnt);
            break;
        }
        case IndexType::kFullText: {
            const auto *index_fulltext = static_cast<const IndexFullText *>(index_base.get());
            SharedPtr<MemoryIndexer> memory_indexer;
            bool need_to_update_ft_segment_ts = false;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }

                SharedPtr<String> index_dir = segment_index_meta.GetSegmentIndexDir();
                String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
                String full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
                memory_indexer = MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_);
                need_to_update_ft_segment_ts = true;
                mem_index->SetFulltextIndex(memory_indexer);
            } else {
                memory_indexer = mem_index->GetFulltextIndex();
                LOG_TRACE(fmt::format("AppendMemIndex: memory_indexer_ is not null, base_row_id: {}, doc_count: {}",
                                      base_row_id.ToUint64(),
                                      memory_indexer->GetDocCount()));
                RowID exp_begin_row_id = memory_indexer->GetBeginRowID() + memory_indexer->GetDocCount();
                assert(base_row_id >= exp_begin_row_id);
                if (base_row_id > exp_begin_row_id) {
                    LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                         base_row_id.ToUint64(),
                                         exp_begin_row_id.ToUint64(),
                                         base_row_id - exp_begin_row_id));
                    memory_indexer->InsertGap(base_row_id - exp_begin_row_id);
                }
            }

            auto col_ptr = MakeShared<ColumnVector>(std::move(col));
            if (index_fulltext->IsRealtime()) {
                UniquePtr<std::binary_semaphore> sema = memory_indexer->AsyncInsert(col_ptr, offset, row_cnt);
                this->AddSemaphore(std::move(sema));
            } else {
                // mem_index->GetFulltextIndex()->Insert(col_ptr, offset, row_cnt, false);
                SharedPtr<AppendMemIndexTask> append_mem_index_task = MakeShared<AppendMemIndexTask>(mem_index, col_ptr, offset, row_cnt);
                memory_indexer->AsyncInsertTop(append_mem_index_task.get());
                auto *mem_index_appender = InfinityContext::instance().storage()->mem_index_appender();
                mem_index_appender->Submit(append_mem_index_task);
            }
            if (need_to_update_ft_segment_ts) {
                // To avoid deadlock of mem index mutex and table index reader cache mutex, update the ts here.
                // query will lock table index reader cache mutex first, then mem index mutex.
                // append will lock mem index mutex first, then table index reader cache mutex.
                segment_index_meta.table_index_meta().UpdateFulltextSegmentTS(commit_ts);
            }
            break;
        }
        case IndexType::kIVF: {
            SharedPtr<IVFIndexInMem> memory_ivf_index;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                memory_ivf_index = IVFIndexInMem::NewIVFIndexInMem(column_def.get(), index_base.get(), base_row_id);
                mem_index->SetIVFIndex(memory_ivf_index);

            } else {
                memory_ivf_index = mem_index->GetIVFIndex();
            }
            memory_ivf_index->InsertBlockData(block_offset, col, offset, row_cnt);
            break;
        }
        case IndexType::kHnsw: {
            SharedPtr<HnswIndexInMem> memory_hnsw_index;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                memory_hnsw_index = HnswIndexInMem::Make(base_row_id, index_base.get(), column_def.get(), true /*trace*/);
                mem_index->SetHnswIndex(memory_hnsw_index);
            } else {
                memory_hnsw_index = mem_index->GetHnswIndex();
            }
            memory_hnsw_index->InsertVecs(block_offset, col, offset, row_cnt);
            break;
        }
        case IndexType::kBMP: {
            SharedPtr<BMPIndexInMem> memory_bmp_index;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                memory_bmp_index = MakeShared<BMPIndexInMem>(base_row_id, index_base.get(), column_def.get());
                mem_index->SetBMPIndex(memory_bmp_index);
            } else {
                memory_bmp_index = mem_index->GetBMPIndex();
            }
            memory_bmp_index->AddDocs(block_offset, col, offset, row_cnt);
            break;
        }
        case IndexType::kEMVB: {
            SharedPtr<EMVBIndexInMem> memory_emvb_index;
            if (is_null) {
                auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                memory_emvb_index = EMVBIndexInMem::NewEMVBIndexInMem(index_base, column_def, base_row_id);
                TableMeeta &table_meta = segment_index_meta.table_index_meta().table_meta();
                memory_emvb_index->SetSegmentID(table_meta.db_id_str(), table_meta.table_id_str(), segment_index_meta.segment_id());
                mem_index->SetEMVBIndex(memory_emvb_index);
            } else {
                memory_emvb_index = mem_index->GetEMVBIndex();
            }
            memory_emvb_index->Insert(col, offset, row_cnt, segment_index_meta.kv_instance(), begin_ts);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    // Trigger dump if necessary
    SizeT row_count = mem_index->GetRowCount();
    SizeT row_quota = InfinityContext::instance().config()->MemIndexMemoryQuota();
    if (row_count >= row_quota) {
        TableMeeta &table_meta = segment_index_meta.table_index_meta().table_meta();
        auto [db_name, table_name] = table_meta.GetDBTableName();
        auto [index_base, _] = segment_index_meta.table_index_meta().GetIndexBase();
        String index_name = *index_base->index_name_;
        SegmentID segment_id = segment_index_meta.segment_id();
        RowID begin_row_id = mem_index->GetBeginRowID();
        SharedPtr<DumpMemIndexTask> dump_task = MakeShared<DumpMemIndexTask>(db_name, table_name, index_name, segment_id, begin_row_id);
        DumpIndexProcessor *dump_index_processor = InfinityContext::instance().storage()->dump_index_processor();
        LOG_INFO(fmt::format("MemIndex row count {} exceeds quota {}.  Submit dump task: {}", row_count, row_quota, dump_task->ToString()));
        dump_index_processor->Submit(std::move(dump_task));
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
                             WalCmdCreateIndexV2 *create_index_cmd_ptr) {
    // PopulateIndex is used in create index / import and compact
    Optional<SegmentIndexMeta> segment_index_meta;
    {
        Status status = NewCatalog::AddNewSegmentIndex1(table_index_meta, this, segment_meta.segment_id(), segment_index_meta);
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
        auto [old_chunk_ids_ptr, status] = segment_index_meta->GetChunkIDs1();
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
            case IndexType::kDiskAnn: { // TODO
                LOG_WARN("Not implemented yet");
                return Status::OK();
            }
            default: {
                UnrecoverableError("Invalid index type");
                return Status::OK();
            }
        }
        {
            Status status = segment_index_meta->GetNextChunkID(new_chunk_id);
            if (!status.ok()) {
                return status;
            }
            status = segment_index_meta->SetNextChunkID(new_chunk_id + 1);
            if (!status.ok()) {
                return status;
            }

            status = DumpSegmentMemIndex(*segment_index_meta, new_chunk_id);
            if (!status.ok()) {
                return status;
            }
            if (new_chunk_id == static_cast<ChunkID>(-1) && segment_row_cnt > 0) {
                UnrecoverableError(fmt::format("Failed to dump {} rows", segment_row_cnt));
            }
        }
    }

    ChunkIndexMeta chunk_index_meta(new_chunk_id, *segment_index_meta);
    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(chunk_index_meta);

    // Put the index info local txn store
    switch (dump_index_cause) {
        case DumpIndexCause::kCompact: {
            CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store_.get());
            compact_txn_store->chunk_infos_in_segments_.emplace(segment_meta.segment_id(), chunk_infos);
            compact_txn_store->deprecate_ids_in_segments_.emplace(segment_meta.segment_id(), old_chunk_ids);
            break;
        }
        case DumpIndexCause::kImport: {
            ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store_.get());
            import_txn_store->chunk_infos_in_segments_.emplace(segment_meta.segment_id(), chunk_infos);
            import_txn_store->deprecate_ids_in_segments_.emplace(segment_meta.segment_id(), old_chunk_ids);
        }
        default: {
        }
    }

    if (create_index_cmd_ptr) {
        WalCmdCreateIndexV2 &create_index_cmd = *create_index_cmd_ptr;
        create_index_cmd.segment_index_infos_.emplace_back(segment_meta.segment_id(), std::move(chunk_infos));
    }

    return Status::OK();
}

Status NewTxn::ReplayDumpIndex(WalCmdDumpIndexV2 *dump_index_cmd) {
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

    SegmentID segment_id = dump_index_cmd->segment_id_;
    Optional<SegmentIndexMeta> segment_index_meta_opt;
    {
        auto [segment_ids_ptr, status] = table_index_meta->GetSegmentIndexIDs1();
        if (!status.ok()) {
            return status;
        }
        auto iter = std::find(segment_ids_ptr->begin(), segment_ids_ptr->end(), segment_id);
        if (iter == segment_ids_ptr->end()) {
            status = NewCatalog::AddNewSegmentIndex1(*table_index_meta, this, segment_id, segment_index_meta_opt);
            if (!status.ok()) {
                return status;
            }
        } else {
            segment_index_meta_opt.emplace(segment_id, *table_index_meta);
        }
    }
    SegmentIndexMeta &segment_index_meta = *segment_index_meta_opt;

    Vector<ChunkID> chunk_ids_to_delete;
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    {
        HashSet<ChunkID> deprecate_chunk_ids(dump_index_cmd->deprecate_ids_.begin(), dump_index_cmd->deprecate_ids_.end());
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        if (!status.ok()) {
            return status;
        }
        for (ChunkID chunk_id : *chunk_ids_ptr) {
            if (deprecate_chunk_ids.contains(chunk_id)) {
                chunk_ids_to_delete.push_back(chunk_id);
            }
        }
    }

    // Remove old ones;
    status = segment_index_meta.RemoveChunkIDs(chunk_ids_to_delete);
    if (!status.ok()) {
        return status;
    }

    bool dump_success = true;
    for (const WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
        ChunkID chunk_id = chunk_info.chunk_id_;
        if (std::find(chunk_ids_ptr->begin(), chunk_ids_ptr->end(), chunk_id) == chunk_ids_ptr->end()) {

            String drop_ts{};
            kv_instance_->Get(
                KeyEncode::DropChunkIndexKey(dump_index_cmd->db_id_, dump_index_cmd->table_id_, dump_index_cmd->index_id_, segment_id, chunk_id),
                drop_ts);

            if (drop_ts.empty()) {
                dump_success = false;
                break;
            }
        }
    }

    if (dump_success) {
        for (const WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
            status = NewCatalog::LoadFlushedChunkIndex1(segment_index_meta, chunk_info, this);
            if (!status.ok()) {
                return status;
            }
        }

        SharedPtr<MemIndex> mem_index = segment_index_meta.PopMemIndex();
        if (mem_index != nullptr) {
            mem_index->ClearMemIndex();
        }
    } else {
        u32 dump_row_count = 0;
        for (const WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
            dump_row_count += chunk_info.row_count_;
        }

        SizeT mem_index_row_count = 0;
        bool valid_mem_index = false;
        if (segment_index_meta.HasMemIndex()) {
            SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            mem_index_row_count = mem_index->GetRowCount();

            if (mem_index_row_count == dump_row_count) {
                valid_mem_index = true;
            } else {
                LOG_DEBUG(fmt::format("Dump row count {} is not equal to row count in mem index {}, clear the mem index",
                                      mem_index_row_count,
                                      dump_row_count));
                mem_index->ClearMemIndex();
            }
        }

        if (!valid_mem_index) {
            Vector<Pair<RowID, u64>> append_ranges;
            SegmentMeta segment_meta(segment_id, *table_meta);
            status = CountMemIndexGapInSegment(segment_index_meta, segment_meta, append_ranges);
            if (!status.ok()) {
                return status;
            }

            u32 append_ranges_row_count = 0;
            for (const auto &range : append_ranges) {
                append_ranges_row_count += range.second;
            }

            if (dump_row_count != append_ranges_row_count) {
                UnrecoverableError(
                    fmt::format("Dump row count {} is not equal to row count of append ranges {}", dump_row_count, append_ranges_row_count));
            }

            for (const auto &range : append_ranges) {
                status = this->AppendIndex(*table_index_meta, range);
                if (!status.ok()) {
                    return status;
                }
            }
        }
        CommitBottomDumpMemIndex(dump_index_cmd);
    }

    ChunkID next_chunk_id = 0;
    status = segment_index_meta.GetNextChunkID(next_chunk_id);
    if (!status.ok()) {
        return status;
    }

    ChunkID max_chunk_id = 0;
    for (const WalChunkIndexInfo &chunk_info : dump_index_cmd->chunk_infos_) {
        if (chunk_info.chunk_id_ > max_chunk_id) {
            max_chunk_id = chunk_info.chunk_id_;
        }

        if (next_chunk_id <= max_chunk_id) {
            status = segment_index_meta.SetNextChunkID(max_chunk_id + 1);
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::ReplayDumpIndex(WalCmdDumpIndexV2 *dump_cmd, TxnTimeStamp commit_ts, i64 txn_id) { return Status::OK(); }

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
        status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorMode::kReadOnly, col);
        if (!status.ok()) {
            return status;
        }
        u32 offset = 0;
        status = this->AppendMemIndex(segment_index_meta, block_id, col, offset, row_cnt);
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
    Status status;
    SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();

    RowID base_row_id(segment_index_meta.segment_id(), 0);
    String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
    String full_path;
    {
        SharedPtr<String> index_dir = segment_index_meta.GetSegmentIndexDir();
        full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
    }
    SharedPtr<MemoryIndexer> memory_indexer =
        MakeShared<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_);
    mem_index->SetFulltextIndex(memory_indexer);

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
        status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorMode::kReadOnly, col);
        if (!status.ok()) {
            return status;
        }
        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id();
        RowID begin_row_id(segment_meta.segment_id(), block_offset);

        RowID exp_begin_row_id = memory_indexer->GetBeginRowID() + memory_indexer->GetDocCount();
        assert(begin_row_id >= exp_begin_row_id);
        if (begin_row_id > exp_begin_row_id) {
            LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                 begin_row_id.ToUint64(),
                                 exp_begin_row_id.ToUint64(),
                                 begin_row_id - exp_begin_row_id));
            memory_indexer->InsertGap(begin_row_id - exp_begin_row_id);
        }

        auto col_ptr = MakeShared<ColumnVector>(std::move(col));
        memory_indexer->Insert(col_ptr, 0, row_cnt, false /*offline*/);
        memory_indexer->Commit(false /*offline*/);
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
        Status status = NewCatalog::AddNewChunkIndex1(segment_index_meta,
                                                      this,
                                                      chunk_id,
                                                      base_row_id,
                                                      row_count,
                                                      "" /*base_name*/,
                                                      0 /*index_size*/,
                                                      chunk_index_meta);
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
        Status status = NewCatalog::AddNewChunkIndex1(segment_index_meta,
                                                      this,
                                                      chunk_id,
                                                      base_row_id,
                                                      row_count,
                                                      "" /*base_name*/,
                                                      0 /*index_size*/,
                                                      chunk_index_meta);
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
        auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
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

    SharedPtr<String> index_dir = segment_index_meta.GetSegmentIndexDir();
    ColumnIndexMerger column_index_merger(*index_dir, index_fulltext->flag_);
    column_index_merger.Merge(base_names, base_rowids, dst_base_name);
    {
        base_rowid_out = base_rowid;
        row_cnt_out = total_row_count;
        base_name_out = dst_base_name;
    }

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

        UniquePtr<HnswIndexInMem> memory_hnsw_index = HnswIndexInMem::Make(base_rowid, index_base.get(), column_def.get());
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
            status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorMode::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
            u32 offset = 0;
            memory_hnsw_index->InsertVecs(base_rowid.segment_offset_, col, offset, row_cnt);
        }

        memory_hnsw_index->Dump(buffer_obj);
    } else if (index_base->index_type_ == IndexType::kBMP) {
        auto memory_bmp_index = MakeShared<BMPIndexInMem>(base_rowid, index_base.get(), column_def.get());

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
            status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorMode::kReadOnly, col);
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

Status NewTxn::OptimizeSegmentIndexByParams(SegmentIndexMeta &segment_index_meta, const Vector<UniquePtr<InitParameter>> &raw_params) {
    Status status;
    SharedPtr<IndexBase> index_base;

    std::tie(index_base, status) = segment_index_meta.table_index_meta().GetIndexBase();
    if (!status.ok()) {
        return status;
    }
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();

    switch (index_base->index_type_) {
        case IndexType::kBMP: {
            Optional<BMPOptimizeOptions> ret = BMPUtil::ParseBMPOptimizeOptions(raw_params);
            if (!ret) {
                break;
            }
            const auto &options = ret.value();

#ifdef INDEX_HANDLER
#else
            auto optimize_index = [&](const AbstractBMP &index) {
                std::visit(
                    [&](auto &&index) {
                        using T = std::decay_t<decltype(index)>;
                        if constexpr (std::is_same_v<T, std::nullptr_t>) {
                            UnrecoverableError("Invalid index type.");
                        } else {
                            using IndexT = typename std::remove_pointer_t<T>;
                            if constexpr (IndexT::kOwnMem) {
                                index->Optimize(options);
                            } else {
                                UnrecoverableError("Invalid index type.");
                            }
                        }
                    },
                    index);
            };
#endif
            for (ChunkID chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                BufferObj *index_buffer = nullptr;
                status = chunk_index_meta.GetIndexBuffer(index_buffer);
                if (!status.ok()) {
                    return status;
                }
                BufferHandle buffer_handle = index_buffer->Load();
#ifdef INDEX_HANDLER
                BMPHandlerPtr bmp_handler = *static_cast<BMPHandlerPtr *>(buffer_handle.GetDataMut());
                bmp_handler->Optimize(options);
#else
                auto *abstract_bmp = static_cast<AbstractBMP *>(buffer_handle.GetDataMut());
                optimize_index(*abstract_bmp);
#endif
            }
            SharedPtr<BMPIndexInMem> bmp_index = mem_index->GetBMPIndex();
            if (bmp_index) {
#ifdef INDEX_HANDLER
                BMPHandlerPtr bmp_handler = bmp_index->get();
                bmp_handler->Optimize(options);
#else
                optimize_index(mem_index->GetBMPIndex()->get());
#endif
            }

            break;
        }
        case IndexType::kHnsw: {
            auto params = HnswUtil::ParseOptimizeOptions(raw_params);
            if (!params) {
                break;
            }
#ifdef INDEX_HANDLER
#else
            auto optimize_index = [&](AbstractHnsw *abstract_hnsw) {
                std::visit(
                    [&](auto &&index) {
                        using T = std::decay_t<decltype(index)>;
                        if constexpr (std::is_same_v<T, std::nullptr_t>) {
                            UnrecoverableError("Invalid index type.");
                        } else {
                            using IndexT = typename std::remove_pointer_t<T>;
                            if constexpr (IndexT::kOwnMem) {
                                using HnswIndexDataType = typename std::remove_pointer_t<T>::DataType;
                                if (params->compress_to_lvq) {
                                    if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
                                        UnrecoverableError("Invalid index type.");
                                    } else {
                                        auto *p = std::move(*index).CompressToLVQ().release();
                                        delete index;
                                        *abstract_hnsw = p;
                                    }
                                }
                                if (params->lvq_avg) {
                                    index->Optimize();
                                }
                            } else {
                                UnrecoverableError("Invalid index type.");
                            }
                        }
                    },
                    *abstract_hnsw);
            };
#endif
            for (ChunkID chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                BufferObj *index_buffer = nullptr;
                status = chunk_index_meta.GetIndexBuffer(index_buffer);
                if (!status.ok()) {
                    return status;
                }
                BufferHandle buffer_handle = index_buffer->Load();
#ifdef INDEX_HANDLER
                HnswHandlerPtr hnsw_handler = *static_cast<HnswHandlerPtr *>(buffer_handle.GetDataMut());
                if (params->compress_to_lvq) {
                    hnsw_handler->CompressToLVQ();
                }
                if (params->lvq_avg) {
                    hnsw_handler->Optimize();
                }
#else
                auto *abstract_hnsw = static_cast<AbstractHnsw *>(buffer_handle.GetDataMut());
                optimize_index(abstract_hnsw);
#endif
            }
            if (mem_index) {
                SharedPtr<HnswIndexInMem> memory_hnsw_index = mem_index->GetHnswIndex();
                if (memory_hnsw_index) {
#ifdef INDEX_HANDLER
                    HnswHandlerPtr hnsw_handler = memory_hnsw_index->get();
                    if (params->compress_to_lvq) {
                        hnsw_handler->CompressToLVQ();
                    }
                    if (params->lvq_avg) {
                        hnsw_handler->Optimize();
                    }
#else
                    optimize_index(memory_hnsw_index->get_ptr());
#endif
                }
            }
            break;
        }
        default: {
            return Status::OK();
        }
    }

    return Status::OK();
}

Status NewTxn::ReplayOptimizeIndeByParams(WalCmdOptimizeV2 *optimize_cmd) {
    return OptimizeIndexByParams(optimize_cmd->db_name_, optimize_cmd->table_name_, optimize_cmd->index_name_, std::move(optimize_cmd->params_));
}

Status NewTxn::ReplayOptimize(WalCmdOptimizeV2 *optimize_cmd, TxnTimeStamp commit_ts, i64 txn_id) { return Status::OK(); }

Status NewTxn::DumpSegmentMemIndex(SegmentIndexMeta &segment_index_meta, const ChunkID &new_chunk_id) {
    SharedPtr<MemIndex> mem_index = segment_index_meta.PopMemIndex();
    if (mem_index == nullptr || (mem_index->GetBaseMemIndex() == nullptr && mem_index->GetEMVBIndex() == nullptr)) {
        UnrecoverableError("Invalid mem index");
    }
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    auto [index_base, index_status] = table_index_meta.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }

    SharedPtr<SecondaryIndexInMem> memory_secondary_index = nullptr;
    SharedPtr<MemoryIndexer> memory_indexer = nullptr;
    SharedPtr<IVFIndexInMem> memory_ivf_index = nullptr;
    SharedPtr<HnswIndexInMem> memory_hnsw_index = nullptr;
    SharedPtr<BMPIndexInMem> memory_bmp_index = nullptr;
    SharedPtr<EMVBIndexInMem> memory_emvb_index = nullptr;

    // dump mem index only happens in parallel with read, not write, so no lock is needed.
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            memory_secondary_index = mem_index->GetSecondaryIndex();
            if (memory_secondary_index == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kFullText: {
            memory_indexer = mem_index->GetFulltextIndex();
            if (memory_indexer == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kIVF: {
            memory_ivf_index = mem_index->GetIVFIndex();
            if (memory_ivf_index == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kHnsw: {
            memory_hnsw_index = mem_index->GetHnswIndex();
            if (memory_hnsw_index == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kBMP: {
            memory_bmp_index = mem_index->GetBMPIndex();
            if (memory_bmp_index == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kEMVB: {
            memory_emvb_index = mem_index->GetEMVBIndex();
            if (memory_emvb_index == nullptr) {
                return Status::OK();
            }
            break;
        }
        case IndexType::kDiskAnn: {
            LOG_WARN("Not implemented yet");
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    ChunkIndexMetaInfo chunk_index_meta_info;
    if (mem_index->GetBaseMemIndex() != nullptr) {
        chunk_index_meta_info = mem_index->GetBaseMemIndex()->GetChunkIndexMetaInfo();
    } else if (mem_index->GetEMVBIndex() != nullptr) {
        chunk_index_meta_info = mem_index->GetEMVBIndex()->GetChunkIndexMetaInfo();
    } else {
        UnrecoverableError("Invalid mem index");
    }

    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = NewCatalog::AddNewChunkIndex1(segment_index_meta,
                                                      this,
                                                      new_chunk_id,
                                                      chunk_index_meta_info.base_row_id_,
                                                      chunk_index_meta_info.row_cnt_,
                                                      chunk_index_meta_info.base_name_,
                                                      chunk_index_meta_info.index_size_,
                                                      chunk_index_meta);
        if (!status.ok()) {
            return status;
        }

        chunk_infos_.push_back(ChunkInfoForCreateIndex{table_index_meta.table_meta().db_id_str(),
                                                       table_index_meta.table_meta().table_id_str(),
                                                       segment_index_meta.segment_id(),
                                                       new_chunk_id});

        status = chunk_index_meta->GetIndexBuffer(buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            memory_secondary_index->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        case IndexType::kFullText: {
            memory_indexer->Dump(false /*offline*/, false /*spill*/);
            u64 len_sum = memory_indexer->GetColumnLengthSum();
            u32 len_cnt = memory_indexer->GetDocCount();
            Status status = segment_index_meta.UpdateFtInfo(len_sum, len_cnt);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case IndexType::kIVF: {
            memory_ivf_index->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        case IndexType::kHnsw: {
            memory_hnsw_index->Dump(buffer_obj);
            buffer_obj->Save();
            if (buffer_obj->type() != BufferType::kMmap) {
                buffer_obj->ToMmap();
            }
            break;
        }
        case IndexType::kBMP: {
            memory_bmp_index->Dump(buffer_obj);
            buffer_obj->Save();
            if (buffer_obj->type() != BufferType::kMmap) {
                buffer_obj->ToMmap();
            }
            break;
        }
        case IndexType::kEMVB: {
            memory_emvb_index->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    mem_index->ClearMemIndex();
    auto *storage = InfinityContext::instance().storage();
    if (storage != nullptr) {
        auto *memindex_tracer = storage->memindex_tracer();
        if (memindex_tracer != nullptr) {
            memindex_tracer->DumpDone(mem_index);
        }
    }
    return Status::OK();
}

Status NewTxn::CountMemIndexGapInSegment(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, Vector<Pair<RowID, u64>> &append_ranges) {
    Status status;
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
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
        if (chunk_index_meta_info.base_row_id_ != start_row_id) {
            UnrecoverableError(fmt::format("Chunk index row alignment error: Expected {} but got {}",
                                           chunk_index_meta_info.base_row_id_.segment_id_,
                                           start_row_id.segment_id_));
        }
        start_row_id = chunk_index_meta_info.base_row_id_ + chunk_index_meta_info.row_cnt_;
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
        SizeT i = start_block_id;
        if (i >= block_ids.size()) {
            return Status::OK();
        }
        for (; i < block_ids.size(); ++i) {
            BlockID block_id = block_ids[i];
            BlockMeta block_meta(block_id, segment_meta);
            SizeT block_row_cnt = 0;
            // std::tie(block_row_cnt, status) = block_meta.GetRowCnt();
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            if (!status.ok() || block_row_cnt == block_offset) {
                return status;
            }
            append_ranges.emplace_back(RowID(segment_id, (u32(block_id) << BLOCK_OFFSET_SHIFT) + block_offset), block_row_cnt - block_offset);
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
    std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }

    Vector<Pair<RowID, u64>> append_ranges;
    HashSet<SegmentID> index_segment_ids_set(index_segment_ids_ptr->begin(), index_segment_ids_ptr->end());
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        if (!index_segment_ids_set.contains(segment_id)) {
            //            SharedPtr<String> error_msg = MakeShared<String>(fmt::format("Segment {} not in index {}", segment_id,
            //            table_index_meta.index_id_str())); LOG_WARN(*error_msg); UnrecoverableError(*error_msg);//
            Optional<SegmentIndexMeta> segment_index_meta;
            status = NewCatalog::AddNewSegmentIndex1(table_index_meta, this, segment_id, segment_index_meta);
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
    for (const auto &range : append_ranges) {
        status = this->AppendIndex(table_index_meta, range);
        if (!status.ok()) {
            return status;
        }
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
    std::tie(index_segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *index_segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);

        SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
        if (mem_index) {
            SharedPtr<MemoryIndexer> memory_indexer = mem_index->GetFulltextIndex();
            if (memory_indexer) {
                memory_indexer->Commit();
            }
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
        // Add cache if not exist
        if (status.code() == ErrorCode::kCatalogError) {
            ft_index_cache = MakeShared<TableIndexReaderCache>(table_meta->db_id_str(), table_meta->table_id_str());
            status = table_meta->AddFtIndexCache(ft_index_cache);
            if (!status.ok()) {
                return status;
            }
        } else {
            return status;
        }
    }
    index_reader = ft_index_cache->GetIndexReader(this);
    return Status::OK();
}

Status NewTxn::PrepareCommitCreateIndex(WalCmdCreateIndexV2 *create_index_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    String db_name = create_index_cmd->db_name_;
    String table_name = create_index_cmd->table_name_;
    String index_name = *create_index_cmd->index_base_->index_name_;
    String db_id_str = create_index_cmd->db_id_;
    String table_id_str = create_index_cmd->table_id_;
    String table_key = create_index_cmd->table_key_;
    String index_id_str = create_index_cmd->index_id_;

    TableMeeta table_meta(db_id_str, table_id_str, this);
    Optional<TableIndexMeeta> table_index_meta_opt;
    Status status = new_catalog_->AddNewTableIndex(table_meta, index_id_str, commit_ts, create_index_cmd->index_base_, table_index_meta_opt);
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
        WalCmdDumpIndexV2 dump_index_cmd(db_name, db_id_str, table_name, table_id_str, index_name, index_id_str, 0, table_key);
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
        status = table_meta.AddFtIndexCache(ft_cache);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kCatalogError) {
                return status;
            }
        }

        table_index_meta.UpdateFulltextSegmentTS(commit_ts);
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitDropIndex(const WalCmdDropIndexV2 *drop_index_cmd) {
    const String &db_id_str = drop_index_cmd->db_id_;
    const String &table_id_str = drop_index_cmd->table_id_;
    const String &index_id_str = drop_index_cmd->index_id_;
    const String &index_key = drop_index_cmd->index_key_;
    const TxnTimeStamp create_ts = infinity::GetTimestampFromKey(index_key);

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    auto ts_str = std::to_string(commit_ts);
    kv_instance_->Put(KeyEncode::DropTableIndexKey(db_id_str, table_id_str, drop_index_cmd->index_name_, create_ts, index_id_str), ts_str);

    TableMeeta table_meta(db_id_str, table_id_str, this);
    TableIndexMeeta table_index_meta(index_id_str, table_meta);
    SharedPtr<IndexBase> index_base;
    Status status;
    std::tie(index_base, status) = table_index_meta.GetIndexBase();
    if (!status.ok()) {
        return status;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        table_index_meta.UpdateFulltextSegmentTS(commit_ts);
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitDumpIndex(const WalCmdDumpIndexV2 *dump_index_cmd, KVInstance *kv_instance) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &db_id_str = dump_index_cmd->db_id_;
    const String &table_id_str = dump_index_cmd->table_id_;
    const String &index_id_str = dump_index_cmd->index_id_;
    SegmentID segment_id = dump_index_cmd->segment_id_;

    TableMeeta table_meta(db_id_str, table_id_str, this);

    const String &index_id_str_ = dump_index_cmd->index_id_;
    TableIndexMeeta table_index_meta(index_id_str_, table_meta);

    auto [index_base, status] = table_index_meta.GetIndexBase();
    if (!status.ok()) {
        return status;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        table_index_meta.UpdateFulltextSegmentTS(commit_ts);
    }

    for (ChunkID deprecate_id : dump_index_cmd->deprecate_ids_) {

        auto ts_str = std::to_string(commit_ts);
        kv_instance_->Put(KeyEncode::DropChunkIndexKey(db_id_str, table_id_str, index_id_str, segment_id, deprecate_id), ts_str);
    }
    return Status::OK();
}

} // namespace infinity

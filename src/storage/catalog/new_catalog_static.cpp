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
import block_version;
import infinity_exception;
import table_def;
import kv_code;
import kv_store;
import column_vector;
import wal_entry;
import infinity_context;
import logger;
import default_values;
import new_txn;
import data_access_state;
import roaring_bitmap;

import catalog_meta;
import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import mem_index;
import scalar_function_set;
import special_function;

import logical_type;
import data_type;

namespace infinity {

// namespace {

// String IndexFileName(ChunkID chunk_id) { return fmt::format("chunk_{}.idx", chunk_id); }

// } // namespace

void NewTxnGetVisibleRangeState::Init(SharedPtr<BlockLock> block_lock,
                                      BufferHandle version_buffer_handle,
                                      TxnTimeStamp begin_ts,
                                      TxnTimeStamp commit_ts) {
    block_lock_ = std::move(block_lock);
    version_buffer_handle_ = std::move(version_buffer_handle);
    begin_ts_ = begin_ts;
    commit_ts_ = commit_ts;
    {
        std::shared_lock<std::shared_mutex> lock(block_lock_->mtx_);
        const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());
        block_offset_end_ = block_version->GetRowCount(begin_ts_);
    }
}

bool NewTxnGetVisibleRangeState::Next(BlockOffset block_offset_begin, Pair<BlockOffset, BlockOffset> &visible_range) {
    if (end_) {
        return false;
    }

    const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());

    if (block_offset_begin == block_offset_end_) {
        auto [offset, commit_cnt] = block_version->GetCommitRowCount(commit_ts_);
        visible_range = {offset, offset + commit_cnt};
        end_ = true;
        return commit_cnt;
    }

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

Optional<BlockOffset> NewTxnBlockVisitor::Next() {
    if (end_) {
        return None;
    }
    while (cur_ >= visible_range_.second) {
        // get next range (without delete) in [0, block_offset_end_) & commit_ts block
        bool has_next = visit_state_->Next(visible_range_.second, visible_range_);
        if (!has_next) {
            end_ = true;
            return None;
        }
        cur_ = visible_range_.first;
    }
    return cur_++;
}

Status NewCatalog::InitCatalog(KVInstance *kv_instance, TxnTimeStamp checkpoint_ts) {
    Status status;

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(kv_instance);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    auto InitBlockColumn = [&](ColumnMeta &column_meta) { return column_meta.LoadSet(); };
    auto InitBlock = [&](BlockMeta &block_meta) {
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        for (SizeT column_idx = 0; column_idx < column_defs_ptr->size(); ++column_idx) {
            ColumnMeta column_meta(column_idx, block_meta);
            status = InitBlockColumn(column_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return block_meta.LoadSet(checkpoint_ts);
    };
    auto InitSegment = [&](SegmentMeta &segment_meta) {
        auto [block_ids, blocks_status] = segment_meta.GetBlockIDs1(kv_instance, checkpoint_ts, UNCOMMIT_TS);
        if (!blocks_status.ok()) {
            return blocks_status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            blocks_status = InitBlock(block_meta);
            if (!blocks_status.ok()) {
                return status;
            }
        }

        return Status::OK();
    };
    auto InitChunkIndex = [&](ChunkID chunk_id, SegmentIndexMeta &segment_index_meta) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        Status status = chunk_index_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }
        return Status::OK();
    };
    auto InitSegmentIndex = [&](SegmentID segment_id, TableIndexMeeta &table_index_meta) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        status = segment_index_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }

        auto [chunk_ids_ptr, chunk_status] = segment_index_meta.GetChunkIDs1();
        if (!chunk_status.ok()) {
            return status;
        }
        for (ChunkID chunk_id : *chunk_ids_ptr) {
            status = InitChunkIndex(chunk_id, segment_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto InitTableIndex = [&](const String &index_id_str, TableMeeta &table_meta) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta);

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            status = InitSegmentIndex(segment_id, table_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto InitTable = [&](const String &table_id_str, DBMeeta &db_meta) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, kv_instance, checkpoint_ts, MAX_TIMESTAMP);

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta);
            status = InitSegment(segment_meta);
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
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            status = InitTableIndex(index_id_str, table_meta);
            if (!status.ok()) {
                return status;
            }
        }

        status = table_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }

        return Status::OK();
    };
    auto InitDB = [&](const String &db_id_str) {
        DBMeeta db_meta(db_id_str, kv_instance);

        Vector<String> *table_id_strs_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &table_id_str : *table_id_strs_ptr) {
            status = InitTable(table_id_str, db_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    for (const String &db_id_str : *db_id_strs_ptr) {
        status = InitDB(db_id_str);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewCatalog::MemIndexRecover(NewTxn *txn) {
    Status status;
    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(txn);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    auto IndexRecoverTable = [&](TableMeeta &table_meta) {
        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &index_id_str : *index_id_strs_ptr) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            status = txn->RecoverMemIndex(table_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto IndexRecoverDB = [&](DBMeeta &db_meta) {
        Vector<String> *table_id_strs_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &table_id_str : *table_id_strs_ptr) {
            TableMeeta table_meta(db_meta.db_id_str(), table_id_str, txn);
            status = IndexRecoverTable(table_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, txn);
        status = IndexRecoverDB(db_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::MemIndexCommit(NewTxn *new_txn) {
    Status status;
    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(new_txn);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    auto IndexCommitTable = [&](TableMeeta &table_meta) {
        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &index_id_str : *index_id_strs_ptr) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            status = new_txn->CommitMemIndex(table_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto IndexCommitDB = [&](DBMeeta &db_meta) {
        Vector<String> *table_id_strs_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &table_id_str : *table_id_strs_ptr) {
            TableMeeta table_meta(db_meta.db_id_str(), table_id_str, new_txn);
            status = IndexCommitTable(table_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, new_txn);
        status = IndexCommitDB(db_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetAllMemIndexes(NewTxn *txn, Vector<SharedPtr<MemIndex>> &mem_indexes, Vector<MemIndexID> &mem_index_ids) {
    auto TraverseTableIndex = [&](TableIndexMeeta &table_index_meta, const String &db_name, const String &table_name, const String &index_name) {
        auto [index_segment_ids_ptr, status] = table_index_meta.GetSegmentIndexIDs1();
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *index_segment_ids_ptr) {
            SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
            SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            if (mem_index == nullptr) {
                continue;
            }
            mem_indexes.push_back(mem_index);
            mem_index_ids.push_back(MemIndexID{db_name, table_name, index_name, segment_id});
        }
        return Status::OK();
    };
    auto TraverseTable = [&](TableMeeta &table_meta, const String &db_name, const String &table_name) {
        Vector<String> *index_id_strs_ptr = nullptr;
        Vector<String> *index_names_ptr = nullptr;
        Status status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        if (!status.ok()) {
            return status;
        }
        for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
            const String &index_id_str = (*index_id_strs_ptr)[i];
            const String &index_name = (*index_names_ptr)[i];
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            status = TraverseTableIndex(table_index_meta, db_name, table_name, index_name);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto TraverseDB = [&](DBMeeta &db_meta, const String &db_name) {
        Vector<String> *table_id_strs_ptr = nullptr;
        Vector<String> *table_names_ptr = nullptr;
        Status status = db_meta.GetTableIDs(table_id_strs_ptr, &table_names_ptr);
        if (!status.ok()) {
            return status;
        }
        for (SizeT i = 0; i < table_id_strs_ptr->size(); ++i) {
            const String &table_id_str = (*table_id_strs_ptr)[i];
            const String &table_name = (*table_names_ptr)[i];
            TableMeeta table_meta(db_meta.db_id_str(), table_id_str, txn);
            status = TraverseTable(table_meta, db_name, table_name);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    Vector<String> *db_id_strs_ptr = nullptr;
    Vector<String> *db_names_ptr = nullptr;
    CatalogMeta catalog_meta(txn);
    Status status = catalog_meta.GetDBIDs(db_id_strs_ptr, &db_names_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SizeT i = 0; i < db_id_strs_ptr->size(); ++i) {
        const String &db_id_str = (*db_id_strs_ptr)[i];
        const String &db_name = (*db_names_ptr)[i];
        DBMeeta db_meta(db_id_str, txn);
        status = TraverseDB(db_meta, db_name);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::AddNewDB(NewTxn *txn,
                            const String &db_id_str,
                            TxnTimeStamp commit_ts,
                            const String &db_name,
                            const String *db_comment,
                            SharedPtr<DBMeeta> &db_meta) {
    KVInstance *kv_instance = txn->kv_instance();

    String db_key = KeyEncode::CatalogDbKey(db_name, commit_ts);
    Status status = kv_instance->Put(db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    db_meta = MakeShared<DBMeeta>(db_id_str, txn);
    status = db_meta->InitSet(db_comment);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::CleanDB(KVInstance *kv_instance, DBMeeta &db_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanDB: cleaning db_id: {}", db_meta.db_id_str()));

    Status status;

    Vector<String> *table_id_strs_ptr = nullptr;
    Vector<String> *table_names_ptr = nullptr;
    status = db_meta.GetTableIDs(table_id_strs_ptr, &table_names_ptr);
    if (!status.ok()) {
        return status;
    }

    for (SizeT i = 0; i < table_id_strs_ptr->size(); ++i) {
        const String &table_id_str = (*table_id_strs_ptr)[i];
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, db_meta.kv_instance(), begin_ts, MAX_TIMESTAMP);
        status = NewCatalog::CleanTable(kv_instance, table_meta, begin_ts, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }

    status = db_meta.UninitSet(usage_flag);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewTable(DBMeeta &db_meta,
                               const String &table_id_str,
                               TxnTimeStamp begin_ts,
                               TxnTimeStamp commit_ts,
                               const SharedPtr<TableDef> &table_def,
                               Optional<TableMeeta> &table_meta) {
    // Create table a key value pair
    KVInstance *kv_instance = db_meta.kv_instance();
    String table_key = KeyEncode::CatalogTableKey(db_meta.db_id_str(), *table_def->table_name(), commit_ts);
    Status status = kv_instance->Put(table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }

    table_meta.emplace(db_meta.db_id_str(), table_id_str, kv_instance, begin_ts, commit_ts);
    status = table_meta->InitSet(table_def);
    if (!status.ok()) {
        return status;
    }

    return status;
}

Status NewCatalog::CleanTable(KVInstance *kv_instance, TableMeeta &table_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanTable: cleaning table_id: {}", table_meta.table_id_str()));

    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        status = NewCatalog::CleanSegment(kv_instance, segment_meta, begin_ts, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta);
        status = NewCatalog::CleanTableIndex(table_index_meta, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_meta.UninitSet(usage_flag);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewTableIndex(TableMeeta &table_meta,
                                    String &index_id_str,
                                    TxnTimeStamp commit_ts,
                                    const SharedPtr<IndexBase> &index_base,
                                    Optional<TableIndexMeeta> &table_index_meta) {
    // Create index a key value pair
    KVInstance *kv_instance = table_meta.kv_instance();
    const String &index_name = *index_base->index_name_;
    String index_key = KeyEncode::CatalogIndexKey(table_meta.db_id_str(), table_meta.table_id_str(), index_name, commit_ts);
    Status status = kv_instance->Put(index_key, index_id_str);
    if (!status.ok()) {
        return status;
    }

    table_index_meta.emplace(index_id_str, table_meta);
    status = table_index_meta->InitSet1(index_base, this);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::CleanTableIndex(TableIndexMeeta &table_index_meta, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanTableIndex: cleaning table id: {}, index_id: {}",
                          table_index_meta.table_meta().table_id_str(),
                          table_index_meta.index_id_str()));

    auto [segment_ids_ptr, status] = table_index_meta.GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        status = NewCatalog::CleanSegmentIndex(segment_index_meta, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_index_meta.UninitSet1(usage_flag);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

// Status NewCatalog::AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta) {
//     {
//         Status status = table_meta.AddSegmentID(segment_id);
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     segment_meta.emplace(segment_id, table_meta);
//     {
//         Status status = segment_meta->InitSet();
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     return Status::OK();
// }

Status NewCatalog::AddNewSegment1(TableMeeta &table_meta, KVInstance *kv_instance, TxnTimeStamp commit_ts, Optional<SegmentMeta> &segment_meta) {
    Status status;
    SegmentID segment_id = 0;
    std::tie(segment_id, status) = table_meta.AddSegmentID1(commit_ts);
    if (!status.ok()) {
        return status;
    }
    segment_meta.emplace(segment_id, table_meta);
    return segment_meta->InitSet(kv_instance);
}

Status NewCatalog::AddNewSegmentWithID(TableMeeta &table_meta,
                                       KVInstance *kv_instance,
                                       TxnTimeStamp commit_ts,
                                       Optional<SegmentMeta> &segment_meta,
                                       SegmentID segment_id) {
    Status status = table_meta.AddSegmentWithID(commit_ts, segment_id);
    if (!status.ok()) {
        return status;
    }
    segment_meta.emplace(segment_id, table_meta);
    return segment_meta->InitSet(kv_instance);
}

Status NewCatalog::LoadFlushedSegment2(TableMeeta &table_meta,
                                       const WalSegmentInfo &segment_info,
                                       KVInstance *kv_instance,
                                       TxnTimeStamp begin_ts,
                                       TxnTimeStamp commit_ts) {
    Status status = table_meta.AddSegmentWithID(begin_ts, segment_info.segment_id_);
    if (!status.ok()) {
        return status;
    }

    SegmentMeta segment_meta(segment_info.segment_id_, table_meta);
    status = segment_meta.InitSet(kv_instance);
    if (!status.ok()) {
        return status;
    }
    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        status = NewCatalog::LoadFlushedBlock1(segment_meta, block_info, kv_instance, begin_ts, commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewCatalog::CleanSegment(KVInstance *kv_instance, SegmentMeta &segment_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanSegment: cleaning table: {}, segment_id: {}", segment_meta.table_meta().table_id_str(), segment_meta.segment_id()));
    auto [block_ids, status] = segment_meta.GetBlockIDs1(kv_instance, begin_ts, begin_ts);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids) {
        BlockMeta block_meta(block_id, segment_meta);
        status = NewCatalog::CleanBlock(kv_instance, block_meta, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }
    segment_meta.UninitSet(usage_flag, kv_instance, begin_ts);

    return Status::OK();
}

// Status NewCatalog::AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta) {
//     {
//         Status status = segment_meta.AddBlockID(block_id);
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     block_meta.emplace(block_id, segment_meta);
//     {
//         Status status = block_meta->InitSet();
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     SizeT column_num = 0;
//     {
//         TableMeeta &table_meta = segment_meta.table_meta();
//         auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
//         if (!status.ok()) {
//             return status;
//         }
//         column_num = column_defs_ptr->size();
//     }
//     for (SizeT column_idx = 0; column_idx < column_num; ++column_idx) {
//         [[maybe_unused]] Optional<ColumnMeta> column_meta;
//         Status status = NewCatalog::AddNewBlockColumn(*block_meta, column_idx, column_meta);
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     return Status::OK();
// }

Status NewCatalog::AddNewBlock1(SegmentMeta &segment_meta,
                                KVInstance *kv_instance,
                                TxnTimeStamp begin_ts,
                                TxnTimeStamp commit_ts,
                                Optional<BlockMeta> &block_meta) {
    Status status;

    BlockID block_id;
    std::tie(block_id, status) = segment_meta.AddBlockID1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return status;
    }
    block_meta.emplace(block_id, segment_meta);
    status = block_meta->InitSet();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    for (SizeT column_idx = 0; column_idx < column_defs_ptr->size(); ++column_idx) {
        ColumnMeta column_meta(column_idx, *block_meta);
        status = column_meta.InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::LoadImportedOrCompactedSegment(TableMeeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp commit_ts) {
    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockID block_id = block_info.block_id_;
        SegmentMeta segment_meta(segment_info.segment_id_, table_meta);
        Optional<BlockMeta> block_meta;
        block_meta.emplace(block_id, segment_meta);
        Status status = block_meta->LoadSet(commit_ts);
        if (!status.ok()) {
            return status;
        }

        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        {
            TableMeeta &table_meta = segment_meta.table_meta();
            std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
            if (!status.ok()) {
                return status;
            }
        }
        for (const auto &column_def : *column_defs_ptr) {
            ColumnMeta column_meta(column_def->id(), *block_meta);
            status = column_meta.LoadSet();
            if (!status.ok()) {
                return status;
            }
        }
    }

    // Load segment index
    Vector<String> *index_id_strs_ptr = nullptr;
    Status status = table_meta.GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta);
        SegmentIndexMeta segment_index_meta(segment_info.segment_id_, table_index_meta);
        status = segment_index_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }

        auto [chunk_ids_ptr, chunk_status] = segment_index_meta.GetChunkIDs1();
        if (!chunk_status.ok()) {
            return status;
        }
        for (ChunkID chunk_id : *chunk_ids_ptr) {
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            status = chunk_index_meta.LoadSet();
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewCatalog::AddNewBlockWithID(SegmentMeta &segment_meta,
                                     KVInstance *kv_instance,
                                     TxnTimeStamp commit_ts,
                                     Optional<BlockMeta> &block_meta,
                                     BlockID block_id) {
    Status status = segment_meta.AddBlockWithID(kv_instance, commit_ts, block_id);
    if (!status.ok()) {
        return status;
    }
    block_meta.emplace(block_id, segment_meta);
    status = block_meta->InitSet();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    for (SizeT column_idx = 0; column_idx < column_defs_ptr->size(); ++column_idx) {
        ColumnMeta column_meta(column_idx, *block_meta);
        status = column_meta.InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

//
// Status NewCatalog::AddNewTableIndexForTransform(TableMeeta &table_meta, TxnTimeStamp commit_ts, Optional<TableIndexMeta> &table_index_meta) {
//     Status status;
//     BlockID block_id;
//     std::tie(block_id, status) = table_meta.AddBlockID1(commit_ts);
//     return Status::OK();
// }

Status NewCatalog::LoadFlushedBlock1(SegmentMeta &segment_meta,
                                     const WalBlockInfo &block_info,
                                     KVInstance *kv_instance,
                                     TxnTimeStamp begin_ts,
                                     TxnTimeStamp commit_ts) {
    Status status;

    auto *pm = InfinityContext::instance().persistence_manager();
    if (pm) {
        block_info.addr_serializer_.AddToPersistenceManager(pm);
    }

    BlockID block_id = 0;
    std::tie(block_id, status) = segment_meta.AddBlockID1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return status;
    }
    if (block_id != block_info.block_id_) {
        UnrecoverableError(fmt::format("Block id mismatch, expect: {}, actual: {}", block_info.block_id_, block_id));
    }

    BlockMeta block_meta(block_id, segment_meta);
    status = block_meta.LoadSet(begin_ts);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    for (const auto &column_def : *column_defs_ptr) {
        //    const auto &[chunk_idx, chunk_offset] = block_info.outline_infos_[column_def->id()];
        ColumnMeta column_meta(column_def->id(), block_meta);
        //    status = column_meta.SetChunkOffset(chunk_offset);
        //    if (!status.ok()) {
        //        return status;
        //    }

        status = column_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }
    }
    /*
    status = block_meta.SetRowCnt(block_info.row_count_);
    if (!status.ok()) {
        return status;
    }
    */

    return Status::OK();
}

Status NewCatalog::CleanBlock(KVInstance *kv_instance, BlockMeta &block_meta, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanBlock: cleaning table id: {}, segment_id: {}, block_id: {}",
                          block_meta.segment_meta().table_meta().table_id_str(),
                          block_meta.segment_meta().segment_id(),
                          block_meta.block_id()));
    block_meta.RestoreSet();
    Status status;
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;

    TableMeeta &table_meta = block_meta.segment_meta().table_meta();
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    for (const auto &column_def : *column_defs_ptr) {
        ColumnMeta column_meta(column_def->id(), block_meta);
        status = NewCatalog::CleanBlockColumn(kv_instance, column_meta, column_def.get(), usage_flag);
        if (!status.ok()) {
            return status;
        }
    }
    block_meta.UninitSet(kv_instance, usage_flag);

    return Status::OK();
}

Status NewCatalog::AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta) {
    column_meta.emplace(column_idx, block_meta);
    {
        Status status = column_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanBlockColumn(KVInstance *kv_instance, ColumnMeta &column_meta, const ColumnDef *column_def, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanBlockColumn: cleaning table id: {}, segment_id: {}, block_id: {}, column_id: {}",
                          column_meta.block_meta().segment_meta().table_meta().table_id_str(),
                          column_meta.block_meta().segment_meta().segment_id(),
                          column_meta.block_meta().block_id(),
                          column_def->id()));
    column_meta.RestoreSet(column_def, kv_instance);
    Status status;

    status = column_meta.UninitSet(column_def, kv_instance, usage_flag);
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
        segment_index_meta.emplace(segment_id, table_index_meta);
        Status status = segment_index_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::AddNewSegmentIndex1(TableIndexMeeta &table_index_meta,
                                       NewTxn *new_txn,
                                       SegmentID segment_id,
                                       Optional<SegmentIndexMeta> &segment_index_meta) {
    Status status = table_index_meta.AddSegmentIndexID1(segment_id, new_txn);
    if (!status.ok()) {
        return status;
    }

    segment_index_meta.emplace(segment_id, table_index_meta);
    status = segment_index_meta->InitSet1();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::CleanSegmentIndex(SegmentIndexMeta &segment_index_meta, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanSegmentIndex: cleaning table id: {}, segment_id: {}, index_id: {}",
                          segment_index_meta.table_index_meta().table_meta().table_id_str(),
                          segment_index_meta.segment_id(),
                          segment_index_meta.table_index_meta().index_id_str()));
    if (usage_flag != UsageFlag::kTransform) {
        // Invalidate the fulltext index cache for this segment
        TableMeeta &table_meta = segment_index_meta.table_index_meta().table_meta();
        Status status = table_meta.InvalidateFtIndexCache(segment_index_meta.segment_id());
        if (!status.ok()) {
            return status;
        }
    }

    auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
    if (!status.ok()) {
        return status;
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        status = NewCatalog::CleanChunkIndex(chunk_index_meta, usage_flag);
        if (!status.ok()) {
            return status;
        }
    }

    status = segment_index_meta.UninitSet1(usage_flag);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewChunkIndex1(SegmentIndexMeta &segment_index_meta,
                                     NewTxn *new_txn,
                                     ChunkID chunk_id,
                                     RowID base_row_id,
                                     SizeT row_count,
                                     const String &base_name,
                                     SizeT index_size,
                                     Optional<ChunkIndexMeta> &chunk_index_meta) {
    ChunkIndexMetaInfo chunk_info;
    chunk_info.base_name_ = base_name;
    chunk_info.base_row_id_ = base_row_id;
    chunk_info.row_cnt_ = row_count;
    chunk_info.index_size_ = index_size;
    {
        chunk_index_meta.emplace(chunk_id, segment_index_meta);
        Status status = chunk_index_meta->InitSet(chunk_info);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = segment_index_meta.AddChunkIndexID1(chunk_id, new_txn);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::LoadFlushedChunkIndex1(SegmentIndexMeta &segment_index_meta, const WalChunkIndexInfo &chunk_info, NewTxn *new_txn) {
    Status status;
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
    if (!status.ok()) {
        return status;
    }

    if (std::find(chunk_ids_ptr->begin(), chunk_ids_ptr->end(), chunk_info.chunk_id_) == chunk_ids_ptr->end()) {
        status = segment_index_meta.AddChunkIndexID1(chunk_info.chunk_id_, new_txn);
        if (!status.ok()) {
            return status;
        }
    }

    ChunkIndexMeta chunk_index_meta(chunk_info.chunk_id_, segment_index_meta);
    ChunkIndexMetaInfo chunk_meta_info;
    {
        chunk_meta_info.base_name_ = chunk_info.base_name_;
        chunk_meta_info.base_row_id_ = chunk_info.base_rowid_;
        chunk_meta_info.row_cnt_ = chunk_info.row_count_;
        chunk_meta_info.index_size_ = 0;
    }
    status = chunk_index_meta.SetChunkInfo(chunk_meta_info);
    if (!status.ok()) {
        return status;
    }
    status = chunk_index_meta.LoadSet();
    if (!status.ok()) {
        return status;
    }

    auto *pm = InfinityContext::instance().persistence_manager();
    if (pm) {
        chunk_info.addr_serializer_.AddToPersistenceManager(pm);
    }
    return Status::OK();
}

Status NewCatalog::CleanChunkIndex(ChunkIndexMeta &chunk_index_meta, UsageFlag usage_flag) {
    LOG_TRACE(fmt::format("CleanChunkIndex: cleaning table id: {}, segment_id: {}, index_id: {}, chunk_id: {}",
                          chunk_index_meta.segment_index_meta().table_index_meta().table_meta().table_id_str(),
                          chunk_index_meta.segment_index_meta().segment_id(),
                          chunk_index_meta.segment_index_meta().table_index_meta().index_id_str(),
                          chunk_index_meta.chunk_id()));
    chunk_index_meta.RestoreSet();
    Status status;

    status = chunk_index_meta.UninitSet(usage_flag);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::GetColumnVector(ColumnMeta &column_meta, SizeT row_count, const ColumnVectorMode &tipe, ColumnVector &column_vector) {
    SharedPtr<DataType> column_type{};
    {
        TableMeeta &table_meta = column_meta.block_meta().segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        //        ColumnDef *col_def = nullptr;
        ColumnDef *col_def = (*column_defs_ptr)[column_meta.column_idx()].get();
        column_type = col_def->type();
        //        for (const auto &column_def_ptr : *column_defs_ptr) {
        //            if (column_def_ptr->id() == i64(column_meta.column_idx())) {
        //                column_type = column_def_ptr->type();
        //                break;
        //            }
        //        }
        //        if (column_type == nullptr) {
        //            UnrecoverableError("Null ptr of column type");
        //        }
    }

    BufferObj *buffer_obj = nullptr;
    BufferObj *outline_buffer_obj = nullptr;
    Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    column_vector = ColumnVector(column_type);
    column_vector.Initialize(buffer_obj, outline_buffer_obj, row_count, tipe);
    return Status::OK();
}

Status NewCatalog::GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, NewTxnGetVisibleRangeState &state) {
    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    BufferHandle buffer_handle = version_buffer->Load();
    SharedPtr<BlockLock> block_lock;
    {
        status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
    }
    state.Init(std::move(block_lock), std::move(buffer_handle), begin_ts, commit_ts);
    return Status::OK();
}

Status NewCatalog::GetCreateTSVector(BlockMeta &block_meta, SizeT offset, SizeT size, ColumnVector &column_vector) {
    column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kBigInt));
    column_vector.Initialize(ColumnVectorType::kFlat, size);

    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<BlockLock> block_lock;
    status = block_meta.GetBlockLock(block_lock);
    if (!status.ok()) {
        return status;
    }

    BufferHandle buffer_handle = version_buffer->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(buffer_handle.GetData());
    {
        std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
        block_version->GetCreateTS(offset, size, column_vector);
    }
    return Status::OK();
}

Status NewCatalog::GetDeleteTSVector(BlockMeta &block_meta, SizeT offset, SizeT size, ColumnVector &column_vector) {
    column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kBigInt));
    column_vector.Initialize(ColumnVectorType::kFlat, size);

    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<BlockLock> block_lock;
    status = block_meta.GetBlockLock(block_lock);
    if (!status.ok()) {
        return status;
    }

    BufferHandle buffer_handle = version_buffer->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(buffer_handle.GetData());
    {
        std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
        block_version->GetDeleteTS(offset, size, column_vector);
    }
    return Status::OK();
}

Status
NewCatalog::GetDBFilePaths(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, DBMeeta &db_meta, Vector<String> &file_paths) {
    Vector<String> *table_id_strs_ptr = nullptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, db_meta.kv_instance(), begin_ts, commit_ts);
        status = GetTableFilePaths(kv_instance, begin_ts, commit_ts, table_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetTableFilePaths(KVInstance *kv_instance,
                                     TxnTimeStamp begin_ts,
                                     TxnTimeStamp commit_ts,
                                     TableMeeta &table_meta,
                                     Vector<String> &file_paths,
                                     SharedPtr<ColumnDef> column_def) {
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    Status status;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        status = GetSegmentFilePaths(kv_instance, begin_ts, commit_ts, segment_meta, file_paths, column_def);
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
        TableIndexMeeta table_index_meta(index_id_str, table_meta);
        status = GetTableIndexFilePaths(table_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentFilePaths(KVInstance *kv_instance,
                                       TxnTimeStamp begin_ts,
                                       TxnTimeStamp commit_ts,
                                       SegmentMeta &segment_meta,
                                       Vector<String> &file_paths,
                                       SharedPtr<ColumnDef> column_def) {
    Vector<BlockID> *block_ids_ptr = nullptr;
    Status status;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        status = GetBlockFilePaths(block_meta, file_paths, column_def);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetBlockFilePaths(BlockMeta &block_meta, Vector<String> &file_paths, SharedPtr<ColumnDef> column_def) {
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    Status status;
    std::tie(column_defs_ptr, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    if (column_def == nullptr) {
        for (const auto &column_def_ptr : *column_defs_ptr) {
            ColumnMeta column_meta(column_def_ptr->id(), block_meta);
            status = GetBlockColumnFilePaths(column_meta, file_paths);
            if (!status.ok()) {
                return status;
            }
        }
        Vector<String> paths = block_meta.FilePaths();
        file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    } else {
        ColumnMeta column_meta(column_def->id(), block_meta);
        status = GetBlockColumnFilePaths(column_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetBlockColumnFilePaths(ColumnMeta &column_meta, Vector<String> &file_paths) {
    Status status;
    Vector<String> paths;
    status = column_meta.FilePaths(paths);
    if (!status.ok()) {
        return status;
    }
    file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    return Status::OK();
}

Status NewCatalog::GetColumnFilePaths(KVInstance *kv_instance,
                                      TxnTimeStamp begin_ts,
                                      TxnTimeStamp commit_ts,
                                      TableMeeta &table_meta,
                                      SharedPtr<ColumnDef> column_def,
                                      Vector<String> &file_paths) {
    return NewCatalog::GetTableFilePaths(kv_instance, begin_ts, commit_ts, table_meta, file_paths, column_def);
}

Status NewCatalog::GetTableIndexFilePaths(TableIndexMeeta &table_index_meta, Vector<String> &file_paths) {

    auto [segment_ids_ptr, status] = table_index_meta.GetSegmentIndexIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        status = GetSegmentIndexFilepaths(segment_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentIndexFilepaths(SegmentIndexMeta &segment_index_meta, Vector<String> &file_paths) {
    auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
    if (!status.ok()) {
        return status;
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
        status = GetChunkIndexFilePaths(chunk_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetChunkIndexFilePaths(ChunkIndexMeta &chunk_index_meta, Vector<String> &file_paths) {
    Vector<String> paths;
    Status status = chunk_index_meta.FilePaths(paths);
    if (!status.ok()) {
        return status;
    }
    file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    return Status::OK();
}

Status NewCatalog::CheckColumnIfIndexed(TableMeeta &table_meta, ColumnID column_id, bool &has_index) {
    Vector<String> *index_id_strs_ptr = nullptr;
    Status status = table_meta.GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta);
        SharedPtr<IndexBase> index_base;
        std::tie(index_base, status) = table_index_meta.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        ColumnID column_id1 = 0;
        std::tie(column_id1, status) = table_meta.GetColumnIDByColumnName(index_base->column_name());
        if (!status.ok()) {
            return status;
        }
        if (column_id1 == column_id) {
            has_index = true;
            return Status::OK();
        }
    }
    has_index = false;
    return Status::OK();
}

Status NewCatalog::CheckTableIfDelete(TableMeeta &table_meta, KVInstance *kv_instance, TxnTimeStamp begin_ts, bool &has_delete) {
    Status status;
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        TxnTimeStamp first_delete_ts = 0;
        status = segment_meta.GetFirstDeleteTS(kv_instance, first_delete_ts, begin_ts);
        if (!status.ok()) {
            return status;
        }
        if (first_delete_ts < begin_ts) {
            has_delete = true;
            return Status::OK();
        }
    }
    has_delete = false;
    return Status::OK();
}

Status NewCatalog::SetBlockDeleteBitmask(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Bitmask &bitmask) {
    NewTxnGetVisibleRangeState state;
    Status status = GetBlockVisibleRange(block_meta, begin_ts, commit_ts, state);
    if (!status.ok()) {
        return status;
    }
    Pair<BlockOffset, BlockOffset> range;
    BlockOffset offset = 0;
    while (true) {
        bool has_next = state.Next(offset, range);
        if (!has_next) {
            break;
        }
        for (BlockOffset i = offset; i < range.first; ++i) {
            SegmentOffset off = block_meta.block_capacity() * block_meta.block_id() + i;
            bitmask.SetFalse(off);
        }
        offset = range.second;
    }
    for (BlockOffset i = offset; i < state.block_offset_end(); ++i) {
        bitmask.SetFalse(i);
    }

    return Status::OK();
}

Status NewCatalog::CheckSegmentRowsVisible(SegmentMeta &segment_meta,
                                           KVInstance *kv_instance,
                                           TxnTimeStamp begin_ts,
                                           TxnTimeStamp commit_ts,
                                           Bitmask &bitmask) {
    TxnTimeStamp first_delete_ts = 0;
    Status status = segment_meta.GetFirstDeleteTS(kv_instance, first_delete_ts, begin_ts);
    if (!status.ok()) {
        return status;
    }
    if (first_delete_ts > begin_ts) {
        return Status::OK();
    }
    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        status = NewCatalog::SetBlockDeleteBitmask(block_meta, begin_ts, commit_ts, bitmask);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

SharedPtr<FunctionSet> NewCatalog::GetFunctionSetByName(NewCatalog *catalog, String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if (!catalog->function_sets_.contains(function_name)) {
        Status status = Status::FunctionNotFound(function_name);
        RecoverableError(status);
    }
    return catalog->function_sets_[function_name];
}

void NewCatalog::AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (catalog->function_sets_.contains(name)) {
        String error_message = fmt::format("Trying to add duplicated function {} into catalog", name);
        UnrecoverableError(error_message);
    }
    catalog->function_sets_.emplace(name, function_set);
}

void NewCatalog::AppendToScalarFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set) {
    String name = function_set->name();
    StringToLower(name);
    if (!catalog->function_sets_.contains(name)) {
        String error_message = fmt::format("Trying to append to non-existent function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    auto target_scalar_function_set = std::dynamic_pointer_cast<ScalarFunctionSet>(catalog->function_sets_[name]);
    if (!target_scalar_function_set) {
        String error_message = fmt::format("Trying to append to non-scalar function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    auto source_function_set = std::dynamic_pointer_cast<ScalarFunctionSet>(function_set);
    if (!source_function_set) {
        String error_message = fmt::format("Trying to append non-scalar function to scalar function {} in catalog", name);
        UnrecoverableError(error_message);
    }
    for (const auto &function : source_function_set->GetAllScalarFunctions()) {
        target_scalar_function_set->AddFunction(function);
    }
}

void NewCatalog::AddSpecialFunction(NewCatalog *catalog, const SharedPtr<SpecialFunction> &special_function) {
    String name = special_function->name();
    StringToLower(name);
    if (catalog->special_functions_.contains(name)) {
        String error_message = fmt::format("Trying to add duplicated special function into catalog: {}", name);
        UnrecoverableError(error_message);
    }
    catalog->special_functions_.emplace(name, special_function);
    switch (special_function->special_type()) {
        case SpecialType::kRowID:
        case SpecialType::kDistance:
        case SpecialType::kDistanceFactors:
        case SpecialType::kSimilarity:
        case SpecialType::kSimilarityFactors:
        case SpecialType::kScore:
        case SpecialType::kScoreFactors:
        case SpecialType::kFilterFullText: {
            return;
        }
        case SpecialType::kCreateTs:
        case SpecialType::kDeleteTs: {
            break;
        }
    }
    auto special_column_def = MakeUnique<ColumnDef>(special_function->extra_idx(),
                                                    MakeShared<DataType>(special_function->data_type()),
                                                    special_function->name(),
                                                    std::set<ConstraintType>());
    catalog->special_columns_.emplace(name, std::move(special_column_def));
}

Tuple<SpecialFunction *, Status> NewCatalog::GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, String function_name) {
    StringToLower(function_name);
    if (!catalog->special_functions_.contains(function_name)) {
        return {nullptr, Status::SpecialFunctionNotFound()};
    }
    return {catalog->special_functions_[function_name].get(), Status::OK()};
}

} // namespace infinity

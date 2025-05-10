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

#include <filesystem>
#include <string>
module new_catalog;

import stl;
import new_txn;
import status;
import extra_ddl_info;
import kv_store;
import third_party;
import logger;
import infinity_exception;
import default_values;
import mem_index;
import column_index_reader;
import data_type;
import parsed_expr;
import constant_expr;
import meta_key;
import catalog;
import catalog_delta_entry;
import db_meeta;
import table_def;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_entry;
import table_index_meeta;
import index_secondary;
import index_ivf;
import index_hnsw;
import index_full_text;
import index_bmp;
import index_emvb;
import kv_code;
import config;
import create_index_info;
import statement_common;
import virtual_store;
import logger;
import utility;
import db_entry;
import table_entry;
import segment_entry;
import block_entry;
import block_column_entry;
import buffer_manager;
import infinity_context;
import segment_index_meta;
import chunk_index_meta;
import fast_rough_filter;
import persistence_manager;
import meta_key;
import catalog_cache;
import segment_index_entry;
import chunk_index_entry;

namespace infinity {

NewCatalog::NewCatalog(KVStore *kv_store) : kv_store_(kv_store) {}

NewCatalog::~NewCatalog() = default;

Status NewCatalog::Init(KVStore *kv_store) {
    auto kv_instance = kv_store->GetInstance();
    String db_string_id;
    Status status = kv_instance->Get(LATEST_DATABASE_ID.data(), db_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_DATABASE_ID.data(), "0");
    }
    String table_string_id;
    status = kv_instance->Get(LATEST_TABLE_ID.data(), table_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_TABLE_ID.data(), "0");
    }
    String index_string_id;
    status = kv_instance->Get(LATEST_INDEX_ID.data(), index_string_id);
    if (!status.ok()) {
        kv_instance->Put(LATEST_INDEX_ID.data(), "0");
    }
    status = kv_instance->Commit();
    if (!status.ok()) {
        UnrecoverableError("Can't initialize latest ID");
    }
    return Status::OK();
}

Status NewCatalog::TransformDeltaMeta(Config *config_ptr, const Vector<String> &delta_ckp_paths, KVInstance *kv_instance, bool is_vfs) {
    // Read delta checkpoint files
    for (const auto &delta_ckp_path : delta_ckp_paths) {
        // const auto &catalog_path = Path(InfinityContext::instance().config()->DataDir()) / file_name;
        // UniquePtr<CatalogDeltaEntry> catalog_delta_entry = Catalog::LoadFromFileDelta(catalog_path);
        // BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        // this->LoadFromEntryDelta(std::move(catalog_delta_entry), buffer_mgr);

        SharedPtr<PersistenceManager> pm_ptr;
        pm_ptr.reset();
        if (is_vfs) {
            String persistence_dir = config_ptr->PersistenceDir();
            if (!persistence_dir.empty()) {
                i64 persistence_object_size_limit = config_ptr->PersistenceObjectSizeLimit();
                pm_ptr = MakeShared<PersistenceManager>(persistence_dir, config_ptr->DataDir(), (SizeT)persistence_object_size_limit);
            }
        }

        Status status;
        UniquePtr<CatalogDeltaEntry> catalog_delta_entry = Catalog::LoadFromFileDelta(delta_ckp_path, pm_ptr.get());
        auto &delta_ops = catalog_delta_entry->operations();
        for (auto &op : delta_ops) {
            auto type = op->GetType();
            auto commit_ts = op->commit_ts_;
            auto begin_ts = op->begin_ts_;
            std::string_view encode = *op->encode_;
            auto merge_flag = op->merge_flag_;
            auto &addr_serializer = op->addr_serializer_;
            switch (type) {
                case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
                    auto add_db_entry_op = static_cast<AddDBEntryOp *>(op.get());
                    auto decodes = DBEntry::DecodeIndex(encode);
                    auto db_name = static_cast<String>(decodes[0]);
                    auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    // const auto &db_entry_dir = add_db_entry_op->db_entry_dir_;
                    auto db_comment = add_db_entry_op->comment_;

                    if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kDeleteAndNew) {
                        DBMeeta db_meta{db_id_str, *kv_instance};
                        status = this->CleanDB(db_meta, db_name, begin_ts, UsageFlag::kTransform);
                        if (!status.ok()) {
                            return status;
                        }
                    }
                    if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                        Optional<DBMeeta> db_meta;
                        status = this->AddNewDB(kv_instance, db_id_str, commit_ts, db_name, db_comment.get(), db_meta);
                        if (!status.ok()) {
                            return status;
                        }
                    } else if (merge_flag == MergeFlag::kUpdate) {
                        String error_message = "Update database entry is not supported.";
                        UnrecoverableError(error_message);
                    }
                    break;
                }
                case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
                    auto add_table_entry_op = static_cast<AddTableEntryOp *>(op.get());
                    auto decodes = TableEntry::DecodeIndex(encode);
                    auto db_name = MakeShared<String>(decodes[0]);
                    auto db_id_str = dbname_to_idstr_.contains(*db_name) ? dbname_to_idstr_[*db_name] : "";
                    auto table_name = MakeShared<String>(decodes[1]);

                    const auto &column_defs = add_table_entry_op->column_defs_;
                    auto unsealed_id = add_table_entry_op->unsealed_id_;

                    const SharedPtr<String> &table_comment = add_table_entry_op->table_comment_;
                    auto table_def = TableDef::Make(db_name, table_name, table_comment, column_defs);
                    auto table_full_name_str = fmt::format("{}.{}", *db_name, *table_name);
                    auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    DBMeeta db_meta{db_id_str, *kv_instance};
                    Optional<TableMeeta> tmp_optional_table_meta;

                    if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                        status = this->AddNewTable(db_meta, table_id_str, begin_ts, commit_ts, table_def, tmp_optional_table_meta);
                        if (!status.ok()) {
                            return status;
                        }
                        tmp_optional_table_meta->SetUnsealedSegmentID(unsealed_id);
                    } else if (merge_flag == MergeFlag::kUpdate) {
                        // TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance &kv_instance, TxnTimeStamp begin_ts);
                        TableMeeta table_meta{db_id_str, table_id_str, *kv_instance, begin_ts};
                        status = this->CleanTable(table_meta, *table_name, begin_ts, UsageFlag::kTransform);
                        if (!status.ok()) {
                            return status;
                        }
                        status = this->AddNewTable(db_meta, table_id_str, begin_ts, commit_ts, table_def, tmp_optional_table_meta);
                        if (!status.ok()) {
                            return status;
                        }
                        // db_entry->UpdateTableReplay(table_name, table_comment, init_table_entry, txn_id, begin_ts);
                    }
                    break;
                }
                case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
                    auto add_segment_entry_op = static_cast<AddSegmentEntryOp *>(op.get());
                    auto decodes = SegmentEntry::DecodeIndex(encode);
                    auto db_name = String(decodes[0]);
                    auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    auto table_name = String(decodes[1]);
                    auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
                    auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    SegmentID segment_id = 0;
                    std::from_chars(decodes[2].begin(), decodes[2].end(), segment_id);
                    auto segment_filter_binary_data = add_segment_entry_op->segment_filter_binary_data_;
                    SharedPtr<FastRoughFilter> fast_rough_filter = MakeShared<FastRoughFilter>();
                    fast_rough_filter->DeserializeFromString(segment_filter_binary_data);

                    // auto *db_entry = this->GetDatabaseReplay(db_name, txn_id, begin_ts);
                    // auto *table_entry = db_entry->GetTableReplay(table_name, txn_id, begin_ts);
                    TableMeeta table_meta{db_id_str, table_id_str, *kv_instance, begin_ts};
                    SegmentMeta segment_meta{segment_id, table_meta};

                    if (fast_rough_filter->IsValid()) {
                        segment_meta.SetFastRoughFilter(std::move(fast_rough_filter));
                    }

                    if (merge_flag == MergeFlag::kNew) {
                        // if (!segment_filter_binary_data.empty()) {
                        //     // segment_entry->LoadFilterBinaryData(std::move(segment_filter_binary_data));
                        // }
                        Optional<SegmentMeta> tmp_optional_segment_meta;
                        status = this->AddNewSegment1(table_meta, commit_ts, tmp_optional_segment_meta);
                        if (!status.ok()) {
                            return status;
                        }
                    } else if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kUpdate) {
                        status = this->CleanSegment(segment_meta, commit_ts, UsageFlag::kTransform);
                        if (!status.ok()) {
                            return status;
                        }
                        Optional<SegmentMeta> tmp_optional_segment_meta;
                        status = this->AddNewSegment1(table_meta, commit_ts, tmp_optional_segment_meta);
                        if (!status.ok()) {
                            return status;
                        }
                    } else {
                        String error_message = fmt::format("Unsupported merge flag {} for segment entry", static_cast<i8>(merge_flag));
                        UnrecoverableError(error_message);
                    }
                    break;
                }

                case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
                    auto add_block_entry_op = static_cast<AddBlockEntryOp *>(op.get());
                    auto decodes = BlockEntry::DecodeIndex(encode);
                    auto db_name = String(decodes[0]);
                    auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    auto table_name = String(decodes[1]);
                    auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
                    auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    SegmentID segment_id = 0;
                    std::from_chars(decodes[2].begin(), decodes[2].end(), segment_id);
                    BlockID block_id = 0;
                    std::from_chars(decodes[3].begin(), decodes[3].end(), block_id);
                    auto block_filter_binary_data = add_block_entry_op->block_filter_binary_data_;
                    SharedPtr<FastRoughFilter> fast_rough_filter = MakeShared<FastRoughFilter>();
                    fast_rough_filter->DeserializeFromString(block_filter_binary_data);

                    TableMeeta table_meta{db_id_str, table_id_str, *kv_instance, begin_ts};
                    SegmentMeta segment_meta{segment_id, table_meta};
                    BlockMeta block_meta{block_id, segment_meta};
                    if (fast_rough_filter->IsValid()) {
                        block_meta.SetFastRoughFilter(std::move(fast_rough_filter));
                    }

                    if (merge_flag == MergeFlag::kNew) {
                        // if (!block_filter_binary_data.empty()) {
                        //     // new_block->LoadFilterBinaryData(std::move(block_filter_binary_data));
                        // }
                        Optional<BlockMeta> tmp_optional_block_meta;
                        status = this->AddNewBlockForTransform(segment_meta, commit_ts, tmp_optional_block_meta);
                        if (!status.ok()) {
                            return status;
                        }
                    } else if (merge_flag == MergeFlag::kUpdate) {
                        status = this->CleanBlock(block_meta, UsageFlag::kTransform);
                        if (!status.ok()) {
                            return status;
                        }
                        Optional<BlockMeta> tmp_optional_block_meta;
                        status = this->AddNewBlockForTransform(segment_meta, commit_ts, tmp_optional_block_meta);
                        if (!status.ok()) {
                            return status;
                        }
                    } else {
                        String error_message = fmt::format("Unsupported merge flag {} for block entry", static_cast<i8>(merge_flag));
                        UnrecoverableError(error_message);
                    }
                    break;
                }
                case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
                    // auto add_column_entry_op = static_cast<AddColumnEntryOp *>(op.get());
                    auto decodes = BlockColumnEntry::DecodeIndex(encode);
                    auto db_name = String(decodes[0]);
                    auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    auto table_name = String(decodes[1]);
                    auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
                    auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    SegmentID segment_id = 0;
                    std::from_chars(decodes[2].begin(), decodes[2].end(), segment_id);
                    BlockID block_id = 0;
                    std::from_chars(decodes[3].begin(), decodes[3].end(), block_id);
                    ColumnID column_id = 0;
                    std::from_chars(decodes[4].begin(), decodes[4].end(), column_id);
                    // const auto [next_outline_idx, last_chunk_offset] = add_column_entry_op->outline_info_;
                    TableMeeta table_meta{db_id_str, table_id_str, *kv_instance, begin_ts};
                    SegmentMeta segment_meta{segment_id, table_meta};
                    BlockMeta block_meta{block_id, segment_meta};
                    ColumnMeta column_meta{column_id, block_meta};
                    if (merge_flag == MergeFlag::kDelete) {
                        auto [column_def, status] = table_meta.GetColumnDefByColumnID(column_id);
                        if (!status.ok()) {
                            return status;
                        }
                        status = this->CleanBlockColumn(column_meta, column_def.get(), UsageFlag::kTransform);
                        if (!status.ok()) {
                            return status;
                        }
                    } else if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kUpdate) {
                        Optional<ColumnMeta> tmp_optional_column_meta;
                        status = this->AddNewBlockColumnForTransform(block_meta, column_id, tmp_optional_column_meta, commit_ts);
                        if (!status.ok()) {
                            return status;
                        }
                    } else {
                        UnrecoverableError(fmt::format("Unsupported merge flag {} for column entry {}", static_cast<i8>(merge_flag), column_id));
                    }
                    // We need record the offset in the vfs mode.
                    if (is_vfs) {
                        // assert(addr_serializer.)
                        auto &paths = addr_serializer.paths_;
                        auto &obj_addrs_ = addr_serializer.obj_addrs_;

                        auto paths_size = paths.size();
                        for (SizeT i = 0; i < paths_size; ++i) {
                            auto &obj_addr_path_str = paths[i];
                            String fine_path;
                            status = RefactorPath(obj_addr_path_str, fine_path, '|');
                            if (!status.ok()) {
                                return status;
                            }
                            auto obj_addr_path_key = KeyEncode::PMObjectPathKey(fine_path);
                            // obj_addr_path_key = "more|" + obj_addr_path_key;
                            auto &obj_addr = obj_addrs_[i];
                            nlohmann::json json_obj;
                            json_obj["obj_key"] = obj_addr.obj_key_;
                            json_obj["part_offset"] = obj_addr.part_offset_;
                            json_obj["part_size"] = obj_addr.part_size_;
                            kv_instance->Put(obj_addr_path_key, json_obj.dump());
                        }
                    }
                    break;
                }
                    // -----------------------------
                    // INDEX
                    // -----------------------------
                    // case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
                    //     auto add_table_index_entry_op = static_cast<AddTableIndexEntryOp *>(op.get());
                    //     auto decodes = TableIndexEntry::DecodeIndex(encode);
                    //     auto db_name = String(decodes[0]);
                    //     auto table_name = String(decodes[1]);
                    //     auto index_name = MakeShared<String>(decodes[2]);
                    //     const auto &index_dir = add_table_index_entry_op->index_dir_;
                    //     auto index_base = add_table_index_entry_op->index_base_;
                    //
                    //     auto *db_entry = this->GetDatabaseReplay(db_name, txn_id, begin_ts);
                    //     auto *table_entry = db_entry->GetTableReplay(table_name, txn_id, begin_ts);
                    //     if (merge_flag == MergeFlag::kDelete || merge_flag == MergeFlag::kDeleteAndNew) {
                    //         table_entry->DropIndexReplay(
                    //             *index_name,
                    //             [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
                    //                 auto index_base = MakeShared<IndexBase>(index_meta->index_name());
                    //                 auto index_entry = TableIndexEntry::NewTableIndexEntry(index_base, true, nullptr, index_meta, txn_id,
                    //                 begin_ts); index_entry->commit_ts_.store(commit_ts); return index_entry;
                    //             },
                    //             txn_id,
                    //             begin_ts);
                    //     }
                    //     if (merge_flag == MergeFlag::kNew || merge_flag == MergeFlag::kDeleteAndNew) {
                    //         table_entry->CreateIndexReplay(
                    //             index_name,
                    //             [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
                    //                 return TableIndexEntry::ReplayTableIndexEntry(index_meta, false, index_base, index_dir, txn_id, begin_ts,
                    //                 commit_ts);
                    //             },
                    //             txn_id,
                    //             begin_ts);
                    //     } else if (merge_flag == MergeFlag::kUpdate) {
                    //         table_entry->UpdateIndexReplay(*index_name, txn_id, begin_ts, commit_ts);
                    //     }
                    //     break;
                    // }
                    // case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                    //     auto add_segment_index_entry_op = static_cast<AddSegmentIndexEntryOp *>(op.get());
                    //     auto decodes = SegmentIndexEntry::DecodeIndex(encode);
                    //     auto db_name = String(decodes[0]);
                    //     auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    //     auto table_name = String(decodes[1]);
                    //     auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
                    //     auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    //     auto index_name = String(decodes[2]);
                    //     String index_id_str = "";
                    //     SegmentID segment_id = 0;
                    //     std::from_chars(decodes[3].begin(), decodes[3].end(), segment_id);
                    //     // auto min_ts = add_segment_index_entry_op->min_ts_;
                    //     // auto max_ts = add_segment_index_entry_op->max_ts_;
                    //     // auto next_chunk_id = add_segment_index_entry_op->next_chunk_id_;
                    //     // auto deprecate_ts = add_segment_index_entry_op->deprecate_ts_;
                    //
                    //     TableMeeta table_meeta{db_id_str, table_id_str, *kv_instance, begin_ts};
                    //     TableIndexMeeta table_index_meta{index_id_str, table_meeta};
                    //     Optional<SegmentIndexMeta> segment_index_meta;
                    //
                    //     if (auto iter = table_entry->segment_map_.find(segment_id); iter != table_entry->segment_map_.end()) {
                    //         auto *table_index_entry = table_entry->GetIndexReplay(index_name, txn_id, begin_ts);
                    //         auto *segment_entry = iter->second.get();
                    //         if (merge_flag != MergeFlag::kDelete && segment_entry->status() == SegmentStatus::kDeprecated) {
                    //             String error_message = fmt::format("Segment {} is deprecated", segment_id);
                    //             UnrecoverableError(error_message);
                    //         }
                    //         if (merge_flag == MergeFlag::kNew) {
                    //             status = this->AddNewSegmentIndex(table_index_meta, segment_id, segment_index_meta);
                    //             if (!status.ok()) {
                    //                 return status;
                    //             }
                    //         } else if (merge_flag == MergeFlag::kUpdate || merge_flag == MergeFlag::kDelete) {
                    //             auto iter = table_index_entry->index_by_segment().find(segment_id);
                    //             if (iter == table_index_entry->index_by_segment().end()) {
                    //                 String error_message = fmt::format("Segment index {} is not found", segment_id);
                    //                 UnrecoverableError(error_message);
                    //             }
                    //             status = this->CleanSegmentIndex(*segment_index_meta, UsageFlag::kTransform);
                    //             if (!status.ok()) {
                    //                 return status;
                    //             }
                    //             status = this->AddNewSegmentIndex(table_index_meta, segment_id, segment_index_meta);
                    //             if (!status.ok()) {
                    //                 return status;
                    //             }
                    //         }
                    //     }
                    //     break;
                    // }
                    // case CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY: {
                    //     auto add_chunk_index_entry_op = static_cast<AddChunkIndexEntryOp *>(op.get());
                    //     auto decodes = ChunkIndexEntry::DecodeIndex(encode);
                    //     auto db_name = String(decodes[0]);
                    //     auto db_id_str = dbname_to_idstr_.contains(db_name) ? dbname_to_idstr_[db_name] : "";
                    //     auto table_name = String(decodes[1]);
                    //     auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
                    //     auto table_id_str = dbname_to_idstr_.contains(table_full_name_str) ? dbname_to_idstr_[table_full_name_str] : "";
                    //     auto index_name = String(decodes[2]);
                    //     String index_id_str = "";
                    //     SegmentID segment_id = 0;
                    //     std::from_chars(decodes[3].begin(), decodes[3].end(), segment_id);
                    //     ChunkID chunk_id = 0;
                    //     std::from_chars(decodes[4].begin(), decodes[4].end(), chunk_id);
                    //     const auto &base_name = add_chunk_index_entry_op->base_name_;
                    //     auto base_rowid = add_chunk_index_entry_op->base_rowid_;
                    //     auto row_count = add_chunk_index_entry_op->row_count_;
                    //     auto commit_ts = add_chunk_index_entry_op->commit_ts_;
                    //     auto deprecate_ts = add_chunk_index_entry_op->deprecate_ts_;
                    //
                    //     TableIndexMeeta table_index_meeta{db_id_str, table_id_str, *kv_instance, begin_ts};
                    //     SegmentIndexMeta segment_index_meta{, table_index_meeta};
                    //
                    //     auto *db_entry = this->GetDatabaseReplay(db_name, txn_id, begin_ts);
                    //     auto *table_entry = db_entry->GetTableReplay(table_name, txn_id, begin_ts);
                    //
                    //     if (auto iter = table_entry->segment_map_.find(segment_id); iter != table_entry->segment_map_.end()) {
                    //         auto *table_index_entry = table_entry->GetIndexReplay(index_name, txn_id, begin_ts);
                    //         auto *segment_entry = iter->second.get();
                    //         if (segment_entry->status() == SegmentStatus::kDeprecated) {
                    //             String error_message = fmt::format("Segment {} is deprecated", segment_id);
                    //             UnrecoverableError(error_message);
                    //         }
                    //         auto iter2 = table_index_entry->index_by_segment().find(segment_id);
                    //         if (iter2 == table_index_entry->index_by_segment().end()) {
                    //             String error_message = fmt::format("Segment index {} is not found", segment_id);
                    //             UnrecoverableError(error_message);
                    //         }
                    //         auto *segment_index_entry = iter2->second.get();
                    //         status = this->AddNewChunkIndex(, chunk_id, base_rowid, row_count, base_name, );
                    //         if (!status.ok()) {
                    //             return status;
                    //         }
                    //     }
                    //
                    //     break;
                    // }

                case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
                    break;
                }
                case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
                    break;
                }
                case CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY: {
                    break;
                }
                // -----------------------------
                // SEGMENT STATUS
                // -----------------------------
                case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
                    break;
                }
                case CatalogDeltaOpType::SET_BLOCK_STATUS_SEALED: {
                    break;
                }
                case CatalogDeltaOpType::INVALID: {
                    break;
                }
                    // default:
                    //     break;
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::RefactorPath(const String &path_key, String &fine_path, char delimiter) {
    // Need rename
    Status status;
    auto path_names = infinity::Partition(std::move(path_key), '/');

    auto &database_str = path_names[0];
    auto &table_str = path_names[1];

    auto database_name = database_str.substr(db_prefix_len_);

    auto database_id_str = dbname_to_idstr_[database_name];
    // if (!status.ok()) {
    //     return status;
    // }

    auto table_name = table_str.substr(table_prefix_len_);
    auto db_name_and_table_name = fmt::format("{}.{}", database_name, table_name);
    auto table_id_str = dbname_to_idstr_[db_name_and_table_name];

    database_str = fmt::format("db_{}", database_id_str);
    table_str = fmt::format("tbl_{}", table_id_str);

    fine_path = infinity::Concat(path_names, delimiter);
    return Status::OK();
}

Status NewCatalog::TransformData(const String &data_path, KVInstance *kv_instance, nlohmann::json *full_ckp_json, bool is_vfs) {
    auto path_tuple_getter = [](const std::filesystem::directory_entry &entry) {
        auto path = entry.path().string();
        auto path_prefix = path.substr(0, path.find_last_of('/') + 1);
        auto path_suffix = path.substr(path.find_last_of('/') + 1);
        return std::make_tuple(path, path_prefix, path_suffix);
    };
    if (is_vfs) {
        Status status;
        for (auto &obj_addr_json : (*full_ckp_json)["obj_addr_map"]["obj_addr_array"]) {
            String path_key = obj_addr_json["local_path"];
            String fine_path;
            status = RefactorPath(path_key, fine_path, '/');
            if (!status.ok()) {
                return status;
            }
            auto ob_addr_key = KeyEncode::PMObjectKey(fine_path);
            kv_instance->Put(ob_addr_key, obj_addr_json["obj_addr"].dump());
            if (!status.ok()) {
                return status;
            }
        }
        for (auto &obj_json : (*full_ckp_json)["obj_addr_map"]["objects"]["obj_stat_array"]) {
            String obj_key = obj_json["obj_key"];
            auto ob_addr_key = KeyEncode::PMObjectStatKey(obj_key);
            kv_instance->Put(ob_addr_key, obj_json["obj_stat"].dump());
            if (!status.ok()) {
                return status;
            }
        }
    } else {
        for (const auto &db_entry : std::filesystem::directory_iterator{data_path}) {
            auto [db_path, db_path_prefix, db_path_suffix] = path_tuple_getter(db_entry);
            if (db_path_suffix == "catalog") {
                continue;
            }
            if (!dir_set_.contains(db_path_suffix)) {
                LOG_WARN(fmt::format("\"{}\" may be a trash dir. Consider to delete it.", db_path));
                continue;
            }
            String db_name = db_path_suffix.substr(db_prefix_len_);
            for (const auto &table_entry : std::filesystem::directory_iterator{db_path}) {
                auto [table_path, table_path_prefix, table_path_suffix] = path_tuple_getter(table_entry);
                auto db_path_suffix_and_table_path_suffix = fmt::format("{}/{}", db_path_suffix, table_path_suffix);
                if (!dir_set_.contains(db_path_suffix_and_table_path_suffix)) {
                    LOG_WARN(fmt::format("\"{}\" may be a trash dir. Consider to delete it.", table_path));
                    continue;
                }
                auto table_name = table_path_suffix.substr(table_prefix_len_);

                auto db_name_and_table_name = fmt::format("{}.{}", db_name, table_name);
                auto table_id_str = dbname_to_idstr_[db_name_and_table_name];
                auto new_table_path_suffix = fmt::format("tbl_{}", table_id_str);
                auto new_table_path = VirtualStore::ConcatenatePath(table_path_prefix, new_table_path_suffix);
                std::filesystem::rename(table_path, new_table_path);
            }
            auto db_id_str = dbname_to_idstr_[db_name];
            auto new_db_path_suffix = fmt::format("db_{}", db_id_str);
            auto new_db_path = VirtualStore::ConcatenatePath(db_path_prefix, new_db_path_suffix);
            std::filesystem::rename(db_path, new_db_path);
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalog(Config *config_ptr, const String &full_ckp_path, const Vector<String> &delta_ckp_paths) {
    // Read full checkpoint file
    // Status status;
    String value;
    Status status = kv_store_->Get(LATEST_DATABASE_ID.data(), value);
    if (status.ok()) {
        return status;
    }
    if (status.code() != ErrorCode::kNotFound) {
        UnrecoverableError("Get element from kv failed.");
    }

    UniquePtr<nlohmann::json> full_ckp_json = Catalog::LoadFullCheckpointToJson(config_ptr, full_ckp_path);
    status = NewCatalog::Init(kv_store_);
    if (!status.ok()) {
        return status;
    }

    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();

    dir_set_.insert(".");
    auto data_path = config_ptr->DataDir();
    // auto data_path = "/home/inf/Downloads/infinity_vfs_off1/data";
    bool is_vfs = full_ckp_json->contains("obj_addr_map") && (*full_ckp_json)["obj_addr_map"].contains("obj_addr_size");

    if (full_ckp_json->contains("databases")) {
        for (const auto &db_json : (*full_ckp_json)["databases"]) {
            status = TransformCatalogDatabase(db_json, kv_instance.get(), data_path, is_vfs);
            if (!status.ok()) {
                return status;
            }
        }
    }

    // Transform delta meta
    TransformDeltaMeta(config_ptr, delta_ckp_paths, kv_instance.get(), is_vfs);

    TransformData(data_path, kv_instance.get(), full_ckp_json.get(), is_vfs);

    // Rename the filename
    // if (is_vfs) {}
    status = kv_instance->Commit();
    if (!status.ok()) {
        return status;
    }
    status = kv_store_->Flush();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogDatabase(const nlohmann::json &db_meta_json, KVInstance *kv_instance, const String &db_path, bool is_vfs) {
    String db_name = db_meta_json["db_name"];
    if (db_meta_json.contains("db_entries")) {

        TxnTimeStamp max_commit_ts = 0;

        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            TxnTimeStamp current_commit_ts = db_entry_json["commit_ts"];
            max_commit_ts = std::max(max_commit_ts, current_commit_ts);
        }

        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            if (max_commit_ts == db_entry_json["commit_ts"]) {
                if (db_entry_json["deleted"] == true) {
                    return Status::OK();
                }

                SharedPtr<String> db_comment;
                if (db_entry_json.contains("db_comment")) {
                    db_comment = MakeShared<String>(db_entry_json["db_comment"]);
                } else {
                    db_comment = MakeShared<String>();
                }

                String db_id_str;
                Status status = IncrLatestID(db_id_str, LATEST_DATABASE_ID);
                if (!status.ok()) {
                    return status;
                }
                dbname_to_idstr_[db_name] = db_id_str;
                Optional<DBMeeta> db_meta;
                String dir_path = db_entry_json["db_entry_dir"];
                dir_set_.emplace(dir_path);
                dir_path = fmt::format("{}/{}", db_path, dir_path);
                if (!is_vfs) {
                    if (!std::filesystem::exists(dir_path)) {
                        UnrecoverableError("Data does not exist.");
                    }
                }
                status = NewCatalog::AddNewDB(kv_instance, db_id_str, max_commit_ts, db_name, db_comment.get(), db_meta);
                if (!status.ok()) {
                    return status;
                }

                if (db_entry_json.contains("tables")) {
                    for (const auto &table_meta_json : db_entry_json["tables"]) {
                        Status status = TransformCatalogTable(db_meta.value(), table_meta_json, db_name, db_path, is_vfs);
                        if (!status.ok()) {
                            return status;
                        }
                    }
                }
                break;
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogTable(DBMeeta &db_meta,
                                         const nlohmann::json &table_meta_json,
                                         String const &db_name,
                                         const String &db_path,
                                         bool is_vfs) {
    String table_name = table_meta_json["table_name"];
    if (table_meta_json.contains("table_entries")) {
        for (auto &table_entry_json : table_meta_json["table_entries"]) {
            bool deleted = table_entry_json["deleted"];
            if (deleted) {
                continue;
            }

            String table_id_str;
            Status status = IncrLatestID(table_id_str, LATEST_TABLE_ID);
            if (!status.ok()) {
                return status;
            }
            auto table_full_name_str = fmt::format("{}.{}", db_name, table_name);
            dbname_to_idstr_[table_full_name_str] = table_id_str;

            TxnTimeStamp table_begin_ts = table_entry_json["begin_ts"];
            TxnTimeStamp table_commit_ts = table_entry_json["commit_ts"];

            Vector<SharedPtr<ColumnDef>> columns;

            SharedPtr<String> table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);

            for (const auto &column_def_json : table_entry_json["column_definition"]) {
                SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
                i64 column_id = column_def_json["column_id"];
                String column_name = column_def_json["column_name"];

                Set<ConstraintType> constraints;
                if (column_def_json.contains("constraints")) {
                    for (const auto &column_constraint : column_def_json["constraints"]) {
                        ConstraintType constraint = column_constraint;
                        constraints.emplace(constraint);
                    }
                }

                String comment;
                if (column_def_json.contains("column_comment")) {
                    comment = column_def_json["column_comment"];
                }

                SharedPtr<ParsedExpr> default_expr = nullptr;
                if (column_def_json.contains("default")) {
                    default_expr = ConstantExpr::Deserialize(column_def_json["default"]);
                }

                SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints, comment, default_expr);
                columns.emplace_back(column_def);
            }

            String table_comment;
            auto table_def = TableDef::Make(MakeShared<String>(db_name), MakeShared<String>(table_name), MakeShared<String>(table_comment), columns);
            Optional<TableMeeta> table_meta;
            String dir_path = table_entry_json["table_entry_dir"];
            dir_set_.emplace(dir_path);
            dir_path = fmt::format("{}/{}", db_path, dir_path);
            if (!is_vfs) {
                if (!std::filesystem::exists(dir_path)) {
                    UnrecoverableError("Data does not exist.");
                }
            }
            status = AddNewTable(db_meta, table_id_str, table_begin_ts, table_commit_ts, table_def, table_meta);
            if (!status.ok()) {
                return status;
            }

            SegmentID unsealed_segment_id = table_entry_json["unsealed_id"];
            table_meta->SetUnsealedSegmentID(unsealed_segment_id);

            if (table_entry_json.contains("segments")) {
                for (auto &segment_json : table_entry_json["segments"]) {
                    if (segment_json["deleted"]) {
                        continue;
                    }
                    status = TransformCatalogSegment(table_meta.value(), segment_json, db_path, is_vfs);
                    if (!status.ok()) {
                        return status;
                    }
                }
            }

            if (table_entry_json.contains("table_indexes")) {
                for (auto &index_json : table_entry_json["table_indexes"]) {
                    status = TransformCatalogTableIndex(table_meta.value(), index_json);
                    if (!status.ok()) {
                        return status;
                    }
                }
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogSegment(TableMeeta &table_meta, const nlohmann::json &segment_entry_json, const String &db_path, bool is_vfs) {
    TxnTimeStamp segment_commit_ts = segment_entry_json["commit_ts"];

    Optional<SegmentMeta> segment_meta;
    String dir_path = segment_entry_json["segment_dir"];
    dir_set_.emplace(dir_path);
    dir_path = fmt::format("{}/{}", db_path, dir_path);
    if (!is_vfs) {
        if (!std::filesystem::exists(dir_path)) {
            UnrecoverableError("Data does not exist.");
        }
    }
    Status status = NewCatalog::AddNewSegment1(table_meta, segment_commit_ts, segment_meta);
    if (!status.ok()) {
        return status;
    }

    if (segment_entry_json.contains("block_entries")) {
        for (const auto &block_entry_json : segment_entry_json["block_entries"]) {
            status = TransformCatalogBlock(segment_meta.value(), block_entry_json, db_path, is_vfs);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogBlock(SegmentMeta &segment_meta, const nlohmann::json &block_entry_json, const String &db_path, bool is_vfs) {
    TxnTimeStamp block_commit_ts = block_entry_json["commit_ts"];
    Optional<BlockMeta> block_meta;

    String dir_path = block_entry_json["block_dir"];
    dir_set_.emplace(dir_path);
    dir_path = fmt::format("{}/{}", db_path, dir_path);
    if (!is_vfs) {
        if (!std::filesystem::exists(dir_path)) {
            UnrecoverableError("Data does not exist.");
        }
    }

    dir_path = block_entry_json["version_file"];
    dir_set_.emplace(dir_path);
    dir_path = fmt::format("{}/{}", db_path, dir_path);
    if (!is_vfs) {
        if (!std::filesystem::exists(dir_path)) {
            UnrecoverableError("Data does not exist.");
        }
    }

    Status status = NewCatalog::AddNewBlockForTransform(segment_meta, block_commit_ts, block_meta);

    for (const auto &block_column_json : block_entry_json["columns"]) {
        SizeT column_id = block_column_json["column_id"];
        dir_path = fmt::format("{}/{}.col", static_cast<String>(block_entry_json["block_dir"]), column_id);
        dir_set_.emplace(dir_path);
        SizeT next_outline_idx = block_column_json["next_outline_idx"];
        if (next_outline_idx) {
            dir_path = fmt::format("{}/col_{}_out_{}", static_cast<String>(block_entry_json["block_dir"]), column_id, next_outline_idx - 1);
            dir_set_.emplace(dir_path);
        }
        Status status = TransformCatalogBlockColumn(block_meta.value(), block_column_json);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::TransformCatalogBlockColumn(BlockMeta &block_meta, const nlohmann::json &block_column_entry_json) {
    Optional<ColumnMeta> column_meta;
    SizeT column_id = block_column_entry_json["column_id"];
    TxnTimeStamp commit_ts = block_column_entry_json["commit_ts"];
    Status status = NewCatalog::AddNewBlockColumnForTransform(block_meta, column_id, column_meta, commit_ts);
    return Status::OK();
}

Status NewCatalog::TransformCatalogTableIndex(TableMeeta &table_meta, const nlohmann::json &table_index_entry_json) {
    String index_id_str;
    Status status = IncrLatestID(index_id_str, LATEST_INDEX_ID);
    if (!status.ok()) {
        return status;
    }
    auto &entries = table_index_entry_json["index_entries"];
    TxnTimeStamp index_commit_ts;
    for (const auto &index_entry_json : entries) {
        index_commit_ts = index_entry_json["commit_ts"];
        Optional<TableIndexMeeta> table_index_meta;

        auto index_name_str = index_entry_json["index_base"]["index_name"];
        SharedPtr<String> index_name_ptr = MakeShared<String>(index_name_str);

        auto index_comment_str = index_entry_json["index_base"]["index_comment"];
        SharedPtr<String> index_comment_ptr = MakeShared<String>(index_comment_str);

        const String file_name = index_entry_json["index_base"]["file_name"];

        // auto index_base_ptr = MakeShared<IndexBase>(index_name_ptr);
        // status = AddNewTableIndex(table_meta, index_id_str, index_commit_ts, index_base_ptr, table_index_meta);

        Vector<String> column_names;
        for (auto column_name : index_entry_json["index_base"]["column_names"]) {
            column_names.push_back(column_name);
        }

        SharedPtr<IndexBase> index_base;

        const auto &type = IndexInfo::StringToIndexType(index_entry_json["index_base"]["index_type"].get<String>());

        Vector<InitParameter *> index_param_list;

        switch (type) {
            case IndexType::kSecondary:
                index_base = IndexSecondary::Make(index_name_ptr, index_comment_ptr, file_name, column_names);
                break;

            case IndexType::kIVF:
                index_param_list = {new InitParameter{"metric", "l2"},
                                    new InitParameter{"scalar_quantization_bits", "4"},
                                    new InitParameter{"centroids_num_ratio", "0.33"},
                                    new InitParameter{"storage_type", "scalar_quantization"}};
                index_base = IndexIVF::Make(index_name_ptr, index_comment_ptr, file_name, column_names, index_param_list);
                break;

            case IndexType::kHnsw:
                index_param_list = {new InitParameter{"metric", "l2"},
                                    new InitParameter{"encode", "plain"},
                                    new InitParameter{"m", "16"},
                                    new InitParameter{"ef_construction", "200"}};
                index_base = IndexHnsw::Make(index_name_ptr, index_comment_ptr, file_name, column_names, index_param_list);
                break;

            case IndexType::kFullText:
                // index_param_list.emplace_back(new InitParameter("analyzer", "chinese"));
                index_base = IndexFullText::Make(index_name_ptr, index_comment_ptr, file_name, column_names, index_param_list);
                break;

            case IndexType::kBMP:
                index_param_list = {new InitParameter{"block_size", "16"}, new InitParameter{"compress_type", "compress"}};
                index_base = IndexBMP::Make(index_name_ptr, index_comment_ptr, file_name, column_names, index_param_list);
                break;

            case IndexType::kEMVB:
                index_param_list = {new InitParameter{"pq_subspace_num", "32"}, new InitParameter{"pq_subspace_bits", "8"}};
                index_base = IndexEMVB::Make(index_name_ptr, index_comment_ptr, file_name, column_names, index_param_list);
                break;

            default:
                UnrecoverableError("Unsupported index type: " + IndexInfo::IndexTypeToString(type));
        }

        for (auto *param : index_param_list) {
            delete param;
        }

        // MakeShared<IndexBase>(IndexBase(index_type,index_name_ptr,index_comment_ptr,file_name,column_names));

        status = AddNewTableIndex(table_meta, index_id_str, index_commit_ts, index_base, table_index_meta);

        // explicit IndexBase(IndexType index_type,
        //               SharedPtr<String> index_name,
        //              SharedPtr<String> index_comment,
        //               const String &file_name,
        //               Vector<String> column_names)
        //: index_type_(index_type), index_name_(std::move(index_name)), index_comment_(std::move(index_comment)), file_name_(file_name),
        //  column_names_(std::move(column_names))
        if (!status.ok()) {
            return status;
        }
    }
    // TxnTimeStamp index_commit_ts = entries["commit_ts"];
    //  Optional<TableIndexMeeta> table_index_meta;
    //  auto index_name_str = table_index_entry_json["index_name"];
    //  SharedPtr<String> index_name_ptr = MakeShared<String>(index_name_str);
    //  auto index_base_ptr = MakeShared<IndexBase>(index_name_ptr);
    //  status = AddNewTableIndex(table_meta, index_id_str, index_commit_ts, index_base_ptr, table_index_meta);
    return Status::OK();
}

Status NewCatalog::TransformCatalogSegmentIndex(TableIndexMeeta &table_index_meta, const nlohmann::json &segment_index_entry_json) {

    for (const auto &index_entry_json : segment_index_entry_json) {
        SegmentID segment_id = index_entry_json["segment_id"];
        Optional<SegmentIndexMeta> segment_index_meta;
        Status status = AddNewSegmentIndex(table_index_meta, segment_id, segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }
    // AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta);
    return Status::OK();
}

Status NewCatalog::TransformCatalogChunkIndex(SegmentIndexMeta &segment_index_meta, const nlohmann::json &chunk_index_entry_json) {

    for (const auto &index_entry_json : chunk_index_entry_json) {
        ChunkID chunk_id = index_entry_json["chunk_id"];
        RowID base_row_id = RowID(index_entry_json["base_rowid"].get<uint64_t>());
        SizeT row_count = index_entry_json["row_count"];
        String base_name = index_entry_json["base_name"];
        Optional<ChunkIndexMeta> chunk_index_meta;
        AddNewChunkIndex(segment_index_meta, chunk_id, base_row_id, row_count, base_name, row_count, chunk_index_meta);
    }
    // static Status AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
    //                                ChunkID chunk_id,
    //                                RowID base_row_id,
    //                                SizeT row_count,
    //                                const String &base_name,
    //                                SizeT index_size,
    //                                Optional<ChunkIndexMeta> &chunk_index_meta);
    return Status::OK();
}

String NewCatalog::GetPathNameTail(const String &path) {
    SizeT delimiter_i = path.rfind('/');
    if (delimiter_i == String::npos) {
        return path;
    }
    return path.substr(delimiter_i + 1);
}

Status NewCatalog::AddBlockLock(String block_key) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>());
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::AddBlockLock(String block_key, TxnTimeStamp checkpoint_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<BlockLock>>::iterator iter;
    {
        std::unique_lock lock(block_lock_mtx_);
        std::tie(iter, insert_success) = block_lock_map_.emplace(std::move(block_key), MakeShared<BlockLock>(checkpoint_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("Block key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock) {
    block_lock = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(block_lock_mtx_);
        if (auto iter = block_lock_map_.find(block_key); iter != block_lock_map_.end()) {
            block_lock = iter->second;
        }
    }
    if (block_lock == nullptr) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::DropBlockLockByBlockKey(const String &block_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(block_lock_mtx_);
        delete_success = block_lock_map_.erase(block_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("Block key: {} not found", block_key));
    }
    return Status::OK();
}

Status NewCatalog::AddMemIndex(String mem_index_key, SharedPtr<MemIndex> mem_index) {
    bool insert_success = false;
    HashMap<String, SharedPtr<MemIndex>>::iterator iter;
    {
        std::unique_lock lock(mem_index_mtx_);
        std::tie(iter, insert_success) = mem_index_map_.emplace(std::move(mem_index_key), std::move(mem_index));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetMemIndex(const String &mem_index_key, SharedPtr<MemIndex> &mem_index) {
    mem_index = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(mem_index_mtx_);
        if (auto iter = mem_index_map_.find(mem_index_key); iter != mem_index_map_.end()) {
            mem_index = iter->second;
        }
    }
    if (mem_index == nullptr) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Status NewCatalog::DropMemIndexByMemIndexKey(const String &mem_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(mem_index_mtx_);
        delete_success = mem_index_map_.erase(mem_index_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("MemIndex key: {} not found", mem_index_key));
    }
    return Status::OK();
}

Vector<Pair<String, String>> NewCatalog::GetAllMemIndexInfo() {
    Vector<Pair<String, String>> result;
    {
        std::unique_lock lock(mem_index_mtx_);
        for (const auto &mem_index_pair : mem_index_map_) {
            if (mem_index_pair.second->memory_hnsw_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "hnsw"});
                continue;
            }
            if (mem_index_pair.second->memory_ivf_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "ivf"});
                continue;
            }
            if (mem_index_pair.second->memory_indexer_ != nullptr) {
                result.push_back({mem_index_pair.first, "full-text"});
                continue;
            }
            if (mem_index_pair.second->memory_secondary_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "secondary"});
                continue;
            }
            if (mem_index_pair.second->memory_emvb_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "emvb"});
                continue;
            }
            if (mem_index_pair.second->memory_bmp_index_ != nullptr) {
                result.push_back({mem_index_pair.first, "bmp"});
                continue;
            }
            result.push_back({mem_index_pair.first, "empty"});
        }
    }

    return result;
}

Status NewCatalog::IncreaseTableReferenceCountForMemIndex(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        table_lock_for_mem_index_[table_key] = MakeShared<TableLockForMemIndex>();
    }

    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is dumping mem index", table_key));
    }

    ++table_lock_for_mem_index->append_count_;

    return Status::OK();
}

Status NewCatalog::DecreaseTableReferenceCountForMemIndex(const String &table_key, SizeT count) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return Status::OK();
    }

    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->dumping_mem_index_) {
        UnrecoverableError(fmt::format("Table key: {} is dumping mem index", table_key));
    }

    SizeT &append_count = table_lock_for_mem_index->append_count_;
    if (append_count >= count) {
        append_count -= count;
    } else {
        UnrecoverableError(fmt::format("Attempt to reduce reference count {} by {}, of {}", append_count, count, table_key));
    }

    if (append_count == 0) {
        table_lock_for_mem_index_.erase(table_key);
    }

    return Status::OK();
}

SizeT NewCatalog::GetTableReferenceCountForMemIndex(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return 0;
    }

    return iter->second->append_count_;
}

SizeT NewCatalog::GetTableReferenceCountForMemIndex() const {
    std::unique_lock lock(mem_index_mtx_);
    return table_lock_for_mem_index_.size();
}

Status NewCatalog::SetMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        table_lock_for_mem_index_[table_key] = MakeShared<TableLockForMemIndex>();
    }
    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (table_lock_for_mem_index->append_count_ > 0) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is appending mem index", table_key));
    }

    if (table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} is dumping mem index", table_key));
    }
    table_lock_for_mem_index->dumping_mem_index_ = true;
    return Status::OK();
}

Status NewCatalog::UnsetMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        UnrecoverableError(fmt::format("Table key: {} isn't found in mem index dump", table_key));
    }
    TableLockForMemIndex *table_lock_for_mem_index = table_lock_for_mem_index_[table_key].get();
    if (!table_lock_for_mem_index->dumping_mem_index_) {
        return Status::DumpingMemIndex(fmt::format("Table key: {} isn't dumping mem index", table_key));
    }

    table_lock_for_mem_index->dumping_mem_index_ = false;
    if (table_lock_for_mem_index->append_count_ == 0) {
        table_lock_for_mem_index_.erase(table_key);
    } else {
        UnrecoverableError(fmt::format("Table key: {} is appending, why unset mem index dump", table_key));
    }

    return Status::OK();
}

bool NewCatalog::IsMemIndexDump(const String &table_key) {
    std::unique_lock lock(mem_index_mtx_);
    auto iter = table_lock_for_mem_index_.find(table_key);
    if (iter == table_lock_for_mem_index_.end()) {
        return false;
    }

    return iter->second->dumping_mem_index_;
}

Status NewCatalog::AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache) {
    bool insert_success = false;
    HashMap<String, SharedPtr<TableIndexReaderCache>>::iterator iter;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        std::tie(iter, insert_success) = ft_index_cache_map_.emplace(std::move(ft_index_cache_key), std::move(ft_index_cache));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache) {
    ft_index_cache = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(ft_index_cache_mtx_);
        if (auto iter = ft_index_cache_map_.find(ft_index_cache_key); iter != ft_index_cache_map_.end()) {
            ft_index_cache = iter->second;
        }
    }
    if (ft_index_cache == nullptr) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(ft_index_cache_mtx_);
        delete_success = ft_index_cache_map_.erase(ft_index_cache_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("FtIndexCache key: {} not found", ft_index_cache_key));
    }
    return Status::OK();
}

Status NewCatalog::AddSegmentIndexFtInfo(String segment_index_key, SharedPtr<SegmentIndexFtInfo> segment_index_ft_info) {
    bool insert_success = false;
    HashMap<String, SharedPtr<SegmentIndexFtInfo>>::iterator iter;
    {
        std::unique_lock lock(segment_index_ft_info_mtx_);
        std::tie(iter, insert_success) = segment_index_ft_info_map_.emplace(std::move(segment_index_key), std::move(segment_index_ft_info));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentIndexFtInfo(const String &segment_index_key, SharedPtr<SegmentIndexFtInfo> &segment_index_ft_info) {
    segment_index_ft_info = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(segment_index_ft_info_mtx_);
        if (auto iter = segment_index_ft_info_map_.find(segment_index_key); iter != segment_index_ft_info_map_.end()) {
            segment_index_ft_info = iter->second;
        }
    }
    if (segment_index_ft_info == nullptr) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} not found", segment_index_key));
    }
    return Status::OK();
}

Status NewCatalog::DropSegmentIndexFtInfoByKey(const String &segment_index_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(segment_index_ft_info_mtx_);
        delete_success = segment_index_ft_info_map_.erase(segment_index_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("SegmentIndexFtInfo key: {} not found", segment_index_key));
    }
    return Status::OK();
}

Status NewCatalog::AddSegmentUpdateTS(String segment_update_ts_key, SharedPtr<SegmentUpdateTS> segment_update_ts) {
    bool insert_success = false;
    HashMap<String, SharedPtr<SegmentUpdateTS>>::iterator iter;
    {
        std::unique_lock lock(segment_update_ts_mtx_);
        std::tie(iter, insert_success) = segment_update_ts_map_.emplace(std::move(segment_update_ts_key), std::move(segment_update_ts));
    }
    if (!insert_success) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} already exists", iter->first));
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentUpdateTS(const String &segment_update_ts_key, SharedPtr<SegmentUpdateTS> &segment_update_ts) {
    segment_update_ts = nullptr;
    {
        std::shared_lock<std::shared_mutex> lck(segment_update_ts_mtx_);
        if (auto iter = segment_update_ts_map_.find(segment_update_ts_key); iter != segment_update_ts_map_.end()) {
            segment_update_ts = iter->second;
        }
    }
    if (segment_update_ts == nullptr) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} not found", segment_update_ts_key));
    }
    return Status::OK();
}

Status NewCatalog::DropSegmentUpdateTSByKey(const String &segment_update_ts_key) {
    bool delete_success = false;
    {
        std::unique_lock lock(segment_update_ts_mtx_);
        delete_success = segment_update_ts_map_.erase(segment_update_ts_key) > 0;
    }
    if (!delete_success) {
        return Status::CatalogError(fmt::format("SegmentUpdateTS key: {} not found", segment_update_ts_key));
    }
    return Status::OK();
}

void NewCatalog::AddCleanedMeta(TxnTimeStamp ts, UniquePtr<MetaKey> meta) {
    std::unique_lock lock(cleaned_meta_mtx_);

    cleaned_meta_.emplace(ts, std::move(meta));
}

void NewCatalog::GetCleanedMeta(TxnTimeStamp ts, Vector<UniquePtr<MetaKey>> &metas) {
    std::unique_lock lock(cleaned_meta_mtx_);

    auto iter = cleaned_meta_.lower_bound(ts);
    for (auto it = cleaned_meta_.begin(); it != iter; ++it) {
        metas.push_back(std::move(it->second));
    }
    cleaned_meta_.erase(cleaned_meta_.begin(), iter);
}

Status NewCatalog::IncrLatestID(String &id_str, std::string_view id_name) {
    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();
    Status s = kv_instance->Get(id_name.data(), id_str);
    if (!s.ok()) {
        kv_instance->Rollback();
        return s;
    }
    SizeT id_num = std::stoull(id_str);
    ++id_num;
    s = kv_instance->Put(id_name.data(), fmt::format("{}", id_num));
    if (!s.ok()) {
        kv_instance->Rollback();
        return s;
    }
    s = kv_instance->Commit();
    return s;
}

SharedPtr<MetaTree> NewCatalog::MakeMetaTree() const {
    auto kv_instance_ptr = kv_store_->GetInstance();

    Vector<SharedPtr<MetaKey>> entries;
    auto kv_pairs = kv_instance_ptr->GetAllKeyValue();

    for (const auto &[key, value] : kv_pairs) {
        auto meta_key_ptr = infinity::MetaParse(key, value);
        entries.push_back(std::move(meta_key_ptr));
    }

    kv_instance_ptr->Commit();
    auto meta_tree_ptr = MetaTree::MakeMetaTree(entries);
    return meta_tree_ptr;
}

Status NewCatalog::RestoreCatalogCache(Storage *storage_ptr) {
    LOG_INFO("Restore catalog cache");
    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();

    Vector<Pair<String, String>> all_key_values = kv_instance->GetAllKeyValue();
    SizeT meta_count = all_key_values.size();
    Vector<SharedPtr<MetaKey>> meta_keys;
    meta_keys.reserve(meta_count);
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const auto &pair = all_key_values[idx];
        SharedPtr<MetaKey> meta_key = MetaParse(pair.first, pair.second);
        if (meta_key == nullptr) {
            LOG_ERROR(fmt::format("Can't parse {}: {}: {}", idx, pair.first, pair.second));
        } else {
            LOG_INFO(fmt::format("META[{}] KEY: {}", idx, meta_key->ToString()));
        }
        meta_keys.emplace_back(meta_key);
    }
    kv_instance->Commit();
    SharedPtr<MetaTree> meta_tree = MetaTree::MakeMetaTree(meta_keys);
    LOG_INFO(meta_tree->ToJson().dump());

    // system_cache_ = meta_tree->RestoreSystemCache(storage_ptr);
    // Vector<MetaTableObject *> table_ptrs = meta_tree->ListTables();
    // for (const auto &table_ptr : table_ptrs) {
    //     SegmentID unsealed_segment_id = table_ptr->GetUnsealedSegmentID();
    //     SegmentID next_segment_id = table_ptr->GetNextSegmentID();
    //     SizeT current_segment_row_count = table_ptr->GetCurrentSegmentRowCount(storage_ptr);
    //     LOG_INFO(fmt::format("Table: {}, unsealed_segment_id: {}, next_segment_id: {}, current_segment_row_count: {}",
    //                          table_ptr->GetTableName(),
    //                          unsealed_segment_id,
    //                          next_segment_id,
    //                          current_segment_row_count));
    // }

    return Status::OK();
}

SharedPtr<SystemCache> NewCatalog::GetSystemCache() const {
    std::unique_lock lock(catalog_cache_mtx_);
    return system_cache_;
}

u64 NewCatalog::AddNewDbCache() {
    std::unique_lock lock(catalog_cache_mtx_);
    u64 db_id = system_cache_->RequestNextDbID();
    SharedPtr<DbCache> db_cache = MakeShared<DbCache>(db_id, 0);
    system_cache_->AddDbCache(db_cache);
    return db_id;
}

SharedPtr<DbCache> NewCatalog::GetDbCache(u64 db_id) const {
    std::unique_lock lock(catalog_cache_mtx_);
    return system_cache_->GetDbCache(db_id);
}

void NewCatalog::DropDbCache(u64 db_id) { system_cache_->DropDbCache(db_id); }

SharedPtr<TableCache> NewCatalog::GetTableCache(u64 db_id, u64 table_id) const {
    std::unique_lock lock(catalog_cache_mtx_);
    auto db_iter = table_cache_map_.find(db_id);
    if (db_iter == table_cache_map_.end()) {
        return nullptr;
    }
    SharedPtr<HashMap<u64, SharedPtr<TableCache>>> db_map = db_iter->second;
    auto table_iter = db_map->find(table_id);
    if (table_iter == db_map->end()) {
        return nullptr;
    }
    return table_iter->second;
}

Tuple<SharedPtr<TableCache>, Status> NewCatalog::AddNewTableCache(u64 db_id, u64 table_id) {
    std::unique_lock lock(catalog_cache_mtx_);
    auto db_iter = table_cache_map_.find(db_id);
    if (db_iter == table_cache_map_.end()) {
        table_cache_map_[db_id] = MakeShared<HashMap<u64, SharedPtr<TableCache>>>();
        //  return Status::NotFound(fmt::format("Table cache with db id: {} not found", db_id));
    }
    SharedPtr<HashMap<u64, SharedPtr<TableCache>>> &db_map = table_cache_map_[db_id];
    auto table_iter = db_map->find(table_id);
    if (table_iter != db_map->end()) {
        return {nullptr, Status::DuplicateEntry(fmt::format("Table cache with id: {} already exists", table_id))};
    }
    SharedPtr<TableCache> table_cache = MakeShared<TableCache>(db_id, table_id);
    db_map->emplace(table_id, table_cache);
    return {table_cache, Status::OK()};
}

Status NewCatalog::DropTableCache(u64 db_id, u64 table_id) {
    std::unique_lock lock(catalog_cache_mtx_);
    auto db_iter = table_cache_map_.find(db_id);
    if (db_iter == table_cache_map_.end()) {
        return Status::NotFound(fmt::format("Table cache with db id: {} not found", db_id));
    }
    SharedPtr<HashMap<u64, SharedPtr<TableCache>>> db_map = db_iter->second;
    auto table_iter = db_map->find(table_id);
    if (table_iter == db_map->end()) {
        return Status::NotFound(fmt::format("Table cache with id: {} not found", table_id));
    }
    db_map->erase(table_iter);
    return Status::OK();
}

} // namespace infinity

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

#include <re2/re2.h>

#include <filesystem>
#include <ranges>
#include <regex>

module meta_tree;

import stl;
import third_party;
import infinity_context;
import infinity_exception;
import internal_types;
import default_values;
import logger;
import buffer_obj;
import storage;
import config;
import buffer_manager;
import block_version;
import buffer_handle;
import new_catalog;
import status;
import kv_code;
import catalog_cache;
import check_statement;
import kv_utility;
import kv_store;
import new_txn_manager;
import utility;

namespace infinity {

SharedPtr<MetaTree> MetaTree::MakeMetaTree(const Vector<SharedPtr<MetaKey>> &meta_keys) {
    SharedPtr<MetaTree> meta_tree = MakeShared<MetaTree>();
    meta_tree->metas_ = std::move(meta_keys);
    auto &meta_keys_ref = meta_tree->metas_;
    SizeT meta_count = meta_keys_ref.size();
    // Get all dbs
    HashSet<String> db_names, db_ids;
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kDB: {
                auto db_key = static_cast<DBMetaKey *>(meta_key.get());
                auto db_object = MakeShared<MetaDBObject>(meta_key);
                meta_tree->db_map_.emplace(db_key->db_id_str_, db_object);

                // Check if duplicated
                if (db_names.contains(db_key->db_name_)) {
                    String error_message = fmt::format("Duplicate db name: {}, idx: {}", db_key->ToString(), idx);
                    UnrecoverableError(error_message);
                } else {
                    db_names.emplace(db_key->db_name_);
                }

                if (db_ids.contains(db_key->db_id_str_)) {
                    String error_message = fmt::format("Duplicate db id: {}, idx: {}", db_key->ToString(), idx);
                    UnrecoverableError(error_message);
                } else {
                    db_ids.emplace(db_key->db_id_str_);
                }
                break;
            }
            case MetaType::kSystemTag: {
                auto system_tag_key = static_cast<SystemTagMetaKey *>(meta_key.get());
                meta_tree->system_tag_map_.emplace(system_tag_key->tag_name_, system_tag_key->value_);
                break;
            }
            default:
                break;
        }
    }

    // Get all tables and attach to the db
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kTable: {
                auto table_key = static_cast<TableMetaKey *>(meta_key.get());
                auto table_object = MakeShared<MetaTableObject>(meta_key);

                auto iter = meta_tree->db_map_.find(table_key->db_id_str_);
                if (iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found for table: {}, idx: {}", table_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = iter->second->table_map_;
                if (table_map.contains(table_key->table_id_str_)) {
                    String error_message = fmt::format("Duplicate table id: {}, idx: {}", table_key->ToString(), idx);
                    UnrecoverableError(error_message);
                }

                table_map.emplace(table_key->table_id_str_, table_object);
                break;
            }
            case MetaType::kDBTag: {
                auto db_tag_key = static_cast<DBTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(db_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found for tag: {}, idx: {}", db_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }
                auto &db_object = db_iter->second;
                auto &tag_map = db_object->tag_map_;
                if (tag_map.contains(db_tag_key->tag_name_)) {
                    String error_message = fmt::format("Duplicate db tag: {}, idx: {}", db_tag_key->ToString(), idx);
                    UnrecoverableError(error_message);
                }
                tag_map.emplace(db_tag_key->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all table segments / table tag / table index / table column and attach them to the table
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kSegment: {
                auto segment_key = static_cast<SegmentMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", segment_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", segment_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                if (segment_map.contains(segment_key->segment_id_)) {
                    String error_message = fmt::format("Duplicate segment id: {}, idx: {}", segment_key->ToString(), idx);
                    UnrecoverableError(error_message);
                }

                auto segment_object = MakeShared<MetaSegmentObject>(meta_key);
                segment_map.emplace(segment_key->segment_id_, segment_object);
                break;
            }
            case MetaType::kTableTag: {
                auto table_tag_key = static_cast<TableTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", table_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_tag_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", table_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }
                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &tag_map = table_object->tag_map_;
                tag_map.emplace(table_tag_key->tag_name_, meta_key);
                break;
            }
            case MetaType::kTableIndex: {
                auto table_index_meta_key = static_cast<TableIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_index_meta_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", table_index_meta_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_index_meta_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", table_index_meta_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                auto table_index_object = MakeShared<MetaTableIndexObject>(meta_key);
                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                index_map.emplace(table_index_meta_key->index_id_str_, table_index_object);
                break;
            }
            case MetaType::kTableColumn: {
                auto table_column_meta_key = static_cast<TableColumnMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_column_meta_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", table_column_meta_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_column_meta_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", table_column_meta_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &column_map = table_object->column_map_;
                column_map.emplace(table_column_meta_key->column_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all blocks and attach to segment
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kBlock: {
                auto block_key = static_cast<BlockMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_key->segment_id_);
                if (segment_iter == segment_map.end()) {
                    String error_message = fmt::format("Segment not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_message);
                    continue;
                }

                MetaSegmentObject *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto block_object = MakeShared<MetaBlockObject>(meta_key);
                auto &block_map = segment_object->block_map_;
                block_map.emplace(block_key->block_id_, block_object);
                break;
            }
            case MetaType::kSegmentTag: {
                auto segment_tag = static_cast<SegmentTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_tag->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_tag->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(segment_tag->segment_id_);
                if (segment_iter == segment_map.end()) {
                    String error_message = fmt::format("Segment not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    continue;
                }

                MetaSegmentObject *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto &tag_map = segment_object->tag_map_;
                tag_map.emplace(segment_tag->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all block column / block tag and attach to blocks
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kBlockColumn: {
                auto block_key = static_cast<BlockMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_key->segment_id_);
                if (segment_iter == segment_map.end()) {
                    String error_message = fmt::format("Segment not found: {}, idx: {}", block_key->ToString(), idx);
                    UnrecoverableError(error_message);
                }

                MetaSegmentObject *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto block_object = MakeShared<MetaBlockObject>(meta_key);
                auto &block_map = segment_object->block_map_;
                block_map.emplace(block_key->block_id_, block_object);
                break;
            }
            case MetaType::kBlockTag: {
                auto block_tag = static_cast<BlockTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_tag->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_tag->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_tag->segment_id_);
                if (segment_iter == segment_map.end()) {
                    // Segment is compacted
                    String error_message = fmt::format("Segment not found, maybe is compacted: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_message);
                    continue;
                }

                MetaSegmentObject *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto &block_map = segment_object->block_map_;
                auto block_iter = block_map.find(block_tag->block_id_);
                if (block_iter == block_map.end()) {
                    String error_message = fmt::format("Block not found: {}, idx: {}", block_tag->ToString(), idx);
                    UnrecoverableError(error_message);
                }

                MetaBlockObject *block_object = static_cast<MetaBlockObject *>(block_iter->second.get());
                auto &tag_map = block_object->tag_map_;
                tag_map.emplace(block_tag->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all segment indexes and attach to table index
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kSegmentIndex: {
                auto segment_index_key = static_cast<SegmentIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_index_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_index_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(segment_index_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    String error_message = fmt::format("Table index not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // Table index is dropped
                    continue;
                }

                MetaTableIndexObject *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto segment_index_object = MakeShared<MetaSegmentIndexObject>(meta_key);
                table_index_object->segment_map_.emplace(segment_index_key->segment_id_, segment_index_object);
                break;
            }
            case MetaType::kTableIndexTag: {
                auto table_index_tag_key = static_cast<TableIndexTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_index_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_index_tag_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(table_index_tag_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    String error_message = fmt::format("Table index not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // Table index is dropped.
                    continue;
                }

                MetaTableIndexObject *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto &tag_map = table_index_object->tag_map_;
                tag_map.emplace(table_index_tag_key->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all chunk indexes and attach to segment index
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kChunkIndex: {
                auto chunk_index_key = static_cast<ChunkIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(chunk_index_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    String error_message = fmt::format("DB not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(chunk_index_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    String error_message = fmt::format("Table not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableObject *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(chunk_index_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    String error_message = fmt::format("Table index not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    // The Table is dropped
                    continue;
                }

                MetaTableIndexObject *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto &segment_map = table_index_object->segment_map_;
                auto segment_index_iter = segment_map.find(chunk_index_key->segment_id_);
                if (segment_index_iter == segment_map.end()) {
                    String error_message = fmt::format("Segment index not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_message);
                    continue;
                }

                MetaSegmentIndexObject *segment_index_object = static_cast<MetaSegmentIndexObject *>(segment_index_iter->second.get());
                auto &chunk_map = segment_index_object->chunk_map_;
                chunk_map.emplace(chunk_index_key->chunk_id_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all pm object stat
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kPmStat: {
                auto pm_obj_key = static_cast<PmStatMetaKey *>(meta_key.get());
                auto pm_obj_iter = meta_tree->pm_object_map_.find(pm_obj_key->object_key_);
                if (pm_obj_iter != meta_tree->pm_object_map_.end()) {
                    String error_message = fmt::format("Duplicate pm stat key: {}, idx: {}", pm_obj_key->ToString(), idx);
                    UnrecoverableError(error_message);
                }

                auto pm_object = MakeShared<MetaPmObject>(meta_key);
                meta_tree->pm_object_map_.emplace(pm_obj_key->object_key_, pm_object);
                break;
            }
            default:
                break;
        }
    }

    // Get pm object
    for (SizeT idx = 0; idx < meta_count; ++idx) {
        const SharedPtr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kPmObject: {
                auto pm_path_key = static_cast<PmObjectMetaKey *>(meta_key.get());
                nlohmann::json pm_path_json = nlohmann::json::parse(pm_path_key->value_);
                String object_key = pm_path_json["obj_key"];
                if (object_key == "KEY_EMPTY") {
                    continue;
                }
                auto pm_obj_iter = meta_tree->pm_object_map_.find(object_key);
                if (pm_obj_iter == meta_tree->pm_object_map_.end()) {
                    LOG_WARN(fmt::format("PM object not found: {}, idx: {}", pm_path_key->ToString(), idx));
                    continue;
                }

                pm_obj_iter->second->path_map_.emplace(pm_path_key->path_key_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    return meta_tree;
}

nlohmann::json MetaTree::ToJson() const {
    nlohmann::json json_res;
    for (const auto &tag_pair : system_tag_map_) {
        json_res["system_tags"].push_back({{"tag_name", tag_pair.first}, {"value", tag_pair.second}});
    }

    for (const auto &db_pair : db_map_) {
        json_res["databases"].push_back(db_pair.second->ToJson());
    }
    for (const auto &pm : pm_object_map_) {
        json_res["objects"].push_back(pm.second->ToJson());
    }
    return json_res;
}

Vector<MetaTableObject *> MetaTree::ListTables() const {
    Vector<MetaTableObject *> tables;
    for (const auto &db_pair : db_map_) {
        for (const auto &table_pair : db_pair.second->table_map_) {
            tables.push_back(static_cast<MetaTableObject *>(table_pair.second.get()));
        }
    }
    return tables;
};

SharedPtr<SystemCache> MetaTree::RestoreSystemCache(Storage *storage_ptr) const {
    u64 next_db_id{0};
    auto tag_iter = system_tag_map_.find(NEXT_DATABASE_ID.data());
    if (tag_iter != system_tag_map_.end()) {
        try {
            next_db_id = std::stoull(tag_iter->second);
        } catch (const std::exception &e) {
            String error_message = fmt::format("Latest database id is invalid: {}, cause: {}", tag_iter->second, e.what());
            UnrecoverableError(error_message);
        }
    } else {
        String error_message = fmt::format("Can't find 'latest database id' in system tag");
        UnrecoverableError(error_message);
    }

    SharedPtr<SystemCache> system_cache = MakeShared<SystemCache>(next_db_id);
    for (const auto &db_pair : db_map_) {
        MetaDBObject *meta_db_object = static_cast<MetaDBObject *>(db_pair.second.get());
        SharedPtr<DbCache> db_cache = meta_db_object->RestoreDbCache(storage_ptr);
        system_cache->AddDbCacheNolock(db_cache);
    }
    return system_cache;
}

nlohmann::json MetaDBObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &table_pair : table_map_) {
        json_res["tables"].push_back(table_pair.second->ToJson());
    }
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    return json_res;
}

SharedPtr<DbCache> MetaDBObject::RestoreDbCache(Storage *storage_ptr) const {
    auto db_key = static_cast<DBMetaKey *>(meta_key_.get());
    u64 db_id{0};
    try {
        db_id = std::stoull(db_key->db_id_str_);
    } catch (const std::exception &e) {
        String error_message = fmt::format("DB id is invalid: {}, cause: {}", db_key->db_id_str_, e.what());
        UnrecoverableError(error_message);
    }
    SharedPtr<DbCache> db_cache = MakeShared<DbCache>(db_id, db_key->db_name_, 0);
    for (const auto &table_pair : table_map_) {
        MetaTableObject *meta_table_object = static_cast<MetaTableObject *>(table_pair.second.get());
        SharedPtr<TableCache> table_cache = meta_table_object->RestoreTableCache(storage_ptr);
        db_cache->AddTableCacheNolock(table_cache);
    }

    return db_cache;
}

nlohmann::json MetaTableObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &column_pair : column_map_) {
        json_res["columns"].push_back(column_pair.second->ToJson());
    }
    for (const auto &index_pair : index_map_) {
        json_res["indexes"].push_back(index_pair.second->ToJson());
    }

    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }

    for (const auto &segment_pair : segment_map_) {
        json_res["segments"].push_back(segment_pair.second->ToJson());
    }
    return json_res;
}

const String &MetaTableObject::GetTableName() const {
    TableMetaKey *table_meta_key = static_cast<TableMetaKey *>(meta_key_.get());
    return table_meta_key->table_name_;
}

SegmentID MetaTableObject::GetNextSegmentID() const {
    Vector<SegmentID> segments;
    segments.reserve(segment_map_.size());
    for (const auto &segment_pair : segment_map_) {
        segments.push_back(segment_pair.first);
    }
    std::sort(segments.begin(), segments.end());
    SegmentID next_segment_id = segments.empty() ? 0 : segments.back() + 1;
    return next_segment_id;
}

SegmentID MetaTableObject::GetUnsealedSegmentID() const {
    auto meta_iter = tag_map_.find("unsealed_segment_id");
    if (meta_iter == tag_map_.end()) {
        SegmentID next_segment_id = this->GetNextSegmentID();
        String error_message =
            fmt::format("Can't find 'unsealed_segment_id' in table: {}, use next segment id: {}", meta_key_->ToString(), next_segment_id);
        LOG_WARN(error_message);
        return next_segment_id;
    }
    MetaKey *meta_key = meta_iter->second.get();
    TableTagMetaKey *table_tag_meta_key = static_cast<TableTagMetaKey *>(meta_key);

    SegmentID unsealed_segment_id{INVALID_SEGMENT_ID};
    try {
        unsealed_segment_id = std::stoull(table_tag_meta_key->value_);
    } catch (const std::exception &e) {
        String error_message = fmt::format("Unsealed segment id is invalid: {}, cause: {}", table_tag_meta_key->value_, e.what());
        UnrecoverableError(error_message);
    }
    return unsealed_segment_id;
}

SizeT MetaTableObject::GetCurrentSegmentRowCount(Storage *storage_ptr) const {
    if (segment_map_.empty()) {
        return 0;
    }
    SegmentID unsealed_segment_id = this->GetUnsealedSegmentID();
    auto seg_iter = segment_map_.find(unsealed_segment_id);
    if (seg_iter == segment_map_.end()) {
        String error_message = fmt::format("Can't find unsealed segment id: {}, table: {}", unsealed_segment_id, meta_key_->ToString());
        LOG_WARN(error_message);
        return 0;
    }
    auto segment_object = static_cast<MetaSegmentObject *>(seg_iter->second.get());
    SizeT block_count = segment_object->block_map_.size();
    if (block_count == 0) {
        return 0;
    }
    TableMetaKey *table_meta_key = static_cast<TableMetaKey *>(meta_key_.get());
    BlockID current_block_id = segment_object->GetCurrentBlockID();
    BufferManager *buffer_mgr_ptr = storage_ptr->buffer_manager();
    Config *config_ptr = storage_ptr->config();
    String version_filepath = fmt::format("{}/db_{}/tbl_{}/seg_{}/blk_{}/{}",
                                          config_ptr->DataDir(),
                                          table_meta_key->db_id_str_,
                                          table_meta_key->table_id_str_,
                                          unsealed_segment_id,
                                          current_block_id,
                                          BlockVersion::PATH);
    BufferObj *version_buffer = buffer_mgr_ptr->GetBufferObject(version_filepath);
    if (version_buffer == nullptr) {
        UnrecoverableError(fmt::format("Can't get version from: {}", version_filepath));
    }
    BufferHandle buffer_handle = version_buffer->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(buffer_handle.GetData());
    SizeT row_cnt = 0;
    {
        SharedPtr<BlockLock> block_lock{};
        NewCatalog *new_catalog = storage_ptr->new_catalog();
        String block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(table_meta_key->db_id_str_,
                                                                          table_meta_key->table_id_str_,
                                                                          unsealed_segment_id,
                                                                          current_block_id,
                                                                          "lock");
        Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        std::shared_lock lock(block_lock->mtx_);
        row_cnt = block_version->GetRowCount();
    }
    SizeT segment_row_count = (block_count - 1) * DEFAULT_BLOCK_CAPACITY + row_cnt;
    return segment_row_count;
}

SharedPtr<TableCache> MetaTableObject::RestoreTableCache(Storage *storage_ptr) const {
    auto table_key = static_cast<TableMetaKey *>(meta_key_.get());
    u64 db_id = 0;
    u64 table_id = 0;
    SegmentID unsealed_segment_id = 0;
    SegmentOffset unsealed_segment_offset = 0;
    SegmentID next_segment_id = 0;
    try {
        db_id = std::stoull(table_key->db_id_str_);
        table_id = std::stoull(table_key->table_id_str_);
        unsealed_segment_id = this->GetUnsealedSegmentID();
        unsealed_segment_offset = this->GetCurrentSegmentRowCount(storage_ptr);
        next_segment_id = this->GetNextSegmentID();
    } catch (const std::exception &e) {
        String error_message = fmt::format("DB id or table id is invalid: {}, cause: {}", table_key->ToString(), e.what());
        UnrecoverableError(error_message);
    }

    SharedPtr<TableCache> table_cache = nullptr;
    if (unsealed_segment_id == 0 and unsealed_segment_offset == 0) {
        table_cache = MakeShared<TableCache>(table_id, table_key->table_name_);
    } else {
        table_cache = MakeShared<TableCache>(table_id, unsealed_segment_id, unsealed_segment_offset, next_segment_id);
    }

    TxnTimeStamp current_ts = storage_ptr->new_txn_manager()->CurrentTS();
    KVStore *kv_store = storage_ptr->new_catalog()->kv_store();
    auto kv_instance_ptr = kv_store->GetInstance();
    for (const auto &segment_pair : segment_map_) {
        SegmentID segment_id = segment_pair.first;
        SharedPtr<SegmentCache> segment_cache = nullptr;
        if (segment_id == unsealed_segment_id) {
            table_cache->unsealed_segment_cache_ = MakeShared<SegmentCache>(segment_id, unsealed_segment_offset);
        } else {
            SizeT segment_row_count = infinity::GetSegmentRowCount(kv_instance_ptr.get(),
                                                                   table_key->db_id_str_,
                                                                   table_key->table_id_str_,
                                                                   segment_id,
                                                                   current_ts,
                                                                   MAX_TIMESTAMP);
            segment_cache = MakeShared<SegmentCache>(segment_id, segment_row_count);
        }
        table_cache->segment_cache_map_.emplace(segment_id, segment_cache);
    }

    for (const auto &index_pair : index_map_) {
        const String &index_name = index_pair.first;
        MetaTableIndexObject *table_index_obj = static_cast<MetaTableIndexObject *>(index_pair.second.get());
        TableIndexMetaKey *table_index_meta_key = static_cast<TableIndexMetaKey *>(table_index_obj->meta_key_.get());
        u64 index_id = std::stoull(table_index_meta_key->index_id_str_);
        SharedPtr<TableIndexCache> table_index_cache = MakeShared<TableIndexCache>(db_id, table_id, index_id, index_name);
        for (const auto &segment_index_pair : table_index_obj->segment_map_) {
            SegmentID segment_id = segment_index_pair.first;
            MetaSegmentIndexObject *segment_index_obj = static_cast<MetaSegmentIndexObject *>(segment_index_pair.second.get());
            SharedPtr<SegmentIndexCache> segment_index_cache = MakeShared<SegmentIndexCache>(segment_id);
            ChunkID max_chunk_id = 0;
            for (const auto &chunk_index_pair : segment_index_obj->chunk_map_) {
                ChunkID chunk_id = chunk_index_pair.first;
                if (chunk_id > max_chunk_id) {
                    max_chunk_id = chunk_id;
                }
            }
            segment_index_cache->next_chunk_id_ = max_chunk_id + 1;
            table_index_cache->segment_index_cache_map_.emplace(segment_id, segment_index_cache);
        }
        table_cache->index_cache_map_.emplace(index_id, table_index_cache);
    }

    return table_cache;
}

nlohmann::json MetaSegmentObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &block_pair : block_map_) {
        json_res["blocks"].push_back(block_pair.second->ToJson());
    }
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    return json_res;
}

BlockID MetaSegmentObject::GetCurrentBlockID() const {
    Vector<BlockID> blocks;
    blocks.reserve(block_map_.size());
    for (const auto &block_pair : block_map_) {
        blocks.push_back(block_pair.first);
    }
    std::sort(blocks.begin(), blocks.end());
    BlockID current_block_id = blocks.empty() ? 0 : blocks.back();
    return current_block_id;
}

nlohmann::json MetaBlockObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &column_pair : column_map_) {
        json_res["columns"].push_back(column_pair.second->ToJson());
    }
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    return json_res;
}

nlohmann::json MetaBlockColumnObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    return json_res;
}

nlohmann::json MetaTableIndexObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    for (const auto &segment_pair : segment_map_) {
        json_res["segments"].push_back(segment_pair.second->ToJson());
    }
    return json_res;
}

SharedPtr<TableIndexCache> MetaTableIndexObject::RestoreTableIndexCache(Storage *storage_ptr) const {
    auto table_index_key = static_cast<TableIndexMetaKey *>(meta_key_.get());
    u64 db_id{};
    u64 table_id{};
    u64 index_id{};
    try {
        db_id = std::stoull(table_index_key->db_id_str_);
        table_id = std::stoull(table_index_key->table_id_str_);
        index_id = std::stoull(table_index_key->index_id_str_);
    } catch (const std::exception &e) {
        String error_message = fmt::format("DB id or table id is invalid: {}, cause: {}", table_index_key->ToString(), e.what());
        UnrecoverableError(error_message);
    }
    SharedPtr<TableIndexCache> table_index_cache = MakeShared<TableIndexCache>(db_id, table_id, index_id, table_index_key->index_name_);
    return table_index_cache;
}

nlohmann::json MetaSegmentIndexObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &chunk_pair : chunk_map_) {
        json_res["chunks"].push_back(chunk_pair.second->ToJson());
    }
    return json_res;
}

nlohmann::json MetaChunkIndexObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &tag_pair : tag_map_) {
        json_res["tags"].push_back(tag_pair.second->ToJson());
    }
    return json_res;
}

nlohmann::json MetaPmObject::ToJson() const {
    nlohmann::json json_res = meta_key_->ToJson();
    for (const auto &path_pair : path_map_) {
        json_res["paths"].push_back(path_pair.second->ToJson());
    }
    return json_res;
}

bool MetaTree::PathFilter(std::string_view path, CheckStmtType tag, Optional<String> db_table_str) {
    switch (tag) {
        case CheckStmtType::kSystem: {
            return true;
        }
        case CheckStmtType::kTable: {
            auto table_str = fmt::format("{}", db_table_str.value());
            return path.find(table_str) != String::npos;
        }
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid entity tag");
        }
        default:
            return false;
    }
    return true;
}

bool MetaTree::ExistInMetas(MetaType meta_type, const std::function<bool(MetaKey *)> &predicate) const {
    return std::any_of(metas_.begin(), metas_.end(), [&](auto &meta) { return meta->type_ == meta_type && predicate(meta.get()); });
}

std::function<bool(MetaKey *)> MetaTree::MakeColumnPredicate(std::string_view db_id_str, std::string_view table_id_str, ColumnID column_id) {
    return [=](MetaKey *meta) {
        auto *table_column_meta = static_cast<TableColumnMetaKey *>(meta);
        auto json = table_column_meta->ToJson();
        auto meta_column_id = static_cast<ColumnID>(json[0]["column_id"]);
        return table_column_meta->db_id_str_ == db_id_str && table_column_meta->table_id_str_ == table_id_str && meta_column_id == column_id;
    };
}

std::function<bool(MetaKey *)>
MetaTree::MakeBlockPredicate(std::string_view db_id_str, std::string_view table_id_str, SegmentID segment_id, BlockID block_id) {
    return [=](MetaKey *meta) {
        auto *block_meta = static_cast<BlockMetaKey *>(meta);
        return block_meta->db_id_str_ == db_id_str && block_meta->table_id_str_ == table_id_str && block_meta->segment_id_ == segment_id &&
               block_meta->block_id_ == block_id;
    };
}

std::function<bool(MetaKey *)> MetaTree::MakeSegmentPredicate(std::string_view db_id_str, std::string_view table_id_str, SegmentID segment_id) {
    return [=](MetaKey *meta) {
        auto *segment_meta = static_cast<SegmentMetaKey *>(meta);
        return segment_meta->db_id_str_ == db_id_str && segment_meta->table_id_str_ == table_id_str && segment_meta->segment_id_ == segment_id;
    };
}

std::function<bool(MetaKey *)>
MetaTree::MakeSegmentIndexPredicate(std::string_view db_id_str, std::string_view table_id_str, std::string_view index_id_str, SegmentID segment_id) {
    return [=](MetaKey *meta) {
        auto *seg_idx = static_cast<SegmentIndexMetaKey *>(meta);
        return seg_idx->db_id_str_ == db_id_str && seg_idx->table_id_str_ == table_id_str && seg_idx->index_id_str_ == index_id_str &&
               seg_idx->segment_id_ == segment_id;
    };
}

std::function<bool(MetaKey *)> MetaTree::MakeChunkIndexPredicate(std::string_view db_id_str,
                                                                 std::string_view table_id_str,
                                                                 std::string_view index_id_str,
                                                                 SegmentID segment_id,
                                                                 ChunkID chunk_id) {
    return [=](MetaKey *meta) {
        auto *chunk_idx = static_cast<ChunkIndexMetaKey *>(meta);
        return chunk_idx->db_id_str_ == db_id_str && chunk_idx->table_id_str_ == table_id_str && chunk_idx->index_id_str_ == index_id_str &&
               chunk_idx->segment_id_ == segment_id && chunk_idx->chunk_id_ == chunk_id;
    };
}

std::function<bool(MetaKey *)>
MetaTree::MakeTableIndexPredicate(std::string_view db_id_str, std::string_view table_id_str, std::string_view index_id_str) {
    return [=](MetaKey *meta) {
        auto *tbl_idx = static_cast<TableIndexMetaKey *>(meta);
        return tbl_idx->db_id_str_ == db_id_str && tbl_idx->table_id_str_ == table_id_str && tbl_idx->index_id_str_ == index_id_str;
    };
}

bool MetaTree::CheckData(const String &path) {
    static const RE2 regex_column(R"(^(?:db_(\d+)/tbl_(\d+)/seg_(\d+)/blk_(\d+)/(?:version|(\d+)\.col|col_(\d+)_out))$)");

    static const RE2 regex_index(R"(^(?:db_(\d+)/tbl_(\d+)/idx_(\d+)/seg_(\d+)/(?:chunk_(\d+)\.idx|ft_(\d+)\.(\w+)))$)");

    re2::StringPiece sp1, sp2, sp3, sp4, sp5, sp6, sp7;
    String db_id_str, table_id_str, index_id_str, suffix_id, suffix;
    SegmentID segment_id = INVALID_SEGMENT_ID;
    BlockID block_id = INVALID_BLOCK_ID;

    if (RE2::FullMatch(path, regex_column, &sp1, &sp2, &sp3, &sp4, &sp5, &sp6)) {
        // db_{1}, tbl_{2}, seg_{3}, blk_{4}, {5}.col, col_{6}_out
        db_id_str = sp1.as_string();
        table_id_str = sp2.as_string();
        segment_id = static_cast<SegmentID>(std::stoull(sp3.as_string()));
        block_id = static_cast<BlockID>(std::stoull(sp4.as_string()));

        if (!sp5.empty()) {
            suffix_id = sp5.as_string();
            suffix = "col";
        } else if (!sp6.empty()) {
            suffix_id = sp6.as_string();
            suffix = "out";
        } else {
            suffix = "version";
        }
    } else if (RE2::FullMatch(path, regex_index, &sp1, &sp2, &sp3, &sp4, &sp5, &sp6, &sp7)) {
        // db_{1}, tbl_{2}, idx_{3}, seg_{4},
        // chunk_{5}.idx,
        // ft_{6}.{7}   sp[7] == len | pos | dic
        db_id_str = sp1.as_string();
        table_id_str = sp2.as_string();
        index_id_str = sp3.as_string();
        segment_id = static_cast<SegmentID>(std::stoull(sp4.as_string()));

        if (!sp5.empty()) {
            suffix_id = sp5.as_string();
            suffix = "idx";
        } else {
            suffix_id = sp6.as_string();
            suffix = sp7.as_string(); // len | pos | dic
        }
    } else {
        return true;
    }

    if (suffix == "dic" || suffix == "pos" || suffix == "len") {
        // TODO...
        return false;
    }
    if (suffix == "version") {
        return !ExistInMetas(MetaType::kSegment, MakeSegmentPredicate(db_id_str, table_id_str, segment_id)) ||
               !ExistInMetas(MetaType::kBlock, MakeBlockPredicate(db_id_str, table_id_str, segment_id, block_id));
    }
    if (suffix == "col" || suffix == "out") {
        auto column_id = static_cast<ColumnID>(std::stoull(suffix_id));
        return !ExistInMetas(MetaType::kSegment, MakeSegmentPredicate(db_id_str, table_id_str, segment_id)) ||
               !ExistInMetas(MetaType::kBlock, MakeBlockPredicate(db_id_str, table_id_str, segment_id, block_id)) ||
               !ExistInMetas(MetaType::kTableColumn, MakeColumnPredicate(db_id_str, table_id_str, column_id));
    }
    if (suffix == "idx") {
        auto chunk_id = static_cast<ChunkID>(std::stoull(suffix_id));
        return !ExistInMetas(MetaType::kSegmentIndex, MakeSegmentIndexPredicate(db_id_str, table_id_str, index_id_str, segment_id)) ||
               !ExistInMetas(MetaType::kChunkIndex, MakeChunkIndexPredicate(db_id_str, table_id_str, index_id_str, segment_id, chunk_id)) ||
               !ExistInMetas(MetaType::kTableIndex, MakeTableIndexPredicate(db_id_str, table_id_str, index_id_str));
    }
    return true;
}

HashSet<String> MetaTree::GetDataVfsPathSet() {
    HashSet<String> data_path_set;
    const auto *pm = InfinityContext::instance().storage()->buffer_manager()->persistence_manager();
    for (auto files = pm->GetAllFiles(); const auto &path : files | std::views::keys) {
        data_path_set.emplace(path);
    }
    return data_path_set;
}

HashSet<String> MetaTree::GetDataVfsOffPathSet() {
    HashSet<String> data_path_set;
    Path data_dir = InfinityContext::instance().config()->DataDir();

    for (const auto &entry : std::filesystem::recursive_directory_iterator{data_dir}) {
        if (entry.is_regular_file()) {
            auto relative_path = std::filesystem::relative(entry.path(), data_dir);
            data_path_set.emplace(relative_path.string());
        }
    }
    return data_path_set;
}

Vector<String> MetaTree::CheckMetaDataMapping(CheckStmtType tag, Optional<String> db_table_str) {
    const auto *pm = InfinityContext::instance().storage()->buffer_manager()->persistence_manager();
    auto data_path_set = pm != nullptr ? this->GetDataVfsPathSet() : this->GetDataVfsOffPathSet();

    Vector<String> data_mismatch_entry;

    for (auto &path : data_path_set) {
        if (CheckData(path) && PathFilter(path, tag, db_table_str)) {
            data_mismatch_entry.emplace_back(path);
        }
    }

    return data_mismatch_entry;
}

} // namespace infinity
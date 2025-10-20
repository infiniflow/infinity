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

#include <ranges>

module infinity_core:meta_tree.impl;

import :meta_tree;
import :infinity_context;
import :infinity_exception;
import :default_values;
import :logger;
import :storage;
import :config;
import :fileworker_manager;
import :block_version;
import :new_catalog;
import :status;
import :kv_code;
import :catalog_cache;
import :kv_utility;
import :kv_store;
import :new_txn_manager;
import :utility;
import :infinity_type;

import std.compat;
import third_party;

import check_statement;
import internal_types;

namespace infinity {

std::shared_ptr<MetaTree> MetaTree::MakeMetaTree(const std::vector<std::shared_ptr<MetaKey>> &meta_keys) {
    std::shared_ptr<MetaTree> meta_tree = std::make_shared<MetaTree>();
    meta_tree->metas_ = std::move(meta_keys);
    auto &meta_keys_ref = meta_tree->metas_;
    size_t meta_count = meta_keys_ref.size();
    // Get all dbs
    std::unordered_set<std::string> db_names, db_ids;
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kDB: {
                auto db_key = static_cast<DBMetaKey *>(meta_key.get());
                auto db_object = std::make_shared<MetaDBObject>(meta_key);
                meta_tree->db_map_.emplace(db_key->db_id_str_, db_object);

                // Check if duplicated
                if (db_names.contains(db_key->db_name_)) {
                    UnrecoverableError(fmt::format("Duplicate db name: {}, idx: {}", db_key->ToString(), idx));
                } else {
                    db_names.emplace(db_key->db_name_);
                }

                if (db_ids.contains(db_key->db_id_str_)) {
                    UnrecoverableError(fmt::format("Duplicate db id: {}, idx: {}", db_key->ToString(), idx));
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
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kTable: {
                auto table_key = static_cast<TableMetaKey *>(meta_key.get());
                auto table_object = std::make_shared<MetaTableObject>(meta_key);

                auto iter = meta_tree->db_map_.find(table_key->db_id_str_);
                if (iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found for table: {}, idx: {}", table_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = iter->second->table_map_;
                if (table_map.contains(table_key->table_id_str_)) {
                    UnrecoverableError(fmt::format("Duplicate table id: {}, idx: {}", table_key->ToString(), idx));
                }

                table_map.emplace(table_key->table_id_str_, table_object);
                break;
            }
            case MetaType::kDBTag: {
                auto db_tag_key = static_cast<DBTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(db_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found for tag: {}, idx: {}", db_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }
                auto &db_object = db_iter->second;
                auto &tag_map = db_object->tag_map_;
                if (tag_map.contains(db_tag_key->tag_name_)) {
                    UnrecoverableError(fmt::format("Duplicate db tag: {}, idx: {}", db_tag_key->ToString(), idx));
                }
                tag_map.emplace(db_tag_key->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all table segments / table tag / table index / table column and attach them to the table
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kSegment: {
                auto segment_key = static_cast<SegmentMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", segment_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", segment_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                if (segment_map.contains(segment_key->segment_id_)) {
                    UnrecoverableError(fmt::format("Duplicate segment id: {}, idx: {}", segment_key->ToString(), idx));
                }

                auto segment_object = std::make_shared<MetaSegmentObject>(meta_key);
                segment_map.emplace(segment_key->segment_id_, segment_object);
                break;
            }
            case MetaType::kTableTag: {
                auto table_tag_key = static_cast<TableTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", table_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_tag_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", table_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }
                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &tag_map = table_object->tag_map_;
                tag_map.emplace(table_tag_key->tag_name_, meta_key);
                break;
            }
            case MetaType::kTableIndex: {
                auto table_index_meta_key = static_cast<TableIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_index_meta_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", table_index_meta_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_index_meta_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", table_index_meta_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto table_index_object = std::make_shared<MetaTableIndexObject>(meta_key);
                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                index_map.emplace(table_index_meta_key->index_id_str_, table_index_object);
                break;
            }
            case MetaType::kTableColumn: {
                auto table_column_meta_key = static_cast<TableColumnMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_column_meta_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", table_column_meta_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_column_meta_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", table_column_meta_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &column_map = table_object->column_map_;
                column_map.emplace(table_column_meta_key->column_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all blocks and attach to segment
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kBlock: {
                auto block_key = static_cast<BlockMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_key->segment_id_);
                if (segment_iter == segment_map.end()) {
                    std::string error_msg = fmt::format("Segment not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    continue;
                }

                auto *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto block_object = std::make_shared<MetaBlockObject>(meta_key);
                auto &block_map = segment_object->block_map_;
                block_map.emplace(block_key->block_id_, block_object);
                break;
            }
            case MetaType::kSegmentTag: {
                auto segment_tag = static_cast<SegmentTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_tag->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_tag->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(segment_tag->segment_id_);
                if (segment_iter == segment_map.end()) {
                    std::string error_msg = fmt::format("Segment not found: {}, idx: {}", segment_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    continue;
                }

                auto *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto &tag_map = segment_object->tag_map_;
                tag_map.emplace(segment_tag->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all block column / block tag and attach to blocks
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kBlockColumn: {
                auto block_key = static_cast<BlockMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", block_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_key->segment_id_);
                if (segment_iter == segment_map.end()) {
                    UnrecoverableError(fmt::format("Segment not found: {}, idx: {}", block_key->ToString(), idx));
                }

                auto *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto block_object = std::make_shared<MetaBlockObject>(meta_key);
                auto &block_map = segment_object->block_map_;
                block_map.emplace(block_key->block_id_, block_object);
                break;
            }
            case MetaType::kBlockTag: {
                auto block_tag = static_cast<BlockTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(block_tag->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(block_tag->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &segment_map = table_object->segment_map_;
                auto segment_iter = segment_map.find(block_tag->segment_id_);
                if (segment_iter == segment_map.end()) {
                    // Segment is compacted
                    std::string error_msg = fmt::format("Segment not found, maybe is compacted: {}, idx: {}", block_tag->ToString(), idx);
                    LOG_WARN(error_msg);
                    continue;
                }

                auto *segment_object = static_cast<MetaSegmentObject *>(segment_iter->second.get());
                auto &block_map = segment_object->block_map_;
                auto block_iter = block_map.find(block_tag->block_id_);
                if (block_iter == block_map.end()) {
                    UnrecoverableError(fmt::format("Block not found: {}, idx: {}", block_tag->ToString(), idx));
                }

                auto *block_object = static_cast<MetaBlockObject *>(block_iter->second.get());
                auto &tag_map = block_object->tag_map_;
                tag_map.emplace(block_tag->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all segment indexes and attach to table index
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kSegmentIndex: {
                auto segment_index_key = static_cast<SegmentIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(segment_index_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(segment_index_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(segment_index_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    std::string error_msg = fmt::format("Table index not found: {}, idx: {}", segment_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // Table index is dropped
                    continue;
                }

                auto *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto segment_index_object = std::make_shared<MetaSegmentIndexObject>(meta_key);
                table_index_object->segment_map_.emplace(segment_index_key->segment_id_, segment_index_object);
                break;
            }
            case MetaType::kTableIndexTag: {
                auto table_index_tag_key = static_cast<TableIndexTagMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(table_index_tag_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(table_index_tag_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(table_index_tag_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    std::string error_msg = fmt::format("Table index not found: {}, idx: {}", table_index_tag_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // Table index is dropped.
                    continue;
                }

                auto *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto &tag_map = table_index_object->tag_map_;
                tag_map.emplace(table_index_tag_key->tag_name_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all chunk indexes and attach to segment index
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kChunkIndex: {
                auto chunk_index_key = static_cast<ChunkIndexMetaKey *>(meta_key.get());
                auto db_iter = meta_tree->db_map_.find(chunk_index_key->db_id_str_);
                if (db_iter == meta_tree->db_map_.end()) {
                    std::string error_msg = fmt::format("DB not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // DB is dropped
                    continue;
                }

                auto &table_map = db_iter->second->table_map_;
                auto table_iter = table_map.find(chunk_index_key->table_id_str_);
                if (table_iter == table_map.end()) {
                    std::string error_msg = fmt::format("Table not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_object = static_cast<MetaTableObject *>(table_iter->second.get());
                auto &index_map = table_object->index_map_;
                auto table_index_iter = index_map.find(chunk_index_key->index_id_str_);
                if (table_index_iter == index_map.end()) {
                    std::string error_msg = fmt::format("Table index not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    // The Table is dropped
                    continue;
                }

                auto *table_index_object = static_cast<MetaTableIndexObject *>(table_index_iter->second.get());
                auto &segment_map = table_index_object->segment_map_;
                auto segment_index_iter = segment_map.find(chunk_index_key->segment_id_);
                if (segment_index_iter == segment_map.end()) {
                    std::string error_msg = fmt::format("Segment index not found: {}, idx: {}", chunk_index_key->ToString(), idx);
                    LOG_WARN(error_msg);
                    continue;
                }

                auto *segment_index_object = static_cast<MetaSegmentIndexObject *>(segment_index_iter->second.get());
                auto &chunk_map = segment_index_object->chunk_map_;
                chunk_map.emplace(chunk_index_key->chunk_id_, meta_key);
                break;
            }
            default:
                break;
        }
    }

    // Get all pm object stat
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kPmStat: {
                auto pm_obj_key = static_cast<PmStatMetaKey *>(meta_key.get());
                auto pm_obj_iter = meta_tree->pm_object_map_.find(pm_obj_key->object_key_);
                if (pm_obj_iter != meta_tree->pm_object_map_.end()) {
                    UnrecoverableError(fmt::format("Duplicate pm stat key: {}, idx: {}", pm_obj_key->ToString(), idx));
                }

                auto pm_object = std::make_shared<MetaPmObject>(meta_key);
                meta_tree->pm_object_map_.emplace(pm_obj_key->object_key_, pm_object);
                break;
            }
            default:
                break;
        }
    }

    // Get pm object
    for (size_t idx = 0; idx < meta_count; ++idx) {
        const std::shared_ptr<MetaKey> &meta_key = meta_keys_ref[idx];
        switch (meta_key->type_) {
            case MetaType::kPmObject: {
                auto pm_path_key = static_cast<PmObjectMetaKey *>(meta_key.get());
                simdjson::padded_string json(pm_path_key->value_);
                simdjson::parser parser;
                simdjson::document doc = parser.iterate(json);
                std::string object_key = doc["obj_key"].get<std::string>();
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

std::vector<MetaTableObject *> MetaTree::ListTables() const {
    std::vector<MetaTableObject *> tables;
    for (const auto &db_pair : db_map_) {
        for (const auto &table_pair : db_pair.second->table_map_) {
            tables.push_back(static_cast<MetaTableObject *>(table_pair.second.get()));
        }
    }
    return tables;
};

std::unique_ptr<SystemCache> MetaTree::RestoreSystemCache(Storage *storage_ptr) const {
    u64 next_db_id{0};
    auto tag_iter = system_tag_map_.find(NEXT_DATABASE_ID.data());
    if (tag_iter != system_tag_map_.end()) {
        try {
            next_db_id = std::stoull(tag_iter->second);
        } catch (const std::exception &e) {
            UnrecoverableError(fmt::format("Latest database id is invalid: {}, cause: {}", tag_iter->second, e.what()));
        }
    } else {
        UnrecoverableError(fmt::format("Can't find 'latest database id' in system tag"));
    }

    std::unique_ptr<SystemCache> system_cache = std::make_unique<SystemCache>(next_db_id);
    for (const auto &db_pair : db_map_) {
        MetaDBObject *meta_db_object = static_cast<MetaDBObject *>(db_pair.second.get());
        std::shared_ptr<DbCache> db_cache = meta_db_object->RestoreDbCache(storage_ptr);
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

std::shared_ptr<DbCache> MetaDBObject::RestoreDbCache(Storage *storage_ptr) const {
    auto db_key = static_cast<DBMetaKey *>(meta_key_.get());
    u64 db_id{0};
    try {
        db_id = std::stoull(db_key->db_id_str_);
    } catch (const std::exception &e) {
        UnrecoverableError(fmt::format("DB id is invalid: {}, cause: {}", db_key->db_id_str_, e.what()));
    }
    std::shared_ptr<DbCache> db_cache = std::make_shared<DbCache>(db_id, db_key->db_name_, 0);
    for (const auto &table_pair : table_map_) {
        MetaTableObject *meta_table_object = static_cast<MetaTableObject *>(table_pair.second.get());
        std::shared_ptr<TableCache> table_cache = meta_table_object->RestoreTableCache(storage_ptr);
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

const std::string &MetaTableObject::GetTableName() const {
    TableMetaKey *table_meta_key = static_cast<TableMetaKey *>(meta_key_.get());
    return table_meta_key->table_name_;
}

SegmentID MetaTableObject::GetNextSegmentID() const {
    std::vector<SegmentID> segments;
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
        SegmentID next_segment_id = GetNextSegmentID();
        std::string error_msg =
            fmt::format("Can't find 'unsealed_segment_id' in table: {}, use next segment id: {}", meta_key_->ToString(), next_segment_id);
        LOG_WARN(error_msg);
        return next_segment_id;
    }
    MetaKey *meta_key = meta_iter->second.get();
    TableTagMetaKey *table_tag_meta_key = static_cast<TableTagMetaKey *>(meta_key);

    SegmentID unsealed_segment_id{INVALID_SEGMENT_ID};
    try {
        unsealed_segment_id = std::stoull(table_tag_meta_key->value_);
    } catch (const std::exception &e) {
        UnrecoverableError(fmt::format("Unsealed segment id is invalid: {}, cause: {}", table_tag_meta_key->value_, e.what()));
    }
    return unsealed_segment_id;
}

size_t MetaTableObject::GetCurrentSegmentRowCount(Storage *storage_ptr) const {
    if (segment_map_.empty()) {
        return 0;
    }
    SegmentID unsealed_segment_id = GetUnsealedSegmentID();
    auto seg_iter = segment_map_.find(unsealed_segment_id);
    if (seg_iter == segment_map_.end()) {
        std::string error_msg = fmt::format("Can't find unsealed segment id: {}, table: {}", unsealed_segment_id, meta_key_->ToString());
        LOG_WARN(error_msg);
        return 0;
    }
    auto segment_object = static_cast<MetaSegmentObject *>(seg_iter->second.get());
    size_t block_count = segment_object->block_map_.size();
    if (block_count == 0) {
        return 0;
    }
    TableMetaKey *table_meta_key = static_cast<TableMetaKey *>(meta_key_.get());
    BlockID current_block_id = segment_object->GetCurrentBlockID();
    auto *file_worker_mgr = storage_ptr->fileworker_manager();
    auto rel_version_path = fmt::format("db_{}/tbl_{}/seg_{}/blk_{}/{}",
                                             table_meta_key->db_id_str_,
                                             table_meta_key->table_id_str_,
                                             unsealed_segment_id,
                                             current_block_id,
                                             BlockVersion::PATH);
    auto *version_file_worker = file_worker_mgr->GetFileWorker(rel_version_path);
    if (version_file_worker == nullptr) {
        UnrecoverableError(fmt::format("Can't get version from: {}", rel_version_path));
    }
    const BlockVersion *block_version{};
    version_file_worker->Read(block_version);
    size_t row_cnt = 0;
    {
        std::shared_ptr<BlockLock> block_lock{};
        NewCatalog *new_catalog = storage_ptr->new_catalog();
        std::string block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(table_meta_key->db_id_str_,
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
    size_t segment_row_count = (block_count - 1) * DEFAULT_BLOCK_CAPACITY + row_cnt;
    return segment_row_count;
}

std::shared_ptr<TableCache> MetaTableObject::RestoreTableCache(Storage *storage_ptr) const {
    auto table_key = static_cast<TableMetaKey *>(meta_key_.get());
    u64 db_id = 0;
    u64 table_id = 0;
    SegmentID unsealed_segment_id = 0;
    SegmentOffset unsealed_segment_offset = 0;
    SegmentID next_segment_id = 0;
    try {
        db_id = std::stoull(table_key->db_id_str_);
        table_id = std::stoull(table_key->table_id_str_);
        unsealed_segment_id = GetUnsealedSegmentID();
        unsealed_segment_offset = GetCurrentSegmentRowCount(storage_ptr);
        next_segment_id = GetNextSegmentID();
    } catch (const std::exception &e) {
        UnrecoverableError(fmt::format("DB id or table id is invalid: {}, cause: {}", table_key->ToString(), e.what()));
    }

    std::shared_ptr<TableCache> table_cache = nullptr;
    if (unsealed_segment_id == 0 and unsealed_segment_offset == 0) {
        table_cache = std::make_shared<TableCache>(table_id, table_key->table_name_);
    } else {
        table_cache = std::make_shared<TableCache>(table_id, unsealed_segment_id, unsealed_segment_offset, next_segment_id);
    }

    TxnTimeStamp current_ts = storage_ptr->new_txn_manager()->CurrentTS();
    KVStore *kv_store = storage_ptr->new_catalog()->kv_store();
    auto kv_instance_ptr = kv_store->GetInstance();
    for (const auto &segment_pair : segment_map_) {
        SegmentID segment_id = segment_pair.first;
        std::shared_ptr<SegmentCache> segment_cache = nullptr;
        if (segment_id == unsealed_segment_id) {
            table_cache->unsealed_segment_cache_ = std::make_shared<SegmentCache>(segment_id, unsealed_segment_offset);
        } else {
            size_t segment_row_count = infinity::GetSegmentRowCount(kv_instance_ptr.get(),
                                                                    table_key->db_id_str_,
                                                                    table_key->table_id_str_,
                                                                    segment_id,
                                                                    current_ts,
                                                                    MAX_TIMESTAMP);
            segment_cache = std::make_shared<SegmentCache>(segment_id, segment_row_count);
        }
        table_cache->segment_cache_map_.emplace(segment_id, segment_cache);
    }

    for (const auto &index_pair : index_map_) {
        const std::string &index_name = index_pair.first;
        MetaTableIndexObject *table_index_obj = static_cast<MetaTableIndexObject *>(index_pair.second.get());
        TableIndexMetaKey *table_index_meta_key = static_cast<TableIndexMetaKey *>(table_index_obj->meta_key_.get());
        u64 index_id = std::stoull(table_index_meta_key->index_id_str_);
        std::shared_ptr<TableIndexCache> table_index_cache = std::make_shared<TableIndexCache>(db_id, table_id, index_id, index_name);
        for (const auto &segment_index_pair : table_index_obj->segment_map_) {
            SegmentID segment_id = segment_index_pair.first;
            MetaSegmentIndexObject *segment_index_obj = static_cast<MetaSegmentIndexObject *>(segment_index_pair.second.get());
            std::shared_ptr<SegmentIndexCache> segment_index_cache = std::make_shared<SegmentIndexCache>(segment_id);
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
    std::vector<BlockID> blocks;
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

std::shared_ptr<TableIndexCache> MetaTableIndexObject::RestoreTableIndexCache(Storage *storage_ptr) const {
    auto table_index_key = static_cast<TableIndexMetaKey *>(meta_key_.get());
    u64 db_id{};
    u64 table_id{};
    u64 index_id{};
    try {
        db_id = std::stoull(table_index_key->db_id_str_);
        table_id = std::stoull(table_index_key->table_id_str_);
        index_id = std::stoull(table_index_key->index_id_str_);
    } catch (const std::exception &e) {
        UnrecoverableError(fmt::format("DB id or table id is invalid: {}, cause: {}", table_index_key->ToString(), e.what()));
    }
    std::shared_ptr<TableIndexCache> table_index_cache = std::make_shared<TableIndexCache>(db_id, table_id, index_id, table_index_key->index_name_);
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

bool MetaTree::PathFilter(std::string_view path, CheckStmtType tag, std::optional<std::string> db_table_str) {
    switch (tag) {
        case CheckStmtType::kSystem: {
            return true;
        }
        case CheckStmtType::kTable: {
            auto table_str = fmt::format("{}", db_table_str.value());
            return path.find(table_str) != std::string::npos;
        }
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid entity tag");
        }
        default:
            return false;
    }
    return true;
}

void MetaTree::Prepare() {
    for (const auto &meta : metas_) {
        switch (meta->type_) {
            // case MetaType::kTable: {
            //
            // }
            case MetaType::kSegment: {
                auto *segment_meta = static_cast<const SegmentMetaKey *>(meta.get());
                segment_dic_.emplace(segment_meta->db_id_str_, segment_meta->table_id_str_, segment_meta->segment_id_);
                break;
            }
            case MetaType::kBlock: {
                auto *block_meta = static_cast<BlockMetaKey *>(meta.get());
                block_dic_.emplace(block_meta->db_id_str_, block_meta->table_id_str_, block_meta->segment_id_, block_meta->block_id_);
                break;
            }
            case MetaType::kTableColumn: {
                auto *column_meta = static_cast<TableColumnMetaKey *>(meta.get());
                simdjson::padded_string json_pad(column_meta->ToJson().dump());
                simdjson::parser parser;
                simdjson::document doc = parser.iterate(json_pad);
                auto meta_column_id = (ColumnID)doc["column_definition"]["column_id"].get<ColumnID>();
                column_dic_.emplace(column_meta->db_id_str_, column_meta->table_id_str_, meta_column_id);
                break;
            }
            case MetaType::kTableIndex: {
                auto *index_meta = static_cast<TableIndexMetaKey *>(meta.get());
                index_dic_.emplace(index_meta->db_id_str_, index_meta->table_id_str_, index_meta->index_id_str_);
                break;
            }
            case MetaType::kSegmentIndex: {
                auto *segment_index_meta = static_cast<SegmentIndexMetaKey *>(meta.get());
                segment_index_dic_.emplace(segment_index_meta->db_id_str_,
                                           segment_index_meta->table_id_str_,
                                           segment_index_meta->index_id_str_,
                                           segment_index_meta->segment_id_);
                break;
            }
            case MetaType::kChunkIndex: {
                auto *chunk_index_meta = static_cast<ChunkIndexMetaKey *>(meta.get());
                chunk_index_dic_.emplace(chunk_index_meta->db_id_str_,
                                         chunk_index_meta->table_id_str_,
                                         chunk_index_meta->index_id_str_,
                                         chunk_index_meta->segment_id_,
                                         chunk_index_meta->chunk_id_);
                break;
            }
            default:;
        }
    }
}

bool MetaTree::SegmentExistInMetas(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id) const {
    return segment_dic_.contains({db_id_str, table_id_str, segment_id});
}

bool MetaTree::BlockExistInMetas(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id, BlockID block_id) const {
    return block_dic_.contains({db_id_str, table_id_str, segment_id, block_id});
}

bool MetaTree::ColumnExistInMetas(const std::string &db_id_str, const std::string &table_id_str, ColumnID column_id) const {
    return column_dic_.contains({db_id_str, table_id_str, column_id});
}

bool MetaTree::IndexExistInMetas(const std::string &db_id_str, const std::string &table_id_str, const std::string &index_id_str) const {
    return index_dic_.contains({db_id_str, table_id_str, index_id_str});
}

bool MetaTree::SegmentIndexExistInMetas(const std::string &db_id_str,
                                        const std::string &table_id_str,
                                        const std::string &index_id_str,
                                        SegmentID segment_id) const {
    return segment_index_dic_.contains({db_id_str, table_id_str, index_id_str, segment_id});
}

bool MetaTree::ChunkIndexExistInMetas(const std::string &db_id_str,
                                      const std::string &table_id_str,
                                      const std::string &index_id_str,
                                      SegmentID segment_id,
                                      ChunkID chunk_id) const {
    return chunk_index_dic_.contains({db_id_str, table_id_str, index_id_str, segment_id, chunk_id});
}

bool MetaTree::CheckData(const std::string &path) {
    Prepare();
    static const re2::RE2 regex_column(R"(^(?:db_(\d+)/tbl_(\d+)/seg_(\d+)/blk_(\d+)/(?:version|(\d+)\.col|col_(\d+)_out))$)");

    static const re2::RE2 regex_index(R"(^(?:db_(\d+)/tbl_(\d+)/idx_(\d+)/seg_(\d+)/(?:chunk_(\d+)\.idx|ft_(\d+)\.(\w+)))$)");

    std::string_view sp1, sp2, sp3, sp4, sp5, sp6, sp7;
    std::string db_id_str, table_id_str, index_id_str, suffix_id, suffix;
    SegmentID segment_id = INVALID_SEGMENT_ID;
    BlockID block_id = INVALID_BLOCK_ID;

    if (re2::RE2::FullMatch(path, regex_column, &sp1, &sp2, &sp3, &sp4, &sp5, &sp6)) {
        // db_{1}, tbl_{2}, seg_{3}, blk_{4}, {5}.col, col_{6}_out
        db_id_str = sp1.data();
        table_id_str = sp2.data();
        segment_id = static_cast<SegmentID>(std::stoull(sp3.data()));
        block_id = static_cast<BlockID>(std::stoull(sp4.data()));

        if (!sp5.empty()) {
            suffix_id = sp5.data();
            suffix = "col";
        } else if (!sp6.empty()) {
            suffix_id = sp6.data();
            suffix = "out";
        } else {
            suffix = "version";
        }
    } else if (re2::RE2::FullMatch(path, regex_index, &sp1, &sp2, &sp3, &sp4, &sp5, &sp6, &sp7)) {
        // db_{1}, tbl_{2}, idx_{3}, seg_{4},
        // chunk_{5}.idx,
        // ft_{6}.{7}   sp[7] == len | pos | dic
        db_id_str = sp1.data();
        table_id_str = sp2.data();
        index_id_str = sp3.data();
        segment_id = static_cast<SegmentID>(std::stoull(sp4.data()));

        if (!sp5.empty()) {
            suffix_id = sp5.data();
            suffix = "idx";
        } else {
            suffix_id = sp6.data();
            suffix = sp7.data(); // len | pos | dic
        }
    } else {
        return true;
    }

    if (suffix == "dic" || suffix == "pos" || suffix == "len") {
        // TODO...
        return false;
    }
    if (suffix == "version") {
        return !SegmentExistInMetas(db_id_str, table_id_str, segment_id) || !BlockExistInMetas(db_id_str, table_id_str, segment_id, block_id);
    }
    if (suffix == "col" || suffix == "out") {
        auto column_id = static_cast<ColumnID>(std::stoull(suffix_id));
        return !SegmentExistInMetas(db_id_str, table_id_str, segment_id) || !BlockExistInMetas(db_id_str, table_id_str, segment_id, block_id) ||
               !ColumnExistInMetas(db_id_str, table_id_str, column_id);
    }
    if (suffix == "idx") {
        auto chunk_id = static_cast<ChunkID>(std::stoull(suffix_id));
        return !SegmentIndexExistInMetas(db_id_str, table_id_str, index_id_str, segment_id) ||
               !ChunkIndexExistInMetas(db_id_str, table_id_str, index_id_str, segment_id, chunk_id) ||
               !IndexExistInMetas(db_id_str, table_id_str, index_id_str);
    }
    return true;
}

std::unordered_set<std::string> MetaTree::GetDataVfsPathSet() {
    std::unordered_set<std::string> data_path_set;
    const auto *pm = InfinityContext::instance().storage()->fileworker_manager()->persistence_manager();
    for (auto files = pm->GetAllFiles(); const auto &path : files | std::views::keys) {
        data_path_set.emplace(path);
    }
    return data_path_set;
}

std::unordered_set<std::string> MetaTree::GetDataVfsOffPathSet() {
    std::unordered_set<std::string> data_path_set;
    std::filesystem::path data_dir = InfinityContext::instance().config()->DataDir();

    for (const auto &entry : std::filesystem::recursive_directory_iterator{data_dir}) {
        if (entry.is_regular_file()) {
            auto relative_path = std::filesystem::relative(entry.path(), data_dir);
            data_path_set.emplace(relative_path.string());
        }
    }
    return data_path_set;
}

std::vector<std::string> MetaTree::CheckMetaDataMapping(CheckStmtType tag, std::optional<std::string> db_table_str) {
    const auto *pm = InfinityContext::instance().storage()->fileworker_manager()->persistence_manager();
    auto data_path_set = pm != nullptr ? GetDataVfsPathSet() : GetDataVfsOffPathSet();

    std::vector<std::string> data_mismatch_entry;

    for (auto &path : data_path_set) {
        if (CheckData(path) && PathFilter(path, tag, db_table_str)) {
            data_mismatch_entry.emplace_back(path);
        }
    }

    return data_mismatch_entry;
}

} // namespace infinity
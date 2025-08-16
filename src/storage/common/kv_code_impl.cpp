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

module infinity_core:kv_code.impl;

import :kv_code;
import :infinity_exception;

import std;
import third_party;

import column_def;

namespace infinity {

std::unique_ptr<KeyBase> KeyBase::FromString(const std::string &key_str) {
    if (key_str.empty()) {
        UnrecoverableError(fmt::format("Empty key: {}", key_str));
        return nullptr;
    }

    auto pos = key_str.find_first_of('|');
    if (pos == std::string::npos) {
        UnrecoverableError(fmt::format("Unrecognized key: {}", key_str));
        return nullptr;
    }

    std::string key_type_str = key_str.substr(0, pos);
    if (key_type_str == "db") {
        auto key_db = std::make_unique<KeyDb>();
        key_db->db_name_ = key_str.substr(pos + 1);
        key_db->ts_ = std::stoull(key_db->db_name_.substr(key_db->db_name_.find_first_of('|') + 1));
        key_db->db_name_ = key_db->db_name_.substr(0, key_db->db_name_.find_first_of('|'));
        return key_db;
    } else if (key_type_str == "db_tag") {
        auto key_db_tag = std::make_unique<KeyDbTag>();
        key_db_tag->db_id_ = key_str.substr(pos + 1);
        key_db_tag->tag_name_ = key_db_tag->db_id_.substr(key_db_tag->db_id_.find_first_of('|') + 1);
        key_db_tag->db_id_ = key_db_tag->db_id_.substr(0, key_db_tag->db_id_.find_first_of('|'));
        return key_db_tag;
    } else if (key_type_str == "tbl") {
        auto key_table = std::make_unique<KeyTable>();
        key_table->db_id_ = key_str.substr(pos + 1);
        key_table->table_name_ = key_table->db_id_.substr(key_table->db_id_.find_first_of('|') + 1);
        key_table->db_id_ = key_table->db_id_.substr(0, key_table->db_id_.find_first_of('|'));
        key_table->ts_ = std::stoull(key_table->table_name_.substr(key_table->table_name_.find_first_of('|') + 1));
        key_table->table_name_ = key_table->table_name_.substr(0, key_table->table_name_.find_first_of('|'));
        return key_table;
    } else if (key_type_str == "tbl_tag") {
        auto key_table_tag = std::make_unique<KeyTableTag>();
        key_table_tag->db_id_ = key_str.substr(pos + 1);
        key_table_tag->table_id_ = key_table_tag->db_id_.substr(key_table_tag->db_id_.find_first_of('|') + 1);
        key_table_tag->db_id_ = key_table_tag->db_id_.substr(0, key_table_tag->db_id_.find_first_of('|'));
        key_table_tag->tag_name_ = key_table_tag->table_id_.substr(key_table_tag->table_id_.find_first_of('|') + 1);
        key_table_tag->table_id_ = key_table_tag->table_id_.substr(0, key_table_tag->table_id_.find_first_of('|'));
        return key_table_tag;
    } else if (key_type_str == "idx") {
        auto key_index = std::make_unique<KeyIndex>();
        key_index->db_id_ = key_str.substr(pos + 1);
        key_index->table_id_ = key_index->db_id_.substr(key_index->db_id_.find_first_of('|') + 1);
        key_index->db_id_ = key_index->db_id_.substr(0, key_index->db_id_.find_first_of('|'));
        key_index->index_name_ = key_index->table_id_.substr(key_index->table_id_.find_first_of('|') + 1);
        key_index->table_id_ = key_index->table_id_.substr(0, key_index->table_id_.find_first_of('|'));
        key_index->ts_ = std::stoull(key_index->index_name_.substr(key_index->index_name_.find_first_of('|') + 1));
        key_index->index_name_ = key_index->index_name_.substr(0, key_index->index_name_.find_first_of('|'));
        return key_index;
    } else if (key_type_str == "idx_tag") {
        auto key_index_tag = std::make_unique<KeyIndexTag>();
        key_index_tag->db_id_ = key_str.substr(pos + 1);
        key_index_tag->table_id_ = key_index_tag->db_id_.substr(key_index_tag->db_id_.find_first_of('|') + 1);
        key_index_tag->db_id_ = key_index_tag->db_id_.substr(0, key_index_tag->db_id_.find_first_of('|'));
        key_index_tag->index_id_ = key_index_tag->table_id_.substr(key_index_tag->table_id_.find_first_of('|') + 1);
        key_index_tag->table_id_ = key_index_tag->table_id_.substr(0, key_index_tag->table_id_.find_first_of('|'));
        key_index_tag->tag_name_ = key_index_tag->index_id_.substr(key_index_tag->index_id_.find_first_of('|') + 1);
        key_index_tag->index_id_ = key_index_tag->index_id_.substr(0, key_index_tag->index_id_.find_first_of('|'));
        return key_index_tag;
    } else {
        UnrecoverableError(fmt::format("Unrecognized key: {}", key_str));
        return nullptr;
    }
}

std::string KeyDb::ToString() const { return fmt::format("db|{}|{}", db_name_, ts_); }

std::string KeyDbTag::ToString() const { return fmt::format("db|{}|{}", db_id_, tag_name_); }

std::string KeyTable::ToString() const { return fmt::format("tbl|{}|{}|{}", db_id_, table_name_, ts_); }

std::string KeyIndex::ToString() const { return fmt::format("tbl|{}|{}|{}", db_id_, table_id_, index_name_); }

std::string KeyIndexTag::ToString() const { return fmt::format("idx|{}|{}|{}|{}", db_id_, table_id_, index_id_, tag_name_); }

std::string KeyEncode::CatalogDbKey(const std::string &db_name, TxnTimeStamp ts) { return fmt::format("catalog|db|{}|{}", db_name, ts); }

std::string KeyEncode::CatalogDbPrefix(const std::string &db_name) { return fmt::format("catalog|db|{}|", db_name); }

std::string KeyEncode::CatalogDbTagKey(const std::string &db_id, const std::string &tag_name) { return fmt::format("db|{}|{}", db_id, tag_name); }

std::string KeyEncode::CatalogDbTagPrefix(const std::string &db_id, const std::string &tag_name) { return fmt::format("db|{}|{}|", db_id, tag_name); }

std::string KeyEncode::CatalogTableKey(const std::string &db_id, const std::string &table_name, TxnTimeStamp ts) {
    return fmt::format("catalog|tbl|{}|{}|{}", db_id, table_name, ts);
}

std::string KeyEncode::CatalogTablePrefix(const std::string &db_id, const std::string &table_name) { return fmt::format("catalog|tbl|{}|{}|", db_id, table_name); }
std::string KeyEncode::CatalogDbTablePrefix(const std::string &db_id) { return fmt::format("catalog|tbl|{}|", db_id); }

std::string KeyEncode::CatalogTableTagKey(const std::string &db_id, const std::string &table_id, const std::string &tag_name) {
    return fmt::format("tbl|{}|{}|{}", db_id, table_id, tag_name);
}

std::string KeyEncode::CatalogTableTagPrefix(const std::string &db_id, const std::string &table_id, const std::string &tag_name) {
    return fmt::format("tbl|{}|{}|{}|", db_id, table_id, tag_name);
}

std::string KeyEncode::CatalogIndexKey(const std::string &db_id, const std::string &table_id, const std::string &index_name, TxnTimeStamp ts) {
    return fmt::format("catalog|idx|{}|{}|{}|{}", db_id, table_id, index_name, ts);
}
std::string KeyEncode::CatalogIndexPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_name) {
    return fmt::format("catalog|idx|{}|{}|{}|", db_id, table_id, index_name);
}
std::string KeyEncode::CatalogTableIndexPrefix(const std::string &db_id, const std::string &table_id) { return fmt::format("catalog|idx|{}|{}|", db_id, table_id); }

std::string KeyEncode::CatalogIndexTagKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, const std::string &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}", db_id, table_id, index_id, tag_name);
}

std::string KeyEncode::CatalogIndexTagKeyPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id, const std::string &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}|", db_id, table_id, index_id, tag_name);
}

std::string KeyEncode::CatalogIdxSegmentKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id) {
    return fmt::format("idx_seg|{}|{}|{}|{}", db_id, table_id, index_id, segment_id);
}

std::string KeyEncode::CatalogIdxSegmentKeyPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id) {
    return fmt::format("idx_seg|{}|{}|{}|", db_id, table_id, index_id);
}

std::string KeyEncode::CatalogIdxSegmentTagKey(const std::string &db_id,
                                          const std::string &table_id,
                                          const std::string &index_id,
                                          SegmentID segment_id,
                                          const std::string &tag_name) {
    return fmt::format("idx_seg|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, tag_name);
}

std::string KeyEncode::CatalogIdxChunkKey(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id, ChunkID chunk_id) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, chunk_id);
}

std::string KeyEncode::CatalogIdxChunkPrefix(const std::string &db_id, const std::string &table_id, const std::string &index_id, SegmentID segment_id) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|", db_id, table_id, index_id, segment_id);
}

std::string KeyEncode::CatalogIdxChunkTagKey(const std::string &db_id,
                                        const std::string &table_id,
                                        const std::string &index_id,
                                        SegmentID segment_id,
                                        ChunkID chunk_id,
                                        const std::string &tag_name) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, chunk_id, tag_name);
}

std::string KeyEncode::TableColumnKey(const std::string &db_id, const std::string &table_id, const std::string &column_name, TxnTimeStamp ts) {
    return fmt::format("tbl|col|{}|{}|{}|{}", db_id, table_id, column_name, ts);
}

std::string KeyEncode::TableColumnPrefix(const std::string &db_id, const std::string &table_id) { return fmt::format("tbl|col|{}|{}|", db_id, table_id); }

std::string KeyEncode::TableColumnPrefix(const std::string &db_id, const std::string &table_id, const std::string &column_name) {
    return fmt::format("tbl|col|{}|{}|{}", db_id, table_id, column_name);
}

std::string KeyEncode::TableColumnTagKey(const std::string &db_id, const std::string &table_id, const std::string &column_id, const std::string &tag_name) {
    return fmt::format("col|{}|{}|{}|{}", db_id, table_id, column_id, tag_name);
}

std::string KeyEncode::CatalogTableSegmentKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id) {
    return fmt::format("catalog|seg|{}|{}|{}", db_id, table_id, segment_id);
}
std::string KeyEncode::CatalogTableSegmentKeyPrefix(const std::string &db_id, const std::string &table_id) {
    return fmt::format("catalog|seg|{}|{}|", db_id, table_id);
}
std::string KeyEncode::CatalogTableSegmentTagKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id, const std::string &tag_name) {
    return fmt::format("seg|{}|{}|{}|{}", db_id, table_id, segment_id, tag_name);
}
std::string KeyEncode::CatalogTableSegmentBlockKey(const std::string &db_id, const std::string &table_id, SegmentID segment_id, BlockID block_id) {
    return fmt::format("catalog|blk|{}|{}|{}|{}", db_id, table_id, segment_id, block_id);
}
std::string KeyEncode::CatalogTableSegmentBlockKeyPrefix(const std::string &db_id, const std::string &table_id, SegmentID segment_id) {
    return fmt::format("catalog|blk|{}|{}|{}|", db_id, table_id, segment_id);
}
std::string KeyEncode::CatalogTableSegmentBlockTagKey(const std::string &db_id,
                                                 const std::string &table_id,
                                                 SegmentID segment_id,
                                                 BlockID block_id,
                                                 const std::string &tag_name) {
    return fmt::format("blk|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, tag_name);
}
std::string KeyEncode::CatalogTableSegmentBlockColumnKey(const std::string &db_id,
                                                    const std::string &table_id,
                                                    SegmentID segment_id,
                                                    BlockID block_id,
                                                    ColumnID column_id,
                                                    TxnTimeStamp ts) {
    return fmt::format("catalog|blk_col|{}|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, column_id, ts);
}
std::string KeyEncode::CatalogTableSegmentBlockColumnKeyPrefix(const std::string &db_id,
                                                          const std::string &table_id,
                                                          SegmentID segment_id,
                                                          BlockID block_id/*,
                                                          ColumnID column_id*/) {
    return fmt::format("catalog|blk_col|{}|{}|{}|{}|", db_id, table_id, segment_id, block_id);
}
std::string KeyEncode::CatalogTableSegmentBlockColumnTagKey(const std::string &db_id,
                                                       const std::string &table_id,
                                                       SegmentID segment_id,
                                                       BlockID block_id,
                                                       ColumnID column_id,
                                                       const std::string &tag_name) {
    return fmt::format("blk_col|{}|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, column_id, tag_name);
}

std::string KeyEncode::DatabaseKey(const std::string &db_name, TxnTimeStamp ts) { return fmt::format("db|{}|{}", db_name, ts); }
std::string KeyEncode::DatabasePrefix(const std::string &db_name) { return fmt::format("db|{}|", db_name); }

std::string KeyEncode::TableKey(const std::string &db_name, const std::string &tbl_name, TxnTimeStamp ts) {
    return fmt::format("tbl|{}|{}|{}", db_name, tbl_name, ts);
}
std::string KeyEncode::TableKeyPrefix(const std::string &db_name, const std::string &tbl_name) { return fmt::format("tbl|{}|{}|", db_name, tbl_name); }

std::string KeyEncode::TableSegmentKey(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, TxnTimeStamp ts) {
    return fmt::format("seg|{}|{}|{}|{}", db_name, tbl_name, segment_id, ts);
}
std::string KeyEncode::TableSegmentKeyPrefix(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id) {
    return fmt::format("seg|{}|{}|{}|", db_name, tbl_name, segment_id);
}

std::string KeyEncode::TableSegmentBlockKey(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts) {
    return fmt::format("blk|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, ts);
}
std::string KeyEncode::TableSegmentBlockKeyPrefix(const std::string &db_name, const std::string &tbl_name, SegmentID segment_id, BlockID block_id) {
    return fmt::format("blk|{}|{}|{}|{}|", db_name, tbl_name, segment_id, block_id);
}

std::string KeyEncode::TableSegmentBlockColumnKey(const std::string &db_name,
                                             const std::string &tbl_name,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             ColumnID column_id,
                                             TxnTimeStamp ts) {
    return fmt::format("col|{}|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, column_id, ts);
}
std::string KeyEncode::TableSegmentBlockColumnKeyPrefix(const std::string &db_name,
                                                   const std::string &tbl_name,
                                                   SegmentID segment_id,
                                                   BlockID block_id,
                                                   ColumnID column_id) {
    return fmt::format("col|{}|{}|{}|{}|{}|", db_name, tbl_name, segment_id, block_id, column_id);
}

std::string KeyEncode::TableIndexKey(const std::string &db_name, const std::string &tbl_name, const std::string &index_name, TxnTimeStamp ts) {
    return fmt::format("idx|{}|{}|{}|{}", db_name, tbl_name, index_name, ts);
}
std::string KeyEncode::TableIndexKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name) {
    return fmt::format("idx|{}|{}|{}|", db_name, tbl_name, index_name);
}

std::string
KeyEncode::TableIndexSegmentKey(const std::string &db_name, const std::string &tbl_name, const std::string &index_name, SegmentID segment_id, TxnTimeStamp ts) {
    return fmt::format("idx_seg|{}|{}|{}|{}|{}", db_name, tbl_name, index_name, segment_id, ts);
}

std::string KeyEncode::TableIndexSegmentKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name) {
    return fmt::format("idx_seg|{}|{}|{}|", db_name, tbl_name, index_name);
}

std::string KeyEncode::TableIndexChunkKey(const std::string &db_name,
                                     const std::string &tbl_name,
                                     const std::string &index_name,
                                     SegmentID segment_id,
                                     ChunkID chunk_id,
                                     TxnTimeStamp ts) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}|{}", db_name, tbl_name, index_name, segment_id, chunk_id, ts);
}
std::string KeyEncode::TableIndexChunkKeyPrefix(const std::string &db_name, const std::string &tbl_name, const std::string &index_name) {
    return fmt::format("idx_chunk|{}|{}|{}|", db_name, tbl_name, index_name);
}

std::string KeyEncode::PMObjectPrefix() { return "pm|object|"; }

std::string KeyEncode::PMObjectStatPrefix() { return "pm|object_stat|"; }

std::string KeyEncode::PMObjectKey(const std::string &key) { return fmt::format("pm|object|{}", key); }

std::string KeyEncode::PMObjectStatKey(const std::string &key) { return fmt::format("pm|object_stat|{}", key); }

std::string KeyEncode::DropDBKey(const std::string &db_name, const TxnTimeStamp &commit_ts, const std::string &db_id_str) {
    return fmt::format("drop|db|{}/{}/{}", db_name, commit_ts, db_id_str);
}

std::string KeyEncode::DropTableKey(const std::string &db_id_str, const std::string &table_name, const std::string &table_id_str, TxnTimeStamp create_ts) {
    return fmt::format("drop|tbl|{}/{}/{}/{}", db_id_str, table_name, create_ts, table_id_str);
}

std::string KeyEncode::RenameTableKey(const std::string &db_id_str, const std::string &table_name, const std::string &table_id_str, TxnTimeStamp create_ts) {
    return fmt::format("drop|tbl_name|{}/{}/{}/{}", db_id_str, table_name, create_ts, table_id_str);
}

std::string KeyEncode::DropTableKeyPrefix(const std::string &db_id_str, const std::string &table_name) {
    return fmt::format("drop|tbl|{}/{}/", db_id_str, table_name);
}

std::string KeyEncode::DropSegmentKey(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id) {
    return fmt::format("drop|seg|{}/{}/{}", db_id_str, table_id_str, segment_id);
}

std::string KeyEncode::DropBlockKey(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id, BlockID block_id) {
    return fmt::format("drop|blk|{}/{}/{}/{}", db_id_str, table_id_str, segment_id, block_id);
}

std::string KeyEncode::DropTableColumnKey(const std::string &db_id_str, const std::string &table_id_str, const std::string &column_name, TxnTimeStamp create_ts) {
    return fmt::format("drop|tbl_col|{}/{}/{}/{}", db_id_str, table_id_str, column_name, create_ts);
}

std::string KeyEncode::DropBlockColumnKey(const std::string &db_id_str,
                                     const std::string &table_id_str,
                                     SegmentID segment_id,
                                     BlockID block_id,
                                     const std::shared_ptr<ColumnDef> &column_def) {
    return fmt::format("drop|blk_col|{}/{}/{}/{}/{}", db_id_str, table_id_str, segment_id, block_id, column_def->ToJson().dump());
}

std::string KeyEncode::DropTableIndexKey(const std::string &db_id_str,
                                    const std::string &table_id_str,
                                    const std::string &index_name,
                                    const TxnTimeStamp &commit_ts,
                                    const std::string &index_id_str) {
    return fmt::format("drop|idx|{}/{}/{}/{}/{}", db_id_str, table_id_str, index_name, commit_ts, index_id_str);
}

std::string KeyEncode::DropSegmentIndexKey(const std::string &db_id_str, const std::string &table_id_str, const std::string &index_id_str, SegmentID segment_id) {
    return fmt::format("drop|idx_seg|{}/{}/{}/{}", db_id_str, table_id_str, index_id_str, segment_id);
}

std::string KeyEncode::DropChunkIndexKey(const std::string &db_id_str,
                                    const std::string &table_id_str,
                                    const std::string &index_id_str,
                                    SegmentID segment_id,
                                    ChunkID chunk_id) {
    return fmt::format("drop|idx_chunk|{}/{}/{}/{}/{}", db_id_str, table_id_str, index_id_str, segment_id, chunk_id);
}

} // namespace infinity

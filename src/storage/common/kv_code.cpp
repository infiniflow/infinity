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

#include <string>

module kv_code;

import stl;
import third_party;
import infinity_exception;

namespace infinity {

UniquePtr<KeyBase> KeyBase::FromString(const String &key_str) {
    if (key_str.empty()) {
        UnrecoverableError(fmt::format("Empty key: {}", key_str));
        return nullptr;
    }

    auto pos = key_str.find_first_of('|');
    if (pos == String::npos) {
        UnrecoverableError(fmt::format("Unrecognized key: {}", key_str));
        return nullptr;
    }

    String key_type_str = key_str.substr(0, pos);
    if (key_type_str == "db") {
        auto key_db = MakeUnique<KeyDb>();
        key_db->db_name_ = key_str.substr(pos + 1);
        key_db->ts_ = std::stoull(key_db->db_name_.substr(key_db->db_name_.find_first_of('|') + 1));
        key_db->db_name_ = key_db->db_name_.substr(0, key_db->db_name_.find_first_of('|'));
        return key_db;
    } else if (key_type_str == "db_tag") {
        auto key_db_tag = MakeUnique<KeyDbTag>();
        key_db_tag->db_id_ = key_str.substr(pos + 1);
        key_db_tag->tag_name_ = key_db_tag->db_id_.substr(key_db_tag->db_id_.find_first_of('|') + 1);
        key_db_tag->db_id_ = key_db_tag->db_id_.substr(0, key_db_tag->db_id_.find_first_of('|'));
        return key_db_tag;
    } else if (key_type_str == "tbl") {
        auto key_table = MakeUnique<KeyTable>();
        key_table->db_id_ = key_str.substr(pos + 1);
        key_table->table_name_ = key_table->db_id_.substr(key_table->db_id_.find_first_of('|') + 1);
        key_table->db_id_ = key_table->db_id_.substr(0, key_table->db_id_.find_first_of('|'));
        key_table->ts_ = std::stoull(key_table->table_name_.substr(key_table->table_name_.find_first_of('|') + 1));
        key_table->table_name_ = key_table->table_name_.substr(0, key_table->table_name_.find_first_of('|'));
        return key_table;
    } else if (key_type_str == "tbl_tag") {
        auto key_table_tag = MakeUnique<KeyTableTag>();
        key_table_tag->db_id_ = key_str.substr(pos + 1);
        key_table_tag->table_id_ = key_table_tag->db_id_.substr(key_table_tag->db_id_.find_first_of('|') + 1);
        key_table_tag->db_id_ = key_table_tag->db_id_.substr(0, key_table_tag->db_id_.find_first_of('|'));
        key_table_tag->tag_name_ = key_table_tag->table_id_.substr(key_table_tag->table_id_.find_first_of('|') + 1);
        key_table_tag->table_id_ = key_table_tag->table_id_.substr(0, key_table_tag->table_id_.find_first_of('|'));
        return key_table_tag;
    } else if (key_type_str == "idx") {
        auto key_index = MakeUnique<KeyIndex>();
        key_index->db_id_ = key_str.substr(pos + 1);
        key_index->table_id_ = key_index->db_id_.substr(key_index->db_id_.find_first_of('|') + 1);
        key_index->db_id_ = key_index->db_id_.substr(0, key_index->db_id_.find_first_of('|'));
        key_index->index_name_ = key_index->table_id_.substr(key_index->table_id_.find_first_of('|') + 1);
        key_index->table_id_ = key_index->table_id_.substr(0, key_index->table_id_.find_first_of('|'));
        key_index->ts_ = std::stoull(key_index->index_name_.substr(key_index->index_name_.find_first_of('|') + 1));
        key_index->index_name_ = key_index->index_name_.substr(0, key_index->index_name_.find_first_of('|'));
        return key_index;
    } else if (key_type_str == "idx_tag") {
        auto key_index_tag = MakeUnique<KeyIndexTag>();
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

String KeyDb::ToString() const { return fmt::format("db|{}|{}", db_name_, ts_); }

String KeyDbTag::ToString() const { return fmt::format("db|{}|{}", db_id_, tag_name_); }

String KeyTable::ToString() const { return fmt::format("tbl|{}|{}|{}", db_id_, table_name_, ts_); }

String KeyIndex::ToString() const { return fmt::format("tbl|{}|{}|{}", db_id_, table_id_, index_name_); }

String KeyIndexTag::ToString() const { return fmt::format("idx|{}|{}|{}|{}", db_id_, table_id_, index_id_, tag_name_); }

String KeyEncode::CatalogDbKey(const String &db_name, TxnTimeStamp ts) { return fmt::format("catalog|db|{}|{}", db_name, ts); }

String KeyEncode::CatalogDbPrefix(const String &db_name) { return fmt::format("catalog|db|{}|", db_name); }

String KeyEncode::CatalogDbTagKey(const String &db_id, const String &tag_name) { return fmt::format("db|{}|{}", db_id, tag_name); }

String KeyEncode::CatalogDbTagPrefix(const String &db_id, const String &tag_name) { return fmt::format("db|{}|{}|", db_id, tag_name); }

String KeyEncode::CatalogTableKey(const String &db_id, const String &table_name, TxnTimeStamp ts) {
    return fmt::format("catalog|tbl|{}|{}|{}", db_id, table_name, ts);
}

String KeyEncode::CatalogTablePrefix(const String &db_id, const String &table_name) { return fmt::format("catalog|tbl|{}|{}|", db_id, table_name); }
String KeyEncode::CatalogDbTablePrefix(const String &db_id) { return fmt::format("catalog|tbl|{}|", db_id); }

String KeyEncode::CatalogTableTagKey(const String &db_id, const String &table_id, const String &tag_name) {
    return fmt::format("tbl|{}|{}|{}", db_id, table_id, tag_name);
}

String KeyEncode::CatalogTableTagPrefix(const String &db_id, const String &table_id, const String &tag_name) {
    return fmt::format("tbl|{}|{}|{}|", db_id, table_id, tag_name);
}

String KeyEncode::CatalogIndexKey(const String &db_id, const String &table_id, const String &index_name, TxnTimeStamp ts) {
    return fmt::format("catalog|idx|{}|{}|{}|{}", db_id, table_id, index_name, ts);
}
String KeyEncode::CatalogIndexPrefix(const String &db_id, const String &table_id, const String &index_name) {
    return fmt::format("catalog|idx|{}|{}|{}|", db_id, table_id, index_name);
}
String KeyEncode::CatalogTableIndexPrefix(const String &db_id, const String &table_id) { return fmt::format("catalog|idx|{}|{}|", db_id, table_id); }

String KeyEncode::CatalogIndexTagKey(const String &db_id, const String &table_id, const String &index_id, const String &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}", db_id, table_id, index_id, tag_name);
}

String KeyEncode::CatalogIndexTagKeyPrefix(const String &db_id, const String &table_id, const String &index_id, const String &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}|", db_id, table_id, index_id, tag_name);
}

String KeyEncode::CatalogIdxSegmentKey(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id) {
    return fmt::format("idx_seg|{}|{}|{}|{}", db_id, table_id, index_id, segment_id);
}

String KeyEncode::CatalogIdxSegmentKeyPrefix(const String &db_id, const String &table_id, const String &index_id) {
    return fmt::format("idx_seg|{}|{}|{}|", db_id, table_id, index_id);
}

String KeyEncode::CatalogIdxSegmentTagKey(const String &db_id,
                                          const String &table_id,
                                          const String &index_id,
                                          SegmentID segment_id,
                                          const String &tag_name) {
    return fmt::format("idx_seg|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, tag_name);
}

String KeyEncode::CatalogIdxChunkKey(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id, ChunkID chunk_id) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, chunk_id);
}

String KeyEncode::CatalogIdxChunkPrefix(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|", db_id, table_id, index_id, segment_id);
}

String KeyEncode::CatalogIdxChunkTagKey(const String &db_id,
                                        const String &table_id,
                                        const String &index_id,
                                        SegmentID segment_id,
                                        ChunkID chunk_id,
                                        const String &tag_name) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}|{}", db_id, table_id, index_id, segment_id, chunk_id, tag_name);
}

String KeyEncode::TableColumnKey(const String &db_id, const String &table_id, const String &column_name) {
    return fmt::format("tbl|col|{}|{}|{}", db_id, table_id, column_name);
}

String KeyEncode::TableColumnPrefix(const String &db_id, const String &table_id) { return fmt::format("tbl|col|{}|{}|", db_id, table_id); }

String KeyEncode::TableColumnTagKey(const String &db_id, const String &table_id, const String &column_id, const String &tag_name) {
    return fmt::format("col|{}|{}|{}|{}", db_id, table_id, column_id, tag_name);
}

String KeyEncode::CatalogTableSegmentKey(const String &db_id, const String &table_id, SegmentID segment_id) {
    return fmt::format("catalog|seg|{}|{}|{}", db_id, table_id, segment_id);
}
String KeyEncode::CatalogTableSegmentKeyPrefix(const String &db_id, const String &table_id) {
    return fmt::format("catalog|seg|{}|{}|", db_id, table_id);
}
String KeyEncode::CatalogTableSegmentTagKey(const String &db_id, const String &table_id, SegmentID segment_id, const String &tag_name) {
    return fmt::format("seg|{}|{}|{}|{}", db_id, table_id, segment_id, tag_name);
}
String KeyEncode::CatalogTableSegmentBlockKey(const String &db_id, const String &table_id, SegmentID segment_id, BlockID block_id) {
    return fmt::format("catalog|blk|{}|{}|{}|{}", db_id, table_id, segment_id, block_id);
}
String KeyEncode::CatalogTableSegmentBlockKeyPrefix(const String &db_id, const String &table_id, SegmentID segment_id) {
    return fmt::format("catalog|blk|{}|{}|{}|", db_id, table_id, segment_id);
}
String KeyEncode::CatalogTableSegmentBlockTagKey(const String &db_id,
                                                 const String &table_id,
                                                 SegmentID segment_id,
                                                 BlockID block_id,
                                                 const String &tag_name) {
    return fmt::format("blk|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, tag_name);
}
String KeyEncode::CatalogTableSegmentBlockColumnKey(const String &db_id,
                                                    const String &table_id,
                                                    SegmentID segment_id,
                                                    BlockID block_id,
                                                    ColumnID column_id) {
    return fmt::format("catalog|blk_col|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, column_id);
}
String KeyEncode::CatalogTableSegmentBlockColumnKeyPrefix(const String &db_id,
                                                          const String &table_id,
                                                          SegmentID segment_id,
                                                          BlockID block_id/*,
                                                          ColumnID column_id*/) {
    return fmt::format("catalog|blk_col|{}|{}|{}|{}|", db_id, table_id, segment_id, block_id);
}
String KeyEncode::CatalogTableSegmentBlockColumnTagKey(const String &db_id,
                                                       const String &table_id,
                                                       SegmentID segment_id,
                                                       BlockID block_id,
                                                       ColumnID column_id,
                                                       const String &tag_name) {
    return fmt::format("blk_col|{}|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, column_id, tag_name);
}

String KeyEncode::DatabaseKey(const String &db_name, TxnTimeStamp ts) { return fmt::format("db|{}|{}", db_name, ts); }
String KeyEncode::DatabasePrefix(const String &db_name) { return fmt::format("db|{}|", db_name); }

String KeyEncode::TableKey(const String &db_name, const String &tbl_name, TxnTimeStamp ts) {
    return fmt::format("tbl|{}|{}|{}", db_name, tbl_name, ts);
}
String KeyEncode::TableKeyPrefix(const String &db_name, const String &tbl_name) { return fmt::format("tbl|{}|{}|", db_name, tbl_name); }

String KeyEncode::TableSegmentKey(const String &db_name, const String &tbl_name, SegmentID segment_id, TxnTimeStamp ts) {
    return fmt::format("seg|{}|{}|{}|{}", db_name, tbl_name, segment_id, ts);
}
String KeyEncode::TableSegmentKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id) {
    return fmt::format("seg|{}|{}|{}|", db_name, tbl_name, segment_id);
}

String KeyEncode::TableSegmentBlockKey(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts) {
    return fmt::format("blk|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, ts);
}
String KeyEncode::TableSegmentBlockKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id) {
    return fmt::format("blk|{}|{}|{}|{}|", db_name, tbl_name, segment_id, block_id);
}

String KeyEncode::TableSegmentBlockColumnKey(const String &db_name,
                                             const String &tbl_name,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             ColumnID column_id,
                                             TxnTimeStamp ts) {
    return fmt::format("col|{}|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, column_id, ts);
}
String KeyEncode::TableSegmentBlockColumnKeyPrefix(const String &db_name,
                                                   const String &tbl_name,
                                                   SegmentID segment_id,
                                                   BlockID block_id,
                                                   ColumnID column_id) {
    return fmt::format("col|{}|{}|{}|{}|{}|", db_name, tbl_name, segment_id, block_id, column_id);
}

String KeyEncode::TableIndexKey(const String &db_name, const String &tbl_name, const String &index_name, TxnTimeStamp ts) {
    return fmt::format("idx|{}|{}|{}|{}", db_name, tbl_name, index_name, ts);
}
String KeyEncode::TableIndexKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name) {
    return fmt::format("idx|{}|{}|{}|", db_name, tbl_name, index_name);
}

String
KeyEncode::TableIndexSegmentKey(const String &db_name, const String &tbl_name, const String &index_name, SegmentID segment_id, TxnTimeStamp ts) {
    return fmt::format("idx_seg|{}|{}|{}|{}|{}", db_name, tbl_name, index_name, segment_id, ts);
}

String KeyEncode::TableIndexSegmentKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name) {
    return fmt::format("idx_seg|{}|{}|{}|", db_name, tbl_name, index_name);
}

String KeyEncode::TableIndexChunkKey(const String &db_name,
                                     const String &tbl_name,
                                     const String &index_name,
                                     SegmentID segment_id,
                                     ChunkID chunk_id,
                                     TxnTimeStamp ts) {
    return fmt::format("idx_chunk|{}|{}|{}|{}|{}|{}", db_name, tbl_name, index_name, segment_id, chunk_id, ts);
}
String KeyEncode::TableIndexChunkKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name) {
    return fmt::format("idx_chunk|{}|{}|{}|", db_name, tbl_name, index_name);
}

String KeyEncode::PMObjectPrefix() { return "pm|object|"; }

String KeyEncode::PMObjectStatPrefix() { return "pm|object_stat|"; }

String KeyEncode::PMObjectKey(const String &key) { return fmt::format("pm|object|{}", key); }

String KeyEncode::PMObjectStatKey(const String &key) { return fmt::format("pm|object_stat|{}", key); }

String KeyEncode::DropDBKey(const String &db_id_str, const String &db_name) { return fmt::format("drop|db|{}/{}", db_id_str, db_name); }

String KeyEncode::DropTableKey(const String &db_id_str, const String &table_id_str, const String &table_name) {
    return fmt::format("drop|tbl|{}/{}/{}", db_id_str, table_id_str, table_name);
}

String KeyEncode::DropSegmentKey(const String &db_id_str, const String &table_id_str, SegmentID segment_id) {
    return fmt::format("drop|seg|{}/{}/{}", db_id_str, table_id_str, segment_id);
}

String KeyEncode::DropBlockKey(const String &db_id_str, const String &table_id_str, SegmentID segment_id, BlockID block_id) {
    return fmt::format("drop|blk|{}/{}/{}/{}", db_id_str, table_id_str, segment_id, block_id);
}

String KeyEncode::DropBlockColumnKey(const String &db_id_str,
                                     const String &table_id_str,
                                     SegmentID segment_id,
                                     BlockID block_id,
                                     const SharedPtr<ColumnDef> &column_def) {
    return fmt::format("drop|blk_col|{}/{}/{}/{}/{}", db_id_str, table_id_str, segment_id, block_id, column_def->ToJson().dump());
}

String KeyEncode::DropTableIndexKey(const String &db_id_str, const String &table_id_str, const String &index_id_str, const String &index_name) {
    return fmt::format("drop|idx|{}/{}/{}/{}", db_id_str, table_id_str, index_id_str, index_name);
}

String KeyEncode::DropSegmentIndexKey(const String &db_id_str, const String &table_id_str, const String &index_id_str, SegmentID segment_id) {
    return fmt::format("drop|idx_seg|{}/{}/{}/{}", db_id_str, table_id_str, index_id_str, segment_id);
}

String KeyEncode::DropChunkIndexKey(const String &db_id_str,
                                    const String &table_id_str,
                                    const String &index_id_str,
                                    SegmentID segment_id,
                                    ChunkID chunk_id) {
    return fmt::format("drop|idx_chunk|{}/{}/{}/{}/{}", db_id_str, table_id_str, index_id_str, segment_id, chunk_id);
}

} // namespace infinity

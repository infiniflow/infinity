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

module kv_code;

import stl;
import third_party;

namespace infinity {

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

String KeyEncode::CatalogIndexTagKey(const String &db_id, const String &table_id, const String &index_name, const String &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}", db_id, table_id, index_name, tag_name);
}

String KeyEncode::CatalogIndexTagKeyPrefix(const String &db_id, const String &table_id, const String &index_name, const String &tag_name) {
    return fmt::format("idx|{}|{}|{}|{}|", db_id, table_id, index_name, tag_name);
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
String KeyEncode::CatalogTableSegmentBlockKey(const String &db_id, const String &table_id, SegmentID segment_id, BlockID block_id, TxnTimeStamp ts) {
    return fmt::format("catalog|blk|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, ts);
}
String KeyEncode::CatalogTableSegmentBlockColumnKey(const String &db_id,
                                                    const String &table_id,
                                                    SegmentID segment_id,
                                                    BlockID block_id,
                                                    ColumnID column_id,
                                                    TxnTimeStamp ts) {
    return fmt::format("catalog|blk_col|{}|{}|{}|{}|{}|{}", db_id, table_id, segment_id, block_id, column_id, ts);
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

} // namespace infinity

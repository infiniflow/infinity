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

String KeyEncode::DatabaseKey(const String &db_name, TxnTimeStamp ts) { return fmt::format("db|{}|{}", db_name, ts); }
String KeyEncode::DatabasePrefix(const String &db_name) { return fmt::format("db|{}|", db_name); }

String KeyEncode::TableKey(const String &db_name, const String &tbl_name, TxnTimeStamp ts) {
    return fmt::format("tbl|{}|{}|{}", db_name, tbl_name, ts);
}
String KeyEncode::TableKeyPrefix(const String &db_name, const String &tbl_name) { return fmt::format("tbl|{}|{}|", db_name, tbl_name); }

String KeyEncode::TableSegmentKey(const String &db_name, const String &tbl_name, SegmentID segment_id, TxnTimeStamp ts) {
    return fmt::format("seg|{}|{}|{}|{}", db_name, tbl_name, segment_id, ts);
}
String KeyEncode::TableSegmentKeyPrefix(const String &db_name, const String &tbl_name) { return fmt::format("seg|{}|{}|", db_name, tbl_name); }

String KeyEncode::TableSegmentBlockKey(const String &db_name,
                                       const String &tbl_name,
                                       SegmentID segment_id,
                                       BlockID block_id,
                                       ColumnID column_id,
                                       TxnTimeStamp ts) {
    return fmt::format("blk|{}|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, column_id, ts);
}
String KeyEncode::TableSegmentBlockKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id) {
    return fmt::format("blk|{}|{}|{}|", db_name, tbl_name, segment_id);
}

String KeyEncode::TableSegmentBlockColumnKey(const String &db_name,
                                             const String &tbl_name,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             ColumnID column_id,
                                             TxnTimeStamp ts) {
    return fmt::format("col|{}|{}|{}|{}|{}|{}", db_name, tbl_name, segment_id, block_id, column_id, ts);
}
String KeyEncode::TableSegmentBlockColumnKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id) {
    return fmt::format("col|{}|{}|{}|{}|", db_name, tbl_name, segment_id, block_id);
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

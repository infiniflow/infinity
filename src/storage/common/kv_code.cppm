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

export module kv_code;

import stl;

namespace infinity {

class KeyEncode {
public:
    static String DatabaseKey(const String &db_name, TxnTimeStamp ts, TransactionID txn_id);
    static String DatabasePrefix(const String &db_name);

    static String TableKey(const String &db_name, const String &tbl_name, TxnTimeStamp ts, TransactionID txn_id);
    static String TableKeyPrefix(const String &db_name, const String &tbl_name);

    static String TableSegmentKey(const String &db_name, const String &tbl_name, SegmentID segment_id, TxnTimeStamp ts, TransactionID txn_id);
    static String TableSegmentKeyPrefix(const String &db_name, const String &tbl_name);

    static String TableSegmentBlockKey(const String &db_name,
                                       const String &tbl_name,
                                       SegmentID segment_id,
                                       BlockID block_id,
                                       ColumnID column_id,
                                       TxnTimeStamp ts,
                                       TransactionID txn_id);
    static String TableSegmentBlockKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id);

    static String TableSegmentBlockColumnKey(const String &db_name,
                                             const String &tbl_name,
                                             SegmentID segment_id,
                                             BlockID block_id,
                                             ColumnID column_id,
                                             TxnTimeStamp ts,
                                             TransactionID txn_id);
    static String TableSegmentBlockColumnKeyPrefix(const String &db_name, const String &tbl_name, SegmentID segment_id, BlockID block_id);

    static String TableIndexKey(const String &db_name, const String &tbl_name, const String &index_name, TxnTimeStamp ts, TransactionID txn_id);
    static String TableIndexKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);

    static String TableIndexSegmentKey(const String &db_name,
                                       const String &tbl_name,
                                       const String &index_name,
                                       SegmentID segment_id,
                                       TxnTimeStamp ts,
                                       TransactionID txn_id);
    static String TableIndexSegmentKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);

    static String TableIndexChunkKey(const String &db_name,
                                     const String &tbl_name,
                                     const String &index_name,
                                     SegmentID segment_id,
                                     ChunkID chunk_id,
                                     TxnTimeStamp ts,
                                     TransactionID txn_id);
    static String TableIndexChunkKeyPrefix(const String &db_name, const String &tbl_name, const String &index_name);
};

} // namespace infinity
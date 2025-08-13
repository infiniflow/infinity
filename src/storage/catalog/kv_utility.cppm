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

export module infinity_core:kv_utility;

import :stl;
import :status;

import internal_types;

namespace infinity {

class KVInstance;
class IndexBase;

export Vector<SegmentID> GetTableSegments(KVInstance *kv_instance, const String &db_id_str, const String &table_id_str, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

export Vector<SegmentID> GetTableIndexSegments(KVInstance *kv_instance,
                                               const String &db_id_str,
                                               const String &table_id_str,
                                               const String &index_id_str,
                                               TxnTimeStamp begin_ts,
                                               TxnTimeStamp commit_ts);

export Vector<BlockID> GetTableSegmentBlocks(KVInstance *kv_instance,
                                             const String &db_id_str,
                                             const String &table_id_str,
                                             SegmentID segment_id,
                                             TxnTimeStamp begin_ts,
                                             TxnTimeStamp commit_ts);

export Vector<ColumnID> GetTableSegmentBlockColumns(KVInstance *kv_instance,
                                                    const String &db_id_str,
                                                    const String &table_id_str,
                                                    SegmentID segment_id,
                                                    BlockID block_id,
                                                    TxnTimeStamp begin_ts);
export SharedPtr<IndexBase>
GetTableIndexDef(KVInstance *kv_instance, const String &db_id_str, const String &table_id_str, const String &index_id_str, TxnTimeStamp begin_ts);

export SizeT GetBlockRowCount(KVInstance *kv_instance,
                              const String &db_id_str,
                              const String &table_id_str,
                              SegmentID segment_id,
                              BlockID block_id,
                              TxnTimeStamp begin_ts,
                              TxnTimeStamp commit_ts);

export SizeT GetSegmentRowCount(KVInstance *kv_instance,
                                const String &db_id_str,
                                const String &table_id_str,
                                SegmentID segment_id,
                                TxnTimeStamp begin_ts,
                                TxnTimeStamp commit_ts);

export String GetLastPartOfKey(const String& key, char delimiter = '|');

export u64 GetTimestampFromKey(const String& key);

} // namespace infinity

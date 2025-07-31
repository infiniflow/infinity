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

#include "type/complex/row_id.h"

export module table_meeta;

import stl;
import status;
import column_def;
import new_catalog;
import base_meta;

namespace infinity {

class KVInstance;
class TableDef;
class TableInfo;
class TableIndexReaderCache;
class NewTxn;
// struct SegmentUpdateTS;
struct TableDetail;

export class TableMeeta : public BaseMeta {
public:
    TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    TableMeeta(const String &db_id_str, const String &table_id_str, NewTxn *txn);

    TxnTimeStamp begin_ts() const { return begin_ts_; }
    TxnTimeStamp commit_ts() const { return commit_ts_; }

    KVInstance *kv_instance() const { return kv_instance_; }

    const String &table_id_str() const { return table_id_str_; }
    const String &db_id_str() const { return db_id_str_; }

    Status GetComment(TableInfo &table_info);

    Status GetIndexIDs(Vector<String> *&index_id_strs, Vector<String> **index_names = nullptr);

    Status GetIndexID(const String &index_name, String &index_key, String &index_id_str, TxnTimeStamp &create_index_ts);

    Status InitSet(SharedPtr<TableDef> table_def);

    Status LoadSet();

    Status UninitSet(UsageFlag usage_flag);

    Status GetUnsealedSegmentID(SegmentID &unsealed_segment_id);

    Status SetUnsealedSegmentID(SegmentID unsealed_segment_id);

    Status DelUnsealedSegmentID();

    Status RemoveSegmentIDs1(const Vector<SegmentID> &segment_ids);

    Pair<SegmentID, Status> AddSegmentID1(TxnTimeStamp commit_ts);
    Status AddSegmentWithID(TxnTimeStamp commit_ts, SegmentID segment_id);

    Status CommitSegment(SegmentID segment_id, TxnTimeStamp commit_ts);

    Tuple<ColumnID, Status> GetColumnIDByColumnName(const String &column_name);
    Tuple<String, Status> GetColumnKeyByColumnName(const String &column_name) const;
    SharedPtr<String> GetTableDir();

    Tuple<Vector<SegmentID> *, Status> GetSegmentIDs1();
    Status CheckSegments(const Vector<SegmentID> &segment_ids);

    Tuple<SharedPtr<Vector<SharedPtr<ColumnDef>>>, Status> GetColumnDefs();
    Tuple<SharedPtr<ColumnDef>, Status> GetColumnDefByColumnName(const String &column_name, SizeT *column_idx = nullptr);
    Tuple<SharedPtr<ColumnDef>, Status> GetColumnDefByColumnID(const SizeT &column_idx);

    Status GetTableInfo(TableInfo &table_info);

    Status GetTableDetail(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, TableDetail &table_detail);

    Pair<String, String> GetDBTableName() const;
    void SetDBTableName(const String &db_name, const String &table_name);

    Status AddColumn(const ColumnDef &column_def);

    Status AddFtIndexCache(SharedPtr<TableIndexReaderCache> ft_index_cache);

    Status GetFtIndexCache(SharedPtr<TableIndexReaderCache> &ft_index_cache);

    Status RemoveFtIndexCache();

    Status InvalidateFtIndexCache();

    Status GetNextColumnID(ColumnID &next_column_id);

    Status SetNextColumnID(ColumnID next_column_id);

    Status UpdateFulltextSegmentTS(TxnTimeStamp ts, SegmentUpdateTS &segment_update_ts);

    Tuple<String, Status> GetNextIndexID();

    Status SetNextIndexID(const String &index_id_str);

private:
    Status LoadComment();

    Status LoadColumnDefs();

    Status LoadSegmentIDs1();

    Status LoadIndexIDs();

    Status LoadUnsealedSegmentID();

    Status LoadNextColumnID();

    String GetTableTag(const String &tag) const;

private:
    std::mutex mtx_;

    TxnTimeStamp begin_ts_ = 0;
    TxnTimeStamp commit_ts_;
    NewTxn *txn_{};
    KVInstance *kv_instance_{};
    String db_id_str_;
    String table_id_str_;
    String db_name_{};
    String table_name_{};

    Optional<String> comment_;
    Optional<Vector<SharedPtr<ColumnDef>>> column_defs_;
    // Optional<Vector<SegmentID>> segment_ids_;
    Optional<Vector<SegmentID>> segment_ids1_;

    Optional<Vector<String>> index_id_strs_;
    Optional<Vector<String>> index_names_;
    Optional<SegmentID> next_segment_id_;
    Optional<SegmentID> unsealed_segment_id_;
    Optional<ColumnID> next_column_id_;
};

} // namespace infinity

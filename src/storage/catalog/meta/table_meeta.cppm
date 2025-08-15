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

export module infinity_core:table_meeta;

import :stl;
import :status;
import :new_catalog;
import :snapshot_info;
import :wal_entry;

import row_id;
import column_def;

namespace infinity {

class KVInstance;
class TableDef;
struct TableInfo;
class TableIndexReaderCache;
class NewTxn;
// struct SegmentUpdateTS;
struct TableDetail;

export class TableMeeta {
public:
    // TableMeeta(const std::string &db_id_str, const std::string &table_id_str, KVInstance &kv_instance, TxnTimeStamp begin_ts, UsageEnum usage);
    TableMeeta(const std::string &db_id_str, const std::string &table_id_str, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    TableMeeta(const std::string &db_id_str, const std::string &table_id_str, NewTxn *txn);

    TxnTimeStamp begin_ts() const { return begin_ts_; }
    TxnTimeStamp commit_ts() const { return commit_ts_; }
    

    /**
     * Get the table creation timestamp from KV instance
     * @return std::tuple containing the create timestamp and status
     * @note This parses the actual key in KV store to get the creation timestamp
     */
    std::tuple<TxnTimeStamp, Status> GetCreateTimestampFromKV(const std::string &table_name) ;

    KVInstance *kv_instance() const { return kv_instance_; }

    const std::string &table_id_str() const { return table_id_str_; }
    const std::string &db_id_str() const { return db_id_str_; }

    Status GetComment(TableInfo &table_info);

    Status GetIndexIDs(std::vector<std::string> *&index_id_strs, std::vector<std::string> **index_names = nullptr);

    Status GetIndexID(const std::string &index_name, std::string &index_key, std::string &index_id_str, TxnTimeStamp &create_index_ts);

    Status InitSet(std::shared_ptr<TableDef> table_def);

    Status LoadSet();

    Status UninitSet(UsageFlag usage_flag);

    Status GetUnsealedSegmentID(SegmentID &unsealed_segment_id);

    Status SetUnsealedSegmentID(SegmentID unsealed_segment_id);

    Status DelUnsealedSegmentID();

    Status RemoveSegmentIDs1(const std::vector<SegmentID> &segment_ids);

    std::pair<SegmentID, Status> AddSegmentID1(TxnTimeStamp commit_ts);
    Status AddSegmentWithID(TxnTimeStamp commit_ts, SegmentID segment_id);

    Status CommitSegment(SegmentID segment_id, TxnTimeStamp commit_ts);

    std::tuple<ColumnID, Status> GetColumnIDByColumnName(const std::string &column_name);
    std::tuple<std::string, Status> GetColumnKeyByColumnName(const std::string &column_name) const;
    std::shared_ptr<std::string> GetTableDir();

    std::tuple<std::vector<SegmentID> *, Status> GetSegmentIDs1();
    Status CheckSegments(const std::vector<SegmentID> &segment_ids);

    std::tuple<std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>>, Status> GetColumnDefs();
    std::tuple<std::shared_ptr<ColumnDef>, Status> GetColumnDefByColumnName(const std::string &column_name, size_t *column_idx = nullptr);
    std::tuple<std::shared_ptr<ColumnDef>, Status> GetColumnDefByColumnID(const size_t &column_idx);

    Status GetTableInfo(TableInfo &table_info);

    Status GetTableDetail(TableDetail &table_detail);

    std::pair<std::string, std::string> GetDBTableName() const { return MakePair(db_name_, table_name_); }
    void SetDBTableName(const std::string &db_name, const std::string &table_name) {
        db_name_ = db_name;
        table_name_ = table_name;
    }

    Status AddColumn(const ColumnDef &column_def);

    Status AddFtIndexCache(std::shared_ptr<TableIndexReaderCache> ft_index_cache);

    Status GetFtIndexCache(std::shared_ptr<TableIndexReaderCache> &ft_index_cache);

    Status RemoveFtIndexCache();

    Status InvalidateFtIndexCache();

    Status GetNextColumnID(ColumnID &next_column_id);

    Status SetNextColumnID(ColumnID next_column_id);
    
    Status GetNextRowID(RowID &next_row_id);

    std::tuple<std::string, Status> GetNextIndexID();

    Status SetNextIndexID(const std::string &index_id_str);

    std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> MapMetaToSnapShotInfo(const std::string &db_name, const std::string &table_name);


    Status RestoreFromSnapshot(WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, bool is_link_files = false);

    Status SetBeginTS(TxnTimeStamp begin_ts);

    std::tuple<size_t,Status> GetTableRowCount();


private:
    Status LoadComment();

    Status LoadColumnDefs();

    Status LoadSegmentIDs1();

    Status LoadIndexIDs();

    Status LoadUnsealedSegmentID();

    Status LoadNextColumnID();

    std::string GetTableTag(const std::string &tag) const;

private:
    std::mutex mtx_;

    TxnTimeStamp begin_ts_ = 0;
    TxnTimeStamp commit_ts_;
    NewTxn *txn_{};
    KVInstance *kv_instance_{};
    std::string db_id_str_;
    std::string table_id_str_;
    std::string db_name_{};
    std::string table_name_{};

    std::optional<std::string> comment_;
    std::optional<std::vector<std::shared_ptr<ColumnDef>>> column_defs_;
    // std::optional<std::vector<SegmentID>> segment_ids_;
    std::optional<std::vector<SegmentID>> segment_ids1_;

    std::optional<std::vector<std::string>> index_id_strs_;
    std::optional<std::vector<std::string>> index_names_;
    std::optional<SegmentID> next_segment_id_;
    std::optional<SegmentID> unsealed_segment_id_;
    std::optional<ColumnID> next_column_id_;
};

} // namespace infinity

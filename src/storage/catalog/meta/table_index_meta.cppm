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

export module infinity_core:table_index_meta;

import :status;
import :meta_info;
import :new_catalog;
import :snapshot_info;

import column_def;

namespace infinity {

class KVInstance;
class TableMeta;
class IndexBase;
// struct SegmentUpdateTS;
// class NewCatalog;
class NewTxn;
// enum class UsageFlag;

// Enum for secondary index cardinality
export enum class SecondaryIndexCardinality : u8 { kHighCardinality = 0, kLowCardinality = 1, kInvalid = 255 };

export class TableIndexMeta {
public:
    TableIndexMeta(const std::string &index_id_str, const std::string &index_name, TableMeta &table_meta);

    ~TableIndexMeta();

    KVInstance &kv_instance() const { return kv_instance_; }

    TableMeta &table_meta() const { return table_meta_; }

    const std::string &index_id_str() const { return index_id_str_; }

    std::tuple<std::shared_ptr<IndexBase>, Status> GetIndexBase();

    Status SetIndexBase(const std::shared_ptr<IndexBase> &index_base);

    std::shared_ptr<std::string> GetTableIndexDir();

    std::tuple<std::shared_ptr<ColumnDef>, Status> GetColumnDef();

    Status GetTableIndexInfo(TableIndexInfo &table_index_info);

    std::tuple<std::vector<SegmentID> *, Status> GetSegmentIndexIDs1();

    Status SetSegmentIDs(const std::vector<SegmentID> &segment_ids);

    bool HasSegmentIndexID(SegmentID segment_id);

    Status AddSegmentID(SegmentID segment_id);

    Status AddSegmentIndexID1(SegmentID segment_id, NewTxn *new_txn);

    Status RemoveSegmentIndexIDs(const std::vector<SegmentID> &segment_ids);

    std::tuple<std::shared_ptr<TableIndexSnapshotInfo>, Status> MapMetaToSnapShotInfo();

private:
    Status GetSegmentUpdateTS(std::shared_ptr<SegmentUpdateTS> &segment_update_ts);

public:
    Status InitSet1(const std::shared_ptr<IndexBase> &index_base, NewCatalog *new_catalog);

    Status UninitSet1(UsageFlag usage_flag);

private:
    Status LoadSegmentIDs();

    std::string GetTableIndexTag(const std::string &tag) const;

public:
    std::string FtIndexCacheTag() const;

    // Methods for secondary index cardinality
    Status SetSecondaryIndexCardinality(SecondaryIndexCardinality cardinality);

    std::tuple<SecondaryIndexCardinality, Status> GetSecondaryIndexCardinality();

private:
    mutable std::mutex mtx_;

    KVInstance &kv_instance_;
    TableMeta &table_meta_;
    std::string index_id_str_;
    std::string index_name_str_;

    std::shared_ptr<IndexBase> index_def_;
    std::optional<std::vector<SegmentID>> segment_ids_;
    std::shared_ptr<SegmentUpdateTS> segment_update_ts_;
};

} // namespace infinity

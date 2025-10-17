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

module infinity_core:table_index_meta.impl;

import :table_index_meta;
import :kv_store;
import :table_meta;
import :kv_code;
import :index_base;
import :meta_info;
import :new_catalog;
import :infinity_context;
import :new_txn;
import :txn_state;
import :infinity_exception;
import :utility;
import :kv_utility;
import :snapshot_info;
import :segment_index_meta;
import :meta_cache;

import std;
import third_party;

import column_def;
import create_index_info;

namespace infinity {

TableIndexMeta::TableIndexMeta(const std::string &index_id_str, const std::string &index_name, TableMeta &table_meta)
    : kv_instance_(*table_meta.kv_instance()), table_meta_(table_meta), index_id_str_(index_id_str), index_name_str_(index_name) {
    if (index_name_str_.empty()) {
        index_def_ =
            infinity::GetTableIndexDef(&kv_instance_, table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, table_meta_.begin_ts());
        index_name_str_ = *index_def_->index_name_;
    }
}

TableIndexMeta::~TableIndexMeta() = default;

std::tuple<std::shared_ptr<IndexBase>, Status> TableIndexMeta::GetIndexBase() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (index_def_.get() != nullptr) {
        return {index_def_, Status::OK()};
    }

    MetaCache *meta_cache = table_meta_.meta_cache();
    u64 db_id = table_meta_.db_id();
    u64 table_id = table_meta_.table_id();
    std::shared_ptr<MetaIndexCache> index_cache = meta_cache->GetIndex(db_id, table_id, index_name_str_, table_meta_.begin_ts());
    if (index_cache.get() != nullptr) {
        index_def_ = index_cache->get_index_def();
        if (index_def_.get() != nullptr) {
            return {index_def_, Status::OK()};
        }
    }

    index_def_ =
        infinity::GetTableIndexDef(&kv_instance_, table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, table_meta_.begin_ts());

    if (index_cache.get() != nullptr && table_meta_.txn() != nullptr) {
        table_meta_.txn()->AddCacheInfo(
            std::make_shared<IndexCacheIndexDefInfo>(db_id, table_id, index_name_str_, table_meta_.begin_ts(), index_def_));
    }

    return {index_def_, Status::OK()};
}

Status TableIndexMeta::SetIndexBase(const std::shared_ptr<IndexBase> &index_base) {
    std::string index_def_key = GetTableIndexTag("index_base");
    Status status = kv_instance_.Put(index_def_key, index_base->Serialize().dump());
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

std::shared_ptr<std::string> TableIndexMeta::GetTableIndexDir() {
    return std::make_shared<std::string>(
        fmt::format("db_{}/tbl_{}/idx_{}", table_meta_.db_id_str(), table_meta_.GetTableDir()->c_str(), index_id_str_));
}

std::tuple<std::shared_ptr<ColumnDef>, Status> TableIndexMeta::GetColumnDef() {
    auto [index_base, status] = GetIndexBase();
    if (!status.ok()) {
        return {nullptr, status};
    }

    return table_meta_.GetColumnDefByColumnName(index_base->column_name());
}

std::tuple<std::vector<SegmentID> *, Status> TableIndexMeta::GetSegmentIndexIDs1() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!segment_ids_) {
        segment_ids_ = infinity::GetTableIndexSegments(&kv_instance_,
                                                       table_meta_.db_id_str(),
                                                       table_meta_.table_id_str(),
                                                       index_id_str_,
                                                       table_meta_.begin_ts(),
                                                       table_meta_.commit_ts());
    }
    return {&*segment_ids_, Status::OK()};
}

bool TableIndexMeta::HasSegmentIndexID(SegmentID segment_id) {
    auto [segment_ids_ptr, status] = GetSegmentIndexIDs1();
    if (!status.ok()) {
        return false;
    }
    auto iter = std::find(segment_ids_ptr->begin(), segment_ids_ptr->end(), segment_id);
    if (iter == segment_ids_ptr->end()) {
        return false;
    }
    return true;
}

Status TableIndexMeta::AddSegmentIndexID1(SegmentID segment_id, NewTxn *new_txn) {

    std::string segment_id_key = KeyEncode::CatalogIdxSegmentKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, segment_id);
    std::string commit_ts_str;
    switch (new_txn->GetTxnState()) {
        case TxnState::kStarted: {
            commit_ts_str = "-1"; // Wait for commit
            new_txn->AddMetaKeyForCommit(segment_id_key);
            break;
        }
        case TxnState::kCommitting:
        case TxnState::kCommitted: {
            commit_ts_str = fmt::format("{}", new_txn->CommitTS());
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid transaction state: {}", TxnState2Str(new_txn->GetTxnState())));
        }
    }
    if (!segment_ids_) {
        segment_ids_ = std::vector<SegmentID>();
    }
    segment_ids_->push_back(segment_id);
    return kv_instance_.Put(segment_id_key, commit_ts_str);
}

Status TableIndexMeta::RemoveSegmentIndexIDs(const std::vector<SegmentID> &segment_ids) {

    for (SegmentID segment_id : segment_ids) {
        std::string segment_id_key = KeyEncode::CatalogIdxSegmentKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, segment_id);
        Status status = kv_instance_.Delete(segment_id_key);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status TableIndexMeta::GetSegmentUpdateTS(std::shared_ptr<SegmentUpdateTS> &segment_update_ts) {
    if (segment_update_ts_) {
        segment_update_ts = segment_update_ts_;
        return Status::OK();
    }
    std::string segment_update_ts_key = GetTableIndexTag("segment_update_ts");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->GetSegmentUpdateTS(segment_update_ts_key, segment_update_ts);
    if (!status.ok()) {
        return status;
    }
    segment_update_ts_ = segment_update_ts;
    return Status::OK();
}

Status TableIndexMeta::InitSet1(const std::shared_ptr<IndexBase> &index_base, NewCatalog *new_catalog) {
    {
        Status status = SetIndexBase(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        std::string segment_update_ts_key = GetTableIndexTag("segment_update_ts");
        auto segment_update_ts = std::make_shared<SegmentUpdateTS>();
        Status status = new_catalog->AddSegmentUpdateTS(segment_update_ts_key, segment_update_ts);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status TableIndexMeta::UninitSet1(UsageFlag usage_flag) {
    Status status;

    std::shared_ptr<IndexBase> index_base;
    std::tie(index_base, status) = GetIndexBase();
    if (!status.ok()) {
        return status;
    }
    if (usage_flag == UsageFlag::kOther) {
        if (index_base->index_type_ == IndexType::kFullText) {
            status = table_meta_.RemoveFtIndexCache();
            if (!status.ok() && status.code() != ErrorCode::kCatalogError) {
                return status;
            }

            NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
            std::string segment_update_ts_key = GetTableIndexTag("segment_update_ts");
            new_catalog->DropSegmentUpdateTSByKey(segment_update_ts_key);
        }
    }

    // Remove all segment index ids
    std::string segment_id_prefix = KeyEncode::CatalogIdxSegmentKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        kv_instance_.Delete(iter->Key().ToString());
        iter->Next();
    }

    // Remove index definition
    std::string index_def_key = GetTableIndexTag("index_base");
    status = kv_instance_.Delete(index_def_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

std::string TableIndexMeta::GetTableIndexTag(const std::string &tag) const {
    return KeyEncode::CatalogIndexTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, tag);
}

Status TableIndexMeta::GetTableIndexInfo(TableIndexInfo &table_index_info) {
    Status status;
    if (!segment_ids_) {
        segment_ids_ = infinity::GetTableIndexSegments(&kv_instance_,
                                                       table_meta_.db_id_str(),
                                                       table_meta_.table_id_str(),
                                                       index_id_str_,
                                                       table_meta_.begin_ts(),
                                                       table_meta_.commit_ts());
    }
    if (!index_def_) {
        index_def_ =
            infinity::GetTableIndexDef(&kv_instance_, table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, table_meta_.begin_ts());
    }

    std::shared_ptr<ColumnDef> column_def = nullptr;
    std::tie(column_def, status) = table_meta_.GetColumnDefByColumnName(index_def_->column_names_[0]);
    if (!status.ok()) {
        return status;
    }

    table_index_info.index_name_ = std::make_shared<std::string>(index_name_str_);
    table_index_info.index_entry_dir_ = GetTableIndexDir();
    table_index_info.segment_index_count_ = segment_ids_.value().size();
    table_index_info.index_comment_ = std::make_shared<std::string>(*index_def_->index_comment_);
    table_index_info.index_type_ = std::make_shared<std::string>(IndexInfo::IndexTypeToString(index_def_->index_type_));
    table_index_info.index_other_params_ = std::make_shared<std::string>(index_def_->BuildOtherParamsString());
    table_index_info.index_column_names_ = std::make_shared<std::string>(column_def->name_);
    table_index_info.index_column_ids_ = std::make_shared<std::string>(std::to_string(column_def->id_));

    return Status::OK();
}

std::tuple<std::shared_ptr<TableIndexSnapshotInfo>, Status> TableIndexMeta::MapMetaToSnapShotInfo() {
    std::shared_ptr<TableIndexSnapshotInfo> table_index_snapshot_info = std::make_shared<TableIndexSnapshotInfo>();
    table_index_snapshot_info->index_dir_ = GetTableIndexDir();
    table_index_snapshot_info->index_id_str_ = std::make_shared<std::string>(index_id_str_);
    auto [index_base, status] = GetIndexBase();
    if (!status.ok()) {
        return {nullptr, status};
    }
    table_index_snapshot_info->index_base_ = index_base;
    if (!segment_ids_) {
        segment_ids_ = infinity::GetTableIndexSegments(&kv_instance_,
                                                       table_meta_.db_id_str(),
                                                       table_meta_.table_id_str(),
                                                       index_id_str_,
                                                       table_meta_.begin_ts(),
                                                       table_meta_.commit_ts());
    }
    for (const auto &segment_id : *segment_ids_) {
        SegmentIndexMeta segment_index_meta(segment_id, *this);
        auto [segment_index_snapshot, segment_index_status] = segment_index_meta.MapMetaToSnapShotInfo();
        if (!segment_index_status.ok()) {
            return {nullptr, segment_index_status};
        }
        table_index_snapshot_info->segment_index_snapshots_.emplace_back(segment_index_snapshot);
    }
    return {table_index_snapshot_info, Status::OK()};
}

Status TableIndexMeta::SetSecondaryIndexCardinality(SecondaryIndexCardinality cardinality) {
    std::string cardinality_key = GetTableIndexTag("cardinality");
    u8 cardinality_value = static_cast<u8>(cardinality);
    Status status = kv_instance_.Put(cardinality_key, std::string(reinterpret_cast<const char *>(&cardinality_value), sizeof(cardinality_value)));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

std::tuple<SecondaryIndexCardinality, Status> TableIndexMeta::GetSecondaryIndexCardinality() {
    std::string cardinality_key = GetTableIndexTag("cardinality");
    std::string cardinality_value_str;
    Status status = kv_instance_.Get(cardinality_key, cardinality_value_str);
    if (!status.ok()) {
        // Default to HighCardinality if not set
        return {SecondaryIndexCardinality::kHighCardinality, Status::OK()};
    }

    if (cardinality_value_str.size() != sizeof(u8)) {
        return {SecondaryIndexCardinality::kInvalid, Status::InvalidIndexParam("Invalid cardinality data")};
    }

    u8 cardinality_value = *reinterpret_cast<const u8 *>(cardinality_value_str.data());
    SecondaryIndexCardinality cardinality = static_cast<SecondaryIndexCardinality>(cardinality_value);

    if (cardinality != SecondaryIndexCardinality::kHighCardinality && cardinality != SecondaryIndexCardinality::kLowCardinality) {
        return {SecondaryIndexCardinality::kInvalid, Status::InvalidIndexParam("Invalid cardinality value")};
    }

    return {cardinality, Status::OK()};
}

} // namespace infinity
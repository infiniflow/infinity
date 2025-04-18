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

#include <string>

module table_index_meeta;

import kv_store;
import table_meeta;
import kv_code;
import third_party;
import logger;
import index_base;
import meta_info;
import create_index_info;
import new_catalog;
import infinity_context;

namespace infinity {

TableIndexMeeta::TableIndexMeeta(String index_id_str, TableMeeta &table_meta)
    : kv_instance_(table_meta.kv_instance()), table_meta_(table_meta), index_id_str_(std::move(index_id_str)) {}

TableIndexMeeta::~TableIndexMeeta() = default;

Tuple<SharedPtr<IndexBase>, Status> TableIndexMeeta::GetIndexBase() {
    if (!index_def_) {
        Status status = LoadIndexDef();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {index_def_, Status::OK()};
}

Status TableIndexMeeta::SetIndexBase(const SharedPtr<IndexBase> &index_base) {
    String index_def_key = GetTableIndexTag("index_base");
    Status status = kv_instance_.Put(index_def_key, index_base->Serialize().dump());
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

SharedPtr<String> TableIndexMeeta::GetTableIndexDir() {
    return MakeShared<String>(fmt::format("tbl_{}/idx_{}", table_meta_.GetTableDir()->c_str(), index_id_str_));
}

Tuple<SharedPtr<ColumnDef>, Status> TableIndexMeeta::GetColumnDef() {
    auto [index_base, status] = GetIndexBase();
    if (!status.ok()) {
        return {nullptr, status};
    }

    return table_meta_.GetColumnDefByColumnName(index_base->column_name());
}

Status TableIndexMeeta::GetSegmentIDs(Vector<SegmentID> *&segment_ids) {
    if (!segment_ids_) {
        Status status = LoadSegmentIDs();
        if (!status.ok()) {
            return status;
        }
    }
    segment_ids = &segment_ids_.value();
    return Status::OK();
}

Tuple<Vector<SegmentID> *, Status> TableIndexMeeta::GetSegmentIDs1() {
    if (!segment_ids_) {
        auto status = LoadSegmentIDs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {&*segment_ids_, Status::OK()};
}

Status TableIndexMeeta::SetSegmentIDs(const Vector<SegmentID> &segment_ids) {
    String segment_ids_key = GetTableIndexTag("segment_ids");
    String segment_ids_str = nlohmann::json(segment_ids).dump();
    Status status = kv_instance_.Put(segment_ids_key, segment_ids_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableIndexMeeta::AddSegmentID(SegmentID segment_id) {
    if (!segment_ids_) {
        Status status = LoadSegmentIDs();
        if (!status.ok()) {
            return status;
        }
    }
    segment_ids_->push_back(segment_id);
    Status status = SetSegmentIDs(*segment_ids_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableIndexMeeta::AddSegmentID1(SegmentID segment_id, TxnTimeStamp commit_ts) {
    String segment_id_key = KeyEncode::CatalogIdxSegmentKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, segment_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance_.Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableIndexMeeta::GetSegmentUpdateTS(SharedPtr<SegmentUpdateTS> &segment_update_ts) {
    if (segment_update_ts_) {
        segment_update_ts = segment_update_ts_;
        return Status::OK();
    }
    String segment_update_ts_key = GetTableIndexTag("segment_update_ts");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->GetSegmentUpdateTS(segment_update_ts_key, segment_update_ts);
    if (!status.ok()) {
        return status;
    }
    segment_update_ts_ = segment_update_ts;
    return Status::OK();
}

Status TableIndexMeeta::UpdateFulltextSegmentTS(TxnTimeStamp ts) {
    SharedPtr<SegmentUpdateTS> segment_update_ts;
    Status status = GetSegmentUpdateTS(segment_update_ts);
    if (!status.ok()) {
        return status;
    }
    status = table_meta_.UpdateFulltextSegmentTS(ts, *segment_update_ts);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableIndexMeeta::InitSet(const SharedPtr<IndexBase> &index_base, NewCatalog *new_catalog) {
    {
        Status status = SetSegmentIDs({});
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetIndexBase(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        String segment_update_ts_key = GetTableIndexTag("segment_update_ts");
        auto segment_update_ts = MakeShared<SegmentUpdateTS>();
        Status status = new_catalog->AddSegmentUpdateTS(segment_update_ts_key, segment_update_ts);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status TableIndexMeeta::UninitSet() {
    Status status;

    SharedPtr<IndexBase> index_base;
    std::tie(index_base, status) = GetIndexBase();
    if (!status.ok()) {
        return status;
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        status = table_meta_.RemoveFtIndexCache();
        if (!status.ok()) {
            return status;
        }

        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        String segment_update_ts_key = GetTableIndexTag("segment_update_ts");
        status = new_catalog->DropSegmentUpdateTSByKey(segment_update_ts_key);
        if (!status.ok()) {
            return status;
        }
    }

    String segment_ids_key = GetTableIndexTag("segment_ids");
    status = kv_instance_.Delete(segment_ids_key);
    if (!status.ok()) {
        return status;
    }

    String index_def_key = GetTableIndexTag("index_base");
    status = kv_instance_.Delete(index_def_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status TableIndexMeeta::LoadIndexDef() {
    String index_def_key = GetTableIndexTag("index_base");
    String index_def_str;
    Status status = kv_instance_.Get(index_def_key, index_def_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get index definition from kv store, key: {}, cause: {}", index_def_key, status.message()));
        return status;
    }
    nlohmann::json index_def_json = nlohmann::json::parse(index_def_str);
    index_def_ = IndexBase::Deserialize(index_def_json);
    return Status::OK();
}

Status TableIndexMeeta::LoadSegmentIDs() {
    String segment_ids_key = GetTableIndexTag("segment_ids");
    String segment_ids_str;
    Status status = kv_instance_.Get(segment_ids_key, segment_ids_str);
    if (!status.ok()) {
        return status;
    }
    Vector<SegmentID> segment_ids = nlohmann::json::parse(segment_ids_str).get<Vector<SegmentID>>();
    segment_ids_ = segment_ids;
    return Status::OK();
}

Status TableIndexMeeta::LoadSegmentIDs1() {
    segment_ids_ = Vector<SegmentID>();
    Vector<SegmentID> &segment_ids = *segment_ids_;

    TxnTimeStamp begin_ts = table_meta_.begin_ts();
    String segment_id_prefix = KeyEncode::CatalogIdxSegmentKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(segment_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts) {
            iter->Next();
            continue;
        }
        SegmentID segment_id = std::stoull(iter->Key().ToString().substr(segment_id_prefix.size()));
        segment_ids.push_back(segment_id);
        iter->Next();
    }

    std::sort(segment_ids.begin(), segment_ids.end());
    return Status::OK();
}

String TableIndexMeeta::GetTableIndexTag(const String &tag) const {
    return KeyEncode::CatalogIndexTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, tag);
}

String TableIndexMeeta::FtIndexCacheTag() const { return GetTableIndexTag("ft_cache"); }

Status TableIndexMeeta::GetTableIndexInfo(TableIndexInfo &table_index_info) {
    Status status;
    if (!segment_ids_) {
        status = LoadSegmentIDs();
        if (!status.ok()) {
            return status;
        }
    }
    if (!index_def_) {
        status = LoadIndexDef();
        if (!status.ok()) {
            return status;
        }
    }

    SharedPtr<ColumnDef> column_def = nullptr;
    std::tie(column_def, status) = table_meta_.GetColumnDefByColumnName(index_def_->column_names_[0]);
    if (!status.ok()) {
        return status;
    }

    table_index_info.index_name_ = index_def_->index_name_;
    table_index_info.index_entry_dir_ = GetTableIndexDir();
    table_index_info.segment_index_count_ = segment_ids_.value().size();
    table_index_info.index_comment_ = MakeShared<String>(*index_def_->index_comment_);
    table_index_info.index_type_ = MakeShared<String>(IndexInfo::IndexTypeToString(index_def_->index_type_));
    table_index_info.index_other_params_ = MakeShared<String>(index_def_->BuildOtherParamsString());
    table_index_info.index_column_names_ = MakeShared<String>(column_def->name_);
    table_index_info.index_column_ids_ = MakeShared<String>(std::to_string(column_def->id_));

    return Status::OK();
}

} // namespace infinity
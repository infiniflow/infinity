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

module table_meeta;

import status;
import kv_code;
import kv_store;
import column_def;
import third_party;
import default_values;
import logger;
import table_def;

namespace infinity {

TableMeeta::TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance &kv_instance)
    : kv_instance_(kv_instance), db_id_str_(db_id_str), table_id_str_(table_id_str) {}

Status TableMeeta::GetIndexIDs(Vector<String> *&index_id_strs, Vector<String> **index_names) {
    if (!index_id_strs_ || !index_names_) {
        Status status = LoadIndexIDs();
        if (!status.ok()) {
            return status;
        }
    }
    index_id_strs = &index_id_strs_.value();
    if (index_names) {
        *index_names = &index_names_.value();
    }
    return Status::OK();
}

Tuple<SharedPtr<ColumnDef>, Status> TableMeeta::GetColumnDefByColumnName(const String &column_name) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_def_str;
    Status status = kv_instance_.Get(column_key, column_def_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get column definition from kv store, key: {}, cause: {}", column_key, status.message()));
        return {nullptr, status};
    }
    return {ColumnDef::FromJson(nlohmann::json::parse(column_def_str)), Status::OK()};
}

Status TableMeeta::SetSegmentIDs(const Vector<SegmentID> &segment_ids) {
    segment_ids_ = segment_ids;
    String segment_ids_key = GetTableTag("segment_ids");
    String segment_ids_str = nlohmann::json(segment_ids).dump();
    Status status = kv_instance_.Put(segment_ids_key, segment_ids_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to set segment ids to kv store, key: {}, cause: {}", segment_ids_key, status.message()));
        return status;
    }
    return Status::OK();
}

Status TableMeeta::AddSegmentID(SegmentID segment_id) {
    if (!segment_ids_) {
        Status status = LoadSegmentIDs();
        //        if (!status.ok() && status.code() != ErrorCode::kNotFound) {
        //            return status;
        //        }
        //        segment_ids_ = Vector<SegmentID>();
        if (!status.ok()) {
            return status;
        }
    }

    segment_ids_->emplace_back(segment_id);
    Status status = SetSegmentIDs(*segment_ids_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::InitSet(SharedPtr<TableDef> table_def) {
    Status status;

    // Create table comment;
    if (table_def->table_comment() != nullptr and !table_def->table_comment()->empty()) {
        String &table_comment = *table_def->table_comment();
        String table_comment_key = GetTableTag("comment");
        status = kv_instance_.Put(table_comment_key, table_comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create table column id;
    String table_latest_column_id_key = GetTableTag(LATEST_COLUMN_ID.data());
    status = kv_instance_.Put(table_latest_column_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // Create table segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_.Put(table_latest_segment_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    {
        // Create segment ids
        String table_segment_ids_key = GetTableTag("segment_ids");
        String table_segment_ids_str = nlohmann::json::array().dump();
        Status status = kv_instance_.Put(table_segment_ids_key, table_segment_ids_str);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status TableMeeta::UninitSet() {
    Status status;

    // delete table segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_.Delete(table_latest_segment_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table column id;
    String table_latest_column_id_key = GetTableTag(LATEST_COLUMN_ID.data());
    status = kv_instance_.Delete(table_latest_column_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String table_comment_key = GetTableTag("comment");
    status = kv_instance_.Delete(table_comment_key);
    if (!status.ok()) {
        return status;
    }

    String segment_ids_key = GetTableTag("segment_ids");
    status = kv_instance_.Delete(segment_ids_key);
    if (!status.ok()) {
        return status;
    }

    String unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    status = kv_instance_.Delete(unsealed_seg_id_key);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
    }

    return Status::OK();
}

Status TableMeeta::LoadColumnDefs() {
    Vector<SharedPtr<ColumnDef>> column_defs;

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(table_column_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String column_key = iter2->Key().ToString();
        String column_value = iter2->Value().ToString();
        [[maybe_unused]] String column_name = column_key.substr(column_key.find_last_of('|') + 1);
        auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value));
        column_defs.emplace_back(column_def);
        iter2->Next();
    }
    column_defs_ = std::move(column_defs);

    return Status::OK();
}

Status TableMeeta::LoadSegmentIDs() {
    String segment_ids_key = GetTableTag("segment_ids");
    String segment_ids_str;
    Status status = kv_instance_.Get(segment_ids_key, segment_ids_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get segment ids from kv store, key: {}, cause: {}", segment_ids_key, status.message()));
        return status;
    }
    segment_ids_ = nlohmann::json::parse(segment_ids_str).get<Vector<SegmentID>>();
    return Status::OK();
}

Status TableMeeta::LoadIndexIDs() {
    Vector<String> index_id_strs;
    Vector<String> index_names;
    String index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(index_prefix);
    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        String column_key = iter->Key().ToString();
        String column_key1 = column_key.substr(index_prefix.size());
        String index_name = column_key1.substr(0, column_key1.find('|'));
        String index_id_str = iter->Value().ToString();
        index_names.emplace_back(std::move(index_name));
        index_id_strs.emplace_back(std::move(index_id_str));
        iter->Next();
    }
    index_id_strs_ = std::move(index_id_strs);
    index_names_ = std::move(index_names);
    return Status::OK();
}

Status TableMeeta::LoadNextSegmentID() {
    String next_seg_id_key = GetTableTag("next_segment_id");
    String next_seg_id_str;
    Status status = kv_instance_.Get(next_seg_id_key, next_seg_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next segment id from kv store, key: {}, cause: {}", next_seg_id_key, status.message()));
        return status;
    }
    next_segment_id_ = std::stoull(next_seg_id_str);
    return Status::OK();
}

Status TableMeeta::LoadUnsealedSegmentID() {
    String unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_seg_id_str;
    Status status = kv_instance_.Get(unsealed_seg_id_key, unsealed_seg_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get unsealed segment id from kv store, key: {}, cause: {}", unsealed_seg_id_key, status.message()));
        return status;
    }
    unsealed_segment_id_ = std::stoull(unsealed_seg_id_str);
    return Status::OK();
}

String TableMeeta::GetTableTag(const String &tag) const { return KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag); }

Status TableMeeta::SetNextSegmentID(SegmentID next_segment_id) {
    next_segment_id_ = next_segment_id;
    String next_id_key = GetTableTag("next_segment_id");
    String next_id_str = fmt::format("{}", next_segment_id);
    Status status = kv_instance_.Put(next_id_key, next_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to set next segment id from kv store, key: {}:{}, cause: {}", next_id_key, next_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

Status TableMeeta::SetUnsealedSegmentID(SegmentID unsealed_segment_id) {
    unsealed_segment_id_ = unsealed_segment_id;
    String unsealed_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_id_str = fmt::format("{}", unsealed_segment_id);
    Status status = kv_instance_.Put(unsealed_id_key, unsealed_id_str);
    if (!status.ok()) {
        LOG_ERROR(
            fmt::format("Fail to set unsealed segment id from kv store, key: {}:{}, cause: {}", unsealed_id_key, unsealed_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

Tuple<ColumnID, Status> TableMeeta::GetColumnIDByColumnName(const String &column_name) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_value_str;
    Status status = kv_instance_.Get(column_key, column_value_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get column id by column name: key: {}, cause: {}", column_key, status.message()));
        return {INVALID_COLUMN_ID, status};
    }
    auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value_str));
    return {column_def->id_, Status::OK()};
}

SharedPtr<String> TableMeeta::GetTableDir() { return {MakeShared<String>(table_id_str_)}; }

Tuple<SharedPtr<Vector<SegmentID>>, Status> TableMeeta::GetSegmentIDs() {
    if (!segment_ids_) {
        auto status = LoadSegmentIDs();
        if (!status.ok()) {
            //            if (status.code() == ErrorCode::kNotFound) {
            //                return {MakeShared<Vector<SegmentID>>(), Status::OK()};
            //            } else {
            return {nullptr, status};
            //            }
        }
    }
    return {MakeShared<Vector<SegmentID>>(segment_ids_.value()), Status::OK()};
}

Tuple<SharedPtr<Vector<SharedPtr<ColumnDef>>>, Status> TableMeeta::GetColumnDefs() {
    if (!column_defs_) {
        auto status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {MakeShared<Vector<SharedPtr<ColumnDef>>>(column_defs_.value()), Status::OK()};
}

} // namespace infinity

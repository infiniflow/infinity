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

namespace infinity {

TableMeeta::TableMeeta(const String &db_id_str, const String &table_id_str, const String &db_name, const String &table_name, KVInstance &kv_instance)
    : kv_instance_(kv_instance), db_id_str_(db_id_str), table_id_str_(table_id_str), db_name_(db_name), table_name_(table_name) {}

Status TableMeeta::GetColumnDefs(Vector<SharedPtr<ColumnDef>> *&column_defs) {
    if (!column_defs_) {
        auto status = LoadColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    column_defs = &column_defs_.value();
    return Status::OK();
}

Status TableMeeta::GetSegmentIDs(Vector<SegmentID> *&segment_ids) {
    if (!segment_ids_) {
        auto status = LoadSegmentIDs();
        if (!status.ok()) {
            return status;
        }
    }
    segment_ids = &segment_ids_.value();
    return Status::OK();
}

Status TableMeeta::GetNextSegmentID(SegmentID &next_segment_id) {
    if (!next_segment_id_) {
        auto status = LoadNextSegmentID();
        if (!status.ok()) {
            return status;
        }
    }
    next_segment_id = *next_segment_id_;
    return Status::OK();
}

Status TableMeeta::GetTableDir(String *&table_dir) {
    if (!table_dir_) {
        auto status = LoadTableDir();
        if (!status.ok()) {
            return status;
        }
    }
    table_dir = &table_dir_.value();
    return Status::OK();
}

Status TableMeeta::GetColumnDefByColumnName(const String &column_name, SharedPtr<ColumnDef> &column_def) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_def_str;
    Status status = kv_instance_.Get(column_key, column_def_str);
    if (!status.ok()) {
        return status;
    }
    column_def = ColumnDef::FromJson(nlohmann::json::parse(column_def_str));
    return Status::OK();
}

Status TableMeeta::SetSegmentIDs(const Vector<SegmentID> &segment_ids) {
    segment_ids_ = segment_ids;
    String segment_ids_key = GetTableTag("segment_ids");
    String segment_ids_str = nlohmann::json(segment_ids).dump();
    Status status = kv_instance_.Put(segment_ids_key, segment_ids_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::AddSegmentID(SegmentID segment_id) {
    if (!segment_ids_) {
        LoadSegmentIDs();
    }
    segment_ids_->emplace_back(segment_id);
    Status status = SetSegmentIDs(*segment_ids_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::SetNextSegmentID(SegmentID next_segment_id) {
    next_segment_id_ = next_segment_id;
    String next_id_key = GetTableTag(String(LATEST_SEGMENT_ID));
    String next_id_str = fmt::format("{}", next_segment_id);
    Status status = kv_instance_.Put(next_id_key, next_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::InitSet() {
    {
        Status status = SetSegmentIDs(Vector<SegmentID>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNextSegmentID(0);
        if (!status.ok()) {
            return status;
        }
    }
    // TODO: set table_dir
    return Status::OK();
}

Status TableMeeta::Init() {
    {
        Status status = SetSegmentIDs({0});
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetLatestSegmentID(0);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetTableDir("0");
        if (!status.ok()) {
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
        return status;
    }
    segment_ids_ = nlohmann::json::parse(segment_ids_str).get<Vector<SegmentID>>();
    segment_id_set_.clear();
    for (SegmentID segment_id : *segment_ids_) {
        segment_id_set_.insert(segment_id);
    }
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
    String next_id_key = GetTableTag(String(LATEST_SEGMENT_ID));
    String next_id_str;
    Status status = kv_instance_.Get(next_id_key, next_id_str);
    if (!status.ok()) {
        return status;
    }
    next_segment_id_ = std::stoull(next_id_str);
    return Status::OK();
}

Status TableMeeta::LoadLatestSegmentID() {
    String current_seg_id_key = GetTableTag(String(LATEST_SEGMENT_ID));
    String current_seg_id_str;
    Status status = kv_instance_.Get(current_seg_id_key, current_seg_id_str);
    if (!status.ok()) {
        return status;
    }
    latest_segment_id_ = std::stoull(current_seg_id_str);
    return Status::OK();
}

Status TableMeeta::LoadTableDir() {
    String table_dir_key = GetTableTag("dir");
    String table_dir;
    Status status = kv_instance_.Get(table_dir_key, table_dir);
    if (!status.ok()) {
        return status;
    }
    table_dir_ = table_dir;
    return Status::OK();
}

String TableMeeta::GetTableTag(const String &tag) const { return KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag); }

Tuple<SegmentID, Status> TableMeeta::GetLatestSegmentID() {
    if (!latest_segment_id_) {
        auto status = LoadLatestSegmentID();
        if (!status.ok()) {
            return {INVALID_SEGMENT_ID, status};
        }
    }
    return {*latest_segment_id_, Status::OK()};
}

Status TableMeeta::SetLatestSegmentID(SegmentID latest_segment_id) {
    latest_segment_id_ = latest_segment_id;
    String latest_id_key = GetTableTag(String(LATEST_SEGMENT_ID));
    String latest_id_str = fmt::format("{}", latest_segment_id);
    Status status = kv_instance_.Put(latest_id_key, latest_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Tuple<ColumnID, Status> TableMeeta::GetColumnIDByColumnName(const String &column_name) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_value_str;
    Status status = kv_instance_.Get(column_key, column_value_str);
    if (!status.ok()) {
        return {INVALID_COLUMN_ID, status};
    }
    auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value_str));
    return {column_def->id_, Status::OK()};
}

Tuple<SharedPtr<String>, Status> TableMeeta::GetTableDir() {
    if (!table_dir_) {
        auto status = LoadTableDir();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {MakeShared<String>(table_dir_.value()), Status::OK()};
}

Tuple<SharedPtr<Vector<SegmentID>>, Status> TableMeeta::GetSegmentIDs() {
    if (!segment_ids_) {
        auto status = LoadSegmentIDs();
        if (!status.ok()) {
            return {nullptr, status};
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

Status TableMeeta::SetTableDir(const String &dir) {
    if (table_dir_) {
        return Status::UnexpectedError(fmt::format("Table dir is already set as: {}", table_dir_.value()));
    }

    table_dir_ = dir;
    String table_dir_key = GetTableTag("dir");
    String table_dir;
    Status status = kv_instance_.Get(table_dir_key, table_dir);
    if (status.ok()) {
        return Status::UnexpectedError(fmt::format("Table dir is already set as: {}", table_dir_.value()));
    }

    if (status.code() == ErrorCode::kNotFound) {
        return kv_instance_.Put(table_dir_key, dir);
    } else {
        return status;
    }
}

} // namespace infinity

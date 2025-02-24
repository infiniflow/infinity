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

TableMeeta::TableMeeta(String table_id_str, KVInstance &kv_instance) : kv_instance_(kv_instance), table_id_str_(table_id_str) {}

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

Tuple<SegmentID, Status> TableMeeta::GetLatestSegmentID() {
    if (!current_segment_id_) {
        auto status = LoadCurrentSegmentID();
        if (!status.ok()) {
            return {INVALID_SEGMENT_ID, status};
        }
    }
    return {*current_segment_id_, Status::OK()};
}

Status TableMeeta::GetColumnIDByColumnName(const String &column_name, ColumnID &column_id) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_id_str;
    Status status = kv_instance_.Get(column_key, column_id_str);
    if (!status.ok()) {
        return status;
    }
    column_id = std::stoull(column_id_str);
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

Status TableMeeta::LoadCurrentSegmentID() {
    String current_seg_id_key = GetTableTag(String(LATEST_SEGMENT_ID));
    String current_seg_id_str;
    Status status = kv_instance_.Get(current_seg_id_key, current_seg_id_str);
    if (!status.ok()) {
        return status;
    }
    current_segment_id_ = std::stoull(current_seg_id_str);
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

} // namespace infinity

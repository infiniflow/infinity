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

module table_index_meeta;

import kv_store;
import table_meeta;
import kv_code;
import third_party;

namespace infinity {

TableIndexMeeta::TableIndexMeeta(String index_id_str, TableMeeta &table_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), table_meta_(table_meta), index_id_str_(std::move(index_id_str)) {}

Status TableIndexMeeta::GetColumnDef(SharedPtr<ColumnDef> &column_def) {
    SharedPtr<IndexBase> index_def;
    {
        Status status = GetIndexDef(index_def);
        if (!status.ok()) {
            return status;
        }
    }
    SizeT column_idx;
    {
        Status status = table_meta_.GetColumnIDByColumnName(index_def->column_name(), column_idx);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Vector<SharedPtr<ColumnDef>> *column_defs = nullptr;
        Status status = table_meta_.GetColumnDefs(column_defs);
        if (!status.ok()) {
            return status;
        }
        column_def = (*column_defs)[column_idx];
    }
    return Status::OK();
}

Status TableIndexMeeta::SetSegmentIDs(const Vector<SegmentID> &segment_ids) {
    //
    return Status::OK();
}

Status TableIndexMeeta::AddSegmentID(SegmentID segment_id) {
    //
    return Status::OK();
}

Status TableIndexMeeta::LoadIndexDef() {
    String index_def_key = GetTableIndexTag("index_def");
    String index_def_str;
    Status status = kv_instance_.Get(index_def_key, index_def_str);
    if (!status.ok()) {
        return status;
    }
    nlohmann::json index_def_json = nlohmann::json::parse(index_def_str);
    index_def_ = IndexBase::Deserialize(index_def_json);
    return Status::OK();
}

Status TableIndexMeeta::LoadIndexDir() {
    //
    return Status::OK();
}

Status TableIndexMeeta::LoadSegmentIDs() {
    //
    return Status::OK();
}

String TableIndexMeeta::GetTableIndexTag(const String &tag) {
    return KeyEncode::CatalogIndexTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), index_id_str_, tag);
}

} // namespace infinity
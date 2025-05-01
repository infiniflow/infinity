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

module meta_key;

import column_def;
import utility;
import third_party;
import kv_code;

namespace infinity {

ColumnMetaKey::ColumnMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, SharedPtr<ColumnDef> column_def)
    : MetaKey(Type::kColumn), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id), block_id_(block_id),
      column_def_(std::move(column_def)) {}

ColumnMetaKey::~ColumnMetaKey() = default;

String DBMetaKey::ToString() const { return fmt::format("{}:{}", KeyEncode::CatalogDbKey(db_name_, commit_ts_), db_id_str_); }
String TableMetaKey::ToString() const { return fmt::format("{}:{}", KeyEncode::CatalogTableKey(db_id_str_, table_name_, commit_ts_), table_id_str_); }
String SegmentMetaKey::ToString() const { return ""; }
String BlockMetaKey::ToString() const { return ""; }
String ColumnMetaKey::ToString() const { return ""; }
String TableIndexMetaKey::ToString() const { return ""; }
String SegmentIndexMetaKey::ToString() const { return ""; }
String ChunkIndexMetaKey::ToString() const { return ""; }

SharedPtr<MetaKey> MetaParse(const String &key, const String &value) {
    Vector<String> fields = infinity::Partition(key, '|');

    if (fields[0] == "catalog" && fields[1] == "db") {
        const String &db_name_str = fields[2];
        const String &commit_ts_str = fields[3];
        const String &db_id_str = value;
        SharedPtr<DBMetaKey> db_meta_key = MakeShared<DBMetaKey>(db_id_str, db_name_str);
        db_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return db_meta_key;
    }

    if (fields[0] == "catalog" && fields[1] == "tbl") {
        const String &db_id_str = fields[2];
        const String &table_name_str = fields[3];
        const String &commit_ts_str = fields[4];
        const String &table_id_str = value;

        SharedPtr<TableMetaKey> table_meta_key = MakeShared<TableMetaKey>(db_id_str, table_id_str, table_name_str);
        table_meta_key->commit_ts_ = std::stoull(commit_ts_str);
        return table_meta_key;
    }

    return nullptr;
}

} // namespace infinity
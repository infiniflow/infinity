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

module meta_key;

import column_def;

namespace infinity {

ColumnMetaKey::ColumnMetaKey(String db_id_str, String table_id_str, SegmentID segment_id, BlockID block_id, SharedPtr<ColumnDef> column_def)
    : MetaKey(Type::kColumn), db_id_str_(std::move(db_id_str)), table_id_str_(std::move(table_id_str)), segment_id_(segment_id), block_id_(block_id),
      column_def_(std::move(column_def)) {}

ColumnMetaKey::~ColumnMetaKey() = default;

} // namespace infinity
// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

export module cached_match_base;

import stl;
import cached_node_base;
import base_table_ref;
import logical_node_type;

namespace infinity {

export class CachedMatchBase : public CachedNodeBase {
public:
    CachedMatchBase(LogicalNodeType type,
                    SharedPtr<String> schema_name,
                    SharedPtr<String> table_name,
                    const Vector<ColumnID> column_ids,
                    TxnTimeStamp query_ts,
                    SharedPtr<Vector<String>> output_names)
        : CachedNodeBase(type, output_names), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)), column_ids_(column_ids),
          query_ts_(query_ts) {}

    u64 Hash() const override;

    bool Equal(const CachedMatchBase &other) const;

    const String &schema_name() const { return *schema_name_; }
    const String &table_name() const { return *table_name_; }

protected:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> table_name_{};
    Vector<ColumnID> column_ids_{};
    TxnTimeStamp query_ts_{};
};

} // namespace infinity
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

export module logical_create_index;

import stl;
import logical_node_type;
import column_binding;
import logical_node;

import base_table_ref;
import index_base;
import internal_types;
import extra_ddl_info;
import data_type;

namespace infinity {

export class LogicalCreateIndex : public LogicalNode {
public:
    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final; // TTT shenyushi: should be const

    inline String name() override { return "LogicalCreateIndex"; }

public:
    inline LogicalCreateIndex(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<IndexBase> index_base, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateIndex), base_table_ref_(base_table_ref), index_definition_(index_base),
          conflict_type_(conflict_type) {}

public:
    [[nodiscard]] inline SharedPtr<BaseTableRef> base_table_ref() const { return base_table_ref_; }

    [[nodiscard]] inline SharedPtr<IndexBase> index_definition() const { return index_definition_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<IndexBase> index_definition_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};
} // namespace infinity

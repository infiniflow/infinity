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

export module logical_optimize;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import data_type;
import internal_types;
import optimize_statement;
import statement_common;

namespace infinity {

export class LogicalOptimize : public LogicalNode {
public:
    explicit LogicalOptimize(u64 node_id, String schema_name, String table_name, String index_name, Vector<UniquePtr<InitParameter>> opt_params)
        : LogicalNode(node_id, LogicalNodeType::kOptimize), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), opt_params_(std::move(opt_params)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalOptimize"; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return table_name_; }

private:
    String schema_name_;
    String table_name_;

public:
    String index_name_;
    Vector<UniquePtr<InitParameter>> opt_params_;
};

} // namespace infinity

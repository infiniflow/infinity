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

export module infinity_core:logical_optimize;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import optimize_statement;
import statement_common;

namespace infinity {

export class LogicalOptimize : public LogicalNode {
public:
    explicit LogicalOptimize(u64 node_id, std::string schema_name, std::string table_name, std::string index_name, std::vector<std::unique_ptr<InitParameter>> opt_params)
        : LogicalNode(node_id, LogicalNodeType::kOptimize), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), opt_params_(std::move(opt_params)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalOptimize"; }

    [[nodiscard]] inline const std::string &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const std::string &object_name() const { return table_name_; }

private:
    std::string schema_name_;
    std::string table_name_;

public:
    std::string index_name_;
    std::vector<std::unique_ptr<InitParameter>> opt_params_;
};

} // namespace infinity

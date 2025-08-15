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

export module infinity_core:logical_project;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;
import :highlighter;

import data_type;
import third_party;
import internal_types;

namespace infinity {

export class LogicalProject : public LogicalNode {
public:
    inline LogicalProject(u64 node_id,
                          std::vector<std::shared_ptr<BaseExpression>> expressions,
                          u64 projection_index,
                          std::map<size_t, std::shared_ptr<HighlightInfo>> highlight_columns)
        : LogicalNode(node_id, LogicalNodeType::kProjection), expressions_(std::move(expressions)), table_index_(projection_index),
          highlight_columns_(std::move(highlight_columns)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return fmt::format("logical_project_{}", std::to_string(table_index_)); }

    std::vector<std::shared_ptr<BaseExpression>> expressions_{};

    u64 table_index_{};

    std::map<size_t, std::shared_ptr<HighlightInfo>> highlight_columns_{};

    bool total_hits_count_flag_{false};
};

} // namespace infinity

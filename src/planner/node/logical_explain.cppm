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

export module infinity_core:logical_explain;

import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import explain_statement;

namespace infinity {

export class LogicalExplain : public LogicalNode {
public:
    explicit LogicalExplain(u64 node_id, ExplainType type) : LogicalNode(node_id, LogicalNodeType::kExplain), explain_type_(type) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalExplain"; }

    [[nodiscard]] inline ExplainType explain_type() const { return explain_type_; }

    inline void SetText(const std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &texts) { texts_ = texts; }

    [[nodiscard]] inline const std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &TextArray() const { return texts_; }

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    std::shared_ptr<std::vector<std::shared_ptr<std::string>>> texts_{nullptr};
};

} // namespace infinity

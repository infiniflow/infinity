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

export module infinity_core:logical_compact;

import :logical_node;
import :logical_node_type;
import :column_binding;
import :base_table_ref;

import compact_statement;
import data_type;

namespace infinity {

export class LogicalCompact : public LogicalNode {
public:
    LogicalCompact(u64 node_id, std::shared_ptr<BaseTableRef> base_table_ref, CompactStatementType compact_type)
        : LogicalNode(node_id, LogicalNodeType::kCompact), base_table_ref_(base_table_ref), compact_type_(compact_type) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCompact"; }

public:
    std::shared_ptr<BaseTableRef> base_table_ref_;
    CompactStatementType compact_type_;
};

} // namespace infinity

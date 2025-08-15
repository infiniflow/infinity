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

export module infinity_core:logical_fusion;

import :logical_node_type;
import :column_binding;
import :logical_node;
import :fusion_expression;
import :match_expression;
import :base_table_ref;

import internal_types;
import data_type;

namespace infinity {

export class LogicalFusion : public LogicalNode {
public:
    explicit LogicalFusion(u64 node_id, std::shared_ptr<BaseTableRef> base_table_ref, std::shared_ptr<FusionExpression> fusion_expr);

    std::vector<ColumnBinding> GetColumnBindings() const final { return left_node_->GetColumnBindings(); };

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return left_node_->GetOutputNames(); };

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return left_node_->GetOutputTypes(); };

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalFusion"; }

    std::vector<std::shared_ptr<LogicalNode>> other_children_{};
    std::shared_ptr<BaseTableRef> base_table_ref_{};
    std::shared_ptr<FusionExpression> fusion_expr_{};
};

} // namespace infinity

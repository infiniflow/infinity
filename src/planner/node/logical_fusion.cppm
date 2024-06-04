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

export module logical_fusion;

import stl;
import logical_node_type;
import column_binding;
import logical_node;

import fusion_expression;
import match_expression;
import base_table_ref;

import internal_types;
import data_type;

namespace infinity {

export class LogicalFusion : public LogicalNode {
public:
    explicit LogicalFusion(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<FusionExpression> fusion_expr);

    Vector<ColumnBinding> GetColumnBindings() const final { return left_node_->GetColumnBindings(); };

    SharedPtr<Vector<String>> GetOutputNames() const final { return left_node_->GetOutputNames(); };

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_node_->GetOutputTypes(); };

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalFusion"; }

    Vector<SharedPtr<LogicalNode>> other_children_{};
    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<FusionExpression> fusion_expr_{};
};

} // namespace infinity

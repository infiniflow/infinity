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

import logical_node;
import logical_node_type;
import stl;
import base_expression;
import column_expression;
import reference_expression;
import third_party;
import logger;

module column_remapper;

namespace infinity {

void BindingRemapper::VisitNode(LogicalNode &op) {
    if (op.operator_type() == LogicalNodeType::kJoin) {
        VisitNodeChildren(op);
        bindings_ = op.GetColumnBindings();
        VisitNodeExpression(op);
    } else {
        VisitNodeChildren(op);
        VisitNodeExpression(op);
        bindings_ = op.GetColumnBindings();
    }
}

SharedPtr<BaseExpression> BindingRemapper::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    SizeT binding_count = bindings_.size();
    for (SizeT idx = 0; idx < binding_count; ++idx) {
        if (expression->binding() == bindings_[idx]) {
            return ReferenceExpression::Make(expression->Type(), expression->table_name(), expression->column_name(), expression->alias_, idx);
        }
    }
    LOG_ERROR(Format("Can't bind column expression: {} [{}.{}]",
                     expression->table_name(),
                     expression->binding().table_idx,
                     expression->binding().column_idx));
    return nullptr;
}

} // namespace infinity

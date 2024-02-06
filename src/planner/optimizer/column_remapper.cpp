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
import special_function;

import third_party;
import logger;

module column_remapper;

namespace infinity {

void BindingRemapper::VisitNode(LogicalNode &op) {
    auto load_func = [&]() {
        auto load_metas = op.load_metas();

        if (load_metas.get() != nullptr) {
            for (SizeT i = 0; i < load_metas->size(); ++i) {
                auto &load_meta = (*load_metas)[i];
                // fix index_ value (will be used in PhysicalOperator::InputLoad), now always append to the end
                load_meta.index_ = bindings_.size();
                bindings_.push_back(load_meta.binding_);
            }
        }
    };

    if (op.operator_type() == LogicalNodeType::kJoin or op.operator_type() == LogicalNodeType::kKnnScan) {
        VisitNodeChildren(op);
        bindings_ = op.GetColumnBindings();
        output_types_ = op.GetOutputTypes();
        load_func();
        VisitNodeExpression(op);
    } else {
        VisitNodeChildren(op);
        load_func();
        VisitNodeExpression(op);
        bindings_ = op.GetColumnBindings();
        output_types_ = op.GetOutputTypes();
    }
}

SharedPtr<BaseExpression> BindingRemapper::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    auto special = expression->special();
    if (special.has_value()) {
        switch (special.value()) {
            case SpecialType::kRowID: {
                return ReferenceExpression::Make(expression->Type(),
                                                 expression->table_name(),
                                                 expression->column_name(),
                                                 expression->alias_,
                                                 output_types_->size() - 1);
            }
            case SpecialType::kScore:
            case SpecialType::kDistance: {
                return ReferenceExpression::Make(expression->Type(),
                                                 expression->table_name(),
                                                 expression->column_name(),
                                                 expression->alias_,
                                                 output_types_->size() - 2);
            }
            default: {
                LOG_ERROR(fmt::format("Unknown special function: {}", expression->Name()));
            }
        }
    }

    SizeT binding_count = bindings_.size();
    for (SizeT idx = 0; idx < binding_count; ++idx) {
        if (expression->binding() == bindings_[idx]) {
            return ReferenceExpression::Make(expression->Type(), expression->table_name(), expression->column_name(), expression->alias_, idx);
        }
    }
    LOG_ERROR(fmt::format("Can't bind column expression: {} [{}.{}]",
                          expression->table_name(),
                          expression->binding().table_idx,
                          expression->binding().column_idx));
    return nullptr;
}

} // namespace infinity

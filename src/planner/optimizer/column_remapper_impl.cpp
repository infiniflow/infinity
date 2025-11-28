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

module infinity_core:column_remapper.impl;

import :column_remapper;
import :logical_node;
import :logical_node_type;
import :base_expression;
import :column_expression;
import :reference_expression;
import :special_function;
import :default_values;
import :infinity_exception;

import third_party;

namespace infinity {

void BindingRemapper::VisitNode(LogicalNode &op) {
    auto load_func = [&]() {
        auto load_metas = op.load_metas();
        column_cnt_ = output_types_ ? output_types_->size() : 0;

        if (load_metas.get() != nullptr) {
            column_cnt_ += load_metas->size();
            for (size_t i = 0; i < load_metas->size(); ++i) {
                auto &load_meta = (*load_metas)[i];
                // fix index_ value (will be used in PhysicalOperator::InputLoad), now always append to the end
                load_meta.index_ = bindings_.size();
                bindings_.push_back(load_meta.binding_);
            }
        }
    };

    switch (op.operator_type()) {
        case LogicalNodeType::kInsert:
        case LogicalNodeType::kImport:
        case LogicalNodeType::kExport:
        case LogicalNodeType::kCreateTable:
        case LogicalNodeType::kDropTable:
        case LogicalNodeType::kDropIndex:
        case LogicalNodeType::kCreateSchema:
        case LogicalNodeType::kDropSchema:
        case LogicalNodeType::kShow:
        case LogicalNodeType::kCommand:
        case LogicalNodeType::kPrepare: {
            // skip
            return;
        }
        case LogicalNodeType::kJoin:
        case LogicalNodeType::kMatch:
        case LogicalNodeType::kMatchSparseScan:
        case LogicalNodeType::kMatchTensorScan:
        case LogicalNodeType::kKnnScan: {
            VisitNodeChildren(op);
            bindings_ = op.GetColumnBindings();
            output_types_ = op.GetOutputTypes();
            load_func();
            VisitNodeExpression(op);
            break;
        }
        case LogicalNodeType::kTableScan:
        case LogicalNodeType::kIndexScan: {
            VisitNodeChildren(op);
            bindings_ = op.GetColumnBindings();
            output_types_ = op.GetOutputTypes();

            // Special columns are in output_types_ but not in bindings_
            // Calculate this BEFORE processing load_metas
            const size_t special_column_count = output_types_->size() - bindings_.size();

            auto load_metas = op.load_metas();
            if (load_metas.get() != nullptr) {
                // Filter out load_metas that are already in bindings_
                // This prevents columns that are already in the scan output from being "loaded" again
                std::vector<LoadMeta> filtered_metas;
                for (size_t i = 0; i < load_metas->size(); ++i) {
                    auto &load_meta = (*load_metas)[i];
                    auto it = std::find(bindings_.begin(), bindings_.end(), load_meta.binding_);
                    if (it == bindings_.end()) {
                        // Not in bindings_, need to load
                        load_meta.index_ = bindings_.size();
                        bindings_.push_back(load_meta.binding_);
                        filtered_metas.push_back(load_meta);
                    }
                    // If already in bindings_, skip it (don't add to filtered_metas)
                }
                // Update the operator's load_metas to only include columns that actually need loading
                if (filtered_metas.empty()) {
                    op.set_load_metas(nullptr);
                } else {
                    op.set_load_metas(std::make_shared<std::vector<LoadMeta>>(std::move(filtered_metas)));
                }
            }
            // column_cnt_ = all regular columns (initial + new loaded) + special columns
            column_cnt_ = bindings_.size() + special_column_count;
            VisitNodeExpression(op);
            break;
        }
        default: {
            VisitNodeChildren(op);
            load_func();
            VisitNodeExpression(op);
            bindings_ = op.GetColumnBindings();
            output_types_ = op.GetOutputTypes();
            break;
        }
    }
}

std::shared_ptr<BaseExpression> BindingRemapper::VisitReplace(const std::shared_ptr<ColumnExpression> &expression) {
    auto special = expression->special();
    if (special.has_value()) {
        switch (special.value()) {
            case SpecialType::kRowID: {
                return ReferenceExpression::Make(expression->Type(),
                                                 expression->table_name(),
                                                 expression->column_name(),
                                                 expression->alias_,
                                                 column_cnt_ - 1);
            }
            case SpecialType::kScore:
            case SpecialType::kSimilarity:
            case SpecialType::kDistance: {
                return ReferenceExpression::Make(expression->Type(),
                                                 expression->table_name(),
                                                 expression->column_name(),
                                                 expression->alias_,
                                                 column_cnt_ - 2);
            }
            case SpecialType::kScoreFactors:
            case SpecialType::kSimilarityFactors:
            case SpecialType::kDistanceFactors: {
                return ReferenceExpression::Make(expression->Type(),
                                                 expression->table_name(),
                                                 expression->column_name(),
                                                 expression->alias_,
                                                 column_cnt_ - 3);
            }
            case SpecialType::kCreateTs:
            case SpecialType::kDeleteTs: {
                break;
            }
            default: {
                LOG_ERROR(fmt::format("Unknown special function: {}", expression->Name()));
            }
        }
    }

    size_t binding_count = bindings_.size();
    for (size_t idx = 0; idx < binding_count; ++idx) {
        if (expression->binding() == bindings_[idx]) {
            return ReferenceExpression::Make(expression->Type(), expression->table_name(), expression->column_name(), expression->alias_, idx);
        }
    }
    UnrecoverableError(fmt::format("Can't bind column expression: {} [{}.{}]",
                                   expression->table_name(),
                                   expression->binding().table_idx,
                                   expression->binding().column_idx));
    return nullptr;
}

} // namespace infinity

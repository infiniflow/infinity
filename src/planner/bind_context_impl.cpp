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

module infinity_core:bind_context.impl;

import :bind_context;
import :binding;
import :status;
import :infinity_exception;
import :base_expression;
import :column_expression;
import :column_identifier;
import :block_index;
import :meta_info;

import std;
import third_party;

import parsed_expr;
import search_expr;
import data_type;
import knn_expr;
import match_sparse_expr;
import global_resource_usage;
import column_expr;

namespace infinity {

BindContext::~BindContext() {
    Destroy();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("BindContext");
#endif
}

void BindContext::Destroy() {
    // TODO: Bind context need to release the resource carefully.

    // Release raw pointer
    size_t expr_count = select_expression_.size();
    for (size_t idx = 0; idx < expr_count; ++idx) {
        auto &expr = select_expression_[idx];
        if (expr->type_ == ParsedExprType::kColumn) {
            ColumnExpr *col_expr = (ColumnExpr *)expr;
            if (col_expr->generated_) {
                delete expr;
            }
        }
    }
}

std::shared_ptr<CommonTableExpressionInfo> BindContext::GetCTE(const std::string &name) const {
    if (auto entry = CTE_map_.find(name); entry != CTE_map_.end()) {
        std::shared_ptr<CommonTableExpressionInfo> matched_cte_info = entry->second;
        if (matched_cte_info->masked_name_set_.contains(name)) {
            // name is visible in this scope.
            return matched_cte_info;
        }
    }

    if (parent_) {
        return parent_->GetCTE(name);
    }
    return nullptr;
}

bool BindContext::IsCTEBound(const std::shared_ptr<CommonTableExpressionInfo> &cte) const {

    if (bound_cte_set_.contains(cte)) {
        return true;
    }

    if (parent_) {
        return parent_->IsCTEBound(cte);
    }

    return false;
}

bool BindContext::IsViewBound(const std::string &view_name) const {

    if (bound_view_set_.contains(view_name)) {
        return true;
    }

    if (parent_) {
        return parent_->IsViewBound(view_name);
    }

    return false;
}

bool BindContext::IsTableBound(const std::string &table_name) const {

    if (bound_table_set_.contains(table_name)) {
        return true;
    }

    if (parent_) {
        return parent_->IsTableBound(table_name);
    }

    return false;
}

u64 BindContext::GetNewLogicalNodeId() { return parent_ ? parent_->GetNewLogicalNodeId() : next_logical_node_id_++; }

u64 BindContext::GenerateBindingContextIndex() { return parent_ ? parent_->GenerateBindingContextIndex() : next_bind_context_index_++; }

u64 BindContext::GenerateTableIndex() {
    knn_table_index_ = parent_ ? parent_->GenerateTableIndex() : next_table_index_++;
    return knn_table_index_;
}

void BindContext::AddBinding(const std::shared_ptr<Binding> &binding) {
    binding_by_name_.emplace(binding->table_name_, binding);
    size_t column_count = binding->column_names_->size();
    for (size_t idx = 0; idx < column_count; ++idx) {
        auto &column_name = binding->column_names_->at(idx);
        auto iter = binding_names_by_column_.find(column_name);
        if (iter == binding_names_by_column_.end()) {
            binding_names_by_column_.emplace(column_name, std::vector<std::string>());
            binding_names_by_column_[column_name].emplace_back(binding->table_name_);
        } else {
            iter->second.emplace_back(binding->table_name_);
        }
    }
}

void BindContext::AddSubqueryBinding(const std::string &name,
                                     u64 table_index,
                                     std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                     std::shared_ptr<std::vector<std::string>> column_names) {
    auto binding = Binding::MakeBinding(BindingType::kSubquery, name, table_index, std::move(column_types), std::move(column_names));
    AddBinding(binding);
    // Consider the subquery as the table
    table_names_.emplace_back(name);
    table_name2table_index_[name] = table_index;
    table_table_index2table_name_[table_index] = name;
}

void BindContext::AddCTEBinding(const std::string &name,
                                u64 table_index,
                                std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                std::shared_ptr<std::vector<std::string>> column_names) {
    auto binding = Binding::MakeBinding(BindingType::kCTE, name, table_index, std::move(column_types), std::move(column_names));
    AddBinding(binding);
    // Consider the CTE as the table
    table_names_.emplace_back(name);
    table_name2table_index_[name] = table_index;
    table_table_index2table_name_[table_index] = name;
}

void BindContext::AddViewBinding(const std::string &name,
                                 u64 table_index,
                                 std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                 std::shared_ptr<std::vector<std::string>> column_names) {
    auto binding = Binding::MakeBinding(BindingType::kView, name, table_index, std::move(column_types), std::move(column_names));
    AddBinding(binding);
}

void BindContext::AddTableBinding(const std::string &table_alias,
                                  u64 table_index,
                                  std::shared_ptr<TableInfo> table_info,
                                  std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                  std::shared_ptr<std::vector<std::string>> column_names,
                                  std::shared_ptr<BlockIndex> block_index) {
    auto binding = Binding::MakeBinding(BindingType::kTable,
                                        table_alias,
                                        table_index,
                                        table_info,
                                        std::move(column_types),
                                        std::move(column_names),
                                        std::move(block_index));
    AddBinding(binding);
    table_names_.emplace_back(table_alias);
    table_name2table_index_[table_alias] = table_index;
    table_table_index2table_name_[table_index] = table_alias;
}

void BindContext::AddLeftChild(const std::shared_ptr<BindContext> &left_child) {
    this->left_child_ = left_child;
    this->AddBindContext(left_child);
}

void BindContext::AddRightChild(const std::shared_ptr<BindContext> &right_child) {
    this->right_child_ = right_child;
    this->AddBindContext(right_child);
}

void BindContext::AddBindContext(const std::shared_ptr<BindContext> &other_ptr) {

    size_t table_name_count = other_ptr->table_names_.size();
    table_names_.reserve(table_names_.size() + table_name_count);

    for (size_t idx = 0; idx < table_name_count; ++idx) {
        const auto &table_name = other_ptr->table_names_[idx];
        table_names_.emplace_back(table_name);
    }

    for (const auto &table_name2index_pair : other_ptr->table_name2table_index_) {
        const std::string &table_name = table_name2index_pair.first;
        if (table_name2table_index_.contains(table_name)) {
            UnrecoverableError(fmt::format("{} was bound before", table_name));
        }
        table_name2table_index_[table_name] = table_name2index_pair.second;
    }

    for (const auto &table_index2name_pair : other_ptr->table_table_index2table_name_) {
        u64 table_index = table_index2name_pair.first;
        if (table_table_index2table_name_.contains(table_index)) {
            UnrecoverableError(fmt::format("Table index: {} is bound before", table_index));
        }
        table_table_index2table_name_[table_index] = table_index2name_pair.second;
    }

    for (auto &name_binding_pair : other_ptr->binding_by_name_) {
        auto &binding_name = name_binding_pair.first;
        if (binding_by_name_.contains(binding_name)) {
            UnrecoverableError(fmt::format("Table: {} was bound before", binding_name));
        }
        this->binding_by_name_.emplace(name_binding_pair);
    }

    for (auto &column_name_binding_pair : other_ptr->binding_names_by_column_) {
        auto &column_name = column_name_binding_pair.first;
        auto &bindings_names = column_name_binding_pair.second;

        if (this->binding_names_by_column_.contains(column_name)) {
            size_t bindings_names_count = bindings_names.size();
            for (size_t idx = 0; idx < bindings_names_count; ++idx) {
                const auto &binding_name = bindings_names[idx];
                this->binding_names_by_column_[column_name].emplace_back(binding_name);
            }
        } else {
            this->binding_names_by_column_.emplace(column_name, bindings_names);
        }
    }

    size_t corr_column_count = other_ptr->correlated_column_exprs_.size();
    for (size_t idx = 0; idx < corr_column_count; ++idx) {
        auto &correlated_column = other_ptr->correlated_column_exprs_[idx];
        if (correlated_column_map_.contains(correlated_column->binding())) {
            continue;
        }
        correlated_column_map_.emplace(correlated_column->binding(), correlated_column_map_.size());
        correlated_column_exprs_.emplace_back(correlated_column);
    }
}

std::shared_ptr<ColumnExpression> BindContext::ResolveColumnId(const ColumnIdentifier &column_identifier, i64 depth) {

    std::shared_ptr<ColumnExpression> bound_column_expr;

    const std::string &column_name_ref = *column_identifier.column_name_ptr_;

    if (column_identifier.table_name_ptr_.get() == nullptr) {
        // Not table name
        // Try to find the column in current bind context;
        if (binding_names_by_column_.contains(column_name_ref)) {
            // We find the table
            // TODO: What will happen, when different tables have the same column name?
            std::vector<std::string> &binding_names = binding_names_by_column_[column_name_ref];
            if (binding_names.size() > 1) {
                Status status = Status::SyntaxError(fmt::format("Ambiguous column table_name: {}", column_identifier.ToString()));
                RecoverableError(status);
            }

            std::string &binding_name = binding_names[0];

            auto binding_iter = binding_by_name_.find(binding_name);
            if (binding_iter == binding_by_name_.end()) {
                // Found the binding, but the binding don't have the column, which should happen.
                Status status = Status::SyntaxError(fmt::format("{} doesn't exist.", column_identifier.ToString()));
                RecoverableError(status);
            }

            const auto &binding = binding_iter->second;
            if (binding->name2index_.contains(column_name_ref)) {
                i64 column_id = binding->name2index_[column_name_ref];
                bound_column_expr = ColumnExpression::Make(*binding->column_types_->at(column_id),
                                                           binding_name,
                                                           binding->table_index_,
                                                           column_name_ref,
                                                           column_id,
                                                           depth);
                bound_column_expr->source_position_ = SourcePosition(binding_context_id_, ExprSourceType::kBinding);
                bound_column_expr->source_position_.binding_name_ = binding->table_name_;
            } else {
                // Found the binding, but the binding don't have the column, which should happen.
                Status status = Status::SyntaxError(fmt::format("{} doesn't exist.", column_identifier.ToString()));
                RecoverableError(status);
            }
        } else {
            // Table isn't found in current bind context, maybe its parent has it.
            bound_column_expr = nullptr;
        }
    } else {
        const std::string &table_name_ref = *column_identifier.table_name_ptr_;
        auto binding_iter = binding_by_name_.find(table_name_ref);
        if (binding_iter != binding_by_name_.end()) {
            const auto &binding = binding_iter->second;
            if (binding->name2index_.contains(column_name_ref)) {
                // Find the table and column in the bind context.
                i64 column_id = binding->name2index_[column_name_ref];
                bound_column_expr = ColumnExpression::Make(*binding->column_types_->at(column_id),
                                                           table_name_ref,
                                                           binding->table_index_,
                                                           column_name_ref,
                                                           column_id,
                                                           depth);
                bound_column_expr->source_position_ = SourcePosition(binding_context_id_, ExprSourceType::kBinding);
                bound_column_expr->source_position_.binding_name_ = binding->table_name_;
            } else {
                Status status = Status::SyntaxError(fmt::format("{} doesn't exist.", column_identifier.ToString()));
                RecoverableError(status);
            }
        } else {
            // Table isn't found in current bind context, maybe its parent has it.
            bound_column_expr = nullptr;
        }
    }

    if (bound_column_expr.get() != nullptr)
        return bound_column_expr;

    if (parent_ != nullptr) {
        return parent_->ResolveColumnId(column_identifier, depth + 1);
    }

    return nullptr;
    //    PlannerError(column_identifier.ToString() + " isn't found.");
}

void BindContext::AddCorrelatedColumnExpr(const std::shared_ptr<ColumnExpression> &correlated_column) {
    if (parent_ != nullptr) {
        parent_->AddCorrelatedColumnExpr(correlated_column);
    }

    // Not only parent bind context need have the correlated column expression but also current bind context
    if (correlated_column_map_.contains(correlated_column->binding())) {
        return;
    }
    correlated_column_map_.emplace(correlated_column->binding(), correlated_column_exprs_.size());
    correlated_column_exprs_.emplace_back(correlated_column);
}

const Binding *BindContext::GetBindingFromCurrentOrParentByName(const std::string &binding_name) const {
    auto binding_iter = binding_by_name_.find(binding_name);
    if (binding_iter == binding_by_name_.end()) {
        if (parent_ != nullptr) {
            return parent_->GetBindingFromCurrentOrParentByName(binding_name);
        }
        return nullptr;
    }
    return binding_iter->second.get();
}

void BindContext::BoundSearch(ParsedExpr *expr) {
    if (expr == nullptr) {
        return;
    }
    auto search_expr = (SearchExpr *)expr;
    bool conflict = false;
    allow_score = !search_expr->fusion_exprs_.empty();
    if (!search_expr->fusion_exprs_.empty())
        return;

    KnnDistanceType first_distance_type = KnnDistanceType::kInvalid;
    SparseMetricType first_metric_type = SparseMetricType::kInvalid;
    for (size_t i = 0; !conflict && i < search_expr->match_exprs_.size(); i++) {
        auto &match_expr = search_expr->match_exprs_[i];
        switch (match_expr->type_) {
            case ParsedExprType::kKnn: {
                auto knn_expr = (KnnExpr *)match_expr;
                if (first_distance_type == KnnDistanceType::kInvalid) {
                    first_distance_type = knn_expr->distance_type_;
                    switch (first_distance_type) {
                        case KnnDistanceType::kL2:
                        case KnnDistanceType::kHamming: {
                            allow_distance = true;
                            break;
                        }
                        case KnnDistanceType::kInnerProduct:
                        case KnnDistanceType::kCosine: {
                            allow_similarity = true;
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid KNN metric type");
                        }
                    }
                } else if (first_distance_type != knn_expr->distance_type_) {
                    conflict = true;
                    allow_distance = false;
                    allow_similarity = false;
                }
                break;
            }
            case ParsedExprType::kMatchSparse: {
                auto match_sparse_expr = (MatchSparseExpr *)match_expr;
                if (first_metric_type == SparseMetricType::kInvalid) {
                    first_metric_type = match_sparse_expr->metric_type_;
                    switch (first_metric_type) {
                        case SparseMetricType::kInnerProduct: {
                            allow_similarity = true;
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid sparse metric type");
                        }
                    }
                } else if (first_metric_type != match_sparse_expr->metric_type_) {
                    conflict = true;
                    allow_distance = false;
                    allow_similarity = false;
                }
                break;
            }
            case ParsedExprType::kMatchTensor:
            case ParsedExprType::kMatch: {
                allow_score = true;
                break;
            }
            default: {
                UnrecoverableError("Invalid match expr type");
            }
        }
    }
}

// void
// BindContext::AddChild(const std::shared_ptr<BindContext>& child) {
//     child->binding_context_id_ = GenerateBindingContextIndex();
//     children_.emplace_back(child);
// }

} // namespace infinity
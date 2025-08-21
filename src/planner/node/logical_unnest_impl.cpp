// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:logical_unnest.impl;

import :logical_unnest;
import :column_binding;
import :logical_node;
import :expression_type;
import :column_expression;
import :unnest_expression;
import :reference_expression;
import :column_expression;

import std;

import data_type;
import internal_types;

namespace infinity {

std::vector<ColumnBinding> LogicalUnnest::GetColumnBindings() const {
    std::vector<ColumnBinding> result = LogicalCommonFunctionUsingLoadMeta::GetColumnBindings(*this);
    size_t unnest_count = expression_list_.size();
    for (size_t i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        result[unnest_ref_expr->column_index()] = ColumnBinding(unnest_idx_, i);
    }
    return result;
}

std::vector<ColumnBinding> LogicalUnnest::RemoveColumnBindings() const {
    size_t unnest_count = expression_list_.size();
    std::vector<ColumnBinding> result;
    for (size_t i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *col_expr = static_cast<ColumnExpression *>(unnest_expr->arguments()[0].get());
        result.push_back(col_expr->binding());
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalUnnest::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = LogicalCommonFunctionUsingLoadMeta::GetOutputNames(*this);
    size_t unnest_count = expression_list_.size();
    for (size_t i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        (*result)[unnest_ref_expr->column_index()] = unnest_expr->Name();
    }
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalUnnest::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = LogicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
    size_t unnest_count = expression_list_.size();
    for (size_t i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        (*result)[unnest_ref_expr->column_index()] = std::make_shared<DataType>(expression_list_[i]->Type());
    }
    return result;
}

std::string LogicalUnnest::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Logical Unnest: ";

    size_t expr_count = expression_list_.size();
    for (size_t idx = 0; idx < expr_count; ++idx) {
        ss << expression_list_[idx]->Name();
        if (idx < expr_count - 1) {
            ss << ", ";
        }
    }
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity

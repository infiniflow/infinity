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

module;

#include <sstream>

module infinity_core:logical_unnest.impl;

import :logical_unnest;

import :stl;
import :column_binding;
import :logical_node;
import internal_types;
import :expression_type;
import :column_expression;
import :unnest_expression;
import :reference_expression;
import :column_expression;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalUnnest::GetColumnBindings() const {
    Vector<ColumnBinding> result = LogicalCommonFunctionUsingLoadMeta::GetColumnBindings(*this);
    SizeT unnest_count = expression_list_.size();
    for (SizeT i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        result[unnest_ref_expr->column_index()] = ColumnBinding(unnest_idx_, i);
    }
    return result;
}

Vector<ColumnBinding> LogicalUnnest::RemoveColumnBindings() const {
    SizeT unnest_count = expression_list_.size();
    Vector<ColumnBinding> result;
    for (SizeT i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *col_expr = static_cast<ColumnExpression *>(unnest_expr->arguments()[0].get());
        result.push_back(col_expr->binding());
    }
    return result;
}

SharedPtr<Vector<String>> LogicalUnnest::GetOutputNames() const {
    SharedPtr<Vector<String>> result = LogicalCommonFunctionUsingLoadMeta::GetOutputNames(*this);
    SizeT unnest_count = expression_list_.size();
    for (SizeT i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        (*result)[unnest_ref_expr->column_index()] = unnest_expr->Name();
    }
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalUnnest::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = LogicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
    SizeT unnest_count = expression_list_.size();
    for (SizeT i = 0; i < unnest_count; ++i) {
        auto *unnest_expr = static_cast<UnnestExpression *>(expression_list_[i].get());
        auto *unnest_ref_expr = static_cast<ReferenceExpression *>(unnest_expr->arguments()[0].get());
        (*result)[unnest_ref_expr->column_index()] = MakeShared<DataType>(expression_list_[i]->Type());
    }
    return result;
}

String LogicalUnnest::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Logical Unnest: ";

    SizeT expr_count = expression_list_.size();
    for (SizeT idx = 0; idx < expr_count; ++idx) {
        ss << expression_list_[idx]->Name();
        if (idx < expr_count - 1) {
            ss << ", ";
        }
    }
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity

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

#include <sstream>

module infinity_core:in_expression.impl;

import :in_expression;

import :infinity_exception;
import :stl;
import :expression_type;
import :logger;

namespace infinity {

InExpression::InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kIn, arguments), left_operand_ptr_(std::move(left_operand)), in_type_(in_type),
      set_(left_operand_ptr_->Type().type()) {}

String InExpression::ToString() const {

    std::stringstream op;

    op << left_operand_ptr_->Name();

    switch (in_type_) {
        case InType::kIn: {
            op << " IN ";
            break;
        }
        case InType::kNotIn: {
            op << " NOT IN ";
            break;
        }
        default: {
            String error_message = "Unknown IN operator type.";
            UnrecoverableError(error_message);
            break;
        }
    }

    op << "(";

    for (auto &argument_ptr : arguments_) {
        op << argument_ptr->Name() << ", ";
    }

    op << ")" << std::endl;

    return op.str();
}

u64 InExpression::Hash() const {
    auto h = left_operand_ptr_->Hash();
    for (const auto &arg : arguments_) {
        h ^= arg->Hash();
    }
    if (in_type_ != InType::kIn) {
        h ^= 0x1;
    }
    return h;
}

bool InExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kIn) {
        return false;
    }
    const auto &other = static_cast<const InExpression &>(other_base);
    if (in_type_ != other.in_type_) {
        return false;
    }
    if (!left_operand_ptr_->Eq(*other.left_operand_ptr_)) {
        return false;
    }
    if (arguments_.size() != other.arguments_.size()) {
        return false;
    }
    for (SizeT i = 0; i < arguments_.size(); ++i) {
        if (!arguments_[i]->Eq(*other.arguments_[i])) {
            return false;
        }
    }
    return true;
}

} // namespace infinity

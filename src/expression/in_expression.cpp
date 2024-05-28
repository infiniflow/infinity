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

import infinity_exception;
import stl;
import expression_type;
import logger;

module in_expression;

namespace infinity {

InExpression::InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list)
    : BaseExpression(ExpressionType::kIn, value_list), left_operand_ptr_(std::move(left_operand)), in_type_(in_type) {}

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
            LOG_CRITICAL(error_message);
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

} // namespace infinity

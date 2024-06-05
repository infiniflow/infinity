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

#include <vector>

module expression_transformer;

import stl;
import base_expression;
import aggregate_expression;
import between_expression;
import cast_expression;
import case_expression;
import function_expression;
import in_expression;
import conjunction_expression;
import expression_type;

import infinity_exception;
import third_party;
import logger;

namespace infinity {

Vector<SharedPtr<BaseExpression>> SplitExpressionByDelimiter(const SharedPtr<BaseExpression> &expression, ConjunctionType delimiter) {
    Vector<SharedPtr<BaseExpression>> result;

    std::function<VisitControlType(SharedPtr<BaseExpression> & child)> func = [&](const SharedPtr<BaseExpression> &expr_ptr) -> VisitControlType {
        if (expr_ptr->type() == ExpressionType::kConjunction) {
            const auto conjunction_expr_ptr = std::static_pointer_cast<ConjunctionExpression>(expr_ptr);
            if (conjunction_expr_ptr->conjunction_type() == delimiter)
                return VisitControlType::kVisit;
        }
        result.emplace_back(expr_ptr);
        return VisitControlType::kNotVisit;
    };

    VisitExpression(expression, func);

    return result;
}

SharedPtr<BaseExpression> ComposeExpressionWithDelimiter(const Vector<SharedPtr<BaseExpression>> &expressions, ConjunctionType conjunction_type) {
    auto expr_count = expressions.size();
    if (expr_count == 0) {
        return nullptr;
    }
    SharedPtr<BaseExpression> result = expressions[0];
    for (SizeT i = 1; i < expr_count; ++i) {
        result = MakeShared<ConjunctionExpression>(conjunction_type, result, expressions[i]);
    }
    return result;
}

void VisitExpression(const SharedPtr<BaseExpression> &expression, const std::function<VisitControlType(SharedPtr<BaseExpression> &child)> &visitor) {
    Queue<SharedPtr<BaseExpression>> queue;
    queue.push(expression);

    while (!queue.empty()) {
        auto expr = queue.front();
        queue.pop();

        if (visitor(expr) == VisitControlType::kVisit) {
            for (auto &argument : expr->arguments()) {
                queue.push(argument);
            }
        }
    }
}

void VisitExpression(const SharedPtr<BaseExpression> &expression, const std::function<void(SharedPtr<BaseExpression> &child)> &visitor) {
    switch (expression->type()) {
        case ExpressionType::kAggregate: {
            AggregateExpression *agg_expr = (AggregateExpression *)expression.get();
            for (auto &argument : agg_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kBetween: {
            BetweenExpression *between_expr = (BetweenExpression *)expression.get();
            for (auto &argument : between_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kCast: {
            CastExpression *cast_expr = (CastExpression *)expression.get();
            for (auto &argument : cast_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kCase: {
            CaseExpression *case_expr = (CaseExpression *)expression.get();
            for (auto &argument : case_expr->arguments()) {
                visitor(argument);
            }
            for (auto &case_when : case_expr->CaseExpr()) {
                visitor(case_when.when_expr_);
                visitor(case_when.then_expr_);
            }
            visitor(case_expr->ElseExpr());
            break;
        }

        case ExpressionType::kConjunction: {
            ConjunctionExpression *conjunction_expr = (ConjunctionExpression *)expression.get();
            for (auto &argument : conjunction_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kFunction: {
            FunctionExpression *function_expr = (FunctionExpression *)expression.get();
            for (auto &argument : function_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kIn: {
            InExpression *function_expr = (InExpression *)expression.get();
            visitor(function_expr->left_operand());

            for (auto &argument : function_expr->arguments()) {
                visitor(argument);
            }
            break;
        }
        case ExpressionType::kSubQuery:
            break;
        case ExpressionType::kColumn:
        case ExpressionType::kValue:
            break;
        default: {
            String error_message = fmt::format("Unsupported expression type: {}", expression->Name());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

} // namespace infinity
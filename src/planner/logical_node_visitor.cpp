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

module logical_node_visitor;

import stl;
import base_expression;
import logical_node_type;
import infinity_exception;

import third_party;
import expression_type;
import logical_node_type;
import logical_node;
import logical_aggregate;
import logical_join;
import logical_limit;
import logical_filter;
import logical_project;
import logical_sort;
import logical_top;
import logical_insert;
import logical_update;
import logical_knn_scan;
import logical_index_scan;
import logical_match;
import logical_tensor_maxsim_scan;
import aggregate_expression;
import between_expression;
import case_expression;
import cast_expression;
import column_expression;
import function_expression;
import value_expression;
import in_expression;
import subquery_expression;
import knn_expression;
import conjunction_expression;
import logger;

namespace infinity {

void LogicalNodeVisitor::VisitNodeChildren(LogicalNode &op) {
    if (op.left_node()) {
        VisitNode(*op.left_node());
    }
    if (op.right_node()) {
        VisitNode(*op.right_node());
    }
}

void LogicalNodeVisitor::VisitNodeExpression(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kAggregate: {
            auto &node = (LogicalAggregate &)op;
            for (auto &expression : node.groups_) {
                VisitExpression(expression);
            }
            for (auto &expression : node.aggregates_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kJoin: {
            auto &node = (LogicalJoin &)op;
            for (auto &expression : node.conditions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kLimit: {
            auto &node = (LogicalLimit &)op;
            if (node.limit_expression_.get() != nullptr) {
                VisitExpression(node.limit_expression_);
            }
            if (node.offset_expression_.get() != nullptr) {
                VisitExpression(node.offset_expression_);
            }
            break;
        }
        case LogicalNodeType::kFilter: {
            auto &node = (LogicalFilter &)op;
            VisitExpression(node.expression());
            break;
        }
        case LogicalNodeType::kProjection: {
            auto &node = (LogicalProject &)op;
            for (auto &expression : node.expressions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kSort: {
            auto &node = (LogicalSort &)op;
            for (auto &expression : node.expressions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kTop: {
            auto &node = (LogicalTop &)op;
            for (auto &expression : node.sort_expressions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kInsert: {
            auto &node = (LogicalInsert &)op;
            for (auto &value : node.value_list())
                for (auto &expression : value) {
                    VisitExpression(expression);
                }
            break;
        }
        case LogicalNodeType::kUpdate: {
            auto &node = (LogicalUpdate &)op;
            for (auto &update_column_pair : node.update_columns_) {
                SharedPtr<BaseExpression> &expression = update_column_pair.second;
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kKnnScan: {
            auto &node = (LogicalKnnScan &)op;
            if (node.common_query_filter_ and node.common_query_filter_->filter_leftover_) {
                VisitExpression(node.common_query_filter_->filter_leftover_);
            }
            break;
        }
        case LogicalNodeType::kMatch: {
            auto &node = (LogicalMatch &)op;
            if (node.common_query_filter_ and node.common_query_filter_->filter_leftover_) {
                VisitExpression(node.common_query_filter_->filter_leftover_);
            }
            break;
        }
        case LogicalNodeType::kTensorMaxSimScan: {
            auto &node = (LogicalTensorMaxSimScan &)op;
            if (node.common_query_filter_ and node.common_query_filter_->filter_leftover_) {
                VisitExpression(node.common_query_filter_->filter_leftover_);
            }
            break;
        }
        case LogicalNodeType::kIndexScan: {
            // always keep the original expression
            break;
        }
        default: {
//            LOG_TRACE(fmt::format("Visit logical node: {}", op.name()));
        }
    }
}

void LogicalNodeVisitor::VisitExpression(SharedPtr<BaseExpression> &expression) {
    SharedPtr<BaseExpression> result;
    switch (expression->type()) {

        case ExpressionType::kAggregate: {
            auto aggregate_expression = static_pointer_cast<AggregateExpression>(expression);
            for (auto &argument : aggregate_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(aggregate_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kCast: {
            auto cast_expression = static_pointer_cast<CastExpression>(expression);
            for (auto &argument : cast_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(cast_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kCase: {
            auto case_expression = static_pointer_cast<CaseExpression>(expression);
            if (!case_expression->arguments().empty()) {
                UnrecoverableError("Case expression shouldn't have arguments");
            }
            for (auto &case_expr : case_expression->CaseExpr()) {
                VisitExpression(case_expr.then_expr_);
                VisitExpression(case_expr.when_expr_);
            }

            VisitExpression(case_expression->ElseExpr());

            result = VisitReplace(case_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kConjunction: {
            auto conjunction_expression = static_pointer_cast<ConjunctionExpression>(expression);
            for (auto &argument : conjunction_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(conjunction_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kColumn: {
            auto column_expression = static_pointer_cast<ColumnExpression>(expression);
            if (!column_expression->arguments().empty()) {
                UnrecoverableError("Column expression shouldn't have arguments");
            }

            result = VisitReplace(column_expression);
            if (result.get() == nullptr) {
                UnrecoverableError("Visit column expression will always rewrite the expression");
            }
            expression = result;
            break;
        }
        case ExpressionType::kFunction: {
            auto function_expression = static_pointer_cast<FunctionExpression>(expression);
            for (auto &argument : function_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(function_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kValue: {
            auto value_expression = static_pointer_cast<ValueExpression>(expression);

            if (!value_expression->arguments().empty()) {
                UnrecoverableError("Column expression shouldn't have arguments");
            }

            result = VisitReplace(value_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kIn: {
            auto in_expression = static_pointer_cast<InExpression>(expression);

            VisitExpression(in_expression->left_operand());
            for (auto &argument : in_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(in_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kSubQuery: {
            auto subquery_expression = static_pointer_cast<SubqueryExpression>(expression);

            result = VisitReplace(subquery_expression);
            if (result.get() != nullptr) {
                UnrecoverableError("Visit subquery expression will always rewrite the expression");
            }
            break;
        }
        case ExpressionType::kKnn: {
            auto knn_expression = static_pointer_cast<KnnExpression>(expression);
            VisitExpression(knn_expression->arguments()[0]);

            result = VisitReplace(knn_expression);
            if (result.get() != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kReference: {
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unexpected expression type: {}", expression->Name()));
        }
    }
}

void LogicalNodeVisitor::VisitExpressionChildren(SharedPtr<BaseExpression> &expression) {
    switch (expression->type()) {

        case ExpressionType::kAggregate: {
            auto aggregate_expression = (AggregateExpression *)(expression.get());
            for (auto &argument : aggregate_expression->arguments()) {
                VisitExpression(argument);
            }
            break;
        }
        case ExpressionType::kGroupingFunction:
            break;
        case ExpressionType::kArithmetic:
            break;
        case ExpressionType::kCast: {
            auto cast_expression = (CastExpression *)(expression.get());
            for (auto &argument : cast_expression->arguments()) {
                VisitExpression(argument);
            }
            break;
        }
        case ExpressionType::kCase:
            break;
        case ExpressionType::kAnd:
            break;
        case ExpressionType::kConjunction:
            break;
        case ExpressionType::kOr:
            break;
        case ExpressionType::kNot:
            break;
        case ExpressionType::kColumn:
            break;
        case ExpressionType::kCorrelatedColumn:
            break;
        case ExpressionType::kExists:
            break;
        case ExpressionType::kExtract:
            break;
        case ExpressionType::kInterval:
            break;
        case ExpressionType::kFunction:
            break;
        case ExpressionType::kList:
            break;
        case ExpressionType::kLogical:
            break;
        case ExpressionType::kEqual:
            break;
        case ExpressionType::kNotEqual:
            break;
        case ExpressionType::kLessThan:
            break;
        case ExpressionType::kGreaterThan:
            break;
        case ExpressionType::kLessThanEqual:
            break;
        case ExpressionType::kGreaterThanEqual:
            break;
        case ExpressionType::kBetween:
            break;
        case ExpressionType::kNotBetween:
            break;
        case ExpressionType::kSubQuery:
            break;
        case ExpressionType::kUnaryMinus:
            break;
        case ExpressionType::kIsNull:
            break;
        case ExpressionType::kIsNotNull:
            break;
        case ExpressionType::kValue:
            break;
        case ExpressionType::kDefault:
            break;
        case ExpressionType::kParameter:
            break;
        case ExpressionType::kIn:
            break;
        case ExpressionType::kNotIn:
            break;
        case ExpressionType::kWindowRank:
            break;
        case ExpressionType::kWindowRowNumber:
            break;
        case ExpressionType::kDistinctFrom:
            break;
        case ExpressionType::kNotDistinctFrom:
            break;
        case ExpressionType::kPlaceholder:
            break;
        case ExpressionType::kPredicate:
            break;
        case ExpressionType::kRaw:
            break;
        default: {
            break;
        }
    }
}

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<AggregateExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<BetweenExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<CaseExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<CastExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<ColumnExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<ConjunctionExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<FunctionExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<ValueExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<InExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<SubqueryExpression> &) { return nullptr; }

SharedPtr<BaseExpression> LogicalNodeVisitor::VisitReplace(const SharedPtr<KnnExpression> &) { return nullptr; }

} // namespace infinity

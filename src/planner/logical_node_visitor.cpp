//
// Created by jinhai on 23-2-16.
//

#include "logical_node_visitor.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_sort.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"
#include "planner/node/logical_aggregate.h"
#include "planner/node/logical_insert.h"

namespace infinity {

void
LogicalNodeVisitor::VisitNodeChildren(LogicalNode &op) {
    if(op.left_node()) {
        VisitNode(*op.left_node());
    }
    if(op.right_node()) {
        VisitNode(*op.right_node());
    }
}

void
LogicalNodeVisitor::VisitNodeExpression(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kAggregate: {
            auto& node = (LogicalAggregate&)op;
            for(auto& expression: node.groups_) {
                VisitExpression(expression);
            }
            for(auto& expression: node.aggregates_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kJoin: {
            auto& node = (LogicalJoin&)op;
            for(auto& expression: node.conditions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kLimit: {
            auto& node = (LogicalLimit&)op;
            if(node.limit_expression_ != nullptr) {
                VisitExpression(node.limit_expression_);
            }
            if(node.offset_expression_ != nullptr) {
                VisitExpression(node.offset_expression_);
            }
            break;
        }
        case LogicalNodeType::kFilter: {
            auto& node = (LogicalFilter&)op;
            VisitExpression(node.expression());
            break;
        }
        case LogicalNodeType::kProjection: {
            auto& node = (LogicalProject&)op;
            for(auto& expression: node.expressions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kSort: {
            auto& node = (LogicalSort&)op;
            for(auto& expression: node.expressions_) {
                VisitExpression(expression);
            }
            break;
        }
        case LogicalNodeType::kInsert: {
            auto& node = (LogicalInsert&)op;
            for(auto& expression: node.value_list()) {
                VisitExpression(expression);
            }
            break;
        }
        default: {
            LOG_TRACE("Visit logical node: {}", op.name());
        }
    }

}

void
LogicalNodeVisitor::VisitExpression(SharedPtr<BaseExpression>& expression) {

    SharedPtr<BaseExpression> result;
    switch (expression->type()) {

        case ExpressionType::kAggregate: {
            auto aggregate_expression = std::static_pointer_cast<AggregateExpression>(expression);
            for(auto& argument: aggregate_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(aggregate_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kCast: {
            auto cast_expression = std::static_pointer_cast<CastExpression>(expression);
            for(auto& argument: cast_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(cast_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kCase: {
            auto case_expression = std::static_pointer_cast<CaseExpression>(expression);
            PlannerAssert(case_expression->arguments().empty(), "Case expression shouldn't have arguments");
            for(auto& case_expr: case_expression->CaseExpr()) {
                VisitExpression(case_expr.then_expr_);
                VisitExpression(case_expr.when_expr_);
            }

            VisitExpression(case_expression->ElseExpr());

            result = VisitReplace(case_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kConjunction: {
            auto conjunction_expression = std::static_pointer_cast<ConjunctionExpression>(expression);
            for(auto& argument: conjunction_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(conjunction_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kColumn: {
            auto column_expression = std::static_pointer_cast<ColumnExpression>(expression);
            PlannerAssert(column_expression->arguments().empty(), "Column expression shouldn't have arguments");

            result = VisitReplace(column_expression);
            if(result == nullptr) {
                PlannerError("Visit column expression will always rewrite the expression");
            }
            expression = result;
            break;
        }
        case ExpressionType::kFunction: {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            for(auto& argument: function_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(function_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kValue: {
            auto value_expression = std::static_pointer_cast<ValueExpression>(expression);
            PlannerAssert(value_expression->arguments().empty(), "Column expression shouldn't have arguments");

            result = VisitReplace(value_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kIn: {
            auto in_expression = std::static_pointer_cast<InExpression>(expression);

            VisitExpression(in_expression->left_operand());
            for(auto& argument: in_expression->arguments()) {
                VisitExpression(argument);
            }

            result = VisitReplace(in_expression);
            if(result != nullptr) {
                expression = result;
            }
            break;
        }
        case ExpressionType::kSubQuery: {
            auto subquery_expression = std::static_pointer_cast<SubqueryExpression>(expression);
            result = VisitReplace(subquery_expression);
            if(result != nullptr) {
                PlannerError("Visit subquery expression will always rewrite the expression");
            }
            break;
        }
        default: {
            PlannerError("Unexpected expression type: " + expression->Name())
        }
    }
}

void
LogicalNodeVisitor::VisitExpressionChildren(SharedPtr<BaseExpression>& expression) {
    switch (expression->type()) {

        case ExpressionType::kAggregate: {
            auto aggregate_expression = std::static_pointer_cast<AggregateExpression>(expression);
            for(auto& argument: aggregate_expression->arguments()) {
                VisitExpression(argument);
            }
            break;
        }
        case ExpressionType::kGroupingFunction:
            break;
        case ExpressionType::kArithmetic:
            break;
        case ExpressionType::kCast: {
            auto cast_expression = std::static_pointer_cast<CastExpression>(expression);
            for (auto &argument: cast_expression->arguments()) {
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

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<AggregateExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<BetweenExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<CaseExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<CastExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<ColumnExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<ConjunctionExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<FunctionExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<ValueExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<InExpression>& expression) {
    return nullptr;
}

SharedPtr<BaseExpression>
LogicalNodeVisitor::VisitReplace(const SharedPtr<SubqueryExpression>& expression) {
    return nullptr;
}

}

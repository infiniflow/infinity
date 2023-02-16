//
// Created by jinhai on 23-2-16.
//

#include "logical_node_visitor.h"
#include "planner/node/logical_filter.h"

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
        case LogicalNodeType::kInvalid:
            break;
        case LogicalNodeType::kAggregate:
            break;
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion:
            break;
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin:
            break;
        case LogicalNodeType::kCrossProduct:
            break;
        case LogicalNodeType::kLimit:
            break;
        case LogicalNodeType::kFilter: {
            auto& filter = (LogicalFilter&)op;
            VisitExpression(filter.expression());
            break;
        }
        case LogicalNodeType::kProjection:
            break;
        case LogicalNodeType::kSort:
            break;
        case LogicalNodeType::kDelete:
            break;
        case LogicalNodeType::kUpdate:
            break;
        case LogicalNodeType::kInsert:
            break;
        case LogicalNodeType::kImport:
            break;
        case LogicalNodeType::kExport:
            break;
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable:
            break;
        case LogicalNodeType::kCreateView:
            break;
        case LogicalNodeType::kDropTable:
            break;
        case LogicalNodeType::kDropView:
            break;
        case LogicalNodeType::kChunkScan:
            break;
        case LogicalNodeType::kTableScan:
            break;
        case LogicalNodeType::kViewScan:
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kAlias:
            break;
        case LogicalNodeType::kMock:
            break;
        case LogicalNodeType::kValidate:
            break;
        case LogicalNodeType::kStoredTable:
            break;
        case LogicalNodeType::kStaticTable:
            break;
        case LogicalNodeType::kRoot:
            break;
        case LogicalNodeType::kPrepare:
            break;
    }

}

void
LogicalNodeVisitor::VisitExpression(SharedPtr<BaseExpression>& expression) {

    switch (expression->type()) {

        case ExpressionType::kAggregate:
            break;
        case ExpressionType::kGroupingFunction:
            break;
        case ExpressionType::kArithmetic:
            break;
        case ExpressionType::kCast:
            break;
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
    }
}

void
LogicalNodeVisitor::VisitExpressionChildren(SharedPtr<BaseExpression>& expression) {

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

}
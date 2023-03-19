//
// Created by JinHai on 2022/8/11.
//

#include "expression/between_expression.h"
#include "expression/in_expression.h"
#include "expression/cast_expression.h"
#include "expression/case_expression.h"
#include "expression/value_expression.h"
#include "expression/function_expression.h"
#include "expression/aggregate_expression.h"
#include "function/scalar_function_set.h"
#include "function/aggregate_function_set.h"

#include "common/types/info/varchar_info.h"

#include "common/utility/infinity_assert.h"
#include "main/infinity.h"
#include "expression_binder.h"
#include "query_binder.h"

namespace infinity {

SharedPtr<BaseExpression>
ExpressionBinder::Bind(const ParsedExpr& expr,
                       BindContext* bind_context_ptr,
                       i64 depth,
                       bool root) {
    // Call implemented BuildExpression
    SharedPtr<BaseExpression> result = BuildExpression(expr,
                                                       bind_context_ptr,
                                                       depth,
                                                       root);
    if(result == nullptr) {
        PlannerAssert(result != nullptr, "Fail to bind the expression: " + expr.GetName());

        // Maybe the correlated expression, trying to bind it in the parent context.
        result = Bind(expr, bind_context_ptr->parent_, depth + 1, root);
    }
    return result;
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildExpression(const ParsedExpr& expr,
                                  BindContext* bind_context_ptr,
                                  i64 depth,
                                  bool root) {
    switch (expr.type_) {
        case ParsedExprType::kConstant: {
            return BuildValueExpr((const ConstantExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kColumn: {
            return BuildColExpr((const ColumnExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kFunction: {
            return BuildFuncExpr((const FunctionExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kSubquery: {
            // subquery expression
            return BuildSubquery((const SubqueryExpr&)expr, bind_context_ptr, SubqueryType::kScalar, depth, root);
        }
        case ParsedExprType::kBetween: {
            return BuildBetweenExpr((const BetweenExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCase: {
            return BuildCaseExpr((const CaseExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCast: {
            // cast function expression
            return BuildCastExpr((const CastExpr&)expr, bind_context_ptr, depth, root);
        }
        default: {
            PlannerError("Unexpected expression type.");
        }
    }

    return SharedPtr<BaseExpression>();
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildBetweenExpr(const BetweenExpr& expr,
                                   BindContext* bind_context_ptr,
                                   i64 depth,
                                   bool root) {
    auto value_ptr = BuildExpression(*expr.value_, bind_context_ptr, depth, false);
    auto lower_ptr = BuildExpression(*expr.lower_bound_, bind_context_ptr, depth, false);
    auto upper_ptr = BuildExpression(*expr.upper_bound_, bind_context_ptr, depth, false);

    auto &catalog = Infinity::instance().catalog();
    SharedPtr<FunctionExpression> left_function_expr{nullptr}, right_function_expr{nullptr};

    {
        String left_func = ">";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(lower_ptr);
        SharedPtr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(left_func);
        CheckFuncType(function_set_ptr->type_);
        auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
        ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
        left_function_expr = MakeShared<FunctionExpression>(scalar_function, arguments);
    }

    {
        String left_func = "<";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(upper_ptr);
        SharedPtr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(left_func);
        CheckFuncType(function_set_ptr->type_);
        auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
        ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
        right_function_expr = MakeShared<FunctionExpression>(scalar_function, arguments);
    }

    String and_func = "and";
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(2);
    arguments.emplace_back(left_function_expr);
    arguments.emplace_back(right_function_expr);

    SharedPtr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(and_func);
    CheckFuncType(function_set_ptr->type_);
    auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
    ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
    return MakeShared<FunctionExpression>(scalar_function, arguments);
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildValueExpr(const ConstantExpr& expr,
                                 BindContext* bind_context_ptr,
                                 i64 depth,
                                 bool root) {
    switch(expr.literal_type_) {
        case LiteralType::kInteger: {
            Value value = Value::MakeBigInt(expr.integer_value_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kString: {
            auto data_type = DataType(LogicalType::kVarchar);
            i64 varchar_len = static_cast<i64>(std::strlen(expr.str_value_));
            auto type_info_ptr = VarcharInfo::Make(varchar_len);
            Value value = Value::MakeVarchar(expr.str_value_, type_info_ptr);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kDouble: {
            Value value = Value::MakeDouble(expr.double_value_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kDate: {
            SizeT date_str_len = std::strlen(expr.date_value_);
            DateT date_value;
            date_value.FromString(expr.date_value_, date_str_len);
            Value value = Value::MakeDate(date_value);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kInterval: {
            // IntervalT should be a struct including the type of the value and an value of the interval
            // It will be bound into a ValueExpression here.
            IntervalT interval_value(expr.integer_value_);
            if(expr.interval_type_ == TimeUnit::kInvalidUnit) {
                PlannerError("Invalid time unit");
            }
            interval_value.unit = expr.interval_type_;
            Value value = Value::MakeInterval(interval_value);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kBoolean: {
            Value value = Value::MakeBool(expr.bool_value_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kNull: {
            Value value = Value::MakeNull();
            return MakeShared<ValueExpression>(value);
        }
    }

    PlannerError("Unreachable.");
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildColExpr(const ColumnExpr& expr,
                               BindContext* bind_context_ptr,
                               i64 depth,
                               bool root) {
    ColumnIdentifier column_identifier = ColumnIdentifier::MakeColumnIdentifier(query_context_, expr);
    SharedPtr<BaseExpression> column_expr = bind_context_ptr->ResolveColumnId(column_identifier, depth);
    return column_expr;
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildFuncExpr(const FunctionExpr& expr,
                                BindContext* bind_context_ptr,
                                i64 depth,
                                bool root) {

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);

    CheckFuncType(function_set_ptr->type_);

    // Check if it is count(*)
    if(function_set_ptr->name() == "COUNT") {
        if(expr.arguments_->size() == 1) {
            if((*expr.arguments_)[0]->type_ == ParsedExprType::kColumn) {
                ColumnExpr* col_expr = (ColumnExpr*)(*expr.arguments_)[0];
                if(col_expr->star_) {
                    delete (*expr.arguments_)[0];
                    (*expr.arguments_)[0] = new ConstantExpr(LiteralType::kBoolean);
                }
            }
        }
    }


    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(expr.arguments_->size());
    for(const auto* arg_expr : *expr.arguments_) {
        // The argument expression isn't root expression.
        // SharedPtr<BaseExpression> expr_ptr
        auto expr_ptr = BuildExpression(*arg_expr, bind_context_ptr, depth, false);
        arguments.emplace_back(expr_ptr);
    }

    switch(function_set_ptr->type_) {
        case FunctionType::kScalar:{
            // SharedPtr<ScalarFunctionSet> scalar_function_set_ptr
            auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
            ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
            SharedPtr<FunctionExpression> function_expr_ptr
                = MakeShared<FunctionExpression>(scalar_function, arguments);
            return function_expr_ptr;
        }
        case FunctionType::kAggregate: {
            // SharedPtr<AggregateFunctionSet> aggregate_function_set_ptr
            auto aggregate_function_set_ptr = std::static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
            AggregateFunction aggregate_function = aggregate_function_set_ptr->GetMostMatchFunction(arguments[0]);
            auto aggregate_function_ptr = MakeShared<AggregateExpression>(aggregate_function, arguments);
            return aggregate_function_ptr;
        }
        case FunctionType::kTable:
            PlannerError("Table function shouldn't be bound here.");
        default: {
            PlannerError("Unknown function type: " + function_set_ptr->name());
        }
    }
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildCastExpr(const CastExpr& expr,
                                BindContext* bind_context_ptr,
                                i64 depth,
                                bool root) {
    SharedPtr<BaseExpression> source_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);
    return CastExpression::AddCastToType(source_expr_ptr, expr.data_type_);
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildCaseExpr(const CaseExpr& expr,
                                BindContext* bind_context_ptr,
                                i64 depth,
                                bool root) {
    PlannerAssert(expr.case_check_array_, "No when and then expression");
    PlannerAssert(!expr.case_check_array_->empty(), "No when and then expression list");

    SharedPtr<CaseExpression> case_expression_ptr = MakeShared<CaseExpression>();
    // two kinds of case statement, please check:
    // https://docs.oracle.com/en/database/oracle/oracle-database/21/lnpls/CASE-statement.html

    if(expr.expr_) {
        // Simple case
        SharedPtr<BaseExpression> left_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);

        String function_name = "=";
        auto &catalog = Infinity::instance().catalog();
        SharedPtr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(function_name);
        auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);

        for (const WhenThen *when_then_expr : *expr.case_check_array_) {
            // Construct when expression: left_expr = value_expr
            Vector<SharedPtr<BaseExpression>> arguments;
            arguments.reserve(2);
            // SharedPtr<BaseExpression> value_expr
            auto value_expr = BuildExpression(*(when_then_expr->when_),
                                              bind_context_ptr,
                                              depth,
                                              false);
            arguments.emplace_back(left_expr_ptr);
            arguments.emplace_back(value_expr);
            ScalarFunction equal_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
            SharedPtr<FunctionExpression> when_expr_ptr
                    = MakeShared<FunctionExpression>(equal_function, arguments);

            // Construct then expression
            // SharedPtr<BaseExpression> then_expr
            SharedPtr<BaseExpression> then_expr_ptr = BuildExpression(*(when_then_expr->then_),
                                                                      bind_context_ptr,
                                                                      depth,
                                                                      false);
            case_expression_ptr->AddCaseCheck(when_expr_ptr, then_expr_ptr);
        }
    } else {
        // Searched case
        for (const WhenThen *when_then_expr : *expr.case_check_array_) {
            // Construct when expression: left_expr = value_expr
            // SharedPtr<BaseExpression> when_expr
            auto when_expr_ptr = BuildExpression(*(when_then_expr->when_),
                                                 bind_context_ptr,
                                                 depth,
                                                 false);

            // Construct then expression
            // SharedPtr<BaseExpression> then_expr
            SharedPtr<BaseExpression> then_expr_ptr = BuildExpression(*(when_then_expr->then_),
                                                                      bind_context_ptr,
                                                                      depth,
                                                                      false);
            case_expression_ptr->AddCaseCheck(when_expr_ptr, then_expr_ptr);
        }
    }
    // Construct else expression
    SharedPtr<BaseExpression> else_expr_ptr;
    if (expr.else_expr_ != nullptr) {
        else_expr_ptr = BuildExpression(*expr.else_expr_, bind_context_ptr, depth, false);
    } else {
        else_expr_ptr = MakeShared<ValueExpression>(Value::MakeNull());
    }
    case_expression_ptr->AddElseExpr(else_expr_ptr);

    return case_expression_ptr;
}

// Bind subquery expression.
SharedPtr<SubqueryExpression>
ExpressionBinder::BuildSubquery(const SubqueryExpr& expr,
                                BindContext* bind_context_ptr,
                                SubqueryType subquery_type,
                                i64 depth,
                                bool root) {

//    SharedPtr<BindContext> subquery_binding_context_ptr = MakeShared<BindContext>(bind_context_ptr);
//    SharedPtr<BoundSelectNode> select_node_ptr
//        = PlanBuilder::BuildSelect(query_context_, select, subquery_binding_context_ptr);
//
//    SharedPtr<SubqueryExpression> subquery_expr
//        = MakeShared<SubqueryExpression>(select_node_ptr, subquery_type);


    SharedPtr<BindContext> subquery_binding_context_ptr = BindContext::Make(bind_context_ptr);
    QueryBinder query_binder(this->query_context_, subquery_binding_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder.BindSelect(*expr.select_);

    SharedPtr<SubqueryExpression> subquery_expr
            = MakeShared<SubqueryExpression>(bound_statement_ptr, subquery_type);

    return subquery_expr;
}
//
//// Bind window function.
//SharedPtr<BaseExpression>
//ExpressionBinder::BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//    PlannerError("ExpressionBinder::BuildWindow");
//}

}

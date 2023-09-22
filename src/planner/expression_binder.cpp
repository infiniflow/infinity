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

#include "common/utility/infinity_assert.h"
#include "expression_binder.h"
#include "query_binder.h"
#include "expression/knn_expression.h"

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
        // result = Bind(expr, bind_context_ptr->parent_, depth + 1, root);
    }

    if(!expr.alias_.empty()) {
        result->alias_ = expr.alias_;
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
            const SubqueryExpr& sub_expr = (const SubqueryExpr&)expr;
            return BuildSubquery(sub_expr, bind_context_ptr, sub_expr.subquery_type_, depth, root);
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
        case ParsedExprType::kIn: {
            return BuildInExpr((const InExpr&)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kKnn: {
            return BuildKnnExpr((const KnnExpr&)expr, bind_context_ptr, depth, root);
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

    NewCatalog* catalog = query_context_->storage()->catalog();
    SharedPtr<FunctionExpression> left_function_expr{nullptr}, right_function_expr{nullptr};

    {
        String left_func = ">";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(lower_ptr);
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, left_func);
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
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, left_func);
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

    SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, and_func);
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
            Value value = Value::MakeVarchar(expr.str_value_);
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
        case LiteralType::kIntegerArray: {
            PlannerError("Unexpected literal type: integer array.");
        }
        case LiteralType::kDoubleArray: {
            PlannerError("Unexpected literal type: float array.");
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
    SharedPtr<ColumnExpression> column_expr = bind_context_ptr->ResolveColumnId(column_identifier, depth);
    if(column_expr != nullptr && column_expr->IsCorrelated()) {
        // Correlated column expression
        LOG_TRACE("Has correlated expr {}", column_expr->column_name());
        bind_context_ptr->AddCorrelatedColumnExpr(column_expr);
    }
    return column_expr;
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildFuncExpr(const FunctionExpr& expr,
                                BindContext* bind_context_ptr,
                                i64 depth,
                                bool root) {

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);

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

            for (SizeT idx = 0; idx < arguments.size(); ++idx) {
                    // check if the argument types are matched to the scalar function parameter types
                    // if not match, add the cast function to the input parameter.
                    if (arguments[idx]->Type() == scalar_function.parameter_types_[idx]) {
                        continue;
                    }
                    String name = arguments[idx]->Name();
                    arguments[idx] = CastExpression::AddCastToType(arguments[idx], scalar_function.parameter_types_[idx]);
                    // reset the alias name
                    arguments[idx]->alias_ = name;
            }

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

    DataType return_type{kInvalid};
    if(expr.expr_) {
        // Simple case
        SharedPtr<BaseExpression> left_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);

        String function_name = "=";
        NewCatalog* catalog = query_context_->storage()->catalog();
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, function_name);
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
            return_type.MaxDataType(then_expr_ptr->Type());
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
            return_type.MaxDataType(then_expr_ptr->Type());
        }
    }
    // Construct else expression
    SharedPtr<BaseExpression> else_expr_ptr;
    if (expr.else_expr_ != nullptr) {
        else_expr_ptr = BuildExpression(*expr.else_expr_, bind_context_ptr, depth, false);
        return_type.MaxDataType(else_expr_ptr->Type());
    } else {
        else_expr_ptr = MakeShared<ValueExpression>(Value::MakeNull());
    }
    case_expression_ptr->AddElseExpr(else_expr_ptr);

    case_expression_ptr->SetReturnType(return_type);
    return case_expression_ptr;
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildInExpr(const InExpr& expr,
                              BindContext* bind_context_ptr,
                              i64 depth,
                              bool root) {
    auto bound_left_expr = BuildExpression(*expr.left_, bind_context_ptr, depth, false);

    SizeT argument_count = expr.arguments_->size();
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(argument_count);

    for(SizeT idx = 0; idx < argument_count; ++ idx) {
        auto bound_argument_expr = BuildExpression(*expr.arguments_->at(idx), bind_context_ptr, depth, false);
        arguments.emplace_back(bound_argument_expr);
    }

    InType in_type{InType::kIn};
    if(expr.not_in_) {
        in_type = InType::kNotIn;
    } else {
        in_type = InType::kIn;
    }
    SharedPtr<InExpression> in_expression_ptr = MakeShared<InExpression>(in_type, bound_left_expr, arguments);
    return in_expression_ptr;
}

SharedPtr<BaseExpression>
ExpressionBinder::BuildKnnExpr(const KnnExpr& parsed_knn_expr,
                               BindContext* bind_context_ptr,
                               i64 depth,
                               bool root) {

    // Bind KNN expression
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(1);

    // Bind query column
    if(parsed_knn_expr.column_expr_->type_ != ParsedExprType::kColumn) {
        PlannerError("Knn expression expect a column expression");
    }
    auto expr_ptr = BuildColExpr((ColumnExpr&)*parsed_knn_expr.column_expr_, bind_context_ptr, depth, false);
    arguments.emplace_back(expr_ptr);

    // Create query embedding
    EmbeddingT query_embedding((ptr_t) parsed_knn_expr.embedding_data_ptr_);
    const_cast<KnnExpr&>(parsed_knn_expr).embedding_data_ptr_ = nullptr;

    SharedPtr<KnnExpression> bound_knn_expr = MakeShared<KnnExpression>(parsed_knn_expr.embedding_data_type_,
                                                                        parsed_knn_expr.dimension_,
                                                                        parsed_knn_expr.distance_type_,
                                                                        std::move(query_embedding),
                                                                        arguments);

    return bound_knn_expr;

}

// Bind subquery expression.
SharedPtr<SubqueryExpression>
ExpressionBinder::BuildSubquery(const SubqueryExpr& expr,
                                BindContext* bind_context_ptr,
                                SubqueryType subquery_type,
                                i64 depth,
                                bool root) {

    switch(subquery_type) {

        case SubqueryType::kIn:
        case SubqueryType::kNotIn: {
            auto bound_left_expr = BuildExpression(*expr.left_, bind_context_ptr, depth, false);

            SharedPtr<BindContext> subquery_binding_context_ptr = BindContext::Make(bind_context_ptr);
            QueryBinder query_binder(this->query_context_, subquery_binding_context_ptr);
            SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder.BindSelect(*expr.select_);

            SharedPtr<SubqueryExpression> in_subquery_expr
                    = MakeShared<SubqueryExpression>(bound_statement_ptr, subquery_type);
            in_subquery_expr->left_ = bound_left_expr;
            in_subquery_expr->correlated_columns = bind_context_ptr->correlated_column_exprs_;
            return in_subquery_expr;
        }
        case SubqueryType::kExists:
        case SubqueryType::kNotExists:
        case SubqueryType::kScalar: {
            SharedPtr<BindContext> subquery_binding_context_ptr = BindContext::Make(bind_context_ptr);
            QueryBinder query_binder(this->query_context_, subquery_binding_context_ptr);
            SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder.BindSelect(*expr.select_);

            SharedPtr<SubqueryExpression> subquery_expr
                    = MakeShared<SubqueryExpression>(bound_statement_ptr, subquery_type);

            subquery_expr->correlated_columns = bind_context_ptr->correlated_column_exprs_;
            return subquery_expr;
        }
        case SubqueryType::kAny: {
            NotImplementError("Any");
        }
    }

    PlannerError("Unreachable");
}
//
//// Bind window function.
//SharedPtr<BaseExpression>
//ExpressionBinder::BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//    PlannerError("ExpressionBinder::BuildWindow");
//}

}

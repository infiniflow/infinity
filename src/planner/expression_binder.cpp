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

#include <string>

module expression_binder;

import stl;

import type_info;
import infinity_exception;
import bind_context;
import third_party;
import value;
import logical_type;
import internal_types;
import base_expression;
import aggregate_expression;
import column_expression;
import in_expression;
import subquery_expression;
import knn_expression;
import cast_expression;
import case_expression;
import function_expression;
import value_expression;
import fusion_expression;
import search_expression;
import match_expression;
import match_tensor_expression;
import function;
import aggregate_function;
import aggregate_function_set;

import column_identifer;

import query_binder;
import bound_select_statement;

import function_set;
import scalar_function_set;
import scalar_function;
import special_function;
import status;

import query_context;
import logger;
import embedding_info;
import parsed_expr;
import function_expr;
import constant_expr;
import column_expr;
import case_expr;
import in_expr;
import knn_expr;
import search_expr;
import cast_expr;
import between_expr;
import subquery_expr;
import match_expr;
import match_tensor_expr;
import data_type;

import catalog;
import table_entry;

namespace infinity {

SharedPtr<BaseExpression> ExpressionBinder::Bind(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    // Call implemented BuildExpression

    SharedPtr<BaseExpression> result = BuildExpression(expr, bind_context_ptr, depth, root);
    if (result.get() == nullptr) {
        if (result.get() == nullptr) {
            Status status = Status::SyntaxError(fmt::format("Fail to bind the expression: {}", expr.GetName()));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        // Maybe the correlated expression, trying to bind it in the parent context.
        // result = Bind(expr, bind_context_ptr->parent_, depth + 1, root);
    }

    if (!expr.alias_.empty()) {
        result->alias_ = expr.alias_;
    }

    return result;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    switch (expr.type_) {
        case ParsedExprType::kConstant: {
            return BuildValueExpr((const ConstantExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kColumn: {
            return BuildColExpr((const ColumnExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kFunction: {
            return BuildFuncExpr((const FunctionExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kSubquery: {
            // subquery expression
            const SubqueryExpr &sub_expr = (const SubqueryExpr &)expr;
            return BuildSubquery(sub_expr, bind_context_ptr, sub_expr.subquery_type_, depth, root);
        }
        case ParsedExprType::kBetween: {
            return BuildBetweenExpr((const BetweenExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCase: {
            return BuildCaseExpr((const CaseExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCast: {
            // cast function expression
            return BuildCastExpr((const CastExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kIn: {
            return BuildInExpr((const InExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kKnn: {
            return BuildKnnExpr((const KnnExpr &)expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kSearch: {
            return BuildSearchExpr((const SearchExpr &)expr, bind_context_ptr, depth, root);
        }
        default: {
            UnrecoverableError("Unexpected expression type.");
        }
    }

    return SharedPtr<BaseExpression>();
}

SharedPtr<BaseExpression> ExpressionBinder::BuildBetweenExpr(const BetweenExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    auto value_ptr = BuildExpression(*expr.value_, bind_context_ptr, depth, false);
    auto lower_ptr = BuildExpression(*expr.lower_bound_, bind_context_ptr, depth, false);
    auto upper_ptr = BuildExpression(*expr.upper_bound_, bind_context_ptr, depth, false);

    Catalog *catalog = query_context_->storage()->catalog();
    SharedPtr<FunctionExpression> left_function_expr{nullptr}, right_function_expr{nullptr};

    {
        String left_func = ">";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(lower_ptr);
        SharedPtr<FunctionSet> function_set_ptr = Catalog::GetFunctionSetByName(catalog, left_func);
        CheckFuncType(function_set_ptr->type_);
        auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
        ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
        left_function_expr = MakeShared<FunctionExpression>(scalar_function, arguments);
    }

    {
        String left_func = "<";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(upper_ptr);
        SharedPtr<FunctionSet> function_set_ptr = Catalog::GetFunctionSetByName(catalog, left_func);
        CheckFuncType(function_set_ptr->type_);
        auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
        ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
        right_function_expr = MakeShared<FunctionExpression>(scalar_function, arguments);
    }

    String and_func = "and";
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(2);
    arguments.emplace_back(left_function_expr);
    arguments.emplace_back(right_function_expr);

    SharedPtr<FunctionSet> function_set_ptr = Catalog::GetFunctionSetByName(catalog, and_func);
    CheckFuncType(function_set_ptr->type_);
    auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
    ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
    return MakeShared<FunctionExpression>(scalar_function, arguments);
}

SharedPtr<BaseExpression> ExpressionBinder::BuildValueExpr(const ConstantExpr &expr, BindContext *, i64, bool) {
    switch (expr.literal_type_) {
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
        case LiteralType::kTime: {
            SizeT date_str_len = std::strlen(expr.date_value_);
            TimeT date_value;
            date_value.FromString(expr.date_value_, date_str_len);
            Value value = Value::MakeTime(date_value);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kDateTime: {
            SizeT date_str_len = std::strlen(expr.date_value_);
            DateTimeT date_value;
            date_value.FromString(expr.date_value_, date_str_len);
            Value value = Value::MakeDateTime(date_value);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kTimestamp: {
            SizeT date_str_len = std::strlen(expr.date_value_);
            TimestampT date_value;
            date_value.FromString(expr.date_value_, date_str_len);
            Value value = Value::MakeTimestamp(date_value);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kInterval: {
            // IntervalT should be a struct including the type of the value and an value of the interval
            // It will be bound into a ValueExpression here.
            IntervalT interval_value(expr.integer_value_);
            if (expr.interval_type_ == TimeUnit::kInvalidUnit) {
                UnrecoverableError("Invalid time unit");
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
            Value value = Value::MakeEmbedding(expr.long_array_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kDoubleArray: {
            Value value = Value::MakeEmbedding(expr.double_array_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kNull: {
            Value value = Value::MakeNull();
            return MakeShared<ValueExpression>(value);
        }
    }

    UnrecoverableError("Unreachable.");
}

SharedPtr<BaseExpression> ExpressionBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    ColumnIdentifier column_identifier = ColumnIdentifier::MakeColumnIdentifier(query_context_, expr);
    SharedPtr<ColumnExpression> column_expr = bind_context_ptr->ResolveColumnId(column_identifier, depth);
    if (column_expr.get() != nullptr && column_expr->IsCorrelated()) {
        // Correlated column expression
        LOG_TRACE(fmt::format("Has correlated expr {}", column_expr->column_name()));
        bind_context_ptr->AddCorrelatedColumnExpr(column_expr);
    }
    return column_expr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    auto special_function = TryBuildSpecialFuncExpr(expr, bind_context_ptr, depth);

    if (special_function.has_value()) {
        return special_function.value();
    }

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);

    CheckFuncType(function_set_ptr->type_);

    // Check if it is count(*)
    if (function_set_ptr->name() == "COUNT") {
        if (expr.arguments_->size() == 1) {
            if ((*expr.arguments_)[0]->type_ == ParsedExprType::kColumn) {
                ColumnExpr *col_expr = (ColumnExpr *)(*expr.arguments_)[0];
                if (col_expr->star_) {
                    String &table_name = bind_context_ptr->table_names_[0];
                    TableEntry *table_entry = bind_context_ptr->binding_by_name_[table_name]->table_collection_entry_ptr_;
                    col_expr->names_.clear();
                    col_expr->names_.emplace_back(table_entry->GetColumnDefByID(0)->name_);
                }
            }
        }
    }

    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(expr.arguments_->size());
    for (const auto *arg_expr : *expr.arguments_) {
        // The argument expression isn't root expression.
        // SharedPtr<BaseExpression> expr_ptr
        auto expr_ptr = BuildExpression(*arg_expr, bind_context_ptr, depth, false);
        arguments.emplace_back(expr_ptr);
    }

    switch (function_set_ptr->type_) {
        case FunctionType::kScalar: {
            // SharedPtr<ScalarFunctionSet> scalar_function_set_ptr
            auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
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

            SharedPtr<FunctionExpression> function_expr_ptr = MakeShared<FunctionExpression>(scalar_function, arguments);
            return function_expr_ptr;
        }
        case FunctionType::kAggregate: {
            // SharedPtr<AggregateFunctionSet> aggregate_function_set_ptr
            auto aggregate_function_set_ptr = static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
            AggregateFunction aggregate_function = aggregate_function_set_ptr->GetMostMatchFunction(arguments[0]);
            auto aggregate_function_ptr = MakeShared<AggregateExpression>(aggregate_function, arguments);
            return aggregate_function_ptr;
        }
        case FunctionType::kTable:
            UnrecoverableError("Table function shouldn't be bound here.");
        default: {
            UnrecoverableError(fmt::format("Unknown function type: {}", function_set_ptr->name()));
        }
    }
    return nullptr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildCastExpr(const CastExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    SharedPtr<BaseExpression> source_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);
    return CastExpression::AddCastToType(source_expr_ptr, expr.data_type_);
}

SharedPtr<BaseExpression> ExpressionBinder::BuildCaseExpr(const CaseExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    if (!expr.case_check_array_) {
        UnrecoverableError("No when and then expression");
    }
    if (expr.case_check_array_->empty()) {
        UnrecoverableError("No when and then expression");
    }

    SharedPtr<CaseExpression> case_expression_ptr = MakeShared<CaseExpression>();
    // two kinds of case statement, please check:
    // https://docs.oracle.com/en/database/oracle/oracle-database/21/lnpls/CASE-statement.html

    DataType return_type{kInvalid};
    if (expr.expr_) {
        // Simple case
        SharedPtr<BaseExpression> left_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);

        String function_name = "=";
        Catalog *catalog = query_context_->storage()->catalog();
        SharedPtr<FunctionSet> function_set_ptr = Catalog::GetFunctionSetByName(catalog, function_name);
        auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);

        for (const WhenThen *when_then_expr : *expr.case_check_array_) {
            // Construct when expression: left_expr = value_expr
            Vector<SharedPtr<BaseExpression>> arguments;
            arguments.reserve(2);
            // SharedPtr<BaseExpression> value_expr
            auto value_expr = BuildExpression(*(when_then_expr->when_), bind_context_ptr, depth, false);
            arguments.emplace_back(left_expr_ptr);
            arguments.emplace_back(value_expr);
            ScalarFunction equal_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);
            SharedPtr<FunctionExpression> when_expr_ptr = MakeShared<FunctionExpression>(equal_function, arguments);

            // Construct then expression
            // SharedPtr<BaseExpression> then_expr
            SharedPtr<BaseExpression> then_expr_ptr = BuildExpression(*(when_then_expr->then_), bind_context_ptr, depth, false);
            case_expression_ptr->AddCaseCheck(when_expr_ptr, then_expr_ptr);
            return_type.MaxDataType(then_expr_ptr->Type());
        }
    } else {
        // Searched case
        for (const WhenThen *when_then_expr : *expr.case_check_array_) {
            // Construct when expression: left_expr = value_expr
            // SharedPtr<BaseExpression> when_expr
            auto when_expr_ptr = BuildExpression(*(when_then_expr->when_), bind_context_ptr, depth, false);

            // Construct then expression
            // SharedPtr<BaseExpression> then_expr
            SharedPtr<BaseExpression> then_expr_ptr = BuildExpression(*(when_then_expr->then_), bind_context_ptr, depth, false);
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

SharedPtr<BaseExpression> ExpressionBinder::BuildInExpr(const InExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    auto bound_left_expr = BuildExpression(*expr.left_, bind_context_ptr, depth, false);

    SizeT argument_count = expr.arguments_->size();
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(argument_count);

    for (SizeT idx = 0; idx < argument_count; ++idx) {
        auto bound_argument_expr = BuildExpression(*expr.arguments_->at(idx), bind_context_ptr, depth, false);
        arguments.emplace_back(bound_argument_expr);
    }

    InType in_type{InType::kIn};
    if (expr.not_in_) {
        in_type = InType::kNotIn;
    } else {
        in_type = InType::kIn;
    }
    SharedPtr<InExpression> in_expression_ptr = MakeShared<InExpression>(in_type, bound_left_expr, arguments);
    return in_expression_ptr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildKnnExpr(const KnnExpr &parsed_knn_expr, BindContext *bind_context_ptr, i64 depth, bool) {
    // Bind KNN expression
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(1);

    // Bind query column
    if (parsed_knn_expr.column_expr_->type_ != ParsedExprType::kColumn) {
        UnrecoverableError("Knn expression expect a column expression");
    }
    if (parsed_knn_expr.topn_ <= 0) {
        String topn = std::to_string(parsed_knn_expr.topn_);
        Status status = Status::InvalidParameterValue("topn", topn, "topn should be greater than 0");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    auto expr_ptr = BuildColExpr((ColumnExpr &)*parsed_knn_expr.column_expr_, bind_context_ptr, depth, false);
    TypeInfo *type_info = expr_ptr->Type().type_info().get();
    if (type_info == nullptr or type_info->type() != TypeInfoType::kEmbedding) {
        Status status = Status::SyntaxError("Expect the column search is an embedding column");
        LOG_ERROR(status.message());
        RecoverableError(status);
    } else {
        EmbeddingInfo *embedding_info = (EmbeddingInfo *)type_info;
        if ((i64)embedding_info->Dimension() != parsed_knn_expr.dimension_) {
            Status status = Status::SyntaxError(fmt::format("Query embedding with dimension: {} which doesn't not matched with {}",
                                                            parsed_knn_expr.dimension_,
                                                            embedding_info->Dimension()));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }

    arguments.emplace_back(expr_ptr);

    // Create query embedding
    EmbeddingT query_embedding((ptr_t)parsed_knn_expr.embedding_data_ptr_, false);

    SharedPtr<KnnExpression> bound_knn_expr = MakeShared<KnnExpression>(parsed_knn_expr.embedding_data_type_,
                                                                        parsed_knn_expr.dimension_,
                                                                        parsed_knn_expr.distance_type_,
                                                                        std::move(query_embedding),
                                                                        arguments,
                                                                        parsed_knn_expr.topn_,
                                                                        parsed_knn_expr.opt_params_);

    return bound_knn_expr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    // Bind query column
    Vector<SharedPtr<BaseExpression>> arguments;
    if (expr.column_expr_->type_ != ParsedExprType::kColumn) {
        UnrecoverableError("MatchTensor expression expect a column expression");
    }
    // TODO: now only support MaxSim search method
    if (expr.search_method_ != MatchTensorMethod::kMaxSim) {
        Status status = Status::NotSupport(fmt::format("Unsupported search method: {}, now only support MaxSim search method",
                                                       MatchTensorExpr::MethodToString(expr.search_method_)));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    auto expr_ptr = BuildColExpr((ColumnExpr &)*expr.column_expr_, bind_context_ptr, depth, false);
    auto column_data_type = expr_ptr->Type();
    TypeInfo *type_info = column_data_type.type_info().get();
    u32 tensor_column_basic_embedding_dim = 0;
    if (column_data_type.type() != LogicalType::kTensor or type_info == nullptr or type_info->type() != TypeInfoType::kEmbedding) {
        Status status = Status::SyntaxError("Expect the column search is an tensor column");
        LOG_ERROR(status.message());
        RecoverableError(status);
    } else {
        EmbeddingInfo *embedding_info = (EmbeddingInfo *)type_info;
        tensor_column_basic_embedding_dim = embedding_info->Dimension();
        if (expr.dimension_ == 0 or expr.dimension_ % tensor_column_basic_embedding_dim != 0) {
            Status status = Status::SyntaxError(fmt::format("Query embedding with dimension: {} which doesn't match with tensor basic dimension {}",
                                                            expr.dimension_, embedding_info->Dimension()));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        // TODO: now only support float query tensor
        // TODO: now only support search on tensor column with float data type
        if (expr.embedding_data_type_ != EmbeddingDataType::kElemFloat) {
            Status status = Status::NotSupport(fmt::format("Unsupported query tensor data type: {}, now only support float input",
                                                           EmbeddingT::EmbeddingDataType2String(expr.embedding_data_type_)));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        if (embedding_info->Type() != EmbeddingDataType::kElemFloat) {
            Status status = Status::NotSupport(
                fmt::format("Unsupported tensor column type: {}, now only support search on float tensor column", embedding_info->ToString()));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
    arguments.emplace_back(std::move(expr_ptr));
    // Create query embedding
    EmbeddingT query_embedding((ptr_t)expr.embedding_data_ptr_, false);
    auto bound_tensor_maxsim_expr = MakeShared<MatchTensorExpression>(std::move(arguments),
                                                                      expr.search_method_,
                                                                      expr.embedding_data_type_,
                                                                      expr.dimension_,
                                                                      std::move(query_embedding),
                                                                      tensor_column_basic_embedding_dim,
                                                                      expr.options_text_);
    return bound_tensor_maxsim_expr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    Vector<SharedPtr<MatchExpression>> match_exprs;
    Vector<SharedPtr<KnnExpression>> knn_exprs;
    Vector<SharedPtr<MatchTensorExpression>> tensor_maxsim_exprs;
    SharedPtr<FusionExpression> fusion_expr = nullptr;
    for (MatchExpr *match_expr : expr.match_exprs_) {
        match_exprs.push_back(MakeShared<MatchExpression>(match_expr->fields_, match_expr->matching_text_, match_expr->options_text_));
    }
    for (KnnExpr *knn_expr : expr.knn_exprs_) {
        knn_exprs.push_back(static_pointer_cast<KnnExpression>(BuildKnnExpr(*knn_expr, bind_context_ptr, depth, false)));
    }
    for (MatchTensorExpr *tensor_maxsim_expr : expr.tensor_maxsim_exprs_) {
        tensor_maxsim_exprs.push_back(
            static_pointer_cast<MatchTensorExpression>(BuildMatchTensorExpr(*tensor_maxsim_expr, bind_context_ptr, depth, false)));
    }
    if (expr.fusion_expr_ != nullptr)
        fusion_expr = MakeShared<FusionExpression>(expr.fusion_expr_->method_, expr.fusion_expr_->options_);
    SharedPtr<SearchExpression> bound_search_expr = MakeShared<SearchExpression>(match_exprs, knn_exprs, tensor_maxsim_exprs, fusion_expr);
    return bound_search_expr;
}

// Bind subquery expression.
SharedPtr<SubqueryExpression>
ExpressionBinder::BuildSubquery(const SubqueryExpr &expr, BindContext *bind_context_ptr, SubqueryType subquery_type, i64 depth, bool) {

    switch (subquery_type) {

        case SubqueryType::kIn:
        case SubqueryType::kNotIn: {
            auto bound_left_expr = BuildExpression(*expr.left_, bind_context_ptr, depth, false);

            SharedPtr<BindContext> subquery_binding_context_ptr = BindContext::Make(bind_context_ptr);
            QueryBinder query_binder(this->query_context_, subquery_binding_context_ptr);
            UniquePtr<BoundSelectStatement> bound_statement_ptr = query_binder.BindSelect(*expr.select_);

            SharedPtr<SubqueryExpression> in_subquery_expr = MakeShared<SubqueryExpression>(std::move(bound_statement_ptr), subquery_type);
            in_subquery_expr->left_ = bound_left_expr;
            in_subquery_expr->correlated_columns = bind_context_ptr->correlated_column_exprs_;
            return in_subquery_expr;
        }
        case SubqueryType::kExists:
        case SubqueryType::kNotExists:
        case SubqueryType::kScalar: {
            SharedPtr<BindContext> subquery_binding_context_ptr = BindContext::Make(bind_context_ptr);
            QueryBinder query_binder(this->query_context_, subquery_binding_context_ptr);
            UniquePtr<BoundSelectStatement> bound_statement_ptr = query_binder.BindSelect(*expr.select_);

            SharedPtr<SubqueryExpression> subquery_expr = MakeShared<SubqueryExpression>(std::move(bound_statement_ptr), subquery_type);

            subquery_expr->correlated_columns = bind_context_ptr->correlated_column_exprs_;
            return subquery_expr;
        }
        case SubqueryType::kAny: {
            UnrecoverableError("Not implement: Any");
        }
    }

    UnrecoverableError("Unreachable");
    return nullptr;
}

Optional<SharedPtr<BaseExpression>> ExpressionBinder::TryBuildSpecialFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth) {
    auto [special_function_ptr, status] = Catalog::GetSpecialFunctionByNameNoExcept(query_context_->storage()->catalog(), expr.func_name_);
    if (status.ok()) {
        switch (special_function_ptr->special_type()) {
            case SpecialType::kDistance: {
                if (!bind_context_ptr->allow_distance) {
                    Status status = Status::SyntaxError("DISTANCE() needs to be allowed only when there is only MATCH VECTOR");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
                break;
            }
            case SpecialType::kScore: {
                if (!bind_context_ptr->allow_score) {
                    Status status = Status::SyntaxError("SCORE() requires Fusion or MATCH TEXT or MATCH TENSOR");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
                break;
            }
            default: {
                break;
            }
        }

        String &table_name = bind_context_ptr->table_names_[0];
        String column_name = special_function_ptr->name();

        TableEntry *table_entry = bind_context_ptr->binding_by_name_[table_name]->table_collection_entry_ptr_;
        SharedPtr<ColumnExpression> bound_column_expr = ColumnExpression::Make(special_function_ptr->data_type(),
                                                                               table_name,
                                                                               bind_context_ptr->table_name2table_index_[table_name],
                                                                               column_name,
                                                                               table_entry->ColumnCount() + special_function_ptr->extra_idx(),
                                                                               depth,
                                                                               special_function_ptr->special_type());
        return bound_column_expr;
    } else {
        return None;
    }
}

//
//// Bind window function.
// SharedPtr<BaseExpression>
// ExpressionBinder::BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//     PlannerError("ExpressionBinder::BuildWindow");
// }

} // namespace infinity

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
#include <memory>

module infinity_core:expression_binder.impl;

import :expression_binder;

import :stl;
import :default_values;
import type_info;
import :infinity_exception;
import :bind_context;
import :third_party;
import :value;
import logical_type;
import internal_types;
import :base_expression;
import :aggregate_expression;
import :column_expression;
import :in_expression;
import :subquery_expression;
import :knn_expression;
import :cast_expression;
import :case_expression;
import :function_expression;
import :value_expression;
import :fusion_expression;
import :search_expression;
import :match_expression;
import :match_tensor_expression;
import :match_sparse_expression;
import :filter_fulltext_expression;
import :unnest_expression;
import :function;
import :aggregate_function;
import :aggregate_function_set;
import embedding_info;

import :column_identifier;

import :query_binder;
import :bound_select_statement;

import :function_set;
import :scalar_function_set;
import :scalar_function;
import :special_function;
import :cast_function;
import :bound_cast_func;
import :status;

import :query_context;
import :logger;
import embedding_info;
import sparse_info;
import array_info;
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
import match_sparse_expr;
import fusion_expr;
import data_type;
import :expression_type;
import :meta_info;
import :column_vector;
import :new_catalog;

namespace infinity {

template <typename T>
ptr_t GetConcatenatedTensorData(const ConstantExpr *tensor_expr_, const u32 tensor_column_basic_embedding_dim, u32 &query_total_dimension);

SharedPtr<BaseExpression> ExpressionBinder::Bind(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    // Call implemented BuildExpression

    SharedPtr<BaseExpression> result = BuildExpression(expr, bind_context_ptr, depth, root);
    if (result.get() == nullptr) {
        Status status = Status::SyntaxError(fmt::format("Fail to bind the expression: {}", expr.GetName()));
        RecoverableError(status);
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
            return BuildValueExpr(static_cast<const ConstantExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kColumn: {
            return BuildColExpr(static_cast<const ColumnExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kFunction: {
            auto &func_expr = static_cast<const FunctionExpr &>(expr);
            if (IsUnnestedFunction(func_expr.func_name_)) {
                return BuildUnnestExpr(func_expr, bind_context_ptr, depth, root);
            }
            return BuildFuncExpr(func_expr, bind_context_ptr, depth, root);
        }
        case ParsedExprType::kSubquery: {
            // subquery expression
            const SubqueryExpr &sub_expr = static_cast<const SubqueryExpr &>(expr);
            return BuildSubquery(sub_expr, bind_context_ptr, sub_expr.subquery_type_, depth, root);
        }
        case ParsedExprType::kBetween: {
            return BuildBetweenExpr(static_cast<const BetweenExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCase: {
            return BuildCaseExpr(static_cast<const CaseExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kCast: {
            // cast function expression
            return BuildCastExpr(static_cast<const CastExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kIn: {
            return BuildInExpr(static_cast<const InExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kSearch: {
            return BuildSearchExpr(static_cast<const SearchExpr &>(expr), bind_context_ptr, depth, root);
        }
        case ParsedExprType::kMatch: {
            return BuildMatchTextExpr(static_cast<const MatchExpr &>(expr), bind_context_ptr, depth, root);
        }
        default: {
            UnrecoverableError(fmt::format("ExpressionBinder::BuildExpression: Unexpected ParsedExprType for ParsedExpr: {}.", expr.ToString()));
        }
    }

    return SharedPtr<BaseExpression>();
}

SharedPtr<BaseExpression> ExpressionBinder::BuildBetweenExpr(const BetweenExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    auto value_ptr = BuildExpression(*expr.value_, bind_context_ptr, depth, false);
    auto lower_ptr = BuildExpression(*expr.lower_bound_, bind_context_ptr, depth, false);
    auto upper_ptr = BuildExpression(*expr.upper_bound_, bind_context_ptr, depth, false);

    NewCatalog *catalog = query_context_->storage()->new_catalog();
    SharedPtr<FunctionExpression> left_function_expr{nullptr}, right_function_expr{nullptr};

    {
        String left_func = ">";
        Vector<SharedPtr<BaseExpression>> arguments;
        arguments.reserve(2);
        arguments.emplace_back(value_ptr);
        arguments.emplace_back(lower_ptr);
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, left_func);
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
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, left_func);
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

    SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, and_func);
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
                String error_message = "Invalid time unit";
                UnrecoverableError(error_message);
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
        case LiteralType::kLongSparseArray: {
            Value value = Value::MakeSparse(expr.long_sparse_array_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kDoubleSparseArray: {
            Value value = Value::MakeSparse(expr.double_sparse_array_);
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kSubArrayArray: {
            if (expr.sub_array_array_.size() == 0) {
                String error_message = "Empty subarray array";
                UnrecoverableError(error_message);
            }
            switch (expr.sub_array_array_[0]->literal_type_) {
                case LiteralType::kIntegerArray:
                case LiteralType::kDoubleArray: {
                    // expect it to be tensor of i64 or double data type
                    const u32 basic_embedding_dim = expr.sub_array_array_[0]->long_array_.size() + expr.sub_array_array_[0]->double_array_.size();
                    bool have_double = false;
                    for (const auto &sub_array : expr.sub_array_array_) {
                        if (sub_array->literal_type_ == LiteralType::kDoubleArray) {
                            have_double = true;
                            break;
                        }
                    }
                    if (have_double) {
                        u32 tensor_total_dim = 0;
                        const auto embedding_data_ptr = GetConcatenatedTensorData<double>(&expr, basic_embedding_dim, tensor_total_dim);
                        UniquePtr<double[]> embedding_data;
                        embedding_data.reset(reinterpret_cast<double *>(embedding_data_ptr));
                        auto type_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemDouble, basic_embedding_dim);
                        Value value = Value::MakeTensor(embedding_data_ptr, tensor_total_dim * sizeof(double), std::move(type_info_ptr));
                        return MakeShared<ValueExpression>(std::move(value));
                    } else {
                        u32 tensor_total_dim = 0;
                        const auto embedding_data_ptr = GetConcatenatedTensorData<i64>(&expr, basic_embedding_dim, tensor_total_dim);
                        UniquePtr<i64[]> embedding_data;
                        embedding_data.reset(reinterpret_cast<i64 *>(embedding_data_ptr));
                        auto type_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, basic_embedding_dim);
                        Value value = Value::MakeTensor(embedding_data_ptr, tensor_total_dim * sizeof(i64), std::move(type_info_ptr));
                        return MakeShared<ValueExpression>(std::move(value));
                    }
                }
                case LiteralType::kSubArrayArray: {
                    // expect it to be tensor-array type
                    bool have_double = false;
                    for (const auto &sub_array : expr.sub_array_array_) {
                        if (sub_array->literal_type_ != LiteralType::kSubArrayArray) {
                            const auto error_info = "Invalid TensorArray input format.";
                            RecoverableError(Status::SyntaxError(error_info));
                        }
                        if (have_double) {
                            // skip the check if already have double
                            continue;
                        }
                        for (const auto &sub_sub_array : sub_array->sub_array_array_) {
                            if (sub_sub_array->literal_type_ == LiteralType::kDoubleArray) {
                                have_double = true;
                                break;
                            }
                        }
                    }
                    const auto &grand_child_ptr = expr.sub_array_array_[0]->sub_array_array_[0];
                    const u32 basic_embedding_dim = grand_child_ptr->long_array_.size() + grand_child_ptr->double_array_.size();
                    if (have_double) {
                        auto type_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemDouble, basic_embedding_dim);
                        Value value = Value::MakeTensorArray(std::move(type_info_ptr));
                        for (const auto &sub_array : expr.sub_array_array_) {
                            u32 child_tensor_total_dim = 0;
                            const auto embedding_data_ptr =
                                GetConcatenatedTensorData<double>(sub_array.get(), basic_embedding_dim, child_tensor_total_dim);
                            UniquePtr<double[]> embedding_data;
                            embedding_data.reset(reinterpret_cast<double *>(embedding_data_ptr));
                            value.AppendToTensorArray(embedding_data_ptr, child_tensor_total_dim * sizeof(double));
                        }
                        return MakeShared<ValueExpression>(std::move(value));
                    } else {
                        auto type_info_ptr = EmbeddingInfo::Make(EmbeddingDataType::kElemInt64, basic_embedding_dim);
                        Value value = Value::MakeTensorArray(std::move(type_info_ptr));
                        for (const auto &sub_array : expr.sub_array_array_) {
                            u32 child_tensor_total_dim = 0;
                            const auto embedding_data_ptr =
                                GetConcatenatedTensorData<i64>(sub_array.get(), basic_embedding_dim, child_tensor_total_dim);
                            UniquePtr<i64[]> embedding_data;
                            embedding_data.reset(reinterpret_cast<i64 *>(embedding_data_ptr));
                            value.AppendToTensorArray(embedding_data_ptr, child_tensor_total_dim * sizeof(i64));
                        }
                        return MakeShared<ValueExpression>(std::move(value));
                    }
                }
                default: {
                    String error_message = "Unexpected subarray type";
                    UnrecoverableError(error_message);
                    return nullptr;
                }
            }
        }
        case LiteralType::kNull: {
            Value value = Value::MakeNull();
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kEmptyArray: {
            Value value = Value::MakeEmptyArray();
            return MakeShared<ValueExpression>(value);
        }
        case LiteralType::kCurlyBracketsArray: {
            Vector<Value> element_values;
            for (const auto &element : expr.curly_brackets_array_) {
                const auto elem_expr = BuildValueExpr(*element, nullptr, 0, false);
                auto *val_expr = dynamic_cast<const ValueExpression *>(elem_expr.get());
                element_values.emplace_back(val_expr->GetValue());
            }
            // check types
            DataType common_element_type{LogicalType::kInvalid};
            for (const auto &element : element_values) {
                if (common_element_type.type() == LogicalType::kInvalid) {
                    common_element_type = element.type();
                } else if (common_element_type != element.type()) {
                    // try cast
                    try {
                        CastFunction::GetBoundFunc(element.type(), common_element_type);
                    } catch (...) {
                        try {
                            CastFunction::GetBoundFunc(common_element_type, element.type());
                            common_element_type = element.type();
                        } catch (...) {
                            RecoverableError(Status::SyntaxError(fmt::format("Array elements have incompatible types: {} and {}",
                                                                             common_element_type.ToString(),
                                                                             element.type().ToString())));
                        }
                    }
                }
            }
            // do necessary cast
            for (auto &element : element_values) {
                if (element.type() != common_element_type) {
                    // cast
                    BoundCastFunc cast = CastFunction::GetBoundFunc(element.type(), common_element_type);
                    auto element_column_vector = MakeShared<ColumnVector>(MakeShared<DataType>(element.type()));
                    element_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
                    element_column_vector->AppendValue(element);
                    auto cast_column_vector = MakeShared<ColumnVector>(MakeShared<DataType>(common_element_type));
                    cast_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
                    CastParameters cast_parameters;
                    cast.function(element_column_vector, cast_column_vector, 1, cast_parameters);
                    Value cast_val = cast_column_vector->GetValueByIndex(0);
                    element = std::move(cast_val);
                }
            }
            Value value = Value::MakeArray(std::move(element_values), ArrayInfo::Make(std::move(common_element_type)));
            return MakeShared<ValueExpression>(value);
        }
    }

    String error_message = "Unreachable";
    UnrecoverableError(error_message);
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

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->new_catalog(), expr);

    CheckFuncType(function_set_ptr->type_);

    // Check if it is count(*)
    if (function_set_ptr->name() == "COUNT") {
        if (!expr.arguments_ || expr.arguments_->empty()) {
            RecoverableError(Status::SyntaxError("No arguments for COUNT function found."));
        }
        if (expr.arguments_->size() == 1) {
            if ((*expr.arguments_)[0]->type_ == ParsedExprType::kColumn) {
                ColumnExpr *col_expr = (ColumnExpr *)(*expr.arguments_)[0];
                if (col_expr->star_) {
                    String &table_name = bind_context_ptr->table_names_[0];
                    TableInfo *table_info = bind_context_ptr->binding_by_name_[table_name]->table_info_.get();
                    col_expr->names_.clear();
                    col_expr->names_.emplace_back(table_info->GetColumnDefByID(0)->name_);
                }
            }
        }
    }

    Vector<SharedPtr<BaseExpression>> arguments;
    if (expr.arguments_ != nullptr) {
        arguments.reserve(expr.arguments_->size());
        for (const auto *arg_expr : *expr.arguments_) {
            // The argument expression isn't root expression.
            // SharedPtr<BaseExpression> expr_ptr
            auto expr_ptr = BuildExpression(*arg_expr, bind_context_ptr, depth, false);
            arguments.emplace_back(expr_ptr);
        }
    }

    switch (function_set_ptr->type_) {
        case FunctionType::kScalar: {
            // SharedPtr<ScalarFunctionSet> scalar_function_set_ptr
            auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
            ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);

            for (SizeT idx = 0; idx < arguments.size(); ++idx) {
                // Check if the argument is an embedding type but the function doesn't expect it
                if (arguments[idx]->Type().type() == LogicalType::kEmbedding &&
                    scalar_function.parameter_types_[idx].type() != LogicalType::kEmbedding &&
                    scalar_function.parameter_types_[idx].type() != LogicalType::kArray) {
                    return nullptr;
                }
                // check if the argument types are matched to the scalar function parameter types
                // if not match, add the cast function to the input parameter.
                if (arguments[idx]->Type() == scalar_function.parameter_types_[idx]) {
                    continue;
                }

                // Special handling for FDE function - skip casting for embedding/tensor types with different dimensions
                if (scalar_function.name() == "FDE" && idx == 0 &&
                    (arguments[idx]->Type().type() == LogicalType::kEmbedding || arguments[idx]->Type().type() == LogicalType::kTensor) &&
                    (scalar_function.parameter_types_[idx].type() == LogicalType::kEmbedding ||
                     scalar_function.parameter_types_[idx].type() == LogicalType::kTensor)) {
                    // For FDE function, skip casting between embedding/tensor types - let the function handle it at runtime
                    continue;
                }

                String name = arguments[idx]->Name();
                arguments[idx] = CastExpression::AddCastToType(arguments[idx], scalar_function.parameter_types_[idx]);
                // reset the alias name
                arguments[idx]->alias_ = name;
            }

            SharedPtr<FunctionExpression> function_expr_ptr = MakeShared<FunctionExpression>(scalar_function, arguments);

            // Special handling for FDE function - adjust return type based on target dimension parameter
            if (scalar_function.name() == "FDE" && arguments.size() >= 2) {
                // Extract target dimension from the second argument if it's a constant
                if (arguments[1]->type() == ExpressionType::kValue) {
                    auto value_expr = std::static_pointer_cast<ValueExpression>(arguments[1]);
                    Value target_dim_value = value_expr->GetValue();
                    if (target_dim_value.type().type() == LogicalType::kBigInt) {
                        i32 target_dimension = static_cast<i32>(target_dim_value.GetValue<BigIntT>());
                        // Create the correct return type with the target dimension
                        auto target_embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, target_dimension);
                        auto target_return_type = DataType(LogicalType::kEmbedding, target_embedding_info);
                        // Update the scalar function's return type
                        function_expr_ptr->func_.return_type_ = target_return_type;
                    }
                }
            }

            return function_expr_ptr;
        }
        case FunctionType::kAggregate: {
            // SharedPtr<AggregateFunctionSet> aggregate_function_set_ptr
            auto aggregate_function_set_ptr = static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
            AggregateFunction aggregate_function = aggregate_function_set_ptr->GetMostMatchFunction(arguments[0]);
            auto aggregate_function_ptr = MakeShared<AggregateExpression>(aggregate_function, arguments);
            return aggregate_function_ptr;
        }
        case FunctionType::kTable: {
            String error_message = "Table function shouldn't be bound here.";
            UnrecoverableError(error_message);
        }
        default: {
            String error_message = fmt::format("Unknown function type: {}", function_set_ptr->name());
            UnrecoverableError(error_message);
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
        String error_message = "No when and then expression";
        UnrecoverableError(error_message);
    }
    if (expr.case_check_array_->empty()) {
        String error_message = "No when and then expression";
        UnrecoverableError(error_message);
    }

    SharedPtr<CaseExpression> case_expression_ptr = MakeShared<CaseExpression>();
    // two kinds of case statement, please check:
    // https://docs.oracle.com/en/database/oracle/oracle-database/21/lnpls/CASE-statement.html

    DataType return_type{LogicalType::kInvalid};
    if (expr.expr_) {
        // Simple case
        SharedPtr<BaseExpression> left_expr_ptr = BuildExpression(*expr.expr_, bind_context_ptr, depth, false);

        String function_name = "=";
        NewCatalog *catalog = query_context_->storage()->new_catalog();
        SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, function_name);
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

    // in operator, all data type shouhld be the same
    SharedPtr<DataType> arguments_type = nullptr;

    for (SizeT idx = 0; idx < argument_count; ++idx) {
        if (expr.arguments_->at(idx)->type_ != ParsedExprType::kConstant) {
            Status status = Status::SyntaxError("In expression now only supports constant list!");
            RecoverableError(status);
        }
        auto bound_argument_expr = BuildExpression(*expr.arguments_->at(idx), bind_context_ptr, depth, false);

        if (arguments_type != nullptr && bound_argument_expr->Type() != *arguments_type) {
            Status status = Status::SyntaxError("Expressions in In expression must be of the same data type!");
            RecoverableError(status);
        } else if (arguments_type == nullptr) {
            arguments_type = MakeShared<DataType>(bound_argument_expr->Type());
        }

        arguments.emplace_back(bound_argument_expr);
    }

    InType in_type{InType::kIn};
    if (expr.in_) {
        in_type = InType::kIn;
    } else {
        in_type = InType::kNotIn;
    }

    SharedPtr<InExpression> in_expression_ptr = MakeShared<InExpression>(in_type, bound_left_expr, arguments);

    // if match
    if (arguments_type->type() == bound_left_expr->Type().type()) {
        for (SizeT idx = 0; idx < argument_count; idx++) {
            ValueExpression *val_expr = static_cast<ValueExpression *>(arguments[idx].get());
            Value val = val_expr->GetValue();
            in_expression_ptr->TryPut(std::move(val));
        }
    } else if (CastExpression::CanCast(*arguments_type, bound_left_expr->Type())) {
        // cast
        BoundCastFunc cast = CastFunction::GetBoundFunc(*arguments_type, bound_left_expr->Type());

        SharedPtr<ColumnVector> argument_column_vector = MakeShared<ColumnVector>(arguments_type);
        argument_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

        for (SizeT idx = 0; idx < argument_count; idx++) {
            ValueExpression *val_expr = static_cast<ValueExpression *>(arguments[idx].get());
            argument_column_vector->AppendValue(val_expr->GetValue());
        }

        SharedPtr<ColumnVector> cast_column_vector = MakeShared<ColumnVector>(MakeShared<DataType>(bound_left_expr->Type()));
        // will overflow when passing argument_count
        cast_column_vector->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        CastParameters cast_parameters;
        cast.function(argument_column_vector, cast_column_vector, argument_count, cast_parameters);

        for (SizeT idx = 0; idx < argument_count; idx++) {
            Value val = cast_column_vector->GetValueByIndex(idx);
            in_expression_ptr->TryPut(std::move(val));
        }
    } else {
        Status status = Status::NotSupportedTypeConversion(arguments_type->ToString(), bound_left_expr->Type().ToString());
        RecoverableError(status);
    }

    return in_expression_ptr;
}

inline bool EmbeddingEmbeddingQueryTypeValidated(const EmbeddingDataType column_embedding_type, const EmbeddingDataType query_embedding_type) {
    switch (column_embedding_type) {
        case EmbeddingDataType::kElemBit: {
            // accept all query embedding types
            return query_embedding_type != EmbeddingDataType::kElemInvalid;
        }
        case EmbeddingDataType::kElemUInt8:
        case EmbeddingDataType::kElemInt8: {
            // expect query embedding to be the same type
            if (query_embedding_type == column_embedding_type) {
                return true;
            }
            Status status = Status::SyntaxError(fmt::format("Query embedding with data type: {} which doesn't match with column embedding type {}.",
                                                            EmbeddingInfo::EmbeddingDataTypeToString(query_embedding_type),
                                                            EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
            RecoverableError(std::move(status));
            return false;
        }
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64: {
            // TODO: not supported yet
            Status status = Status::SyntaxError(
                fmt::format("Cannot query on column with embedding data type: {}.", EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));

            RecoverableError(std::move(status));
            return false;
        }
        case EmbeddingDataType::kElemFloat:
        case EmbeddingDataType::kElemDouble:
        case EmbeddingDataType::kElemFloat16:
        case EmbeddingDataType::kElemBFloat16: {
            // expect query embedding to be also float type
            switch (query_embedding_type) {
                case EmbeddingDataType::kElemFloat:
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemFloat16:
                case EmbeddingDataType::kElemBFloat16: {
                    return true;
                }
                default: {
                    Status status =
                        Status::SyntaxError(fmt::format("Query embedding with int data type: {} which doesn't match with column embedding type {}.",
                                                        EmbeddingInfo::EmbeddingDataTypeToString(query_embedding_type),
                                                        EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
                    RecoverableError(std::move(status));
                    return false;
                }
            }
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Invalid embedding data type");
            return false;
        }
    }
    return false;
}

void ValidateSearchSubExprOptionalFilter(BaseExpression *optional_filter,
                                         const int inner_used_depth = 0,
                                         const BaseExpression *original_filter = nullptr) {
    // expect the optional filter to be a boolean expression
    if (inner_used_depth == 0 && optional_filter->Type().type() != LogicalType::kBoolean) {
        RecoverableError(
            Status::InvalidFilterExpression(fmt::format("SearchSubExprOptionalFilter: Invalid expression type {} for filter expression {}.",
                                                        optional_filter->Type().ToString(),
                                                        optional_filter->ToString())));
    }
    if (!original_filter) {
        original_filter = optional_filter;
    }
    // can use: column, cast, constant, function, in, match_text, filter_fulltext
    switch (optional_filter->type()) {
        case ExpressionType::kColumn:
        case ExpressionType::kCast:
        case ExpressionType::kValue:
        case ExpressionType::kFunction:
        case ExpressionType::kIn:
        case ExpressionType::kMatch:
        case ExpressionType::kFilterFullText: {
            break;
        }
        default: {
            RecoverableError(Status::InvalidFilterExpression(
                fmt::format("SearchSubExprOptionalFilter: Invalid subexpression '{}' for filter expression '{}'."
                            "Now can only use column, cast, value, function, in, match_text, filter_fulltext expressions in filter.",
                            optional_filter->ToString(),
                            original_filter->ToString())));
        }
    }
    // check all children
    for (const auto &child : optional_filter->arguments()) {
        ValidateSearchSubExprOptionalFilter(child.get(), inner_used_depth + 1, optional_filter);
    }
}

auto BuildSearchSubExprOptionalFilter(ExpressionBinder *src_this, const ParsedExpr *filter_expr, BindContext *bind_context_ptr, const i64 depth) {
    SharedPtr<BaseExpression> optional_filter;
    if (filter_expr) {
        optional_filter = src_this->Bind(*filter_expr, bind_context_ptr, depth, false);
        ValidateSearchSubExprOptionalFilter(optional_filter.get());
    }
    return optional_filter;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildKnnExpr(const KnnExpr &parsed_knn_expr, BindContext *bind_context_ptr, i64 depth, bool) {
    // Bind KNN expression
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.reserve(1);

    // Bind query column
    if (parsed_knn_expr.column_expr_->type_ != ParsedExprType::kColumn) {
        String error_message = "Knn expression expect a column expression";
        UnrecoverableError(error_message);
    }
    if (parsed_knn_expr.topn_ <= 0) {
        String topn = std::to_string(parsed_knn_expr.topn_);
        Status status = Status::InvalidParameterValue("topn", topn, "topn should be greater than 0");
        RecoverableError(status);
    }
    auto expr_ptr = BuildColExpr((ColumnExpr &)*parsed_knn_expr.column_expr_, bind_context_ptr, depth, false);
    TypeInfo *type_info = expr_ptr->Type().type_info().get();
    if (type_info == nullptr or type_info->type() != TypeInfoType::kEmbedding) {
        Status status = Status::SyntaxError("Expect the column search is an embedding column");
        RecoverableError(status);
    } else {
        EmbeddingInfo *embedding_info = (EmbeddingInfo *)type_info;

        // For function expressions (like FDE), skip dimension validation at parse time
        // The dimension will be validated at execution time when the function is evaluated
        if (!parsed_knn_expr.query_embedding_expr_) {
            // Traditional array case - validate dimension
            if ((i64)embedding_info->Dimension() != parsed_knn_expr.dimension_) {
                Status status = Status::SyntaxError(fmt::format("Query embedding with dimension: {} which doesn't not matched with {}",
                                                                parsed_knn_expr.dimension_,
                                                                embedding_info->Dimension()));
                RecoverableError(status);
            }
            if (const auto column_embedding_type = embedding_info->Type(), query_embedding_type = parsed_knn_expr.embedding_data_type_;
                !EmbeddingEmbeddingQueryTypeValidated(column_embedding_type, query_embedding_type)) {
                Status status =
                    Status::SyntaxError(fmt::format("Query embedding with data type: {} which doesn't match with column embedding type {}.",
                                                    EmbeddingInfo::EmbeddingDataTypeToString(query_embedding_type),
                                                    EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
                RecoverableError(std::move(status));
            }
        }
        // For function expressions, we'll validate compatibility at execution time
    }

    arguments.emplace_back(expr_ptr);

    // Handle query embedding - either from array or function expression
    EmbeddingDataType embedding_data_type;
    i64 dimension;

    if (parsed_knn_expr.query_embedding_expr_) {
        // Function expression case (e.g., FDE function)
        // Bind the function expression and add it to arguments
        auto bound_func_expr = Bind(*parsed_knn_expr.query_embedding_expr_, bind_context_ptr, depth, false);
        arguments.emplace_back(bound_func_expr);

        // Try to infer embedding data type from string, default to float
        if (parsed_knn_expr.embedding_data_type_str_ == "float") {
            embedding_data_type = EmbeddingDataType::kElemFloat;
        } else if (parsed_knn_expr.embedding_data_type_str_ == "double") {
            embedding_data_type = EmbeddingDataType::kElemDouble;
        } else {
            embedding_data_type = EmbeddingDataType::kElemFloat; // default
        }

        // Use placeholder dimension - will be determined at execution time
        dimension = 0;

        // Create a placeholder embedding - the actual embedding will be computed at execution time
        EmbeddingT query_embedding(static_cast<char *>(nullptr), false);

        if (parsed_knn_expr.ignore_index_ && !parsed_knn_expr.index_name_.empty()) {
            Status status = Status::SyntaxError(fmt::format("Force to use index {} conflicts with Ignore index flag.", parsed_knn_expr.index_name_));
            RecoverableError(std::move(status));
        }

        // create optional filter
        auto optional_filter = BuildSearchSubExprOptionalFilter(this, parsed_knn_expr.filter_expr_.get(), bind_context_ptr, depth);

        SharedPtr<KnnExpression> bound_knn_expr = MakeShared<KnnExpression>(embedding_data_type,
                                                                            dimension,
                                                                            parsed_knn_expr.distance_type_,
                                                                            std::move(query_embedding),
                                                                            std::move(arguments),
                                                                            parsed_knn_expr.topn_,
                                                                            parsed_knn_expr.opt_params_,
                                                                            std::move(optional_filter),
                                                                            parsed_knn_expr.index_name_,
                                                                            parsed_knn_expr.ignore_index_);
        return bound_knn_expr;
    } else {
        // Traditional array case
        embedding_data_type = parsed_knn_expr.embedding_data_type_;
        dimension = parsed_knn_expr.dimension_;

        // Create query embedding from array data
        EmbeddingT query_embedding((ptr_t)parsed_knn_expr.embedding_data_ptr_, false);

        if (parsed_knn_expr.ignore_index_ && !parsed_knn_expr.index_name_.empty()) {
            Status status = Status::SyntaxError(fmt::format("Force to use index {} conflicts with Ignore index flag.", parsed_knn_expr.index_name_));
            RecoverableError(std::move(status));
        }

        // create optional filter
        auto optional_filter = BuildSearchSubExprOptionalFilter(this, parsed_knn_expr.filter_expr_.get(), bind_context_ptr, depth);

        SharedPtr<KnnExpression> bound_knn_expr = MakeShared<KnnExpression>(embedding_data_type,
                                                                            dimension,
                                                                            parsed_knn_expr.distance_type_,
                                                                            std::move(query_embedding),
                                                                            std::move(arguments),
                                                                            parsed_knn_expr.topn_,
                                                                            parsed_knn_expr.opt_params_,
                                                                            std::move(optional_filter),
                                                                            parsed_knn_expr.index_name_,
                                                                            parsed_knn_expr.ignore_index_);
        return bound_knn_expr;
    }
}

SharedPtr<BaseExpression> ExpressionBinder::BuildMatchTextExpr(const MatchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    auto match_text = MakeShared<MatchExpression>(expr.fields_, expr.matching_text_, expr.options_text_, expr.index_names_);
    match_text->optional_filter_ = BuildSearchSubExprOptionalFilter(this, expr.filter_expr_.get(), bind_context_ptr, depth);
    return match_text;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    // Bind query column
    if (expr.column_expr_->type_ != ParsedExprType::kColumn) {
        UnrecoverableError("MatchTensor expression expect a column expression");
    }
    auto expr_ptr = BuildColExpr((ColumnExpr &)*expr.column_expr_.get(), bind_context_ptr, depth, false);
    auto column_data_type = expr_ptr->Type();
    TypeInfo *type_info = column_data_type.type_info().get();
    u32 tensor_column_basic_embedding_dim = 0;
    if ((column_data_type.type() == LogicalType::kTensor or column_data_type.type() == LogicalType::kTensorArray) and type_info != nullptr and
        type_info->type() == TypeInfoType::kEmbedding) {
        // valid search column
        EmbeddingInfo *embedding_info = (EmbeddingInfo *)type_info;
        tensor_column_basic_embedding_dim = embedding_info->Dimension();
        if (tensor_column_basic_embedding_dim == 0) {
            const auto error_info = "The tensor column basic embedding dimension should be greater than 0";
            UnrecoverableError(error_info);
        }
        // check if query embedding type and column embedding type can be matched
        if (const auto column_embedding_type = embedding_info->Type(), query_embedding_type = expr.embedding_data_type_;
            !EmbeddingEmbeddingQueryTypeValidated(column_embedding_type, query_embedding_type)) {
            Status status = Status::SyntaxError(fmt::format("Query embedding with data type: {} which doesn't match with column embedding type {}.",
                                                            EmbeddingInfo::EmbeddingDataTypeToString(query_embedding_type),
                                                            EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
            RecoverableError(std::move(status));
        }
    } else {
        const auto error_info = fmt::format("Expect the column search is an tensor / tensorarray column, but got: {}", column_data_type.ToString());
        RecoverableError(Status::SyntaxError(error_info));
    }
    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.emplace_back(std::move(expr_ptr));
    // Create query embedding
    EmbeddingT query_embedding(expr.query_tensor_data_ptr_.get(), false);
    // create optional filter
    auto optional_filter = BuildSearchSubExprOptionalFilter(this, expr.filter_expr_.get(), bind_context_ptr, depth);
    auto bound_match_tensor_expr = MakeShared<MatchTensorExpression>(std::move(arguments),
                                                                     expr.search_method_enum_,
                                                                     expr.embedding_data_type_,
                                                                     expr.dimension_,
                                                                     std::move(query_embedding),
                                                                     tensor_column_basic_embedding_dim,
                                                                     expr.options_text_,
                                                                     std::move(optional_filter),
                                                                     expr.index_name_,
                                                                     expr.ignore_index_);
    return bound_match_tensor_expr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildMatchSparseExpr(MatchSparseExpr &&expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    if (expr.column_expr_->type_ != ParsedExprType::kColumn) {
        UnrecoverableError("MatchSparse expression expect a column expression");
    }
    auto expr_ptr = BuildColExpr(static_cast<ColumnExpr &>(*expr.column_expr_), bind_context_ptr, depth, false);
    auto column_data_type = expr_ptr->Type();
    TypeInfo *type_info = column_data_type.type_info().get();
    if (column_data_type.type() != LogicalType::kSparse or type_info == nullptr or type_info->type() != TypeInfoType::kSparse) {
        const auto error_info = fmt::format("Expect the column search is a sparse column, but got: {}", column_data_type.ToString());
        RecoverableError(Status::SyntaxError(error_info));
    }

    Vector<SharedPtr<BaseExpression>> arguments;
    arguments.emplace_back(expr_ptr);

    SharedPtr<BaseExpression> query_expr = BuildExpression(*expr.query_sparse_expr_, bind_context_ptr, depth, root);

    if (expr.topn_ == 0) {
        RecoverableError(Status::InvalidParameterValue("topn", std::to_string(expr.topn_), "100"));
    }
    // create optional filter
    auto optional_filter = BuildSearchSubExprOptionalFilter(this, expr.filter_expr_.get(), bind_context_ptr, depth);
    auto bound_match_sparse_expr = MakeShared<MatchSparseExpression>(std::move(arguments),
                                                                     query_expr,
                                                                     expr.metric_type_,
                                                                     expr.query_n_,
                                                                     expr.topn_,
                                                                     std::move(expr.opt_params_),
                                                                     std::move(optional_filter),
                                                                     expr.index_name_,
                                                                     expr.ignore_index_);
    return bound_match_sparse_expr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool) {
    Vector<SharedPtr<BaseExpression>> match_exprs;
    Vector<SharedPtr<FusionExpression>> fusion_exprs;
    bool have_filter_in_subsearch = false;
    for (const ParsedExpr *match_expr : expr.match_exprs_) {
        switch (match_expr->type_) {
            case ParsedExprType::kKnn: {
                const auto &match_dense = *static_cast<const KnnExpr *>(match_expr);
                if (match_dense.filter_expr_) {
                    have_filter_in_subsearch = true;
                }
                match_exprs.push_back(BuildKnnExpr(match_dense, bind_context_ptr, depth, false));
                break;
            }
            case ParsedExprType::kMatch: {
                const auto &match_text = *static_cast<const MatchExpr *>(match_expr);
                if (match_text.filter_expr_) {
                    have_filter_in_subsearch = true;
                }
                match_exprs.push_back(BuildMatchTextExpr(match_text, bind_context_ptr, depth, false));
                break;
            }
            case ParsedExprType::kMatchTensor: {
                const auto &match_tensor = *static_cast<const MatchTensorExpr *>(match_expr);
                if (match_tensor.filter_expr_) {
                    have_filter_in_subsearch = true;
                }
                match_exprs.push_back(BuildMatchTensorExpr(match_tensor, bind_context_ptr, depth, false));
                break;
            }
            case ParsedExprType::kMatchSparse: {
                auto &match_sparse = const_cast<MatchSparseExpr &>(*static_cast<const MatchSparseExpr *>(match_expr));
                for (auto &param : match_sparse.opt_params_) {
                    if (param->param_name_ != "alpha" and param->param_name_ != "beta" and param->param_name_ != "tail" and
                        param->param_name_ != "threshold") {
                        RecoverableError(Status::SyntaxError(fmt::format("Unsupported optional parameter: {}", param->param_name_)));
                    }
                }
                if (match_sparse.filter_expr_) {
                    have_filter_in_subsearch = true;
                }
                match_exprs.push_back(BuildMatchSparseExpr(std::move(match_sparse), bind_context_ptr, depth, false));
                break;
            }
            default: {
                RecoverableError(Status::SyntaxError(fmt::format("Unsupported match expression: {}", match_expr->ToString())));
            }
        }
    }
    for (FusionExpr *fusion_expr : expr.fusion_exprs_) {
        auto output_expr = MakeShared<FusionExpression>(fusion_expr->method_, fusion_expr->options_);
        if (fusion_expr->match_tensor_expr_) {
            output_expr->match_tensor_expr_ =
                static_pointer_cast<MatchTensorExpression>(BuildMatchTensorExpr(*fusion_expr->match_tensor_expr_, bind_context_ptr, depth, false));
        }
        fusion_exprs.push_back(std::move(output_expr));
    }
    SharedPtr<SearchExpression> bound_search_expr = MakeShared<SearchExpression>(match_exprs, fusion_exprs);
    bound_search_expr->have_filter_in_subsearch_ = have_filter_in_subsearch;
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
            const auto error_info = "Not implement: Any";
            UnrecoverableError(error_info);
        }
    }

    const auto error_info = "Unreachable";
    UnrecoverableError(error_info);
    return nullptr;
}

SharedPtr<BaseExpression> ExpressionBinder::BuildUnnestExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    if (depth > 0) {
        RecoverableError(Status::SyntaxError("UNNEST() for correlated expression is not supported."));
        return nullptr;
    }

    if (expr.arguments_->size() != 1) {
        RecoverableError(Status::SyntaxError("UNNEST() requires a single argument."));
        return nullptr;
    }
    if (expr.distinct_) {
        RecoverableError(Status::SyntaxError("DISTINCT is not applicable to UNNEST()."));
        return nullptr;
    }
    String expr_name = expr.GetName();
    if (bind_context_ptr->group_index_by_name_.contains(expr_name)) {
        i64 group_index = bind_context_ptr->group_index_by_name_[expr_name];
        const SharedPtr<BaseExpression> &group_expr = bind_context_ptr->group_exprs_[group_index];

        SharedPtr<ColumnExpression> col_expr = ColumnExpression::Make(group_expr->Type(),
                                                                      bind_context_ptr->group_by_table_name_,
                                                                      bind_context_ptr->group_by_table_index_,
                                                                      expr_name,
                                                                      group_index,
                                                                      depth);
        return col_expr;
    }
    if (bind_context_ptr->unnest_index_by_name_.contains(expr_name)) {
        i64 unnest_index = bind_context_ptr->unnest_index_by_name_[expr_name];
        const SharedPtr<BaseExpression> &unnest_expr = bind_context_ptr->unnest_exprs_[unnest_index];

        SharedPtr<ColumnExpression> col_expr = ColumnExpression::Make(unnest_expr->Type(),
                                                                      bind_context_ptr->unnest_table_name_,
                                                                      bind_context_ptr->unnest_table_index_,
                                                                      expr_name,
                                                                      unnest_index,
                                                                      depth);
        return col_expr;
    }

    SharedPtr<BaseExpression> child_expr = ExpressionBinder::BuildExpression(*(*expr.arguments_)[0], bind_context_ptr, depth, root);
    DataType return_type(LogicalType::kInvalid);
    switch (child_expr->Type().type()) {
        case LogicalType::kArray: {
            auto *array_info = static_cast<ArrayInfo *>(child_expr->Type().type_info().get());
            return_type = DataType(array_info->ElemType().type());
            break;
        }
        default: {
            RecoverableError(Status::SyntaxError("UNNEST() requires an array argument."));
            return nullptr;
        }
    }

    SharedPtr<BaseExpression> unnest_expr = MakeShared<UnnestExpression>(child_expr);

    u64 table_index = bind_context_ptr->GenerateTableIndex();
    String table_name = fmt::format("unnest{:d}", table_index);
    bind_context_ptr->unnest_table_index_ = table_index;
    bind_context_ptr->unnest_table_name_ = table_name;

    bind_context_ptr->unnest_index_by_name_[expr_name] = bind_context_ptr->unnest_exprs_.size();
    bind_context_ptr->unnest_exprs_.emplace_back(unnest_expr);

    SharedPtr<ColumnExpression> col_expr = ColumnExpression::Make(return_type, table_name, table_index, expr_name, 0, depth);
    return col_expr;
}

Optional<SharedPtr<BaseExpression>> ExpressionBinder::TryBuildSpecialFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth) {
    auto [special_function_ptr, status] = NewCatalog::GetSpecialFunctionByNameNoExcept(query_context_->storage()->new_catalog(), expr.func_name_);
    if (status.ok()) {
        switch (special_function_ptr->special_type()) {
            case SpecialType::kDistanceFactors:
            case SpecialType::kDistance: {
                if (!bind_context_ptr->allow_distance) {
                    RecoverableError(Status::SyntaxError(
                        "DISTANCE() / DISTANCE_FACTORS() needs to be allowed only when there is only MATCH VECTOR with distance metrics, like L2"));
                }
                break;
            }
            case SpecialType::kSimilarityFactors:
            case SpecialType::kSimilarity: {
                if (!bind_context_ptr->allow_similarity) {
                    RecoverableError(Status::SyntaxError("SIMILARITY() / SIMILARITY_FACTORS() needs to be allowed only when there is only MATCH "
                                                         "VECTOR with similarity metrics, like Inner product"));
                }
                break;
            }
            case SpecialType::kScoreFactors:
            case SpecialType::kScore: {
                if (!bind_context_ptr->allow_score) {
                    RecoverableError(Status::SyntaxError("SCORE() / SCORE_FACTORS() requires Fusion or MATCH TEXT or MATCH TENSOR"));
                }
                break;
            }
            case SpecialType::kRowID:
            case SpecialType::kCreateTs:
            case SpecialType::kDeleteTs: {
                break;
            }
            case SpecialType::kFilterFullText: {
                return FilterFulltextExpression::BuildFilterFulltextExpression(expr);
            }
        }

        String &table_name = bind_context_ptr->table_names_[0];
        String column_name = special_function_ptr->name();

        TableInfo *table_info = bind_context_ptr->binding_by_name_[table_name]->table_info_.get();
        switch (special_function_ptr->special_type()) {
            case SpecialType::kCreateTs: {
                return ColumnExpression::Make(special_function_ptr->data_type(),
                                              table_name,
                                              bind_context_ptr->table_name2table_index_[table_name],
                                              column_name,
                                              COLUMN_IDENTIFIER_CREATE,
                                              depth,
                                              special_function_ptr->special_type());
            }
            case SpecialType::kDeleteTs: {
                return ColumnExpression::Make(special_function_ptr->data_type(),
                                              table_name,
                                              bind_context_ptr->table_name2table_index_[table_name],
                                              column_name,
                                              COLUMN_IDENTIFIER_DELETE,
                                              depth,
                                              special_function_ptr->special_type());
            }
            default: {
                return ColumnExpression::Make(special_function_ptr->data_type(),
                                              table_name,
                                              bind_context_ptr->table_name2table_index_[table_name],
                                              column_name,
                                              table_info->column_count_ + special_function_ptr->extra_idx(),
                                              depth,
                                              special_function_ptr->special_type());
            }
        }
    } else {
        return None;
    }
}

bool ExpressionBinder::IsUnnestedFunction(const String &function_name) { return function_name == String("unnest"); }

template <typename T, typename U>
void FillConcatenatedTensorData(T *output_ptr, const Vector<U> &data_array, const u32 expect_dim) {
    if (data_array.size() != expect_dim) {
        const auto error_info = fmt::format("Mismatch in tensor member dimension, expect: {}, but got: {}", expect_dim, data_array.size());
        RecoverableError(Status::SyntaxError(error_info));
    }
    for (u32 i = 0; i < expect_dim; ++i) {
        output_ptr[i] = data_array[i];
    }
}

template <typename T>
ptr_t GetConcatenatedTensorDataFromSubArray(const Vector<SharedPtr<ConstantExpr>> &sub_array_array,
                                            const u32 tensor_column_basic_embedding_dim,
                                            u32 &query_total_dimension) {
    static_assert(!std::is_same_v<T, bool>);
    // expect children to be embedding of dimension tensor_column_basic_embedding_dim
    query_total_dimension = sub_array_array.size() * tensor_column_basic_embedding_dim;
    auto output_data = MakeUniqueForOverwrite<T[]>(query_total_dimension);
    for (u32 i = 0; i < sub_array_array.size(); ++i) {
        switch (sub_array_array[i]->literal_type_) {
            case LiteralType::kIntegerArray: {
                FillConcatenatedTensorData(output_data.get() + i * tensor_column_basic_embedding_dim,
                                           sub_array_array[i]->long_array_,
                                           tensor_column_basic_embedding_dim);
                break;
            }
            case LiteralType::kDoubleArray: {
                FillConcatenatedTensorData(output_data.get() + i * tensor_column_basic_embedding_dim,
                                           sub_array_array[i]->double_array_,
                                           tensor_column_basic_embedding_dim);
                break;
            }
            default: {
                const auto error_info = "Tensor subarray type should be IntegerArray or DoubleArray.";
                RecoverableError(Status::SyntaxError(error_info));
                break;
            }
        }
    }
    auto output_ptr = output_data.release();
    return reinterpret_cast<ptr_t>(output_ptr);
}

template <typename T, typename U>
void FillConcatenatedTensorDataBit(T *output_ptr, const Vector<U> &data_array, const u32 expect_dim) {
    static_assert(std::is_same_v<T, u8>);
    if (data_array.size() != expect_dim) {
        const auto error_info = fmt::format("Mismatch in tensor member dimension, expect: {}, but got: {}", expect_dim, data_array.size());
        RecoverableError(Status::SyntaxError(error_info));
    }
    for (u32 i = 0; i < expect_dim; ++i) {
        if (data_array[i]) {
            output_ptr[i / 8] |= (1u << (i % 8));
        }
    }
}

template <>
ptr_t GetConcatenatedTensorDataFromSubArray<bool>(const Vector<SharedPtr<ConstantExpr>> &sub_array_array,
                                                  const u32 tensor_column_basic_embedding_dim,
                                                  u32 &query_total_dimension) {
    // expect children to be embedding of dimension tensor_column_basic_embedding_dim
    query_total_dimension = sub_array_array.size() * tensor_column_basic_embedding_dim;
    auto output_data = MakeUnique<u8[]>(query_total_dimension / 8);
    const u32 basic_u8_dim = tensor_column_basic_embedding_dim / 8;
    for (u32 i = 0; i < sub_array_array.size(); ++i) {
        switch (sub_array_array[i]->literal_type_) {
            case LiteralType::kIntegerArray: {
                FillConcatenatedTensorDataBit(output_data.get() + i * basic_u8_dim,
                                              sub_array_array[i]->long_array_,
                                              tensor_column_basic_embedding_dim);
                break;
            }
            case LiteralType::kDoubleArray: {
                FillConcatenatedTensorDataBit(output_data.get() + i * basic_u8_dim,
                                              sub_array_array[i]->double_array_,
                                              tensor_column_basic_embedding_dim);
                break;
            }
            default: {
                const auto error_info = "Tensor subarray type should be IntegerArray or DoubleArray.";
                RecoverableError(Status::SyntaxError(error_info));
                break;
            }
        }
    }
    auto output_ptr = output_data.release();
    return reinterpret_cast<ptr_t>(output_ptr);
}

template <typename T, typename U>
ptr_t GetConcatenatedTensorData(const Vector<U> &data_array, const u32 tensor_column_basic_embedding_dim, u32 &query_total_dimension) {
    query_total_dimension = data_array.size();
    if (query_total_dimension == 0 or query_total_dimension % tensor_column_basic_embedding_dim != 0) {
        const auto error_info = fmt::format("Query embedding with dimension: {} which doesn't match with tensor basic dimension {}",
                                            query_total_dimension,
                                            tensor_column_basic_embedding_dim);
        RecoverableError(Status::SyntaxError(error_info));
    }
    if constexpr (std::is_same_v<T, bool>) {
        auto *embedding_data_ptr = new u8[query_total_dimension / 8]();
        for (u32 i = 0; i < query_total_dimension; ++i) {
            if (data_array[i]) {
                embedding_data_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        return reinterpret_cast<ptr_t>(embedding_data_ptr);
    } else {
        T *embedding_data_ptr = new T[query_total_dimension];
        for (u32 i = 0; i < query_total_dimension; ++i) {
            embedding_data_ptr[i] = data_array[i];
        }
        return reinterpret_cast<ptr_t>(embedding_data_ptr);
    }
}

template <typename T>
ptr_t GetConcatenatedTensorData(const ConstantExpr *tensor_expr_, const u32 tensor_column_basic_embedding_dim, u32 &query_total_dimension) {
    if constexpr (std::is_same_v<T, bool>) {
        if (tensor_column_basic_embedding_dim % 8 != 0) {
            String error_message = "The tensor column basic embedding dimension should be multiple of 8";
            UnrecoverableError(error_message);
        }
    }
    switch (tensor_expr_->literal_type_) {
        case LiteralType::kIntegerArray: {
            return GetConcatenatedTensorData<T>(tensor_expr_->long_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        case LiteralType::kDoubleArray: {
            return GetConcatenatedTensorData<T>(tensor_expr_->double_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        case LiteralType::kSubArrayArray: {
            return GetConcatenatedTensorDataFromSubArray<T>(tensor_expr_->sub_array_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        default: {
            String error_message = "Unexpected case!";
            UnrecoverableError(error_message);
            return nullptr;
        }
    }
}

} // namespace infinity

//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

#include <cmath>
#include <memory>
#include <string>

module secondary_index_scan_middle_expression;

import stl;
import base_expression;
import expression_type;
import expression_state;
import expression_evaluator;
import value_expression;
import column_expression;
import column_vector;
import function_expression;
import infinity_exception;
import value;
import third_party;
import secondary_index_data;
import internal_types;
import data_type;
import logical_type;

namespace infinity {

class FilterEvaluatorBuilder {
private:
    Vector<FilterEvaluatorElem> result_;
    SharedPtr<BaseExpression> &index_filter_qualified_;

public:
    FilterEvaluatorBuilder(SharedPtr<BaseExpression> &index_filter_qualified) : index_filter_qualified_(index_filter_qualified) {}

    [[nodiscard]] bool Solve() { return BuildFilterEvaluator(index_filter_qualified_); }

    [[nodiscard]] auto &GetResult() { return result_; }

private:
    inline bool BuildFilterEvaluator(SharedPtr<BaseExpression> &expression) {
        // expression: 1. basic component "[cast] x compare value_expr" 2. combine component with "and/or/not"
        if (expression->type() == ExpressionType::kFunction) {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            auto const &function_name = function_expression->ScalarFunctionName();
            if (function_name == "AND" or function_name == "OR") {
                for (auto &arg : function_expression->arguments()) {
                    if (not BuildFilterEvaluator(arg)) {
                        UnrecoverableError(fmt::format("BuildFilterEvaluator(): subexpression error in \"{}\".", expression->Name()));
                        return false;
                    }
                }
                auto combine_type = GetBooleanCombineType(function_name);
                if (combine_type != BooleanCombineType::kInvalid) [[likely]] {
                    result_.emplace_back(combine_type);
                    return true;
                } else {
                    UnrecoverableError(fmt::format("BuildFilterEvaluator(): function name error in: {}.", expression->Name()));
                    return false;
                }
            } else {
                // "[cast] x compare value_expr"
                static constexpr std::array<const char *, 5> compare_functions = {"<", ">", "<=", ">=", "="};
                static constexpr std::array<FilterCompareType, 5> corresponding_compare_types = {FilterCompareType::kLess,
                                                                                                 FilterCompareType::kGreater,
                                                                                                 FilterCompareType::kLessEqual,
                                                                                                 FilterCompareType::kGreaterEqual,
                                                                                                 FilterCompareType::kEqual};
                if (auto it = std::find(compare_functions.begin(), compare_functions.end(), function_name); it != compare_functions.end()) {
                    auto compare_type = corresponding_compare_types[std::distance(compare_functions.begin(), it)];
                    auto &left = function_expression->arguments()[0];  // [cast] x
                    auto &right = function_expression->arguments()[1]; // value-expression
                    // 2. right
                    auto right_val = CalcValueResult(right);
                    // 1. left, maybe with cast
                    auto [column_id, final_val, final_compare_type] = UnwindCast(left, std::move(right_val), compare_type);
                    if (final_compare_type != FilterCompareType::kInvalid) [[likely]] {
                        result_.emplace_back(column_id);
                        result_.emplace_back(final_val);
                        // make sure that result_ only contain FilterCompareType of kEqual, kLessEqual, kGreaterEqual, kAlwaysFalse, kAlwaysTrue
                        switch (final_compare_type) {
                            case FilterCompareType::kEqual:
                            case FilterCompareType::kLessEqual:
                            case FilterCompareType::kGreaterEqual:
                            case FilterCompareType::kAlwaysFalse:
                            case FilterCompareType::kAlwaysTrue: {
                                result_.emplace_back(final_compare_type);
                                break;
                            }
                            default: {
                                UnrecoverableError(fmt::format("BuildFilterEvaluator(): compare type error in: {}.", expression->Name()));
                                return false;
                            }
                        }
                        return true;
                    } else {
                        UnrecoverableError(fmt::format("BuildFilterEvaluator(): unwind cast error in: {}.", expression->Name()));
                        return false;
                    }
                } else {
                    UnrecoverableError(fmt::format("BuildFilterEvaluator(): function name error: {}.", expression->Name()));
                    return false;
                }
            }
        } else {
            UnrecoverableError(fmt::format("BuildFilterEvaluator(): expression type error: {}.", expression->Name()));
            return false;
        }
    }
    inline Value CalcValueResult(SharedPtr<BaseExpression> &expression) {
        if (expression->type() == ExpressionType::kValue) {
            // does not need ExpressionEvaluator
            auto value_expression = std::static_pointer_cast<ValueExpression>(expression);
            return value_expression->GetValue();
        } else {
            auto expression_state = ExpressionState::CreateState(expression);
            auto result_vector = MakeShared<ColumnVector>(MakeShared<DataType>(expression->Type()));
            result_vector->Initialize();
            ExpressionEvaluator expr_evaluator; // does not need input_data_block_
            expr_evaluator.Execute(expression, expression_state, result_vector);
            return result_vector->GetValue(0);
        }
    }
    inline Tuple<ColumnID, Value, FilterCompareType>
    UnwindCast(SharedPtr<BaseExpression> &cast_expr, Value &&right_val, FilterCompareType compare_type) {
        SimplifyCompareTypeAndValue(right_val, compare_type);
        if (compare_type == FilterCompareType::kAlwaysFalse or compare_type == FilterCompareType::kAlwaysTrue) {
            return {0, Value::MakeNull(), compare_type};
        }
        // now compare_type is one of {kLessEqual, kGreaterEqual, kEqual}
        if (cast_expr->type() == ExpressionType::kColumn) {
            auto column_expression = std::static_pointer_cast<ColumnExpression>(cast_expr);
            ColumnID column_id = column_expression->binding().column_idx;
            return std::make_tuple(column_id, std::move(right_val), compare_type);
        } else if (cast_expr->type() == ExpressionType::kCast) {
            auto &source_expr = cast_expr->arguments()[0];
            auto source_type = source_expr->Type();
            auto source_logical_type = source_type.type();
            auto target_type = cast_expr->Type();
            auto target_logical_type = target_type.type();
            if (target_logical_type != right_val.type().type()) {
                UnrecoverableError(fmt::format("UnwindCast(): type mismatch: {} vs {}.", target_type.ToString(), right_val.type().ToString()));
                return {0, Value::MakeNull(), FilterCompareType::kInvalid};
            }
            // case 0. keep the original type?
            if (source_logical_type == target_logical_type) {
                return UnwindCast(source_expr, std::move(right_val), compare_type);
            }
            // case 1. cast "smaller ints" to BigIntT
            if (target_logical_type == LogicalType::kBigInt) {
                auto right_val_bigint = right_val.GetValue<BigIntT>();
                switch (source_logical_type) {
                    case LogicalType::kTinyInt: {
                        if (IntegralContinueUnwind<TinyIntT>(right_val_bigint, compare_type)) {
                            right_val = Value::MakeTinyInt(static_cast<TinyIntT>(right_val_bigint));
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kSmallInt: {
                        if (IntegralContinueUnwind<SmallIntT>(right_val_bigint, compare_type)) {
                            right_val = Value::MakeSmallInt(static_cast<SmallIntT>(right_val_bigint));
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kInteger: {
                        if (IntegralContinueUnwind<IntegerT>(right_val_bigint, compare_type)) {
                            right_val = Value::MakeInt(static_cast<IntegerT>(right_val_bigint));
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    default: {
                        UnrecoverableError(fmt::format("UnwindCast(): error in: {}.", cast_expr->Name()));
                        return {0, Value::MakeNull(), FilterCompareType::kInvalid};
                    }
                }
            }
            // case 2. cast "ints" or "floats" to DoubleT
            if (target_logical_type == LogicalType::kDouble) {
                auto right_val_double = right_val.GetValue<DoubleT>();
                switch (source_logical_type) {
                    case LogicalType::kTinyInt: {
                        TinyIntT prev;
                        if (IntegralContinueUnwind<TinyIntT>(right_val_double, compare_type, prev)) {
                            right_val = Value::MakeTinyInt(prev);
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kSmallInt: {
                        SmallIntT prev;
                        if (IntegralContinueUnwind<SmallIntT>(right_val_double, compare_type, prev)) {
                            right_val = Value::MakeSmallInt(prev);
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kInteger: {
                        IntegerT prev;
                        if (IntegralContinueUnwind<IntegerT>(right_val_double, compare_type, prev)) {
                            right_val = Value::MakeInt(prev);
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kBigInt: {
                        BigIntT prev;
                        if (IntegralContinueUnwind<BigIntT>(right_val_double, compare_type, prev)) {
                            right_val = Value::MakeBigInt(prev);
                            return UnwindCast(source_expr, std::move(right_val), compare_type);
                        } else {
                            return {0, Value::MakeNull(), compare_type};
                        }
                    }
                    case LogicalType::kFloat: {
                        // TODO: cast double to float? check if this is correct
                        right_val = Value::MakeFloat(static_cast<FloatT>(right_val_double));
                        return UnwindCast(source_expr, std::move(right_val), compare_type);
                    }
                    default: {
                        UnrecoverableError(fmt::format("UnwindCast(): error in: {}.", cast_expr->Name()));
                        return {0, Value::MakeNull(), FilterCompareType::kInvalid};
                    }
                }
            }
            UnrecoverableError(fmt::format("UnwindCast(): error in: {}.", cast_expr->Name()));
            return {0, Value::MakeNull(), FilterCompareType::kInvalid};
        } else {
            UnrecoverableError(fmt::format("UnwindCast(): expression type error: {}.", cast_expr->Name()));
            return {0, Value::MakeNull(), FilterCompareType::kInvalid};
        }
    }
    template <std::integral FromT>
    inline bool IntegralContinueUnwind(BigIntT right_val_bigint, FilterCompareType &compare_type) {
        switch (compare_type) {
            case FilterCompareType::kLessEqual: {
                if (right_val_bigint < std::numeric_limits<FromT>::lowest()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                    return false;
                } else if (right_val_bigint >= std::numeric_limits<FromT>::max()) {
                    compare_type = FilterCompareType::kAlwaysTrue;
                    return false;
                } else {
                    return true;
                }
            }
            case FilterCompareType::kGreaterEqual: {
                if (right_val_bigint > std::numeric_limits<FromT>::max()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                    return false;
                } else if (right_val_bigint <= std::numeric_limits<FromT>::lowest()) {
                    compare_type = FilterCompareType::kAlwaysTrue;
                    return false;
                } else {
                    return true;
                }
            }
            case FilterCompareType::kEqual: {
                if (right_val_bigint < std::numeric_limits<FromT>::lowest() or right_val_bigint > std::numeric_limits<FromT>::max()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                    return false;
                } else {
                    return true;
                }
            }
            default: {
                UnrecoverableError("IntegralContinueUnwind(): compare type error.");
                compare_type = FilterCompareType::kInvalid;
                return false;
            }
        }
    }
    template <std::integral FromT>
    inline bool IntegralContinueUnwind(DoubleT right_val_double, FilterCompareType &compare_type, FromT &prev) {
        constexpr auto lowest_val = static_cast<DoubleT>(std::numeric_limits<FromT>::lowest());
        constexpr auto max_val = static_cast<DoubleT>(std::numeric_limits<FromT>::max());
        switch (compare_type) {
            case FilterCompareType::kLessEqual: {
                right_val_double = std::floor(right_val_double);
                if (right_val_double < lowest_val) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                    return false;
                } else if (right_val_double >= max_val) {
                    compare_type = FilterCompareType::kAlwaysTrue;
                    return false;
                } else {
                    prev = static_cast<FromT>(right_val_double);
                    return true;
                }
            }
            case FilterCompareType::kGreaterEqual: {
                right_val_double = std::ceil(right_val_double);
                if (right_val_double > max_val) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                    return false;
                } else if (right_val_double <= lowest_val) {
                    compare_type = FilterCompareType::kAlwaysTrue;
                    return false;
                } else {
                    prev = static_cast<FromT>(right_val_double);
                    return true;
                }
            }
            case FilterCompareType::kEqual: {
                // TODO: does not allow expression like "(cast to double)(c1 (int type)) == 3.0" ?
                compare_type = FilterCompareType::kInvalid;
                UnrecoverableError("IntegralContinueUnwind(): should not cast int type column to double in equality comparison.");
                return false;
            }
            default: {
                UnrecoverableError("IntegralContinueUnwind(): compare type error.");
                compare_type = FilterCompareType::kInvalid;
                return false;
            }
        }
    }
    inline void SimplifyCompareTypeAndValue(Value &right_val, FilterCompareType &compare_type) {
        switch (compare_type) {
            case FilterCompareType::kLess: {
                RewriteCompare(right_val, compare_type);
                break;
            }
            case FilterCompareType::kGreater: {
                RewriteCompare(right_val, compare_type);
                break;
            }
            case FilterCompareType::kLessEqual:
            case FilterCompareType::kGreaterEqual:
            case FilterCompareType::kEqual:
            case FilterCompareType::kAlwaysFalse:
            case FilterCompareType::kAlwaysTrue: {
                // do nothing
                break;
            }
            case FilterCompareType::kInvalid: {
                UnrecoverableError("SimplifyCompareTypeAndValue(): compare type error.");
            }
        }
    }
    inline void RewriteCompare(Value &right_val, FilterCompareType &compare_type) {
        switch (right_val.type().type()) {
            case LogicalType::kTinyInt: {
                auto right_val_tinyint = right_val.GetValue<TinyIntT>();
                RewriteCompareT(right_val_tinyint, compare_type);
                right_val = Value::MakeTinyInt(right_val_tinyint);
                break;
            }
            case LogicalType::kSmallInt: {
                auto right_val_smallint = right_val.GetValue<SmallIntT>();
                RewriteCompareT(right_val_smallint, compare_type);
                right_val = Value::MakeSmallInt(right_val_smallint);
                break;
            }
            case LogicalType::kInteger: {
                auto right_val_integer = right_val.GetValue<IntegerT>();
                RewriteCompareT(right_val_integer, compare_type);
                right_val = Value::MakeInt(right_val_integer);
                break;
            }
            case LogicalType::kBigInt: {
                auto right_val_bigint = right_val.GetValue<BigIntT>();
                RewriteCompareT(right_val_bigint, compare_type);
                right_val = Value::MakeBigInt(right_val_bigint);
                break;
            }
            case LogicalType::kFloat: {
                auto right_val_float = right_val.GetValue<FloatT>();
                RewriteCompareT(right_val_float, compare_type);
                right_val = Value::MakeFloat(right_val_float);
                break;
            }
            case LogicalType::kDouble: {
                auto right_val_double = right_val.GetValue<DoubleT>();
                RewriteCompareT(right_val_double, compare_type);
                right_val = Value::MakeDouble(right_val_double);
                break;
            }
            case LogicalType::kDate: {
                auto right_val_date = right_val.GetValue<DateT>();
                RewriteCompareT(right_val_date, compare_type);
                right_val = Value::MakeDate(right_val_date);
                break;
            }
            case LogicalType::kTime: {
                auto right_val_time = right_val.GetValue<TimeT>();
                RewriteCompareT(right_val_time, compare_type);
                right_val = Value::MakeTime(right_val_time);
                break;
            }
            case LogicalType::kDateTime: {
                auto right_val_datetime = right_val.GetValue<DateTimeT>();
                RewriteCompareT(right_val_datetime, compare_type);
                right_val = Value::MakeDateTime(right_val_datetime);
                break;
            }
            case LogicalType::kTimestamp: {
                auto right_val_timestamp = right_val.GetValue<TimestampT>();
                RewriteCompareT(right_val_timestamp, compare_type);
                right_val = Value::MakeTimestamp(right_val_timestamp);
                break;
            }
            default: {
                UnrecoverableError(fmt::format("FindPrev(): type error: {}.", right_val.type().ToString()));
            }
        }
    }
    template <typename T>
        requires std::integral<T> or std::floating_point<T>
    inline void RewriteCompareT(T &right_val, FilterCompareType &compare_type) {
        // used when try to convert "<" into "<=", or ">" into ">="
        switch (compare_type) {
            case FilterCompareType::kLess: {
                if (right_val <= std::numeric_limits<T>::lowest()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                } else {
                    FindPrevT(right_val);
                    compare_type = FilterCompareType::kLessEqual;
                }
                break;
            }
            case FilterCompareType::kGreater: {
                if (right_val >= std::numeric_limits<T>::max()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                } else {
                    FindNextT(right_val);
                    compare_type = FilterCompareType::kGreaterEqual;
                }
                break;
            }
            default: {
                UnrecoverableError("RewriteCompareT(): compare type error.");
            }
        }
    }

    template <typename TIME>
        requires IsAnyOf<TIME, DateT, TimeT, DateTimeT, TimestampT>
    inline void RewriteCompareT(TIME &right_val_time, FilterCompareType &compare_type) {
        // used when try to convert "<" into "<=", or ">" into ">="
        using T = ConvertToOrderedType<TIME>;
        T right_val = ConvertToOrderedKeyValue(right_val_time);
        switch (compare_type) {
            case FilterCompareType::kLess: {
                if (right_val <= std::numeric_limits<T>::lowest()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                } else {
                    FindPrevT(right_val);
                    compare_type = FilterCompareType::kLessEqual;
                }
                break;
            }
            case FilterCompareType::kGreater: {
                if (right_val >= std::numeric_limits<T>::max()) {
                    compare_type = FilterCompareType::kAlwaysFalse;
                } else {
                    FindNextT(right_val);
                    compare_type = FilterCompareType::kGreaterEqual;
                }
                break;
            }
            default: {
                UnrecoverableError("RewriteCompareT(): compare type error.");
            }
        }
        // recover original TIME type
        // constructed TIME value may be out of range ,but can be safely used in comparison
        std::construct_at(&right_val_time, right_val);
    }

    template <typename T>
    inline void FindPrevT(T &right_val) {
        static_assert(false, "unsupported type.");
    }
    template <std::integral T>
    inline void FindPrevT(T &right_val) {
        --right_val;
    }
    template <std::floating_point T>
    inline void FindPrevT(T &right_val) {
        right_val = std::nextafter(right_val, std::numeric_limits<T>::lowest());
    }
    template <typename T>
    inline void FindNextT(T &right_val) {
        static_assert(false, "unsupported type.");
    }
    template <std::integral T>
    inline void FindNextT(T &right_val) {
        ++right_val;
    }
    template <std::floating_point T>
    inline void FindNextT(T &right_val) {
        right_val = std::nextafter(right_val, std::numeric_limits<T>::max());
    }
    inline BooleanCombineType GetBooleanCombineType(const String &function_name) {
        if (function_name == "AND") {
            return BooleanCombineType::kAnd;
        } else if (function_name == "OR") {
            return BooleanCombineType::kOr;
        } else {
            UnrecoverableError(fmt::format("GetBooleanCombineType(): function name error: {}.", function_name));
            return BooleanCombineType::kInvalid;
        }
    }
};

Vector<FilterEvaluatorElem> BuildSecondaryIndexScanMiddleCommand(SharedPtr<BaseExpression> &index_filter_qualified_) {
    // step 1. middle representation
    // Use Reverse Polish notation to evaluate the filter
    // For example, the filter "a >= 1 AND a <= 2" will be converted to "a 1 >= a 2 <= AND"
    // filter_evaluator_ only contain FilterCompareType of kEqual, kLessEqual, kGreaterEqual, kAlwaysFalse, kAlwaysTrue
    // filter_evaluator_ only contain BooleanCombineType of kAnd, kOr
    Vector<FilterEvaluatorElem> filter_evaluator;
    FilterEvaluatorBuilder filter_builder(index_filter_qualified_);
    if (filter_builder.Solve()) {
        filter_evaluator = std::move(filter_builder.GetResult());
    } else {
        UnrecoverableError("PhysicalIndexScan::Init(): filter solver error.");
    }
    return filter_evaluator;
}

} // namespace infinity
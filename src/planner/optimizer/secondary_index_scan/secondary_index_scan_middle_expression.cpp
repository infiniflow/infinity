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

#include <memory>
#include <string>

module secondary_index_scan_middle_expression;

import stl;
import base_expression;
import expression_type;
import value_expression;
import column_expression;
import function_expression;
import infinity_exception;
import value;
import third_party;
import secondary_index_data;
import internal_types;
import data_type;
import logical_type;
import filter_expression_push_down_helper;
import logger;

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
                        String error_message = fmt::format("BuildFilterEvaluator(): subexpression error in \"{}\".", expression->Name());
                        UnrecoverableError(error_message);
                        return false;
                    }
                }
                auto combine_type = GetBooleanCombineType(function_name);
                if (combine_type != BooleanCombineType::kInvalid) [[likely]] {
                    result_.emplace_back(combine_type);
                    return true;
                } else {
                    String error_message = fmt::format("BuildFilterEvaluator(): function name error in: {}.", expression->Name());
                    UnrecoverableError(error_message);
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
                    auto right_val = FilterExpressionPushDownHelper::CalcValueResult(right);
                    // 1. left, maybe with cast
                    auto [column_id, final_val, final_compare_type] =
                        FilterExpressionPushDownHelper::UnwindCast(left, std::move(right_val), compare_type);
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
                                String error_message = fmt::format("BuildFilterEvaluator(): compare type error in: {}.", expression->Name());
                                UnrecoverableError(error_message);
                                return false;
                            }
                        }
                        return true;
                    } else {
                        String error_message = fmt::format("BuildFilterEvaluator(): unwind cast error in: {}.", expression->Name());
                        UnrecoverableError(error_message);
                        return false;
                    }
                } else {
                    String error_message = fmt::format("BuildFilterEvaluator(): function name error: {}.", expression->Name());
                    UnrecoverableError(error_message);
                    return false;
                }
            }
        } else {
            String error_message = fmt::format("BuildFilterEvaluator(): expression type error: {}.", expression->Name());
            UnrecoverableError(error_message);
            return false;
        }
    }

    inline static BooleanCombineType GetBooleanCombineType(const String &function_name) {
        if (function_name == "AND") {
            return BooleanCombineType::kAnd;
        } else if (function_name == "OR") {
            return BooleanCombineType::kOr;
        } else {
            String error_message = fmt::format("GetBooleanCombineType(): function name error: {}.", function_name);
            UnrecoverableError(error_message);
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
        String error_message = "PhysicalIndexScan::Init(): filter solver error.";
        UnrecoverableError(error_message);
    }
    return filter_evaluator;
}

} // namespace infinity
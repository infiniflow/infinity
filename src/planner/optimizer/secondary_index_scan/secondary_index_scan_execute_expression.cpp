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

#include <concepts>
#include <vector>

module secondary_index_scan_execute_expression;

import stl;
import value;
import base_expression;
import infinity_exception;
import secondary_index_data;
import secondary_index_scan_middle_expression;
import third_party;
import logical_type;
import status;

namespace infinity {

template <typename T1, typename T2>
concept IncompatibleFilterIntervalRangePair = (not std::same_as<T1, T2>) or std::same_as<T1, std::monostate>;

class FilterCommandBuilder {
private:
    // filter_evaluator_ only contain FilterCompareType of kEqual, kLessEqual, kGreaterEqual, kAlwaysFalse, kAlwaysTrue
    // filter_evaluator_ only contain BooleanCombineType of kAnd, kOr
    const Vector<FilterEvaluatorElem> &filter_evaluator_;
    Vector<FilterExecuteElem> result_;

    inline void SetResultIntervalValue(FilterExecuteSingleRange &result, const Value &value, FilterCompareType compare_type) {
        switch (value.type().type()) {
            case LogicalType::kTinyInt: {
                result.SetIntervalRange<TinyIntT>(value, compare_type);
                break;
            }
            case LogicalType::kSmallInt: {
                result.SetIntervalRange<SmallIntT>(value, compare_type);
                break;
            }
            case LogicalType::kInteger: {
                result.SetIntervalRange<IntegerT>(value, compare_type);
                break;
            }
            case LogicalType::kBigInt: {
                result.SetIntervalRange<BigIntT>(value, compare_type);
                break;
            }
            case LogicalType::kFloat: {
                result.SetIntervalRange<FloatT>(value, compare_type);
                break;
            }
            case LogicalType::kDouble: {
                result.SetIntervalRange<DoubleT>(value, compare_type);
                break;
            }
            case LogicalType::kDate: {
                result.SetIntervalRange<DateT>(value, compare_type);
                break;
            }
            case LogicalType::kTime: {
                result.SetIntervalRange<TimeT>(value, compare_type);
                break;
            }
            case LogicalType::kDateTime: {
                result.SetIntervalRange<DateTimeT>(value, compare_type);
                break;
            }
            case LogicalType::kTimestamp: {
                result.SetIntervalRange<TimestampT>(value, compare_type);
                break;
            }
            case LogicalType::kVarchar: {
                if (compare_type == FilterCompareType::kEqual) {
                    result.SetIntervalRange<VarcharT>(value, compare_type);
                } else {
                    String error_message = "SaveToResult(): VarcharT only support kEqual compare type.";
                    RecoverableError(Status::SyntaxError(error_message));
                }
                break;
            }
            default: {
                String error_message = fmt::format("SaveToResult(): type error: {}.", value.type().ToString());
                UnrecoverableError(error_message);
            }
        }
    }

    inline void SaveToResult(ColumnID column_id, const Value &value, FilterCompareType compare_type) {
        // step 1. check compare_type
        switch (compare_type) {
            case FilterCompareType::kAlwaysFalse: {
                result_.emplace_back(std::in_place_index<1>, column_id, FilterRangeType::kEmpty);
                return;
            }
            case FilterCompareType::kEqual:
            case FilterCompareType::kLessEqual:
            case FilterCompareType::kGreaterEqual:
            case FilterCompareType::kAlwaysTrue: {
                // step 2. set interval range for kEqual, kLessEqual, kGreaterEqual and kAlwaysTrue
                auto &result_variant = result_.emplace_back(std::in_place_index<1>, column_id, FilterRangeType::kInterval);
                auto &result = std::get<FilterExecuteSingleRange>(result_variant);
                SetResultIntervalValue(result, value, compare_type);
                return;
            }
            default: {
                String error_message = "SaveToResult(): compare type error.";
                UnrecoverableError(error_message);
                return;
            }
        }
    }

    // try to compact adjacent elements
    // case 1. one kEmpty range and another range
    // case 2. two intervals of same ColumnID
    inline bool TryCompactNearbyFilterAnd() {
        if (result_.size() < 2) {
            String error_message = "FilterCommandBuilder::TryCompactNearbyFilter(): result size < 2.";
            UnrecoverableError(error_message);
            return false;
        }
        auto &last_elem_variant = result_[result_.size() - 1];
        auto &second_last_elem_variant = result_[result_.size() - 2];
        // prepare: check if the last two elements are "AND" or "OR" operations
        if (last_elem_variant.index() == 0 or second_last_elem_variant.index() == 0) {
            return false;
        }
        auto &last_elem = std::get<FilterExecuteSingleRange>(last_elem_variant);
        auto &second_last_elem = std::get<FilterExecuteSingleRange>(second_last_elem_variant);
        // case 1. one empty range and another range
        if (second_last_elem.IsEmpty()) {
            // "AND" result is still empty
            result_.pop_back();
            return true;
        } else if (last_elem.IsEmpty()) {
            second_last_elem.SetEmpty();
            result_.pop_back();
            return true;
        }
        // case 2. two intervals of same ColumnID
        if (last_elem.GetColumnID() != second_last_elem.GetColumnID()) {
            return false;
        }
        // same column id, same type
        auto &second_last_interval = second_last_elem.GetIntervalRange();
        auto &last_interval = last_elem.GetIntervalRange();
        bool merge_result = std::visit(Overload{
            []<typename T>(FilterIntervalRangeT<T> &second_last, const FilterIntervalRangeT<T> &last) -> bool { return second_last.MergeAnd(last); },
            []<typename T1, typename T2>
                requires IncompatibleFilterIntervalRangePair<T1, T2>
            (T1 & x, T2 & y) -> bool {
                String error_message = "FilterCommandBuilder::TryCompactNearbyFilterAnd(): Unreachable branch! Type mismatch.";
                UnrecoverableError(error_message);
                return false;
            }},
            second_last_interval,
            last_interval);
        if (!merge_result) {
            // empty result
            second_last_elem.SetEmpty();
        }
        result_.pop_back();
        return true;
    }

    // try to compact adjacent elements
    // case 1. one kEmpty range and another range
    inline bool TryCompactNearbyFilterOr() {
        if (result_.size() < 2) {
            String error_message = "FilterCommandBuilder::TryCompactNearbyFilter(): result size < 2.";
            UnrecoverableError(error_message);
            return false;
        }
        auto &last_elem_variant = result_[result_.size() - 1];
        auto &second_last_elem_variant = result_[result_.size() - 2];
        // prepare: check if the last two elements are "AND" or "OR" operations
        if (last_elem_variant.index() == 0 or second_last_elem_variant.index() == 0) {
            return false;
        }
        auto &last_elem = std::get<FilterExecuteSingleRange>(last_elem_variant);
        auto &second_last_elem = std::get<FilterExecuteSingleRange>(second_last_elem_variant);
        // case 1. one empty range and another range
        if (last_elem.IsEmpty()) {
            // "OR" result is still second_last_elem
            result_.pop_back();
            return true;
        } else if (second_last_elem.IsEmpty()) {
            // "OR" result is last_elem (non-empty)
            second_last_elem = last_elem; // copy
            result_.pop_back();
            return true;
        } else {
            // both non-empty
            return false;
        }
    }

public:
    FilterCommandBuilder(const Vector<FilterEvaluatorElem> &filter_evaluator) : filter_evaluator_(filter_evaluator) {}

    [[nodiscard]] auto &GetResult() { return result_; }

    [[nodiscard]] bool Build() {
        // progress: 0. set column_id 1. set value_ptr 2. set compare_type 3. save to result_
        enum class Progress : i8 { kColumnID, kValuePtr, kCompareType, kSavedToResult };
        Progress progress = Progress::kSavedToResult;
        ColumnID column_id{};
        const Value *value_ptr;
        FilterCompareType compare_type;

        for (auto &elem : filter_evaluator_) {
            if (std::holds_alternative<ColumnID>(elem)) {
                if (progress != Progress::kSavedToResult) {
                    String error_message = "FilterCommandBuilder::Build(): progress error.";
                    UnrecoverableError(error_message);
                    return false;
                }
                column_id = std::get<ColumnID>(elem);
                progress = Progress::kColumnID;
            } else if (std::holds_alternative<Value>(elem)) {
                if (progress != Progress::kColumnID) {
                    String error_message = "FilterCommandBuilder::Build(): progress error.";
                    UnrecoverableError(error_message);
                    return false;
                }
                value_ptr = &std::get<Value>(elem);
                progress = Progress::kValuePtr;
            } else if (std::holds_alternative<FilterCompareType>(elem)) {
                if (progress != Progress::kValuePtr) {
                    String error_message = "FilterCommandBuilder::Build(): progress error.";
                    UnrecoverableError(error_message);
                    return false;
                }
                compare_type = std::get<FilterCompareType>(elem);
                progress = Progress::kCompareType;
                SaveToResult(column_id, *value_ptr, compare_type);
                progress = Progress::kSavedToResult;
            } else if (std::holds_alternative<BooleanCombineType>(elem)) {
                if (progress != Progress::kSavedToResult) {
                    String error_message = "FilterCommandBuilder::Build(): progress error.";
                    UnrecoverableError(error_message);
                    return false;
                }
                auto combine_type = std::get<BooleanCombineType>(elem);
                switch (combine_type) {
                    case BooleanCombineType::kOr: {
                        // try to compact adjacent elements
                        // case 1. one kEmpty range and another range
                        if (!TryCompactNearbyFilterOr()) {
                            result_.emplace_back(FilterExecuteCombineType::kOr);
                        }
                        break;
                    }
                    case BooleanCombineType::kAnd: {
                        // try to compact adjacent elements
                        // case 1. one kEmpty range and another range
                        // case 2. two intervals of same ColumnID
                        if (!TryCompactNearbyFilterAnd()) {
                            result_.emplace_back(FilterExecuteCombineType::kAnd);
                        }
                        break;
                    }
                    default: {
                        String error_message = "FilterCommandBuilder::Build(): combine type error.";
                        UnrecoverableError(error_message);
                        return false;
                    }
                }
            } else {
                String error_message = "FilterCommandBuilder::Build(): filter evaluator elem error.";
                UnrecoverableError(error_message);
                return false;
            }
        }
        if (progress == Progress::kSavedToResult) {
            return true;
        } else {
            String error_message = "FilterCommandBuilder::Build(): progress error.";
            UnrecoverableError(error_message);
            return false;
        }
    }
};

Vector<FilterExecuteElem> BuildSecondaryIndexScanCommand(SharedPtr<BaseExpression> &index_filter_qualified_) {
    // step 1. middle representation
    // Use Reverse Polish notation to evaluate the filter
    // For example, the filter "a >= 1 AND a <= 2" will be converted to "a 1 >= a 2 <= AND"
    // filter_evaluator_ only contain FilterCompareType of kEqual, kLessEqual, kGreaterEqual, kAlwaysFalse, kAlwaysTrue
    // filter_evaluator_ only contain BooleanCombineType of kAnd, kOr
    Vector<FilterEvaluatorElem> filter_evaluator = BuildSecondaryIndexScanMiddleCommand(index_filter_qualified_);

    // step 2. commands used in PhysicalIndexScan::ExecuteInternal()
    Vector<FilterExecuteElem> filter_execute_command;

    FilterCommandBuilder filter_command_builder(filter_evaluator);
    if (filter_command_builder.Build()) {
        filter_execute_command = std::move(filter_command_builder.GetResult());
    } else {
        String error_message = "PhysicalIndexScan::Init(): filter command builder error.";
        UnrecoverableError(error_message);
    }
    return filter_execute_command;
}

} // namespace infinity
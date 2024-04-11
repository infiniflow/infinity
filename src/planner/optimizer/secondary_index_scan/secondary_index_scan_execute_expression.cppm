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

export module secondary_index_scan_execute_expression;

import stl;
import value;
import base_expression;
import infinity_exception;
import secondary_index_data;
import secondary_index_scan_middle_expression;
import filter_expression_push_down_helper;
import internal_types;
import third_party;
import filter_value_type_classification;

namespace infinity {

// The range will only monotonically shrink
// MergeAnd is meaningful: reduce the search range
// MergeOr is not needed if expression rewrite is done
// index key value type T = ConvertToOrderedType<ColumnValueType>
export template <typename ColumnValueType>
class FilterIntervalRangeT {
public:
    using T = ConvertToOrderedType<ColumnValueType>;
    static_assert(std::is_integral_v<T> or std::is_floating_point_v<T>,
                  "FilterExecuteSingleRangeT: Now only support integral or floating point index key type.");

    explicit FilterIntervalRangeT(const Value &val, FilterCompareType compare_type) {
        ColumnValueType raw_val = val.GetValue<ColumnValueType>();
        T val_ = ConvertToOrderedKeyValue(raw_val);
        AddFilter(val_, compare_type);
    }

    [[nodiscard]] bool MergeAnd(const FilterIntervalRangeT &other) {
        AddLE(other.end_val_);
        AddGE(other.begin_val_);
        return begin_val_ <= end_val_;
    }

    [[nodiscard]] Pair<T, T> GetRange() const { return {begin_val_, end_val_}; }

    inline void SetAlwaysFalse() {
        end_val_ = std::numeric_limits<T>::lowest();
        begin_val_ = std::numeric_limits<T>::max();
    }

private:
    // The interval range will only monotonically shrink
    // default: the whole range of T
    T begin_val_ = std::numeric_limits<T>::lowest();
    T end_val_ = std::numeric_limits<T>::max();

    inline void AddLE(T val) {
        if (val < end_val_) {
            end_val_ = val;
        }
    }
    inline void AddGE(T val) {
        if (val > begin_val_) {
            begin_val_ = val;
        }
    }

    // can only be called in constructor
    inline void AddFilter(const T val, const FilterCompareType compare_type) {
        switch (compare_type) {
            case FilterCompareType::kLessEqual: {
                AddLE(val);
                break;
            }
            case FilterCompareType::kGreaterEqual: {
                AddGE(val);
                break;
            }
            case FilterCompareType::kEqual: {
                AddLE(val);
                AddGE(val);
                break;
            }
            case FilterCompareType::kAlwaysTrue: {
                // default to the whole range of type T
                break;
            }
            default: {
                UnrecoverableError("FilterExecuteSingleRangeT::AddFilter(): compare type error.");
            }
        }
    }
};

export using FilterIntervalRange = std::variant<std::monostate,
                                                FilterIntervalRangeT<TinyIntT>,
                                                FilterIntervalRangeT<SmallIntT>,
                                                FilterIntervalRangeT<IntegerT>,
                                                FilterIntervalRangeT<BigIntT>,
                                                FilterIntervalRangeT<FloatT>,
                                                FilterIntervalRangeT<DoubleT>,
                                                FilterIntervalRangeT<DateT>,
                                                FilterIntervalRangeT<TimeT>,
                                                FilterIntervalRangeT<DateTimeT>,
                                                FilterIntervalRangeT<TimestampT>>;

// because some rows may be deleted, kAlwaysTrue is meaningless
// kInterval of the same column can be merged in "AND" condition
// kAlwaysFalse can be merged with any other condition
enum class FilterRangeType : i8 { kEmpty, kInterval };

export class FilterExecuteSingleRange {
    ColumnID column_id_{};
    FilterRangeType range_type_{};
    FilterIntervalRange interval_range_;

    inline void SetIntervalToEmpty() {
        std::visit(Overload{[]<typename T>(FilterIntervalRangeT<T> &interval) { interval.SetAlwaysFalse(); }, [](const std::monostate &empty) {}},
                   interval_range_);
    }

public:
    explicit FilterExecuteSingleRange(ColumnID column_id, FilterRangeType range_type) : column_id_(column_id), range_type_(range_type) {
        if (IsEmpty()) {
            SetIntervalToEmpty();
        }
    }

    inline bool IsEmpty() const { return range_type_ == FilterRangeType::kEmpty; }

    void SetEmpty() {
        // step 1. set interval_range_ to empty
        SetIntervalToEmpty();
        // step 2. update range_type_
        range_type_ = FilterRangeType::kEmpty;
    }

    ColumnID GetColumnID() const { return column_id_; }

    const FilterIntervalRange &GetIntervalRange() const { return interval_range_; }

    FilterIntervalRange &GetIntervalRange() { return interval_range_; }

    template <typename ColumnValueType>
    void SetIntervalRange(const Value &value, FilterCompareType compare_type) {
        interval_range_.emplace<FilterIntervalRangeT<ColumnValueType>>(value, compare_type);
    }
};

export enum class FilterExecuteCombineType : i8 { kAnd, kOr };

export using FilterExecuteElem = std::variant<FilterExecuteCombineType, FilterExecuteSingleRange>;

export Vector<FilterExecuteElem> BuildSecondaryIndexScanCommand(SharedPtr<BaseExpression> &index_filter_qualified_);

} // namespace infinity
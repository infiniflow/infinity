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

export module min_max_data_filter;
import stl;
import value;
import logger;
import third_party;
import infinity_exception;
import filter_expression_push_down_helper;
import internal_types;
import filter_value_type_classification;

namespace infinity {

// min: empty string
// max: 16 * max char
// compare: out of range < negative char < char '\0' < positive char
// min and max may be truncated
// determine if a string is in range: step 1. truncate to at most 16 char; step 2. compare
export struct InnerMinMaxDataFilterVarcharType {
    constexpr static u32 INNER_STORED_LENGTH = 16;

    constexpr static InnerMinMaxDataFilterVarcharType lowest() { return {}; }

    constexpr static InnerMinMaxDataFilterVarcharType max() {
        InnerMinMaxDataFilterVarcharType result;
        result.length = INNER_STORED_LENGTH;
        std::fill(result.str.begin(), result.str.end(), std::numeric_limits<char>::max());
        return result;
    }

    [[nodiscard]] std::string_view GetStringView() const { return {str.data(), length}; }

    void SetToTruncate(std::string_view &input) {
        length = std::min<u32>(input.size(), INNER_STORED_LENGTH);
        Copy(input.begin(), input.begin() + length, str.begin());
    }

private:
    Array<char, INNER_STORED_LENGTH> str = {};
    u8 length = 0;
};

export template <typename T>
struct InnerMinMaxDataFilterInfo;

template <IsMinMaxInnerValUnchanged ValType>
struct InnerMinMaxDataFilterInfo<ValType> {
    using InnerValueType = ValType;
};

template <IsVarchar ValType>
struct InnerMinMaxDataFilterInfo<ValType> {
    using InnerValueType = InnerMinMaxDataFilterVarcharType;
};

template <IsMinMaxFilterAcceptedTypes OriginalValueType>
class InnerMinMaxDataFilterT {
public:
    using InnerValueType = typename InnerMinMaxDataFilterInfo<OriginalValueType>::InnerValueType;

private:
    InnerValueType min_;
    InnerValueType max_;

public:
    InnerMinMaxDataFilterT() = default;

    explicit InnerMinMaxDataFilterT(InnerValueType &&min, InnerValueType &&max) : min_(std::move(min)), max_(std::move(max)) {}

    [[nodiscard]] inline bool MayInRange(const Value &value, FilterCompareType compare_type) const { return MayInRangeT(value, compare_type); }

    [[nodiscard]] u32 SizeInBytes() const { return sizeof(min_) + sizeof(max_); }

    void SaveToOStringStream(OStringStream &os) const {
        os.write(reinterpret_cast<const char *>(&min_), sizeof(min_));
        os.write(reinterpret_cast<const char *>(&max_), sizeof(max_));
    }

    void LoadFromIStringStream(IStringStream &is) {
        is.read(reinterpret_cast<char *>(&min_), sizeof(min_));
        is.read(reinterpret_cast<char *>(&max_), sizeof(max_));
    }

private:
    template <IsMinMaxInnerValUnchanged T = OriginalValueType>
    [[nodiscard]] inline bool MayInRangeT(const Value &value, FilterCompareType compare_type) const {
        static_assert(std::is_same<InnerValueType, OriginalValueType>::value, "Type mismatch");
        OriginalValueType original_value = value.GetValue<OriginalValueType>();
        switch (compare_type) {
            case FilterCompareType::kLessEqual: {
                return original_value >= min_;
            }
            case FilterCompareType::kGreaterEqual: {
                return original_value <= max_;
            }
            default: {
                String error_message = "InnerMinMaxDataFilterDerived::MayInRange(): Unexpected compare type!";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                // cannot decide, return true
                return true;
            }
        }
    }

    template <IsVarchar T = OriginalValueType>
    [[nodiscard]] inline bool MayInRangeT(const Value &value, FilterCompareType compare_type) const {
        const String &str = value.GetVarchar();
        u32 compare_length = std::min<u32>(str.size(), InnerMinMaxDataFilterVarcharType::INNER_STORED_LENGTH);
        std::string_view compare_str(str.data(), compare_length);
        switch (compare_type) {
            case FilterCompareType::kLessEqual: {
                return compare_str >= min_.GetStringView();
            }
            case FilterCompareType::kGreaterEqual: {
                return compare_str <= max_.GetStringView();
            }
            default: {
                String error_message = "InnerMinMaxDataFilterDerived::MayInRange(): Unexpected compare type!";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                // cannot decide, return true
                return true;
            }
        }
    }
};

template <typename T>
struct InnerMinMaxDataFilterVariant;

template <typename... Args>
struct InnerMinMaxDataFilterVariant<TypeList<Args...>> {
    using Type = std::variant<std::monostate, InnerMinMaxDataFilterT<Args>...>;
};

using InnerMinMaxDataFilter = InnerMinMaxDataFilterVariant<TypeListMinMaxFilterAcceptedTypes>::Type;

template <typename OriginalValueType, typename MinMaxInnerValT>
void CreateInnerMinMaxDataFilter(InnerMinMaxDataFilter &filter, MinMaxInnerValT &&min, MinMaxInnerValT &&max) {
    using DerivedT = InnerMinMaxDataFilterT<std::decay_t<OriginalValueType>>;
    static_assert(std::is_same<std::decay_t<MinMaxInnerValT>, typename DerivedT::InnerValueType>::value, "Type mismatch");
    filter.emplace<DerivedT>(std::forward<MinMaxInnerValT>(min), std::forward<MinMaxInnerValT>(max));
}

// used in block_entry and segment_entry
// can update when data is added
// can not update when data is deleted
export class MinMaxDataFilter {
private:
    Vector<InnerMinMaxDataFilter> min_max_filters_;

public:
    constexpr static std::string_view JsonTag = "min_max_data_filter";

    MinMaxDataFilter() = default;

    explicit MinMaxDataFilter(u32 column_count) : min_max_filters_(column_count) {}

    [[nodiscard]] inline bool MayInRange(ColumnID column_id, const Value &value, FilterCompareType compare_type) const {
        return std::visit(Overload{[column_id](const std::monostate &empty) -> bool {

                                       String error_message = fmt::format("No InnerMinMaxDataFilter for column_id: {}", column_id);
                                       LOG_CRITICAL(error_message);
                                       UnrecoverableError(error_message);

                                       // Should always have minmax filter for sealed segment
                                       return true;
                                   },
                                   [&value, compare_type]<typename T>(const InnerMinMaxDataFilterT<T> &filter) -> bool {
                                       return filter.MayInRange(value, compare_type);
                                   }},
                          min_max_filters_[column_id]);
    }

    // used in build_fast_rough_filter_task
    template <typename OriginalValueType, typename MinMaxInnerValT>
    void Build(ColumnID column_id, MinMaxInnerValT &&min, MinMaxInnerValT &&max) {
        auto &filter = min_max_filters_[column_id];
        if (std::holds_alternative<std::monostate>(filter)) {
            CreateInnerMinMaxDataFilter<OriginalValueType>(filter, std::forward<MinMaxInnerValT>(min), std::forward<MinMaxInnerValT>(max));
        } else {
            String error_message = fmt::format("In MinMaxDataFilter::Build(), InnerMinMaxDataFilter already exist for column_id: {}", column_id);
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    u32 GetSerializeSizeInBytes() const;

    void SerializeToStringStream(OStringStream &os, u32 total_binary_bytes = 0) const;

    void DeserializeFromStringStream(IStringStream &is);

    void SaveToJsonFile(nlohmann::json &entry_json) const;

    bool LoadFromJsonFile(const nlohmann::json &entry_json);
};

} // namespace infinity
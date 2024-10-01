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

#include "base64.hpp"
#include <vector>
module probabilistic_data_filter;
import stl;
import logical_type;
import binary_fuse_filter;
import file_system;
import file_system_type;
import infinity_exception;

namespace infinity {

void ProbabilisticDataFilter::Build(TxnTimeStamp begin_ts, ColumnID column_id, u64 *data, u32 count) {
    auto &binary_fuse_filter = binary_fuse_filters_[column_id];
    if (!binary_fuse_filter) {
        String error_message = fmt::format("BUG: ProbabilisticDataFilter for column_id: {} is nullptr.", column_id);
        UnrecoverableError(error_message);
    }
    binary_fuse_filter->Build(begin_ts, data, count);
}

u32 ProbabilisticDataFilter::GetSerializeSizeInBytes() const {
    // step 0. prepare column_count
    u32 column_count;
    // step 1. prepare space for binary_fuse_filters_
    u32 extra_binary_bytes = 0;
    for (auto &filter : binary_fuse_filters_) {
        extra_binary_bytes += sizeof(char);
        if (filter) {
            extra_binary_bytes += filter->SaveBytes();
        }
    }
    u32 total_binary_bytes = sizeof(total_binary_bytes) + sizeof(column_count) + extra_binary_bytes;
    return total_binary_bytes;
}

void ProbabilisticDataFilter::SerializeToStringStream(OStringStream &os, u32 total_binary_bytes) const {
    // step 0. prepare column_count
    u32 column_count = binary_fuse_filters_.size();
    // step 1. prepare space for binary_fuse_filters_
    if (total_binary_bytes == 0) {
        total_binary_bytes = GetSerializeSizeInBytes();
    }
    // step 2. encode to binary
    auto begin_pos = os.tellp();
    os.write(reinterpret_cast<const char *>(&total_binary_bytes), sizeof(total_binary_bytes));
    os.write(reinterpret_cast<const char *>(&column_count), sizeof(column_count));
    for (auto &filter : binary_fuse_filters_) {
        char exist = filter ? 1 : 0;
        os.write(reinterpret_cast<const char *>(&exist), sizeof(exist));
        if (filter) {
            filter->SaveToOStringStream(os);
        }
    }
    // check position
    auto end_pos = os.tellp();
    if (end_pos - begin_pos != total_binary_bytes) {
        String error_message = "ProbabilisticDataFilter::SerializeToStringStream(): save size error";
        UnrecoverableError(error_message);
    }
}

void ProbabilisticDataFilter::DeserializeFromStringStream(IStringStream &is) {
    auto begin_pos = is.tellg();
    // step 0. load expected_total_binary_bytes and column_count
    u32 expected_total_binary_bytes;
    is.read(reinterpret_cast<char *>(&expected_total_binary_bytes), sizeof(expected_total_binary_bytes));
    u32 column_count;
    is.read(reinterpret_cast<char *>(&column_count), sizeof(column_count));
    // step 1. load binary_fuse_filters_
    if (binary_fuse_filters_.empty()) {
        binary_fuse_filters_.resize(column_count);
        LOG_TRACE("ProbabilisticDataFilter::DeserializeFromStringStream(): begin with empty filter array");
    } else if (binary_fuse_filters_.size() == column_count) {
        LOG_TRACE("ProbabilisticDataFilter::DeserializeFromStringStream(): begin with existing filter array");
    } else {
        String error_message = "ProbabilisticDataFilter::DeserializeFromStringStream(): column_count mismatch";
        UnrecoverableError(error_message);
    }
    for (char binary_fuse_filter_exist; auto &filter : binary_fuse_filters_) {
        is.read(reinterpret_cast<char *>(&binary_fuse_filter_exist), sizeof(binary_fuse_filter_exist));
        if (binary_fuse_filter_exist) {
            if (filter) {
                LOG_TRACE("ProbabilisticDataFilter::DeserializeFromStringStream(): overwrite existing filter");
            } else {
                LOG_TRACE("ProbabilisticDataFilter::DeserializeFromStringStream(): load new filter");
            }
            filter = MakeUnique<BinaryFuse>();
            filter->LoadFromIStringStream(is);
        }
    }
    // check position
    auto end_pos = is.tellg();
    if (end_pos - begin_pos != expected_total_binary_bytes) {
        String error_message = "ProbabilisticDataFilter::DeserializeFromStringStream(): position error";
        UnrecoverableError(error_message);
    }
}

void ProbabilisticDataFilter::SaveToJsonFile(nlohmann::json &entry_json) const {
    // step 1. prepare space for binary_fuse_filters_
    u32 total_binary_bytes = GetSerializeSizeInBytes();
    // step 2. encode to binary
    String save_to_binary;
    save_to_binary.reserve(total_binary_bytes);
    OStringStream os(std::move(save_to_binary));
    SerializeToStringStream(os, total_binary_bytes);
    // step 3. encode to base64, and save to json
    auto result_view = os.view();
    if (result_view.size() != total_binary_bytes) {
        String error_message = "BUG: ProbabilisticDataFilter::SaveToJsonFile(): save size error";
        UnrecoverableError(error_message);
    }
    entry_json[JsonTag] = base64::to_base64(result_view);
}

bool ProbabilisticDataFilter::LoadFromJsonFile(const nlohmann::json &entry_json) {
    if (!entry_json.contains(JsonTag)) {
        LOG_ERROR("ProbabilisticDataFilter::LoadFromJsonFile(): found no data.");
        return false;
    }
    String filter_base64 = entry_json[JsonTag];
    auto filter_binary = base64::from_base64(filter_base64);
    IStringStream is(filter_binary);
    DeserializeFromStringStream(is);
    if (!is or u32(is.tellg()) != is.view().size()) {
        String error_message = "ProbabilisticDataFilter::LoadFromJsonFile(): load size error";
        UnrecoverableError(error_message);
        return false;
    }
    return true;
}

template <typename ValueType>
u64 ConvertValueToU64T(const Value &value) {
    return ConvertValueToU64(value.GetValue<ValueType>());
}

template <>
u64 ConvertValueToU64T<VarcharT>(const Value &value) {
    return ConvertValueToU64(value.GetVarchar());
}

u64 ConvertValueToU64(const Value &value) {
    switch (value.type_.type()) {
        case LogicalType::kBoolean: {
            return ConvertValueToU64T<BooleanT>(value);
        }
        case LogicalType::kTinyInt: {
            return ConvertValueToU64T<TinyIntT>(value);
        }
        case LogicalType::kSmallInt: {
            return ConvertValueToU64T<SmallIntT>(value);
        }
        case LogicalType::kInteger: {
            return ConvertValueToU64T<IntegerT>(value);
        }
        case LogicalType::kBigInt: {
            return ConvertValueToU64T<BigIntT>(value);
        }
        case LogicalType::kHugeInt: {
            return ConvertValueToU64T<HugeIntT>(value);
        }
        case LogicalType::kDecimal: {
            return ConvertValueToU64T<DecimalT>(value);
        }
        case LogicalType::kDate: {
            return ConvertValueToU64T<DateT>(value);
        }
        case LogicalType::kTime: {
            return ConvertValueToU64T<TimeT>(value);
        }
        case LogicalType::kDateTime: {
            return ConvertValueToU64T<DateTimeT>(value);
        }
        case LogicalType::kTimestamp: {
            return ConvertValueToU64T<TimestampT>(value);
        }
        case LogicalType::kVarchar: {
            return ConvertValueToU64T<VarcharT>(value);
        }
        default: {
            UnrecoverableError(fmt::format("ConvertValueToU64() not implemented for type {}", value.type_.ToString()));
            return 0;
        }
    }
}

} // namespace infinity
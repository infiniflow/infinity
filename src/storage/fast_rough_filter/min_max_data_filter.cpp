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
module min_max_data_filter;
import stl;
import value;
import logger;
import third_party;
import infinity_exception;
import filter_expression_push_down_helper;
import internal_types;
import column_vector;
import filter_value_type_classification;

namespace infinity {

using VariantEmplaceFuncType = void(InnerMinMaxDataFilter *ptr);

template <int i>
void VariantEmplaceFunc(InnerMinMaxDataFilter *ptr) {
    (*ptr).emplace<i>();
}

template <std::size_t... I>
consteval auto GetVariantEmplaceFuncs(std::index_sequence<I...>) {
    return Array<VariantEmplaceFuncType *, sizeof...(I)>{VariantEmplaceFunc<I>...};
}

static constexpr auto VariantEmplaceFuncs = GetVariantEmplaceFuncs(std::make_index_sequence<std::variant_size_v<InnerMinMaxDataFilter>>{});

u32 MinMaxDataFilter::GetSerializeSizeInBytes() const {
    // step 0. prepare column_count
    u32 column_count;
    // step 1. prepare space for binary_fuse_filters_
    u32 extra_binary_bytes = 0;
    for (auto &filter : min_max_filters_) {
        extra_binary_bytes += sizeof(u8);
        std::visit(Overload{[](const std::monostate &empty) -> void {},
                            [&extra_binary_bytes]<typename T>(const InnerMinMaxDataFilterT<T> &filter) -> void {
                                extra_binary_bytes += filter.SizeInBytes();
                            }},
                   filter);
    }
    u32 total_binary_bytes = sizeof(total_binary_bytes) + sizeof(column_count) + extra_binary_bytes;
    return total_binary_bytes;
}

void MinMaxDataFilter::SerializeToStringStream(OStringStream &os, u32 total_binary_bytes) const {
    // step 0. prepare column_count
    u32 column_count = min_max_filters_.size();
    // step 1. prepare space for binary_fuse_filters_
    if (total_binary_bytes == 0) {
        total_binary_bytes = GetSerializeSizeInBytes();
    }
    // step 2. encode to binary
    auto begin_pos = os.tellp();
    os.write(reinterpret_cast<const char *>(&total_binary_bytes), sizeof(total_binary_bytes));
    os.write(reinterpret_cast<const char *>(&column_count), sizeof(column_count));
    for (auto &filter : min_max_filters_) {
        u8 var_id = filter.index();
        os.write(reinterpret_cast<const char *>(&var_id), sizeof(var_id));
        std::visit(Overload{[](const std::monostate &empty) -> void {},
                            [&os]<typename T>(const InnerMinMaxDataFilterT<T> &filter) -> void { filter.SaveToOStringStream(os); }},
                   filter);
    }
    // check position
    auto end_pos = os.tellp();
    if (end_pos - begin_pos != total_binary_bytes) {
        String error_message = "MinMaxDataFilter::SerializeToStringStream(): save size error";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void MinMaxDataFilter::DeserializeFromStringStream(IStringStream &is) {
    auto begin_pos = is.tellg();
    // step 0. load expected_total_binary_bytes and column_count
    u32 expected_total_binary_bytes;
    is.read(reinterpret_cast<char *>(&expected_total_binary_bytes), sizeof(expected_total_binary_bytes));
    u32 column_count;
    is.read(reinterpret_cast<char *>(&column_count), sizeof(column_count));
    // step 1. load min_max_filters_
    if (min_max_filters_.empty()) {
        min_max_filters_.resize(column_count);
        LOG_TRACE("MinMaxDataFilter::DeserializeFromStringStream(): begin with empty filter array");
    } else if (min_max_filters_.size() == column_count) {
        LOG_TRACE("MinMaxDataFilter::DeserializeFromStringStream(): begin with existing filter array");
    } else {
        String error_message = "MinMaxDataFilter::DeserializeFromStringStream(): column_count mismatch";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (auto &filter : min_max_filters_) {
        u8 var_id = 0;
        is.read(reinterpret_cast<char *>(&var_id), sizeof(var_id));
        if (var_id == 0) {
            continue;
        }
        if (var_id >= std::variant_size_v<InnerMinMaxDataFilter>) {
            String error_message = "MinMaxDataFilter::DeserializeFromStringStream(): invalid var_id";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            return;
        }
        // have data to load
        // check if filter is empty
        if (filter.index() != 0 and filter.index() != var_id) {
            String error_message = "MinMaxDataFilter::DeserializeFromStringStream(): var_id mismatch";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        } else if (filter.index() != 0) {
            LOG_TRACE("MinMaxDataFilter::DeserializeFromStringStream(): overwrite existing filter");
        } else {
            LOG_TRACE("MinMaxDataFilter::DeserializeFromStringStream(): load new filter");
        }
        VariantEmplaceFuncs[var_id](&filter);
        std::visit(Overload{[](const std::monostate &empty) -> void {
                                 String error_message = "MinMaxDataFilter::DeserializeFromStringStream(): invalid zero var_id";
                                 LOG_CRITICAL(error_message);
                                 UnrecoverableError(error_message);
                            },
                            [&is]<typename T>(InnerMinMaxDataFilterT<T> &filter) -> void { filter.LoadFromIStringStream(is); }},
                   filter);
    }
    // check position
    auto end_pos = is.tellg();
    if (end_pos - begin_pos != expected_total_binary_bytes) {
        String error_message = "MinMaxDataFilter::DeserializeFromStringStream(): load size error";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void MinMaxDataFilter::SaveToJsonFile(nlohmann::json &entry_json) const {
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
        String error_message = "MinMaxDataFilter::SaveToJsonFile(): save size error";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    entry_json[JsonTag] = base64::to_base64(result_view);
}

bool MinMaxDataFilter::LoadFromJsonFile(const nlohmann::json &entry_json) {
    if (!entry_json.contains(JsonTag)) {
        LOG_ERROR("MinMaxDataFilter::LoadFromJsonFile(): found no data.");
        return false;
    }
    String filter_base64 = entry_json[JsonTag];
    auto filter_binary = base64::from_base64(filter_base64);
    IStringStream is(filter_binary);
    DeserializeFromStringStream(is);
    if (!is or u32(is.tellg()) != is.view().size()) {
        String error_message = "MinMaxDataFilter::LoadFromJsonFile(): position error";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return false;
    }
    return true;
}

}; // namespace infinity
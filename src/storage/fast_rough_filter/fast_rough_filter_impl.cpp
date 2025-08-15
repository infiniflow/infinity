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

module infinity_core:fast_rough_filter.impl;

import :fast_rough_filter;
import :stl;
import :value;
import :default_values;
import :probabilistic_data_filter;
import :min_max_data_filter;
import :logger;
import :infinity_exception;
import :filter_expression_push_down_helper;

import std;
import third_party;

namespace infinity {

bool FastRoughFilter::IsValid() const { return min_max_data_filter_ != nullptr; }

std::string FastRoughFilter::SerializeToString() const {
    if (HaveMinMaxFilter()) {
        u32 probabilistic_data_filter_binary_bytes = probabilistic_data_filter_->GetSerializeSizeInBytes();
        u32 min_max_data_filter_binary_bytes = min_max_data_filter_->GetSerializeSizeInBytes();
        u32 total_binary_bytes =
            sizeof(total_binary_bytes) + sizeof(build_time_) + probabilistic_data_filter_binary_bytes + min_max_data_filter_binary_bytes;
        std::string save_to_binary;
        save_to_binary.reserve(total_binary_bytes);
        OStringStream os(std::move(save_to_binary));
        os.write(reinterpret_cast<const char *>(&total_binary_bytes), sizeof(total_binary_bytes));
        os.write(reinterpret_cast<const char *>(&build_time_), sizeof(build_time_));
        probabilistic_data_filter_->SerializeToStringStream(os, probabilistic_data_filter_binary_bytes);
        min_max_data_filter_->SerializeToStringStream(os, min_max_data_filter_binary_bytes);
        if (os.view().size() != total_binary_bytes) {
            UnrecoverableError("FastRoughFilter::SerializeToString(): save size error");
        }
        return std::move(os).str();
    }
    UnrecoverableError("FastRoughFilter::SerializeToString(): No FastRoughFilter data.");
    return {};
}

void FastRoughFilter::DeserializeFromString(const std::string &str) {
    // load necessary parts
    IStringStream is(str);
    u32 total_binary_bytes{};
    if (str.empty()) {
        LOG_ERROR(fmt::format("FastRoughFilter: empty fast rough filter"));
        return;
    }

    is.read(reinterpret_cast<char *>(&total_binary_bytes), sizeof(total_binary_bytes));
    if (total_binary_bytes != is.view().size()) {
        UnrecoverableError("FastRoughFilter::DeserializeToString(): load size error");
    }
    is.read(reinterpret_cast<char *>(&build_time_), sizeof(build_time_));
    if (!probabilistic_data_filter_) {
        probabilistic_data_filter_ = std::make_unique<ProbabilisticDataFilter>();
    }
    probabilistic_data_filter_->DeserializeFromStringStream(is);
    if (!min_max_data_filter_) {
        min_max_data_filter_ = std::make_unique<MinMaxDataFilter>();
    }
    min_max_data_filter_->DeserializeFromStringStream(is);
    // check position
    if (!is or u32(is.tellg()) != is.view().size()) {
        UnrecoverableError("FastRoughFilter::DeserializeToString(): load size error");
    }
    FinishBuildMinMaxFilterTask();
}

void FastRoughFilter::SaveToJsonFile(nlohmann::json &entry_json) const {
    if (HaveMinMaxFilter()) {
        //  LOG_TRACE("FastRoughFilter::SaveToJsonFile(): have FastRoughFilter, save now.");
        entry_json[JsonTagBuildTime] = build_time_;
        probabilistic_data_filter_->SaveToJsonFile(entry_json);
        min_max_data_filter_->SaveToJsonFile(entry_json);
    } else {
        LOG_TRACE("FastRoughFilter::SaveToJsonFile(): No MinMax data.");
    }
}

// will throw in caller function if return false
// called in deserialize, remove unnecessary lock
bool FastRoughFilter::LoadFromJsonFile(std::string_view json_sv) {
    simdjson::padded_string json_pad(json_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_pad);
    if (HaveMinMaxFilter()) [[unlikely]] {
        UnrecoverableError("FastRoughFilter::LoadFromJsonFile(): Already have data.");
    }
    // LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): try to load filter data from json.");
    // try load JsonTagBuildTime first
    if (doc[JsonTagBuildTime].get<TxnTimeStamp>(build_time_) != simdjson::SUCCESS) {
        LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): found no save data in json, stop loading.");
        return false;
    }
    // can load from json
    bool load_success = true;
    {
        // load ProbabilisticDataFilter
        auto load_probabilistic_data_filter = std::make_unique<ProbabilisticDataFilter>();
        if (load_probabilistic_data_filter->LoadFromJsonFile(json_sv)) {
            probabilistic_data_filter_ = std::move(load_probabilistic_data_filter);
            // LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): Finish load ProbabilisticDataFilter data from json.");
        } else {
            load_success = false;
            LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): Cannot load ProbabilisticDataFilter data from json.");
        }
    }
    {
        // load MinMaxDataFilter
        auto load_min_max_data_filter = std::make_unique<MinMaxDataFilter>();
        if (load_min_max_data_filter->LoadFromJsonFile(json_sv)) {
            min_max_data_filter_ = std::move(load_min_max_data_filter);
            // LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): Finish load MinMaxDataFilter data from json.");
        } else {
            load_success = false;
            LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): Cannot load MinMaxDataFilter data from json.");
        }
    }
    if (load_success) {
        FinishBuildMinMaxFilterTask();
        // LOG_TRACE("FastRoughFilter::LoadFromJsonFile(): successfully load FastRoughFilter data from json.");
    } else {
        UnrecoverableError("FastRoughFilter::LoadFromJsonFile(): partially failed to load FastRoughFilter data from json.");
    }
    return load_success;
}

} // namespace infinity

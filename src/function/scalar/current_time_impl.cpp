// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:current_time.impl;

import :current_time;
import :config;
import :new_catalog;
import :status;
import :column_vector;
import :infinity_context;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std;

import logical_type;
import internal_types;
import data_type;

namespace infinity {
using namespace std::chrono;
struct CurrentTimeFunction {
    template <typename TB>
    static inline void Run(TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void CurrentTimeFunction::Run(TimeT &result) {
    InfinityContext& infinityContext = InfinityContext::instance();
    Config* config = infinityContext.config();
    auto now = system_clock::now() + config->hour_offset_;
    auto sys_days = std::chrono::floor<std::chrono::days>(now);
    auto sys_secs = std::chrono::floor<std::chrono::seconds>(now);
    auto seconds_per_day = 24 * 60 * 60;
    result.value = static_cast<i32>(sys_secs.time_since_epoch().count() - (seconds_per_day * sys_days.time_since_epoch().count()));
}

void RegisterCurrentTimeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "current_time";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction current_time_function(func_name,
                                  {},
                                  DataType(LogicalType::kTime),
                                  &ScalarFunction::NullaryFunction<TimeT, CurrentTimeFunction>);
    function_set_ptr->AddFunction(current_time_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
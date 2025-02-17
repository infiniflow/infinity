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
module;
#include <chrono>
module current_timestamp;
import stl;
import config;
import catalog;
import status;
import logical_type;
import infinity_context;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;
import query_context;

namespace infinity {
using namespace std::chrono;
struct CurrentTimestampFunction {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void CurrentTimestampFunction::Run(VarcharT &left, TimestampT &result) {
    String tz_str = left.ToString();
    InfinityContext& infinityContext = InfinityContext::instance();
    Config* config = infinityContext.config();
    auto offset = config->TimeZoneBias();
    hours offset_hour(offset);
    auto now = system_clock::now() + offset_hour;
    auto sys_days = std::chrono::floor<std::chrono::days>(now);
    auto sys_secs = std::chrono::floor<std::chrono::seconds>(now);
    result.time.value = static_cast<i32>(sys_secs.time_since_epoch().count() - sys_days.time_since_epoch().count());
    result.date.value = static_cast<i32>(sys_days.time_since_epoch().count());
}

void RegisterCurrentTimestampFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "currenttimestamp";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction current_timestamp_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  DataType(LogicalType::kTimestamp),
                                  &ScalarFunction::UnaryFunction<VarcharT, TimestampT, CurrentTimestampFunction>);
    function_set_ptr->AddFunction(current_timestamp_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
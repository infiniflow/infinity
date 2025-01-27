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
module current_time;
import stl;
import catalog;
import status;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;

namespace infinity {
using namespace std::chrono;
struct CurrentTimeFunction {
    const char* defaultTZ = "Asia/Shanghai";
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
    static inline void TimeZoneConvertHelper(VarcharT &left) {
        const char* tzValue = std::getenv("TZ");
        const std::string str = left.ToString();
        const char* newTZ = str.c_str();
        if ( tzValue == newTZ) {
            return;
        }
        if (setenv("TZ", newTZ, 1) != 0) {
            const char* newTZ = "Asia/Shanghai";
            setenv("TZ", newTZ, 1);
        }
        tzset();
        return;
    }

    static inline void TimeZoneResetHelper() {
        const char* tzValue = std::getenv("TZ");
        if (tzValue == CurrentTimeFunction().defaultTZ) {
            return;
        }
        setenv("TZ", CurrentTimeFunction().defaultTZ, 1);
        tzset();
        return;
    }
};

template <>
inline void CurrentTimeFunction::Run(VarcharT &left, TimeT &result) {
        TimeZoneConvertHelper(left);
        auto now = system_clock::now();
        auto sys_days = std::chrono::floor<std::chrono::days>(now);
        auto sys_secs = std::chrono::floor<std::chrono::seconds>(now);
        result.value = static_cast<i32>(sys_secs.time_since_epoch().count() - sys_days.time_since_epoch().count());
        TimeZoneResetHelper();
}

void RegisterCurrentTimeFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "currenttime";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction current_time_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  DataType(LogicalType::kTime),
                                  &ScalarFunction::UnaryFunction<VarcharT, TimeT, CurrentTimeFunction>);
    function_set_ptr->AddFunction(current_time_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
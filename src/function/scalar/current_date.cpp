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
module current_date;
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
struct CurrentDateFunction {
    template <typename TA>
    static inline void Run(TA &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return;
        // auto now = system_clock::now();
        // zoned_time local_time(current_zone(), now);
        // year_month_day ymd = floor<std::chrono::days>(local_time.get_local_time());
    }

};

void RegisterCurrentDateFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "currentdate";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction current_date_function(func_name,
                                  {DataType(LogicalType::kDate)},
                                  &ScalarFunction::NullaryFunction<DateT, CurrentDateFunction>);
    function_set_ptr->AddFunction(current_date_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
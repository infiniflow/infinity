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
module convert_timezone;
import stl;
import config;
import catalog;
import status;
import logical_type;
import infinity_exception;
import infinity_context;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;

namespace infinity {
using namespace std::chrono;
struct ConvertTimeZoneFunction {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result, ColumnVector *result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return;
    }
};

template <>
inline void ConvertTimeZoneFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *result_ptr) {
    String tz_str = left.ToString();
    Config::SetUserTimeZone(tz_str);
    result_ptr->AppendVarcharInner(tz_str, result);
}

void RegisterConvertTimeZoneFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "converttz";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction convert_timezone_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  DataType(LogicalType::kVarchar),
                                  &ScalarFunction::UnaryFunctionToVarlen<VarcharT, VarcharT, ConvertTimeZoneFunction>);
    function_set_ptr->AddFunction(convert_timezone_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
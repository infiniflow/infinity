// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include <sstream>
import base_expression;

import infinity_exception;

module aggregate_function;

namespace infinity {

void AggregateFunction::CastArgumentTypes(BaseExpression &input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type

    Error<PlannerException>("Not implemented: need to cast the argument types");
}

std::string AggregateFunction::ToString() const {

    std::stringstream ss;
    ss << name_;
    ss << "(";
    ss << argument_type_.ToString();
    ss << ")->" << return_type_.ToString();

    return ss.str();
}

} // namespace infinity
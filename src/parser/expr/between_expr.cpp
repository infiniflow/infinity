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

#include "between_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

BetweenExpr::~BetweenExpr() {
    delete value_;
    delete upper_bound_;
    delete lower_bound_;
}

std::string BetweenExpr::ToString() const {
    std::stringstream ss;
    ss << value_->ToString() << " BETWEEN " << upper_bound_->ToString() << " AND " << lower_bound_->ToString();
    return ss.str();
}

} // namespace infinity

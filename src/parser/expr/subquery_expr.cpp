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

#include "subquery_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

SubqueryExpr::~SubqueryExpr() {
    if (left_ != nullptr) {
        delete left_;
    }
    if (select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }
}

std::string SubqueryExpr::ToString() const {
    std::stringstream ss;
    if (left_ != nullptr) {
        ss << left_->ToString() << " ";
    }

    switch (subquery_type_) {
        case SubqueryType::kExists: {
            ss << "EXISTS(";
            break;
        }
        case SubqueryType::kNotExists: {
            ss << "NOT EXISTS(";
            break;
        }
        case SubqueryType::kIn: {
            ss << "IN(";
            break;
        }
        case SubqueryType::kNotIn: {
            ss << "NOT IN(";
            break;
        }
        default: {
            ss << "(";
            break;
        }
    }

    ss << select_->ToString() << ")";

    return ss.str();
}

} // namespace infinity

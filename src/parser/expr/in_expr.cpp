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

#include "in_expr.h"
#include <sstream>

namespace infinity {

InExpr::~InExpr() {
    if (left_ != nullptr) {
        delete left_;
        left_ = nullptr;
    }

    if (arguments_ != nullptr) {
        for (auto &expr_ptr : *arguments_) {
            delete expr_ptr;
        }
        delete arguments_;
        arguments_ = nullptr;
    }
}

std::string InExpr::ToString() const {
    std::stringstream ss;
    ss << left_->ToString();
    if (in_) {
        ss << "IN (";
    } else {
        ss << "NOT IN (";
    }
    if (arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr : *arguments_) {
            ss << expr_ptr->ToString();
        }
    }
    ss << ')';
    return ss.str();
}

} // namespace infinity

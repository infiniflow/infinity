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

#include "case_expr.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

WhenThen::~WhenThen() {
    if (when_ != nullptr) {
        delete when_;
        when_ = nullptr;
    }
    if (then_ != nullptr) {
        delete then_;
        then_ = nullptr;
    }
}

CaseExpr::~CaseExpr() {
    if (expr_ != nullptr) {
        delete expr_;
        expr_ = nullptr;
    }
    if (else_expr_ != nullptr) {
        delete else_expr_;
        else_expr_ = nullptr;
    }
    if (case_check_array_ != nullptr) {
        for (WhenThen *check_ptr : *case_check_array_) {
            delete check_ptr;
        }
        delete case_check_array_;
        case_check_array_ = nullptr;
    }
}

std::string CaseExpr::ToString() const {
    std::stringstream ss;

    ss << "CASE ";
    if (expr_ != nullptr) {
        ss << expr_->ToString();
    }

    if (case_check_array_ != nullptr) {
        for (auto &when_then : *case_check_array_) {
            ss << " WHEN " << when_then->when_->ToString() << " THEN " << when_then->then_->ToString();
        }
    }
    if (else_expr_ != nullptr) {
        ss << " ELSE " << else_expr_->ToString();
    }
    return ss.str();
}

} // namespace infinity

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

#include "function_expr.h"
#include "expr/column_expr.h"
#include <sstream>

namespace infinity {

FunctionExpr::~FunctionExpr() {
    if (arguments_ != nullptr) {
        for (auto &expr_ptr : *arguments_) {
            delete expr_ptr;
        }
        delete arguments_;
        arguments_ = nullptr;
    }
}

std::string FunctionExpr::ToString() const {
    std::stringstream ss;

    if(arguments_ == nullptr) {
        // No argument function
        ss << func_name_ << "()";
        return ss.str();
    }

    if (arguments_->size() == 1) {
        // Unary argument function
        ColumnExpr *column_expr = dynamic_cast<ColumnExpr *>(arguments_->at(0));
        if (column_expr && column_expr->star_) {
            ss << func_name_ << "(star)";
            return ss.str();
        } else {
            ss << func_name_ << '(' << arguments_->at(0)->ToString() << ")";
            return ss.str();
        }
    }
    if (arguments_->size() == 2) {
        // Binary argument function
        ss << '(' << arguments_->at(0)->ToString() << " " << func_name_ << " " << arguments_->at(1)->ToString() << ")";
        return ss.str();
    }

    ss << func_name_ << '(';
    if (arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr : *arguments_) {
            ss << expr_ptr->ToString();
        }
    }
    ss << ')';
    return ss.str();
}

} // namespace infinity

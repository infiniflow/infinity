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

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

FunctionExpr::~FunctionExpr() {
    if (arguments_ != nullptr) {
        for (auto &expr_ptr : *arguments_) {
            if (expr_ptr != nullptr) {
                delete expr_ptr;
                expr_ptr = nullptr;
            }
        }
        delete arguments_;
        arguments_ = nullptr;
    }
}

std::string FunctionExpr::ToString() const {
    std::stringstream ss;

    if (arguments_ == nullptr) {
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
            ss << func_name_ << '(';
            if (distinct_) {
                ss << "DISTINCT ";
            }
            ss << arguments_->at(0)->ToString() << ")";
            return ss.str();
        }
    }
    if (arguments_->size() == 2) {
        // Check if it's actually a binary operator
        static const std::set<std::string> infix_operators = {"+", "-", "*", "/", "%", "^", "&&", "||", "=", "!=", "<", ">", "<=", ">="};
        if (infix_operators.contains(func_name_)) {
            // Binary operator - use infix notation
            ss << '(' << arguments_->at(0)->ToString() << " " << func_name_ << " " << arguments_->at(1)->ToString() << ")";
            return ss.str();
        }
    }

    ss << func_name_ << '(';
    if (distinct_) {
        ss << "DISTINCT ";
    }
    if (arguments_ != nullptr) {
        for (size_t i = 0; i < arguments_->size(); ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << arguments_->at(i)->ToString();
        }
    }
    ss << ')';
    return ss.str();
}

bool JsonExtraInfo::Init() {
    if (initialized_) {
        return true;
    }

    std::string_view entire_path_view(json_extra_info_);
    if (entire_path_view.empty() || entire_path_view[0] != '$') {
        return false;
    }

    std::string_view remaining = entire_path_view.substr(1);
    json_tokens_.clear();

    size_t start = 0;
    size_t end = remaining.find('.');
    while (start < remaining.length()) {
        auto token_end = (end == std::string_view::npos) ? remaining.length() : end;

        if (start < token_end) {
            json_tokens_.emplace_back(remaining.substr(start, token_end - start));
        }

        if (end == std::string_view::npos)
            break;

        start = end + 1;
        end = remaining.find('.', start);
    }

    initialized_ = true;
    return true;
}

} // namespace infinity

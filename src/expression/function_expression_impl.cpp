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

module infinity_core:function_expression.impl;

import :function_expression;
import :column_expression;
import :value_expression;
import :cast_expression;

import :expression_type;
import :scalar_function;

import std;

namespace infinity {

FunctionExpression::FunctionExpression(ScalarFunction function, std::vector<std::shared_ptr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_(std::move(function)) {
    if (arguments_.size() == 0) {
        nullary_ = true;
    }
}

std::string FunctionExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    std::stringstream ss;
    if (arguments_.size() == 1) {
        // Unary argument function
        ss << func_.name() << '(' << arguments_.at(0)->Name() << ")";
        return ss.str();
    }

    if (arguments_.size() == 2) {
        // Binary argument function
        static const std::set<std::string> infix_operators = {"+",  "-",  "*",   "/",  "%",    "=",        "<>", "!=",     "<",  ">",
                                                              "<=", ">=", "AND", "OR", "LIKE", "NOT LIKE", "IN", "NOT IN", "IS", "IS NOT"};
        std::string upper_name = func_.name();
        std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), [](unsigned char c) { return std::toupper(c); });
        if (infix_operators.contains(upper_name)) {
            ss << '(' << arguments_.at(0)->Name() << " " << func_.name() << " " << arguments_.at(1)->Name() << ")";
            return ss.str();
        }
    }

    ss << func_.name() << "(";
    if (arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for (size_t i = 0; i < arguments_count - 1; ++i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }
    return ss.str();
}

nlohmann::json FunctionExpression::Serialize() const {
    nlohmann::json j;
    j["type"] = "Function";

    if (!alias_.empty()) {
        j["alias"] = alias_;
    }

    nlohmann::json args_json = nlohmann::json::array();
    if (!nullary_) {
        for (const auto &arg_expr : this->arguments_) {
            args_json.push_back(arg_expr->Serialize());
        }
        j["arguments"] = args_json;
    }

    j["func_name"] = func_.name();

    return j;
}

u64 FunctionExpression::Hash() const {
    u64 h = 0;

    h ^= func_.Hash();
    for (const auto &arg : arguments_) {
        h ^= arg->Hash();
    }
    return h;
}

bool FunctionExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kFunction) {
        return false;
    }
    const auto &other = static_cast<const FunctionExpression &>(other_base);
    if (!func_.Eq(other.func_)) {
        return false;
    }
    if (arguments_.size() != other.arguments_.size()) {
        return false;
    }
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (!arguments_[i]->Eq(*other.arguments_[i])) {
            return false;
        }
    }
    return true;
}

std::string FunctionExpression::ExtractFunctionInfo() {
    // e.g. substring(c1,1,2) -> "substring_COL0_1_2"
    // For LIKE, NOT LIKE operators, just extract the nested function
    // e.g. substring(c1,1,2) LIKE 'c%' -> "substring_COL0_1_2"
    std::string func_name = ScalarFunctionName();
    static const std::set<std::string> like_ops = {"like", "not_like"};
    if (like_ops.contains(func_name) && !arguments().empty()) {
        const auto &first_arg = arguments()[0];
        if (first_arg && first_arg->type() == ExpressionType::kFunction) {
            auto nested_func = std::static_pointer_cast<FunctionExpression>(first_arg);
            return nested_func->ExtractFunctionInfo();
        }
    }

    auto extract_expr_info = [&](const std::shared_ptr<BaseExpression> &expr, std::string &param_str, auto &&self) -> void {
        if (!expr) {
            return;
        }

        switch (expr->type()) {
            case ExpressionType::kColumn: {
                auto col_expr = std::static_pointer_cast<ColumnExpression>(expr);
                ColumnID col_idx = col_expr->binding().column_idx;
                param_str += "COL" + std::to_string(col_idx);
                break;
            }
            case ExpressionType::kValue: {
                auto val_expr = std::static_pointer_cast<ValueExpression>(expr);
                const auto &val = val_expr->GetValue();
                param_str += val.ToString();
                break;
            }
            case ExpressionType::kCast: {
                auto cast_expr = std::static_pointer_cast<CastExpression>(expr);
                self(cast_expr->arguments()[0], param_str, self);
                break;
            }
            default: {
                RecoverableError(
                    Status::NotSupport(fmt::format("Not implemented ExtractFunctionInfo for expression type: {}", ExpressionType2Str(expr->type()))));
                break;
            }
        }
    };

    std::string col_params_str;
    const auto &func_args = arguments();

    for (size_t i = 0; i < func_args.size(); ++i) {
        if (i > 0)
            col_params_str += "_";
        extract_expr_info(func_args[i], col_params_str, extract_expr_info);
    }

    std::ostringstream oss;
    oss << func_name << "_" << col_params_str;
    return oss.str();
}

} // namespace infinity

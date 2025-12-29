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

module infinity_core:base_expression.impl;

import :base_expression;
import :column_expression;
import :function_expression;
import :reference_expression;
import :value_expression;
import :infinity_context;
import :function_set;
import :new_catalog;
import :scalar_function_set;
import :cast_expression;
import :cast_function;
import :bound_cast_func;

import std;

namespace infinity {

std::shared_ptr<BaseExpression> BaseExpression::Deserialize(std::string_view expression_str) {
    nlohmann::json j = nlohmann::json::parse(expression_str.begin(), expression_str.end());
    if (!j.contains("type")) {
        LOG_ERROR(fmt::format("BaseExpression::Deserialize failed: {}", expression_str));
        return nullptr;
    }
    LOG_TRACE(fmt::format("BaseExpression::Deserialize: {}", expression_str));

    ExpressionType expr_type = Str2ExpressionType(j["type"].get<std::string>());
    switch (expr_type) {
        case ExpressionType::kFunction: {
            std::string alias = j.contains("alias") ? j["alias"].get<std::string>() : "";
            std::vector<std::shared_ptr<BaseExpression>> arguments;
            if (j.contains("arguments")) {
                for (const auto &arg_j : j["arguments"]) {
                    arguments.push_back(BaseExpression::Deserialize(arg_j.dump()));
                }
            }
            std::string func_name = j["func_name"];

            auto catalog = InfinityContext::instance().storage()->new_catalog();
            std::shared_ptr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, func_name);
            auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
            ScalarFunction scalar_function = scalar_function_set_ptr->GetMostMatchFunction(arguments);

            auto func_expr = std::make_shared<FunctionExpression>(scalar_function, arguments);
            return func_expr;
        }
        case ExpressionType::kReference: {
            std::string alias = j.contains("alias") ? j["alias"].get<std::string>() : "";
            auto data_type = DataType::Deserialize(j["data_type"].get<nlohmann::json>().dump());
            std::string table_name = j["table_name"].get<std::string>();
            std::string column_name = j["column_name"].get<std::string>();
            i64 column_index = j["column_index"].get<i64>();

            auto ref_expr = ReferenceExpression::Make(*data_type, std::move(table_name), std::move(column_name), std::move(alias), column_index);
            return ref_expr;
        }
        case ExpressionType::kCast: {
            std::vector<std::shared_ptr<BaseExpression>> arguments;
            for (const auto &arg_j : j["arguments"]) {
                arguments.push_back(BaseExpression::Deserialize(arg_j.dump()));
            }
            auto target_type = DataType::Deserialize(j["target_type"].get<nlohmann::json>().dump());

            if (arguments.size() != 1) {
                RecoverableError(Status::NotSupport(fmt::format("Cast expression must have exactly one argument")));
            }
            BoundCastFunc cast(nullptr);
            switch (arguments[0]->type()) {
                case ExpressionType::kReference: {
                    std::shared_ptr<ReferenceExpression> ref_expr = std::dynamic_pointer_cast<ReferenceExpression>(arguments[0]);
                    DataType source_type = ref_expr->Type();
                    cast = CastFunction::GetBoundFunc(source_type, *target_type);
                    break;
                }
                default:
                    RecoverableError(Status::NotSupport(
                        fmt::format("Not support deserialize for cast of expression type: {}", ExpressionType2Str(arguments[0]->type()))));
            }

            auto cast_expr = std::make_shared<CastExpression>(cast, arguments[0], *target_type);
            return cast_expr;
        }
        case ExpressionType::kValue: {
            std::string value_str = j["value_str"].get<std::string>();
            auto value_type = DataType::Deserialize(j["value_type"].get<std::string>());
            Value value = Value::StringToValue(value_str, *value_type);

            auto col_expr = std::make_shared<ValueExpression>(std::move(value));
            return col_expr;
        }
        default: {
            RecoverableError(Status::NotSupport(fmt::format("Deserialize() is not implemented for expression type: {}", expression_str)));
            return nullptr;
        }
    }
}

} // namespace infinity

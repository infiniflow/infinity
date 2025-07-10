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

#include <parser/type/logical_type.h>
#include <sstream>

module scalar_function_set;

import stl;
import base_expression;
import scalar_function;
import embedding_info;
import infinity_exception;
import cast_table;
import logger;
import status;
import third_party;

namespace infinity {

ScalarFunctionSet::~ScalarFunctionSet() { functions_.clear(); }

void ScalarFunctionSet::AddFunction(const ScalarFunction &func) { functions_.emplace_back(func); }

ScalarFunction ScalarFunctionSet::GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>> &input_arguments) {

    i64 lowest_cost = std::numeric_limits<i64>::max();
    SizeT function_count = functions_.size();
    Vector<i64> candidates_index;

    for (SizeT i = 0; i < function_count; ++i) {
        ScalarFunction &function = functions_[i];
        i64 cost = MatchFunctionCost(function, input_arguments);
        if (cost >= 0 && cost <= lowest_cost) {
            // Have matched function and may be one of the candidate
            if (cost == lowest_cost) {
                candidates_index.emplace_back(i);
                continue;
            }
            lowest_cost = cost;
            candidates_index.clear();
            candidates_index.emplace_back(i);
        }
    }

    if (candidates_index.empty()) {
        // No matched function
        std::stringstream ss;
        String function_str = FunctionSet::ToString(name_, input_arguments);
        ss << "Can't find matched function for " << function_str;
        ss << " Candidate functions: ";
        for (auto &function : functions_) {
            ss << function.ToString() << std::endl;
        }
        LOG_ERROR(ss.str());
        Status status = Status::FunctionNotFound(function_str);
        RecoverableError(status);
    }

    if (candidates_index.size() > 1) {
        // multiple functions matched
        String function = FunctionSet::ToString(name_, input_arguments);
        std::stringstream ss;
        for (auto index : candidates_index) {
            ss << functions_[index].ToString() << std::endl;
        }
        String candicates = ss.str();
        RecoverableError(Status::MultipleFunctionMatched(function, candicates));
    }

    return functions_[candidates_index[0]];
}

i64 ScalarFunctionSet::MatchFunctionCost(const ScalarFunction &func, const Vector<SharedPtr<BaseExpression>> &arguments) {
    // TODO: variable argument list function need to handled here.

    if (func.parameter_types_.size() != arguments.size()) {
        // Argument count is mismatched.
        return -1; // Invalid function index
    }

    auto argument_count = arguments.size();
    i64 total_cost = 0;
    for (SizeT i = 0; i < argument_count; ++i) {
        const auto &arg_type = arguments[i]->Type();
        const auto &param_type = func.parameter_types_[i];

        // For tensor and embedding types, we need exact type info match, not just logical type match
        if ((arg_type.type() == LogicalType::kTensor || arg_type.type() == LogicalType::kEmbedding) &&
            (param_type.type() == LogicalType::kTensor || param_type.type() == LogicalType::kEmbedding)) {

            // Both are tensor/embedding types, check if they match exactly
            if (arg_type.type() != param_type.type()) {
                // Different logical types (tensor vs embedding)
                return -1;
            }

            // Same logical type, check type info
            const auto *arg_info = static_cast<const EmbeddingInfo *>(arg_type.type_info().get());
            const auto *param_info = static_cast<const EmbeddingInfo *>(param_type.type_info().get());

            if (arg_info->Type() != param_info->Type() || arg_info->Dimension() != param_info->Dimension()) {
                // Different embedding data type or dimension
                return -1;
            }

            // Exact match for tensor/embedding types
            total_cost += 0;
        } else {
            // For other types, use the standard cast cost
            i64 type_cast_cost = CastTable::instance().GetCastCost(arg_type.type(), param_type.type());
            if (type_cast_cost < 0) {
                // Can't cast the value type;
                return -1;
            } else {
                total_cost += type_cast_cost;
            }
        }
    }

    return total_cost;
}

} // namespace infinity
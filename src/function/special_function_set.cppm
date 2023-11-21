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

import stl;
import base_expression;
import function;
import function_set;
import parser;

export module special_function_set;

namespace infinity {

export class SpecialFunctionSet final : public FunctionSet {
public:
    explicit SpecialFunctionSet(String name, DataType return_type, i64 column_idx_delta)
        : FunctionSet(Move(name), FunctionType::kSpecial), return_type_(Move(return_type)), column_idx_delta_(column_idx_delta) {}

    ~SpecialFunctionSet() final = default;

    [[nodiscard]] const DataType& return_type() const {
        return return_type_;
    }

    [[nodiscard]] i64 column_idx_delta() const {
        return column_idx_delta_;
    }

    DataType return_type_;
    i64 column_idx_delta_{};
};

} // namespace infinity

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

import parser;
import function;
import base_expression;
//import catalog;

export module function_set;

namespace infinity {

struct NewCatalog;

export class FunctionSet {
public:
    static SharedPtr<FunctionSet> GetFunctionSet(NewCatalog *catalog, const FunctionExpr &expr);

    static String ToString(const String &name, const Vector<SharedPtr<BaseExpression>> &arguments);

public:
    explicit FunctionSet(String name, FunctionType type) : name_(Move(name)), type_(type) {}

    virtual ~FunctionSet() = default;

    [[nodiscard]] const String &name() const { return name_; }

    FunctionType type() const  {
        return type_;
    }

    String name_;
    FunctionType type_;
};

} // namespace infinity

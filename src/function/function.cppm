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

export module function;

import stl;

namespace infinity {

export enum class FunctionType {
    kInvalid,
    kScalar,
    kAggregate,
    kTable,
    kWindow,
    kSpecial,
};

export class Function {
public:
    explicit Function(String name, FunctionType type) : name_(std::move(name)), type_(type) {}

    virtual ~Function() = default;

    [[nodiscard]] inline FunctionType type() const { return type_; }

    [[nodiscard]] inline const String &name() const { return name_; }

    [[nodiscard]] virtual String ToString() const = 0;

protected:
    String name_;
    FunctionType type_{FunctionType::kInvalid};
};

} // namespace infinity

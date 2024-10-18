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

export module base_expression;


import stl;
import expression_type;
import data_type;
import internal_types;
import infinity_exception;
import third_party;

namespace infinity {

export enum class ExprSourceType {
    kInvalid,
    kAggregate,
    kGroupBy,
    kProjection,
    kBinding,
};

export struct SourcePosition {
    SourcePosition() = default;

    explicit SourcePosition(u64 bind_context_id, ExprSourceType source_type) : bind_context_id_(bind_context_id), source_type_(source_type) {}

    u64 bind_context_id_{std::numeric_limits<u64>::max()};
    ExprSourceType source_type_{ExprSourceType::kInvalid};
    String binding_name_{};
};

export class BaseExpression : public EnableSharedFromThis<BaseExpression> {
public:
    explicit BaseExpression(ExpressionType type, Vector<SharedPtr<BaseExpression>> arguments) : type_(type), arguments_(std::move(arguments)){};

    explicit BaseExpression(ExpressionType type, Vector<SharedPtr<BaseExpression>> arguments, String alias)
        : alias_(std::move(alias)), type_(type), arguments_(std::move(arguments)){};

    virtual ~BaseExpression() = default;

    inline String Name() const {
        if (alias_.empty()) {
            return ToString();
        } else {
            return alias_;
        }
    }

    virtual DataType Type() const = 0;

    [[nodiscard]] inline ExpressionType type() const { return type_; }

    inline Vector<SharedPtr<BaseExpression>> &arguments() { return arguments_; }

    SourcePosition source_position_{};
    String alias_{};

    [[nodiscard]] virtual String ToString() const = 0;

    virtual u64 Hash() const {
        UnrecoverableError(fmt::format("Not implemented {}'s Hash", int(type_)));
        return 0;
    }

    virtual bool Eq(const BaseExpression &other) const {
        UnrecoverableError(fmt::format("Not implemented {}'s Eq", int(type_)));
        return false;
    }

protected:
    ExpressionType type_{};
    Vector<SharedPtr<BaseExpression>> arguments_{};
};

} // namespace infinity

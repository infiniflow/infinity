//
// Created by JinHai on 2022/9/14.
//

module;

import stl;

export module function;

namespace infinity {

export enum class FunctionType {
    kInvalid,
    kScalar,
    kAggregate,
    kTable,
    kWindow,
};

export class Function {
public:
    explicit Function(String name, FunctionType type) : name_(Move(name)), type_(type) {}

    virtual ~Function() = default;

    [[nodiscard]] inline FunctionType type() const { return type_; }

    [[nodiscard]] inline const String &name() const { return name_; }

    [[nodiscard]] virtual String ToString() const = 0;

protected:
    String name_;
    FunctionType type_{FunctionType::kInvalid};
};

} // namespace infinity

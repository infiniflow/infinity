//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/types/internal_types.h"

#include <string>
#include <functional>

namespace infinity {

enum class FunctionType {
    kInvalid,
    kScalar,
    kAggregate,
    kTable,
    kWindow,
};

class Function {
public:
    explicit
    Function(String name, FunctionType type)
        : name_(std::move(name)),
        type_(type)
        {}

    virtual
    ~Function() = default;

    [[nodiscard]] inline FunctionType
    type() const {
        return type_;
    }

    [[nodiscard]] inline const String&
    name() const {
        return name_;
    }

    [[nodiscard]] virtual String
    ToString() const = 0;

protected:
    String name_;
    FunctionType type_{FunctionType::kInvalid};

};


}


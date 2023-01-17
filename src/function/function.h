//
// Created by JinHai on 2022/9/14.
//

#pragma once



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
    explicit Function(std::string name, FunctionType type) : name_(std::move(name)), type_(type) {}
    virtual ~Function() = default;
    [[nodiscard]] FunctionType type() const { return type_; }
    [[nodiscard]] const std::string& name() const { return name_; }

    virtual std::string ToString() const = 0;
protected:
    std::string name_;
    FunctionType type_{FunctionType::kInvalid};

};


}


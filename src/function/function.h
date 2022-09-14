//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "storage/catalog.h"
#include "function_set.h"

#include <string>

namespace infinity {

enum class FunctionType {
    kInvalid,
    kScalar,
    kAggregate,
    kTable
};

class Function {
public:
    explicit Function(std::string name) : name_(std::move(name)) {}
    virtual ~Function() = default;
    FunctionType type() const { return type_; }
    const std::string& name() const { return name_; }

private:
    std::string name_;
    FunctionType type_;

};

class BuiltinFunctions : public std::enable_shared_from_this<BuiltinFunctions> {
public:
    explicit BuiltinFunctions(const std::unique_ptr<Catalog>& catalog_ptr);

    void Init();


private:
    std::unique_ptr<Catalog>& catalog_ptr_;

private:
    void AddFunctionSet(FunctionSet& function_set);

    void RegisterAggregateFunction();
    void RegisterScalarFunction();
    void RegisterTableFunction();
};


}


//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function_set.h"
#include "table_function.h"

namespace infinity {

class TableFunctionSet final : public FunctionSet {
public:
    explicit TableFunctionSet(String name) : FunctionSet(std::move(name), FunctionType::kTable) {}

    ~TableFunctionSet() final;

    void AddFunction(TableFunction func);

private:
    Vector<TableFunction> functions_;
};

} // namespace infinity

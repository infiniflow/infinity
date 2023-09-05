//
// Created by JinHai on 2022/12/14.
//

#pragma once

#include "function_set.h"

namespace infinity {

class TableFunctionSet: public FunctionSet {
public:
    explicit
    TableFunctionSet(String name): FunctionSet(std::move(name), FunctionType::kTable) {}

    void
    AddFunction(const TableFunction& func);

private:
    Vector<TableFunction> functions_;
};

}

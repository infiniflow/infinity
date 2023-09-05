//
// Created by JinHai on 2022/12/14.
//

#include "table_function_set.h"
#include "table_function.h"

namespace infinity {

void
TableFunctionSet::AddFunction(const TableFunction& func) {
    functions_.emplace_back(func);
}

}

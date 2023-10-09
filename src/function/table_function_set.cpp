//
// Created by JinHai on 2022/12/14.
//

#include "table_function_set.h"

namespace infinity {

TableFunctionSet::~TableFunctionSet() { functions_.clear(); }

void TableFunctionSet::AddFunction(TableFunction func) { functions_.emplace_back(func); }

} // namespace infinity

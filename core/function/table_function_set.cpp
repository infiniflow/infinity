//
// Created by JinHai on 2022/12/14.
//

module;

import stl;
import table_function;

module table_function_set;

namespace infinity {

TableFunctionSet::~TableFunctionSet() { functions_.clear(); }

void TableFunctionSet::AddFunction(TableFunction func) { functions_.emplace_back(func); }

} // namespace infinity

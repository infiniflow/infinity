//
// Created by JinHai on 2022/9/15.
//

#include "function_set.h"

namespace infinity {

void
AggregateFunctionSet::AddFunction(const infinity::AggregateFunction& func) {
    functions_.emplace_back(func);
}

void
ScalarFunctionSet::AddFunction(const infinity::ScalarFunction& func) {
    functions_.emplace_back(func);
}

void
TableFunctionSet::AddFunction(const infinity::TableFunction& func) {
    functions_.emplace_back(func);
}

}


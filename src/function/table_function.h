//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function/function.h"
#include "function/function_data.h"
#include "common/types/logical_type.h"

#include <functional>

namespace infinity {

class QueryContext;
class DataBlock;

class TableFunctionData : public FunctionData {
public:
};

using TableFunctionType = std::function<void(QueryContext *query_context, TableFunctionData *data, DataBlock &output)>;

class TableFunction : public Function {
public:
    explicit TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function);

    [[nodiscard]] String ToString() const override;

    TableFunctionType main_function_;

private:
    Vector<LogicalType> parameter_types_;
};
} // namespace infinity

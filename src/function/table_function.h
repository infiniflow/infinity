//
// Created by JinHai on 2022/9/14.
//

#pragma once
#include "function.h"
#include "function_data.h"
#include "storage/data_block.h"
#include "main/query_context.h"

namespace infinity {

class TableFunctionData : public FunctionData {
public:

};

using TableFunctionType
    = std::function<void(std::shared_ptr<QueryContext>& query_context, std::shared_ptr<TableFunctionData> data, DataBlock &output)>;


class TableFunction : public Function {
public:
    explicit TableFunction(String name,
                           Vector<LogicalType> argument_types,
                           TableFunctionType function
                           );

    [[nodiscard]] String
    ToString() const override;

    TableFunctionType main_function_;

private:
    Vector<LogicalType> parameter_types_;
};
}


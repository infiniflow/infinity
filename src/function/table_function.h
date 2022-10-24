//
// Created by JinHai on 2022/9/14.
//

#pragma once
#include "function.h"
#include "function_data.h"
#include "storage/data_type.h"
#include "storage/transblock.h"
#include "main/query_context.h"
#include <vector>

namespace infinity {

class TableFunctionData : public FunctionData {
public:

};

using TableFunctionType
    = std::function<void(std::shared_ptr<QueryContext>& query_context, std::shared_ptr<TableFunctionData> data, TransBlock &output)>;


class TableFunction : public Function {
public:
    explicit TableFunction(std::string name,
                           std::vector<LogicalType> argument_types,
                           TableFunctionType function
                           );

    [[nodiscard]] std::string
    ToString() const override;

    TableFunctionType main_function_;

private:
    std::vector<LogicalType> parameter_types_;
};
}


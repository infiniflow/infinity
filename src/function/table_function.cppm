//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import function;
import function_data;
import parser;
//import query_context;
import data_block;

export module table_function;

namespace infinity {

class QueryContext;
//class DataBlock;

export class TableFunctionData : public FunctionData {
public:
};

export using TableFunctionType = StdFunction<void(QueryContext *query_context, TableFunctionData *data, DataBlock &output)>;

export class TableFunction : public Function {
public:
    explicit TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function);

    [[nodiscard]] String ToString() const override;

    TableFunctionType main_function_;

private:
    Vector<LogicalType> parameter_types_;
};
} // namespace infinity

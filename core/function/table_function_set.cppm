//
// Created by JinHai on 2022/12/14.
//

module;

import function_set;
import stl;
import table_function;
import function;

export module table_function_set;

namespace infinity {

class TableFunctionSet final : public FunctionSet {
public:
    explicit TableFunctionSet(String name) : FunctionSet(Move(name), FunctionType::kTable) {}

    ~TableFunctionSet() final;

    void AddFunction(TableFunction func);

private:
    Vector<TableFunction> functions_;
};

} // namespace infinity

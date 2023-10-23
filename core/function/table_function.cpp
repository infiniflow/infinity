//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import function;
import parser;
import query_context;

module table_function;
namespace infinity {

TableFunction::TableFunction(String name, Vector<LogicalType> argument_types, TableFunctionType function)
    : Function(Move(name), FunctionType::kTable), parameter_types_(Move(argument_types)), main_function_(Move(function)) {}

String TableFunction::ToString() const {
//    std::stringstream ss;
//    ss << "Table Function: " << name_;
//    return ss.str();
}

} // namespace infinity

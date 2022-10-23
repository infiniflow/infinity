//
// Created by JinHai on 2022/9/14.
//

#include "table_function.h"
#include <sstream>
#include <utility>

namespace infinity {

TableFunction::TableFunction(std::string name,
                             std::vector<LogicalType> argument_types,
                             TableFunctionType function)
                           : Function(std::move(name), FunctionType::kTable),
                             parameter_types_(std::move(argument_types)),
                             main_function_(std::move(function))
{}

std::string
TableFunction::ToString() const {
    std::stringstream ss;
    ss << "Table Function: " << name_;
    return ss.str();
}


}

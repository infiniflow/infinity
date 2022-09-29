//
// Created by JinHai on 2022/9/14.
//

#include "table_function.h"
#include <sstream>

namespace infinity {

TableFunction::TableFunction(std::string name,
                               std::vector<LogicalType> argument_types)
                               : Function(std::move(name), FunctionType::kTable),
                                 parameter_types_(std::move(argument_types))
{}

std::string
TableFunction::ToString() {
    std::stringstream ss;
    ss << "Table Function: " << name_ << std::endl;
    return ss.str();
}


}

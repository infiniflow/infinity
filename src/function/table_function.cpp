//
// Created by JinHai on 2022/9/14.
//

#include "table_function.h"

namespace infinity {

TableFunction::TableFunction(std::string name,
                               std::vector<LogicalType> argument_types)
                               : Function(std::move(name), FunctionType::kTable),
                               argument_types_(std::move(argument_types))
{}


}

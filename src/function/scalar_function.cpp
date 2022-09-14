//
// Created by JinHai on 2022/9/14.
//

#include "scalar_function.h"

namespace infinity {

ScalarFunction::ScalarFunction(std::string name,
                               std::vector<LogicalType> argument_types,
                               LogicalType& return_type)
                               : Function(std::move(name), FunctionType::kScalar),
                               argument_types_(std::move(argument_types)),
                               return_type_(std::move(return_type))
{}


}

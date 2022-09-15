//
// Created by JinHai on 2022/9/14.
//

#include "scalar_function.h"

namespace infinity {

ScalarFunction::ScalarFunction(std::string name,
                               std::vector<LogicalType> argument_types,
                               LogicalType return_type)
                               : Function(std::move(name), FunctionType::kScalar),
                               argument_types_(std::move(argument_types)),
                               return_type_(return_type)
{}

void
ScalarFunction::CastArgumentTypes(std::vector<BaseExpression>& input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type
}


}

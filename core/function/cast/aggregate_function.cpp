//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import base_expression;
import infinity_assert;
import infinity_exception;
import third_party;

module aggregate_function;

namespace infinity {

void AggregateFunction::CastArgumentTypes(BaseExpression &input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type

    Error<PlannerException>("Not implemented: need to cast the argument types", __FILE_NAME__, __LINE__);
}

String AggregateFunction::ToString() const {
    String res = Format("{}({})->{}", name_, argument_type_.ToString(), return_type().ToString());
}

} // namespace infinity
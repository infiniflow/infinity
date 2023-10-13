//
// Created by JinHai on 2022/7/26.
//

#include "physical_operator.h"

namespace infinity {

String PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

} // namespace infinity

//
// Created by JinHai on 2022/7/26.
//

module;

import stl;
import physical_operator_type;

module physical_operator;

namespace infinity {

String PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

} // namespace infinity

//
// Created by JinHai on 2022/7/28.
//

module;

import query_context;
import operator_state;

module physical_dummy_operator;

namespace infinity {

void PhysicalDummyOperator::Init() {}

void PhysicalDummyOperator::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {}

void PhysicalDummyOperator::Execute(QueryContext *query_context) {}

} // namespace infinity
//
// Created by JinHai on 2022/10/25.
//

module;

import query_context;
import operator_state;

module physical_dummy_scan;

namespace infinity {

void PhysicalDummyScan::Init() {}

void PhysicalDummyScan::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {}

void PhysicalDummyScan::Execute(QueryContext *query_context) {}

} // namespace infinity

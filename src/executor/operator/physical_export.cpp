//
// Created by JinHai on 2022/7/28.
//

module;

import query_context;
import operator_state;

module physical_export;

namespace infinity {

void PhysicalExport::Init() {}

void PhysicalExport::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) { output_state->SetComplete(); }

void PhysicalExport::Execute(QueryContext *query_context) {}

void PhysicalExport::ExportCSV(QueryContext *query_context) {}

void PhysicalExport::ExportJSON(QueryContext *query_context) {}

} // namespace infinity

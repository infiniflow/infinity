module;

module invert_task;

import stl;
import column_inverter;
import column_vector;
import parser;

namespace infinity {

InvertTask::InvertTask(ColumnInverter *inverter, SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids)
    : inverter_(inverter), column_vector_(column_vector), row_ids_(row_ids) {}

void InvertTask::Run() {
    // inverter_->InvertColumn(column_vector_, row_ids_);
}

} // namespace infinity

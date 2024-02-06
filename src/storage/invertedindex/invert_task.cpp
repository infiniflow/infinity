module;

module invert_task;

import stl;
import column_inverter;
import column_vector;

import internal_types;

namespace infinity {

InvertTask::InvertTask(ColumnInverter *inverter, const String &value, u32 row_id) : inverter_(inverter), value_(value), row_id_(row_id) {}

void InvertTask::Run() { inverter_->InvertColumn(row_id_, value_); }

BatchInvertTask::BatchInvertTask(ColumnInverter *inverter, SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids)
    : inverter_(inverter), column_vector_(column_vector), row_ids_(row_ids) {}

void BatchInvertTask::Run() { inverter_->InvertColumn(column_vector_, row_ids_); }

} // namespace infinity

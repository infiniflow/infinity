module;

module commit_task;

import stl;
import column_inverter;

namespace infinity {

CommitTask::CommitTask(ColumnInverter *inverter) : inverter_(inverter) {}

void CommitTask::Run() { inverter_->Commit(); }

} // namespace infinity

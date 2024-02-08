module;

module commit_task;

import stl;
import column_inverter;

namespace infinity {

CommitTask::CommitTask(ColumnCommitter *inverter) : inverter_(inverter) { inverter_->Retain(); }

CommitTask::~CommitTask() { inverter_->Release(); }

void CommitTask::Run() { inverter_->Commit(); }

} // namespace infinity

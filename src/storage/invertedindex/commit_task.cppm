module;

export module commit_task;

import stl;
import task_executor;
import column_inverter;
import parser;

namespace infinity {
export class CommitTask : public TaskExecutor::Task {
public:
    CommitTask(ColumnInverter *inverter);

    ~CommitTask();

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};
};
} // namespace infinity

module;

export module commit_task;

import stl;
import task_executor;
import column_inverter;

namespace infinity {
export class CommitTask : public TaskExecutor::Task {
public:
    CommitTask(ColumnCommitter *inverter);

    ~CommitTask();

    void Run() override;

private:
    ColumnCommitter *inverter_{nullptr};
};
} // namespace infinity

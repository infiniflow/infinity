module;

export module commit_task;

import stl;
import task_executor;
import column_inverter;

namespace infinity {
export class CommitTask : public TaskExecutor::Task {
public:
    CommitTask(InverterReference *inverter);

    ~CommitTask();

    void Run() override;

private:
    InverterReference *inverter_{nullptr};
};
} // namespace infinity

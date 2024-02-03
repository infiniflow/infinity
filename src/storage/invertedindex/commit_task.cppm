module;

export module commit_task;

import stl;
import task_executor;
import sequential_column_inverter;
import parser;

namespace infinity {
export class CommitTask : public TaskExecutor::Task {
public:
    CommitTask(SequentialColumnInverter *inverter);

    ~CommitTask();

    void Run() override;

private:
    SequentialColumnInverter *inverter_{nullptr};
};
} // namespace infinity

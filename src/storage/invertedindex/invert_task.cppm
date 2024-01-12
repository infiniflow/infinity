module;

export module invert_task;

import stl;
import task_executor;
import column_inverter;

namespace infinity {
export class InvertTask : public TaskExecutor::Task {
public:
    InvertTask();

    ~InvertTask() = default;

    void Run() override;

private:
    Vector<UniquePtr<ColumnInverter>> inverters_;
};
} // namespace infinity

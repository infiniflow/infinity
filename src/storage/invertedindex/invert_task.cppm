module;

export module invert_task;

import stl;
import task_executor;
import column_inverter;
import column_vector;
import parser;

namespace infinity {
export class InvertTask : public TaskExecutor::Task {
public:
    InvertTask(ColumnInverter *inverter, SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    ~InvertTask() = default;

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};

    SharedPtr<ColumnVector> column_vector_;

    Vector<RowID> &row_ids_;
};
} // namespace infinity

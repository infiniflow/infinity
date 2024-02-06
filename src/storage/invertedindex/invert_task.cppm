module;

export module invert_task;

import stl;
import task_executor;
import column_inverter;
import column_vector;

import internal_types;

namespace infinity {

export class InvertTask : public TaskExecutor::Task {
public:
    explicit InvertTask(ColumnInverter *inverter, const String &value, u32 row_id);

    ~InvertTask() = default;

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};

    const String &value_;

    u32 row_id_;
};

export class BatchInvertTask : public TaskExecutor::Task {
public:
    BatchInvertTask(ColumnInverter *inverter, SharedPtr<ColumnVector> column_vector, RowID start_row_id);

    ~BatchInvertTask() = default;

    void Run() override;

private:
    ColumnInverter *inverter_{nullptr};

    SharedPtr<ColumnVector> column_vector_;

    RowID start_row_id_;
};
} // namespace infinity

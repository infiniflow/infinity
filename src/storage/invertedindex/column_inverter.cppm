module;

import stl;
import task_executor;
export module column_inverter;

namespace infinity {
export class ColumnInverter : public TaskExecutor::Task {
public:
    ColumnInverter();
    ~ColumnInverter();
};
} // namespace infinity

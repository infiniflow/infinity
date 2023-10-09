//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "storage/data_block.h"

namespace infinity {

enum class TaskResultType { kInvalid };

class TaskResult {
public:
    explicit TaskResult(TaskResultType task_result_type) : task_result_type_(task_result_type) {}

    void Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity);

    void Seal();

private:
    UniquePtr<DataBlock> data_{};
    TaskResultType task_result_type_{kInvalid};
};

} // namespace infinity

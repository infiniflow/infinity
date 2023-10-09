//
// Created by jinhai on 23-9-8.
//

#include "task_result.h"

namespace infinity {

void TaskResult::Init(const Vector<SharedPtr<DataType>> &types, SizeT capacity) {
    data_ = MakeUnique<DataBlock>();
    data_->Init(types, capacity);
}

void TaskResult::Seal() { data_->Finalize(); }

} // namespace infinity
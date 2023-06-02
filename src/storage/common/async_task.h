//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

enum class AsyncTaskType {
    kTerminate,
    kDummy,
    kInvalid,
};

class AsyncTask {
public:
    explicit
    AsyncTask(AsyncTaskType task_type): task_type_(task_type) {}

    virtual
    ~AsyncTask() = default;

    [[nodiscard]] inline bool
    IsTerminated() const {
        return task_type_ == AsyncTaskType::kTerminate;
    }

    virtual String
    ToString() = 0;

private:
    AsyncTaskType task_type_{AsyncTaskType::kInvalid};

};

}

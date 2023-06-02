//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "async_task.h"

namespace infinity {

class AsyncTerminateTask final : public AsyncTask {
public:
    explicit
    AsyncTerminateTask(): AsyncTask(AsyncTaskType::kTerminate) {}

    String
    ToString() final {
        return "Async Terminate Task";
    }
private:

};

}

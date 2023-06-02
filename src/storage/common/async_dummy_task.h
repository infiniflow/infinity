//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "async_task.h"

namespace infinity {

class AsyncDummyTask final : public AsyncTask {
public:
    explicit
    AsyncDummyTask(): AsyncTask(AsyncTaskType::kDummy) {}

    String
    ToString() final {
        return "Async Dummy Task";
    }
private:

};

}

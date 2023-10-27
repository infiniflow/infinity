//
// Created by jinhai on 23-6-2.
//

module;

import stl;
import async_task;

export module async_terminate_task;

namespace infinity {

export class AsyncTerminateTask final : public AsyncTask {
public:
    explicit AsyncTerminateTask() : AsyncTask(AsyncTaskType::kTerminate) {}

    String ToString() final { return "Async Terminate Task"; }

    void Prepare() final {}

    void Commit() final {}

private:
};

} // namespace infinity

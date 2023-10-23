//
// Created by jinhai on 23-6-2.
//

module;

import stl;
import async_task;

export module async_dummy_task;

namespace infinity {

export class AsyncDummyTask final : public AsyncTask {
public:
    explicit AsyncDummyTask() : AsyncTask(AsyncTaskType::kDummy) {}

    String ToString() final { return "Async Dummy Task"; }

    void Prepare() final {}

    void Commit() final{};

private:
};

} // namespace infinity

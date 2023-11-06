// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

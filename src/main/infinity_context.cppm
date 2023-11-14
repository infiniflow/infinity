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

export module infinity_context;

import stl;
import config;
import resource_manager;
import new_task_scheduler;
import storage;
import singleton;

namespace infinity {

export class InfinityContext : public Singleton<InfinityContext> {
public:
    [[nodiscard]] inline TaskScheduler*
    task_scheduler() noexcept { return task_scheduler_.get(); }

    [[nodiscard]] inline Config*
    config() noexcept { return config_.get(); }

    [[nodiscard]] inline Storage*
    storage() noexcept { return storage_.get(); }

    [[nodiscard]] inline ResourceManager*
    resource_manager() noexcept { return resource_manager_.get(); }

    void
    Init(const SharedPtr <String>& config_path);

    void
    UnInit();

private:
    friend class Singleton;

    InfinityContext() = default;

    UniquePtr <Config> config_{};
    UniquePtr <ResourceManager> resource_manager_{};
    UniquePtr <TaskScheduler> task_scheduler_{};
    UniquePtr <Storage> storage_{};

    bool initialized_{false};
};

}


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

module backgroud_process;

import stl;
import bg_task;
import logger;
import blocking_queue;
import infinity_exception;
import wal_manager;

namespace infinity {

BGTaskProcessor::BGTaskProcessor(WalManager *wal_manager) : wal_manager_(wal_manager) {}

void BGTaskProcessor::Start() {
    LOG_INFO("Start the background processor.");
    processor_thread_ = Thread([this] { Process(); });
}

void BGTaskProcessor::Stop() {
    UniquePtr<StopProcessorTask> stop_task = MakeUnique<StopProcessorTask>();
    StopProcessorTask* stop_task_ptr = stop_task.get();
    task_queue_.Enqueue(Move(stop_task));
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Shutdown the background processor.");
}

void BGTaskProcessor::Submit(UniquePtr<BGTask> bg_task) { task_queue_.Enqueue(Move(bg_task)); }

void BGTaskProcessor::Process() {
    UniquePtr<BGTask> bg_task{nullptr};
    bool running{true};
    while (running) {
        bg_task = task_queue_.DequeueReturn();

        switch (bg_task->type_) {
            case BGTaskType::kTryCheckpoint: {
                wal_manager_->Checkpoint();
                break;
            }
            case BGTaskType::kForceCheckpoint: {
                ForceCheckpointTask *force_ckp_task = (ForceCheckpointTask *)(bg_task.get());
                wal_manager_->Checkpoint(force_ckp_task);
                break;
            }
            case BGTaskType::kStopProcessor: {
                running = false;
                break;
            }
            case BGTaskType::kInvalid: {
                Error<StorageException>("Invalid background task");
                break;
            }
        }

        bg_task->Complete();
    }
}

} // namespace infinity

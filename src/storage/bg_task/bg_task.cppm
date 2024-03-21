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
import txn;
import catalog;
import catalog_delta_entry;


export module bg_task;

namespace infinity {

export enum class BGTaskType {
    kForceCheckpoint, // Manually triggered by PhysicalFlush
    kStopProcessor,
    kCompactSegments,
    kCleanup,
    kUpdateSegmentBloomFilterData,
    kInvalid
};

export struct BGTask {
    BGTask(BGTaskType type, bool async) : type_(type), async_(async) {}

    virtual ~BGTask() = default;

    BGTaskType type_{BGTaskType::kInvalid};
    bool async_{false};

    bool complete_{false};
    std::mutex mutex_{};
    std::condition_variable cv_{};

    void Wait() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        cv_.wait(locker, [this] { return complete_; });
    }

    void Complete() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        complete_ = true;
        cv_.notify_one();
    }

    virtual String ToString() const = 0;
};

export struct StopProcessorTask final : public BGTask {
    StopProcessorTask() : BGTask(BGTaskType::kStopProcessor, false) {}

    ~StopProcessorTask() = default;

    String ToString() const final { return "Stop Task"; }
};

export struct ForceCheckpointTask final : public BGTask {
    explicit ForceCheckpointTask(Txn *txn) : BGTask(BGTaskType::kForceCheckpoint, false), txn_(txn) {}
    explicit ForceCheckpointTask(Txn *txn, bool is_full_checkpoint)
        : BGTask(BGTaskType::kForceCheckpoint, false), txn_(txn), is_full_checkpoint_(is_full_checkpoint) {}

    ~ForceCheckpointTask() = default;

    String ToString() const final { return "Force Checkpoint Task"; }

    Txn *txn_{};
    bool is_full_checkpoint_{true};
};

} // namespace infinity

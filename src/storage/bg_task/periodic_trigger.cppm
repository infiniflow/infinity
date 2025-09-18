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

export module infinity_core:periodic_trigger;

import :background_process;
import :compaction_process;
import :wal_manager;

import std;

import global_resource_usage;

namespace infinity {

class NewCleanupTask;
class NotifyOptimizeTask;
class NotifyCompactTask;

export class PeriodicTrigger {
public:
    explicit PeriodicTrigger(const i64 interval) : interval_(interval), last_check_(std::chrono::system_clock::now()) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PeriodicTrigger");
#endif
    }

    virtual ~PeriodicTrigger() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PeriodicTrigger");
#endif
    }

    bool Check();

    void UpdateInternal(const i64 new_interval) { interval_ = new_interval; }

    virtual void Trigger() = 0;

    void Reset() { last_check_ = std::chrono::system_clock::now(); }

protected:
    std::atomic_int64_t interval_{};
    std::chrono::system_clock::time_point last_check_{};
    std::atomic_int64_t duration_{0};
};

export class NewCleanupPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit NewCleanupPeriodicTrigger(const i64 interval) : PeriodicTrigger(interval) {}

    std::shared_ptr<NewCleanupTask> CreateNewCleanupTask();

    void Trigger() override;

private:
    //
};

export class CheckpointPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit CheckpointPeriodicTrigger(const i64 interval) : PeriodicTrigger(interval) {}

    void Trigger() override;
};

export class CompactPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit CompactPeriodicTrigger(const i64 interval, CompactionProcessor *compact_processor)
        : PeriodicTrigger(interval), compact_processor_(compact_processor) {}

    explicit CompactPeriodicTrigger(const i64 interval) : PeriodicTrigger(interval) {}

    void Trigger() override;

private:
    CompactionProcessor *const compact_processor_{};
    std::shared_ptr<NotifyCompactTask> compact_task_{};
};

export class OptimizeIndexPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit OptimizeIndexPeriodicTrigger(const i64 interval, CompactionProcessor *compact_processor)
        : PeriodicTrigger(interval), compact_processor_(compact_processor) {}

    explicit OptimizeIndexPeriodicTrigger(const i64 interval) : PeriodicTrigger(interval) {}

    void Trigger() override;

private:
    CompactionProcessor *const compact_processor_ = nullptr;
    std::shared_ptr<NotifyOptimizeTask> optimize_task_{};
};

} // namespace infinity

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

export module periodic_trigger;

import stl;
import background_process;
import compaction_process;
import catalog;
import wal_manager;
import buffer_manager;

namespace infinity {

export class PeriodicTrigger {
public:
    explicit PeriodicTrigger(std::chrono::milliseconds interval) : interval_(interval), last_check_(std::chrono::system_clock::now()) {}

    virtual ~PeriodicTrigger() = default;

    bool Check() {
        const auto now = std::chrono::system_clock::now();
        if (now - last_check_ < interval_) {
            return false;
        }
        last_check_ = now;
        return true;
    }

    virtual void Trigger() = 0;

    void Reset() { last_check_ = std::chrono::system_clock::now(); }

private:
    const std::chrono::milliseconds interval_;
    std::chrono::system_clock::time_point last_check_;
};

export class CleanupPeriodicTrigger final : public PeriodicTrigger {
public:
    CleanupPeriodicTrigger(std::chrono::milliseconds interval,
                           BGTaskProcessor *bg_processor,
                           Catalog *catalog,
                           WalManager *wal_mgr,
                           BufferManager *buffer_mgr)
        : PeriodicTrigger(interval), bg_processor_(bg_processor), catalog_(catalog), wal_mgr_(wal_mgr), buffer_mgr_(buffer_mgr) {}

    virtual void Trigger() override;

private:
    BGTaskProcessor *const bg_processor_{};
    Catalog *const catalog_{};
    WalManager *const wal_mgr_{};
    BufferManager *const buffer_mgr_{};

    TxnTimeStamp last_visible_ts_{0};
};

export class CheckpointPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit CheckpointPeriodicTrigger(std::chrono::milliseconds interval, WalManager *wal_mgr, bool full_checkpoint)
        : PeriodicTrigger(interval), wal_mgr_(wal_mgr), is_full_checkpoint_(full_checkpoint) {}

    virtual void Trigger() override;

private:
    WalManager *const wal_mgr_{};
    bool is_full_checkpoint_{};
};

export class CompactSegmentPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit CompactSegmentPeriodicTrigger(std::chrono::seconds interval, CompactionProcessor *compact_processor)
        : PeriodicTrigger(interval), compact_processor_(compact_processor) {}

    virtual void Trigger() override;

private:
    CompactionProcessor *const compact_processor_{};
};

export class OptimizeIndexPeriodicTrigger final : public PeriodicTrigger {
public:
    explicit OptimizeIndexPeriodicTrigger(std::chrono::seconds interval, CompactionProcessor *compact_processor)
        : PeriodicTrigger(interval), compact_processor_(compact_processor) {}

    virtual void Trigger() override;

private:
    CompactionProcessor *const compact_processor_{};
};

} // namespace infinity

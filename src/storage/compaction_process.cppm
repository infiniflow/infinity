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

export module infinity_core:compaction_process;

import :bg_task_type;
import :blocking_queue;
import :status;

namespace infinity {

class NewTxn;
class BGTask;
class TableMeeta;

class TestCommander {
public:
    void Add(BGTaskType type, const std::string &command) {
        std::lock_guard<std::mutex> lock(mtx_);
        test_commands_[type] = command;
    }

    std::optional<std::string> Check(BGTaskType type) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto iter = test_commands_.find(type);
        if (iter != test_commands_.end()) {
            std::string res = iter->second;
            test_commands_.erase(iter);
            return res;
        }
        return std::nullopt;
    }

private:
    std::unordered_map<BGTaskType, std::string> test_commands_;
    std::mutex mtx_;
};

export class CompactionProcessor {
public:
    CompactionProcessor();
    ~CompactionProcessor();

    void Start();

    void Stop();

    void Submit(std::shared_ptr<BGTask> bg_task);

    void NewDoCompact();

    Status NewManualCompact(const std::string &db_name, const std::string &table_name);

    u64 RunningTaskCount() const { return task_count_; }

    void AddTestCommand(BGTaskType type, const std::string &command) { test_commander_.Add(type, command); }

private:
    void NewScanAndOptimize();

    void Process();

    std::vector<SegmentID> GetCompactableSegments(TableMeeta &table_meta, const std::vector<SegmentID> &segment_ids);

private:
    BlockingQueue<std::shared_ptr<BGTask>> task_queue_{"CompactionProcessor"};

    std::thread processor_thread_{};

    std::atomic<u64> task_count_{};

    TestCommander test_commander_;
};

} // namespace infinity
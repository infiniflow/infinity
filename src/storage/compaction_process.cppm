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

export module infinity_core:compaction_process;

import :stl;
import :bg_task_type;
import :blocking_queue;
import :status;
import :kv_store;

namespace infinity {

class NewTxn;
class BGTask;
class TableMeeta;

class TestCommander {
public:
    void Add(BGTaskType type, const String &command) {
        std::lock_guard<std::mutex> lock(mtx_);
        test_commands_[type] = command;
    }

    Optional<String> Check(BGTaskType type) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto iter = test_commands_.find(type);
        if (iter != test_commands_.end()) {
            String res = iter->second;
            test_commands_.erase(iter);
            return res;
        }
        return None;
    }

private:
    HashMap<BGTaskType, String> test_commands_;
    std::mutex mtx_;
};

export class CompactionProcessor {
public:
    CompactionProcessor();
    ~CompactionProcessor();

    void Start();

    void Stop();

    void Submit(SharedPtr<BGTask> bg_task);

    void NewDoCompact();

    Status NewManualCompact(const String &db_name, const String &table_name);

    u64 RunningTaskCount() const { return task_count_; }

    void AddTestCommand(BGTaskType type, const String &command) { test_commander_.Add(type, command); }

private:
    void NewScanAndOptimize();

    void Process();

    Vector<SegmentID> GetCompactableSegments(TableMeeta &table_meta,
                                             KVInstance *kv_instance,
                                             TxnTimeStamp begin_ts,
                                             TxnTimeStamp commit_ts,
                                             const Vector<SegmentID> &segment_ids);

private:
    BlockingQueue<SharedPtr<BGTask>> task_queue_{"CompactionProcessor"};

    Thread processor_thread_{};

    Atomic<u64> task_count_{};

    TestCommander test_commander_;
};

} // namespace infinity
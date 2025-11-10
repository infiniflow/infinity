// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:optimization_process;

import :bg_task_type;
import :blocking_queue;
import :status;

namespace infinity {

class NewTxn;
class BGTask;
class TableMeta;

export class OptimizationProcessor {
public:
    OptimizationProcessor();
    ~OptimizationProcessor();

    void Start();

    void Stop();

    void Submit(const std::shared_ptr<BGTask> &bg_task);

    u64 RunningTaskCount() const { return task_count_; }

private:
    void NewNotifyOptimize();

    Status NewManualOptimize(NewTxn *new_txn, const std::string &db_name, const std::string &table_name);

    void Process();

private:
    BlockingQueue<std::shared_ptr<BGTask>> task_queue_{"OptimizationProcessor"};

    std::thread processor_thread_{};

    std::atomic<u64> task_count_{};
};

} // namespace infinity
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

export module compaction_process;

import stl;
import txn;
import bg_task;
import blocking_queue;
import base_statement;

namespace infinity {

class Catalog;
class TxnManager;
class SessionManager;

export class CompactionProcessor {
public:
    CompactionProcessor(Catalog *catalog, TxnManager *txn_mgr);
    ~CompactionProcessor();

    void Start();

    void Stop();

    void Submit(SharedPtr<BGTask> bg_task);

    void DoCompact();

    u64 RunningTaskCount() const { return task_count_; }

    TxnTimeStamp ManualDoCompact(const String &schema_name,
                                 const String &table_name,
                                 bool rollback,
                                 Optional<std::function<void()>> mid_func = None); // false unit test

private:
    Vector<Pair<UniquePtr<BaseStatement>, Txn *>> ScanForCompact(Txn *scan_txn);

    void ScanAndOptimize();

    void DoDump(DumpIndexTask *dump_task);

    void DoDumpByline(DumpIndexBylineTask *dump_task);

    void Process();

private:
    BlockingQueue<SharedPtr<BGTask>> task_queue_{"CompactionProcessor"};

    Thread processor_thread_{};

    Catalog *catalog_{};
    TxnManager *txn_mgr_{};
    SessionManager *session_mgr_{};

    Atomic<u64> task_count_{};
};

} // namespace infinity
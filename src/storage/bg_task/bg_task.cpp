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

module bg_task;

import base_memindex;
import chunk_index_entry;
import cleanup_scanner;
import infinity_context;
import storage;

namespace infinity {

void CleanupTask::Execute() {
    auto *storage = InfinityContext::instance().storage();
    CleanupScanner scanner(catalog_, visible_ts_, buffer_mgr_);
    scanner.Scan();

    auto *tracer = storage->cleanup_info_tracer();
    tracer->ResetInfo(visible_ts_);
    std::move(scanner).Cleanup(tracer);
}

DumpIndexTask::DumpIndexTask(BaseMemIndex *mem_index, Txn *txn) : BGTask(BGTaskType::kDumpIndex, true), mem_index_(mem_index), txn_(txn) {}

DumpIndexBylineTask::DumpIndexBylineTask(SharedPtr<String> db_name,
                                         SharedPtr<String> table_name,
                                         SharedPtr<String> index_name,
                                         SegmentID segment_id,
                                         SharedPtr<ChunkIndexEntry> dumped_chunk)
    : BGTask(BGTaskType::kDumpIndexByline, true), db_name_(db_name), table_name_(table_name), index_name_(index_name), segment_id_(segment_id),
      dumped_chunk_(dumped_chunk) {}

} // namespace infinity
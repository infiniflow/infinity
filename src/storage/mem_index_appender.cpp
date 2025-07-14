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

module;

#include <thread>
#include <memory>

module infinity_core;

import :stl;
import :bg_task;
import :logger;
import :infinity_exception;
import :third_party;
import :blocking_queue;
import :infinity_context;
import :base_memindex;
import :status;
import :wal_manager;
import global_resource_usage;

import :new_txn_manager;
import :new_txn;
import :column_vector;
import :mem_index;
import :memory_indexer;

namespace infinity {

MemIndexAppender::MemIndexAppender() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("MemIndexAppender");
#endif
}

MemIndexAppender::~MemIndexAppender() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("MemIndexAppender");
#endif
}

void MemIndexAppender::Start() {
    LOG_INFO("Mem index appender is started.");
    processor_thread_ = Thread([this] { Process(); });
}

void MemIndexAppender::Stop() {
    LOG_INFO("Mem index appender is stopping.");
    SharedPtr<StopProcessorTask> stop_task = MakeShared<StopProcessorTask>();
    this->Submit(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Mem index appender is stopped.");
}

void MemIndexAppender::Submit(SharedPtr<BGTask> bg_task) {
    task_queue_.Enqueue(std::move(bg_task));
    ++task_count_;
}

void MemIndexAppender::Process() {
    Deque<SharedPtr<BGTask>> tasks;
    StopProcessorTask *stop_task_{};
    Vector<MemoryIndexer *> memory_indexers;
    HashMap<MemoryIndexer *, SharedPtr<AppendMemIndexBatch>> memory_indexer_map;
    while (true) {
        task_queue_.DequeueBulk(tasks);

        for (const auto &bg_task : tasks) {
            switch (bg_task->type_) {
                case BGTaskType::kStopProcessor: {
                    stop_task_ = static_cast<StopProcessorTask *>(bg_task.get());
                    break;
                }
                case BGTaskType::kAppendMemIndex: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        auto append_mem_index_task = static_cast<AppendMemIndexTask *>(bg_task.get());
                        SharedPtr<MemoryIndexer> memory_indexer = append_mem_index_task->mem_index_->GetFulltextIndex();
                        if (memory_indexer == nullptr) {
                            // Only used for full text index, currently
                            UnrecoverableError("Not inverted index");
                        }
                        if (memory_indexer_map.find(memory_indexer.get()) == memory_indexer_map.end()) {
                            memory_indexers.push_back(memory_indexer.get());
                            memory_indexer_map.emplace(memory_indexer.get(), MakeShared<AppendMemIndexBatch>());
                        }
                        AppendMemIndexBatch *append_mem_index_batch = memory_indexer_map[memory_indexer.get()].get();
                        append_mem_index_batch->InsertTask(append_mem_index_task);
                        memory_indexer->AsyncInsertBottom(append_mem_index_task->input_column_,
                                                          append_mem_index_task->offset_,
                                                          append_mem_index_task->row_cnt_,
                                                          append_mem_index_task->seq_inserted_,
                                                          append_mem_index_task->doc_count_,
                                                          append_mem_index_batch);
                    }
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid background task: {}", (u8)bg_task->type_);
                    UnrecoverableError(error_message);
                    break;
                }
            }
        }

        for (auto memory_indexer : memory_indexers) {
            AppendMemIndexBatch *append_mem_index_batch = memory_indexer_map[memory_indexer].get();
            append_mem_index_batch->WaitForCompletion();
            memory_indexer->CommitSync();
            for (auto &append_task : append_mem_index_batch->append_tasks_) {
                append_task->Complete();
            }
        }

        task_count_ -= tasks.size();

        if (stop_task_ != nullptr) {
            stop_task_->Complete();
            break;
        }

        memory_indexer_map.clear();
        memory_indexers.clear();
        tasks.clear();
    }
}

} // namespace infinity

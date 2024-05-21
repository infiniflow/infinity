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

#include <bits/chrono.h>
#include <vector>

module memindex_commit_process;

import stl;
import memory_indexer;
import default_values;
import catalog;
import db_meta;
import db_entry;
import table_entry;
import table_index_entry;
import index_base;
import create_index_info;
import bg_query_state;
import txn_manager;
import txn;
import physical_memindex_commit;
import data_type;
import logical_type;
import logger;
import third_party;
import infinity_exception;

namespace infinity {

MemIndexCommitProcessor::MemIndexCommitProcessor() : running_(false) {}

void MemIndexCommitProcessor::Start() {
    running_.store(true);
    mem_index_commit_thread_ = Thread([this]() { MemIndexCommitLoop(); });
}

void MemIndexCommitProcessor::Stop() {
    LOG_INFO("MemIndexCommitProcessor is stopping...");
    running_.store(false);
    mem_index_commit_thread_.join();
    LOG_INFO("MemIndexCommitProcessor is stopped.");
}

MemIndexCommitProcessor::~MemIndexCommitProcessor() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("MemIndexCommitLoop was stopped...");
        return;
    }
    mem_index_commit_thread_.join();
}

void MemIndexCommitProcessor::AddMemoryIndex(SharedPtr<MemoryIndexer> memory_indexer) {
    std::lock_guard guard(mtx_);
    memory_indexers_.insert(std::move(memory_indexer));
}

void MemIndexCommitProcessor::RemoveMemoryIndex(const SharedPtr<MemoryIndexer> &memory_indexer) {
    std::lock_guard guard(mtx_);
    SizeT erase_n = memory_indexers_.erase(memory_indexer);
    if (erase_n == 0) {
        UnrecoverableException("MemoryIndexer not found in MemIndexCommitProcessor");
    }
}

void MemIndexCommitProcessor::MemIndexCommitLoop() {
    auto prev_time = std::chrono::system_clock::now();
    while (running_.load()) {
        auto current_time = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - prev_time).count() < 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        MemIndexCommit();
        prev_time = current_time;
    }
}

void MemIndexCommitProcessor::MemIndexCommit() {
    Vector<SharedPtr<MemoryIndexer>> mem_indexers;
    {
        std::lock_guard guard(mtx_);
        for (auto mem_idx : memory_indexers_) {
            mem_indexers.push_back(std::move(mem_idx));
        }
    }
    Vector<BGQueryContextWrapper> wrappers;

    for (auto &mem_indexer : mem_indexers) {
        auto physical_memindex_commit = this->MakeCommitOperator(mem_indexer.get());
        BGQueryContextWrapper wrapper;
        bool res =
            wrapper.query_context_->ExecuteBGOperator(std::move(physical_memindex_commit), wrapper.bg_state_, MakeUnique<String>("Mem idx commit"));
        if (res) {
            wrappers.emplace_back(std::move(wrapper));
        }
    }
    for (auto &wrapper : wrappers) {
        TxnTimeStamp commit_ts_out = 0;
        wrapper.query_context_->JoinBGStatement(wrapper.bg_state_, commit_ts_out);
    }
}

UniquePtr<PhysicalMemIndexCommit> MemIndexCommitProcessor::MakeCommitOperator(MemoryIndexer *memory_indexer) {
    u64 id = 0;
    auto output_names = MakeShared<Vector<String>>();
    output_names->push_back("OK");
    auto output_types = MakeShared<Vector<SharedPtr<DataType>>>();
    output_types->push_back(MakeShared<DataType>(LogicalType::kInteger));
    return MakeUnique<PhysicalMemIndexCommit>(id, memory_indexer, nullptr, output_names, output_types);
}

} // namespace infinity
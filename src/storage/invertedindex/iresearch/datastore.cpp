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

#include <atomic>
#include <chrono>
#include <ctpl_stl.h>
#include <filesystem>

#include "formats/formats.hpp"
#include "index/index_reader.hpp"
#include "store/fs_directory.hpp"
#include "utils/index_utils.hpp"

import stl;
import logger;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import column_vector;

module iresearch_datastore;

namespace infinity {

class IRSAsync {
public:
    ~IRSAsync() { Stop(); }

    void Stop() noexcept {
        try {
            pool_0_.stop(true);
        } catch (...) {
        }
        try {
            pool_1_.stop(true);
        } catch (...) {
        }
    }

    template <typename T>
    void Queue(SizeT id, T &&fn);

private:
    ThreadPool pool_0_{1};
    ThreadPool pool_1_{1};
};

template <typename T>
void IRSAsync::Queue(SizeT id, T &&fn) {
    if (0 == id)
        pool_0_.push(Move(fn));
    else if (1 == id)
        pool_1_.push(Move(fn));
}

struct MaintenanceState {
    atomic_u32 pending_commits_{0};
    atomic_u32 pending_consolidations_{0};
};

struct Task {
    IRSAsync *async_{nullptr};
    SharedPtr<MaintenanceState> state_;
    SharedPtr<IRSDataStore> store_;
};

struct ConsolidationTask : Task {
    ConsolidationTask() {}

    ConsolidationTask(const ConsolidationTask &other) {
        state_ = other.state_;
        store_ = other.store_;
        async_ = other.async_;
        consolidation_interval_ = other.consolidation_interval_;
    }

    void operator()(int id);

    bool optimize_{false};

    std::chrono::milliseconds consolidation_interval_{};
};

void ConsolidationTask::operator()(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(consolidation_interval_));
    state_->pending_consolidations_.fetch_sub(1, MemoryOrderRelease);
    if (consolidation_interval_ != std::chrono::milliseconds::zero()) {
        // reschedule
        for (auto count = state_->pending_consolidations_.load(MemoryOrderAcquire); count < 1;) {
            if (state_->pending_consolidations_.compare_exchange_weak(count, count + 1, MemoryOrderAcqrel)) {
                ConsolidationTask task(*this);
                async_->Queue(0, Move(task));
                break;
            }
        }
    }

    if (optimize_) {
        store_->GetWriter()->Consolidate(irs::index_utils::MakePolicy(irs::index_utils::ConsolidateCount()));
        store_->GetWriter()->Commit();
    } else {
        store_->GetWriter()->Consolidate(irs::index_utils::MakePolicy(irs::index_utils::ConsolidateByOrder()));
    }
}

struct CommitTask : Task {
    CommitTask() {}

    CommitTask(const CommitTask &other) {
        state_ = other.state_;
        store_ = other.store_;
        async_ = other.async_;
        commit_interval_ = other.commit_interval_;
    }

    void operator()(int id);

    void Finalize();

    std::chrono::milliseconds commit_interval_{0};
};

void CommitTask::operator()(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(commit_interval_));
    state_->pending_commits_.fetch_sub(1, MemoryOrderRelease);
    Finalize();
    store_->Commit();
}

void CommitTask::Finalize() {
    constexpr size_t kMaxPendingConsolidations = 3;
    CommitTask task(*this);
    state_->pending_commits_.fetch_sub(1, MemoryOrderRelease);
    async_->Queue(0, Move(task));
    if (state_->pending_consolidations_.load(MemoryOrderRelease) < kMaxPendingConsolidations) {
        store_->ScheduleConsolidation();
    }
}

IRSDataStore::IRSDataStore(const String &directory) : directory_(directory), path_{Path(directory_)} {
    async_ = MakeUnique<IRSAsync>();
    maintenance_state_ = MakeShared<MaintenanceState>();
    std::error_code ec;
    bool path_exists = std::filesystem::exists(path_);
    if (!path_exists) {
        std::filesystem::create_directories(path_, ec);
    }
    auto open_mode = path_exists ? (irs::OM_CREATE | irs::OM_APPEND) : irs::OM_CREATE;

    IRSIndexReaderOptions reader_options;
    auto format = irs::formats::get("1_5simd");
    irs_directory_ = MakeUnique<irs::FSDirectory>(directory_.c_str());

    IRSIndexWriterOptions options;
    options.reader_options = reader_options;
    options.segment_memory_max = 256 * (1 << 20); // 256MB
    options.lock_repository = false;              //?
    options.comparator = nullptr;
    index_writer_ = IRSIndexWriter::Make(*(irs_directory_), Move(format), OpenMode(open_mode), options);
    if (!path_exists) {
        index_writer_->Commit();
    }
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::StoreSnapshot(DataSnapshotPtr snapshot) { std::atomic_store_explicit(&snapshot_, Move(snapshot), MemoryOrderRelease); }

IRSDataStore::DataSnapshotPtr IRSDataStore::LoadSnapshot() const { return std::atomic_load_explicit(&snapshot_, MemoryOrderAcquire); }

void IRSDataStore::Commit() {
    UniqueLock<Mutex> lk(commit_mutex_);
    index_writer_->Commit();
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::ScheduleCommit() {
    CommitTask task;
    task.state_ = maintenance_state_;
    task.async_ = async_.get();
    maintenance_state_->pending_commits_.fetch_add(1, MemoryOrderRelease);
    async_->Queue<CommitTask>(0, Move(task));
}

void IRSDataStore::ScheduleConsolidation() {
    ConsolidationTask task;
    task.state_ = maintenance_state_;
    task.async_ = async_.get();
    task.optimize_ = false;
    maintenance_state_->pending_consolidations_.fetch_add(1, MemoryOrderRelease);
    async_->Queue<ConsolidationTask>(0, Move(task));
}

void IRSDataStore::ScheduleOptimize() {
    ConsolidationTask task;
    task.state_ = maintenance_state_;
    task.optimize_ = true;
    maintenance_state_->pending_consolidations_.fetch_add(1, MemoryOrderRelease);
    async_->Queue<ConsolidationTask>(0, Move(task));
}

void IRSDataStore::BatchInsert(SharedPtr<DataBlock> data_block) {
    Vector<SharedPtr<ColumnVector>> column_vectors = data_block->column_vectors;
    for (SizeT i = 0; i < column_vectors.size(); ++i) {
    }
}

void IRSDataStore::Reset() { index_writer_.reset(); }

const IRSSubReader *ViewSnapshot::operator[](SizeT i) noexcept { return segments_[i].segment_; }

const ViewSegment &ViewSnapshot::GetSegment(SizeT i) noexcept { return segments_[i]; }

} // namespace infinity
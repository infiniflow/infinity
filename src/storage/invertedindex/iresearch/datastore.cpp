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
import parser;
import logger;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import operator_state;
import column_vector;
import global_block_id;
import table_scan_function_data;
import block_index;
import block_entry;
import query_context;
import column_buffer;
import block_column_entry;
import default_values;
import base_table_ref;

module iresearch_datastore;

namespace infinity {

#define DOCMASK 0xFF

u32 RowID2DocID(RowID row_id) { return (row_id.segment_id_ << 16) + row_id.segment_offset_; }
RowID RowID2DocID(u32 doc_id) { return RowID(doc_id >> 16, doc_id & DOCMASK); }

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
    LOG_INFO(Format("ConsolidationTask id {}", id));
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
    LOG_INFO(Format("CommitTask id {}", id));
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

IRSDataStore::IRSDataStore(const String &table_name, const String &directory, SharedPtr<BaseTableRef> base_table_ref)
    : base_table_ref_(base_table_ref) {
    path_ = Path(directory) / table_name;
    directory_ = path_.string();
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

void IRSDataStore::BatchInsert(u32 block_id, SharedPtr<DataBlock> data_block) {
    SharedPtr<Vector<RowID>> row_ids;
    data_block->FillRowIDVector(row_ids, block_id);
    Vector<SharedPtr<ColumnVector>> column_vectors = data_block->column_vectors;
    auto ctx = index_writer_->GetBatch();

    constexpr static Array<IRSTypeInfo::type_id, 1> TEXT_FEATURES{IRSType<Norm>::id()};
    constexpr static Array<IRSTypeInfo::type_id, 1> NUMERIC_FEATURES{IRSType<GranularityPrefix>::id()};

    static Features text_features{TEXT_FEATURES.data(), TEXT_FEATURES.size()};
    static Features numeric_features{NUMERIC_FEATURES.data(), NUMERIC_FEATURES.size()};

    for (SizeT i = 0; i < row_ids->size(); ++i) {
        RowID row_id = (*row_ids)[i];
        auto doc = ctx.Insert(RowID2DocID(row_id));
        for (SizeT column_id = 0; column_id < column_vectors.size(); ++column_id) {
            SharedPtr<DataType> data_type = (*base_table_ref_->column_types_)[column_id]; // TODO
            String column_name = (*base_table_ref_->column_names_)[column_id];            // TODO
            ptr_t dst_ptr = column_vectors[column_id]->data_ptr_;
            SizeT data_type_size = column_vectors[column_id]->data_type_size_;
            if (column_vectors[column_id]->data_type_->IsNumeric()) { // TODO datetime & fixed string could also be filter
                auto field = MakeShared<NumericField>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                field->value_ = (i64) * (column_vectors[column_id]->data_ptr_ + data_type_size * i);
                doc.Insert<irs::Action::INDEX>(*field);
            } else if (column_vectors[column_id]->data_type_->type() == kVarchar) {
                auto field = MakeShared<TextField>(column_name.c_str(),
                                                   irs::IndexFeatures::FREQ | irs::IndexFeatures::POS,
                                                   text_features,
                                                   AnalyzerPool::instance().Get("jieba"));
                // TODO
                doc.Insert<irs::Action::INDEX>(*field);
            }
        }
    }
}

void IRSDataStore::Reset() { index_writer_.reset(); }

ViewSnapshot *IRSDataStore::GetViewSnapshot() {
    IRSDataStore::DataSnapshotPtr snapshot = LoadSnapshot();
    auto vs = MakeUnique<ViewSnapshot>(&(snapshot->reader_));
    auto &ctx = *vs;
    return &(ctx);
}

ViewSnapshot::ViewSnapshot(IRSDirectoryReader *reader) : reader_(reader) {
    segments_.reserve(reader_->size());
    for (auto &subreader : *reader_) {
        segments_.emplace_back(Move(ViewSegment(subreader)));
    }
}

const IRSSubReader *ViewSnapshot::operator[](SizeT i) noexcept { return segments_[i].segment_; }

const ViewSegment &ViewSnapshot::GetSegment(SizeT i) noexcept { return segments_[i]; }

} // namespace infinity
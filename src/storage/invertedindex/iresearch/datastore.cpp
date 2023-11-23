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
#include "parser/type/datetime/date_type.h"
#include "parser/type/datetime/datetime_type.h"
#include "parser/type/datetime/time_type.h"
#include "parser/type/datetime/timestamp_type.h"
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
import block_index;
import block_entry;
import query_context;
import column_buffer;
import block_column_entry;
import default_values;
import value;
import buffer_manager;
import default_values;
import data_block;
import txn;
import index_def;
import table_collection_entry;
import base_entry;
import buffer_handle;
import local_file_system;
import index_entry;
import segment_entry;
import buffer_manager;

module iresearch_datastore;

namespace infinity {

#define DOCMASK 0xFF

u32 RowID2DocID(RowID row_id) { return (row_id.segment_id_ << 16) + row_id.segment_offset_; }
u32 RowID2DocID(u32 segment_id, u32 block_id, u32 block_offset) {
    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + block_offset;
    return (segment_id << 16) + segment_offset;
}

RowID DocID2RowID(u32 doc_id) { return RowID(doc_id >> 16, doc_id & DOCMASK); }

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

IRSDataStore::IRSDataStore(const String &table_name, const String &directory) {
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

void IRSDataStore::BatchInsert(TableCollectionEntry *table_entry, IndexDef *index_def, SegmentEntry *segment_entry, BufferManager *buffer_mgr) {

    constexpr static Array<IRSTypeInfo::type_id, 1> TEXT_FEATURES{IRSType<Norm>::id()};
    constexpr static Array<IRSTypeInfo::type_id, 1> NUMERIC_FEATURES{IRSType<GranularityPrefix>::id()};

    static Features text_features{TEXT_FEATURES.data(), TEXT_FEATURES.size()};
    static Features numeric_features{NUMERIC_FEATURES.data(), NUMERIC_FEATURES.size()};

    auto segment_id = segment_entry->segment_id_;
    for (const auto &block_entry : segment_entry->block_entries_) {
        auto ctx = index_writer_->GetBatch();

        for (SizeT i = 0; block_entry->row_count_; ++i) {
            auto doc = ctx.Insert(RowID2DocID(segment_id, block_entry->block_id_, i));

            for (const auto &column_name : index_def->column_names_) {
                u64 column_id = table_entry->GetColumnIdByName(column_name);
                auto block_column_entry = block_entry->columns_[column_id].get();
                BufferHandle buffer_handle = block_column_entry->buffer_->Load();
                switch (block_column_entry->column_type_->type()) {
                    case kTinyInt: {
                        auto block_data_ptr = reinterpret_cast<const TinyIntT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TinyIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kSmallInt: {
                        auto block_data_ptr = reinterpret_cast<const SmallIntT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        SmallIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kInteger: {
                        auto block_data_ptr = reinterpret_cast<const IntegerT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        IntegerT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kBigInt: {
                        auto block_data_ptr = reinterpret_cast<const BigIntT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i64>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        BigIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kHugeInt: {
                        auto block_data_ptr = reinterpret_cast<const HugeIntT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i64>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        HugeIntT v = block_data_ptr[i];
                        field->value_ = v.lower; // Lose precision
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kFloat: {
                        auto block_data_ptr = reinterpret_cast<const FloatT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<f32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        FloatT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kDouble: {
                        auto block_data_ptr = reinterpret_cast<const DoubleT *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<f64>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DoubleT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    }
                    case kDate: {
                        auto block_data_ptr = reinterpret_cast<const DateType *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DateType v = block_data_ptr[i];
                        field->value_ = v.value;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kTime: {
                        auto block_data_ptr = reinterpret_cast<const TimeType *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i32>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TimeType v = block_data_ptr[i];
                        field->value_ = v.value;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kDateTime: {
                        auto block_data_ptr = reinterpret_cast<const DateTimeType *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i64>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DateTimeType v = block_data_ptr[i];
                        field->value_ = ((i64)v.date << 32) + v.time;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kTimestamp: {
                        auto block_data_ptr = reinterpret_cast<const TimestampType *>(buffer_handle.GetData());
                        auto field = MakeShared<NumericField<i64>>(column_name.c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TimestampType v = block_data_ptr[i];
                        field->value_ = ((i64)v.date << 32) + v.time;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(*field);
                    } break;
                    case kVarchar: {
                        ColumnBuffer column_buffer(buffer_handle, buffer_mgr);
                        auto field = MakeShared<TextField>(column_name.c_str(),
                                                           irs::IndexFeatures::FREQ | irs::IndexFeatures::POS,
                                                           text_features,
                                                           AnalyzerPool::instance().Get("jieba"));
                        auto [src_ptr, data_size] = column_buffer.GetVarcharAt(i);
                        field->f_ = String(src_ptr, data_size);
                        doc.Insert<irs::Action::INDEX>(*field);
                    } break;
                    default:
                        break;
                }
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
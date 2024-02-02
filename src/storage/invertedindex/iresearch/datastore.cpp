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
#include "index/segment_writer.hpp"
#include "parser/type/datetime/date_type.h"
#include "parser/type/datetime/datetime_type.h"
#include "parser/type/datetime/time_type.h"
#include "parser/type/datetime/timestamp_type.h"
#include "store/fs_directory.hpp"

#include "analysis/analyzers.hpp"
#include "analysis/token_attributes.hpp"
#include "frozen/map.h"
#include "search/bm25.hpp"
#include "search/filter.hpp"
#include "search/score.hpp"

module iresearch_datastore;


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
import query_context;
import default_values;
import value;
import buffer_manager;
import default_values;
import data_block;
import txn;
import index_def;
import catalog;
import buffer_handle;
import local_file_system;
import buffer_manager;
import index_full_text;
import infinity_exception;
import data_type;

namespace infinity {

constexpr SizeT DEFAULT_COMMIT_INTERVAL = 10000;
constexpr SizeT DEFAULT_CONSOLIDATION_INTERVAL_MSEC = 1000;

template <typename T>
void IRSAsync::Queue(SizeT id, T &&fn) {
    T t = std::move(fn);
    if (0 == id)
        pool_0_.push(std::move(t));
    else if (1 == id)
        pool_1_.push(std::move(t));
}

void IRSAsync::ClearQueue() {
    pool_0_.clear_queue();
    pool_1_.clear_queue();
}

struct MaintenanceState {
    atomic_bool cancel_{false};
    atomic_u32 pending_commits_{0};
    atomic_u32 pending_consolidations_{0};
};

struct Task {
    Task() {}

    Task(const Task &other) {
        state_ = other.state_;
        store_ = other.store_;
        async_ = other.async_;
    }

    IRSAsync *async_{nullptr};
    IRSDataStore *store_{nullptr};
    SharedPtr<MaintenanceState> state_;
};

struct ConsolidationTask : Task {
    ConsolidationTask() {}

    ConsolidationTask(const ConsolidationTask &other) {
        state_ = other.state_;
        store_ = other.store_;
        async_ = other.async_;
        consolidation_interval_ = other.consolidation_interval_;
        optimize_ = other.optimize_;
    }

    void operator()(int id);

    bool optimize_{false};

    std::chrono::milliseconds consolidation_interval_{};
};

void ConsolidationTask::operator()(int id) {
    LOG_INFO(fmt::format("ConsolidationTask id {}", id));
    if ((true == state_->cancel_.load(std::memory_order::release)) && (false == optimize_))
        return;
    std::this_thread::sleep_for(std::chrono::milliseconds(consolidation_interval_));
    state_->pending_consolidations_.fetch_sub(1, std::memory_order::release);

    if (optimize_) {
        LOG_INFO(fmt::format("ConsolidationTask optimize {}", id));
        store_->GetWriter()->Consolidate(irs::index_utils::MakePolicy(irs::index_utils::ConsolidateCount()));
        store_->GetWriter()->Commit();
    } else {
        store_->GetWriter()->Consolidate(irs::index_utils::MakePolicy(irs::index_utils::ConsolidateByOrder()));
    }

    if (consolidation_interval_ != std::chrono::milliseconds::zero()) {
        // reschedule
        auto count = state_->pending_consolidations_.load(std::memory_order::acquire);
        if (count > 0) {
            if (state_->pending_consolidations_.compare_exchange_weak(count, count + 1, std::memory_order::acq_rel)) {
                ConsolidationTask task(*this);
                async_->Queue(1, std::move(task));
            }
        }
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
    if (true == state_->cancel_.load(std::memory_order::release))
        return;
    LOG_INFO(fmt::format("CommitTask id {}", id));
    std::this_thread::sleep_for(std::chrono::milliseconds(commit_interval_));
    state_->pending_commits_.fetch_sub(1, std::memory_order::release);
    Finalize();
    store_->Commit();
}

void CommitTask::Finalize() {
    constexpr size_t kMaxPendingConsolidations = 3;
    CommitTask task(*this);
    state_->pending_commits_.fetch_add(1, std::memory_order::release);
    async_->Queue(0, std::move(task));
    if (state_->pending_consolidations_.load(std::memory_order::release) < kMaxPendingConsolidations) {
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
    options.segment_pool_size = 1; // number of index threads
    options.reader_options = reader_options;
    options.segment_memory_max = 256 * (1 << 20); // 256MB
    options.lock_repository = false;              //?
    options.comparator = nullptr;
    options.features = [](irs::type_info::type_id id) {
        const irs::ColumnInfo info{irs::type<irs::compression::none>::get(), {}, false};

        if (irs::type<irs::Norm>::id() == id) {
            return std::make_pair(info, &irs::Norm::MakeWriter);
        }

        if (irs::type<irs::Norm2>::id() == id) {
            return std::make_pair(info, &irs::Norm2::MakeWriter);
        }

        return std::make_pair(info, irs::FeatureWriterFactory{});
    };

    index_writer_ = IRSIndexWriter::Make(*(irs_directory_), std::move(format), OpenMode(open_mode), options);
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(std::move(reader));

    AnalyzerPool::instance().Set(SEGMENT);
    AnalyzerPool::instance().Set(JIEBA);

    StoreSnapshot(data);
}

IRSDataStore::~IRSDataStore() { StopSchedule(); }

void IRSDataStore::StoreSnapshot(DataSnapshotPtr snapshot) {
    std::atomic_store_explicit(&snapshot_, std::move(snapshot), std::memory_order::release);
}

IRSDataStore::DataSnapshotPtr IRSDataStore::LoadSnapshot() const { return std::atomic_load_explicit(&snapshot_, std::memory_order::acquire); }

void IRSDataStore::Commit() {
    std::unique_lock<std::mutex> lk(commit_mutex_);
    index_writer_->Commit();
    auto reader = index_writer_->GetSnapshot();
    reader->Reopen();
    auto data = MakeShared<DataSnapshot>(std::move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::ScheduleCommit() {
    CommitTask task;
    task.state_ = maintenance_state_;
    task.async_ = async_.get();
    task.store_ = this;
    task.commit_interval_ = std::chrono::milliseconds(DEFAULT_COMMIT_INTERVAL);
    maintenance_state_->pending_commits_.fetch_add(1, std::memory_order::release);
    async_->Queue<CommitTask>(0, std::move(task));
}

void IRSDataStore::ScheduleConsolidation() {
    ConsolidationTask task;
    task.state_ = maintenance_state_;
    task.async_ = async_.get();
    task.store_ = this;
    task.optimize_ = false;
    task.consolidation_interval_ = std::chrono::milliseconds{DEFAULT_CONSOLIDATION_INTERVAL_MSEC};
    maintenance_state_->pending_consolidations_.fetch_add(1, std::memory_order::release);
    async_->Queue<ConsolidationTask>(1, std::move(task));
}

void IRSDataStore::ScheduleOptimize() {
    ConsolidationTask task;
    task.state_ = maintenance_state_;
    task.async_ = async_.get();
    task.store_ = this;
    task.optimize_ = true;
    maintenance_state_->pending_consolidations_.fetch_add(1, std::memory_order::release);
    async_->Queue<ConsolidationTask>(1, std::move(task));
}

void IRSDataStore::StopSchedule() {
    async_->ClearQueue();
    maintenance_state_->cancel_.store(true, std::memory_order::release);
}

void IRSDataStore::BatchInsert(TableEntry *table_entry, const IndexDef *index_def, const SegmentEntry *segment_entry, BufferManager *buffer_mgr) {

    constexpr static Array<IRSTypeInfo::type_id, 1> TEXT_FEATURES{IRSType<Norm>::id()};
    constexpr static Array<IRSTypeInfo::type_id, 1> NUMERIC_FEATURES{IRSType<GranularityPrefix>::id()};

    static Features text_features{TEXT_FEATURES.data(), TEXT_FEATURES.size()};
    static Features numeric_features{NUMERIC_FEATURES.data(), NUMERIC_FEATURES.size()};
    bool schedule = false;
    auto segment_id = segment_entry->segment_id();
    Vector<UniquePtr<IRSAnalyzer>> analyzers;
    for (const auto &ibase : index_def->index_array_) {
        auto index_base = reinterpret_cast<IndexFullText *>(ibase.get());
        if (index_base->analyzer_ == JIEBA) {
            UniquePtr<IRSAnalyzer> stream = AnalyzerPool::instance().Get(JIEBA);
            if (!stream.get()) {
                UnrecoverableError("Dict path of Jieba analyzer is not valid");
            }
            analyzers.push_back(std::move(stream));
        } else if (index_base->analyzer_ == SEGMENT) {
            UniquePtr<IRSAnalyzer> stream = AnalyzerPool::instance().Get(SEGMENT);
            analyzers.push_back(std::move(stream));
        } else if (index_base->analyzer_.empty()) {
            // TODO use segmentation analyzer if analyzer is not set
            UniquePtr<IRSAnalyzer> stream = AnalyzerPool::instance().Get(SEGMENT);
            analyzers.push_back(std::move(stream));
        } else {
            UnrecoverableError("Non existing analyzer");
        }
    }

    const auto &block_entries = segment_entry->block_entries();
    for (const auto &block_entry : block_entries) {
        auto ctx = index_writer_->GetBatch();
        for (SizeT i = 0; i < block_entry->row_count(); ++i) {
            auto doc = ctx.Insert(RowID2DocID(segment_id, block_entry->block_id(), i));

            for (SizeT col = 0; col < index_def->index_array_.size(); ++col) {
                auto index_base = reinterpret_cast<IndexFullText *>(index_def->index_array_[col].get());
                u64 column_id = table_entry->GetColumnIdByName(index_base->column_name());
                auto block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                BufferHandle buffer_handle = block_column_entry->buffer()->Load();
                switch (block_column_entry->column_type()->type()) {
                    case kTinyInt: {
                        auto block_data_ptr = reinterpret_cast<const TinyIntT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TinyIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kSmallInt: {
                        auto block_data_ptr = reinterpret_cast<const SmallIntT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        SmallIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kInteger: {
                        auto block_data_ptr = reinterpret_cast<const IntegerT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        IntegerT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kBigInt: {
                        auto block_data_ptr = reinterpret_cast<const BigIntT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i64>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        BigIntT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kHugeInt: {
                        auto block_data_ptr = reinterpret_cast<const HugeIntT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i64>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        HugeIntT v = block_data_ptr[i];
                        field->value_ = v.lower; // Lose precision
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kFloat: {
                        auto block_data_ptr = reinterpret_cast<const FloatT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<f32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        FloatT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kDouble: {
                        auto block_data_ptr = reinterpret_cast<const DoubleT *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<f64>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DoubleT v = block_data_ptr[i];
                        field->value_ = v;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    }
                    case kDate: {
                        auto block_data_ptr = reinterpret_cast<const DateType *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DateType v = block_data_ptr[i];
                        field->value_ = v.value;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kTime: {
                        auto block_data_ptr = reinterpret_cast<const TimeType *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i32>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TimeType v = block_data_ptr[i];
                        field->value_ = v.value;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kDateTime: {
                        auto block_data_ptr = reinterpret_cast<const DateTimeType *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i64>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        DateTimeType v = block_data_ptr[i];
                        field->value_ = ((i64)v.date << 32) + v.time;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kTimestamp: {
                        auto block_data_ptr = reinterpret_cast<const TimestampType *>(buffer_handle.GetData());
                        auto field = MakeUnique<NumericField<i64>>(index_base->column_name().c_str(), irs::IndexFeatures::NONE, numeric_features);
                        TimestampType v = block_data_ptr[i];
                        field->value_ = ((i64)v.date << 32) + v.time;
                        doc.Insert<irs::Action::INDEX | irs::Action::STORE>(field.get());
                    } break;
                    case kVarchar: {
                        auto field = MakeUnique<TextField>(index_base->column_name().c_str(),
                                                           irs::IndexFeatures::FREQ | irs::IndexFeatures::POS,
                                                           text_features,
                                                           analyzers[col].get());
                        ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                        field->f_ = column_vector.GetValue(i).GetVarchar();
                        doc.Insert<irs::Action::INDEX, TextField>(field.get());
                    } break;
                    default:
                        break;
                }
            }
        }
        if (!schedule) {
            ScheduleCommit();
            schedule = true;
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
        segments_.emplace_back(ViewSegment(subreader));
    }
}

const IRSSubReader *ViewSnapshot::operator[](SizeT i) noexcept { return segments_[i].segment_; }

const ViewSegment &ViewSnapshot::GetSegment(SizeT i) noexcept { return segments_[i]; }

enum class ExecutionMode { kAll, kStrictTop, kTop };

int IRSDataStore::Search(IrsFilter *flt, const Map<String, String> &options, ScoredIds &sorted) {
    irs::WandContext wand{.index = 0, .strict = false};

    String scorer(DEFAULT_SCORER);
    String scorer_arg(DEFAULT_SCORER_ARG);
    if (auto it = options.find("scorer"); it != options.end()) {
        scorer = it->second;
    }
    if (auto it = options.find("scorer_arg"); it != options.end()) {
        scorer_arg = it->second;
    }
    auto arg_format = irs::type<irs::text_format::csv>::get();
    auto scr = irs::scorers::get(scorer, arg_format, scorer_arg);
    if (!scr) {
        LOG_ERROR(fmt::format("Unable to instantiate scorer '{}' with argument format csv with arguments '{}'", scorer, scorer_arg));
        return -1;
    }

    const IRSDirectoryReader &reader = GetDirectoryReader();
    LOG_INFO(fmt::format("Index stats: segments={}, docs={},live-docs={}", reader->size(), reader->docs_count(), reader->live_docs_count()));
    irs::Scorers order = irs::Scorers::Prepare(scr.get());

    irs::filter::prepared::ptr filter = flt->prepare({
        .index = reader,
        .scorers = order,
    });

    SizeT topn(DEFAULT_TOPN);
    if (auto it = options.find("topn"); it != options.end()) {
        topn = std::stoi(it->second);
    }

    sorted.reserve(topn);
    for (auto left = topn; auto &segment : reader) {
        auto docs = filter->execute(irs::ExecutionContext{.segment = segment, .scorers = order, .wand = wand});
        IRS_ASSERT(docs);

        const irs::document *doc = irs::get<irs::document>(*docs);
        const irs::score *score = irs::get<irs::score>(*docs);
        auto *threshold = irs::get_mutable<irs::score>(docs.get());
        if (!left && threshold) {
            IRS_ASSERT(!sorted.empty());
            IRS_ASSERT(std::is_heap(std::begin(sorted),
                                    std::end(sorted),
                                    [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                        return lhs.first > rhs.first;
                                    }));
            threshold->Min(sorted.front().first);
        }

        for (float_t score_value; docs->next();) {
            (*score)(&score_value);
            if (left) {
                sorted.emplace_back(score_value, doc->value);
                if (0 == --left) {
                    std::make_heap(std::begin(sorted),
                                   std::end(sorted),
                                   [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                       return lhs.first > rhs.first;
                                   });
                    threshold->Min(sorted.front().first);
                }
            } else if (sorted.front().first < score_value) {
                std::pop_heap(std::begin(sorted),
                              std::end(sorted),
                              [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                  return lhs.first > rhs.first;
                              });
                auto &[score, doc_id] = sorted.back();
                score = score_value;
                doc_id = doc->value;
                std::push_heap(std::begin(sorted),
                               std::end(sorted),
                               [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                   return lhs.first > rhs.first;
                               });
                threshold->Min(sorted.front().first);
            }
        }
    }
    std::sort(
        std::begin(sorted),
        std::end(sorted),
        [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept { return lhs.first > rhs.first; });
    return 0;
}

} // namespace infinity
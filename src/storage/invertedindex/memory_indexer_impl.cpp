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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-W#pragma-messages"

#pragma clang diagnostic pop

#include <cassert>
#include <cstdio>

module infinity_core:memory_indexer.impl;

import :memory_indexer;
import :status;
import :index_defines;
import :posting_writer;
import :column_vector;
import :analyzer;
import :analyzer_pool;
import :term;
import :column_inverter;
import :invert_task;
import :ring;
import :external_sort_merger;
import :file_writer;
import :term_meta;
import :fst.fst;
import :posting_list_format;
import :dict_reader;
import :file_reader;
import :logger;
import :vector_with_lock;
import :infinity_exception;
import :mmap;
import :buf_writer;
import :profiler;
import :infinity_context;
import :defer_op;
import :blocking_queue;
import :persistence_manager;
import :utility;
import :persist_result_handler;
import :virtual_store;
import :local_file_handle;
import :mem_usage_change;
import :bg_task;
import :fst.build;
import :default_values;

import std.compat;
import third_party;

import row_id;

namespace infinity {
bool MemoryIndexer::KeyComp::operator()(const std::string &lhs, const std::string &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

MemoryIndexer::PostingTable::PostingTable() {}

MemoryIndexer::MemoryIndexer(const std::string &index_dir,
                             const std::string &base_name,
                             RowID base_row_id,
                             optionflag_t flag,
                             const std::string &analyzer)
    : index_dir_(index_dir), base_name_(base_name), base_row_id_(base_row_id), flag_(flag), posting_format_(PostingFormatOption(flag_)),
      analyzer_(analyzer), inverting_thread_pool_(infinity::InfinityContext::instance().GetFulltextInvertingThreadPool()),
      commiting_thread_pool_(infinity::InfinityContext::instance().GetFulltextCommitingThreadPool()), ring_inverted_(15UL), ring_sorted_(13UL) {
    assert(std::filesystem::path(index_dir).is_absolute());
    posting_table_ = std::make_shared<PostingTable>();
    prepared_posting_ = std::make_shared<PostingWriter>(posting_format_, column_lengths_);
    spill_full_path_ = std::filesystem::path(index_dir) / (base_name + ".tmp.merge");
    spill_full_path_ = std::filesystem::path(InfinityContext::instance().config()->TempDir()) / StringTransform(spill_full_path_, "/", "_");
}

MemoryIndexer::~MemoryIndexer() {
    while (GetInflightTasks() > 0) {
        CommitSync(100);
    }
    Reset();
}

void MemoryIndexer::Insert(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count, bool offline) {
    if (is_spilled_) {
        Load();
    }

    u64 seq_inserted(0);
    u32 doc_count(0);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        seq_inserted = seq_inserted_++;
        doc_count = doc_count_;
        doc_count_ += row_count;
    }
    // if ((doc_count & 0x0FFF) == 0) {
    //     size_t inverting_que_size = inverting_thread_pool_.queue_size();
    //     size_t commiting_que_size = commiting_thread_pool_.queue_size();
    //     size_t inverted_ring_size = ring_inverted_.Size();
    //     size_t sorted_ring_size = ring_sorted_.Size();
    //     LOG_INFO(fmt::format("doc_count {}, inverting_que_size {}, commiting_que_size {}, inverted_ring_size {}, sorted_ring_size {}",
    //                          doc_count,
    //                          inverting_que_size,
    //                          commiting_que_size,
    //                          inverted_ring_size,
    //                          sorted_ring_size));
    // }

    auto task = std::make_shared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, doc_count);
    if (offline) {
        PostingWriterProvider provider = [this](const std::string &term) -> std::shared_ptr<PostingWriter> { return GetOrAddPosting(term); };
        auto inverter = std::make_shared<ColumnInverter>(provider, column_lengths_);
        inverter->InitAnalyzer(this->analyzer_);
        auto func = [this, task, inverter](int id) {
            size_t column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            column_length_sum_ += column_length_sum;
            if (column_length_sum > 0) {
                inverter->SortForOfflineDump();
            }
            this->ring_sorted_.Put(task->task_seq_, inverter);
        };
        {
            std::unique_lock<std::mutex> lock(mutex_);
            inflight_tasks_++;
        }
        inverting_thread_pool_.push(std::move(func));
    } else {
        // mem trace : the column_lengths_;
        IncreaseMemoryUsage(sizeof(u32) * row_count);
        PostingWriterProvider provider = [this](const std::string &term) -> std::shared_ptr<PostingWriter> { return GetOrAddPosting(term); };
        auto inverter = std::make_shared<ColumnInverter>(provider, column_lengths_);
        inverter->InitAnalyzer(this->analyzer_);
        auto func = [this, task, inverter](int id) {
            // LOG_INFO(fmt::format("online inverter {} begin", id));
            size_t column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            column_length_sum_ += column_length_sum;
            this->ring_inverted_.Put(task->task_seq_, inverter);
            // LOG_INFO(fmt::format("online inverter {} end", id));
        };
        {
            std::unique_lock<std::mutex> lock(mutex_);
            inflight_tasks_++;
        }
        inverting_thread_pool_.push(std::move(func));
    }
}

void MemoryIndexer::AsyncInsertTop(AppendMemIndexTask *append_mem_index_task) {
    if (is_spilled_) {
        Load();
    }

    u64 seq_inserted(0);
    u32 doc_count(0);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        seq_inserted = seq_inserted_++;
        doc_count = doc_count_;
        doc_count_ += append_mem_index_task->row_cnt_;
        ++inflight_tasks_;
    }
    append_mem_index_task->seq_inserted_ = seq_inserted;
    append_mem_index_task->doc_count_ = doc_count;
}

void MemoryIndexer::AsyncInsertBottom(const std::shared_ptr<ColumnVector> &column_vector,
                                      u32 row_offset,
                                      u32 row_count,
                                      u64 seq_inserted,
                                      u32 doc_count,
                                      AppendMemIndexBatch *append_batch) {
    if (is_spilled_) {
        Load();
    }

    //    u64 seq_inserted(0);
    //    u32 doc_count(0);
    //    {
    //        std::unique_lock<std::mutex> lock(mutex_);
    //        seq_inserted = seq_inserted_++;
    //        doc_count = doc_count_;
    //        doc_count_ += row_count;
    //    }
    auto task = std::make_shared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, doc_count);

    IncreaseMemoryUsage(sizeof(u32) * row_count);
    PostingWriterProvider provider = [this](const std::string &term) -> std::shared_ptr<PostingWriter> { return GetOrAddPosting(term); };
    auto inverter = std::make_shared<ColumnInverter>(provider, column_lengths_);
    inverter->InitAnalyzer(this->analyzer_);
    auto func = [this, task, inverter, append_batch](int id) {
        // LOG_INFO(fmt::format("online inverter {} begin", id));
        size_t column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
        column_length_sum_ += column_length_sum;
        this->ring_inverted_.Put(task->task_seq_, inverter);
        // LOG_INFO(fmt::format("online inverter {} end", id));
        {
            std::unique_lock lock(append_batch->mtx_);
            --append_batch->task_count_;
            if (append_batch->task_count_ == 0) {
                append_batch->cv_.notify_one();
            }
        }
    };
    //    {
    //        std::unique_lock<std::mutex> lock(mutex_);
    //        inflight_tasks_++;
    //    }
    inverting_thread_pool_.push(std::move(func));
}

std::unique_ptr<std::binary_semaphore> MemoryIndexer::AsyncInsert(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count) {
    if (is_spilled_) {
        Load();
    }

    u64 seq_inserted(0);
    u32 doc_count(0);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        seq_inserted = seq_inserted_++;
        doc_count = doc_count_;
        doc_count_ += row_count;
    }

    auto task = std::make_shared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, doc_count);
    auto sema = std::make_unique<std::binary_semaphore>(0);

    IncreaseMemoryUsage(sizeof(u32) * row_count);
    PostingWriterProvider provider = [this](const std::string &term) -> std::shared_ptr<PostingWriter> { return GetOrAddPosting(term); };
    auto inverter = std::make_shared<ColumnInverter>(provider, column_lengths_);
    inverter->InitAnalyzer(this->analyzer_);
    inverter->AddSema(sema.get());
    auto func = [this, task, inverter](int id) {
        // LOG_INFO(fmt::format("online inverter {} begin", id));
        size_t column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
        column_length_sum_ += column_length_sum;
        this->ring_inverted_.Put(task->task_seq_, inverter);
        // LOG_INFO(fmt::format("online inverter {} end", id));
        CommitSync(100);
    };
    {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_++;
    }
    inverting_thread_pool_.push(std::move(func));
    return sema;
}

void MemoryIndexer::InsertGap(u32 row_count) {
    if (is_spilled_) {
        Load();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    doc_count_ += row_count;
}

void MemoryIndexer::Commit(bool offline) {
    commiting_thread_pool_.push([offline, weak_ptr = weak_from_this()](int id) {
        if (const auto shared_ptr = weak_ptr.lock(); shared_ptr) {
            auto *memory_indexer = static_cast<MemoryIndexer *>(shared_ptr.get());
            if (offline) {
                memory_indexer->CommitOffline();
            } else {
                memory_indexer->CommitSync();
            }
        }
    });
}

size_t MemoryIndexer::CommitOffline(size_t wait_if_empty_ms) {
    std::unique_lock<std::mutex> lock(mutex_commit_, std::defer_lock);
    if (!lock.try_lock()) {
        return 0;
    }

    std::vector<std::shared_ptr<ColumnInverter>> inverters;
    this->ring_sorted_.GetBatch(inverters, wait_if_empty_ms);
    size_t num = inverters.size();
    if (!num) {
        return num;
    }

    if (nullptr == spill_file_handle_) {
        PrepareSpillFile();
    }

    for (auto &inverter : inverters) {
        inverter->SpillSortResults(this->spill_file_handle_, this->tuple_count_, buf_writer_);
        num_runs_++;
    }

    std::unique_lock<std::mutex> task_lock(mutex_);
    inflight_tasks_ -= num;
    if (inflight_tasks_ == 0) {
        cv_.notify_all();
    }
    return num;
}

size_t MemoryIndexer::CommitSync(size_t wait_if_empty_ms) {
    std::shared_lock commit_sync_lock(mutex_commit_sync_share_);
    std::vector<std::shared_ptr<ColumnInverter>> inverters;
    // LOG_INFO("MemoryIndexer::CommitSync begin");
    u64 seq_commit = this->ring_inverted_.GetBatch(inverters);
    size_t num_sorted = inverters.size();
    size_t num_generated = 0;
    // size_t num_merged = 0;
    if (num_sorted > 0) {
        ColumnInverter::Merge(inverters);
        inverters[0]->Sort();
        this->ring_sorted_.Put(seq_commit, inverters[0]);
    };

    std::unique_lock<std::mutex> lock(mutex_commit_, std::defer_lock);
    if (!lock.try_lock()) {
        return 0;
    }

    MemUsageChange mem_usage_change = {true, 0};
    while (true) {
        this->ring_sorted_.GetBatch(inverters, wait_if_empty_ms);
        // num_merged = inverters.size();
        if (inverters.empty()) {
            break;
        }
        for (auto &inverter : inverters) {
            mem_usage_change.Add(inverter->GeneratePosting());
            num_generated += inverter->GetMerged();

            if (const auto &semas = inverter->semas(); !semas.empty()) {
                for (auto sema : semas) {
                    sema->release();
                }
            }
        }
    }
    if (num_generated > 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_ -= num_generated;
        if (inflight_tasks_ == 0) {
            cv_.notify_all();
        }
    }
    if (mem_usage_change.is_add_) {
        IncreaseMemoryUsage(mem_usage_change.mem_);
    } else {
        DecreaseMemoryUsage(mem_usage_change.mem_);
    }

    // LOG_INFO(fmt::format("MemoryIndexer::CommitSync sorted {} inverters, generated posting for {} inverters(merged to {}), inflight_tasks_ is {}",
    //                      num_sorted,
    //                      num_generated,
    //                      num_merged,
    //                      inflight_tasks_));

    return num_generated;
}

void MemoryIndexer::Dump(bool offline, bool spill) {
    if (offline) {
        assert(!spill);
        while (GetInflightTasks() > 0) {
            CommitOffline(100);
        }
        std::unique_lock lock(mutex_commit_);
        OfflineDump();
        return;
    }

    if (spill) {
        assert(!offline);
    }

    while (GetInflightTasks() > 0) {
        CommitSync(100);
    }
    std::unique_lock commit_sync_lock(mutex_commit_sync_share_);

    std::string posting_file = std::filesystem::path(index_dir_) / (base_name_ + POSTING_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    std::string dict_file = std::filesystem::path(index_dir_) / (base_name_ + DICT_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    std::string column_length_file = std::filesystem::path(index_dir_) / (base_name_ + LENGTH_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    std::string tmp_posting_file(posting_file);
    std::string tmp_dict_file(dict_file);
    std::string tmp_column_length_file(column_length_file);

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr && !spill;
    if (use_object_cache) {
        std::filesystem::path tmp_dir = std::filesystem::path(InfinityContext::instance().config()->TempDir());
        tmp_posting_file = tmp_dir / StringTransform(tmp_posting_file, "/", "_");
        tmp_dict_file = tmp_dir / StringTransform(tmp_dict_file, "/", "_");
        tmp_column_length_file = tmp_dir / StringTransform(tmp_column_length_file, "/", "_");
    } else {
        Status status = VirtualStore::MakeDirectory(index_dir_);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
    }

    std::shared_ptr<FileWriter> posting_file_writer = std::make_shared<FileWriter>(tmp_posting_file, 128000);
    std::shared_ptr<FileWriter> dict_file_writer = std::make_shared<FileWriter>(tmp_dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));

    std::string tmp_fst_file = tmp_dict_file + ".fst";
    std::ofstream ofs(tmp_fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    if (spill) {
        posting_file_writer->WriteVInt(i32(doc_count_));
    }
    if (posting_table_.get() != nullptr) {
        MemoryIndexer::PostingTableStore &posting_store = posting_table_->store_;
        for (auto it = posting_store.UnsafeBegin(); it != posting_store.UnsafeEnd(); ++it) {
            const MemoryIndexer::PostingPtr posting_writer = it->second;
            TermMeta term_meta(posting_writer->GetDF(), posting_writer->GetTotalTF());
            posting_writer->Dump(posting_file_writer, term_meta, spill);
            size_t term_meta_offset = dict_file_writer->TotalWrittenBytes();
            term_meta_dumpler.Dump(dict_file_writer, term_meta);
            const std::string &term = it->first;
            fst_builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
        }
        posting_file_writer->Sync();
        dict_file_writer->Sync();
        fst_builder.Finish();

        LOG_INFO(fmt::format("Merge from FST file: {}, to DICT file: {}", tmp_fst_file, tmp_dict_file));
        VirtualStore::Merge(tmp_dict_file, tmp_fst_file);

        LOG_INFO(fmt::format("Delete FST file: {}", tmp_fst_file));
        VirtualStore::DeleteFile(tmp_fst_file);
    }
    auto [file_handle, status] = VirtualStore::Open(tmp_column_length_file, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    std::vector<u32> &column_length_array = column_lengths_.UnsafeVec();
    file_handle->Append(&column_length_array[0], sizeof(column_length_array[0]) * column_length_array.size());
    file_handle->Sync();
    if (use_object_cache) {
        PersistResultHandler handler(pm);
        PersistWriteResult result1 = pm->Persist(posting_file, tmp_posting_file, false);
        PersistWriteResult result2 = pm->Persist(dict_file, tmp_dict_file, false);
        PersistWriteResult result3 = pm->Persist(column_length_file, tmp_column_length_file, false);
        handler.HandleWriteResult(result1);
        handler.HandleWriteResult(result2);
        handler.HandleWriteResult(result3);
        PersistWriteResult result4 = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result4);
    }

    is_spilled_ = spill;
}

// Similar to DiskIndexSegmentReader::GetSegmentPosting
void MemoryIndexer::Load() {
    if (!is_spilled_) {
        assert(doc_count_ == 0);
    }
    std::filesystem::path path = std::filesystem::path(index_dir_) / base_name_;
    std::string index_prefix = path.string();
    std::string posting_file = index_prefix + POSTING_SUFFIX + SPILL_SUFFIX;
    std::string dict_file = index_prefix + DICT_SUFFIX + SPILL_SUFFIX;

    std::shared_ptr<DictionaryReader> dict_reader = std::make_shared<DictionaryReader>(dict_file, PostingFormatOption(flag_));
    std::shared_ptr<FileReader> posting_reader = std::make_shared<FileReader>(posting_file, 1024);
    std::string term;
    TermMeta term_meta;
    doc_count_ = (u32)posting_reader->ReadVInt();

    while (dict_reader->Next(term, term_meta)) {
        std::shared_ptr<PostingWriter> posting = GetOrAddPosting(term);
        posting_reader->Seek(term_meta.doc_start_);
        posting->Load(posting_reader);
    }

    std::string column_length_file = index_prefix + LENGTH_SUFFIX + SPILL_SUFFIX;
    auto [file_handle, status] = VirtualStore::Open(column_length_file, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    std::vector<u32> &column_lengths = column_lengths_.UnsafeVec();
    column_lengths.resize(doc_count_);
    file_handle->Read(&column_lengths[0], sizeof(column_lengths[0]) * column_lengths.size());
    u32 column_length_sum = column_lengths_.Sum();
    column_length_sum_.store(column_length_sum);

    is_spilled_ = false;
}

std::shared_ptr<PostingWriter> MemoryIndexer::GetOrAddPosting(const std::string &term) {
    assert(posting_table_.get() != nullptr);
    MemoryIndexer::PostingTableStore &posting_store = posting_table_->store_;
    PostingPtr posting;
    bool found = posting_store.GetOrAdd(term, posting, prepared_posting_);
    if (!found) {
        // mem trace : add term's size
        IncreaseMemoryUsage(term.size());
        prepared_posting_ = std::make_shared<PostingWriter>(posting_format_, column_lengths_);
    }
    return posting;
}

void MemoryIndexer::Reset() {
    if (posting_table_.get()) {
        posting_table_->store_.Clear();
    }
    column_lengths_.Clear();
    DecreaseMemoryUsage(mem_used_);
    mem_used_ = 0;
}

MemIndexTracerInfo MemoryIndexer::GetInfo() const {
    return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                              std::make_shared<std::string>(table_name_),
                              std::make_shared<std::string>(db_name_),
                              MemUsed(),
                              doc_count_);
}

const ChunkIndexMetaInfo MemoryIndexer::GetChunkIndexMetaInfo() const { return ChunkIndexMetaInfo{base_name_, base_row_id_, GetDocCount(), 0}; }

size_t MemoryIndexer::MemUsed() const { return mem_used_; }

void MemoryIndexer::ApplyMemUseChange(MemUsageChange mem_change) {
    if (mem_change.is_add_) {
        IncreaseMemoryUsage(mem_change.mem_);
    } else {
        DecreaseMemoryUsage(mem_change.mem_);
    }
}

void MemoryIndexer::IncreaseMemoryUsage(size_t mem) {
    mem_used_ += mem;
    BaseMemIndex::IncreaseMemoryUsageBase(mem);
}

void MemoryIndexer::DecreaseMemoryUsage(size_t mem) {
    assert(mem_used_ >= mem);
    mem_used_ -= mem;
    BaseMemIndex::DecreaseMemoryUsageBase(mem);
}

void MemoryIndexer::TupleListToIndexFile(std::unique_ptr<SortMergerTermTuple<TermTuple, u32>> &merger) {
    auto &count = merger->Count();
    auto &term_tuple_list_queue = merger->TermTupleListQueue();
    std::filesystem::path path = std::filesystem::path(index_dir_) / base_name_;
    std::string index_prefix = path.string();

    auto *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    std::string posting_file = index_prefix + POSTING_SUFFIX;
    std::string dict_file = index_prefix + DICT_SUFFIX;
    std::string column_length_file = index_prefix + LENGTH_SUFFIX;
    std::string tmp_posting_file(posting_file);
    std::string tmp_dict_file(dict_file);
    std::string tmp_column_length_file(column_length_file);

    if (use_object_cache) {
        auto tmp_dir = std::filesystem::path(InfinityContext::instance().config()->TempDir());
        tmp_posting_file = tmp_dir / StringTransform(tmp_posting_file, "/", "_");
        tmp_dict_file = tmp_dir / StringTransform(tmp_dict_file, "/", "_");
        tmp_column_length_file = tmp_dir / StringTransform(tmp_column_length_file, "/", "_");
    }
    std::shared_ptr<FileWriter> posting_file_writer = std::make_shared<FileWriter>(tmp_posting_file, 128000);
    std::shared_ptr<FileWriter> dict_file_writer = std::make_shared<FileWriter>(tmp_dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    std::string tmp_fst_file = tmp_dict_file + ".fst";
    std::ofstream ofs(tmp_fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    u32 term_length = 0;
    u32 doc_pos_list_size = 0;

    std::string last_term_str;
    std::string_view last_term;
    u32 last_doc_id = INVALID_DOCID;
    u32 last_doc_payload = 0;
    std::unique_ptr<PostingWriter> posting;

    while (count > 0) {
        std::deque<std::shared_ptr<TermTupleList>> temp_term_tuple_queue;
        term_tuple_list_queue.DequeueBulk(temp_term_tuple_queue);

        while (!temp_term_tuple_queue.empty()) {
            std::shared_ptr<TermTupleList> temp_term_tuple = temp_term_tuple_queue.front();
            temp_term_tuple_queue.pop_front();
            doc_pos_list_size = temp_term_tuple->Size();
            term_length = temp_term_tuple->term_.size();

            if (count < temp_term_tuple->Size()) {
                UnrecoverableError("Unexpected error in TupleListToIndexFile");
            }
            count -= temp_term_tuple->Size();

            if (term_length >= MAX_TUPLE_LENGTH) {
                continue;
            }

            std::string_view term = std::string_view(temp_term_tuple->term_);

            if (term != last_term) {
                assert(last_term < term);
                if (last_doc_id != INVALID_DOCID) {
                    posting->EndDocument(last_doc_id, last_doc_payload);
                }
                if (posting.get()) {
                    TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
                    posting->Dump(posting_file_writer, term_meta);
                    size_t term_meta_offset = dict_file_writer->TotalWrittenBytes();
                    term_meta_dumpler.Dump(dict_file_writer, term_meta);
                    fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
                }
                posting = std::make_unique<PostingWriter>(posting_format_, column_lengths_);
                last_term_str = std::string(term);
                last_term = std::string_view(last_term_str);
                last_doc_id = INVALID_DOCID;
                last_doc_payload = 0;
            }
            for (size_t i = 0; i < doc_pos_list_size; ++i) {
                u32 &doc_id = std::get<0>(temp_term_tuple->doc_pos_list_[i]);
                u32 &term_pos = std::get<1>(temp_term_tuple->doc_pos_list_[i]);
                u16 &doc_payload = std::get<2>(temp_term_tuple->doc_pos_list_[i]);

                if (last_doc_id != INVALID_DOCID && last_doc_id != doc_id) {
                    assert(last_doc_id < doc_id);
                    assert(posting.get() != nullptr);
                    posting->EndDocument(last_doc_id, last_doc_payload);
                }
                last_doc_id = doc_id;
                last_doc_payload = doc_payload;
                posting->AddPosition(term_pos);
            }
        }
    }
    if (last_doc_id != INVALID_DOCID) {
        posting->EndDocument(last_doc_id, last_doc_payload);
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        posting->Dump(posting_file_writer, term_meta);
        size_t term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_meta_dumpler.Dump(dict_file_writer, term_meta);
        fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
    }
    posting_file_writer->Sync();
    dict_file_writer->Sync();
    fst_builder.Finish();

    LOG_INFO(fmt::format("Merge from FST file: {}, to DICT file: {}", tmp_fst_file, tmp_dict_file));
    VirtualStore::Merge(tmp_dict_file, tmp_fst_file);

    LOG_INFO(fmt::format("Delete FST file: {}", tmp_fst_file));
    VirtualStore::DeleteFile(tmp_fst_file);

    auto [file_handle, status] = VirtualStore::Open(tmp_column_length_file, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    std::vector<u32> &unsafe_column_lengths = column_lengths_.UnsafeVec();
    file_handle->Append(&unsafe_column_lengths[0], sizeof(unsafe_column_lengths[0]) * unsafe_column_lengths.size());
    if (use_object_cache) {
        PersistResultHandler handler(pm);
        PersistWriteResult result1 = pm->Persist(posting_file, tmp_posting_file, false);
        PersistWriteResult result2 = pm->Persist(dict_file, tmp_dict_file, false);
        PersistWriteResult result3 = pm->Persist(column_length_file, tmp_column_length_file, false);
        handler.HandleWriteResult(result1);
        handler.HandleWriteResult(result2);
        handler.HandleWriteResult(result3);
        PersistWriteResult result4 = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result4);
    }
}

void MemoryIndexer::OfflineDump() {
    // Steps of offline dump:
    // 1. External sort merge
    // 2. Generate posting
    // 3. Dump disk segment data
    // LOG_INFO(fmt::format("MemoryIndexer::OfflineDump begin, num_runs_ {}\n", num_runs_));
    FinalSpillFile();
    constexpr u32 buffer_size_of_each_run = 2 * 1024 * 1024;
    std::unique_ptr<SortMergerTermTuple<TermTuple, u32>> merger =
        std::make_unique<SortMergerTermTuple<TermTuple, u32>>(spill_full_path_.c_str(), num_runs_, buffer_size_of_each_run * num_runs_, 2);
    std::vector<std::unique_ptr<std::thread>> threads;
    merger->Run(threads);
    std::unique_ptr<std::thread> output_thread =
        std::make_unique<std::thread>(std::bind(&MemoryIndexer::TupleListToIndexFile, this, std::ref(merger)));
    threads.emplace_back(std::move(output_thread));

    merger->JoinThreads(threads);
    merger->UnInitRunFile();

    std::filesystem::remove(spill_full_path_);
    num_runs_ = 0;
}

void MemoryIndexer::FinalSpillFile() {
    fseek(spill_file_handle_, 0, SEEK_SET);
    fwrite(&tuple_count_, sizeof(u64), 1, spill_file_handle_);
    fclose(spill_file_handle_);
    tuple_count_ = 0;
    spill_file_handle_ = nullptr;
}

void MemoryIndexer::PrepareSpillFile() {
    spill_file_handle_ = fopen(spill_full_path_.c_str(), "w");
    fwrite(&tuple_count_, sizeof(u64), 1, spill_file_handle_);
    const size_t write_buf_size = 128000;
    buf_writer_ = std::make_unique<BufWriter>(spill_file_handle_, write_buf_size);
}

} // namespace infinity

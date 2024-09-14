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

#include <ctpl_stl.h>

#pragma clang diagnostic pop

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string.h>

module memory_indexer;

import stl;

import index_defines;
import posting_writer;
import column_vector;
import analyzer;
import analyzer_pool;
import term;
import column_inverter;
import invert_task;
import third_party;
import ring;
import external_sort_merger;
import local_file_system;
import file_writer;
import term_meta;
import fst;
import posting_list_format;
import dict_reader;
import file_reader;
import logger;
import file_system;
import file_system_type;
import vector_with_lock;
import infinity_exception;
import mmap;
import buf_writer;
import profiler;
import third_party;
import infinity_context;
import defer_op;
import blocking_queue;
import segment_index_entry;
import persistence_manager;
import utility;

namespace infinity {
constexpr int MAX_TUPLE_LENGTH = 1024; // we assume that analyzed term, together with docid/offset info, will never exceed such length
bool MemoryIndexer::KeyComp::operator()(const String &lhs, const String &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

MemoryIndexer::PostingTable::PostingTable() {}

MemoryIndexer::MemoryIndexer(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag, const String &analyzer)
    : index_dir_(index_dir), base_name_(base_name), base_row_id_(base_row_id), flag_(flag), posting_format_(PostingFormatOption(flag_)),
      analyzer_(analyzer), inverting_thread_pool_(infinity::InfinityContext::instance().GetFulltextInvertingThreadPool()),
      commiting_thread_pool_(infinity::InfinityContext::instance().GetFulltextCommitingThreadPool()), ring_inverted_(15UL), ring_sorted_(13UL) {
    assert(std::filesystem::path(index_dir).is_absolute());
    posting_table_ = MakeShared<PostingTable>();
    prepared_posting_ = MakeShared<PostingWriter>(posting_format_, column_lengths_);
    spill_full_path_ = Path(index_dir) / (base_name + ".tmp.merge");
    spill_full_path_ = Path(InfinityContext::instance().config()->TempDir()) / StringTransform(spill_full_path_, "/", "_");
}

MemoryIndexer::~MemoryIndexer() {
    while (GetInflightTasks() > 0) {
        CommitSync(100);
    }
    Reset();
}

void MemoryIndexer::Insert(SharedPtr<ColumnVector> column_vector, u32 row_offset, u32 row_count, bool offline) {
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
    //     SizeT inverting_que_size = inverting_thread_pool_.queue_size();
    //     SizeT commiting_que_size = commiting_thread_pool_.queue_size();
    //     SizeT inverted_ring_size = ring_inverted_.Size();
    //     SizeT sorted_ring_size = ring_sorted_.Size();
    //     LOG_INFO(fmt::format("doc_count {}, inverting_que_size {}, commiting_que_size {}, inverted_ring_size {}, sorted_ring_size {}",
    //                          doc_count,
    //                          inverting_que_size,
    //                          commiting_que_size,
    //                          inverted_ring_size,
    //                          sorted_ring_size));
    // }

    auto task = MakeShared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, doc_count);
    if (offline) {
        auto inverter = MakeShared<ColumnInverter>(nullptr, column_lengths_);
        inverter->InitAnalyzer(this->analyzer_);
        auto func = [this, task, inverter](int id) {
            SizeT column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            column_length_sum_ += column_length_sum;
            if (column_length_sum > 0) {
                inverter->SortForOfflineDump();
            }
            this->ring_sorted_.Put(task->task_seq_, inverter);
        };
        inverting_thread_pool_.push(std::move(func));
    } else {
        PostingWriterProvider provider = [this](const String &term) -> SharedPtr<PostingWriter> { return GetOrAddPosting(term); };
        auto inverter = MakeShared<ColumnInverter>(provider, column_lengths_);
        inverter->InitAnalyzer(this->analyzer_);
        auto func = [this, task, inverter](int id) {
            // LOG_INFO(fmt::format("online inverter {} begin", id));
            SizeT column_length_sum = inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            column_length_sum_ += column_length_sum;
            this->ring_inverted_.Put(task->task_seq_, inverter);
            // LOG_INFO(fmt::format("online inverter {} end", id));
        };
        inverting_thread_pool_.push(std::move(func));
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_++;
    }
}

void MemoryIndexer::InsertGap(u32 row_count) {
    if (is_spilled_) {
        Load();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    doc_count_ += row_count;
}

void MemoryIndexer::Commit(bool offline) {
    if (offline) {
        commiting_thread_pool_.push([this](int id) { this->CommitOffline(); });
    } else {
        commiting_thread_pool_.push([this](int id) { this->CommitSync(); });
    }
}

SizeT MemoryIndexer::CommitOffline(SizeT wait_if_empty_ms) {
    std::unique_lock<std::mutex> lock(mutex_commit_, std::defer_lock);
    if (!lock.try_lock()) {
        return 0;
    }

    Vector<SharedPtr<ColumnInverter>> inverters;
    this->ring_sorted_.GetBatch(inverters, wait_if_empty_ms);
    SizeT num = inverters.size();
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

SizeT MemoryIndexer::CommitSync(SizeT wait_if_empty_ms) {
    Vector<SharedPtr<ColumnInverter>> inverters;
    // LOG_INFO("MemoryIndexer::CommitSync begin");
    u64 seq_commit = this->ring_inverted_.GetBatch(inverters);
    SizeT num_sorted = inverters.size();
    SizeT num_generated = 0;
    // SizeT num_merged = 0;
    if (num_sorted > 0) {
        ColumnInverter::Merge(inverters);
        inverters[0]->Sort();
        this->ring_sorted_.Put(seq_commit, inverters[0]);
    };

    std::unique_lock<std::mutex> lock(mutex_commit_, std::defer_lock);
    if (!lock.try_lock()) {
        return 0;
    }

    while (1) {
        this->ring_sorted_.GetBatch(inverters, wait_if_empty_ms);
        // num_merged = inverters.size();
        if (inverters.empty()) {
            break;
        }
        for (auto &inverter : inverters) {
            inverter->GeneratePosting();
            num_generated += inverter->GetMerged();
        }
    }
    if (num_generated > 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_ -= num_generated;
        if (inflight_tasks_ == 0) {
            cv_.notify_all();
        }
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
        OfflineDump();
        return;
    }

    if (spill) {
        assert(!offline);
    }

    while (GetInflightTasks() > 0) {
        CommitSync(100);
    }

    LocalFileSystem fs;
    String posting_file = Path(index_dir_) / (base_name_ + POSTING_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    String dict_file = Path(index_dir_) / (base_name_ + DICT_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    String column_length_file = Path(index_dir_) / (base_name_ + LENGTH_SUFFIX + (spill ? SPILL_SUFFIX : ""));
    String tmp_posting_file(posting_file);
    String tmp_dict_file(dict_file);
    String tmp_column_length_file(column_length_file);

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr && !spill;
    if (use_object_cache) {
        Path tmp_dir = Path(InfinityContext::instance().config()->TempDir());
        tmp_posting_file = tmp_dir / StringTransform(tmp_posting_file, "/", "_");
        tmp_dict_file = tmp_dir / StringTransform(tmp_dict_file, "/", "_");
        tmp_column_length_file = tmp_dir / StringTransform(tmp_column_length_file, "/", "_");
    }

    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, tmp_posting_file, 128000);
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, tmp_dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));

    String tmp_fst_file = tmp_dict_file + ".fst";
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
            SizeT term_meta_offset = dict_file_writer->TotalWrittenBytes();
            term_meta_dumpler.Dump(dict_file_writer, term_meta);
            const String &term = it->first;
            fst_builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
        }
        posting_file_writer->Sync();
        dict_file_writer->Sync();
        fst_builder.Finish();
        fs.AppendFile(tmp_dict_file, tmp_fst_file);
        fs.DeleteFile(tmp_fst_file);
    }
    auto [file_handler, status] = fs.OpenFile(tmp_column_length_file, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    Vector<u32> &column_length_array = column_lengths_.UnsafeVec();
    fs.Write(*file_handler, &column_length_array[0], sizeof(column_length_array[0]) * column_length_array.size());
    fs.Close(*file_handler);
    if (use_object_cache) {
        pm->Persist(posting_file, tmp_posting_file, false);
        pm->Persist(dict_file, tmp_dict_file, false);
        pm->Persist(column_length_file, tmp_column_length_file, false);
    }

    is_spilled_ = spill;
    Reset();
}

// Similar to DiskIndexSegmentReader::GetSegmentPosting
void MemoryIndexer::Load() {
    if (!is_spilled_) {
        assert(doc_count_ == 0);
    }
    Path path = Path(index_dir_) / base_name_;
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + POSTING_SUFFIX + SPILL_SUFFIX;
    String dict_file = index_prefix + DICT_SUFFIX + SPILL_SUFFIX;

    SharedPtr<DictionaryReader> dict_reader = MakeShared<DictionaryReader>(dict_file, PostingFormatOption(flag_));
    SharedPtr<FileReader> posting_reader = MakeShared<FileReader>(fs, posting_file, 1024);
    String term;
    TermMeta term_meta;
    doc_count_ = (u32)posting_reader->ReadVInt();

    while (dict_reader->Next(term, term_meta)) {
        SharedPtr<PostingWriter> posting = GetOrAddPosting(term);
        posting_reader->Seek(term_meta.doc_start_);
        posting->Load(posting_reader);
    }

    String column_length_file = index_prefix + LENGTH_SUFFIX + SPILL_SUFFIX;
    auto [file_handler, status] = fs.OpenFile(column_length_file, FileFlags::READ_FLAG, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    Vector<u32> &column_lengths = column_lengths_.UnsafeVec();
    column_lengths.resize(doc_count_);
    fs.Read(*file_handler, &column_lengths[0], sizeof(column_lengths[0]) * column_lengths.size());
    fs.Close(*file_handler);
    u32 column_length_sum = column_lengths_.Sum();
    column_length_sum_.store(column_length_sum);

    is_spilled_ = false;
}

SharedPtr<PostingWriter> MemoryIndexer::GetOrAddPosting(const String &term) {
    assert(posting_table_.get() != nullptr);
    MemoryIndexer::PostingTableStore &posting_store = posting_table_->store_;
    PostingPtr posting;
    bool found = posting_store.GetOrAdd(term, posting, prepared_posting_);
    if (!found) {
        prepared_posting_ = MakeShared<PostingWriter>(posting_format_, column_lengths_);
    }
    return posting;
}

void MemoryIndexer::Reset() {
    if (posting_table_.get()) {
        posting_table_->store_.Clear();
    }
    column_lengths_.Clear();
}

void MemoryIndexer::TupleListToIndexFile(UniquePtr<SortMergerTermTuple<TermTuple, u32>> &merger) {
    auto &count = merger->Count();
    auto &term_tuple_list_queue = merger->TermTupleListQueue();
    Path path = Path(index_dir_) / base_name_;
    String index_prefix = path.string();
    LocalFileSystem fs;

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    String posting_file = index_prefix + POSTING_SUFFIX;
    String dict_file = index_prefix + DICT_SUFFIX;
    String column_length_file = index_prefix + LENGTH_SUFFIX;
    String tmp_posting_file(posting_file);
    String tmp_dict_file(dict_file);
    String tmp_column_length_file(column_length_file);

    if (use_object_cache) {
        Path tmp_dir = Path(InfinityContext::instance().config()->TempDir());
        tmp_posting_file = tmp_dir / StringTransform(tmp_posting_file, "/", "_");
        tmp_dict_file = tmp_dir / StringTransform(tmp_dict_file, "/", "_");
        tmp_column_length_file = tmp_dir / StringTransform(tmp_column_length_file, "/", "_");
    }
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, tmp_posting_file, 128000);
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, tmp_dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    String tmp_fst_file = tmp_dict_file + ".fst";
    std::ofstream ofs(tmp_fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    u32 term_length = 0;
    u32 doc_pos_list_size = 0;

    String last_term_str;
    std::string_view last_term;
    u32 last_doc_id = INVALID_DOCID;
    UniquePtr<PostingWriter> posting;

    while (count > 0) {
        Deque<SharedPtr<TermTupleList>> temp_term_tuple_queue;
        term_tuple_list_queue.DequeueBulk(temp_term_tuple_queue);

        while (!temp_term_tuple_queue.empty()) {
            SharedPtr<TermTupleList> temp_term_tuple = temp_term_tuple_queue.front();
            temp_term_tuple_queue.pop_front();
            doc_pos_list_size = temp_term_tuple->Size();
            term_length = temp_term_tuple->term_.size();

            if (term_length >= MAX_TUPLE_LENGTH) {
                continue;
            }
            assert(count >= temp_term_tuple->Size());
            count -= temp_term_tuple->Size();
            std::string_view term = std::string_view(temp_term_tuple->term_);

            if (term != last_term) {
                assert(last_term < term);
                if (last_doc_id != INVALID_DOCID) {
                    posting->EndDocument(last_doc_id, 0);
                }
                if (posting.get()) {
                    TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
                    posting->Dump(posting_file_writer, term_meta);
                    SizeT term_meta_offset = dict_file_writer->TotalWrittenBytes();
                    term_meta_dumpler.Dump(dict_file_writer, term_meta);
                    fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
                }
                posting = MakeUnique<PostingWriter>(posting_format_, column_lengths_);
                last_term_str = String(term);
                last_term = std::string_view(last_term_str);
                last_doc_id = INVALID_DOCID;
            }
            for (SizeT i = 0; i < doc_pos_list_size; ++i) {
                u32 &doc_id = temp_term_tuple->doc_pos_list_[i].first;
                u32 &term_pos = temp_term_tuple->doc_pos_list_[i].second;

                if (last_doc_id != INVALID_DOCID && last_doc_id != doc_id) {
                    assert(last_doc_id < doc_id);
                    assert(posting.get() != nullptr);
                    posting->EndDocument(last_doc_id, 0);
                }
                last_doc_id = doc_id;
                posting->AddPosition(term_pos);
            }
        }
    }
    if (last_doc_id != INVALID_DOCID) {
        posting->EndDocument(last_doc_id, 0);
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        posting->Dump(posting_file_writer, term_meta);
        SizeT term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_meta_dumpler.Dump(dict_file_writer, term_meta);
        fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
    }
    posting_file_writer->Sync();
    dict_file_writer->Sync();
    fst_builder.Finish();
    fs.AppendFile(tmp_dict_file, tmp_fst_file);
    fs.DeleteFile(tmp_fst_file);

    auto [file_handler, status] = fs.OpenFile(tmp_column_length_file, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    Vector<u32> &unsafe_column_lengths = column_lengths_.UnsafeVec();
    fs.Write(*file_handler, &unsafe_column_lengths[0], sizeof(unsafe_column_lengths[0]) * unsafe_column_lengths.size());
    fs.Close(*file_handler);
    if (use_object_cache) {
        pm->Persist(posting_file, tmp_posting_file, false);
        pm->Persist(dict_file, tmp_dict_file, false);
        pm->Persist(column_length_file, tmp_column_length_file, false);
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
    UniquePtr<SortMergerTermTuple<TermTuple, u32>> merger =
        MakeUnique<SortMergerTermTuple<TermTuple, u32>>(spill_full_path_.c_str(), num_runs_, buffer_size_of_each_run * num_runs_, 2);
    Vector<UniquePtr<Thread>> threads;
    merger->Run(threads);
    UniquePtr<Thread> output_thread = MakeUnique<Thread>(std::bind(&MemoryIndexer::TupleListToIndexFile, this, std::ref(merger)));
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
    const SizeT write_buf_size = 128000;
    buf_writer_ = MakeUnique<BufWriter>(spill_file_handle_, write_buf_size);
}

} // namespace infinity
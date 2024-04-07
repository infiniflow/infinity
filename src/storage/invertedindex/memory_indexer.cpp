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
#include <unistd.h>
module memory_indexer;

import stl;
import memory_pool;
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
import column_length_io;
import logger;

namespace infinity {
constexpr int MAX_TUPLE_LENGTH = 1024; // we assume that analyzed term, together with docid/offset info, will never exceed such length

bool MemoryIndexer::KeyComp::operator()(const String &lhs, const String &rhs) const {
    int ret = strcmp(lhs.c_str(), rhs.c_str());
    return ret < 0;
}

MemoryIndexer::PostingTable::PostingTable() {}

MemoryIndexer::MemoryIndexer(const String &index_dir,
                             const String &base_name,
                             RowID base_row_id,
                             optionflag_t flag,
                             const String &analyzer,
                             MemoryPool &byte_slice_pool,
                             RecyclePool &buffer_pool,
                             ThreadPool &thread_pool)
    : index_dir_(index_dir), base_name_(base_name), base_row_id_(base_row_id), flag_(flag), analyzer_(analyzer), byte_slice_pool_(byte_slice_pool),
      buffer_pool_(buffer_pool), thread_pool_(thread_pool), ring_inverted_(10UL), ring_sorted_(10UL) {
    posting_table_ = MakeShared<PostingTable>();
    prepared_posting_ = MakeShared<PostingWriter>(nullptr, nullptr, PostingFormatOption(flag_), column_length_mutex_, column_length_array_);
    Path path = Path(index_dir) / "tmp.merge";
    spill_full_path_ = path.string();
}

MemoryIndexer::~MemoryIndexer() {
    while (GetInflightTasks() > 0) {
        usleep(1000000);
        CommitSync();
    }
    Reset();
}

void MemoryIndexer::Insert(SharedPtr<ColumnVector> column_vector,
                           u32 row_offset,
                           u32 row_count,
                           SharedPtr<FullTextColumnLengthFileHandler> fulltext_length_handler,
                           bool offline) {
    if (is_spilled_)
        Load();

    u64 seq_inserted(0);
    u32 doc_count(0);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        seq_inserted = seq_inserted_++;
        doc_count = doc_count_;
        doc_count_ += row_count;
    }
    auto update_length_job = MakeShared<FullTextColumnLengthUpdateJob>(std::move(fulltext_length_handler),
                                                                       row_count,
                                                                       doc_count,
                                                                       column_length_mutex_,
                                                                       column_length_array_);
    auto task = MakeShared<BatchInvertTask>(seq_inserted, column_vector, row_offset, row_count, doc_count);
    if (offline) {
        auto inverter = MakeShared<ColumnInverter>(this->analyzer_, nullptr);
        auto func = [this, task, length_handler = std::move(update_length_job), inverter](int id) {
            LOG_INFO(fmt::format("offline inverter {} begin", id));
            inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            inverter->GetTermListLength(length_handler->GetColumnLengthArray());
            length_handler->DumpToFile();
            inverter->SortForOfflineDump();
            this->ring_sorted_.Put(task->task_seq_, inverter);
            LOG_INFO(fmt::format("offline inverter {} end", id));
        };
        thread_pool_.push(std::move(func));
    } else {
        PostingWriterProvider provider = [this](const String &term) -> SharedPtr<PostingWriter> { return GetOrAddPosting(term); };
        auto inverter = MakeShared<ColumnInverter>(this->analyzer_, provider);
        auto func = [this, task, length_handler = std::move(update_length_job), inverter](int id) {
            // LOG_INFO(fmt::format("online inverter {} begin", id));
            inverter->InvertColumn(task->column_vector_, task->row_offset_, task->row_count_, task->start_doc_id_);
            inverter->GetTermListLength(length_handler->GetColumnLengthArray());
            length_handler->DumpToFile();
            this->ring_inverted_.Put(task->task_seq_, inverter);
            // LOG_INFO(fmt::format("online inverter {} end", id));
        };
        thread_pool_.push(std::move(func));
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_++;
    }
    // LOG_INFO(fmt::format("MemoryIndexer::Insert inflight_tasks_ {}", inflight_tasks_));
}

void MemoryIndexer::Commit(bool offline) {
    if (offline) {
        thread_pool_.push([this](int id) { this->CommitOffline(); });
    } else
        thread_pool_.push([this](int id) { this->CommitSync(); });
}

SizeT MemoryIndexer::CommitOffline(bool wait_if_empty) {
    bool generating = false;
    bool changed = generating_.compare_exchange_strong(generating, true);
    if (!changed)
        return 0;
    generating = true;
    LOG_INFO("MemoryIndexer::CommitOffline begin");
    if (nullptr == spill_file_handle_) {
        PrepareSpillFile();
    }
    Vector<SharedPtr<ColumnInverter>> inverters;
    this->ring_sorted_.GetBatch(inverters, wait_if_empty);
    SizeT num = inverters.size();
    if (num > 0) {
        for (auto &inverter : inverters) {
            inverter->SpillSortResults(this->spill_file_handle_, this->tuple_count_);
            num_runs_++;
        }
    }
    LOG_INFO(fmt::format("MemoryIndexer::CommitOffline done {} inverters, inflight_tasks_ was {}", num, inflight_tasks_));
    generating_.compare_exchange_strong(generating, false);
    if (num > 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        inflight_tasks_ -= num;
        if (inflight_tasks_ == 0) {
            cv_.notify_all();
        }
    }
    return num;
}

SizeT MemoryIndexer::CommitSync() {
    Vector<SharedPtr<ColumnInverter>> inverters;
    u64 seq_commit = this->ring_inverted_.GetBatch(inverters);
    if (!inverters.empty()) {
        ColumnInverter::Merge(inverters);
        inverters[0]->Sort();
        this->ring_sorted_.Put(seq_commit, inverters[0]);
    };

    bool generating = false;
    LOG_INFO("MemoryIndexer::CommitSync begin");
    bool changed = generating_.compare_exchange_strong(generating, true);
    if (!changed)
        return 0;
    generating = true;
    this->ring_sorted_.GetBatch(inverters);
    SizeT num = 0;
    for (auto &inverter : inverters) {
        inverter->GeneratePosting();
        num += inverter->GetMerged();
    }
    LOG_INFO(fmt::format("MemoryIndexer::CommitSync done {} inverters, inflight_tasks_ was {}", num, inflight_tasks_));
    generating_.compare_exchange_strong(generating, false);
    std::unique_lock<std::mutex> lock(mutex_);
    inflight_tasks_ -= num;
    if (inflight_tasks_ == 0) {
        cv_.notify_all();
    }

    return num;
}

void MemoryIndexer::Dump(bool offline, bool spill) {
    if (offline) {
        assert(!spill);
        while (GetInflightTasks() > 0) {
            CommitOffline(true);
        }
        OfflineDump();
        return;
    }

    if (spill) {
        assert(!offline);
    }

    while (GetInflightTasks() > 0) {
        usleep(1000000);
        CommitSync();
    }
    LOG_INFO("MemoryIndexer::Dump begin");
    Path path = Path(index_dir_) / base_name_;
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + POSTING_SUFFIX + (spill ? SPILL_SUFFIX : "");
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, posting_file, 128000);
    String dict_file = index_prefix + DICT_SUFFIX + (spill ? SPILL_SUFFIX : "");
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));

    String fst_file = dict_file + ".fst";
    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
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
        fs.AppendFile(dict_file, fst_file);
        fs.DeleteFile(fst_file);
    }
    is_spilled_ = spill;
    Reset();
    LOG_INFO("MemoryIndexer::Dump end");
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
    is_spilled_ = false;
}

SharedPtr<PostingWriter> MemoryIndexer::GetOrAddPosting(const String &term) {
    assert(posting_table_.get() != nullptr);
    MemoryIndexer::PostingTableStore &posting_store = posting_table_->store_;
    PostingPtr posting;
    bool found = posting_store.GetOrAdd(term, posting, prepared_posting_);
    if (!found) {
        prepared_posting_ = MakeShared<PostingWriter>(nullptr, nullptr, PostingFormatOption(flag_), column_length_mutex_, column_length_array_);
    }
    return posting;
}

void MemoryIndexer::Reset() {
    if (posting_table_.get()) {
        posting_table_->store_.Clear();
    }
}

void MemoryIndexer::OfflineDump() {
    // Steps of offline dump:
    // 1. External sort merge
    // 2. Generate posting
    // 3. Dump disk segment data
    LOG_INFO(fmt::format("MemoryIndexer::OfflineDump begin, num_runs_ {}", num_runs_));
    FinalSpillFile();
    SortMerger<TermTuple, u16> *merger = new SortMerger<TermTuple, u16>(spill_full_path_.c_str(), num_runs_, 100000000, 2);
    merger->Run();
    delete merger;
    FILE *f = fopen(spill_full_path_.c_str(), "r");
    u64 count;
    fread((char *)&count, sizeof(u64), 1, f);
    Path path = Path(index_dir_) / base_name_;
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + POSTING_SUFFIX;
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, posting_file, 128000);
    String dict_file = index_prefix + DICT_SUFFIX;
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, dict_file, 128000);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    String fst_file = index_prefix + DICT_SUFFIX + ".fst";
    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    u16 record_length;
    char buf[MAX_TUPLE_LENGTH];
    String last_term_str;
    std::string_view last_term;
    u32 last_doc_id = INVALID_DOCID;
    UniquePtr<PostingWriter> posting;

    for (u64 i = 0; i < count; ++i) {
        fread(&record_length, sizeof(u16), 1, f);
        if (record_length >= MAX_TUPLE_LENGTH) {
            // rubbish tuple, abandoned
            char *buffer = new char[record_length];
            fread(buffer, record_length, 1, f);
            // TermTuple tuple(buffer, record_length);
            delete[] buffer;
            continue;
        }
        fread(buf, record_length, 1, f);
        TermTuple tuple(buf, record_length);
        if (tuple.term_ != last_term) {
            assert(last_term < tuple.term_);
            if (last_doc_id != INVALID_DOCID) {
                posting->EndDocument(last_doc_id, 0);
                // printf(" EndDocument1-%u\n", last_doc_id);
            }
            if (posting.get()) {
                TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
                posting->Dump(posting_file_writer, term_meta);
                SizeT term_meta_offset = dict_file_writer->TotalWrittenBytes();
                term_meta_dumpler.Dump(dict_file_writer, term_meta);
                fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
            }
            posting =
                MakeUnique<PostingWriter>(&byte_slice_pool_, &buffer_pool_, PostingFormatOption(flag_), column_length_mutex_, column_length_array_);
            // printf("\nswitched-term-%d-<%s>\n", i.term_num_, term.data());
            last_term_str = String(tuple.term_);
            last_term = std::string_view(last_term_str);
        } else if (last_doc_id != tuple.doc_id_) {
            assert(last_doc_id != INVALID_DOCID);
            assert(last_doc_id < tuple.doc_id_);
            assert(posting.get() != nullptr);
            posting->EndDocument(last_doc_id, 0);
            // printf(" EndDocument2-%u\n", last_doc_id);
        }
        last_doc_id = tuple.doc_id_;
        posting->AddPosition(tuple.term_pos_);
        // printf(" pos-%u", tuple.term_pos_);
    }
    if (last_doc_id != INVALID_DOCID) {
        posting->EndDocument(last_doc_id, 0);
        // printf(" EndDocument3-%u\n", last_doc_id);
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        posting->Dump(posting_file_writer, term_meta);
        SizeT term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_meta_dumpler.Dump(dict_file_writer, term_meta);
        fst_builder.Insert((u8 *)last_term.data(), last_term.length(), term_meta_offset);
    }
    posting_file_writer->Sync();
    dict_file_writer->Sync();
    fst_builder.Finish();
    fs.AppendFile(dict_file, fst_file);
    fs.DeleteFile(fst_file);

    LOG_INFO(fmt::format("MemoryIndexer::OfflineDump done, num_runs_ {}", num_runs_));
    num_runs_ = 0;
    std::filesystem::remove(spill_full_path_);
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
}

} // namespace infinity
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

#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
module column_inverter;
import stl;
import analyzer;
import analyzer_pool;
import string_ref;
import term;
import radix_sort;
import index_defines;
import posting_writer;
import vector_with_lock;
import infinity_exception;
import third_party;
import status;
import logger;
import buf_writer;

namespace infinity {

template <u32 T>
static u32 Align(u32 unaligned) {
    return (unaligned + T - 1) & (-T);
}

ColumnInverter::ColumnInverter(const String &analyzer, PostingWriterProvider posting_writer_provider, VectorWithLock<u32> &column_lengths)
    : analyzer_(AnalyzerPool::instance().Get(analyzer)), posting_writer_provider_(posting_writer_provider), column_lengths_(column_lengths) {
    if (analyzer_.get() == nullptr) {
        RecoverableError(Status::UnexpectedError(fmt::format("Invalid analyzer: {}", analyzer)));
    }
}

ColumnInverter::~ColumnInverter() = default;

bool ColumnInverter::CompareTermRef::operator()(const u32 lhs, const u32 rhs) const { return std::strcmp(GetTerm(lhs), GetTerm(rhs)) < 0; }

SizeT ColumnInverter::InvertColumn(SharedPtr<ColumnVector> column_vector, u32 row_offset, u32 row_count, u32 begin_doc_id) {
    begin_doc_id_ = begin_doc_id;
    doc_count_ = row_count;
    Vector<u32> column_lengths(row_count);
    SizeT term_count_sum = 0;
    for (SizeT i = 0; i < row_count; ++i) {
        String data = column_vector->ToString(row_offset + i);
        if (data.empty()) {
            continue;
        }
        SizeT term_count = InvertColumn(begin_doc_id + i, data);
        column_lengths[i] = term_count;
        term_count_sum += term_count;
    }
    column_lengths_.SetBatch(begin_doc_id, column_lengths);
    return term_count_sum;
}

SizeT ColumnInverter::InvertColumn(u32 doc_id, const String &val) {
    auto terms_once_ = MakeUnique<TermList>();
    analyzer_->Analyze(val, *terms_once_);
    SizeT term_count = terms_once_->size();
    terms_per_doc_.push_back(Pair<u32, UniquePtr<TermList>>(doc_id, std::move(terms_once_)));
    return term_count;
}

u32 ColumnInverter::AddTerm(StringRef term) {
    const u32 terms_size = terms_.size();
    const u32 unpadded_size = terms_size + 4 + term.size() + 1;
    const u32 fully_padded_size = Align<4>(unpadded_size);
    terms_.resize(fully_padded_size);

    char *buf = &terms_[0] + terms_size;
    memset(buf, 0, 4);
    memcpy(buf + 4, term.data(), term.size());
    memset(buf + 4 + term.size(), 0, fully_padded_size - unpadded_size + 1);

    u32 term_ref = (terms_size + 4) >> 2;
    term_refs_.push_back(term_ref);
    return term_ref;
}

void ColumnInverter::MergePrepare() {
    for (auto &doc_terms : terms_per_doc_) {
        u32 doc_id = doc_terms.first;
        auto &terms_once = doc_terms.second;
        for (auto it = terms_once->begin(); it != terms_once->end(); ++it) {
            StringRef term(it->text_);
            u32 term_ref = AddTerm(term);
            positions_.emplace_back(term_ref, doc_id, it->word_offset_);
        }
    }
    terms_per_doc_.clear();
}

void ColumnInverter::Merge(ColumnInverter &rhs) {
    assert(begin_doc_id_ + doc_count_ <= rhs.begin_doc_id_);
    MergePrepare();
    for (auto &doc_terms : rhs.terms_per_doc_) {
        u32 doc_id = doc_terms.first;
        auto &terms_once = doc_terms.second;
        for (auto it = terms_once->begin(); it != terms_once->end(); ++it) {
            StringRef term(it->text_);
            u32 term_ref = AddTerm(term);
            positions_.emplace_back(term_ref, doc_id, it->word_offset_);
        }
    }
    doc_count_ += rhs.doc_count_;
    merged_++;
    rhs.terms_per_doc_.clear();
    rhs.doc_count_ = 0;
    rhs.merged_ = 0;
}

void ColumnInverter::Merge(Vector<SharedPtr<ColumnInverter>> &inverters) {
    assert(!inverters.empty());
    inverters[0]->MergePrepare();
    SizeT end = inverters.size();
    for (SizeT i = 1; i < end; i++) {
        SharedPtr<ColumnInverter> &rhs = inverters[i];
        inverters[0]->Merge(*rhs);
    }
}

struct TermRefRadix {
    u32 operator()(const u64 v) { return v >> 32; }
};

void ColumnInverter::SortTerms() {
    Vector<u64> first_four_bytes(term_refs_.size());
    for (u32 i = 0; i < term_refs_.size(); ++i) {
        u64 first_four = ntohl(*reinterpret_cast<const u32 *>(GetTermFromRef(term_refs_[i])));
        first_four_bytes[i] = (first_four << 32) | term_refs_[i];
    }
    ShiftBasedRadixSorter<u64, TermRefRadix, CompareTermRef, 24, true>::RadixSort(TermRefRadix(),
                                                                                  CompareTermRef(terms_),
                                                                                  &first_four_bytes[0],
                                                                                  first_four_bytes.size(),
                                                                                  16);
    for (u32 i(0); i < first_four_bytes.size(); i++) {
        term_refs_[i] = first_four_bytes[i] & 0xffffffffl;
    }
    auto term_ref_begin(term_refs_.begin());
    uint32_t term_num = 0; // First valid term number
    const char *last_term = GetTermFromRef(*term_ref_begin);
    UpdateTermNum(*term_ref_begin, term_num);
    for (++term_ref_begin; term_ref_begin != term_refs_.end(); ++term_ref_begin) {
        const char *term = GetTermFromRef(*term_ref_begin);
        int cmpres = strcmp(last_term, term);
        // assert(cmpres <= 0);
        if (cmpres < 0) {
            ++term_num;
            term_refs_[term_num] = *term_ref_begin;
            last_term = term;
        }
        UpdateTermNum(*term_ref_begin, term_num);
    }
    // assert(term_refs_.size() >= term_num + 1);
    term_refs_.resize(term_num + 1);
    // Replace initial word reference by word number.
    for (auto &p : positions_) {
        p.term_num_ = GetTermNum(p.term_num_);
    }
}

struct FullRadix {
    u64 operator()(const ColumnInverter::PosInfo &p) const { return (static_cast<u64>(p.term_num_) << 32) | p.doc_id_; }
};

void ColumnInverter::Sort() {
    SortTerms();
    ShiftBasedRadixSorter<PosInfo, FullRadix, std::less<PosInfo>, 56, true>::RadixSort(FullRadix(),
                                                                                       std::less<PosInfo>(),
                                                                                       &positions_[0],
                                                                                       positions_.size(),
                                                                                       16);
}

void ColumnInverter::GeneratePosting() {
    u32 last_term_num = std::numeric_limits<u32>::max();
    u32 last_doc_id = INVALID_DOCID;
    StringRef last_term, term;
    SharedPtr<PostingWriter> posting = nullptr;
    // printf("GeneratePosting() begin begin_doc_id_ %u, doc_count_ %u, merged_ %u", begin_doc_id_, doc_count_, merged_);
    for (auto &i : positions_) {
        if (last_term_num != i.term_num_) {
            if (last_doc_id != INVALID_DOCID) {
                assert(posting.get() != nullptr);
                posting->EndDocument(last_doc_id, 0);
                // printf(" EndDocument1-%u\n", last_doc_id);
            }
            term = GetTermFromNum(i.term_num_);
            posting = posting_writer_provider_(String(term.data()));
            // printf("\nswitched-term-%d-<%s>\n", i.term_num_, term.data());
            if (last_term_num != (u32)(-1)) {
                assert(last_term_num < i.term_num_);
                assert(last_term < term);
            }
            last_term_num = i.term_num_;
            assert(last_term < term);
            last_term = term;
        } else if (last_doc_id != i.doc_id_) {
            assert(last_doc_id != INVALID_DOCID);
            assert(last_doc_id < i.doc_id_);
            assert(posting.get() != nullptr);
            posting->EndDocument(last_doc_id, 0);
            // printf(" EndDocument2-%u\n", last_doc_id);
        }
        last_doc_id = i.doc_id_;
        posting->AddPosition(i.term_pos_);
        // printf(" pos-%u", i.term_pos_);
    }
    if (last_doc_id != INVALID_DOCID) {
        posting->EndDocument(last_doc_id, 0);
        // printf(" EndDocument3-%u\n", last_doc_id);
    }
    // printf("GeneratePosting() end begin_doc_id_ %u, doc_count_ %u, merged_ %u", begin_doc_id_, doc_count_, merged_);
}

void ColumnInverter::SortForOfflineDump() {
    MergePrepare();
    Sort();
}

/// Layout of the input of external sort file
//    +-----------+  +----------------++--------------------++--------------------------++-------------------------------------------------------+
//    |           |  |                ||                    ||                          ||                                                       |
//    |   Count   |  |  Size of A Run ||   Num of records   ||   Position of Next Run   ||             Data of a Run                             |
//    |           |  |                ||   within a Run     ||                          ||                                                       |
//    +-----------+  +----------------++--------------------++--------------------------++-------------------------------------------------------+
//                   ----------------------------------------------------------------------------------------------------------------------------+
//                                                            Data within each group
void ColumnInverter::SpillSortResults(FILE *spill_file, u64 &tuple_count) {
    // spill sort results for external merge sort
    if (positions_.empty()) {
        return;
    }
    // size of this Run in bytes
    u32 data_size = 0;
    u64 data_size_pos = ftell(spill_file);
    fwrite(&data_size, sizeof(u32), 1, spill_file);
    // number of tuples
    u32 num_of_tuples = positions_.size();
    tuple_count += num_of_tuples;
    fwrite(&num_of_tuples, sizeof(u32), 1, spill_file);
    // start offset for next spill
    u64 next_start_offset = 0;
    u64 next_start_offset_pos = ftell(spill_file);
    fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
    u64 data_start_offset = ftell(spill_file);
    // sorted data
    u32 last_term_num = std::numeric_limits<u32>::max();
    StringRef term;
    u32 record_length = 0;
    char str_null = '\0';
    for (auto &i : positions_) {
        if (last_term_num != i.term_num_) {
            last_term_num = i.term_num_;
            term = GetTermFromNum(last_term_num);
        }
        record_length = term.size() + sizeof(docid_t) + sizeof(u32) + 1;
        fwrite(&record_length, sizeof(u32), 1, spill_file);
        fwrite(term.data(), term.size(), 1, spill_file);
        fwrite(&str_null, sizeof(char), 1, spill_file);
        fwrite(&i.doc_id_, sizeof(docid_t), 1, spill_file);
        fwrite(&i.term_pos_, sizeof(u32), 1, spill_file);
    }

    // update data size
    next_start_offset = ftell(spill_file);
    data_size = next_start_offset - data_start_offset;
    fseek(spill_file, data_size_pos, SEEK_SET);
    fwrite(&data_size, sizeof(u32), 1, spill_file); // update offset for next spill
    fseek(spill_file, next_start_offset_pos, SEEK_SET);
    fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
    fseek(spill_file, next_start_offset, SEEK_SET);
}

void ColumnInverter::SpillSortResults(FILE *spill_file, u64 &tuple_count, UniquePtr<char_t[]>& spill_buffer, SizeT spill_buf_size) {
    // spill sort results for external merge sort
    if (positions_.empty()) {
        return;
    }
    SizeT spill_buf_idx = 0;
    SizeT spill_file_tell = ftell(spill_file);
    // size of this Run in bytes
    u32 data_size = 0;
    u64 data_size_pos = spill_file_tell;
    // fwrite(&data_size, sizeof(u32), 1, spill_file);
    memcpy(spill_buffer.get() + spill_buf_idx, &data_size, sizeof(u32));
    spill_buf_idx += sizeof(u32);
    spill_file_tell += sizeof(u32);

    // number of tuples
    u32 num_of_tuples = positions_.size();
    tuple_count += num_of_tuples;
    // fwrite(&num_of_tuples, sizeof(u32), 1, spill_file);
    memcpy(spill_buffer.get() + spill_buf_idx, &num_of_tuples, sizeof(u32));
    spill_buf_idx += sizeof(u32);
    spill_file_tell += sizeof(u32);

    // start offset for next spill
    u64 next_start_offset = 0;
    u64 next_start_offset_pos = spill_file_tell;
    // u64 next_start_offset_pos = ftell(spill_file);
    // fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
    memcpy(spill_buffer.get() + spill_buf_idx, &next_start_offset, sizeof(u64));
    spill_buf_idx += sizeof(u64);
    spill_file_tell += sizeof(u64);

    assert(spill_buf_idx < spill_buf_size);
    fwrite(spill_buffer.get(), spill_buf_idx, 1, spill_file);
    spill_buf_idx = 0;

    // u64 data_start_offset = ftell(spill_file);
    u64 data_start_offset = spill_file_tell;
    assert((SizeT)ftell(spill_file) == spill_file_tell);
    // sorted data
    u32 last_term_num = std::numeric_limits<u32>::max();
    StringRef term;
    u32 record_length = 0;
    char str_null = '\0';
    for (auto &i : positions_) {
        if (last_term_num != i.term_num_) {
            last_term_num = i.term_num_;
            term = GetTermFromNum(last_term_num);
        }
        record_length = term.size() + sizeof(docid_t) + sizeof(u32) + 1;
        //        fwrite(&record_length, sizeof(u32), 1, spill_file);
        //        fwrite(term.data(), term.size(), 1, spill_file);
        //        fwrite(&str_null, sizeof(char), 1, spill_file);
        //        fwrite(&i.doc_id_, sizeof(docid_t), 1, spill_file);
        //        fwrite(&i.term_pos_, sizeof(u32), 1, spill_file);
        memcpy(spill_buffer.get() + spill_buf_idx, &record_length, sizeof(u32));
        spill_buf_idx += sizeof(u32);

        memcpy(spill_buffer.get() + spill_buf_idx, term.data(), term.size());
        spill_buf_idx += term.size();

        memcpy(spill_buffer.get() + spill_buf_idx, &str_null, sizeof(char));
        spill_buf_idx += sizeof(char);

        memcpy(spill_buffer.get() + spill_buf_idx, &i.doc_id_, sizeof(docid_t));
        spill_buf_idx += sizeof(docid_t);

        memcpy(spill_buffer.get() + spill_buf_idx, &i.term_pos_, sizeof(u32));
        spill_buf_idx += sizeof(u32);

        assert(spill_buf_idx < spill_buf_size);
        fwrite(spill_buffer.get(), spill_buf_idx, 1, spill_file);
        spill_buf_idx = 0;
    }

    // update data size
    next_start_offset = ftell(spill_file);
    data_size = next_start_offset - data_start_offset;
    fseek(spill_file, data_size_pos, SEEK_SET);
    fwrite(&data_size, sizeof(u32), 1, spill_file); // update offset for next spill
    fseek(spill_file, next_start_offset_pos, SEEK_SET);
    fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
    fseek(spill_file, next_start_offset, SEEK_SET);
}

void ColumnInverter::SpillSortResults(FILE *spill_file, u64 &tuple_count, UniquePtr<BufWriter>& buf_writer) {
    // spill sort results for external merge sort
    if (positions_.empty()) {
        return;
    }
    // SizeT spill_buf_idx = 0;
    SizeT spill_file_tell = ftell(spill_file);
    // size of this Run in bytes
    u32 data_size = 0;
    u64 data_size_pos = spill_file_tell;
    // fwrite(&data_size, sizeof(u32), 1, spill_file);
//    memcpy(spill_buffer.get() + spill_buf_idx, &data_size, sizeof(u32));
//    spill_buf_idx += sizeof(u32);
    buf_writer->Write((const char*)&data_size, sizeof(u32));
    spill_file_tell += sizeof(u32);

    // number of tuples
    u32 num_of_tuples = positions_.size();
    tuple_count += num_of_tuples;
    // fwrite(&num_of_tuples, sizeof(u32), 1, spill_file);
//    memcpy(spill_buffer.get() + spill_buf_idx, &num_of_tuples, sizeof(u32));
//    spill_buf_idx += sizeof(u32);
    buf_writer->Write((const char*)&num_of_tuples, sizeof(u32));
    spill_file_tell += sizeof(u32);

    // start offset for next spill
    u64 next_start_offset = 0;
    u64 next_start_offset_pos = spill_file_tell;
    // u64 next_start_offset_pos = ftell(spill_file);
    // fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
//    memcpy(spill_buffer.get() + spill_buf_idx, &next_start_offset, sizeof(u64));
//    spill_buf_idx += sizeof(u64);
    buf_writer->Write((const char*)&next_start_offset, sizeof(u64));
    spill_file_tell += sizeof(u64);

    // assert(spill_buf_idx < spill_buf_size);
//    fwrite(spill_buffer.get(), spill_buf_idx, 1, spill_file);
//    spill_buf_idx = 0;

    // u64 data_start_offset = ftell(spill_file);
    u64 data_start_offset = spill_file_tell;
    // assert((SizeT)ftell(spill_file) == spill_file_tell);
    // sorted data
    u32 last_term_num = std::numeric_limits<u32>::max();
    StringRef term;
    u32 record_length = 0;
    char str_null = '\0';
    for (auto &i : positions_) {
        if (last_term_num != i.term_num_) {
            last_term_num = i.term_num_;
            term = GetTermFromNum(last_term_num);
        }
        record_length = term.size() + sizeof(docid_t) + sizeof(u32) + 1;
//        fwrite(&record_length, sizeof(u32), 1, spill_file);
//        fwrite(term.data(), term.size(), 1, spill_file);
//        fwrite(&str_null, sizeof(char), 1, spill_file);
//        fwrite(&i.doc_id_, sizeof(docid_t), 1, spill_file);
//        fwrite(&i.term_pos_, sizeof(u32), 1, spill_file);
//        memcpy(spill_buffer.get() + spill_buf_idx, &record_length, sizeof(u32));
//        spill_buf_idx += sizeof(u32);
//
//        memcpy(spill_buffer.get() + spill_buf_idx, term.data(), term.size());
//        spill_buf_idx += term.size();
//
//        memcpy(spill_buffer.get() + spill_buf_idx, &str_null, sizeof(char));
//        spill_buf_idx += sizeof(char);
//
//        memcpy(spill_buffer.get() + spill_buf_idx, &i.doc_id_, sizeof(docid_t));
//        spill_buf_idx += sizeof(docid_t);
//
//        memcpy(spill_buffer.get() + spill_buf_idx, &i.term_pos_, sizeof(u32));
//        spill_buf_idx += sizeof(u32);
        buf_writer->Write((const char*)&record_length, sizeof(u32));
        buf_writer->Write(term.data(), term.size());
        buf_writer->Write((const char*)&str_null, sizeof(char));
        buf_writer->Write((const char*)&(i.doc_id_), sizeof(docid_t));
        buf_writer->Write((const char*)&(i.term_pos_), sizeof(u32));
        // assert(spill_buf_idx < spill_buf_size);
        // fwrite(spill_buffer.get(), spill_buf_idx, 1, spill_file);
        // spill_buf_idx = 0;
    }
    buf_writer->Flush();
    // update data size
    // next_start_offset = ftell(spill_file);
    next_start_offset = buf_writer->Tell();
    data_size = next_start_offset - data_start_offset;
    fseek(spill_file, data_size_pos, SEEK_SET);
    fwrite(&data_size, sizeof(u32), 1, spill_file); // update offset for next spill
    fseek(spill_file, next_start_offset_pos, SEEK_SET);
    fwrite(&next_start_offset, sizeof(u64), 1, spill_file);
    fseek(spill_file, next_start_offset, SEEK_SET);
}


} // namespace infinity
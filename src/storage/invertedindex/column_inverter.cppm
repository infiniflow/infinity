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

#include <cstdio>

export module column_inverter;

import stl;
import analyzer;

import column_vector;
import term;
import string_ref;
import internal_types;
import posting_writer;
import vector_with_lock;
import buf_writer;

namespace infinity {

export class ColumnInverter {
public:
    ColumnInverter(PostingWriterProvider posting_writer_provider, VectorWithLock<u32> &column_lengths);
    ColumnInverter(const ColumnInverter &) = delete;
    ColumnInverter(const ColumnInverter &&) = delete;
    ColumnInverter &operator=(const ColumnInverter &) = delete;
    ColumnInverter &operator=(const ColumnInverter &&) = delete;
    ~ColumnInverter();

    void InitAnalyzer(const String &analyzer);

    SizeT InvertColumn(SharedPtr<ColumnVector> column_vector, u32 row_offset, u32 row_count, u32 begin_doc_id);

    void SortForOfflineDump();

    void Merge(ColumnInverter &rhs);

    static void Merge(Vector<SharedPtr<ColumnInverter>> &inverters);

    void Sort();

    void GeneratePosting();

    u32 GetDocCount() { return doc_count_; }

    u32 GetMerged() { return merged_; }

    struct PosInfo {
        u32 term_num_{0};
        u32 doc_id_{0};
        u32 term_pos_{0};

        bool operator<(const PosInfo &rhs) const {
            if (term_num_ != rhs.term_num_) {
                return term_num_ < rhs.term_num_;
            }
            if (doc_id_ != rhs.doc_id_) {
                return doc_id_ < rhs.doc_id_;
            }
            return term_pos_ < rhs.term_pos_;
        }
    };

    void SpillSortResults(FILE *spill_file, u64 &tuple_count);

    void SpillSortResults(FILE *spill_file, u64 &tuple_count, UniquePtr<char_t[]>& spill_buffer, SizeT spill_buf_size);

    void SpillSortResults(FILE *spill_file, u64 &tuple_count, UniquePtr<BufWriter>& buf_writer);

private:
    using TermBuffer = Vector<char>;
    using PosInfoVec = Vector<PosInfo>;
    using U32Vec = Vector<u32>;

    struct CompareTermRef {
        const char *const term_buffer_;

        CompareTermRef(const TermBuffer &term_buffer) : term_buffer_(&term_buffer[0]) {}

        const char *GetTerm(u32 term_ref) const { return &term_buffer_[term_ref << 2]; }

        bool operator()(const u32 lhs, const u32 rhs) const;
    };

    SizeT InvertColumn(u32 doc_id, const String &val);

    const char *GetTermFromRef(u32 term_ref) const { return &terms_[term_ref << 2]; }

    const char *GetTermFromNum(u32 term_num) const { return GetTermFromRef(term_refs_[term_num]); }

    u32 GetTermNum(u32 term_ref) const {
        const char *p = &terms_[(term_ref - 1) << 2];
        return *reinterpret_cast<const u32 *>(p);
    }

    void UpdateTermNum(u32 term_ref, u32 term_num) {
        char *p = &terms_[(term_ref - 1) << 2];
        *reinterpret_cast<u32 *>(p) = term_num;
    }

    u32 AddTerm(StringRef term);

    void SortTerms();

    void MergePrepare();

    UniquePtr<Analyzer> analyzer_{nullptr};
    u32 begin_doc_id_{0};
    u32 doc_count_{0};
    u32 merged_{1};
    TermBuffer terms_;
    PosInfoVec positions_;
    U32Vec term_refs_;
    Vector<Pair<u32, UniquePtr<TermList>>> terms_per_doc_;
    PostingWriterProvider posting_writer_provider_{};
    VectorWithLock<u32> &column_lengths_;
};
} // namespace infinity

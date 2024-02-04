module;

#include <string.h>
#include <vector>

module parallel_column_inverter;
import stl;
import analyzer;
import memory_pool;
import pool_allocator;
import string_ref;
import term;
import radix_sort;
import index_defines;
import memory_indexer;
import third_party;
namespace infinity {

TermPostings::TermPostings(MemoryIndexer *memory_indexer) : memory_indexer_(memory_indexer), terms_{0, ValueRefHash{}, TermEq{postings_}} {}

void TermPostings::GetSorted(Vector<const TermPosting *> &term_postings) {
    term_postings.resize(term_postings.size());

    for (auto *p = term_postings.data(); const auto &posting : postings_) {
        *p++ = &posting;
    }

    std::sort(term_postings.begin(), term_postings.end(), [](const auto lhs, const auto rhs) { return StringViewCompare(lhs->term_, rhs->term_); });
}

TermPosting *TermPostings::Emplace(StringView term) {
    const HashedStringView hashed_term{term};

    bool is_new = false;
    const auto it = terms_.lazy_emplace(hashed_term, [&, size = terms_.size()](const auto &ctor) {
        ctor(size, hashed_term.hash_);
        is_new = true;
    });
    if (!is_new) {
        return &postings_[it->ref_];
    }
    const auto term_size = term.size();
    try {
        // TODO using a dedicate object pool
        MemoryPool *pool = memory_indexer_->GetPool();
        auto *start = (char *)pool->Allocate(term_size);
        memcpy(start, term.data(), term_size);
        return &postings_.emplace_back(start, term_size);
    } catch (...) {
        terms_.erase(it);
        return nullptr;
    }
}

ParallelColumnInverter::ParallelColumnInverter(MemoryIndexer *memory_indexer)
    : memory_indexer_(memory_indexer), analyzer_(memory_indexer->GetAnalyzer()), jieba_specialize_(memory_indexer->IsJiebaSpecialize()),
      alloc_(memory_indexer->GetPool()) {
    term_postings_ = MakeUnique<TermPostings>(memory_indexer_);
}

ParallelColumnInverter::~ParallelColumnInverter() {}

void ParallelColumnInverter::InvertColumn(u32 doc_id, const String &val) {
    terms_once_.clear();
    analyzer_->Analyze(val, terms_once_, jieba_specialize_);
    for (auto it = terms_once_.begin(); it != terms_once_.end(); ++it) {
        StringView term(it->text_);
        TermPosting *term_posting = term_postings_->Emplace(term);
        term_posting->values_.emplace_back(doc_id, it->word_offset_);
    }
}

void ParallelColumnInverter::InvertColumn(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) {}

void ParallelColumnInverter::Flush() {}

ParallelColumnInverters::ParallelColumnInverters(MemoryIndexer *memory_indexer, u32 size) : memory_indexer_(memory_indexer), size_(size) {
    inverters_.resize(size_);
    for (u32 i = 0; i < size_; ++i) {
        inverters_[i] = MakeUnique<ParallelColumnInverter>(memory_indexer);
    }
}

void ParallelColumnInverters::Commit() {
    Vector<Vector<const TermPosting *>> all_postings(size_);
    for (u32 i = 0; i < size_; ++i) {
        TermPostings *term_postings_slice = inverters_[i]->GetTermPostings();
        Vector<const TermPosting *> &term_postings = all_postings[i];
        term_postings.resize(term_postings_slice->Size());
        for (u32 j = 0; j < term_postings_slice->Size(); ++j) {
            term_postings.push_back(&(term_postings_slice->postings_[i]));
        }
        term_postings_slice->GetSorted(term_postings);
    }
    auto cmp = [](const Pair<const TermPosting *, u32> &lhs, const Pair<const TermPosting *, u32> &rhs) {
        return StringViewCompare(lhs.first->term_, rhs.first->term_);
    };
    Heap<Pair<const TermPosting *, u32>, decltype(cmp)> pq(cmp);
    for (u32 i = 0; i < size_; ++i) {
        if (!all_postings[i].empty()) {
            pq.push({all_postings[i][0], i});
            all_postings[i].erase(all_postings[i].begin());
        }
    }

    Vector<const TermPosting *> to_merge(size_);
    StringView prev_term = pq.top().first->term_;
    while (!pq.empty()) {
        auto [term_posting, array_index] = pq.top();
        pq.pop();
        {
            if (!StringViewCompare(prev_term, term_posting->term_)) {
                DoMerge(to_merge);
                prev_term = term_posting->term_;
                to_merge.clear();
            } else {
                to_merge.push_back(term_posting);
            }
        }
        if (!all_postings[array_index].empty()) {
            pq.push({all_postings[array_index][0], array_index});
            all_postings[array_index].erase(all_postings[array_index].begin());
        }
    }
    DoMerge(to_merge);
}

void ParallelColumnInverters::DoMerge(Vector<const TermPosting *> &to_merge) {
    Vector<TermPosting::PosInfo> values;
    u32 total_size = 0;
    for (u32 i = 0; i < to_merge.size(); ++i) {
        total_size += to_merge[i]->values_.size();
    }
    values.resize(total_size);
    TermPosting::PosInfo *ptr = values.data();
    for (u32 i = 0; i < to_merge.size(); ++i) {
        memcpy(ptr, to_merge[i]->values_.data(), to_merge[i]->values_.size() * sizeof(TermPosting::PosInfo));
        ptr += to_merge[i]->values_.size();
    }
    std::sort(values.begin(), values.end());

    MemoryIndexer::PostingPtr posting = memory_indexer_->GetOrAddPosting(String(to_merge[0]->term_));
}

} // namespace infinity
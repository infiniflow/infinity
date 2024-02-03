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

TermPosting *TermPostings::Emplace(std::string_view term) {
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
        std::string_view term(it->text_);
        TermPosting *term_posting = term_postings_->Emplace(term);
        term_posting->values_.emplace_back(doc_id, it->word_offset_);
    }
}
} // namespace infinity
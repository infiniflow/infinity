module;
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

void TermPostings::GetSorted(Vector<const TermPosting *> &term_postings) {
    term_postings.resize(term_postings.size());

    for (auto *p = term_postings.data(); const auto &posting : postings_) {
        *p++ = &posting;
    }

    std::sort(term_postings.begin(), term_postings.end(), [](const auto lhs, const auto rhs) { return StringViewCompare(lhs->term_, rhs->term_); });
}

TermPosting *TermPostings::Emplace(StringView term) { return nullptr; }

ParallelColumnInverter::ParallelColumnInverter(MemoryIndexer *memory_indexer)
    : column_indexer_(memory_indexer), analyzer_(memory_indexer->GetAnalyzer()), jieba_specialize_(memory_indexer->IsJiebaSpecialize()),
      alloc_(memory_indexer->GetPool()) {}

ParallelColumnInverter::~ParallelColumnInverter() {}

} // namespace infinity
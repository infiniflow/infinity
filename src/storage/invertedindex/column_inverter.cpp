module;

#include <arpa/inet.h>
#include <cstring>
#include <vector>

module column_inverter;
import stl;
import task_executor;
import analyzer;
import memory_pool;
import pool_allocator;
import string_ref;
import term;
import radix_sort;
namespace infinity {

template <u32 T>
static u32 Align(u32 unaligned) {
    return (unaligned + T - 1) & (-T);
}

ColumnInverter::ColumnInverter(Analyzer *analyzer, bool jieba_specialize, SharedPtr<MemoryPool> pool)
    : analyzer_(analyzer), jieba_specialize_(jieba_specialize), alloc_(pool.get()), terms_(alloc_), positions_(alloc_), term_refs_(alloc_) {}

ColumnInverter::~ColumnInverter() {}

bool ColumnInverter::CompareTermRef::operator()(const u32 lhs, const u32 rhs) const { return std::strcmp(GetTerm(lhs), GetTerm(rhs)) < 0; }

void ColumnInverter::InvertColumn(u32 doc_id, const String &val) {
    terms_once_.clear();
    analyzer_->Analyze(val, terms_once_, jieba_specialize_);
    for (auto it = terms_once_.begin(); it != terms_once_.end(); ++it) {
        StringRef term(it->text_);
        u32 term_ref = AddTerm(term);
        positions_.emplace_back(term_ref, doc_id, it->word_offset_);
    }
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

struct TermRefRadix {
    u32 operator()(const u64 v) { return v >> 32; }
};

void ColumnInverter::SortTerms() {
    Vector<u64> first_four_bytes(term_refs_.size());
    for (u32 i = 1; i < term_refs_.size(); ++i) {
        u64 first_four = ntohl(*reinterpret_cast<const u32 *>(GetTermFromRef(term_refs_[i])));
        first_four_bytes[i] = (first_four << 32) | term_refs_[i];
    }
    ShiftBasedRadixSorter<u64, TermRefRadix, CompareTermRef, 24, true>::RadixSort(TermRefRadix(),
                                                                                  CompareTermRef(terms_),
                                                                                  &first_four_bytes[1],
                                                                                  first_four_bytes.size() - 1,
                                                                                  16);
    for (u32 i(1); i < first_four_bytes.size(); i++) {
        term_refs_[i] = first_four_bytes[i] & 0xffffffffl;
    }
    auto term_ref_begin(term_refs_.begin() + 1);
    uint32_t term_num = 1; // First valid term number
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
} // namespace infinity
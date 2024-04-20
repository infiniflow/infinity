module;

#include <iostream>

module blockmax_phrase_doc_iterator;

import stl;
import index_defines;
import internal_types;
import early_terminate_iterator;
import phrase_doc_iterator;
import blockmax_term_doc_iterator;
import column_length_io;
import third_party;

namespace infinity {

bool BlockMaxPhraseDocIterator::BlockSkipTo(RowID doc_id, float threshold) {
    for (auto &iter : term_doc_iters_) {
        if (!iter->BlockSkipTo(doc_id, threshold)) {
            return false;
        }
    }
    return true;
}

RowID BlockMaxPhraseDocIterator::BlockMinPossibleDocID() const { return term_doc_iters_[0]->BlockMinPossibleDocID(); }

RowID BlockMaxPhraseDocIterator::BlockLastDocID() const { return term_doc_iters_[0]->BlockLastDocID(); }

float BlockMaxPhraseDocIterator::BlockMaxBM25Score() { return term_doc_iters_[0]->BlockMaxBM25Score(); }

Tuple<bool, float, RowID> BlockMaxPhraseDocIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    Vector<Tuple<bool, float, RowID>> all_res;
    for (auto &iter : term_doc_iters_) {
        auto res = iter->SeekInBlockRange(doc_id, doc_id_no_beyond, threshold);
        if (!std::get<0>(res)) {
            return {false, 0.0F, INVALID_ROWID};
        }
        all_res.push_back(res);
    }
    if (all_res.empty()) {
        return {false, 0.0F, INVALID_ROWID};
    }
    return all_res[0];
}

Pair<bool, RowID> BlockMaxPhraseDocIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    Vector<Pair<bool, RowID>> all_res;
    for (auto &iter : term_doc_iters_) {
        auto res = iter->PeekInBlockRange(doc_id, doc_id_no_beyond);
        if (!std::get<0>(res)) {
            return {false, INVALID_ROWID};
        }
        all_res.push_back(res);
    }
    if (all_res.empty()) {
        return {false, INVALID_ROWID};
    }
    return all_res[0];
}

void BlockMaxPhraseDocIterator::InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader) {
    for (auto &iter : term_doc_iters_) {
        iter->InitBM25Info(total_df, avg_column_len, column_length_reader);
    }
}

bool BlockMaxPhraseDocIterator::NotPartCheckExist(RowID doc_id) {
    for (auto &doc_iter : term_doc_iters_) {
        if (!doc_iter->NotPartCheckExist(doc_id)) {
            return false;
        }
    }
    return true;
}

void BlockMaxPhraseDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "BlockMaxPhraseDocIterator";
    os << " (weight: " << weight_ << ")";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (phrase: ";
    for (auto term : *terms_ptr_) {
        os << term << " ";
    }
    os << ")";
    os << " (doc_freq: " << DocFreq() << ")";
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << '\n';
}

}
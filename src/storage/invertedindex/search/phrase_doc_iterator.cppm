module;

export module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import index_defines;
import column_length_io;

namespace infinity {
export class PhraseDocIterator final : public DocIterator {
public:
    PhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters, float weight, u32 slop = 0)
        : pos_iters_(std::move(iters)), weight_(weight), slop_(slop) {
        doc_freq_ = 0;
        phrase_freq_ = 0;
        if (pos_iters_.size()) {
            estimate_doc_freq_ = pos_iters_[0]->GetDocFreq();
        } else {
            estimate_doc_freq_ = 0;
        }
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            estimate_doc_freq_ = std::min(estimate_doc_freq_, pos_iters_[i]->GetDocFreq());
        }
    }

    u32 GetEstimateDF() const { return estimate_doc_freq_; }

    float GetWeight() const { return weight_; }

    float GetPhraseFreq() const { return phrase_freq_; }

    void InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader);

    DocIteratorType GetType() const override { return DocIteratorType::kPhraseIterator; }
    String Name() const override { return "PhraseDocIterator"; }

    bool Next(RowID doc_id) override;

    float BM25Score() override;

    void UpdateScoreThreshold(float threshold) override {
        if (threshold > threshold_)
            threshold_ = threshold;
    }

    u32 LeafCount() const override { return 1; }
    u32 MatchCount() const override { return DocID() != INVALID_ROWID; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    // debug info
    const Vector<String> *terms_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    bool GetPhraseMatchData() {
        if (slop_ == 0) {
            return GetExactPhraseMatchData();
        } else {
            return GetSloppyPhraseMatchData();
        }
    }
    bool GetExactPhraseMatchData();
    bool GetSloppyPhraseMatchData();
    Vector<UniquePtr<PostingIterator>> pos_iters_;
    float weight_;
    u32 slop_{};

    // for BM25 Score
    float bm25_score_cache_ = 0.0f;
    RowID bm25_score_cache_docid_ = INVALID_ROWID;
    float f1 = 0.0f;
    float f2 = 0.0f;
    float f3 = 0.0f;
    float bm25_common_score_ = 0.0f;
    UniquePtr<FullTextColumnLengthReader> column_length_reader_ = nullptr;
    float block_max_bm25_score_cache_ = 0.0f;
    RowID block_max_bm25_score_cache_end_id_ = INVALID_ROWID;

    float tf_ = 0.0f;          // current doc_id_'s tf
    u32 estimate_doc_freq_{0}; // estimated at the beginning
    float phrase_freq_{0.0f};  // increase during search
    Vector<float> all_tf_;     // increase during search

    // debug statistics
    u32 calc_score_cnt_ = 0;
};
}
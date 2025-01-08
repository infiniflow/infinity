module;

export module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import index_defines;
import column_length_io;
import parse_fulltext_options;
import blockmax_leaf_iterator;

namespace infinity {

export class PhraseDocIterator final : public BlockMaxLeafIterator {
public:
    PhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters, float weight, u32 slop, FulltextSimilarity ft_similarity);

    inline u32 GetDocFreq() const { return doc_freq_; }

    u32 GetEstimateDF() const { return estimate_doc_freq_; }

    float GetWeight() const { return weight_; }

    float GetPhraseFreq() const { return phrase_freq_; }

    void InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader, float delta, float k1, float b) override;

    DocIteratorType GetType() const override { return DocIteratorType::kPhraseIterator; }
    String Name() const override { return "PhraseDocIterator"; }

    bool Next(RowID doc_id) override;

    RowID BlockMinPossibleDocID() const override { return block_min_possible_doc_id_; }

    RowID BlockLastDocID() const override { return block_last_doc_id_; }

    void UpdateBlockRangeDocID();

    float BlockMaxBM25Score() override;

    // Move block cursor to ensure its last_doc_id is no less than given doc_id.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
    // Caller may invoke BlockMaxBM25Score() after this routine.
    bool NextShallow(RowID doc_id) override;

    float BM25Score() override;

    float Score() override {
        switch (ft_similarity_) {
            case FulltextSimilarity::kBM25: {
                return BM25Score();
            }
            case FulltextSimilarity::kBoolean: {
                return GetWeight();
            }
        }
    }

    void UpdateScoreThreshold(float threshold) override {
        if (threshold > threshold_)
            threshold_ = threshold;
    }

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

    u32 doc_freq_ = 0;

    Vector<UniquePtr<PostingIterator>> pos_iters_;
    float weight_;
    u32 slop_{};
    const FulltextSimilarity ft_similarity_ = FulltextSimilarity::kBM25;

    // for BM25 Score
    float bm25_score_cache_ = 0.0f;
    RowID bm25_score_cache_docid_ = INVALID_ROWID;
    float f1 = 0.0f;
    float f2 = 0.0f;
    float f3 = 0.0f;
    float f4 = 0.0f;
    float bm25_common_score_ = 0.0f;
    UniquePtr<FullTextColumnLengthReader> column_length_reader_ = nullptr;
    float block_max_bm25_score_cache_ = 0.0f;
    RowID block_max_bm25_score_cache_end_id_ = INVALID_ROWID;
    Vector<RowID> block_max_bm25_score_cache_part_info_end_ids_;
    Vector<float> block_max_bm25_score_cache_part_info_vals_;
    RowID block_min_possible_doc_id_ = INVALID_ROWID;
    RowID block_last_doc_id_ = INVALID_ROWID;

    float tf_ = 0.0f;          // current doc_id_'s tf
    u32 estimate_doc_freq_{0}; // estimated at the beginning
    float phrase_freq_{0.0f};  // increase during search
    Vector<float> all_tf_;     // increase during search

    // debug statistics
    u32 calc_score_cnt_ = 0;
};
} // namespace infinity
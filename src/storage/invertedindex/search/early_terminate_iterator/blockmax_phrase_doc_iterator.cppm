module;

export module blockmax_phrase_doc_iterator;

import stl;
import index_defines;
import internal_types;
import early_terminate_iterator;
import phrase_doc_iterator;
import blockmax_term_doc_iterator;
import column_length_io;
import posting_iterator;
import match_data;
import third_party;

namespace infinity {

export class BlockMaxPhraseDocIterator final : public EarlyTerminateIterator {
public:
//    BlockMaxPhraseDocIterator(Vector<UniquePtr<BlockMaxTermDocIterator>> &&iters)
//        : term_doc_iters_(std::move(iters)) {
//        auto iter_size = term_doc_iters_.size();
//        fmt::print("iter size = {}\n", iter_size);
//        term_column_length_reader_.resize(iter_size, nullptr);
//        term_block_max_bm25_score_cache_.resize(iter_size, 0.0f);
//        term_bm25_common_score_.resize(iter_size, 0.0f);
//        term_block_max_bm25_score_cache_end_id_.resize(iter_size, INVALID_ROWID);
//        term_bm25_score_upper_bound_.resize(iter_size, 0.0f);
//        term_column_length_reader_.resize(iter_size, nullptr);
//        // cache for PeekInBlockRange
//        term_peek_doc_id_range_start_.resize(iter_size, INVALID_ROWID);
//        term_peek_doc_id_range_end_.resize(iter_size, INVALID_ROWID);
//        term_peek_doc_id_val_.resize(iter_size, INVALID_ROWID);
//        term_doc_id_.resize(iter_size, INVALID_ROWID);
//    }

    BlockMaxPhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters)
        : pos_iters_(std::move(iters)) {
        auto iter_size = pos_iters_.size();
        fmt::print("iter size = {}\n", iter_size);
        term_column_length_reader_.resize(iter_size, nullptr);
        term_block_max_bm25_score_cache_.resize(iter_size, 0.0f);
        term_bm25_common_score_.resize(iter_size, 0.0f);
        term_block_max_bm25_score_cache_end_id_.resize(iter_size, INVALID_ROWID);
        term_bm25_score_upper_bound_.resize(iter_size, 0.0f);
        term_column_length_reader_.resize(iter_size, nullptr);
        // cache for PeekInBlockRange
        term_peek_doc_id_range_start_.resize(iter_size, INVALID_ROWID);
        term_peek_doc_id_range_end_.resize(iter_size, INVALID_ROWID);
        term_peek_doc_id_val_.resize(iter_size, INVALID_ROWID);
        term_doc_id_.resize(iter_size, INVALID_ROWID);
        if (iter_size) {
            estimate_doc_freq_ = pos_iters_[0]->GetDocFreq();
        } else {
            estimate_doc_freq_ = 0;
        }
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            estimate_doc_freq_ = std::min(estimate_doc_freq_, pos_iters_[i]->GetDocFreq());
        }
    }

    void UpdateScoreThreshold(float threshold) override {} // do nothing

    bool BlockSkipTo(RowID doc_id, float threshold) override;

    RowID BlockMinPossibleDocID() const override;

    RowID BlockLastDocID() const override;

    float BlockMaxBM25Score() override;

    Pair<bool, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override { return {}; }

    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) override;
    // float BM25Score() override { return 0.0f; }

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

    Pair<u32, u16> GetBlockMaxInfo() const { return TermGetBlockMaxInfo(0); }

    bool NotPartCheckExist(RowID doc_id) override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    void InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader);

    float BM25Score() override;

    bool CheckBeginPosition(pos_t position);

    bool GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id);

    // debug info
    const Vector<String> *terms_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    float TermBlockMaxBM25Score(u32 term_id);

    Pair<u32, u16> TermGetBlockMaxInfo(u32 term_id) const { return pos_iters_[term_id]->GetBlockMaxInfo(); }

    Pair<bool, RowID> TermPeekInBlockRange(u32 term_id, RowID doc_id, RowID doc_id_no_beyond);

    RowID TermBlockLastDocID(u32 term_id) const;

    Tuple<bool, float, RowID> TermSeekInBlockRange(u32 term_id, RowID doc_id, RowID doc_id_no_beyond, float threshold);

    float TermBM25Score(u32 term_id);

    float TermBM25Score(u32 term_id, tf_t phrase_freq);

    void TermInitBM25Info(u32 term_id, u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader);

    bool TermNotPartCheckExist(u32 term_id, RowID doc_id);

    void SeekDoc(RowID doc_id, RowID seek_end);
private:
    float avg_column_len_ = 0;
    Vector<UniquePtr<BlockMaxTermDocIterator>> term_doc_iters_{};
    Vector<UniquePtr<PostingIterator>> pos_iters_{};
    u64 phrase_freq_{0};
    u64 current_phrase_freq_{0};
    Set<RowID> all_doc_ids_{};
    FullTextColumnLengthReader* column_length_reader_{nullptr};
    u32 estimate_doc_freq_{0};
    float weight_ = 1.0f;
    float bm25_common_score_ = 0.0f;
    float block_max_bm25_score_cache_ = 0.0f;
    RowID block_max_bm25_score_cache_end_id_{INVALID_ROWID};

    // info for sub terms
    Vector<FullTextColumnLengthReader*> term_column_length_reader_;
    Vector<float> term_block_max_bm25_score_cache_;
    Vector<float> term_bm25_common_score_;
    Vector<float> term_bm25_score_upper_bound_;
    Vector<RowID> term_doc_id_;
    Vector<RowID> term_block_max_bm25_score_cache_end_id_;

    // cache for PeekInBlockRange
    Vector<RowID> term_peek_doc_id_range_start_;
    Vector<RowID> term_peek_doc_id_range_end_;
    Vector<RowID> term_peek_doc_id_val_;
};

}
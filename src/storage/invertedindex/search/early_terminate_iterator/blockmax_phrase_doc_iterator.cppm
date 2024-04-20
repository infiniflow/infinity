module;

export module blockmax_phrase_doc_iterator;

import stl;
import index_defines;
import internal_types;
import early_terminate_iterator;
import phrase_doc_iterator;
import blockmax_term_doc_iterator;
import column_length_io;

namespace infinity {

export class BlockMaxPhraseDocIterator final : public EarlyTerminateIterator {
public:
    BlockMaxPhraseDocIterator(Vector<UniquePtr<BlockMaxTermDocIterator>> &&iters, u64 column_id)
        : term_doc_iters_(std::move(iters)), column_id_(column_id) {}

    void UpdateScoreThreshold(float threshold) override {} // do nothing

    bool BlockSkipTo(RowID doc_id, float threshold) override;

    RowID BlockMinPossibleDocID() const override;

    RowID BlockLastDocID() const override;

    float BlockMaxBM25Score() override;

    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) override;

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

    bool NotPartCheckExist(RowID doc_id) override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    void InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader);

    // debug info
    const Vector<String> *terms_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    Vector<UniquePtr<BlockMaxTermDocIterator>> term_doc_iters_;
    u64 column_id_;
    float weight_ = 1.0f;
};

}
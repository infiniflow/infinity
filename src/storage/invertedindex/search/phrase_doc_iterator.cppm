module;

export module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import match_data;
import index_defines;

namespace infinity {
export class PhraseDocIterator final : public DocIterator {
public:
    PhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters, float weight, u32 slop = 0)
        : pos_iters_(std::move(iters)), weight_(weight), slop_(slop) {
        doc_ids_.resize(pos_iters_.size());
        doc_freq_ = 0;
        phrase_freq_ = 0;
        if (pos_iters_.size()) {
            estimate_doc_freq_ = pos_iters_[0]->GetDocFreq();
        } else {
            estimate_doc_freq_ = 0;
        }
        for (SizeT i = 0; i < pos_iters_.size(); ++i) {
            all_df_.push_back(pos_iters_[i]->GetDocFreq());
            estimate_doc_freq_ = std::min(estimate_doc_freq_, pos_iters_[i]->GetDocFreq());
        }
    }

    void DoSeek(RowID doc_id) override;

    u32 GetDF() const override;

    u32 GetEstimateDF() const { return estimate_doc_freq_; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    float GetWeight() const { return weight_; }

    bool GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
        if (doc_id != doc_id_) {
            return false;
        }
        match_data.doc_id_ = doc_id;
        match_data.tf_ = 0.0F;
        match_data.begin_positions_.clear();
        if (slop_ == 0) {
            return GetExactPhraseMatchData(match_data, doc_id);
        } else {
            return GetSloppyPhraseMatchData(match_data, doc_id);
        }
    }

    DocIteratorType GetType() const override { return DocIteratorType::kPhraseIterator; }

    const Vector<u32>& GetAllDF() const;

    float GetPhraseFreq() const { return phrase_freq_; }

    // debug info
    const Vector<String> *terms_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    bool GetExactPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id);
    bool GetSloppyPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id);
    Vector<UniquePtr<PostingIterator>> pos_iters_;
    Vector<RowID> doc_ids_;
    u32 doc_freq_{0};
    u32 estimate_doc_freq_{0};
    float phrase_freq_{0.0F};
    Set<RowID> all_doc_ids_{};
    float weight_;
    Vector<u32> all_df_;
    u32 slop_{};
};
}
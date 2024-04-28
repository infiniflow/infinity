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
    PhraseDocIterator(Vector<UniquePtr<PostingIterator>> &&iters, u64 column_id, float weight, u32 slop = 1)
        : iters_(std::move(iters)), column_id_(column_id), weight_(weight), slop_(slop) {
        doc_ids_.resize(iters_.size());
        doc_freq_ = 0;
        phrase_freq_ = 0;
        for (SizeT i = 0; i < iters_.size(); ++i) {
            all_df_.push_back(iters_[i]->GetDocFreq());
            // doc_freq_ = std::max(doc_freq_, iters_[i]->GetDocFreq());
        }
    }

    void DoSeek(RowID doc_id) override;

    u32 GetDF() const override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    float GetWeight() const { return weight_; }

    bool GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id);

    bool CheckBeginPosition(pos_t position);

    DocIteratorType GetType() const override { return DocIteratorType::kPhraseIterator; }

    const Vector<u32>& GetAllDF() const;

    u64 GetPhraseFreq() const { return phrase_freq_; }

    // debug info
    const Vector<String> *terms_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    Vector<UniquePtr<PostingIterator>> iters_;
    Vector<RowID> doc_ids_;
    u64 column_id_;
    u32 doc_freq_{0};
    u64 phrase_freq_{0};
    Set<RowID> all_doc_ids_{};
    float weight_;
    Vector<u32> all_df_;
    u32 slop_; // unused
};
}
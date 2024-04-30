module;

#include <iostream>
#include <cassert>

module phrase_doc_iterator;

import stl;
import doc_iterator;
import internal_types;
import third_party;
import posting_iterator;
import match_data;

namespace infinity {
    void PhraseDocIterator::DoSeek(RowID doc_id) {
        assert(iters_.size() > 0);

        bool need_loop = true;
        while (need_loop) {
            if (doc_id == INVALID_ROWID) {
                break;
            }
            doc_ids_[0] = iters_[0]->SeekDoc(doc_id);
            RowID max_doc_id = doc_ids_[0];
            need_loop = false;
            for (SizeT i = 1; i < iters_.size(); ++i) {
                auto& iter = iters_[i];
                doc_ids_[i] = iter->SeekDoc(doc_id);
                if (max_doc_id != doc_ids_[i]) {
                    max_doc_id = std::max(max_doc_id, doc_ids_[i]);
                    need_loop = true;
                    break;
                }
            }
            doc_id = max_doc_id;
        }
        doc_id_ = doc_id;
        for (SizeT i = 0; i < iters_.size(); ++i) {
            iters_[i]->SeekDoc(doc_id_);
        }
    }

    void PhraseDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
        os << prefix;
        os << (is_final ? "└──" : "├──");
        os << "PhraseDocIterator";
        os << " (weight: " << weight_ << ")";
        os << " (column: " << *column_name_ptr_ << ")";
        os << " (phrase: ";
        for (auto term : *terms_ptr_) {
            os << term << " ";
        }
        os << ")";
        os << " (doc_freq: " << GetDF() << ")";
        os << '\n';
    }

    bool PhraseDocIterator::CheckBeginPosition(pos_t position) {
        pos_t now_position = position;
        for (SizeT i = 1; i < iters_.size(); ++i) {
            auto& iter = iters_[i];
            pos_t next_position = 0;
            iter->SeekPosition(now_position, next_position);
            fmt::print("iter: {}, last_position: {}, next_position: {}\n", i, now_position, next_position);
            if (next_position != now_position + 1) {
                return false;
            }
            now_position = next_position;
        }
        return true;
    }

    bool PhraseDocIterator::GetPhraseMatchData(PhraseColumnMatchData &match_data, RowID doc_id) {
        if (doc_id != doc_id_) {
            return false;
        }
        auto& iter = iters_[0];
        pos_t beg_position = 0;
        match_data.tf_ = 0;
        while (true) {
            pos_t position = INVALID_POSITION;
            iter->SeekPosition(beg_position, position);
            fmt::print("doc_id = {}, position = {}\n", doc_id_.ToUint64(), position);
            if (position == INVALID_POSITION) {
                break;
            }
            if (CheckBeginPosition(position)) {
                match_data.begin_positions_.push_back(position);
                ++(match_data.tf_);
            }
            beg_position = position + 1;
        }
        match_data.doc_id_ = doc_id_;
        if (match_data.begin_positions_.size()) {
            for (SizeT i = 0; i < iters_.size(); ++i) {
                auto& iter = iters_[i];
                match_data.all_tf_.emplace_back(iter->GetCurrentTF());
                match_data.all_doc_payload_.emplace_back(iter->GetCurrentDocPayload());
            }
            if (all_doc_ids_.count(doc_id_) == 0) {
                all_doc_ids_.insert(doc_id_);
                doc_freq_++;
                phrase_freq_ += match_data.begin_positions_.size();
            }
            return true;
        }
        return false;
    }

    const Vector<u32>& PhraseDocIterator::GetAllDF() const {
        return all_df_;
    }

    u32 PhraseDocIterator::GetDF() const {
        return doc_freq_;
    }
}
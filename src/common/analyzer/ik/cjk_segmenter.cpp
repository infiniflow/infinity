module;

#include <string>

import stl;
import hit;
import segmenter;
import analyze_context;
import lexeme;
import character_util;
import ik_dict;

module cjk_segmenter;

namespace infinity {
const std::wstring CJKSegmenter::SEGMENTER_NAME = L"CJK_SEGMENTER";

CJKSegmenter::CJKSegmenter(Dictionary *dict) : dict_(dict) { tmp_hits_ = List<Hit *>(); }

void CJKSegmenter::Analyze(AnalyzeContext *context) {
    if (CharacterUtil::CHAR_USELESS != context->GetCurrentCharType()) {
        if (!tmp_hits_.empty()) {
            std::vector<Hit *> tmp_array(tmp_hits_.begin(), tmp_hits_.end());
            for (Hit *hit : tmp_array) {
                hit = dict_->MatchWithHit(context->GetSegmentBuff(), context->GetCursor(), hit);
                if (hit->IsMatch()) {
                    Lexeme *newLexeme =
                        new Lexeme(context->GetBufferOffset(), hit->GetBegin(), context->GetCursor() - hit->GetBegin() + 1, Lexeme::TYPE_CNWORD);
                    context->AddLexeme(newLexeme);

                    if (!hit->IsPrefix()) {
                        tmp_hits_.remove(hit);
                    }

                } else if (hit->IsUnmatch()) {
                    tmp_hits_.remove(hit);
                }
            }
        }

        Hit *single_char_hit = dict_->MatchInMainDict(context->GetSegmentBuff(), context->GetCursor(), 1);
        if (single_char_hit->IsMatch()) {
            Lexeme *newLexeme = new Lexeme(context->GetBufferOffset(), context->GetCursor(), 1, Lexeme::TYPE_CNWORD);
            context->AddLexeme(newLexeme);

            if (single_char_hit->IsPrefix()) {
                tmp_hits_.push_back(single_char_hit);
            }
        } else if (single_char_hit->IsPrefix()) {
            tmp_hits_.push_back(single_char_hit);
        }

    } else {
        tmp_hits_.clear();
    }

    if (context->IsBufferConsumed()) {
        tmp_hits_.clear();
    }

    if (tmp_hits_.empty()) {
        context->UnlockBuffer(SEGMENTER_NAME);
    } else {
        context->LockBuffer(SEGMENTER_NAME);
    }
}

void CJKSegmenter::Reset() { tmp_hits_.clear(); }

} // namespace infinity
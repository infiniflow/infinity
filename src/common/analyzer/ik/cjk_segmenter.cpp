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

CJKSegmenter::CJKSegmenter(Dictionary *dict) : dict_(dict) {}

void CJKSegmenter::Analyze(AnalyzeContext *context) {
    if (CharacterUtil::CHAR_USELESS != context->GetCurrentCharType()) {
        if (!tmp_hits_.empty()) {
            for (auto it = tmp_hits_.begin(); it != tmp_hits_.end();) {
                Hit *hit = (*it).get();
                hit = dict_->MatchWithHit(context->GetSegmentBuff(), context->GetCursor(), hit);
                if (hit->IsMatch()) {
                    Lexeme *newLexeme =
                        new Lexeme(context->GetBufferOffset(), hit->GetBegin(), context->GetCursor() - hit->GetBegin() + 1, Lexeme::TYPE_CNWORD);
                    context->AddLexeme(newLexeme);

                    if (!hit->IsPrefix()) {
                        it = tmp_hits_.erase(it);
                    } else {
                        ++it;
                    }

                } else if (hit->IsUnmatch()) {
                    it = tmp_hits_.erase(it);
                } else {
                    ++it;
                }
            }
        }

        Hit *single_char_hit = dict_->MatchInMainDict(context->GetSegmentBuff(), context->GetCursor(), 1);
        if (single_char_hit->IsMatch()) {
            Lexeme *newLexeme = new Lexeme(context->GetBufferOffset(), context->GetCursor(), 1, Lexeme::TYPE_CNWORD);
            context->AddLexeme(newLexeme);

            if (single_char_hit->IsPrefix()) {
                tmp_hits_.push_back(UniquePtr<Hit>(single_char_hit));
            }
        } else if (single_char_hit->IsPrefix()) {
            tmp_hits_.push_back(UniquePtr<Hit>(single_char_hit));
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
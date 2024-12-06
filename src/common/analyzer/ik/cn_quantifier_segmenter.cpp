module;

#include <string>

module cn_quantifier_segmenter;

import stl;
import hit;
import segmenter;
import analyze_context;
import lexeme;
import character_util;
import ik_dict;

namespace infinity {

const std::wstring CNQuantifierSegmenter::SEGMENTER_NAME = L"QUAN_SEGMENTER";
const std::wstring CNQuantifierSegmenter::ChnNum = L"一二两三四五六七八九十零壹贰叁肆伍陆柒捌玖拾百千万亿拾佰仟萬億兆卅廿";
HashSet<wchar_t> CNQuantifierSegmenter::ChnNumberChars;

void CNQuantifierSegmenter::InitChnNumber() {
    for (wchar_t nChar : ChnNum) {
        ChnNumberChars.insert(nChar);
    }
}

CNQuantifierSegmenter::CNQuantifierSegmenter(Dictionary *dict) : dict_(dict) {
    nstart_ = -1;
    nend_ = -1;
    InitChnNumber();
}

void CNQuantifierSegmenter::Analyze(AnalyzeContext *context) {
    ProcessCNumber(context);
    ProcessCount(context);
}

void CNQuantifierSegmenter::Reset() {
    nstart_ = -1;
    nend_ = -1;
    count_hits_.clear();
}

void CNQuantifierSegmenter::ProcessCNumber(AnalyzeContext *context) {
    if (nstart_ == -1 && nend_ == -1) {
        if (CharacterUtil::CHAR_CHINESE == context->GetCurrentCharType() && ChnNumberChars.find(context->GetCurrentChar()) != ChnNumberChars.end()) {
            nstart_ = context->GetCursor();
            nend_ = context->GetCursor();
        }
    } else {
        if (CharacterUtil::CHAR_CHINESE == context->GetCurrentCharType() && ChnNumberChars.find(context->GetCurrentChar()) != ChnNumberChars.end()) {
            nend_ = context->GetCursor();
        } else {
            OutputNumLexeme(context);
            nstart_ = -1;
            nend_ = -1;
        }
    }

    if (context->IsBufferConsumed() && (nstart_ != -1 && nend_ != -1)) {
        OutputNumLexeme(context);
        nstart_ = -1;
        nend_ = -1;
    }
}
void CNQuantifierSegmenter::ProcessCount(AnalyzeContext *context) {
    if (!NeedCountScan(context)) {
        return;
    }

    if (CharacterUtil::CHAR_CHINESE == context->GetCurrentCharType()) {
        if (!count_hits_.empty()) {
            for (auto it = count_hits_.begin(); it != count_hits_.end();) {
                Hit *hit = (*it).get();
                hit = dict_->MatchWithHit(context->GetSegmentBuff(), context->GetCursor(), hit);
                if (hit->IsMatch()) {
                    Lexeme *new_lexeme =
                        new Lexeme(context->GetBufferOffset(), hit->GetBegin(), context->GetCursor() - hit->GetBegin() + 1, Lexeme::TYPE_COUNT);
                    if (!context->AddLexeme(new_lexeme))
                        delete new_lexeme;

                    if (!hit->IsPrefix()) {
                        it = count_hits_.erase(it);
                    } else {
                        ++it;
                    }

                } else if (hit->IsUnmatch()) {
                    it = count_hits_.erase(it);
                } else {
                    ++it;
                }
            }
        }

        UniquePtr<Hit> single_char_hit(dict_->MatchInQuantifierDict(context->GetSegmentBuff(), context->GetCursor(), 1));
        if (single_char_hit->IsMatch()) {
            Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), context->GetCursor(), 1, Lexeme::TYPE_COUNT);
            if (!context->AddLexeme(new_lexeme))
                delete new_lexeme;

            if (single_char_hit->IsPrefix()) {
                count_hits_.push_back(std::move(single_char_hit));
            }
        } else if (single_char_hit->IsPrefix()) {
            count_hits_.push_back(std::move(single_char_hit));
        }

    } else {
        count_hits_.clear();
    }

    if (context->IsBufferConsumed()) {
        count_hits_.clear();
    }
}

bool CNQuantifierSegmenter::NeedCountScan(AnalyzeContext *context) {
    if ((nstart_ != -1 && nend_ != -1) || !count_hits_.empty()) {
        return true;
    } else {
        if (!context->GetOrgLexemes()->IsEmpty()) {
            Lexeme *l = context->GetOrgLexemes()->PeekLast();
            if ((Lexeme::TYPE_CNUM == l->GetLexemeType() || Lexeme::TYPE_ARABIC == l->GetLexemeType()) &&
                (l->GetBegin() + l->GetLength() == context->GetCursor())) {
                return true;
            }
        }
    }
    return false;
}

void CNQuantifierSegmenter::OutputNumLexeme(AnalyzeContext *context) {
    if (nstart_ > -1 && nend_ > -1) {
        Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), nstart_, nend_ - nstart_ + 1, Lexeme::TYPE_CNUM);
        if (!context->AddLexeme(new_lexeme))
            delete new_lexeme;
    }
}

} // namespace infinity

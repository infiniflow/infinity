module;

#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>

import stl;
import quick_sort_set;
import character_util;
import lexeme;
import lexeme_path;
import ik_dict;

module analyze_context;

namespace infinity {
AnalyzeContext::AnalyzeContext(Dictionary *dict) : dict_(dict) {
    buff_offset_ = 0;
    cursor_ = 0;
    last_useless_char_num_ = 0;
    available_ = 0;
}

int AnalyzeContext::FillBuffer(std::wstring &text) {
    cursor_ = 0;
    int read_count = text.size();
    segment_buff_.assign(text.begin(), text.end());
    char_types_.resize(segment_buff_.size());
    last_useless_char_num_ = 0;
    available_ = read_count;
    cursor_ = 0;
    return read_count;
}

void AnalyzeContext::InitCursor() {
    cursor_ = 0;
    segment_buff_[cursor_] = CharacterUtil::Regularize(segment_buff_[cursor_], true);
    char_types_[cursor_] = CharacterUtil::IdentifyCharType(segment_buff_[cursor_]);
}

bool AnalyzeContext::MoveCursor() {
    if (cursor_ < available_ - 1) {
        cursor_++;
        segment_buff_[cursor_] = CharacterUtil::Regularize(segment_buff_[cursor_], true);
        char_types_[cursor_] = CharacterUtil::IdentifyCharType(segment_buff_[cursor_]);
        return true;
    } else {
        return false;
    }
}

bool AnalyzeContext::AddLexeme(Lexeme *lexeme) { return org_lexemes_->AddLexeme(lexeme); }

void AnalyzeContext::AddLexemePath(LexemePath *path) {
    if (path != nullptr) {
        path_map_[path->GetPathBegin()] = UniquePtr<LexemePath>(path);
    }
}

void AnalyzeContext::OutputToResult() {
    int index = 0;
    for (; index <= cursor_;) {
        if (CharacterUtil::CHAR_USELESS == char_types_[index]) {
            index++;
            last_useless_char_num_++;
            continue;
        }
        last_useless_char_num_ = 0;
        LexemePath *path = path_map_[index].get();
        if (path != nullptr) {
            Lexeme *l = path->PollFirst();
            while (l != nullptr) {
                results_.push_back(l);
                index = l->GetBegin() + l->GetLength();
                l = path->PollFirst();
                if (l != nullptr) {
                    for (; index < l->GetBegin(); index++) {
                        OutputSingleCJK(index);
                    }
                }
            }
        } else {
            OutputSingleCJK(index);
            index++;
        }
    }
    path_map_.clear();
}

void AnalyzeContext::OutputSingleCJK(int index) {
    if (CharacterUtil::CHAR_CHINESE == char_types_[index]) {
        Lexeme *single_char_lexeme = new Lexeme(buff_offset_, index, 1, Lexeme::TYPE_CNCHAR);
        results_.push_back(single_char_lexeme);
    } else if (CharacterUtil::CHAR_OTHER_CJK == char_types_[index]) {
        Lexeme *single_char_lexeme = new Lexeme(buff_offset_, index, 1, Lexeme::TYPE_OTHER_CJK);
        results_.push_back(single_char_lexeme);
    }
}

Lexeme *AnalyzeContext::GetNextLexeme() {
    Lexeme *result = nullptr;
    while (!results_.empty()) {
        result = results_.front();
        results_.pop_front();
        Compound(result);
        if (!dict_->IsStopWord(segment_buff_, result->GetBegin(), result->GetLength())) {
            result->SetLexemeText(
                std::wstring(segment_buff_.begin() + result->GetBegin(), segment_buff_.begin() + result->GetBegin() + result->GetLength()));
            break;
        }
    }
    return result;
}

void AnalyzeContext::Reset() {
    org_lexemes_ = MakeUnique<QuickSortSet>();
    available_ = 0;
    buff_offset_ = 0;
    char_types_.clear();
    cursor_ = 0;
    results_.clear();
    segment_buff_.clear();
    path_map_.clear();
}

void AnalyzeContext::Compound(Lexeme *result) {
    if (!results_.empty()) {
        if (Lexeme::TYPE_ARABIC == result->GetLexemeType()) {
            Lexeme *next_lexeme = results_.front();
            bool append_ok = false;
            if (Lexeme::TYPE_CNUM == next_lexeme->GetLexemeType()) {
                append_ok = result->Append(*next_lexeme, Lexeme::TYPE_CNUM);
            } else if (Lexeme::TYPE_COUNT == next_lexeme->GetLexemeType()) {
                append_ok = result->Append(*next_lexeme, Lexeme::TYPE_CQUAN);
            }
            if (append_ok) {
                results_.pop_front();
            }
        }
        if (Lexeme::TYPE_CNUM == result->GetLexemeType() && !results_.empty()) {
            Lexeme *next_lexeme = results_.front();
            bool append_ok = false;
            if (Lexeme::TYPE_COUNT == next_lexeme->GetLexemeType()) {
                append_ok = result->Append(*next_lexeme, Lexeme::TYPE_CQUAN);
            }
            if (append_ok) {
                results_.pop_front();
            }
        }
    }
}
} // namespace infinity
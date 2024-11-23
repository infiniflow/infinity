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
AnalyzeContext::AnalyzeContext() {
    segment_buff_.resize(BUFF_SIZE);
    char_types_.resize(BUFF_SIZE);
    buff_locker_ = HashSet<std::wstring>();
    org_lexemes_ = QuickSortSet();
    path_map_ = HashMap<int, LexemePath *>();
    results_ = List<Lexeme *>();
    buff_offset_ = 0;
    cursor_ = 0;
    last_useless_char_num_ = 0;
    available_ = 0;
}

int AnalyzeContext::FillBuffer(std::wifstream &reader) {
    int read_count = 0;
    if (buff_offset_ == 0) {
        reader.read(&segment_buff_[0], BUFF_SIZE);
        read_count = reader.gcount();
        last_useless_char_num_ = 0;
    } else {
        int offset = available_ - cursor_;
        if (offset > 0) {
            std::copy(segment_buff_.begin() + cursor_, segment_buff_.begin() + available_, segment_buff_.begin());
            read_count = offset;
        }
        reader.read(&segment_buff_[offset], BUFF_SIZE - offset);
        read_count += reader.gcount();
    }
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

bool AnalyzeContext::NeedRefillBuffer() const {
    return available_ == BUFF_SIZE && cursor_ < available_ - 1 && cursor_ > available_ - BUFF_EXHAUST_CRITICAL && !IsBufferLocked();
}

void AnalyzeContext::AddLexeme(Lexeme *lexeme) { org_lexemes_.AddLexeme(lexeme); }

void AnalyzeContext::AddLexemePath(LexemePath *path) {
    if (path != nullptr) {
        path_map_[path->GetPathBegin()] = path;
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
        LexemePath *path = path_map_[index];
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
    Lexeme *result = results_.front();
    results_.pop_front();
    while (result != nullptr) {
        Compound(result);
        if (dict_->IsStopWord(segment_buff_, result->GetBegin(), result->GetLength())) {
            result = results_.front();
            results_.pop_front();
        } else {
            result->SetLexemeText(
                std::wstring(segment_buff_.begin() + result->GetBegin(), segment_buff_.begin() + result->GetBegin() + result->GetLength()));
            break;
        }
    }
    return result;
}

void AnalyzeContext::Reset() {
    buff_locker_.clear();
    org_lexemes_ = QuickSortSet();
    available_ = 0;
    buff_offset_ = 0;
    char_types_.clear();
    char_types_.resize(BUFF_SIZE);
    cursor_ = 0;
    results_.clear();
    segment_buff_.clear();
    segment_buff_.resize(BUFF_SIZE);
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
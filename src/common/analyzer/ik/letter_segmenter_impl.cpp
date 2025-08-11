module;

#include <string>

module infinity_core:letter_segmenter.impl;

import :letter_segmenter;
import :stl;
import :segmenter;
import :analyze_context;
import :lexeme;
import :character_util;

namespace infinity {
const std::wstring LetterSegmenter::SEGMENTER_NAME = L"LETTER_SEGMENTER";
Vector<wchar_t> LetterSegmenter::Letter_Connector = {L'#', L'&', L'+', L'-', L'.', L'@', L'_'};
Vector<wchar_t> LetterSegmenter::Num_Connector = {L',', L'.'};

LetterSegmenter::LetterSegmenter() {
    std::sort(Letter_Connector.begin(), Letter_Connector.end());
    std::sort(Num_Connector.begin(), Num_Connector.end());
    start_ = -1;
    end_ = -1;
    english_start_ = -1;
    english_end_ = -1;
    arabic_start_ = -1;
    arabic_end_ = -1;
}

void LetterSegmenter::Analyze(AnalyzeContext *context) {
    ProcessEnglishLetter(context);
    ProcessArabicLetter(context);
    ProcessMixLetter(context);
}

void LetterSegmenter::Reset() {
    start_ = -1;
    end_ = -1;
    english_start_ = -1;
    english_end_ = -1;
    arabic_start_ = -1;
    arabic_end_ = -1;
}

bool LetterSegmenter::ProcessMixLetter(AnalyzeContext *context) {
    bool need_lock = false;

    if (start_ == -1) {
        if (CharacterUtil::CHAR_ARABIC == context->GetCurrentCharType() || CharacterUtil::CHAR_ENGLISH == context->GetCurrentCharType()) {
            start_ = context->GetCursor();
            end_ = start_;
        }
    } else {
        if (CharacterUtil::CHAR_ARABIC == context->GetCurrentCharType() || CharacterUtil::CHAR_ENGLISH == context->GetCurrentCharType()) {
            end_ = context->GetCursor();
        } else if (CharacterUtil::CHAR_USELESS == context->GetCurrentCharType() && IsLetterConnector(context->GetCurrentChar())) {
            end_ = context->GetCursor();
        } else {
            Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), start_, end_ - start_ + 1, Lexeme::TYPE_LETTER);
            if (!context->AddLexeme(new_lexeme))
                delete new_lexeme;
            start_ = -1;
            end_ = -1;
        }
    }

    if (context->IsBufferConsumed() && (start_ != -1 && end_ != -1)) {
        Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), start_, end_ - start_ + 1, Lexeme::TYPE_LETTER);
        if (!context->AddLexeme(new_lexeme))
            delete new_lexeme;
        start_ = -1;
        end_ = -1;
    }

    if (start_ == -1 && end_ == -1) {
        need_lock = false;
    } else {
        need_lock = true;
    }
    return need_lock;
}

bool LetterSegmenter::ProcessEnglishLetter(AnalyzeContext *context) {
    bool need_lock = false;

    if (english_start_ == -1) {
        if (CharacterUtil::CHAR_ENGLISH == context->GetCurrentCharType()) {
            english_start_ = context->GetCursor();
            english_end_ = english_start_;
        }
    } else {
        if (CharacterUtil::CHAR_ENGLISH == context->GetCurrentCharType()) {
            english_end_ = context->GetCursor();
        } else {
            Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), english_start_, english_end_ - english_start_ + 1, Lexeme::TYPE_ENGLISH);
            if (!context->AddLexeme(new_lexeme))
                delete new_lexeme;
            english_start_ = -1;
            english_end_ = -1;
        }
    }

    if (context->IsBufferConsumed() && (english_start_ != -1 && english_end_ != -1)) {
        Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), english_start_, english_end_ - english_start_ + 1, Lexeme::TYPE_ENGLISH);
        if (!context->AddLexeme(new_lexeme))
            delete new_lexeme;
        english_start_ = -1;
        english_end_ = -1;
    }

    if (english_start_ == -1 && english_end_ == -1) {
        need_lock = false;
    } else {
        need_lock = true;
    }
    return need_lock;
}

bool LetterSegmenter::ProcessArabicLetter(AnalyzeContext *context) {
    bool need_lock = false;

    if (arabic_start_ == -1) {
        if (CharacterUtil::CHAR_ARABIC == context->GetCurrentCharType()) {
            arabic_start_ = context->GetCursor();
            arabic_end_ = arabic_start_;
        }
    } else {
        if (CharacterUtil::CHAR_ARABIC == context->GetCurrentCharType()) {
            arabic_end_ = context->GetCursor();
        } else if (CharacterUtil::CHAR_USELESS == context->GetCurrentCharType() && IsNumConnector(context->GetCurrentChar())) {
        } else {
            Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), arabic_start_, arabic_end_ - arabic_start_ + 1, Lexeme::TYPE_ARABIC);
            if (!context->AddLexeme(new_lexeme))
                delete new_lexeme;
            arabic_start_ = -1;
            arabic_end_ = -1;
        }
    }

    if (context->IsBufferConsumed() && (arabic_start_ != -1 && arabic_end_ != -1)) {
        Lexeme *new_lexeme = new Lexeme(context->GetBufferOffset(), arabic_start_, arabic_end_ - arabic_start_ + 1, Lexeme::TYPE_ARABIC);
        if (!context->AddLexeme(new_lexeme))
            delete new_lexeme;
        arabic_start_ = -1;
        arabic_end_ = -1;
    }

    if (arabic_start_ == -1 && arabic_end_ == -1) {
        need_lock = false;
    } else {
        need_lock = true;
    }
    return need_lock;
}

bool LetterSegmenter::IsLetterConnector(wchar_t input) {
    auto it = std::lower_bound(Letter_Connector.begin(), Letter_Connector.end(), input);
    return it != Letter_Connector.end() && *it == input;
}

bool LetterSegmenter::IsNumConnector(wchar_t input) {
    auto it = std::lower_bound(Num_Connector.begin(), Num_Connector.end(), input);
    return it != Num_Connector.end() && *it == input;
}

} // namespace infinity
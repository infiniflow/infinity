module;

#include <string>

export module lexeme;

import stl;

namespace infinity {
export class Lexeme {
public:
    static constexpr int TYPE_UNKNOWN = 0;
    static constexpr int TYPE_ENGLISH = 1;
    static constexpr int TYPE_ARABIC = 2;
    static constexpr int TYPE_LETTER = 3;
    static constexpr int TYPE_CNWORD = 4;
    static constexpr int TYPE_CNCHAR = 64;
    static constexpr int TYPE_OTHER_CJK = 8;
    static constexpr int TYPE_CNUM = 16;
    static constexpr int TYPE_COUNT = 32;
    static constexpr int TYPE_CQUAN = 48;

    int offset_;

    int begin_;

    int length_;

    std::wstring lexeme_text_;

    int lexeme_type_;

    Lexeme(int offset, int begin, int length, int lexeme_type);

    bool Equals(const Lexeme &other) const { return offset_ == other.offset_ && begin_ == other.begin_ && length_ == other.length_; }

    int Hash() const {
        int abs_begin = GetBeginPosition();
        int abs_end = GetEndPosition();
        return (abs_begin * 37) + (abs_end * 31) + ((abs_begin * abs_end) % length_) * 11;
    }

    int CompareTo(const Lexeme &other) const {
        if (begin_ < other.begin_) {
            return -1;
        } else if (begin_ == other.begin_) {
            if (length_ > other.length_) {
                return -1;
            } else if (length_ == other.length_) {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }

    int GetOffset() const { return offset_; }

    void SetOffset(int offset) { offset_ = offset; }

    int GetBegin() const { return begin_; }

    int GetBeginPosition() const { return offset_ + begin_; }

    void SetBegin(int begin) { begin_ = begin; }

    int GetEndPosition() const { return offset_ + begin_ + length_; }

    int GetLength() const { return length_; }

    void SetLength(int length) {
        if (length < 0) {
            throw std::invalid_argument("length_ < 0");
        }
        length_ = length;
    }

    std::wstring GetLexemeText() const { return lexeme_text_; }

    void SetLexemeText(const std::wstring &lexeme_text) {
        if (lexeme_text_.empty()) {
            lexeme_text_ = L"";
            length_ = 0;
        } else {
            lexeme_text_ = lexeme_text;
            length_ = lexeme_text_.length();
        }
    }

    int GetLexemeType() const { return lexeme_type_; }

    std::string GetLexemeTypeString() const {
        switch (lexeme_type_) {
            case TYPE_ENGLISH:
                return "ENGLISH";
            case TYPE_ARABIC:
                return "ARABIC";
            case TYPE_LETTER:
                return "LETTER";
            case TYPE_CNWORD:
                return "CN_WORD";
            case TYPE_CNCHAR:
                return "CN_CHAR";
            case TYPE_OTHER_CJK:
                return "OTHER_CJK";
            case TYPE_COUNT:
                return "COUNT";
            case TYPE_CNUM:
                return "TYPE_CNUM";
            case TYPE_CQUAN:
                return "TYPE_CQUAN";
            default:
                return "UNKNOWN";
        }
    }

    void SetLexemeType(int lexeme_type) { lexeme_type_ = lexeme_type; }

    bool Append(const Lexeme &l, int lexeme_type);

    std::string ToString() const;
};
} // namespace infinity
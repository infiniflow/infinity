module;

module infinity_core:lexeme.impl;

import :lexeme;

import std;

namespace infinity {

Lexeme::Lexeme(int offset, int begin, int length, int lexeme_type) {
    offset_ = offset;
    begin_ = begin;
    length_ = length;
    lexeme_type_ = lexeme_type;
}

Lexeme *Lexeme::Copy() {
    Lexeme *copy = new Lexeme(offset_, begin_, length_, lexeme_type_);
    return copy;
}

bool Lexeme::Append(const Lexeme &l, int lexeme_type) {
    if (!l.lexeme_text_.empty() && GetEndPosition() == l.GetBeginPosition()) {
        length_ += l.length_;
        lexeme_type_ = lexeme_type;
        return true;
    } else {
        return false;
    }
}

std::string Lexeme::ToString() const {
    std::ostringstream strbuf;
    strbuf << GetBeginPosition() << "-" << GetEndPosition();
    // strbuf << " : " << lexeme_text_ << " : \t";
    strbuf << GetLexemeTypeString();
    return strbuf.str();
}

} // namespace infinity
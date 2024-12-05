module;

#include <sstream>
#include <stdexcept>

module lexeme_path;

import lexeme;
import quick_sort_set;

namespace infinity {

LexemePath::LexemePath() {
    path_begin_ = -1;
    path_end_ = -1;
    payload_length_ = 0;
}

bool LexemePath::AddCrossLexeme(Lexeme *lexeme) {
    if (IsEmpty()) {
        AddLexeme(lexeme);
        path_begin_ = lexeme->GetBegin();
        path_end_ = lexeme->GetBegin() + lexeme->GetLength();
        payload_length_ += lexeme->GetLength();
        return true;
    } else if (CheckCross(lexeme)) {
        AddLexeme(lexeme);
        if (lexeme->GetBegin() + lexeme->GetLength() > path_end_) {
            path_end_ = lexeme->GetBegin() + lexeme->GetLength();
        }
        payload_length_ = path_end_ - path_begin_;
        return true;
    } else {
        return false;
    }
}

bool LexemePath::AddNotCrossLexeme(Lexeme *lexeme) {
    if (IsEmpty()) {
        AddLexeme(lexeme);
        path_begin_ = lexeme->GetBegin();
        path_end_ = lexeme->GetBegin() + lexeme->GetLength();
        payload_length_ += lexeme->GetLength();
        return true;
    } else if (CheckCross(lexeme)) {
        return false;
    } else {
        AddLexeme(lexeme);
        payload_length_ += lexeme->GetLength();
        Lexeme *head = PeekFirst();
        path_begin_ = head->GetBegin();
        Lexeme *tail = PeekLast();
        path_end_ = tail->GetBegin() + tail->GetLength();
        return true;
    }
}

Lexeme *LexemePath::RemoveTail() {
    Lexeme *tail = PollLast();
    if (IsEmpty()) {
        path_begin_ = -1;
        path_end_ = -1;
        payload_length_ = 0;
    } else {
        payload_length_ -= tail->GetLength();
        Lexeme *new_tail = PeekLast();
        path_end_ = new_tail->GetBegin() + new_tail->GetLength();
    }
    return tail;
}

bool LexemePath::CheckCross(Lexeme *lexeme) {
    return (lexeme->GetBegin() >= path_begin_ && lexeme->GetBegin() < path_end_) ||
           (path_begin_ >= lexeme->GetBegin() && path_begin_ < lexeme->GetBegin() + lexeme->GetLength());
}

int LexemePath::GetXWeight() const {
    int product = 1;
    Cell *c = GetHead();
    while (c != nullptr && c->GetLexeme() != nullptr) {
        product *= c->GetLexeme()->GetLength();
        c = c->GetNext();
    }
    return product;
}

int LexemePath::GetPWeight() const {
    int p_weight = 0;
    int p = 0;
    Cell *c = GetHead();
    while (c != nullptr && c->GetLexeme() != nullptr) {
        p++;
        p_weight += p * c->GetLexeme()->GetLength();
        c = c->GetNext();
    }
    return p_weight;
}

LexemePath *LexemePath::Copy() const {
    LexemePath *the_copy = new LexemePath();
    the_copy->path_begin_ = path_begin_;
    the_copy->path_end_ = path_end_;
    the_copy->payload_length_ = payload_length_;
    Cell *c = GetHead();
    while (c != nullptr && c->GetLexeme() != nullptr) {
        Lexeme *lexeme = c->GetLexeme()->Copy();
        if (!(the_copy->AddLexeme(lexeme)))
            delete lexeme;
        c = c->GetNext();
    }
    return the_copy;
}

int LexemePath::CompareTo(const LexemePath &o) const {
    if (payload_length_ > o.payload_length_) {
        return -1;
    } else if (payload_length_ < o.payload_length_) {
        return 1;
    } else {
        if (Size() < o.Size()) {
            return -1;
        } else if (Size() > o.Size()) {
            return 1;
        } else {
            if (GetPathLength() > o.GetPathLength()) {
                return -1;
            } else if (GetPathLength() < o.GetPathLength()) {
                return 1;
            } else {
                if (path_end_ > o.path_end_) {
                    return -1;
                } else if (path_end_ < o.path_end_) {
                    return 1;
                } else {
                    if (GetXWeight() > o.GetXWeight()) {
                        return -1;
                    } else if (GetXWeight() < o.GetXWeight()) {
                        return 1;
                    } else {
                        if (GetPWeight() > o.GetPWeight()) {
                            return -1;
                        } else if (GetPWeight() < o.GetPWeight()) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

std::string LexemePath::ToString() const {
    std::ostringstream sb;
    sb << "path_begin_  : " << path_begin_ << "\r\n";
    sb << "path_end_  : " << path_end_ << "\r\n";
    sb << "payload_length_  : " << payload_length_ << "\r\n";
    Cell *head = GetHead();
    while (head != nullptr) {
        sb << "lexeme : " << head->GetLexeme()->ToString() << "\r\n";
        head = head->GetNext();
    }
    return sb.str();
}

} // namespace infinity
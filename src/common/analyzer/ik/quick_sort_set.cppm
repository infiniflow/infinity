module;

#include <stdexcept>

export module infinity_core:quick_sort_set;

import :lexeme;

namespace infinity {

export class QuickSortSet {
public:
    class Cell {
    public:
        Cell *prev_{nullptr};
        Cell *next_{nullptr};
        Lexeme *lexeme_;

        Cell(Lexeme *lexeme) {
            if (lexeme == nullptr) {
                throw std::invalid_argument("lexeme must not be null");
            }
            lexeme_ = lexeme;
        }

        int CompareTo(Cell *other) { return lexeme_->CompareTo(*(other->lexeme_)); }

        Cell *GetPrev() { return prev_; }

        Cell *GetNext() { return next_; }

        Lexeme *GetLexeme() { return lexeme_; }
    };

    Cell *head_{nullptr};
    Cell *tail_{nullptr};
    int size_{0};

    QuickSortSet();

    virtual ~QuickSortSet();

    bool AddLexeme(Lexeme *lexeme);

    Lexeme *PeekFirst() {
        if (head_ != nullptr) {
            return head_->lexeme_;
        }
        return nullptr;
    }

    Lexeme *PollFirst() {
        if (size_ == 1) {
            Lexeme *first = head_->lexeme_;
            delete head_;
            head_ = nullptr;
            tail_ = nullptr;
            size_--;
            return first;
        } else if (size_ > 1) {
            Lexeme *first = head_->lexeme_;
            Cell *next_ = head_->next_;
            delete head_;
            head_ = next_;
            size_--;
            return first;
        } else {
            return nullptr;
        }
    }

    Lexeme *PeekLast() {
        if (tail_ != nullptr) {
            return tail_->lexeme_;
        }
        return nullptr;
    }

    Lexeme *PollLast() {
        if (size_ == 1) {
            Lexeme *last = head_->lexeme_;
            delete head_;
            head_ = nullptr;
            tail_ = nullptr;
            size_--;
            return last;
        } else if (size_ > 1) {
            Lexeme *last = tail_->lexeme_;
            Cell *prev_ = tail_->prev_;
            delete tail_;
            tail_ = prev_;
            size_--;
            return last;
        } else {
            return nullptr;
        }
    }

    int Size() const { return size_; }

    bool IsEmpty() const { return size_ == 0; }

    Cell *GetHead() const { return head_; }
};
} // namespace infinity
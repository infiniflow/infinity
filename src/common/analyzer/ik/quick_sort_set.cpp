module;

import lexeme;

module quick_sort_set;

namespace infinity {
QuickSortSet::QuickSortSet() {}

QuickSortSet::~QuickSortSet() {
    while (size_ > 0) {
        Lexeme *tail = PollLast();
        delete tail;
        size_--;
    }
}

bool QuickSortSet::AddLexeme(Lexeme *lexeme) {
    Cell *new_cell = new Cell(lexeme);
    if (size_ == 0) {
        head_ = new_cell;
        tail_ = new_cell;
        size_++;
        return true;
    } else {
        if (tail_->CompareTo(new_cell) == 0) {
            delete new_cell;
            return false;
        } else if (tail_->CompareTo(new_cell) < 0) {
            tail_->next_ = new_cell;
            new_cell->prev_ = tail_;
            tail_ = new_cell;
            size_++;
            return true;
        } else if (head_->CompareTo(new_cell) > 0) {
            head_->prev_ = new_cell;
            new_cell->next_ = head_;
            head_ = new_cell;
            size_++;
            return true;
        } else {
            Cell *index = tail_;
            while (index != nullptr && index->CompareTo(new_cell) > 0) {
                index = index->prev_;
            }
            if (index->CompareTo(new_cell) == 0) {
                delete new_cell;
                return false;
            } else if (index->CompareTo(new_cell) < 0) {
                new_cell->prev_ = index;
                new_cell->next_ = index->next_;
                index->next_->prev_ = new_cell;
                index->next_ = new_cell;
                size_++;
                return true;
            }
        }
    }
    delete new_cell;
    return false;
}
} // namespace infinity

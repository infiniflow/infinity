module infinity_core:quick_sort_set.impl;

import :lexeme;
import :quick_sort_set;

import std;

namespace infinity {

QuickSortSet::QuickSortSet() {}

QuickSortSet::~QuickSortSet() {
    while (size_ > 0) {
        Lexeme *tail = PollLast();
        delete tail;
    }
}

bool QuickSortSet::AddLexeme(Lexeme *lexeme) {
    std::unique_ptr<Cell> new_cell = std::make_unique<Cell>(lexeme);
    if (size_ == 0) {
        Cell *cell_ptr = new_cell.release();
        head_ = cell_ptr;
        tail_ = cell_ptr;
        size_++;
        return true;
    } else {
        if (tail_->CompareTo(new_cell.get()) == 0) {
            return false;
        } else if (tail_->CompareTo(new_cell.get()) < 0) {
            Cell *cell_ptr = new_cell.release();
            tail_->next_ = cell_ptr;
            cell_ptr->prev_ = tail_;
            tail_ = cell_ptr;
            size_++;
            return true;
        } else if (head_->CompareTo(new_cell.get()) > 0) {
            Cell *cell_ptr = new_cell.release();
            head_->prev_ = cell_ptr;
            cell_ptr->next_ = head_;
            head_ = cell_ptr;
            size_++;
            return true;
        } else {
            Cell *index = tail_;
            while (index != nullptr && index->CompareTo(new_cell.get()) > 0) {
                index = index->prev_;
            }
            if (index->CompareTo(new_cell.get()) == 0) {
                return false;
            } else if (index->CompareTo(new_cell.get()) < 0) {
                Cell *cell_ptr = new_cell.release();
                cell_ptr->prev_ = index;
                cell_ptr->next_ = index->next_;
                index->next_->prev_ = cell_ptr;
                index->next_ = cell_ptr;
                size_++;
                return true;
            }
        }
    }
    return false;
}
} // namespace infinity

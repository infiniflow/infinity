#pragma once

namespace infinity {
template<typename T, int I = 1>
struct IntrusiveListNode {
    IntrusiveListNode() {
        for (int i = 0; i < I; i++)
            previous_[i] = next_[i] = 0;
    }

    IntrusiveListNode(const IntrusiveListNode&) = delete;
    IntrusiveListNode& operator=(const IntrusiveListNode&) = delete;

    T *previous_[I];
    T *next_[I];
};

template<typename T, int I = 0>
struct IntrusiveList {
    IntrusiveList() {
        Clear();
    }

    T *Head() const {
        return head_;
    }

    T *Tail() const {
        return tail_;
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

    size_t Size() const {
        return size_;
    }

    void Put(T *t) {
        t->list_node_.previous_[I] = 0;
        t->list_node_.next_[I] = 0;
        if (head_) {
            t->list_node_.next_[I] = head_;
            head_->list_node_.previous_[I] = t;
        }
        head_ = t;
        if (!tail_)
            tail_ = t;
        size_++;
    }

    void Append(T *t) {
        t->list_node_.previous_[I] = 0;
        t->list_node_.next_[I] = 0;
        if (!head_) {
            assert(tail_ == 0);
            head_ = t;
            tail_ = t;
        } else {
            tail_->list_node_.next_[I] = t;
            tail_ = t;
            if (!head_)
                head_ = t;
        }
        size_++;
    }

    void Del(T *t) {
        assert(Has(t));

        if (t == tail_)
            tail_ = t->list_node_.previous_[I];
        if (t == head_) {
            T *next_ = head_->list_node_.next_[I];
            if (next_)
                next_->list_node_.previous_[I] = 0;
            head_ = next_;
        } else {
            T *next_ = t->list_node_.next_[I];
            T *prev = t->list_node_.previous_[I];
            if (prev)
                prev->list_node_.next_[I] = next_;
            if (next_)
                next_->list_node_.previous_[I] = prev;
        }
        t->list_node_.next_[I] = 0;
        t->list_node_.previous_[I] = 0;
        size_--;
    }

    bool Has(const T *t) const {
        return t->list_node_.previous_[I] != 0
               || t->list_node_.next_[I] != 0
               || t == head_;
    }

    void Clear() {
        head_ = 0;
        tail_ = 0;
        size_ = 0;
    }

    T *head_;
    T *tail_;
    size_t size_;
};
}
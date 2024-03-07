// Copyright (c) 2020 Dolev
// MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This MPSC queue comes from Dolev Adas
// https://arxiv.org/abs/2010.14189
// Jiffy: A Fast, Memory Efficient, Wait-Free Multi-Producers Single-Consumer Queue
//
#pragma once

#include <atomic>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <thread>

namespace infinity {

#define QUEUE_PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define NODE_SIZE 1620
static inline void *align_malloc(size_t align, size_t size) {
    void *ptr;
    int ret = posix_memalign(&ptr, align, size);
    if (ret != 0) {
        abort();
    }
    return ptr;
}

template <class T>
class MPSCQueue {
private:
    struct Node {
        Node() : data_(), is_set_(0) {}

        Node(T d) : data_(d), is_set_(0) {}

        Node(const Node &n) {
            data_ = n.data_;
            is_set_ = n.is_set_;
        }

        T data_;

        std::atomic<char> is_set_; // if the node and its data are valid.  0- empty  , 1- set , 2 - handled
    };

    typedef char cache_line_pad[64];

    struct BufferList {
        BufferList() : next_(NULL), prev_(NULL), head_(0), position_in_queue_(0) {}
        BufferList(unsigned int buffer_size_z) : next_(NULL), prev_(NULL), head_(0), position_in_queue_(1) {
            memset(curr_buffer_, 0, NODE_SIZE * sizeof(Node));
        }
        BufferList(unsigned int buffer_size_z, unsigned int position_in_queue, BufferList *prev)
            : next_(NULL), prev_(prev), head_(0), position_in_queue_(position_in_queue) {
            memset(curr_buffer_, 0, NODE_SIZE * sizeof(Node));
        }

        alignas(CACHE_LINE_SIZE) Node curr_buffer_[NODE_SIZE];

        std::atomic<BufferList *> next_ alignas(CACHE_LINE_SIZE);

        BufferList *prev_ alignas(CACHE_LINE_SIZE);

        unsigned int head_; // we have one thread that takes out elelemts and it is the only one that moves the head_

        unsigned int position_in_queue_; // start with 1
    };

    unsigned int buffer_size_;
    BufferList *head_of_queue_; // the first array that contains data for the thread that takes elements from the queue (for the single consumer)
    std::atomic<BufferList *>
        tail_of_queue_ alignas(CACHE_LINE_SIZE);               // the beginning of the last array which we insert elements into (for the producers)
    std::atomic<uint_fast64_t> tail_ alignas(CACHE_LINE_SIZE); // we need a global tail so the queue will be wait free - if not the queue is look free

public:
    MPSCQueue(unsigned int size) : buffer_size_(NODE_SIZE), tail_of_queue_(NULL), tail_(0) {
        void *buffer = align_malloc(QUEUE_PAGE_SIZE, sizeof(BufferList));
        head_of_queue_ = new (buffer) BufferList(buffer_size_);
        tail_of_queue_ = head_of_queue_;
    }
    MPSCQueue() : buffer_size_(NODE_SIZE), tail_of_queue_(NULL), tail_(0) {
        void *buffer = align_malloc(QUEUE_PAGE_SIZE, sizeof(BufferList));
        head_of_queue_ = new (buffer) BufferList(buffer_size_);
        tail_of_queue_ = head_of_queue_;
    }

    ~MPSCQueue() {

        while (head_of_queue_->next_.load(std::memory_order_acquire) != NULL) {
            BufferList *next = head_of_queue_->next_.load(std::memory_order_acquire);
            free(head_of_queue_);
            head_of_queue_ = next;
        }
        free(head_of_queue_);
    }

    // return false if the queue is empty
    bool dequeue(T &data) {
        while (true) {
            BufferList *temp_tail = tail_of_queue_.load(std::memory_order_seq_cst);
            unsigned int prev_size = buffer_size_ * (temp_tail->position_in_queue_ - 1);
            if ((head_of_queue_ == tail_of_queue_.load(std::memory_order_acquire)) &&
                (head_of_queue_->head_ == (tail_.load(std::memory_order_acquire) - prev_size))) { // the queue is empty
                // empty
                return false;
            } else if (head_of_queue_->head_ < buffer_size_) { // there is elements in the current array.
                Node *n = &(head_of_queue_->curr_buffer_[head_of_queue_->head_]);
                if (n->is_set_.load(std::memory_order_acquire) == 2) {
                    head_of_queue_->head_++;
                    continue;
                }

                BufferList *temphead_of_queue = head_of_queue_;
                unsigned int temphead = temphead_of_queue->head_;
                bool flag_move_to_new_buffer = false, flag_buffer_all_handeld = true;

                while (n->is_set_.load(std::memory_order_acquire) == 0) { // is not set yet - try to take out set elements that are next_ in line

                    if (temphead < buffer_size_) { // there is elements in the current array.
                        Node *tn = &(temphead_of_queue->curr_buffer_[temphead++]);
                        if (tn->is_set_.load(std::memory_order_acquire) == 1 &&
                            n->is_set_.load(std::memory_order_acquire) ==
                                0) { // the data is valid and the element in the head_ is still in insert process

                            //************* scan****************************
                            BufferList *scanhead_of_queue = head_of_queue_;
                            for (unsigned int scanhead = scanhead_of_queue->head_;
                                 (scanhead_of_queue != temphead_of_queue ||
                                  (scanhead < (temphead - 1) && n->is_set_.load(std::memory_order_acquire) == 0));
                                 scanhead++) {
                                if (scanhead >= buffer_size_) { // we reach the end of the buffer -move to the next_
                                    scanhead_of_queue = scanhead_of_queue->next_.load(std::memory_order_acquire);
                                    scanhead = scanhead_of_queue->head_;
                                    continue;
                                }
                                Node *scanN = &(scanhead_of_queue->curr_buffer_[scanhead]);
                                if (scanN->is_set_.load(std::memory_order_acquire) ==
                                    1) { // there is another element that is set - the scan start again until him
                                    // this is the new item to be evicted
                                    temphead = scanhead;
                                    temphead_of_queue = scanhead_of_queue;
                                    tn = scanN;

                                    scanhead_of_queue = head_of_queue_;
                                    scanhead = scanhead_of_queue->head_;
                                }
                            }

                            if (n->is_set_.load(std::memory_order_acquire) == 1) {
                                break;
                            }
                            //************* scan  end ****************************

                            data = tn->data_;
                            tn->is_set_.store(2, std::memory_order_release); // set taken
                            if (flag_move_to_new_buffer &&
                                (temphead - 1) == temphead_of_queue->head_) { // if we moved to a new buffer ,we need to move forward the head_ so in
                                                                              // the end we can delete the buffer
                                temphead_of_queue->head_++;
                            }

                            return true;

                        } // the data is valid and the element in the head_ is still in insert process

                        if (tn->is_set_.load(std::memory_order_acquire) == 0) {
                            flag_buffer_all_handeld = false;
                        }
                    }
                    if (temphead >= buffer_size_) { // we reach the end of the buffer -move to the next_
                        if (flag_buffer_all_handeld &&
                            flag_move_to_new_buffer) { // we want to "fold" the queue - if there is a thread that got stuck - we want to keep only
                                                       // that buffer and delete the rest( upfront from  it)
                            if (temphead_of_queue == tail_of_queue_.load(std::memory_order_acquire))
                                return false; // there is no place to move

                            BufferList *next_ = temphead_of_queue->next_.load(std::memory_order_acquire);
                            BufferList *prev_ = temphead_of_queue->prev_;
                            if (next_ == NULL)
                                return false; // if we do not have where to move

                            next_->prev_ = prev_;
                            prev_->next_.store(next_, std::memory_order_release);
                            free(temphead_of_queue);

                            temphead_of_queue = next_;
                            temphead = temphead_of_queue->head_;
                            flag_buffer_all_handeld = true;
                            flag_move_to_new_buffer = true;

                        } else {
                            BufferList *next_ = temphead_of_queue->next_.load(std::memory_order_acquire);
                            if (next_ == NULL)
                                return false; // if we do not have where to move
                            temphead_of_queue = next_;
                            temphead = temphead_of_queue->head_;
                            flag_move_to_new_buffer = true;
                            flag_buffer_all_handeld = true;
                        }
                    }

                } // try to take out set elements that are next_ in line

                //---------------------------n->is_set.load() == State::empty-------------------------------------
                if (n->is_set_.load(std::memory_order_acquire) == 1) { // valid
                    head_of_queue_->head_++;
                    data = n->data_;
                    // n->is_set_.store(2, std::memory_order_seq_cst); //set taken
                    return true;
                }
            }
            if (head_of_queue_->head_ >= buffer_size_) { // move to the next_ array and delete the prev_ array
                if (head_of_queue_ == tail_of_queue_.load(std::memory_order_acquire))
                    return false; // there is no place ro move
                BufferList *next_ = head_of_queue_->next_.load(std::memory_order_acquire);
                if (next_ == NULL)
                    return false; // if we do not have where to move
                free(head_of_queue_);
                head_of_queue_ = next_;
            }
        }
    }

    void enqueue(T const &data) {

        BufferList *temp_tail;
        unsigned int location = tail_.fetch_add(1, std::memory_order_seq_cst);
        bool go_back = false;
        while (true) {

            temp_tail = tail_of_queue_.load(std::memory_order_acquire); // points to the last buffer in queue

            unsigned int prev_size =
                buffer_size_ * (temp_tail->position_in_queue_ - 1); // the amount of items in the queue without the current buffer

            while (location < prev_size) { // the location is back in the queue - we need to go back in the queue to the right buffer
                go_back = true;
                temp_tail = temp_tail->prev_;
                prev_size -= buffer_size_;
            }

            unsigned int global_size = buffer_size_ + prev_size; // the amount of items in the queue

            if (prev_size <= location && location < global_size) { // we are in the right buffer
                int index = location - prev_size;

                Node *n = &(temp_tail->curr_buffer_[index]);
                n->data_ = data;
                n->is_set_.store(1, std::memory_order_relaxed); // need this to signal the thread that the data is ready
                if (index == 1 && !go_back) {                   // allocating a new buffer and adding it to the queue
                    void *buffer = align_malloc(QUEUE_PAGE_SIZE, sizeof(BufferList));
                    BufferList *newArr = new (buffer) BufferList(buffer_size_, temp_tail->position_in_queue_ + 1, temp_tail);
                    BufferList *Nullptr = NULL;

                    if (!(temp_tail->next_).compare_exchange_strong(Nullptr, newArr)) {
                        free(newArr);
                    }
                }

                return;
            }

            if (location >= global_size) { // the location we got is in the next_ buffer
                BufferList *next_ = (temp_tail->next_).load(std::memory_order_acquire);
                if (next_ == NULL) { // we do not have a next_ buffer - so we can try to allocate a new one

                    void *buffer = align_malloc(QUEUE_PAGE_SIZE, sizeof(BufferList));
                    BufferList *newArr = new (buffer) BufferList(buffer_size_, temp_tail->position_in_queue_ + 1, temp_tail);
                    BufferList *Nullptr = NULL;

                    if ((temp_tail->next_)
                            .compare_exchange_strong(Nullptr,
                                                     newArr)) { //	if (CAS(tail_of_queue_->next_, NULL, newArr)) 1 next_-> new array
                        tail_of_queue_.store(newArr, std::memory_order_release);
                    } else {
                        free(newArr);
                        // if several threads try simultaneously and the cas fail one succeed - delete the rest
                    }

                } else {
                    tail_of_queue_.compare_exchange_strong(temp_tail,
                                                           next_); // if it is not null move to the next_ buffer;
                }
            }

        } // while
    }
};

} // namespace infinity
#pragma once

#include <array>
#include <atomic>
#include <cstdint>

namespace infinity {

// Multi producer, single consumer queue with unlimited slots.
// http://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue
// Todo, memory allocation should be managed
template <typename T>
class MPSCQueue {
private:
    template <typename E>
    struct Node {
        explicit Node(E* element = nullptr) : element(element) {}

        E* element;
        std::atomic<Node<E>*> next = ATOMIC_VAR_INIT(nullptr);
    };

public:
    MPSCQueue() {
        tail_ = new Node<T>();
        head_.store(tail_);
    }

    ~MPSCQueue() {
        while (auto element = pop_front()) {
            delete element;
        }
        delete tail_;
    }

    void push_back(T* element) {
        auto new_node = new Node<T>(element);

        auto old_head = head_.exchange(new_node, std::memory_order_acq_rel);

        old_head->next.store(new_node, std::memory_order_release);
    }

    T* pop_front() {
        auto current_tail = tail_;
        auto next_tail = current_tail->next.load(std::memory_order_acquire);

        if (next_tail == nullptr) {
            if (head_.load(std::memory_order_relaxed) == tail_) {
                return nullptr;
            }
            do {
                next_tail = current_tail->next.load(std::memory_order_acquire);
            } while (next_tail == nullptr);
        }

        current_tail->next.store(nullptr, std::memory_order_release);

        auto element = next_tail->element;
        next_tail->element = nullptr;

        tail_ = next_tail;
        delete current_tail;

        return element;
    }

    bool empty() {
        return tail_->next.load(std::memory_order_relaxed) == nullptr &&
               head_.load(std::memory_order_relaxed) == tail_;
    }

private:
    alignas(64) std::atomic<Node<T>*> head_;

    alignas(64) Node<T>* tail_;
};
}
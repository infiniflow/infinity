// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <cassert>
#include <iostream>

export module skiplist;
import stl;
import memory_pool;

namespace infinity {

const int MAX_LEVEL = 16;

template <typename KeyType, typename ValueType>
struct Node {
    KeyType key_;
    ValueType value_;
    Vector<Atomic<Node<KeyType, ValueType> *>> next_;

    Node(const KeyType &k, const ValueType &v, int level_) : key_(k), value_(v), next_(level_) {}
};

export template <typename KeyType, typename ValueType, class Comparator>
class SkipList {
public:
    SkipList(Comparator cmp, MemoryPool *arena) : level_(1), size_(0), arena_(arena), compare_(cmp) {
        head_ = (new Node<KeyType, ValueType>(KeyType(), ValueType(), MAX_LEVEL));
    }

    ~SkipList() {
        Clear();
        delete head_.load();
    }

    class Iterator {
    public:
        Iterator(Node<KeyType, ValueType> *node) : current_(node) {}

        Pair<KeyType, ValueType> operator*() const { return MakePair(current_->key_, current_->value_); }

        const KeyType &Key() { return current_->key_; }

        const ValueType &Value() { return current_->value_; }

        bool operator!=(const Iterator &other) const { return current_ != other.current_; }

        Iterator &operator++() {
            if (current_ != nullptr) {
                current_ = current_->next_[0].load();
            }
            return *this;
        }

        bool Valid() { return current_ != nullptr; }

    private:
        Node<KeyType, ValueType> *current_;
    };

    Iterator Begin() { return Iterator(head_.load()->next_[0].load()); }

    Iterator Begin(const KeyType &key) {
        Node<KeyType, ValueType> *node = FindGreaterOrEqual(key);
        return Iterator(node);
    }

    Iterator End() { return Iterator(nullptr); }

    bool Insert(const KeyType &key, const ValueType &value) {
        Vector<Node<KeyType, ValueType> *> previous(MAX_LEVEL, nullptr);
        Node<KeyType, ValueType> *current = head_.load();

        for (int i = level_ - 1; i >= 0; --i) {
            while (true) {
                Node<KeyType, ValueType> *next_node = current->next_[i].load();
                if (next_node != nullptr && next_node->key_ < key) {
                    current = next_node;
                } else {
                    break;
                }
            }

            previous[i] = current;
        }

        Node<KeyType, ValueType> *next_node = current->next_[0].load();

        if (next_node != nullptr && next_node->key_ == key) {
            next_node->value_ = value; // Update value_ if key_ already exists
            return false;
        }

        int new_level = RandomLevel();

        if (new_level > level_) {
            for (int i = level_; i < new_level; ++i) {
                previous[i] = head_.load();
            }
            level_ = new_level;
        }

        Node<KeyType, ValueType> *new_node = new Node<KeyType, ValueType>(key, value, new_level);
        for (int i = 0; i < new_level; ++i) {
            new_node->next_[i] = previous[i]->next_[i].load();
            previous[i]->next_[i] = new_node;
        }

        return true;
    }

    bool Search(const KeyType &key, ValueType &value) {
        Node<KeyType, ValueType> *current = head_.load();

        for (int i = level_ - 1; i >= 0; --i) {
            while (true) {
                Node<KeyType, ValueType> *next_node = current->next_[i].load();
                if (next_node != nullptr && next_node->key_ < key) {
                    current = next_node;
                } else {
                    break;
                }
            }
        }

        Node<KeyType, ValueType> *next_node = current->next_[0].load();

        if (next_node != nullptr && next_node->key_ == key) {
            value = next_node->value_;
            return true;
        }

        return false;
    }

    Iterator Find(const KeyType &key) {
        Node<KeyType, ValueType> *x = FindGreaterOrEqual(key);
        if (x != nullptr && Equal(key, x->key_)) {
            return Iterator(x);
        }
        return Iterator(nullptr);
    }

    Node<KeyType, ValueType> *FindGreaterOrEqual(const KeyType &target) {
        Node<KeyType, ValueType> *x = head_.load();
        int level = level_ - 1;

        while (true) {
            Node<KeyType, ValueType> *next = x->next_[level].load();
            if (next == nullptr || next->key_ >= target) {
                if (level == 0) {
                    return next;
                } else {
                    // Move one level down
                    level--;
                }
            } else {
                x = next;
            }
        }
        return nullptr;
    }

    Node<KeyType, ValueType> *FindLessThan(const KeyType &target) {
        Node<KeyType, ValueType> *x = head_.load();
        int level = level_ - 1;

        while (true) {
            Node<KeyType, ValueType> *next = x->next_[level].load();
            if (next == nullptr || next->key_ >= target) {
                if (level == 0) {
                    return x; // Return the node before the node with key >= target
                } else {
                    // Move one level down
                    level--;
                }
            } else {
                x = next;
            }
        }
        return nullptr;
    }

    void Clear() {
        Node<KeyType, ValueType> *current = head_.load()->next_[0].load();
        while (current != nullptr) {
            Node<KeyType, ValueType> *next = current->next_[0].load();
            delete current;
            current = next;
        }
        for (int i = 0; i < level_; ++i) {
            head_.load()->next_[i] = nullptr;
        }
    }

private:
    bool Equal(const KeyType &a, const KeyType &b) const { return (compare_(a, b) == 0); }

    Atomic<int> level_;
    Atomic<int> size_;
    Atomic<Node<KeyType, ValueType> *> head_;
    MemoryPool *arena_{nullptr};
    Comparator const compare_;

    int RandomLevel() {
        int new_level = 1;
        while (static_cast<double>(rand() / RAND_MAX) < 0.5 && new_level < MAX_LEVEL) {
            new_level++;
        }
        return new_level;
    }
};
} // namespace infinity

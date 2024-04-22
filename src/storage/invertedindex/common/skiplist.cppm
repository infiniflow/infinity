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
#include <algorithm>
#include <atomic>
#include <random>
#include <cassert>
export module skiplist;

import stl;
import memory_pool;

namespace infinity {

constexpr int MAX_LEVEL = 20;
//const int INT_MAX = 2147483647;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, INT_MAX);

export template <typename KeyType, typename ValueType>
class Arena;
class Iterator;

u64 EncodeValue(u32 valOffset, u32 valSize) { return u64(valSize) << 32 | u64(valOffset); }
Pair<u32, u32> DecodeVal(u64 value) {
    auto valOffset = u32(value);
    auto valSize = u32(value >> 32);
    return {valOffset, valSize};
}

export class KeyComparator {
public:
    int operator()(const Pair<const char *, u16> &lhs, const Pair<const char *, u16> &rhs) const {
        return std::memcmp(lhs.first, rhs.first, std::min(lhs.second, rhs.second));
    }
};

export template <typename KeyType, typename ValueType>
class SkipListNode {
public:
    Pair<u32, u32> GetValueOffset() {
        u64 v = value_.load(std::memory_order_relaxed);
        return DecodeVal(v);
    }

    const ValueType GetValue(Arena<KeyType, ValueType> *arena) {
        auto [valOffset, valSize] = GetValueOffset();
        return arena->GetVal(valOffset, valSize);
    }

    void SetValue(u64 valueoffset) { std::atomic_store(&value_, valueoffset); }

    Pair<const char *, u16> Key(Arena<KeyType, ValueType> *arena) { return {arena->GetKey(key_offset_, key_size_), key_size_}; }

    u32 GetNextOffset(int h) { return next_[h].load(std::memory_order_relaxed); }

    bool CasNextOffset(int h, u32 old, u32 val) { return next_[h].compare_exchange_strong(old, val); }

public:
    Atomic<u64> value_;
    u32 key_offset_;
    u16 key_size_;
    Atomic<u16> height_;
    Atomic<u32> next_[MAX_LEVEL];
};

export template <typename KeyType, typename ValueType, typename Comparator = KeyComparator>
class SkipList {
    typedef SkipListNode<KeyType, ValueType> Node;

public:
    SkipList(Comparator cmp = KeyComparator(), MemoryPool *arena = nullptr, u32 sz = 1 << 30) : comparator_(cmp), height_(1), ref_(1), buf_size_(sz) {
        arena_ = new Arena<KeyType, ValueType>(buf_size_);
        auto head = NewNode(KeyType(), ValueType(), MAX_LEVEL);
        head_offset_ = arena_->GetNodeOffset(head);
    }

    ~SkipList() {
        arena_->Clear();
        delete arena_;
    }

    bool Insert(const KeyType &key, const ValueType &value) {
        i32 curh = GetHeight();
        u32 prev[MAX_LEVEL + 1] = {0};
        u32 next[MAX_LEVEL + 1] = {0};
        // update all level's prev and next
        prev[curh] = head_offset_;
        for (int i = int(curh) - 1; i >= 0; i--) {
            auto ret = FindSpliceForLevel(key, prev[i + 1], i);
            prev[i] = ret.first;
            next[i] = ret.second;
            if (prev[i] == next[i]) {

                auto valueoffset = arena_->AddVal(value);
                auto encVal = EncodeValue(valueoffset, GetTargetLen(value));
                auto preNode = arena_->GetNode(prev[i]);
                preNode->SetValue(encVal);
                return true;
            }
        }
        // create new node
        auto h = RandomLevel();
        auto x = NewNode(key, value, h);

        // try update skiplist height_ (CAS)
        curh = GetHeight();
        while (h > int(curh)) {
            if (height_.compare_exchange_strong(curh, h)) {
                break;
            }
            curh = GetHeight();
        }

        // insert and update skiplist
        for (int i = 0; i < h; i++) {
            while (1) {
                if (arena_->GetNode(prev[i]) == nullptr) {
                    assert(i > 1);
                    auto ret = FindSpliceForLevel(key, head_offset_, i);
                    prev[i] = ret.first;
                    next[i] = ret.second;
                    assert(prev[i] != next[i]);
                }
                x->next_[i] = next[i];
                auto pnode = arena_->GetNode(prev[i]);
                if (pnode->CasNextOffset(i, next[i], arena_->GetNodeOffset(x))) {
                    break;
                }

                // cas failed
                auto ret = FindSpliceForLevel(key, prev[i], i);
                prev[i] = ret.first;
                next[i] = ret.second;

                if (prev[i] == next[i]) {
                    assert(i == 0);
                    auto valueoffset = arena_->AddVal(value);
                    auto encVal = EncodeValue(valueoffset, GetTargetLen(value));
                    auto preNode = arena_->GetNode(prev[i]);
                    preNode->SetValue(encVal);
                    return true;
                }
            }
        }
        return true;
    }

    bool Search(const KeyType &key, ValueType &value) {
        auto [n, b] = FindNear(key, false, true);
        if (!n) {
            return false;
        }

        auto nextKey = arena_->GetKey(n->key_offset_, n->key_size_);
        const char* mk = nullptr;
        int l = 0;
        ParseKey(key,mk,l);
        if (0 != comparator_({mk,l}, {nextKey, n->key_size_})) {
            return false;
        }

        auto [valOffset, valSize] = n->GetValueOffset();
        value = arena_->GetVal(valOffset, valSize);
        return true;
    }

    void Clear() {}

    bool Empty() { return FindLast() == nullptr; }

    // skiplist iterator
    class Iterator {
    public:
        Iterator(SkipListNode<KeyType, ValueType> *node, SkipList<KeyType, ValueType, Comparator> *s) : s_(s), current_(node) {}

        Pair<KeyType, ValueType> operator*() { return {current_->Key(s_->arena_), current_->GetValue(s_->arena_)}; }

        const KeyType Key() { return current_->Key(s_->arena_); }

        const ValueType Value() { return current_->GetValue(s_->arena_); }

        bool operator!=(const Iterator &other) const { return current_ != other.current_; }

        Iterator &operator++() {
            if (current_ != nullptr) {
                current_ = s_->arena_->GetNode(current_->GetNextOffset(0));
            }
            return *this;
        }
        bool Valid() { return current_ != nullptr; }

    private:
        SkipList *s_;
        SkipListNode<KeyType, ValueType> *current_;
    };

    Iterator Begin() { return Iterator((GetNext(GetHead(), 0)), this); }

    Iterator Begin(const KeyType &key) {
        auto p = FindNear(key, false, true);
        if (!p.second)
            return End();
        return Iterator(p.first, this);
    }

    Iterator End() { return Iterator(nullptr, this); }

    Iterator Prev() {
        auto key = Key(arena_);
        return Iterator(FindNear(key, true, false).first, this);
    }

    Iterator Find(const KeyType &key) { return Iterator(FindNear(key, false, true).first, this); }

    Iterator FindForPrev(const KeyType &key) { return Iterator(FindNear(key, true, true).first, this); }

private:
    Node *NewNode(KeyType key, ValueType value, int h) {
        auto nodeOffset = arena_->AddNode(h);
        auto keyOffset = arena_->AddKey(key);
        auto val = EncodeValue(arena_->AddVal(value), GetTargetLen(value));
        Node *node = arena_->GetNode(nodeOffset);
        node->key_offset_ = keyOffset;

        if constexpr (std::is_same_v<ValueType, String>) {
            node->key_size_ = u16(key.size());
        } else {
            node->key_size_ = u16(sizeof(key));
        }
        node->height_ = u16(h);
        node->value_ = val;
        return node;
    }

    int RandomLevel() {
        int new_level = 1;
        while (dis(gen) < INT_MAX / 3 && new_level < MAX_LEVEL) {
            new_level++;
        }
        return new_level;
    }

    u64 GetTargetLen(const ValueType &val) {
        if constexpr (std::is_same_v<ValueType, String>) {
            return val.size();
        } else {
            return sizeof(val);
        }
    }

    void ParseKey(const KeyType &key, const char * &mk, int& l) {
        if constexpr (std::is_same_v<KeyType, String>) {
            mk = key.data();
            l = key.size();
        } else {
            mk = reinterpret_cast<const char *>(&key);
            l = sizeof(key);
        }
    }

    Node *GetNext(Node *node, int h) { return arena_->GetNode(node->GetNextOffset(h)); }

    Node *GetHead() { return arena_->GetNode(head_offset_); }

    i32 GetHeight() { return height_.load(std::memory_order_relaxed); }

    Pair<Node *, bool> FindNear(const KeyType &k, bool less, bool allowEqual) {
        auto x = GetHead();
        auto level = int(GetHeight() - 1);

        while (1) {
            auto next = GetNext(x, level);
            if (next == nullptr) {
                if (level > 0) {
                    level--;
                    continue;
                }

                if (!less) {
                    return {nullptr, false};
                }
                if (x == GetHead()) {
                    return {nullptr, false};
                }
                return {x, false};
            }

            auto nextkey = next->Key(arena_);
            const char* mk = nullptr;
            int l = 0;
            ParseKey(k,mk,l);
            int cmp = comparator_({mk,l}, nextkey);

            if (cmp > 0) {
                x = next;
                continue;
            } else if (cmp == 0) {
                if (allowEqual) {
                    return {next, true};
                }
                if (!less) {
                    return {GetNext(next, 0), false};
                }
                if (level > 0) {
                    level--;
                    continue;
                }

                if (x == GetHead())
                    return {nullptr, false};
                return {x, false};

            } else {
                // x.key < key
                if (level > 0) {
                    level--;
                    continue;
                }
                if (!less) {
                    return {next, false};
                }
                if (x == GetHead()) {
                    return {nullptr, false};
                }
                return {x, false};
            }
        }
    }

    Pair<u32, u32> FindSpliceForLevel(const KeyType &k, u32 before, int level) {
        while (1) {
            auto beforeNode = arena_->GetNode(before);
            auto nextOffset = beforeNode->GetNextOffset(level);
            auto nextNode = arena_->GetNode(nextOffset);

            if (!nextNode) {
                return {before, nextOffset};
            }

            auto nextKey = nextNode->Key(arena_);
            const char *mk = nullptr;
            int l = 0;
            if constexpr (std::is_same_v<KeyType, String>) {
                mk = k.data();
                l = k.size();
            } else {
                mk = reinterpret_cast<const char *>(&k);
                l = sizeof(k);
            }
            int cmp = comparator_({mk, l}, nextKey);

            if (cmp == 0) {
                return {nextOffset, nextOffset};
            } else if (cmp < 0) {
                return {before, nextOffset};
            }
            before = nextOffset;
        }
    }

    Node *FindLast() {
        auto n = GetHead();
        auto level = int(GetHeight()) - 1;
        while (1) {
            auto next = GetNext(n, level);
            if (next != nullptr) {
                n = next;
                continue;
            }
            if (level == 0) {
                if (n == GetHead()) {
                    return nullptr;
                }
                return n;
            }
            level--;
        }
    }

    void IncrRef() { ref_.fetch_add(1); }

    // TODO
    void DecRef() {
        ref_.fetch_sub(1);
        if (ref_ > 0)
            return;
    }

protected:
    Arena<KeyType, ValueType> *arena_;

private:
    Comparator const comparator_;

    // cur_height
    Atomic<i32> height_;
    Atomic<u32> head_offset_;
    Atomic<i32> ref_;

    u32 buf_size_;
};

static constexpr u32 OFFSET_SIZE = sizeof(u32(0));
static constexpr u32 NODE_ALIGN = sizeof(u64(0)) - 1;

export template <typename KeyType, typename ValueType>
class Arena {
    typedef SkipListNode<KeyType, ValueType> Node;
    static constexpr u32 MAX_NODE_SIZE = sizeof(Node);

public:
    explicit Arena(SizeT n) : n_(1) { buf_.resize(n); }

    u32 Allocate(u32 sz) {
        n_.fetch_add(sz);

        // TODO: not support reisze buf
        if (buf_.size() - n_ < MAX_NODE_SIZE) {
            u32 grow = buf_.size();
            if (grow > (1 << 30)) {
                grow = 1 << 30;
            } else if (grow < sz) {
                grow = sz;
            }
            buf_.resize(buf_.size() + grow);
        }
        return n_ - sz;
    }

    u32 AddNode(int height) {
        SizeT len = MAX_NODE_SIZE - ((MAX_LEVEL - height) * OFFSET_SIZE) + NODE_ALIGN;
        u32 n = Allocate(len);
        u32 alignN = (n + NODE_ALIGN) & ~(NODE_ALIGN);
        return alignN;
    }

    u32 AddVal(const ValueType &v) {
        u32 offset = 0;
        if constexpr (std::is_same_v<ValueType, String>) {
            auto l = v.size();
            offset = Allocate(l);
            std::memcpy(&buf_[offset], v.data(), l);
            // buf_[offset + l + 1] = '\0';
        } else {
            auto l = sizeof(v);
            offset = Allocate(l);
            std::memcpy(&buf_[offset], &v, l);
            // buf_[offset + l + 1] = '\0';
        }
        return offset;
    }

    u32 AddKey(const KeyType &key) {
        u32 offset = 0;
        if constexpr (std::is_same_v<KeyType, String>) {
            auto l = key.size();
            offset = Allocate(l + 1);
            std::memcpy(&buf_[offset], key.data(), l);
            // buf_[offset + key.size() + 1] = '\0';
        } else {
            auto l = sizeof(key);
            offset = Allocate(l + 1);
            std::memcpy(&buf_[offset], &key, l);
            // buf_[offset + l + 1] = '\0';
        }
        return offset;
    }

    Node *GetNode(u32 offset) {
        if (offset == 0) {
            return nullptr;
        }
        return reinterpret_cast<Node *>(&buf_[offset]);
    }

    u32 GetNodeOffset(Node *node) {
        if (node == nullptr) {
            return 0;
        }
        return static_cast<u32>(reinterpret_cast<uintptr_t>(node) - reinterpret_cast<uintptr_t>(&buf_[0]));
    }

    i64 ArenaSize() const { return static_cast<i64>(n_.load()); }

    const char *GetKey(u32 offset, u16 size) {
        if (offset + size > ArenaSize()) {
            return nullptr;
        }
        return reinterpret_cast<const char *>(&buf_[offset]);
    }

    ValueType GetVal(u32 offset, u32 size) {
        if (offset + size > ArenaSize()) {
            return ValueType{};
        }
        if constexpr (std::is_same_v<ValueType, String>) {
            return String(&buf_[offset], size);
        } else {
            ValueType val;
            std::memcpy(&val, &buf_[offset], size);
            return val;
        }
    }

    void Clear() {
        n_ = 0;
        Vector<char>().swap(buf_);
    }

    char Buf(int idx) {
        if (i64(idx) > buf_.size()) {
            return ' ';
        }
        return buf_[idx];
    }

private:
    Atomic<u32> n_;
    Vector<char> buf_;
};
} 
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

// Thread safety
// -------------
//
// Writes require external synchronization, most likely a mutex.
// Reads require a guarantee that the SkipList will not be destroyed
// while the read is in progress.  Apart from that, reads progress
// without any internal locking or synchronization.
//
// Invariants:
//
// (1) Allocated nodes are never deleted until the SkipList is
// destroyed.  This is trivially guaranteed by the code since we
// never delete any skip list nodes.
//
// (2) The contents of a Node except for the next/prev pointers are
// immutable after the Node has been linked into the SkipList.
// Only Insert() modifies the list, and it is careful to initialize
// a node and use release-stores to publish the nodes in one or
// more lists.
//
// ... prev vs. next pointer ordering ...
module;

#include <cassert>

export module skiplist;
import stl;
import memory_pool;

namespace infinity {

class Random {
private:
    u32 seed_;

public:
    explicit Random(u32 s) : seed_(s & 0x7fffffffu) {
        // Avoid bad seeds.
        if (seed_ == 0 || seed_ == 2147483647L) {
            seed_ = 1;
        }
    }
    u32 Next() {
        static const u32 M = 2147483647L; // 2^31-1
        static const u64 A = 16807;       // bits 14, 8, 7, 5, 2, 1, 0
        // We are computing
        //       seed_ = (seed_ * A) % M,    where M = 2^31-1
        //
        // seed_ must not be zero or M, or else all subsequent computed values
        // will be zero or M respectively.  For all other values, seed_ will end
        // up cycling through every number in [1,M-1]
        u64 product = seed_ * A;

        // Compute (product % M) using the fact that ((x << 31) % M) == x.
        seed_ = static_cast<u32>((product >> 31) + (product & M));
        // The first reduction may overflow by 1 bit, so we may need to
        // repeat.  mod == M is not possible; using > allows the faster
        // sign-bit-based test.
        if (seed_ > M) {
            seed_ -= M;
        }
        return seed_;
    }
    // Returns a uniformly distributed value in the range [0..n-1]
    // REQUIRES: n > 0
    u32 Uniform(int n) { return Next() % n; }

    // Randomly returns true ~"1/n" of the time, and false otherwise.
    // REQUIRES: n > 0
    bool OneIn(int n) { return (Next() % n) == 0; }

    // Skewed: pick "base" uniformly from range [0,max_log] and then
    // return "base" random bits.  The effect is to pick a number in the
    // range [0,2^max_log-1] with exponential bias towards smaller numbers.
    u32 Skewed(int max_log) { return Uniform(1 << Uniform(max_log + 1)); }
};

export template <typename Key, typename Value, class Comparator>
class SkipList {
private:
    struct Node;

public:
    // Create a new SkipList object that will use "cmp" for comparing keys,
    // and will allocate memory using "*arena".  Objects allocated in the arena
    // must remain allocated for the lifetime of the skiplist object.
    explicit SkipList(Comparator cmp, MemoryPool *arena);

    SkipList(const SkipList &) = delete;
    SkipList &operator=(const SkipList &) = delete;

    // Insert key into the list.
    // REQUIRES: nothing that compares equal to key is currently in the list.
    void Insert(const Key &key, const Value &);

    // Returns true iff an entry that compares equal to key is in the list.
    bool Contains(const Key &key) const;

    // Iteration over the contents of a skip list
    class Iterator {
    public:
        // Initialize an iterator over the specified list.
        // The returned iterator is not valid.
        explicit Iterator(const SkipList *list);

        // Returns true iff the iterator is positioned at a valid node.
        bool Valid() const;

        // Returns the key at the current position.
        // REQUIRES: Valid()
        const Key &key() const;

        const Value &value() const;

        // Advances to the next position.
        // REQUIRES: Valid()
        void Next();

        // Advances to the previous position.
        // REQUIRES: Valid()
        void Prev();

        // Advance to the first entry with a key >= target
        void Seek(const Key &target);

        // Position at the first entry in list.
        // Final state of iterator is Valid() iff list is not empty.
        void SeekToFirst();

        // Position at the last entry in list.
        // Final state of iterator is Valid() iff list is not empty.
        void SeekToLast();

    private:
        const SkipList *list_;
        Node *node_;
        // Intentionally copyable
    };

    bool Find(const Key &key, Value &value);

private:
    enum { kMaxHeight = 12 };

    inline int GetMaxHeight() const { return max_height_.load(std::memory_order_relaxed); }

    Node *NewNode(const Key &key, const Value &value, int height);
    int RandomHeight();
    bool Equal(const Key &a, const Key &b) const { return (compare_(a, b) == 0); }

    // Return true if key is greater than the data stored in "n"
    bool KeyIsAfterNode(const Key &key, Node *n) const;

    // Return the earliest node that comes at or after key.
    // Return nullptr if there is no such node.
    //
    // If prev is non-null, fills prev[level] with pointer to previous
    // node at "level" for every level in [0..max_height_-1].
    Node *FindGreaterOrEqual(const Key &key, Node **prev) const;

    // Return the latest node with a key < key.
    // Return head_ if there is no such node.
    Node *FindLessThan(const Key &key) const;

    // Return the last node in the list.
    // Return head_ if list is empty.
    Node *FindLast() const;

    // Immutable after construction
    Comparator const compare_;
    MemoryPool *const arena_; // MemoryPool used for allocations of nodes

    Node *const head_;

    // Modified only by Insert().  Read racily by readers, but stale
    // values are ok.
    Atomic<int> max_height_; // Height of the entire list

    // Read/written only by Insert().
    Random rnd_;
};

// Implementation details follow
template <typename Key, typename Value, class Comparator>
struct SkipList<Key, Value, Comparator>::Node {
    explicit Node(const Key &k, const Value &v) : key(k), value(v) {}

    Key const key;

    Value const value;

    // Accessors/mutators for links.  Wrapped in methods so we can
    // add the appropriate barriers as necessary.
    Node *Next(int n) {
        assert(n >= 0);
        // Use an 'acquire load' so that we observe a fully initialized
        // version of the returned Node.
        return next_[n].load(std::memory_order_acquire);
    }
    void SetNext(int n, Node *x) {
        assert(n >= 0);
        // Use a 'release store' so that anybody who reads through this
        // pointer observes a fully initialized version of the inserted node.
        next_[n].store(x, std::memory_order_release);
    }

    // No-barrier variants that can be safely used in a few locations.
    Node *NoBarrier_Next(int n) {
        assert(n >= 0);
        return next_[n].load(std::memory_order_relaxed);
    }
    void NoBarrier_SetNext(int n, Node *x) {
        assert(n >= 0);
        next_[n].store(x, std::memory_order_relaxed);
    }

private:
    // Array of length equal to the node height.  next_[0] is lowest level link.
    Atomic<Node *> next_[1];
};

template <typename Key, typename Value, class Comparator>
typename SkipList<Key, Value, Comparator>::Node *SkipList<Key, Value, Comparator>::NewNode(const Key &key, const Value &value, int height) {
    void *node_memory = arena_->Allocate(sizeof(Node) + sizeof(Atomic<Node *>) * (height - 1));
    return new (node_memory) Node(key, value);
}

template <typename Key, typename Value, class Comparator>
inline SkipList<Key, Value, Comparator>::Iterator::Iterator(const SkipList *list) {
    list_ = list;
    node_ = nullptr;
}

template <typename Key, typename Value, class Comparator>
inline bool SkipList<Key, Value, Comparator>::Iterator::Valid() const {
    return node_ != nullptr;
}

template <typename Key, typename Value, class Comparator>
inline const Key &SkipList<Key, Value, Comparator>::Iterator::key() const {
    assert(Valid());
    return node_->key;
}

template <typename Key, typename Value, class Comparator>
inline const Value &SkipList<Key, Value, Comparator>::Iterator::value() const {
    assert(Valid());
    return node_->value;
}

template <typename Key, typename Value, class Comparator>
inline void SkipList<Key, Value, Comparator>::Iterator::Next() {
    assert(Valid());
    node_ = node_->Next(0);
}

template <typename Key, typename Value, class Comparator>
inline void SkipList<Key, Value, Comparator>::Iterator::Prev() {
    // Instead of using explicit "prev" links, we just search for the
    // last node that falls before key.
    assert(Valid());
    node_ = list_->FindLessThan(node_->key);
    if (node_ == list_->head_) {
        node_ = nullptr;
    }
}

template <typename Key, typename Value, class Comparator>
inline void SkipList<Key, Value, Comparator>::Iterator::Seek(const Key &target) {
    node_ = list_->FindGreaterOrEqual(target, nullptr);
}

template <typename Key, typename Value, class Comparator>
inline void SkipList<Key, Value, Comparator>::Iterator::SeekToFirst() {
    node_ = list_->head_->Next(0);
}

template <typename Key, typename Value, class Comparator>
inline void SkipList<Key, Value, Comparator>::Iterator::SeekToLast() {
    node_ = list_->FindLast();
    if (node_ == list_->head_) {
        node_ = nullptr;
    }
}

template <typename Key, typename Value, class Comparator>
int SkipList<Key, Value, Comparator>::RandomHeight() {
    // Increase height with probability 1 in kBranching
    static const unsigned int kBranching = 4;
    int height = 1;
    while (height < kMaxHeight && rnd_.OneIn(kBranching)) {
        height++;
    }
    assert(height > 0);
    assert(height <= kMaxHeight);
    return height;
}

template <typename Key, typename Value, class Comparator>
bool SkipList<Key, Value, Comparator>::KeyIsAfterNode(const Key &key, Node *n) const {
    // null n is considered infinite
    return (n != nullptr) && (compare_(n->key, key) < 0);
}

template <typename Key, typename Value, class Comparator>
typename SkipList<Key, Value, Comparator>::Node *SkipList<Key, Value, Comparator>::FindGreaterOrEqual(const Key &key, Node **prev) const {
    Node *x = head_;
    int level = GetMaxHeight() - 1;
    while (true) {
        Node *next = x->Next(level);
        if (KeyIsAfterNode(key, next)) {
            // Keep searching in this list
            x = next;
        } else {
            if (prev != nullptr)
                prev[level] = x;
            if (level == 0) {
                return next;
            } else {
                // Switch to next list
                level--;
            }
        }
    }
}

template <typename Key, typename Value, class Comparator>
typename SkipList<Key, Value, Comparator>::Node *SkipList<Key, Value, Comparator>::FindLessThan(const Key &key) const {
    Node *x = head_;
    int level = GetMaxHeight() - 1;
    while (true) {
        assert(x == head_ || compare_(x->key, key) < 0);
        Node *next = x->Next(level);
        if (next == nullptr || compare_(next->key, key) >= 0) {
            if (level == 0) {
                return x;
            } else {
                // Switch to next list
                level--;
            }
        } else {
            x = next;
        }
    }
}

template <typename Key, typename Value, class Comparator>
typename SkipList<Key, Value, Comparator>::Node *SkipList<Key, Value, Comparator>::FindLast() const {
    Node *x = head_;
    int level = GetMaxHeight() - 1;
    while (true) {
        Node *next = x->Next(level);
        if (next == nullptr) {
            if (level == 0) {
                return x;
            } else {
                // Switch to next list
                level--;
            }
        } else {
            x = next;
        }
    }
}

template <typename Key, typename Value, class Comparator>
SkipList<Key, Value, Comparator>::SkipList(Comparator cmp, MemoryPool *arena)
    : compare_(cmp), arena_(arena), head_(NewNode(0, 0, kMaxHeight)), max_height_(1), rnd_(0xdeadbeef) {
    for (int i = 0; i < kMaxHeight; i++) {
        head_->SetNext(i, nullptr);
    }
}

template <typename Key, typename Value, class Comparator>
void SkipList<Key, Value, Comparator>::Insert(const Key &key, const Value &value) {
    // TODO(opt): We can use a barrier-free variant of FindGreaterOrEqual()
    // here since Insert() is externally synchronized.
    Node *prev[kMaxHeight];
    Node *x = FindGreaterOrEqual(key, prev);

    // Our data structure does not allow duplicate insertion
    assert(x == nullptr || !Equal(key, x->key));

    int height = RandomHeight();
    if (height > GetMaxHeight()) {
        for (int i = GetMaxHeight(); i < height; i++) {
            prev[i] = head_;
        }
        // It is ok to mutate max_height_ without any synchronization
        // with concurrent readers.  A concurrent reader that observes
        // the new value of max_height_ will see either the old value of
        // new level pointers from head_ (nullptr), or a new value set in
        // the loop below.  In the former case the reader will
        // immediately drop to the next level since nullptr sorts after all
        // keys.  In the latter case the reader will use the new node.
        max_height_.store(height, std::memory_order_relaxed);
    }

    x = NewNode(key, value, height);
    for (int i = 0; i < height; i++) {
        // NoBarrier_SetNext() suffices since we will add a barrier when
        // we publish a pointer to "x" in prev[i].
        x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
        prev[i]->SetNext(i, x);
    }
}

template <typename Key, typename Value, class Comparator>
bool SkipList<Key, Value, Comparator>::Contains(const Key &key) const {
    Node *x = FindGreaterOrEqual(key, nullptr);
    if (x != nullptr && Equal(key, x->key)) {
        return true;
    } else {
        return false;
    }
}

template <typename Key, typename Value, class Comparator>
bool SkipList<Key, Value, Comparator>::Find(const Key &key, Value &value) {
    Node *x = FindGreaterOrEqual(key, nullptr);
    if (x != nullptr && SkipList<Key, Value, Comparator>::Equal(key, x->key)) {
        value = x->value;
        return true;
    } else {
        return false;
    }
}

} // namespace infinity

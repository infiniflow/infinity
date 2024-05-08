// This file is part of PGM-index <https://github.com/gvinciguerra/PGM-index>.
// Copyright (c) 2019 Giorgio Vinciguerra.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "pgm_index.hpp"
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace pgm {

/**
 * A sorted associative container that contains key-value pairs with unique keys.
 * @tparam K the type of a key
 * @tparam V the type of a value
 * @tparam PGMType the type of @ref PGMIndex to use in the container
 */
template<typename K, typename V, typename PGMType = PGMIndex<K, 16>>
class DynamicPGMIndex {
    class ItemA;
    class ItemB;
    class Iterator;

    using Item = std::conditional_t<std::is_pointer_v<V> || std::is_arithmetic_v<V>, ItemA, ItemB>;
    using Level = std::vector<Item>;

    const uint8_t base;            ///< base^i is the maximum size of the ith level.
    const uint8_t min_level;       ///< Levels 0..min_level are combined into one large level.
    const uint8_t min_index_level; ///< Minimum level on which an index is constructed.
    size_t buffer_max_size;        ///< Size of the combined upper levels, i.e. max_size(0) + ... + max_size(min_level).
    uint8_t used_levels;           ///< Equal to 1 + last level whose size is greater than 0, or = min_level if no data.
    std::vector<Level> levels;     ///< (i-min_level)th element is the data array at the ith level.
    std::vector<PGMType> pgms;     ///< (i-min_index_level)th element is the index at the ith level.

    const Level &level(uint8_t level) const { return levels[level - min_level]; }
    const PGMType &pgm(uint8_t level) const { return pgms[level - min_index_level]; }
    Level &level(uint8_t level) { return levels[level - min_level]; }
    PGMType &pgm(uint8_t level) { return pgms[level - min_index_level]; }
    bool has_pgm(uint8_t level) const { return level >= min_index_level; }
    size_t max_size(uint8_t level) const { return size_t(1) << (level * ceil_log2(base)); }
    uint8_t max_fully_allocated_level() const { return min_level + 2; }
    uint8_t ceil_log_base(size_t n) const { return (ceil_log2(n) + ceil_log2(base) - 1) / ceil_log2(base); }
    constexpr static uint8_t ceil_log2(size_t n) { return n <= 1 ? 0 : sizeof(long long) * 8 - __builtin_clzll(n - 1); }

    void pairwise_merge(const Item &new_item,
                        uint8_t target,
                        size_t size_hint,
                        typename Level::iterator insertion_point) {
        Level tmp_a(size_hint + level(target).size());
        Level tmp_b(size_hint + level(target).size());

        // Insert new_item in sorted order in the first level
        auto alternate = true;
        auto it = std::move(level(min_level).begin(), insertion_point, tmp_a.begin());
        *it++ = new_item;
        it = std::move(insertion_point, level(min_level).end(), it);
        auto tmp_size = std::distance(tmp_a.begin(), it);

        // Merge subsequent levels
        uint8_t merge_limit = level(target).empty() ? target - 1 : target;
        for (uint8_t i = 1 + min_level; i <= merge_limit; ++i, alternate = !alternate) {
            auto tmp_begin = (alternate ? tmp_a : tmp_b).begin();
            auto tmp_end = tmp_begin + tmp_size;
            auto out_begin = (alternate ? tmp_b : tmp_a).begin();
            decltype(out_begin) out_end;

            auto can_delete_permanently = i == used_levels - 1;
            if (can_delete_permanently)
                out_end = merge<true, true>(tmp_begin, tmp_end, level(i).begin(), level(i).end(), out_begin);
            else
                out_end = merge<false, true>(tmp_begin, tmp_end, level(i).begin(), level(i).end(), out_begin);
            tmp_size = std::distance(out_begin, out_end);

            // Empty this level and the corresponding index
            level(i).clear();
            if (i >= max_fully_allocated_level())
                level(i).shrink_to_fit();
            if (has_pgm(i))
                pgm(i) = PGMType();
        }

        level(min_level).clear();
        level(target) = std::move(alternate ? tmp_a : tmp_b);
        level(target).resize(tmp_size);

        // Rebuild index, if needed
        if (has_pgm(target))
            pgm(target) = PGMType(level(target).begin(), level(target).end());
    }

    void insert(const Item &new_item) {
        auto insertion_point = lower_bound_bl(level(min_level).begin(), level(min_level).end(), new_item);
        if (insertion_point != level(min_level).end() && *insertion_point == new_item) {
            *insertion_point = new_item;
            return;
        }

        if (level(min_level).size() < buffer_max_size) {
            level(min_level).insert(insertion_point, new_item);
            used_levels = used_levels == min_level ? min_level + 1 : used_levels;
            return;
        }

        size_t slots_required = buffer_max_size + 1;
        uint8_t i;
        for (i = min_level + 1; i < used_levels; ++i) {
            auto slots_left_in_level = max_size(i) - level(i).size();
            if (slots_required <= slots_left_in_level)
                break;
            slots_required += level(i).size();
        }

        auto need_new_level = i == used_levels;
        if (need_new_level) {
            ++used_levels;
            levels.emplace_back();
            if (i - min_index_level >= int(pgms.size()))
                pgms.emplace_back();
        }

        pairwise_merge(new_item, i, slots_required, insertion_point);
    }

public:

    using key_type = K;
    using mapped_type = V;
    using value_type = Item;
    using size_type = size_t;
    using iterator = Iterator;

    /**
     * Constructs an empty container.
     * @param base determines the size of the ith level as base^i
     * @param buffer_level determines the size of level 0, equal to the sum of base^i for i = 0, ..., buffer_level
     * @param index_level the minimum level at which an index is constructed to speed up searches
     */
    DynamicPGMIndex(uint8_t base = 8, uint8_t buffer_level = 0, uint8_t index_level = 0)
        : base(base),
          min_level(buffer_level ? buffer_level : ceil_log_base(128) - (base == 2)),
          min_index_level(std::max<size_t>(min_level + 1, index_level ? index_level : ceil_log_base(size_t(1) << 24))),
          buffer_max_size(),
          used_levels(min_level),
          levels(),
          pgms() {
        if (base < 2 || (base & (base - 1u)) != 0)
            throw std::invalid_argument("base must be a power of two");

        for (auto j = 0; j <= min_level; ++j)
            buffer_max_size += max_size(j);

        levels.resize(32 - used_levels);
        level(min_level).reserve(buffer_max_size);
        for (uint8_t i = min_level + 1; i < max_fully_allocated_level(); ++i)
            level(i).reserve(max_size(i));
    }

    /**
     * Constructs the container on the sorted data in the range [first, last).
     * @tparam Iterator
     * @param first, last the range containing the sorted elements to be indexed
     * @param base determines the size of the ith level as base^i
     * @param buffer_level determines the size of level 0, equal to the sum of base^i for i = 0, ..., buffer_level
     * @param index_level the minimum level at which an index is constructed to speed up searches
     */
    template<typename Iterator>
    DynamicPGMIndex(Iterator first, Iterator last, uint8_t base = 8, uint8_t buffer_level = 0, uint8_t index_level = 0)
        : DynamicPGMIndex(base, buffer_level, index_level) {
        size_t n = std::distance(first, last);
        used_levels = std::max<uint8_t>(ceil_log_base(n), min_level) + 1;
        levels.resize(std::max<uint8_t>(used_levels, 32) - min_level + 1);
        level(min_level).reserve(buffer_max_size);
        for (uint8_t i = min_level + 1; i < max_fully_allocated_level(); ++i)
            level(i).reserve(max_size(i));

        if (n == 0) {
            used_levels = min_level;
            return;
        }

        // Copy only the first of each group of pairs with same key value
        auto &target = level(used_levels - 1);
        target.resize(n);
        auto out = target.begin();
        *out++ = Item(first->first, first->second);
        while (++first != last) {
            if (first->first < std::prev(out)->first)
                throw std::invalid_argument("Range is not sorted");
            if (first->first != std::prev(out)->first)
                *out++ = Item(first->first, first->second);
        }
        target.resize(std::distance(target.begin(), out));

        if (has_pgm(used_levels - 1)) {
            pgms = decltype(pgms)(used_levels - min_index_level);
            pgm(used_levels - 1) = PGMType(target.begin(), target.end());
        }
    }

    /**
     * Inserts an element into the container if @p key does not exists in the container. If @p key already exists, the
     * corresponding value is updated with @p value.
     * @param key element key to insert or update
     * @param value element value to insert
     */
    void insert_or_assign(const K &key, const V &value) { insert(Item(key, value)); }

    /**
     * Removes the specified element from the container.
     * @param key key value of the element to remove
     */
    void erase(const K &key) { insert(Item(key)); }

    /**
     * Finds an element with key equivalent to @p key.
     * @param key key value of the element to search for
     * @return an iterator to an element with key equivalent to @p key. If no such element is found, end() is returned
     */
    iterator find(const K &key) const {
        for (auto i = min_level; i < used_levels; ++i) {
            if (level(i).empty())
                continue;

            auto first = level(i).begin();
            auto last = level(i).end();
            if (has_pgm(i)) {
                auto range = pgm(i).search(key);
                first = level(i).begin() + range.lo;
                last = level(i).begin() + range.hi;
            }

            auto it = lower_bound_bl(first, last, key);
            if (it != level(i).end() && it->first == key)
                return it->deleted() ? end() : iterator(this, i, it);
        }

        return end();
    }

    /**
     * Returns a copy of the elements with key between and including @p lo and @p hi.
     * @param lo lower endpoint of the range query
     * @param hi upper endpoint of the range query, must be greater than or equal to @p lo
     * @return a vector of key-value pairs satisfying the range query
     */
    std::vector<std::pair<K, V>> range(const K &lo, const K &hi) const {
        if (lo > hi)
            throw std::invalid_argument("lo > hi");

        Level tmp_a;
        Level tmp_b;
        auto alternate = true;

        for (auto i = min_level; i < used_levels; ++i) {
            if (level(i).empty())
                continue;

            auto lo_first = level(i).begin();
            auto lo_last = level(i).end();
            auto hi_first = level(i).begin();
            auto hi_last = level(i).end();
            if (has_pgm(i)) {
                auto range = pgm(i).search(lo);
                lo_first = level(i).begin() + range.lo;
                lo_last = level(i).begin() + range.hi;
                range = pgm(i).search(hi);
                hi_first = level(i).begin() + range.lo;
                hi_last = level(i).begin() + range.hi;
            }

            auto it_lo = lower_bound_bl(lo_first, lo_last, lo);
            auto it_hi = std::upper_bound(std::max(it_lo, hi_first), hi_last, hi);
            auto range_size = std::distance(it_lo, it_hi);
            if (range_size == 0)
                continue;

            auto tmp_size = (alternate ? tmp_a : tmp_b).size();
            (alternate ? tmp_b : tmp_a).resize(tmp_size + range_size);
            auto tmp_it = (alternate ? tmp_a : tmp_b).begin();
            auto out_it = (alternate ? tmp_b : tmp_a).begin();
            tmp_size = std::distance(out_it, merge<false, false>(tmp_it, tmp_it + tmp_size, it_lo, it_hi, out_it));
            (alternate ? tmp_b : tmp_a).resize(tmp_size);
            alternate = !alternate;
        }

        std::vector<std::pair<K, V>> result;
        result.reserve((alternate ? tmp_a : tmp_b).size());
        auto first = (alternate ? tmp_a : tmp_b).begin();
        auto last = (alternate ? tmp_a : tmp_b).end();
        for (auto it = first; it != last; ++it)
            if (!it->deleted())
                result.emplace_back(it->first, it->second);
        return result;
    }

    /**
     * Returns an iterator pointing to the first element that is not less than (i.e. greater or equal to) @p key.
     * @param key key value to compare the elements to
     * @return an iterator to an element with key not less than @p key. If no such element is found, end() is returned
     */
    iterator lower_bound(const K &key) const {
        typename Level::const_iterator lb;
        auto lb_set = false;
        uint8_t lb_level;
        std::set<K> deleted;

        for (auto i = min_level; i < used_levels; ++i) {
            if (level(i).empty())
                continue;

            auto first = level(i).begin();
            auto last = level(i).end();
            if (has_pgm(i)) {
                auto range = pgm(i).search(key);
                first = level(i).begin() + range.lo;
                last = level(i).begin() + range.hi;
            }

            for (auto it = lower_bound_bl(first, last, key);
                 it != level(i).end() && (!lb_set || it->first < lb->first); ++it) {
                if (it->deleted())
                    deleted.emplace(it->first);
                else if (deleted.find(it->first) == deleted.end()) {
                    if (it->first == key)
                        return iterator(this, i, it);
                    lb = it;
                    lb_level = i;
                    lb_set = true;
                    break;
                }
            }
        }

        if (lb_set)
            return iterator(this, lb_level, lb);
        return end();
    }

    /**
     * Checks if the container has no elements, i.e. whether begin() == end().
     * @return true if the container is empty, false otherwise
     */
    bool empty() const { return begin() == end(); }

    /**
     * Returns an iterator to the beginning.
     * @return an iterator to the beginning
     */
    iterator begin() const { return lower_bound(std::numeric_limits<K>::min()); }

    /**
     * Returns an iterator to the end.
     * @return an iterator to the end
     */
    iterator end() const { return iterator(this, levels.size() - 1, levels.back().end()); }

    /**
     * Returns the number of elements with key that compares equal to the specified argument key, which is either 1
     * or 0 since this container does not allow duplicates.
     * @param key key value of the elements to count
     * @return number of elements with the given key, which is either 1 or 0.
     */
    size_t count(const K &key) const { return find(key) == end() ? 0 : 1; }

    /**
     * Returns the number of elements in the container.
     * @return the number of elements in the container
     */
    size_t size() const {
        // TODO: scanning the levels and using a hash table for the encountered keys could be more time efficient
        return std::distance(begin(), end());
    }

    /**
     * Returns the size of the container (data + index structure) in bytes.
     * @return the size of the container in bytes
     */
    size_t size_in_bytes() const {
        size_t bytes = levels.size() * sizeof(Level);
        for (auto &l: levels)
            bytes += l.size() * sizeof(Item);
        return index_size_in_bytes() + bytes;
    }

    /**
     * Returns the size of the index used in this container in bytes.
     * @return the size of the index used in this container in bytes
     */
    size_t index_size_in_bytes() const {
        size_t bytes = 0;
        for (auto &p: pgms)
            bytes += p.size_in_bytes();
        return bytes;
    }

private:

    template<bool SkipDeleted, bool Move, typename In1, typename In2, typename OutIterator>
    static OutIterator merge(In1 first1, In1 last1, In2 first2, In2 last2, OutIterator result) {
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                if constexpr (Move) *result = std::move(*first2);
                else *result = *first2;
                ++first2;
                ++result;
            } else if (*first1 < *first2) {
                if constexpr (Move) *result = std::move(*first1);
                else *result = *first1;
                ++first1;
                ++result;
            } else if (SkipDeleted && first1->deleted()) {
                ++first1;
                ++first2;
            } else {
                if constexpr (Move) *result = std::move(*first1);
                else *result = *first1;
                ++first1;
                ++first2;
                ++result;
            }
        }
        if constexpr (Move)
            return std::move(first2, last2, std::move(first1, last1, result));
        return std::copy(first2, last2, std::copy(first1, last1, result));
    }

    template<class RandomIt>
    static RandomIt lower_bound_bl(RandomIt first, RandomIt last, const K &x) {
        if (first == last)
            return first;
        auto n = std::distance(first, last);
        while (n > 1) {
            auto half = n / 2;
            __builtin_prefetch(&*(first + half / 2), 0, 0);
            __builtin_prefetch(&*(first + half + half / 2), 0, 0);
            first = first[half] < x ? first + half : first;
            n -= half;
        }
        return first + (*first < x);
    }
};

namespace internal {

/* LoserTree implementation adapted from Timo Bingmann's https://tlx.github.io and http://stxxl.org, and from
 * Johannes Singler's http://algo2.iti.uni-karlsruhe.de/singler/mcstl. These three libraries are distributed under the
 * Boost Software License 1.0. */
template<typename T>
class LoserTree {
    using Source = uint8_t;

    struct Loser {
        T key;         ///< Copy of the current key in the sequence.
        Source source; ///< Index of the sequence.
    };

    Source k;                  ///< Smallest power of 2 greater than the number of nodes.
    std::vector<Loser> losers; ///< Vector of size 2k containing loser tree nodes.

    static uint64_t next_pow2(uint64_t x) {
        return x == 1 ? 1 : uint64_t(1) << (sizeof(unsigned long long) * 8 - __builtin_clzll(x - 1));
    }

    /** Called recursively to build the initial tree. */
    Source init_winner(const Source &root) {
        if (root >= k)
            return root;

        auto left = init_winner(2 * root);
        auto right = init_winner(2 * root + 1);
        if (losers[right].key >= losers[left].key) {
            losers[root] = losers[right];
            return left;
        } else {
            losers[root] = losers[left];
            return right;
        }
    }

public:

    LoserTree() = default;

    explicit LoserTree(const Source &ik) : k(next_pow2(ik)), losers(2 * k) {
        for (auto i = ik - 1u; i < k; ++i) {
            losers[i + k].key = std::numeric_limits<T>::max();
            losers[i + k].source = std::numeric_limits<Source>::max();
        }
    }

    /** Returns the index of the sequence with the smallest element. */
    Source min_source() const {
        assert(losers[0].source != std::numeric_limits<Source>::max());
        return losers[0].source;
    }

    /** Inserts the initial element of the sequence source. */
    void insert_start(const T *key_ptr, const Source &source) {
        Source pos = k + source;
        assert(pos < losers.size());
        losers[pos].source = source;
        losers[pos].key = *key_ptr;
    }

    /** Deletes the smallest element and insert a new element in its place. */
    void delete_min_insert(const T *key_ptr) {
        auto source = losers[0].source;
        auto key = key_ptr ? *key_ptr : std::numeric_limits<T>::max();

        for (auto pos = (k + source) / 2; pos > 0; pos /= 2) {
            if (losers[pos].key < key || (key >= losers[pos].key && losers[pos].source < source)) {
                std::swap(losers[pos].source, source);
                std::swap(losers[pos].key, key);
            }
        }

        losers[0].source = source;
        losers[0].key = key;
    }

    /** Initializes the tree. */
    void init() { losers[0] = losers[init_winner(1)]; }
};

} // namespace internal

template<typename K, typename V, typename PGMType>
class DynamicPGMIndex<K, V, PGMType>::Iterator {
    friend class DynamicPGMIndex;

    using level_iterator = typename Level::const_iterator;
    using dynamic_pgm_type = DynamicPGMIndex<K, V, PGMType>;

    struct Cursor {
        uint8_t level_number;
        level_iterator iterator;
        Cursor() = default;
        Cursor(uint8_t level_number, const level_iterator iterator) : level_number(level_number), iterator(iterator) {}
    };

    const dynamic_pgm_type *super;  ///< Pointer to the container that is being iterated.
    Cursor current;                 ///< Pair (level number, iterator to the current element).
    bool initialized;               ///< true iff the members tree and iterators have been initialized.
    uint8_t unconsumed_count;       ///< Number of iterators that have not yet reached the end.
    internal::LoserTree<K> tree;    ///< Tournament tree with one leaf for each iterator.
    std::vector<Cursor> iterators;  ///< Vector with pairs (level number, iterator).

    void lazy_initialize() {
        if (initialized)
            return;

        // For each level create and position an iterator to the first key > current
        iterators.reserve(super->used_levels - super->min_level);
        for (uint8_t i = super->min_level; i < super->used_levels; ++i) {
            auto &level = super->level(i);
            if (level.empty())
                continue;

            size_t lo = 0;
            size_t hi = level.size();
            if (super->has_pgm(i)) {
                auto range = super->pgm(i).search(current.iterator->first);
                lo = range.lo;
                hi = range.hi;
            }

            auto pos = std::upper_bound(level.begin() + lo, level.begin() + hi, current.iterator->first);
            if (pos != level.end())
                iterators.emplace_back(i, pos);
        }

        tree = decltype(tree)(iterators.size());
        for (size_t i = 0; i < iterators.size(); ++i)
            tree.insert_start(&iterators[i].iterator->first, i);
        tree.init();

        initialized = true;
        unconsumed_count = iterators.size();
    }

    void advance() {
        if (unconsumed_count == 0) {
            *this = super->end();
            return;
        }

        auto step = [&] {
            auto &it_min = iterators[tree.min_source()];
            auto level_number = it_min.level_number;
            auto result = it_min.iterator;
            ++it_min.iterator;
            if (it_min.iterator == super->level(level_number).end()) {
                tree.delete_min_insert(nullptr);
                --unconsumed_count;
            } else
                tree.delete_min_insert(&it_min.iterator->first);
            return Cursor(level_number, result);
        };

        Cursor tmp;
        do {
            tmp = step();
            while (unconsumed_count > 0 && iterators[tree.min_source()].iterator->first == tmp.iterator->first)
                step();
        } while (unconsumed_count > 0 && tmp.iterator->deleted());

        if (tmp.iterator->deleted())
            *this = super->end();
        else
            current = tmp;
    }

    Iterator(const dynamic_pgm_type *p, uint8_t level_number, const level_iterator it)
        : super(p), current(level_number, it), initialized(), unconsumed_count(), tree(), iterators() {};

public:

    using difference_type = typename decltype(levels)::difference_type;
    using value_type = const Item;
    using pointer = const Item *;
    using reference = const Item &;
    using iterator_category = std::forward_iterator_tag;

    Iterator &operator++() {
        lazy_initialize();
        advance();
        return *this;
    }

    Iterator operator++(int) {
        Iterator i(current);
        ++i;
        return i;
    }

    reference operator*() const { return *current.iterator; }
    pointer operator->() const { return &*current.iterator; };

    bool operator==(const Iterator &rhs) const {
        return current.level_number == rhs.current.level_number && current.iterator == rhs.current.iterator;
    }

    bool operator!=(const Iterator &rhs) const { return !(*this == rhs); }
};

#pragma pack(push, 1)

template<typename K, typename V, typename PGMType>
class DynamicPGMIndex<K, V, PGMType>::ItemA {
    const static V tombstone;

    template<typename T = V, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
    static V get_tombstone() { return new std::remove_pointer_t<V>(); }

    template<typename T = V, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    static V get_tombstone() { return std::numeric_limits<V>::max(); }

public:
    K first;
    V second;

    ItemA() { /* do not (default-)initialize for a more efficient std::vector<ItemA>::resize */ }
    explicit ItemA(const K &key) : first(key), second(tombstone) {}
    explicit ItemA(const K &key, const V &value) : first(key), second(value) {
        if (second == tombstone)
            throw std::invalid_argument("The specified value is reserved and cannot be used.");
    }

    operator K() const { return first; }
    bool deleted() const { return this->second == tombstone; }
};

template<typename K, typename V, typename PGMType>
const V DynamicPGMIndex<K, V, PGMType>::ItemA::tombstone = get_tombstone<V>();

template<typename K, typename V, typename PGMType>
class DynamicPGMIndex<K, V, PGMType>::ItemB {
    bool flag;

public:
    K first;
    V second;

    ItemB() = default;
    explicit ItemB(const K &key) : flag(true), first(key), second() {}
    explicit ItemB(const K &key, const V &value) : flag(false), first(key), second(value) {}

    operator K() const { return first; }
    bool deleted() const { return flag; }
};

#pragma pack(pop)

}
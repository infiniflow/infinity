// This file is part of PGM-index <https://github.com/gvinciguerra/PGM-index>.
// Copyright (c) 2020 Giorgio Vinciguerra.
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

#include "morton_nd.hpp"
#include "piecewise_linear_model.hpp"
#include "pgm_index.hpp"
#include "sdsl.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <algorithm>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace pgm {

/** Computes the smallest integral value not less than x / y, where y must be a positive integer. */
#define CEIL_INT_DIV(x, y) ((x) / (y) + ((x) % (y) > 0))

/** Computes the number of bits needed to store x, that is, 0 if x is 0, 1 + floor(log2(x)) otherwise. */
#define BIT_WIDTH(x) ((x) == 0 ? 0 : 64 - __builtin_clzll(x))

/**
 * A variant of @ref PGMIndex that does not build a recursive structure but uses a binary search on the segments.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the search range
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, size_t Epsilon, typename Floating = float>
using OneLevelPGMIndex = PGMIndex<K, Epsilon, 0, Floating>;

/**
 * A variant of @ref PGMIndex that uses compression on the segments to reduce the space of the index.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the returned search range
 * @tparam EpsilonRecursive controls the size of the search range in the internal structure
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, size_t Epsilon, size_t EpsilonRecursive = 4, typename Floating = float>
class CompressedPGMIndex {
    static_assert(Epsilon > 0);
    struct CompressedLevel;

    size_t n;                             ///< The number of elements in the indexed data.
    K first_key;                          ///< The smallest element in the data.
    Floating root_slope;                  ///< The slope of the root segment.
    int64_t root_intercept;               ///< The intercept of the root segment.
    size_t root_range;                    ///< The size of the level below the root segment.
    std::vector<Floating> slopes_table;   ///< The vector containing the slopes used by the segments in the index.
    std::vector<CompressedLevel> levels;  ///< The levels composing the compressed index.

    /// Sentinel value to avoid bounds checking.
    static constexpr K sentinel = std::numeric_limits<K>::has_infinity ? std::numeric_limits<K>::infinity()
                                                                       : std::numeric_limits<K>::max();
    using floating_pair = std::pair<Floating, Floating>;
    using canonical_segment = typename internal::OptimalPiecewiseLinearModel<K, size_t>::CanonicalSegment;

public:

    static constexpr size_t epsilon_value = Epsilon;

    /**
     * Constructs an empty index.
     */
    CompressedPGMIndex() = default;

    /**
     * Constructs the compressed index on the given sorted vector.
     * @param data the vector of elements to be indexed, must be sorted
     */
    explicit CompressedPGMIndex(const std::vector<K> &data) : CompressedPGMIndex(data.begin(), data.end()) {}

    /**
     * Constructs the compressed index on the sorted elements in the range [first, last).
     * @param first, last the range containing the sorted elements to be indexed
     */
    template<typename Iterator>
    CompressedPGMIndex(Iterator first, Iterator last) : n(std::distance(first, last)) {
        if (n == 0)
            return;

        std::vector<size_t> levels_offsets({0});
        std::vector<canonical_segment> segments;
        segments.reserve(n / (Epsilon * Epsilon));

        if (*std::prev(last) == sentinel)
            throw std::invalid_argument("The value " + std::to_string(sentinel) + " is reserved as a sentinel.");

        // Build first level
        auto in_fun = [&](auto i) { return first[i]; };
        auto out_fun = [&](auto cs) { segments.emplace_back(cs); };
        auto last_n = internal::make_segmentation_par(n, Epsilon, in_fun, out_fun);
        levels_offsets.push_back(levels_offsets.back() + last_n);

        // Build upper levels
        while (EpsilonRecursive && last_n > 1) {
            auto offset = levels_offsets[levels_offsets.size() - 2];
            auto in_fun_rec = [&](auto i) { return segments[offset + i].get_first_x(); };
            last_n = internal::make_segmentation(last_n, EpsilonRecursive, in_fun_rec, out_fun);
            levels_offsets.push_back(levels_offsets.back() + last_n);
        }

        // Compress the slopes
        auto[tmp_table, map, intercepts] = merge_slopes(segments);
        slopes_table = tmp_table;

        // Build levels
        auto n_levels = levels_offsets.size() - 1;
        first_key = *first;
        if constexpr (EpsilonRecursive > 0) {
            auto root = *std::prev(levels_offsets.end(), 2);
            std::tie(root_slope, root_intercept) = segments[root].get_floating_point_segment(first_key);
            root_range = n_levels == 1 ? n : levels_offsets[n_levels - 1] - levels_offsets[n_levels - 2];
        }

        levels.reserve(n_levels - 1);
        for (int i = EpsilonRecursive == 0 ? 1 : n_levels - 1; i > 0; --i) {
            auto l = levels_offsets[i - 1];
            auto r = levels_offsets[i];
            auto prev_level_size = i == 1 ? n : l - levels_offsets[i - 2];
            levels.emplace_back(segments.begin() + l, segments.begin() + r,
                                intercepts.begin() + l, intercepts.begin() + r,
                                map.begin() + l, map.begin() + r,
                                slopes_table, prev_level_size, *std::prev(last));
        }
    }

    /**
     * Returns the size of the index in bytes.
     * @return the size of the index in bytes
     */
    size_t size_in_bytes() const {
        size_t accum = 0;
        for (auto &l : levels)
            accum += l.size_in_bytes();
        return accum + slopes_table.size() * sizeof(Floating);
    }

    /**
     * Returns the approximate position and the range where @p key can be found.
     * @param key the value of the element to search for
     * @return a struct with the approximate position and bounds of the range
     */
    ApproxPos search(const K &key) const {
        auto k = std::max(first_key, key);

        if constexpr (EpsilonRecursive == 0) {
            auto &level = levels.front();
            auto it = std::upper_bound(level.keys.begin(), level.keys.begin() + level.size(), key);
            auto i = std::distance(level.keys.begin(), it) - 1;
            auto pos = std::min<size_t>(level(slopes_table, i, k), level.get_intercept(i + 1));
            auto lo = PGM_SUB_EPS(pos, Epsilon);
            auto hi = PGM_ADD_EPS(pos, Epsilon, n);
            return {pos, lo, hi};
        }

        auto p = int64_t(root_slope * (k - first_key)) + root_intercept;
        auto pos = std::min<size_t>(p > 0 ? size_t(p) : 0ull, root_range);

        for (const auto &level : levels) {
            auto lo = level.keys.begin() + PGM_SUB_EPS(pos, EpsilonRecursive + 1);

            static constexpr size_t linear_search_threshold = 8 * 64 / sizeof(K);
            if constexpr (EpsilonRecursive <= linear_search_threshold) {
                for (; *std::next(lo) <= key; ++lo)
                    continue;
            } else {
                auto hi = level.keys.begin() + PGM_ADD_EPS(pos, EpsilonRecursive, level.size());
                auto it = std::prev(std::upper_bound(lo, hi, k));
            }

            auto i = std::distance(level.keys.begin(), lo);
            pos = std::min<size_t>(level(slopes_table, i, k), level.get_intercept(i + 1));
        }

        auto lo = PGM_SUB_EPS(pos, Epsilon);
        auto hi = PGM_ADD_EPS(pos, Epsilon, n);
        return {pos, lo, hi};
    }

    /**
     * Returns the number of segments in the last level of the index.
     * @return the number of segments
     */
    size_t segments_count() const {
        return levels.back().size();
    }

    /**
     * Returns the number of levels of the index.
     * @return the number of levels of the index
     */
    size_t height() const {
        return levels.size() + 1;
    }

private:

    template<typename T, typename Cmp>
    static std::vector<size_t> sort_indexes(const std::vector<T> &v, Cmp cmp) {
        std::vector<size_t> idx(v.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(), cmp);
        return idx;
    }

    static std::tuple<std::vector<Floating>, std::vector<uint32_t>, std::vector<int64_t>>
    merge_slopes(const std::vector<canonical_segment> &segments) {
        std::vector<K> keys;
        std::vector<Floating> slopes_table;
        std::vector<int64_t> intercepts;
        intercepts.reserve(segments.size());
        slopes_table.reserve(segments.size());

        auto cmp = [&](auto i1, auto i2) { return segments[i1].get_slope_range() < segments[i2].get_slope_range(); };
        auto sorted_indexes = sort_indexes(segments, cmp);
        auto[current_min, current_max] = segments[sorted_indexes[0]].get_slope_range();

        std::vector<uint32_t> mapping(segments.size());
        mapping[sorted_indexes[0]] = 0;

        for (size_t i = 1; i < sorted_indexes.size(); ++i) {
            auto[min, max] = segments[sorted_indexes[i]].get_slope_range();
            if (min > current_max) {
                auto slope = 0.5 * (current_min + current_max);
                slopes_table.push_back(slope);
                current_min = min;
                current_max = max;
            } else {
                if (min > current_min)
                    current_min = min;
                if (max < current_max)
                    current_max = max;
            }
            mapping[sorted_indexes[i]] = uint32_t(slopes_table.size());
        }

        slopes_table.push_back(0.5 * (current_min + current_max));

        // Compute intercepts
        intercepts.reserve(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            auto[i_x, i_y] = segments[i].get_intersection();
            auto slope = slopes_table[mapping[i]];
            auto intercept = (int64_t) std::round(i_y - (i_x - segments[i].get_first_x()) * slope);
            intercepts.push_back(intercept);
        }

        return {slopes_table, mapping, intercepts};
    }
};

template<typename K, size_t Epsilon, size_t EpsilonRecursive, typename Floating>
struct CompressedPGMIndex<K, Epsilon, EpsilonRecursive, Floating>::CompressedLevel {
    std::vector<K> keys;                       ///< The keys of the segment in this level.
    sdsl::int_vector<> slopes_map;             ///< The ith element is an index into slopes_table.
    int64_t intercept_offset;                  ///< An offset to make the intercepts start from 0 in the bitvector.
    sdsl::sd_vector<> compressed_intercepts;   ///< The compressed bitvector storing the intercepts.
    sdsl::sd_vector<>::select_1_type sel1;     ///< The select1 succinct data structure on compressed_intercepts.

    template<typename IterK, typename IterI, typename IterM>
    CompressedLevel(IterK first_segment, IterK last_segment,
                    IterI first_intercept, IterI last_intercept,
                    IterM first_slope, IterM last_slope,
                    const std::vector<Floating> &slopes_table,
                    size_t prev_level_size,
                    K last_key)
        : keys(),
          intercept_offset(*first_intercept) {
        // If true, we need an extra segment to ensure that keys > *(last-1) are approximated to a position == n
        auto need_extra_segment = slopes_table[*std::prev(last_slope)] == 0;

        // Store keys
        keys.reserve(std::distance(first_segment, last_segment) + need_extra_segment + 1);
        for (auto it = first_segment; it != last_segment; ++it)
            keys.emplace_back(it->get_first_x());
        if (need_extra_segment)
            keys.emplace_back(last_key + 1);
        keys.emplace_back(sentinel);

        // Compress and store intercepts
        auto max_intercept = prev_level_size - intercept_offset + 2;
        auto intercepts_count = std::distance(first_intercept, last_intercept) + need_extra_segment + 1;
        sdsl::sd_vector_builder builder(max_intercept, intercepts_count);
        builder.set(0);
        for (auto it = first_intercept + 1; it != last_intercept; ++it)
            builder.set(std::clamp<int64_t>(*it, *(it - 1) + 1, prev_level_size - 1) - intercept_offset);
        if (need_extra_segment)
            builder.set(max_intercept - 2);
        builder.set(max_intercept - 1);
        compressed_intercepts = sdsl::sd_vector<>(builder);
        sdsl::util::init_support(sel1, &compressed_intercepts);

        // Compress and store slopes_map
        auto map_size = std::distance(first_slope, last_slope) + need_extra_segment;
        slopes_map = sdsl::int_vector<>(map_size, 0, sdsl::bits::hi(slopes_table.size() - 1) + 1);
        std::copy(first_slope, last_slope, slopes_map.begin());
        if (need_extra_segment)
            slopes_map.back() = slopes_table[*std::prev(last_slope)];
    }

    inline size_t operator()(const std::vector<Floating> &slopes, size_t i, K k) const {
        auto pos = int64_t(get_slope(slopes, i) * (k - keys[i])) + get_intercept(i);
        return pos > 0 ? size_t(pos) : 0ull;
    }

    inline Floating get_slope(const std::vector<Floating> &slopes, size_t i) const {
        return slopes[slopes_map[i]];
    }

    inline int64_t get_intercept(size_t i) const {
        return intercept_offset + int64_t(sel1(i + 1));
    }

    inline size_t size() const {
        return keys.size() - 1;
    }

    inline size_t size_in_bytes() const {
        return keys.size() * sizeof(K) + slopes_map.bit_size() / 8 + sdsl::size_in_bytes(compressed_intercepts);
    }
};

/**
 * A simple variant of @ref OneLevelPGMIndex that uses length reduction to speed up the search on the segments.
 *
 * Segments are assigned to a number of buckets specified with the @p TopLevelSize template argument. A top-level table
 * of size @p TopLevelSize stores the position of the first segment in each bucket. The @p TopLevelBitSize template
 * argument allows to specify the bit-size of the memory cells in the top-level table. If set to 0, the bit-size of the
 * cells will be determined dynamically so that the table is bit-compressed.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the returned search range
 * @tparam TopLevelSize the number of cells allocated for the top-level table
 * @tparam TopLevelBitSize the bit-size of the cells in the top-level table
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, size_t Epsilon, size_t TopLevelSize, uint8_t TopLevelBitSize = 32, typename Floating = float>
class BucketingPGMIndex {
protected:
    static_assert(Epsilon > 0 && TopLevelSize > 0);

    using Segment = typename PGMIndex<K, Epsilon, 0, Floating>::Segment;
    static constexpr bool pow_two_top_level = (TopLevelSize & (TopLevelSize - 1u)) == 0;

    size_t n;                                     ///< The number of elements this index was built on.
    K first_key;                                  ///< The smallest element.
    K last_key;                                   ///< The largest element.
    std::vector<Segment> segments;                ///< The segments composing the index.
    sdsl::int_vector<TopLevelBitSize> top_level;  ///< The structure on the segment.
    K step;

    void build_top_level() {
        // Compute the size of the partitioned universe
        size_t actual_top_level_size = TopLevelSize + 2;
        if constexpr (pow_two_top_level) {
            step = K(1) << (sizeof(K) * CHAR_BIT - BIT_WIDTH(TopLevelSize) + 1);
            actual_top_level_size = CEIL_INT_DIV(last_key - first_key, step) + 2;
        } else
            step = std::max<K>(CEIL_INT_DIV(last_key - first_key, TopLevelSize), 1);

        // Allocate the top-level table
        auto log_segments = (size_t) BIT_WIDTH(segments.size());
        if constexpr (TopLevelBitSize == 0)
            top_level = sdsl::int_vector<>(actual_top_level_size, 0, log_segments);
        else {
            if (TopLevelBitSize < log_segments)
                throw std::invalid_argument("TopLevelBitSize must be >=" + std::to_string(log_segments));
            top_level = sdsl::int_vector<TopLevelBitSize>(actual_top_level_size, 0, TopLevelBitSize);
        }

        // Fill the top-level table
        for (uint64_t i = 1, k = 1; i < actual_top_level_size - 1; ++i) {
            K upper_bound;
            if (__builtin_mul_overflow(K(i), step, &upper_bound))
                upper_bound = std::numeric_limits<K>::max();
            while (k < segments.size() && (segments[k].key - first_key) < upper_bound)
                ++k;
            top_level[i] = k;
        }
        top_level[actual_top_level_size - 1] = segments.size();
    }

    /**
     * Returns the segment responsible for a given key, that is, the rightmost segment having key <= the sought key.
     * @param key the value of the element to search for
     * @return an iterator to the segment responsible for the given key
     */
    auto segment_for_key(const K &key) const {
        size_t j;
        if constexpr (pow_two_top_level)
            j = (key - first_key) >> (sizeof(K) * CHAR_BIT - BIT_WIDTH(TopLevelSize) + 1);
        else
            j = (key - first_key) / step;
        auto first = segments.begin() + top_level[j];
        auto last = segments.begin() + top_level[j + 1];
        return std::prev(std::upper_bound(first, last, key));
    }

public:

    static constexpr size_t epsilon_value = Epsilon;

    /**
     * Constructs an empty index.
     */
    BucketingPGMIndex() = default;

    /**
     * Constructs the index on the given sorted vector.
     * @param data the vector of keys, must be sorted
     */
    BucketingPGMIndex(const std::vector<K> &data) : BucketingPGMIndex(data.begin(), data.end()) {}

    /**
     * Constructs the index on the sorted keys in the range [first, last).
     * @param first, last the range containing the sorted keys to be indexed
     */
    template<typename RandomIt>
    BucketingPGMIndex(RandomIt first, RandomIt last)
        : n(std::distance(first, last)),
          first_key(n ? *first : K(0)),
          last_key(n ? *(last - 1) : K(0)),
          segments(),
          top_level() {
        if (n == 0)
            return;
        std::vector<size_t> offsets;
        PGMIndex<K, Epsilon, 0, Floating>::build(first, last, Epsilon, 0, segments, offsets);
        build_top_level();
    }

    /**
     * Returns the approximate position and the range where @p key can be found.
     * @param key the value of the element to search for
     * @return a struct with the approximate position and bounds of the range
     */
    ApproxPos search(const K &key) const {
        if (__builtin_expect(key < first_key, 0))
            return {0, 0, 0};
        if (__builtin_expect(key > last_key, 0))
            return {n, n, n};
        auto it = segment_for_key(key);
        auto pos = std::min<size_t>((*it)(key), std::next(it)->intercept);
        auto lo = PGM_SUB_EPS(pos, Epsilon);
        auto hi = PGM_ADD_EPS(pos, Epsilon, n);
        return {pos, lo, hi};
    }

    /**
     * Returns the number of segments in the last level of the index.
     * @return the number of segments
     */
    size_t segments_count() const {
        return segments.size();
    }

    /**
     * Returns the number of levels of the index.
     * @return the number of levels of the index
     */
    size_t height() const {
        return 1;
    }

    /**
     * Returns the size of the index in bytes.
     * @return the size of the index in bytes
     */
    size_t size_in_bytes() const {
        return segments.size() * sizeof(Segment) + top_level.size() * top_level.width() / CHAR_BIT;
    }
};

/**
 * A variant of @ref OneLevelPGMIndex that builds a top-level succinct structure to speed up the search on the
 * segments.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the returned search range
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, size_t Epsilon = 64, typename Floating = float>
class EliasFanoPGMIndex {
protected:
    static_assert(Epsilon > 0);

    using Segment = typename PGMIndex<K, Epsilon, 0, Floating>::Segment;

    struct SegmentData {
        Floating slope;    ///< The slope of the segment.
        int32_t intercept; ///< The intercept of the segment.

        SegmentData() = default;

        SegmentData(Segment &s) : slope(s.slope), intercept(s.intercept) {}

        inline size_t operator()(const K &origin, const K &k) const {
            auto pos = int64_t(slope * (k - origin)) + intercept;
            return pos > 0 ? size_t(pos) : 0ull;
        }
    };

    size_t n;                           ///< The number of elements this index was built on.
    K first_key;                        ///< The smallest segment key.
    std::vector<SegmentData> segments;  ///< The segments composing the index.
    sdsl::sd_vector<> ef;               ///< The Elias-Fano structure on the segment.

public:

    static constexpr size_t epsilon_value = Epsilon;

    /**
     * Constructs an empty index.
     */
    EliasFanoPGMIndex() = default;

    /**
     * Constructs the index on the given sorted vector.
     * @param data the vector of keys, must be sorted
     */
    explicit EliasFanoPGMIndex(const std::vector<K> &data) : EliasFanoPGMIndex(data.begin(), data.end()) {}

    /**
     * Constructs the index on the sorted keys in the range [first, last).
     * @param first, last the range containing the sorted keys to be indexed
     */
    template<typename RandomIt>
    EliasFanoPGMIndex(RandomIt first, RandomIt last)
        : n(std::distance(first, last)),
          first_key(n ? *first : K(0)),
          segments(),
          ef() {
        if (n == 0)
            return;

        std::vector<Segment> tmp;
        std::vector<size_t> offsets;
        PGMIndex<K, Epsilon, 0, Floating>::build(first, last, Epsilon, 0, tmp, offsets);

        segments.reserve(tmp.size());
        for (auto &x: tmp) {
            segments.push_back(x);
            x.key -= first_key;
        }

        ef = decltype(ef)(tmp.begin(), std::prev(tmp.end()));
    }

    /**
     * Returns the approximate position and the range where @p key can be found.
     * @param key the value of the element to search for
     * @return a struct with the approximate position and bounds of the range
     */
    ApproxPos search(const K &key) const {
        auto k = std::max(first_key, key);
        auto[r, origin] = pred(k - first_key);
        auto pos = std::min<size_t>(segments[r](origin + first_key, k), segments[r + 1].intercept);
        auto lo = PGM_SUB_EPS(pos, Epsilon);
        auto hi = PGM_ADD_EPS(pos, Epsilon, n);
        return {pos, lo, hi};
    }

    /**
     * Returns the number of segments in the last level of the index.
     * @return the number of segments
     */
    size_t segments_count() const {
        return segments.size();
    }

    /**
     * Returns the number of levels of the index.
     * @return the number of levels of the index
     */
    size_t height() const {
        return 1;
    }

    /**
     * Returns the size of the index in bytes.
     * @return the size of the index in bytes
     */
    size_t size_in_bytes() const {
        return segments.size() * sizeof(SegmentData) + sdsl::size_in_bytes(ef);
    }

private:

    std::pair<size_t, uint64_t> pred(uint64_t i) const {
        if (i > ef.size()) {
            auto j = ef.low.size();
            return {j - 1, ef.low[j - 1] + ((ef.high_1_select(j) + 1 - j) << (ef.wl))};
        }
        ++i;

        auto high_val = (i >> (ef.wl));
        auto sel_high = ef.high_0_select(high_val + 1);
        auto rank_hi = sel_high - high_val;
        if (0 == rank_hi)
            return {0, ef.low[0] + (high_val << ef.wl)};

        auto rank_lo = high_val == 0 ? 0 : ef.high_0_select(high_val) - high_val + 1;
        auto val_low = i & sdsl::bits::lo_set[ef.wl];
        auto count = rank_hi - rank_lo;
        while (count > 0) {
            auto step = count / 2;
            auto mid = rank_lo + step;
            if (ef.low[mid] < val_low) {
                rank_lo = mid + 1;
                count -= step + 1;
            } else {
                count = step;
            }
        }
        --rank_lo;
        sel_high -= rank_hi - rank_lo;
        auto h = ef.high[sel_high] ? high_val : sdsl::bits::prev(ef.high.data(), sel_high) - rank_lo;
        return {rank_lo, ef.low[rank_lo] + (h << ef.wl)};
    }
};

/**
 * A disk-backed container storing a sorted sequence of numbers and a @ref PGMIndex for fast search operations.
 *
 * @tparam K the type of the indexed keys
 * @tparam Epsilon controls the size of the returned search range
 * @tparam EpsilonRecursive controls the size of the search range in the internal structure
 * @tparam Floating the floating-point type to use for slopes
 */
template<typename K, size_t Epsilon, size_t EpsilonRecursive = 4, typename Floating = float>
class MappedPGMIndex : public PGMIndex<K, Epsilon, EpsilonRecursive, Floating> {
    using base = PGMIndex<K, Epsilon, EpsilonRecursive, Floating>;
    K *data;
    size_t file_bytes;
    size_t header_bytes;

public:

    /**
     * Constructs a new disk-backed container on the sorted keys in the range [first, last).
     * @param first, last the range containing the sorted keys to copy
     * @param out_filename the name of the output file
     */
    template<class RandomIt>
    MappedPGMIndex(RandomIt first, RandomIt last, const std::string &out_filename)
        : base(first, last),
          data(),
          file_bytes(),
          header_bytes() {
        serialize_and_map(first, last, out_filename);
    }

    /**
     * Constructs a new disk-backed container on the sorted keys from the given input file.
     *
     * The @p in_filename must be a binary file containing a sorted sequence numbers of type @p K with the same
     * endianness of the CPU.
     *
     * @param in_filename the name of the input file
     * @param out_filename the name of the output file
     */
    MappedPGMIndex(const std::string &in_filename, const std::string &out_filename)
        : base(),
          data(),
          file_bytes(),
          header_bytes() {
        struct stat fs;
        stat(in_filename.c_str(), &fs);
        auto in_bytes = fs.st_size;
        if (in_bytes % sizeof(K) != 0)
            throw std::runtime_error("Input file size must be a multiple of " + std::to_string(sizeof(K)) + " bytes.");

        auto in_data = map_file(in_filename, in_bytes);
        this->n = in_bytes / sizeof(K);
        this->template build(in_data, in_data + this->n, Epsilon, EpsilonRecursive,
                             this->segments, this->levels_offsets);
        serialize_and_map(in_data, in_data + this->n, out_filename);
        unmap_file(in_data, in_bytes);
    }

    /**
     * Loads a disk-backed container from the given file.
     * @param in_filename the name of the input file
     */
    explicit MappedPGMIndex(const std::string &in_filename)
        : base(),
          data(),
          file_bytes(),
          header_bytes() {
        auto in = std::fstream(in_filename, std::ios::in | std::ios::binary);
        read_member(header_bytes, in);
        read_member(this->n, in);
        read_member(this->first_key, in);
        read_container(this->levels_offsets, in);
        read_container(this->segments, in);
        file_bytes = header_bytes + this->n * sizeof(K);
        data = map_file(in_filename, file_bytes);
    }

    /**
     * Destructs the object and closes the file backing the container.
     */
    ~MappedPGMIndex() { unmap_file(data, file_bytes); }

    /**
     * Checks if there is an element with key equivalent to @p key in the container.
     * @param key the value of the element to search for
     * @return @c true if there is such an element, otherwise @c false
     */
    bool contains(const K &key) const {
        auto range = this->search(key);
        return std::binary_search(begin() + range.lo, begin() + range.hi, key);
    }

    /**
     * Returns an iterator pointing to the first element that is not less than (i.e. greater or equal to) @p key.
     * @param key value to compare the elements to
     * @return iterator to the first element that is not less than @p key, or @ref end() if no such element is found
     */
    auto lower_bound(const K &key) const {
        auto range = this->search(key);
        return std::lower_bound(begin() + range.lo, begin() + range.hi, key);
    }

    /**
     * Returns an iterator pointing to the first element that is greater than @p key.
     * @param key value to compare the elements to
     * @return iterator to the first element that is greater than @p key, or @ref end() if no such element is found
     */
    auto upper_bound(const K &key) const {
        auto range = this->search(key);
        auto it = std::upper_bound(begin() + range.lo, begin() + range.hi, key);
        auto step = 1ull;
        while (it + step < end() && *(it + step) == key)  // exponential search to skip duplicates
            step *= 2;
        return std::upper_bound(it + (step / 2), std::min(it + step, end()), key);
    }

    /**
     * Returns the number of elements with key equal to the specified argument.
     * @param key value of the elements to count
     * @return the number of elements with key equal to @p key
     */
    size_t count(const K &key) const {
        auto lb = lower_bound(key);
        if (lb == end() || *lb != key)
            return 0;
        return std::distance(lb, upper_bound(key));
    }

    /**
     * Returns the number of elements in the container.
     * @return the number of elements in the container
     */
    size_t size() const { return this->n; }

    /**
     * Returns the size in bytes of the file backing the container.
     * @return the size in bytes of the file backing the container
     */
    size_t file_size_in_bytes() const { return file_bytes; }

    /**
     * Returns an iterator to the first element of the container.
     * @return an iterator to the first element of the container
     */
    auto begin() const { return (K *) ((char *) data + header_bytes); }

    /**
     * Returns an iterator to the element following the last element of the container.
     * @return an iterator to the element following the last element of the container
     */
    auto end() const { return begin() + size(); }

private:

    template<class RandomIt>
    void serialize_and_map(RandomIt first, RandomIt last, const std::string &out_filename) {
        auto out = std::fstream(out_filename, std::ios::out | std::ios::binary);
        header_bytes += write_member(header_bytes, out);
        header_bytes += write_member(this->n, out);
        header_bytes += write_member(this->first_key, out);
        header_bytes += write_container(this->levels_offsets, out);
        header_bytes += write_container(this->segments, out);
        for (auto it = first; it != last; ++it)
            write_member(*it, out);
        file_bytes = header_bytes + this->n * sizeof(K);
        out.seekp(0);
        write_member(header_bytes, out);
        data = map_file(out_filename, file_bytes);
    }

    static K *map_file(const std::string &in_filename, size_t file_bytes) {
        auto fd = open(in_filename.c_str(), O_RDONLY);
        if (fd == -1)
            throw std::runtime_error("Open file error" + std::string(strerror(errno)));

        auto data = (K *) mmap(nullptr, file_bytes, PROT_READ, MAP_SHARED, fd, 0);
        if (data == MAP_FAILED)
            throw std::runtime_error("mmap error" + std::string(strerror(errno)));
        return data;
    }

    static void unmap_file(K *data, size_t file_bytes) {
        if (data && munmap(data, file_bytes))
            std::cerr << "munmap error " << std::string(strerror(errno));
    }

    template<typename T>
    static size_t write_member(const T &x, std::ostream &out) {
        out.write((char *) &x, sizeof(T));
        return sizeof(T);
    }

    template<typename T>
    static void read_member(T &x, std::istream &in) {
        in.read((char *) &x, sizeof(T));
    }

    template<typename C>
    static size_t write_container(const C &container, std::ostream &out) {
        using value_type = typename C::value_type;
        size_t written_bytes = write_member(container.size(), out);
        for (auto &x: container) {
            out.write((char *) &x, sizeof(value_type));
            written_bytes += sizeof(value_type);
        }
        return written_bytes;
    }

    template<typename C>
    static void read_container(C &container, std::istream &in) {
        using value_type = typename C::value_type;
        typename C::size_type size;
        read_member(size, in);
        container.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            value_type s;
            in.read((char *) &s, sizeof(value_type));
            container.push_back(s);
        }
    }
};

#ifdef MORTON_ND_BMI2_ENABLED

#include <immintrin.h>

/**
 * A multidimensional container that uses a @ref PGMIndex for fast orthogonal range queries.
 *
 * @tparam Dimensions the number of fields/dimensions
 * @tparam T the type of the stored elements
 * @tparam Epsilon the Epsilon parameter for the internal @ref PGMIndex
 * @tparam EpsilonRecursive the EpsilonRecursive parameter for the internal @ref PGMIndex
 * @tparam Floating the Floating parameter for the internal @ref PGMIndex
 */
template<uint8_t Dimensions, typename T, size_t Epsilon, size_t EpsilonRecursive = 4, typename Floating = float>
class MultidimensionalPGMIndex {
    std::vector<T> data;
    PGMIndex<T, Epsilon, EpsilonRecursive, Floating> pgm;

    using morton = mortonnd::MortonNDBmi<Dimensions, T>;
    static constexpr auto selector = mortonnd::BuildSelector<morton::FieldBits>(Dimensions);
    static constexpr auto miss_threshold = 64;

    class RangeIterator;
    friend class RangeIterator;

public:

    using iterator = RangeIterator;
    using size_type = size_t;
    using value_type = decltype(morton::Decode(0));

    /**
     * Constructs an empty multidimensional container.
     */
    MultidimensionalPGMIndex() = default;

    /**
     * Constructs the multidimensional container with the elements in the range [first, last).
     * @param first, last the range to copy the elements from
     */
    template<typename RandomIt>
    MultidimensionalPGMIndex(RandomIt first, RandomIt last) : data(), pgm() {
        data.reserve(std::distance(first, last));
        std::for_each(first, last, [&](const auto &x) {
            std::apply([](auto &...x) {
                if (((BIT_WIDTH(x) >= morton::FieldBits) || ...)) {
                    auto tuple_str = ((std::to_string(x) + ",") + ...);
                    throw std::runtime_error("Type is too small to encode (" + tuple_str + "\b)");
                }
            }, x);
            data.emplace_back(encode(x));
        });
        std::sort(data.begin(), data.end());
        pgm = decltype(pgm)(data.begin(), data.end());
    }

    /**
     * Returns the size of the index in bytes.
     * @return the size of the index in bytes
     */
    size_t size_in_bytes() const { return pgm.size_in_bytes(); }

    /**
     * Checks if there is an element equal to @p p in the container.
     * @param p the element to search for
     * @return @c true if there is such an element, @c false otherwise
     */
    bool contains(const value_type &p) {
        auto zp = encode(p);
        auto range = pgm.search(zp);
        auto it = std::lower_bound(data.begin() + range.lo, data.begin() + range.hi, zp);
        return it != data.end() || morton::Decode(*it) == p;
    }

    /**
     * Returns an iterator to the first element of the container.
     * @return an iterator to the first element of the container
     */
    iterator begin() const { return iterator(data.begin()); }

    /**
     * Returns an iterator to the element following the last element of the container.
     * @return an iterator to the element following the last element of the container
     */
    iterator end() const { return iterator(data.end(), value_type()); }

    /**
     * Returns an iterator pointing to an element satisfying an orthogonal range query.
     *
     * Equivalently, returns an iterator to an element lying inside the hyperrectangle defined by the extreme points
     * @p min and @p max.
     *
     * Successive elements can be retrieved by calling @c operator++() on the returned iterator @c it until
     * @c it != end().
     *
     * @param min the lower extreme of the query hyperrectangle
     * @param max the upper extreme of the query hyperrectangle, must be greater than or equal to min.
     * @return an iterator pointing to an element inside the query hyperrectangle
     */
    iterator range(const value_type &min, const value_type &max) { return iterator(this, min, max); }
    
    /**
     * (approximate) k-nearest neighbor query.
     * Returns @p k nearest points from query point @p p.
     * 
     * @param p the query point.
     * @param k the number of nearest points.
     * @return a vector of k nearest points.
     */
    std::vector<value_type> knn(const value_type &p, uint32_t k){
        // to access coordinate of point dynamically
        using swallow = int[];
        auto sequence = std::make_index_sequence<Dimensions>{};

        // return euclidean distance between given point and query point p
        auto dist_from_p = [&]<std::size_t... indices>(value_type point, std::index_sequence<indices...>){
            uint64_t squared_sum = 0;
            squared_sum = (std::pow((int64_t)std::get<indices>(point) - (int64_t)std::get<indices>(p), 2) + ... );
            return std::sqrt(squared_sum);
        };

        // return first point of range query for knn query
        auto k_range_first = [&]<std::size_t... indices>(uint64_t dist, std::index_sequence<indices...>) -> value_type{
            value_type point;
            swallow{
                (std::get<indices>(point) = std::max<int64_t>((int64_t)std::get<indices>(p) - dist, 0), 0)...
            };
            return point;
        };

        // return end point of range query for knn query
        auto k_range_end = [&]<std::size_t... indices>(uint64_t dist, std::index_sequence<indices...>) -> value_type{
            value_type point;
            swallow{
                (std::get<indices>(point) = std::min<uint64_t>(std::get<indices>(p) + dist, this->data.size()), 0)...
            };
            return point;
        };

        // for debug
        auto print_point = [&]<std::size_t... indices>(value_type point, std::index_sequence<indices...>){
            std::cout << "(";
            swallow{
                (std::cout << std::get<indices>(point) << " , ", 0)...
            };
            std::cout << ")";
        };

        // get 2k points around zp to make temporary answer
        auto zp = encode(p);
        auto range = pgm.search(zp);
        auto it = std::lower_bound(data.begin() + range.lo, data.begin() + range.hi, zp);

        std::vector<value_type> tmp_ans;
        for (auto i = it - k >= data.begin() ? it - k : data.begin(); i != it + k && i != data.end(); ++i)
            tmp_ans.push_back(morton::Decode(*i));

        std::sort(tmp_ans.begin(), tmp_ans.end(), [&](auto const& lhs, auto const& rhs) {
            double dist_l = dist_from_p(lhs, sequence);
            double dist_r = dist_from_p(rhs, sequence);
            return dist_l < dist_r;
        });

        // calc distance of k nearest point in tmp_ans, and get a range(hyperrectangle) that contains more than k points around p
        uint64_t k_range_dist = dist_from_p(tmp_ans[k - 1], sequence) + 1;
        value_type first = k_range_first(k_range_dist, sequence);
        value_type end = k_range_end(k_range_dist, sequence);
        
        // execute range query and get k nearest points
        std::vector<value_type> ans;
        for (auto it = this->range(first, end); it != this->end(); ++it)
            ans.push_back(*it);

        std::sort(ans.begin(), ans.end(), [&](auto const& lhs, auto const& rhs) {
            double dist_l = dist_from_p(lhs, sequence);
            double dist_r = dist_from_p(rhs, sequence);
            return dist_l < dist_r;
        });
        
        return std::vector<value_type> {ans.begin(), ans.begin() + k};
    }

private:

    class RangeIterator {
        using multidimensional_pgm_type = MultidimensionalPGMIndex<Dimensions, T, Epsilon, EpsilonRecursive, Floating>;
        using internal_iterator = typename decltype(multidimensional_pgm_type::data)::const_iterator;

    public:

        using value_type = multidimensional_pgm_type::value_type;
        using difference_type = typename internal_iterator::difference_type;
        using pointer = const value_type *;
        using reference = const value_type &;
        using iterator_category = std::forward_iterator_tag;

    private:

        const multidimensional_pgm_type *super;
        multidimensional_pgm_type::value_type p;
        internal_iterator it;
        value_type min;
        value_type max;
        T zmin;
        T zmax;
        int miss;

        void advance() {
            ++it;
            if (miss == -1)
                return;

            while (it != super->data.end() && *it <= zmax) {
                if (box_zcontains(zmin, zmax, *it)) {
                    this->p = morton::Decode(*it);
                    return;
                }
                else if (++miss > miss_threshold) {
                    miss = 0;
                    auto bmin = bigmin(*it, zmin, zmax);
                    auto range = super->pgm.search(bmin);
                    it = std::upper_bound(super->data.begin() + range.lo, super->data.begin() + range.hi, bmin);
                    --it;
                }
                ++it;
            }

            if (*it > zmax)
                it = super->data.end();
        }

    public:

        RangeIterator(internal_iterator it) : RangeIterator(it, morton::Decode(*it)) {}

        RangeIterator(internal_iterator it, const value_type &p) : p(p), it(it), miss(-1) {}

        RangeIterator(const decltype(super) super, const value_type &min, const value_type &max)
            : super(super),
              min(min),
              max(max),
              zmin(multidimensional_pgm_type::encode(min)),
              zmax(multidimensional_pgm_type::encode(max)),
              miss(0) {
            if (zmin > zmax)
                throw std::invalid_argument("min > max");

            auto range = super->pgm.search(zmin);
            this->it = std::lower_bound(super->data.begin() + range.lo, super->data.begin() + range.hi, zmin);
            if (this->it == super->data.end())
                return;

            if (box_zcontains(zmin, zmax, *this->it))
                this->p = morton::Decode(*this->it);
            else
                advance();
        }

        RangeIterator &operator++() {
            advance();
            return *this;
        }

        RangeIterator operator++(int) {
            RangeIterator i(this->super, min, max);
            i.it = this->it;
            i.advance();
            return i;
        }

        reference operator*() const { return p; }
        pointer operator->() const { return &p; };
        bool operator==(const iterator &rhs) const { return it == rhs.it; }
        bool operator!=(const iterator &rhs) const { return it != rhs.it; }
    };

    template<typename Head, typename... Tail>
    constexpr static T encode(const std::tuple<Head, Tail...> &t) {
        return apply([](const auto &head, const auto &... tail) { return morton::Encode(head, tail...); }, t);
    }

    template<typename T1, typename T2>
    constexpr static T encode(const std::pair<T1, T2> &t) { return encode(std::tuple<T1, T2>(t.first, t.second)); }

    template<size_t ...I>
    constexpr static bool box_zcontains_field(const T &min, const T &max, const T &p, std::index_sequence<I...>) {
        return (((min & (selector << I)) <= (p & (selector << I))
            && (p & (selector << I)) <= (max & (selector << I))) && ... );
    }

    /**
     * Returns @c true if and only if the given element @p p lies inside the hyperrectangle defined by the extremes
     * @p min and @p max.
     */
    constexpr static bool box_zcontains(const T &min, const T &max, const T &p) {
        return box_zcontains_field(min, max, p, std::make_index_sequence<Dimensions>());
    }

    /**
     * Loads @p pattern into the bits of @p target associated to the given @p dimension, starting at @p bit_position,
     * leaving the other bits untouched.
     */
    static T load(T target, T pattern, uint8_t bit_position, uint8_t dimension) {
        auto mask = ~_pdep_u64(sdsl::bits::lo_set[bit_position], selector << dimension);
        auto pdep = _pdep_u64(pattern, selector << dimension);
        return (target & mask) | pdep;
    }

    /**
     * Computes the lowest morton code within the range [@p min, @p max] greater than @p x.
     */
    static T bigmin(const T &xd, const T &min, const T &max) {
        // http://hermanntropf.de/media/multidimensionalrangequery.pdf
        T bigmin = 0;
        T zmin = min;
        T zmax = max;
        auto hi_bit = std::max(std::max(sdsl::bits::hi(xd), sdsl::bits::hi(zmin)), sdsl::bits::hi(zmax));

        for (int b = hi_bit; b >= 0; --b) {
            auto bits = b / Dimensions + 1;
            auto dim = b % Dimensions;
            auto decision = uint8_t((((xd >> b) & 1) << 2) | (((zmin >> b) & 1) << 1) | ((zmax >> b) & 1));
            switch (decision) {
                case 0b001:
                    bigmin = load(zmin, T(1) << (bits - 1), bits, dim);
                    zmax = load(zmax, sdsl::bits::lo_set[bits - 1], bits, dim);
                    break;

                case 0b011:
                    bigmin = zmin;
                    return bigmin;

                case 0b100:
                    return bigmin;

                case 0b101:
                    zmin = load(zmin, T(1) << (bits - 1), bits, dim);
                    break;

                default:
                    continue;
            }
        }

        return bigmin;
    }
};

#endif

}
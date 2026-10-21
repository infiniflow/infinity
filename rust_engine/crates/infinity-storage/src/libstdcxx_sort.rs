//! libstdc++'s `std::sort`, reproduced.
//!
//! The reference's BMP index sorts a block's `(term_id, block_offset, score)`
//! triples with a comparator that looks at the **term id only**
//! (`TailFwd::ToBlockFwd`, `bmp_fwd.cppm:309`):
//!
//! ```cpp
//! std::sort(term_pairs.begin(), term_pairs.end(),
//!           [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });
//! ```
//!
//! `std::sort` is not stable, so the rows of one term come out in whatever
//! order libstdc++'s introsort leaves them in — and that order is written to
//! the chunk. It carries no meaning (both readers accumulate per row:
//! `scores[off] += q * value`), but it is part of the file's bytes.
//!
//! This module reproduces the *algorithm*, so the port writes the same bytes:
//! introsort — median-of-3 quicksort, an insertion sort for the tail, and a
//! heapsort fallback once the recursion depth is exhausted — exactly as
//! `bits/stl_algo.h` and `bits/stl_heap.h` implement it. It is the same kind
//! of dependency the port already takes on libstdc++'s `std::hash` for the
//! varchar secondary index's key (`hash_varchar`, `secondary_segment.rs`).
//!
//! The reference links libstdc++ 15.2.0 (the builder image's toolchain).
//! libstdc++ 16's `std::sort` is the same algorithm: the only differences are
//! a `__second` alias for `__first + 1`, value-vs-iterator comparator
//! arguments, and a redundant `*i < *first` guard in
//! `__unguarded_insertion_sort` whose condition cannot hold (the first
//! `_S_threshold` elements are the smallest of the range after
//! `__introsort_loop`). So both versions leave the same permutation.

/// `std::__lg`: floor(log2(n)), for `n >= 1`.
fn lg(n: usize) -> usize {
    (usize::BITS - 1 - n.leading_zeros()) as usize
}

/// `_S_threshold`: ranges of at most this many elements are finished with an
/// insertion sort, and introsort stops recursing below it.
const THRESHOLD: usize = 16;

/// `std::sort(first, last, less)`.
pub fn sort_by<T, F>(v: &mut [T], less: F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    if v.is_empty() {
        return;
    }
    let n = v.len();
    introsort_loop(v, 0, n, lg(n) * 2, &less);
    final_insertion_sort(v, 0, n, &less);
}

/// `__introsort_loop`: quicksort the right part, then loop on the left.
fn introsort_loop<T, F>(v: &mut [T], first: usize, mut last: usize, mut depth: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    while last - first > THRESHOLD {
        if depth == 0 {
            partial_sort(v, first, last, last, less);
            return;
        }
        depth -= 1;
        let cut = unguarded_partition_pivot(v, first, last, less);
        introsort_loop(v, cut, last, depth, less);
        last = cut;
    }
}

/// `__unguarded_partition_pivot`: median-of-3 onto `first`, then partition.
fn unguarded_partition_pivot<T, F>(v: &mut [T], first: usize, last: usize, less: &F) -> usize
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let mid = first + (last - first) / 2;
    move_median_to_first(v, first, first + 1, mid, last - 1, less);
    unguarded_partition(v, first + 1, last, first, less)
}

/// `__move_median_to_first`: swap the median of `a`, `b`, `c` to `result`.
fn move_median_to_first<T, F>(v: &mut [T], result: usize, a: usize, b: usize, c: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    if less(&v[a], &v[b]) {
        if less(&v[b], &v[c]) {
            v.swap(result, b);
        } else if less(&v[a], &v[c]) {
            v.swap(result, c);
        } else {
            v.swap(result, a);
        }
    } else if less(&v[a], &v[c]) {
        v.swap(result, a);
    } else if less(&v[b], &v[c]) {
        v.swap(result, c);
    } else {
        v.swap(result, b);
    }
}

/// `__unguarded_partition`: Hoare partition around `pivot`.
fn unguarded_partition<T, F>(
    v: &mut [T],
    mut first: usize,
    mut last: usize,
    pivot: usize,
    less: &F,
) -> usize
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    loop {
        while less(&v[first], &v[pivot]) {
            first += 1;
        }
        last -= 1;
        while less(&v[pivot], &v[last]) {
            last -= 1;
        }
        if first >= last {
            return first;
        }
        v.swap(first, last);
        first += 1;
    }
}

/// `__final_insertion_sort`.
fn final_insertion_sort<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    if last - first > THRESHOLD {
        insertion_sort(v, first, first + THRESHOLD, less);
        unguarded_insertion_sort(v, first + THRESHOLD, last, less);
    } else {
        insertion_sort(v, first, last, less);
    }
}

/// `__insertion_sort`: an insertion sort that moves the element to the front
/// when it is smaller than everything in the range.
fn insertion_sort<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    for i in (first + 1)..last {
        if less(&v[i], &v[first]) {
            v[first..=i].rotate_right(1);
        } else {
            unguarded_linear_insert(v, i, less);
        }
    }
}

/// `__unguarded_insertion_sort`.
fn unguarded_insertion_sort<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    for i in first..last {
        unguarded_linear_insert(v, i, less);
    }
}

/// `__unguarded_linear_insert`: shift the element left until its predecessor
/// is not greater. The caller guarantees that a sentinel (an element not
/// greater than it) precedes `hole`, so the scan never runs off the front;
/// the `hole > 0` test only keeps Rust from underflowing in the case the
/// sentinel guarantees cannot happen.
fn unguarded_linear_insert<T, F>(v: &mut [T], hole: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let value = v[hole];
    let mut hole = hole;
    while hole > 0 && less(&value, &v[hole - 1]) {
        v[hole] = v[hole - 1];
        hole -= 1;
    }
    v[hole] = value;
}

/// `__partial_sort(first, middle, last)` with `middle == last`: libstdc++'s
/// depth-limit fallback, which turns the whole range into a heap and sorts it
/// (`__heap_select` is only `__make_heap` when `middle == last`).
fn partial_sort<T, F>(v: &mut [T], first: usize, middle: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    debug_assert_eq!(
        middle, last,
        "only __partial_sort(first, last, last) is used"
    );
    make_heap(v, first, last, less);
    sort_heap(v, first, last, less);
}

/// `__make_heap`.
fn make_heap<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    if last - first < 2 {
        return;
    }
    let len = last - first;
    let mut parent = (len - 2) / 2;
    loop {
        let value = v[first + parent];
        adjust_heap(v, first, parent, len, value, less);
        if parent == 0 {
            return;
        }
        parent -= 1;
    }
}

/// `__adjust_heap`: sift `value` down from `hole`, then up to its place.
fn adjust_heap<T, F>(v: &mut [T], first: usize, hole: usize, len: usize, value: T, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let top = hole;
    let mut hole = hole;
    let mut second_child = hole;
    while second_child < (len - 1) / 2 {
        second_child = 2 * (second_child + 1);
        if less(&v[first + second_child], &v[first + second_child - 1]) {
            second_child -= 1;
        }
        v[first + hole] = v[first + second_child];
        hole = second_child;
    }
    if len >= 2 && (len & 1) == 0 && second_child == (len - 2) / 2 {
        second_child = 2 * (second_child + 1);
        v[first + hole] = v[first + second_child - 1];
        hole = second_child - 1;
    }
    push_heap(v, first, hole, top, value, less);
}

/// `__push_heap`.
fn push_heap<T, F>(v: &mut [T], first: usize, hole: usize, top: usize, value: T, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let mut hole = hole;
    while hole > top {
        let parent = (hole - 1) / 2;
        if !less(&v[first + parent], &value) {
            break;
        }
        v[first + hole] = v[first + parent];
        hole = parent;
    }
    v[first + hole] = value;
}

/// `__sort_heap`.
fn sort_heap<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let mut last = last;
    while last - first > 1 {
        last -= 1;
        pop_heap(v, first, last, less);
    }
}

/// `__pop_heap(__first, __last, __last)`.
fn pop_heap<T, F>(v: &mut [T], first: usize, last: usize, less: &F)
where
    T: Copy,
    F: Fn(&T, &T) -> bool,
{
    let value = v[last];
    v[last] = v[first];
    adjust_heap(v, first, 0, last - first, value, less);
}

#[cfg(test)]
mod tests {
    use super::sort_by;

    /// The order the reference's chunk has for the cross-engine test's first
    /// block (`sp` = `{0: 0.5, i % 5 + 1: 1.0}`, 16 documents): term 0's rows
    /// come out `12 9 8 10 7 6 11 5 4 13 3 14 2 15 1 0`. Captured from the
    /// reference binary's `idx_5/seg_0/chunk_0.idx`.
    #[test]
    fn matches_the_reference_chunk_order() {
        let mut triples: Vec<(i32, u32, f32)> = Vec::new();
        for doc in 0..16u32 {
            triples.push((0, doc, 0.5));
            triples.push(((doc % 5 + 1) as i32, doc, 1.0));
        }
        sort_by(&mut triples, |a, b| a.0 < b.0);
        let rows: Vec<u32> = triples
            .iter()
            .filter(|(term, _, _)| *term == 0)
            .map(|(_, row, _)| *row)
            .collect();
        assert_eq!(
            rows,
            vec![12, 9, 8, 10, 7, 6, 11, 5, 4, 13, 3, 14, 2, 15, 1, 0]
        );
        // The sort is still a sort: the term ids are ascending.
        assert!(triples.windows(2).all(|w| w[0].0 <= w[1].0));
    }

    #[test]
    fn sorts() {
        let mut v: Vec<u32> = vec![5, 3, 9, 1, 1, 0, 7, 2, 8, 6, 4, 3, 2, 0, 9, 9, 5];
        sort_by(&mut v, |a, b| a < b);
        assert_eq!(v, vec![0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 9, 9]);
    }

    #[test]
    fn sorts_every_size() {
        // The threshold, the recursion and the heapsort fallback all depend on
        // the length, so walk across them.
        for n in 0..300usize {
            let mut v: Vec<u32> = (0..n as u32).map(|i| (i * 7919) % 101).collect();
            sort_by(&mut v, |a, b| a < b);
            assert!(v.windows(2).all(|w| w[0] <= w[1]), "n = {n}");
        }
    }

    #[test]
    fn sorts_adversarial_input() {
        // A median-of-3 killer: it exhausts introsort's depth limit, so the
        // heapsort fallback runs.
        let mut v: Vec<u32> = Vec::new();
        let mut k = 1u32;
        while v.len() < 300 {
            v.push(k);
            v.push(300 - k);
            k += 1;
        }
        let expect = {
            let mut e = v.clone();
            e.sort_unstable();
            e
        };
        sort_by(&mut v, |a, b| a < b);
        assert_eq!(v, expect);
    }
}

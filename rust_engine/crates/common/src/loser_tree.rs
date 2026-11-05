//! Loser tree for k-way merges.
//!
//! C++: `LoserTreeBase` / `LoserTree`
//! (`src/storage/invertedindex/common/loser_tree.cppm`, derived from
//! tlx/container/loser_tree.hpp). A tournament tree over `ik` sorted runs:
//! `init` computes the winner once, then `delete_top_insert` replaces the
//! current minimum in O(log k).

/// C++: `round_up_to_power_of_two` (`LoserTreeBase`).
pub fn round_up_to_power_of_two(n: u32) -> u32 {
    if n == 0 {
        return 0;
    }
    n.next_power_of_two()
}

/// C++: `LoserTreeBase::Loser`.
#[derive(Debug, Clone)]
struct Loser<T> {
    /// A virtual maximum sentinel: this source has no current key.
    sup: bool,
    source: u32,
    key: T,
}

/// C++: `LoserTreeBase` + `LoserTree`.
///
/// `cmp(a, b)` is the reference's strict comparator (`std::less` for a
/// min-merge). Sources without a key are inserted with `sup = true`.
#[derive(Debug, Clone)]
pub struct LoserTree<T, F>
where
    T: Clone + Default,
    F: Fn(&T, &T) -> bool,
{
    /// The requested number of sources (`ik_` in the reference; not used by
    /// the tournament itself).
    _ik: u32,
    k: u32,
    losers: Vec<Loser<T>>,
    cmp: F,
    first_insert: bool,
}

impl<T, F> LoserTree<T, F>
where
    T: Clone + Default,
    F: Fn(&T, &T) -> bool,
{
    /// C++: `LoserTreeBase::invalid_`.
    pub const INVALID: u32 = u32::MAX;

    /// C++: `LoserTreeBase::LoserTreeBase`.
    pub fn new(k: u32, cmp: F) -> Self {
        let rounded = Self::round_up_to_power_of_two(k);
        Self {
            _ik: k,
            k: rounded,
            losers: vec![
                Loser {
                    sup: true,
                    source: Self::INVALID,
                    key: T::default(),
                };
                2 * rounded as usize
            ],
            cmp,
            first_insert: true,
        }
    }

    /// C++: `round_up_to_power_of_two`.
    pub fn round_up_to_power_of_two(n: u32) -> u32 {
        round_up_to_power_of_two(n)
    }

    /// C++: `InsertStart`.
    pub fn insert_start(&mut self, key: Option<&T>, source: u32, sup: bool) {
        let pos = (self.k + source) as usize;
        self.losers[pos].source = source;
        self.losers[pos].sup = sup;
        if self.first_insert {
            let value = key.cloned().unwrap_or_default();
            for loser in &mut self.losers {
                loser.key = value.clone();
            }
            self.first_insert = false;
        } else {
            self.losers[pos].key = key.cloned().unwrap_or_default();
        }
    }

    /// C++: `InitWinner`.
    fn init_winner(&mut self, root: u32) -> u32 {
        if root >= self.k {
            return root;
        }
        let left = self.init_winner(2 * root);
        let right = self.init_winner(2 * root + 1);
        let right_loser = self.losers[right as usize].clone();
        let left_loser = self.losers[left as usize].clone();
        if right_loser.sup || (!left_loser.sup && !(self.cmp)(&right_loser.key, &left_loser.key)) {
            self.losers[root as usize] = right_loser;
            left
        } else {
            self.losers[root as usize] = left_loser;
            right
        }
    }

    /// C++: `Init`.
    pub fn init(&mut self) {
        if self.k == 0 {
            return;
        }
        let winner = self.init_winner(1);
        self.losers[0] = self.losers[winner as usize].clone();
    }

    /// C++: `TopSource` (`None` for `invalid_`).
    pub fn top_source(&self) -> Option<u32> {
        let source = self.losers[0].source;
        (source != Self::INVALID).then_some(source)
    }

    /// C++: `TopKey`.
    pub fn top_key(&self) -> T {
        self.losers[0].key.clone()
    }

    /// C++: `DeleteTopInsert`.
    pub fn delete_top_insert(&mut self, key: Option<&T>, sup: bool) {
        assert_eq!(sup, key.is_none(), "sup must match the missing key");
        let mut source = self.losers[0].source;
        let mut key = key.cloned().unwrap_or_default();
        let mut sup = sup;
        let mut pos = (self.k + source) / 2;
        while pos > 0 {
            let loser = &mut self.losers[pos as usize];
            if sup {
                std::mem::swap(&mut loser.sup, &mut sup);
                std::mem::swap(&mut loser.source, &mut source);
                std::mem::swap(&mut loser.key, &mut key);
            } else if loser.sup {
                // The incumbent is a sentinel: keep it.
            } else if (self.cmp)(&loser.key, &key) {
                std::mem::swap(&mut loser.source, &mut source);
                std::mem::swap(&mut loser.key, &mut key);
            }
            pos /= 2;
        }
        self.losers[0].source = if sup { Self::INVALID } else { source };
        self.losers[0].sup = sup;
        self.losers[0].key = key;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct Rng(u64);
    impl Rng {
        fn next(&mut self) -> u64 {
            self.0 ^= self.0 << 13;
            self.0 ^= self.0 >> 7;
            self.0 ^= self.0 << 17;
            self.0
        }
    }

    /// C++: `LoserTreeTest::MultiWayMerge`.
    fn multi_way_merge(num_size: usize, loser_num: usize, max_val: u64) {
        let mut rng = Rng(0x2545_F491_4F6C_DD1D);
        let mut numbers = Vec::with_capacity(num_size);
        let mut loser: Vec<Vec<u64>> = vec![Vec::new(); loser_num];
        for _ in 0..num_size {
            let val = rng.next() % max_val;
            numbers.push(val);
            let source = (rng.next() % loser_num as u64) as usize;
            loser[source].push(val);
        }
        for run in &mut loser {
            run.sort_unstable();
        }
        let mut pos = vec![0usize; loser_num];
        let mut tree: LoserTree<u64, _> =
            LoserTree::new(loser_num as u32, |a: &u64, b: &u64| a < b);
        for (source, run) in loser.iter().enumerate() {
            if let Some(key) = run.first() {
                tree.insert_start(Some(key), source as u32, false);
            } else {
                tree.insert_start(None, source as u32, true);
            }
            pos[source] += 1;
        }
        tree.init();
        let mut merged = Vec::with_capacity(num_size);
        while let Some(source) = tree.top_source() {
            let key = tree.top_key();
            merged.push(key);
            let source = source as usize;
            let at = pos[source];
            if at < loser[source].len() {
                tree.delete_top_insert(Some(&loser[source][at]), false);
            } else {
                tree.delete_top_insert(None, true);
            }
            pos[source] += 1;
        }
        numbers.sort_unstable();
        assert_eq!(merged, numbers);
    }

    /// C++: `LoserTreeTest.BasicMerge1`.
    #[test]
    fn basic_merge_1() {
        multi_way_merge(10, 3, 30);
    }

    /// C++: `LoserTreeTest.BasicMerge2`.
    #[test]
    fn basic_merge_2() {
        multi_way_merge(10, 12, 30);
    }

    /// C++: `LoserTreeTest.BasicMerge3`.
    #[test]
    fn basic_merge_3() {
        multi_way_merge(1000, 200, 10_000);
    }

    /// C++: `LoserTreeTest.BasicMerge4`.
    #[test]
    fn basic_merge_4() {
        multi_way_merge(1000, 2000, 10_000);
    }

    #[test]
    fn round_up_matches_the_reference() {
        assert_eq!(round_up_to_power_of_two(3), 4);
        assert_eq!(round_up_to_power_of_two(1), 1);
        assert_eq!(round_up_to_power_of_two(5), 8);
    }
}

//! The reference's BM25 ranker and block-max WAND iterator, ported as classes
//! so the retrieval control flow stays 1:1 with C++ (plan §3.6):
//!
//! * `BM25Ranker` — `src/storage/invertedindex/search/bm25_ranker_impl.cpp`
//! * `BlockMaxWandIterator` — `src/storage/invertedindex/search/blockmax_wand_iterator_impl.cpp`
//!
//! The iterator owns the cursors, the current top-k and the pruning threshold;
//! `run()` is the reference's `Next()` loop driven to exhaustion.

use std::collections::HashSet;

use super::MergedIndex;

/// `BM25Ranker` (`bm25_ranker_impl.cpp:25-34`): k1 = 1.2, b = 0.75,
/// `idf = ln(1 + (N - df + 0.5) / (df + 0.5))`, plus the optional `delta_term`
/// added to the tf norm.
pub(crate) struct Bm25Ranker {
    pub k1: f64,
    pub b: f64,
    pub delta: f64,
}

impl Bm25Ranker {
    pub fn new(delta: f64) -> Self {
        Self {
            k1: 1.2,
            b: 0.75,
            delta,
        }
    }

    pub fn idf(&self, n_docs: f64, df: f64) -> f64 {
        (1.0 + (n_docs - df + 0.5) / (df + 0.5)).ln()
    }

    /// `idf * (tf_norm + delta) * weight`, with
    /// `tf_norm = (k1+1)*tf / (tf + k1*((1-b) + b*dl/avgdl))`.
    pub fn score(&self, tf: f64, dl: f64, avgdl: f64, idf: f64, weight: f64) -> f64 {
        let norm = (self.k1 + 1.0) * tf
            / (tf + self.k1 * ((1.0 - self.b) + self.b * dl / avgdl.max(1e-9)));
        idf * (norm + self.delta) * weight
    }

    /// `tf_norm <= k1+1`, so this is a safe upper bound for pruning.
    pub fn upper_bound(&self, idf: f64, weight: f64) -> f64 {
        idf * (self.k1 + 1.0 + self.delta.max(0.0)) * weight
    }
}

/// `similarity=boolean` scores one point per matched term (per field); the
/// default is BM25. Both are handled inside the same iterator.
#[derive(Clone, Copy, PartialEq, Eq)]
pub(crate) enum WandMode {
    Bm25,
    Boolean,
}

/// One `(field, term)` posting cursor: `(field, term, upper bound, position)`.
struct Cursor {
    field: usize,
    term: usize,
    upper_bound: f64,
    pos: usize,
}

/// `BlockMaxWandIterator` over the merged per-field postings. Terms are
/// single-unit OR terms (no phrase / require-all).
pub(crate) struct BlockMaxWandIterator<'a> {
    fields: &'a [(MergedIndex, Vec<f64>)],
    terms: &'a [String],
    dead: &'a HashSet<u64>,
    k: usize,
    min_should: usize,
    mode: WandMode,
    ranker: Bm25Ranker,
    cursors: Vec<Cursor>,
    /// Top-k, sorted by score (desc) then doc id (asc), like the scan path.
    best: Vec<(u64, f64)>,
    threshold: f64,
}

impl<'a> BlockMaxWandIterator<'a> {
    pub fn new(
        fields: &'a [(MergedIndex, Vec<f64>)],
        terms: &'a [String],
        k: usize,
        dead: &'a HashSet<u64>,
        delta: f64,
        min_should: usize,
        mode: WandMode,
    ) -> Self {
        let mut cursors = Vec::new();
        for (fi, (index, weights)) in fields.iter().enumerate() {
            let n = index.n_docs.max(1) as f64;
            for (ti, term) in terms.iter().enumerate() {
                let Some(list) = index.postings.get(term) else {
                    continue;
                };
                if list.is_empty() {
                    continue;
                }
                let idf = Bm25Ranker::new(delta).idf(n, list.len() as f64);
                let w = weights.get(ti).copied().unwrap_or(1.0);
                let upper_bound = match mode {
                    WandMode::Bm25 => Bm25Ranker::new(delta).upper_bound(idf, w),
                    WandMode::Boolean => w,
                };
                cursors.push(Cursor {
                    field: fi,
                    term: ti,
                    upper_bound,
                    pos: 0,
                });
            }
        }
        Self {
            fields,
            terms,
            dead,
            k,
            min_should,
            mode,
            ranker: Bm25Ranker::new(delta),
            cursors,
            best: Vec::new(),
            threshold: f64::NEG_INFINITY,
        }
    }

    fn doc_at(&self, cursor: usize) -> u64 {
        let c = &self.cursors[cursor];
        self.fields[c.field].0.postings[&self.terms[c.term]][c.pos].0
    }

    /// Drives the reference's `Next()` loop until the cursors are exhausted or
    /// the remaining upper bound cannot beat the k-th score.
    pub fn run(mut self) -> Vec<(u64, f64)> {
        if self.k == 0 || self.fields.is_empty() || self.cursors.is_empty() {
            return Vec::new();
        }
        loop {
            let mut alive: Vec<usize> = (0..self.cursors.len()).collect();
            alive.retain(|&c| {
                let cur = &self.cursors[c];
                cur.pos < self.fields[cur.field].0.postings[&self.terms[cur.term]].len()
            });
            if alive.is_empty() {
                break;
            }
            alive.sort_by_key(|&c| self.doc_at(c));
            let total_ub: f64 = alive.iter().map(|&c| self.cursors[c].upper_bound).sum();
            // `<` (not `<=`): a document that ties the k-th score can still win
            // on the smaller doc id.
            if total_ub < self.threshold {
                break;
            }
            // pivot = first prefix whose upper bound reaches the threshold
            let mut pivot = alive.len();
            let mut prefix = 0.0;
            for (pos, &c) in alive.iter().enumerate() {
                prefix += self.cursors[c].upper_bound;
                if prefix >= self.threshold {
                    pivot = pos;
                    break;
                }
            }
            if pivot >= alive.len() {
                break;
            }
            let d = self.doc_at(alive[pivot]);
            while pivot + 1 < alive.len() && self.doc_at(alive[pivot + 1]) == d {
                pivot += 1;
            }
            for &c in alive.iter().take(pivot + 1) {
                let cur = &self.cursors[c];
                let list = &self.fields[cur.field].0.postings[&self.terms[cur.term]];
                let mut j = cur.pos;
                while j < list.len() && list[j].0 < d {
                    j += 1;
                }
                self.cursors[c].pos = j;
            }
            let at_d: Vec<usize> = (0..=pivot)
                .map(|pos| alive[pos])
                .filter(|&c| {
                    let cur = &self.cursors[c];
                    self.fields[cur.field].0.postings[&self.terms[cur.term]]
                        .get(cur.pos)
                        .map(|p| p.0)
                        == Some(d)
                })
                .collect();
            // block-max bound on the prefix, then the exact score at `d`
            let block_max: f64 = at_d.iter().map(|&c| self.cursors[c].upper_bound).sum();
            let worth = if self.best.len() < self.k {
                true
            } else {
                match self.best.last() {
                    Some((worst, wscore)) => {
                        block_max >= *wscore && !(block_max == *wscore && d > *worst)
                    }
                    None => true,
                }
            };
            if !at_d.is_empty() && worth {
                let score = match self.mode {
                    WandMode::Bm25 => {
                        let mut score = 0.0;
                        for &c in &at_d {
                            let cur = &self.cursors[c];
                            let (index, weights) = &self.fields[cur.field];
                            let n = index.n_docs.max(1) as f64;
                            let avg = (index.total_len as f64 / n).max(1e-9);
                            let post = &index.postings[&self.terms[cur.term]][cur.pos];
                            let tf = post.1 as f64;
                            let dl = *index.doc_len.get(&post.0).unwrap_or(&1) as f64;
                            let df = index.postings[&self.terms[cur.term]].len() as f64;
                            let idf = self.ranker.idf(n, df);
                            let w = weights.get(cur.term).copied().unwrap_or(1.0);
                            score += self.ranker.score(tf, dl, avg, idf, w);
                        }
                        score
                    }
                    // `similarity=boolean`: one point per matched term.
                    WandMode::Boolean => at_d
                        .iter()
                        .map(|&c| {
                            let cur = &self.cursors[c];
                            self.fields[cur.field]
                                .1
                                .get(cur.term)
                                .copied()
                                .unwrap_or(1.0)
                        })
                        .sum(),
                };
                // `minimum_should_match`: BM25 counts distinct query units
                // across fields; boolean counts every (field, term) match.
                let matched = match self.mode {
                    WandMode::Bm25 => {
                        let mut seen = HashSet::new();
                        for &c in &at_d {
                            seen.insert(self.cursors[c].term);
                        }
                        seen.len()
                    }
                    WandMode::Boolean => at_d.len(),
                };
                let enough_terms = matched >= self.min_should.max(1);
                let better = if self.best.len() < self.k {
                    true
                } else {
                    match self.best.last() {
                        Some((worst, wscore)) => {
                            score > *wscore || (score == *wscore && d < *worst)
                        }
                        None => true,
                    }
                };
                if enough_terms && !self.dead.contains(&d) && better {
                    self.best.push((d, score));
                    self.best.sort_by(|a, b| {
                        b.1.partial_cmp(&a.1)
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.0.cmp(&b.0))
                    });
                    self.best.truncate(self.k);
                    if self.best.len() == self.k {
                        self.threshold = self.best[self.k - 1].1;
                    }
                }
            }
            for &c in alive.iter().take(pivot + 1) {
                let cur = &self.cursors[c];
                if self.fields[cur.field].0.postings[&self.terms[cur.term]]
                    .get(cur.pos)
                    .map(|p| p.0)
                    == Some(d)
                {
                    self.cursors[c].pos += 1;
                }
            }
        }
        self.best
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;

    fn index(postings: &[(&str, &[(u64, u32)])], doc_len: &[(u64, u32)], n: usize) -> MergedIndex {
        let mut map = HashMap::new();
        for (term, list) in postings {
            map.insert(term.to_string(), list.to_vec());
        }
        MergedIndex {
            n_docs: n,
            total_len: doc_len.iter().map(|(_, l)| *l as u64).sum(),
            postings: map,
            doc_len: doc_len.iter().copied().collect(),
        }
    }

    #[test]
    fn ranker_matches_the_reference_formula() {
        let r = Bm25Ranker::new(0.0);
        let idf = r.idf(4.0, 3.0);
        assert!((idf - (1.0f64 + 1.5 / 3.5).ln()).abs() < 1e-12);
        // tf=1, dl=2, avg=2.25
        let expected = idf * (2.2 * 1.0 / (1.0 + 1.2 * (0.25 + 0.75 * 2.0 / 2.25)));
        assert!((r.score(1.0, 2.0, 2.25, idf, 1.0) - expected).abs() < 1e-12);
        // delta adds to the norm
        let rd = Bm25Ranker::new(1.0);
        assert!((rd.score(1.0, 2.0, 2.25, idf, 1.0) - (expected + idf)).abs() < 1e-12);
    }

    #[test]
    fn wand_matches_brute_force() {
        // Two terms over four docs; BM25 ranking by hand-checkable scores.
        let idx = index(
            &[
                ("alpha", &[(0, 1), (1, 1), (3, 2)]),
                ("beta", &[(2, 1), (3, 1)]),
            ],
            &[(0, 2), (1, 2), (2, 2), (3, 3)],
            4,
        );
        let terms = vec!["alpha".to_string(), "beta".to_string()];
        let weights = vec![1.0, 1.0];
        let fields = vec![(idx, weights.clone())];
        let dead = HashSet::new();
        let got =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 1, WandMode::Bm25).run();
        assert_eq!(got.len(), 4);
        assert_eq!(got[0].0, 3); // both terms + length boost
        assert_eq!(got[1].0, 2); // only "beta" (higher idf than "alpha")

        let boolean =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 1, WandMode::Boolean).run();
        assert_eq!(boolean[0], (3, 2.0));
        assert_eq!(boolean[1].1, 1.0);
        // minimum_should_match=2 keeps only the doc with both terms
        let both =
            BlockMaxWandIterator::new(&fields, &terms, 4, &dead, 0.0, 2, WandMode::Bm25).run();
        assert_eq!(both.len(), 1);
        assert_eq!(both[0].0, 3);
    }
}

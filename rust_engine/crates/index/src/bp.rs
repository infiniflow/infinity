//! Recursive graph-bisection document reordering for sparse indexes.
//!
//! C++: `BPReordering` (`src/storage/knn_index/sparse/bp_reordering.cppm`,
//! after PISA's recursive graph bisection). Documents are reordered so each
//! term's postings become more local, which improves posting-list compression.
//! The reference parallelizes the gain computation; this port keeps the same
//! deterministic computation single-threaded (the threading only changes the
//! wall clock, not the result).

/// C++: `BPReorderContext`.
#[derive(Debug, Clone)]
struct BpReorderContext {
    ldegs: Vec<i32>,
    rdegs: Vec<i32>,
    term_gains_to_right: Vec<f32>,
    term_gains_to_left: Vec<f32>,
    term_gains_to_right_valid: Vec<bool>,
    term_gains_to_left_valid: Vec<bool>,
    gains: Vec<f32>,
    gains_permu: Vec<usize>,
}

impl BpReorderContext {
    fn new(term_num: usize, doc_num: usize) -> Self {
        Self {
            ldegs: vec![0; term_num],
            rdegs: vec![0; term_num],
            term_gains_to_right: vec![0.0; term_num],
            term_gains_to_left: vec![0.0; term_num],
            term_gains_to_right_valid: vec![false; term_num],
            term_gains_to_left_valid: vec![false; term_num],
            gains: vec![0.0; doc_num],
            gains_permu: vec![0; doc_num],
        }
    }
}

/// C++: `BPReordering<IdxType, DocID>` with `DocID = u32`.
#[derive(Debug, Clone)]
pub struct BpReordering {
    fwd: Vec<Vec<u32>>,
    query_n: i32,
    terminate_length: i32,
    iter_n: i32,
}

impl BpReordering {
    /// C++: `BPReordering(i32 query_n)`.
    pub fn new(query_n: i32) -> Self {
        Self {
            fwd: Vec::new(),
            query_n,
            terminate_length: 32,
            iter_n: 20,
        }
    }

    pub fn set_terminate_length(&mut self, terminate_length: i32) {
        self.terminate_length = terminate_length;
    }

    pub fn set_iter_n(&mut self, iter_n: i32) {
        self.iter_n = iter_n;
    }

    /// C++: `AddDoc(terms)`.
    pub fn add_doc(&mut self, terms: &[u32]) {
        self.fwd.push(terms.to_vec());
    }

    /// C++: `operator()`.
    pub fn reorder(&self) -> Vec<u32> {
        let data_n = self.fwd.len();
        let mut permutation: Vec<u32> = (0..data_n as u32).collect();
        let mut ctx = BpReorderContext::new(self.query_n.max(0) as usize, data_n);
        self.work(0, data_n, &mut permutation, &mut ctx);
        permutation
    }

    /// C++: `Work`.
    fn work(&self, start: usize, end: usize, permutation: &mut [u32], ctx: &mut BpReorderContext) {
        permutation[start..end].sort_unstable();
        if end - start <= self.terminate_length.max(0) as usize {
            return;
        }
        let mid = (start + end) / 2;
        self.bisection(start, mid, end, ctx, permutation);
        self.work(start, mid, permutation, ctx);
        self.work(mid, end, permutation, ctx);
    }

    /// C++: `Bisection`.
    fn bisection(
        &self,
        start: usize,
        mid: usize,
        end: usize,
        ctx: &mut BpReorderContext,
        permutation: &mut [u32],
    ) {
        ctx.ldegs.fill(0);
        ctx.rdegs.fill(0);
        ctx.term_gains_to_right_valid.fill(false);
        ctx.term_gains_to_left_valid.fill(false);

        for (i, &doc) in permutation.iter().enumerate().take(end).skip(start) {
            let doc = doc as usize;
            for &term in &self.fwd[doc] {
                if i < mid {
                    ctx.ldegs[term as usize] += 1;
                } else {
                    ctx.rdegs[term as usize] += 1;
                }
            }
        }

        for _ in 0..self.iter_n.max(0) {
            let mut swapped = false;
            self.compute_gains(start, mid, end, ctx, permutation);
            for (offset, slot) in ctx.gains_permu[start..end].iter_mut().enumerate() {
                *slot = start + offset;
            }
            ctx.gains_permu[start..mid].sort_by(|&i, &j| ctx.gains[j].total_cmp(&ctx.gains[i]));
            ctx.gains_permu[mid..end].sort_by(|&i, &j| ctx.gains[j].total_cmp(&ctx.gains[i]));

            let mut i = start;
            let mut j = mid;
            while i < mid {
                let li = ctx.gains_permu[i];
                let ri = ctx.gains_permu[j];
                if ctx.gains[li] + ctx.gains[ri] <= 0.0 {
                    break;
                }
                swapped = true;
                for &term in &self.fwd[permutation[li] as usize] {
                    ctx.ldegs[term as usize] -= 1;
                    ctx.rdegs[term as usize] += 1;
                    ctx.term_gains_to_right_valid[term as usize] = false;
                    ctx.term_gains_to_left_valid[term as usize] = false;
                }
                for &term in &self.fwd[permutation[ri] as usize] {
                    ctx.rdegs[term as usize] -= 1;
                    ctx.ldegs[term as usize] += 1;
                    ctx.term_gains_to_right_valid[term as usize] = false;
                    ctx.term_gains_to_left_valid[term as usize] = false;
                }
                permutation.swap(li, ri);
                i += 1;
                j += 1;
            }
            if !swapped {
                break;
            }
        }
    }

    /// C++: `ComputeGains`.
    fn compute_gains(
        &self,
        start: usize,
        mid: usize,
        end: usize,
        ctx: &mut BpReorderContext,
        permutation: &[u32],
    ) {
        let left_log_n = (mid - start).ilog2() as f32;
        let right_log_n = (end - mid).ilog2() as f32;
        for (i, &doc) in permutation.iter().enumerate().take(end).skip(start) {
            if i < mid {
                ctx.gains[i] = self.compute_gain(doc, left_log_n, right_log_n, true, ctx);
            } else {
                ctx.gains[i] = self.compute_gain(doc, right_log_n, left_log_n, false, ctx);
            }
        }
    }

    /// C++: `ComputeGain`.
    fn compute_gain(
        &self,
        doc_id: u32,
        from_log_n: f32,
        to_log_n: f32,
        from_left: bool,
        ctx: &mut BpReorderContext,
    ) -> f32 {
        let mut gain = 0.0;
        for &term in &self.fwd[doc_id as usize] {
            let term = term as usize;
            let (deg1, deg2) = if from_left {
                (ctx.ldegs[term], ctx.rdegs[term])
            } else {
                (ctx.rdegs[term], ctx.ldegs[term])
            };
            let (term_gains, valid) = if from_left {
                (
                    &mut ctx.term_gains_to_right,
                    &mut ctx.term_gains_to_right_valid,
                )
            } else {
                (
                    &mut ctx.term_gains_to_left,
                    &mut ctx.term_gains_to_left_valid,
                )
            };
            if !valid[term] {
                term_gains[term] = Self::compute_term_gain(deg1, from_log_n, deg2, to_log_n);
                valid[term] = true;
            }
            gain += term_gains[term];
        }
        gain
    }

    /// C++: `ComputeTermGain`.
    fn compute_term_gain(deg1: i32, log_n1: f32, deg2: i32, log_n2: f32) -> f32 {
        Self::compute_term_cost(deg1, log_n1, deg2, log_n2)
            - Self::compute_term_cost(deg1 - 1, log_n1, deg2 + 1, log_n2)
    }

    /// C++: `ComputeTermCost`.
    fn compute_term_cost(deg1: i32, log_n1: f32, deg2: i32, log_n2: f32) -> f32 {
        deg1 as f32 * log_n1 - deg1 as f32 * ((deg1 + 1) as f32).log2() + deg2 as f32 * log_n2
            - deg2 as f32 * ((deg2 + 1) as f32).log2()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;

    struct Rng(u64);
    impl Rng {
        fn next(&mut self) -> u64 {
            self.0 ^= self.0 << 13;
            self.0 ^= self.0 >> 7;
            self.0 ^= self.0 << 17;
            self.0
        }
    }

    /// C++: `cost_func` (posting gaps in log2).
    fn cost_func(fwd: &[Vec<u32>], query_n: usize, permutation: Option<&[u32]>) -> i64 {
        let mut last = vec![-1i32; query_n];
        let mut cost = 0i64;
        let n = permutation.map(|p| p.len()).unwrap_or(fwd.len());
        for i in 0..n {
            let doc = permutation.map(|p| p[i] as usize).unwrap_or(i);
            for &term in &fwd[doc] {
                let term = term as usize;
                if last[term] != -1 {
                    cost += (i as i32 - last[term]).ilog2() as i64;
                }
                last[term] = i as i32;
            }
        }
        cost
    }

    /// C++: `GenerateFwd` (random boundaries, unique terms per row).
    fn generate_fwd(data_n: usize, query_n: usize, edge_n: usize) -> Vec<Vec<u32>> {
        let mut rng = Rng(0x2545_F491_4F6C_DD1D);
        let mut indices: Vec<usize> = (0..data_n.saturating_sub(1))
            .map(|_| (rng.next() % (edge_n as u64 + 1)) as usize)
            .collect();
        indices.push(0);
        indices.push(edge_n);
        indices.sort_unstable();
        let mut fwd = Vec::with_capacity(data_n);
        for i in 0..data_n {
            let count = indices[i + 1].saturating_sub(indices[i]).min(query_n);
            let mut seen = HashSet::new();
            let mut posting = Vec::with_capacity(count);
            while posting.len() < count {
                let term = (rng.next() % query_n as u64) as u32;
                if seen.insert(term) {
                    posting.push(term);
                }
            }
            fwd.push(posting);
        }
        fwd
    }

    /// C++: `BPReorderingTest.test1`.
    #[test]
    fn reordering_does_not_increase_cost() {
        let data_n = 1000;
        let query_n = 1000;
        let edge_n = data_n * query_n / 10;
        let fwd = generate_fwd(data_n, query_n, edge_n);
        let mut bp = BpReordering::new(query_n as i32);
        bp.set_terminate_length(4);
        for doc in &fwd {
            bp.add_doc(doc);
        }
        let reorder = bp.reorder();
        let old_cost = cost_func(&fwd, query_n, None);
        let new_cost = cost_func(&fwd, query_n, Some(&reorder));
        assert!(new_cost <= old_cost, "{new_cost} > {old_cost}");
    }

    /// C++: `BPReorderingTest.test2` — skipped when the reference's CSR
    /// fixture is absent, like the C++ test.
    #[test]
    fn reordering_on_the_small_csr_when_present() {
        let path = std::path::Path::new("test/data/benchmark/splade/base_small.csr");
        let Ok(bytes) = std::fs::read(path) else {
            return;
        };
        // `SparseMatrix::Load`: i64 nrow, i64 ncol, i64 nnz, i64 indptr[],
        // i32 indices[], f32 data[].
        let i64_at = |at: usize| i64::from_le_bytes(bytes[at..at + 8].try_into().unwrap()) as usize;
        let nrow = i64_at(0);
        let ncol = i64_at(8);
        let nnz = i64_at(16);
        let indptr_at = 24;
        let indices_at = indptr_at + (nrow + 1) * 8;
        let data_at = indices_at + nnz * 4;
        assert!(bytes.len() >= data_at + nnz * 4);
        let mut fwd = Vec::with_capacity(nrow);
        for row in 0..nrow {
            let start = i64_at(indptr_at + row * 8);
            let end = i64_at(indptr_at + (row + 1) * 8);
            let mut posting = Vec::with_capacity(end - start);
            for i in start..end {
                posting.push(u32::from_le_bytes(
                    bytes[indices_at + i * 4..indices_at + i * 4 + 4]
                        .try_into()
                        .unwrap(),
                ));
            }
            fwd.push(posting);
        }
        let _ = data_at;
        let old_cost = cost_func(&fwd, ncol, None);
        let mut bp = BpReordering::new(ncol as i32);
        for doc in &fwd {
            bp.add_doc(doc);
        }
        let reorder = bp.reorder();
        let new_cost = cost_func(&fwd, ncol, Some(&reorder));
        assert!(new_cost <= old_cost, "{new_cost} > {old_cost}");
    }
}

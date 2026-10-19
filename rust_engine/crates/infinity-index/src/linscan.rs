//! Linear-scan sparse index.
//!
//! C++: `LinScan` (`src/storage/knn_index/sparse/linscan_alg.cppm`). An
//! inverted list per term; `SearchBF` scores every matching document, and
//! `SearchKnn` reads the query's heaviest terms until a posting budget is
//! exhausted (the caller re-ranks the candidates).

use std::collections::HashMap;

use infinity_common::{Error, Result};

/// C++: `Posting<DataType>`.
#[derive(Debug, Clone, Copy, PartialEq)]
struct Posting {
    doc_id: u32,
    val: f32,
}

/// C++: `LinScan<f32, i32>`.
#[derive(Debug, Clone, Default)]
pub struct LinScan {
    inverted: Vec<Vec<Posting>>,
    row_num: u32,
}

impl LinScan {
    /// C++: `LinScan(size_t max_col)`.
    pub fn new(max_col: usize) -> Self {
        Self {
            inverted: vec![Vec::new(); max_col],
            row_num: 0,
        }
    }

    /// C++: `Insert(vec, doc_id)`.
    pub fn insert(&mut self, vec: &[(u32, f32)], doc_id: u32) -> Result<()> {
        for &(index, value) in vec {
            if value == 0.0 {
                continue;
            }
            let list = self
                .inverted
                .get_mut(index as usize)
                .ok_or_else(|| Error::Invalid(format!("sparse index {index} out of range")))?;
            list.push(Posting { doc_id, val: value });
        }
        self.row_num += 1;
        Ok(())
    }

    pub fn row_num(&self) -> u32 {
        self.row_num
    }

    /// C++: `SearchBF(query, top_k)` — exact inner-product top-k (negative
    /// scores are dropped).
    pub fn search_bf(&self, query: &[(u32, f32)], top_k: usize) -> (Vec<u32>, Vec<f32>) {
        let mut scores: HashMap<u32, f32> = HashMap::new();
        for &(index, value) in query {
            let Some(list) = self.inverted.get(index as usize) else {
                continue;
            };
            for posting in list {
                *scores.entry(posting.doc_id).or_insert(0.0) += value * posting.val;
            }
        }
        let mut hits: Vec<(u32, f32)> = scores
            .into_iter()
            .filter(|(_, score)| *score >= 0.0)
            .collect();
        hits.sort_by(|a, b| b.1.total_cmp(&a.1).then(a.0.cmp(&b.0)));
        hits.truncate(top_k.min(self.row_num as usize));
        (
            hits.iter().map(|(id, _)| *id).collect(),
            hits.iter().map(|(_, score)| *score).collect(),
        )
    }

    /// C++: `SearchKnn(query, top_k, budget)` — candidates in posting order
    /// (the caller re-ranks), plus the postings actually read.
    pub fn search_knn(
        &self,
        query: &[(u32, f32)],
        top_k: usize,
        budget: i64,
    ) -> (Vec<u32>, Vec<f32>, i64) {
        if budget <= 0 {
            return (Vec::new(), Vec::new(), 0);
        }
        let mut order: Vec<usize> = (0..query.len()).collect();
        order.sort_by(|&a, &b| {
            query[b]
                .1
                .abs()
                .total_cmp(&query[a].1.abs())
                .then(a.cmp(&b))
        });
        let mut scores: HashMap<u32, f32> = HashMap::new();
        let mut used = 0i64;
        for &at in &order {
            if used >= budget {
                break;
            }
            let (index, value) = query[at];
            let Some(list) = self.inverted.get(index as usize) else {
                continue;
            };
            used += list.len() as i64;
            for posting in list {
                *scores.entry(posting.doc_id).or_insert(0.0) += value * posting.val;
            }
        }
        let mut hits: Vec<(u32, f32)> = scores
            .into_iter()
            .filter(|(_, score)| *score >= 0.0)
            .collect();
        // `HeapResultHandler` keeps the best `top_k`; `EndWithoutSort` only
        // skips the final ordering, so select by score before truncating.
        hits.sort_by(|a, b| b.1.total_cmp(&a.1).then(a.0.cmp(&b.0)));
        hits.truncate(top_k.min(self.row_num as usize));
        (
            hits.iter().map(|(id, _)| *id).collect(),
            hits.iter().map(|(_, score)| *score).collect(),
            used,
        )
    }

    /// C++: `LinScan::Save`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = Vec::new();
        out.extend_from_slice(&self.row_num.to_le_bytes());
        out.extend_from_slice(&(self.inverted.len() as u64).to_le_bytes());
        for (index, list) in self.inverted.iter().enumerate() {
            out.extend_from_slice(&(index as u32).to_le_bytes());
            out.extend_from_slice(&(list.len() as u64).to_le_bytes());
            for posting in list {
                out.extend_from_slice(&posting.doc_id.to_le_bytes());
                out.extend_from_slice(&posting.val.to_le_bytes());
            }
        }
        out
    }

    /// C++: `LinScan::Load` (`ReadAdv`'s duplicate/ordering checks included).
    pub fn from_bytes(bytes: &[u8]) -> Result<Self> {
        let mut at = 0usize;
        let read_u32 = |at: &mut usize| -> Result<u32> {
            let v = bytes
                .get(*at..*at + 4)
                .ok_or_else(|| Error::Invalid("LinScan: truncated".into()))?;
            *at += 4;
            Ok(u32::from_le_bytes(v.try_into().unwrap()))
        };
        let read_u64 = |at: &mut usize| -> Result<u64> {
            let v = bytes
                .get(*at..*at + 8)
                .ok_or_else(|| Error::Invalid("LinScan: truncated".into()))?;
            *at += 8;
            Ok(u64::from_le_bytes(v.try_into().unwrap()))
        };
        let row_num = read_u32(&mut at)?;
        let terms = read_u64(&mut at)? as usize;
        let mut scan = Self::new(terms);
        scan.row_num = row_num;
        for _ in 0..terms {
            let index = read_u32(&mut at)? as usize;
            let count = read_u64(&mut at)? as usize;
            let mut list = Vec::with_capacity(count);
            for j in 0..count {
                let doc_id = read_u32(&mut at)?;
                let val = f32::from_le_bytes(
                    bytes
                        .get(at..at + 4)
                        .ok_or_else(|| Error::Invalid("LinScan: truncated".into()))?
                        .try_into()
                        .unwrap(),
                );
                at += 4;
                if j > 0 && doc_id <= list.last().map(|p: &Posting| p.doc_id).unwrap_or(0) {
                    return Err(Error::Invalid(
                        "LinScan: duplicate doc_id in posting list".into(),
                    ));
                }
                list.push(Posting { doc_id, val });
            }
            if scan.inverted[index].is_empty() {
                scan.inverted[index] = list;
            } else if !list.is_empty() {
                return Err(Error::Invalid(
                    "LinScan: duplicate term in inverted index".into(),
                ));
            }
        }
        Ok(scan)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// C++: `SparseTestUtil::GenerateDataset` (deterministic stand-in for the
    /// reference's `std::random_device` seeding).
    fn generate_dataset(nrow: usize, ncol: usize, sparsity: f32) -> Vec<Vec<(u32, f32)>> {
        let nnz = (nrow as f32 * ncol as f32 * sparsity) as usize;
        let mut state = 0x9E37_79B9_7F4A_7C15u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        // Row boundaries: sorted random cut points, cap per row at ncol.
        let mut bounds: Vec<usize> = (0..nrow)
            .map(|_| (next() % (nnz as u64 + 1)) as usize)
            .collect();
        bounds.sort_unstable();
        bounds.push(nnz);
        let mut rows = Vec::with_capacity(nrow);
        for i in 0..nrow {
            let start = bounds[i];
            let end = bounds[i + 1].min(start + ncol);
            let mut used = std::collections::HashSet::new();
            let mut row = Vec::new();
            for j in start..end {
                let index = (next() % ncol as u64) as u32;
                if !used.insert(index) {
                    continue;
                }
                let value = ((next() % 20_000) as f32 / 1000.0) - 10.0;
                if value != 0.0 {
                    row.push((index, value));
                }
                let _ = j;
            }
            rows.push(row);
        }
        rows
    }

    fn groundtruth(
        dataset: &[Vec<(u32, f32)>],
        query: &[(u32, f32)],
        topk: usize,
    ) -> (Vec<u32>, Vec<f32>) {
        let mut hits: Vec<(u32, f32)> = dataset
            .iter()
            .enumerate()
            .map(|(row, vec)| {
                let map: std::collections::HashMap<u32, f32> = vec.iter().copied().collect();
                let score: f32 = query
                    .iter()
                    .map(|(i, qv)| map.get(i).copied().unwrap_or(0.0) * qv)
                    .sum();
                (row as u32, score)
            })
            .filter(|(_, score)| *score > 0.0)
            .collect();
        hits.sort_by(|a, b| b.1.total_cmp(&a.1).then(a.0.cmp(&b.0)));
        hits.truncate(topk);
        (
            hits.iter().map(|(id, _)| *id).collect(),
            hits.iter().map(|(_, s)| *s).collect(),
        )
    }

    /// C++: `LinScanAlgTest.accurate_scan`.
    #[test]
    fn accurate_scan() {
        let nrow = 1000;
        let ncol = 10_000;
        let dataset = generate_dataset(nrow, ncol, 0.01);
        let queries = generate_dataset(50, ncol, 0.01);
        let mut index = LinScan::new(ncol);
        for (row, vec) in dataset.iter().enumerate() {
            index.insert(vec, row as u32).unwrap();
        }
        for query in &queries {
            let (indices, scores) = index.search_bf(query, 10);
            let (gt_indices, gt_scores) = groundtruth(&dataset, query, 10);
            let n = indices.len().min(gt_indices.len());
            for i in 0..n {
                if gt_scores[i] == 0.0 {
                    break;
                }
                assert_eq!(indices[i], gt_indices[i], "rank {i}");
                assert!(
                    (scores[i] - gt_scores[i]).abs() <= 1e-6,
                    "{} vs {}",
                    scores[i],
                    gt_scores[i]
                );
            }
        }
    }

    /// C++: `LinScanAlgTest.approximate_scan`.
    #[test]
    fn approximate_scan() {
        let nrow = 1000;
        let ncol = 10_000;
        let dataset = generate_dataset(nrow, ncol, 0.01);
        let queries = generate_dataset(50, ncol, 0.01);
        let mut index = LinScan::new(ncol);
        for (row, vec) in dataset.iter().enumerate() {
            index.insert(vec, row as u32).unwrap();
        }
        let budget = (nrow as f32 * ncol as f32 * 0.01 * 0.01) as i64;
        let mut hit = 0usize;
        let mut total = 0usize;
        for query in &queries {
            let (candidates, _, _) = index.search_knn(query, 30, budget);
            let (gt_indices, gt_scores) = groundtruth(&dataset, query, 10);
            let gt_set: std::collections::HashSet<u32> = gt_indices
                .iter()
                .zip(&gt_scores)
                .take_while(|(_, s)| **s != 0.0)
                .map(|(id, _)| *id)
                .collect();
            total += gt_set.len();
            hit += candidates.iter().filter(|id| gt_set.contains(id)).count();
        }
        assert!(hit as f32 >= total as f32 * 0.9, "hit {hit} of {total}");
    }

    #[test]
    fn save_and_load_roundtrip() {
        let mut index = LinScan::new(16);
        index.insert(&[(1, 2.0), (3, -1.0)], 0).unwrap();
        index.insert(&[(1, 0.5), (7, 4.0)], 1).unwrap();
        let bytes = index.to_bytes();
        let loaded = LinScan::from_bytes(&bytes).unwrap();
        assert_eq!(loaded.row_num(), 2);
        let (ids, scores) = loaded.search_bf(&[(1, 1.0)], 2);
        assert_eq!(ids, vec![0, 1]);
        assert_eq!(scores, vec![2.0, 0.5]);
    }
}

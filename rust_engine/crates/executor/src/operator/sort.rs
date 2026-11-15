//! `PhysicalSort`: chunked runs plus a k-way merge.
//!
//! The stage pipeline sorted the whole relation with one `sort_by` over all
//! rows. This operator sorts one chunk at a time and merges the sorted runs,
//! which is the reference's shape (`PhysicalSort` materialises blocks and
//! `PhysicalMergeSort` merges them; ties go to the left/earlier run, exactly
//! like `CompareTwoRowAndPreferLeft`).
//!
//! Stability is what the ORDER BY ties depend on: each run is sorted with the
//! shared stable comparison (so rows inside a run keep their input order), and
//! the merge takes the earlier run on a full tie while the runs are the input
//! chunks in order. The merged result is therefore the same sequence a single
//! stable sort over the whole relation produces.
//!
//! A relation that fits in one chunk is sorted in place and never merged.

#[cfg(test)]
use std::cell::Cell;

use ::common::Result;
use expression::{Expr, Value};

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

#[cfg(test)]
thread_local! {
    static LAST_RUNS: Cell<usize> = const { Cell::new(0) };
}

/// How many sorted runs the last sort merged (1 when no merge was needed).
#[cfg(test)]
pub fn last_run_count() -> usize {
    LAST_RUNS.with(|runs| runs.get())
}

/// A sort operator.
#[derive(Debug, Clone)]
pub struct Sort {
    keys: Vec<(Expr, bool)>,
    /// Rows per run: the chunk the input arrives in, so a run never exceeds what
    /// one chunk holds.
    run_capacity: usize,
}

impl Sort {
    pub fn new(keys: Vec<(Expr, bool)>) -> Self {
        Self {
            keys,
            run_capacity: crate::chunk::DEFAULT_VECTOR_SIZE,
        }
    }

    /// Sorts every accumulated block, returns the merged blocks.
    fn sort_blocks(&self, blocks: Vec<DataChunk>, width: usize) -> Vec<DataChunk> {
        // One run per input chunk, sorted in place; the chunk is dropped as soon
        // as its rows have been converted.
        let mut runs: Vec<std::vec::IntoIter<Vec<Value>>> = blocks
            .into_iter()
            .filter(|block| block.row_count() > 0)
            .map(|block| {
                let mut rows = block.into_rows();
                rows.sort_by(|a, b| crate::compare_rows(a, b, &self.keys));
                rows.into_iter()
            })
            .collect();
        #[cfg(test)]
        LAST_RUNS.with(|count| count.set(runs.len()));

        if runs.is_empty() {
            let mut block = DataChunk::new(width);
            block.finalize();
            return vec![block];
        }
        if runs.len() == 1 {
            // Everything fit in one block: no merge needed.
            let rows: Vec<Vec<Value>> = runs.pop().expect("one run").collect();
            return DataChunk::from_rows(width, rows);
        }

        let mut merged: Vec<DataChunk> = Vec::new();
        let mut block = DataChunk::with_capacity(width, self.run_capacity);
        loop {
            // Pick the earliest run whose head is the smallest. Scanning the
            // heads keeps the tie rule obvious; the run count is the relation
            // size over the chunk size, so it stays small.
            let mut best: Option<usize> = None;
            for (index, run) in runs.iter().enumerate() {
                let Some(head) = run.as_slice().first() else {
                    continue;
                };
                best = match best {
                    None => Some(index),
                    Some(current) => {
                        let current_head = runs[current]
                            .as_slice()
                            .first()
                            .expect("chosen run still has a head");
                        // A tie keeps the earlier run, which preserves the
                        // global input order.
                        if crate::compare_rows(head, current_head, &self.keys)
                            == std::cmp::Ordering::Less
                        {
                            Some(index)
                        } else {
                            Some(current)
                        }
                    }
                };
            }
            let Some(index) = best else { break };
            let row = runs[index].next().expect("head exists");
            if block.is_full() {
                block.finalize();
                merged.push(std::mem::replace(
                    &mut block,
                    DataChunk::with_capacity(width, self.run_capacity),
                ));
            }
            // `push_row` cannot fail: the row comes from the same relation.
            let _ = block.push_row(&row);
        }
        if block.row_count() > 0 || merged.is_empty() {
            block.finalize();
            merged.push(block);
        }
        merged
    }
}

impl PhysicalOperator for Sort {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Sort
    }

    fn output_names(&self) -> Vec<String> {
        Vec::new()
    }

    fn execute(
        &self,
        _ctx: &mut OperatorContext,
        states: &mut OperatorStates,
        id: usize,
    ) -> Result<bool> {
        let prev_complete = states.prev_complete(id);
        let input = states.take_prev(id);
        let state = states.state_mut(id);
        let width = input
            .first()
            .map(DataChunk::column_count)
            .or_else(|| state.data_blocks.first().map(DataChunk::column_count))
            .unwrap_or(0);
        state.data_blocks.extend(input);
        if !prev_complete {
            // Keep accumulating; nothing can be emitted before the last chunk.
            return Ok(false);
        }
        let blocks = std::mem::take(&mut state.data_blocks);
        state.data_blocks = self.sort_blocks(blocks, width);
        state.set_complete();
        Ok(true)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::operator::RowsSource;

    fn key(column: usize) -> (Expr, bool) {
        (Expr::Column(column), true)
    }

    fn row(id: i64, rank: i64) -> Vec<Value> {
        vec![Value::Int(id), Value::Int(rank)]
    }

    fn run(sort: Sort, rows: Vec<Vec<Value>>, batch: usize) -> Vec<Vec<Value>> {
        let mut operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(sort)];
        for operator in &mut operators {
            operator.init().unwrap();
        }
        let mut ctx = OperatorContext {
            block_capacity: batch,
        };
        let mut states = OperatorStates::new(&operators);
        let mut source = RowsSource::new(rows, 2, batch);
        crate::operator::run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect()
    }

    #[test]
    fn sorts_across_chunk_boundaries() {
        let rows = vec![row(1, 30), row(2, 10), row(3, 50), row(4, 20), row(5, 40)];
        let out = run(Sort::new(vec![key(1)]), rows, 2);
        assert_eq!(
            out,
            vec![row(2, 10), row(4, 20), row(1, 30), row(5, 40), row(3, 50)]
        );
    }

    /// ORDER BY ties keep the input order, across the merge.
    #[test]
    fn sort_ties_keep_input_order() {
        // Ranks repeat inside a run and across runs.
        let rows = vec![row(1, 7), row(2, 7), row(3, 7), row(4, 7), row(5, 7)];
        let out = run(Sort::new(vec![key(1)]), rows, 2);
        assert_eq!(
            out,
            vec![row(1, 7), row(2, 7), row(3, 7), row(4, 7), row(5, 7)]
        );
    }

    #[test]
    fn a_single_run_is_not_merged() {
        let rows: Vec<Vec<Value>> = (0..5).map(|i| row(i, 5 - i)).collect();
        let out = run(Sort::new(vec![key(1)]), rows, 8192);
        assert_eq!(out, (0..5).map(|i| row(i, 5 - i)).rev().collect::<Vec<_>>());
        assert_eq!(last_run_count(), 1);
    }

    #[test]
    fn several_runs_are_merged() {
        let rows: Vec<Vec<Value>> = (0..10).map(|i| row(i, 10 - i)).collect();
        let out = run(Sort::new(vec![key(1)]), rows, 3);
        assert_eq!(
            out,
            (0..10).map(|i| row(i, 10 - i)).rev().collect::<Vec<_>>()
        );
        assert_eq!(last_run_count(), 4);
    }

    #[test]
    fn a_descending_key_reverses_the_order() {
        let rows = vec![row(1, 30), row(2, 10), row(3, 20)];
        let out = run(Sort::new(vec![(Expr::Column(1), false)]), rows, 2);
        assert_eq!(out, vec![row(1, 30), row(3, 20), row(2, 10)]);
    }

    #[test]
    fn nulls_sort_first_and_stay_stable() {
        let rows = vec![
            row(1, 30),
            vec![Value::Int(2), Value::Null],
            vec![Value::Int(3), Value::Null],
            row(4, 10),
        ];
        let out = run(Sort::new(vec![key(1)]), rows, 2);
        assert_eq!(out[0][0], Value::Int(2));
        assert_eq!(out[1][0], Value::Int(3));
        assert_eq!(out[2], row(4, 10));
        assert_eq!(out[3], row(1, 30));
    }

    #[test]
    fn the_merge_matches_a_single_stable_sort() {
        // Any batch size must produce the same sequence as one stable sort over
        // the whole relation.
        let rows: Vec<Vec<Value>> = (0..37).map(|i| row(i, (i * 7) % 5)).collect();
        let mut expected = rows.clone();
        crate::sort_rows(&mut expected, &[key(1)]);
        for batch in [1usize, 3, 5, 8192] {
            let out = run(Sort::new(vec![key(1)]), rows.clone(), batch);
            assert_eq!(out, expected, "batch {batch}");
        }
    }

    #[test]
    fn an_empty_input_emits_an_empty_block() {
        let out = run(Sort::new(vec![key(1)]), Vec::new(), 4);
        assert!(out.is_empty());
    }
}

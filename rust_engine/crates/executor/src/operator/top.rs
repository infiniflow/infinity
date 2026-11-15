//! `PhysicalTop` (`physical_top_impl.cpp:437`): sort and limit fused.
//!
//! The reference accumulates every input block in the operator state and only
//! sorts when the input is complete (`execution_count_`, `TopSolver::Finalize`),
//! then drops the first `offset_` rows. Doing the same here means the sorted
//! result is materialised once instead of being copied out of a full sort by a
//! `Limit` above it.
//!
//! Ties keep insertion order: the sort below is the stable one the stage
//! pipeline uses, which is the C++ `CompareTwoRowAndPreferLeft` rule.

use ::common::Result;
use expression::{Expr, Value};

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

/// A sort + limit operator.
#[derive(Debug, Clone)]
pub struct Top {
    keys: Vec<(Expr, bool)>,
    limit: u64,
    offset: u64,
    /// `LIMIT` over `SEARCH`: count every input row (`physical_top_impl.cpp:462`).
    total_hits_count_flag: bool,
}

impl Top {
    pub fn new(keys: Vec<(Expr, bool)>, limit: u64, offset: u64) -> Self {
        Self {
            keys,
            limit,
            offset,
            total_hits_count_flag: false,
        }
    }

    /// A top that also counts its input rows.
    pub fn counting_hits(keys: Vec<(Expr, bool)>, limit: u64, offset: u64) -> Self {
        Self {
            total_hits_count_flag: true,
            ..Self::new(keys, limit, offset)
        }
    }
}

impl PhysicalOperator for Top {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Top
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
        let hits: usize = input.iter().map(DataChunk::row_count).sum();
        let width = input
            .first()
            .map(DataChunk::column_count)
            .or_else(|| {
                states
                    .state_mut(id)
                    .data_blocks
                    .first()
                    .map(DataChunk::column_count)
            })
            .unwrap_or(0);

        let state = states.state_mut(id);
        state.data_blocks.extend(input);
        if self.total_hits_count_flag {
            state.total_hits_count_flag = true;
            state.total_hits_count += hits;
        }
        if !prev_complete {
            // Nothing to emit yet; the reference returns false until the last
            // input block has arrived.
            return Ok(false);
        }

        let mut rows: Vec<Vec<Value>> = std::mem::take(&mut state.data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        crate::sort_rows(&mut rows, &self.keys);
        let start = (self.offset as usize).min(rows.len());
        let end = self
            .offset
            .saturating_add(self.limit)
            .min(rows.len() as u64) as usize;
        let end = end.max(start);
        rows.truncate(end);
        rows.drain(..start);
        state.data_blocks = DataChunk::from_rows(width, rows);
        state.set_complete();
        Ok(true)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use expression::Value;

    fn key(column: usize) -> (Expr, bool) {
        (Expr::Column(column), true)
    }

    fn row(id: i64, rank: i64) -> Vec<Value> {
        vec![Value::Int(id), Value::Int(rank)]
    }

    fn run(top: Top, input: Vec<Vec<Value>>, batch: usize) -> Vec<Vec<Value>> {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(top)];
        let mut ctx = OperatorContext {
            block_capacity: batch,
        };
        let mut states = OperatorStates::new(&operators);
        let mut source = super::super::RowsSource::new(input, 2, batch);
        super::super::run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect()
    }

    #[test]
    fn sorts_and_keeps_the_first_rows() {
        let input = vec![row(1, 30), row(2, 10), row(3, 20)];
        let out = run(Top::new(vec![key(1)], 2, 0), input, 2);
        assert_eq!(out, vec![row(2, 10), row(3, 20)]);
    }

    #[test]
    fn offset_drops_the_head_of_the_sorted_result() {
        let input = vec![row(1, 30), row(2, 10), row(3, 20)];
        let out = run(Top::new(vec![key(1)], 5, 1), input, 2);
        assert_eq!(out, vec![row(3, 20), row(1, 30)]);
    }

    #[test]
    fn ties_keep_the_insertion_order() {
        // All ranks equal: a stable sort must keep ids in insertion order.
        let input = vec![row(3, 7), row(1, 7), row(2, 7)];
        let out = run(Top::new(vec![key(1)], 2, 0), input, 2);
        assert_eq!(out, vec![row(3, 7), row(1, 7)]);
    }

    #[test]
    fn a_descending_key_reverses_the_order() {
        let input = vec![row(1, 30), row(2, 10), row(3, 20)];
        let out = run(Top::new(vec![(Expr::Column(1), false)], 10, 0), input, 2);
        assert_eq!(out, vec![row(1, 30), row(3, 20), row(2, 10)]);
    }

    #[test]
    fn nulls_sort_first() {
        let input = vec![row(1, 30), vec![Value::Int(2), Value::Null], row(3, 10)];
        let out = run(Top::new(vec![key(1)], 10, 0), input, 2);
        assert_eq!(out[0], vec![Value::Int(2), Value::Null]);
    }

    #[test]
    fn an_empty_input_still_completes() {
        let out = run(Top::new(vec![key(1)], 2, 0), Vec::new(), 2);
        assert!(out.is_empty());
    }

    #[test]
    fn the_sort_matches_the_pipeline_order() {
        // The Top sort reuses `sort_rows`, so a full-table Top with an
        // unlimited limit must produce the pipeline's stable order.
        let mut rows = vec![row(1, 1), row(2, 1), row(3, 1)];
        crate::sort_rows(&mut rows, &[(Expr::Column(1), true)]);
        assert_eq!(rows, vec![row(1, 1), row(2, 1), row(3, 1)]);
    }
}

//! `PhysicalLimit` (`physical_limit_impl.cpp:161`) and its `UnSyncCounter`.

use std::cell::Cell;

use infinity_common::Result;

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

/// `UnSyncCounter` (`physical_limit.cppm:71`, `physical_limit_impl.cpp:92`): the
/// offset/limit budget that is consumed block by block.
///
/// Keeping the reference's shape matters: `offset`/`limit` are *budgets* that
/// shrink as blocks arrive, which is what makes `OFFSET 10000` skip whole
/// blocks without touching their rows.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub struct UnSyncCounter {
    offset: i64,
    limit: i64,
    total_hits_count: usize,
}

impl UnSyncCounter {
    pub fn new(offset: u64, limit: u64) -> Self {
        Self {
            offset: offset.min(i64::MAX as u64) as i64,
            limit: limit.min(i64::MAX as u64) as i64,
            total_hits_count: 0,
        }
    }

    /// Rows to skip at the front of a `row_count`-row batch.
    pub fn offset(&mut self, row_count: usize) -> usize {
        if self.offset <= 0 {
            return 0;
        }
        let last_offset = self.offset - row_count as i64;
        if last_offset > 0 {
            self.offset = last_offset;
            row_count
        } else {
            let result = self.offset as usize;
            self.offset = 0;
            result
        }
    }

    /// Rows to keep at the back of a `row_count`-row batch.
    pub fn limit(&mut self, row_count: usize) -> usize {
        if self.limit <= 0 {
            return 0;
        }
        let last_limit = self.limit - row_count as i64;
        if last_limit > 0 {
            self.limit = last_limit;
            row_count
        } else {
            let result = self.limit as usize;
            self.limit = 0;
            result
        }
    }

    /// `UnSyncCounter::IsLimitOver`.
    pub fn is_limit_over(&self) -> bool {
        self.limit == 0
    }

    /// `LimitCounter::AddHitsCount`: every input row counts, kept or not.
    pub fn add_hits_count(&mut self, row_count: usize) {
        self.total_hits_count += row_count;
    }

    pub fn total_hits_count(&self) -> usize {
        self.total_hits_count
    }
}

/// A limit/offset operator.
#[derive(Debug)]
pub struct Limit {
    offset: u64,
    limit: u64,
    /// `LIMIT` over `SEARCH`: the counter has to see every input row to report
    /// the total hit count, so it must not stop early
    /// (`physical_limit_impl.cpp:209`).
    total_hits_count_flag: bool,
    counter: Cell<UnSyncCounter>,
}

impl Limit {
    pub fn new(offset: u64, limit: u64) -> Self {
        Self {
            offset,
            limit,
            total_hits_count_flag: false,
            counter: Cell::new(UnSyncCounter::new(offset, limit)),
        }
    }

    /// A limit that counts every input row instead of stopping at `limit`
    /// (C++ `total_hits_count_flag_`).
    pub fn counting_hits(offset: u64, limit: u64) -> Self {
        Self {
            total_hits_count_flag: true,
            ..Self::new(offset, limit)
        }
    }
}

impl PhysicalOperator for Limit {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Limit
    }

    fn output_names(&self) -> Vec<String> {
        Vec::new()
    }

    fn init(&mut self) -> Result<()> {
        // `PhysicalLimit::Init` rebuilds the counter from the offset/limit
        // expressions on every run.
        self.counter = Cell::new(UnSyncCounter::new(self.offset, self.limit));
        Ok(())
    }

    fn execute(
        &self,
        _ctx: &mut OperatorContext,
        states: &mut OperatorStates,
        id: usize,
    ) -> Result<bool> {
        let prev_complete = states.prev_complete(id);
        let input = states.take_prev(id);
        let input_row_count: usize = input.iter().map(DataChunk::row_count).sum();

        let mut counter = self.counter.get();
        let mut offset = counter.offset(input_row_count);
        let mut output = Vec::new();
        if offset < input_row_count {
            let mut limit = counter.limit(input_row_count - offset);
            // Skip whole blocks while the offset budget lasts
            // (`physical_limit_impl.cpp:180`).
            let mut block_start = input.len();
            for (index, block) in input.iter().enumerate() {
                if offset >= block.row_count() {
                    offset -= block.row_count();
                } else {
                    block_start = index;
                    break;
                }
            }
            for block in &input[block_start..] {
                if block.row_count() == 0 {
                    continue;
                }
                let append_count = (block.row_count() - offset).min(limit);
                let mut chunk = DataChunk::with_capacity(block.column_count(), block.capacity());
                chunk.append_with(block, offset, append_count);
                chunk.finalize();
                output.push(chunk);
                offset = 0;
                limit -= append_count;
                if limit == 0 {
                    break;
                }
            }
            if self.total_hits_count_flag {
                counter.add_hits_count(input_row_count);
            }
        }
        self.counter.set(counter);

        let state = states.state_mut(id);
        state.data_blocks.extend(output);
        // `physical_limit_impl.cpp:209` completes as soon as the budget is
        // spent, but a `LIMIT` that has to report the total hit count must see
        // every input row first, so it only stops with the input.
        if prev_complete || (counter.is_limit_over() && !self.total_hits_count_flag) {
            if self.total_hits_count_flag {
                state.total_hits_count_flag = true;
                state.total_hits_count = counter.total_hits_count();
            }
            state.set_complete();
        }
        Ok(true)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_expression::Value;

    fn rows(range: std::ops::Range<i64>) -> Vec<Vec<Value>> {
        range.map(|i| vec![Value::Int(i)]).collect()
    }

    fn run(limit: Limit, input: Vec<Vec<Value>>, batch: usize) -> (Vec<Vec<Value>>, usize) {
        let total = input.len();
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(limit)];
        let mut ctx = OperatorContext {
            block_capacity: batch,
        };
        let mut states = OperatorStates::new(&operators);
        let mut source = super::super::RowsSource::new(input, 1, batch);
        super::super::run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        let pulled = total - source.remaining();
        let out = std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        (out, pulled)
    }

    #[test]
    fn limit_stops_pulling_the_source_once_it_is_done() {
        let (out, pulled) = run(Limit::new(0, 10), rows(0..100), 8);
        assert_eq!(out, rows(0..10));
        // Two batches of 8 rows answer LIMIT 10; the remaining 84 rows are
        // never converted into chunks.
        assert_eq!(pulled, 16);
    }

    #[test]
    fn offset_skips_whole_blocks_and_then_limits() {
        let (out, _) = run(Limit::new(10, 5), rows(0..100), 8);
        assert_eq!(out, rows(10..15));
    }

    #[test]
    fn offset_only_keeps_the_tail() {
        let (out, _) = run(Limit::new(95, u64::MAX), rows(0..100), 8);
        assert_eq!(out, rows(95..100));
    }

    #[test]
    fn a_limit_past_the_end_emits_everything() {
        let (out, pulled) = run(Limit::new(0, 1000), rows(0..13), 8);
        assert_eq!(out, rows(0..13));
        assert_eq!(pulled, 13);
    }

    #[test]
    fn counting_hits_does_not_stop_early() {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Limit::counting_hits(0, 10))];
        let mut ctx = OperatorContext { block_capacity: 8 };
        let mut states = OperatorStates::new(&operators);
        let mut source = super::super::RowsSource::new(rows(0..100), 1, 8);
        super::super::run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        let out: Vec<Vec<Value>> = std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        assert_eq!(out, rows(0..10));
        // Every input row was counted, not just the first batch.
        assert_eq!(source.remaining(), 0);
        assert!(states.state(0).total_hits_count_flag);
        assert_eq!(states.state(0).total_hits_count, 100);
    }

    #[test]
    fn the_counter_budgets_shrink_block_by_block() {
        let mut counter = UnSyncCounter::new(10, 5);
        assert_eq!(counter.offset(4), 4);
        assert_eq!(counter.offset(4), 4);
        assert_eq!(counter.offset(4), 2);
        assert_eq!(counter.limit(3), 3);
        assert_eq!(counter.limit(3), 2);
        assert!(counter.is_limit_over());
    }
}

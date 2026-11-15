//! `PhysicalAggregate`: hash grouping with streaming accumulators.
//!
//! The stage pipeline this replaces materialised the whole relation twice: once
//! as the input rows and once as one cloned row per group per input row
//! (`groups[idx].1.push(r.clone())`), then ran `aggregate()` per group, which
//! built another `Vec<Vec<Value>>` of tuples. This operator keeps only
//!
//! * one entry per group in a `HashMap` (first-seen order preserved in a
//!   parallel `Vec`), and
//! * one accumulator per (group, aggregate),
//!
//! so peak memory is O(groups) instead of O(rows), and the output block is
//! built once at the end.
//!
//! The accumulator reproduces `expression::aggregate` exactly,
//! including its quirks: `SUM` over integers goes through `f64` and comes back
//! as `Int`, `AVG`'s denominator counts every considered value (not just the
//! numeric ones), `DISTINCT` dedupes on the rendered form of the tuple, and
//! `MIN`/`MAX` keep the first value even when it is NULL.

use std::cell::RefCell;
use std::collections::{HashMap, HashSet};

use ::common::Result;
use expression::{Expr, Value};
use parser::AggFunc;

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

/// One aggregate of the plan: function, argument columns, DISTINCT, label.
#[derive(Debug, Clone)]
pub struct AggregateSpec {
    pub func: AggFunc,
    pub columns: Vec<usize>,
    pub distinct: bool,
    pub label: String,
}

impl AggregateSpec {
    pub fn new(func: AggFunc, columns: Vec<usize>, distinct: bool, label: String) -> Self {
        Self {
            func,
            columns,
            distinct,
            label,
        }
    }

    /// The tuple's text key, used for DISTINCT. Values may hold floats (not
    /// `Hash`), so the rendered form is the key — exactly what
    /// `expression::aggregate` dedupes on.
    fn tuple_key(tuple: &[Value]) -> Vec<String> {
        tuple.iter().map(Value::to_slt_string).collect()
    }
}

/// The running state of one aggregate within one group.
#[derive(Debug)]
enum Accumulator {
    Count {
        seen: usize,
    },
    /// `SUM` and `AVG` share the accumulation; `average` picks the output.
    Sum {
        total: f64,
        all_int: bool,
        seen: usize,
        average: bool,
    },
    /// `MIN`/`MAX`: the first considered value is taken as-is (even NULL), after
    /// which only a strictly better value replaces it.
    Extreme {
        best: Option<Value>,
        max: bool,
    },
    First {
        first: Option<Value>,
    },
    Last {
        last: Option<Value>,
    },
}

impl Accumulator {
    fn new(func: AggFunc) -> Self {
        match func {
            AggFunc::Count => Accumulator::Count { seen: 0 },
            AggFunc::Sum => Accumulator::Sum {
                total: 0.0,
                all_int: true,
                seen: 0,
                average: false,
            },
            AggFunc::Avg => Accumulator::Sum {
                total: 0.0,
                all_int: true,
                seen: 0,
                average: true,
            },
            AggFunc::Min => Accumulator::Extreme {
                best: None,
                max: false,
            },
            AggFunc::Max => Accumulator::Extreme {
                best: None,
                max: true,
            },
            AggFunc::First => Accumulator::First { first: None },
            AggFunc::Last => Accumulator::Last { last: None },
        }
    }

    /// Feeds one considered value (`None` when the row has no value for this
    /// aggregate, e.g. a NULL in a single-column aggregate).
    fn update(&mut self, value: Option<&Value>) {
        match self {
            Accumulator::Count { seen } => {
                if value.is_some() {
                    *seen += 1;
                }
            }
            Accumulator::Sum {
                total,
                all_int,
                seen,
                ..
            } => {
                let Some(value) = value else { return };
                match value {
                    Value::Int(i) => *total += *i as f64,
                    Value::Float(f) => {
                        *total += *f;
                        *all_int = false;
                    }
                    _ => {}
                }
                *seen += 1;
            }
            Accumulator::Extreme { best, max } => {
                let Some(value) = value else { return };
                *best = Some(match best.take() {
                    None => value.clone(),
                    Some(previous) => match value.partial_cmp_value(&previous) {
                        Some(std::cmp::Ordering::Less) if !*max => value.clone(),
                        Some(std::cmp::Ordering::Greater) if *max => value.clone(),
                        _ => previous,
                    },
                });
            }
            Accumulator::First { first } => {
                if first.is_none() {
                    *first = value.cloned();
                }
            }
            Accumulator::Last { last } => {
                if let Some(value) = value {
                    *last = Some(value.clone());
                }
            }
        }
    }

    fn finish(&self) -> Value {
        match self {
            Accumulator::Count { seen } => Value::Int(*seen as i64),
            Accumulator::Sum {
                total,
                all_int,
                seen,
                average,
            } => {
                if *seen == 0 {
                    // SUM/AVG over no values is NULL, not zero.
                    return Value::Null;
                }
                if *average {
                    Value::Float(*total / *seen as f64)
                } else if *all_int {
                    Value::Int(*total as i64)
                } else {
                    Value::Float(*total)
                }
            }
            Accumulator::Extreme { best, .. } => best.clone().unwrap_or(Value::Null),
            Accumulator::First { first } => first.clone().unwrap_or(Value::Null),
            Accumulator::Last { last } => last.clone().unwrap_or(Value::Null),
        }
    }
}

/// One aggregate's slot in a group: the accumulator plus the DISTINCT set.
#[derive(Debug)]
struct Slot {
    accumulator: Accumulator,
    seen: Option<HashSet<Vec<String>>>,
}

impl Slot {
    fn new(spec: &AggregateSpec) -> Self {
        Self {
            accumulator: Accumulator::new(spec.func),
            seen: spec.distinct.then(HashSet::new),
        }
    }

    /// Feeds one row. `present` is false when the row is shorter than one of
    /// the aggregate's argument columns, which `aggregate` skips entirely.
    fn update(&mut self, spec: &AggregateSpec, tuple: &[Value], present: bool) {
        if let Some(seen) = &mut self.seen {
            if !seen.insert(AggregateSpec::tuple_key(tuple)) {
                return;
            }
        }
        // `COUNT(*)` has no argument and counts every row.
        let null = Value::Null;
        let value = if !present {
            None
        } else if spec.columns.is_empty() {
            Some(&null)
        } else if spec.columns.len() == 1 {
            // A single-column aggregate ignores NULLs; a multi-column one takes
            // the first column as the value (as `aggregate` does).
            tuple.first().filter(|v| **v != Value::Null)
        } else {
            tuple.first()
        };
        self.accumulator.update(value);
    }
}

/// The groups seen so far, in first-seen order.
#[derive(Debug, Default)]
struct GroupTable {
    index: HashMap<Vec<String>, usize>,
    keys: Vec<Vec<Value>>,
    slots: Vec<Vec<Slot>>,
}

/// A hash aggregate operator.
#[derive(Debug)]
pub struct Aggregate {
    group_keys: Vec<Expr>,
    group_names: Vec<String>,
    aggs: Vec<AggregateSpec>,
    table: RefCell<GroupTable>,
}

/// The rendering of a group key used for hashing. The type tag keeps
/// `Value::PartialEq`'s variant sensitivity: `Int(1)` and `Str("1")` render the
/// same text but are different keys.
fn group_key(values: &[Value]) -> Vec<String> {
    values
        .iter()
        .map(|value| {
            let tag = match value {
                Value::Int(_) => "i",
                Value::Float(_) => "f",
                Value::Str(_) => "s",
                Value::Bool(_) => "b",
                Value::Null => "n",
            };
            format!("{tag}:{}", value.to_slt_string())
        })
        .collect()
}

impl Aggregate {
    pub fn new(
        group_keys: Vec<Expr>,
        group_names: Vec<String>,
        aggs: Vec<(AggFunc, Vec<usize>, bool, String)>,
    ) -> Self {
        let aggs = aggs
            .into_iter()
            .map(|(func, columns, distinct, label)| {
                AggregateSpec::new(func, columns, distinct, label)
            })
            .collect();
        Self {
            group_keys,
            group_names,
            aggs,
            table: RefCell::new(GroupTable::default()),
        }
    }

    /// Column names of the operator's output.
    pub fn output_names(&self) -> Vec<String> {
        let mut names = self.group_names.clone();
        names.extend(self.aggs.iter().map(|agg| agg.label.clone()));
        names
    }

    /// Values of the grouping expressions for one row.
    fn key_of(&self, block: &DataChunk, row: usize) -> Vec<Value> {
        if self
            .group_keys
            .iter()
            .all(|expr| matches!(expr, Expr::Column(_)))
        {
            // Fast path: no expression to evaluate, read the cells.
            return self
                .group_keys
                .iter()
                .map(|expr| match expr {
                    Expr::Column(c) => block.get(row, *c).cloned().unwrap_or(Value::Null),
                    _ => unreachable!("checked above"),
                })
                .collect();
        }
        let cells: Vec<Value> = (0..block.column_count())
            .map(|column| block.get(row, column).cloned().unwrap_or(Value::Null))
            .collect();
        self.group_keys.iter().map(|key| key.eval(&cells)).collect()
    }

    /// Feeds one chunk into the group table.
    fn consume(&self, block: &DataChunk) {
        let mut table = self.table.borrow_mut();
        for row in 0..block.row_count() {
            let key = self.key_of(block, row);
            let index = match table.index.get(&group_key(&key)) {
                Some(index) => *index,
                None => {
                    let index = table.keys.len();
                    table.index.insert(group_key(&key), index);
                    table.keys.push(key);
                    table.slots.push(self.aggs.iter().map(Slot::new).collect());
                    index
                }
            };
            let slots = &mut table.slots[index];
            for (spec, slot) in self.aggs.iter().zip(slots.iter_mut()) {
                // Missing columns (a row shorter than the schema) contribute
                // nothing, as in `aggregate`'s tuple loop.
                let present = spec.columns.iter().all(|c| *c < block.column_count());
                let tuple: Vec<Value> = spec
                    .columns
                    .iter()
                    .map(|c| block.get(row, *c).cloned().unwrap_or(Value::Null))
                    .collect();
                slot.update(spec, &tuple, present);
            }
        }
    }

    /// Builds the output chunks from the group table.
    fn finish(&self) -> Vec<DataChunk> {
        let table = self.table.borrow();
        let width = self.group_names.len() + self.aggs.len();
        let mut chunks: Vec<DataChunk> = Vec::new();
        let mut block = DataChunk::new(width);
        for (key, slots) in table.keys.iter().zip(table.slots.iter()) {
            let mut row = key.clone();
            row.extend(slots.iter().map(|slot| slot.accumulator.finish()));
            if block.is_full() {
                block.finalize();
                chunks.push(std::mem::replace(&mut block, DataChunk::new(width)));
            }
            // `push_row` cannot fail: the width is fixed and the block is not
            // full.
            let _ = block.push_row(&row);
        }
        if block.row_count() > 0 || chunks.is_empty() {
            block.finalize();
            chunks.push(block);
        }
        chunks
    }
}

impl PhysicalOperator for Aggregate {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Aggregate
    }

    fn output_names(&self) -> Vec<String> {
        Aggregate::output_names(self)
    }

    fn init(&mut self) -> Result<()> {
        let mut table = GroupTable::default();
        if self.group_keys.is_empty() {
            // A global aggregate always has exactly one output row, even over an
            // empty relation.
            table.keys.push(Vec::new());
            table.slots.push(self.aggs.iter().map(Slot::new).collect());
            table.index.insert(Vec::new(), 0);
        }
        *self.table.borrow_mut() = table;
        Ok(())
    }

    fn execute(
        &self,
        _ctx: &mut OperatorContext,
        states: &mut OperatorStates,
        id: usize,
    ) -> Result<bool> {
        let prev_complete = states.prev_complete(id);
        let blocks = states.take_prev(id);
        for block in &blocks {
            self.consume(block);
        }
        if !prev_complete {
            // Nothing to emit until the input is complete; the rows have been
            // folded into the accumulators and the chunks are dropped.
            return Ok(true);
        }
        let chunks = self.finish();
        let state = states.state_mut(id);
        state.data_blocks.extend(chunks);
        state.set_complete();
        Ok(true)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::operator::{ChunkSource, RowsSource};

    fn spec(func: AggFunc, columns: Vec<usize>) -> (AggFunc, Vec<usize>, bool, String) {
        (func, columns, false, func.as_str().to_string())
    }

    fn run(
        aggregate: Aggregate,
        rows: Vec<Vec<Value>>,
        width: usize,
        batch: usize,
    ) -> Vec<Vec<Value>> {
        let mut operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(aggregate)];
        for operator in &mut operators {
            operator.init().unwrap();
        }
        let mut ctx = OperatorContext {
            block_capacity: batch,
        };
        let mut states = OperatorStates::new(&operators);
        let mut source = RowsSource::new(rows, width, batch);
        crate::operator::run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect()
    }

    fn referenced(func: AggFunc, columns: Vec<usize>) -> Aggregate {
        Aggregate::new(
            vec![Expr::Column(0)],
            vec!["g".into()],
            vec![spec(func, columns)],
        )
    }

    #[test]
    fn groups_in_first_seen_order_across_chunk_boundaries() {
        // "b" first appears in chunk 0, "a" in chunk 1, "b" again in chunk 2:
        // the output order is b, a and "b" is one group.
        let rows = vec![
            vec![Value::Str("b".into()), Value::Int(1)],
            vec![Value::Str("b".into()), Value::Int(2)],
            vec![Value::Str("a".into()), Value::Int(3)],
            vec![Value::Str("b".into()), Value::Int(4)],
            vec![Value::Str("a".into()), Value::Int(5)],
        ];
        let aggregate = referenced(AggFunc::Sum, vec![1]);
        let out = run(aggregate, rows, 2, 2);
        assert_eq!(
            out,
            vec![
                vec![Value::Str("b".into()), Value::Int(7)],
                vec![Value::Str("a".into()), Value::Int(8)],
            ]
        );
    }

    #[test]
    fn count_star_counts_rows_and_count_of_a_column_skips_nulls() {
        let rows = vec![
            vec![Value::Str("g".into()), Value::Int(1)],
            vec![Value::Str("g".into()), Value::Null],
            vec![Value::Str("g".into()), Value::Int(3)],
        ];
        let out = run(referenced(AggFunc::Count, vec![]), rows.clone(), 2, 2);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(3)]]);
        let out = run(referenced(AggFunc::Count, vec![1]), rows, 2, 2);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(2)]]);
    }

    #[test]
    fn a_global_aggregate_over_an_empty_relation_is_one_row() {
        let aggregate = Aggregate::new(
            Vec::new(),
            Vec::new(),
            vec![spec(AggFunc::Count, vec![]), spec(AggFunc::Sum, vec![0])],
        );
        let out = run(aggregate, Vec::new(), 1, 4);
        assert_eq!(out, vec![vec![Value::Int(0), Value::Null]]);
    }

    #[test]
    fn a_global_aggregate_folds_every_chunk() {
        let aggregate = Aggregate::new(
            Vec::new(),
            Vec::new(),
            vec![spec(AggFunc::Sum, vec![0]), spec(AggFunc::Avg, vec![0])],
        );
        let rows: Vec<Vec<Value>> = (1..=6).map(|i| vec![Value::Int(i)]).collect();
        let out = run(aggregate, rows, 1, 2);
        assert_eq!(out, vec![vec![Value::Int(21), Value::Float(3.5)]]);
    }

    #[test]
    fn sum_of_floats_stays_a_float_and_avg_divides_by_the_value_count() {
        let rows = vec![
            vec![Value::Str("g".into()), Value::Float(1.5)],
            vec![Value::Str("g".into()), Value::Int(2)],
        ];
        let out = run(referenced(AggFunc::Sum, vec![1]), rows.clone(), 2, 4);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Float(3.5)]]);
        let out = run(referenced(AggFunc::Avg, vec![1]), rows, 2, 4);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Float(1.75)]]);
    }

    #[test]
    fn min_and_max_keep_the_first_winner() {
        let rows = vec![
            vec![Value::Str("g".into()), Value::Int(5)],
            vec![Value::Str("g".into()), Value::Int(1)],
            vec![Value::Str("g".into()), Value::Int(9)],
        ];
        let out = run(referenced(AggFunc::Min, vec![1]), rows.clone(), 2, 2);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(1)]]);
        let out = run(referenced(AggFunc::Max, vec![1]), rows, 2, 2);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(9)]]);
    }

    #[test]
    fn first_and_last_follow_the_input_order() {
        let rows = vec![
            vec![Value::Str("g".into()), Value::Int(7)],
            vec![Value::Str("g".into()), Value::Int(8)],
        ];
        let out = run(referenced(AggFunc::First, vec![1]), rows.clone(), 2, 1);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(7)]]);
        let out = run(referenced(AggFunc::Last, vec![1]), rows, 2, 1);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(8)]]);
    }

    #[test]
    fn distinct_dedupes_on_the_rendered_tuple() {
        let rows = vec![
            vec![Value::Str("g".into()), Value::Int(1)],
            vec![Value::Str("g".into()), Value::Int(1)],
            vec![Value::Str("g".into()), Value::Int(2)],
        ];
        let aggregate = Aggregate::new(
            vec![Expr::Column(0)],
            vec!["g".into()],
            vec![(AggFunc::Count, vec![1], true, "c".into())],
        );
        let out = run(aggregate, rows, 2, 2);
        assert_eq!(out, vec![vec![Value::Str("g".into()), Value::Int(2)]]);
    }

    #[test]
    fn the_accumulator_matches_the_reference_aggregate() {
        // Same rows, same answer as `expression::aggregate`, which is
        // what the stage pipeline used.
        let rows: Vec<Vec<Value>> = vec![
            vec![Value::Str("g".into()), Value::Int(3)],
            vec![Value::Str("g".into()), Value::Null],
            vec![Value::Str("g".into()), Value::Int(-4)],
            vec![Value::Str("g".into()), Value::Float(1.5)],
        ];
        for func in [
            AggFunc::Count,
            AggFunc::Sum,
            AggFunc::Avg,
            AggFunc::Min,
            AggFunc::Max,
            AggFunc::First,
            AggFunc::Last,
        ] {
            for distinct in [false, true] {
                for columns in [vec![1usize], vec![1, 0]] {
                    let reference = expression::aggregate(func, &columns, distinct, &rows);
                    let aggregate = Aggregate::new(
                        vec![Expr::Column(0)],
                        vec!["g".into()],
                        vec![(func, columns.clone(), distinct, "a".into())],
                    );
                    let out = run(aggregate, rows.clone(), 2, 3);
                    assert_eq!(
                        out,
                        vec![vec![Value::Str("g".into()), reference.clone()]],
                        "{func:?} distinct={distinct} columns={columns:?}"
                    );
                }
            }
        }
    }

    /// The reason this operator exists: a GROUP BY over 100k rows must hold one
    /// entry per group, not one cloned row per input row.
    #[test]
    fn a_group_by_holds_one_entry_per_group_not_per_row() {
        let total = 100_000usize;
        let groups = 1_000usize;
        let mut aggregate = Aggregate::new(
            vec![Expr::Column(0)],
            vec!["g".into()],
            vec![spec(AggFunc::Sum, vec![1])],
        );
        aggregate.init().unwrap();
        let rows: Vec<Vec<Value>> = (0..total)
            .map(|i| vec![Value::Int((i % groups) as i64), Value::Int(1)])
            .collect();

        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(aggregate)];
        let mut ctx = OperatorContext::default();
        let mut states = OperatorStates::new(&operators);
        let mut source = RowsSource::new(rows, 2, crate::chunk::DEFAULT_VECTOR_SIZE);
        while !states.is_complete() {
            source.next(&mut states.source).unwrap();
            crate::operator::execute_task(&operators, &mut ctx, &mut states).unwrap();
            // The operator keeps only its group table; every consumed chunk is
            // dropped before the next pass.
            assert!(states.source.data_blocks.is_empty());
        }
        let out: Vec<Vec<Value>> = std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        assert_eq!(out.len(), groups);
        assert!(out.iter().all(|row| row[1] == Value::Int(100)));
    }

    /// The group table never grows past the number of distinct keys, however
    /// many rows arrive.
    #[test]
    fn the_group_table_size_is_bounded_by_the_group_count() {
        let rows: Vec<Vec<Value>> = (0..30_000)
            .map(|i| vec![Value::Int((i % 7) as i64), Value::Int(1)])
            .collect();
        let chunks = DataChunk::from_rows(2, rows);
        let mut operator = Aggregate::new(
            vec![Expr::Column(0)],
            vec!["g".into()],
            vec![spec(AggFunc::Sum, vec![1])],
        );
        operator.init().unwrap();
        for chunk in chunks {
            operator.consume(&chunk);
            assert!(operator.table.borrow().keys.len() <= 7);
            assert!(operator.table.borrow().slots.len() <= 7);
        }
        assert_eq!(operator.table.borrow().keys.len(), 7);
    }

    #[test]
    fn groups_split_across_chunks_do_not_duplicate() {
        // Two groups alternating over three chunks.
        let rows: Vec<Vec<Value>> = (0..9)
            .map(|i| {
                vec![
                    Value::Str(if i % 2 == 0 { "even" } else { "odd" }.into()),
                    Value::Int(1),
                ]
            })
            .collect();
        let out = run(referenced(AggFunc::Sum, vec![1]), rows, 2, 2);
        assert_eq!(
            out,
            vec![
                vec![Value::Str("even".into()), Value::Int(5)],
                vec![Value::Str("odd".into()), Value::Int(4)],
            ]
        );
    }
}

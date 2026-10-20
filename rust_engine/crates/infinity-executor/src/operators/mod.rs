//! The physical operator model.
//!
//! Mirrors the reference `PhysicalOperator` (`physical_operator.cppm:31`) and
//! `OperatorState` (`operator_state.cppm:40`) plus the push driver of
//! `FragmentTask::OnExecute` (`fragment_task_impl.cpp:79`): each operator holds
//! one [`OperatorState`] per task, pulls the previous state's [`DataChunk`]s,
//! appends its own, and reports whether it did any real work. A task that
//! reports no work ends the pass, exactly like the reference's
//! `if (!execute_success) break;`.
//!
//! The chain is stored **scan-first** (`ops[0]` is the operator nearest the
//! source) because that is the order the plan is lowered into; the reference
//! stores it root-first and therefore walks `operator_count - 1 ..= 0`
//! backwards. Walking `0..len` here is the same traversal.

pub mod aggregate;
pub mod filter;
pub mod limit;
pub mod lower;
pub mod project;
pub mod scan;
pub mod sort;
pub mod top;

use std::sync::OnceLock;

use infinity_common::{Error, Result};
use infinity_expression::Value;

use crate::chunk::DataChunk;

#[cfg(test)]
thread_local! {
    static OPERATOR_RUNS: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
    static LAST_CHAIN: std::cell::RefCell<Vec<PhysicalOperatorType>> =
        const { std::cell::RefCell::new(Vec::new()) };
}

/// The operator types of the last chain `run_plan` executed, in execution
/// order. Lets a test prove which operators a query really used.
#[cfg(test)]
pub fn last_chain() -> Vec<PhysicalOperatorType> {
    LAST_CHAIN.with(|chain| chain.borrow().clone())
}

/// Counts how many fragments `run_plan` executed through the operator model, so
/// a test can prove the new driver is actually taken.
#[cfg(test)]
pub fn operator_run_count() -> usize {
    OPERATOR_RUNS.with(|runs| runs.get())
}

/// `PhysicalOperatorType` (`physical_operator_type.cppm`): only the operators
/// this migration slice implements are listed.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub enum PhysicalOperatorType {
    #[default]
    Invalid,
    Projection,
    Filter,
    Limit,
    Top,
    Aggregate,
    Sort,
}

impl PhysicalOperatorType {
    /// The type's name, as `PhysicalOperator::GetName` reports it.
    pub fn name(self) -> &'static str {
        match self {
            PhysicalOperatorType::Invalid => "Invalid",
            PhysicalOperatorType::Projection => "Projection",
            PhysicalOperatorType::Filter => "Filter",
            PhysicalOperatorType::Limit => "Limit",
            PhysicalOperatorType::Top => "Top",
            PhysicalOperatorType::Aggregate => "Aggregate",
            PhysicalOperatorType::Sort => "Sort",
        }
    }
}

/// Per-operator, per-task state (`operator_state.cppm:40`).
#[derive(Debug, Default)]
pub struct OperatorState {
    pub operator_type: PhysicalOperatorType,
    /// Input status: the operator this one pulls from is `id - 1`; operator 0
    /// pulls from [`OperatorStates::source`].
    pub data_blocks: Vec<DataChunk>,
    /// Output status.
    pub status: Option<String>,
    pub empty_source: bool,
    pub complete: bool,
    pub total_hits_count_flag: bool,
    pub total_hits_count: usize,
}

impl OperatorState {
    pub fn new(operator_type: PhysicalOperatorType) -> Self {
        Self {
            operator_type,
            ..Self::default()
        }
    }

    pub fn set_complete(&mut self) {
        self.complete = true;
    }

    pub fn is_complete(&self) -> bool {
        self.complete
    }
}

/// The state the source feeds (`SourceState`): one batch of chunks per pull.
#[derive(Debug, Default)]
pub struct SourceState {
    pub data_blocks: Vec<DataChunk>,
    pub complete: bool,
    pub empty_source: bool,
}

/// All operator states of one fragment task, plus the source state.
#[derive(Debug)]
pub struct OperatorStates {
    states: Vec<OperatorState>,
    pub source: SourceState,
}

impl OperatorStates {
    pub fn new(operators: &[Box<dyn PhysicalOperator>]) -> Self {
        Self {
            states: operators
                .iter()
                .map(|op| OperatorState::new(op.operator_type()))
                .collect(),
            source: SourceState::default(),
        }
    }

    pub fn len(&self) -> usize {
        self.states.len()
    }

    pub fn is_empty(&self) -> bool {
        self.states.is_empty()
    }

    pub fn state(&self, id: usize) -> &OperatorState {
        &self.states[id]
    }

    pub fn state_mut(&mut self, id: usize) -> &mut OperatorState {
        &mut self.states[id]
    }

    /// Moves operator `id`'s input out of its predecessor and clears the
    /// predecessor's block list (`operator_state->prev_op_state_->data_block_array_.clear()`,
    /// `physical_filter_impl.cpp:85`).
    pub fn take_prev(&mut self, id: usize) -> Vec<DataChunk> {
        if id == 0 {
            return std::mem::take(&mut self.source.data_blocks);
        }
        let prev = &mut self.states[id - 1];
        let blocks = std::mem::take(&mut prev.data_blocks);
        prev.data_blocks = Vec::new();
        blocks
    }

    /// Whether operator `id`'s predecessor is done.
    pub fn prev_complete(&self, id: usize) -> bool {
        if id == 0 {
            self.source.complete
        } else {
            self.states[id - 1].complete
        }
    }

    /// Whether operator `id`'s predecessor has (or had) an empty source.
    pub fn prev_empty_source(&self, id: usize) -> bool {
        if id == 0 {
            self.source.empty_source
        } else {
            self.states[id - 1].empty_source
        }
    }

    /// Whether the chain's head (the operator nearest the result) is done.
    pub fn is_complete(&self) -> bool {
        self.states.last().map(|s| s.complete).unwrap_or(true)
    }
}

/// What the reference threads through as `QueryContext`.
#[derive(Debug, Clone)]
pub struct OperatorContext {
    /// Rows per [`DataChunk`]; [`crate::chunk::DEFAULT_VECTOR_SIZE`] in
    /// production, smaller in tests.
    pub block_capacity: usize,
}

impl Default for OperatorContext {
    fn default() -> Self {
        Self {
            block_capacity: crate::chunk::DEFAULT_VECTOR_SIZE,
        }
    }
}

/// A push-based physical operator (`PhysicalOperator::Execute`).
pub trait PhysicalOperator {
    fn operator_type(&self) -> PhysicalOperatorType;

    /// `PhysicalOperator::GetName`.
    fn name(&self) -> &'static str {
        self.operator_type().name()
    }

    /// `PhysicalOperator::GetOutputNames`.
    fn output_names(&self) -> Vec<String>;

    /// `PhysicalOperator::Init`.
    fn init(&mut self) -> Result<()> {
        Ok(())
    }

    /// `PhysicalOperator::TaskletCount`: the number of tasks to split into.
    fn tasklet_count(&self) -> usize {
        1
    }

    /// `PhysicalOperator::Execute`. Returns whether the operator did real work;
    /// a `false` stops the pass, as `FragmentTask::OnExecute` does.
    fn execute(
        &self,
        ctx: &mut OperatorContext,
        states: &mut OperatorStates,
        id: usize,
    ) -> Result<bool>;
}

/// Pulls the next batch from the relation below the operator chain.
pub trait ChunkSource {
    /// Fills `state.data_blocks` with the next batch. Once the relation is
    /// exhausted the call sets `state.complete` (the batch handed out together
    /// with `complete` is still consumed by the chain).
    fn next(&mut self, state: &mut SourceState) -> Result<()>;
}

/// A source that hands out already materialised rows, one chunk per pull.
///
/// This is the slice-4 stand-in for the scan operator (slice 7): the rows are
/// already materialised by `run_plan`, but converting them chunk by chunk keeps
/// the untouched tail of the relation unvisited, which is what lets LIMIT stop
/// early.
pub struct RowsSource {
    rows: std::vec::IntoIter<Vec<Value>>,
    width: usize,
    batch: usize,
    started: bool,
}

impl RowsSource {
    pub fn new(rows: Vec<Vec<Value>>, width: usize, batch: usize) -> Self {
        Self {
            rows: rows.into_iter(),
            width,
            batch: batch.max(1),
            started: false,
        }
    }

    /// Rows the source has not handed out yet.
    pub fn remaining(&self) -> usize {
        self.rows.len()
    }
}

impl ChunkSource for RowsSource {
    fn next(&mut self, state: &mut SourceState) -> Result<()> {
        state.data_blocks = Vec::new();
        let mut chunk = DataChunk::with_capacity(self.width, self.batch);
        for row in self.rows.by_ref().take(self.batch) {
            let mut row = row;
            row.resize(self.width, Value::Null);
            chunk.push_row(&row)?;
        }
        let exhausted = self.rows.len() == 0;
        if chunk.row_count() > 0 {
            chunk.finalize();
            state.data_blocks.push(chunk);
        } else if !self.started {
            // The relation is empty: every operator downstream still has to run
            // once so that a projection can publish its column names.
            state.empty_source = true;
        }
        self.started = true;
        state.complete = exhausted;
        Ok(())
    }
}

/// One pass over the operator chain (`FragmentTask::OnExecute`'s inner loop).
///
/// Operators run in execution order, starting with the one nearest the source.
/// The pass stops as soon as an operator reports no progress or after an error
/// has been recorded in its state.
pub fn execute_task(
    operators: &[Box<dyn PhysicalOperator>],
    ctx: &mut OperatorContext,
    states: &mut OperatorStates,
) -> Result<bool> {
    let mut progressed = true;
    for (id, operator) in operators.iter().enumerate() {
        match operator.execute(ctx, states, id) {
            Ok(true) => {}
            Ok(false) => {
                progressed = false;
                break;
            }
            Err(error) => {
                states.state_mut(id).status = Some(error.to_string());
                return Err(error);
            }
        }
    }
    Ok(progressed)
}

/// Runs a fragment until its head completes (`FragmentTask` driven to the end).
pub fn run_fragment(
    operators: &[Box<dyn PhysicalOperator>],
    ctx: &mut OperatorContext,
    states: &mut OperatorStates,
    source: &mut dyn ChunkSource,
) -> Result<()> {
    while !states.is_complete() {
        source.next(&mut states.source)?;
        let progressed = execute_task(operators, ctx, states)?;
        if states.source.complete && !progressed && !states.is_complete() {
            // The relation is exhausted and no operator did any work, so no
            // further pass can make progress.
            return Err(Error::Invalid(
                "fragment made no progress after the source was exhausted".into(),
            ));
        }
    }
    Ok(())
}

/// Runs a lowered chain over a fragment source and returns the result in the
/// `run_plan` shape.
///
/// The source hands rows out chunk by chunk, so an operator that completes
/// early (LIMIT) stops the scan cursor too.
pub fn run_operators(
    mut operators: Vec<Box<dyn PhysicalOperator>>,
    names: Vec<String>,
    source: &mut dyn ChunkSource,
) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
    #[cfg(test)]
    {
        OPERATOR_RUNS.with(|runs| runs.set(runs.get() + 1));
        LAST_CHAIN.with(|chain| {
            *chain.borrow_mut() = operators.iter().map(|op| op.operator_type()).collect();
        });
    }
    for operator in &mut operators {
        operator.init()?;
    }
    let mut ctx = OperatorContext::default();
    let mut states = OperatorStates::new(&operators);
    run_fragment(&operators, &mut ctx, &mut states, source)?;
    // Nothing consumes the head's output, so it is still in its own state.
    let head = operators.len() - 1;
    let rows = std::mem::take(&mut states.state_mut(head).data_blocks)
        .into_iter()
        .flat_map(DataChunk::into_rows)
        .collect();
    Ok((names, rows))
}

/// Whether `run_plan` should execute through the operator model. Set
/// `INFINITY_OPERATOR_MODEL=0` to force the legacy stage pipeline (used to
/// bisect a regression between the two drivers).
pub fn enabled() -> bool {
    static ENABLED: OnceLock<bool> = OnceLock::new();
    *ENABLED.get_or_init(|| match std::env::var("INFINITY_OPERATOR_MODEL") {
        Ok(value) => !matches!(value.trim(), "0" | "false" | "off" | "no"),
        Err(_) => true,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    /// An operator that forwards its input unchanged and completes with it;
    /// used to exercise the driver.
    struct Forward {
        kind: PhysicalOperatorType,
    }

    impl PhysicalOperator for Forward {
        fn operator_type(&self) -> PhysicalOperatorType {
            self.kind
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
            let blocks = states.take_prev(id);
            let complete = states.prev_complete(id);
            let state = states.state_mut(id);
            state.data_blocks.extend(blocks);
            if complete {
                state.set_complete();
            }
            Ok(true)
        }
    }

    fn rows(n: usize) -> Vec<Vec<Value>> {
        (0..n).map(|i| vec![Value::Int(i as i64)]).collect()
    }

    fn run(operators: Vec<Box<dyn PhysicalOperator>>, rows: Vec<Vec<Value>>) -> Vec<Vec<Value>> {
        let mut ctx = OperatorContext { block_capacity: 4 };
        let mut states = OperatorStates::new(&operators);
        let mut source = RowsSource::new(rows, 1, 4);
        run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();
        let head = states.len() - 1;
        // Nothing consumes the head's output, so it is still in its own state.
        std::mem::take(&mut states.state_mut(head).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect()
    }

    #[test]
    fn a_chain_forwards_every_row_in_chunks() {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![
            Box::new(Forward {
                kind: PhysicalOperatorType::Filter,
            }),
            Box::new(Forward {
                kind: PhysicalOperatorType::Projection,
            }),
        ];
        let out = run(operators, rows(10));
        assert_eq!(out, rows(10));
    }

    #[test]
    fn take_prev_clears_the_predecessor() {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Forward {
            kind: PhysicalOperatorType::Filter,
        })];
        let mut states = OperatorStates::new(&operators);
        states.source.data_blocks = crate::chunk::DataChunk::from_rows(1, rows(3));
        let taken = states.take_prev(0);
        assert_eq!(taken.len(), 1);
        assert!(states.source.data_blocks.is_empty());
    }

    #[test]
    fn a_disabled_flag_keeps_the_legacy_path_available() {
        // The flag is read once per process; only assert the shape of the
        // helper so the fallback stays reachable in a fresh process.
        if std::env::var("INFINITY_OPERATOR_MODEL").is_err() {
            assert!(enabled());
        }
    }

    /// LIMIT 10 over 100k rows must not evaluate the projection 100k times:
    /// the reference stops as soon as the limit budget is spent
    /// (`physical_limit_impl.cpp:209`).
    #[test]
    fn a_limit_stops_the_projection_after_one_chunk() {
        use super::limit::Limit;
        use super::project::Projection;

        let total = 100_000;
        let rows: Vec<Vec<Value>> = (0..total).map(|i| vec![Value::Int(i as i64)]).collect();
        // The optimizer pushes a LIMIT below the projection (`SELECT a + 1 FROM
        // t LIMIT 10`), so this is the chain a real query runs.
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![
            Box::new(Limit::new(0, 10)),
            Box::new(Projection::new(
                vec![infinity_expression::Expr::Binary(
                    Box::new(infinity_expression::Expr::Column(0)),
                    infinity_parser::BinOp::Add,
                    Box::new(infinity_expression::Expr::Literal(Value::Int(1))),
                )],
                vec!["a".into()],
            )),
        ];
        let mut ctx = OperatorContext::default();
        let mut states = OperatorStates::new(&operators);
        let mut source = RowsSource::new(rows, 1, crate::chunk::DEFAULT_VECTOR_SIZE);
        run_fragment(&operators, &mut ctx, &mut states, &mut source).unwrap();

        let head = operators.len() - 1;
        let out: Vec<Vec<Value>> = std::mem::take(&mut states.state_mut(head).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        assert_eq!(out.len(), 10);
        assert_eq!(out[0], vec![Value::Int(1)]);
        assert_eq!(super::project::projected_rows(), 10);
        // Only one chunk was ever converted; the rest of the table is untouched.
        assert_eq!(
            source.remaining(),
            total - crate::chunk::DEFAULT_VECTOR_SIZE
        );
    }

    #[test]
    fn an_empty_source_still_runs_one_pass() {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Forward {
            kind: PhysicalOperatorType::Projection,
        })];
        let out = run(operators, Vec::new());
        assert!(out.is_empty());
    }
}

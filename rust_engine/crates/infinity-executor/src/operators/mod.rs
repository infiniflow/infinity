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
///
/// `Send` is what lets the fragment run one tasklet per thread: a tasklet owns
/// its own copy of the chain (see [`PhysicalOperator::fork`]), so the operators
/// themselves never have to be `Sync`.
pub trait PhysicalOperator: Send {
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
    ///
    /// The chain is a flat list in execution order, so the port cannot ask the
    /// operator below (the reference's `left_->TaskletCount()`); [`tasklet_count`]
    /// resolves the same rule over the whole chain instead. Only the operators
    /// whose count is `1` regardless of their input implement this.
    fn tasklet_count(&self) -> usize {
        1
    }

    /// A copy of this operator for another tasklet, or `None` when it cannot be
    /// split (it keeps state in itself, as `Limit` does with its counter).
    fn fork(&self) -> Option<Box<dyn PhysicalOperator>> {
        None
    }

    /// Width of the rows this operator emits, given its input's width.
    fn output_width(&self, input: usize) -> usize {
        input
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
    /// Width of the rows the source emits, before any projection.
    fn width(&self) -> usize {
        0
    }

    /// Fills `state.data_blocks` with the next batch. Once the relation is
    /// exhausted the call sets `state.complete` (the batch handed out together
    /// with `complete` is still consumed by the chain).
    fn next(&mut self, state: &mut SourceState) -> Result<()>;

    /// Reads the whole relation. Used when the plan has no operator above the
    /// scan, where the source *is* the result.
    fn collect(&mut self) -> Result<Vec<Vec<Value>>> {
        let mut state = SourceState::default();
        let mut rows = Vec::new();
        while !state.complete {
            self.next(&mut state)?;
            for block in std::mem::take(&mut state.data_blocks) {
                rows.extend(block.into_rows());
            }
        }
        Ok(rows)
    }
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
    fn width(&self) -> usize {
        self.width
    }

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

/// The fragment's tasklet count (`FragmentContext::CreateTasks`).
///
/// The reference asks the operator nearest the root for `TaskletCount()`; the
/// port's chain is a flat list, so the same rule is resolved bottom-up here:
/// a scan reports its block count, `Filter`/`Projection`/`Limit`/`Top`/`Sort`
/// pass their input's count through (`left_->TaskletCount()`), and every other
/// operator keeps the default of 1.
pub fn tasklet_count(operators: &[Box<dyn PhysicalOperator>], source_tasklets: usize) -> usize {
    let mut count = source_tasklets;
    for operator in operators {
        count = match operator.operator_type() {
            PhysicalOperatorType::Filter
            | PhysicalOperatorType::Projection
            | PhysicalOperatorType::Limit
            | PhysicalOperatorType::Top
            | PhysicalOperatorType::Sort => count,
            _ => operator.tasklet_count(),
        };
    }
    count
}

/// The tasklets a relation of `rows` rows supports: `PhysicalScanBase::
/// TaskletCount` is the scan's block count, and a block holds
/// `DEFAULT_VECTOR_SIZE` rows.
pub fn block_tasklets(rows: usize) -> usize {
    rows.div_ceil(crate::chunk::DEFAULT_VECTOR_SIZE).max(1)
}

/// `min(cpu_count, TaskletCount())`, as `CreateTasks` caps the parallel width.
fn cpu_count() -> usize {
    std::thread::available_parallelism()
        .map(|count| count.get())
        .unwrap_or(1)
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

/// Runs the chain once over `source` and returns the head's output.
fn run_serial(
    mut operators: Vec<Box<dyn PhysicalOperator>>,
    names: Vec<String>,
    source: &mut dyn ChunkSource,
) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
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

/// Capacity of one tasklet's exchange queue, in chunks.
///
/// The reference's [`QueueSourceState::source_queue_`] is a `BlockingQueue`
/// with `DEFAULT_BLOCKING_QUEUE_SIZE` (1024) slots, and a slot holds one
/// `DataBlock`; the same here, so a tasklet blocks only once the sink is more
/// than 1024 chunks behind. The chunks are built from rows the statement has
/// already materialised, so the queue cannot grow the relation's footprint.
///
/// [`QueueSourceState::source_queue_`]: ../../infinity_executor/index.html
const EXCHANGE_CAPACITY: usize = 1024;

/// The exchange queue of one parallel fragment: one channel per tasklet,
/// drained in tasklet order (`FragmentContext::Tasks` hands the merge task the
/// tasklet sources in that order).
struct ExchangeSource {
    receivers: std::vec::IntoIter<std::sync::mpsc::Receiver<std::result::Result<DataChunk, String>>>,
    current: Option<std::sync::mpsc::Receiver<std::result::Result<DataChunk, String>>>,
    width: usize,
    started: bool,
}

impl ExchangeSource {
    fn new(receivers: Vec<std::sync::mpsc::Receiver<std::result::Result<DataChunk, String>>>, width: usize) -> Self {
        Self {
            receivers: receivers.into_iter(),
            current: None,
            width,
            started: false,
        }
    }
}

impl ChunkSource for ExchangeSource {
    fn width(&self) -> usize {
        self.width
    }

    fn next(&mut self, state: &mut SourceState) -> Result<()> {
        state.data_blocks = Vec::new();
        loop {
            if self.current.is_none() {
                self.current = self.receivers.next();
            }
            let Some(receiver) = self.current.as_ref() else {
                // Every tasklet is done. A fragment that never produced a chunk
                // still has to run once, as an empty source does.
                if !self.started {
                    state.empty_source = true;
                    self.started = true;
                }
                state.complete = true;
                return Ok(());
            };
            match receiver.recv() {
                Ok(Ok(chunk)) => {
                    self.started = true;
                    state.data_blocks.push(chunk);
                    return Ok(());
                }
                Ok(Err(message)) => return Err(Error::Invalid(message)),
                // This tasklet is finished; continue with the next one.
                Err(_) => self.current = None,
            }
        }
    }
}

/// Drives one tasklet's chain and forwards its head's chunks to the exchange.
fn run_forward(
    mut operators: Vec<Box<dyn PhysicalOperator>>,
    source: &mut dyn ChunkSource,
    tx: std::sync::mpsc::SyncSender<std::result::Result<DataChunk, String>>,
) -> Result<()> {
    for operator in &mut operators {
        operator.init()?;
    }
    let mut ctx = OperatorContext::default();
    let mut states = OperatorStates::new(&operators);
    let head = operators.len() - 1;
    while !states.is_complete() {
        source.next(&mut states.source)?;
        let progressed = execute_task(&operators, &mut ctx, &mut states)?;
        for chunk in std::mem::take(&mut states.state_mut(head).data_blocks) {
            // A closed queue means the fragment above has what it needs
            // (`LIMIT` reached): the tasklet can stop.
            if tx.send(Ok(chunk)).is_err() {
                return Ok(());
            }
        }
        if states.source.complete && !progressed && !states.is_complete() {
            return Err(Error::Invalid(
                "fragment made no progress after the source was exhausted".into(),
            ));
        }
    }
    Ok(())
}

/// Runs `tasks` tasklets over disjoint contiguous ranges of `rows`, each with
/// its own copy of the chain, and feeds their output through the exchange to
/// `merge` — which runs as one task, in tasklet order, so the rows come out
/// exactly as the serial run returns them.
fn run_tasklets(
    operators: &[Box<dyn PhysicalOperator>],
    merge: Vec<Box<dyn PhysicalOperator>>,
    names: Vec<String>,
    rows: Vec<Vec<Value>>,
    source_width: usize,
    prefix_width: usize,
    tasks: usize,
) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
    let per_task = rows.len().div_ceil(tasks);
    let mut ranges: Vec<Vec<Vec<Value>>> = Vec::with_capacity(tasks);
    let mut rest = rows;
    for _ in 0..tasks {
        if rest.is_empty() {
            ranges.push(Vec::new());
        } else {
            let tail = rest.split_off(per_task.min(rest.len()));
            ranges.push(rest);
            rest = tail;
        }
    }
    // Every tasklet runs its own copy of the chain, so none of the operators
    // has to be `Sync`.
    let chains: Vec<Vec<Box<dyn PhysicalOperator>>> = (0..tasks)
        .map(|_| {
            operators
                .iter()
                .map(|operator| {
                    operator.fork().ok_or_else(|| {
                        Error::Invalid(format!(
                            "operator {} cannot be split into tasklets",
                            operator.name()
                        ))
                    })
                })
                .collect::<Result<Vec<_>>>()
        })
        .collect::<Result<Vec<_>>>()?;
    let batch = crate::chunk::DEFAULT_VECTOR_SIZE;
    // The tasklets go to the resident pool (`TaskScheduler::ScheduleFragment`),
    // not to threads of their own, so fragments from different statements share
    // the same workers.
    let mut receivers = Vec::with_capacity(tasks);
    for (chain, range) in chains.into_iter().zip(ranges) {
        let (tx, rx) = std::sync::mpsc::sync_channel(EXCHANGE_CAPACITY);
        receivers.push(rx);
        crate::scheduler::scheduler().submit(Box::new(move || {
            let mut source = RowsSource::new(range, source_width, batch);
            if let Err(error) = run_forward(chain, &mut source, tx.clone()) {
                // The merge task picks the error up from the queue, so a
                // failing tasklet fails the statement instead of looking like a
                // short result.
                let _ = tx.send(Err(error.to_string()));
            }
        }));
    }
    // The sink is the fragment's last task (`FragmentContext::Tasks`), so it
    // runs on the pool too and the caller waits for it.
    crate::scheduler::scheduler().run(move || -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        let mut source = ExchangeSource::new(receivers, prefix_width);
        if merge.is_empty() {
            return Ok((names, source.collect()?));
        }
        run_serial(merge, names, &mut source)
    })
}

/// Runs a lowered chain over a fragment source and returns the result in the
/// `run_plan` shape.
///
/// The source hands rows out chunk by chunk, so an operator that completes
/// early (LIMIT) stops the scan cursor too. A chain made only of row-local
/// operators is split into `min(cpu_count, TaskletCount())` tasklets; a chain
/// with a merge point (LIMIT/SORT/AGGREGATE) runs its row-local prefix in
/// tasklets and the merge part serially over the concatenated output, which is
/// the fragment-with-exchange split of `FragmentBuilder` with a single merge
/// task.
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
    if operators.is_empty() {
        // A plan with no operator above the scan: the source is the result.
        return Ok((names, source.collect()?));
    }
    // `PhysicalOperatorType` has no exchange operator: the first operator that
    // needs the whole relation is where the parallel fragment ends.
    let split = operators
        .iter()
        .position(|operator| {
            !matches!(
                operator.operator_type(),
                PhysicalOperatorType::Filter | PhysicalOperatorType::Projection
            )
        })
        .unwrap_or(operators.len());
    if split == 0 {
        // The merge point sits right on the scan (LIMIT pushed below the
        // projection, ORDER BY without a filter): nothing to split.
        return run_serial(operators, names, source);
    }
    let source_width = source.width();
    let prefix_width = operators[..split]
        .iter()
        .fold(source_width, |width, operator| {
            operator.output_width(width)
        });
    // No operator in the prefix can finish early, so reading the relation up
    // front is what the serial run would do anyway.
    let rows = source.collect()?;
    let tasks = tasklet_count(&operators, block_tasklets(rows.len())).min(cpu_count());
    let merge = operators.split_off(split);
    let mut prefix = operators;
    if tasks > 1 {
        return run_tasklets(
            &prefix,
            merge,
            names,
            rows,
            source_width,
            prefix_width,
            tasks,
        );
    }
    // One tasklet: the prefix still runs, just serially.
    let rows = {
        let mut source = RowsSource::new(rows, source_width, crate::chunk::DEFAULT_VECTOR_SIZE);
        run_serial(std::mem::take(&mut prefix), Vec::new(), &mut source)?.1
    };
    if merge.is_empty() {
        return Ok((names, rows));
    }
    // The merge fragment: one task over the tasklets' output.
    let mut source = RowsSource::new(rows, prefix_width, crate::chunk::DEFAULT_VECTOR_SIZE);
    run_serial(merge, names, &mut source)
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

    /// `PhysicalOperator::TaskletCount` per operator, resolved over the chain:
    /// the row-wise operators pass the scan's block count through, a blocking
    /// one drops to a single tasklet.
    #[test]
    fn tasklet_count_follows_the_operator_rules() {
        use super::aggregate::Aggregate;
        use super::filter::Filter;
        use super::limit::Limit;
        use super::project::Projection;
        use super::sort::Sort;

        let row_wise: Vec<Box<dyn PhysicalOperator>> = vec![
            Box::new(Filter::new(infinity_expression::Expr::Literal(
                Value::Bool(true),
            ))),
            Box::new(Projection::new(Vec::new(), Vec::new())),
            Box::new(Limit::new(0, 5)),
            Box::new(Sort::new(Vec::new())),
        ];
        assert_eq!(tasklet_count(&row_wise, 12), 12);
        let aggregate: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Aggregate::new(
            Vec::new(),
            Vec::new(),
            Vec::new(),
        ))];
        assert_eq!(tasklet_count(&aggregate, 12), 1);
    }

    /// Tasklets read disjoint contiguous ranges and their output is joined in
    /// tasklet order, so the rows come back exactly as the serial run returns
    /// them.
    #[test]
    fn tasklets_join_their_output_in_tasklet_order() {
        use super::filter::Filter;
        use super::project::Projection;

        let operators: Vec<Box<dyn PhysicalOperator>> = vec![
            Box::new(Filter::new(infinity_expression::Expr::Cmp(
                Box::new(infinity_expression::Expr::Column(0)),
                infinity_parser::CmpOp::Gt,
                Box::new(infinity_expression::Expr::Literal(Value::Int(10))),
            ))),
            Box::new(Projection::new(
                vec![infinity_expression::Expr::Column(0)],
                vec!["a".into()],
            )),
        ];
        let input = rows(41);
        let mut serial_source = RowsSource::new(input.clone(), 1, 4);
        let chains: Vec<Box<dyn PhysicalOperator>> =
            operators.iter().map(|op| op.fork().unwrap()).collect();
        let expected = run_serial(chains, Vec::new(), &mut serial_source).unwrap().1;
        assert_eq!(expected.len(), 30);
        // Tasklet counts that divide the rows evenly and ones that do not, down
        // to one row per tasklet.
        for tasks in [2, 3, 5, 41] {
            let (_, out) = run_tasklets(
                &operators,
                Vec::new(),
                Vec::new(),
                input.clone(),
                1,
                1,
                tasks,
            )
            .unwrap();
            assert_eq!(out, expected, "tasks = {tasks}");
        }
    }

    /// The merge task pulls through the exchange queue, tasklet by tasklet, and
    /// stops the tasklets once it has what it needs: a `LIMIT` above the split
    /// returns at once instead of waiting for every tasklet to finish.
    #[test]
    fn the_merge_task_reads_the_tasklets_through_the_exchange_queue() {
        use super::filter::Filter;
        use super::limit::Limit;
        use super::project::Projection;

        let operators: Vec<Box<dyn PhysicalOperator>> = vec![
            Box::new(Filter::new(infinity_expression::Expr::Cmp(
                Box::new(infinity_expression::Expr::Column(0)),
                infinity_parser::CmpOp::Gt,
                Box::new(infinity_expression::Expr::Literal(Value::Int(100))),
            ))),
            Box::new(Projection::new(
                vec![infinity_expression::Expr::Column(0)],
                vec!["a".into()],
            )),
        ];
        let merge: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Limit::new(0, 10))];
        let (_, out) = run_tasklets(
            &operators,
            merge,
            vec!["a".into()],
            rows(20_000),
            1,
            1,
            4,
        )
        .unwrap();
        assert_eq!(out.len(), 10);
        assert_eq!(out[0], vec![Value::Int(101)]);
        assert_eq!(out[9], vec![Value::Int(110)]);
    }

    /// A tasklet's failure travels through the exchange instead of ending the
    /// statement with a short result.
    #[test]
    fn a_tasklet_error_fails_the_statement() {
        /// Forwarding operator that fails on its first pass.
        struct Boom;

        impl PhysicalOperator for Boom {
            fn operator_type(&self) -> PhysicalOperatorType {
                PhysicalOperatorType::Filter
            }

            fn output_names(&self) -> Vec<String> {
                Vec::new()
            }

            fn fork(&self) -> Option<Box<dyn PhysicalOperator>> {
                Some(Box::new(Boom))
            }

            fn execute(
                &self,
                _ctx: &mut OperatorContext,
                _states: &mut OperatorStates,
                _id: usize,
            ) -> Result<bool> {
                Err(Error::Invalid("boom".into()))
            }
        }

        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Boom)];
        let result = run_tasklets(&operators, Vec::new(), Vec::new(), rows(8), 1, 1, 2);
        assert!(result.is_err(), "{result:?}");
    }

    /// The entry point takes the same split: a row-wise chain over more than one
    /// block runs in tasklets, and a LIMIT above it still sees the rows in
    /// order.
    #[test]
    fn run_operators_splits_a_row_wise_chain_and_limits_after_the_merge() {
        use super::filter::Filter;
        use super::limit::Limit;
        use super::project::Projection;

        let chain = || -> Vec<Box<dyn PhysicalOperator>> {
            vec![
                Box::new(Filter::new(infinity_expression::Expr::Cmp(
                    Box::new(infinity_expression::Expr::Column(0)),
                    infinity_parser::CmpOp::Gt,
                    Box::new(infinity_expression::Expr::Literal(Value::Int(100))),
                ))),
                Box::new(Projection::new(
                    vec![infinity_expression::Expr::Column(0)],
                    vec!["a".into()],
                )),
            ]
        };
        // 20_000 rows is more than two blocks, so a two-cpu machine already
        // runs two tasklets.
        let input = rows(20_000);
        let mut source = RowsSource::new(input.clone(), 1, crate::chunk::DEFAULT_VECTOR_SIZE);
        let (names, out) = run_operators(chain(), vec!["a".into()], &mut source).unwrap();
        assert_eq!(names, vec!["a".to_string()]);
        assert_eq!(out.len(), 20_000 - 101);
        assert_eq!(out[0], vec![Value::Int(101)]);
        assert_eq!(out[out.len() - 1], vec![Value::Int(19_999)]);

        // LIMIT is the merge point: the filter runs in tasklets, the limit takes
        // the first rows of their concatenation.
        let mut operators = chain();
        operators.push(Box::new(Limit::new(0, 10)));
        let mut source = RowsSource::new(input, 1, crate::chunk::DEFAULT_VECTOR_SIZE);
        let (_, out) = run_operators(operators, vec!["a".into()], &mut source).unwrap();
        assert_eq!(out.len(), 10);
        assert_eq!(out[0], vec![Value::Int(101)]);
        assert_eq!(out[9], vec![Value::Int(110)]);
    }

    #[test]
    fn a_chain_forwards_every_row_in_chunks() {        let operators: Vec<Box<dyn PhysicalOperator>> = vec![
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

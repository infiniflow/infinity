//! Query profiling: phase timings, optimizer-rule timings and task records.
//!
//! C++: `BaseProfiler` / `QueryPhase` / `OptimizerProfiler` / `TaskProfiler` /
//! `QueryProfiler` / `ProfileHistory` (`src/main/profiler.cppm`), with the
//! semantics of `src/main/profiler_impl.cpp`.

#![allow(dead_code)]

use std::collections::{HashMap, VecDeque};
use std::sync::{Arc, Mutex};
use std::time::{Duration, SystemTime, UNIX_EPOCH};

use infinity_common::{Error, Result};

/// C++: `QueryPhase` (`profiler.cppm:70`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum QueryPhase {
    Parser,
    LogicalPlan,
    Optimizer,
    PhysicalPlan,
    PipelineBuild,
    TaskBuild,
    Execution,
    Commit,
    Rollback,
    Invalid,
}

impl QueryPhase {
    /// C++: `QueryProfiler::QueryPhaseToString` (`profiler_impl.cpp:177`).
    /// `kInvalid` raises "Invalid query phase in query profiler".
    pub fn as_str(self) -> Result<&'static str> {
        query_phase_to_string(self)
    }

    fn index(self) -> usize {
        self as usize
    }
}

/// C++: `QueryProfiler::QueryPhaseToString`.
pub fn query_phase_to_string(phase: QueryPhase) -> Result<&'static str> {
    Ok(match phase {
        QueryPhase::Parser => "Parser",
        QueryPhase::LogicalPlan => "LogicalPlan",
        QueryPhase::Optimizer => "Optimizer",
        QueryPhase::PhysicalPlan => "PhysicalPlan",
        QueryPhase::PipelineBuild => "PipelineBuild",
        QueryPhase::TaskBuild => "TaskBuild",
        QueryPhase::Execution => "Execution",
        QueryPhase::Commit => "Commit",
        QueryPhase::Rollback => "Rollback",
        QueryPhase::Invalid => {
            return Err(Error::Invalid(
                "Invalid query phase in query profiler".to_string(),
            ))
        }
    })
}

/// C++: `BaseProfiler` (`profiler.cppm:24`).
#[derive(Debug, Clone, Default)]
pub struct BaseProfiler {
    name: String,
    begin: Option<SystemTime>,
    end: Option<SystemTime>,
    finished: bool,
}

impl BaseProfiler {
    pub fn new(name: impl Into<String>) -> Self {
        Self {
            name: name.into(),
            ..Default::default()
        }
    }

    pub fn begin(&mut self) {
        self.finished = false;
        self.begin = Some(SystemTime::now());
    }

    pub fn end(&mut self) {
        if self.finished {
            return;
        }
        self.end = Some(SystemTime::now());
        self.finished = true;
    }

    fn elapsed_ns(&self) -> i64 {
        let Some(begin) = self.begin else {
            // A profiler that never began has no duration (the reference would
            // measure from the epoch).
            return 0;
        };
        let now = if self.finished {
            self.end.unwrap_or(begin)
        } else {
            SystemTime::now()
        };
        now.duration_since(begin)
            .map(|d| d.as_nanos() as i64)
            .unwrap_or(0)
    }

    /// C++: `BaseProfiler::Elapsed`.
    pub fn elapsed(&self) -> i64 {
        self.elapsed_ns()
    }

    /// C++: `BaseProfiler::ElapsedToString`.
    pub fn elapsed_to_string_with_scale(&self, scale: i64) -> String {
        elapsed_to_string(self.elapsed_ns(), scale)
    }

    pub fn elapsed_to_string(&self) -> String {
        self.elapsed_to_string_with_scale(1)
    }

    pub fn get_begin(&self) -> i64 {
        self.begin
            .and_then(|t| t.duration_since(UNIX_EPOCH).ok())
            .map(|d| d.as_nanos() as i64)
            .unwrap_or(0)
    }

    pub fn get_end(&self) -> i64 {
        self.end
            .and_then(|t| t.duration_since(UNIX_EPOCH).ok())
            .map(|d| d.as_nanos() as i64)
            .unwrap_or(0)
    }

    /// C++: `BaseProfiler::BeginTime` — local `HH:MM:SS:mmm:uuu:nnn`.
    pub fn begin_time(&self) -> String {
        match self.begin {
            Some(ts) => format_time(ts, false),
            None => String::new(),
        }
    }

    /// C++: `BaseProfiler::EndTime` — local `YYYY-MM-DD HH:MM:SS.mmm.uuu`.
    pub fn end_time(&self) -> String {
        match self.end {
            Some(ts) => format_time(ts, true),
            None => String::new(),
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn set_name(&mut self, name: impl Into<String>) {
        self.name = name.into();
    }
}

/// C++: `BaseProfiler::ElapsedToString` (`profiler_impl.cpp:98`).
pub fn elapsed_to_string(nanos: i64, scale: i64) -> String {
    let scale = scale.max(1);
    if nanos <= 1_000 * scale {
        format!("{nanos}ns")
    } else if nanos <= 1_000_000 * scale {
        format!("{}us", nanos / 1_000)
    } else if nanos <= 1_000_000_000 * scale {
        format!("{}ms", nanos / 1_000_000)
    } else {
        format!("{}s", nanos / 1_000_000_000)
    }
}

/// Renders a `SystemTime` in UTC as C++'s `%T` (or `%F %T`) plus fractions.
fn format_time(ts: SystemTime, with_date: bool) -> String {
    let dur = ts.duration_since(UNIX_EPOCH).unwrap_or(Duration::ZERO);
    let secs = dur.as_secs() as i64;
    let nanos = dur.subsec_nanos();
    let (year, month, day, hour, minute, second) = civil(secs);
    let millis = nanos / 1_000_000;
    let micros = (nanos / 1_000) % 1_000;
    let sub_nanos = nanos % 1_000;
    if with_date {
        format!("{year:04}-{month:02}-{day:02} {hour:02}:{minute:02}:{second:02}.{millis:03}.{micros:03}")
    } else {
        format!("{hour:02}:{minute:02}:{second:02}:{millis:03}:{micros:03}:{sub_nanos:03}")
    }
}

/// Days-since-epoch to a civil UTC date (Howard Hinnant's algorithm).
fn civil(secs: i64) -> (i64, u32, u32, u32, u32, u32) {
    let days = secs.div_euclid(86_400);
    let rem = secs.rem_euclid(86_400) as u32;
    let z = days + 719_468;
    let era = z.div_euclid(146_097);
    let doe = z.rem_euclid(146_097);
    let yoe = (doe - doe / 1_460 + doe / 36_524 - doe / 146_096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = (doy - (153 * mp + 2) / 5 + 1) as u32;
    let m = if mp < 10 { mp + 3 } else { mp - 9 } as u32;
    let year = if m <= 2 { y + 1 } else { y };
    (year, m, d, rem / 3_600, (rem % 3_600) / 60, rem % 60)
}

/// C++: `OptimizerProfiler` (`profiler.cppm:126`).
#[derive(Debug, Default)]
pub struct OptimizerProfiler {
    profilers: Vec<BaseProfiler>,
}

impl OptimizerProfiler {
    pub fn start_rule(&mut self, rule_name: impl Into<String>) {
        let mut p = BaseProfiler::new(rule_name);
        p.begin();
        self.profilers.push(p);
    }

    /// `StopRule` ends the most recently started rule, as the reference does.
    pub fn stop_rule(&mut self) {
        if let Some(p) = self.profilers.last_mut() {
            p.end();
        }
    }

    pub fn to_string(&self, indent: usize) -> String {
        let space = " ".repeat(indent);
        let mut out = String::new();
        for p in &self.profilers {
            out.push_str(&format!("{}{}: {}", space, p.name(), p.elapsed_to_string()));
        }
        out
    }
}

/// C++: `TaskBinding` (`profiler.cppm:121`).
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub struct TaskBinding {
    pub fragment_id: u64,
    pub task_id: i64,
}

/// C++: `OperatorInformation` (`profiler.cppm:83`).
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct OperatorInformation {
    pub name: String,
    pub start: i64,
    pub end: i64,
    pub elapsed: i64,
    pub input_rows: u16,
    pub output_data_size: i32,
    pub output_rows: u16,
}

/// C++: `TaskProfiler` (`profiler.cppm:142`).
///
/// The reference reads operator names and row/data sizes from
/// `PhysicalOperator` / `OperatorState`; the Rust pipeline passes those values
/// in directly, since its stages carry no per-operator runtime object.
#[derive(Debug, Default)]
pub struct TaskProfiler {
    pub binding: TaskBinding,
    pub timings: Vec<OperatorInformation>,
    pub task_profiler: BaseProfiler,
    enable: bool,
    profiler: BaseProfiler,
    active_operator: Option<String>,
}

impl TaskProfiler {
    pub fn new(binding: TaskBinding, enable: bool, operators_len: usize) -> Self {
        Self {
            binding,
            timings: Vec::with_capacity(if enable { operators_len } else { 0 }),
            enable,
            ..Default::default()
        }
    }

    pub fn enable(&self) -> bool {
        self.enable
    }

    pub fn begin(&mut self) {
        if self.enable {
            self.task_profiler.begin();
        }
    }

    pub fn end(&mut self) {
        if self.enable {
            self.task_profiler.end();
        }
    }

    pub fn start_operator(&mut self, name: impl Into<String>) -> Result<()> {
        if !self.enable {
            return Ok(());
        }
        if self.active_operator.is_some() {
            return Err(Error::Invalid(
                "Attempting to call StartOperator while another operator is active.".to_string(),
            ));
        }
        self.active_operator = Some(name.into());
        self.profiler.begin();
        Ok(())
    }

    pub fn stop_operator(
        &mut self,
        input_rows: u16,
        output_data_size: i32,
        output_rows: u16,
    ) -> Result<()> {
        if !self.enable {
            return Ok(());
        }
        let Some(name) = self.active_operator.take() else {
            return Err(Error::Invalid(
                "Attempting to call StartOperator while another operator is active.".to_string(),
            ));
        };
        self.profiler.end();
        self.timings.push(OperatorInformation {
            name,
            start: self.profiler.get_begin(),
            end: self.profiler.get_end(),
            elapsed: self.profiler.elapsed(),
            input_rows,
            output_data_size,
            output_rows,
        });
        Ok(())
    }
}

/// C++: `QueryProfiler` (`profiler.cppm:180`).
#[derive(Debug)]
pub struct QueryProfiler {
    enable: bool,
    records: HashMap<u64, HashMap<i64, Vec<TaskProfiler>>>,
    profilers: Vec<BaseProfiler>,
    optimizer: OptimizerProfiler,
    current_phase: QueryPhase,
}

impl Default for QueryProfiler {
    fn default() -> Self {
        Self::new()
    }
}

impl QueryProfiler {
    /// The reference's default constructor leaves profiling disabled, so the
    /// phase calls are no-ops (its unit test relies on that).
    pub fn new() -> Self {
        Self {
            enable: false,
            records: HashMap::new(),
            profilers: vec![BaseProfiler::default(); QueryPhase::Invalid.index()],
            optimizer: OptimizerProfiler::default(),
            current_phase: QueryPhase::Invalid,
        }
    }

    pub fn with_enable(enable: bool) -> Self {
        Self {
            enable,
            ..Self::new()
        }
    }

    /// C++: `QueryProfiler::StartPhase` (`profiler_impl.cpp:213`).
    pub fn start_phase(&mut self, phase: QueryPhase) -> Result<()> {
        if !self.enable {
            return Ok(());
        }
        if self.current_phase == QueryPhase::Invalid {
            self.current_phase = phase;
        } else {
            return Err(Error::Invalid(format!(
                "Can't start new query phase before current phase({}) is finished",
                self.current_phase.as_str()?
            )));
        }
        let name = phase.as_str()?;
        let profiler = &mut self.profilers[phase.index()];
        profiler.set_name(name);
        profiler.begin();
        Ok(())
    }

    /// C++: `QueryProfiler::StopPhase` (`profiler_impl.cpp:231`).
    pub fn stop_phase(&mut self, phase: QueryPhase) -> Result<()> {
        if !self.enable {
            return Ok(());
        }
        if self.current_phase == QueryPhase::Invalid {
            return Err(Error::Invalid("Query phase isn't started, yet".to_string()));
        }
        self.current_phase = QueryPhase::Invalid;
        self.profilers[phase.index()].end();
        Ok(())
    }

    /// C++: `QueryProfiler::Stop`: ends the active phase when profiler is closed.
    pub fn stop(&mut self) {
        if !self.enable || self.current_phase == QueryPhase::Invalid {
            return;
        }
        self.profilers[self.current_phase.index()].end();
        self.current_phase = QueryPhase::Invalid;
    }

    /// C++: `QueryProfiler::Flush`.
    pub fn flush(&mut self, profiler: TaskProfiler) {
        self.records
            .entry(profiler.binding.fragment_id)
            .or_default()
            .entry(profiler.binding.task_id)
            .or_default()
            .push(profiler);
    }

    pub fn elapsed_at(&self, index: usize) -> i64 {
        self.profilers[index].elapsed()
    }

    pub fn optimizer(&self) -> &OptimizerProfiler {
        &self.optimizer
    }

    pub fn optimizer_mut(&mut self) -> &mut OptimizerProfiler {
        &mut self.optimizer
    }

    /// C++: `QueryProfiler::GetTaskProfile`.
    pub fn task_profile(&mut self, fragment_id: u64, task_id: i64) -> &mut Vec<TaskProfiler> {
        self.records
            .entry(fragment_id)
            .or_default()
            .entry(task_id)
            .or_default()
    }

    fn render_tasks(&self) -> String {
        let mut out = String::new();
        for (fragment_id, tasks) in &self.records {
            out.push_str(&format!("Fragment #{fragment_id}\n"));
            for (task_id, runs) in tasks {
                out.push_str(&format!("|- Task #{task_id}\n"));
                for (times, profiler) in runs.iter().enumerate() {
                    out.push_str(&format!("  |- Times: {times}\n"));
                    for op in &profiler.timings {
                        out.push_str(&format!(
                            "    -> {}: BeginTime: {}: EndTime: {}: ElapsedTime: {}, InputRows: {}, OutputRows: {}, OutputDataSize: {}\n",
                            op.name, op.start, op.end, op.elapsed, op.input_rows, op.output_rows,
                            op.output_data_size
                        ));
                    }
                }
            }
        }
        out
    }
}

/// C++: `QueryProfiler::ToString`.
impl std::fmt::Display for QueryProfiler {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let cost_sum: i64 = self.profilers.iter().map(BaseProfiler::elapsed).sum();
        for (idx, profiler) in self.profilers.iter().enumerate() {
            let pct = if cost_sum == 0 {
                0.0
            } else {
                profiler.elapsed() as f64 * 100.0 / cost_sum as f64
            };
            writeln!(
                f,
                "{}: {}({:.2}%)",
                profiler.name(),
                profiler.elapsed_to_string(),
                pct
            )?;
            if idx == QueryPhase::Optimizer.index() {
                writeln!(f, "{}", self.optimizer.to_string(4))?;
            }
            if idx == QueryPhase::Execution.index() {
                write!(f, "{}", self.render_tasks())?;
            }
        }
        Ok(())
    }
}

/// C++: `ProfileHistory` (`profiler.cppm:218`).
#[derive(Debug)]
pub struct ProfileHistory {
    inner: Mutex<ProfileHistoryInner>,
}

#[derive(Debug)]
struct ProfileHistoryInner {
    deque: VecDeque<Arc<QueryProfiler>>,
    max_size: usize,
}

impl ProfileHistory {
    pub fn new(size: usize) -> Self {
        Self {
            inner: Mutex::new(ProfileHistoryInner {
                deque: VecDeque::new(),
                max_size: size,
            }),
        }
    }

    pub fn history_capacity(&self) -> usize {
        self.inner.lock().unwrap().max_size
    }

    /// C++: `ProfileHistory::Resize`. The reference truncates `max_size_ -
    /// new_size` entries; this pops until the deque fits, which is the
    /// intended (and defined) behaviour.
    pub fn resize(&self, new_size: usize) {
        let mut inner = self.inner.lock().unwrap();
        if new_size == 0 {
            inner.deque.clear();
            return;
        }
        if new_size == inner.max_size {
            return;
        }
        while inner.deque.len() > new_size {
            inner.deque.pop_back();
        }
        inner.max_size = new_size;
    }

    /// C++: `ProfileHistory::Enqueue` (dropping the oldest entry at capacity).
    pub fn enqueue(&self, profiler: Arc<QueryProfiler>) {
        let mut inner = self.inner.lock().unwrap();
        if inner.deque.len() >= inner.max_size {
            inner.deque.pop_back();
        }
        inner.deque.push_front(profiler);
    }

    pub fn get_element(&self, index: usize) -> Option<Arc<QueryProfiler>> {
        let inner = self.inner.lock().unwrap();
        inner.deque.get(index).cloned()
    }

    pub fn get_elements(&self) -> Vec<Arc<QueryProfiler>> {
        let inner = self.inner.lock().unwrap();
        inner.deque.iter().cloned().collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The `Error::Invalid` message of a failed call (`UnrecoverableError`'s
    /// text in the reference).
    fn invalid_message<T: std::fmt::Debug>(r: Result<T>) -> String {
        match r {
            Err(Error::Invalid(m)) => m,
            other => panic!("expected an invalid-argument error, got {other:?}"),
        }
    }

    /// `src/unit_test/main/query_profiler_ut.cpp::QueryProfilerTest/test1`.
    #[test]
    fn query_phase_names_match_the_reference() {
        assert_eq!(QueryPhase::Parser.as_str().unwrap(), "Parser");
        assert_eq!(QueryPhase::LogicalPlan.as_str().unwrap(), "LogicalPlan");
        assert_eq!(QueryPhase::Optimizer.as_str().unwrap(), "Optimizer");
        assert_eq!(QueryPhase::PhysicalPlan.as_str().unwrap(), "PhysicalPlan");
        assert_eq!(QueryPhase::PipelineBuild.as_str().unwrap(), "PipelineBuild");
        assert_eq!(QueryPhase::TaskBuild.as_str().unwrap(), "TaskBuild");
        assert_eq!(QueryPhase::Execution.as_str().unwrap(), "Execution");
        assert_eq!(
            invalid_message(QueryPhase::Invalid.as_str()),
            "Invalid query phase in query profiler"
        );
    }

    /// `QueryProfilerTest/test2`: the default profiler is disabled, so phase
    /// errors are unreachable (as in the reference, where `enable_{}` is
    /// false).
    #[test]
    fn disabled_profiler_ignores_phase_transitions() {
        let mut profiler = QueryProfiler::new();
        assert!(profiler.stop_phase(QueryPhase::Parser).is_ok());
        assert!(profiler.start_phase(QueryPhase::Parser).is_ok());
        assert!(profiler.start_phase(QueryPhase::LogicalPlan).is_ok());
    }

    /// An enabled profiler enforces the reference's phase state machine.
    #[test]
    fn enabled_profiler_enforces_phase_order() {
        let mut profiler = QueryProfiler::with_enable(true);
        assert_eq!(
            invalid_message(profiler.stop_phase(QueryPhase::Parser)),
            "Query phase isn't started, yet"
        );
        profiler.start_phase(QueryPhase::Parser).unwrap();
        assert_eq!(
            invalid_message(profiler.start_phase(QueryPhase::LogicalPlan)),
            "Can't start new query phase before current phase(Parser) is finished"
        );
        profiler.stop_phase(QueryPhase::Parser).unwrap();
        profiler.start_phase(QueryPhase::LogicalPlan).unwrap();
        profiler.stop_phase(QueryPhase::LogicalPlan).unwrap();
        profiler.stop();
        assert!(profiler.to_string().contains("Parser: "));
    }

    /// `QueryProfilerTest/test3`: every phase and optimizer rule records time.
    #[test]
    fn all_phases_and_optimizer_rules_record_time() {
        let mut profiler = QueryProfiler::with_enable(true);
        for phase in [
            QueryPhase::Parser,
            QueryPhase::LogicalPlan,
            QueryPhase::PhysicalPlan,
            QueryPhase::PipelineBuild,
            QueryPhase::TaskBuild,
            QueryPhase::Execution,
        ] {
            profiler.start_phase(phase).unwrap();
            profiler.stop_phase(phase).unwrap();
        }
        profiler.start_phase(QueryPhase::Optimizer).unwrap();
        for rule in [
            "OptimizerRule1",
            "OptimizerRule2",
            "OptimizerRule3",
            "OptimizerRule4",
        ] {
            profiler.optimizer_mut().start_rule(rule);
            profiler.optimizer_mut().stop_rule();
        }
        profiler.stop_phase(QueryPhase::Optimizer).unwrap();
        let text = profiler.to_string();
        assert!(text.contains("Optimizer: "));
        assert!(text.contains("    OptimizerRule1: "));
    }

    /// `ProfileHistory` keeps the newest profiles up to its capacity.
    #[test]
    fn history_keeps_the_newest_profiles() {
        let history = ProfileHistory::new(2);
        for _ in 0..3 {
            history.enqueue(Arc::new(QueryProfiler::new()));
        }
        assert_eq!(history.get_elements().len(), 2);
        assert!(history.get_element(2).is_none());
        history.resize(1);
        assert_eq!(history.history_capacity(), 1);
        assert_eq!(history.get_elements().len(), 1);
        history.resize(0);
        assert!(history.get_elements().is_empty());
    }

    /// Task profiling records one timing per operator and rejects nesting.
    #[test]
    fn task_profiler_records_operator_timings() {
        let mut task = TaskProfiler::new(
            TaskBinding {
                fragment_id: 0,
                task_id: 1,
            },
            true,
            2,
        );
        assert!(task.enable());
        task.start_operator("Projection").unwrap();
        assert!(task.start_operator("Filter").is_err());
        task.stop_operator(3, 120, 2).unwrap();
        assert_eq!(task.timings.len(), 1);
        assert_eq!(task.timings[0].name, "Projection");
        assert_eq!(task.timings[0].input_rows, 3);
        assert_eq!(task.timings[0].output_rows, 2);
        assert!(task.stop_operator(0, 0, 0).is_err());
    }
}

//! Runtime diagnostics: the running-query registry and the statement profile
//! history the `SHOW QUERIES` / `SHOW QUERY <id>` / `SHOW PROFILES` operators
//! and the HTTP `/instance/*` routes read.
//!
//! C++: `SessionManager::QueryRecords` (`src/main/session_manager.cppm:96`) and
//! the profile history (`NewCatalog::GetProfileRecords`). The reference keys
//! the registry by session id because one server hosts many sessions; this
//! engine is single-session, so the registry is per [`Engine`] and every row
//! reports session id `0`.

use std::collections::VecDeque;
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

use parser::Statement;

/// Default size of the finished-statement history (`[profile]
/// profile_record_capacity`, the reference's `ProfileHistory` default).
pub const DEFAULT_PROFILE_CAPACITY: usize = 64;

/// The session id every [`QueryRow`] reports: this engine serves one session.
pub const SESSION_ID: u64 = 0;

/// One row of `SHOW QUERIES` (C++ `QueryInfo`, `session_manager.cppm:20`):
/// `session_id`, `query_id`, `query_kind`, `start_time`, `time_consumption`.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct QueryRow {
    pub session_id: u64,
    pub query_id: u64,
    /// The statement kind, as `StatementType2Str` spells it (`SELECT`, …).
    pub query_kind: String,
    /// The statement text as submitted.
    pub query_text: String,
    /// Local `HH:MM:SS:mmm:uuu:nnn` start time (`BaseProfiler::BeginTime`).
    pub start_time: String,
    /// Elapsed time so far (`BaseProfiler::ElapsedToString`).
    pub time_consumption: String,
}

/// One entry of the finished-statement history (`SHOW PROFILES`).
///
/// The reference stores a whole `QueryProfiler` per entry and reports a column
/// per query phase. This engine has no per-phase breakdown — the ported
/// pipeline is not split into the reference's physical phases — so an entry
/// carries the one timing that is real here: the statement's wall-clock
/// duration.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct ProfileRow {
    pub query_id: u64,
    pub query_kind: String,
    pub query_text: String,
    /// Wall-clock duration of the statement, in nanoseconds.
    pub duration_ns: i64,
}

/// One row of `SHOW MEMINDEX`: an index that keeps an in-memory segment.
///
/// C++ `MemIndexTracerInfo` (`src/storage/tracer/memindex_tracer.cppm`) —
/// `index_name`, `table_name`, `db_name`, `mem_used`, `row_count`. The index
/// type is added here because one row set covers every index family (HNSW,
/// full-text, sparse, multi-vector) instead of the reference's HNSW-only
/// tracer.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct MemoryIndexRow {
    pub table: String,
    pub index: String,
    pub index_type: String,
    /// Rows the in-memory part of the index holds.
    pub row_count: usize,
    /// Bytes the in-memory part occupies (full-text: tokens held).
    pub mem_used: usize,
}

/// A query that is executing right now.
#[derive(Clone, Debug)]
pub struct RunningQuery {
    pub query_id: u64,
    pub query_kind: String,
    pub query_text: String,
    pub started: Instant,
    pub started_wall: SystemTime,
}

impl RunningQuery {
    /// The `SHOW QUERIES` row for this query.
    pub fn row(&self) -> QueryRow {
        QueryRow {
            session_id: SESSION_ID,
            query_id: self.query_id,
            query_kind: self.query_kind.clone(),
            query_text: self.query_text.clone(),
            start_time: local_time(self.started_wall, false),
            time_consumption: elapsed_to_string(self.started.elapsed()),
        }
    }
}

/// The running-query registry plus the finished-statement history.
///
/// C++: `SessionManager`'s `query_map_` and `NewCatalog`'s profile history.
#[derive(Debug)]
pub struct QueryRegistry {
    next_id: u64,
    running: Vec<RunningQuery>,
    finished: VecDeque<ProfileRow>,
    capacity: usize,
}

impl Default for QueryRegistry {
    fn default() -> Self {
        Self::new(DEFAULT_PROFILE_CAPACITY)
    }
}

impl QueryRegistry {
    pub fn new(capacity: usize) -> Self {
        Self {
            next_id: 1,
            running: Vec::new(),
            finished: VecDeque::new(),
            capacity,
        }
    }

    /// Allocates the next query id (C++ `BaseSession::query_count`).
    pub fn next_id(&mut self) -> u64 {
        let id = self.next_id;
        self.next_id += 1;
        id
    }

    pub fn add_running(&mut self, query: RunningQuery) {
        self.running.push(query);
    }

    pub fn remove_running(&mut self, query_id: u64) {
        self.running.retain(|q| q.query_id != query_id);
    }

    pub fn running(&self) -> Vec<QueryRow> {
        self.running.iter().map(RunningQuery::row).collect()
    }

    /// A running query by id; the finished history is searched too, so
    /// `SHOW QUERY <id>` keeps working for a statement that just returned.
    pub fn query(&self, query_id: u64) -> Option<QueryRow> {
        if let Some(row) = self.running.iter().find(|q| q.query_id == query_id) {
            return Some(row.row());
        }
        self.finished
            .iter()
            .find(|p| p.query_id == query_id)
            .map(|p| QueryRow {
                session_id: SESSION_ID,
                query_id: p.query_id,
                query_kind: p.query_kind.clone(),
                query_text: p.query_text.clone(),
                start_time: String::new(),
                time_consumption: elapsed_to_string(Duration::from_nanos(
                    p.duration_ns.max(0) as u64
                )),
            })
    }

    /// Keeps `profile` at the front of the history (C++ `Enqueue`).
    pub fn push_finished(&mut self, profile: ProfileRow) {
        if self.capacity == 0 {
            return;
        }
        while self.finished.len() >= self.capacity {
            self.finished.pop_back();
        }
        self.finished.push_front(profile);
    }

    pub fn profiles(&self) -> Vec<ProfileRow> {
        self.finished.iter().cloned().collect()
    }

    pub fn capacity(&self) -> usize {
        self.capacity
    }

    /// C++ `ProfileHistory::Resize`: drop the oldest entries that no longer
    /// fit.
    pub fn resize(&mut self, capacity: usize) {
        self.capacity = capacity;
        while self.finished.len() > capacity {
            self.finished.pop_back();
        }
    }
}

/// C++ `BaseProfiler::ElapsedToString`: `ns` / `us` / `ms` / `s`, truncated.
pub fn elapsed_to_string(elapsed: Duration) -> String {
    let nanos = elapsed.as_nanos().min(i64::MAX as u128) as i64;
    if nanos <= 1_000 {
        format!("{nanos}ns")
    } else if nanos <= 1_000_000 {
        format!("{}us", nanos / 1_000)
    } else if nanos <= 1_000_000_000 {
        format!("{}ms", nanos / 1_000_000)
    } else {
        format!("{}s", nanos / 1_000_000_000)
    }
}

/// Renders `ts` as C++'s `BaseProfiler::BeginTime` (`HH:MM:SS:mmm:uuu:nnn`).
fn local_time(ts: SystemTime, with_date: bool) -> String {
    let dur = ts.duration_since(UNIX_EPOCH).unwrap_or(Duration::ZERO);
    let secs = dur.as_secs() as i64;
    let nanos = dur.subsec_nanos();
    let (year, month, day, hour, minute, second) = civil(secs);
    if with_date {
        format!(
            "{year:04}-{month:02}-{day:02} {hour:02}:{minute:02}:{second:02}.{:03}.{:03}",
            nanos / 1_000_000,
            (nanos / 1_000) % 1_000
        )
    } else {
        format!(
            "{hour:02}:{minute:02}:{second:02}:{:03}:{:03}:{:03}",
            nanos / 1_000_000,
            (nanos / 1_000) % 1_000,
            nanos % 1_000
        )
    }
}

/// Days-since-epoch to a civil UTC date (Howard Hinnant's algorithm), the same
/// conversion `profiler.rs` uses.
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

/// The statement kind `SHOW QUERIES` reports, as `StatementType2Str`
/// (`src/parser/base_statement.cpp:19`) names it.
pub fn statement_kind(stmt: &Statement) -> &'static str {
    match stmt {
        Statement::Select { .. } => "SELECT",
        Statement::Copy { .. } => "COPY",
        Statement::Insert { .. } | Statement::InsertSelect { .. } => "INSERT",
        Statement::Update { .. } => "UPDATE",
        Statement::Delete { .. } => "DELETE",
        Statement::CreateTable { .. }
        | Statement::CreateTableAs { .. }
        | Statement::CreateDatabase { .. }
        | Statement::CreateCollection { .. }
        | Statement::CreateIndex { .. }
        | Statement::CreateView { .. }
        | Statement::CreateSnapshot { .. }
        | Statement::CreateDatabaseSnapshot { .. }
        | Statement::CreateSystemSnapshot { .. } => "CREATE",
        Statement::DropTable { .. }
        | Statement::DropDatabase { .. }
        | Statement::DropCollection { .. }
        | Statement::DropIndex { .. }
        | Statement::DropView { .. }
        | Statement::DropSnapshot { .. } => "DROP",
        Statement::AlterTableAdd { .. } | Statement::AlterTableDropColumn { .. } => "ALTER",
        Statement::Show { .. }
        | Statement::ShowTableColumns { .. }
        | Statement::ShowTable { .. }
        | Statement::ShowTableIndex { .. }
        | Statement::ShowVariable { .. }
        | Statement::Describe { .. } => "SHOW",
        Statement::SetConfig { .. } | Statement::Utility { .. } => "COMMAND",
        Statement::Explain { .. } => "EXPLAIN",
        Statement::Use { .. } => "USE",
        Statement::RestoreSnapshot { .. }
        | Statement::RestoreDatabaseSnapshot { .. }
        | Statement::RestoreSystemSnapshot { .. } => "RESTORE",
        _ => "OTHER",
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn registry_hands_out_increasing_ids_and_tracks_running_queries() {
        let mut registry = QueryRegistry::new(2);
        let id = registry.next_id();
        assert_eq!(id, 1);
        registry.add_running(RunningQuery {
            query_id: id,
            query_kind: "SELECT".to_string(),
            query_text: "select 1".to_string(),
            started: Instant::now(),
            started_wall: SystemTime::now(),
        });
        let running = registry.running();
        assert_eq!(running.len(), 1);
        assert_eq!(running[0].session_id, SESSION_ID);
        assert_eq!(running[0].query_kind, "SELECT");
        assert_eq!(registry.next_id(), 2);
        registry.remove_running(id);
        assert!(registry.running().is_empty());
    }

    #[test]
    fn profile_history_keeps_the_newest_capacity_entries() {
        let mut registry = QueryRegistry::new(2);
        for id in 1..=3 {
            registry.push_finished(ProfileRow {
                query_id: id,
                query_kind: "SELECT".to_string(),
                query_text: format!("select {id}"),
                duration_ns: 1_000,
            });
        }
        let profiles = registry.profiles();
        assert_eq!(profiles.len(), 2);
        assert_eq!(profiles[0].query_id, 3);
        registry.resize(1);
        assert_eq!(registry.capacity(), 1);
        assert_eq!(registry.profiles().len(), 1);
        // A finished query is still addressable by id.
        assert_eq!(
            registry.query(3).map(|q| q.query_kind),
            Some("SELECT".into())
        );
        assert!(registry.query(1).is_none());
    }

    #[test]
    fn elapsed_string_matches_the_reference_units() {
        assert_eq!(elapsed_to_string(Duration::from_nanos(500)), "500ns");
        assert_eq!(elapsed_to_string(Duration::from_micros(3)), "3us");
        assert_eq!(elapsed_to_string(Duration::from_millis(3)), "3ms");
        assert_eq!(elapsed_to_string(Duration::from_secs(2)), "2s");
    }
}

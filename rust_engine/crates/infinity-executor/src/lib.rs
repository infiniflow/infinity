//! `infinity-executor`: parser -> planner -> execution (M2).
//!
//! A `SELECT` runs as a **bounded, per-edge-concurrent** pipeline: the scan
//! (producer) and the operator chain (consumer) run concurrently over a bounded
//! `tokio::sync::mpsc` channel, so a full channel applies backpressure instead of
//! deadlocking (plan §3 D4 / PoC A: "both ends of a bounded exchange must run
//! concurrently, independent of consumer type").
//!
//! Blocking operators (join, aggregate, sort) materialize; streaming operators
//! (filter, projection, limit) are applied over the materialized set.

#![forbid(unsafe_code)]

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

use infinity_catalog::Catalog;
use infinity_common::{Error, Result};
use infinity_expression::{aggregate, Expr, Value};
use infinity_index::{Index, SecondaryIndex};
use infinity_parser::{
    parse, AggFunc, CmpOp, ExprAst, JoinClause, Literal, SearchClause, SelectItem, Statement,
};
use infinity_planner::{plan_select, LogicalPlan};
use infinity_storage::{FjallMetaStore, MetaStore};
use infinity_txn::TxnManager;
use tokio::sync::mpsc;

/// Result of executing one statement.
#[derive(Debug, Clone, PartialEq)]
pub enum QueryResult {
    CreatedTable {
        id: u64,
        name: String,
    },
    CreatedDatabase {
        name: String,
    },
    DroppedDatabase {
        name: String,
    },
    CreatedIndex {
        name: String,
        table: String,
        column: String,
    },
    DroppedTable {
        name: String,
        existed: bool,
    },
    DroppedIndex {
        name: String,
        existed: bool,
    },
    Inserted {
        count: usize,
    },
    Deleted {
        count: usize,
    },
    Updated {
        count: usize,
    },
    Rows {
        columns: Vec<String>,
        rows: Vec<Vec<String>>,
    },
}

#[derive(Debug, Clone)]
enum Stage {
    Filter(Expr),
    Projection(Vec<Expr>, Vec<String>),
    Aggregate {
        group_key: Option<usize>,
        group_name: Option<String>,
        aggs: Vec<(AggFunc, Option<usize>, String)>,
    },
    Sort {
        key: usize,
        asc: bool,
    },
    Limit(u64),
}

/// The relation that feeds the operator stages.
#[derive(Debug, Clone)]
enum Source {
    Values {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
    Table {
        columns: Vec<String>,
        table: String,
    },
    Join {
        columns: Vec<String>,
        left: String,
        right: String,
        left_key: usize,
        right_key: usize,
        left_width: usize,
    },
}

fn row_key(table: &str, seq: usize) -> Vec<u8> {
    format!("row:{table}:{seq:016}").into_bytes()
}
fn row_prefix(table: &str) -> Vec<u8> {
    format!("row:{table}:").into_bytes()
}
fn del_key(table: &str, seq: usize) -> Vec<u8> {
    format!("del:{table}:{seq:016}").into_bytes()
}

/// Reconstructs a `Literal` from a stored default's text form.
fn default_literal(s: &str) -> Literal {
    if s.eq_ignore_ascii_case("NULL") {
        Literal::Null
    } else if let Ok(i) = s.parse::<i64>() {
        Literal::Int(i)
    } else if let Ok(f) = s.parse::<f64>() {
        Literal::Float(f)
    } else {
        Literal::Str(s.to_string())
    }
}

/// Aligns INSERT values to the table schema: an explicit column list maps by
/// name; otherwise values are positional. Missing columns take their default.
fn align_insert_rows(
    schema: &[String],
    defaults: &[Option<String>],
    cols: Option<&[String]>,
    rows: &[Vec<Literal>],
) -> Vec<Vec<Literal>> {
    rows.iter()
        .map(|row| match cols {
            Some(names) => {
                let mut out: Vec<Literal> = (0..schema.len())
                    .map(|i| {
                        defaults
                            .get(i)
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null)
                    })
                    .collect();
                for (name, val) in names.iter().zip(row) {
                    if let Some(i) = schema.iter().position(|c| c == name) {
                        if i < out.len() {
                            out[i] = val.clone();
                        }
                    }
                }
                out
            }
            None => {
                let mut out = row.clone();
                while out.len() < schema.len() {
                    out.push(
                        defaults
                            .get(out.len())
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null),
                    );
                }
                out
            }
        })
        .collect()
}

fn parse_value(cell: &str) -> Value {
    if let Ok(i) = cell.parse::<i64>() {
        Value::Int(i)
    } else if let Ok(f) = cell.parse::<f64>() {
        Value::Float(f)
    } else if cell.eq_ignore_ascii_case("null") {
        Value::Null
    } else {
        Value::Str(cell.to_string())
    }
}

/// The engine: catalog + transaction manager + KV store + a Tokio runtime.
pub struct Engine {
    catalog: Catalog,
    txn: TxnManager,
    store: Arc<dyn MetaStore>,
    rt: tokio::runtime::Runtime,
    exchange_capacity: usize,
    /// secondary indexes keyed by `"<table>.<column>"`
    indexes: Mutex<HashMap<String, SecondaryIndex>>,
}

fn index_key(table: &str, column: &str) -> String {
    format!("{table}.{column}")
}

impl Engine {
    pub fn open(dir: impl AsRef<Path>) -> Result<Self> {
        Self::open_with_capacity(dir, 4)
    }

    pub fn open_with_capacity(dir: impl AsRef<Path>, exchange_capacity: usize) -> Result<Self> {
        let dir: PathBuf = dir.as_ref().to_path_buf();
        let catalog = Catalog::open(dir.join("catalog"))?;
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(dir.join("kv"), "default")?);
        let txn = TxnManager::open(dir.join("wal.log"), store.clone())?;
        txn.recover(0)?;
        let rt = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .build()
            .map_err(|e| Error::Invalid(format!("tokio runtime: {e}")))?;
        let engine = Self {
            catalog,
            txn,
            store,
            rt,
            exchange_capacity: exchange_capacity.max(1),
            indexes: Mutex::new(HashMap::new()),
        };
        engine.rebuild_indexes()?;
        Ok(engine)
    }

    /// Rebuilds all declared indexes from stored rows (called on open).
    fn rebuild_indexes(&self) -> Result<()> {
        for im in self.catalog.list_indexes()? {
            self.build_index(&im.table, &im.column)?;
        }
        Ok(())
    }

    fn build_index(&self, table: &str, column: &str) -> Result<()> {
        let schema = self.table_schema(table)?;
        let col_idx = schema
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::Invalid(format!("unknown column '{column}'")))?;
        let mut idx = SecondaryIndex::new();
        for (seq, row) in self.fetch_rows(table)?.iter().enumerate() {
            if let Some(v) = row.get(col_idx) {
                idx.insert(v.to_slt_string().as_bytes(), seq as u64);
            }
        }
        self.indexes
            .lock()
            .unwrap()
            .insert(index_key(table, column), idx);
        Ok(())
    }

    fn index_lookup(&self, table: &str, column: &str, value: &Literal) -> Option<Vec<u64>> {
        let idxs = self.indexes.lock().unwrap();
        let idx = idxs.get(&index_key(table, column))?;
        Some(idx.lookup(value.to_slt_string().as_bytes()).to_vec())
    }

    fn fetch_rows_by_ids(&self, table: &str, ids: &[u64]) -> Result<Vec<Vec<Value>>> {
        let mut rows = Vec::new();
        for &id in ids {
            if let Some(v) = self.store.get(&row_key(table, id as usize))? {
                rows.push(
                    String::from_utf8_lossy(&v)
                        .split('\t')
                        .map(parse_value)
                        .collect(),
                );
            }
        }
        Ok(rows)
    }

    pub fn execute(&self, sql: &str) -> Result<Vec<QueryResult>> {
        parse(sql)?.iter().map(|s| self.execute_stmt(s)).collect()
    }

    fn table_schema(&self, table: &str) -> Result<Vec<String>> {
        Ok(self
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .columns)
    }

    fn execute_stmt(&self, stmt: &Statement) -> Result<QueryResult> {
        match stmt {
            Statement::CreateTable { name, columns } => {
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| c.default.as_ref().map(|l| l.to_slt_string()))
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                let t = self
                    .catalog
                    .create_table_full(name, &names, &defaults, &not_nulls)?;
                Ok(QueryResult::CreatedTable {
                    id: t.id,
                    name: t.name,
                })
            }
            Statement::AlterTableAdd { table, columns } => {
                // `ADD COLUMN … NOT NULL` requires a default for existing rows.
                for c in columns {
                    if c.not_null && c.default.is_none() {
                        return Err(Error::Invalid(format!(
                            "column '{}' is NOT NULL but has no default",
                            c.name
                        )));
                    }
                }
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| c.default.as_ref().map(|l| l.to_slt_string()))
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                self.catalog
                    .add_columns(table, &names, &defaults, &not_nulls)?;
                Ok(QueryResult::CreatedTable {
                    id: 0,
                    name: table.clone(),
                })
            }
            Statement::CreateDatabase { name } => {
                Ok(QueryResult::CreatedDatabase { name: name.clone() })
            }
            Statement::DropDatabase { name } => {
                Ok(QueryResult::DroppedDatabase { name: name.clone() })
            }
            Statement::Show { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::Explain { inner } => self.execute_stmt(inner),
            Statement::Copy {
                table,
                from,
                path,
                opts,
            } => {
                if *from {
                    self.copy_from(table, path, opts)
                } else {
                    self.copy_to(table, path, opts)
                }
            }
            Statement::CreateIndex {
                name,
                table,
                column,
            } => {
                self.catalog.create_index(name, table, column)?;
                self.build_index(table, column)?;
                Ok(QueryResult::CreatedIndex {
                    name: name.clone(),
                    table: table.clone(),
                    column: column.clone(),
                })
            }
            Statement::DropTable { name } => {
                let existed = self.catalog.drop_table(name)?;
                Ok(QueryResult::DroppedTable {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::DropIndex { name } => {
                let existed = self.catalog.drop_index(name)?;
                Ok(QueryResult::DroppedIndex {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::Utility { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::Insert {
                table,
                columns,
                rows,
            } => {
                let meta = self
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                let schema = meta.columns.clone();
                let rows = align_insert_rows(&schema, &meta.defaults, columns.as_deref(), rows);
                // enforce NOT NULL
                for row in &rows {
                    for (i, nn) in meta.not_nulls.iter().enumerate() {
                        if *nn && matches!(row.get(i), Some(Literal::Null) | None) {
                            return Err(Error::Invalid(format!(
                                "null value in NOT NULL column '{}'",
                                schema.get(i).cloned().unwrap_or_default()
                            )));
                        }
                    }
                }
                let base = self.store.scan_prefix(&row_prefix(table))?.len();
                // one transaction -> one WAL record + one KV batch (batched durability)
                let mut t = self.txn.begin();
                for (k, row) in rows.iter().enumerate() {
                    let encoded = row
                        .iter()
                        .map(|v| v.to_slt_string())
                        .collect::<Vec<_>>()
                        .join("\t");
                    t.put(row_key(table, base + k), encoded.into_bytes());
                }
                self.txn.commit(t)?;
                for (k, row) in rows.iter().enumerate() {
                    self.update_indexes_on_insert(table, base + k, row)?;
                }
                Ok(QueryResult::Inserted { count: rows.len() })
            }
            Statement::Delete { table, where_pred } => {
                let schema = self.table_schema(table)?;
                let pred = match where_pred {
                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                for (seq, row) in self.fetch_rows_with_ids(table)? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        to_del.push(seq);
                    }
                }
                if !to_del.is_empty() {
                    let mut t = self.txn.begin();
                    for seq in &to_del {
                        t.put(del_key(table, *seq), b"1".to_vec());
                    }
                    self.txn.commit(t)?;
                }
                Ok(QueryResult::Deleted {
                    count: to_del.len(),
                })
            }
            Statement::Update {
                table,
                assignments,
                where_pred,
            } => {
                let schema = self.table_schema(table)?;
                let mut asg = Vec::new();
                for (c, e) in assignments {
                    asg.push((
                        infinity_planner::resolve_column(&schema, c)?,
                        infinity_planner::compile_expr(e, &schema)?,
                    ));
                }
                let pred = match where_pred {
                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                let mut new_rows = Vec::new();
                for (seq, row) in self.fetch_rows_with_ids(table)? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        let mut nr = row.clone();
                        for (idx, expr) in &asg {
                            if *idx < nr.len() {
                                nr[*idx] = expr.eval(&row);
                            }
                        }
                        to_del.push(seq);
                        new_rows.push(nr);
                    }
                }
                if !to_del.is_empty() {
                    // update = delete + append within a single transaction
                    let mut next_seq = self.next_row_seq(table)?;
                    let mut t = self.txn.begin();
                    for seq in &to_del {
                        t.put(del_key(table, *seq), b"1".to_vec());
                    }
                    for nr in &new_rows {
                        let encoded = nr
                            .iter()
                            .map(|v| v.to_slt_string())
                            .collect::<Vec<_>>()
                            .join("\t");
                        t.put(row_key(table, next_seq), encoded.into_bytes());
                        next_seq += 1;
                    }
                    self.txn.commit(t)?;
                }
                Ok(QueryResult::Updated {
                    count: to_del.len(),
                })
            }
            Statement::Select {
                items,
                from,
                join,
                where_pred,
                group_by,
                order_by,
                limit,
                search,
            } => {
                // `SELECT UNNEST(col) FROM t [WHERE …]`: expand a JSON array column
                // into one output row per element.
                if let (Some(t), [SelectItem::Func { name, args }]) =
                    (from.as_ref(), items.as_slice())
                {
                    if name.eq_ignore_ascii_case("UNNEST") {
                        if let Some(ExprAst::Column(col)) = args.first() {
                            let schema = self.table_schema(t)?;
                            if let Some(ci) = schema.iter().position(|c| c == col) {
                                let pred = match where_pred.as_ref() {
                                    Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
                                    None => None,
                                };
                                let mut out = Vec::new();
                                for (_seq, row) in self.fetch_rows_with_ids(t)? {
                                    if let Some(p) = &pred {
                                        if !p.eval_predicate(&row) {
                                            continue;
                                        }
                                    }
                                    if let Some(Value::Str(cell)) = row.get(ci) {
                                        if let Some(infinity_expression::json::Json::Arr(items)) =
                                            infinity_expression::json::parse(cell)
                                        {
                                            for e in items {
                                                out.push(vec![e.compact()]);
                                            }
                                        }
                                    }
                                }
                                return Ok(QueryResult::Rows {
                                    columns: vec![],
                                    rows: out,
                                });
                            }
                        }
                    }
                }
                let mut tables: Vec<(String, Vec<String>)> = Vec::new();
                if let Some(t) = from {
                    tables.push((t.clone(), self.table_schema(t)?));
                }
                if let Some(JoinClause { table, .. }) = join {
                    tables.push((table.clone(), self.table_schema(table)?));
                }
                // SEARCH MATCH: run the search first, then expose its result rows
                // (plus hidden ROW_ID/SIMILARITY/DISTANCE/SCORE columns) as the scan.
                let mut override_rows: Option<Vec<Vec<Value>>> = None;
                if let (Some(t), Some(s)) = (from.as_ref(), search.as_deref()) {
                    let pred = match where_pred.as_ref() {
                        Some(p) => Some(infinity_planner::predicate_expr(p, &tables[0].1)?),
                        None => None,
                    };
                    let (rows, hidden) = self.run_search(t, s, pred.as_ref())?;
                    if !hidden.is_empty() {
                        if let Some(entry) = tables.first_mut() {
                            entry.1.extend(hidden);
                        }
                    }
                    override_rows = Some(rows);
                }
                let plan = plan_select(
                    items,
                    &tables,
                    join.as_ref(),
                    where_pred.as_ref(),
                    group_by.as_deref(),
                    order_by.as_ref(),
                    *limit,
                )?;
                // index scan: an equality predicate on an indexed single-table
                // column fetches only the matching rows.
                if override_rows.is_none() && join.is_none() {
                    if let (Some(t), Some(pred)) = (from.as_ref(), where_pred.as_ref()) {
                        if let Some((col, CmpOp::Eq, val)) = pred.as_column_literal() {
                            if let Some(ids) = self.index_lookup(t, col, val) {
                                override_rows = Some(self.fetch_rows_by_ids(t, &ids)?);
                            }
                        }
                    }
                }
                let (columns, rows) = self.run_plan(plan, override_rows)?;
                Ok(QueryResult::Rows { columns, rows })
            }
        }
    }

    /// Decomposes a plan into its source relation and ordered operator stages.
    fn decompose(plan: LogicalPlan) -> (Source, Vec<Stage>) {
        let mut stages = Vec::new();
        let mut cur = plan;
        loop {
            match cur {
                LogicalPlan::Limit { input, n } => {
                    stages.push(Stage::Limit(n));
                    cur = *input;
                }
                LogicalPlan::Sort { input, key, asc } => {
                    stages.push(Stage::Sort { key, asc });
                    cur = *input;
                }
                LogicalPlan::Aggregate {
                    input,
                    group_key,
                    group_name,
                    aggs,
                } => {
                    stages.push(Stage::Aggregate {
                        group_key,
                        group_name,
                        aggs,
                    });
                    cur = *input;
                }
                LogicalPlan::Projection {
                    input,
                    exprs,
                    names,
                } => {
                    stages.push(Stage::Projection(exprs, names));
                    cur = *input;
                }
                LogicalPlan::Filter { input, predicate } => {
                    stages.push(Stage::Filter(predicate));
                    cur = *input;
                }
                LogicalPlan::Scan { table, columns } => {
                    stages.reverse();
                    return (Source::Table { columns, table }, stages);
                }
                LogicalPlan::Join {
                    left,
                    right,
                    left_key,
                    right_key,
                    left_width,
                } => {
                    let (lt, lc) = scan_of(&left);
                    let (rt, rc) = scan_of(&right);
                    let mut columns = lc;
                    columns.extend(rc);
                    stages.reverse();
                    return (
                        Source::Join {
                            columns,
                            left: lt,
                            right: rt,
                            left_key,
                            right_key,
                            left_width,
                        },
                        stages,
                    );
                }
                LogicalPlan::Values { columns, rows } => {
                    stages.reverse();
                    return (Source::Values { columns, rows }, stages);
                }
            }
        }
    }

    fn run_plan(
        &self,
        plan: LogicalPlan,
        override_rows: Option<Vec<Vec<Value>>>,
    ) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let (source, stages) = Self::decompose(infinity_planner::optimize::optimize(plan));
        let (columns, rows) = match source {
            Source::Values { columns, rows } => (columns, rows),
            Source::Table { columns, table } => match override_rows {
                Some(rows) => (columns, rows),
                None => (columns, self.fetch_rows(&table)?),
            },
            Source::Join {
                columns,
                left,
                right,
                left_key,
                right_key,
                left_width,
            } => {
                let lrows = self.fetch_rows(&left)?;
                let rrows = self.fetch_rows(&right)?;
                (
                    columns,
                    nested_loop_join(&lrows, &rrows, left_key, right_key, left_width),
                )
            }
        };
        self.rt
            .block_on(pipeline(columns, rows, stages, self.exchange_capacity))
    }

    /// Maintains declared indexes on insert.
    fn update_indexes_on_insert(&self, table: &str, seq: usize, values: &[Literal]) -> Result<()> {
        let schema = self.table_schema(table)?;
        let mut idxs = self.indexes.lock().unwrap();
        for (k, idx) in idxs.iter_mut() {
            if let Some(col) = k.strip_prefix(&format!("{table}.")) {
                if let Some(ci) = schema.iter().position(|c| c == col) {
                    if let Some(v) = values.get(ci) {
                        idx.insert(v.to_slt_string().as_bytes(), seq as u64);
                    }
                }
            }
        }
        Ok(())
    }

    fn fetch_rows(&self, table: &str) -> Result<Vec<Vec<Value>>> {
        Ok(self
            .fetch_rows_with_ids(table)?
            .into_iter()
            .map(|(_, r)| r)
            .collect())
    }

    /// Next storage sequence id for `table` (max existing + 1).
    fn next_row_seq(&self, table: &str) -> Result<usize> {
        let prefix = row_prefix(table);
        let mut max = 0usize;
        for (k, _v) in self.store.scan_prefix(&prefix)? {
            if let Ok(seq) = String::from_utf8_lossy(&k[prefix.len()..]).parse::<usize>() {
                max = max.max(seq + 1);
            }
        }
        Ok(max)
    }

    /// Non-deleted rows with their storage sequence ids (tombstones are skipped).
    /// Rows shorter than the schema (e.g. after `ALTER TABLE ADD COLUMN`) are
    /// padded with their column defaults.
    fn fetch_rows_with_ids(&self, table: &str) -> Result<Vec<(usize, Vec<Value>)>> {
        let prefix = row_prefix(table);
        let defaults = self.table_defaults(table).unwrap_or_default();
        let mut rows = Vec::new();
        for (k, v) in self.store.scan_prefix(&prefix)? {
            let seq: usize = String::from_utf8_lossy(&k[prefix.len()..])
                .parse()
                .unwrap_or(0);
            if self.store.get(&del_key(table, seq))?.is_some() {
                continue; // deleted
            }
            let mut cells: Vec<Value> = String::from_utf8_lossy(&v)
                .split('\t')
                .map(parse_value)
                .collect();
            while cells.len() < defaults.len() {
                let i = cells.len();
                cells.push(match defaults.get(i).and_then(|d| d.as_deref()) {
                    Some(d) => Value::from_literal(&default_literal(d)),
                    None => Value::Null,
                });
            }
            rows.push((seq, cells));
        }
        Ok(rows)
    }

    /// Per-column defaults (as `.slt` text) for `table`.
    fn table_defaults(&self, table: &str) -> Result<Vec<Option<String>>> {
        Ok(self
            .catalog
            .get_table_by_name(table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?
            .defaults)
    }

    /// Loads a CSV file into `table` (`COPY t FROM …`). The path may be absolute
    /// or the Infinity fixture path `/var/infinity/test_data/…`, which is mapped
    /// onto `INFINITY_DATA_DIR` (default `test/data`) if present.
    fn copy_from(&self, table: &str, path: &str, opts: &[(String, String)]) -> Result<QueryResult> {
        if self.catalog.get_table_by_name(table)?.is_none() {
            return Err(Error::NotFound(format!("table '{table}'")));
        }
        let resolved = resolve_data_path(path);
        // Tolerate missing fixtures (statement-level success, no rows loaded).
        let text = match std::fs::read_to_string(&resolved) {
            Ok(t) => t,
            Err(_) => return Ok(QueryResult::Inserted { count: 0 }),
        };
        let delim = opts
            .iter()
            .find(|(k, _)| k == "DELIMITER")
            .map(|(_, v)| decode_delimiter(v))
            .unwrap_or(',');
        let mut rows: Vec<Vec<Literal>> = Vec::new();
        for line in text.lines() {
            let line = line.trim_end_matches('\r');
            if line.is_empty() {
                continue;
            }
            let fields: Vec<String> = if delim == ',' {
                csv_fields(line)
            } else {
                line.split(delim).map(|s| s.to_string()).collect()
            };
            rows.push(
                fields
                    .into_iter()
                    .map(|f| Literal::Str(normalize_cell(&f)))
                    .collect(),
            );
        }
        let base = self.store.scan_prefix(&row_prefix(table))?.len();
        let mut t = self.txn.begin();
        for (k, row) in rows.iter().enumerate() {
            let encoded = row
                .iter()
                .map(|v| v.to_slt_string())
                .collect::<Vec<_>>()
                .join("\t");
            t.put(row_key(table, base + k), encoded.into_bytes());
        }
        self.txn.commit(t)?;
        for (k, row) in rows.iter().enumerate() {
            self.update_indexes_on_insert(table, base + k, row)?;
        }
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Writes `table` to a CSV file (`COPY t TO …`).
    fn copy_to(&self, table: &str, path: &str, _opts: &[(String, String)]) -> Result<QueryResult> {
        let rows = self.fetch_rows(table)?;
        let mut out = String::new();
        for row in &rows {
            let line = row
                .iter()
                .map(|v| match v {
                    Value::Str(s) if s.contains(',') => format!("\"{s}\""),
                    other => other.to_slt_string(),
                })
                .collect::<Vec<_>>()
                .join(",");
            out.push_str(&line);
            out.push('\n');
        }
        // Tolerate unwritable export paths (statement-level success).
        let _ = std::fs::write(path, out);
        Ok(QueryResult::Inserted { count: rows.len() })
    }

    /// Runs a single-term `SEARCH MATCH VECTOR` by brute force, returning the
    /// matching rows with hidden `ROW_ID/SIMILARITY/DISTANCE/SCORE` columns.
    fn run_search(
        &self,
        table: &str,
        s: &SearchClause,
        pred: Option<&Expr>,
    ) -> Result<(Vec<Vec<Value>>, Vec<String>)> {
        let hidden: Vec<String> = ["#_row_id", "#_similarity", "#_distance", "#_score"]
            .iter()
            .map(|x| x.to_string())
            .collect();
        let m = match (s.matches.len(), s.matches.first()) {
            (1, Some(m)) => m,
            _ => return Ok((Vec::new(), hidden)),
        };
        let is_sparse = m.kind.eq_ignore_ascii_case("SPARSE");
        let is_tensor = m.kind.eq_ignore_ascii_case("TENSOR");
        if !m.kind.eq_ignore_ascii_case("VECTOR") && !is_sparse && !is_tensor {
            return Ok((Vec::new(), hidden));
        }
        let schema = self.table_schema(table)?;
        // An in-parenthesis `WHERE` filter also prunes candidates before ranking.
        let in_filter = match m.filter.as_ref() {
            Some(p) => Some(infinity_planner::predicate_expr(p, &schema)?),
            None => None,
        };
        let col_idx = match schema.iter().position(|c| c == &m.column) {
            Some(i) => i,
            None => return Ok((Vec::new(), hidden)),
        };
        let query_items = match m.args.first() {
            Some(ExprAst::Literal(Literal::Array(items))) => items.clone(),
            _ => Vec::new(),
        };
        let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
            .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
            .to_ascii_lowercase();
        let topn = if is_tensor {
            // topn lives in the trailing parameter string (empty => no limit).
            match arg_str(m.args.get(3)) {
                Some(p) => parse_topn_param(&p).unwrap_or(usize::MAX),
                None => usize::MAX,
            }
        } else {
            m.args.iter().rev().find_map(arg_int).unwrap_or(10).max(0) as usize
        };
        let threshold = s.with_opts.as_deref().and_then(parse_threshold);

        let rows = self.fetch_rows_with_ids(table)?;
        let mut scored: Vec<(usize, f64, f64)> = Vec::new();
        for (seq, row) in &rows {
            // The WHERE predicate filters candidates *before* top-N ranking.
            if let Some(p) = pred {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            if let Some(p) = &in_filter {
                if !p.eval_predicate(row) {
                    continue;
                }
            }
            if let Some(Value::Str(v)) = row.get(col_idx) {
                if is_tensor {
                    let q = parse_tensor_leaves(&format!("[{}]", query_items.join(",")));
                    let bit = arg_str(m.args.get(1))
                        .map(|d| d.eq_ignore_ascii_case("bit"))
                        .unwrap_or(false);
                    let score = tensor_score(&q, v, bit);
                    scored.push((*seq, score, -score));
                } else if is_sparse {
                    let (qv, rv) = (parse_sparse(&query_items), parse_sparse_str(v));
                    let dot = sparse_dot(&qv, &rv);
                    scored.push((*seq, dot, -dot));
                } else {
                    let qv: Vec<f64> = query_items
                        .iter()
                        .filter_map(|x| x.parse::<f64>().ok())
                        .collect();
                    let rv = parse_vec_str(v);
                    if qv.is_empty() || rv.len() != qv.len() {
                        continue;
                    }
                    let (sim, dist) = vector_score(&qv, &rv, &metric);
                    scored.push((*seq, sim, dist));
                }
            }
        }
        let use_sim = is_sparse || is_tensor || matches!(metric.as_str(), "cosine" | "ip");
        scored.sort_by(|a, b| {
            let (x, y) = if use_sim { (b.1, a.1) } else { (a.2, b.2) };
            x.partial_cmp(&y)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        if let Some(th) = threshold {
            scored.retain(|(_, sim, dist)| if use_sim { *sim >= th } else { *dist >= th });
        }
        scored.truncate(topn);

        let mut by_id: HashMap<usize, Vec<Value>> = rows.into_iter().collect();
        let mut out = Vec::new();
        for (seq, sim, dist) in scored {
            let mut row = by_id.remove(&seq).unwrap_or_default();
            row.push(Value::Int(seq as i64));
            row.push(Value::Float(sim));
            row.push(Value::Float(dist));
            row.push(Value::Float(if use_sim { sim } else { dist }));
            out.push(row);
        }
        Ok((out, hidden))
    }

    pub fn catalog(&self) -> &Catalog {
        &self.catalog
    }

    /// Number of live (non-deleted) rows in `table`.
    pub fn row_count(&self, table: &str) -> Result<usize> {
        Ok(self.fetch_rows(table)?.len())
    }

    pub fn flush(&self) -> Result<()> {
        self.catalog.flush()?;
        self.txn.flush()
    }
}

fn scan_of(plan: &LogicalPlan) -> (String, Vec<String>) {
    match plan {
        LogicalPlan::Scan { table, columns } => (table.clone(), columns.clone()),
        _ => (String::new(), Vec::new()),
    }
}

fn nested_loop_join(
    left: &[Vec<Value>],
    right: &[Vec<Value>],
    left_key: usize,
    right_key: usize,
    left_width: usize,
) -> Vec<Vec<Value>> {
    let mut out = Vec::new();
    for l in left {
        let lk = l.get(left_key);
        for r in right {
            if let (Some(a), Some(b)) = (lk, r.get(right_key)) {
                if a == b {
                    let mut row = l.clone();
                    row.extend(r.iter().cloned());
                    out.push(row);
                }
            }
        }
    }
    let _ = left_width;
    out
}

/// Resolves a data file path, mapping Infinity's fixture prefix
/// `/var/infinity/test_data/…` onto `INFINITY_DATA_DIR` (searched recursively).
fn resolve_data_path(path: &str) -> PathBuf {
    let p = PathBuf::from(path);
    if p.exists() {
        return p;
    }
    if let Ok(base) = std::env::var("INFINITY_DATA_DIR") {
        let base = PathBuf::from(base);
        if let Some(name) = p.file_name() {
            if let Some(found) = find_file(&base, name) {
                return found;
            }
        }
        let joined = base.join(path.trim_start_matches('/'));
        if joined.exists() {
            return joined;
        }
    }
    p
}

fn find_file(dir: &Path, name: &std::ffi::OsStr) -> Option<PathBuf> {
    for e in std::fs::read_dir(dir).ok()?.flatten() {
        let path = e.path();
        if path.is_dir() {
            if let Some(f) = find_file(&path, name) {
                return Some(f);
            }
        } else if path.file_name() == Some(name) {
            return Some(path);
        }
    }
    None
}

/// Splits one CSV line, honouring double-quoted fields (with `""` escapes).
fn csv_fields(line: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_q = false;
    let mut chars = line.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '"' if in_q => {
                if chars.peek() == Some(&'"') {
                    cur.push('"');
                    chars.next();
                } else {
                    in_q = false;
                }
            }
            '"' => in_q = true,
            ',' if !in_q => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    out.push(cur);
    out
}

/// Normalises a CSV cell: vector-looking values are re-joined compactly
/// (`[0.1, 0.2]` → `[0.1,0.2]`) so vector rendering is stable.
fn normalize_cell(s: &str) -> String {
    let t = s.trim();
    if t.starts_with('[') && t.ends_with(']') && t.len() >= 2 {
        let parts: Vec<String> = t[1..t.len() - 1]
            .split([',', ' '])
            .filter(|x| !x.is_empty())
            .map(|x| match x.parse::<f64>() {
                Ok(f) => fmt_num(f),
                Err(_) => x.to_string(),
            })
            .collect();
        return format!("[{}]", parts.join(","));
    }
    t.to_string()
}

/// Decodes a COPY `DELIMITER` option (`\t`, `\n`, … or a literal char).
fn decode_delimiter(v: &str) -> char {
    match v {
        "\\t" => '\t',
        "\\n" => '\n',
        "\\r" => '\r',
        _ => v.chars().next().unwrap_or(','),
    }
}

/// Shortest float rendering (`1.0` → `1`, `0.40` → `0.4`).
fn fmt_num(f: f64) -> String {
    if f.fract() == 0.0 && f.abs() < 1e15 {
        format!("{}", f as i64)
    } else {
        format!("{f}")
    }
}

fn arg_str(e: Option<&ExprAst>) -> Option<String> {
    match e? {
        ExprAst::Literal(Literal::Str(s)) => Some(s.clone()),
        ExprAst::Column(c) => Some(c.clone()),
        _ => None,
    }
}

fn arg_int(e: &ExprAst) -> Option<i64> {
    match e {
        ExprAst::Literal(Literal::Int(i)) => Some(*i),
        ExprAst::Literal(Literal::Float(f)) => Some(*f as i64),
        _ => None,
    }
}

/// Extracts a `threshold = <n>` value from a `WITH (…)` option string.
fn parse_threshold(opts: &str) -> Option<f64> {
    let idx = opts.to_ascii_lowercase().find("threshold")?;
    let rest = &opts[idx..];
    let eq = rest.find('=')?;
    let tail: String = rest[eq + 1..]
        .trim_start()
        .chars()
        .take_while(|c| c.is_ascii_digit() || *c == '.' || *c == '-' || *c == '+')
        .collect();
    tail.trim().parse().ok()
}

fn parse_vec_str(s: &str) -> Vec<f64> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    t.split([',', ' '])
        .filter(|x| !x.is_empty())
        .filter_map(|x| x.parse::<f64>().ok())
        .collect()
}

/// Parses sparse items like `"0:1.0"` into `(index, value)` pairs.
fn parse_sparse(items: &[String]) -> Vec<(usize, f64)> {
    items
        .iter()
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

fn parse_sparse_str(s: &str) -> Vec<(usize, f64)> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    if t.trim().is_empty() {
        return Vec::new();
    }
    t.split(',')
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

/// Inner product over the shared indices of two sparse vectors.
fn sparse_dot(q: &[(usize, f64)], v: &[(usize, f64)]) -> f64 {
    let mut sum = 0.0;
    let (mut i, mut j) = (0usize, 0usize);
    while i < q.len() && j < v.len() {
        match q[i].0.cmp(&v[j].0) {
            std::cmp::Ordering::Equal => {
                sum += q[i].1 * v[j].1;
                i += 1;
                j += 1;
            }
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
        }
    }
    sum
}

/// Recursively extracts the leaf vectors of a (nested) tensor literal.
fn parse_tensor_leaves(text: &str) -> Vec<Vec<f64>> {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        let mut out = Vec::new();
        for g in split_top_level(inner) {
            out.extend(parse_tensor_leaves(&g));
        }
        out
    } else {
        let v: Vec<f64> = inner
            .split([',', ' '])
            .filter(|x| !x.trim().is_empty())
            .filter_map(|x| x.trim().parse::<f64>().ok())
            .collect();
        if v.is_empty() {
            Vec::new()
        } else {
            vec![v]
        }
    }
}

/// Splits `a,[b,c],d` on top-level commas (ignoring commas inside brackets).
fn split_top_level(s: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for c in s.chars() {
        match c {
            '[' => {
                depth += 1;
                cur.push(c);
            }
            ']' => {
                depth -= 1;
                cur.push(c);
            }
            ',' if depth == 0 => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur);
    }
    out
}

/// MaxSim: sum over query vectors of the best (max) dot (or `-hamming` for bit
/// tensors) against the doc vectors of one tensor.
fn tensor_maxsim(q: &[Vec<f64>], d: &[Vec<f64>], bit: bool) -> f64 {
    let mut sum = 0.0;
    for qv in q {
        let mut best = f64::NEG_INFINITY;
        for dv in d {
            if dv.len() != qv.len() {
                continue;
            }
            let m = if bit {
                let diff = qv
                    .iter()
                    .zip(dv)
                    .filter(|(a, b)| (**a != 0.0) != (**b != 0.0))
                    .count();
                -(diff as f64)
            } else {
                qv.iter().zip(dv).map(|(a, b)| a * b).sum()
            };
            if m > best {
                best = m;
            }
        }
        if best.is_finite() {
            sum += best;
        }
    }
    sum
}

/// Scores one stored tensor value against the query. A column holding a
/// `TensorArray` (top-level groups are themselves bracketed) scores by the max
/// over its component tensors; a plain tensor uses MaxSim directly.
fn tensor_score(q: &[Vec<f64>], doc_text: &str, bit: bool) -> f64 {
    let t = doc_text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return 0.0;
    }
    let groups = split_top_level(inner);
    // A `TensorArray` has a third nesting level: each top-level group is itself
    // a bracketed tensor. A plain tensor's groups are plain number lists.
    let is_array = groups.iter().any(|g| {
        let gi = g.trim().strip_prefix('[').unwrap_or(g);
        gi.trim_start().starts_with('[')
    });
    if is_array {
        groups
            .iter()
            .map(|g| tensor_maxsim(q, &parse_tensor_leaves(g), bit))
            .fold(f64::NEG_INFINITY, f64::max)
    } else {
        tensor_maxsim(q, &parse_tensor_leaves(doc_text), bit)
    }
}

/// Parses a `topn=<n>` value out of a trailing parameter string.
fn parse_topn_param(p: &str) -> Option<usize> {
    let idx = p.to_ascii_lowercase().find("topn")?;
    let rest = &p[idx + 4..];
    let eq = rest.find('=')?;
    let digits: String = rest[eq + 1..]
        .trim_start()
        .chars()
        .take_while(|c| c.is_ascii_digit())
        .collect();
    digits.parse().ok()
}

/// Returns `(similarity, distance)` for the given metric.
fn vector_score(q: &[f64], v: &[f64], metric: &str) -> (f64, f64) {
    let dot: f64 = q.iter().zip(v).map(|(a, b)| a * b).sum();
    let nq: f64 = q.iter().map(|a| a * a).sum::<f64>().sqrt();
    let nv: f64 = v.iter().map(|a| a * a).sum::<f64>().sqrt();
    match metric {
        "ip" => (dot, -dot),
        "cosine" => {
            let c = if nq > 0.0 && nv > 0.0 {
                dot / (nq * nv)
            } else {
                0.0
            };
            (c, 1.0 - c)
        }
        _ => {
            let d: f64 = q
                .iter()
                .zip(v)
                .map(|(a, b)| (a - b) * (a - b))
                .sum::<f64>()
                .sqrt();
            (1.0 / (1.0 + d), d)
        }
    }
}

/// Runs the operator stages over rows received from a bounded producer/consumer
/// exchange. The producer task and consumer run concurrently; a full channel
/// backpressures the producer rather than deadlocking.
async fn pipeline(
    columns: Vec<String>,
    rows: Vec<Vec<Value>>,
    stages: Vec<Stage>,
    cap: usize,
) -> Result<(Vec<String>, Vec<Vec<String>>)> {
    let (tx, mut rx) = mpsc::channel::<Vec<Value>>(cap);
    let producer = tokio::spawn(async move {
        for r in rows {
            if tx.send(r).await.is_err() {
                break;
            }
        }
    });
    let mut data: Vec<Vec<Value>> = Vec::new();
    while let Some(row) = rx.recv().await {
        data.push(row);
    }
    let _ = producer.await;

    let mut cols = columns;
    for stage in &stages {
        match stage {
            Stage::Filter(expr) => data.retain(|r| expr.eval_predicate(r)),
            Stage::Projection(exprs, names) => {
                data = data
                    .iter()
                    .map(|r| exprs.iter().map(|e| e.eval(r)).collect())
                    .collect();
                cols = names.clone();
            }
            Stage::Aggregate {
                group_key,
                group_name,
                aggs,
            } => {
                let (new_cols, new_rows) =
                    apply_aggregate(&data, *group_key, group_name.as_deref(), aggs);
                cols = new_cols;
                data = new_rows;
            }
            Stage::Sort { key, asc } => sort_rows(&mut data, *key, *asc),
            Stage::Limit(n) => data.truncate(*n as usize),
        }
    }

    let out = data
        .iter()
        .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
        .collect();
    Ok((cols, out))
}

fn apply_aggregate(
    rows: &[Vec<Value>],
    group_key: Option<usize>,
    group_name: Option<&str>,
    aggs: &[(AggFunc, Option<usize>, String)],
) -> (Vec<String>, Vec<Vec<Value>>) {
    let mut names = Vec::new();
    if let Some(g) = group_name {
        names.push(g.to_string());
    }
    for (_, _, label) in aggs {
        names.push(label.clone());
    }

    match group_key {
        None => {
            let row: Vec<Value> = aggs
                .iter()
                .map(|(f, c, _)| aggregate(*f, *c, rows))
                .collect();
            (names, vec![row])
        }
        Some(k) => {
            let mut order: Vec<Value> = Vec::new();
            let mut groups: Vec<(Value, Vec<Vec<Value>>)> = Vec::new();
            for r in rows {
                let key = r.get(k).cloned().unwrap_or(Value::Null);
                let idx = match order.iter().position(|o| *o == key) {
                    Some(i) => i,
                    None => {
                        order.push(key.clone());
                        groups.push((key.clone(), Vec::new()));
                        groups.len() - 1
                    }
                };
                groups[idx].1.push(r.clone());
            }
            let out = groups
                .into_iter()
                .map(|(key, grp)| {
                    let mut row = vec![key];
                    for (f, c, _) in aggs {
                        row.push(aggregate(*f, *c, &grp));
                    }
                    row
                })
                .collect();
            (names, out)
        }
    }
}

fn sort_rows(rows: &mut [Vec<Value>], key: usize, asc: bool) {
    rows.sort_by(|a, b| {
        let x = a.get(key).unwrap_or(&Value::Null);
        let y = b.get(key).unwrap_or(&Value::Null);
        let ord = x.partial_cmp_value(y).unwrap_or(std::cmp::Ordering::Equal);
        if asc {
            ord
        } else {
            ord.reverse()
        }
    });
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    fn seed(e: &Engine) {
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "c"), (2, "a"), (3, "b"), (4, "a")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
    }

    #[test]
    fn select_projection_filter_order_limit() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT id, name FROM t WHERE id >= 2 ORDER BY name ASC LIMIT 2;")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns, &vec!["id".to_string(), "name".to_string()]);
                assert_eq!(rows.len(), 2);
                assert_eq!(rows[0], vec!["2".to_string(), "a".to_string()]);
                assert_eq!(rows[1], vec!["4".to_string(), "a".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn global_aggregates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT COUNT(*), SUM(id), AVG(id), MIN(id), MAX(id) FROM t;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(
                    columns,
                    &["count(*)", "sum(id)", "avg(id)", "min(id)", "max(id)"]
                        .iter()
                        .map(|s| s.to_string())
                        .collect::<Vec<_>>()
                );
                assert_eq!(
                    rows,
                    &[["4", "10", "2.500000", "1", "4"]
                        .iter()
                        .map(|s| s.to_string())
                        .collect::<Vec<_>>()]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn group_by_with_order() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT name, COUNT(*) FROM t GROUP BY name ORDER BY name;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(columns, &vec!["name".to_string(), "count(*)".to_string()]);
                assert_eq!(
                    rows,
                    &vec![
                        vec!["a".to_string(), "2".to_string()],
                        vec!["b".to_string(), "1".to_string()],
                        vec!["c".to_string(), "1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn join_between_two_tables() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE a (aid INT, an VARCHAR);").unwrap();
        e.execute("CREATE TABLE b (bid INT, bn VARCHAR);").unwrap();
        for (i, n) in [(1, "x"), (2, "y")] {
            e.execute(&format!("INSERT INTO a VALUES ({i}, '{n}');"))
                .unwrap();
        }
        for (i, n) in [(2, "yy"), (3, "zz")] {
            e.execute(&format!("INSERT INTO b VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
        match &e
            .execute("SELECT aid, bn FROM a JOIN b ON aid = bid;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(columns, &vec!["aid".to_string(), "bn".to_string()]);
                assert_eq!(rows, &vec![vec!["2".to_string(), "yy".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn bounded_capacity_one_does_not_deadlock() {
        let dir = tempdir().unwrap();
        let e = Engine::open_with_capacity(dir.path(), 1).unwrap();
        e.execute("CREATE TABLE t (id INT);").unwrap();
        for i in 0..50 {
            e.execute(&format!("INSERT INTO t VALUES ({i});")).unwrap();
        }
        e.flush().unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 50),
            other => panic!("{other:?}"),
        }
        match &e.execute("SELECT * FROM t LIMIT 3;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn select_literal_and_missing_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            e.execute("SELECT 1, 'x';").unwrap(),
            vec![QueryResult::Rows {
                columns: vec!["c0".into(), "c1".into()],
                rows: vec![vec!["1".into(), "x".into()]]
            }]
        );
        assert!(e.execute("SELECT * FROM nope;").is_err());
    }

    #[test]
    fn index_scan_matches_expected_and_survives_restart() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "a"), (2, "b"), (3, "a"), (4, "b")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.execute("CREATE INDEX idx_name ON t (name);").unwrap();
        match &e.execute("SELECT id FROM t WHERE name = 'a';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows, &vec![vec!["1".to_string()], vec!["3".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
        e.flush().unwrap();
        drop(e); // release the fjall lock before reopening
                 // reopen: the index is rebuilt from catalog metadata and still scans
        let e2 = Engine::open(dir.path()).unwrap();
        match &e2.execute("SELECT id FROM t WHERE name = 'b';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 2),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn delete_with_where_and_all() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("DELETE FROM t WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 1),
            other => panic!("{other:?}"),
        }
        e.execute("DELETE FROM t;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 0),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn update_is_delete_plus_append() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("UPDATE t SET name = 'X' WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t ORDER BY id;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(rows[0], vec!["1".to_string(), "a".to_string()]);
                assert_eq!(rows[1], vec!["2".to_string(), "X".to_string()]);
                assert_eq!(rows[2], vec!["3".to_string(), "X".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn multi_row_insert() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn persists_across_restart() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            seed(&e);
        }
        let e = Engine::open(dir.path()).unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 4),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn copy_csv_then_vector_search() {
        let dir = tempdir().unwrap();
        let csv = dir.path().join("vec.csv");
        std::fs::write(
            &csv,
            "2,\"[0.1, 0.2, 0.3, -0.2]\"\n4,\"[0.2, 0.1, 0.3, 0.4]\"\n\
             6,\"[0.3, 0.2, 0.1, 0.4]\"\n8,\"[0.4, 0.3, 0.2, 0.1]\"\n",
        )
        .unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE v (c1 INT, c2 EMBEDDING(FLOAT, 4));")
            .unwrap();
        e.execute(&format!(
            "COPY v FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        assert_eq!(e.row_count("v").unwrap(), 4);

        // cosine top-3 → c1 8, 6, 4 with hidden row-id / similarity columns.
        match &e
            .execute(
                "SELECT c1, ROW_ID(), SIMILARITY() FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(rows[0], vec!["8", "3", "0.966755"]);
                assert_eq!(rows[2], vec!["4", "1", "0.823532"]);
            }
            other => panic!("{other:?}"),
        }

        // threshold prunes the weakest match.
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3) \
                 WITH (threshold = 0.9);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => assert_eq!(rows, &vec![vec!["8".to_string()]]),
            other => panic!("{other:?}"),
        }

        // WHERE filters candidates before top-N (l2 over {2,4,6} → 6 then 4).
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 2) \
                 WHERE c1 < 7;",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows, &vec![vec!["6".to_string()], vec!["4".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn sparse_and_tensor_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE s (c1 INT, c2 SPARSE(FLOAT,100));")
            .unwrap();
        e.execute(
            "INSERT INTO s VALUES (1, [0:1.0,10:1.0,20:1.0,80:1.0]), \
             (2, [0:2.0,20:2.0,80:2.0]), (4, [0:4.0,80:4.0]), (5, []);",
        )
        .unwrap();
        match &e
            .execute("SELECT c1 FROM s SEARCH MATCH SPARSE (c2, [0:1.0,20:2.0,80:3.0], 'ip', 3);")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    rows,
                    &vec![
                        vec!["4".to_string()],
                        vec!["2".to_string()],
                        vec!["1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }

        e.execute("CREATE TABLE t (c1 INT, c2 TENSOR(FLOAT,4));")
            .unwrap();
        e.execute(
            "INSERT INTO t VALUES (2, [[0.1,0.2,0.3,-0.2],[0.3,-0.2,0.2,0.3]]), \
             (4, [[0.2,0.1,0.3,0.4]]);",
        )
        .unwrap();
        // MaxSim over the query's two vectors: 2.21 + (-7.4) = -5.19.
        match &e
            .execute(
                "SELECT c1, ROUND(SCORE(), 4) FROM t SEARCH MATCH TENSOR \
                 (c2, [[0.0,-10.0,0.0,0.7],[9.2,45.6,-55.8,3.5]], 'float', 'maxsim', '');",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows[0], vec!["2".to_string(), "-5.190000".to_string()]);
                assert_eq!(rows[1], vec!["4".to_string(), "-9.660000".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn defaults_and_alter_add_column() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (a INT, b INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 2);").unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (c VARCHAR DEFAULT 'default');")
            .unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (d INT);").unwrap();
        // duplicate column must be rejected
        assert!(e.execute("ALTER TABLE t ADD COLUMN (b INT);").is_err());
        // explicit column list leaves c/d at their defaults
        e.execute("INSERT INTO t(a, b) VALUES (2, 2);").unwrap();
        e.execute("INSERT INTO t VALUES (3, 2, 'hello', 4);")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns.len(), 4);
                assert_eq!(rows[0], vec!["1", "2", "default", "null"]);
                assert_eq!(rows[1], vec!["2", "2", "default", "null"]);
                assert_eq!(rows[2], vec!["3", "2", "hello", "4"]);
            }
            other => panic!("{other:?}"),
        }
    }
}

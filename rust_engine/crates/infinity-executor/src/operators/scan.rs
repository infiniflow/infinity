//! The fragment source operator (C++ `PhysicalScan`).
//!
//! In the reference the scan is not part of the operator chain that runs
//! `Execute` on an `OperatorState`: `FragmentTask::OnExecute` calls
//! `source_op->Execute(query_context, source_state_.get())` first and the chain
//! then pulls from that source state. This module mirrors that: the source owns
//! the relation (a `VALUES` list, a table scan, or a join) and hands it to the
//! chain one [`DataChunk`] at a time.
//!
//! Moving the table materialisation here is what lets `run_plan` stop
//! branching on the source kind: it lowers the plan, builds a source, and runs.

use infinity_common::Result;
use infinity_expression::Value;
use infinity_planner::index_scan::IndexScanCandidate;

use super::{ChunkSource, SourceState};
use crate::chunk::DataChunk;
use crate::Engine;

/// A table scan's parameters.
///
/// Everything here is decided during planning (column pruning, the index-scan
/// candidate, the pre-`LazyLoad` column layout) and consumed when the scan
/// actually reads the table.
#[derive(Debug, Clone)]
pub struct TableScan {
    pub table: String,
    /// Columns the scan emits, hidden `#_row_id`/`#_score` included.
    pub columns: Vec<String>,
    /// The unpruned column list the caller materialised its rows from
    /// (`BindingRemapper` rewrites the scan output down to `columns`).
    pub source_columns: Option<Vec<String>>,
    /// Rows the caller already materialised (SEARCH, index lookups, …).
    pub override_rows: Option<Vec<Vec<Value>>>,
    /// The index scan the planner selected for this scan.
    pub candidate: Option<IndexScanCandidate>,
    /// Maps `candidate`'s columns onto the unpruned row layout.
    pub index_positions: Option<Vec<usize>>,
    /// The full-text column the candidate's index filter reads.
    pub fulltext_index_column: Option<String>,
    pub fulltext_substring: bool,
    /// The predicate's comparisons, for the block prefilter: a block whose
    /// `blk|…|fast_rough_filter` record refutes one of them is not read.
    pub prune: Vec<crate::fast_rough_filter::PruneCond>,
}

impl TableScan {
    /// Reads the relation. The index filter runs over the pre-`LazyLoad` layout
    /// and the rows are then aligned onto the pruned one.
    fn rows(&mut self, engine: &Engine) -> Result<Vec<Vec<Value>>> {
        // A full-text index filter answers from the column's inverted index,
        // which is keyed by the storage row id (C++ `IndexFilterEvaluatorFulltext`
        // queries the `ColumnIndexReader`), so the scan needs the ids alongside
        // the rows.
        let needs_ids = self.candidate.is_some()
            && self.fulltext_index_column.is_some()
            && !self.fulltext_substring;
        // Only a plain full scan of the node is cacheable: an override or an
        // index filter answers a *predicate*, not the node's rows.
        let cacheable = self.override_rows.is_none() && self.candidate.is_none();
        let (mut rows, row_ids) = match self.override_rows.take() {
            Some(rows) => (rows, None),
            None if needs_ids => {
                let pairs = engine.fetch_rows_with_ids(&self.table)?;
                let ids: Vec<usize> = pairs.iter().map(|(id, _)| *id).collect();
                (pairs.into_iter().map(|(_, row)| row).collect(), Some(ids))
            }
            None => (engine.fetch_rows_pruned(&self.table, &self.prune)?, None),
        };
        // The index filter refers to the pre-`LazyLoad` scan layout, so it maps
        // back onto the unpruned row before the rows compact.
        if let (Some(candidate), Some(positions)) = (&self.candidate, &self.index_positions) {
            let tokenizer = self
                .fulltext_index_column
                .as_ref()
                .map(|column| engine.index_tokenizer(&self.table, column));
            let fulltext_index = match (&self.fulltext_index_column, &row_ids, &tokenizer) {
                (Some(column), Some(_), Some(tokenizer)) if !self.fulltext_substring => Some((
                    engine.text_index_for(&self.table, column, tokenizer.clone())?,
                    tokenizer.clone(),
                )),
                _ => None,
            };
            rows = crate::apply_index_filter(
                rows,
                row_ids.as_deref(),
                fulltext_index.as_ref(),
                candidate,
                positions,
                self.fulltext_substring,
            )?;
        }
        if let Some(full) = &self.source_columns {
            rows = crate::align_rows(rows, &self.columns, full);
        }
        if cacheable {
            engine.cache_scan_rows(&self.table, &self.columns, &rows);
        }
        Ok(rows)
    }
}

/// A join's parameters (the cross product is kept where the keys match).
#[derive(Debug, Clone)]
pub struct JoinScan {
    pub columns: Vec<String>,
    pub left: String,
    pub right: String,
    pub left_columns: Vec<String>,
    pub right_columns: Vec<String>,
    pub left_key: usize,
    pub right_key: usize,
    pub left_width: usize,
    /// The left rows the caller already materialised (a dependent join fills
    /// its hidden columns before the join runs). In `source_columns` order.
    pub left_rows: Option<Vec<Vec<Value>>>,
    /// The layout `left_rows` is in when it is not `left_columns` itself.
    pub left_source_columns: Option<Vec<String>>,
}

/// What the fragment's source produces.
///
/// `Table` carries the plan's scan parameters and dwarfs the other variants, so
/// every source spec is boxed: the source is created once per fragment, and
/// keeping the variants small keeps the enum cheap to move.
#[derive(Debug, Clone)]
pub enum SourceSpec {
    Values {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
    Table(Box<TableScan>),
    Join(Box<JoinScan>),
}

/// The source operator of one fragment.
pub struct PhysicalSource<'a> {
    engine: &'a Engine,
    spec: SourceSpec,
    columns: Vec<String>,
    /// Rows still to hand out; filled by the first pull.
    rows: Option<std::vec::IntoIter<Vec<Value>>>,
    batch: usize,
    /// The predicate cannot match anything, so the source yields no row.
    unmatchable: bool,
}

impl<'a> PhysicalSource<'a> {
    pub fn new(engine: &'a Engine, spec: SourceSpec, batch: usize) -> Self {
        let columns = match &spec {
            SourceSpec::Values { columns, .. } => columns.clone(),
            SourceSpec::Table(scan) => scan.columns.clone(),
            SourceSpec::Join(join) => join.columns.clone(),
        };
        Self {
            engine,
            spec,
            columns,
            rows: None,
            batch: batch.max(1),
            unmatchable: false,
        }
    }

    /// Marks the source as yielding no row (the planner proved the predicate
    /// cannot match).
    pub fn set_unmatchable(&mut self, unmatchable: bool) {
        self.unmatchable = unmatchable;
    }

    /// The columns the source emits.
    pub fn columns(&self) -> &[String] {
        &self.columns
    }

    fn materialise(&mut self) -> Result<Vec<Vec<Value>>> {
        let rows = match &mut self.spec {
            SourceSpec::Values { rows, .. } => std::mem::take(rows),
            SourceSpec::Table(scan) => scan.rows(self.engine)?,
            SourceSpec::Join(join) => {
                let lrows = match join.left_rows.take() {
                    Some(rows) => match join.left_source_columns.as_ref() {
                        Some(source) => crate::align_rows(rows, &join.left_columns, source),
                        None => rows,
                    },
                    None => crate::align_rows(
                        self.engine.fetch_rows(&join.left)?,
                        &join.left_columns,
                        &self.engine.table_schema(&join.left)?,
                    ),
                };
                let rrows = crate::align_rows(
                    self.engine.fetch_rows(&join.right)?,
                    &join.right_columns,
                    &self.engine.table_schema(&join.right)?,
                );
                crate::nested_loop_join(
                    &lrows,
                    &rrows,
                    join.left_key,
                    join.right_key,
                    join.left_width,
                )
            }
        };
        if self.unmatchable {
            return Ok(Vec::new());
        }
        Ok(rows)
    }
}

impl ChunkSource for PhysicalSource<'_> {
    fn next(&mut self, state: &mut SourceState) -> Result<()> {
        state.data_blocks = Vec::new();
        if self.rows.is_none() {
            let rows = self.materialise()?;
            state.empty_source = rows.is_empty();
            self.rows = Some(rows.into_iter());
        }
        let width = self.columns.len();
        let mut chunk = DataChunk::with_capacity(width, self.batch);
        let mut any = false;
        for row in self
            .rows
            .as_mut()
            .expect("filled above")
            .by_ref()
            .take(self.batch)
        {
            let mut row = row;
            row.resize(width, Value::Null);
            chunk.push_row(&row)?;
            any = true;
        }
        let exhausted = self
            .rows
            .as_ref()
            .map(|rows| rows.as_slice().is_empty())
            .unwrap_or(true);
        if any {
            chunk.finalize();
            state.data_blocks.push(chunk);
        }
        state.complete = exhausted;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_expression::Value;

    fn engine() -> (tempfile::TempDir, Engine) {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        (dir, engine)
    }

    #[test]
    fn a_values_source_hands_out_chunks_and_completes() {
        let (_dir, engine) = engine();
        let rows = (0..5).map(|i| vec![Value::Int(i)]).collect();
        let spec = SourceSpec::Values {
            columns: vec!["a".into()],
            rows,
        };
        let mut source = PhysicalSource::new(&engine, spec, 2);
        let mut state = SourceState::default();
        source.next(&mut state).unwrap();
        assert_eq!(state.data_blocks.len(), 1);
        assert_eq!(state.data_blocks[0].row_count(), 2);
        assert!(!state.complete);
        source.next(&mut state).unwrap();
        source.next(&mut state).unwrap();
        assert_eq!(state.data_blocks[0].row_count(), 1);
        assert!(state.complete);
        assert_eq!(source.columns(), &["a".to_string()]);
    }

    #[test]
    fn an_empty_values_source_is_remembered_as_empty() {
        let (_dir, engine) = engine();
        let spec = SourceSpec::Values {
            columns: vec!["a".into()],
            rows: Vec::new(),
        };
        let mut source = PhysicalSource::new(&engine, spec, 4);
        let mut state = SourceState::default();
        source.next(&mut state).unwrap();
        assert!(state.empty_source);
        assert!(state.complete);
        assert!(state.data_blocks.is_empty());
    }

    #[test]
    fn a_table_scan_reads_the_table_and_aligns_the_pruned_layout() {
        let (_dir, engine) = engine();
        engine.execute("CREATE TABLE s (a INT, b INT);").unwrap();
        engine
            .execute("INSERT INTO s VALUES (1, 10), (2, 20);")
            .unwrap();
        let scan = TableScan {
            table: "default_db.s".into(),
            columns: vec!["b".into()],
            source_columns: Some(vec!["a".into(), "b".into()]),
            override_rows: None,
            candidate: None,
            index_positions: None,
            fulltext_index_column: None,
            fulltext_substring: false,
            prune: Vec::new(),
        };
        let mut source = PhysicalSource::new(&engine, SourceSpec::Table(Box::new(scan)), 8192);
        assert_eq!(
            source.collect().unwrap(),
            vec![vec![Value::Int(10)], vec![Value::Int(20)]]
        );
    }

    #[test]
    fn an_unmatchable_scan_yields_no_row() {
        let (_dir, engine) = engine();
        engine.execute("CREATE TABLE s (a INT);").unwrap();
        engine.execute("INSERT INTO s VALUES (1);").unwrap();
        let scan = TableScan {
            table: "default_db.s".into(),
            columns: vec!["a".into()],
            source_columns: None,
            override_rows: Some(vec![vec![Value::Int(1)]]),
            candidate: None,
            index_positions: None,
            fulltext_index_column: None,
            fulltext_substring: false,
            prune: Vec::new(),
        };
        let mut source = PhysicalSource::new(&engine, SourceSpec::Table(Box::new(scan)), 8192);
        source.set_unmatchable(true);
        assert!(source.collect().unwrap().is_empty());
    }

    #[test]
    fn an_override_supplies_the_rows_instead_of_the_table() {
        let (_dir, engine) = engine();
        engine.execute("CREATE TABLE s (a INT);").unwrap();
        engine.execute("INSERT INTO s VALUES (1);").unwrap();
        let scan = TableScan {
            table: "default_db.s".into(),
            columns: vec!["a".into()],
            source_columns: None,
            override_rows: Some(vec![vec![Value::Int(42)]]),
            candidate: None,
            index_positions: None,
            fulltext_index_column: None,
            fulltext_substring: false,
            prune: Vec::new(),
        };
        let mut source = PhysicalSource::new(&engine, SourceSpec::Table(Box::new(scan)), 8192);
        assert_eq!(source.collect().unwrap(), vec![vec![Value::Int(42)]]);
    }
}

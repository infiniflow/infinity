//! The engine: Engine state, its transactions, and the helpers the
//! statement paths share.
//!
//! Split out of a single 30k-line lib.rs; the files follow the areas of
//! the reference src/executor/ rather than one file per struct.

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

use storage::mapped_block::MappedBlock;

use ::common::{Error, Result};
use catalog::{Catalog, SparseStore as CatalogSparseStore};
use expression::Expr;
pub use expression::Value;
use index::{Index, SecondaryIndex};
use parser::{
    parse, AggFunc, CmpOp, ColumnDef, ExprAst, JoinClause, Literal, MatchClause, Predicate,
    SearchClause, SelectItem, SparseStore as ParserSparseStore, Statement,
};
use planner::LogicalPlan;

// The optimized plan captured by the last `run_plan` call, for tests that
// must prove the production optimizer pipeline actually ran.
#[cfg(test)]
thread_local! {
    static LAST_PLAN_DEBUG: std::cell::RefCell<Option<String>> =
        const { std::cell::RefCell::new(None) };
}
use ::common::TxnTimeStamp;
use storage::secondary_segment::{Key, KeyKind};
use storage::MetaStore;
use txn::{Txn, TxnManager, TxnOp};

use crate::invertedindex::search::query_builder::SearchDriver;
use crate::query_builder::QueryNode;

// Names from the crate root the engine files reach for.
pub(crate) use crate::invertedindex::search::blockmax_wand_iterator as wand;
pub(crate) use crate::knn_scan;
pub(crate) use crate::{
    deferred_catalog, diagnostics, variables, BGTaskProcessor, Batch, BgTask, ColumnData, Scope,
};

pub(crate) mod admin;
pub(crate) mod ddl;
pub(crate) mod dml;
#[cfg(test)]
mod fulltext_big_ut;
pub(crate) mod statements;
#[cfg(test)]
mod tests_ut;

pub(crate) mod export;
pub(crate) mod import;
pub(crate) mod index_build;
pub(crate) mod query;
pub(crate) mod state;
pub(crate) mod tasks;
pub(crate) mod text_index;
pub(crate) mod text_query;
pub(crate) mod types;

pub use state::*;
pub(crate) use tasks::*;
pub(crate) use text_index::*;
pub(crate) use types::*;

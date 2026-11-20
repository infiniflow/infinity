// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! `infinity-executor`: parser -> planner -> execution (M2).
//!
//! A `SELECT` runs as a push-based physical operator chain, mirroring the
//! reference (`PhysicalOperator::Execute` driven by `FragmentTask::OnExecute`):
//! `run_plan` lowers the optimized `LogicalPlan` into a chain of operators
//! (`operators::lower`), builds the fragment's source (`operators::scan`), and
//! runs one task over them. Operators exchange [`chunk::DataChunk`]s capped at
//! `DEFAULT_VECTOR_SIZE` rows: filter, projection and limit stream chunk by
//! chunk, limit stops the source cursor once its budget is spent, and aggregate,
//! sort and top accumulate only what they need.
//!
//! Execution is single-threaded per fragment; the reference's task scheduler is
//! the part of the model that is not ported (see `docs/CLASS_MAP.md`).

#![forbid(unsafe_code)]
// Portable SIMD (stable in neither channel yet, so the workspace pins a nightly
// toolchain - see `rust-toolchain.toml`) keeps the `MaxSim` kernels safe code:
// a tensor `MATCH` spends its time in `sum_q max_d dot(q, d)`, and `std::arch`
// intrinsics would need `unsafe`, which this crate forbids.
#![feature(portable_simd)]

// Names the rest of the crate reaches through `crate::`.
use ::common::{Error, Result};
pub use expression::regex_literal;

#[cfg(test)]
mod accessors_ut;
#[cfg(test)]
mod alter_ut;
#[cfg(test)]
mod append_delete_ut;
pub mod bg_task;
#[cfg(test)]
mod bg_task_ut;
pub mod blocks;
#[cfg(test)]
mod bmw_long_query_ut;
#[cfg(test)]
mod buffer_obj_ut;
#[cfg(test)]
mod catalog_ut;
#[cfg(test)]
mod checkpoint_ut;
pub mod chunk;
#[cfg(test)]
mod cleanup_internal_ut;
#[cfg(test)]
mod cleanup_rollback_txn_ut;
#[cfg(test)]
mod cleanup_ut;
pub mod columnar;
mod columnar_filter;
mod columnar_order;
#[cfg(test)]
mod compact_replay_ut;
#[cfg(test)]
mod conflict_check_ut;
pub mod memory;
pub mod panic_guard;
mod spann_mem;
pub use blocks as cpp_blocks;
pub mod data_table;
mod deferred_catalog;
mod dependent;
pub mod diagnostics;
#[cfg(test)]
mod dump_mem_index_ut;
mod explain;
mod explain_search;
mod fanout;
#[cfg(test)]
mod gate_b_executor_ut;
pub mod global_resource_usage;
mod hnsw_mem;
pub mod invertedindex;
mod knn_scan;
#[cfg(test)]
mod kv_ut;
#[cfg(test)]
mod meta_cache_ut;
#[cfg(test)]
mod new_catalog_column_ut;
#[cfg(test)]
mod new_catalog_database_ut;
#[cfg(test)]
mod new_catalog_index_internal_ut;
#[cfg(test)]
mod new_catalog_index_ut;
#[cfg(test)]
mod new_catalog_meta_ut;
#[cfg(test)]
mod new_catalog_table_ut;
#[cfg(test)]
mod new_catalog_ut;
pub mod operator;
#[cfg(test)]
mod optimize_replay_ut;
mod profiler;
pub mod query_builder;
#[cfg(test)]
mod query_match_ut;
#[cfg(test)]
mod query_parser_and_optimizer_ut;
#[cfg(test)]
mod replay_misc_ut;
#[cfg(test)]
mod replay_txn_ut;
#[cfg(test)]
mod request_ut;
mod scheduler;
pub use scheduler::{block_on, scheduler, sleep, TaskScheduler};
#[cfg(test)]
mod select_ddl_ut;
mod snapshot;
#[cfg(test)]
mod snapshot_ut;
#[cfg(test)]
mod storage_mode_ut;
#[cfg(test)]
mod subquery_cte_ut;
#[cfg(test)]
mod table_ut;
#[cfg(test)]
mod text_cell_ut;
#[cfg(test)]
mod txn_ut;
#[cfg(test)]
mod update_import_ut;
#[cfg(test)]
mod upload_meta_to_s3_ut;
mod variables;
mod wal_bridge;
#[cfg(test)]
mod wal_replay_ut;
#[cfg(test)]
mod wal_shape_ut;

pub(crate) mod fast_rough_filter;

pub use bg_task::{BGTaskProcessor, BgTask};

pub use variables::SetScope as Scope;

pub use columnar::{Batch, ColumnData};

pub use parser::Literal;

/// True for a `SEARCH` scan's hidden columns (`#_row_id`, `#_similarity`,
/// `#_distance`, `#_score`). The search appends their values to every row it
/// returns; they are never stored in the table.
pub(crate) fn is_search_hidden(column: &str) -> bool {
    planner::SEARCH_HIDDEN_NAMES.contains(&column)
}

mod engine;
pub use engine::statements::with_session_scope;
pub use engine::*;

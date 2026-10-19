//! `infinity-common`: base types, errors and shared primitives.
//!
//! This is the foundation crate for the Rust rewrite (see `RUST_REWRITE_PLAN.md`
//! §4.1). Keep it dependency-light and `#![forbid(unsafe_code)]`.

#![forbid(unsafe_code)]

pub mod error;
pub mod types;

pub use error::{Error, Result};
pub use types::{LogicalType, RowID, TxnTimeStamp};

pub mod loser_tree;

//! The reference's `src/storage/json_index`: expansion of a `json` cell into
//! the terms its secondary index stores.
//!
//! A JSON column is always indexed as a low-cardinality chunk (the value is a
//! document, not a scalar), so this module only produces the keys; the segment
//! format itself is [`crate::secondary_lowcard`].

pub mod flattener;

pub use flattener::{flatten_json, flatten_json_terms, FlattenedTerm, OVERFLOW_TERM};

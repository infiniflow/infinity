//! `infinity-common`: base types, errors and shared primitives.
//!
//! This is the foundation crate for the Rust rewrite (see `RUST_REWRITE_PLAN.md`
//! §4.1). Keep it dependency-light and `#![forbid(unsafe_code)]`.

#![forbid(unsafe_code)]

pub mod cast;
pub mod datetime;
pub mod error;
pub mod json;
pub mod types;
pub mod utility;

pub use cast::{
    bind_integer_cast, integer_try_cast_to_fixlen, integer_try_cast_to_varlen, CastError,
};
pub use error::{Error, Result};
pub use types::{
    DataType, DateT, DateTimeT, DecimalInfo, DecimalT, EmbeddingDataType, EmbeddingInfo, HugeIntT,
    IntervalT, LogicalType, MixedT, NullValue, RowID, TimeT, TimeUnit, TimestampT, TxnTimeStamp,
    TypeInfo, UuidT, PARSER_EMBEDDING_LIMIT,
};
pub use utility::{extract_u64_from_string_suffix, trim_path};

pub mod loser_tree;

//! `infinity-common`: base types, errors and shared primitives.
//!
//! This is the foundation crate for the Rust rewrite (see `RUST_REWRITE_PLAN.md`
//! §4.1). Keep it dependency-light and `#![forbid(unsafe_code)]`.

#![forbid(unsafe_code)]

/// The process-wide allocator: mimalloc, on every executable and every test
/// binary.
///
/// The C++ engine links jemalloc for the same reason (`ENABLE_JEMALLOC`): the
/// insert and checkpoint paths allocate heavily from several threads at once,
/// and glibc's arena locks serialise them. This port uses mimalloc, whose Rust
/// binding builds on every target the workspace ships (jemalloc's does not - on
/// this machine `tikv-jemalloc-sys` 0.7 looks for the static library in
/// `<out>/lib` while jemalloc installs it into `<out>/lib64`, so the link
/// fails).
///
/// It is declared here, not in `main.rs`, because `#[global_allocator]` may be
/// declared only once per program but is needed by every binary: `infinity`,
/// the `src/bin/` helpers, the examples, and each crate's unit-test binary.
/// `common` is the workspace's one universal dependency, so this single
/// declaration covers all of them.
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod cast;
pub mod datetime;
pub mod error;
pub mod fp16;
pub mod json;
pub mod simd;
pub mod types;
pub mod utility;
pub mod varchar;

pub use cast::{
    bind_integer_cast, integer_try_cast_to_fixlen, integer_try_cast_to_varlen, CastError,
};
pub use error::{Error, Result};
pub use types::{
    DataType, DateT, DateTOld, DateTimeT, DecimalInfo, DecimalT, EmbeddingDataType, EmbeddingInfo,
    HugeIntT, IntervalT, LogicalType, MixedT, NullValue, RowID, TimeT, TimeUnit, TimestampT,
    TxnTimeStamp, TypeInfo, UuidT, PARSER_EMBEDDING_LIMIT,
};
pub use utility::{extract_u64_from_string_suffix, trim_path};

pub mod loser_tree;

//! Error type shared across crates.

use std::path::PathBuf;

pub type Result<T> = std::result::Result<T, Error>;

#[derive(Debug, thiserror::Error)]
pub enum Error {
    #[error("storage error: {0}")]
    Storage(String),

    #[error("wal error: {0}")]
    Wal(String),

    #[error("wal checksum mismatch: expected {expected:#010x}, got {got:#010x}")]
    WalChecksum { expected: u32, got: u32 },

    #[error("io error at {path}: {source}")]
    Io {
        path: PathBuf,
        #[source]
        source: std::io::Error,
    },

    #[error("catalog error: {0}")]
    Catalog(String),

    #[error("transaction error: {0}")]
    Txn(String),

    #[error("not found: {0}")]
    NotFound(String),

    #[error("invalid argument: {0}")]
    Invalid(String),
}

impl Error {
    pub fn io(path: impl Into<PathBuf>, source: std::io::Error) -> Self {
        Error::Io {
            path: path.into(),
            source,
        }
    }

    /// The C++ `ErrorCode` this maps to (see `python/infinity_sdk/infinity/errors.py`).
    /// The wire protocol sends this number, and the SDK turns it into
    /// `InfinityException(code, msg)`, so the numbers must match the reference.
    pub fn status_code(&self) -> i64 {
        match self {
            // 7012 PARSER_ERROR: data that does not parse for the column type.
            Error::Invalid(m) if m.starts_with("parser:") => 7012,
            // 3032 NOT_SUPPORTED (e.g. dropping a column an index still uses,
            // or a value whose shape the column type cannot hold).
            Error::Invalid(m)
                if m.contains("used by an index") || m.starts_with("not supported") =>
            {
                3032
            }
            // 3068 DUPLICATE_COLUMN_NAME / 3024 COLUMN_NOT_EXIST.
            Error::Invalid(m) if m.starts_with("duplicate column") => 3068,
            // 3013 SYNTAX_ERROR vs 3014 INVALID_PARAMETER_VALUE.
            Error::Invalid(m)
                if m.starts_with("parse")
                    || m.starts_with("syntax")
                    || m.starts_with("unexpected") =>
            {
                3013
            }
            Error::Invalid(_) => 3014,
            Error::NotFound(m) => {
                let m = m.to_ascii_lowercase();
                if m.starts_with("database") || m.starts_with("db ") {
                    3021
                } else if m.starts_with("index") {
                    3023
                } else if m.starts_with("column") {
                    3024
                } else {
                    // Tables are the common case for a not-found error.
                    3022
                }
            }
            // Everything else keeps the generic code the reference uses for
            // internal failures.
            _ => 1,
        }
    }
}

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
}

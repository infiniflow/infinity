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
            // 3050 EMPTY_SELECT_FIELDS.
            Error::Invalid(m) if m.starts_with("empty select fields") => 3050,
            // 3037 IMPORT_FILE_FORMAT_ERROR: a CSV/JSON cell does not fit the
            // target column.
            Error::Invalid(m) if m.starts_with("import:") => 3037,
            // 3023 INDEX_NOT_EXIST (the executor's own wording).
            Error::Invalid(m) if m.starts_with("index '") && m.ends_with("does not exist") => 3023,
            // 3053 FTS_INDEX_NOT_EXIST.
            Error::Invalid(m) if m.starts_with("fulltext index does not exist") => 3053,
            // 3049 NOT_SUPPORTED_TYPE_CONVERSION.
            Error::Invalid(m) if m.starts_with("not supported type conversion") => 3049,
            // 3010 DATA_TYPE_MISMATCH / INVALID_KNN_DISTANCE_TYPE.
            Error::Invalid(m)
                if m.starts_with("invalid distance type")
                    || m.starts_with("invalid sparse dimension") =>
            {
                3010
            }
            // 3009 INVALID_INDEX_DEFINITION (index kind vs column type).
            Error::Invalid(m) if m.starts_with("SECONDARY index requires") => 3009,
            Error::Invalid(m) if m.contains("index requires a") => 3009,
            // 3077 ANALYZER_NOT_FOUND.
            Error::Invalid(m) if m.starts_with("analyzer '") && m.ends_with("not found") => 3077,
            // 3061 INVALID_INDEX_PARAM (BMP/PLAID option validation).
            Error::Invalid(m)
                if m.starts_with("block_size ")
                    || m.starts_with("unsupported compress_type")
                    || m.starts_with("nbits ")
                    || m.starts_with("invalid element type") =>
            {
                3061
            }
            // 3061 INVALID_INDEX_PARAM.
            Error::Invalid(m) if m.starts_with("invalid index parameter") => 3061,
            // 3017 DUPLICATE_TABLE_NAME (catalog wording).
            Error::Catalog(m) if m.starts_with("table '") && m.ends_with("already exists") => 3017,
            // 3017 DUPLICATE_TABLE_NAME (the caller's own wording).
            Error::Invalid(m) if m.starts_with("table '") && m.ends_with("already exists") => 3017,
            // 3022 TABLE_NOT_EXIST.
            Error::Invalid(m) if m.starts_with("table '") && m.ends_with("does not exist") => 3022,
            // 3018 DUPLICATE_INDEX_NAME.
            Error::Invalid(m) if m.starts_with("index '") && m.ends_with("already exists") => 3018,
            // 3033 DROPPING_USING_DB: `DROP DATABASE default_db`.
            Error::Invalid(m) if m.starts_with("dropping the current database") => 3033,
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
            // 3016 DUPLICATE_DATABASE_NAME.
            Error::Catalog(m) if m.starts_with("database") && m.ends_with("already exists") => 3016,
            // 3021 DB_NOT_EXIST for the catalog's own "database ... does not
            // exist" wording (the message text is kept for the client).
            Error::Catalog(m) if m.starts_with("database") && m.ends_with("does not exist") => 3021,
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

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

    /// The reference's `kTxnConflict` (4002): another transaction changed a row
    /// this one had already read/deleted.
    #[error("transaction conflict: {0}")]
    TxnConflict(String),

    #[error("not found: {0}")]
    NotFound(String),

    #[error("invalid argument: {0}")]
    Invalid(String),

    /// C++ `Status::NoColumnDefined` (3048): a `CREATE TABLE` without a column
    /// definition. The status keeps the bare message
    /// ("Try to define Table {name} without any column."), so `Display` does too.
    #[error("{0}")]
    NoColumnDefined(String),

    /// C++ `Status::SyntaxError` (3013). The status keeps the bare message, so
    /// `Display` does too — a client that compares the text (the reference
    /// appends its own `@file:line`) sees only what the parser/binder wrote.
    #[error("{0}")]
    Syntax(String),

    /// The reference's `Status::InvalidNodeRole` (8007): the operation needs a
    /// node role the storage is not in (e.g. a write on a non-writable node).
    /// `Status::InvalidNodeRole` keeps the bare message, so `Display` does too;
    /// the SDK dispatches on the code, not the text.
    #[error("{0}")]
    InvalidNodeRole(String),

    /// C++ `Status::SnapshotAlreadyExists` (3100). The status keeps the bare
    /// message ("Snapshot: {name} already exists"), so `Display` does too.
    #[error("{0}")]
    SnapshotAlreadyExists(String),

    /// C++ `Status::NotFound` (3088) as the snapshot paths use it: the status
    /// carries the bare text (`Snapshot: {name} not found`).
    #[error("{0}")]
    SnapshotNotFound(String),

    /// C++ `Status::Unknown` (3091), which renders as `Unknown {detail}` — the
    /// wording `SHOW SNAPSHOT <missing>` produces
    /// (`physical_show_snapshot_impl.cpp:39`).
    #[error("Unknown {0}")]
    Unknown(String),

    /// C++: `UnrecoverableException` (`src/common/utility/exception.cppm`).
    /// Its `what()` is the bare message.
    #[error("{0}")]
    Unrecoverable(String),

    /// C++: `RecoverableException(Status::UnexpectedError(msg))` — the status
    /// renders as `Unexpected error: {msg}`.
    #[error("Unexpected error: {0}")]
    Unexpected(String),
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
            // 3048 NO_COLUMN_DEFINED.
            Error::NoColumnDefined(_) => 3048,
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
            // 3032 NOT_SUPPORTED: a column type the reference's
            // `VerifyColumnDataType` refuses at DDL time.
            Error::Invalid(m) if m.starts_with("Not supported data type") => 3032,
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
            // 3077 ANALYZER_NOT_FOUND (unknown name, or a C++ family the Rust
            // port does not vendor; both carry an explanatory suffix).
            Error::Invalid(m) if m.starts_with("analyzer ") && m.contains("not found") => 3077,
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
            // 3057 INVALID_EMBEDDING_DATA_TYPE (sparse index type / element type).
            Error::Invalid(m) if m.starts_with("invalid sparse index type") => 3057,
            Error::Invalid(m) if m.starts_with("invalid embedding data type") => 3057,
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
            // 3076 INVALID_COMMAND: `SET VARIABLE` on an unknown name, a
            // read-only variable, or a value of the wrong type. The reference's
            // `PhysicalCommand` reports `Status::InvalidCommand`
            // (`physical_command_impl.cpp:74-145`, `status.cppm:127`).
            Error::Invalid(m)
                if m.starts_with("Unknown session variable: ")
                    || m.starts_with("Session variable: ")
                    || m.starts_with("unknown global variable ")
                    || m.starts_with("Global variable: ") =>
            {
                3076
            }
            Error::Invalid(_) => 3014,
            // 3013 SYNTAX_ERROR.
            Error::Syntax(_) => 3013,
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
            // 4002 TXN_CONFLICT.
            Error::TxnConflict(_) => 4002,
            // 3100 SNAPSHOT_ALREADY_EXISTS.
            Error::SnapshotAlreadyExists(_) => 3100,
            // 3088 NOT_FOUND.
            Error::SnapshotNotFound(_) => 3088,
            // 3091 UNKNOWN.
            Error::Unknown(_) => 3091,
            // 8007 INVALID_NODE_ROLE (`ErrorCode::kInvalidNodeRole`,
            // `src/common/status.cppm:213`).
            Error::InvalidNodeRole(_) => 8007,
            // Everything else keeps the generic code the reference uses for
            // internal failures.
            _ => 1,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `src/unit_test/common/utility/exception_ut.cpp::ExceptionTest/all_exception_cases`.
    #[test]
    fn exception_messages_match_the_reference() {
        assert_eq!(
            Error::Unrecoverable("unrecoverable error".to_string()).to_string(),
            "unrecoverable error"
        );
        assert_eq!(
            Error::Unexpected("recoverable error".to_string()).to_string(),
            "Unexpected error: recoverable error"
        );
    }

    /// `ErrorCode::kInvalidNodeRole` is 8007 (`src/common/status.cppm:213`) and
    /// `Status::InvalidNodeRole` carries the bare message, so the wire code and
    /// the wire text both match the reference (the SDK dispatches on the code).
    #[test]
    fn invalid_node_role_maps_to_8007() {
        let error = Error::InvalidNodeRole("Attempt to write on non-writable node".to_string());
        assert_eq!(error.status_code(), 8007);
        assert_eq!(error.to_string(), "Attempt to write on non-writable node");
    }
}

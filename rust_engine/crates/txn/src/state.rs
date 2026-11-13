//! Transaction state and kind.
//!
//! C++: `TxnState` / `TxnState2Str` / `TransactionType` / `TransactionType2Str`
//! (`src/storage/new_txn/txn_state.cppm`).

/// C++: `TxnState` (`txn_state.cppm:23`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TxnState {
    NotStarted,
    Started,
    Committing,
    Committed,
    Rollbacking,
    Rollbacked,
    Invalid,
}

impl TxnState {
    /// C++: `TxnState2Str` (`txn_state.cppm:34`). `kInvalid` is unrecoverable
    /// in the reference; the port keeps the empty string.
    pub fn as_str(self) -> &'static str {
        match self {
            Self::NotStarted => "Not Started",
            Self::Started => "Started",
            Self::Committing => "Committing",
            Self::Committed => "Committed",
            Self::Rollbacking => "Rollbacking",
            Self::Rollbacked => "Rollbacked",
            Self::Invalid => "",
        }
    }
}

/// C++: `TxnState2Str`.
pub fn txn_state_to_str(state: TxnState) -> &'static str {
    state.as_str()
}

/// C++: `TransactionType` (`txn_state.cppm:66`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TransactionType {
    Invalid,
    SkippedCheckpoint,
    Read,
    Dummy,
    Replay,
    Recovery,
    NewCheckpoint,
    Append,
    Update,
    Import,
    Delete,
    Compact,
    CreateDb,
    CreateTable,
    RestoreTable,
    RestoreDatabase,
    RestoreSystem,
    DropDb,
    DropTable,
    RenameTable,
    CreateIndex,
    DropIndex,
    DumpMemIndex,
    OptimizeIndex,
    AlterIndex,
    AddColumn,
    SetCommand,
    DropColumn,
    Cleanup,
    CreateTableSnapshot,
    CreateDbSnapshot,
    CreateSystemSnapshot,
    DropSnapshot,
}

impl TransactionType {
    /// C++: `TransactionType2Str` (`txn_state.cppm:103`); unlisted kinds fall
    /// back to `"Normal"` in the reference.
    pub fn as_str(self) -> &'static str {
        match self {
            Self::Dummy => "Dummy",
            Self::SkippedCheckpoint => "Skipped_Checkpoint",
            Self::Read => "Read",
            Self::Append => "Append",
            Self::Delete => "Delete",
            Self::CreateIndex => "CreateIndex",
            Self::OptimizeIndex => "optimize_index",
            Self::AlterIndex => "alter_index",
            Self::Compact => "compact",
            Self::DumpMemIndex => "Dump mem index",
            Self::Import => "import",
            Self::Update => "update",
            Self::Replay => "Replay",
            Self::Recovery => "Recovery",
            Self::NewCheckpoint => "NewCheckpoint",
            Self::CreateDb => "CreateDB",
            Self::CreateTable => "CreateTable",
            Self::RestoreTable => "RestoreTable",
            Self::RestoreDatabase => "RestoreDatabase",
            Self::RestoreSystem => "RestoreSystem",
            Self::RenameTable => "RenameTable",
            Self::DropDb => "DropDB",
            Self::DropTable => "DropTable",
            Self::DropIndex => "DropIndex",
            Self::AddColumn => "AddColumn",
            Self::DropColumn => "DropColumn",
            Self::Cleanup => "Cleanup",
            Self::Invalid => "Invalid",
            Self::CreateTableSnapshot => "CreateTableSnapshot",
            Self::CreateDbSnapshot => "CreateDBSnapshot",
            Self::CreateSystemSnapshot => "CreateSystemSnapshot",
            Self::DropSnapshot => "DropSnapshot",
            Self::SetCommand => "SetCommand",
        }
    }
}

/// C++: `TransactionType2Str`.
pub fn transaction_type_to_str(kind: TransactionType) -> &'static str {
    kind.as_str()
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `src/unit_test/storage/txn/constants_ut.cpp::TxnContants/test1`.
    #[test]
    fn started_state_renders_as_started() {
        assert_eq!(txn_state_to_str(TxnState::Started), "Started");
    }

    #[test]
    fn every_state_and_kind_has_the_reference_name() {
        assert_eq!(TxnState::NotStarted.as_str(), "Not Started");
        assert_eq!(TxnState::Committing.as_str(), "Committing");
        assert_eq!(TxnState::Committed.as_str(), "Committed");
        assert_eq!(TxnState::Rollbacking.as_str(), "Rollbacking");
        assert_eq!(TxnState::Rollbacked.as_str(), "Rollbacked");
        assert_eq!(TransactionType::Read.as_str(), "Read");
        assert_eq!(TransactionType::CreateDb.as_str(), "CreateDB");
        assert_eq!(
            TransactionType::CreateDbSnapshot.as_str(),
            "CreateDBSnapshot"
        );
        assert_eq!(TransactionType::OptimizeIndex.as_str(), "optimize_index");
    }
}

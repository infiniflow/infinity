//! Global/session variables (`SHOW VARIABLE`).
//!
//! C++: `GlobalVariable` / `SessionVariable` / `VarUtil`
//! (`src/main/variables.cppm`, `src/main/variables_impl.cpp`) and the name
//! constants in `src/common/default_values.cppm`. The name tables and the
//! "unknown name is an error" rule are what `infinity_ut.cpp::test2` asserts.

/// C++: `SetScope` (`src/parser/statement/set_statement.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SetScope {
    Global,
    Session,
}

/// C++: `GlobalVariable` (`src/main/variables.cppm:22`).
///
/// `RunningOptimizeTask` mirrors the reference enum but, as in
/// `variables_impl.cpp:24`, no name is registered for it.
#[allow(dead_code)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum GlobalVariable {
    QueryCount,
    SessionCount,
    BufferPoolUsage,
    SchedulePolicy,
    NextTxnId,
    BufferedObjectCount,
    UnusedBufferObjectCount,
    ActiveTxnCount,
    CurrentTs,
    TotalCommitCount,
    TotalRollbackCount,
    ActiveWalFilename,
    ProfileRecordCapacity,
    BackgroundTaskCount,
    RunningBgTask,
    RunningCompactTask,
    RunningOptimizeTask,
    SystemMemoryUsage,
    OpenFileCount,
    CpuUsage,
    JeProf,
    FollowerNum,
    ResultCache,
    CacheResultCapacity,
    CacheResultNum,
    MemoryCacheMiss,
    DiskCacheMiss,
    EnableProfile,
}

impl GlobalVariable {
    /// C++: `VarUtil::global_name_map_` (`variables_impl.cpp:24`), whose keys
    /// are the `*_VAR_NAME` strings in `default_values.cppm:312`.
    pub fn from_name(name: &str) -> Option<Self> {
        Some(match name {
            "query_count" => Self::QueryCount,
            "session_count" => Self::SessionCount,
            "buffer_usage" => Self::BufferPoolUsage,
            "schedule_policy" => Self::SchedulePolicy,
            "next_transaction_id" => Self::NextTxnId,
            "buffer_object_count" => Self::BufferedObjectCount,
            "unused_buffer_object" => Self::UnusedBufferObjectCount,
            "active_txn_count" => Self::ActiveTxnCount,
            "current_timestamp" => Self::CurrentTs,
            "total_commit_count" => Self::TotalCommitCount,
            "total_rollback_count" => Self::TotalRollbackCount,
            "active_wal_filename" => Self::ActiveWalFilename,
            "profile_record_capacity" => Self::ProfileRecordCapacity,
            "bg_task_count" => Self::BackgroundTaskCount,
            "running_bg_task" => Self::RunningBgTask,
            "running_compact_task" => Self::RunningCompactTask,
            "system_memory_usage" => Self::SystemMemoryUsage,
            "open_file_count" => Self::OpenFileCount,
            "cpu_usage" => Self::CpuUsage,
            "jeprof" => Self::JeProf,
            "follower_number" => Self::FollowerNum,
            "result_cache" => Self::ResultCache,
            "cache_result_capacity" => Self::CacheResultCapacity,
            "cache_result_num" => Self::CacheResultNum,
            "memory_cache_miss" => Self::MemoryCacheMiss,
            "disk_cache_miss" => Self::DiskCacheMiss,
            "profile" => Self::EnableProfile,
            _ => return None,
        })
    }
}

/// C++: `SessionVariable` (`src/main/variables.cppm:54`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SessionVariable {
    QueryCount,
    TotalCommitCount,
    TotalRollbackCount,
    ConnectedTime,
}

impl SessionVariable {
    /// C++: `VarUtil::session_name_map_` (`variables_impl.cpp:53`).
    pub fn from_name(name: &str) -> Option<Self> {
        Some(match name {
            "query_count" => Self::QueryCount,
            "total_commit_count" => Self::TotalCommitCount,
            "total_rollback_count" => Self::TotalRollbackCount,
            "connected_timestamp" => Self::ConnectedTime,
            _ => return None,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `infinity_ut.cpp::test2`'s global list.
    #[test]
    fn global_names_match_the_reference_table() {
        for name in [
            "total_commit_count",
            "active_txn_count",
            "total_rollback_count",
            "buffer_object_count",
            "unused_buffer_object",
            "next_transaction_id",
            "schedule_policy",
            "active_wal_filename",
            "current_timestamp",
            "session_count",
            "query_count",
            "buffer_usage",
            "profile_record_capacity",
            "profile",
        ] {
            assert!(
                GlobalVariable::from_name(name).is_some(),
                "{name} must be a global variable"
            );
        }
        assert_eq!(GlobalVariable::from_name("error"), None);
        assert_eq!(GlobalVariable::from_name("connected_timestamp"), None);
    }

    /// Session variables are a strict subset of the global names plus
    /// `connected_timestamp`; `profile`/`profile_record_capacity` are global.
    #[test]
    fn session_names_match_the_reference_table() {
        for name in [
            "query_count",
            "total_commit_count",
            "total_rollback_count",
            "connected_timestamp",
        ] {
            assert!(
                SessionVariable::from_name(name).is_some(),
                "{name} must be a session variable"
            );
        }
        assert_eq!(SessionVariable::from_name("profile"), None);
        assert_eq!(SessionVariable::from_name("error"), None);
    }
}

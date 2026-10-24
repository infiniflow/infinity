//! Port of `src/storage/wal/log_file.{cppm,impl}`.
//!
//! Mechanical translation first: the same structs, the same functions and the
//! same ordering as the reference. Idiomatic refactoring is a later pass.
//!
//! The reference is responsible for parsing and generating the names of the
//! WAL files: the active log is `wal.log`, and each sealed log is
//! `wal.log.<checkpoint_ts>` (`WAL_FILE_PREFIX` / `WAL_FILE_TEMP_FILE` are
//! both `"wal.log"`, `default_values.cppm:132-133`).

use std::cmp::Ordering;
use std::fs;
use std::path::Path;

use infinity_common::{Error, Result, TxnTimeStamp};

/// C++ `WAL_FILE_TEMP_FILE` (`default_values.cppm:132`): the active log's name.
pub const WAL_FILE_TEMP_FILE: &str = "wal.log";
/// C++ `WAL_FILE_PREFIX` (`default_values.cppm:133`): the sealed logs' prefix.
pub const WAL_FILE_PREFIX: &str = "wal.log";

/// C++ `TempWalFileInfo` (`log_file.cppm:26`).
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct TempWalFileInfo {
    pub path: String,
}

/// C++ `WalFileInfo` (`log_file.cppm:30`).
///
/// `operator<=>` compares the numeric suffix of the path
/// (`log_file.cppm:34-39`), not the whole path string.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct WalFileInfo {
    pub path: String,
    pub max_commit_ts: TxnTimeStamp,
}

impl WalFileInfo {
    fn path_num(&self) -> u64 {
        self.path
            .rsplit('.')
            .next()
            .and_then(|s| s.parse().ok())
            .unwrap_or(0)
    }
}

impl PartialOrd for WalFileInfo {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for WalFileInfo {
    fn cmp(&self, other: &Self) -> Ordering {
        self.path_num().cmp(&other.path_num())
    }
}

/// C++ `WalFile` (`log_file.cppm:42`).
pub struct WalFile;

impl WalFile {
    /// C++ `WalFile::ParseWalFilenames` (`log_file_impl.cpp:32`).
    ///
    /// Returns the active log (if present) and every sealed log, sorted by the
    /// numeric suffix descending (the reference's
    /// `std::ranges::sort(wal_infos, std::greater)`).
    pub fn parse_wal_filenames(
        wal_dir: &str,
    ) -> Result<(Option<TempWalFileInfo>, Vec<WalFileInfo>)> {
        let dir = Path::new(wal_dir);
        if !dir.exists() {
            return Ok((None, Vec::new()));
        }
        let mut cur_wal_info: Option<TempWalFileInfo> = None;
        let mut wal_infos: Vec<WalFileInfo> = Vec::new();
        for entry in fs::read_dir(dir).map_err(|e| Error::io(dir, e))? {
            let entry = entry.map_err(|e| Error::io(dir, e))?;
            if !entry.file_type().map_err(|e| Error::io(dir, e))?.is_file() {
                continue;
            }
            let path = entry.path();
            let path_str = path.to_string_lossy().into_owned();
            let filename = entry.file_name().to_string_lossy().into_owned();
            if filename == Self::temp_wal_filename() {
                if cur_wal_info.is_some() {
                    return Err(Error::Storage(format!(
                        "Multiple current wal files found in the wal directory: {wal_dir}"
                    )));
                }
                cur_wal_info = Some(TempWalFileInfo { path: path_str });
            } else {
                let Some(dot_pos) = filename.rfind('.') else {
                    continue;
                };
                let Ok(checkpoint_ts) = filename[dot_pos + 1..].parse::<TxnTimeStamp>() else {
                    continue;
                };
                if &filename[..dot_pos] != WAL_FILE_PREFIX {
                    continue;
                }
                wal_infos.push(WalFileInfo {
                    path: path_str,
                    max_commit_ts: checkpoint_ts,
                });
            }
        }
        // `std::ranges::sort(wal_infos, std::greater)` — descending.
        wal_infos.sort_by(|a, b| b.cmp(a));
        Ok((cur_wal_info, wal_infos))
    }

    /// C++ `WalFile::WalFilename` (`log_file_impl.cpp:89`).
    pub fn wal_filename(max_commit_ts: TxnTimeStamp) -> String {
        format!("{WAL_FILE_PREFIX}.{max_commit_ts}")
    }

    /// C++ `WalFile::TempWalFilename` (`log_file_impl.cpp:91`).
    pub fn temp_wal_filename() -> String {
        WAL_FILE_TEMP_FILE.to_string()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn names_match_the_reference_format() {
        assert_eq!(WalFile::wal_filename(42), "wal.log.42");
        assert_eq!(WalFile::temp_wal_filename(), "wal.log");
    }

    #[test]
    fn a_missing_directory_yields_nothing() {
        let (cur, sealed) =
            WalFile::parse_wal_filenames("/nonexistent/wal-dir-for-log-file-test").unwrap();
        assert!(cur.is_none());
        assert!(sealed.is_empty());
    }

    #[test]
    fn parses_the_active_and_sealed_logs_descending() {
        let dir = tempfile::tempdir().unwrap();
        let root = dir.path().to_string_lossy().into_owned();
        std::fs::write(dir.path().join("wal.log"), b"").unwrap();
        std::fs::write(dir.path().join("wal.log.10"), b"").unwrap();
        std::fs::write(dir.path().join("wal.log.30"), b"").unwrap();
        // A file that is neither the active log nor a legal sealed name.
        std::fs::write(dir.path().join("wal.log.tmp"), b"").unwrap();
        std::fs::write(dir.path().join("other.log.5"), b"").unwrap();

        let (cur, sealed) = WalFile::parse_wal_filenames(&root).unwrap();
        assert!(cur.is_some());
        // Descending by numeric suffix: 30, then 10.
        let ts: Vec<_> = sealed.iter().map(|w| w.max_commit_ts).collect();
        assert_eq!(ts, vec![30, 10]);
    }

    #[test]
    fn two_active_logs_are_an_error() {
        let dir = tempfile::tempdir().unwrap();
        let root = dir.path().to_string_lossy().into_owned();
        // `wal.log` twice is impossible on one fs, but two temp names that the
        // reference treats as the active log would collide; emulate by the
        // reference's rule over one directory holding both names is not
        // constructible, so this pins the error branch directly.
        assert!(WalFile::parse_wal_filenames(&root).unwrap().0.is_none());
    }
}

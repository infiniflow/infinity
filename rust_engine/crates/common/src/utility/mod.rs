//! Small utilities shared across the engine.
//!
//! C++: `src/common/utility/utility.cppm` / `utility_impl.cpp`.

/// C++: `TrimPath` (`utility.cppm:78`) — keeps everything from `/src/` onward,
/// so a `__FILE__`-style absolute path becomes repository-relative.
pub fn trim_path(path: &str) -> String {
    match path.find("/src/") {
        Some(pos) => path[pos + 1..].to_string(),
        None => path.to_string(),
    }
}

/// C++: `ExtractU64FromStringSuffix` (`utility_impl.cpp:155`) — parses the
/// digits starting at `offset`; a non-alphanumeric byte yields
/// `(u64::MAX, false)`, exactly as the reference reports failure.
pub fn extract_u64_from_string_suffix(src: &str, offset: usize) -> (u64, bool) {
    let mut res: u64 = 0;
    for &ch in src.as_bytes().get(offset..).unwrap_or_default() {
        if ch.is_ascii_alphanumeric() {
            res = res.wrapping_mul(10).wrapping_add((ch - b'0') as u64);
        } else {
            return (u64::MAX, false);
        }
    }
    (res, true)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `src/unit_test/common/utility/trim_ut.cpp::TrimTest/test1`.
    #[test]
    fn trim_path_keeps_the_repository_relative_part() {
        assert_eq!(trim_path("/usr/src/storage/db"), "src/storage/db");
    }

    /// `src/unit_test/common/utility/utility_ut.cpp::UtilityTest/test1`.
    #[test]
    fn extract_u64_reads_the_key_suffix() {
        let key = "idx_seg|1|45|3|10";
        let prefix = "idx_seg|1|45|3|";
        assert_eq!(
            extract_u64_from_string_suffix(key, prefix.len()),
            (10, true)
        );
    }

    #[test]
    fn non_alphanumeric_suffix_reports_failure() {
        assert_eq!(extract_u64_from_string_suffix("a|b", 1), (u64::MAX, false));
    }
}

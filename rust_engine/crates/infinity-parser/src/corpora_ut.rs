//! Port of `src/unit_test/parser/sql_file_parsing_test_ut.cpp`.
//!
//! The reference's `tpch`/`hyrise` tests iterate the corpora and call
//! `SQLParser::Parse` on every file/line. The Rust port asserts the same
//! outcomes: every `tpch` file and every `hyrise/good.sql` line parses, and the
//! `hyrise/bad.sql` entries are rejected (apart from the documented
//! divergences below).

#[cfg(test)]
mod tests {
    use std::collections::HashSet;
    use std::fs;
    use std::path::PathBuf;

    use crate::{parse, Statement};

    fn data_dir() -> PathBuf {
        PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("../../../test/data")
    }

    fn read(path: &PathBuf) -> String {
        fs::read_to_string(path).unwrap_or_else(|e| panic!("{}: {e}", path.display()))
    }

    /// The C++ `ReadSQLs` filter: drop `#`/`--` comments and the leading `!`
    /// that marks a line as expected-to-fail.
    fn corpus_lines(text: &str) -> Vec<(usize, String, bool)> {
        let mut out = Vec::new();
        for (i, line) in text.lines().enumerate() {
            if line.starts_with('#') || line.starts_with("--") {
                continue;
            }
            let (sql, bad) = match line.strip_prefix('!') {
                Some(rest) => (rest, true),
                None => (line, false),
            };
            out.push((i + 1, sql.to_string(), bad));
        }
        out
    }

    /// C++ `SQLFileParsingTest.infinity`: every line of
    /// `test/data/infinity/good.sql` parses. The file holds three `FLUSH`
    /// utilities, which the reference accepts as statements.
    #[test]
    fn sql_file_parsing_infinity() {
        let sql = read(&data_dir().join("infinity/good.sql"));
        let statements = parse(&sql).expect("infinity corpus must parse");
        assert_eq!(statements.len(), 3);
        // The parser's `Utility` text holds the tokens after `FLUSH`.
        for (statement, expected) in statements.iter().zip(["DATA", "LOG", "BUFFER"]) {
            let Statement::Utility { text } = statement else {
                panic!("expected a utility statement, got {statement:?}");
            };
            assert_eq!(text, expected);
        }
    }

    /// C++ `SQLFileParsingTest.tpch`: every SQL file in `test/data/tpch`
    /// parses (the C++ loop skips only `README.md`).
    #[test]
    fn sql_file_parsing_tpch() {
        let dir = data_dir().join("tpch");
        let mut files: Vec<PathBuf> = fs::read_dir(&dir)
            .unwrap_or_else(|e| panic!("{}: {e}", dir.display()))
            .map(|entry| entry.expect("tpch entry").path())
            .filter(|p| p.file_name().is_some_and(|n| n != "README.md"))
            .collect();
        files.sort();

        assert_eq!(files.len(), 32, "tpch corpus size changed");
        for file in &files {
            let sql = read(file);
            parse(&sql).unwrap_or_else(|e| {
                panic!(
                    "{} must parse: {e}",
                    file.file_name().unwrap().to_string_lossy()
                )
            });
        }
    }

    /// C++ `SQLFileParsingTest.hyrise`: every line of `good.sql` parses and
    /// every `!`-marked line of `bad.sql` is rejected.
    #[test]
    fn sql_file_parsing_hyrise() {
        let good = read(&data_dir().join("hyrise/good.sql"));
        let good_lines = corpus_lines(&good);
        let mut good_sql: HashSet<String> = HashSet::new();
        for (line_no, sql, bad) in &good_lines {
            assert!(!bad, "good.sql line {line_no} is marked as bad");
            parse(sql).unwrap_or_else(|e| panic!("good.sql:{line_no} must parse: {e}"));
            good_sql.insert(sql.clone());
        }

        let bad = read(&data_dir().join("hyrise/bad.sql"));
        let mut rejected = 0;
        for (line_no, sql, marked_bad) in corpus_lines(&bad) {
            assert!(marked_bad, "bad.sql line {line_no} lacks the '!' marker");
            // A lone `!` strips to the empty string; `parse("")` is the
            // documented representation of "no statements" (there is no
            // statement to reject).
            if sql.trim().is_empty() {
                continue;
            }
            // `bad.sql` repeats a line that is also present in `good.sql`
            // (bad.sql:31 == good.sql:85). The same text cannot both parse and
            // fail, so the duplicate is not asserted.
            if good_sql.contains(&sql) {
                continue;
            }
            // The Rust port intentionally accepts the Postgres-incompatible
            // `DROP INDEX <name> ON <table>` spelling; the executor test
            // suites rely on it.
            if sql.to_ascii_uppercase().starts_with("DROP INDEX ") && sql.contains(" ON ") {
                continue;
            }
            let result = parse(&sql);
            assert!(result.is_err(), "bad.sql:{line_no} must be rejected: {sql}");
            rejected += 1;
        }
        assert_eq!(rejected, 43, "bad.sql rejection count changed");
    }

    /// The four contradictory primary-key/`NULL` declarations from
    /// `hyrise/bad.sql` are rejected, while the plain spelling still parses.
    #[test]
    fn hyrise_rejects_conflicting_null_primary_key() {
        for sql in [
            "CREATE TABLE a_table (a_column INT PRIMARY KEY NULL)",
            "CREATE TABLE a_table (a_column INT NULL PRIMARY KEY)",
            "CREATE TABLE a_table (a_column INT NOT NULL NULL)",
            "CREATE TABLE a_table (a_column INT NULL NOT NULL)",
        ] {
            assert!(parse(sql).is_err(), "{sql} must be rejected");
        }
        assert!(parse("CREATE TABLE a_table (a_column INT PRIMARY KEY)").is_ok());
    }
}

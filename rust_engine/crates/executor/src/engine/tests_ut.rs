use super::*;

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    /// A `NULL` slot and a vector column both reach the WAL as typed columns:
    /// the first through the null bitmap, the second as inline element bytes.
    #[test]
    fn typed_append_accepts_null_and_embedding_rows() {
        let cols = typed_append_block(&[vec![Literal::Null]], &["int32".to_string()])
            .expect("a NULL int row is representable");
        assert_eq!(cols.len(), 1);
        assert_eq!(cols[0].nulls, vec![0]);

        let cols = typed_append_block(
            &[vec![Literal::Vector(vec![1.0, 2.0])]],
            &["vector,2,float".to_string()],
        )
        .expect("a float vector row is representable");
        assert_eq!(cols.len(), 1);
        assert_eq!(
            cols[0].data_type.logical_type,
            storage::wal::ddl_def::LogicalType::Embedding
        );
    }

    /// C++ `VerifyColumnDataType` (`logical_planner_impl.cpp:551`): the types
    /// the reference refuses at DDL time, with the message it prints.
    #[test]
    fn the_reference_column_type_whitelist_is_enforced() {
        for (text, expected) in [
            (
                "DECIMAL(10,2)",
                "Not supported data type: Decimal(decimal(10, 2))",
            ),
            ("HUGEINT", "Not supported data type: HugeInt"),
            ("UUID", "Not supported data type: UUID"),
            ("INTERVAL", "Not supported data type: Interval"),
            (
                "ARRAY(DECIMAL(10,2))",
                "Not supported data type: Array(Decimal(decimal(10, 2))), \
                 error in element type: Not supported data type: Decimal(decimal(10, 2))",
            ),
        ] {
            assert_eq!(unsupported_column_type(text).as_deref(), Some(expected));
        }
        // Everything the whitelist accepts, including an array of a scalar.
        for text in [
            "BOOLEAN",
            "TINYINT",
            "SMALLINT",
            "INTEGER",
            "BIGINT",
            "FLOAT",
            "DOUBLE",
            "FLOAT16",
            "BFLOAT16",
            "VARCHAR",
            "DATE",
            "TIME",
            "DATETIME",
            "TIMESTAMP",
            "JSON",
            "EMBEDDING(FLOAT, 4)",
            "VECTOR(4, FLOAT)",
            "TENSOR(FLOAT, 4)",
            "MULTIVECTOR(FLOAT, 2)",
            "SPARSE(FLOAT, INT, 8)",
            "ARRAY(INT)",
        ] {
            assert!(
                unsupported_column_type(text).is_none(),
                "{text} should be accepted"
            );
        }
    }

    /// A subquery that reads the enclosing row runs once per row: `EXISTS`,
    /// `NOT EXISTS` and a scalar aggregate all see that row's values.
    #[test]
    fn a_correlated_subquery_is_evaluated_per_outer_row() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cs_dept (id INTEGER, name VARCHAR)")
            .unwrap();
        e.execute("CREATE TABLE cs_emp (id INTEGER, dept_id INTEGER, salary INTEGER)")
            .unwrap();
        e.execute("INSERT INTO cs_dept VALUES (1, 'eng'), (2, 'ops'), (3, 'idle');")
            .unwrap();
        e.execute("INSERT INTO cs_emp VALUES (1, 1, 100), (2, 1, 200), (3, 2, 50);")
            .unwrap();
        let names = |sql: &str| rows(&e, sql);
        assert_eq!(
            names("SELECT name FROM cs_dept WHERE EXISTS (SELECT 1 FROM cs_emp WHERE cs_emp.dept_id = cs_dept.id);"),
            vec![vec!["eng".to_string()], vec!["ops".to_string()]]
        );
        assert_eq!(
            names("SELECT name FROM cs_dept WHERE NOT EXISTS (SELECT 1 FROM cs_emp WHERE cs_emp.dept_id = cs_dept.id);"),
            vec![vec!["idle".to_string()]]
        );
        assert_eq!(
            names("SELECT name FROM cs_dept WHERE (SELECT COUNT(*) FROM cs_emp WHERE cs_emp.dept_id = cs_dept.id) >= 2;"),
            vec![vec!["eng".to_string()]]
        );
        // The rest of the predicate still runs alongside it.
        assert_eq!(
            names("SELECT name FROM cs_dept WHERE id >= 2 AND EXISTS (SELECT 1 FROM cs_emp WHERE cs_emp.dept_id = cs_dept.id);"),
            vec![vec!["ops".to_string()]]
        );
    }

    #[test]
    fn probe_exact() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cj_o (id INT, a INT);").unwrap();
        e.execute("CREATE TABLE cj_s (a INT, x INT);").unwrap();
        e.execute("CREATE TABLE cj_t (x INT);").unwrap();
        e.execute("INSERT INTO cj_o VALUES (1, 10), (2, 20);")
            .unwrap();
        e.execute("INSERT INTO cj_s VALUES (10, 100), (20, 200);")
            .unwrap();
        e.execute("INSERT INTO cj_t VALUES (10);").unwrap();
        for sql in [
            "WITH c AS (SELECT * FROM cj_o) SELECT id, (SELECT COUNT(*) FROM cj_s WHERE cj_s.a = c.a) AS n FROM c ORDER BY id;",
            "SELECT id FROM (SELECT * FROM cj_o) WHERE EXISTS (SELECT 1 FROM cj_s WHERE cj_s.a = a) ORDER BY id;",
            "SELECT id FROM cj_o JOIN cj_s ON cj_s.a = cj_o.a WHERE EXISTS (SELECT 1 FROM cj_t WHERE cj_t.x = a) ORDER BY id;",
        ] {
            match e.execute(sql) {
                Ok(r) => println!("PROBE ok {sql} => {r:?}"),
                Err(err) => println!("PROBE err {sql} => {err}"),
            }
        }
    }

    /// The dependent join also works when the outer relation is a CTE or the
    /// left side of a join: the hidden columns ride along with the rows the
    /// source hands to the plan. (A derived table is not a correlatable scope:
    /// the parser drops its alias, so a nested query cannot qualify it.)
    #[test]
    fn a_correlated_subquery_reads_a_cte_or_joined_relation() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cj_o (id INT, a INT);").unwrap();
        e.execute("CREATE TABLE cj_s (a INT, x INT);").unwrap();
        e.execute("CREATE TABLE cj_t (x INT);").unwrap();
        e.execute("INSERT INTO cj_o VALUES (1, 10), (2, 20);")
            .unwrap();
        e.execute("INSERT INTO cj_s VALUES (10, 100), (20, 200);")
            .unwrap();
        e.execute("INSERT INTO cj_t VALUES (10);").unwrap();

        // A CTE as the outer relation: `c.a` is substituted with the alias the
        // statement used, not the internal one.
        assert_eq!(
            rows(
                &e,
                "WITH c AS (SELECT * FROM cj_o) \
                 SELECT id, (SELECT COUNT(*) FROM cj_s WHERE cj_s.a = c.a) AS n FROM c ORDER BY id;"
            ),
            vec![
                vec!["1".to_string(), "1".to_string()],
                vec!["2".to_string(), "1".to_string()],
            ]
        );
        // Joined relation: the correlated columns read the FROM side, whose
        // rows the join takes from the dependent join's output.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM cj_o JOIN cj_s ON cj_s.a = cj_o.a \
                 WHERE EXISTS (SELECT 1 FROM cj_t WHERE cj_t.x = a) ORDER BY id;"
            ),
            vec![vec!["1".to_string()]]
        );
    }

    /// `GAPS.md` 2.2: the correlated side is a dependent join, not a per-row
    /// re-execution. Four outer rows over two distinct keys run the inner query
    /// twice, and the cached rows answer `IN`/`NOT IN` per row.
    #[test]
    fn a_correlated_subquery_runs_once_per_distinct_key() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ck (id INT, k INT);").unwrap();
        e.execute("INSERT INTO ck VALUES (1, 10), (2, 10), (3, 20), (4, 20);")
            .unwrap();
        e.execute("CREATE TABLE cv (k INT, x INT);").unwrap();
        e.execute("INSERT INTO cv VALUES (10, 1), (20, 2);")
            .unwrap();

        crate::dependent::INNER_RUNS.with(|runs| runs.set(0));
        assert_eq!(
            rows(
                &e,
                "SELECT id, (SELECT MAX(x) FROM cv WHERE cv.k = ck.k) AS m FROM ck ORDER BY id;"
            ),
            vec![
                vec!["1".to_string(), "1".to_string()],
                vec!["2".to_string(), "1".to_string()],
                vec!["3".to_string(), "2".to_string()],
                vec!["4".to_string(), "2".to_string()],
            ]
        );
        assert_eq!(
            crate::dependent::INNER_RUNS.with(|runs| runs.get()),
            2,
            "the inner side must run once per distinct key, not once per row"
        );

        // `IN` / `NOT IN` probe the cached rows with the row's own operand.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM ck WHERE k IN (SELECT k FROM cv WHERE cv.x >= ck.id) ORDER BY id;"
            ),
            vec![vec!["1".to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM ck WHERE k NOT IN (SELECT k FROM cv WHERE cv.x >= ck.id) ORDER BY id;"
            ),
            vec![
                vec!["2".to_string()],
                vec!["3".to_string()],
                vec!["4".to_string()],
            ]
        );
    }

    /// `GAPS.md` 2.1: a correlated subquery is not limited to a `WHERE`
    /// conjunct. The dependent join (`dependent.rs`) turns every nested query
    /// that reads the outer row into a hidden column, so the projection, the
    /// projection's surrounding expression and `HAVING` all evaluate it
    /// through the ordinary column path.
    #[test]
    fn a_correlated_subquery_works_outside_the_where_clause() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cs_dept2 (id INTEGER, name VARCHAR)")
            .unwrap();
        e.execute("CREATE TABLE cs_emp2 (id INTEGER, dept_id INTEGER, salary INTEGER)")
            .unwrap();
        e.execute("INSERT INTO cs_dept2 VALUES (1, 'eng'), (2, 'ops'), (3, 'idle');")
            .unwrap();
        e.execute("INSERT INTO cs_emp2 VALUES (1, 1, 100), (2, 1, 200), (3, 2, 50);")
            .unwrap();

        // Projected scalar subquery, one value per outer row.
        assert_eq!(
            rows(
                &e,
                "SELECT name, (SELECT COUNT(*) FROM cs_emp2 WHERE cs_emp2.dept_id = cs_dept2.id) AS n \
                 FROM cs_dept2 ORDER BY id;"
            ),
            vec![
                vec!["eng".to_string(), "2".to_string()],
                vec!["ops".to_string(), "1".to_string()],
                vec!["idle".to_string(), "0".to_string()],
            ]
        );
        // It composes with the rest of the item: the hidden column is an
        // ordinary operand of the projected expression.
        assert_eq!(
            rows(
                &e,
                "SELECT (SELECT MAX(salary) FROM cs_emp2 WHERE cs_emp2.dept_id = cs_dept2.id) + 1 AS m \
                 FROM cs_dept2 ORDER BY id;"
            ),
            vec![
                vec!["201".to_string()],
                vec!["51".to_string()],
                vec!["null".to_string()],
            ]
        );
        // `HAVING`: the correlated column depends only on the grouping key, so
        // each group sees one value.
        assert_eq!(
            rows(
                &e,
                "SELECT dept_id FROM cs_emp2 GROUP BY dept_id \
                 HAVING (SELECT COUNT(*) FROM cs_dept2 WHERE cs_dept2.id = cs_emp2.dept_id) > 0 \
                 ORDER BY dept_id;"
            ),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    // A full-text index is built and queried with the analyzer it declared,
    // not with the standard one: an `ngram-3` index turns `quick` into
    // `qui`/`uic`/`ick`, so a literal `qui` is a token there and not in a
    // `standard` index.

    /// A reopened engine maps the chunks the previous process sealed instead of
    /// re-indexing the table, so its memory tail holds no document and the
    /// sealed readers cover every row.
    #[test]
    fn a_reopened_engine_indexes_only_the_unsealed_tail() {
        let dir = tempdir().unwrap();
        let before = {
            let e = Engine::open(dir.path()).unwrap();
            e.set_memindex_capacity(4);
            e.execute("CREATE TABLE rt (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX rt_idx ON rt (body) USING FULLTEXT")
                .unwrap();
            for i in 0..12 {
                e.execute(&format!(
                    "INSERT INTO rt VALUES ({i}, 'alpha beta row{i}');"
                ))
                .unwrap();
            }
            e.flush().unwrap();
            rows(
                &e,
                "SELECT id FROM rt SEARCH MATCH TEXT (body, 'alpha', 100);",
            )
        };
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM rt SEARCH MATCH TEXT (body, 'alpha', 100);",
            ),
            before // Rows come back in the same order: the mapped chunks and the
                   // unsealed tail are searched exactly as the single in-memory index
                   // was before the restart.
        );
        let (sealed, pending) = {
            let tables = e.tables.lock().unwrap();
            let (mut sealed, mut pending) = (0, 0);
            for entry in tables.values() {
                for shared in entry.text.values() {
                    let guard = shared.read().unwrap();
                    sealed += guard.reader.doc_count();
                    pending += guard.mem.pending_docs();
                }
            }
            (sealed, pending)
        };
        assert_eq!(sealed, 12, "the sealed chunks should cover the table");
        assert_eq!(pending, 0, "no row should be indexed again");
    }

    /// `NewTxn::OptimizeIndexInner` → `OptimizeFtIndex`: an index holding
    /// several chunks is merged into one `ft_{base:016x}_{rows:x}` chunk, the
    /// sources are dropped, and the merged chunk serves the same rows after a
    /// restart.
    #[test]
    fn optimizing_a_fulltext_index_merges_its_chunks() {
        let dir = tempdir().unwrap();
        let query = "SELECT id FROM opt SEARCH MATCH TEXT (body, 'alpha', 100);";
        let before;
        {
            let e = Engine::open(dir.path()).unwrap();
            // One chunk per two rows, so the index starts fragmented.
            e.set_memindex_capacity(2);
            e.execute("CREATE TABLE opt (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX opt_idx ON opt (body) USING FULLTEXT")
                .unwrap();
            for i in 0..10 {
                e.execute(&format!(
                    "INSERT INTO opt VALUES ({i}, 'alpha beta row{i}');"
                ))
                .unwrap();
            }
            e.flush().unwrap();
            before = rows(&e, query);
            assert_eq!(before.len(), 10);
            let chunks = fulltext_chunks(&e);
            assert!(chunks > 1, "five seals leave five chunks, got {chunks}");

            // The production entry point: `optimize_indexes` is what the
            // `OPTIMIZE` statement and the periodic task drive
            // (`OptimizationProcessor::NewNotifyOptimize`).
            e.optimize_indexes().unwrap();
            assert_eq!(
                fulltext_chunks(&e),
                1,
                "the chunks tile one row range, so they merge into one"
            );
            assert_eq!(rows(&e, query), before, "the merge must not lose rows");
            assert_eq!(
                e.optimize_fulltext("opt").unwrap(),
                0,
                "an already-merged index is a no-op"
            );
        }

        // The merged chunk is what `load_sealed` finds on the next open.
        let reopened = Engine::open(dir.path()).unwrap();
        assert_eq!(fulltext_chunks(&reopened), 1);
        assert_eq!(rows(&reopened, query), before);
    }

    /// A query that spans several sealed segments plus the memory tail must
    /// return exactly what one unsealed segment returns: same rows, same order.
    /// That is the concat path's contract (the segments' row ranges are
    /// disjoint, so their posting lists are concatenated instead of merged).
    #[test]
    fn a_query_over_many_segments_matches_a_single_segment() {
        const QUERIES: [&str; 4] = [
            "SELECT id FROM seg SEARCH MATCH TEXT (body, 'alpha', 100);",
            "SELECT id FROM seg SEARCH MATCH TEXT (body, '\"beta gamma\"', 100);",
            "SELECT id FROM seg SEARCH MATCH TEXT (body, 'alpha beta', 100);",
            "SELECT id FROM seg SEARCH MATCH TEXT (body, 'gamma epsilon alpha', 100);",
        ];
        let build = |capacity: usize| -> Vec<Vec<Vec<String>>> {
            let dir = tempdir().unwrap();
            let e = Engine::open(dir.path()).unwrap();
            // Two rows per segment: the query has to walk several of them.
            e.set_memindex_capacity(capacity);
            e.execute("CREATE TABLE seg (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX seg_idx ON seg (body) USING FULLTEXT")
                .unwrap();
            for i in 0..20 {
                let body = match i % 4 {
                    0 => "alpha beta gamma",
                    1 => "alpha delta",
                    2 => "beta gamma",
                    _ => "gamma epsilon",
                };
                e.execute(&format!("INSERT INTO seg VALUES ({i}, '{body}');"))
                    .unwrap();
            }
            e.flush().unwrap();
            QUERIES.iter().map(|sql| rows(&e, sql)).collect()
        };
        let many = build(2);
        let one = build(1024);
        for (i, sql) in QUERIES.iter().enumerate() {
            assert!(!one[i].is_empty(), "query {i} returned nothing: {sql}");
            assert_eq!(many[i], one[i], "query {i}: {sql}");
        }
    }

    #[test]
    fn a_fulltext_index_uses_its_declared_analyzer() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        for (table, analyzer) in [("an_std", "standard"), ("an_gram", "ngram-3")] {
            e.execute(&format!("CREATE TABLE {table} (id INTEGER, body VARCHAR)"))
                .unwrap();
            e.execute(&format!(
                "CREATE INDEX {table}_idx ON {table} (body) USING FULLTEXT WITH (analyzer='{analyzer}')"
            ))
            .unwrap();
            e.execute(&format!("INSERT INTO {table} VALUES (1, 'quick brown');"))
                .unwrap();
        }
        e.flush().unwrap();
        let hits = |table: &str| {
            rows(
                &e,
                &format!("SELECT id FROM {table} SEARCH MATCH TEXT (body, 'qui', 10);"),
            )
        };
        assert_eq!(hits("an_std"), Vec::<Vec<String>>::new());
        assert_eq!(hits("an_gram"), vec![vec!["1".to_string()]]);
        // The whole word still matches both.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM an_std SEARCH MATCH TEXT (body, 'quick', 10);"
            ),
            vec![vec!["1".to_string()]]
        );
    }

    /// With `result_cache` on, a repeated scan is answered from the plan-node
    /// cache (`ResultCacheGetter` + `ResultCacheSetter`), and a write drops it.
    #[test]
    fn the_plan_cache_answers_a_repeated_scan_and_a_write_drops_it() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rc (id INTEGER, name VARCHAR)")
            .unwrap();
        for i in 0..5 {
            e.execute(&format!("INSERT INTO rc VALUES ({i}, 'n{i}');"))
                .unwrap();
        }
        assert_eq!(e.plan_cache.len(), 0);
        // The cache is opt-in, as in the reference (`DEFAULT_RESULT_CACHE`).
        let before = rows(&e, "SELECT id FROM rc WHERE id >= 2;");
        assert_eq!(e.plan_cache.len(), 0);
        e.execute("SET CONFIG result_cache ON").unwrap();
        let first = rows(&e, "SELECT id FROM rc WHERE id >= 2;");
        assert_eq!(first, before);
        assert!(!e.plan_cache.is_empty(), "the scan did not fill the cache");
        assert_eq!(rows(&e, "SELECT id FROM rc WHERE id >= 2;"), first);
        // A write clears every entry: the answer could have changed.
        e.execute("INSERT INTO rc VALUES (9, 'n9');").unwrap();
        assert_eq!(e.plan_cache.len(), 0);
        assert_eq!(
            rows(&e, "SELECT id FROM rc WHERE id >= 2;").len(),
            first.len() + 1
        );
    }

    /// `CompactionProcessor::NewNotifyCompact`: the periodic trigger visits
    /// every table and merges the four segments `NewCompactionAlg` picks.
    #[test]
    fn notify_compact_merges_the_segments_the_alg_picks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE nc (id INTEGER)").unwrap();
        // Four imports of one block each: layer 0 holds four segments, which is
        // exactly what `DBTCompactionAlg` merges.
        for part in 0..4 {
            let path = dir.path().join(format!("nc{part}.csv"));
            let text: String = (0..8192)
                .map(|i| format!("{}\n", part * 8192 + i))
                .collect();
            std::fs::write(&path, text).unwrap();
            e.execute(&format!(
                "COPY nc FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
                path.display()
            ))
            .unwrap();
        }
        assert_eq!(e.segments("nc").len(), 4);
        assert_eq!(e.row_count("nc").unwrap(), 4 * 8192);
        assert_eq!(e.notify_compact(), 1);
        assert_eq!(e.segments("nc").len(), 1, "{:?}", e.segments("nc"));
        assert_eq!(e.row_count("nc").unwrap(), 4 * 8192);
        // Nothing left to pick: a second pass is a no-op.
        assert_eq!(e.notify_compact(), 0);
        let rows = rows(&e, "SELECT id FROM nc WHERE id = 12345;");
        assert_eq!(rows, vec![vec!["12345".to_string()]]);
    }

    /// A scan that carries a range predicate prunes whole blocks through their
    /// `blk|…|fast_rough_filter` record: a block whose min/max refutes the
    /// predicate is never decoded, and the rows the answer needs still come
    /// back.
    #[test]
    fn the_block_prefilter_skips_blocks_that_cannot_match() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pf (id INTEGER)").unwrap();
        // `COPY` seals the segment, which is when the reference builds the
        // block prefilters. Two blocks: ids 8192..8999 live in the second one,
        // so `id < 10` can never match it.
        let csv = dir.path().join("pf.csv");
        let text: String = (0..9000).map(|i| format!("{i}\n")).collect();
        std::fs::write(&csv, text).unwrap();
        e.execute(&format!(
            "COPY pf FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        e.flush().unwrap();
        crate::blocks::PRUNED_BLOCKS.with(|n| n.set(0));
        let got = rows(&e, "SELECT id FROM pf WHERE id < 10;");
        assert_eq!(got.len(), 10, "{got:?}");
        assert_eq!(got[0][0], "0");
        assert_eq!(got[9][0], "9");
        let pruned = crate::blocks::PRUNED_BLOCKS.with(|n| n.get());
        assert!(pruned > 0, "no block was pruned (read {pruned})");
        // A predicate that spans both blocks still returns every row.
        let got = rows(&e, "SELECT id FROM pf WHERE id >= 8190 AND id < 8200;");
        assert_eq!(got.len(), 10, "{got:?}");
    }

    /// `CREATE TABLE … PROPERTIES (bloom_filter_columns = '…')` is accepted and
    /// validated, but the flag never reaches the segment: the reference's
    /// `ColumnDef::ToJson` omits `build_bloom_filter_` (`column_def.cpp:229`),
    /// and the transaction that seals a segment reads its column defs back from
    /// that record, so `BuildOnlyBloomFilter` never runs. Measured on the
    /// reference: a `BOOLEAN` column declared with the property still
    /// serializes `1, 0` in `seg|…|fast_rough_filter`.
    #[test]
    fn a_bloom_filter_column_is_accepted_but_never_built() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute(
            "CREATE TABLE bf (b BOOLEAN, i INTEGER) PROPERTIES (bloom_filter_columns = ' b ');",
        )
        .unwrap();

        // One `COPY` seals a segment, which is when the filter is built.
        let csv = dir.path().join("bf.csv");
        let text: String = (0..100)
            .map(|i| format!("{},{i}\n", if i % 2 == 0 { "true" } else { "false" }))
            .collect();
        std::fs::write(&csv, text).unwrap();
        e.execute(&format!(
            "COPY bf FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        e.flush().unwrap();
        let record = {
            let mut found = None;
            for (k, v) in e.storage.catalog.store().scan_prefix(b"seg|1|").unwrap() {
                if String::from_utf8_lossy(&k).ends_with("|fast_rough_filter") {
                    found = Some(v);
                }
            }
            found.expect("the segment filter")
        };
        // `[u32 total][u64 build_time][u32 probabilistic_size][u32 columns]`
        // then per column `[u8 exists][u8 built]`.
        assert_eq!(u32::from_le_bytes(record[12..16].try_into().unwrap()), 12);
        assert_eq!(u32::from_le_bytes(record[16..20].try_into().unwrap()), 2);
        assert_eq!(
            &record[20..24],
            &[1, 0, 1, 0],
            "both filter objects exist, nothing was built into either"
        );

        // DDL errors, as the reference's planner reports them.
        let error = e
            .execute("CREATE TABLE bf2 (x FLOAT) PROPERTIES (bloom_filter_columns = 'x');")
            .expect_err("FLOAT has no bloom filter");
        assert!(
            error
                .to_string()
                .contains("Bloom filter can't be created for Float type column x"),
            "{error}"
        );
        assert_eq!(error.status_code(), 3013);
        let error = e
            .execute("CREATE TABLE bf3 (b BOOLEAN) PROPERTIES (bloom_filter_columns = 'nope');")
            .expect_err("unknown column");
        assert!(
            error
                .to_string()
                .contains("Column nope not found in table bf3"),
            "{error}"
        );
        let error = e
            .execute(
                "CREATE TABLE bf4 (b BOOLEAN, c BOOLEAN) PROPERTIES (bloom_filter_columns = 'b', bloom_filter_columns = 'c');",
            )
            .expect_err("duplicate property");
        assert!(
            error
                .to_string()
                .contains("Duplicate table property param name: BLOOM_FILTER_COLUMNS"),
            "{error}"
        );
    }

    /// The reference has no `CREATE TABLE … AS SELECT`: without a column
    /// definition the statement reaches its binder and fails with
    /// `Status::NoColumnDefined` (3048), not with a table created from the
    /// query's result.
    #[test]
    fn create_table_as_select_is_rejected_like_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cta_src (a INT);").unwrap();
        e.execute("INSERT INTO cta_src VALUES (1);").unwrap();
        let error = e
            .execute("CREATE TABLE cta_dst AS SELECT * FROM cta_src;")
            .expect_err("CTAS must be rejected");
        assert_eq!(
            error.to_string(),
            "Try to define Table cta_dst without any column."
        );
        assert_eq!(error.status_code(), 3048);
        // Nothing was created.
        assert!(e.execute("SELECT * FROM cta_dst;").is_err());
    }

    /// The rows of a `SELECT`, as rendered strings.
    /// A secondary index answers an equality predicate **after** a checkpoint:
    /// the memory tail the index was built into is dumped into a sealed chunk,
    /// and the lookup has to read that chunk with the same key the writer wrote.
    #[test]
    fn a_secondary_index_answers_equality_after_a_checkpoint() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE si (id INT, score DOUBLE, name VARCHAR);")
            .unwrap();
        for base in (0..20000).step_by(2000) {
            let values: Vec<String> = (base..base + 2000)
                .map(|i| format!("({i}, {}.0, 'n-{i}')", i % 100))
                .collect();
            e.execute(&format!("INSERT INTO si VALUES {};", values.join(", ")))
                .unwrap();
        }
        e.execute("CREATE INDEX si_id ON si (id);").unwrap();
        e.execute("CREATE INDEX si_score ON si (score);").unwrap();
        e.execute("CREATE INDEX si_name ON si (name);").unwrap();
        let count = |sql: &str| rows(&e, sql).len();
        assert_eq!(count("SELECT id FROM si WHERE id = 5;"), 1);
        assert_eq!(count("SELECT id FROM si WHERE score = 5.0;"), 200);
        assert_eq!(count("SELECT id FROM si WHERE name = 'n-5';"), 1);
        e.flush().unwrap();
        assert_eq!(
            count("SELECT id FROM si WHERE id = 5;"),
            1,
            "an INT key must still be found in the sealed chunk"
        );
        assert_eq!(
            count("SELECT id FROM si WHERE score = 5.0;"),
            200,
            "a DOUBLE key must still be found in the sealed chunk"
        );
        assert_eq!(
            count("SELECT id FROM si WHERE name = 'n-5';"),
            1,
            "a VARCHAR key must still be found in the sealed chunk"
        );
        // The server's SELECT goes through the columnar batch path, which maps
        // the index's row ids onto the block's visible slots: a mis-mapped run
        // returned NULL cells for every row whose slot was not the first few.
        let batch = |sql: &str| {
            e.select_columns(sql)
                .unwrap()
                .unwrap_or_else(|| panic!("{sql} did not take the columnar path"))
        };
        assert_eq!(
            batch("SELECT id FROM si WHERE id = 5;").row_count,
            1,
            "a columnar select must find a sealed row by its index"
        );
        assert_eq!(
            batch("SELECT id FROM si WHERE score = 5.0;").row_count,
            200,
            "every row the index names must survive the batch mapping"
        );
        assert_eq!(batch("SELECT id FROM si WHERE id > 10;").row_count, 19989);
    }
    fn rows(e: &Engine, sql: &str) -> Vec<Vec<String>> {
        for r in e.execute(sql).unwrap() {
            if let QueryResult::Rows { .. } = r {
                return r.rows_text();
            }
        }
        Vec::new()
    }

    /// How many sealed chunks the table's full-text indexes hold, i.e. the
    /// reference's `SegmentIndexMeta` chunk list length.
    fn fulltext_chunks(e: &Engine) -> usize {
        let tables = e.tables.lock().unwrap();
        tables
            .values()
            .flat_map(|entry| entry.text.values())
            .map(|shared| shared.read().unwrap().reader.segments.len())
            .sum()
    }

    /// A chunk is written under one `base_row_id` and its `.len` spans
    /// `base_row_id..=last_row`, so the unsealed segment may not mix rows of two
    /// `ROW_ID` segments - they are 2^32 apart, and the span would neither fit
    /// `u32` nor be meant to be allocated (the reference keeps one
    /// `ChunkIndexMetaInfo` per segment). The insert path therefore seals the old
    /// tail before the first row of the next segment lands.
    #[test]
    fn an_unsealed_tail_never_mixes_two_row_id_segments() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Only the byte budget may seal, so the tail is exactly what the test
        // inserts.
        e.set_memindex_capacity(0);
        e.execute("CREATE TABLE seg2 (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX seg2_idx ON seg2 (body) USING FULLTEXT")
            .unwrap();
        let schema = vec!["id".to_string(), "body".to_string()];
        let row = |text: &str| vec!["0".to_string(), text.to_string()];
        let first = row("alpha beta");
        e.update_text_indexes_batch("seg2", &[(0usize, first.as_slice())], &schema, None);
        // A row of the next segment: `1 << 32` is its first offset.
        let second = row("gamma alpha");
        e.update_text_indexes_batch("seg2", &[(1usize << 32, second.as_slice())], &schema, None);

        let tables = e.tables.lock().unwrap();
        let shared = tables
            .values()
            .flat_map(|entry| entry.text.values())
            .next()
            .expect("the column's index");
        let index = shared.read().unwrap();
        assert_eq!(
            index.reader.segments.len(),
            1,
            "the segment-0 tail was sealed before the segment-1 row arrived"
        );
        let sealed = &index.reader.segments[0];
        assert_eq!(sealed.reader.base_row_id(), 0);
        assert_eq!(sealed.docs, 1, "one row, no padding across the segment gap");
        assert_eq!(index.unsealed_segment(), Some(1), "the new tail");
        assert_eq!(index.mem.row_range, Some(((1 << 32), (1 << 32))));
    }

    /// A chunk's `.len` covers every row of its span, so a seal must not span a
    /// row another writer has announced but not published yet: the chunk would
    /// record length 0 for it, and the chunk that publishes it a moment later
    /// would start before the end of this one - two chunks over one row, which
    /// `optimize_chunks` then reports as an overlap for good.
    #[test]
    fn a_seal_defers_while_a_lower_row_is_still_being_published() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Only the explicit seals below may write a chunk.
        e.set_memindex_capacity(0);
        e.execute("CREATE TABLE publish (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX publish_idx ON publish (body) USING FULLTEXT")
            .unwrap();
        let schema = vec!["id".to_string(), "body".to_string()];
        let row = |text: &str| vec!["0".to_string(), text.to_string()];
        let shared = {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.text.values())
                .next()
                .cloned()
                .expect("the column's index")
        };

        // Writer A announced row 0 and is still tokenising it.
        shared.write().unwrap().announce_publish(0);
        // Writer B got the next row id, published it and asked for a seal.
        let later = row("beta alpha");
        e.update_text_indexes_batch("publish", &[(1usize, later.as_slice())], &schema, None);
        {
            let mut index = shared.write().unwrap();
            assert!(
                !index.seal(),
                "the seal covered a row whose text was still pending"
            );
            assert!(
                index.reader.segments.is_empty(),
                "nothing was written while row 0 was pending"
            );
            assert!(index.mem.docs.contains_key(&1), "row 1 stays in the tail");
        }

        // Row 0 lands, the barrier lifts, and one seal covers both rows.
        shared.write().unwrap().finish_publish(0);
        let first = row("alpha gamma");
        e.update_text_indexes_batch("publish", &[(0usize, first.as_slice())], &schema, None);
        {
            let mut index = shared.write().unwrap();
            assert!(index.seal(), "the seal runs once the barrier is lifted");
            assert_eq!(index.reader.segments.len(), 1);
            assert_eq!(
                index.reader.segments[0].docs, 2,
                "one dense chunk over rows 0 and 1, with no zero-length padding"
            );
            assert_eq!(index.reader.segments[0].reader.base_row_id(), 0);
        }
        // Both rows kept their own length, i.e. neither was published as a gap.
        let index = shared.read().unwrap();
        assert_eq!(index.doc_len_opt(0), Some(2), "row 0: alpha + gamma");
        assert_eq!(index.doc_len_opt(1), Some(2), "row 1: beta + alpha");
    }

    /// The unsealed segment is written **with the column's lock released**
    /// (`seal_text_index_unlocked`), so the tail has to stay searchable while its
    /// segment is on disk: it moves to `TextIndex::inflight`, which every read
    /// path merges after the resident tail.
    ///
    /// This takes the tail out exactly as the seal does and then, with the write
    /// not yet started, asks the index for the rows: they must all still answer,
    /// with their lengths and document frequencies.
    #[test]
    fn a_query_still_sees_the_tail_while_its_segment_is_being_written() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(0);
        e.execute("CREATE TABLE slip (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX slip_idx ON slip (body) USING FULLTEXT")
            .unwrap();
        for i in 0..6 {
            e.execute(&format!(
                "INSERT INTO slip VALUES ({i}, 'alpha beta row{i}');"
            ))
            .unwrap();
        }
        let query = "SELECT id FROM slip SEARCH MATCH TEXT (body, 'alpha', 100);";
        let before = rows(&e, query);
        assert_eq!(before.len(), 6);
        let shared = {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.text.values())
                .next()
                .cloned()
                .expect("the column's index")
        };

        // What `claim_seal` does, minus the write.
        let job = shared
            .write()
            .unwrap()
            .claim_seal()
            .expect("the tail is sealable");
        {
            let index = shared.read().unwrap();
            assert!(
                index.inflight.is_some(),
                "the tail moved to the in-flight slot"
            );
            assert_eq!(index.reader.segments.len(), 0, "nothing is sealed yet");
            assert_eq!(
                index.mem.pending_docs(),
                0,
                "the resident tail is empty; the rows live in the in-flight one"
            );
            // The rows the segment will hold are still fully searchable: the
            // postings (df), the lengths and the corpus statistics include the
            // in-flight tail.
            assert_eq!(index.term_df("alpha"), 6);
            assert_eq!(index.total_docs(), 6);
            assert_eq!(index.doc_len_opt(3), Some(3), "alpha beta row3");
        }
        assert_eq!(
            rows(&e, query).len(),
            6,
            "a query runs while the segment is not written yet"
        );

        // The write finishes and the same rows answer from the sealed chunk.
        let segment = job.write().expect("the segment is written");
        shared.write().unwrap().complete_seal(job, segment);
        {
            let index = shared.read().unwrap();
            assert!(index.inflight.is_none());
            assert_eq!(index.reader.segments.len(), 1);
            assert_eq!(index.mem.pending_docs(), 0);
        }
        assert_eq!(rows(&e, query).len(), 6);
        assert_eq!(
            rows(&e, query),
            before,
            "the sealed chunk answers exactly what the in-flight tail did"
        );
    }

    /// A seal whose write fails must not lose the rows and must not merge the
    /// tail back into a resident tail that other writers may have appended to:
    /// the tail stays in flight, keeps answering, and the next seal retries it.
    #[test]
    fn a_failed_seal_keeps_the_rows_and_the_next_one_retries() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(0);
        e.execute("CREATE TABLE fail (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX fail_idx ON fail (body) USING FULLTEXT")
            .unwrap();
        for i in 0..4 {
            e.execute(&format!("INSERT INTO fail VALUES ({i}, 'alpha row{i}');"))
                .unwrap();
        }
        let query = "SELECT id FROM fail SEARCH MATCH TEXT (body, 'alpha', 100);";
        let shared = {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.text.values())
                .next()
                .cloned()
                .expect("the column's index")
        };

        // A seal that is abandoned instead of completed (the I/O-error path).
        let job = shared.write().unwrap().claim_seal().expect("a tail");
        shared.write().unwrap().abort_seal(job);
        {
            let index = shared.read().unwrap();
            assert!(index.inflight.is_some(), "the tail waits for a retry");
            assert_eq!(
                index.inflight.as_ref().unwrap().mem.pending_docs(),
                4,
                "no row was lost"
            );
        }
        assert_eq!(rows(&e, query).len(), 4, "the rows still answer");

        // The next write retries: the in-flight tail becomes the chunk and the
        // new row stays in the (fresh) resident tail.
        e.execute("INSERT INTO fail VALUES (4, 'alpha row4');")
            .unwrap();
        {
            let index = shared.read().unwrap();
            assert!(
                index.inflight.is_none(),
                "the retry consumed the in-flight tail"
            );
            assert_eq!(index.reader.segments.len(), 1);
            assert_eq!(index.reader.segments[0].docs, 4);
        }
        assert_eq!(rows(&e, query).len(), 5, "the new row is searchable too");
    }

    /// `PhraseDocIterator::GetSloppyPhraseMatchData`
    /// (`phrase_doc_iterator_impl.cpp:238-368`): a solution picks one position
    /// per term, is acceptable while the spread of `pos_i - i` stays within the
    /// slop, and contributes `1/(1+spread)`.
    #[test]
    fn sloppy_phrase_frequency_matches_the_reference() {
        // Strict adjacency: one occurrence.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![0], vec![1]], 2, 0),
            1.0
        );
        // `"a b"~1` also accepts a single-token gap; spread 1 -> weight 1/2.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![0], vec![2]], 2, 1),
            0.5
        );
        // Two-token gap with slop 1: no solution.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![0], vec![3]], 2, 1),
            0.0
        );
        // Two exact solutions sum up.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![0, 2], vec![1, 3]], 2, 1),
            2.0
        );
        // Reordering is allowed once the slop covers the offset: `b a` gives
        // phrase positions 1 and -1, spread 2 -> weight 1/3.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![1], vec![0]], 2, 2),
            1.0 / 3.0
        );
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![1], vec![0]], 2, 1),
            0.0
        );
        // A position below the prefix's window is too low, but the term's next
        // occurrence still fits: `a b c`~2 with a=[4], b=[3], c=[2, 4] gives the
        // prefix positions (2, 4); c=2 has spread 4-0=4 and is rejected, c=4
        // yields spread 2 -> weight 1/3. Unwinding on the rejected position
        // instead of advancing the level scored 0.0 and dropped the match.
        assert_eq!(
            super::state::phrase_freq_lists_sloppy(&[vec![4], vec![3], vec![2, 4]], 3, 2),
            1.0 / 3.0
        );
    }

    /// `"a b"~N` end to end: the slop widens the adjacency requirement and the
    /// `~N` no longer becomes a term of its own.
    #[test]
    fn search_honours_the_phrase_slop() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sl (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX sl_idx ON sl (body) USING FULLTEXT")
            .unwrap();
        for (id, body) in [
            (0, "alpha beta"),
            (1, "alpha x beta"),
            (2, "alpha x x beta"),
            (3, "beta alpha"),
        ] {
            e.execute(&format!("INSERT INTO sl VALUES ({id}, '{body}');"))
                .unwrap();
        }
        e.flush().unwrap();

        let exact = rows(
            &e,
            "SELECT id FROM sl SEARCH MATCH TEXT (body, '\"alpha beta\"', 10);",
        );
        assert_eq!(exact, vec![vec!["0".to_string()]]);

        let one = rows(
            &e,
            "SELECT id FROM sl SEARCH MATCH TEXT (body, '\"alpha beta\"~1', 10);",
        );
        let mut one_ids: Vec<String> = one.iter().map(|r| r[0].clone()).collect();
        one_ids.sort();
        assert_eq!(
            one_ids,
            vec!["0".to_string(), "1".to_string()],
            "slop 1 reaches a one-token gap; the swapped order needs 2"
        );

        let two = rows(
            &e,
            "SELECT id FROM sl SEARCH MATCH TEXT (body, '\"alpha beta\"~2', 10);",
        );
        let mut two_ids: Vec<String> = two.iter().map(|r| r[0].clone()).collect();
        two_ids.sort();
        assert_eq!(
            two_ids,
            vec![
                "0".to_string(),
                "1".to_string(),
                "2".to_string(),
                "3".to_string()
            ],
            "slop 2 also reaches the two-token gap"
        );
    }

    /// `operator=and` is the reference's `AndQueryNode`: every clause has to
    /// match the same document, and adjacency is *not* required (only a phrase
    /// asks for that). `minimum_should_match` is the same predicate expressed
    /// as a count, so the two must select the same rows.
    #[test]
    fn operator_and_requires_every_clause_without_adjacency() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ba (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX ba_idx ON ba (body) USING FULLTEXT")
            .unwrap();
        for (id, body) in [
            (0, "alpha beta"),
            (1, "alpha"),
            (2, "beta"),
            (3, "alpha x beta"),
        ] {
            e.execute(&format!("INSERT INTO ba VALUES ({id}, '{body}');"))
                .unwrap();
        }
        e.flush().unwrap();

        let default_or = rows(
            &e,
            "SELECT id FROM ba SEARCH MATCH TEXT (body, 'alpha beta', 10);",
        );
        assert_eq!(default_or.len(), 4, "the default operator is OR");

        let and = rows(
            &e,
            "SELECT id FROM ba SEARCH MATCH TEXT (body, 'alpha beta', 10, 'operator=and');",
        );
        let mut and_ids: Vec<String> = and.iter().map(|r| r[0].clone()).collect();
        and_ids.sort();
        assert_eq!(
            and_ids,
            vec!["0".to_string(), "3".to_string()],
            "both clauses must match; `alpha x beta` still qualifies"
        );

        let msm = rows(
            &e,
            "SELECT id FROM ba SEARCH MATCH TEXT (body, 'alpha beta', 10, 'minimum_should_match=2');",
        );
        let mut msm_ids: Vec<String> = msm.iter().map(|r| r[0].clone()).collect();
        msm_ids.sort();
        assert_eq!(msm_ids, and_ids, "minimum_should_match=2 is operator=and");
    }

    /// An explicit `operator=` selects the term-list grammar. The reference's
    /// `FulltextQueryOperatorOption` is `kAnd`/`kOr` when the option is given and
    /// `kInfinitySyntax` only when it is absent (`bound_select_statement_impl.cpp:200-210`),
    /// so `alpha OR beta` with `operator=` is analysed into terms - `or` is a
    /// word, not an operator - and must never be rejected as "unsupported with
    /// the query-string grammar".
    #[test]
    fn an_explicit_operator_switches_off_the_query_grammar() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE og (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX og_idx ON og (body) USING FULLTEXT")
            .unwrap();
        for (id, body) in [(0, "alpha or beta"), (1, "alpha beta"), (2, "gamma")] {
            e.execute(&format!("INSERT INTO og VALUES ({id}, '{body}');"))
                .unwrap();
        }
        e.flush().unwrap();
        let sorted = |sql: &str| -> Vec<String> {
            let mut ids: Vec<String> = rows(&e, sql).iter().map(|r| r[0].clone()).collect();
            ids.sort();
            ids
        };
        // The term list is `alpha`, `or`, `beta`: the disjunction reaches the doc
        // that spells `or` out and the one that does not.
        assert_eq!(
            sorted(
                "SELECT id FROM og SEARCH MATCH TEXT (body, 'alpha OR beta', 10, 'operator=or');"
            ),
            vec!["0".to_string(), "1".to_string()]
        );
        // ... and requiring all three terms keeps only the doc that has them.
        assert_eq!(
            sorted(
                "SELECT id FROM og SEARCH MATCH TEXT (body, 'alpha OR beta', 10, 'operator=and');"
            ),
            vec!["0".to_string()]
        );
        // `block_max` only picks an early-termination algorithm, so it is accepted
        // on a parsed query too (`EarlyTermAlgo`): same rows as without it.
        assert_eq!(
            sorted(
                "SELECT id FROM og SEARCH MATCH TEXT (body, 'alpha OR beta', 10, 'block_max=compare');"
            ),
            vec!["0".to_string(), "1".to_string()]
        );
    }

    /// The reference's query-string grammar (`SearchDriver::ParseSingle` +
    /// `QueryBuilder::CreateSearch`): `AND` / `OR` / `NOT`, groups and
    /// `column:term`. `alpha NOT beta` is the reference's `AndNotQueryNode`,
    /// and a group is a real subtree, not a flat term list.
    #[test]
    fn search_supports_the_query_string_grammar() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bt (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX bt_idx ON bt (body) USING FULLTEXT")
            .unwrap();
        for (id, body) in [
            (0, "alpha beta"),
            (1, "alpha"),
            (2, "beta"),
            (3, "alpha x beta"),
            (4, "gamma"),
            (5, "alpha gamma"),
        ] {
            e.execute(&format!("INSERT INTO bt VALUES ({id}, '{body}');"))
                .unwrap();
        }
        e.flush().unwrap();

        let ids = |query: &str| {
            let mut out: Vec<String> = rows(
                &e,
                &format!("SELECT id FROM bt SEARCH MATCH TEXT (body, '{query}', 10);"),
            )
            .iter()
            .map(|r| r[0].clone())
            .collect();
            out.sort();
            out
        };

        assert_eq!(
            ids("alpha AND beta"),
            vec!["0".to_string(), "3".to_string()],
            "AND is an intersection"
        );
        assert_eq!(
            ids("alpha OR beta"),
            vec![
                "0".to_string(),
                "1".to_string(),
                "2".to_string(),
                "3".to_string(),
                "5".to_string()
            ],
            "OR is a union"
        );
        assert_eq!(
            ids("alpha AND NOT beta"),
            vec!["1".to_string(), "5".to_string()],
            "NOT excludes the right-hand documents"
        );
        assert_eq!(
            ids("alpha NOT beta"),
            vec!["1".to_string(), "5".to_string()],
            "`a NOT b` is the reference's AndNotQueryNode"
        );
        assert_eq!(
            ids("(alpha OR beta) AND gamma"),
            vec!["5".to_string()],
            "a group is evaluated as a subtree"
        );
        assert_eq!(
            ids("body:alpha"),
            vec![
                "0".to_string(),
                "1".to_string(),
                "3".to_string(),
                "5".to_string()
            ],
            "a field qualifier restricts the term to that column"
        );
        assert!(
            ids("\"alpha beta\" AND gamma").is_empty(),
            "a phrase inside an AND still needs adjacency"
        );
        assert_eq!(
            ids("\"alpha beta\" AND alpha"),
            vec!["0".to_string()],
            "a phrase and a term can be combined"
        );
    }

    /// Writes one full-text chunk for `rows` (every document holds `term` once).
    fn write_chunk(dir: &str, base: u64, rows: u32, term: &str) {
        use std::collections::BTreeMap;
        let docs: Vec<(u64, u32)> = (base..base + u64::from(rows)).map(|row| (row, 1)).collect();
        let mut postings: BTreeMap<String, Vec<(u64, u32, Vec<u32>)>> = BTreeMap::new();
        postings.insert(
            term.to_string(),
            docs.iter().map(|(row, _)| (*row, 1, vec![0])).collect(),
        );
        storage::invertedindex::disk_segment_reader::write_segment_index(
            dir,
            &format!("ft_{base:016x}"),
            base,
            &docs,
            &postings,
            storage::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
    }

    /// The block-max WAND's cursor (`TermStream::advance`) hands a target that is
    /// past the loaded block to the merged cursor, which hops the sealed
    /// segment's blocks through the doc skip list
    /// (`TermDocIterator::NextShallow` -> `PostingIterator::SkipTo`). A shallow
    /// reposition over a long posting list therefore costs one block decode, not
    /// one per 128 documents.
    #[test]
    fn a_shallow_seek_on_a_sealed_segment_hops_blocks() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        // 5000 rows: 39 full blocks of 128 postings plus a partial one.
        write_chunk(d, 0, 5000, "a");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        assert_eq!(index.load_sealed(), 5000);
        let _ = crate::take_decode_stats();
        let mut stream = index.term_stream("a");
        assert_eq!(stream.doc(), Some(0));
        assert_eq!(crate::take_decode_stats().0, 1, "the first block");
        stream.advance(4900);
        assert_eq!(stream.doc(), Some(4900));
        // The cursor seeked instead of decoding blocks one by one: the stream
        // refilled **once** from the position the seek landed on, so its block
        // starts at the target and not at the next unread posting. That the seek
        // itself hops the sealed segment's blocks is asserted in `storage`
        // (`advance_hops_blocks_with_the_skip_list`).
        assert_eq!(stream.docs.first().copied(), Some(4900));
        let (blocks, _) = crate::take_decode_stats();
        assert_eq!(blocks, 1, "the seek refilled the block more than once");
        // Past the hop the cursor steps one document at a time again.
        stream.advance(4901);
        assert_eq!(stream.doc(), Some(4901));
        stream.advance(4902);
        assert_eq!(stream.doc(), Some(4902));
        assert_eq!(crate::take_decode_stats().0, 0, "still the same block");
    }

    /// A `MATCH TEXT (…, WHERE …)` filter reaches the scorers as a row list they
    /// seek with, so the rows it rejects are neither fetched nor scored. The
    /// answer must be exactly the unfiltered one narrowed.
    #[test]
    fn a_match_text_filter_narrows_the_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Eight rows per seal: several chunks plus a memory tail, so the cursors
        // of a sealed segment and the ones of the tail both take the pushdown.
        e.set_memindex_capacity(8);
        e.execute("CREATE TABLE fts (id INT, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX fts_i ON fts (body) USING FULLTEXT")
            .unwrap();
        for i in 0..40 {
            e.execute(&format!("INSERT INTO fts VALUES ({i}, 'alpha row{i}');"))
                .unwrap();
        }
        e.flush().unwrap();
        let all = rows(
            &e,
            "SELECT id FROM fts SEARCH MATCH TEXT (body, 'alpha', 'topn=100');",
        );
        assert_eq!(all.len(), 40, "every row holds the term");
        let filtered = rows(
            &e,
            "SELECT id FROM fts SEARCH MATCH TEXT (body, 'alpha', 'topn=100', WHERE id < 10);",
        );
        let want: Vec<Vec<String>> = all
            .iter()
            .filter(|row| row[0].parse::<i32>().unwrap() < 10)
            .cloned()
            .collect();
        assert_eq!(want.len(), 10, "the fixture admits ten rows");
        assert_eq!(
            filtered, want,
            "the pushed-down filter changed which rows survive, or their order"
        );
        // A filter that admits nothing is an empty answer, not an error.
        assert!(rows(
            &e,
            "SELECT id FROM fts SEARCH MATCH TEXT (body, 'alpha', 'topn=100', WHERE id > 99);",
        )
        .is_empty());
    }

    /// A run of one chunk is skipped (`chunk_ids.size() <= 1`), so two chunks
    /// separated by a gap both stay.
    #[test]
    fn optimize_skips_a_run_of_one_chunk() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        write_chunk(d, 0, 2, "a");
        write_chunk(d, 10, 2, "b");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        index.load_sealed();
        assert_eq!(index.reader.segments.len(), 2);
        assert_eq!(index.optimize_chunks().unwrap(), 0);
        assert_eq!(index.reader.segments.len(), 2, "both chunks stay");
    }

    /// `Engine::fulltext_stats` reports where a query's cost comes from: the
    /// chunk count (every query probes all of them), the unsealed tail, the spill
    /// runs the next seal will merge, and the stale copies left by deletes.
    #[test]
    fn fulltext_stats_reports_the_chunk_and_tail_shape() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Two rows per chunk: six rows become three sealed chunks.
        e.set_memindex_capacity(2);
        e.execute("CREATE TABLE stat (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX stat_idx ON stat (body) USING FULLTEXT")
            .unwrap();
        for i in 0..6 {
            e.execute(&format!("INSERT INTO stat VALUES ({i}, 'alpha row{i}');"))
                .unwrap();
        }
        let stats = e.fulltext_stats("stat");
        assert_eq!(stats.len(), 1, "one full-text column");
        let stats = &stats[0];
        assert_eq!(stats.index, "body");
        assert_eq!(stats.chunks, 3);
        assert_eq!(stats.chunk_rows, 6);
        assert_eq!(stats.tail_rows, 0, "the trigger sealed every chunk");
        assert_eq!(stats.spill_runs, 0);
        assert_eq!(stats.inflight_rows, 0);
        assert!(stats.last_seal_us > 0, "the seal was timed");

        // A delete touches no index file: the row's postings stay in its chunk
        // and the scan drops them through the tombstone. `overridden` only tracks
        // a row re-indexed **in place**, which the append-only `UPDATE` path does
        // not produce (it appends a new row id), so the set stays empty.
        e.execute("DELETE FROM stat WHERE id = 0;").unwrap();
        let stats = e.fulltext_stats("stat");
        assert_eq!(stats[0].chunks, 3, "a delete does not seal anything");
        assert_eq!(
            stats[0].overridden_rows, 0,
            "no override marks for a delete"
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM stat SEARCH MATCH TEXT (body, 'alpha', 100);"
            )
            .len(),
            5,
            "the deleted row is filtered by the tombstone, not by the index"
        );
    }

    /// The rows of one statement are tokenised **before** the table's write gate
    /// is taken (`prepare_text_terms`), then inserted by position
    /// (`base + position`). A batch larger than one row is where a position/
    /// row-id mix-up would show: every row carries its own term, so the terms
    /// must land on the row that owns them and nowhere else.
    #[test]
    fn a_batch_tokenised_before_the_gate_indexes_every_row_once() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE batch (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX batch_idx ON batch (body) USING FULLTEXT")
            .unwrap();
        e.execute(
            "INSERT INTO batch VALUES \
             (0, 'unique0 common'), (1, 'unique1 common'), (2, 'unique2 common'), \
             (3, 'unique3 common'), (4, 'unique4 common'), (5, 'unique5 common');",
        )
        .unwrap();
        for id in 0..6 {
            let hits = rows(
                &e,
                &format!("SELECT id FROM batch SEARCH MATCH TEXT (body, 'unique{id}', 100);"),
            );
            assert_eq!(
                hits,
                vec![vec![id.to_string()]],
                "unique{id} belongs to row {id} only"
            );
        }
        // A term every row shares still finds all six, once each.
        let common = rows(
            &e,
            "SELECT id FROM batch SEARCH MATCH TEXT (body, 'common', 100);",
        );
        assert_eq!(common.len(), 6);
        assert_eq!(
            e.fulltext_stats("batch")[0].chunk_rows + e.fulltext_stats("batch")[0].tail_rows,
            6
        );
    }

    /// Timing probe for same-table insert concurrency, run by hand:
    /// `cargo test --lib probe_same_table_insert_concurrency -- --ignored --nocapture`.
    #[test]
    #[ignore = "timing probe"]
    fn probe_same_table_insert_concurrency() {
        for threads in [1usize, 2, 4] {
            let dir = tempdir().unwrap();
            let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
            e.execute("CREATE TABLE conc (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX conc_idx ON conc (body) USING FULLTEXT")
                .unwrap();
            let words: String = (0..40).map(|w| format!("word{w} ")).collect::<String>();
            let started = std::time::Instant::now();
            let mut handles = Vec::new();
            for t in 0..threads {
                let e = e.clone();
                let words = words.clone();
                handles.push(std::thread::spawn(move || {
                    for i in 0..(2000 / threads) {
                        e.execute(&format!(
                            "INSERT INTO conc VALUES ({}, '{words}');",
                            1000 * t + i
                        ))
                        .unwrap();
                    }
                }));
            }
            for handle in handles {
                handle.join().unwrap();
            }
            eprintln!(
                "PROBE threads={threads} rows=2000 elapsed={:?}",
                started.elapsed()
            );
        }
    }

    /// Timing probe for the tombstone check of a bulk delete:
    /// `Engine::delete_row_ids` asks `is_deleted` once per row, so a linear
    /// scan of the journal makes each pass slower than the one before while
    /// the hashed set keeps them flat.
    /// `cargo test --lib probe_bulk_delete_journal_scan -- --ignored --nocapture`.
    #[test]
    #[ignore = "timing probe"]
    fn probe_bulk_delete_journal_scan() {
        const BATCH: usize = 4_000;
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE del (id INTEGER)").unwrap();
        for chunk in 0..3 {
            let values: String = (0..BATCH)
                .map(|i| format!("({})", chunk * BATCH + i))
                .collect::<Vec<_>>()
                .join(",");
            e.execute(&format!("INSERT INTO del VALUES {values}"))
                .unwrap();
        }
        for pass in 0..3u32 {
            let ids: Vec<(u32, u32)> = (0..BATCH as u32)
                .map(|i| (0, pass * BATCH as u32 + i))
                .collect();
            let started = std::time::Instant::now();
            let mut txn = e.begin_txn();
            assert_eq!(txn.delete_row_ids("del", &ids).unwrap(), BATCH);
            txn.commit().unwrap();
            eprintln!(
                "PROBE delete pass={pass} journal_before={} rows={BATCH} elapsed={:?}",
                pass as usize * BATCH,
                started.elapsed()
            );
        }
    }

    /// The merge itself runs **with the column's lock released**
    /// (`optimize_text_index_unlocked`): the plan is made from the chunk headers,
    /// the files are rewritten unlocked, and the swap is refused when the chunk
    /// list moved in the meantime — otherwise a seal that appended a chunk while
    /// the merge was running would be overwritten by the stale list.
    #[test]
    fn a_merge_is_dropped_when_the_chunk_list_moved_while_it_ran() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        write_chunk(d, 0, 2, "a");
        write_chunk(d, 2, 2, "a");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        index.load_sealed();
        let plan = index
            .plan_merge()
            .unwrap()
            .expect("two contiguous chunks are one run");

        // A seal lands while the merge is writing its files.
        write_chunk(d, 4, 2, "a");
        let reader = storage::invertedindex::disk_segment_reader::SegmentIndexReader::open(
            d,
            "ft_0000000000000004",
            4,
            storage::invertedindex::format::OPTION_FLAG_ALL,
        )
        .unwrap();
        index.reader.segments.push(SealedSegment {
            docs: 2,
            total_len: 0,
            reader,
        });

        let merged = TextIndex::merge_runs(d, &plan);
        assert_eq!(
            index.apply_merge(&plan, merged),
            0,
            "the plan no longer matches the chunk list"
        );
        assert_eq!(index.reader.segments.len(), 3, "no chunk was replaced");
        let leftover = format!("{d}/ft_0000000000000000_4.len");
        assert!(
            !std::path::Path::new(&leftover).exists(),
            "the merged chunk of the stale plan is removed"
        );
        // The list is still fully readable.
        assert_eq!(index.total_docs(), 3 * 2);
    }

    /// **Deviation from the reference, deliberate**: every contiguous run is
    /// merged, not just the prefix before the first gap. Stopping at the gap
    /// (`new_txn_index_impl.cpp:2219-2227`) would leave the chunk list frozen
    /// after one hole, and every query pays for the extra segments.
    #[test]
    fn optimize_merges_every_contiguous_run() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        write_chunk(d, 0, 2, "a");
        write_chunk(d, 2, 2, "a");
        write_chunk(d, 10, 2, "a");
        write_chunk(d, 12, 2, "a");
        write_chunk(d, 20, 2, "a");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        index.load_sealed();
        assert_eq!(index.reader.segments.len(), 5);
        assert_eq!(
            index.optimize_chunks().unwrap(),
            2,
            "the two runs of two merge; the lone chunk does not"
        );
        let bases: Vec<u64> = index
            .reader
            .segments
            .iter()
            .map(|s| s.reader.base_row_id())
            .collect();
        assert_eq!(bases, vec![0, 10, 20], "one merged chunk per run");
        assert_eq!(index.reader.segments[0].docs, 4);
        assert_eq!(index.reader.segments[1].docs, 4);
        assert_eq!(index.reader.segments[2].docs, 2);
    }

    /// The contiguous prefix is merged; the chunks behind the gap stay as they
    /// are and keep their row ranges.
    #[test]
    fn optimize_merges_the_prefix_and_keeps_the_rest() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        write_chunk(d, 0, 2, "a");
        write_chunk(d, 2, 2, "a");
        write_chunk(d, 10, 2, "a");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        index.load_sealed();
        assert_eq!(index.reader.segments.len(), 3);
        assert_eq!(index.optimize_chunks().unwrap(), 1);
        assert_eq!(
            index.reader.segments.len(),
            2,
            "one merged prefix plus the tail"
        );
        assert_eq!(index.reader.segments[0].docs, 4);
        assert_eq!(index.reader.segments[0].reader.base_row_id(), 0);
        assert_eq!(index.reader.segments[1].reader.base_row_id(), 10);
        // The merged chunk is named after the row range it covers.
        assert_eq!(
            index.reader.segments[0].reader.base_name(),
            storage::fulltext_base_name(0, Some(4))
        );
        // Every row of the prefix is still reachable through the merged copy.
        for row in 0..4u64 {
            assert!(
                index.reader.segments[0].reader.ordinal(row).is_some(),
                "row {row}"
            );
        }
    }

    /// An overlapping chunk list is unrecoverable in the reference
    /// (`new_txn_index_impl.cpp:2228-2233`); merging it would produce a doc list
    /// no reference run could produce, so it is refused.
    #[test]
    fn optimize_refuses_overlapping_chunks() {
        let dir = tempdir().unwrap();
        let d = dir.path().to_str().unwrap();
        write_chunk(d, 0, 2, "a");
        write_chunk(d, 1, 2, "a");
        let mut index = TextIndex::new(d.to_string(), TextTokenizer::default());
        index.load_sealed();
        assert_eq!(index.reader.segments.len(), 2);
        assert!(index.optimize_chunks().is_err());
        assert_eq!(
            index.reader.segments.len(),
            2,
            "the chunks survive the refusal"
        );
    }

    /// An `UPDATE` is the reference's delete-plus-append, so the rewritten row
    /// gets a **fresh** `ROW_ID` past every sealed chunk: its chunk starts where
    /// the previous one ended, and the chunk list stays gap-free and disjoint.
    ///
    /// This is what keeps `optimize` (which refuses a chunk starting *before*
    /// the run it belongs to) safe after updates, and what keeps `load_sealed`
    /// (which drops a plain chunk whose base falls inside a merged chunk) from
    /// deleting the copy that carries the new text. If an update ever re-used
    /// the old row id, both would break - hence the assertions.
    #[test]
    fn an_update_lands_in_a_chunk_after_the_sealed_range() {
        let dir = tempdir().unwrap();
        let query = "SELECT id FROM upd SEARCH MATCH TEXT (body, 'gamma', 100);";
        let stale = "SELECT id FROM upd SEARCH MATCH TEXT (body, 'alpha', 100);";
        let hit = || vec![vec!["1".to_string()]];
        {
            let e = Engine::open(dir.path()).unwrap();
            // Two rows per chunk, so row 1 is sealed by the first one.
            e.set_memindex_capacity(2);
            e.execute("CREATE TABLE upd (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX upd_idx ON upd (body) USING FULLTEXT")
                .unwrap();
            for i in 0..6 {
                e.execute(&format!("INSERT INTO upd VALUES ({i}, 'alpha row{i}');"))
                    .unwrap();
            }
            e.flush().unwrap();
            assert!(fulltext_chunks(&e) > 1, "the table starts fragmented");

            e.execute("UPDATE upd SET body = 'gamma' WHERE id = 1;")
                .unwrap();
            // Dumping the tail is what turns the update into its own chunk.
            e.flush().unwrap();
            assert_eq!(rows(&e, query), hit(), "the new text is searchable");
            assert_eq!(rows(&e, stale).len(), 5, "the old text is gone");

            e.optimize_indexes()
                .expect("one update chunk must not make optimize refuse the list");
            assert_eq!(rows(&e, query), hit(), "the merge keeps the new text");
            assert_eq!(rows(&e, stale).len(), 5, "the merge cannot resurrect it");
        }
        // The restart path is where the stale copy used to come back.
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            rows(&e, query),
            hit(),
            "reopening dropped the update chunk and served the old text"
        );
        assert_eq!(rows(&e, stale).len(), 5, "the old text came back");
    }

    /// Lowers the full-text budget of every open index on `e`, so the next
    /// inserts spill their tail into sorted runs.
    ///
    /// `TEXT_INDEX_BUDGET_BYTES` (512 MiB) is unreachable from a test, and
    /// `INFINITY_TEXT_INDEX_BUDGET_BYTES` is read once per process, so the field
    /// is set directly: this is the only way to drive the **offline** dump
    /// (`for_each_merged_term` over several runs) instead of the realtime one.
    fn set_text_budget(e: &Engine, bytes: usize) {
        let indexes: Vec<_> = {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .flat_map(|entry| entry.text.values().cloned())
                .collect()
        };
        for shared in indexes {
            shared.write().unwrap().budget = bytes;
        }
    }

    /// Every file under `<dir>/fulltext`, keyed by its path below that root.
    fn fulltext_files(dir: &std::path::Path) -> std::collections::BTreeMap<String, Vec<u8>> {
        fn walk(
            root: &std::path::Path,
            at: &std::path::Path,
            out: &mut std::collections::BTreeMap<String, Vec<u8>>,
        ) {
            let Ok(entries) = std::fs::read_dir(at) else {
                return;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    walk(root, &path, out);
                } else {
                    let rel = path
                        .strip_prefix(root)
                        .map(|p| p.to_string_lossy().into_owned())
                        .unwrap_or_default();
                    out.insert(rel, std::fs::read(&path).unwrap_or_default());
                }
            }
        }
        let root = dir.join("fulltext");
        let mut out = std::collections::BTreeMap::new();
        walk(&root, &root, &mut out);
        out
    }

    /// The **offline** dump (`MemoryIndexer::spill` + `for_each_merged_term`)
    /// only runs when the resident tail outgrows its budget, which 512 MiB makes
    /// impossible in a test. Lowering the budget makes every insert spill, so the
    /// seal merges several sorted runs; the chunk it writes must be **byte for
    /// byte** what the resident (realtime) dump writes for the same rows.
    #[test]
    fn a_spilled_build_writes_the_same_chunk_as_a_resident_one() {
        const ROWS: usize = 20;
        let resident = tempdir().unwrap();
        let spilled = tempdir().unwrap();
        let query = "SELECT id FROM offl SEARCH MATCH TEXT (body, 'alpha', 100);";

        let build = |dir: &std::path::Path, budget: Option<usize>| {
            let e = Engine::open(dir).unwrap();
            e.execute("CREATE TABLE offl (id INTEGER, body VARCHAR)")
                .unwrap();
            e.execute("CREATE INDEX offl_idx ON offl (body) USING FULLTEXT")
                .unwrap();
            // One seal at the twentieth row, so both builds write one chunk.
            e.set_memindex_capacity(ROWS);
            for i in 0..ROWS {
                e.execute(&format!(
                    "INSERT INTO offl VALUES ({i}, 'alpha beta row{i}');"
                ))
                .unwrap();
                if i == 0 {
                    if let Some(bytes) = budget {
                        set_text_budget(&e, bytes);
                    }
                }
            }
            e
        };

        let expected;
        {
            let resident_engine = build(resident.path(), None);
            let spilled_engine = build(spilled.path(), Some(1));
            // The spilled column really did go through the offline path: one
            // sealed chunk, no run left behind and nothing in the tail.
            {
                let tables = spilled_engine.tables.lock().unwrap();
                for shared in tables.values().flat_map(|entry| entry.text.values()) {
                    let guard = shared.read().unwrap();
                    assert_eq!(guard.reader.segments.len(), 1, "one sealed chunk");
                    assert_eq!(guard.mem.runs.len(), 0, "the seal consumed the runs");
                    assert_eq!(guard.mem.pending_docs(), 0, "the tail is empty");
                }
            }
            let a = fulltext_files(resident.path());
            let b = fulltext_files(spilled.path());
            assert_eq!(
                a.keys().collect::<Vec<_>>(),
                b.keys().collect::<Vec<_>>(),
                "the two builds must write the same files"
            );
            for (name, bytes) in &a {
                assert_eq!(
                    bytes,
                    b.get(name).unwrap(),
                    "{name} differs: the offline dump does not reproduce the realtime one"
                );
            }
            expected = rows(&resident_engine, query);
            assert_eq!(
                expected,
                rows(&spilled_engine, query),
                "the two builds must answer the same rows"
            );
            assert_eq!(expected.len(), ROWS);
        }

        // And the chunk is the one a restart maps.
        let reopened = Engine::open(spilled.path()).unwrap();
        assert_eq!(rows(&reopened, query), expected);
    }

    /// Rows as display text, for assertions.
    fn texts(rows: &[Vec<Value>]) -> Vec<Vec<String>> {
        rows.iter()
            .map(|r| r.iter().map(|v| v.to_slt_string()).collect())
            .collect()
    }

    fn row_text(rows: &[Vec<Value>], i: usize) -> Vec<String> {
        rows[i].iter().map(|v| v.to_slt_string()).collect()
    }

    /// Round-4 review G1: prove that production `run_plan` (not `plan_select`)
    /// applies the optimizer. A hand-built plan keeps the unused column in the
    /// scan, so only the `ColumnPruner` inside `run_plan` can remove it.
    #[test]
    fn run_plan_runs_the_production_optimizer() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rp (a INT, b INT);").unwrap();
        e.execute("INSERT INTO rp VALUES (1, 2);").unwrap();
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Scan {
                table: "default_db.rp".to_string(),
                columns: vec!["a".to_string(), "b".to_string()],
            }),
            exprs: vec![expression::Expr::Column(0)],
            names: vec!["a".to_string()],
        };
        let (_columns, rows) = e
            .run_plan(
                plan,
                None,
                Some(vec!["a".to_string(), "b".to_string()]),
                None,
                false,
                false,
                None,
                false,
            )
            .unwrap();
        assert_eq!(rows, vec![vec![Value::Int(1)]]);
        let debug = LAST_PLAN_DEBUG.with(|slot| slot.borrow().clone()).unwrap();
        assert!(debug.contains("columns: [\"a\"]"), "{debug}");
        assert!(!debug.contains("\"b\""), "{debug}");
    }

    /// The operator model must actually drive the query, not just exist: a
    /// plan that `operators::lower` understands runs through
    /// `operators::run_rows`, and produces the same rows as before.
    #[test]
    fn run_plan_executes_a_projection_through_the_operator_model() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE om (a INT, b INT);").unwrap();
        e.execute("INSERT INTO om VALUES (1, 2), (3, 4);").unwrap();
        let before = crate::operator::operator_run_count();
        let plan = LogicalPlan::Projection {
            input: Box::new(LogicalPlan::Scan {
                table: "default_db.om".to_string(),
                columns: vec!["a".to_string(), "b".to_string()],
            }),
            exprs: vec![expression::Expr::Binary(
                Box::new(expression::Expr::Column(0)),
                parser::BinOp::Add,
                Box::new(expression::Expr::Column(1)),
            )],
            names: vec!["sum".to_string()],
        };
        let (columns, rows) = e
            .run_plan(plan, None, None, None, false, false, None, false)
            .unwrap();
        assert_eq!(columns, vec!["sum".to_string()]);
        assert_eq!(rows, vec![vec![Value::Int(3)], vec![Value::Int(7)]]);
        assert_eq!(crate::operator::operator_run_count(), before + 1);
    }

    /// A `WHERE` runs through the filter operator, and a predicate that matches
    /// nothing still reports the projected column names.
    #[test]
    fn run_plan_executes_a_filter_through_the_operator_model() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE of (a INT, b INT);").unwrap();
        e.execute("INSERT INTO of VALUES (1, 10), (2, 20), (3, 30);")
            .unwrap();
        let before = crate::operator::operator_run_count();
        // An expression in the select list keeps the plan on the operator
        // model; a plain column list is answered by `direct_projection`.
        assert_eq!(
            rows(&e, "SELECT b + 1 FROM of WHERE a > 1;"),
            vec![vec!["21".to_string()], vec!["31".to_string()]]
        );
        assert_eq!(crate::operator::operator_run_count(), before + 1);
        assert_eq!(
            rows(&e, "SELECT b FROM of WHERE a > 1;"),
            vec![vec!["20".to_string()], vec!["30".to_string()]]
        );
        // No match: the projection still defines the schema.
        let mut result = e.execute("SELECT b FROM of WHERE a > 9;").unwrap();
        let QueryResult::Rows { columns, rows } = result.remove(0) else {
            panic!("expected rows");
        };
        assert_eq!(columns, vec!["b".to_string()]);
        assert!(rows.is_empty());
    }

    /// LIMIT/OFFSET and ORDER BY ... LIMIT run through the limit and top
    /// operators, and produce the same rows as the stage pipeline.
    #[test]
    fn run_plan_executes_limit_and_top_through_the_operator_model() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ol (a INT, b INT);").unwrap();
        e.execute("INSERT INTO ol VALUES (1, 10), (2, 20), (3, 10), (4, 40);")
            .unwrap();

        let before = crate::operator::operator_run_count();
        assert_eq!(
            rows(&e, "SELECT b FROM ol LIMIT 2;"),
            vec![vec!["10".to_string()], vec!["20".to_string()]]
        );
        assert_eq!(crate::operator::operator_run_count(), before + 1);
        assert_eq!(
            crate::operator::last_chain(),
            vec![
                crate::operator::PhysicalOperatorType::Limit,
                crate::operator::PhysicalOperatorType::Projection
            ]
        );

        assert_eq!(
            rows(&e, "SELECT b FROM ol LIMIT 2 OFFSET 1;"),
            vec![vec!["20".to_string()], vec!["10".to_string()]]
        );
        // ORDER BY ties keep insertion order (`a` 1 before `a` 3 both have 10).
        assert_eq!(
            rows(&e, "SELECT b FROM ol ORDER BY b LIMIT 3;"),
            vec![
                vec!["10".to_string()],
                vec!["10".to_string()],
                vec!["20".to_string()]
            ]
        );
        assert_eq!(
            rows(&e, "SELECT b FROM ol ORDER BY b DESC LIMIT 1;"),
            vec![vec!["40".to_string()]]
        );
        // ORDER BY ... LIMIT collapses into one fused Top.
        assert_eq!(
            crate::operator::last_chain(),
            vec![
                crate::operator::PhysicalOperatorType::Projection,
                crate::operator::PhysicalOperatorType::Top
            ]
        );
        assert_eq!(
            rows(&e, "SELECT b FROM ol ORDER BY b DESC LIMIT 2 OFFSET 1;"),
            vec![vec!["20".to_string()], vec!["10".to_string()]]
        );
    }

    /// Eight tables inserting at once: every table's index answers for its own
    /// rows, and the index work runs on that table's own bottom job (there is no
    /// appender thread to queue behind).
    #[test]
    fn concurrent_inserts_on_different_tables_keep_each_index() {
        let dir = tempdir().unwrap();
        let e = Arc::new(Engine::open(dir.path()).unwrap());
        e.attach_bottom_pool();
        for t in 0..8 {
            e.execute(&format!("CREATE TABLE ct{t} (id INT, body VARCHAR);"))
                .unwrap();
            e.execute(&format!(
                "CREATE INDEX ct{t}_body ON ct{t} (body) WITH (analyzer = 'standard');"
            ))
            .unwrap();
        }
        let before = bottom_work_count();
        let mut handles = Vec::new();
        for t in 0..8 {
            let e = Arc::clone(&e);
            handles.push(std::thread::spawn(move || {
                for i in 0..20 {
                    e.execute(&format!(
                        "INSERT INTO ct{t} VALUES ({i}, 'word{i} of t{t}');"
                    ))
                    .unwrap();
                }
            }));
        }
        for handle in handles {
            handle.join().unwrap();
        }
        assert!(
            bottom_work_count() >= before + 8,
            "every table's insert ran its own bottom"
        );
        for t in 0..8 {
            assert_eq!(
                rows(&e, &format!("SELECT count(*) FROM ct{t};")),
                vec![vec!["20".to_string()]]
            );
            assert_eq!(
                rows(
                    &e,
                    &format!("SELECT count(*) FROM ct{t} WHERE filter_fulltext('body', 'word3');")
                ),
                vec![vec!["1".to_string()]],
                "table {t} finds its own row"
            );
        }
    }

    /// GROUP BY and global aggregates run through the aggregate operator, keep
    /// first-seen group order, and still produce one row over an empty relation.
    /// An `INSERT` on a pooled engine runs its bottom half on the table's
    /// worker and waits for it: the rows, the index entries and the block
    /// mirror are in place when the statement returns.
    #[test]
    fn an_insert_runs_its_bottom_half_on_the_bottom_executor() {
        let dir = tempdir().unwrap();
        let e = Arc::new(Engine::open(dir.path()).unwrap());
        e.attach_bottom_pool();
        e.execute("CREATE TABLE be (a INT, b VARCHAR);").unwrap();
        e.execute("CREATE INDEX be_b ON be (b);").unwrap();

        let before = bottom_work_count();
        e.execute("INSERT INTO be VALUES (1, 'x'), (2, 'y');")
            .unwrap();
        assert_eq!(
            bottom_work_count(),
            before + 1,
            "the insert went through the pool"
        );
        // The worker's writes are visible: rows, the index entry and the counts.
        assert_eq!(
            rows(&e, "SELECT a FROM be WHERE b = 'y';"),
            vec![vec!["2".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM be;"),
            vec![vec!["2".to_string()]]
        );
        e.flush().unwrap();
        assert!(dir.path().join("data").exists());
    }

    #[test]
    fn emvb_index_options_are_validated_like_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE em (id INT, v TENSOR(FLOAT,2), m MULTIVECTOR(FLOAT,2));")
            .unwrap();
        for sql in [
            // a multi-vector column is not a tensor column
            "CREATE INDEX a ON em (m) USING EMVB WITH (pq_subspace_num = 2, pq_subspace_bits = 8);",
            // bits must be 8 or 16
            "CREATE INDEX c ON em (v) USING EMVB WITH (pq_subspace_num = 2, pq_subspace_bits = 4);",
            // unknown option
            "CREATE INDEX d ON em (v) USING EMVB WITH (nbits = 4);",
            // the reference defaults both options to 0 and rejects that, so
            // omitting one is an error
            "CREATE INDEX e ON em (v) USING EMVB WITH (pq_subspace_bits = 8);",
            "CREATE INDEX f ON em (v) USING EMVB WITH (pq_subspace_num = 2);",
        ] {
            assert!(e.execute(sql).is_err(), "{sql} should be rejected");
        }
        e.execute("CREATE INDEX ok_i ON em (v) USING EMVB WITH (pq_subspace_num = 2, pq_subspace_bits = 8);")
            .unwrap();
        // "dimension % subspace_num != 0" is only checked by `GetEMVBOPQ`
        // when the index is built, so the DDL itself succeeds (the reference
        // accepts it too; `parallel_test` relies on that).
        e.execute("CREATE INDEX ok_odd ON em (v) USING EMVB WITH (pq_subspace_num = 8, pq_subspace_bits = 8);")
            .unwrap();
    }

    /// `Storage::txn_allocator_`: every append's row ids come from one thread
    /// (`TxnAllocator::Process` -> `AllocateRowID`), which is what keeps them
    /// contiguous when several transactions allocate at the same time.
    #[test]
    fn row_ids_are_allocated_on_the_allocator_thread() {
        let dir = tempdir().unwrap();
        let e = Arc::new(Engine::open(dir.path()).unwrap());
        e.install_engine_handle();
        e.execute("CREATE TABLE alloc_t (a INT);").unwrap();
        let before = e.allocator_jobs.load(std::sync::atomic::Ordering::SeqCst);
        e.execute("INSERT INTO alloc_t VALUES (1), (2), (3);")
            .unwrap();
        assert!(
            e.allocator_jobs.load(std::sync::atomic::Ordering::SeqCst) > before,
            "the ids must come from the allocator thread, not the inline fallback"
        );
        // Two concurrent appends still get disjoint, contiguous ranges.
        let mut handles = Vec::new();
        for _ in 0..4 {
            let e = Arc::clone(&e);
            handles.push(std::thread::spawn(move || {
                e.execute("INSERT INTO alloc_t VALUES (9);").unwrap();
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert_eq!(
            rows(&e, "SELECT count(*) FROM alloc_t;"),
            vec![vec!["7".to_string()]]
        );
        let ids = rows(&e, "SELECT a FROM alloc_t;");
        assert_eq!(ids.len(), 7);
    }

    /// The write gate is per table: a batch on one table does not block a batch
    /// on another, which is the parallelism the reference's per-block
    /// `BlockLock` gives (`batch_locks`).
    #[test]
    fn write_gates_are_per_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        let a = e.batch_gate("db1.t1");
        let b = e.batch_gate("db1.t2");
        assert!(
            !Arc::ptr_eq(&a, &b),
            "different tables need different gates"
        );
        assert!(
            Arc::ptr_eq(&a, &e.batch_gate("db1.t1")),
            "the same table keeps one gate"
        );
        let held = a.lock().unwrap();
        // The other table is still writable while this one's batch runs.
        assert!(b.try_lock().is_ok());
        drop(held);
    }

    /// `EXPLAIN FRAGMENT` prints the fragment envelope the reference prints:
    /// title, sink, every operator (the root gets an arrow), then the source.
    #[test]
    fn explain_fragment_wraps_a_plain_plan() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ef (a INT, b INT);").unwrap();

        let physical = rows(&e, "EXPLAIN PHYSICAL SELECT a FROM ef WHERE b = 1;");
        let fragment = rows(&e, "EXPLAIN FRAGMENT SELECT a FROM ef WHERE b = 1;");
        assert_eq!(
            fragment[0],
            vec!["FRAGMENT (1, ParallelStream)".to_string()]
        );
        assert!(fragment[1][0].starts_with("-> SINK ("), "{:?}", fragment[1]);
        // The plan follows the envelope: the root operator gains an arrow, the
        // rest is the physical text.
        assert_eq!(fragment[2][0], format!("->{}", physical[0][0]));
        // `ExplainFragment::Explain` appends a blank element after each
        // fragment, so the plan text is followed by the source line and then
        // one empty line.
        assert!(
            fragment.last().unwrap()[0].is_empty(),
            "{:?}",
            fragment.last()
        );
        assert_eq!(&fragment[3..fragment.len() - 2], &physical[1..]);
        assert!(fragment[fragment.len() - 2][0].starts_with("-> SOURCE ("));
    }

    /// A blocking operator turns the fragment into a parallel materialize, as
    /// `FragmentBuilder` does for `TOP`/`AGGREGATE`.
    #[test]
    fn explain_fragment_type_follows_the_blocking_operator() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE eft (a INT, b INT);").unwrap();
        for (sql, want) in [
            ("EXPLAIN FRAGMENT SELECT a FROM eft;", "ParallelStream"),
            (
                "EXPLAIN FRAGMENT SELECT a FROM eft ORDER BY a LIMIT 3;",
                "ParallelMaterialize",
            ),
            (
                "EXPLAIN FRAGMENT SELECT count(*) FROM eft;",
                "ParallelMaterialize",
            ),
        ] {
            let out = rows(&e, sql);
            assert_eq!(out[0], vec![format!("FRAGMENT (1, {want})")], "{sql}");
        }
    }

    /// Splitting the row-wise prefix into tasklets must not cost the early stop
    /// of `LIMIT`: the limit is the merge point, and the scan below it still
    /// stops once its budget is spent (`physical_limit_impl.cpp:209`).
    #[test]
    fn a_limit_over_a_big_table_still_stops_early() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lim (a INT);").unwrap();
        let input: Vec<Vec<Literal>> = (0..100_000).map(|i| vec![Literal::Int(i)]).collect();
        e.insert_literals("default_db.lim", None, input).unwrap();

        let before = crate::operator::project::projected_rows();
        let out = rows(&e, "SELECT a + 1 FROM lim LIMIT 3;");
        assert_eq!(
            out,
            vec![
                vec!["1".to_string()],
                vec!["2".to_string()],
                vec!["3".to_string()]
            ]
        );
        let projected = crate::operator::project::projected_rows() - before;
        assert!(
            projected <= crate::chunk::DEFAULT_VECTOR_SIZE,
            "projected {projected} rows"
        );
    }

    #[test]
    fn run_plan_executes_a_group_by_through_the_operator_model() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE og (a INT, b INT);").unwrap();
        e.execute("INSERT INTO og VALUES (1, 10), (2, 20), (3, 10), (4, 20), (5, 30);")
            .unwrap();

        let before = crate::operator::operator_run_count();
        assert_eq!(
            rows(&e, "SELECT b, count(*) FROM og GROUP BY b;"),
            vec![
                vec!["10".to_string(), "2".to_string()],
                vec!["20".to_string(), "2".to_string()],
                vec!["30".to_string(), "1".to_string()],
            ]
        );
        assert_eq!(crate::operator::operator_run_count(), before + 1);
        assert!(crate::operator::last_chain()
            .contains(&crate::operator::PhysicalOperatorType::Aggregate));
        // Global aggregates: one row, also when nothing matches.
        assert_eq!(
            rows(&e, "SELECT count(*) FROM og;"),
            vec![vec!["5".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT sum(a), avg(a), min(a), max(a) FROM og;"),
            vec![vec![
                "15".to_string(),
                "3.000000".to_string(),
                "1".to_string(),
                "5".to_string()
            ]]
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM og WHERE a > 99;"),
            vec![vec!["0".to_string()]]
        );
        // GROUP BY over an empty relation has no groups at all.
        assert!(rows(&e, "SELECT b, count(*) FROM og WHERE a > 99 GROUP BY b;").is_empty());
    }

    /// ORDER BY without a limit now lowers to the sort operator, which sorts
    /// chunk runs and merges them; ties still come out in insertion order.
    #[test]
    fn run_plan_executes_a_sort_through_the_operator_model() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE os (a INT, b INT);").unwrap();
        e.execute("INSERT INTO os VALUES (1, 30), (2, 10), (3, 20), (4, 10);")
            .unwrap();

        let before = crate::operator::operator_run_count();
        // `b` 10 comes from rows 2 and 4: insertion order keeps 2 first.
        assert_eq!(
            rows(&e, "SELECT a, b FROM os ORDER BY b;"),
            vec![
                vec!["2".to_string(), "10".to_string()],
                vec!["4".to_string(), "10".to_string()],
                vec!["3".to_string(), "20".to_string()],
                vec!["1".to_string(), "30".to_string()],
            ]
        );
        assert_eq!(crate::operator::operator_run_count(), before + 1);
        assert!(
            crate::operator::last_chain().contains(&crate::operator::PhysicalOperatorType::Sort)
        );
        assert_eq!(
            rows(&e, "SELECT a, b FROM os ORDER BY b DESC;"),
            vec![
                vec!["1".to_string(), "30".to_string()],
                vec!["3".to_string(), "20".to_string()],
                vec!["2".to_string(), "10".to_string()],
                vec!["4".to_string(), "10".to_string()],
            ]
        );
    }

    fn seed(e: &Engine) {
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "c"), (2, "a"), (3, "b"), (4, "a")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
    }

    #[test]
    fn a_derived_table_scans_the_subquery_result() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE d (id INT, s VARCHAR);").unwrap();
        e.execute("INSERT INTO d VALUES (1,'a'), (2,'b'), (3,'c');")
            .unwrap();
        // The outer query scans the subquery's rows, and may filter/sort them.
        assert_eq!(
            rows(&e, "SELECT id FROM (SELECT id FROM d) ORDER BY id DESC;"),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM (SELECT id FROM d WHERE id > 1) ORDER BY id;"
            ),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        // A table legitimately named `select3` is not mistaken for a subquery.
        e.execute("CREATE TABLE select3 (c1 INT, c2 INT, c3 INT);")
            .unwrap();
        e.execute("INSERT INTO select3 VALUES (1,2,3);").unwrap();
        assert_eq!(
            rows(&e, "SELECT c3, c2, c1 from select3;"),
            vec![vec!["3".to_string(), "2".to_string(), "1".to_string()]]
        );
    }

    /// Round-5 review: a subquery that reads the enclosing query is rejected
    /// explicitly. Folding it once as a constant would silently answer
    /// `true`/`false` for every outer row, which is what the old path did.
    #[test]
    fn a_correlated_subquery_sees_each_outer_row() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cs (id INT, v INT);").unwrap();
        e.execute("INSERT INTO cs VALUES (1, 10), (2, 20);")
            .unwrap();
        e.execute("CREATE TABLE cs2 (id INT);").unwrap();
        e.execute("INSERT INTO cs2 VALUES (2);").unwrap();
        // Uncorrelated: the subquery is evaluated on its own, so every outer
        // row sees the same answer.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM cs WHERE EXISTS (SELECT id FROM cs2) ORDER BY id;"
            ),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        // Correlated on the outer `cs.id`: only row 2 has a `cs2` match.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM cs WHERE EXISTS (SELECT id FROM cs2 WHERE cs2.id = cs.id) ORDER BY id;"
            ),
            vec![vec!["2".to_string()]]
        );
    }

    /// A CTE alias may shadow a base table. The correlated-subquery binder must
    /// not resolve the alias against the catalog and reject the query with the
    /// wrong schema.
    #[test]
    fn a_cte_alias_shadowing_a_table_is_not_bound_as_that_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE shadow (other INT);").unwrap();
        e.execute("INSERT INTO shadow VALUES (99);").unwrap();
        e.execute("CREATE TABLE src (id INT);").unwrap();
        e.execute("INSERT INTO src VALUES (1), (2);").unwrap();
        // `shadow` in the subquery's FROM is the CTE, which exposes `id`; the
        // base table of the same name only has `other`.
        assert_eq!(
            rows(
                &e,
                "WITH shadow AS (SELECT id FROM src) \
                 SELECT id FROM src WHERE EXISTS (SELECT id FROM shadow) ORDER BY id;"
            ),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    /// The enclosing query's FROM may itself be a CTE alias; then the CTE, not
    /// a same-named base table, supplies the correlation scope.
    #[test]
    fn an_outer_cte_supplies_the_correlation_scope() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE otc (id INT);").unwrap();
        e.execute("INSERT INTO otc VALUES (1);").unwrap();
        e.execute("CREATE TABLE otc2 (id INT);").unwrap();
        // The CTE supplies the outer rows; `otc2` is empty, so nothing matches.
        assert_eq!(
            rows(
                &e,
                "WITH cs AS (SELECT id FROM otc) \
                 SELECT id FROM cs WHERE EXISTS (SELECT id FROM otc2 WHERE otc2.id = cs.id);",
            ),
            Vec::<Vec<String>>::new()
        );
    }

    /// A `SEARCH MATCH (…) WHERE …` filter is compiled by the search path, not
    /// the planner, so a subquery there must be resolved (or rejected) before
    /// it can be folded into a constant.
    #[test]
    fn a_subquery_in_a_match_filter_is_not_folded_to_a_constant() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mf (id INT, body VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX mf_ft ON mf (body) USING FULLTEXT;")
            .unwrap();
        e.execute("INSERT INTO mf VALUES (1, 'x'), (2, 'x');")
            .unwrap();
        e.execute("CREATE TABLE mf2 (id INT);").unwrap();
        e.execute("INSERT INTO mf2 VALUES (1);").unwrap();
        let err = e
            .execute(
                "SELECT id FROM mf SEARCH MATCH TEXT ('body', 'x', 'topn=10', \
                 WHERE EXISTS (SELECT id FROM mf2 WHERE mf2.id = mf.id));",
            )
            .unwrap_err();
        assert!(
            err.to_string()
                .contains("correlated subquery is not supported here"),
            "{err}"
        );
    }

    /// `filter=` is an opaque option string parsed after resolution, so a
    /// subquery there must be rejected rather than folded to NULL.
    #[test]
    fn a_subquery_in_a_search_option_filter_is_rejected() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sf (id INT, body VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX sf_ft ON sf (body) USING FULLTEXT;")
            .unwrap();
        e.execute("INSERT INTO sf VALUES (1, 'x');").unwrap();
        e.execute("CREATE TABLE sf2 (id INT);").unwrap();
        let err = e
            .execute(
                "SELECT id FROM sf SEARCH MATCH TEXT ('body', 'x', \
                 'topn=10;filter=EXISTS (SELECT id FROM sf2)');",
            )
            .unwrap_err();
        assert!(
            err.to_string()
                .contains("subqueries are not supported in a search filter"),
            "{err}"
        );
    }

    /// A CTE alias bound by an enclosing `WITH` shadows a base table of the
    /// same name inside a subquery, just as it does in the outer statement.
    /// The CTE exposes `wid`; the base table of the same name exposes `id`.
    #[test]
    fn an_inherited_cte_alias_shadows_the_base_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ivb (id INT);").unwrap();
        e.execute("INSERT INTO ivb VALUES (1);").unwrap();
        assert_eq!(
            rows(
                &e,
                "WITH ivw AS (SELECT id AS wid FROM ivb) \
                 SELECT id FROM ivb WHERE EXISTS (SELECT wid FROM ivw);"
            ),
            vec![vec!["1".to_string()]]
        );
    }

    /// A `WITH` declared inside the subquery itself also shadows a base table.
    #[test]
    fn a_subquery_local_cte_shadows_a_base_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lcte (id INT);").unwrap();
        e.execute("INSERT INTO lcte VALUES (1), (2);").unwrap();
        e.execute("CREATE TABLE lshadow (other INT);").unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM lcte WHERE EXISTS \
                 (WITH lshadow AS (SELECT id FROM lcte WHERE id = 1) \
                 SELECT id FROM lshadow) ORDER BY id;"
            ),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    #[test]
    fn match_tensor_fusion_reranks_by_the_search_tensor() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tf (title VARCHAR, t TENSOR(FLOAT,2), body VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX tf_ft ON tf (body) USING FULLTEXT;")
            .unwrap();
        e.execute("INSERT INTO tf VALUES ('a', [[1,0]], 'off'), ('b', [[0,1]], 'off');")
            .unwrap();
        // The rerank ignores the children's BM25: only the tensor similarity counts.
        let rows = rows(
            &e,
            "SELECT title, ROUND(SCORE(), 4) FROM tf SEARCH MATCH TEXT ('body', 'off', 'topn=4'), \
             FUSION('match_tensor', 'column_name=t;search_tensor=[[10.0,0.0]];\
             tensor_data_type=float;match_method=MaxSim;topn=2');",
        );
        assert_eq!(
            rows,
            vec![
                vec!["a".to_string(), "10.000000".to_string()],
                vec!["b".to_string(), "0.000000".to_string()],
            ]
        );
    }

    #[test]
    fn fulltext_threshold_filter_uses_the_index_score() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ft (num INT, doc VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO ft VALUES (1,'alpha'), (2,'alpha beta'), (3,'gamma delta');")
            .unwrap();
        e.execute("CREATE INDEX ft_idx ON ft (doc) USING FULLTEXT;")
            .unwrap();
        // A threshold is a *score* filter, which only the index can answer.
        let keep = rows(
            &e,
            "SELECT num FROM ft WHERE filter_fulltext('doc','alpha','threshold=0.0001');",
        );
        assert_eq!(keep, vec![vec!["1".to_string()], vec!["2".to_string()]]);
        let none = rows(
            &e,
            "SELECT num FROM ft WHERE filter_fulltext('doc','alpha','threshold=999');",
        );
        assert!(none.is_empty());
    }

    #[test]
    fn a_gram_index_filter_requires_the_query_contiguously() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE g (id INT, doc VARCHAR);").unwrap();
        e.execute(
            "INSERT INTO g VALUES (1,'harmful chemical reaction'), \
             (2,'chemical processes are harmful');",
        )
        .unwrap();
        e.execute("CREATE INDEX g_sg ON g (doc) USING FULLTEXT WITH (analyzer='sparsegram-3-12');")
            .unwrap();
        e.execute("CREATE INDEX g_std ON g (doc) USING FULLTEXT WITH (analyzer='standard');")
            .unwrap();
        // The gram index needs the query contiguous; the regular one accepts the
        // terms apart. Same filter, different index named by `@`.
        let gram = rows(
            &e,
            "SELECT id FROM g WHERE filter_fulltext('doc@g_sg','harmful chemical','operator=and');",
        );
        assert_eq!(gram, vec![vec!["1".to_string()]]);
        let regular = rows(
            &e,
            "SELECT id FROM g WHERE filter_fulltext('doc','harmful chemical','operator=and');",
        );
        assert_eq!(regular.len(), 2);
    }

    /// The index filter answers a term from the column's postings
    /// (`ColumnIndexReader::Lookup` in the reference), not from "every row".
    /// Before the port it returned every position for a term index, so the
    /// filter never narrowed the candidate set.
    #[test]
    fn fulltext_index_filter_reads_postings_by_row_id() {
        use planner::index_scan::FulltextIndex;
        let dir = tempdir().unwrap();
        let mut index = TextIndex::new(
            dir.path().to_string_lossy().into_owned(),
            TextTokenizer::default(),
        );
        index_row(&mut index, 7, "alpha beta");
        index_row(&mut index, 9, "gamma delta");
        // A posting for a row the scan did not materialise must not admit
        // anything: the postings are intersected with the scan's own row ids.
        index_row(&mut index, 42, "alpha stale");
        let index: SharedTextIndex = std::sync::Arc::new(std::sync::RwLock::new(index));
        let rows = vec![
            vec![Value::Str("alpha beta".into())],
            vec![Value::Str("gamma delta".into())],
        ];
        let ids = vec![7usize, 9];
        let tokenizer = TextTokenizer::default();
        let index = (index, tokenizer.clone());
        let provider = RowFulltextProvider {
            rows: &rows,
            column: 0,
            row_ids: Some(&ids),
            index: Some(&index),
            substring: false,
        };
        assert_eq!(provider.postings("alpha"), vec![0]);
        assert_eq!(provider.postings("gamma"), vec![1]);
        assert!(provider.postings("absent").is_empty());
        // An index built with another analyzer cannot answer this filter, so the
        // rows stay and the residual predicate decides.
        let other = TextTokenizer::new(Some(Vec::new()), None);
        let mismatched = (index.0.clone(), other);
        let provider = RowFulltextProvider {
            rows: &rows,
            column: 0,
            row_ids: Some(&ids),
            index: Some(&mismatched),
            substring: false,
        };
        assert_eq!(provider.postings("alpha"), vec![0, 1]);
        // Without an index (or ids) the term cannot be resolved either.
        let blind = RowFulltextProvider {
            rows: &rows,
            column: 0,
            row_ids: None,
            index: None,
            substring: false,
        };
        assert_eq!(blind.postings("alpha"), vec![0, 1]);
    }

    #[test]
    fn rank_features_bonus_sums_listed_tags() {
        let cell = r#"[{"Tag1":1},{"Tag2":2}]"#;
        let tags = [("Tag1".to_string(), 1.0), ("Tag2".to_string(), 2.0)];
        assert_eq!(rank_features_bonus(cell, &tags), 5.0);
        // A tag the document does not carry contributes nothing.
        let tags = [("Tag3".to_string(), 4.0), ("Tag1".to_string(), 1.0)];
        assert_eq!(rank_features_bonus(cell, &tags), 1.0);
        assert_eq!(rank_features_bonus("[]", &tags), 0.0);
    }

    /// A store opened exactly the way `Engine::open` opens its catalog, so a
    /// test can plant or inspect the C++ key space at `<dir>/catalog`.
    fn open_catalog_store(dir: &std::path::Path) -> std::sync::Arc<dyn MetaStore> {
        std::sync::Arc::new(storage::RocksMetaStore::open(dir).unwrap())
    }

    #[test]
    fn flush_mirrors_the_catalog_into_the_cpp_key_space() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        // The C++ keys the C++ engine reads sit beside the native catalog in
        // the same `<dir>/catalog` store.
        let cpp = catalog::layout::read_layout(e.catalog().store().as_ref()).unwrap();
        assert_eq!(cpp.len(), 1, "expected one C++ table record");
        assert_eq!(cpp[0].table_name, "t");
        assert_eq!(cpp[0].db_name, "default_db");
        assert_eq!(
            cpp[0]
                .columns
                .iter()
                .map(|c| c.column_name.clone())
                .collect::<Vec<_>>(),
            vec!["id", "name"]
        );
        assert_eq!(
            catalog::layout::type_text_of(&cpp[0].columns[0].column_type).unwrap(),
            "INTEGER"
        );
    }

    #[test]
    fn opens_a_directory_written_by_the_cpp_engine() {
        // A C++ engine's schema, dropped into an otherwise empty directory's
        // catalog store with no native catalog: opening it must import it.
        let src = tempdir().unwrap();
        let cat = Catalog::open(src.path()).unwrap();
        cat.create_table_typed(
            "default_db.ts",
            &["id".to_string(), "v".to_string()],
            &[None, None],
            &[true, false],
            &["INTEGER".to_string(), "EMBEDDING(FLOAT,3)".to_string()],
            &[None, None],
        )
        .unwrap();

        let dir = tempdir().unwrap();
        {
            let store = open_catalog_store(&dir.path().join("catalog"));
            cat.write_layout(store.as_ref()).unwrap();
        } // release the store lock before the engine opens it

        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            e.table_schema("default_db.ts").unwrap(),
            vec!["id".to_string(), "v".to_string()]
        );
        assert_eq!(e.column_types("default_db.ts").unwrap()[0], "INTEGER");
        // The imported table is usable.
        e.execute("INSERT INTO default_db.ts VALUES (7, [1.0, 2.0, 3.0]);")
            .unwrap();
        match &e.execute("SELECT id FROM default_db.ts;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 1),
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn select_projection_filter_order_limit() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT id, name FROM t WHERE id >= 2 ORDER BY name ASC LIMIT 2;")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns, &vec!["id".to_string(), "name".to_string()]);
                assert_eq!(rows.len(), 2);
                assert_eq!(row_text(rows, 0), vec!["2".to_string(), "a".to_string()]);
                assert_eq!(row_text(rows, 1), vec!["4".to_string(), "a".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn global_aggregates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT COUNT(*), SUM(id), AVG(id), MIN(id), MAX(id) FROM t;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(
                    columns,
                    &[
                        "count(star)",
                        "sum(id)",
                        "(sum(id) / count(id))",
                        "min(id)",
                        "max(id)"
                    ]
                    .iter()
                    .map(|s| s.to_string())
                    .collect::<Vec<_>>()
                );
                assert_eq!(
                    texts(rows),
                    &[["4", "10", "2.500000", "1", "4"]
                        .iter()
                        .map(|s| s.to_string())
                        .collect::<Vec<_>>()]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn group_by_with_order() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        seed(&e);
        match &e
            .execute("SELECT name, COUNT(*) FROM t GROUP BY name ORDER BY name;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(
                    columns,
                    &vec!["name".to_string(), "count(star)".to_string()]
                );
                assert_eq!(
                    texts(rows),
                    vec![
                        vec!["a".to_string(), "2".to_string()],
                        vec!["b".to_string(), "1".to_string()],
                        vec!["c".to_string(), "1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn join_between_two_tables() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE a (aid INT, an VARCHAR);").unwrap();
        e.execute("CREATE TABLE b (bid INT, bn VARCHAR);").unwrap();
        for (i, n) in [(1, "x"), (2, "y")] {
            e.execute(&format!("INSERT INTO a VALUES ({i}, '{n}');"))
                .unwrap();
        }
        for (i, n) in [(2, "yy"), (3, "zz")] {
            e.execute(&format!("INSERT INTO b VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.flush().unwrap();
        match &e
            .execute("SELECT aid, bn FROM a JOIN b ON aid = bid;")
            .unwrap()[0]
        {
            QueryResult::Rows { columns, rows } => {
                assert_eq!(columns, &vec!["aid".to_string(), "bn".to_string()]);
                assert_eq!(texts(rows), vec![vec!["2".to_string(), "yy".to_string()]]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn bounded_capacity_one_does_not_deadlock() {
        let dir = tempdir().unwrap();
        let e = Engine::open_with_capacity(dir.path(), 1).unwrap();
        e.execute("CREATE TABLE t (id INT);").unwrap();
        for i in 0..50 {
            e.execute(&format!("INSERT INTO t VALUES ({i});")).unwrap();
        }
        e.flush().unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 50),
            other => panic!("{other:?}"),
        }
        match &e.execute("SELECT * FROM t LIMIT 3;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn select_literal_and_missing_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            e.execute("SELECT 1, 'x';").unwrap(),
            vec![QueryResult::Rows {
                columns: vec!["c0".into(), "c1".into()],
                rows: vec![vec![Value::Int(1), Value::Str("x".into())]]
            }]
        );
        assert!(e.execute("SELECT * FROM nope;").is_err());
    }

    #[test]
    fn index_scan_matches_expected_and_survives_restart() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for (i, n) in [(1, "a"), (2, "b"), (3, "a"), (4, "b")] {
            e.execute(&format!("INSERT INTO t VALUES ({i}, '{n}');"))
                .unwrap();
        }
        e.execute("CREATE INDEX idx_name ON t (name);").unwrap();
        e.execute("CREATE INDEX idx_id ON t (id);").unwrap();
        match &e.execute("SELECT id FROM t WHERE name = 'a';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    texts(rows),
                    vec![vec!["1".to_string()], vec!["3".to_string()]]
                );
            }
            other => panic!("{other:?}"),
        }
        e.flush().unwrap();
        drop(e); // release the RocksDB lock before reopening
                 // reopen: the index is rebuilt from catalog metadata and still scans
        let e2 = Engine::open(dir.path()).unwrap();
        match &e2.execute("SELECT id FROM t WHERE name = 'b';").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 2),
            other => panic!("{other:?}"),
        }
        // The same after reopen: the integer range reads the chunks that were
        // rebuilt from the catalog, not a memory tail.
        assert_eq!(
            query_column(
                &e2,
                "SELECT id FROM t WHERE id >= 2 AND id <= 3 ORDER BY id;"
            ),
            vec!["2".to_string(), "3".to_string()]
        );
    }

    /// The first column of a one-column query over `t`.
    fn query_column(e: &Engine, sql: &str) -> Vec<String> {
        match &e.execute(sql).unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                rows.iter().map(|row| row[0].to_slt_string()).collect()
            }
            other => panic!("{other:?}"),
        }
    }

    /// The `id` column of a `WHERE`-filtered select over `t`, in `id` order.
    fn ids(e: &Engine, predicate: &str) -> Vec<i64> {
        ids_of(e, "t", predicate)
    }

    /// The `id` column of a `WHERE`-filtered select over `table`, in `id` order.
    fn ids_of(e: &Engine, table: &str, predicate: &str) -> Vec<i64> {
        query_column(
            e,
            &format!("SELECT id FROM {table} WHERE {predicate} ORDER BY id;"),
        )
        .into_iter()
        .map(|id| id.parse().expect("id is an integer"))
        .collect()
    }

    /// `(id, ROW_ID())` pairs, optionally filtered, in `id` order.
    fn id_and_row_id(e: &Engine, predicate: Option<&str>) -> Vec<(i64, i64)> {
        let filter = predicate.map(|p| format!("WHERE {p}")).unwrap_or_default();
        match &e
            .execute(&format!("SELECT id, ROW_ID() FROM t {filter} ORDER BY id;"))
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => rows
                .iter()
                .map(|row| {
                    (
                        row[0].to_slt_string().parse().expect("id is an integer"),
                        row[1]
                            .to_slt_string()
                            .parse()
                            .expect("ROW_ID() is an integer"),
                    )
                })
                .collect(),
            other => panic!("{other:?}"),
        }
    }

    /// A range predicate is answered from the secondary index instead of a
    /// full scan: every interval the planner merged must produce the same rows
    /// as the residual filter, both from the memory tail and from the sealed
    /// chunks.
    #[test]
    fn index_range_scan_answers_from_sealed_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // The same SQL runs twice below; keep the result cache from answering
        // the second round, which is the one that reads all sealed chunks.
        e.execute("SET CONFIG result_cache off;").unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        for i in 0..150 {
            e.execute(&format!("INSERT INTO t VALUES ({i}, 'v{i}');"))
                .unwrap();
        }
        // `CREATE INDEX` builds from the rows that exist, so 0..150 land in a
        // sealed chunk; 150..200 stay in the memory tail until `flush`.
        e.execute("CREATE INDEX idx_id ON t (id);").unwrap();
        for i in 150..200 {
            e.execute(&format!("INSERT INTO t VALUES ({i}, 'v{i}');"))
                .unwrap();
        }
        let cases: [(&str, Vec<i64>); 7] = [
            ("id >= 10 AND id <= 12", vec![10, 11, 12]),
            ("id BETWEEN 5 AND 7", vec![5, 6, 7]),
            ("id > 197", vec![198, 199]),
            ("id < 3", vec![0, 1, 2]),
            ("id <= 0", vec![0]),
            (
                "(id >= 1 AND id <= 2) OR (id >= 198 AND id <= 199)",
                vec![1, 2, 198, 199],
            ),
            ("id >= 1000", vec![]),
        ];
        // Round 0 merges the sealed chunk with the memory tail (the tail is
        // keyed by decimal text, so it is walked and compared numerically);
        // `flush` seals the tail, so round 1 answers purely from the ordered
        // key arrays.
        for round in 0..2 {
            for (predicate, expected) in &cases {
                assert_eq!(&ids(&e, predicate), expected, "round {round}: {predicate}");
            }
            e.flush().unwrap();
        }
        // `=` keeps its own path, and must still agree with the range path.
        assert_eq!(ids(&e, "id = 42"), vec![42]);
        // `ROW_ID()` must ride along with the narrowed rows.
        let all = id_and_row_id(&e, None);
        assert_eq!(
            id_and_row_id(&e, Some("id >= 10 AND id <= 12")),
            all[10..=12].to_vec()
        );
    }

    /// A `FLOAT`/`DOUBLE` chunk keeps the raw value ordered (the reference's
    /// `KeepOrderedSelf`), so a range predicate is answered from the index and
    /// must agree with the same table carrying no index at all. An integer
    /// range keeps working beside them, and a varchar range still falls back
    /// because its keys are hashes.
    #[test]
    fn float_range_scan_matches_the_unindexed_oracle() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("SET CONFIG result_cache off;").unwrap();
        for table in ["t_indexed", "t_plain"] {
            e.execute(&format!(
                "CREATE TABLE {table} (id INT, f FLOAT, d DOUBLE, tag VARCHAR);"
            ))
            .unwrap();
        }
        // 0..60 exist when the indexes are built, so they are sealed into
        // chunks; 60..80 land in the memory tail until the first `flush`.
        for i in 0..60 {
            for table in ["t_indexed", "t_plain"] {
                e.execute(&format!(
                    "INSERT INTO {table} VALUES ({i}, {i}.5, {i}.5, 'tag{i}');"
                ))
                .unwrap();
            }
        }
        for column in ["id", "f", "d", "tag"] {
            e.execute(&format!(
                "CREATE INDEX idx_{column} ON t_indexed ({column});"
            ))
            .unwrap();
        }
        for i in 60..80 {
            for table in ["t_indexed", "t_plain"] {
                e.execute(&format!(
                    "INSERT INTO {table} VALUES ({i}, {i}.5, {i}.5, 'tag{i}');"
                ))
                .unwrap();
            }
        }
        let cases: [(&str, Vec<i64>); 6] = [
            // 1.5 and 3.5 themselves do not satisfy a strict bound.
            ("f > 1.5 AND f < 3.5", vec![2]),
            ("f >= 1.5 AND f <= 3.5", vec![1, 2, 3]),
            ("f BETWEEN 70.5 AND 71.5", vec![70, 71]),
            ("d >= 10.5 AND d <= 12.5", vec![10, 11, 12]),
            // A range that matches nothing must stay empty.
            ("f < 0.5", vec![]),
            ("id > 76", vec![77, 78, 79]),
        ];
        for round in 0..2 {
            for (predicate, expected) in &cases {
                assert_eq!(
                    &ids_of(&e, "t_indexed", predicate),
                    expected,
                    "round {round}: {predicate}"
                );
                // The unindexed twin is the oracle for the same rows.
                assert_eq!(
                    ids_of(&e, "t_indexed", predicate),
                    ids_of(&e, "t_plain", predicate),
                    "round {round}: oracle disagrees for {predicate}"
                );
            }
            // A checkpoint does not dump the memory indexes; one dump
            // transaction per index seals the rows written since the last one.
            for column in ["id", "f", "d", "tag"] {
                e.dump_index_transaction("t_indexed", &format!("idx_{column}"))
                    .unwrap();
            }
        }
        // VARCHAR keys are hashes: the range cannot use the index, but the rows
        // still have to be right (lexicographic, so `tag8`/`tag9` are above
        // `tag78` — the oracle decides, not a hand-written list).
        assert_eq!(
            ids_of(&e, "t_indexed", "tag > 'tag78'"),
            ids_of(&e, "t_plain", "tag > 'tag78'")
        );
        assert_eq!(ids_of(&e, "t_indexed", "tag = 'tag42'"), vec![42]);
        // Equality on a float/double column is the `index_lookup` path.
        assert_eq!(ids_of(&e, "t_indexed", "f = 2.5"), vec![2]);
        assert_eq!(ids_of(&e, "t_indexed", "d = 42.5"), vec![42]);
        // The float and double columns really did seal chunks.
        assert!(!e.secondary_chunk_meta("t_indexed", "f").is_empty());
        assert!(!e.secondary_chunk_meta("t_indexed", "d").is_empty());
    }

    /// A date/time range is answered from the ordered index: `DATE`/`TIME` use
    /// the reference's 4-byte day/second key (`ConvertToOrderedI32`) and
    /// `DATETIME` its 8-byte epoch key (`ConvertToOrderedI64`), so
    /// `WHERE d >= … AND d <= …` narrows instead of scanning. Dates before 1970
    /// are negative keys, which is the case a hash-based key could never order.
    #[test]
    fn temporal_range_scan_matches_the_unindexed_oracle() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("SET CONFIG result_cache off;").unwrap();
        for table in ["t_indexed", "t_plain"] {
            e.execute(&format!(
                "CREATE TABLE {table} (id INT, d DATE, tm TIME, ts DATETIME);"
            ))
            .unwrap();
        }
        // Consecutive days around the epoch; the time of day advances by an
        // hour per row.
        let days = [
            "1969-12-29",
            "1969-12-30",
            "1969-12-31",
            "1970-01-01",
            "1970-01-02",
            "1970-01-03",
            "1970-01-04",
            "1970-01-05",
            "1970-01-06",
            "1970-01-07",
        ];
        let insert = |e: &Engine, from: usize, to: usize| {
            for (i, day) in days.iter().enumerate().take(to).skip(from) {
                let time = format!("{:02}:00:00", i);
                for table in ["t_indexed", "t_plain"] {
                    e.execute(&format!(
                        "INSERT INTO {table} VALUES ({i}, '{day}', '{time}', '{day} {time}');"
                    ))
                    .unwrap();
                }
            }
        };
        // 0..7 are sealed by `CREATE INDEX`; 7..10 land in the memory tail.
        insert(&e, 0, 7);
        for column in ["id", "d", "tm", "ts"] {
            e.execute(&format!(
                "CREATE INDEX idx_{column} ON t_indexed ({column});"
            ))
            .unwrap();
        }
        insert(&e, 7, 10);

        let cases: [(&str, Vec<i64>); 7] = [
            // Pre-epoch dates are negative keys.
            ("d < '1970-01-01'", vec![0, 1, 2]),
            ("d >= '1969-12-30' AND d <= '1970-01-01'", vec![1, 2, 3]),
            ("d = '1970-01-02'", vec![4]),
            // A datetime bound compares the day and the second of the day.
            // Row 5 is 05:00:00 on 01-03, past the upper bound.
            (
                "ts >= '1970-01-01 00:00:00' AND ts <= '1970-01-03 02:00:00'",
                vec![3, 4],
            ),
            ("tm >= '02:00:00'", vec![2, 3, 4, 5, 6, 7, 8, 9]),
            ("d BETWEEN '1970-01-05' AND '1970-01-06'", vec![7, 8]),
            // A strict date bound cannot be pushed down: the planner's rewrite
            // is lexicographic for a string literal, so this one is answered by
            // the residual filter (correct rows, no index narrowing).
            ("d > '1970-01-04'", vec![7, 8, 9]),
        ];
        for round in 0..2 {
            for (predicate, expected) in &cases {
                assert_eq!(
                    &ids_of(&e, "t_indexed", predicate),
                    expected,
                    "round {round}: {predicate}"
                );
                assert_eq!(
                    ids_of(&e, "t_indexed", predicate),
                    ids_of(&e, "t_plain", predicate),
                    "round {round}: oracle disagrees for {predicate}"
                );
            }
            // One dump transaction per index; a checkpoint keeps the tails.
            for column in ["id", "d", "tm", "ts"] {
                e.dump_index_transaction("t_indexed", &format!("idx_{column}"))
                    .unwrap();
            }
        }
        // Equality on a date column rides the same ordered key.
        assert_eq!(ids_of(&e, "t_indexed", "d = '1969-12-31'"), vec![2]);
        assert!(!e.secondary_chunk_meta("t_indexed", "d").is_empty());
        assert!(!e.secondary_chunk_meta("t_indexed", "ts").is_empty());
    }

    /// A range the ordered scan cannot answer — a hashed varchar key, or a
    /// column with no index at all — must still return the right rows through
    /// the fallback. A float range used to be in this list; it is answered from
    /// the index now (`float_range_scan_matches_the_unindexed_oracle`).
    #[test]
    fn range_on_an_unordered_key_falls_back() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("SET CONFIG result_cache off;").unwrap();
        e.execute("CREATE TABLE t (id INT, tag VARCHAR, note INT);")
            .unwrap();
        for i in 0..50 {
            e.execute(&format!("INSERT INTO t VALUES ({i}, 'tag{i}', {i});"))
                .unwrap();
        }
        e.execute("CREATE INDEX idx_tag ON t (tag);").unwrap();
        for round in 0..2 {
            // A varchar chunk stores a hash of the key, so there is no order to
            // scan: the rows must still be right.
            assert_eq!(
                ids(&e, "tag BETWEEN 'tag48' AND 'tag49'"),
                vec![48, 49],
                "round {round}: varchar range"
            );
            // No index on `note`: there is no candidate at all.
            assert_eq!(
                ids(&e, "note >= 45"),
                vec![45, 46, 47, 48, 49],
                "round {round}: unindexed column"
            );
            e.flush().unwrap();
        }
    }

    #[test]
    fn delete_with_where_and_all() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("DELETE FROM t WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 1),
            other => panic!("{other:?}"),
        }
        e.execute("DELETE FROM t;").unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 0),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn update_is_delete_plus_append() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        e.execute("UPDATE t SET name = 'X' WHERE id >= 2;").unwrap();
        match &e.execute("SELECT * FROM t ORDER BY id;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(row_text(rows, 0), vec!["1".to_string(), "a".to_string()]);
                assert_eq!(row_text(rows, 1), vec!["2".to_string(), "X".to_string()]);
                assert_eq!(row_text(rows, 2), vec!["3".to_string(), "X".to_string()]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn multi_row_insert() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 'a'), (2, 'b'), (3, 'c');")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 3),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn persists_across_restart() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            seed(&e);
        }
        let e = Engine::open(dir.path()).unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, .. } => assert_eq!(rows.len(), 4),
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn copy_csv_then_vector_search() {
        let dir = tempdir().unwrap();
        let csv = dir.path().join("vec.csv");
        std::fs::write(
            &csv,
            "2,\"[0.1, 0.2, 0.3, -0.2]\"\n4,\"[0.2, 0.1, 0.3, 0.4]\"\n\
             6,\"[0.3, 0.2, 0.1, 0.4]\"\n8,\"[0.4, 0.3, 0.2, 0.1]\"\n",
        )
        .unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE v (c1 INT, c2 EMBEDDING(FLOAT, 4));")
            .unwrap();
        e.execute(&format!(
            "COPY v FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        assert_eq!(e.row_count("v").unwrap(), 4);

        // cosine top-3 → c1 8, 6, 4 with hidden row-id / similarity columns.
        match &e
            .execute(
                "SELECT c1, ROW_ID(), SIMILARITY() FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(rows.len(), 3);
                assert_eq!(row_text(rows, 0), vec!["8", "3", "0.966755"]);
                assert_eq!(row_text(rows, 2), vec!["4", "1", "0.823532"]);
            }
            other => panic!("{other:?}"),
        }

        // threshold prunes the weakest match.
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'cosine', 3) \
                 WITH (threshold = 0.9);",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => assert_eq!(texts(rows), vec![vec!["8".to_string()]]),
            other => panic!("{other:?}"),
        }

        // WHERE filters candidates before top-N (l2 over {2,4,6} → 6 then 4).
        match &e
            .execute(
                "SELECT c1 FROM v \
                 SEARCH MATCH VECTOR (c2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 2) \
                 WHERE c1 < 7;",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    texts(rows),
                    vec![vec!["6".to_string()], vec!["4".to_string()]]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    /// `InitAbstractIndexT` picks the distance from `metric`; the mirror's
    /// graph answers self-queries for l2, cosine and inner product.
    #[test]
    fn hnsw_metric_variants_answer_self_queries() {
        // Inner product is maximised by the largest-norm row, not the row
        // itself; l2/cosine are maximised by the row.
        for (metric, target) in [("l2", 42), ("cosine", 42), ("ip", 199)] {
            let dir = tempdir().unwrap();
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE hm (id INT, v EMBEDDING(FLOAT,4));")
                .unwrap();
            let mut sql = String::from("INSERT INTO hm VALUES ");
            for i in 0..200 {
                sql.push_str(&format!("({i}, [{i}.0,{}.0,{}.0,1.0])", i + 1, i + 2));
                if i < 199 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute(&format!(
                "CREATE INDEX hm_i ON hm (v) USING HNSW WITH (metric='{metric}');"
            ))
            .unwrap();
            e.flush().unwrap();
            let got = rows(
                &e,
                &format!(
                    "SELECT id FROM hm SEARCH MATCH VECTOR \
                     (v, [{target}.0,{}.0,{}.0,1.0], 'float', '{metric}', 1);",
                    target + 1,
                    target + 2
                ),
            );
            assert_eq!(got, vec![vec![target.to_string()]], "metric {metric}");
        }
    }

    /// A bad HNSW option is a DDL error, not a silent default: `M = 0` builds a
    /// graph with no neighbour slots at all and `metric = 'l1'` would quietly
    /// become `l2`, and both are persisted into the catalog.
    #[test]
    fn hnsw_rejects_invalid_index_options() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE io (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        for (i, opt) in [
            "M=0",
            "M=abc",
            "M=99999",
            "EF_CONSTRUCTION=1",
            "METRIC='l1'",
            "ENCODE='foo'",
        ]
        .iter()
        .enumerate()
        {
            let sql = format!("CREATE INDEX io_{i} ON io (v) USING HNSW WITH ({opt});");
            assert!(e.execute(&sql).is_err(), "{opt} must be rejected");
        }
        // The valid spellings still work.
        e.execute(
            "CREATE INDEX io_ok ON io (v) USING HNSW              WITH (M=8, EF_CONSTRUCTION=100, METRIC='cosine', ENCODE='plain');",
        )
        .unwrap();
    }

    /// One `DELETE` must not send every later KNN query to the exact scan: the
    /// coverage check asks whether every *live* row is labelled, not whether the
    /// counts match (the graph still labels the deleted row).
    #[test]
    fn a_deleted_row_leaves_the_graph_in_charge() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE dv (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        let mut sql = String::from("INSERT INTO dv VALUES ");
        for i in 0..200 {
            sql.push_str(&format!("({i}, [{i}.0,0.0,0.0,0.0])"));
            if i < 199 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.execute("CREATE INDEX dv_i ON dv (v) USING HNSW WITH (metric='l2');")
            .unwrap();
        e.flush().unwrap();
        e.execute("DELETE FROM dv WHERE id = 7;").unwrap();
        // The deleted row is gone and the nearest survivor answers.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM dv SEARCH MATCH VECTOR (v, [7.4,0.0,0.0,0.0], 'float', 'l2', 1);"
            ),
            vec![vec!["8".to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM dv SEARCH MATCH VECTOR (v, [7.0,0.0,0.0,0.0], 'float', 'l2', 3);"
            )
            .into_iter()
            .flatten()
            .collect::<Vec<_>>(),
            vec!["6".to_string(), "8".to_string(), "5".to_string()]
        );
    }

    /// A filtered KNN answered from a secondary index must rank exactly like the
    /// same filter evaluated over decoded columns: the bitmask comes from the
    /// index (`CommonQueryFilter::filter_result_`), the answers must not.
    #[test]
    fn an_index_answered_filter_matches_the_decoded_one() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // `fvi` indexes `tag`; `fvd` does not, so the same predicate is answered
        // from the index in one table and by decoding it in the other.
        for t in ["fvi", "fvd"] {
            e.execute(&format!(
                "CREATE TABLE {t} (id INT, tag INT, v EMBEDDING(FLOAT,4));"
            ))
            .unwrap();
            let mut sql = format!("INSERT INTO {t} VALUES ");
            for i in 0..200 {
                sql.push_str(&format!("({i}, {}, [{i}.0,0.0,0.0,0.0])", i % 7));
                if i < 199 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute(&format!(
                "CREATE INDEX {t}_v ON {t} (v) USING HNSW WITH (metric='l2');"
            ))
            .unwrap();
        }
        e.execute("CREATE INDEX fvi_tag ON fvi (tag);").unwrap();
        e.flush().unwrap();

        for filter in [
            "tag = 3",
            "tag >= 5",
            "tag <= 1",
            "tag > 4",
            "tag < 2",
            "tag >= 2 AND tag <= 3",
            "3 = tag",
        ] {
            let query = |t: &str| {
                format!(
                    "SELECT id FROM {t} SEARCH MATCH VECTOR                      (v, [97.4,0.0,0.0,0.0], 'float', 'l2', 5) WHERE {filter};"
                )
            };
            let indexed = rows(&e, &query("fvi"));
            let decoded = rows(&e, &query("fvd"));
            assert_eq!(indexed, decoded, "filter `{filter}`");
            assert!(!indexed.is_empty(), "filter `{filter}` admits rows");
            // Every answer satisfies the filter.
            let tags: Vec<i64> = indexed
                .iter()
                .map(|r| r[0].parse::<i64>().unwrap() % 7)
                .collect();
            assert_eq!(tags.len(), indexed.len(), "filter `{filter}`");
        }
    }

    /// A filter that admits very few rows still returns a full `topn`: the probe
    /// widens its beam instead of handing back a short answer, and the rows it
    /// finds are the nearest admitted ones.
    #[test]
    fn a_selective_filter_still_returns_topn() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sf (id INT, tag INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        let mut sql = String::from("INSERT INTO sf VALUES ");
        for i in 0..400 {
            sql.push_str(&format!(
                "({i}, {}, [{i}.0,0.0,0.0,0.0])",
                if i % 200 == 7 { 1 } else { 0 }
            ));
            if i < 399 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        // Two admitted rows out of four hundred, both far from the query.
        e.execute("CREATE INDEX sf_i ON sf (v) USING HNSW WITH (metric='l2', M=8);")
            .unwrap();
        e.flush().unwrap();
        let got = rows(
            &e,
            "SELECT id FROM sf SEARCH MATCH VECTOR (v, [399.0,0.0,0.0,0.0], 'float', 'l2', 2)              WHERE tag = 1;",
        );
        assert_eq!(
            got,
            vec![vec!["207".to_string()], vec!["7".to_string()]],
            "both admitted rows come back, nearest first"
        );
    }

    /// A sealed HNSW chunk is **mapped**, not read: its vector payload stays in
    /// the page cache and a loaded segment's heap is its graph, which is the
    /// reference's shape (`PlainVecStoreInner` points into the buffer).
    #[test]
    fn a_sealed_hnsw_chunk_maps_its_vectors() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE mp (id INT, v EMBEDDING(FLOAT,8));")
                .unwrap();
            let mut sql = String::from("INSERT INTO mp VALUES ");
            for i in 0..300 {
                let parts: Vec<String> = (0..8).map(|j| format!("{}", i + j)).collect();
                sql.push_str(&format!("({i}, [{}])", parts.join(",")));
                if i < 299 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute("CREATE INDEX mp_i ON mp (v) USING HNSW WITH (metric='l2');")
                .unwrap();
            e.flush().unwrap();
        }
        let e = Engine::open(dir.path()).unwrap();
        let loaded = e.load_vector_index("mp", "mp_i").unwrap();
        let index = loaded.index.index();
        assert!(
            index.vectors_are_mapped(),
            "the payload must be borrowed from the mapping"
        );
        assert_eq!(
            index.store_mem_bytes(),
            0,
            "a mapped store keeps no vectors"
        );
        assert!(
            index.neighbors_are_mapped(),
            "the neighbour lists must be read out of the mapping too"
        );
        assert_eq!(index.mem_usage(), 0, "a loaded chunk costs no heap");
        assert_eq!(index.len(), 300);
        // The mapped payload is what answers: this engine has no memory segment.
        // Row 42's own vector: [42, 43, …, 49].
        let query: Vec<String> = (0..8).map(|j| format!("{}", 42 + j)).collect();
        assert_eq!(
            rows(
                &e,
                &format!(
                    "SELECT id FROM mp SEARCH MATCH VECTOR (v, [{}], 'float', 'l2', 1);",
                    query.join(",")
                )
            ),
            vec![vec!["42".to_string()]]
        );
    }

    /// A filtered query on an indexed column evaluates its predicate on the
    /// columns it names and pushes the result into the graph as a bitmask
    /// (`CalculateFilterBitmask`): the answer is the nearest *admitted* row, not
    /// the nearest row that then gets discarded.
    #[test]
    fn a_filtered_hnsw_query_answers_only_admitted_rows() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE fv (id INT, tag INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        let mut sql = String::from("INSERT INTO fv VALUES ");
        for i in 0..200 {
            sql.push_str(&format!("({i}, {}, [{i}.0,0.0,0.0,0.0])", i % 2));
            if i < 199 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.execute("CREATE INDEX fv_i ON fv (v) USING HNSW WITH (metric='l2');")
            .unwrap();
        e.flush().unwrap();

        // 49 is the nearest row and is odd; only the even rows are admitted, so
        // the answer is the nearest of those.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM fv SEARCH MATCH VECTOR (v, [49.0,0.0,0.0,0.0], 'float', 'l2', 1) \
                 WHERE tag = 0;"
            ),
            vec![vec!["48".to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM fv SEARCH MATCH VECTOR (v, [49.0,0.0,0.0,0.0], 'float', 'l2', 1);"
            ),
            vec![vec!["49".to_string()]]
        );
        // More results than the graph would return unfiltered: every one of them
        // still satisfies the predicate.
        let got = rows(
            &e,
            "SELECT id, tag FROM fv SEARCH MATCH VECTOR (v, [49.0,0.0,0.0,0.0], 'float', 'l2', 5) \
             WHERE tag = 0;",
        );
        assert_eq!(got.len(), 5);
        assert!(got.iter().all(|r| r[1] == "0"), "{got:?}");
    }

    /// C++ `test_lsg_build_ut.cpp::test1`: an LSG-built HNSW keeps >= 95%
    /// recall@1 for self-queries.
    #[test]
    fn lsg_hnsw_keeps_self_query_recall() {
        let dim = 16usize;
        let n = 128usize;
        let vector = |i: usize| -> String {
            let parts: Vec<String> = (0..dim)
                .map(|j| {
                    let v = ((i * 31 + j * 17 + (i * j) % 13) % 100) as f32 / 10.0;
                    format!("{v}")
                })
                .collect();
            format!("[{}]", parts.join(","))
        };
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lg (id INT, v EMBEDDING(FLOAT,16));")
            .unwrap();
        let mut sql = String::from("INSERT INTO lg VALUES ");
        for i in 0..n {
            sql.push_str(&format!("({i}, {})", vector(i)));
            if i + 1 < n {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.execute(
            "CREATE INDEX lg_i ON lg (v) USING HNSW WITH \
             (metric='l2', build_type='lsg', lsg_config='sample_ratio=0.5,ls_k=10,alpha=1.0');",
        )
        .unwrap();
        e.flush().unwrap();
        let mut hits = 0;
        for i in 0..n {
            let got = rows(
                &e,
                &format!(
                    "SELECT id FROM lg SEARCH MATCH VECTOR (v, {}, 'float', 'l2', 1);",
                    vector(i)
                ),
            );
            if got == vec![vec![i.to_string()]] {
                hits += 1;
            }
        }
        let recall = hits as f64 / n as f64;
        assert!(recall >= 0.95, "LSG recall@1 = {recall}");
    }

    /// C++ `InitAbstractIndexT`: the HNSW mirror stores plain, LVQ or RaBitQ
    /// vectors per the `encode` option and answers the same top-1.
    #[test]
    fn hnsw_encodings_answer_vector_search() {
        for encode in ["plain", "lvq", "rabitq"] {
            let dir = tempdir().unwrap();
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE he (id INT, v EMBEDDING(FLOAT,4));")
                .unwrap();
            let mut sql = String::from("INSERT INTO he VALUES ");
            for i in 0..200 {
                sql.push_str(&format!("({i}, [{i}.0,{}.0,{}.0,1.0])", i + 1, i + 2));
                if i < 199 {
                    sql.push(',');
                }
            }
            sql.push(';');
            e.execute(&sql).unwrap();
            e.execute(&format!(
                "CREATE INDEX he_i ON he (v) USING HNSW WITH (metric='l2', encode='{encode}');"
            ))
            .unwrap();
            e.flush().unwrap();
            let rows = rows(
                &e,
                "SELECT id FROM he SEARCH MATCH VECTOR \
                 (v, [42.0,43.0,44.0,1.0], 'float', 'l2', 1);",
            );
            assert_eq!(rows, vec![vec!["42".to_string()]], "encode {encode}");
        }
    }

    /// C++ `Import` populates the memory indexes: a `COPY` into a table with an
    /// HNSW index feeds the memory segment and honours `mem_index_capacity`,
    /// so the graph covers the imported rows without waiting for an INSERT.
    #[test]
    fn copy_populates_the_hnsw_memory_segment() {
        let dir = tempdir().unwrap();
        let csv = dir.path().join("hnsw.csv");
        std::fs::write(
            &csv,
            "1,\"[0.0, 0.0, 0.0, 0.0]\"\n2,\"[1.0, 0.0, 0.0, 0.0]\"\n\
             3,\"[0.0, 1.0, 0.0, 0.0]\"\n4,\"[0.0, 0.0, 1.0, 0.0]\"\n",
        )
        .unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(3);
        e.execute("CREATE TABLE cvi (c1 INT, c2 EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX cvi_i ON cvi (c2) USING HNSW WITH (metric = l2);")
            .unwrap();
        e.execute(&format!(
            "COPY cvi FROM '{}' WITH (DELIMITER ',', FORMAT CSV);",
            csv.display()
        ))
        .unwrap();
        let indexed: usize = e
            .tables
            .lock()
            .unwrap()
            .values()
            .flat_map(|t| t.hnsw_mem.values())
            .map(|m| m.read().map(|m| m.len()).unwrap_or(0))
            .sum();
        assert_eq!(
            indexed, 0,
            "the 4-row batch reached the capacity and dumped"
        );
        let dumped: usize = e
            .load_vector_indexes("default_db.cvi", "cvi_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(dumped, 4);
    }

    #[test]
    fn sparse_and_tensor_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE s (c1 INT, c2 SPARSE(FLOAT,100));")
            .unwrap();
        e.execute(
            "INSERT INTO s VALUES (1, [0:1.0,10:1.0,20:1.0,80:1.0]), \
             (2, [0:2.0,20:2.0,80:2.0]), (4, [0:4.0,80:4.0]), (5, []);",
        )
        .unwrap();
        match &e
            .execute("SELECT c1 FROM s SEARCH MATCH SPARSE (c2, [0:1.0,20:2.0,80:3.0], 'ip', 3);")
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    texts(rows),
                    vec![
                        vec!["4".to_string()],
                        vec!["2".to_string()],
                        vec!["1".to_string()]
                    ]
                );
            }
            other => panic!("{other:?}"),
        }

        e.execute("CREATE TABLE t (c1 INT, c2 TENSOR(FLOAT,4));")
            .unwrap();
        e.execute(
            "INSERT INTO t VALUES (2, [[0.1,0.2,0.3,-0.2],[0.3,-0.2,0.2,0.3]]), \
             (4, [[0.2,0.1,0.3,0.4]]);",
        )
        .unwrap();
        // MaxSim over the query's two vectors: 2.21 + (-7.4) = -5.19.
        match &e
            .execute(
                "SELECT c1, ROUND(SCORE(), 4) FROM t SEARCH MATCH TENSOR \
                 (c2, [[0.0,-10.0,0.0,0.7],[9.2,45.6,-55.8,3.5]], 'float', 'maxsim', '');",
            )
            .unwrap()[0]
        {
            QueryResult::Rows { rows, .. } => {
                assert_eq!(
                    row_text(rows, 0),
                    vec!["2".to_string(), "-5.190000".to_string()]
                );
                assert_eq!(
                    row_text(rows, 1),
                    vec!["4".to_string(), "-9.660000".to_string()]
                );
            }
            other => panic!("{other:?}"),
        }
    }

    /// `USING PLAID`: the tensor index builds the reference's `PlaidIndex`
    /// once enough embeddings are buffered, answers `MATCH TENSOR` from it, and
    /// `flush()` mirrors it as a C++ PLAID chunk (`PlaidIndex::SaveIndexInner`).
    #[test]
    fn plaid_index_answers_tensor_search_and_mirrors_a_chunk() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pt (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..300 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 7) as f32 + j as f32 * 0.5;
                let y = ((i + j) % 5) as f32 - 1.0;
                let z = ((i * j) % 11) as f32 * 0.25;
                tokens.push(format!("[{x},{y},{z},{}]", (i % 3) as f32));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO pt VALUES {};", values.join(",")))
            .unwrap();
        let query = "SELECT id FROM pt SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');";
        // Without an index the scan is exhaustive.
        let baseline = rows(&e, query);
        assert_eq!(baseline.len(), 3);

        e.execute("CREATE INDEX pidx ON pt (t) USING PLAID WITH (nbits = 4, n_centroids = 8);")
            .unwrap();
        // The scan builds `PlaidIndexInMem` over the 1200 embeddings.
        let indexed = rows(&e, query);
        let tkey = e.table_key("default_db.pt");
        let built = e
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .and_then(|t| t.mv.get("t"))
            .map(|m| m.read().unwrap().plaid.is_some())
            .unwrap_or(false);
        assert!(built, "PLAID index was not built");
        // The residual rerank keeps the brute-force ranking.
        assert_eq!(indexed.len(), 3);
        assert_eq!(indexed[0], baseline[0]);

        e.flush().unwrap();
        let loaded = e.read_plaid_index("default_db.pt", "pidx").unwrap();
        assert_eq!(loaded.doc_num(), 300);
        assert_eq!(loaded.total_embedding_num(), 1200);
    }

    /// Concurrent searches while rows are appended and the tail is dumped: the
    /// readers never deadlock, every answer is a subset of the table, and the
    /// final answer covers every row.
    #[test]
    fn plaid_index_answers_concurrent_searches() {
        use std::sync::{Arc, Mutex};

        let dir = tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        engine
            .execute("CREATE TABLE pcc (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let tensor_rows = |lo: usize, hi: usize| -> String {
            let mut values = Vec::new();
            for i in lo..hi {
                let mut tokens = Vec::new();
                for j in 0..4 {
                    let x = (i % 7) as f32 + j as f32 * 0.5;
                    tokens.push(format!("[{x},0.5,0.25,0.125]"));
                }
                values.push(format!("({i}, [{}])", tokens.join(",")));
            }
            values.join(",")
        };
        engine
            .execute(&format!("INSERT INTO pcc VALUES {};", tensor_rows(0, 200)))
            .unwrap();
        engine
            .execute("CREATE INDEX pcci ON pcc (t) USING PLAID WITH (n_centroids = 8);")
            .unwrap();

        let query = "SELECT id FROM pcc SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=400');";
        let writer = {
            let engine = Arc::clone(&engine);
            std::thread::spawn(move || {
                for step in 0..4 {
                    let lo = 200 + step * 50;
                    engine
                        .execute(&format!(
                            "INSERT INTO pcc VALUES {};",
                            tensor_rows(lo, lo + 50)
                        ))
                        .unwrap();
                }
                engine.flush().unwrap();
            })
        };
        let observed: Arc<Mutex<Vec<i64>>> = Arc::new(Mutex::new(Vec::new()));
        let mut readers = Vec::new();
        for _ in 0..2 {
            let engine = Arc::clone(&engine);
            let observed = Arc::clone(&observed);
            readers.push(std::thread::spawn(move || {
                for _ in 0..15 {
                    let seen: Vec<i64> = rows(&engine, query)
                        .into_iter()
                        .map(|row| row[0].parse().expect("id column"))
                        .collect();
                    observed.lock().unwrap().extend(seen);
                }
            }));
        }
        writer.join().unwrap();
        for reader in readers {
            reader.join().unwrap();
        }

        // No ghost rows: every id a concurrent reader saw exists.
        for id in observed.lock().unwrap().iter() {
            assert!((0..400).contains(id), "reader saw the ghost id {id}");
        }
        // And the final answer covers the whole table.
        let final_ids: std::collections::BTreeSet<i64> = rows(&engine, query)
            .into_iter()
            .map(|row| row[0].parse().expect("id column"))
            .collect();
        let expected: std::collections::BTreeSet<i64> = (0..400).collect();
        assert_eq!(final_ids, expected);
    }

    /// The whole PLAID lifecycle in one run: build, search the corpus, append,
    /// flush, reopen (mmap load), optimize (chunk merge), a `filter=`, a delete
    /// and an update. Every step must return exactly the rows the table holds, so
    /// the index stays coherent across the operations a production table sees.
    fn plaid_lifecycle(colbertsar: bool) {
        let dir = tempdir().unwrap();
        let table = if colbertsar { "pcb" } else { "pld" };
        let index = if colbertsar { "pcbi" } else { "pldi" };
        let ddl = if colbertsar {
            // `nbits` is unused without residuals and is accepted only here.
            format!(
                "CREATE INDEX {index} ON {table} (t) USING PLAID \
                 WITH (centroids = 8, nbits = 8, colbertsar_mode = true);"
            )
        } else {
            format!("CREATE INDEX {index} ON {table} (t) USING PLAID WITH (n_centroids = 8);")
        };
        let query = |topn: usize, filter: Option<&str>| {
            let mut params = format!("topn={topn}");
            if let Some(filter) = filter {
                params.push_str(&format!(";filter={filter}"));
            }
            format!(
                "SELECT id FROM {table} SEARCH MATCH TENSOR \
                 (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', '{params}');"
            )
        };
        let ids = |engine: &Engine, sql: &str| -> std::collections::BTreeSet<i64> {
            rows(engine, sql)
                .into_iter()
                .map(|row| row[0].parse().expect("id column"))
                .collect()
        };
        let tensor_rows = |lo: usize, hi: usize| -> String {
            let mut values = Vec::new();
            for i in lo..hi {
                let mut tokens = Vec::new();
                for j in 0..4 {
                    let x = (i % 7) as f32 + j as f32 * 0.5;
                    let y = ((i + j) % 5) as f32 - 1.0;
                    tokens.push(format!("[{x},{y},0.25,0.125]"));
                }
                values.push(format!("({i}, [{}])", tokens.join(",")));
            }
            values.join(",")
        };

        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute(&format!(
                "CREATE TABLE {table} (id INT, t TENSOR(FLOAT,4));"
            ))
            .unwrap();
        engine
            .execute(&format!(
                "INSERT INTO {table} VALUES {};",
                tensor_rows(0, 200)
            ))
            .unwrap();
        engine.execute(&ddl).unwrap();

        // 1. The index answers for the whole corpus.
        let mut expected: std::collections::BTreeSet<i64> = (0..200).collect();
        assert_eq!(ids(&engine, &query(200, None)), expected);

        // 2. Rows appended after the build are indexed too (incremental add).
        engine
            .execute(&format!(
                "INSERT INTO {table} VALUES {};",
                tensor_rows(200, 260)
            ))
            .unwrap();
        expected.extend(200..260);
        assert_eq!(ids(&engine, &query(260, None)), expected);

        // 3. A flush + reopen loads the flushed chunk (mmap) instead of rebuilding.
        engine.flush().unwrap();
        drop(engine);
        let engine = Engine::open(dir.path()).unwrap();
        assert_eq!(ids(&engine, &query(260, None)), expected);
        {
            let index = engine.multivector_index_for(table, "t", 4).unwrap();
            let guard = index.read().unwrap();
            let plaid = guard.plaid.as_ref().expect("PLAID not restored");
            assert_eq!(plaid.doc_num() as usize, 260);
            assert_eq!(plaid.is_colbertsar_mode(), colbertsar);
            if colbertsar {
                // Residual-free: nothing to map, and nothing on the heap either.
                assert!(plaid.packed_residuals().is_empty());
            } else {
                assert!(
                    plaid.is_packed_mapped(),
                    "the PLAID chunk was copied onto the heap instead of mapped"
                );
                assert!(!plaid.packed_residuals().is_empty());
            }
        }

        // 4. The optimize pass merges the chunks; the answers stay the same.
        engine.optimize_index_chunks().unwrap();
        assert_eq!(ids(&engine, &query(260, None)), expected);

        // 5. A `filter=` is answered from the index bitmask.
        let filtered: std::collections::BTreeSet<i64> = (0..100).collect();
        assert_eq!(ids(&engine, &query(260, Some("id < 100"))), filtered);

        // 6. A tombstoned row is no longer returned.
        engine
            .execute(&format!("DELETE FROM {table} WHERE id = 5;"))
            .unwrap();
        expected.remove(&5);
        assert_eq!(ids(&engine, &query(260, None)), expected);

        // 7. An update rewrites the tensor and the row stays searchable.
        engine
            .execute(&format!(
                "UPDATE {table} SET t = [[1.0,1.0,0.0,0.0]] WHERE id = 6;"
            ))
            .unwrap();
        assert_eq!(ids(&engine, &query(260, None)), expected);

        // 8. A clean shutdown and restart keeps the same answers.
        engine.flush().unwrap();
        drop(engine);
        let engine = Engine::open(dir.path()).unwrap();
        assert_eq!(ids(&engine, &query(260, None)), expected);
    }

    /// PLAID mode, end to end.
    #[test]
    fn plaid_index_lifecycle_is_coherent() {
        plaid_lifecycle(false);
    }

    /// ColBERTSaR mode, end to end: residual-free chunks and the same lifecycle
    /// guarantees.
    #[test]
    fn colbertsar_index_lifecycle_is_coherent() {
        plaid_lifecycle(true);
    }

    /// `filter=` on `SEARCH MATCH TENSOR` narrows the candidates through the
    /// index's bitmask (`CalculateFilterBitmask`) instead of falling back to a
    /// row scan: with the whole corpus requested, the filtered answer is exactly
    /// the unfiltered one restricted to the filter's rows.
    #[test]
    fn match_tensor_filter_narrows_through_the_index() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pf (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..300 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 7) as f32 + j as f32 * 0.5;
                tokens.push(format!("[{x},0.5,0.25,0.125]"));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO pf VALUES {};", values.join(",")))
            .unwrap();
        e.execute("CREATE INDEX pfi ON pf (t) USING PLAID WITH (n_centroids = 8);")
            .unwrap();

        let all = rows(
            &e,
            "SELECT id FROM pf SEARCH MATCH TENSOR \
             (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=300');",
        );
        let filtered = rows(
            &e,
            "SELECT id FROM pf SEARCH MATCH TENSOR \
             (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=300;filter=id < 150');",
        );
        let all_ids: std::collections::HashSet<String> =
            all.into_iter().map(|row| row[0].clone()).collect();
        let expected: std::collections::HashSet<String> = all_ids
            .iter()
            .filter(|id| id.parse::<i64>().unwrap() < 150)
            .cloned()
            .collect();
        let got: std::collections::HashSet<String> =
            filtered.into_iter().map(|row| row[0].clone()).collect();
        assert_eq!(got, expected);
        assert!(
            got.len() >= 100,
            "the filter dropped too many rows: {}",
            got.len()
        );
    }

    /// A `flush` writes the PLAID index out as C++ chunks; the first search
    /// after a reopen must load them instead of retraining over the whole table
    /// (that cost is what made a restart pay O(rows) before answering).
    #[test]
    fn plaid_index_loads_its_flushed_chunks_after_reopen() {
        let dir = tempdir().unwrap();
        let q = "SELECT id FROM pr SEARCH MATCH TENSOR \
                 (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');";
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE pr (id INT, t TENSOR(FLOAT,4));")
                .unwrap();
            let mut values = Vec::new();
            for i in 0..300 {
                let mut tokens = Vec::new();
                for j in 0..4 {
                    let x = (i % 7) as f32 + j as f32 * 0.5;
                    tokens.push(format!("[{x},0.5,0.25,0.125]"));
                }
                values.push(format!("({i}, [{}])", tokens.join(",")));
            }
            e.execute(&format!("INSERT INTO pr VALUES {};", values.join(",")))
                .unwrap();
            e.execute("CREATE INDEX pri ON pr (t) USING PLAID WITH (n_centroids = 8);")
                .unwrap();
            assert_eq!(rows(&e, q).len(), 3);
            e.flush().unwrap();
        }
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(rows(&e, q).len(), 3);
        // A filter can exclude every PLAID candidate; the sealed readers loaded
        // beside PLAID are what keeps such a search answered.
        let filtered = "SELECT id FROM pr SEARCH MATCH TENSOR \
                        (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3;filter=id < 5');";
        let filtered_hits = rows(&e, filtered);
        assert!(
            !filtered_hits.is_empty(),
            "a filtered search lost its fallback"
        );
        assert!(
            filtered_hits
                .iter()
                .all(|row| row[0].parse::<i64>().unwrap_or(99) < 5),
            "the filter was not applied: {filtered_hits:?}"
        );
        let index = e.multivector_index_for("pr", "t", 4).unwrap();
        let guard = index.read().unwrap();
        assert!(
            guard.plaid.is_some(),
            "PLAID was not restored from its flushed chunks"
        );
        assert_eq!(guard.plaid_seqs.len(), 300, "the row ids were not restored");
        assert!(
            guard.plaid.as_ref().is_some_and(|p| p.is_packed_mapped()),
            "the loaded PLAID chunk was copied onto the heap instead of mapped"
        );
        // The sealed readers are loaded beside PLAID on purpose: they are the
        // fallback a filtered search needs when PLAID returns no admissible
        // candidate. Their presence also proves the chunk path ran - a rebuild
        // would have written new ones from the rows.
        assert_eq!(
            guard.sealed.len(),
            1,
            "the flushed chunk was not mapped for the sealed fallback"
        );
        assert!(
            guard
                .sealed
                .iter()
                .all(|(reader, ids, _)| reader.doc_count() == ids.len()),
            "the restored row ids do not line up with the chunk's documents"
        );
        assert!(!guard.plaid_dropped, "PLAID was left dropped");
    }

    /// A NULL tensor adds no PLAID document, but the row still has to count as
    /// *seen*: `is_complete(reserved)` compares the counter with the table's
    /// live rows, so a row it never advances on would make every search rebuild
    /// the whole index, PLAID training included.
    #[test]
    fn a_null_tensor_row_keeps_the_multivector_index_complete() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mn (id INT, m MULTIVECTOR(FLOAT,4));")
            .unwrap();
        let values: Vec<String> = (0..300)
            .map(|i| format!("({i}, [[1.0,0.5,0.25,0.125]])"))
            .collect();
        e.execute(&format!("INSERT INTO mn VALUES {};", values.join(",")))
            .unwrap();
        e.execute("CREATE INDEX mni ON mn (m) USING PLAID WITH (n_centroids = 8);")
            .unwrap();
        let first = e.multivector_index_for("mn", "m", 4).unwrap();
        e.execute("INSERT INTO mn VALUES (300, NULL);").unwrap();
        let after = e.multivector_index_for("mn", "m", 4).unwrap();
        assert!(
            std::sync::Arc::ptr_eq(&first, &after),
            "the index was rebuilt although the NULL row was counted as seen"
        );
    }

    /// The four `plaid_*` keys reach `plaid_search_options`; the statement's
    /// option validator runs first and used to reject them as unknown.
    #[test]
    fn plaid_search_options_are_accepted_by_the_validator() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pv (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let values: Vec<String> = (0..300)
            .map(|i| format!("({i}, [[{},0.5,0.25,0.125]])", (i % 7) as f32))
            .collect();
        e.execute(&format!("INSERT INTO pv VALUES {};", values.join(",")))
            .unwrap();
        e.execute("CREATE INDEX pvi ON pv (t) USING PLAID WITH (n_centroids = 8);")
            .unwrap();
        let query = "SELECT id FROM pv SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', \
                      'topn=3;plaid_n_ivf_probe=32;plaid_centroid_score_threshold=-0.5;\
                      plaid_n_doc_to_score=300;plaid_n_full_scores=30');";
        assert_eq!(rows(&e, query).len(), 3);
    }

    /// A chunk written after a delete covers only the live rows. The restored
    /// row list has to be that same live population, or the document count
    /// never matches `doc_num()` and every reopen retrains the index.
    #[test]
    fn a_chunk_written_after_a_delete_reopens_without_retraining() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE md (id INT, t TENSOR(FLOAT,4));")
                .unwrap();
            let values: Vec<String> = (0..300)
                .map(|i| format!("({i}, [[{},0.5,0.25,0.125]])", (i % 7) as f32))
                .collect();
            e.execute(&format!("INSERT INTO md VALUES {};", values.join(",")))
                .unwrap();
            e.execute("CREATE INDEX mdi ON md (t) USING PLAID WITH (n_centroids = 8);")
                .unwrap();
            e.flush().unwrap();
            e.execute("DELETE FROM md WHERE id = 0;").unwrap();
            // A delete drops the in-memory PLAID index, and the next search
            // rebuilds it; the following flush then rewrites the chunk from the
            // live rows (`plaid_chunk` -> `column_docs`), which is the chunk
            // this test reopens.
            e.execute(
                "SELECT id FROM md SEARCH MATCH TENSOR \
                 (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');",
            )
            .unwrap();
            e.flush().unwrap();
        }
        let e = Engine::open(dir.path()).unwrap();
        let hits = rows(
            &e,
            "SELECT id FROM md SEARCH MATCH TENSOR \
             (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');",
        );
        assert!(
            !hits.iter().any(|row| row[0] == "0"),
            "the deleted row was scored after the reopen"
        );
        let index = e.multivector_index_for("md", "t", 4).unwrap();
        let guard = index.read().unwrap();
        assert!(guard.plaid.is_some(), "PLAID was not restored");
        // The restored id lists have to line up with the chunks that carry
        // them; a tombstoned row in the list would shift every later id.
        assert!(
            guard
                .sealed
                .iter()
                .all(|(reader, ids, _)| reader.doc_count() == ids.len()),
            "the sealed fallback was loaded with a stale row list"
        );
        assert_eq!(
            guard.plaid_seqs.len(),
            guard
                .plaid
                .as_ref()
                .map(|p| p.doc_num() as usize)
                .unwrap_or(0),
            "the restored document ids do not cover the loaded index"
        );
    }

    /// `IndexOptions`' PLAID knobs: the reference's defaults
    /// (`default_values.cppm:154-157`) and the per-query overrides
    /// (`logical_match_tensor_scan_impl.cpp:116-152`), where a value the
    /// reference would reject keeps the default.
    #[test]
    fn plaid_search_options_follow_the_reference_defaults_and_overrides() {
        let defaults = crate::engine::state::plaid_search_options(|_| None, 5);
        assert_eq!(defaults.n_ivf_probe, 8);
        assert!((defaults.centroid_score_threshold + 0.1).abs() < 1e-6);
        assert_eq!(defaults.n_doc_to_score, 500);
        assert_eq!(defaults.n_full_scores, 50);

        let custom = crate::engine::state::plaid_search_options(
            |k| match k {
                "plaid_n_ivf_probe" => Some("32".to_string()),
                "plaid_n_doc_to_score" => Some("40".to_string()),
                "plaid_n_full_scores" => Some("20".to_string()),
                _ => None,
            },
            5,
        );
        assert_eq!(custom.n_ivf_probe, 32);
        assert_eq!(custom.n_doc_to_score, 40);
        assert_eq!(custom.n_full_scores, 20);

        // `n_doc_to_score < topn` and `n_full_scores > n_doc_to_score` are the
        // reference's rejections, so the defaults survive.
        let rejected = crate::engine::state::plaid_search_options(
            |k| match k {
                "plaid_n_doc_to_score" => Some("2".to_string()),
                "plaid_n_full_scores" => Some("600".to_string()),
                _ => None,
            },
            5,
        );
        assert_eq!(rejected.n_doc_to_score, 500);
        assert_eq!(rejected.n_full_scores, 50);
    }

    /// A row whose tensor is NULL must not make the multi-vector index look
    /// incomplete: `doc_count` counts parsed documents, so comparing it with the
    /// table's live rows rebuilt the whole index (PLAID training included) on
    /// every search. Rewriting a row must leave PLAID covering every document
    /// instead of dropping it for good.
    #[test]
    fn plaid_index_rebuilds_only_when_the_rows_change() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE pm (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..320 {
            if i % 10 == 0 {
                values.push(format!("({i}, NULL)"));
                continue;
            }
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 7) as f32 + j as f32 * 0.5;
                tokens.push(format!("[{x},0.5,0.25,0.125]"));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO pm VALUES {};", values.join(",")))
            .unwrap();
        e.execute("CREATE INDEX pmi ON pm (t) USING PLAID WITH (n_centroids = 8);")
            .unwrap();

        let q = "SELECT id FROM pm SEARCH MATCH TENSOR \
                 (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');";
        assert_eq!(rows(&e, q).len(), 3);
        let first = e.multivector_index_for("pm", "t", 4).unwrap();
        assert!(
            first.read().unwrap().plaid.is_some(),
            "PLAID was not built over 320 rows"
        );
        // A second search must reuse the index; a NULL row used to make it look
        // incomplete and rebuild it every time.
        assert_eq!(rows(&e, q).len(), 3);
        let second = e.multivector_index_for("pm", "t", 4).unwrap();
        assert!(
            Arc::ptr_eq(&first, &second),
            "the second search rebuilt the whole multi-vector index"
        );

        // An update appends a new row id (the old one is tombstoned), so PLAID
        // either grows by one or is rebuilt - it must cover every document
        // afterwards either way.
        e.execute("UPDATE pm SET t = [[1.0,1.0,0.0,0.0]] WHERE id = 1;")
            .unwrap();
        assert!(!rows(&e, q).is_empty());
        let after = e.multivector_index_for("pm", "t", 4).unwrap();
        {
            let guard = after.read().unwrap();
            let covered = guard.plaid.as_ref().map(|p| p.doc_num() as usize);
            assert_eq!(
                covered,
                Some(guard.doc_count()),
                "PLAID does not cover every document after the update"
            );
            assert!(!guard.plaid_dropped, "PLAID was left dropped");
        }
    }

    /// `USING PLAID … WITH (colbertsar_mode = true)` is the reference's
    /// ColBERTSaR path (`d3ee9f6`, #3379): query-aware centroids
    /// (`PlaidIndex::TrainQueryAwareCentroids`) and **no residual storage**, so
    /// the chunk holds no quantizer and no packed residual. The mode travels in
    /// the chunk, so a reload restores it.
    #[test]
    fn colbertsar_plaid_index_trains_query_aware_and_stores_no_residual() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE cs (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..300 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 7) as f32 + j as f32 * 0.5;
                let y = ((i + j) % 5) as f32 - 1.0;
                let z = ((i * j) % 11) as f32 * 0.25;
                tokens.push(format!("[{x},{y},{z},{}]", (i % 3) as f32));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO cs VALUES {};", values.join(",")))
            .unwrap();

        // `nbits` is unused without residuals, so `IndexPLAID::Make` skips its
        // check in this mode; `centroids` is the SQL spelling of
        // `n_centroids` and has to reach the index.
        e.execute(
            "CREATE INDEX csi ON cs (t) USING PLAID \
             WITH (centroids = 8, nbits = 8, colbertsar_mode = true);",
        )
        .unwrap();
        let options = e.plaid_options("default_db.cs", "csi");
        assert!(options.colbertsar, "colbertsar_mode was not picked up");
        assert_eq!(options.n_centroids, 8, "`centroids` was ignored");

        let query = "SELECT id FROM cs SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.0,0.0]], 'float', 'maxsim', 'topn=3');";
        assert_eq!(rows(&e, query).len(), 3);

        e.flush().unwrap();
        let chunks = e.read_plaid_index_chunks("cs", "csi").unwrap();
        assert_eq!(chunks.len(), 1);
        assert!(chunks[0].is_colbertsar_mode(), "chunk lost the mode");
        assert!(
            chunks[0].packed_residuals().is_empty(),
            "ColBERTSaR must store no residual"
        );
        assert_eq!(chunks[0].doc_num(), 300);
        // Reload round-trips the mode and stays residual-free.
        let loaded = e.read_plaid_index("cs", "csi").unwrap();
        assert!(loaded.is_colbertsar_mode());
        assert!(loaded.packed_residuals().is_empty());

        // A flag value outside `true/false/1/0` is rejected, as in `IndexPLAID::Make`.
        assert!(e
            .execute("CREATE INDEX bad ON cs (t) USING PLAID WITH (colbertsar_mode = maybe);")
            .is_err());
        // Without the mode `nbits` is validated as before.
        assert!(e
            .execute("CREATE INDEX bad2 ON cs (t) USING PLAID WITH (nbits = 8);")
            .is_err());
    }

    /// `USING SMVE`: the tensor scan builds `SMVEIndexInMem` (projection +
    /// BMP) and re-ranks its candidates with exact MaxSim.
    #[test]
    fn smve_index_answers_tensor_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE st (id INT, t TENSOR(FLOAT,4));")
            .unwrap();
        let mut values = Vec::new();
        for i in 0..40 {
            let mut tokens = Vec::new();
            for j in 0..4 {
                let x = (i % 5) as f32 + j as f32 * 0.25;
                let y = ((i + j) % 7) as f32 * 0.5 - 1.0;
                tokens.push(format!("[{x},{y},0.5,{}]", (i % 3) as f32));
            }
            values.push(format!("({i}, [{}])", tokens.join(",")));
        }
        e.execute(&format!("INSERT INTO st VALUES {};", values.join(",")))
            .unwrap();
        let query = "SELECT id FROM st SEARCH MATCH TENSOR \
                     (t, [[1.0,0.0,0.5,0.0]], 'float', 'maxsim', 'topn=3');";
        let baseline = rows(&e, query);
        assert_eq!(baseline.len(), 3);

        e.execute("CREATE INDEX sidx ON st (t) USING SMVE WITH (width = 64, topk = 8);")
            .unwrap();
        let indexed = rows(&e, query);
        eprintln!(
            "DEBUG indexes: {:?}",
            e.storage
                .catalog
                .list_indexes()
                .unwrap()
                .iter()
                .map(|i| (
                    i.name.clone(),
                    i.index_type.clone(),
                    i.column.clone(),
                    i.options.clone()
                ))
                .collect::<Vec<_>>()
        );
        let tkey = e.table_key("default_db.st");
        let built = e
            .tables
            .lock()
            .unwrap()
            .get(&tkey)
            .and_then(|t| t.mv.get("t"))
            .map(|m| m.read().unwrap().smve.is_some())
            .unwrap_or(false);
        assert!(built, "SMVE index was not built");
        assert_eq!(indexed[0], baseline[0]);
    }

    /// The multi-vector index is built once (first use), appended at write time,
    /// and dumped by `flush()`; queries never rebuild it.
    #[test]
    fn multivector_index_is_built_at_write_time() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mv (id INT, v MULTIVECTOR(FLOAT,2));")
            .unwrap();
        e.execute("INSERT INTO mv VALUES (1, [[1.0,0.0],[0.0,1.0]]), (2, [[0.2,0.2]]);")
            .unwrap();
        let q = "SELECT id FROM mv SEARCH MATCH TENSOR \
                 (v, [1.0,0.0], 'float', 'maxsim', 'topn=3');";
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        let idx_dir = std::fs::read_dir(dir.path().join("multivector"))
            .unwrap()
            .next()
            .unwrap()
            .unwrap()
            .path();
        let count = |ext: &str| {
            std::fs::read_dir(&idx_dir)
                .unwrap()
                .filter_map(|x| x.ok())
                .filter(|x| x.path().extension().is_some_and(|y| y == ext))
                .count()
        };
        // One chunk of raw tokens plus its EMVB chunk, written by the build.
        assert_eq!(count("mvec"), 1);
        assert_eq!(count("emvb"), 1);

        // A later insert goes to the memory tail and ranks immediately.
        e.execute("INSERT INTO mv VALUES (3, [[1.0,0.0]]);")
            .unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["3".to_string()],
                vec!["2".to_string()]
            ]
        );

        // A dump transaction seals the tail as a second chunk; the query still
        // agrees. A checkpoint keeps the tail in memory.
        e.dump_all_memory_indexes();
        assert_eq!(count("mvec"), 2);
        assert_eq!(count("emvb"), 2);
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["3".to_string()],
                vec!["2".to_string()]
            ]
        );

        // A tombstoned document is filtered out of the index hits.
        e.execute("DELETE FROM mv WHERE id = 3;").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
    }

    /// A `MULTIVECTOR` column's HNSW graph is one vertex per token, and it stays
    /// that way across the restart boundary. The query path materialises the
    /// memory segment from the catalog before any write touches it
    /// (`hnsw_mem_segments`), so whichever path creates the segment decides
    /// whether multi-token cells are indexed at all
    /// (`MemIndexInserterIter1<MultiVectorRef>`).
    ///
    /// The answers alone cannot prove the graph holds the tokens — the coverage
    /// check folds unlabelled rows back in (`include_uncovered`) — so the token
    /// accounting is pinned by `hnsw_mem::tests` instead.
    #[test]
    fn multivector_hnsw_indexes_every_token_after_a_restart() {
        let dir = tempdir().unwrap();
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE mvt (id INT, v MULTIVECTOR(FLOAT,2));")
                .unwrap();
            e.execute("INSERT INTO mvt VALUES (1, [[0.0,1.0],[1.0,0.0]]), (2, [[0.0,0.5]]);")
                .unwrap();
            e.execute("CREATE INDEX mvt_i ON mvt (v) USING HNSW WITH (metric='l2');")
                .unwrap();
            e.flush().unwrap();
        }
        let e = Engine::open(dir.path()).unwrap();
        // The first query is what creates the reopened engine's memory segment.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM mvt SEARCH MATCH VECTOR (v, [0.0,1.0], 'float', 'l2', 1);"
            ),
            vec![vec!["1".to_string()]]
        );
        // A cell with two tokens: the single-vector path would drop it whole.
        e.execute("INSERT INTO mvt VALUES (3, [[0.3,0.7],[0.9,0.9]]);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM mvt SEARCH MATCH VECTOR (v, [0.3,0.7], 'float', 'l2', 1);"
            ),
            vec![vec!["3".to_string()]]
        );
    }

    /// The sparse BMP index is built once, appended at write time, and dumped
    /// by `flush()`; queries read the chunk files instead of rebuilding.
    #[test]
    fn sparse_index_is_built_at_write_time() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sp (id INT, v SPARSE(FLOAT,100));")
            .unwrap();
        e.execute("INSERT INTO sp VALUES (1, [0:1.0,10:1.0]), (2, [0:2.0,20:2.0]);")
            .unwrap();
        let q = "SELECT id FROM sp SEARCH MATCH SPARSE \
                 (v, [0:1.0,20:2.0], 'ip', 3);";
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
        let idx_dir = std::fs::read_dir(dir.path().join("sparse"))
            .unwrap()
            .next()
            .unwrap()
            .unwrap()
            .path();
        let count = |ext: &str| {
            std::fs::read_dir(&idx_dir)
                .unwrap()
                .filter_map(|x| x.ok())
                .filter(|x| x.path().extension().is_some_and(|y| y == ext))
                .count()
        };
        assert_eq!(count("bmp"), 1);

        // The write-time tail is visible before the dump.
        e.execute("INSERT INTO sp VALUES (3, [0:4.0,20:4.0]);")
            .unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );

        // A dump transaction seals the tail as a second chunk and the ranking
        // holds.
        e.dump_all_memory_indexes();
        assert_eq!(count("bmp"), 2);
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["3".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );

        // Tombstoned rows never appear.
        e.execute("DELETE FROM sp WHERE id = 3;").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
    }

    /// `SPARSE … WITH (SORTED)` is carried through the catalog, survives a
    /// restart, and the BMP index records it. The BMP chunk layout is the same
    /// for both hints, so top-k is unchanged.
    #[test]
    fn sparse_sorted_hint_is_persisted_and_ranks() {
        let dir = tempdir().unwrap();
        let q = "SELECT id FROM ss SEARCH MATCH SPARSE (v, [0:1.0,20:2.0], 'ip', 3);";
        let first;
        {
            let e = Engine::open(dir.path()).unwrap();
            e.execute("CREATE TABLE ss (id INT, v SPARSE(FLOAT,30000) WITH (SORTED));")
                .unwrap();
            e.execute("CREATE TABLE so (id INT, v SPARSE(FLOAT,30000) WITH (sort));")
                .unwrap();

            let meta = e
                .storage
                .catalog
                .get_table_by_name("default_db.ss")
                .unwrap()
                .unwrap();
            // `DataType::ToString` renders a sparse column as
            // `SPARSE(<element>,<index>,<dimension>)` in the reference's
            // lowercase element names (`SparseInfo::ToString`), which is what a
            // `SHOW TABLE … COLUMNS` prints on either engine.
            assert_eq!(meta.types[1], "SPARSE(float,int16,30000)");
            assert_eq!(meta.sparse_stores[1], Some(CatalogSparseStore::Sorted));
            let meta = e
                .storage
                .catalog
                .get_table_by_name("default_db.so")
                .unwrap()
                .unwrap();
            assert_eq!(meta.sparse_stores[1], Some(CatalogSparseStore::Sort));

            e.execute("CREATE INDEX ss_i ON ss (v) USING BMP;").unwrap();
            e.execute("INSERT INTO ss VALUES (1, [0:1.0,10:1.0]), (2, [0:2.0,20:2.0]);")
                .unwrap();
            first = rows(&e, q);
            assert_eq!(first, vec![vec!["2".to_string()], vec!["1".to_string()]]);
            let recorded = e
                .tables
                .lock()
                .unwrap()
                .values()
                .find_map(|t| t.sparse.get("v").cloned())
                .unwrap();
            assert_eq!(recorded.read().unwrap().store, CatalogSparseStore::Sorted);

            // Sealing the tail rewrites the same-format chunks; the ranking
            // and the recorded hint both hold.
            e.flush().unwrap();
            assert_eq!(rows(&e, q), first);
            assert_eq!(recorded.read().unwrap().store, CatalogSparseStore::Sorted);
        }

        // Reopen: the hint comes back from the catalog and the rebuilt index
        // records it again.
        let e = Engine::open(dir.path()).unwrap();
        let meta = e
            .storage
            .catalog
            .get_table_by_name("default_db.ss")
            .unwrap()
            .unwrap();
        assert_eq!(meta.types[1], "SPARSE(float,int16,30000)");
        assert_eq!(meta.sparse_stores[1], Some(CatalogSparseStore::Sorted));
        assert_eq!(rows(&e, q), first);
        let recorded = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.sparse.get("v").cloned())
            .unwrap();
        assert_eq!(recorded.read().unwrap().store, CatalogSparseStore::Sorted);
    }

    /// A secondary index keeps only post-dump rows in memory; a dump
    /// transaction seals the tail into a chunk and lookups merge chunks with
    /// the tail. A checkpoint does not seal it.
    #[test]
    fn secondary_index_seals_its_memory_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sec (id INT, v INT);").unwrap();
        e.execute("INSERT INTO sec VALUES (1, 10), (2, 20);")
            .unwrap();
        e.execute("CREATE INDEX idx_v ON sec (v);").unwrap();
        let q = "SELECT id FROM sec WHERE v = 20;";
        assert_eq!(rows(&e, q), vec![vec!["2".to_string()]]);

        // New rows land in the memory tail and are visible immediately.
        e.execute("INSERT INTO sec VALUES (3, 20);").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert!(idx.key_count() > 0);
        drop(tables);

        // The checkpoint keeps the tail...
        e.flush().unwrap();
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert!(idx.key_count() > 0);
        drop(tables);

        // ...and the dump drains it; the answer comes from the chunk + new tail.
        e.dump_index_transaction("sec", "idx_v").unwrap();
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(idx.key_count(), 0);
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT id FROM sec WHERE v = 10;"),
            vec![vec!["1".to_string()]]
        );

        // Tombstoned rows are filtered from both forms.
        e.execute("DELETE FROM sec WHERE id = 2;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// Boolean columns use the low-cardinality (roaring) chunks with the same
    /// tail/dump lifecycle.
    #[test]
    fn low_card_secondary_index_seals_its_memory_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE b (id INT, flag BOOL);").unwrap();
        e.execute("INSERT INTO b VALUES (1, true), (2, false);")
            .unwrap();
        e.execute("CREATE INDEX idx_flag ON b (flag);").unwrap();
        let q = "SELECT id FROM b WHERE flag = true;";
        assert_eq!(rows(&e, q), vec![vec!["1".to_string()]]);
        e.execute("INSERT INTO b VALUES (3, true);").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        e.flush().unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        e.execute("DELETE FROM b WHERE id = 1;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// A `json` column is always a low-cardinality chunk (`index_secondary.cppm:56`
    /// forces it there) keyed by the flattener's `path:type:value` terms, so the
    /// planner's `json_*` filters read a term chunk rather than a hashed key.
    #[test]
    fn json_secondary_index_seals_term_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE j (id INT, meta JSON);").unwrap();
        e.execute(r#"INSERT INTO j VALUES (1, '{"a":1,"b":"x"}'), (2, '{"a":2}');"#)
            .unwrap();
        e.execute("CREATE INDEX idx_meta ON j (meta);").unwrap();
        let by_a = "SELECT id FROM j WHERE json_extract_int(meta, '$.a') = 1;";
        let has_b = "SELECT id FROM j WHERE json_exists_path(meta, '$.b');";
        assert_eq!(rows(&e, by_a), vec![vec!["1".to_string()]]);
        assert_eq!(rows(&e, has_b), vec![vec!["1".to_string()]]);

        // The dump turns the tail into one low-cardinality chunk per segment: the
        // document's scalar, path and array terms, not a single hashed document.
        e.dump_index_transaction("j", "idx_meta").unwrap();
        {
            let tables = e.tables.lock().unwrap();
            let segments = tables
                .values()
                .find_map(|t| t.secondary.low.get("meta"))
                .unwrap();
            assert_eq!(segments.len(), 1);
            assert!(segments[0].1.len() > 1);
            assert_eq!(
                segments[0].1.kind(),
                storage::secondary_segment::KeyKind::Bytes
            );
        }
        // Both filters still answer from the chunk plus the (now empty) tail.
        assert_eq!(rows(&e, by_a), vec![vec!["1".to_string()]]);
        assert_eq!(rows(&e, has_b), vec![vec!["1".to_string()]]);

        // A new row lands in the tail and is visible through the same filters.
        e.execute(r#"INSERT INTO j VALUES (3, '{"a":1,"b":"y"}');"#)
            .unwrap();
        assert_eq!(
            rows(&e, by_a),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(
            rows(&e, has_b),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
    }

    /// A `json` document nesting past the flattener's depth bound cannot be
    /// expanded into terms, but it is valid JSON that may still match a
    /// predicate. The index therefore keeps it as a candidate instead of
    /// reporting "no terms" and dropping the row before the residual filter
    /// above the index scan runs.
    ///
    /// The marker is an ordinary term-chunk key, so the row survives the dump
    /// into a chunk and a cold restart alike.
    #[test]
    fn over_depth_json_rows_stay_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE jd (id INT, meta JSON);").unwrap();
        // Row 2 nests past `MAX_JSON_DEPTH` (512) and is still valid JSON whose
        // `$.a` is 1 - exactly the row a term-range read would lose.
        let deep = format!("{{\"a\":1,\"d\":{}1{}}}", "[".repeat(513), "]".repeat(513));
        e.execute(&format!(
            "INSERT INTO jd VALUES (1, '{{\"a\":1}}'), (2, '{deep}');"
        ))
        .unwrap();
        e.execute("CREATE INDEX idx_jd ON jd (meta);").unwrap();
        let q = "SELECT id FROM jd WHERE json_extract_int(meta, '$.a') = 1;";
        let both = vec![vec!["1".to_string()], vec!["2".to_string()]];
        assert_eq!(rows(&e, q), both);

        // The dump rewrites the tail's terms into a low-cardinality chunk. The
        // marker is a key like any other, so the row is still found.
        e.dump_index_transaction("jd", "idx_jd").unwrap();
        assert_eq!(rows(&e, q), both);

        // Widening the candidate set must not become a false positive: the
        // residual filter above the index scan still answers the predicate.
        assert!(rows(
            &e,
            "SELECT id FROM jd WHERE json_extract_int(meta, '$.a') = 999;"
        )
        .is_empty());
        assert!(rows(
            &e,
            "SELECT id FROM jd WHERE json_exists_path(meta, '$.zzz');"
        )
        .is_empty());

        // The chunk also carries the marker on disk, which is what a cold
        // restart - or a rebuild from the rows - reads back.
        {
            let tables = e.tables.lock().unwrap();
            let low = tables
                .values()
                .find_map(|t| t.secondary.low.get("meta"))
                .expect("a json column uses term chunks");
            assert!(
                low.iter().any(|(_, reader)| reader
                    .range(
                        storage::secondary_lowcard::LowCardKey::Bytes(
                            storage::json_index::OVERFLOW_TERM.as_bytes().to_vec()
                        ),
                        storage::secondary_lowcard::LowCardKey::Bytes(
                            storage::json_index::OVERFLOW_TERM.as_bytes().to_vec()
                        ),
                    )
                    .len()
                    == 1),
                "the over-depth row's candidate term must be written to the chunk"
            );
        }

        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(rows(&e, q), both);
    }

    /// `WITH (cardinality = low)` moves any column onto the bitmap layout, not
    /// just BOOL and JSON (`NewSecondaryIndexInMem` honours the option for every
    /// type).
    #[test]
    fn low_cardinality_option_uses_bitmap_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lc (id INT, v INT);").unwrap();
        e.execute("CREATE INDEX lcv ON lc (v) WITH (cardinality = low);")
            .unwrap();
        e.execute("INSERT INTO lc VALUES (1, 10), (2, 20), (3, 10);")
            .unwrap();
        let q = "SELECT id FROM lc WHERE v = 10;";
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        e.dump_index_transaction("lc", "lcv").unwrap();
        {
            let tables = e.tables.lock().unwrap();
            let low = tables
                .values()
                .find_map(|t| t.secondary.low.get("v"))
                .expect("an explicit low-cardinality column uses bitmap chunks");
            assert_eq!(low.len(), 1);
            let high = tables.values().find_map(|t| t.secondary.disk.get("v"));
            assert!(high.is_none(), "no PGM chunk for a low-cardinality column");
        }
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
    }

    /// Concurrent inserts, dumps and index reads on one secondary index must
    /// agree with the rows: no ghost row, and no row missing from the final
    /// index.
    ///
    /// Three races are exercised at once:
    /// * `SecondaryIndex::fold` republishing the tail while a reader is between
    ///   its two views (fixed by taking `pending` before the snapshot);
    /// * a dump emptying the tail before its chunk is registered (fixed by the
    ///   frozen `sealing` tails the readers merge);
    /// * many writers appending to one tail concurrently.
    #[test]
    fn secondary_index_is_correct_under_concurrent_writes_and_dumps() {
        use std::collections::HashSet;
        use std::sync::{Arc, Mutex};

        const WRITERS: usize = 4;
        const ROWS_PER_WRITER: usize = 1500;
        const READERS: usize = 2;
        const READS: usize = 400;

        let dir = tempdir().unwrap();
        let e = Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE c (id INT, v INT);").unwrap();
        e.execute("CREATE INDEX cv ON c (v);").unwrap();

        // Every row gets its own value, so the tail folds its pending map
        // several times while the readers probe it.
        let mut writers = Vec::new();
        for w in 0..WRITERS {
            let e = Arc::clone(&e);
            writers.push(std::thread::spawn(move || {
                let rows: Vec<Vec<Literal>> = (0..ROWS_PER_WRITER)
                    .map(|i| {
                        let id = (w * ROWS_PER_WRITER + i) as i64;
                        vec![Literal::Int(id), Literal::Int(id)]
                    })
                    .collect();
                e.insert_literals("default_db.c", None, rows).unwrap();
            }));
        }
        // A dump loop interleaves with the writers: each dump freezes a tail and
        // registers a chunk, which is exactly the window a reader can land in.
        let dumper = {
            let e = Arc::clone(&e);
            std::thread::spawn(move || {
                for _ in 0..40 {
                    std::thread::sleep(std::time::Duration::from_millis(3));
                    let _ = e.dump_index_transaction("c", "cv");
                }
            })
        };
        // Readers record every row id the index ever returns, per value.
        let observed: Arc<Mutex<Vec<(i64, u64)>>> = Arc::new(Mutex::new(Vec::new()));
        let mut readers = Vec::new();
        for r in 0..READERS {
            let e = Arc::clone(&e);
            let observed = Arc::clone(&observed);
            readers.push(std::thread::spawn(move || {
                let mut at = r * 7;
                for _ in 0..READS {
                    at = (at * 31 + 17) % (WRITERS * ROWS_PER_WRITER);
                    let value = Literal::Int(at as i64);
                    if let Some(ids) = e.index_lookup("c", "v", &value) {
                        let mut seen = observed.lock().unwrap();
                        seen.extend(ids.into_iter().map(|id| (at as i64, id)));
                    }
                }
            }));
        }
        for w in writers {
            w.join().unwrap();
        }
        for r in readers {
            r.join().unwrap();
        }
        dumper.join().unwrap();
        // Everything written is sealed or in the tail.
        e.dump_index_transaction("c", "cv").unwrap();

        // The rows themselves, as `seq -> value`.
        let rows: Vec<(u64, i64)> = e
            .fetch_rows_with_ids("c")
            .unwrap()
            .into_iter()
            .map(|(seq, row)| {
                let value = match row.get(1) {
                    Some(Value::Int(v)) => *v,
                    _ => 0,
                };
                (seq as u64, value)
            })
            .collect();
        assert_eq!(rows.len(), WRITERS * ROWS_PER_WRITER);

        // The index must return exactly the row ids of each value.
        for (seq, value) in &rows {
            let indexed: HashSet<u64> = e
                .index_lookup("c", "v", &Literal::Int(*value))
                .unwrap()
                .into_iter()
                .collect();
            assert!(
                indexed.contains(seq),
                "row {seq} (v = {value}) is missing from the index"
            );
        }
        // No ghost: every id a concurrent reader saw belongs to the final rows.
        let final_ids: HashSet<u64> = rows.iter().map(|(seq, _)| *seq).collect();
        for (value, id) in observed.lock().unwrap().iter() {
            assert!(
                final_ids.contains(id),
                "index returned the ghost row {id} for v = {value}"
            );
        }
    }

    /// `v = 3.0` is the same predicate as `v = 3` for an integer column, but the
    /// index used to answer neither side of it: the float probe only reads float
    /// keys (so every sealed chunk returned nothing) and the memory tail cannot
    /// parse `"3.000000"` as an integer key either. The value is converted to the
    /// column's key exactly; a value that is not integral declines, so the
    /// predicate is evaluated against the decoded rows instead of a truncated
    /// probe answering a different question.
    #[test]
    fn an_integral_float_equality_reads_the_integer_keys() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE fq (id INT, v INT);").unwrap();
        e.execute("INSERT INTO fq VALUES (1, 3), (2, 4);").unwrap();
        e.execute("CREATE INDEX fqv ON fq (v);").unwrap();
        // The memory tail answers first, then the sealed chunk.
        let hits = e
            .index_lookup("fq", "v", &Literal::Float(3.0))
            .expect("an integral float is answerable");
        assert_eq!(hits.len(), 1);
        e.dump_index_transaction("fq", "fqv").unwrap();
        let hits = e
            .index_lookup("fq", "v", &Literal::Float(3.0))
            .expect("an integral float is answerable from a chunk");
        assert_eq!(hits.len(), 1);
        assert!(e.index_lookup("fq", "v", &Literal::Float(3.5)).is_none());
        assert_eq!(
            rows(&e, "SELECT id FROM fq WHERE v = 3.0;"),
            vec![vec!["1".to_string()]]
        );
        assert!(rows(&e, "SELECT id FROM fq WHERE v = 3.5;").is_empty());
    }

    /// The memory tail's `FLOAT` keys are `f32` while a bound arrives as `f64`,
    /// and the nearest `f32` can sit on the wrong side of the bound. The indexed
    /// KNN path never re-checks the predicate (its rows are a vector projection),
    /// so the narrowed interval has to be exact: a lower bound rounds up to the
    /// first representable value inside it, an upper bound rounds down.
    #[test]
    fn a_float_tail_bound_never_widens_the_interval() {
        use crate::RangeBound;
        use storage::secondary_segment::{Key, KeyKind};
        let low = 1.0f32;
        let high = f32::from_bits(1.0f32.to_bits() + 1);
        let gap = f64::from(high) - f64::from(low);
        // 30% of the way up: the nearest `f32` is `low`, which is *below* the
        // bound, so `>= bound` must start at `high`.
        let lower = f64::from(low) + 0.3 * gap;
        assert_eq!(
            tail_bound(KeyKind::F32, Key::F64(lower), RangeBound::Lower),
            Key::F32(high).ordered_bytes()
        );
        // 70% of the way up: the nearest `f32` is `high`, which is *above* the
        // bound, so `<= bound` must stop at `low`.
        let upper = f64::from(low) + 0.7 * gap;
        assert_eq!(
            tail_bound(KeyKind::F32, Key::F64(upper), RangeBound::Upper),
            Key::F32(low).ordered_bytes()
        );
        // A bound the `f32` grid holds exactly is used as it is.
        for side in [RangeBound::Lower, RangeBound::Upper] {
            assert_eq!(
                tail_bound(KeyKind::F32, Key::F64(1.0), side),
                Key::F32(1.0).ordered_bytes()
            );
        }
    }

    /// `CREATE INDEX` reads the table without excluding writers (the reference
    /// leans on its transaction conflict detection, which this port does not
    /// have yet). A row committed after the scan had read the table is in
    /// neither the chunks the build wrote nor the tail it installs - the write
    /// hook could not put it there while the index was still being built - so
    /// the build appends it to the tail afterwards (`secondary_catch_up`).
    /// Without that step the index silently loses every row in the window.
    #[test]
    fn create_index_covers_rows_committed_while_it_builds() {
        use std::sync::Arc;

        const BEFORE: usize = 200;
        const DURING: usize = 200;

        let dir = tempdir().unwrap();
        let e = Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE cd (id INT, v INT);").unwrap();
        for i in 0..BEFORE {
            e.execute(&format!("INSERT INTO cd VALUES ({i}, {i});"))
                .unwrap();
        }
        // One writer keeps appending while the build scans; the values are
        // unique so every expected row is individually checkable.
        let writer = {
            let e = Arc::clone(&e);
            std::thread::spawn(move || {
                for i in BEFORE..(BEFORE + DURING) {
                    e.execute(&format!("INSERT INTO cd VALUES ({i}, {i});"))
                        .unwrap();
                }
            })
        };
        e.execute("CREATE INDEX cdv ON cd (v);").unwrap();
        writer.join().unwrap();

        // A range predicate is answered by the index (chunk keys plus the
        // tail), so a row the build dropped would be missing from the result.
        let found = rows(&e, "SELECT v FROM cd WHERE v >= 0;");
        let values: std::collections::HashSet<String> =
            found.iter().map(|row| row[0].clone()).collect();
        for i in 0..(BEFORE + DURING) {
            assert!(
                values.contains(&i.to_string()),
                "v = {i} is missing from the index after a concurrent insert"
            );
        }
        assert_eq!(found.len(), BEFORE + DURING, "duplicate index entries");
    }

    /// Timing harness for the `MATCH TENSOR` scoring loop: the current
    /// implementation against the per-`(query, doc)` `Vec` allocation plus
    /// scalar dot it replaced.
    ///
    /// ```text
    /// cargo test --release -p executor --lib tensor_maxsim_throughput -- --ignored --nocapture
    /// ```
    #[test]
    #[ignore = "timing"]
    fn tensor_maxsim_throughput() {
        let (nq, nd, dim) = (32usize, 32usize, 128usize);
        let q: Vec<Vec<f64>> = (0..nq)
            .map(|i| {
                (0..dim)
                    .map(|j| ((i * 3 + j) % 17) as f64 * 0.5 - 4.0)
                    .collect()
            })
            .collect();
        let d: Vec<Vec<f64>> = (0..nd)
            .map(|i| (0..dim).map(|j| ((i * 7 + j) % 13) as f64 - 6.0).collect())
            .collect();

        let rounds = 500;
        let t0 = std::time::Instant::now();
        let mut acc = 0f64;
        for _ in 0..rounds {
            acc += tensor_maxsim(&q, &d, ElemType::Float32, ElemType::Float32);
        }
        let now = t0.elapsed() / rounds;

        // The loop this replaced: a `Vec<&[f64]>` per pair and a sequential dot.
        let t0 = std::time::Instant::now();
        let mut old_acc = 0f64;
        for _ in 0..rounds {
            let mut sum = 0f32;
            for qv in &q {
                let mut best = f32::NEG_INFINITY;
                for dv in &d {
                    let pieces: Vec<&[f64]> = if qv.is_empty() {
                        continue;
                    } else if dv.len() == qv.len() {
                        vec![dv.as_slice()]
                    } else if dv.len() % qv.len() == 0 {
                        dv.chunks(qv.len()).collect()
                    } else {
                        continue;
                    };
                    for piece in pieces {
                        let mut s = 0f32;
                        for (a, b) in qv.iter().zip(piece) {
                            s += quantize_elem(*a, ElemType::Float32)
                                * quantize_elem(*b, ElemType::Float32);
                        }
                        if s > best {
                            best = s;
                        }
                    }
                }
                if best.is_finite() {
                    sum += best;
                }
            }
            old_acc += sum as f64;
        }
        let old = t0.elapsed() / rounds;
        eprintln!(
            "tensor_maxsim nq={nq} nd={nd} dim={dim}: per_pair_vec+scalar={old:?} \
             chunks+simd={now:?} ({:.2}x) acc={acc} {old_acc}",
            old.as_secs_f64() / now.as_secs_f64()
        );
    }

    /// The PLAID budget is "do not build", not "build and spill": this port has
    /// no buffer-manager/mmap search path, so a table past
    /// `INFINITY_PLAID_MAX_BYTES` answers from the sealed brute-force scan. That
    /// decision has to be final for the process - otherwise every later query
    /// rescans the table, retrains the centroids and rewrites the chunk set only
    /// to decline again.
    ///
    /// The budget is a process-wide `OnceLock`, so the case runs in a child
    /// started with a 1-byte budget.
    #[test]
    fn a_plaid_index_over_budget_keeps_answering_from_the_sealed_scan() {
        const CHILD: &str = "INFINITY_PLAID_BUDGET_CHILD";
        const DIR: &str = "INFINITY_PLAID_BUDGET_DIR";
        if let Ok(dir) = std::env::var(DIR) {
            let e = Engine::open(std::path::Path::new(&dir)).unwrap();
            e.execute("CREATE TABLE pb (id INT, t TENSOR(FLOAT,4));")
                .unwrap();
            let values: Vec<String> = (0..300)
                .map(|i| format!("({i}, [[{},0.5,0.25,0.125]])", (i % 7) as f32))
                .collect();
            e.execute(&format!("INSERT INTO pb VALUES {};", values.join(",")))
                .unwrap();
            e.execute("CREATE INDEX pbi ON pb (t) USING PLAID;")
                .unwrap();
            let first = e.multivector_index_for("default_db.pb", "t", 4).unwrap();
            {
                let guard = first.read().unwrap();
                assert!(guard.plaid.is_none(), "PLAID must not be built over budget");
                assert!(guard.plaid_unavailable, "the refusal must be recorded");
                assert!(guard.plaid_dropped, "the index must report the fallback");
            }
            // The refusal is final, so the cached index is reused: a rebuild
            // would scan the table and retrain the centroids again.
            let again = e.multivector_index_for("default_db.pb", "t", 4).unwrap();
            assert!(
                std::sync::Arc::ptr_eq(&first, &again),
                "the index was rebuilt although the budget refused to build PLAID"
            );
            return;
        }
        let dir = tempdir().unwrap();
        let out = std::process::Command::new(std::env::current_exe().unwrap())
            .args([
                "--exact",
                "engine::tests_ut::tests::a_plaid_index_over_budget_keeps_answering_from_the_sealed_scan",
                "--nocapture",
            ])
            .env(CHILD, "1")
            .env(DIR, dir.path())
            .env("INFINITY_PLAID_MAX_BYTES", "1")
            .output()
            .unwrap();
        let stdout = String::from_utf8_lossy(&out.stdout);
        assert!(
            out.status.success(),
            "child failed:\n{stdout}\n{}",
            String::from_utf8_lossy(&out.stderr)
        );
        assert!(
            stdout.contains("1 passed"),
            "the child ran no test:\n{stdout}"
        );
    }

    /// A NULL sparse cell writes no postings, so `doc_count` can never reach the
    /// table's row count on such a table: the coverage check based on it never
    /// agreed and every `MATCH SPARSE` rebuilt the whole index, chunks
    /// included. Coverage is measured by the rows the index has *seen*.
    #[test]
    fn sparse_index_with_null_rows_is_not_rebuilt_per_query() {
        use storage::bmp::IdxWidth;

        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE spn (id INT, v SPARSE(FLOAT,30000));")
            .unwrap();
        e.execute("INSERT INTO spn VALUES (1, [0:1.0,10:1.0]), (2, NULL), (3, [20:2.0]);")
            .unwrap();
        e.execute("CREATE INDEX spnv ON spn (v);").unwrap();
        let first = e
            .sparse_index_for("default_db.spn", "v", IdxWidth::I16)
            .unwrap();
        let again = e
            .sparse_index_for("default_db.spn", "v", IdxWidth::I16)
            .unwrap();
        assert!(
            std::sync::Arc::ptr_eq(&first, &again),
            "the index was rebuilt although it had seen every row"
        );
        // A NULL insert keeps the same instance: the write hook counts the row
        // even though it contributes no postings.
        e.execute("INSERT INTO spn VALUES (4, NULL);").unwrap();
        let after = e
            .sparse_index_for("default_db.spn", "v", IdxWidth::I16)
            .unwrap();
        assert!(
            std::sync::Arc::ptr_eq(&first, &after),
            "the index was rebuilt after a NULL row landed in the tail"
        );
    }

    /// `kill -9` in the middle of a dump leaves a spilled chunk file half
    /// written while the rows themselves are already durable. The restart
    /// rebuilds every secondary index from the rows (`rebuild_indexes`), and the
    /// rebuild rewrites the spill set, so the truncated file must never reach
    /// the reloaded index - a query answered from a half-parsed chunk would
    /// silently lose rows.
    #[test]
    fn a_truncated_chunk_is_rebuilt_from_the_rows_on_startup() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tc (id INT, v INT);").unwrap();
        let values: Vec<String> = (0..500).map(|i| format!("({i}, {i})")).collect();
        e.execute(&format!("INSERT INTO tc VALUES {};", values.join(",")))
            .unwrap();
        e.execute("CREATE INDEX tcv ON tc (v);").unwrap();
        e.dump_index_transaction("tc", "tcv").unwrap();
        e.flush().unwrap();
        drop(e);

        assert!(
            truncate_spilled_chunks(dir.path()) > 0,
            "the dump must have written a chunk file to truncate"
        );

        let e = Engine::open(dir.path()).unwrap();
        let found = rows(&e, "SELECT v FROM tc WHERE v >= 0;");
        assert_eq!(found.len(), 500, "a truncated chunk lost rows");
    }

    /// Cuts every spilled secondary chunk down to half its length: what a
    /// process killed in the middle of a dump leaves behind.
    fn truncate_spilled_chunks(root: &std::path::Path) -> usize {
        let mut cut = 0;
        for sub in ["secondary", "secondary_low"] {
            let Ok(entries) = std::fs::read_dir(root.join(sub)) else {
                continue;
            };
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    if let Ok(files) = std::fs::read_dir(&path) {
                        for file in files.flatten() {
                            cut += truncate_chunk_file(&file.path());
                        }
                    }
                } else {
                    cut += truncate_chunk_file(&path);
                }
            }
        }
        cut
    }

    fn truncate_chunk_file(path: &std::path::Path) -> usize {
        let Ok(meta) = std::fs::metadata(path) else {
            return 0;
        };
        if !path.extension().is_some_and(|e| e == "idx") || meta.len() < 16 {
            return 0;
        }
        let Ok(file) = std::fs::OpenOptions::new().write(true).open(path) else {
            return 0;
        };
        usize::from(file.set_len(meta.len() / 2).is_ok())
    }

    /// The sustained-load shape: a wide table written in bursts with index
    /// dumps between them, then a reopen. It is the million-row counterpart of
    /// `secondary_index_is_correct_under_concurrent_writes_and_dumps`, and it is
    /// ignored by default because it writes `INFINITY_STRESS_ROWS` rows
    /// (1,000,000 unless the variable says otherwise):
    ///
    /// ```text
    /// INFINITY_STRESS_ROWS=2000000 cargo test -p executor --lib \
    ///     secondary_stress_sustained_dumps -- --ignored --nocapture
    /// ```
    #[test]
    #[ignore = "writes up to a million rows; run explicitly"]
    fn secondary_stress_sustained_dumps() {
        let total: usize = std::env::var("INFINITY_STRESS_ROWS")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(1_000_000);
        const BATCH: usize = 5_000;

        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE st (id INT, v INT);").unwrap();
        e.execute("CREATE INDEX stv ON st (v);").unwrap();

        let mut written = 0usize;
        let mut batches = 0usize;
        while written < total {
            let end = (written + BATCH).min(total);
            let values: Vec<String> = (written..end).map(|i| format!("({i}, {i})")).collect();
            e.execute(&format!("INSERT INTO st VALUES {};", values.join(",")))
                .unwrap();
            written = end;
            batches += 1;
            // Every tenth burst dumps what the tail accumulated, so the chunk
            // set grows while later bursts keep appending to the fresh tail.
            if batches % 10 == 0 {
                e.dump_index_transaction("st", "stv").unwrap();
            }
            // A read in the middle of the load: it must see at least the rows
            // the dumps have already sealed, and never a row that is not there.
            if batches % 25 == 0 {
                let _ = rows(&e, "SELECT v FROM st WHERE v >= 0;");
            }
        }
        // The tail is dumped so the final read comes from the chunks, not from
        // the buffer that would hide a broken chunk set.
        e.dump_index_transaction("st", "stv").unwrap();

        let found = rows(&e, "SELECT v FROM st WHERE v >= 0;");
        assert_eq!(found.len(), total, "the index lost rows under load");
        drop(e);

        let e = Engine::open(dir.path()).unwrap();
        let found = rows(&e, "SELECT v FROM st WHERE v >= 0;");
        assert_eq!(found.len(), total, "the rebuild lost rows under load");
    }

    /// `CREATE INDEX i ON t (lower(name))` is the reference's
    /// `kSecondaryFunctional`: the expression is the catalog's column, the base
    /// column is what the rows are read from, the key domain is the function's
    /// return type, and `lower(name) = 'abc'` is answered from the index.
    #[test]
    fn functional_index_answers_a_function_predicate() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE f (id INT, name VARCHAR);").unwrap();
        e.execute("INSERT INTO f VALUES (1, 'Abc'), (2, 'abc'), (3, 'XYZ');")
            .unwrap();
        e.execute("CREATE INDEX fi ON f (lower(name));").unwrap();

        let q = "SELECT id FROM f WHERE lower(name) = 'abc';";
        assert_eq!(
            rows(&e, q),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        // A new row lands in the memory tail, evaluated the same way.
        e.execute("INSERT INTO f VALUES (4, 'ABC');").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["2".to_string()],
                vec!["4".to_string()]
            ]
        );
        // And after the dump the answer comes from the term chunk.
        e.dump_index_transaction("f", "fi").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["2".to_string()],
                vec!["4".to_string()]
            ]
        );
        assert_eq!(
            rows(&e, "SELECT id FROM f WHERE lower(name) = 'xyz';"),
            vec![vec!["3".to_string()]]
        );

        // The record carries the function, so reopening restores the index.
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(
            rows(&e, q),
            vec![
                vec!["1".to_string()],
                vec!["2".to_string()],
                vec!["4".to_string()]
            ]
        );
    }

    /// A hashed (`VARCHAR`) key has no order, but an equality range still has an
    /// exact key: `ConvertToHashU64` plus the chunk's lower/upper_bound pair.
    /// A non-equality range has no hash order, so it falls back to the rows.
    #[test]
    fn hashed_text_equality_uses_the_index() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE h (id INT, name VARCHAR);").unwrap();
        e.execute("CREATE INDEX hn ON h (name);").unwrap();
        e.execute("INSERT INTO h VALUES (1, 'a'), (2, 'b'), (3, 'a');")
            .unwrap();
        let probe = |e: &Engine| {
            e.index_range_lookup(
                "h",
                "name",
                &[(Value::Str("a".into()), Value::Str("a".into()))],
            )
            .unwrap()
        };
        assert_eq!(probe(&e), vec![0, 2]);

        e.dump_index_transaction("h", "hn").unwrap();
        assert_eq!(probe(&e), vec![0, 2]);
        assert_eq!(
            rows(&e, "SELECT id FROM h WHERE name = 'a';"),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert!(e
            .index_range_lookup(
                "h",
                "name",
                &[(Value::Str("a".into()), Value::Str("b".into()))],
            )
            .is_none());
    }

    /// A NULL is not a key: `SecondaryIndexInMemT::InsertInner` skips a row
    /// whose null bit is set, so neither the memory tail nor a chunk can answer
    /// for it.
    #[test]
    fn null_cells_are_not_indexed() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE n (id INT, flag BOOL, name VARCHAR);")
            .unwrap();
        e.execute("CREATE INDEX nf ON n (flag);").unwrap();
        e.execute("CREATE INDEX nn ON n (name);").unwrap();
        e.execute("INSERT INTO n VALUES (1, true, 'a'), (2, false, NULL), (3, NULL, 'b');")
            .unwrap();

        // Row 3's NULL flag must not join the `false` bitmap.
        assert_eq!(
            e.index_lookup("n", "flag", &Literal::Str("false".into()))
                .unwrap(),
            vec![1]
        );
        // Row 2's NULL name has no key at all.
        assert!(e
            .index_lookup("n", "name", &Literal::Str(String::new()))
            .unwrap()
            .is_empty());
        assert_eq!(
            rows(&e, "SELECT id FROM n WHERE name = 'a';"),
            vec![vec!["1".to_string()]]
        );

        // Same after the dump: the chunks carry only the non-NULL keys.
        e.dump_index_transaction("n", "nf").unwrap();
        assert_eq!(
            e.index_lookup("n", "flag", &Literal::Str("false".into()))
                .unwrap(),
            vec![1]
        );
        assert_eq!(
            rows(&e, "SELECT id FROM n WHERE flag = false;"),
            vec![vec!["2".to_string()]]
        );
    }

    /// The index-scan filter reads the column's chunk bitmaps rather than
    /// re-scanning the rows (`IndexFilterEvaluatorSecondary` ->
    /// `TrunkReaderT::OutPut`), so its answers must still match a plain
    /// predicate across a dump, a range and a tombstone.
    #[test]
    fn secondary_filter_answers_from_the_index() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE s (id INT, v INT);").unwrap();
        e.execute("CREATE INDEX sv ON s (v);").unwrap();
        let values: Vec<String> = (0..200).map(|i| format!("({i}, {})", i % 13)).collect();
        e.execute(&format!("INSERT INTO s VALUES {};", values.join(", ")))
            .unwrap();
        e.dump_index_transaction("s", "sv").unwrap();

        let mut expected: Vec<String> = (0..200)
            .filter(|i| (10..=12).contains(&(i % 13)))
            .map(|i| i.to_string())
            .collect();
        expected.sort_by_key(|id| id.parse::<u32>().unwrap());
        let mut got: Vec<String> = rows(&e, "SELECT id FROM s WHERE v BETWEEN 10 AND 12;")
            .into_iter()
            .map(|row| row[0].clone())
            .collect();
        got.sort_by_key(|id| id.parse::<u32>().unwrap());
        assert_eq!(got, expected);

        // A tombstone is not in the row set, so its index entry selects no row.
        e.execute("DELETE FROM s WHERE id = 22;").unwrap();
        let after = rows(&e, "SELECT id FROM s WHERE v = 9;");
        assert!(!after.iter().any(|row| row[0] == "22"));
        // 15 rows carry v = 9 before the delete, so 14 remain.
        assert_eq!(after.len(), 14);
    }

    /// `OptimizeIndex` folds a segment's secondary chunks into one: the merge is
    /// a sort over the old chunks' `(key, offset)` pairs, whose offsets are
    /// already segment-relative, so the merged chunk answers the same queries.
    #[test]
    fn optimize_merges_secondary_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE m (id INT, v INT, flag BOOL);")
            .unwrap();
        e.execute("CREATE INDEX mv ON m (v);").unwrap();
        e.execute("CREATE INDEX mf ON m (flag);").unwrap();
        e.execute("INSERT INTO m VALUES (1, 10, true), (2, 20, false);")
            .unwrap();
        for index in ["mv", "mf"] {
            e.dump_index_transaction("m", index).unwrap();
        }
        e.execute("INSERT INTO m VALUES (3, 10, true);").unwrap();
        for index in ["mv", "mf"] {
            e.dump_index_transaction("m", index).unwrap();
        }
        let high = |e: &Engine, column: &str| -> usize {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .find_map(|t| t.secondary.disk.get(column))
                .map(|r| r.len())
                .unwrap_or(0)
        };
        let low = |e: &Engine, column: &str| -> usize {
            let tables = e.tables.lock().unwrap();
            tables
                .values()
                .find_map(|t| t.secondary.low.get(column))
                .map(|r| r.len())
                .unwrap_or(0)
        };
        assert_eq!(high(&e, "v"), 2);
        assert_eq!(low(&e, "flag"), 2);

        e.optimize_index_chunks().unwrap();

        assert_eq!(high(&e, "v"), 1);
        assert_eq!(low(&e, "flag"), 1);
        assert_eq!(e.secondary_chunk_meta("m", "v"), vec![(0, 3)]);
        assert_eq!(
            rows(&e, "SELECT id FROM m WHERE v = 10;"),
            vec![vec!["1".to_string()], vec!["3".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT id FROM m WHERE flag = false;"),
            vec![vec!["2".to_string()]]
        );
        // A merged segment publishes one chunk; the next dump adds a tail chunk.
        assert_eq!(
            rows(&e, "SELECT id FROM m WHERE v = 20;"),
            vec![vec!["2".to_string()]]
        );
    }

    /// One quota governs every index form: when it is exceeded the tails are
    /// dumped and released, and the results stay correct.
    #[test]
    fn memindex_quota_seals_every_tail() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // Any positive budget is exceeded by the first row's tails.
        e.set_memindex_quota(1);
        // EMVB is a tensor-column index in the reference
        // (`IndexEMVB::ValidateColumnDataType`), so the multi-vector form here
        // is a `TENSOR(FLOAT,2)`.
        e.execute("CREATE TABLE q (id INT, m TENSOR(FLOAT,2), s SPARSE(FLOAT,10), c INT);")
            .unwrap();
        e.execute("CREATE INDEX qi ON q (c);").unwrap();
        // Both PQ options are mandatory in the reference (`IndexEMVB::Make`
        // starts them at 0 and 0 is not an acceptable value).
        e.execute(
            "CREATE INDEX qm ON q (m) USING EMVB WITH (pq_subspace_num = 2, pq_subspace_bits = 8);",
        )
        .unwrap();
        e.execute("CREATE INDEX qs ON q (s) USING BMP;").unwrap();
        e.execute(
            "INSERT INTO q VALUES (1, [1.0,0.0], [0:1.0, 1:2.0], 7), \
             (2, [0.0,1.0], [1:3.0], 7);",
        )
        .unwrap();

        let mv = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.mv.values().next().cloned())
            .unwrap();
        let mv_tail = mv.read().unwrap().tail_bytes;
        assert_eq!(mv_tail, 0);
        let sp = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.sparse.values().next().cloned())
            .unwrap();
        let sp_tail = sp.read().unwrap().tail_bytes;
        assert_eq!(sp_tail, 0);
        let tables = e.tables.lock().unwrap();
        let sec = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(sec.key_count(), 0);
        drop(tables);

        // The sealed forms answer every path.
        assert_eq!(
            rows(&e, "SELECT id FROM q WHERE c = 7;"),
            vec![vec!["1".to_string()], vec!["2".to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM q SEARCH MATCH SPARSE (s, [1:1.0], 'ip', 2);"
            ),
            vec![vec!["2".to_string()], vec!["1".to_string()]]
        );
        assert!(!rows(
            &e,
            "SELECT id FROM q SEARCH MATCH TENSOR (m, [1.0,0.0], 'float', 'maxsim', 'topn=2');"
        )
        .is_empty());
    }

    /// WAND now covers filters, `minimum_should_match`, `delta_term` and
    /// `similarity=boolean`; each must agree with the reference semantics.
    #[test]
    fn wand_covers_boolean_filters_min_should_and_delta() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ftw (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO ftw VALUES (1,'alpha beta'),(2,'alpha gamma'),\
             (3,'beta gamma'),(4,'alpha beta gamma');",
        )
        .unwrap();
        e.execute("CREATE INDEX ftw_i ON ftw(body) USING FULLTEXT;")
            .unwrap();

        // Boolean similarity: one point per matched term.
        assert_eq!(
            rows(
                &e,
                "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
                 ('body','alpha beta','topn=10;similarity=boolean');"
            ),
            vec![
                vec!["1".to_string(), "2.000000".to_string()],
                vec!["4".to_string(), "2.000000".to_string()],
                vec!["2".to_string(), "1.000000".to_string()],
                vec!["3".to_string(), "1.000000".to_string()],
            ]
        );

        // `minimum_should_match=2` keeps only documents with both terms.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM ftw SEARCH MATCH TEXT \
                 ('body','alpha beta','topn=10;minimum_should_match=2');"
            ),
            vec![vec!["1".to_string()], vec!["4".to_string()]]
        );

        // A filter is applied before ranking; the surviving scores match the
        // unfiltered query.
        let all = rows(
            &e,
            "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10');",
        );
        let filtered = rows(
            &e,
            "SELECT id, ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10;filter=id <= 2');",
        );
        assert_eq!(filtered.len(), 2);
        for row in &filtered {
            assert!(all.contains(row), "{row:?} not in {all:?}");
        }

        // `delta_term` adds a constant to the tf norm:
        // df=3, N=4 → idf=ln(1+1.5/3.5), len=2, avg=2.25,
        // norm=2.2/(1+1.2*(0.25+0.75*2/2.25))=1.047619…
        let idf = (1.0f64 + 1.5 / 3.5).ln();
        let norm = 2.2 / (1.0 + 1.2 * (0.25 + 0.75 * 2.0 / 2.25));
        let expected = 2.0 * idf * (norm + 1.0);
        let got = rows(
            &e,
            "SELECT ROUND(SCORE(),4) FROM ftw SEARCH MATCH TEXT \
             ('body','alpha beta','topn=1;bm25_param_delta_term=1');",
        );
        assert_eq!(got.len(), 1);
        let value: f64 = got[0][0].parse().unwrap();
        assert!((value - expected).abs() < 1e-4, "{value} vs {expected}");
    }

    /// `bm25_param_k1` / `bm25_param_b` override the ranker's constants, exactly
    /// as `bound_select_statement_impl.cpp:239-252` writes them into
    /// `BM25Params`, and every scoring path reads them.
    #[test]
    fn bm25_params_override_the_constants() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bm (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO bm VALUES (1,'alpha beta'),(2,'alpha gamma'),\
             (3,'beta gamma'),(4,'alpha beta gamma');",
        )
        .unwrap();
        e.execute("CREATE INDEX bm_i ON bm(body) USING FULLTEXT;")
            .unwrap();

        // id=4: tf=1 per term, dl=3; N=4, total_len=9 → avgdl=2.25; df=3.
        let idf = (1.0f64 + 1.5 / 3.5).ln();
        let score_of = |k1: f64, b: f64| {
            let norm = (k1 + 1.0) / (1.0 + k1 * ((1.0 - b) + b * 3.0 / 2.25));
            2.0 * idf * norm
        };
        let pick = |rs: &[Vec<String>]| -> f64 {
            rs.iter()
                .find(|r| r[0] == "4")
                .unwrap_or_else(|| panic!("id 4 missing: {rs:?}"))[1]
                .parse()
                .unwrap()
        };

        // Defaults: k1 = 1.2, b = 0.75.
        let base = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM bm SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10');",
        );
        assert!((pick(&base) - score_of(1.2, 0.75)).abs() < 1e-5);

        // The two options move the score onto the new formula.
        let tuned = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM bm SEARCH MATCH TEXT \
             ('body','alpha beta','topn=10;bm25_param_k1=0.5;bm25_param_b=0.5');",
        );
        assert!((pick(&tuned) - score_of(0.5, 0.5)).abs() < 1e-5);
        assert!((pick(&tuned) - pick(&base)).abs() > 1e-3);

        // The query-tree path (`AND`) reads the same parameters: its two term
        // leaves score with the same formula against the same statistics.
        let tree = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM bm SEARCH MATCH TEXT \
             ('body','alpha AND beta','topn=10;bm25_param_k1=0.5;bm25_param_b=0.5');",
        );
        assert!((pick(&tree) - score_of(0.5, 0.5)).abs() < 1e-5);

        // The reference rejects out-of-range values.
        assert!(e
            .execute("SELECT id FROM bm SEARCH MATCH TEXT ('body','alpha','bm25_param_k1=-1');")
            .is_err());
        assert!(e
            .execute("SELECT id FROM bm SEARCH MATCH TEXT ('body','alpha','bm25_param_b=2');")
            .is_err());
    }

    /// `threshold=` is a strict lower bound (`term_doc_iterator_impl.cpp:118`)
    /// and it raises the block-max pruning bar rather than only filtering the
    /// answer afterwards (`UpdateScoreThreshold`), so the pruned and the
    /// unpruned scans return the same documents.
    #[test]
    fn threshold_is_a_strict_lower_bound_and_prunes_the_scan() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tt (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO tt VALUES (1,'alpha beta'),(2,'alpha gamma'),\
             (3,'beta gamma'),(4,'alpha beta gamma');",
        )
        .unwrap();
        e.execute("CREATE INDEX tt_i ON tt(body) USING FULLTEXT;")
            .unwrap();

        // `similarity=boolean` scores one point per matched term, so the scores
        // are exact integers: 2 for the rows carrying both terms, 1 otherwise.
        let strict = "SELECT id FROM tt SEARCH MATCH TEXT \
                      ('body','alpha beta','topn=10;similarity=boolean;threshold=2');";
        assert!(
            rows(&e, strict).is_empty(),
            "threshold is a strict lower bound"
        );

        let above_one = "SELECT id FROM tt SEARCH MATCH TEXT \
                         ('body','alpha beta','topn=10;similarity=boolean;threshold=1');";
        assert_eq!(
            rows(&e, above_one),
            vec![vec!["1".to_string()], vec!["4".to_string()]]
        );

        // Same answer with pruning switched off: the threshold changes what the
        // iterator decodes, never the result set.
        let unpruned = "SELECT id FROM tt SEARCH MATCH TEXT \
                        ('body','alpha beta','topn=10;similarity=boolean;threshold=1;block_max=false');";
        assert_eq!(rows(&e, above_one), rows(&e, unpruned));

        // A non-positive threshold is the reference's `threshold_ <= 0.0f`
        // shortcut: it filters nothing.
        let zero = "SELECT id FROM tt SEARCH MATCH TEXT \
                    ('body','alpha beta','topn=10;similarity=boolean;threshold=0');";
        assert_eq!(rows(&e, zero).len(), 4);
    }

    /// `rank_features` adds the posting's doc-payload column, decoded with
    /// `SmallFloat::UInt16ToFloat122` - `RankFeatureDocIterator`'s number - and
    /// not the raw JSON value of the row. The payload survives both the memory
    /// tail and the sealed segment, so the score is the same before and after a
    /// flush.
    #[test]
    fn rank_features_reads_the_index_doc_payload() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rf (id INT, body VARCHAR, feat VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO rf VALUES (1,'alpha','[{\"Tag1\":1}]'),\
             (2,'alpha','[{\"Tag1\":2}]');",
        )
        .unwrap();
        e.execute("CREATE INDEX rf_body ON rf (body) USING FULLTEXT;")
            .unwrap();
        e.execute(
            "CREATE INDEX rf_feat ON rf (feat) USING FULLTEXT WITH (analyzer='rankfeatures');",
        )
        .unwrap();

        // Both rows carry the same `body`, so the BM25 half is identical and the
        // difference between the two scores is exactly the feature weight.
        let base = rows(
            &e,
            "SELECT id, SCORE() FROM rf SEARCH MATCH TEXT ('body','alpha','topn=10');",
        );
        let ranked = rows(
            &e,
            "SELECT id, SCORE() FROM rf SEARCH MATCH TEXT \
             ('body','alpha','topn=10;rank_features=feat^Tag1^1');",
        );
        let pick = |rs: &[Vec<String>], id: &str| -> f64 {
            rs.iter()
                .find(|r| r[0] == id)
                .unwrap_or_else(|| panic!("id {id} missing: {rs:?}"))[1]
                .parse()
                .unwrap()
        };
        assert!((pick(&ranked, "1") - pick(&base, "1") - 1.0).abs() < 1e-4);
        assert!((pick(&ranked, "2") - pick(&base, "2") - 2.0).abs() < 1e-4);

        // Round-trip through the sealed segment: the payload is written into the
        // doc list's `of_doc_payload` column and read back by
        // `SegmentIndexReader::doc_payloads`.
        e.flush().unwrap();
        let sealed = rows(
            &e,
            "SELECT id, SCORE() FROM rf SEARCH MATCH TEXT \
             ('body','alpha','topn=10;rank_features=feat^Tag1^1');",
        );
        assert_eq!(sealed.len(), ranked.len());
        assert!((pick(&sealed, "1") - pick(&ranked, "1")).abs() < 1e-4);
        assert!((pick(&sealed, "2") - pick(&ranked, "2")).abs() < 1e-4);
    }

    /// A phrase, a multi-token clause under `operator=and`, and a boolean
    /// similarity query answer from the index's postings and position lists: no
    /// row is re-tokenised (`MATCH_ROWS_TOKENIZED` stays 0), which is what the
    /// row-scan path used to do for every query it could not answer from the
    /// postings alone.
    /// DROP TABLE must take the full-text chunks with it. The chunk directory is
    /// keyed by <db>.<table>.<column>, not by table id, so a table recreated
    /// under the same name would otherwise search the dropped one's documents
    /// (and score them twice).
    #[test]
    fn a_recreated_table_does_not_search_the_dropped_tables_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rt (id INT, body VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO rt VALUES (1,'alpha beta'),(2,'alpha gamma');")
            .unwrap();
        e.execute("CREATE INDEX rt_i ON rt(body) USING FULLTEXT;")
            .unwrap();
        e.flush().unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM rt SEARCH MATCH TEXT (body,'alpha','topn=10');"
            )
            .len(),
            2
        );

        e.execute("DROP TABLE rt;").unwrap();
        e.execute("CREATE TABLE rt (id INT, body VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO rt VALUES (1,'delta epsilon');")
            .unwrap();
        e.execute("CREATE INDEX rt_i ON rt(body) USING FULLTEXT;")
            .unwrap();
        e.flush().unwrap();

        assert_eq!(
            rows(
                &e,
                "SELECT id FROM rt SEARCH MATCH TEXT (body,'alpha','topn=10');"
            ),
            Vec::<Vec<String>>::new(),
            "the dropped table's documents came back"
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM rt SEARCH MATCH TEXT (body,'delta','topn=10');"
            )
            .len(),
            1
        );
    }

    #[test]
    fn a_phrase_match_answers_from_the_index() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE fp (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO fp VALUES (1,'alpha beta'),(2,'alpha gamma'),\
             (3,'beta gamma'),(4,'alpha beta gamma'),(5,'beta alpha'),(6,'gamma');",
        )
        .unwrap();
        e.execute("CREATE INDEX fp_i ON fp(body) USING FULLTEXT;")
            .unwrap();

        MATCH_ROWS_TOKENIZED.with(|n| n.set(0));
        // A quoted phrase is one unit of two tokens: only adjacent occurrences
        // in that order match, and the positions come from the index. The score
        // is `PhraseWeight`: the phrase frequency is the tf and the idf is the
        // sum over the phrase's terms. n=6, avg_len=2, df(alpha)=df(beta)=4
        // (documents 1, 2, 4 and 5 hold `alpha`; 1, 3, 4 and 5 hold `beta`)
        // → idf sum = 2 * ln(1+2.5/4.5) = 0.883666; doc 1 has len 2 → norm
        // 1.0, doc 4 has len 3 → norm 2.2/2.65.
        assert_eq!(
            rows(
                &e,
                "SELECT id, ROUND(SCORE(),4) FROM fp SEARCH MATCH TEXT \
                 (body,'\"alpha beta\"','topn=10');"
            ),
            vec![
                vec!["1".to_string(), "0.883700".to_string()],
                vec!["4".to_string(), "0.733600".to_string()]
            ]
        );
        // `operator=and` is an AND of the bare terms, not a phrase: `beta
        // alpha` matches too, and the order is score-then-id.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM fp SEARCH MATCH TEXT \
                 (body,'alpha beta','topn=10;operator=and');"
            ),
            vec![
                vec!["1".to_string()],
                vec!["5".to_string()],
                vec!["4".to_string()]
            ]
        );
        // Mixed units: a bare term OR a phrase, with a filter that needs the
        // row value but no tokenisation. Documents 1, 2, 3 and 4 match; the
        // filter drops the rest.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM fp SEARCH MATCH TEXT \
                 (body,'gamma \"alpha beta\"','topn=10;filter=id < 5');"
            )
            .len(),
            4
        );
        // `similarity=boolean` scores per field from the postings too; it
        // counts matched *terms* (not phrase units), so every document with
        // `alpha` or `beta` is returned.
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM fp SEARCH MATCH TEXT \
                 (body,'\"alpha beta\"','topn=10;similarity=boolean');"
            ),
            vec![
                vec!["1".to_string()],
                vec!["4".to_string()],
                vec!["5".to_string()],
                vec!["2".to_string()],
                vec!["3".to_string()]
            ]
        );
        assert_eq!(
            MATCH_ROWS_TOKENIZED.with(|n| n.get()),
            0,
            "the column form must answer from the index"
        );
    }

    /// The field-group form (`MATCH TEXT ('', '(doc:…)')`) also starts from the
    /// index: only the documents that hold one of the query terms are
    /// tokenised, in both scoring modes.
    #[test]
    fn a_field_group_match_tokenises_only_the_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE fg2 (id INT, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO fg2 VALUES (1,'alpha beta'),(2,'gamma'),(3,'delta'),\
             (4,'alpha gamma'),(5,'epsilon'),(6,'zeta');",
        )
        .unwrap();
        e.execute("CREATE INDEX fg2_i ON fg2(body) USING FULLTEXT;")
            .unwrap();

        MATCH_ROWS_TOKENIZED.with(|n| n.set(0));
        let hits = rows(
            &e,
            "SELECT id FROM fg2 SEARCH MATCH TEXT ('','(body:alpha)^1 (body:gamma)^1', 10);",
        );
        // `4` holds both terms, then the shorter `gamma` document outranks
        // `alpha beta`.
        assert_eq!(
            hits,
            vec![
                vec!["4".to_string()],
                vec!["2".to_string()],
                vec!["1".to_string()]
            ]
        );
        assert_eq!(
            MATCH_ROWS_TOKENIZED.with(|n| n.get()),
            3,
            "three documents hold a query term"
        );

        // `similarity=boolean` takes the other field-query path; it starts from
        // the same candidate set.
        MATCH_ROWS_TOKENIZED.with(|n| n.set(0));
        let hits = rows(
            &e,
            "SELECT id FROM fg2 SEARCH MATCH TEXT \
             ('','(body:alpha)^1 (body:gamma)^1', 'similarity=boolean');",
        );
        assert_eq!(hits.len(), 3, "{hits:?}");
        assert_eq!(MATCH_ROWS_TOKENIZED.with(|n| n.get()), 3);
    }

    /// IVF chunks written by the mirror are loaded and probed for candidates.
    #[test]
    fn ivf_index_serves_vector_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE iv (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute(
            "INSERT INTO iv VALUES (1,[0.0,0.0]),(2,[1.0,0.0]),\
             (3,[0.0,1.0]),(4,[2.0,2.0]);",
        )
        .unwrap();
        e.execute("CREATE INDEX ivx ON iv (v) USING IVF WITH (metric = l2);")
            .unwrap();
        // Rows written before the dump live in the IVF memory segment and are
        // merged with the sealed chunks.
        e.execute("INSERT INTO iv VALUES (5,[0.9,0.1]);").unwrap();
        let tables = e.tables.lock().unwrap();
        let entry = tables.get("default_db.iv").unwrap();
        assert!(!entry.ivf_mem.get("v").unwrap().is_empty());
        drop(tables);
        let q = "SELECT id FROM iv SEARCH MATCH VECTOR \
                 (v, [0.9,0.1], 'float', 'l2', 2) WITH (nprobe = 16);";
        assert_eq!(
            rows(&e, q),
            vec![vec!["5".to_string()], vec!["2".to_string()]]
        );
        e.flush().unwrap();
        let tables = e.tables.lock().unwrap();
        let entry = tables.get("default_db.iv").unwrap();
        assert!(entry.ivf_mem.get("v").unwrap().is_empty());
        assert!(entry.ivf.values().any(|segments| !segments.is_empty()));
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["5".to_string()], vec!["2".to_string()]]
        );
    }

    /// `CREATE INDEX` materialises the index (C++ `IndexBuilder`); the first
    /// query must not be what builds it.
    #[test]
    fn create_index_builds_fulltext_and_hnsw_from_existing_rows() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bi (c1 INT, body VARCHAR, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute(
            "INSERT INTO bi VALUES (1,'alpha beta',[0.1,0.2,0.3,0.4]),\
             (2,'alpha gamma',[0.4,0.3,0.2,0.1]),\
             (3,'delta',[0.0,0.0,0.0,0.0]);",
        )
        .unwrap();
        e.execute("CREATE INDEX bi_ft ON bi (body) USING FULLTEXT;")
            .unwrap();
        e.execute(
            "CREATE INDEX bi_v ON bi (v) USING HNSW WITH (M=16, ef_construction=20, metric=l2);",
        )
        .unwrap();

        // No query has run yet: both indexes already hold every live row.
        let tables = e.tables.lock().unwrap();
        let entry = tables.get("default_db.bi").unwrap();
        let text = entry.text.get("body").unwrap().read().unwrap();
        assert_eq!(text.total_docs(), 3);
        drop(text);
        let mem = entry.hnsw_mem.get("v").unwrap().read().unwrap();
        assert_eq!(mem.rows().len(), 3, "HNSW memory segment must be seeded");
    }

    /// `memindex_memory_quota` accounts one batch at a time and subtracts only
    /// the quota when it dumps, exactly like `MemIndexTracer`: six rows of a
    /// four-dimensional vector cost 1008 bytes, so the first batch stays under
    /// 1 KB, the second crosses (and dumps), the third crosses again.
    /// Measured against the reference: three batches leave two segments.
    #[test]
    fn the_memindex_quota_seals_one_hnsw_chunk_per_batch() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_quota(1024);
        e.execute("CREATE TABLE hc (c1 INT, c2 EMBEDDING(FLOAT,4));")
            .unwrap();
        let row = "(2,[0.1,0.2,0.3,-0.2])";
        e.execute(&format!(
            "INSERT INTO hc VALUES {row},{row},{row},{row},{row},{row};"
        ))
        .unwrap();
        e.execute(
            "CREATE INDEX hx ON hc (c2) USING HNSW WITH (M = 16, ef_construction = 20, \
             metric = l2, block_size = 1);",
        )
        .unwrap();
        for _ in 0..3 {
            e.execute(&format!(
                "INSERT INTO hc VALUES {row},{row},{row},{row},{row},{row};"
            ))
            .unwrap();
        }
        // Two chunks for three batches: the first batch fits, the second
        // crosses the quota, and the bytes above the quota stay accounted so
        // the third crosses again.
        assert_eq!(e.hnsw_chunk_ids("hc", "hx").len(), 2);
    }

    /// A write that reaches the engine through the Thrift/import primitives
    /// rather than SQL still has to invalidate cached `SELECT` results: the
    /// memidx restart suite counts rows after an SDK insert and must see it.
    #[test]
    fn a_direct_insert_invalidates_cached_select_results() {
        use parser::Literal;
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE dc (c1 INT);").unwrap();
        e.insert_literals(
            "default_db.dc",
            None,
            vec![vec![Literal::Int(1)], vec![Literal::Int(2)]],
        )
        .unwrap();
        assert_eq!(
            rows(&e, "SELECT count(*) FROM dc;"),
            vec![vec!["2".to_string()]]
        );
        // The same primitive again: the cached `count(*)` above must not win.
        e.insert_literals("default_db.dc", None, vec![vec![Literal::Int(3)]])
            .unwrap();
        assert_eq!(
            rows(&e, "SELECT count(*) FROM dc;"),
            vec![vec!["3".to_string()]]
        );
    }

    /// `storage_type = product_quantization` writes the C++ PQ chunk layout
    /// (per-subspace codebook + packed codes) and searches it.
    #[test]
    fn a_product_quantized_ivf_index_mirrors_and_serves_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ivpq (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute(
            "INSERT INTO ivpq VALUES (1,[0.0,0.0,0.0,0.0]),(2,[1.0,0.0,0.0,0.0]),\
             (3,[0.0,1.0,0.0,0.0]),(4,[0.0,0.0,1.0,0.0]),\
             (5,[0.0,0.0,0.0,1.0]),(6,[1.0,1.0,0.0,0.0]),\
             (7,[0.0,0.0,1.0,1.0]),(8,[2.0,2.0,2.0,2.0]);",
        )
        .unwrap();
        e.execute(
            "CREATE INDEX ivpq_idx ON ivpq (v) USING IVF WITH \
             (metric = l2, storage_type = product_quantization, \
             product_quantization_subspace_num = 2, \
             product_quantization_subspace_bits = 12);",
        )
        .unwrap();
        e.flush().unwrap();
        let segments = e.load_ivf_indexes("ivpq", "ivpq_idx");
        assert!(
            matches!(segments.as_slice(), [index::ivf::IvfSegment::Product(_)]),
            "expected one PQ segment"
        );
        let q = "SELECT id FROM ivpq SEARCH MATCH VECTOR \
                 (v, [0.0,0.0,0.0,0.95], 'float', 'l2', 1) WITH (nprobe = 16);";
        assert_eq!(rows(&e, q), vec![vec!["5".to_string()]]);
    }

    /// `storage_type = scalar_quantization` writes the C++ SQ chunk layout and
    /// searches it (residual codes + `dot_v_e`), instead of silently mirroring
    /// the index as plain storage.
    #[test]
    fn a_scalar_quantized_ivf_index_mirrors_and_serves_candidates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ivsq (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute(
            "INSERT INTO ivsq VALUES (1,[0.0,0.0]),(2,[1.0,0.0]),\
             (3,[0.0,1.0]),(4,[2.0,2.0]);",
        )
        .unwrap();
        e.execute(
            "CREATE INDEX ivsq_idx ON ivsq (v) USING IVF WITH \
             (metric = l2, storage_type = scalar_quantization, \
             scalar_quantization_bits = 8);",
        )
        .unwrap();
        e.flush().unwrap();
        // The mirror is a scalar-quantized chunk, not a plain one.
        let segments = e.load_ivf_indexes("ivsq", "ivsq_idx");
        assert!(
            matches!(segments.as_slice(), [index::ivf::IvfSegment::Scalar(_)]),
            "expected one SQ segment"
        );
        // Candidates come from the quantized codes; the executor then scores
        // them exactly, so the top-1 is the same as an exhaustive scan.
        let q = "SELECT id FROM ivsq SEARCH MATCH VECTOR \
                 (v, [0.9,0.1], 'float', 'l2', 1) WITH (nprobe = 16);";
        assert_eq!(rows(&e, q), vec![vec!["2".to_string()]]);
    }

    /// A sealed multi-vector chunk ranks with its persisted residual codes
    /// (EMVB stage 2) before the exact MaxSim re-score.
    #[test]
    fn multivector_prefilter_uses_the_persisted_residual_codes() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mvp (id INT, v TENSOR(FLOAT,2));")
            .unwrap();
        e.execute(
            "CREATE INDEX mvp_i ON mvp (v) USING EMVB \
             WITH (pq_subspace_num = 2, pq_subspace_bits = 8);",
        )
        .unwrap();
        let mut sql = String::from("INSERT INTO mvp VALUES ");
        for i in 0..300 {
            sql.push_str(&format!("({i}, [{i}.0, 0.0])"));
            if i < 299 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        e.dump_all_memory_indexes();
        let mv = e
            .tables
            .lock()
            .unwrap()
            .values()
            .find_map(|t| t.mv.values().next().cloned())
            .unwrap();
        let has_codes = {
            let idx = mv.read().unwrap();
            idx.sealed
                .first()
                .map(|(_, _, pq)| pq.is_some())
                .unwrap_or(false)
        };
        assert!(has_codes, "the sealed chunk carries residual codes");
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM mvp SEARCH MATCH TENSOR \
                 (v, [299.0, 0.0], 'float', 'maxsim', 'topn=1');"
            ),
            vec![vec!["299".to_string()]]
        );
    }

    /// Multi-field WAND: each field keeps its own BM25 statistics and the
    /// scores sum, matching the reference's per-field `BM25Ranker`.
    #[test]
    fn wand_scores_multiple_fields_separately() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ftm (id INT, title VARCHAR, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO ftm VALUES (1,'alpha','beta'),(2,'alpha alpha',''),\
             (3,'beta','alpha');",
        )
        .unwrap();
        e.execute("CREATE INDEX ftm_t ON ftm(title) USING FULLTEXT;")
            .unwrap();
        e.execute("CREATE INDEX ftm_b ON ftm(body) USING FULLTEXT;")
            .unwrap();
        let got = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM ftm SEARCH MATCH TEXT \
             ('title^2,body','alpha beta','topn=10');",
        );
        let expected = [(3, 2.9990f64), (1, 1.8614), (2, 1.1332)];
        assert_eq!(got.len(), expected.len(), "{got:?}");
        for (row, (id, score)) in got.iter().zip(expected) {
            assert_eq!(row[0], id.to_string());
            let value: f64 = row[1].parse().unwrap();
            assert!((value - score).abs() < 1e-3, "{row:?} vs {score}");
        }
        // Boolean similarity sums the per-field points.
        assert_eq!(
            rows(
                &e,
                "SELECT id, ROUND(SCORE(),4) FROM ftm SEARCH MATCH TEXT \
                 ('title^2,body','alpha beta','topn=10;similarity=boolean');"
            ),
            vec![
                vec!["1".to_string(), "3.000000".to_string()],
                vec!["3".to_string(), "3.000000".to_string()],
                vec!["2".to_string(), "2.000000".to_string()],
            ]
        );
    }

    /// A column-less field-qualified query must score exactly like the same
    /// query written against a named column: `SearchDriver` builds the query
    /// tree and the BM25/idf/tf/document-length formula is shared.
    #[test]
    fn field_group_match_scores_like_the_named_column() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE fg (id INT, title VARCHAR, body VARCHAR);")
            .unwrap();
        e.execute(
            "INSERT INTO fg VALUES (1,'alpha beta','gamma'),(2,'alpha','beta gamma'),\
             (3,'beta','alpha'),(4,'gamma','alpha beta');",
        )
        .unwrap();
        e.execute("CREATE INDEX fg_t ON fg(title) USING FULLTEXT;")
            .unwrap();
        e.execute("CREATE INDEX fg_b ON fg(body) USING FULLTEXT;")
            .unwrap();

        // OR of two terms.
        let named = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('title','alpha beta','topn=10');",
        );
        let grouped = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('','(title:alpha title:beta)','topn=10');",
        );
        assert_eq!(grouped, named);
        assert_eq!(named.len(), 3);

        // Phrase.
        let named = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('body','\"alpha beta\"','topn=10');",
        );
        let grouped = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('','(body:\"alpha beta\")','topn=10');",
        );
        assert_eq!(grouped, named);
        assert_eq!(named.len(), 1);

        // Term boost.
        let named = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('title','alpha^3 beta','topn=10');",
        );
        let grouped = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('','(title:alpha)^3 title:beta','topn=10');",
        );
        assert_eq!(grouped, named);
        assert!(!named.is_empty());
        // The boost really changed the ranking/scores.
        let plain = rows(
            &e,
            "SELECT id, ROUND(SCORE(),6) FROM fg SEARCH MATCH TEXT \
             ('','(title:alpha title:beta)','topn=10');",
        );
        assert_ne!(named, plain);
    }

    /// `SELECT id, name` reads only those column files from the C++ block
    /// mirror, and only while the mirror matches the row store.
    #[test]
    fn projection_reads_columns_from_the_block_mirror() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE p (id INT, name VARCHAR, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute(
            "INSERT INTO p VALUES (1,'a',[0.0,0.0]),(2,'b',[1.0,0.0]),\
             (3,'c',[0.0,1.0]);",
        )
        .unwrap();
        e.flush().unwrap();
        assert_eq!(e.mirror_block_row_count("p").unwrap(), 3);
        let items = [
            SelectItem::Column("id".to_string()),
            SelectItem::Column("name".to_string()),
        ];
        let projected = e.try_projection("p", &items).unwrap();
        assert!(projected.is_some());
        assert_eq!(
            rows(&e, "SELECT id, name FROM p;"),
            vec![
                vec!["1".to_string(), "a".to_string()],
                vec!["2".to_string(), "b".to_string()],
                vec!["3".to_string(), "c".to_string()],
            ]
        );
        // A row written after the flush is not in the block snapshot: the
        // projection overlays it from the row journal, so the new row is
        // visible without waiting for the next checkpoint.
        e.execute("INSERT INTO p VALUES (4,'d',[1.0,1.0]);")
            .unwrap();
        let projected = e.try_projection("p", &items).unwrap().unwrap();
        assert_eq!(projected.len(), 4);
        assert_eq!(projected[3][0], Value::Int(4));
        assert_eq!(projected[3][1], Value::Str("d".into()));
        assert_eq!(rows(&e, "SELECT id, name FROM p;").len(), 4);
    }

    /// Wide vector-like cells are kept in an append-only sidecar; the KV row
    /// holds only a short reference, and reads (including after restart)
    /// reconstruct the original value.
    /// A checkpoint rewrites the block it wrote before: every rewrite must carry
    /// every row the file already holds, so a table that is inserted into and
    /// flushed repeatedly never loses a row.
    #[test]
    fn rewriting_a_block_keeps_every_row() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rw (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        for i in 0..20i64 {
            e.execute(&format!(
                "INSERT INTO rw VALUES ({}, [1.0,0.0,0.0,0.0]), ({}, [0.0,1.0,0.0,0.0]);",
                i * 2,
                i * 2 + 1
            ))
            .unwrap();
            assert_eq!(e.row_count("rw").unwrap(), (i as usize + 1) * 2);
            e.flush().unwrap();
            assert_eq!(
                e.row_count("rw").unwrap(),
                (i as usize + 1) * 2,
                "a checkpoint lost a row at batch {i}"
            );
        }
    }

    /// A multi-vector column is stored in the block like any other: its slots
    /// point into the column's outline, and the reader walks that outline back.
    /// A directory written this way has to answer after a restart without the
    /// row ever existing as text.
    #[test]
    fn a_multivector_table_round_trips_through_its_blocks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE mv (v MULTIVECTOR(FLOAT,2));")
            .unwrap();
        e.execute("INSERT INTO mv VALUES ([[1,2],[3,4]]);").unwrap();
        assert_eq!(e.row_count("mv").unwrap(), 1);
        e.flush().unwrap();
        assert_eq!(e.row_count("mv").unwrap(), 1);
        drop(e);

        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(e.row_count("mv").unwrap(), 1);
        // The SDK reads one column (`output([...])`), which is the projection
        // path - not the same code as a full `SELECT v`.
        let items = [SelectItem::Column("v".to_string())];
        let projected = e.try_projection("mv", &items).unwrap();
        assert_eq!(
            projected.map(|r| r.len()),
            Some(1),
            "projection lost the row"
        );
        assert_eq!(
            rows(&e, "SELECT v FROM mv;"),
            vec![vec!["[[1,2],[3,4]]".to_string()]]
        );
    }

    #[test]
    fn wide_vectors_stay_inline_and_survive_a_reopen() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sb (id INT, v EMBEDDING(FLOAT,64), s SPARSE(FLOAT,64));")
            .unwrap();
        let vector = (0..64)
            .map(|i| format!("{i}.0"))
            .collect::<Vec<_>>()
            .join(",");
        let sparse = (0..20)
            .map(|i| format!("{i}:1.0"))
            .collect::<Vec<_>>()
            .join(",");
        e.execute(&format!(
            "INSERT INTO sb VALUES (1, [{vector}], [{sparse}]);"
        ))
        .unwrap();
        // The block is where the row lives, and its stored text form is inline:
        // the port keeps no row sidecar, so nothing of the row lands on disk
        // outside the block files.
        let types = e.column_types("default_db.sb").unwrap();
        let row = e.fetch_rows_with_ids("default_db.sb").unwrap()[0].1.clone();
        let literals: Vec<parser::Literal> = row
            .iter()
            .map(|v| parser::Literal::Str(v.to_slt_string()))
            .collect();
        let raw = e
            .encode_row_stored("default_db.sb", &literals, &types)
            .unwrap();
        assert!(raw.len() > 200, "row is only {} bytes", raw.len());
        assert!(
            !raw.as_bytes().contains(&1u8),
            "no cell is offloaded: {raw}"
        );
        // No row payload outside the blocks: the reference has no `rowdata/`
        // tree and this engine keeps no sidecar either.
        assert!(!dir.path().join("rowdata").exists());
        assert!(!dir.path().join("persistence").exists());

        let vector = rows(&e, "SELECT v FROM sb;");
        assert_eq!(vector.len(), 1);
        assert!(vector[0][0].starts_with("[0"), "{:?}", vector[0]);
        assert!(vector[0][0].contains("63"));

        e.flush().unwrap();
        drop(e);
        let e = Engine::open(dir.path()).unwrap();
        let vector = rows(&e, "SELECT v FROM sb;");
        assert!(vector[0][0].starts_with("[0"), "{:?}", vector[0]);
        assert_eq!(rows(&e, "SELECT id FROM sb;"), vec![vec!["1".to_string()]]);
    }

    /// VARCHAR secondary indexes seal hashed keys into PGM chunks and keep only
    /// the tail in memory, like the reference's `ConvertToOrderedKeyValue`.
    #[test]
    fn string_secondary_index_seals_hashed_chunks() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ss (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO ss VALUES (1,'a'),(2,'b');").unwrap();
        e.execute("CREATE INDEX ss_n ON ss (name);").unwrap();
        let q = "SELECT id FROM ss WHERE name = 'b';";
        assert_eq!(rows(&e, q), vec![vec!["2".to_string()]]);
        e.execute("INSERT INTO ss VALUES (3,'b');").unwrap();
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert!(idx.key_count() > 0);
        drop(tables);
        // A dump transaction drains the tail; a checkpoint keeps it.
        e.dump_index_transaction("ss", "ss_n").unwrap();
        let tables = e.tables.lock().unwrap();
        let idx = tables
            .values()
            .find_map(|t| t.secondary.indexes.values().next())
            .unwrap();
        assert_eq!(idx.key_count(), 0);
        drop(tables);
        assert_eq!(
            rows(&e, q),
            vec![vec!["2".to_string()], vec!["3".to_string()]]
        );
        e.execute("DELETE FROM ss WHERE id = 2;").unwrap();
        assert_eq!(rows(&e, q), vec![vec!["3".to_string()]]);
    }

    /// Port of the reference's `ConflictCheckTest.conflict_check_delete`: three
    /// transactions delete the same row, the first commit wins and the other
    /// two get `kTxnConflict`; overlapping sets behave the same way.
    #[test]
    fn delete_conflicts_follow_the_reference() {
        fn stale_delete(e: &Engine) -> Txn {
            // `commit_with_deletes` stages the tombstones; the transaction only
            // has to be open, exactly as `NewTxn::Delete` begins it.
            e.txn.begin()
        }
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE dc (v INT);").unwrap();
        e.execute("INSERT INTO dc VALUES (0),(1),(2),(3),(4),(5),(6);")
            .unwrap();
        let table = "default_db.dc";
        let seqs: Vec<usize> = e
            .fetch_rows_with_ids(table)
            .unwrap()
            .iter()
            .map(|(seq, _)| *seq)
            .collect();

        let t1 = stale_delete(&e);
        let t2 = stale_delete(&e);
        let t3 = stale_delete(&e);
        e.commit_with_deletes(table, &[seqs[0]], 0, &[], t1)
            .unwrap();
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[0]], 0, &[], t2),
            Err(Error::TxnConflict(_))
        ));
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[0]], 0, &[], t3),
            Err(Error::TxnConflict(_))
        ));
        assert_eq!(e.row_count("dc").unwrap(), 6);

        // A transaction that starts after the winning commit is not stale.
        let t4 = stale_delete(&e);
        e.commit_with_deletes(table, &[seqs[1]], 0, &[], t4)
            .unwrap();
        assert_eq!(e.row_count("dc").unwrap(), 5);

        // Overlapping delete sets: the later committer conflicts on the shared
        // row even though it has its own disjoint write.
        let a = stale_delete(&e);
        let b = stale_delete(&e);
        e.commit_with_deletes(table, &[seqs[2], seqs[3]], 0, &[], a)
            .unwrap();
        assert!(matches!(
            e.commit_with_deletes(table, &[seqs[2]], 0, &[], b),
            Err(Error::TxnConflict(_))
        ));
        assert_eq!(e.row_count("dc").unwrap(), 3);
    }

    #[test]
    fn defaults_and_alter_add_column() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (a INT, b INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1, 2);").unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (c VARCHAR DEFAULT 'default');")
            .unwrap();
        e.execute("ALTER TABLE t ADD COLUMN (d INT);").unwrap();
        // duplicate column must be rejected
        assert!(e.execute("ALTER TABLE t ADD COLUMN (b INT);").is_err());
        // explicit column list leaves c/d at their defaults
        e.execute("INSERT INTO t(a, b) VALUES (2, 2);").unwrap();
        e.execute("INSERT INTO t VALUES (3, 2, 'hello', 4);")
            .unwrap();
        match &e.execute("SELECT * FROM t;").unwrap()[0] {
            QueryResult::Rows { rows, columns } => {
                assert_eq!(columns.len(), 4);
                assert_eq!(row_text(rows, 0), vec!["1", "2", "default", "null"]);
                assert_eq!(row_text(rows, 1), vec!["2", "2", "default", "null"]);
                assert_eq!(row_text(rows, 2), vec!["3", "2", "hello", "4"]);
            }
            other => panic!("{other:?}"),
        }
    }

    #[test]
    fn f16_round_trip_is_nearest_even() {
        // Exact halves round to even; ordinary values round to the nearest f16.
        assert_eq!(round_f16(1.0), 1.0);
        assert_eq!(round_f16(0.5), 0.5);
        // 0.1 is not representable in f16; the nearest value is 0.0999755859375.
        assert!((round_f16(0.1) - 0.099_975_586).abs() < 1e-9);
        // 65504 is the largest finite f16; beyond it rounds to infinity.
        assert_eq!(round_f16(65504.0), 65504.0);
        assert!(round_f16(1.0e30).is_infinite());
    }

    #[test]
    fn bf16_truncates_low_bits() {
        // bfloat16 keeps the top 16 bits of an f32 by truncation.
        assert_eq!(round_bf16(1.0).to_bits(), 0x3f80_0000);
        assert_eq!(round_bf16(0.1).to_bits(), 0.1f32.to_bits() & 0xffff_0000);
        // The truncated value is slightly below the rounded-to-nearest value.
        assert!(round_bf16(0.1) < 0.1f32);
    }

    #[test]
    fn tensor_maxsim_quantizes_by_element_type() {
        // Two query vectors against two doc vectors; MaxSim sums the per-query
        // row maxima. A float16 doc rounds each stored value before the dot.
        let q = vec![vec![0.0, -10.0, 0.0, 0.7], vec![9.2, 45.6, -55.8, 3.5]];
        let d = vec![vec![0.1, 0.2, 0.3, -0.2], vec![0.3, -0.2, 0.2, 0.3]];
        let f32_score = tensor_maxsim(&q, &d, ElemType::Float32, ElemType::Float32);
        assert!((f32_score - (-5.19)).abs() < 1e-6, "got {f32_score}");
        let f16_score = tensor_maxsim(&q, &d, ElemType::Float16, ElemType::Float32);
        assert!((f16_score - (-5.1955)).abs() < 1e-3, "got {f16_score}");
    }

    #[test]
    fn bit_tensor_scores_over_set_bits() {
        // `bit` columns contribute the query component where the doc bit is set.
        let q = vec![vec![0.0, -10.0, 0.0, 0.7]];
        let d = vec![vec![1.0, 0.0, 0.0, 0.0]];
        let s = tensor_maxsim(&q, &d, ElemType::Bit, ElemType::Float32);
        assert!((s - 0.0).abs() < 1e-9, "got {s}");
        let d2 = vec![vec![0.0, 1.0, 0.0, 0.0]];
        let s2 = tensor_maxsim(&q, &d2, ElemType::Bit, ElemType::Float32);
        assert!((s2 - (-10.0)).abs() < 1e-9, "got {s2}");
    }

    #[test]
    fn l2_distance_is_squared() {
        // `l2` reports the squared Euclidean distance (integer and float alike).
        let (_, d) = vector_score(&[0.0, 0.0, 0.0], &[2.0, 3.0, 4.0], "l2");
        assert!((d - 29.0).abs() < 1e-9, "got {d}");
        // ip is a plain dot product.
        let (s, _) = vector_score(&[3.0, 2.0, 1.0], &[10.0, 11.0, 12.0], "ip");
        assert!((s - 64.0).abs() < 1e-9, "got {s}");
        // hamming counts differing positions.
        let (_, h) = vector_score(&[1.0, 0.0, 1.0], &[1.0, 1.0, 1.0], "hamming");
        assert!((h - 1.0).abs() < 1e-9, "got {h}");
    }

    #[test]
    fn element_type_detection_is_space_insensitive() {
        assert_eq!(elem_type_of("UNSIGNED TINYINT"), ElemType::UInt8);
        assert_eq!(elem_type_of("UNSIGNEDTINYINT"), ElemType::UInt8);
        assert_eq!(
            tensor_elem_type("EMBEDDING(UNSIGNEDTINYINT,3)"),
            ElemType::UInt8
        );
        assert_eq!(tensor_elem_type("TENSOR(FLOAT16, 4)"), ElemType::Float16);
    }

    #[test]
    fn databases_are_namespaced() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE DATABASE db1;").unwrap();
        // a duplicate without IF NOT EXISTS is an error
        assert!(e.execute("CREATE DATABASE db1;").is_err());
        e.execute("CREATE DATABASE IF NOT EXISTS db1;").unwrap();
        e.execute("USE db1;").unwrap();
        e.execute("CREATE TABLE t (a INT);").unwrap();
        e.execute("INSERT INTO t VALUES (1);").unwrap();
        e.execute("USE default_db;").unwrap();
        e.execute("CREATE TABLE t (a INT);").unwrap();
        e.execute("INSERT INTO t VALUES (2);").unwrap();
        let rows = |sql: &str| match &e.execute(sql).unwrap()[0] {
            QueryResult::Rows { .. } => e.execute(sql).unwrap()[0].rows_text(),
            other => panic!("{other:?}"),
        };
        assert_eq!(rows("SELECT * FROM t;"), vec![vec!["2".to_string()]]);
        assert_eq!(rows("SELECT * FROM db1.t;"), vec![vec!["1".to_string()]]);
        // dropping the database removes its tables
        e.execute("DROP DATABASE db1;").unwrap();
        assert!(e.execute("SELECT * FROM db1.t;").is_err());
        assert_eq!(rows("SELECT * FROM t;"), vec![vec!["2".to_string()]]);
    }

    #[test]
    fn tensor_dimension_validation() {
        let t = "TENSOR(FLOAT,16)";
        // a flat literal must be a whole multiple of the dimension
        assert!(validate_tensor_dimension(t, "[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]").is_ok());
        assert!(validate_tensor_dimension(t, "[1,2,3]").is_err());
        // nested literals must have every embedding of exactly `dim`
        assert!(validate_tensor_dimension(
            t,
            "[[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]]"
        )
        .is_ok());
        // a TENSORARRAY must be triple-nested and every leaf the declared size
        let ta = "TENSORARRAY(BIT,8)";
        assert!(
            validate_tensor_dimension(ta, "[[[0,1,2,3,0,5,6,7],[8,9,0,11,12,0,14,15]]]").is_ok()
        );
        assert!(
            validate_tensor_dimension(ta, "[[[1,2,3,4,5,6,7,8],[9,10,11,12,13,14,15]]]").is_err()
        );
        // scalars / NULL padding are not dimension-checked
        assert!(validate_tensor_dimension(t, "null").is_ok());
        assert!(validate_tensor_dimension("VARCHAR", "'no tensor'").is_ok());
    }

    /// `SHOW TABLE … COLUMNS` prints what the reference's
    /// `DataType::ToString()` prints: `Sparse(<elem>,<index>,<dimension>)` with
    /// the element and index names of `EmbeddingDataType` and the dimension the
    /// column was declared with. The index width follows the bound for the
    /// two-part spelling and the declaration for the three-part one. Measured
    /// on the reference: `SPARSE(FLOAT,8)` → `Sparse(float,int8,8)`,
    /// `SPARSE(FLOAT,30000)` → `Sparse(float,int16,30000)`, and the SDK's
    /// `sparse,8,float,int` (which the server renders as
    /// `SPARSE(float,int32,8)`) → `Sparse(float,int32,8)`.
    #[test]
    fn a_sparse_column_renders_like_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute(
            "CREATE TABLE sp_render (id INT, a SPARSE(FLOAT, 8), \
             b SPARSE(FLOAT, INT32, 8), c SPARSE(FLOAT, 30000));",
        )
        .unwrap();
        let got = rows(&e, "SHOW TABLE sp_render COLUMNS;");
        assert_eq!(got[1][1], "Sparse(float,int8,8)");
        assert_eq!(got[2][1], "Sparse(float,int32,8)");
        assert_eq!(got[3][1], "Sparse(float,int16,30000)");

        // An unknown bound is the SDK's `sparse,0,…` spelling; the reference
        // prints the same text for it.
        crate::with_zero_dimension_sparse(|| {
            e.execute("CREATE TABLE sp_zero (id INT, s SPARSE(FLOAT, 0));")
        })
        .unwrap();
        let got = rows(&e, "SHOW TABLE sp_zero COLUMNS;");
        assert_eq!(got[1][1], "Sparse(float,int8,0)");

        // The rendering survives a restart, which reads the type back from the
        // catalog record (`type_text_of`).
        e.flush().unwrap();
        let e = crate::new_catalog_ut::reopen(e, dir.path());
        let got = rows(&e, "SHOW TABLE sp_render COLUMNS;");
        assert_eq!(got[1][1], "Sparse(float,int8,8)");
        assert_eq!(got[2][1], "Sparse(float,int32,8)");
        assert_eq!(got[3][1], "Sparse(float,int16,30000)");
    }

    #[test]
    fn describe_type_names() {
        assert_eq!(describe_type("INT"), "Integer");
        assert_eq!(describe_type("ARRAY(VARCHAR)"), "Array(Varchar)");
        assert_eq!(
            describe_type("EMBEDDING(FLOAT,1024)"),
            "Embedding(float,1024)"
        );
        assert_eq!(
            describe_type("ARRAY(EMBEDDING(FLOAT,1024))"),
            "Array(Embedding(float,1024))"
        );
        assert_eq!(
            describe_type("SPARSE(INT,30000)"),
            "Sparse(int32,int16,30000)"
        );
        assert_eq!(
            describe_type("SPARSE(FLOAT,10000000000)"),
            "Sparse(float,int64,10000000000)"
        );
        // The three-field spelling keeps the *declared* index type; the
        // reference prints the same text for a `sparse,8,float,int` column
        // ("Sparse(float,int32,8)"), and the type text is part of the
        // catalogue the two engines exchange.
        assert_eq!(
            describe_type("SPARSE(FLOAT,INT32,8)"),
            "Sparse(float,int32,8)"
        );
        assert_eq!(
            describe_type("SPARSE(FLOAT,INT8,8)"),
            "Sparse(float,int8,8)"
        );
        // The reference's spelling of every plain type.
        for (text, expected) in [
            ("BOOLEAN", "Boolean"),
            ("TINYINT", "TinyInt"),
            ("SMALLINT", "SmallInt"),
            ("INTEGER", "Integer"),
            ("BIGINT", "BigInt"),
            ("HUGEINT", "HugeInt"),
            ("FLOAT", "Float"),
            ("DOUBLE", "Double"),
            ("FLOAT16", "Float16"),
            ("BFLOAT16", "BFloat16"),
            ("VARCHAR", "Varchar"),
            ("DATE", "Date"),
            ("TIME", "Time"),
            ("DATETIME", "DateTime"),
            ("TIMESTAMP", "Timestamp"),
            ("INTERVAL", "Interval"),
            ("JSON", "Json"),
        ] {
            assert_eq!(describe_type(text), expected, "{text}");
        }
    }

    #[test]
    fn column_default_rendering() {
        assert_eq!(render_column_default(None, "INT"), "Null");
        assert_eq!(render_column_default(Some(&"0".to_string()), "INT"), "0");
        // an empty sparse default prints as `(empty)`
        assert_eq!(
            render_column_default(Some(&"[]".to_string()), "SPARSE(FLOAT,30000)"),
            "(empty)"
        );
        // the C++ renderer prints the value before the index for float sparses
        assert_eq!(
            render_column_default(Some(&"[0:0.0]".to_string()), "SPARSE(FLOAT,30000)"),
            "[0.000000:0]"
        );
        assert_eq!(
            render_column_default(Some(&"[0]".to_string()), "SPARSE(BIT,30000)"),
            "[0]"
        );
    }

    /// Invariant G5 (full-text): the index analyzer emits the word **and** its
    /// stem, while query analysis emits the stem only. Measured against the C++
    /// engine (doc_len 2,4,6,5,4 and doc_len_sum 21 on the five-row probe).
    #[test]
    fn analyzer_emits_word_and_stem_only_on_the_index_side() {
        let none = TextTokenizer::default();
        assert_eq!(
            tokenize_index_text("second text multiple", &none),
            vec!["second", "text", "multiple", "multipl"]
        );
        assert_eq!(
            tokenize_text("second text multiple", &none),
            vec!["second", "text", "multipl"]
        );
    }

    /// Invariant G5: the standard tokenizer is the C++ byte table — `/` unites
    /// (`2012/02/14` -> `20120214`), while `_`, `.` and `,` split.
    #[test]
    fn standard_tokenizer_matches_the_cpp_byte_table() {
        assert_eq!(standard_tokenize("2012/02/14"), vec!["20120214"]);
        assert_eq!(standard_tokenize("x/y"), vec!["xy"]);
        assert_eq!(
            standard_tokenize("www.example.com/path"),
            vec!["www", "example", "compath"]
        );
        assert_eq!(standard_tokenize("a_b"), vec!["a", "b"]);
        assert_eq!(standard_tokenize("1,234"), vec!["1", "234"]);
        assert_eq!(standard_tokenize("a@b#c$d"), vec!["a", "b", "c", "d"]);
    }

    /// Invariant G5: a highlighted cell keeps only the sentences with a hit and
    /// joins them with `" ... "`.
    #[test]
    fn highlight_keeps_hit_sentences_and_marks_terms() {
        let none = TextTokenizer::default();
        let q = tokenize_index_text("anarchism", &none);
        let text = "Anarchism is a philosophy. Cats sleep. Anarchism spread.";
        assert_eq!(
            highlight_text(text, &q, &none.delims),
            "<em>Anarchism</em> is a philosophy.  ... <em>Anarchism</em> spread."
        );
    }

    /// A delimiter family highlights whole tokens, like
    /// `Highlighter::GetASCIIWordHighlight` with that analyzer: a substring that
    /// is not a token of its own is not a match. `Some([])` is the keyword
    /// family (whitespace-split), `Some(chars)` splits on those characters.
    #[test]
    fn a_delimiter_field_highlights_tokens_not_substrings() {
        let q = vec!["cat".to_string()];
        let delims: Option<Vec<char>> = Some(Vec::new());
        // A keyword field splits on whitespace only, so `cat.` is one token and
        // does not equal the query term.
        assert_eq!(
            highlight_text("cat concatenate cat.", &q, &delims),
            "<em>cat</em> concatenate cat."
        );
        let comma: Option<Vec<char>> = Some(vec![',']);
        assert_eq!(
            highlight_text("cat,dog,cat", &q, &comma),
            "<em>cat</em>,dog,<em>cat</em>"
        );
        // A term that only occurs inside a longer token stays unmarked.
        assert_eq!(highlight_text("concatenate", &q, &delims), "concatenate");
    }

    /// `HIGHLIGHT` analyses the document with the **column's** analyzer
    /// (`Highlighter::GetHighlight` takes the analyzer the project operator
    /// fetched for the index), so an `ngram` field is highlighted through its
    /// n-grams: `qui` is a term of the index, while the standard byte scanner
    /// never produces it out of `quick` and the cell would stay unmarked.
    #[test]
    fn highlight_uses_the_columns_analyzer() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE hg (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX hg_idx ON hg (body) USING FULLTEXT WITH (analyzer='ngram-3')")
            .unwrap();
        e.execute("INSERT INTO hg VALUES (1, 'quick brown');")
            .unwrap();
        e.flush().unwrap();
        let out = rows(
            &e,
            "SELECT body HIGHLIGHT body FROM hg SEARCH MATCH TEXT (body, 'qui', 10);",
        );
        assert_eq!(out, vec![vec!["<em>qui</em>ck brown".to_string()]]);
    }

    /// A column indexed with `ik` must be tokenised by the IK analyzer on both
    /// the write and the query side. `index_delims` alone mapped every family
    /// but `keyword`/`whitespace` onto the standard analyzer, so the index
    /// stored one token per Chinese run (the standard byte table lets every
    /// UTF-8 byte through) and no `中华` posting existed.
    #[test]
    fn an_ik_fulltext_index_is_tokenised_by_its_analyzer() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ik_t (doc VARCHAR);").unwrap();
        e.execute("INSERT INTO ik_t VALUES ('我们热爱中华人民共和国');")
            .unwrap();
        e.execute("CREATE INDEX ik_idx ON ik_t (doc) USING FULLTEXT WITH (analyzer='ik');")
            .unwrap();
        let tokenizer = e.index_tokenizer("ik_t", "doc");
        assert_eq!(tokenizer.analyzer.as_deref(), Some("ik"));
        let index = e.text_index_for("ik_t", "doc", tokenizer).unwrap();
        let guard = index.read().unwrap();
        // The analyzer's own words (`我们就热爱` -> 我们/热爱/中华人民共和国);
        // the standard analyzer cannot produce any of them.
        let word = "热爱".to_string();
        assert_eq!(
            guard.merge(std::slice::from_ref(&word)).postings[&word].len(),
            1
        );
        // The whole run is exactly one standard token; the index must not hold
        // it, or the index was built with the standard analyzer.
        let run = "我们热爱中华人民共和国".to_string();
        assert!(guard.merge(std::slice::from_ref(&run)).postings[&run].is_empty());
    }

    /// The `keyword`/`whitespace` families take their delimiter set from the
    /// name, and the name is kept byte for byte: `keyword-,` splits on the
    /// comma while a bare `keyword` keeps the whole value, and `whitespace-AB`
    /// must not be lowercased into splitting on `a`/`b`.
    #[test]
    fn a_keyword_fulltext_index_takes_its_delimiters_from_the_name() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE kw_t (doc VARCHAR);").unwrap();
        e.execute("INSERT INTO kw_t VALUES ('a,b');").unwrap();
        e.execute("CREATE INDEX kw_idx ON kw_t (doc) USING FULLTEXT WITH (analyzer='keyword-,');")
            .unwrap();

        assert_eq!(e.index_delims("kw_t", "doc"), Some(vec![',']));
        let tokenizer = e.index_tokenizer("kw_t", "doc");
        // `keyword-<chars>` is carried by the delimiters, not by a family
        // analyzer, exactly as the reference's `WhitespaceAnalyzer` is.
        assert!(tokenizer.analyzer.is_none());
        let index = e.text_index_for("kw_t", "doc", tokenizer).unwrap();
        let guard = index.read().unwrap();
        for term in ["a", "b"] {
            let term = term.to_string();
            assert_eq!(
                guard.merge(std::slice::from_ref(&term)).postings[&term].len(),
                1,
                "{term}"
            );
        }
        let whole = "a,b".to_string();
        assert!(guard.merge(std::slice::from_ref(&whole)).postings[&whole].is_empty());
        drop(guard);

        // The delimiter bytes are not lowercased on the way in.
        e.execute("CREATE TABLE kw_case (doc VARCHAR);").unwrap();
        e.execute(
            "CREATE INDEX kwc ON kw_case (doc) USING FULLTEXT WITH (analyzer='whitespace-AB');",
        )
        .unwrap();
        assert_eq!(e.index_delims("kw_case", "doc"), Some(vec!['A', 'B']));
    }

    /// DDL accepts exactly the names `AnalyzerPool::GetAnalyzer` constructs:
    /// the family is case sensitive, `ngram` parses a numeric prefix, and
    /// `sparsegram` validates its window.
    #[test]
    fn create_index_validates_the_analyzer_name_like_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        // One table per name: a column carries at most one full-text index.
        for (i, name) in [
            "Chinese",
            "ngram-0",
            "sparsegram--3",
            "sparsegram-100-5",
            "bogus",
        ]
        .iter()
        .enumerate()
        {
            let table = format!("an_bad_{i}");
            e.execute(&format!("CREATE TABLE {table} (doc VARCHAR);"))
                .unwrap();
            let sql = format!(
                "CREATE INDEX bad_{i} ON {table} (doc) USING FULLTEXT WITH (analyzer='{name}');"
            );
            assert!(e.execute(&sql).is_err(), "{name} must be rejected");
        }
        for (i, name) in [
            "ngram-3x",
            "sparsegram-fold",
            "keyword-,",
            "standard-PORTER",
        ]
        .iter()
        .enumerate()
        {
            let table = format!("an_ok_{i}");
            e.execute(&format!("CREATE TABLE {table} (doc VARCHAR);"))
                .unwrap();
            let sql = format!(
                "CREATE INDEX ok_{i} ON {table} (doc) USING FULLTEXT WITH (analyzer='{name}');"
            );
            assert!(e.execute(&sql).is_ok(), "{name} must be accepted");
        }
    }

    /// `alpha` is a real `BmpSearchOptions` field, not a hard-coded constant:
    /// `alpha=0` prunes at the first full block, so the higher-scoring tail is
    /// never scored. C++ `BMPUtil::ParseBmpSearchOptions` +
    /// `BmpAlg::SearchKnn`'s `ub_score * alpha < min_score` break.
    #[test]
    fn match_sparse_alpha_option_reaches_the_bmp_search() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sp (id INT, v SPARSE(FLOAT,100));")
            .unwrap();
        // Block 0 (16 docs) has a loose bound: `10` peaks in one document and
        // `20` in another, so its bound is 2.0 while its best document scores
        // 1.0. Block 1 holds four documents scoring 2.0 on the same query.
        let mut values = String::new();
        for id in 1..=16 {
            let doc = match id {
                1 => "[10:1.0]".to_string(),
                2 => "[20:1.0]".to_string(),
                _ => "[10:0.1]".to_string(),
            };
            values.push_str(&format!("({id}, {doc}),"));
        }
        for id in 17..=20 {
            values.push_str(&format!("({id}, [10:1.0,20:1.0]),"));
        }
        e.execute(&format!(
            "INSERT INTO sp VALUES {};",
            values.trim_end_matches(',')
        ))
        .unwrap();
        let score = |opts: &str| -> f64 {
            rows(
                &e,
                &format!(
                    "SELECT SCORE() FROM sp SEARCH MATCH SPARSE \
                     (v, [10:1.0,20:1.0], 'ip', '{opts}');"
                ),
            )[0][0]
                .parse()
                .unwrap()
        };
        assert_eq!(score("topn=1"), 2.0);
        // `alpha=0` prunes block 1 (bound 0 * 2.0 < the k-th score 1.0), so the
        // exact answer is never scored evidence that the option reached
        // `BmpAlg`/`BmpReader::search` instead of a hard-coded 0.9.
        assert_eq!(score("topn=1;alpha=0"), 1.0);
        // `alpha`/`beta` are parsed; out-of-range values are ignored, and the
        // binder's `tail` name does not touch `use_tail` (the reference's
        // `ParseBmpSearchOptions` reads `use_tail`, which the binder rejects).
        let options = parse_bmp_search_options(Some("alpha=0.5;beta=0.25"), "alpha=0.9;tail=F");
        assert_eq!(
            (options.alpha, options.beta, options.use_tail),
            (0.5, 0.25, true)
        );
        let ignored = parse_bmp_search_options(None, "alpha=7;beta=-1;tail=X");
        assert_eq!(
            (ignored.alpha, ignored.beta, ignored.use_tail),
            (1.0, 1.0, true)
        );
    }

    /// `MATCH SPARSE`'s option whitelist is the reference's
    /// (`alpha`/`beta`/`tail`/`threshold`): anything else is a syntax error.
    #[test]
    fn sparse_rejects_unknown_options() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sp (id INTEGER, v SPARSE(FLOAT, 64))")
            .unwrap();
        e.execute("INSERT INTO sp VALUES (1, [10:1.0]);").unwrap();
        let err = e
            .execute("SELECT id FROM sp SEARCH MATCH SPARSE (v, [10:1.0], 'ip', 'use_tail=F')")
            .unwrap_err();
        let message = err.to_string();
        assert!(
            message.contains("Unsupported optional parameter: use_tail"),
            "{message}"
        );
        // The reference's four names are accepted (`tail` is a no-op there).
        for opts in ["alpha=0.5", "beta=0.5", "tail=F", "threshold=0.1"] {
            assert!(
                e.execute(&format!(
                    "SELECT id FROM sp SEARCH MATCH SPARSE (v, [10:1.0], 'ip', '{opts}')"
                ))
                .is_ok(),
                "{opts}"
            );
        }
    }

    /// A column declared `DEFAULT ''` keeps the empty string, not NULL.
    #[test]
    fn empty_string_default_survives_a_catalog_round_trip() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE t (num INTEGER, c VARCHAR DEFAULT '')")
            .unwrap();
        e.execute("INSERT INTO t (num) VALUES (1)").unwrap();
        let rows = rows(&e, "SELECT * FROM t");
        assert_eq!(rows, vec![vec!["1".to_string(), String::new()]]);
    }

    #[test]
    fn memory_indexer_records_tf_positions_and_length() {
        // C++ `column_inverter_ut.cpp`: tf, positions and the document length
        // come out of one insertion.
        let delims = TextTokenizer::new(Some(vec![' ']), None);
        let mut mem = MemoryIndexer::new();
        assert!(mem.insert(10, "alpha beta alpha", &delims, usize::MAX));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.docs[&10], 3);
        let alpha = mem.postings_of("alpha").unwrap();
        assert_eq!(alpha.list.len(), 1);
        assert_eq!(alpha.list[0].0, 10);
        assert_eq!(alpha.list[0].1, 2);
        assert_eq!(alpha.positions(0), &[0, 2]);

        // Re-indexing a row replaces its length, not the document count.
        assert!(mem.insert(10, "gamma", &delims, usize::MAX));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.docs[&10], 1);
        assert_eq!(mem.total_len, 1);

        // A full segment refuses the next document without changing itself.
        let before = mem.mem_used();
        assert!(!mem.insert(11, "delta", &delims, before));
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.mem_used(), before);
    }

    /// C++: `function/scalar/{plus,minus,subtract,mul,modulo,and,or,not}_functions_ut.cpp`.
    #[test]
    fn arithmetic_and_boolean_operators_match_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE lg (a INTEGER, b INTEGER, x BOOLEAN, y BOOLEAN);")
            .unwrap();
        e.execute("INSERT INTO lg VALUES (7, 3, true, false);")
            .unwrap();
        assert_eq!(
            rows(&e, "SELECT -a, a + b, a - b, a * b, a % b FROM lg;"),
            vec![vec![
                "-7".to_string(),
                "10".to_string(),
                "4".to_string(),
                "21".to_string(),
                "1".to_string(),
            ]]
        );
        // Unary `+` in a predicate.
        assert_eq!(
            rows(&e, "SELECT count(*) FROM lg WHERE +a = 7;"),
            vec![vec!["1".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT x AND y, x OR y, NOT x FROM lg;"),
            vec![vec![
                "false".to_string(),
                "true".to_string(),
                "false".to_string(),
            ]]
        );
    }

    /// C++: `function/scalar/{reverse,substr,like,pow,truncate}_*_ut.cpp`.
    #[test]
    fn string_and_power_functions_match_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sf (s VARCHAR);").unwrap();
        e.execute("INSERT INTO sf VALUES ('Hello');").unwrap();
        assert_eq!(
            rows(&e, "SELECT reverse(s), substr(s, 1, 3) FROM sf;"),
            vec![vec!["olleH".to_string(), "ell".to_string()]]
        );
        // `LIKE` in a predicate: one match and one non-match.
        assert_eq!(
            rows(&e, "SELECT count(*) FROM sf WHERE s LIKE 'He%';"),
            vec![vec!["1".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM sf WHERE s LIKE 'xx%';"),
            vec![vec!["0".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT power(2, 10), trunc(3.14159, 2) FROM sf;"),
            vec![vec!["1024.000000".to_string(), "3.14".to_string()]]
        );
    }

    /// C++: `function/scalar/{century,day,day_of_month,day_of_week,day_of_year,era,extract}_*_ut.cpp`:
    /// the date-component family over a DATE column.
    #[test]
    fn date_component_functions_match_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE dtf (d DATE, ts DATETIME);")
            .unwrap();
        e.execute("INSERT INTO dtf VALUES ('2023-12-25', '2023-12-25 14:30:45');")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT century(d), day(d), dayofmonth(d), dayofweek(d), dayofyear(d), era(d), quarter(d) FROM dtf;"
            ),
            vec![vec![
                "21".to_string(),
                "25".to_string(),
                "25".to_string(),
                "1".to_string(),
                "359".to_string(),
                "1".to_string(),
                "4".to_string(),
            ]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT year(ts), month(ts), quarter(ts), hour(ts), minute(ts), second(ts), weekday(ts), weekofyear(ts) FROM dtf;"
            ),
            vec![vec![
                "2023".to_string(),
                "12".to_string(),
                "4".to_string(),
                "14".to_string(),
                "30".to_string(),
                "45".to_string(),
                "1".to_string(),
                "52".to_string(),
            ]]
        );
        // `EXTRACT(field FROM source)` and `DATE_PART(field, source)`.
        assert_eq!(
            rows(&e, "SELECT EXTRACT(day FROM d) FROM dtf;"),
            vec![vec!["25".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT date_part('day', d) FROM dtf;"),
            vec![vec!["25".to_string()]]
        );
    }

    /// C++: `function/aggregate/{sum,avg,count,min,max,first}_functions_ut.cpp`:
    /// the six aggregates over one column, including NULL handling.
    #[test]
    fn aggregate_functions_match_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ag (b BOOLEAN, t TINYINT);")
            .unwrap();
        e.execute("INSERT INTO ag VALUES (true, 10), (false, -5), (true, 127), (NULL, NULL);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT sum(t), avg(t), count(t), min(t), max(t), first(t) FROM ag;"
            ),
            vec![vec![
                "132".to_string(),
                "44.000000".to_string(),
                "3".to_string(),
                "-5".to_string(),
                "127".to_string(),
                "10".to_string(),
            ]]
        );
        // Booleans: min=false, max=true, first=true.
        assert_eq!(
            rows(&e, "SELECT min(b), max(b), first(b) FROM ag;"),
            vec![vec![
                "false".to_string(),
                "true".to_string(),
                "true".to_string()
            ]]
        );
    }

    /// C++: `function/scalar/{add,div,equals}_functions_ut.cpp`: checked
    /// arithmetic (overflow -> NULL), division by zero -> NULL, and equality.
    #[test]
    fn arithmetic_and_equality_follow_the_reference() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ar (a TINYINT, b TINYINT);")
            .unwrap();
        e.execute("INSERT INTO ar VALUES (50, 50), (100, 100), (-128, -1), (5, 2), (0, 0);")
            .unwrap();

        // `+` on TINYINT (values within range; the i64-wide evaluator does not
        // model per-width checked overflow — recorded in CLASS_MAP).
        assert_eq!(
            rows(&e, "SELECT a + b FROM ar WHERE a = 50;"),
            vec![vec!["100".to_string()]]
        );
        // `=` compares values.
        assert_eq!(
            rows(&e, "SELECT a = b FROM ar WHERE a = 50;"),
            vec![vec!["true".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT a = b FROM ar WHERE a = 5;"),
            vec![vec!["false".to_string()]]
        );
        // The comparison family.
        for (sql, want) in [
            ("SELECT a > b FROM ar WHERE a = 5;", "true"),
            ("SELECT a >= b FROM ar WHERE a = 5;", "true"),
            ("SELECT a < b FROM ar WHERE a = 5;", "false"),
            ("SELECT a <= b FROM ar WHERE a = 5;", "false"),
            ("SELECT a != b FROM ar WHERE a = 5;", "true"),
            ("SELECT a != b FROM ar WHERE a = 50;", "false"),
            ("SELECT a >= b FROM ar WHERE a = 50;", "true"),
            ("SELECT a <= b FROM ar WHERE a = 50;", "true"),
        ] {
            assert_eq!(rows(&e, sql), vec![vec![want.to_string()]], "{sql}");
        }
        // `/` always returns DOUBLE; 0/0 is NULL.
        assert_eq!(
            rows(&e, "SELECT a / b FROM ar WHERE a = 5;"),
            vec![vec!["2.500000".to_string()]]
        );
        assert_eq!(
            rows(&e, "SELECT count(*) FROM ar WHERE a = 0 AND a / b IS NULL;"),
            vec![vec!["1".to_string()]]
        );
    }

    /// C++: `planner/node/logical_{create_table,drop_table,create_index,drop_index,
    /// insert,update,aggregate,project,limit,optimize,compact,flush,check,command,
    /// show}_ut.cpp` — each statement builds a logical node and must execute;
    /// the reference asserts only `HandleQueryResult(query_result)`.
    #[test]
    fn logical_planner_statements_succeed() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tb (col1 INTEGER, col2 VARCHAR);")
            .unwrap();
        for i in 0..200 {
            e.execute(&format!("INSERT INTO tb VALUES ({i}, 'abc');"))
                .unwrap();
        }
        for sql in [
            "select min(col1), max(col1), sum(col1), avg(col1) from tb;",
            "select col1 from tb where col1 > 50 and col1 < 150 and col2 = 'abc';",
            "select col1 from tb limit 100;",
            "select col1 from tb order by col1;",
            "update tb set col1 = 666 where col1 = 100;",
            "create index idx on tb(col1) using secondary;",
            "show databases;",
            "optimize tb;",
            "compact table tb;",
            "flush data;",
            "flush log;",
            "check table tb;",
            "create snapshot tb_snapshot on table tb;",
            "drop index idx;",
        ] {
            e.execute(sql).unwrap_or_else(|err| panic!("{sql}: {err}"));
        }
        e.execute("drop table tb;").unwrap();
    }

    /// C++: `function/scalar/abs_functions_ut.cpp::AbsFunctionsTest/abs_func`:
    /// `ABS` over TinyInt/SmallInt/Integer/BigInt/Float/Double, with NULL
    /// propagation.
    #[test]
    fn abs_function_covers_every_numeric_type() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute(
            "CREATE TABLE av (a TINYINT, b SMALLINT, c INTEGER, d BIGINT, f FLOAT, g DOUBLE);",
        )
        .unwrap();
        e.execute("INSERT INTO av VALUES (-5, -300, -70000, -5000000000, -1.5, -2.5);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT abs(a), abs(b), abs(c), abs(d), abs(f), abs(g) FROM av;"
            ),
            vec![vec![
                "5".to_string(),
                "300".to_string(),
                "70000".to_string(),
                "5000000000".to_string(),
                "1.500000".to_string(),
                "2.500000".to_string(),
            ]]
        );
        // A NULL input yields a NULL output for every width.
        e.execute("INSERT INTO av VALUES (NULL, NULL, NULL, NULL, NULL, NULL);")
            .unwrap();
        assert_eq!(
            rows(
                &e,
                "SELECT count(*) FROM av WHERE abs(a) IS NULL AND abs(b) IS NULL AND abs(c) IS NULL AND abs(d) IS NULL AND abs(f) IS NULL AND abs(g) IS NULL;"
            ),
            vec![vec!["1".to_string()]]
        );
    }

    /// C++: `TestTxnManagerTest.DISABLED_SLOW_test_parallel_ts` — two threads
    /// create and drop the same database; conflicting commits are tolerated.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_test_parallel_ts (run with --ignored)"]
    fn disabled_slow_test_parallel_ts() {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        let mut handles = Vec::new();
        for _ in 0..2 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for _ in 0..1280 {
                    let _ = e.execute("CREATE DATABASE db1;");
                    let _ = e.execute("DROP DATABASE db1;");
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));
    }

    /// C++: `TestTxnManagerTest.DISABLED_SLOW_test_parallel_insert_table` —
    /// concurrent inserts into one table.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_test_parallel_insert_table (run with --ignored)"]
    fn disabled_slow_test_parallel_insert_table() {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE pit (col1 INT, col2 VARCHAR);")
            .unwrap();
        let mut handles = Vec::new();
        for t in 0..2 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for i in 0..600 {
                    e.execute(&format!(
                        "INSERT INTO pit VALUES ({}, 'abc');",
                        t * 1000 + i
                    ))
                    .unwrap();
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert_eq!(e.row_count("pit").unwrap(), 1200);
    }

    /// The engine's lock order is `batch_gate -> tables -> index` for a writer
    /// and `index(read) -> tables` for a query (a query fetches its hit rows
    /// while it still holds the column's read guard — `text_index_for`'s comment
    /// spells the cycle out). The two orders are compatible **only** because no
    /// writer acquires an index lock while it holds `tables`: every site clones
    /// the `Arc` out of the map and drops the guard first. An edit that takes the
    /// lock inside the guard turns the pair into a deadlock that hangs the suite
    /// instead of failing it.
    ///
    /// This drives insert, query, seal (through the row-count trigger, so it runs
    /// on the writer threads) and optimize together, and reports a deadlock as a
    /// failure after a timeout instead of hanging.
    #[test]
    fn concurrent_insert_query_seal_and_optimize_do_not_deadlock() {
        const WRITERS: usize = 3;
        const READERS: usize = 3;
        const PER_WRITER: usize = 12;
        const READS: usize = 12;
        const OPTIMIZES: usize = 12;
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        // Seven rows per chunk, so the writers seal while the readers search.
        e.set_memindex_capacity(7);
        e.execute("CREATE TABLE mix (id INTEGER, body VARCHAR)")
            .unwrap();
        e.execute("CREATE INDEX mix_idx ON mix (body) USING FULLTEXT")
            .unwrap();
        let barrier = std::sync::Arc::new(std::sync::Barrier::new(WRITERS + READERS + 1));
        // `()` results: every send is one finished unit of work.
        let (tx, rx) = std::sync::mpsc::channel::<crate::Result<()>>();
        for w in 0..WRITERS {
            let (e, barrier, tx) = (e.clone(), barrier.clone(), tx.clone());
            std::thread::spawn(move || {
                barrier.wait();
                for i in 0..PER_WRITER {
                    let id = w * 1000 + i;
                    let _ = tx.send(
                        e.execute(&format!(
                            "INSERT INTO mix VALUES ({id}, 'alpha beta row{id}');"
                        ))
                        .map(|_| ()),
                    );
                }
            });
        }
        for _ in 0..READERS {
            let (e, barrier, tx) = (e.clone(), barrier.clone(), tx.clone());
            std::thread::spawn(move || {
                barrier.wait();
                for _ in 0..READS {
                    let _ = tx.send(
                        e.execute("SELECT id FROM mix SEARCH MATCH TEXT (body, 'alpha', 100);")
                            .map(|_| ()),
                    );
                }
            });
        }
        {
            let (e, barrier, tx) = (e.clone(), barrier.clone(), tx.clone());
            std::thread::spawn(move || {
                barrier.wait();
                for _ in 0..OPTIMIZES {
                    // Optimize takes `batch_gate` and then the index's write
                    // lock (the writer's order) while the readers hold the read
                    // guard: exactly the pair a reversed acquisition would hang.
                    let _ = tx.send(e.optimize_fulltext("mix").map(|_| ()));
                }
            });
        }
        drop(tx);
        let expected = WRITERS * PER_WRITER + READERS * READS + OPTIMIZES;
        let deadline = std::time::Instant::now() + std::time::Duration::from_secs(120);
        for i in 0..expected {
            let left = deadline.saturating_duration_since(std::time::Instant::now());
            let unit = rx
                .recv_timeout(left)
                .unwrap_or_else(|_| panic!("deadlock: only {i} of {expected} units finished"));
            unit.unwrap_or_else(|err| panic!("unit {i} failed: {err}"));
        }
        // Every row survived the concurrent seals and merges, and the index can
        // still answer for all of them.
        assert_eq!(
            rows(&e, "SELECT count(*) FROM mix;"),
            vec![vec![(WRITERS * PER_WRITER).to_string()]]
        );
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM mix SEARCH MATCH TEXT (body, 'alpha', 100);"
            )
            .len(),
            WRITERS * PER_WRITER
        );
    }

    /// C++: `TestTxnAppend.DISABLED_SLOW_test_append_append_concurrent` —
    /// concurrent appends into one table. `#[ignore]` mirrors `DISABLED_`.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_test_append_append_concurrent (run with --ignored)"]
    fn disabled_slow_test_append_append_concurrent() {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE cac (id INT, name VARCHAR);")
            .unwrap();
        let mut handles = Vec::new();
        for t in 0..4 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for i in 0..100 {
                    e.execute(&format!(
                        "INSERT INTO cac VALUES ({}, 'abc');",
                        t * 1000 + i
                    ))
                    .unwrap();
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert_eq!(e.row_count("cac").unwrap(), 400);
    }

    /// C++: `ParallelTest.DISABLED_SLOW_ChaosTest` — four threads for
    /// `kRunningTime` seconds mixing delete / vector search / update /
    /// full-text search. `#[ignore]` mirrors `DISABLED_`; set
    /// `INFINITY_CHAOS_SECONDS` to shorten the run.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_ChaosTest (run with --ignored)"]
    fn disabled_slow_chaos_test() {
        let seconds: u64 = std::env::var("INFINITY_CHAOS_SECONDS")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(30);
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute(
            "CREATE TABLE chaos_test (id INT, body VARCHAR, other_vector EMBEDDING(FLOAT,4));",
        )
        .unwrap();
        e.execute("CREATE INDEX body_index ON chaos_test (body) USING FULLTEXT;")
            .unwrap();
        e.execute(
            "CREATE INDEX other_index ON chaos_test (other_vector) USING HNSW WITH (metric = l2);",
        )
        .unwrap();
        for i in 0..100 {
            e.execute(&format!(
                "INSERT INTO chaos_test VALUES ({i}, 'the quick brown fox {i}', [{i}.0,1.0,2.0,3.0]);"
            ))
            .unwrap();
        }
        e.flush().unwrap();
        let start = std::time::Instant::now();
        let mut handles = Vec::new();
        for t in 0..4 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                let mut n = t * 1_000_000;
                while start.elapsed() < std::time::Duration::from_secs(seconds) {
                    n += 1;
                    match n % 4 {
                        0 => {
                            let _ = e.execute(&format!("DELETE FROM chaos_test WHERE id = {};", n % 100));
                        }
                        1 => {
                            let _ = e.execute(
                                "SELECT id FROM chaos_test SEARCH MATCH VECTOR (other_vector, [0.0,1.0,2.0,3.0], 'float', 'l2', 3);",
                            );
                        }
                        2 => {
                            let _ = e.execute(&format!(
                                "UPDATE chaos_test SET body = 'the quick brown fox {}' WHERE id = {};",
                                n,
                                n % 100
                            ));
                        }
                        _ => {
                            let _ = e.execute(
                                "SELECT id FROM chaos_test SEARCH MATCH TEXT ('body', 'quick', 'topn=3');",
                            );
                        }
                    }
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        e.execute("DROP TABLE chaos_test;").unwrap();
        e.flush().unwrap();
    }

    /// C++: `MemoryIndexerTest.DISABLED_SLOW_Memory` — the reference appends
    /// two blocks and checks the postings after the async dump; the port keeps
    /// the same postings assertion and is `#[ignore]` exactly like the
    /// `DISABLED_` case (run with `--ignored`).
    #[test]
    #[ignore = "C++ DISABLED_SLOW_Memory (run with --ignored)"]
    fn disabled_slow_memory() {
        let paragraphs = [
            "A finite-state transducer (FST) is a finite-state machine with two memory tapes, following the terminology for Turing machines: an input tape and an output tape. This contrasts with an ordinary finite-state automaton, which has a single tape. An FST is a type of finite-state automaton (FSA) that maps between two sets of symbols.[1] An FST is more general than an FSA. An FSA defines a formal language by defining a set of accepted strings, while an FST defines a relation between sets of strings.",
            "An FST will read a set of strings on the input tape and generates a set of relations on the output tape. An FST can be thought of as a translator or relater between strings in a set.",
            "In morphological parsing, an example would be inputting a string of letters into the FST, the FST would then output a string of morphemes.",
            "An automaton can be said to recognize a string if we view the content of its tape as input. In other words, the automaton computes a function that maps strings into the set {0,1}. Alternatively, we can say that an automaton generates strings, which means viewing its tape as an output tape. On this view, the automaton generates a formal language, which is a set of strings. The two views of automata are equivalent: the function that the automaton computes is precisely the indicator function of the set of strings it generates. The class of languages generated by finite automata is known as the class of regular languages.",
            "The two tapes of a transducer are typically viewed as an input tape and an output tape. On this view, a transducer is said to transduce (i.e., translate) the contents of its input tape to its output tape, by accepting a string on its input tape and generating another string on its output tape. It may do so nondeterministically and it may produce more than one output for each input string. A transducer may also produce no output for a given input string, in which case it is said to reject the input. In general, a transducer computes a relation between two formal languages.",
        ];
        let mut mem = MemoryIndexer::new();
        for (i, paragraph) in paragraphs.iter().enumerate() {
            assert!(mem.insert(i as u64, paragraph, &TextTokenizer::default(), usize::MAX));
        }
        let expected: [(&str, &[(u64, u32)]); 3] = [
            ("fst", &[(0, 4), (1, 2), (2, 2)]),
            ("automaton", &[(0, 2), (3, 5)]),
            ("transducer", &[(0, 1), (4, 4)]),
        ];
        for (term, want) in expected {
            let got: Vec<(u64, u32)> = mem
                .postings_of(term)
                .unwrap()
                .list
                .iter()
                .map(|(doc, tf, _, _)| (*doc, *tf))
                .collect();
            assert_eq!(got, want, "term {term}");
        }
    }

    /// C++: `MemoryIndexerTest.DISABLED_SLOW_SeekPosition` — 8192 copies of
    /// "A B C" repeated seven times; every document has tf 7 for `a`, `b`, `c`
    /// at positions 0,3,6,…,18.
    #[test]
    #[ignore = "C++ DISABLED_SLOW_SeekPosition (run with --ignored)"]
    fn disabled_slow_seek_position() {
        let paragraph = "A B C A B C A B C A B C A B C A B C A B C";
        let mut mem = MemoryIndexer::new();
        for i in 0..8192u64 {
            assert!(mem.insert(i, paragraph, &TextTokenizer::default(), usize::MAX));
        }
        let a = mem.postings_of("a").unwrap();
        assert_eq!(a.list.len(), 8192);
        for (i, (doc, tf, _, _)) in a.list.iter().enumerate() {
            assert_eq!(*tf, 7, "doc {doc}");
            assert_eq!(a.positions(i), &[0, 3, 6, 9, 12, 15, 18], "doc {doc}");
        }
        for doc in [0usize, 1, 2, 5, 127, 128, 512, 1024, 2048, 4096, 8191] {
            assert_eq!(a.list[doc].0, doc as u64);
            assert_eq!(a.list[doc].1, 7);
        }
    }

    /// C++: `MemoryIndexerTest.Chunk` (`memory_indexer_ut.cpp:191`): the
    /// five FST paragraphs produce the reference postings.
    #[test]
    fn memory_indexer_builds_the_reference_postings() {
        let paragraphs = [
            "A finite-state transducer (FST) is a finite-state machine with two memory tapes, following the terminology for Turing machines: an input tape and an output tape. This contrasts with an ordinary finite-state automaton, which has a single tape. An FST is a type of finite-state automaton (FSA) that maps between two sets of symbols.[1] An FST is more general than an FSA. An FSA defines a formal language by defining a set of accepted strings, while an FST defines a relation between sets of strings.",
            "An FST will read a set of strings on the input tape and generates a set of relations on the output tape. An FST can be thought of as a translator or relater between strings in a set.",
            "In morphological parsing, an example would be inputting a string of letters into the FST, the FST would then output a string of morphemes.",
            "An automaton can be said to recognize a string if we view the content of its tape as input. In other words, the automaton computes a function that maps strings into the set {0,1}. Alternatively, we can say that an automaton generates strings, which means viewing its tape as an output tape. On this view, the automaton generates a formal language, which is a set of strings. The two views of automata are equivalent: the function that the automaton computes is precisely the indicator function of the set of strings it generates. The class of languages generated by finite automata is known as the class of regular languages.",
            "The two tapes of a transducer are typically viewed as an input tape and an output tape. On this view, a transducer is said to transduce (i.e., translate) the contents of its input tape to its output tape, by accepting a string on its input tape and generating another string on its output tape. It may do so nondeterministically and it may produce more than one output for each input string. A transducer may also produce no output for a given input string, in which case it is said to reject the input. In general, a transducer computes a relation between two formal languages.",
        ];
        let mut mem = MemoryIndexer::new();
        for (i, paragraph) in paragraphs.iter().enumerate() {
            assert!(mem.insert(i as u64, paragraph, &TextTokenizer::default(), usize::MAX));
        }
        let expected: [(&str, &[(u64, u32)]); 3] = [
            ("fst", &[(0, 4), (1, 2), (2, 2)]),
            ("automaton", &[(0, 2), (3, 5)]),
            ("transducer", &[(0, 1), (4, 4)]),
        ];
        for (term, want) in expected {
            let got: Vec<(u64, u32)> = mem
                .postings_of(term)
                .unwrap()
                .list
                .iter()
                .map(|(doc, tf, _, _)| (*doc, *tf))
                .collect();
            assert_eq!(got, want, "term {term}");
        }
    }

    #[test]
    fn memory_indexer_delete_keeps_the_postings() {
        let delims = TextTokenizer::new(Some(vec![' ']), None);
        let mut mem = MemoryIndexer::new();
        assert!(mem.insert(7, "alpha", &delims, usize::MAX));
        mem.delete(7);
        assert!(!mem.live.contains(&7));
        // The reference keeps deleted documents until compaction.
        assert_eq!(mem.doc_count(), 1);
        assert_eq!(mem.postings_of("alpha").unwrap().list.len(), 1);
    }

    /// A dump writes only the memory segment; new rows form the next segment
    /// and every row is indexed exactly once (Gate C: rebuilding the whole
    /// graph on every write caused the OOM). A checkpoint does not dump it.
    #[test]
    fn hnsw_dump_writes_only_the_memory_segment() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE hv (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX hv_i ON hv (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut sql = String::from("INSERT INTO hv VALUES ");
        for i in 0..10 {
            sql.push_str(&format!("({i}, [{i}.0, 0.0, 0.0, 0.0])"));
            if i < 9 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        let indexed = |e: &Engine| {
            e.tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|t| t.hnsw_mem.values())
                .map(|m| m.read().map(|m| m.len()).unwrap_or(0))
                .sum::<usize>()
        };
        assert_eq!(indexed(&e), 10);

        e.dump_index_transaction("hv", "hv_i").unwrap();
        assert!(
            e.tables.lock().unwrap().values().all(|t| t
                .hnsw_mem
                .values()
                .all(|m| m.read().map(|m| m.is_empty()).unwrap_or(true))),
            "the dump empties the HNSW memory segment"
        );

        e.execute("INSERT INTO hv VALUES (10, [10.0,0.0,0.0,0.0]), (11, [11.0,0.0,0.0,0.0]);")
            .unwrap();
        assert_eq!(indexed(&e), 2);

        e.dump_index_transaction("hv", "hv_i").unwrap();
        let indexed: usize = e
            .load_vector_indexes("default_db.hv", "hv_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(indexed, 12, "every row is in exactly one dumped chunk");
        assert_eq!(
            rows(
                &e,
                "SELECT id FROM hv SEARCH MATCH VECTOR (v, [11.0,0.0,0.0,0.0], 'float', 'l2', 1);"
            ),
            vec![vec!["11".to_string()]]
        );
    }

    /// C++ `AppendMemIndex`: after a write batch, an HNSW memory segment that
    /// reached `mem_index_capacity` rows is dumped without waiting for a
    /// checkpoint (the row bound that keeps a long import's memory flat).
    #[test]
    fn mem_index_capacity_dumps_the_hnsw_segment_after_the_batch() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(4);
        e.execute("CREATE TABLE hc (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX hc_i ON hc (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut sql = String::from("INSERT INTO hc VALUES ");
        for i in 0..10 {
            sql.push_str(&format!("({i}, [{i}.0, 0.0, 0.0, 0.0])"));
            if i < 9 {
                sql.push(',');
            }
        }
        sql.push(';');
        e.execute(&sql).unwrap();
        let indexed = |e: &Engine| {
            e.tables
                .lock()
                .unwrap()
                .values()
                .flat_map(|t| t.hnsw_mem.values())
                .map(|m| m.read().map(|m| m.len()).unwrap_or(0))
                .sum::<usize>()
        };
        assert_eq!(indexed(&e), 0, "the batch reached the capacity and dumped");

        e.execute("INSERT INTO hc VALUES (10, [10.0,0.0,0.0,0.0]);")
            .unwrap();
        assert_eq!(indexed(&e), 1, "below the capacity the segment stays");
        let dumped: usize = e
            .load_vector_indexes("default_db.hc", "hc_i")
            .iter()
            .map(|seg| seg.labels.len())
            .sum();
        assert_eq!(dumped, 10);
    }

    /// C++ optimize/cleanup: incremental chunks are all needed until a merged
    /// chunk covers every live row; cleanup must not delete them early.
    #[test]
    fn cleanup_keeps_incremental_hnsw_chunks_until_a_merge_covers_the_table() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.set_memindex_capacity(2);
        e.execute("CREATE TABLE hk (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute("CREATE INDEX hk_i ON hk (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        e.execute("INSERT INTO hk VALUES (0,[0.0,0.0]),(1,[1.0,0.0]);")
            .unwrap();
        e.execute("INSERT INTO hk VALUES (2,[2.0,0.0]),(3,[3.0,0.0]);")
            .unwrap();
        let chunks = |e: &Engine| e.load_vector_indexes("default_db.hk", "hk_i");
        assert_eq!(chunks(&e).len(), 2, "one chunk per memory-segment dump");

        e.cleanup().unwrap();
        assert_eq!(chunks(&e).len(), 2, "no chunk covers the whole table yet");

        e.optimize_indexes().unwrap();
        e.cleanup().unwrap();
        let merged = chunks(&e);
        assert_eq!(merged.len(), 1, "the merged chunk supersedes the others");
        assert_eq!(merged[0].labels.len(), 4);
    }

    /// `OptimizeIndexTxnStore::ToWalEntry` and `RemoveChunkIDs`: the optimize
    /// transaction logs one `DumpIndexV2` per entry - the chunk it wrote and the
    /// chunks it replaced - and those chunks stop being read when it commits.
    #[test]
    fn optimize_logs_one_dump_command_per_entry() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE ot (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute("CREATE INDEX ot_i ON ot (v) USING HNSW WITH (metric = l2, encode = plain);")
            .unwrap();
        e.execute("INSERT INTO ot VALUES (0,[0.0,0.0]),(1,[1.0,0.0]);")
            .unwrap();
        e.dump_index_transaction("ot", "ot_i").unwrap();
        e.execute("INSERT INTO ot VALUES (2,[2.0,0.0]),(3,[3.0,0.0]);")
            .unwrap();
        e.dump_index_transaction("ot", "ot_i").unwrap();
        assert_eq!(e.hnsw_chunk_ids("ot", "ot_i"), vec![0, 1]);

        e.optimize_indexes().unwrap();

        // `RemoveChunkIDs(deprecate_ids)` is part of the transaction, so the
        // merged chunk is the only one a reader sees before cleanup runs.
        assert_eq!(e.hnsw_chunk_ids("ot", "ot_i"), vec![2]);
        let chunks = e.load_vector_indexes("ot", "ot_i");
        assert_eq!(chunks.len(), 1);
        assert_eq!(chunks[0].labels.len(), 4);

        // The superseded chunks are out of the chunk list but their bytes are
        // still on disk: the reference's `RemoveChunkIDs` runs inside the
        // commit and `PrepareCommitDumpIndex` leaves one `drop|idx_chunk|…`
        // marker per id; `Cleanup` is what deletes the files.
        let chunk_dir = e.hnsw_dir("ot", "ot_i").unwrap();
        assert!(
            chunk_dir.join("chunk_0.idx").is_file() && chunk_dir.join("chunk_1.idx").is_file(),
            "the merge must not delete the chunks it superseded"
        );
        let store = e.storage.catalog.store().clone();
        let db_id = e.database_id_of("ot").to_string();
        let table_id = e
            .storage
            .catalog
            .get_table_by_name("default_db.ot")
            .unwrap()
            .unwrap()
            .id
            .to_string();
        let index_id = e
            .index_position("default_db.ot", "ot_i")
            .unwrap()
            .to_string();
        assert_eq!(
            catalog::layout::deprecated_index_chunks(
                store.as_ref(),
                &db_id,
                &table_id,
                &index_id,
                0
            )
            .unwrap(),
            vec![0, 1]
        );
        for deprecated in 0..2 {
            let key = storage::catalog_format::catalog_index_chunk_key(
                &db_id, &table_id, &index_id, 0, deprecated,
            );
            assert!(
                store.get(key.as_bytes()).unwrap().is_none(),
                "a deprecated chunk leaves the chunk list"
            );
        }

        // The entry reached the log: one command, naming chunk 2 as the new one
        // and 0/1 as the ones it supersedes.
        let wal = dir.path().join("wal").join("wal.log");
        let records = storage::WalManager::read_all(&wal).unwrap();
        let merged = records
            .iter()
            .flat_map(|entry| entry.cmds.iter())
            .filter_map(|cmd| match cmd {
                storage::WalCmd::DumpIndex {
                    index_name,
                    segment_id,
                    chunk_infos,
                    deprecate_ids,
                    ..
                } if index_name == "ot_i" && deprecate_ids == &[0, 1] => {
                    Some((*segment_id, chunk_infos.clone(), deprecate_ids.clone()))
                }
                _ => None,
            })
            .next()
            .expect("the optimize transaction logs the merged chunk");
        assert_eq!(merged.0, 0);
        assert_eq!(merged.2, vec![0, 1]);
        assert_eq!(merged.1.len(), 1);
        assert_eq!(merged.1[0].chunk_id, 2);
        assert_eq!(merged.1[0].row_count, 4);
        assert_eq!(
            merged.1[0].base_row_id,
            storage::wal_entry::WalRowId::new(0, 0)
        );
        assert_eq!(merged.1[0].deprecate_ts, u64::MAX);

        // `Cleanup` is the half that reclaims the bytes: the files go, the
        // markers go, and only the merged chunk is left.
        e.cleanup().unwrap();
        assert!(
            !chunk_dir.join("chunk_0.idx").exists() && !chunk_dir.join("chunk_1.idx").exists(),
            "cleanup reclaims the bytes"
        );
        assert!(catalog::layout::deprecated_index_chunks(
            store.as_ref(),
            &db_id,
            &table_id,
            &index_id,
            0
        )
        .unwrap()
        .is_empty());
        assert_eq!(e.hnsw_chunk_ids("ot", "ot_i"), vec![2]);
    }

    /// C++ `OptimizeKnnTest.test_hnsw_optimize` (`test_optimize_ut.cpp`):
    /// three memory-index dumps (chunks 0..2) over 24 appended rows; optimize
    /// appends one merged chunk (id 3) covering every live row, cleanup keeps
    /// only it, and the merged graph still answers the top-1 query.
    #[test]
    fn test_hnsw_optimize() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE tb1 (col1 INT, col2 EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX idx1 ON tb1 (col2) USING HNSW WITH (metric = l2, encode = plain);")
            .unwrap();
        for _ in 0..3 {
            for _ in 0..2 {
                e.execute(
                    "INSERT INTO tb1 VALUES \
                     (2,[0.1,0.2,0.3,-0.2]),(4,[0.2,0.1,0.3,0.4]),\
                     (6,[0.3,0.2,0.1,0.4]),(8,[0.4,0.3,0.2,0.1]);",
                )
                .unwrap();
            }
            // DumpMemIndex: one chunk per iteration's memory segment.
            e.dump_index_transaction("tb1", "idx1").unwrap();
        }
        assert_eq!(
            e.hnsw_chunk_ids("default_db.tb1", "idx1"),
            vec![0, 1, 2],
            "one chunk per memory-index dump"
        );

        e.optimize_indexes().unwrap();
        e.cleanup().unwrap();

        assert_eq!(
            e.hnsw_chunk_ids("default_db.tb1", "idx1"),
            vec![3],
            "the merged chunk supersedes the incremental ones"
        );
        let chunks = e.load_vector_indexes("default_db.tb1", "idx1");
        assert_eq!(chunks.len(), 1);
        assert_eq!(chunks[0].chunk_id, 3);
        assert_eq!(chunks[0].index.len(), 24);
        // `base_row_id == RowID(0,0)` and 24 rows: the mirror stores the
        // segment offset of each row as its label.
        assert_eq!(chunks[0].labels, (0..24).collect::<Vec<u32>>());
        // The memory segment carries only the HNSW graph and row ids; there is
        // no secondary index to merge (C++ `GetSecondaryIndex() == nullptr`).
        assert_eq!(
            rows(
                &e,
                "SELECT col1 FROM tb1 SEARCH MATCH VECTOR \
                 (col2, [0.1,0.2,0.3,-0.2], 'float', 'l2', 1);"
            ),
            vec![vec!["2".to_string()]]
        );
    }

    /// Scalar tables keep a decoded row cache (the reference's in-memory
    /// `DataBlock`s); array/vector tables do not, because their decoded form is
    /// not stable across the write and scan paths.
    /// Once the mirror covers every live row, full scans read the values from
    /// the column blocks, and the KV holds no row record at all.
    #[test]
    fn full_rows_come_from_the_block_mirror_when_current() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bm (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO bm VALUES (1,'a'),(2,'b');").unwrap();
        e.flush().unwrap();
        // The scalar row cache would answer first; drop it so the test proves
        // the block path.
        e.invalidate_row_cache("default_db.bm");
        // No row record exists anywhere in the KV: the blocks are the table.
        assert!(e.store().scan_prefix(b"row:").unwrap().is_empty());
        let rows = e.fetch_rows("default_db.bm").unwrap();
        assert_eq!(rows.len(), 2);
        assert_eq!(rows[0][0], Value::Int(1));
        assert_eq!(rows[1][0], Value::Int(2));
    }

    #[test]
    fn block_mirror_covers_rows_after_deletes_and_updates() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE bmu (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO bmu VALUES (1,'a'),(2,'b'),(3,'c');")
            .unwrap();
        e.flush().unwrap();
        e.invalidate_row_cache("default_db.bmu");
        e.execute("DELETE FROM bmu WHERE id = 2;").unwrap();
        e.execute("UPDATE bmu SET name = 'u' WHERE id = 3;")
            .unwrap();
        let rows = e.try_rows("default_db.bmu").unwrap().unwrap();
        let ids: Vec<&Value> = rows.iter().map(|(_, r)| &r[0]).collect();
        let names: Vec<&Value> = rows.iter().map(|(_, r)| &r[1]).collect();
        assert_eq!(ids, vec![&Value::Int(1), &Value::Int(3)]);
        assert_eq!(
            names,
            vec![&Value::Str("a".into()), &Value::Str("u".into())]
        );
        assert_eq!(
            rows.iter().map(|(seq, _)| *seq).collect::<Vec<_>>(),
            vec![0, 3]
        );
    }

    #[test]
    fn scalar_tables_use_the_row_cache() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rc (id INT, name VARCHAR);")
            .unwrap();
        e.execute("INSERT INTO rc VALUES (1,'a'),(2,'b');").unwrap();
        assert_eq!(rows(&e, "SELECT * FROM rc;").len(), 2);
        let key = e.table_key("rc");
        assert!(e
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|t| t.row_cache.as_ref())
            .is_some());
        assert_eq!(
            rows(&e, "SELECT id FROM rc WHERE id = 2;"),
            vec![vec!["2".to_string()]]
        );

        e.execute("CREATE TABLE av (id INT, v EMBEDDING(FLOAT,2));")
            .unwrap();
        e.execute("INSERT INTO av VALUES (1,[0.0,0.0]);").unwrap();
        assert_eq!(rows(&e, "SELECT * FROM av;").len(), 1);
        let key = e.table_key("av");
        assert!(e
            .tables
            .lock()
            .unwrap()
            .get(&key)
            .and_then(|t| t.row_cache.as_ref())
            .is_none());
    }

    /// Row-id allocation creates segments; `SHOW SEGMENTS` reports their ids
    /// and row counts.
    #[test]
    fn segments_track_row_id_allocations() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE sg (id INT);").unwrap();
        e.reserve_rows("sg", 2, true).unwrap();
        e.reserve_rows("sg", 3, true).unwrap();
        assert_eq!(e.segments("sg"), vec![(0, 2), (1, 3)]);
        // An INSERT after a sealed COPY opens a segment; the next one appends.
        e.reserve_rows("sg", 4, false).unwrap();
        e.reserve_rows("sg", 4, false).unwrap();
        assert_eq!(e.segments("sg"), vec![(0, 2), (1, 3), (2, 8)]);
    }

    /// `NewTxn::BeginTS`: a reader pinned to a timestamp does not see a row
    /// committed after it, and does see one deleted after it.
    #[test]
    fn a_read_at_an_older_ts_hides_later_commits() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE rts (id INT);").unwrap();
        e.execute("INSERT INTO rts VALUES (1);").unwrap();
        let pin = e.current_ts();
        e.execute("INSERT INTO rts VALUES (2);").unwrap();

        // Outside a statement the reader is at `u64::MAX`: both rows.
        assert_eq!(e.block_visible_row_count("default_db.rts"), 2);
        assert_eq!(e.fetch_rows("default_db.rts").unwrap().len(), 2);

        {
            let _pinned = ReadTsGuard::enter(pin);
            assert_eq!(e.block_visible_row_count("default_db.rts"), 1);
            let seen = e.fetch_rows("default_db.rts").unwrap();
            assert_eq!(seen.len(), 1, "the second insert committed after `pin`");
        }

        // A delete after the pin is invisible to the pinned reader.
        let deleted_pin = e.current_ts();
        e.execute("DELETE FROM rts WHERE id = 1;").unwrap();
        assert_eq!(e.block_visible_row_count("default_db.rts"), 1);
        {
            let _pinned = ReadTsGuard::enter(deleted_pin);
            assert_eq!(e.block_visible_row_count("default_db.rts"), 2);
            assert_eq!(e.fetch_rows("default_db.rts").unwrap().len(), 2);
        }
    }

    /// Four writers, checkpoints in the middle of the stream, then a restart:
    /// every committed row has to come back.
    ///
    /// This is the regression the "hole in the block" used to hide: a block
    /// written from a buffer that was missing a row in the middle shifted every
    /// later row onto the wrong slot, and the rows at the end were lost. The
    /// ids are handed out in commit-ts order and a checkpoint only writes what
    /// had finished, so a block's ids are a dense prefix.
    #[test]
    fn concurrent_appends_survive_a_checkpoint_and_reopen() {
        let dir = tempdir().unwrap();
        {
            let e = Arc::new(Engine::open(dir.path()).unwrap());
            e.install_engine_handle();
            e.execute("CREATE TABLE ck (id INT);").unwrap();
            let mut handles = Vec::new();
            for t in 0..4 {
                let e = Arc::clone(&e);
                handles.push(std::thread::spawn(move || {
                    for b in 0..10 {
                        let first = t * 1000 + b * 2;
                        e.execute(&format!(
                            "INSERT INTO ck VALUES ({first}), ({});",
                            first + 1
                        ))
                        .unwrap();
                        if b % 3 == 0 {
                            e.flush().unwrap();
                        }
                    }
                }));
            }
            for handle in handles {
                handle.join().unwrap();
            }
            assert_eq!(e.row_count("default_db.ck").unwrap(), 80);
        }
        let e = Engine::open(dir.path()).unwrap();
        assert_eq!(e.row_count("default_db.ck").unwrap(), 80);
        assert_eq!(
            rows(&e, "SELECT count(*) FROM ck;"),
            vec![vec!["80".to_string()]]
        );
    }

    /// Writers and readers share the table/index state; this exercises the
    /// table-lock boundaries introduced for `TableEntry` (a deadlock hangs the
    /// test binary, which the harness timeout catches).
    #[test]
    fn concurrent_inserts_and_vector_searches_do_not_deadlock() {
        let dir = tempdir().unwrap();
        let e = std::sync::Arc::new(Engine::open(dir.path()).unwrap());
        e.execute("CREATE TABLE cc (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute("CREATE INDEX cc_i ON cc (v) USING HNSW WITH (metric = l2);")
            .unwrap();
        let mut handles = Vec::new();
        for t in 0..4 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for b in 0..25 {
                    e.execute(&format!(
                        "INSERT INTO cc VALUES ({}, [1.0,0.0,0.0,0.0]), ({}, [0.0,1.0,0.0,0.0]);",
                        t * 1000 + b * 2,
                        t * 1000 + b * 2 + 1
                    ))
                    .unwrap();
                    if b % 5 == 0 {
                        e.flush().unwrap();
                    }
                }
            }));
        }
        for _ in 0..2 {
            let e = e.clone();
            handles.push(std::thread::spawn(move || {
                for _ in 0..40 {
                    let _ = e.execute(
                        "SELECT id FROM cc SEARCH MATCH VECTOR (v, [1.0,0.0,0.0,0.0], 'float', 'l2', 3);",
                    );
                    let _ = e.execute("SELECT count(*) FROM cc;");
                }
            }));
        }
        for h in handles {
            h.join().unwrap();
        }
        assert_eq!(e.row_count("default_db.cc").unwrap(), 200);
    }

    /// `src/unit_test/main/infinity_ut.cpp::InfinityTest/test1`: the catalog
    /// lifecycle through the SQL path (`ListDatabases` / `CreateDatabase` /
    /// `DropDatabase` / `ShowTables` / insert + select / `Query`).
    #[test]
    fn catalog_lifecycle_matches_the_reference_test() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();

        // `ListDatabases` starts with `default_db` only, and its
        // `db_entry_dir_` is the directory name the startup transaction gave
        // it (`db_1`).
        assert_eq!(
            rows(&e, "SHOW DATABASES;"),
            vec![vec![
                "default_db".to_string(),
                "db_1".to_string(),
                String::new()
            ]]
        );

        e.execute("CREATE DATABASE db1;").unwrap();
        let dbs = rows(&e, "SHOW DATABASES;");
        // The reference lists `db1` first (hash order); the Rust catalog is
        // sorted, so compare the set.
        let mut names: Vec<&str> = dbs.iter().map(|r| r[0].as_str()).collect();
        names.sort_unstable();
        assert_eq!(names, vec!["db1", "default_db"]);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db1"));
        assert!(!e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db2"));
        // Dropping a missing database with the default (`kError`) conflict type
        // fails, while `default_db` stays.
        assert!(e.catalog().drop_database("db2", false).is_err());
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));
        e.catalog().drop_database("db1", false).unwrap();
        assert_eq!(rows(&e, "SHOW DATABASES;").len(), 1);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));

        // Create/Show/Get/Drop a table in `default_db`.
        e.execute("CREATE TABLE default_db.table1 (col1 BOOLEAN, col2 BIGINT);")
            .unwrap();
        let tables = rows(&e, "SHOW TABLES FROM default_db;");
        assert_eq!(tables.len(), 1);
        assert_eq!(tables[0][0], "default_db");
        assert_eq!(tables[0][1], "table1");
        assert_eq!(tables[0][3], "2");
        assert!(e
            .catalog()
            .get_table_by_name("default_db.table1")
            .unwrap()
            .is_some());
        e.execute("DROP TABLE default_db.table1;").unwrap();
        assert!(rows(&e, "SHOW TABLES FROM default_db;").is_empty());
        assert!(e
            .catalog()
            .get_table_by_name("default_db.table1")
            .unwrap()
            .is_none());

        // Insert and select through two columns of different widths.
        e.execute("CREATE TABLE default_db.table1 (col1 BIGINT, col2 SMALLINT);")
            .unwrap();
        e.execute("INSERT INTO default_db.table1 (col1, col2) VALUES (11, 22);")
            .unwrap();
        assert_eq!(
            rows(&e, "SELECT col1, col2 FROM default_db.table1;"),
            vec![vec!["11".to_string(), "22".to_string()]]
        );
        e.execute("DROP TABLE default_db.table1;").unwrap();

        // `Query("create database db1;")` + `Query("show databases;")`.
        e.execute("CREATE DATABASE db1;").unwrap();
        let mut names: Vec<String> = rows(&e, "SHOW DATABASES;")
            .into_iter()
            .map(|r| r[0].clone())
            .collect();
        names.sort();
        assert_eq!(names, vec!["db1", "default_db"]);
        e.execute("DROP DATABASE db1;").unwrap();
        assert_eq!(rows(&e, "SHOW DATABASES;").len(), 1);
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "default_db"));
    }

    /// `src/unit_test/main/infinity_ut.cpp::InfinityTest/test2`: every listed
    /// global/session variable resolves, and `error` is rejected at both scopes
    /// (`Status::NoSysVar`).
    #[test]
    fn show_variable_covers_the_reference_names() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
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
            let r = e.show_variable(name, variables::SetScope::Global).unwrap();
            let QueryResult::Rows { columns, rows } = r else {
                panic!("{name} must return a row")
            };
            assert_eq!(columns, vec!["value".to_string()]);
            assert_eq!(rows.len(), 1);
        }
        for name in [
            "connected_timestamp",
            "total_rollback_count",
            "total_commit_count",
            "query_count",
        ] {
            e.show_variable(name, variables::SetScope::Session).unwrap();
        }
        assert!(e
            .show_variable("error", variables::SetScope::Global)
            .is_err());
        assert!(e
            .show_variable("error", variables::SetScope::Session)
            .is_err());
    }

    /// `src/unit_test/main/table_ut.cpp::InfinityTableTest/test1`: the facade
    /// scenario — create the database, create a table with an
    /// `EMBEDDING(FLOAT, 128)` and a `BIGINT` column, fetch it back, insert
    /// rows, query them, and explain in every plan mode without executing the
    /// inner statement.
    #[test]
    fn explain_covers_every_plan_kind() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE DATABASE db1;").unwrap();
        assert!(e
            .catalog()
            .list_databases()
            .unwrap()
            .iter()
            .any(|d| d == "db1"));
        e.execute("CREATE TABLE db1.knn_table (col1 EMBEDDING(FLOAT, 128), col2 BIGINT);")
            .unwrap();

        // `Infinity::GetDatabase` / `GetTable`: the created table is
        // retrievable with both declared columns.
        let table = e
            .catalog()
            .get_table_by_name("db1.knn_table")
            .unwrap()
            .expect("created table is retrievable");
        assert_eq!(table.columns, vec!["col1".to_string(), "col2".to_string()]);
        assert_eq!(table.types.len(), 2);
        assert!(
            table.types[0].to_lowercase().contains("embedding"),
            "col1 type: {}",
            table.types[0]
        );
        assert!(
            table.types[1].to_lowercase().contains("bigint"),
            "col2 type: {}",
            table.types[1]
        );

        // Insert and read back through the same facade.
        let embedding = vec!["0.5"; 128].join(",");
        for col2 in [42, 7] {
            e.execute(&format!(
                "INSERT INTO db1.knn_table (col1, col2) VALUES ([{embedding}], {col2});"
            ))
            .unwrap();
        }
        assert_eq!(
            rows(&e, "SELECT col2 FROM db1.knn_table ORDER BY col2;"),
            vec![vec!["7".to_string()], vec!["42".to_string()]]
        );

        // The reference asks for AST, unoptimized/optimized logical, physical,
        // fragment and pipeline plans; each must succeed.
        for sql in [
            "EXPLAIN AST SELECT col2 FROM db1.knn_table;",
            "EXPLAIN LOGICAL SELECT col2 FROM db1.knn_table;",
            "EXPLAIN OPTIMIZED SELECT col2 FROM db1.knn_table;",
            "EXPLAIN PHYSICAL SELECT col2 FROM db1.knn_table;",
            "EXPLAIN FRAGMENT SELECT col2 FROM db1.knn_table;",
            "EXPLAIN PIPELINE SELECT col2 FROM db1.knn_table;",
        ] {
            let mut res = e.execute(sql).unwrap_or_else(|err| panic!("{sql}: {err}"));
            assert_eq!(res.len(), 1, "{sql}");
            assert!(matches!(res.remove(0), QueryResult::Rows { .. }), "{sql}");
        }
    }
    /// A `DROP TABLE` that commits the catalog delete and then dies before the
    /// row sweep leaves `row:` keys no catalog entry refers to. A later
    /// `CREATE TABLE` with the same name must clear them: `fetch_rows` walks
    /// `row:{table}:` and `next_row_seq` starts past the highest orphan id, so
    /// without the sweep the new table reads the dropped table's rows back.
    #[test]
    fn a_crashed_drop_does_not_resurrect_rows() {
        let dir = tempdir().unwrap();
        let e = Engine::open(dir.path()).unwrap();
        e.execute("CREATE TABLE crash_t (id INT);").unwrap();
        e.execute("INSERT INTO crash_t VALUES (1), (2), (3);")
            .unwrap();
        assert_eq!(
            rows(&e, "SELECT count(*) FROM crash_t;"),
            vec![vec!["3".to_string()]]
        );

        // The crash window: catalog entry gone, row keys still present.
        e.storage.catalog.drop_table("default_db.crash_t").unwrap();
        e.tables.lock().unwrap().clear();

        assert!(
            e.storage
                .catalog
                .get_table_by_name("default_db.crash_t")
                .unwrap()
                .is_none(),
            "the drop must be visible before the sweep runs"
        );
        e.execute("CREATE TABLE crash_t (id INT);").unwrap();
        assert_eq!(
            rows(&e, "SELECT count(*) FROM crash_t;"),
            vec![vec!["0".to_string()]],
            "the recreated table must not see the dropped table's rows"
        );
        e.execute("INSERT INTO crash_t VALUES (7);").unwrap();
        assert_eq!(
            rows(&e, "SELECT id FROM crash_t;"),
            vec![vec!["7".to_string()]]
        );
    }
}

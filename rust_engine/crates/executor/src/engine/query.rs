use super::*;

impl Engine {
    /// Runs a derived-table subquery (`FROM (SELECT …)`), returning its columns
    /// and rows so the outer query can scan them.
    pub(crate) fn derived_table(&self, sql: &str) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        for r in self.execute(sql)? {
            if let QueryResult::Rows { columns, rows } = r {
                return Ok((columns, rows));
            }
        }
        Ok((Vec::new(), Vec::new()))
    }

    /// Runs a scalar/`EXISTS` subquery, returning typed rows. The SQL text is
    /// re-parsed here exactly like a derived table, but the typed
    /// [`Self::run_select`] path is used so values are not round-tripped
    /// through their `.slt` text. `ctes` are the bindings already visible in
    /// the enclosing query.
    pub(crate) fn run_subquery(
        &self,
        sql: &str,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        let Some(stmt) = parse(sql)?.into_iter().next() else {
            return Ok((Vec::new(), Vec::new()));
        };
        if !matches!(stmt, Statement::Select { .. }) {
            return Err(Error::Invalid("subquery must be a SELECT".into()));
        }
        // A correlated subquery is executed by the dependent join
        // (`dependent.rs`), which substitutes the outer values and caches the
        // inner rows per key. Reaching this point with an outer reference left
        // would silently resolve the name against the inner table instead, so
        // it is an error rather than a wrong answer.
        if self.correlated_sql_columns(sql, outer, ctes)?.is_some() {
            return Err(Error::Invalid(
                "correlated subquery is not supported here (only a single table's WHERE, HAVING or projection)".into(),
            ));
        }
        let visible: Vec<String> = ctes.keys().cloned().collect();
        let stmt = self.qualify_tables_in(&stmt, &visible);
        self.run_select_with_ctes(&stmt, ctes)
    }

    /// C++ `QueryBinder::BindSubquery` + `CorrelatedExpressionsDetector`: the
    /// outer columns `sql` reads, as `(column name, outer schema index)`.
    ///
    /// `None` when the nested query is uncorrelated, or when it is not a
    /// single-table base-table query (a CTE alias, a missing table, a shape the
    /// binder cannot represent) — those stay on the constant-folding path.
    pub(crate) fn correlated_sql_columns(
        &self,
        sql: &str,
        outer: &[String],
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
    ) -> Result<Option<Vec<(String, usize)>>> {
        if outer.is_empty() {
            return Ok(None);
        }
        let Ok(mut statements) = parse(sql) else {
            return Ok(None);
        };
        let Some(Statement::Select {
            items,
            from,
            where_pred,
            ctes: local_ctes,
            ..
        }) = statements.pop()
        else {
            return Ok(None);
        };
        let Some(table) = from else {
            return Ok(None);
        };
        // A CTE alias in FROM is not a base table: binding it against the
        // catalog would read the shadowed table's schema and invent a
        // correlation.
        if ctes.contains_key(&table.to_ascii_lowercase())
            || local_ctes
                .iter()
                .any(|c| c.alias.eq_ignore_ascii_case(&table))
        {
            return Ok(None);
        }
        // The catalog keys tables by `db.table`; an unqualified name in the
        // subquery's FROM has to be resolved the same way.
        let Ok(columns) = self.table_schema(&self.table_key(&table)) else {
            return Ok(None);
        };
        let Ok(bound) = planner::binder::QueryBinder::new().bind_subquery(
            planner::subquery::SubqueryType::Scalar,
            Some(&table),
            &columns,
            &items,
            where_pred.as_ref(),
            outer,
        ) else {
            return Ok(None);
        };
        if bound.correlated_columns.is_empty() {
            return Ok(None);
        }
        Ok(Some(
            bound
                .correlated_columns
                .iter()
                .filter_map(|c| {
                    outer
                        .get(c.binding.column_idx)
                        .map(|name| (name.clone(), c.binding.column_idx))
                })
                .collect(),
        ))
    }

    pub(crate) fn scalar_subquery_value(
        &self,
        sql: &str,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<Value> {
        let (columns, rows) = self.run_subquery(sql, ctes, outer)?;
        if columns.len() > 1 {
            return Err(Error::Invalid(format!(
                "scalar subquery must return only one column, got {}",
                columns.len()
            )));
        }
        if rows.len() > 1 {
            return Err(Error::Invalid(format!(
                "scalar subquery returned more than one row ({} rows)",
                rows.len()
            )));
        }
        Ok(rows
            .into_iter()
            .next()
            .and_then(|mut r| {
                if r.is_empty() {
                    None
                } else {
                    Some(r.remove(0))
                }
            })
            .unwrap_or(Value::Null))
    }

    /// `EXISTS (SELECT …)`: true when the subquery produces at least one row.
    pub(crate) fn subquery_exists(
        &self,
        sql: &str,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<bool> {
        Ok(!self.run_subquery(sql, ctes, outer)?.1.is_empty())
    }

    /// Rewrites the parser's `SUBQUERY`/`EXISTS` pseudo-functions into
    /// constants that the planner can evaluate. A scalar subquery becomes a
    /// literal (NULL when empty); `EXISTS` becomes a boolean comparison, which
    /// keeps `NOT EXISTS` working through `Predicate::Not`.
    pub(crate) fn resolve_subquery_expr(
        &self,
        e: &ExprAst,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<ExprAst> {
        Ok(match e {
            ExprAst::Func { name, args } if name.eq_ignore_ascii_case("SUBQUERY") => {
                let sql = subquery_sql_arg(args)?;
                ExprAst::Literal(value_to_literal(
                    self.scalar_subquery_value(sql, ctes, outer)?,
                ))
            }
            ExprAst::Func { name, args } if name.eq_ignore_ascii_case("EXISTS") => {
                let sql = exists_subquery_sql(args)?;
                boolean_expr(self.subquery_exists(sql, ctes, outer)?)
            }
            ExprAst::Func { name, args } => ExprAst::Func {
                name: name.clone(),
                args: args
                    .iter()
                    .map(|a| self.resolve_subquery_expr(a, ctes, outer))
                    .collect::<Result<Vec<_>>>()?,
            },
            ExprAst::Binary(a, op, b) => ExprAst::Binary(
                Box::new(self.resolve_subquery_expr(a, ctes, outer)?),
                *op,
                Box::new(self.resolve_subquery_expr(b, ctes, outer)?),
            ),
            ExprAst::Cmp(a, op, b) => ExprAst::Cmp(
                Box::new(self.resolve_subquery_expr(a, ctes, outer)?),
                *op,
                Box::new(self.resolve_subquery_expr(b, ctes, outer)?),
            ),
            ExprAst::Not(a) => ExprAst::Not(Box::new(self.resolve_subquery_expr(a, ctes, outer)?)),
            ExprAst::And(a, b) => ExprAst::And(
                Box::new(self.resolve_subquery_expr(a, ctes, outer)?),
                Box::new(self.resolve_subquery_expr(b, ctes, outer)?),
            ),
            ExprAst::Or(a, b) => ExprAst::Or(
                Box::new(self.resolve_subquery_expr(a, ctes, outer)?),
                Box::new(self.resolve_subquery_expr(b, ctes, outer)?),
            ),
            other => other.clone(),
        })
    }

    pub(crate) fn resolve_subquery_pred(
        &self,
        p: &Predicate,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<Predicate> {
        Ok(match p {
            // The parser wraps `EXISTS` as `IsTrue(EXISTS(SUBQUERY))`. Rewrite
            // the whole predicate (not the inner expression) so the result is
            // a comparison, which `validate_where_predicate` accepts.
            Predicate::IsTrue(ExprAst::Func { name, args })
                if name.eq_ignore_ascii_case("EXISTS") =>
            {
                let sql = exists_subquery_sql(args)?;
                Predicate::Cmp(
                    ExprAst::Literal(Literal::Int(1)),
                    CmpOp::Eq,
                    ExprAst::Literal(Literal::Int(i64::from(
                        self.subquery_exists(sql, ctes, outer)?,
                    ))),
                )
            }
            Predicate::Cmp(a, op, b) => Predicate::Cmp(
                self.resolve_subquery_expr(a, ctes, outer)?,
                *op,
                self.resolve_subquery_expr(b, ctes, outer)?,
            ),
            Predicate::IsTrue(e) => Predicate::IsTrue(self.resolve_subquery_expr(e, ctes, outer)?),
            Predicate::IsNull(e, neg) => {
                Predicate::IsNull(self.resolve_subquery_expr(e, ctes, outer)?, *neg)
            }
            Predicate::Like(a, pat, esc, neg) => Predicate::Like(
                self.resolve_subquery_expr(a, ctes, outer)?,
                self.resolve_subquery_expr(pat, ctes, outer)?,
                *esc,
                *neg,
            ),
            Predicate::In(a, vals, neg) => Predicate::In(
                self.resolve_subquery_expr(a, ctes, outer)?,
                vals.iter()
                    .map(|v| self.resolve_subquery_expr(v, ctes, outer))
                    .collect::<Result<Vec<_>>>()?,
                *neg,
            ),
            Predicate::Bool(a, op, b) => Predicate::Bool(
                Box::new(self.resolve_subquery_pred(a, ctes, outer)?),
                *op,
                Box::new(self.resolve_subquery_pred(b, ctes, outer)?),
            ),
            Predicate::Not(a) => {
                Predicate::Not(Box::new(self.resolve_subquery_pred(a, ctes, outer)?))
            }
        })
    }

    /// Resolves every subquery in a `SELECT`'s expressions. Returns `None`
    /// (without cloning the statement) when there is nothing to do.
    pub(crate) fn resolve_stmt_subqueries(
        &self,
        stmt: &Statement,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer: &[String],
    ) -> Result<Option<Statement>> {
        let Statement::Select {
            items,
            where_pred,
            group_by,
            having,
            order_by,
            search,
            ..
        } = stmt
        else {
            return Ok(None);
        };
        let changed = items.iter().any(SelectItem::contains_subquery)
            || where_pred
                .as_ref()
                .is_some_and(Predicate::contains_subquery)
            || group_by.iter().any(ExprAst::contains_subquery)
            || having.as_ref().is_some_and(|h| h.contains_subquery())
            || order_by
                .iter()
                .any(|k| k.expr.as_ref().is_some_and(ExprAst::contains_subquery))
            || search.as_ref().is_some_and(|s| search_has_subquery(s));
        if !changed {
            return Ok(None);
        }
        let mut s = stmt.clone();
        let Statement::Select {
            items,
            where_pred,
            group_by,
            having,
            order_by,
            search,
            ..
        } = &mut s
        else {
            unreachable!()
        };
        for it in items.iter_mut() {
            match it {
                SelectItem::Func { args, .. } => {
                    for a in args.iter_mut() {
                        *a = self.resolve_subquery_expr(a, ctes, outer)?;
                    }
                }
                SelectItem::Expr(e) => *e = self.resolve_subquery_expr(e, ctes, outer)?,
                SelectItem::Aggregate(a) => {
                    for e in a.arg_exprs.iter_mut().flatten() {
                        *e = self.resolve_subquery_expr(e, ctes, outer)?;
                    }
                }
                _ => {}
            }
        }
        if let Some(p) = where_pred {
            *p = self.resolve_subquery_pred(p, ctes, outer)?;
        }
        for g in group_by.iter_mut() {
            *g = self.resolve_subquery_expr(g, ctes, outer)?;
        }
        if let Some(h) = having.as_mut() {
            **h = self.resolve_subquery_pred(h, ctes, outer)?;
        }
        for k in order_by.iter_mut() {
            if let Some(e) = k.expr.as_mut() {
                *e = self.resolve_subquery_expr(e, ctes, outer)?;
            }
        }
        // `SEARCH MATCH (…) WHERE <filter>` is compiled directly by the search
        // path, so an unresolved subquery there would never reach the planner's
        // guard and would evaluate as an unknown function (NULL).
        if let Some(search) = search.as_mut() {
            for m in search.matches.iter_mut() {
                for a in m.args.iter_mut() {
                    *a = self.resolve_subquery_expr(a, ctes, outer)?;
                }
                if let Some(f) = m.filter.as_mut() {
                    *f = self.resolve_subquery_pred(f, ctes, outer)?;
                }
            }
        }
        Ok(Some(s))
    }

    /// Evaluates a constant `LIMIT`/`OFFSET` expression. A subquery is not a
    /// constant here: it is rejected rather than silently ignored.
    pub(crate) fn eval_limit_expr(&self, expr: &ExprAst) -> Result<u64> {
        if expr.contains_subquery() {
            return Err(Error::Invalid(
                "LIMIT/OFFSET must be a constant expression, not a subquery".into(),
            ));
        }
        let compiled = planner::compile_expr(expr, &[])?;
        match compiled.eval(&[]) {
            Value::Int(n) if n >= 0 => Ok(n as u64),
            _ => Err(Error::Invalid(
                "LIMIT/OFFSET must evaluate to a non-negative integer".into(),
            )),
        }
    }

    pub fn execute(&self, sql: &str) -> Result<Vec<QueryResult>> {
        let __q_p = parse(sql)?;
        let mut out = Vec::new();
        for s in __q_p {
            // `NewTxnManager::BeginTxnShared` runs once per statement, read-only
            // ones included; the id lands in the WAL frame of a write. An RPC
            // that already took one for its statement keeps it.
            self.txn.ensure_statement_id();
            self.context
                .query_count
                .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
            // `USE <db>` switches the session database for later statements.
            if let Statement::Use { name } = &s {
                // The reference rejects `USE` of an unregistered database
                // instead of switching the session into it.
                if !self
                    .storage
                    .catalog
                    .list_databases()?
                    .iter()
                    .any(|db| db.eq_ignore_ascii_case(name))
                {
                    return Err(Error::NotFound(format!("database '{name}'")));
                }
                *self.current_db.lock().unwrap() = name.clone();
                out.push(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                });
                self.txn.end_statement();
                continue;
            }
            let qualified = self.qualify_tables(&s);
            // Storage mode gate, before the statement can touch the engine.
            // Every write operator checks the mode in the reference, and this
            // covers INSERT/INSERT SELECT/DELETE/UPDATE, the DDL and snapshot
            // statements, and COPY FROM.
            //
            // `FLUSH`/`OPTIMIZE`/`COMPACT`/`CLEANUP`/`DUMP` are not covered:
            // they have no AST node, and the parser folds them into
            // `Utility { text }` while dropping the keyword itself (`FLUSH
            // DATA` parses to `text == "DATA"`, see
            // `infinity-parser/src/corpora_ut.rs:50-57`), so the verb cannot be
            // recovered here. Nothing is bypassed today because the executor
            // answers every `Utility` with an empty result; they must be gated
            // once they get real `Statement` variants. Their engine-method
            // entry points (`flush`, `optimize_indexes`, `cleanup`) are gated.
            if invalidates_result_cache(&qualified) {
                self.ensure_writable()?;
            }
            // Result cache: `SELECT` results are reused verbatim when the cache
            // is active; any data-modifying statement invalidates it.
            if matches!(qualified, Statement::Select { .. }) && self.result_cache_active() {
                // Key on the statement, not the whole input: a multi-statement
                // string must not serve the first SELECT's rows for the second.
                let key = format!("{qualified:?}");
                if let Some(hit) = self.result_cache.lock().unwrap().get(&key).cloned() {
                    out.extend(hit);
                    self.txn.end_statement();
                    continue;
                }
                let res = self.execute_stmt_retrying(&qualified, sql)?;
                // Only small results are worth caching: retaining (and later
                // cloning on a hit) a whole-table `SELECT *` keeps hundreds of
                // megabytes to gigabytes alive.
                if cacheable_result(&res)
                    && self.result_cache.lock().unwrap().len() < self.result_cache_capacity()
                {
                    self.result_cache
                        .lock()
                        .unwrap()
                        .insert(key, vec![res.clone()]);
                }
                out.push(res);
                self.txn.end_statement();
                continue;
            }
            let mutating = invalidates_result_cache(&qualified);
            if mutating {
                self.result_cache.lock().unwrap().clear();
                self.plan_cache.clear();
            }
            out.push(self.execute_stmt_retrying(&qualified, sql)?);
            // No per-statement durability step: the reference's commit appends
            // its frame with `ofs_.write` (userspace, no fsync,
            // `wal_manager_impl.cpp:311`) and writes the catalog with
            // `disableWAL` (`kv_store_impl.cpp:311`). What a crash keeps is
            // therefore decided by the checkpoint, not by the commit — see
            // `Engine::flush_storage`.
            self.txn.end_statement();
        }
        Ok(out)
    }

    /// The columnar fast path the thrift writer uses: a single-table `SELECT`
    /// whose items are columns, `*`, or scalar expressions, optionally with
    /// `WHERE`, `ORDER BY`, `LIMIT` and `OFFSET`.
    ///
    /// `None` means "run it through `execute`": a join, an aggregate, `GROUP BY`,
    /// `DISTINCT`, a CTE, a search, a non-scalar column, or a stat function the
    /// scalar binder does not cover.
    pub fn select_columns(&self, sql: &str) -> Result<Option<Batch>> {
        let statements = parse(sql)?;
        if statements.len() != 1 {
            return Ok(None);
        }
        let statement = &statements[0];
        let Statement::Select {
            items,
            from: Some(from),
            join,
            where_pred,
            group_by,
            aliases,
            having,
            order_by,
            limit,
            offset,
            search,
            distinct,
            highlight,
            ctes,
            cross_tables,
            limit_expr,
            offset_expr,
        } = statement
        else {
            return Ok(None);
        };
        if join.is_some()
            || !group_by.is_empty()
            || having.is_some()
            || search.is_some()
            || *distinct
            || !highlight.is_empty()
            || !ctes.is_empty()
            || !cross_tables.is_empty()
            || limit_expr.is_some()
            || offset_expr.is_some()
        {
            return Ok(None);
        }
        let table = self.qualified_table(from);
        let schema = self.table_schema(&table)?;
        let binder = planner::binder::QueryBinder::new();

        // One output column: a table column, or a scalar expression evaluated
        // per row.
        enum Out {
            Column { pos: usize, label: String },
            Expr { expr: Expr, label: String },
        }
        let mut projection: Vec<Out> = Vec::with_capacity(items.len());
        for (i, item) in items.iter().enumerate() {
            let label = aliases.get(i).cloned().flatten();
            match item {
                SelectItem::Star => {
                    for (pos, name) in schema.iter().enumerate() {
                        projection.push(Out::Column {
                            pos,
                            label: name.clone(),
                        });
                    }
                }
                SelectItem::Column(name) => {
                    let base = name.rsplit('.').next().unwrap_or(name.as_str());
                    let Some(pos) = schema.iter().position(|c| c == base) else {
                        return Ok(None);
                    };
                    projection.push(Out::Column {
                        pos,
                        label: label.unwrap_or_else(|| base.to_string()),
                    });
                }
                SelectItem::Expr(ast) => {
                    let expr = binder.bind_expression(ast, &schema)?;
                    projection.push(Out::Expr {
                        expr,
                        label: label.unwrap_or_else(|| planner::item_label(item)),
                    });
                }
                _ => return Ok(None),
            }
        }
        if projection.is_empty() {
            return Ok(None);
        }

        // `filter_text`/`filter_fulltext` are answered by a full-text index and
        // may carry an `@index` hint. The columnar path below evaluates the
        // predicate row by row against the column, which silently ignores the
        // hint (a sparse-gram query then matches like the default analyzer), so
        // hand these statements to the general path.
        if let Some(pred) = where_pred.as_ref() {
            if has_fulltext_call(pred) {
                return Ok(None);
            }
        }
        let predicate = match where_pred.as_ref() {
            Some(pred) => Some(planner::predicate_expr(pred, &schema)?),
            None => None,
        };
        // Sort keys: a plain column, or the expression when there is one.
        let mut keys: Vec<(Expr, bool)> = Vec::with_capacity(order_by.len());
        for key in order_by {
            let expr = match &key.expr {
                Some(ast) => binder.bind_expression(ast, &schema)?,
                None => {
                    let base = key.column.rsplit('.').next().unwrap_or(key.column.as_str());
                    let Some(pos) = schema.iter().position(|c| c == base) else {
                        return Ok(None);
                    };
                    Expr::Column(pos)
                }
            };
            keys.push((expr, key.asc));
        }

        // The columns the work needs, in table order.
        let mut needed: Vec<usize> = Vec::new();
        for out in &projection {
            match out {
                Out::Column { pos, .. } => needed.push(*pos),
                // An expression reads columns the projection does not name.
                Out::Expr { expr, .. } => predicate_columns(expr, &mut needed),
            }
        }
        if let Some(pred) = &predicate {
            predicate_columns(pred, &mut needed);
        }
        for (key, _) in &keys {
            predicate_columns(key, &mut needed);
        }
        needed.sort_unstable();
        needed.dedup();
        if needed.is_empty() {
            return Ok(None);
        }
        // Only scalar and text columns are packed from column buffers; an
        // embedding / tensor / sparse / array column has its own wire encoding.
        let Some(meta) = self.storage.catalog.get_table_by_name(&table)? else {
            return Ok(None);
        };
        // A whitelist, not a blacklist: the columnar reader knows the scalar and
        // text layouts, and anything else (embedding, tensor, tensor array,
        // sparse, array, multi-vector) has a wire encoding of its own.
        for pos in &needed {
            let Some(declared) = meta.types.get(*pos) else {
                return Ok(None);
            };
            let up = declared.trim().to_ascii_uppercase();
            let base = up
                .split('(')
                .next()
                .unwrap_or("")
                .split(',')
                .next()
                .unwrap_or("")
                .trim();
            let known = matches!(
                base,
                "BOOL"
                    | "BOOLEAN"
                    | "TINYINT"
                    | "SMALLINT"
                    | "INT"
                    | "INTEGER"
                    | "BIGINT"
                    | "UINT8"
                    | "UINT16"
                    | "UINT32"
                    | "UINT64"
                    | "FLOAT"
                    | "FLOAT32"
                    | "DOUBLE"
                    | "FLOAT64"
                    | "FLOAT16"
                    | "BFLOAT16"
                    | "VARCHAR"
                    | "CHAR"
                    | "TEXT"
                    | "STRING"
                    | "JSON"
                    | "DATE"
                    | "TIME"
                    | "DATETIME"
                    | "TIMESTAMP"
            );
            if !known {
                return Ok(None);
            }
        }

        // The same statement prologue as `execute`: an id for the statement,
        // visibility at `current_ts + 1`, and the query registry entry.
        self.txn.ensure_statement_id();
        self.context
            .query_count
            .fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        let _query = self.begin_query(statement, sql);
        let _read = ReadTsGuard::enter(self.txn.current_ts() + 1);
        // A predicate the secondary index can answer already names its rows:
        // decoding the whole table first made every indexed lookup cost the
        // same as a full scan. An equality probes the key, a conjunction of
        // bounds scans the key range - both land in `read_columns_batch_for_rows`.
        let indexed_rows = predicate
            .as_ref()
            .and_then(|expr| self.indexed_predicate_rows(&table, schema.as_slice(), expr));
        // No index answered: the scan still narrows itself. A predicate on one
        // fixed-width column is checked against the block's bytes, so the rows
        // it excludes are never decoded - the difference between a filtered
        // scan costing more than an unfiltered one (it did: everything was
        // materialised, then filtered) and costing what it returns.
        let scan_bounds = match &indexed_rows {
            Some(_) => None,
            None => predicate
                .as_ref()
                .and_then(crate::blocks::ColumnBounds::from_expr),
        };
        let batch = match &indexed_rows {
            Some(ids) => self
                .read_columns_batch_for_rows(&table, &needed, ids)
                .or_else(|| self.read_columns_batch(&table, &needed, scan_bounds.as_ref())),
            None => self.read_columns_batch(&table, &needed, scan_bounds.as_ref()),
        };
        self.txn.end_statement();
        let Some(mut batch) = batch else {
            return Ok(None);
        };

        // One row buffer, filled from the decoded columns, for the predicate,
        // the sort keys and the projected expressions.
        let width = schema.len();
        let mut scratch = vec![Value::Null; width];
        let fill_row = |scratch: &mut Vec<Value>, batch: &Batch, r: usize| {
            for (c, pos) in needed.iter().enumerate() {
                if *pos < width {
                    scratch[*pos] =
                        if batch.valid.get(c).and_then(|v| v.get(r)).copied() == Some(false) {
                            Value::Null
                        } else {
                            batch.data[c].value_at(r)
                        };
                }
            }
        };

        if let Some(pred) = &predicate {
            // `col = constant` is answered straight off the column.
            let mut mask: Option<Vec<bool>> = None;
            if let Expr::Cmp(a, parser::CmpOp::Eq, b) = pred {
                let (col, value) = match (&**a, &**b) {
                    (Expr::Column(i), Expr::Literal(v)) => (Some(*i), Some(v)),
                    (Expr::Literal(v), Expr::Column(i)) => (Some(*i), Some(v)),
                    _ => (None, None),
                };
                if let (Some(pos), Some(value)) = (col, value) {
                    if let Some(c) = needed.iter().position(|p| *p == pos) {
                        mask = Some(batch.data[c].equality_mask(&batch.valid[c], value));
                    }
                }
            }
            let mask = match mask {
                Some(mask) => mask,
                None => {
                    let mut mask = Vec::with_capacity(batch.row_count);
                    for r in 0..batch.row_count {
                        fill_row(&mut scratch, &batch, r);
                        mask.push(pred.eval_predicate(&scratch));
                    }
                    mask
                }
            };
            if mask.iter().any(|k| !k) {
                for c in 0..batch.data.len() {
                    batch.data[c].retain(&mask);
                    let mut at = 0usize;
                    batch.valid[c].retain(|_| {
                        let v = mask.get(at).copied().unwrap_or(false);
                        at += 1;
                        v
                    });
                }
                batch.row_count = mask.iter().filter(|k| **k).count();
            }
        }

        if !keys.is_empty() && batch.row_count > 1 {
            let mut order: Vec<usize> = (0..batch.row_count).collect();
            let mut sort_keys: Vec<Vec<Value>> = Vec::with_capacity(keys.len());
            for (key, _) in &keys {
                let mut column = Vec::with_capacity(batch.row_count);
                for r in 0..batch.row_count {
                    fill_row(&mut scratch, &batch, r);
                    column.push(key.eval(&scratch));
                }
                sort_keys.push(column);
            }
            order.sort_by(|a, b| {
                for (k, (_, asc)) in keys.iter().enumerate() {
                    let ord = sort_keys[k][*a].partial_cmp_value(&sort_keys[k][*b]);
                    let Some(ord) = ord else { continue };
                    if ord != std::cmp::Ordering::Equal {
                        return if *asc { ord } else { ord.reverse() };
                    }
                }
                a.cmp(b)
            });
            for column in &mut batch.data {
                *column = crate::blocks::permute_column(column, &order);
            }
            for v in &mut batch.valid {
                *v = order.iter().map(|i| v[*i]).collect();
            }
        }

        let skip = offset.unwrap_or(0) as usize;
        let skip = skip.min(batch.row_count);
        let take = match limit {
            Some(n) => (*n as usize).min(batch.row_count - skip),
            None => batch.row_count - skip,
        };
        let keep: Vec<bool> = (0..batch.row_count)
            .map(|r| r >= skip && r < skip + take)
            .collect();
        if skip > 0 || take < batch.row_count {
            for c in 0..batch.data.len() {
                batch.data[c].retain(&keep);
                let mut at = 0usize;
                batch.valid[c].retain(|_| {
                    let v = keep.get(at).copied().unwrap_or(false);
                    at += 1;
                    v
                });
            }
            batch.row_count = take;
        }

        // Projection: hand over the decoded column, or evaluate the expression
        // per row into a new one.
        let mut data = Vec::with_capacity(projection.len());
        let mut valid = Vec::with_capacity(projection.len());
        let mut labels = Vec::with_capacity(projection.len());
        for out in &projection {
            match out {
                Out::Column { pos, label } => {
                    let Some(c) = needed.iter().position(|p| p == pos) else {
                        return Ok(None);
                    };
                    data.push(batch.data[c].clone());
                    valid.push(batch.valid[c].clone());
                    labels.push(label.clone());
                }
                Out::Expr { expr, label } => {
                    // The column's type is decided from **all** the values: a
                    // JSON number can be an integer in one row and a float in
                    // the next, and picking the first one would truncate.
                    let mut values: Vec<Value> = Vec::with_capacity(batch.row_count);
                    for r in 0..batch.row_count {
                        fill_row(&mut scratch, &batch, r);
                        values.push(expr.eval(&scratch));
                    }
                    let mut column = if values.iter().any(|v| matches!(v, Value::Str(_))) {
                        ColumnData::Text(Default::default())
                    } else if values.iter().any(|v| matches!(v, Value::Float(_))) {
                        ColumnData::Float(Vec::new())
                    } else if values.iter().any(|v| matches!(v, Value::Bool(_)))
                        && !values.iter().any(|v| matches!(v, Value::Int(_)))
                    {
                        ColumnData::Bool(Vec::new())
                    } else {
                        ColumnData::Int(Vec::new())
                    };
                    let mut col_valid = Vec::with_capacity(values.len());
                    for value in &values {
                        col_valid.push(!matches!(value, Value::Null));
                        column.push_value(value);
                    }
                    data.push(column);
                    valid.push(col_valid);
                    labels.push(label.clone());
                }
            }
        }
        let row_count = batch.row_count;
        Ok(Some(Batch {
            columns: labels,
            data,
            valid,
            row_count,
        }))
    }

    /// Resolves unqualified table names to `<current_db>.<name>`; a name that
    /// already contains a `.` is taken as fully qualified.
    pub(crate) fn qualify_tables(&self, stmt: &Statement) -> Statement {
        self.qualify_tables_in(stmt, &[])
    }

    /// [`Self::qualify_tables`] with the CTE aliases that are visible from an
    /// enclosing `WITH` clause. A CTE reference is a query-local name, not a
    /// catalog table, so it must never be qualified (or view-expanded).
    pub(crate) fn qualify_tables_in(&self, stmt: &Statement, outer_ctes: &[String]) -> Statement {
        let db = self.db();
        // Identifiers are case-insensitive: the catalog keeps lower-case names,
        // so every table reference is normalised before it reaches it.
        let q = |t: &str| -> String {
            let t = t.to_ascii_lowercase();
            if t.is_empty() || t.contains('.') {
                t
            } else {
                format!("{db}.{t}")
            }
        };
        let mut s = stmt.clone();
        match &mut s {
            Statement::CreateTable { name, .. }
            | Statement::DropTable { name, .. }
            | Statement::DropView { name, .. }
            | Statement::CreateCollection { name, .. }
            | Statement::DropCollection { name, .. } => *name = q(name),
            Statement::CreateTableAs { name, query, .. } => {
                *name = q(name);
                **query = self.qualify_tables_in(query, outer_ctes);
            }
            Statement::CreateView { name, query, .. } => {
                *name = q(name);
                **query = self.qualify_tables_in(query, outer_ctes);
            }
            Statement::Insert { table, .. }
            | Statement::Delete { table, .. }
            | Statement::Update { table, .. }
            | Statement::AlterTableAdd { table, .. }
            | Statement::AlterTableDropColumn { table, .. }
            | Statement::ShowTableColumns { table }
            | Statement::ShowTable { table }
            | Statement::ShowTableIndex { table, .. }
            | Statement::Describe { table }
            | Statement::Copy { table, .. }
            | Statement::CreateIndex { table, .. }
            | Statement::CreateSnapshot { table, .. } => *table = q(table),
            Statement::InsertSelect { table, query, .. } => {
                *table = q(table);
                **query = self.qualify_tables_in(query, outer_ctes);
            }
            Statement::RenameTable { table, new_name } => {
                *table = q(table);
                *new_name = q(new_name);
            }
            Statement::Select {
                from, join, ctes, ..
            } => {
                let cte_names: Vec<String> =
                    ctes.iter().map(|c| c.alias.to_ascii_lowercase()).collect();
                let is_cte = |name: &str| {
                    let name = name.to_ascii_lowercase();
                    outer_ctes.contains(&name) || cte_names.contains(&name)
                };
                // A CTE query sees only the bindings that precede it.
                for i in 0..ctes.len() {
                    let mut visible = outer_ctes.to_vec();
                    visible.extend(cte_names[..i].iter().cloned());
                    *ctes[i].query = self.qualify_tables_in(ctes[i].query.as_ref(), &visible);
                }
                if let Some(t) = from.as_mut() {
                    // A derived table holds the subquery's SQL, not a table name.
                    // (A table may legitimately be called `select3`, so require the
                    // keyword and its trailing space.)
                    if !t.trim_start().to_ascii_uppercase().starts_with("SELECT ") && !is_cte(t) {
                        *t = q(t);
                    }
                }
                if let Some(j) = join.as_mut() {
                    if !is_cte(&j.table) {
                        j.table = q(&j.table);
                    }
                }
            }
            Statement::Explain { inner, .. } => {
                let inner_q = self.qualify_tables_in(inner.as_ref(), outer_ctes);
                **inner = inner_q;
            }
            _ => {}
        }
        s
    }

    /// `QueryContext::QueryStatement`: a **retryable** conflict re-runs the
    /// whole statement against a fresh transaction
    /// (`query_context_impl.cpp:114-121`), and a non-retryable one goes back to
    /// the client (4005).
    ///
    /// The reference retries without a bound (the conflicting transaction
    /// finishes and the retry sees it, so the loop converges). The bound here
    /// only keeps a statement that conflicts with itself from spinning: the
    /// retry is given up after a thousand turns, and a turn that saw no commit
    /// at all yields the CPU first.
    pub(crate) fn execute_stmt_retrying(&self, stmt: &Statement, sql: &str) -> Result<QueryResult> {
        for _ in 0..RETRY_LIMIT {
            let before = self.txn.current_ts();
            match self.execute_stmt(stmt, sql) {
                Err(Error::TxnConflict(_reason)) => {
                    // `QueryStatementInternal` begins a new transaction for the
                    // retry, so the statement spends another txn id.
                    self.txn.end_statement();
                    self.txn.ensure_statement_id();
                    // The conflicting transaction may still be committing (its
                    // commit ts is taken, its entry not yet written): give it
                    // the CPU instead of binning the statement.
                    if self.txn.current_ts() == before {
                        std::thread::yield_now();
                    }
                    continue;
                }
                Err(Error::TxnConflictNoRetry(reason)) => {
                    return Err(Error::TxnConflictNoRetry(reason))
                }
                other => return other,
            }
        }
        Err(Error::TxnConflict(format!(
            "the statement kept conflicting for {RETRY_LIMIT} attempts"
        )))
    }

    /// Runs one statement with the query registry open around it (C++
    /// `QueryContext::QueryStatement`, `query_context_impl.cpp:180-205`): the
    /// statement is visible to `SHOW QUERIES` while it runs, and its wall-clock
    /// duration moves to the profile history when it returns — including on the
    /// error path, because the guard deregisters on drop.
    pub(crate) fn execute_stmt(&self, stmt: &Statement, sql: &str) -> Result<QueryResult> {
        let _query = self.begin_query(stmt, sql);
        // `NewTxnManager::BeginTxnShared`: a statement reads the state as of the
        // clock when it started, so a commit that lands while it runs is not
        // half-visible (`NewTxn::BeginTS`, `new_txn_manager_impl.cpp:113`).
        let _read = ReadTsGuard::enter(self.txn.current_ts() + 1);
        let mut res = self.execute_stmt_inner(stmt)?;
        if let Statement::Select {
            highlight,
            search,
            from,
            ..
        } = stmt
        {
            if !highlight.is_empty() {
                self.apply_highlights(&mut res, highlight, search.as_deref(), from.as_deref());
            }
        }
        Ok(res)
    }

    pub(crate) fn execute_stmt_inner(&self, stmt: &Statement) -> Result<QueryResult> {
        match stmt {
            Statement::CreateTable {
                name,
                columns,
                opts,
            } => {
                // A sparse column written as `SPARSE(elem, dim)` may declare 0
                // (an unknown bound); a negative one is invalid.
                for c in columns {
                    let up = c.type_text.to_ascii_uppercase();
                    if up.starts_with("SPARSE(") {
                        let inner =
                            c.type_text[c.type_text.find('(').unwrap() + 1..].trim_end_matches(')');
                        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
                        // Two spellings reach us: `SPARSE(elem, dim)` (the
                        // reference DDL) and `Sparse(elem, index_type, dim)` (the
                        // REST physical type). The three-part form's index type
                        // must be an integer width.
                        let (idx_ty, dim_part) = if parts.len() >= 3 {
                            (Some(parts[1]), parts[2])
                        } else {
                            (None, parts[1])
                        };
                        if let Some(idx_ty) = idx_ty {
                            let t = idx_ty.to_ascii_lowercase();
                            let int_like = [
                                "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32",
                                "uint64", "tinyint", "smallint", "int", "bigint",
                            ]
                            .contains(&t.as_str());
                            if !int_like {
                                return Err(Error::Invalid(format!(
                                    "invalid sparse index type '{idx_ty}' for column '{}'",
                                    c.name
                                )));
                            }
                        }
                        if parts.len() >= 2 {
                            let dim: i64 = dim_part.parse().unwrap_or(0);
                            // The reference rejects a zero bound at DDL time, but
                            // the SDK's `sparse,0,…` column type (physical type
                            // dimension 0) is accepted.
                            let allow_zero = ALLOW_ZERO_SPARSE.with(|c| c.get())
                                // the REST physical-type spelling declares an
                                // unknown bound as 0
                                || parts.len() >= 3;
                            if dim < 0 || (dim == 0 && !allow_zero) {
                                return Err(Error::Invalid(format!(
                                    "invalid sparse dimension {dim} for column '{}'",
                                    c.name
                                )));
                            }
                        }
                    }
                }
                // A vector-like column's declared dimension must be positive.
                for c in columns {
                    if !declares_dimension(&c.type_text) {
                        continue;
                    }
                    if let Some(d) = type_dim_deep(&c.type_text) {
                        let up = c.type_text.to_ascii_uppercase();
                        // A sparse column may declare a zero bound and fail at
                        // search time instead.
                        if up.starts_with("SPARSE(") {
                            continue;
                        }
                        let embedding = up.contains("EMBEDDING") || up.contains("VECTOR");
                        if d == 0 || (embedding && d > 16384) {
                            return Err(Error::Invalid(format!(
                                "invalid embedding dimension {d} for column '{}'",
                                c.name
                            )));
                        }
                    }
                }
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| {
                        c.default
                            .as_ref()
                            .map(|l| normalize_default(&c.type_text, &l.to_slt_string()))
                    })
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                let types: Vec<String> = columns.iter().map(|c| c.type_text.clone()).collect();
                let comments: Vec<Option<String>> =
                    columns.iter().map(|c| c.comment.clone()).collect();
                let sparse_stores = sparse_store_hints(columns);
                // `WITH (bloom_filter_columns = '…')`: which columns ask for a
                // binary-fuse prefilter (`logical_planner_impl.cpp:690`).
                // The reference's message names the table without its schema
                // (`*table_def_ptr->table_name()`).
                let bloom = bloom_filter_flags(
                    opts,
                    &names,
                    &types,
                    name.rsplit('.').next().unwrap_or(name),
                )?;
                let t = self.ddl_commit_typed(
                    TxnOp::CreateTable {
                        db: split_table_name(name).0,
                        table: name.clone(),
                    },
                    || {
                        self.create_table_clearing_orphans(
                            name,
                            &names,
                            &defaults,
                            &not_nulls,
                            &types,
                            &comments,
                            &sparse_stores,
                        )
                    },
                    // The reference puts the flag into the WAL command itself
                    // (`TableDef::WriteAdv` -> `ColumnDef::WriteAdv`), not into
                    // the catalog, whose `ColumnDef::ToJson` omits it.
                    |batch| {
                        let (db, bare) = split_table_name(name);
                        let cmd =
                            crate::wal_bridge::create_table_command_from_batch(batch, &db, &bare)?;
                        match cmd {
                            storage::WalCmd::CreateTable {
                                db_name,
                                db_id,
                                table_id,
                                mut table_def,
                            } => {
                                for (i, column) in table_def.columns.iter_mut().enumerate() {
                                    column.build_bloom_filter =
                                        bloom.get(i).copied().unwrap_or(false);
                                }
                                Some(storage::WalCmd::CreateTable {
                                    db_name,
                                    db_id,
                                    table_id,
                                    table_def,
                                })
                            }
                            other => Some(other),
                        }
                    },
                )?;
                Ok(QueryResult::CreatedTable {
                    id: t.id,
                    name: t.name,
                })
            }
            Statement::RenameTable { table, new_name } => {
                let meta = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                if self.storage.catalog.get_table_by_name(new_name)?.is_some() {
                    return Err(Error::Invalid(format!("table '{new_name}' already exists")));
                }
                let _ = meta;
                // `RenameTable` moves the catalog only: the row ids and the
                // segment ids stay the same (the reference's
                // `cleanup_and_rename_table` asserts the pre-rename segment id),
                // and so does the block directory, which is keyed by table id.
                // The block directory is keyed by table id, which a rename
                // keeps, so the files are already the renamed table's.
                // Move the per-table entry (allocator, registries) to the new
                // name; the row ids it describes did not change.
                let old_tkey = self.table_key(table);
                let new_tkey = self.table_key(new_name);
                let migrated = self.tables.lock().unwrap().remove(&old_tkey);
                if let Some(entry) = migrated {
                    self.tables.lock().unwrap().insert(new_tkey.clone(), entry);
                }
                // move the secondary-index declarations; the old table entry
                // (whose registries refer to the old row ids) is dropped and
                // rebuilt from the renamed rows below.
                let pairs: Vec<String> = self
                    .tables
                    .lock()
                    .unwrap()
                    .get(&new_tkey)
                    .map(|e| e.secondary.indexes.keys().cloned().collect())
                    .unwrap_or_default();
                let rename_cmd = self.rename_table_command(table, new_name)?;
                self.ddl_commit_typed(
                    TxnOp::RenameTable {
                        db: split_table_name(table).0,
                        old: table.to_string(),
                        new: new_name.to_string(),
                    },
                    || {
                        // The reference's rename writes the new name's record
                        // and the old name's marker only: an index record is
                        // keyed by the **table id**, which a rename keeps, so it
                        // needs neither a drop nor a re-create (doing that
                        // stamped it with a new ts and made the catalog differ).
                        self.storage.catalog.rename_table(table, new_name)?;
                        for column in &pairs {
                            let _ = self.build_index(new_name, column);
                        }
                        Ok(())
                    },
                    |_| rename_cmd,
                )?;
                if let Some(meta) = self.storage.catalog.get_table_by_name(new_name)? {
                    self.register_table_ids(&meta);
                }
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::AlterTableDropColumn { table, column } => {
                self.invalidate_row_cache(table);
                // Dropping a column that does not exist is `COLUMN_NOT_EXIST`.
                let schema = self.table_schema(table)?;
                if !schema.iter().any(|c| c == column) {
                    return Err(Error::NotFound(format!("column '{column}'")));
                }
                // an index still referencing the column blocks the drop
                if self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.table == *table && im.column == *column)
                {
                    return Err(Error::Invalid(format!(
                        "column '{column}' is used by an index"
                    )));
                }
                // The stored layout does not change at all: `DropColumnsData`
                // writes a marker per block and leaves every `{column_id}.col`
                // where it is (`Cleanup` removes it later), so the rows are not
                // re-encoded and the ids do not move.
                self.invalidate_row_cache(table);
                self.invalidate_block_mirror(table);
                let dc_cmd = self.drop_columns_command(table, column)?;
                // `GetBlockIDs1`: the blocks the column files live in. Read
                // before the DDL, because the marker set is part of its batch.
                // The column's position is read here too: after the DDL the
                // schema no longer has it.
                let drop_position = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .and_then(|m| m.columns.iter().position(|c| c == column));
                let blocks: Vec<(u32, u16)> = self
                    .visible_block_ids(table)
                    .into_iter()
                    .map(|b| (b.segment_id, b.block_id))
                    .collect();
                self.ddl_commit_with_effects(
                    TxnOp::DropColumn {
                        table: table.to_string(),
                        columns: vec![column.to_string()],
                    },
                    || {
                        self.storage.catalog.drop_column(table, column, &blocks)?;
                        if let Some(im) = self
                            .storage
                            .catalog
                            .list_indexes()?
                            .into_iter()
                            .find(|i| i.table == *table && i.column == *column)
                        {
                            let _ = self.storage.catalog.drop_index_on(&im.table, &im.name);
                        }
                        Ok(((), drop_position))
                    },
                    |position| {
                        // A buffered row has no block file to leave alone: it
                        // loses the cell at the column's position, exactly as
                        // the reference's block buffer does. Applied only once
                        // the transaction has won.
                        if let Some(position) = position {
                            self.patch_buffered_drop_column(table, position);
                        }
                        Ok(())
                    },
                    |_| dc_cmd,
                )?;
                let tkey = self.table_key(table);
                self.tables
                    .lock()
                    .unwrap()
                    .entry(tkey)
                    .or_default()
                    .secondary
                    .indexes
                    .remove(column);
                // remaining indexes now hold stale row ids: rebuild them
                let remaining: Vec<String> = self
                    .storage
                    .catalog
                    .list_indexes()?
                    .into_iter()
                    .filter(|i| i.table == *table && is_secondary_kind(&i.index_type))
                    .map(|i| i.column)
                    .collect();
                for col in remaining {
                    self.build_index(table, &col)?;
                }
                Ok(QueryResult::CreatedTable {
                    id: 0,
                    name: table.clone(),
                })
            }
            Statement::AlterTableAdd { table, columns } => {
                // Rows cached before the new columns lack their defaults.
                self.invalidate_row_cache(table);
                self.invalidate_block_mirror(table);
                // A column may not be added twice (`DUPLICATE_COLUMN_NAME`).
                let existing = self.table_schema(table)?;
                let mut seen: Vec<String> = existing.clone();
                for c in columns {
                    if seen.iter().any(|n| n.eq_ignore_ascii_case(&c.name)) {
                        return Err(Error::Invalid(format!("duplicate column: '{}'", c.name)));
                    }
                    seen.push(c.name.clone());
                }
                // `ADD COLUMN … NOT NULL` requires a default for existing rows.
                for c in columns {
                    if c.not_null && c.default.is_none() {
                        return Err(Error::Invalid(format!(
                            "column '{}' is NOT NULL but has no default",
                            c.name
                        )));
                    }
                }
                let names: Vec<String> = columns.iter().map(|c| c.name.clone()).collect();
                let defaults: Vec<Option<String>> = columns
                    .iter()
                    .map(|c| c.default.as_ref().map(|l| l.to_slt_string()))
                    .collect();
                let not_nulls: Vec<bool> = columns.iter().map(|c| c.not_null).collect();
                let types: Vec<String> = columns.iter().map(|c| c.type_text.clone()).collect();
                let sparse_stores = sparse_store_hints(columns);
                let bare = table.rsplit('.').next().unwrap_or(table).to_string();
                let base_index = self.table_schema(table)?.len() as u32;
                let ac_ident = self.table_ident(table)?;
                let ac_names = names.clone();
                // The blocks and their bitmap coverage, read **before** the DDL:
                // the coverage comes from the append journal, which is derived
                // from the block files under the current schema, and the added
                // column has no file yet.
                let ac_blocks = self.add_column_blocks(table);
                // The cells the new columns take in the rows that are still
                // buffered, and the block files they need: computed here,
                // applied only once the transaction has won its conflicts.
                let added: Vec<Value> = (0..names.len())
                    .map(|k| match defaults.get(k) {
                        Some(Some(d)) => Value::from_literal(&default_literal(d)),
                        _ => Value::Null,
                    })
                    .collect();
                self.ddl_commit_with_effects(
                    TxnOp::AddColumn {
                        table: table.to_string(),
                    },
                    || {
                        self.storage.catalog.add_columns_typed_with_sparse_stores(
                            table,
                            &names,
                            &defaults,
                            &not_nulls,
                            &types,
                            &vec![None; names.len()],
                            &sparse_stores,
                        )?;
                        Ok(((), ac_blocks))
                    },
                    |blocks| {
                        // Rows still in the append buffer have no block file
                        // yet (`AddColumnsData` walks the segments and would
                        // skip them): the reference's block *buffer* gains the
                        // column, so they get the default cell here.
                        self.patch_buffered_add_columns(table, base_index as usize, &added);
                        // And the blocks that already exist get one file for
                        // each new column, before the commit is visible.
                        self.add_column_files(table, base_index as usize, &blocks)
                    },
                    move |b| match &ac_ident {
                        Some((db_id, table_id, table_key)) => {
                            let (db, _) = split_table_name(table);
                            crate::wal_bridge::add_columns_command_from_batch(
                                b, &db, db_id, table_id, &bare, table_key, base_index, &ac_names,
                            )
                        }
                        None => None,
                    },
                )?;
                Ok(QueryResult::CreatedTable {
                    id: 0,
                    name: table.clone(),
                })
            }
            // `USE` is handled by `execute`; reaching here is a no-op.
            Statement::Use { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::CreateDatabase {
                name,
                if_not_exists,
            } => {
                self.ddl_commit_typed(
                    TxnOp::CreateDatabase { db: name.clone() },
                    || self.storage.catalog.create_database(name, *if_not_exists),
                    |batch| crate::wal_bridge::create_database_command_from_batch(batch, name),
                )?;
                Ok(QueryResult::CreatedDatabase { name: name.clone() })
            }
            // A collection is the legacy alias of a table: the reference's
            // `PhysicalCreateCollection`/`PhysicalDropCollection` only check
            // that the node is writable and acknowledge with `OK`.
            Statement::CreateCollection { .. } => Ok(QueryResult::Rows {
                columns: vec!["OK".to_string()],
                rows: vec![],
            }),
            Statement::DropCollection { .. } => Ok(QueryResult::Rows {
                columns: vec!["OK".to_string()],
                rows: vec![],
            }),
            Statement::DropDatabase { name, if_exists } => {
                // The default database backs every session, so the C++ engine
                // refuses to drop it (3033 DROPPING_USING_DB).
                if name.eq_ignore_ascii_case("default_db") {
                    // `IF EXISTS` tolerates it (a no-op); a plain drop is 3033.
                    if *if_exists {
                        return Ok(QueryResult::DroppedDatabase { name: name.clone() });
                    }
                    return Err(Error::Invalid(
                        "dropping the current database is not allowed".into(),
                    ));
                }
                let drop_db_cmd = self.drop_database_command(name)?;
                // The tables the database holds, read **before** the drop: the
                // marker hides the database, and with it every table it
                // contains.
                let held: Vec<String> = self
                    .storage
                    .catalog
                    .tables_of_database(name)?
                    .into_iter()
                    .map(|t| t.name)
                    .collect();
                self.ddl_commit_typed(
                    TxnOp::DropDatabase { db: name.clone() },
                    || {
                        let existed = self.storage.catalog.drop_database(name, *if_exists)?;
                        if existed || *if_exists {
                            for t in &held {
                                if self.storage.catalog.is_catalog_only() {
                                    // `DROP DATABASE` is a marker; its tables go
                                    // with it in `Cleanup` (`NewCatalog::
                                    // CleanDB`), so only the in-memory registry
                                    // forgets them now.
                                    let key = self.table_key(t);
                                    self.tables.lock().unwrap().remove(&key);
                                } else {
                                    self.drop_table_all(t)?;
                                }
                            }
                        }
                        Ok(existed)
                    },
                    |_batch| drop_db_cmd,
                )?;
                Ok(QueryResult::DroppedDatabase { name: name.clone() })
            }
            Statement::Show { text } => {
                let mut words = text.split_ascii_whitespace();
                match words.next().map(str::to_ascii_lowercase).as_deref() {
                    Some("databases") => self.show_databases(),
                    Some("tables") => {
                        let db = match (words.next(), words.next()) {
                            (Some(kw), Some(db)) if kw.eq_ignore_ascii_case("from") => Some(db),
                            _ => None,
                        };
                        self.show_tables(db)
                    }
                    // `SHOW SNAPSHOTS` lists every snapshot directory
                    // (`physical_show_list_snapshots_impl.cpp`): name, scope,
                    // creation time, commit ts and size.
                    Some("snapshots") => Ok(QueryResult::Rows {
                        columns: vec![
                            "name".to_string(),
                            "scope".to_string(),
                            "time".to_string(),
                            "commit".to_string(),
                            "size".to_string(),
                        ],
                        rows: text_rows(
                            self.snapshot_briefs()?
                                .into_iter()
                                .map(|b| {
                                    vec![
                                        b.name,
                                        b.scope,
                                        b.create_time,
                                        b.commit_ts.to_string(),
                                        crate::snapshot::SnapshotBrief::format_size(b.size),
                                    ]
                                })
                                .collect(),
                        ),
                    }),
                    // `SHOW SNAPSHOT <name>` describes one
                    // (`physical_show_snapshot_impl.cpp`).
                    Some("snapshot") => {
                        let name = words.next().unwrap_or_default();
                        let brief = self
                            .snapshot_briefs()?
                            .into_iter()
                            .find(|b| b.name == name)
                            .ok_or_else(|| {
                                Error::Unknown(format!("can't find snapshot: {name}"))
                            })?;
                        Ok(QueryResult::Rows {
                            columns: vec!["name".to_string(), "value".to_string()],
                            rows: text_rows(vec![
                                vec!["snapshot_name".to_string(), brief.name],
                                vec!["snapshot_scope".to_string(), brief.scope],
                                vec!["create_time".to_string(), brief.create_time],
                                vec!["commit_timestamp".to_string(), brief.commit_ts.to_string()],
                                vec![
                                    "snapshot_size".to_string(),
                                    crate::snapshot::SnapshotBrief::format_size(brief.size),
                                ],
                            ]),
                        })
                    }
                    _ => Ok(QueryResult::Rows {
                        columns: vec![],
                        rows: vec![],
                    }),
                }
            }
            Statement::ShowTableIndex { table, index } => {
                if !self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == *index && im.table == *table)
                {
                    return Err(Error::Invalid(format!(
                        "index '{index}' does not exist on '{table}'"
                    )));
                }
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            // EXPLAIN only describes a plan; it must not execute the statement,
            // but binding the plan surfaces a missing relation.
            Statement::Explain { kind, inner } => {
                let logical = matches!(
                    kind,
                    parser::ExplainKind::Logical
                        | parser::ExplainKind::UnOpt
                        | parser::ExplainKind::Opt
                );
                // `EXPLAIN AST` prints the parsed statement, so it has no plan
                // to build (and must not bind the relation).
                let text = if matches!(kind, parser::ExplainKind::Ast) {
                    Some(vec![format!("{inner:?}")])
                } else {
                    match inner.as_ref() {
                        Statement::Select {
                            items,
                            from: Some(t),
                            join: Some(j),
                            where_pred,
                            search,
                            ..
                        } if logical && search.is_none() => {
                            Some(crate::explain::explain_logical_join(
                                items,
                                t,
                                &j.table,
                                j,
                                where_pred.as_ref(),
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            search,
                            ..
                        } if logical
                            && join.is_none()
                            && search.is_none()
                            && crate::explain::has_aggregate(items) =>
                        {
                            crate::explain::explain_logical_agg(items, t, group_by)
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            where_pred,
                            search,
                            ..
                        } if !logical
                            && join.is_none()
                            && search.is_none()
                            && group_by.is_empty()
                            && crate::explain::is_count_star_only(items) =>
                        {
                            Some(crate::explain::explain_count_star(
                                t,
                                where_pred.as_ref(),
                                self.multi_segment(t),
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            order_by,
                            search,
                            ..
                        } if join.is_none()
                            && search.is_none()
                            && crate::explain::is_distinct_agg_plan(items, group_by) =>
                        {
                            Some(crate::explain::explain_distinct_agg(
                                &crate::explain::DistinctAggPlan {
                                    items,
                                    table: t,
                                    group_by,
                                    order_by,
                                },
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            order_by,
                            search,
                            ..
                        } if join.is_none()
                            && search.is_none()
                            && crate::explain::has_aggregate(items)
                            && !group_by.is_empty()
                            // A `DISTINCT` aggregate keeps the two stacked
                            // aggregates (`MERGE HASH AGGREGATE` over `HASH
                            // AGGREGATE`), which `explain_distinct_agg` prints.
                            && !crate::explain::has_distinct_aggregate(items) =>
                        {
                            Some(crate::explain::explain_group_agg(
                                &crate::explain::HashAggPlan {
                                    items,
                                    table: t,
                                    group_by,
                                    order_by,
                                },
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            group_by,
                            order_by,
                            search,
                            ..
                        } if join.is_none()
                            && search.is_none()
                            && crate::explain::has_aggregate(items)
                            && crate::explain::aggregates_renderable(items) =>
                        {
                            Some(crate::explain::explain_hash_agg(
                                &crate::explain::HashAggPlan {
                                    items,
                                    table: t,
                                    group_by,
                                    order_by,
                                },
                            ))
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            search: Some(s),
                            where_pred,
                            limit,
                            offset,
                            ..
                        } if join.is_none() => {
                            let meta = self.table_meta(t)?;
                            let indexed = self.indexed_columns(t)?;
                            let plan = crate::explain_search::SearchPlan {
                                items,
                                table: t,
                                search: s,
                                limit: *limit,
                                offset: *offset,
                                where_pred: where_pred.as_ref(),
                                schema: &meta.columns,
                                types: &meta.types,
                                indexed: &indexed,
                                multi: self.multi_segment(t),
                            };
                            if matches!(kind, parser::ExplainKind::Fragment) {
                                crate::explain_search::fragment_lines(&plan)
                            } else {
                                crate::explain_search::explain_search(&plan)
                            }
                        }
                        Statement::Select {
                            items,
                            from: Some(t),
                            join,
                            where_pred,
                            group_by,
                            order_by,
                            distinct,
                            search,
                            limit,
                            ..
                        } if join.is_none()
                            && group_by.is_empty()
                            && !crate::explain::has_aggregate(items)
                            && search.is_none() =>
                        {
                            let meta = self.table_meta(t)?;
                            let schema = &meta.columns;
                            // Try to push the filter into an index scan first.
                            let index_plan = match where_pred.as_ref() {
                                Some(w) if !logical && !*distinct => {
                                    let indexed = self.indexed_columns(t)?;
                                    let index_names = self.index_name_pairs(t)?;
                                    let sparsegram = self.sparsegram_index_params(t)?;
                                    crate::explain::explain_index_scan(
                                        &crate::explain::IndexScanPlan {
                                            items,
                                            table: t,
                                            where_pred: w,
                                            order_by,
                                            limit: *limit,
                                            schema,
                                            types: &meta.types,
                                            indexed: &indexed,
                                            index_names: &index_names,
                                            sparsegram: &sparsegram,
                                        },
                                    )
                                }
                                _ => None,
                            };
                            Some(index_plan.unwrap_or_else(|| {
                                crate::explain::explain_select(&crate::explain::SelectPlan {
                                    items,
                                    table: t,
                                    where_pred: where_pred.as_ref(),
                                    order_by,
                                    distinct: *distinct,
                                    schema,
                                    logical,
                                    limit: *limit,
                                })
                            }))
                        }
                        Statement::Delete { table, where_pred } if logical => Some(
                            crate::explain::explain_logical_delete(table, where_pred.as_ref()),
                        ),
                        Statement::CreateTable { name, columns, .. } => {
                            let (db, tbl) = match name.split_once('.') {
                                Some((d, t)) => (d.to_string(), t.to_string()),
                                None => ("default_db".to_string(), name.clone()),
                            };
                            let cols: Vec<(String, String, Option<String>)> = columns
                                .iter()
                                .map(|c| {
                                    (
                                        c.name.clone(),
                                        describe_type(&c.type_text),
                                        c.default.as_ref().map(|l| l.to_slt_string()),
                                    )
                                })
                                .collect();
                            Some(crate::explain::explain_create_table(&db, &tbl, &cols))
                        }
                        _ => None,
                    }
                };
                // The reference names the single output column after the
                // plan kind (PhysicalExplain::Init,
                // physical_explain_impl.cpp:57-90); a client reads it from the
                // RowDescription, and PG rejects a DataRow whose field count
                // does not match it.
                let column = match kind {
                    parser::ExplainKind::Ast => "Abstract Syntax Tree",
                    parser::ExplainKind::UnOpt | parser::ExplainKind::Raw => {
                        "Unoptimized Logical Plan"
                    }
                    parser::ExplainKind::Opt | parser::ExplainKind::Logical => {
                        "Optimized Logical Plan"
                    }
                    parser::ExplainKind::Fragment => "Fragment",
                    parser::ExplainKind::Pipeline | parser::ExplainKind::Analyze => "Pipeline",
                    parser::ExplainKind::Physical => "Physical Plan",
                };
                match text {
                    Some(mut lines) => {
                        if matches!(kind, parser::ExplainKind::Fragment) {
                            lines = fragment_text(lines);
                        }
                        Ok(QueryResult::Rows {
                            columns: vec![column.to_string()],
                            rows: lines.into_iter().map(|l| vec![Value::Str(l)]).collect(),
                        })
                    }
                    None => {
                        if let Statement::Select { from: Some(t), .. } = inner.as_ref() {
                            self.table_schema(t)?;
                        }
                        Ok(QueryResult::Rows {
                            columns: vec![column.to_string()],
                            rows: vec![],
                        })
                    }
                }
            }
            Statement::Copy {
                table,
                from,
                path,
                opts,
            } => {
                if *from {
                    let r = self.copy_from(table, path, opts);
                    if r.is_ok() {
                        self.bump_writes(table);
                        // `NewTxn::Import` populates every index of the table
                        // inside the import transaction (`PopulateIndex` per
                        // segment), so the chunks exist as soon as the
                        // statement returns - that is how a 100-row import
                        // gets a full-text and a secondary chunk without any
                        // dump ever running. The port rebuilds its full-text,
                        // sparse and vector chunks from the rows at checkpoint
                        // time, but a secondary chunk is derived from spill
                        // files that a small import never fills, so build it
                        // here.
                        self.populate_secondary_indexes(table);
                    }
                    r
                } else {
                    self.copy_to(table, path, opts)
                }
            }
            Statement::CreateIndex {
                name,
                table,
                column,
                index_type,
                opts,
                if_not_exists,
            } => {
                self.validate_index(table, column, index_type, opts)?;
                let duplicate = self
                    .storage
                    .catalog
                    .list_indexes()?
                    .iter()
                    .any(|im| im.name == *name && im.table == *table);
                if duplicate && !*if_not_exists {
                    return Err(Error::Invalid(format!("index '{name}' already exists")));
                }
                // The command carries the bare table name; `table` is
                // qualified (`default_db.wdldl`).
                let table_name = table.rsplit('.').next().unwrap_or(table).to_string();
                let table_key = self.table_catalog_key(table)?;
                self.ddl_commit_typed(
                    TxnOp::CreateIndex {
                        table: table.clone(),
                        index: name.clone(),
                        columns: vec![column.clone()],
                    },
                    || {
                        self.storage
                            .catalog
                            .create_index_typed(name, table, column, index_type, opts)
                    },
                    |batch| {
                        let (db, _) = split_table_name(table);
                        crate::wal_bridge::create_index_command_from_batch(
                            batch,
                            &db,
                            &table_name,
                            &table_key,
                            name,
                        )
                    },
                )?;
                // remember the analyzer so fulltext matching can tokenise compatibly
                // The option name is `ANALYZER` in SQL but `analyzer` over the
                // API (the reference lower-cases every `InitParameter` there),
                // so match it case-insensitively.
                if let Some((_, a)) = opts
                    .iter()
                    .find(|(k, _)| k.eq_ignore_ascii_case("ANALYZER"))
                {
                    // C++ keeps `InitParameter::param_value_` verbatim: the
                    // name is case sensitive and its suffix carries parameters
                    // (`whitespace-<delims>`, `ngram-<n>`, `rag-fine-<lang>`).
                    let a = a.trim().trim_matches(|c| c == '\'' || c == '"').to_string();
                    let key = self.table_key(table);
                    let mut tables = self.tables.lock().unwrap();
                    let entry = tables.entry(key).or_default();
                    entry.analyzers.insert(column.clone(), a.clone());
                    entry
                        .column_index_analyzers
                        .entry(column.clone())
                        .or_default()
                        .insert(name.clone(), a.clone());
                    entry.analyzers_by_index.insert(name.clone(), a);
                }
                // a functional-index target is not a plain column, so there is
                // no materialised index to build
                let has_rows = self.table_schema(table)?.iter().any(|c| c == column);
                if is_secondary_kind(index_type) && has_rows {
                    self.build_index(table, column)?;
                }
                // C++ `IndexBuilder`: a full-text or HNSW index is materialised
                // by `CREATE INDEX`, not by the first query. Building lazily
                // moved the cost into the first query and produced an index that
                // did not exist on disk until somebody searched.
                if has_rows && index_type.eq_ignore_ascii_case("FULLTEXT") {
                    let tokenizer = self.index_tokenizer(table, column);
                    self.text_index_for(table, column, tokenizer)?;
                }
                if has_rows && index_type.eq_ignore_ascii_case("HNSW") {
                    self.build_hnsw_index(table, column)?;
                }
                // A multi-vector index is materialised when it is created, so
                // queries only read segment files; later inserts go to the
                // memory tail and `flush()` dumps it.
                if index_type.eq_ignore_ascii_case("EMVB")
                    || index_type.eq_ignore_ascii_case("PLAID")
                    || index_type.eq_ignore_ascii_case("SMVE")
                {
                    let dims = self
                        .table_schema(table)
                        .ok()
                        .and_then(|s| s.iter().position(|c| c == column))
                        .and_then(|i| self.column_types(table).ok()?.get(i).cloned())
                        .and_then(|ty| type_dim(&ty));
                    if let Some(dims) = dims {
                        // The memory index is built once, with the configs the
                        // catalog had at that moment; a second index on the same
                        // column (`USING SMVE` after `USING PLAID`) would
                        // silently reuse it. The reference keeps one object per
                        // index, so rebuild when the cached one does not know
                        // this kind.
                        self.forget_multivector_index(table, column, index_type);
                        let _ = self.multivector_index_for(table, column, dims);
                    }
                }
                // The sparse BMP index is materialised the same way.
                if index_type.eq_ignore_ascii_case("BMP") {
                    let width = self
                        .table_schema(table)
                        .ok()
                        .and_then(|s| s.iter().position(|c| c == column))
                        .and_then(|i| self.column_types(table).ok()?.get(i).cloned())
                        .map(|ty| sparse_idx_width(&ty));
                    if let Some(width) = width {
                        let _ = self.sparse_index_for(table, column, width);
                    }
                }
                // An IVF index keeps a memory segment for rows written before
                // the next mirror dump.
                if index_type.eq_ignore_ascii_case("IVF") {
                    let tkey = self.table_key(table);
                    self.tables
                        .lock()
                        .unwrap()
                        .entry(tkey)
                        .or_default()
                        .ivf_mem
                        .entry(column.clone())
                        .or_default();
                }
                // `PrepareCommitCreateIndex` -> `PopulateIndex` over every
                // segment the table already has: the first chunk of a
                // `CREATE INDEX` on a populated table is built and published
                // **inside the create-index transaction**, so its record carries
                // that transaction's ts and the later explicit `DumpMemIndex`
                // has nothing left to seal. The index is built above; this
                // publishes what it produced, stamped with the DDL's ts (read
                // back from the index's own record, whose key names it).
                if !self
                    .block_segment_row_counts(&self.table_key(table))
                    .is_empty()
                {
                    if let Ok(Some(meta)) = self.storage.catalog.get_table_by_name(table) {
                        let record = catalog::layout::index_record(
                            self.storage.catalog.store().as_ref(),
                            &self.database_id_of(table).to_string(),
                            &meta.id.to_string(),
                            name,
                        );
                        if let Ok(Some((_, create_ts))) = record {
                            self.publish_index_chunks(
                                table,
                                meta.id,
                                Some((name.as_str(), create_ts)),
                            )?;
                        }
                    }
                }
                Ok(QueryResult::CreatedIndex {
                    name: name.clone(),
                    table: table.clone(),
                    column: column.clone(),
                })
            }
            Statement::DropTable { name, if_exists } => {
                // The reference rolls the transaction back on a failing DDL,
                // and a rollback spends no timestamp: reject before the DDL
                // transaction is opened.
                if !*if_exists && self.storage.catalog.get_table_by_name(name)?.is_none() {
                    return Err(Error::Invalid(format!("table '{name}' does not exist")));
                }
                let drop_cmd = self.drop_table_command(name)?;
                let existed = self.ddl_commit_typed(
                    TxnOp::DropTable {
                        db: split_table_name(name).0,
                        table: name.clone(),
                    },
                    || self.drop_table_all(name),
                    |_batch| drop_cmd.clone(),
                )?;
                if existed {
                    self.drop_text_index_storage(name, None);
                }
                Ok(QueryResult::DroppedTable {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::DropIndex { name, if_exists } => {
                let target = self
                    .storage
                    .catalog
                    .list_indexes()
                    .unwrap_or_default()
                    .into_iter()
                    .find(|im| im.name == *name)
                    .map(|im| (im.table, im.column, im.index_type));
                if target.is_none() && !*if_exists {
                    return Err(Error::Invalid(format!("index '{name}' does not exist")));
                }
                let was_fulltext = target
                    .as_ref()
                    .map(|(_, _, t)| t.eq_ignore_ascii_case("FULLTEXT"))
                    .unwrap_or(false);
                let drop_idx_cmd = self.drop_index_command(name)?;
                let existed = self.ddl_commit_typed(
                    TxnOp::DropIndex {
                        table: target
                            .as_ref()
                            .map(|(table, _, _)| table.clone())
                            .unwrap_or_default(),
                        index: name.clone(),
                    },
                    || self.storage.catalog.drop_index(name),
                    |_batch| drop_idx_cmd,
                )?;
                if existed {
                    if let Some((table, column, _)) = target.filter(|_| was_fulltext) {
                        self.drop_text_index_storage(&table, Some(&column));
                    }
                }
                Ok(QueryResult::DroppedIndex {
                    name: name.clone(),
                    existed,
                })
            }
            Statement::InsertSelect {
                table,
                columns,
                query,
            } => self.insert_from_select(table, columns.as_deref(), query),
            // The reference has no `CREATE TABLE … AS SELECT`: the statement
            // reaches its binder as a table definition without columns and
            // fails there (`Status::NoColumnDefined`,
            // `query_context_impl.cpp:238`). Rejecting it with the same message
            // keeps the two engines indistinguishable to a client.
            Statement::CreateTableAs { name, .. } => {
                // `CreateTableInfo::table_name_`, not the qualified name.
                let table = name.rsplit('.').next().unwrap_or(name.as_str());
                Err(Error::NoColumnDefined(format!(
                    "Try to define Table {table} without any column."
                )))
            }
            // The reference has no view implementation: `PhysicalCreateView` is
            // an empty stub and the DDL dispatch does not know the statement,
            // so both statements die in
            // `UnrecoverableException("Unknown DDL type")`
            // (`query_context_impl.cpp:467` for CREATE, `:489` for DROP). Just
            // as with CTAS, the port reports the same failure instead of
            // keeping a mechanism the reference does not have.
            Statement::CreateView { .. } | Statement::DropView { .. } => {
                Err(Error::Unrecoverable("Unknown DDL type".to_string()))
            }
            Statement::Describe { table } => self.describe_table(table),
            Statement::Utility { .. } => Ok(QueryResult::Rows {
                columns: vec![],
                rows: vec![],
            }),
            Statement::SetConfig { key, value } => {
                // a known key with an out-of-range/malformed value is rejected
                if let Some(msg) = invalid_config_value(key, value) {
                    return Err(Error::Invalid(msg));
                }
                // cache knobs cannot be changed while the cache is off
                if key.to_ascii_lowercase().starts_with("cache_result")
                    && value != "off"
                    && !self.result_cache_enabled()
                {
                    return Err(Error::Invalid(format!(
                        "config '{key}' is unavailable while result_cache is off"
                    )));
                }
                self.context
                    .config
                    .lock()
                    .unwrap()
                    .insert(key.to_ascii_lowercase(), value.clone());
                // `SET CONFIG result_cache "clear"` empties the cache.
                if key.eq_ignore_ascii_case("result_cache") && value.eq_ignore_ascii_case("clear") {
                    self.result_cache.lock().unwrap().clear();
                    self.plan_cache.clear();
                }
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::ShowVariable { key } => {
                let cache_var = key.to_ascii_lowercase();
                if cache_var.starts_with("cache_result") && !self.result_cache_enabled() {
                    return Err(Error::Invalid(format!(
                        "variable '{key}' is unavailable while result_cache is off"
                    )));
                }
                let v = if cache_var == "cache_result_num" {
                    self.result_cache.lock().unwrap().len().to_string()
                } else {
                    self.context
                        .config
                        .lock()
                        .unwrap()
                        .get(&key.to_ascii_lowercase())
                        .cloned()
                        .unwrap_or_else(|| "0".to_string())
                };
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![vec![Value::Str(v)]],
                })
            }
            Statement::ShowTableColumns { table } => {
                let meta = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                let rows: Vec<Vec<String>> = (0..meta.columns.len())
                    .map(|i| {
                        let ty = meta.types.get(i).cloned().unwrap_or_default();
                        vec![
                            meta.columns[i].clone(),
                            describe_type(&ty),
                            render_column_default(
                                meta.defaults.get(i).and_then(|d| d.as_ref()),
                                &ty,
                            ),
                            meta.comments
                                .get(i)
                                .cloned()
                                .flatten()
                                .unwrap_or_else(|| "(empty)".to_string()),
                        ]
                    })
                    .collect();
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: text_rows(rows),
                })
            }
            // `SHOW TABLE <t>`: two columns (`name`, `value`) with the six
            // label/value rows from `physical_show_table_impl.cpp`.
            Statement::ShowTable { table } => {
                let meta = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
                let (db, name) = table
                    .split_once('.')
                    .unwrap_or(("default_db", table.as_str()));
                Ok(QueryResult::Rows {
                    columns: vec!["name".to_string(), "value".to_string()],
                    rows: text_rows(vec![
                        vec!["database_name".to_string(), db.to_string()],
                        vec!["table_name".to_string(), name.to_string()],
                        vec!["table_comment".to_string(), String::new()],
                        vec!["storage_directory".to_string(), String::new()],
                        vec!["column_count".to_string(), meta.columns.len().to_string()],
                        vec!["segment_count".to_string(), "0".to_string()],
                    ]),
                })
            }
            Statement::CreateSnapshot { name, table } => {
                self.create_table_snapshot(&self.db(), table, name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::CreateDatabaseSnapshot { name, database } => {
                self.create_database_snapshot(name, database)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::CreateSystemSnapshot { name } => {
                self.create_system_snapshot(name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::DropSnapshot { name } => {
                self.drop_snapshot(name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::RestoreSnapshot { name } => {
                self.restore_table_snapshot(name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::RestoreDatabaseSnapshot { name } => {
                self.restore_database_snapshot(name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::RestoreSystemSnapshot { name } => {
                self.restore_system_snapshot(name)?;
                Ok(QueryResult::Rows {
                    columns: vec![],
                    rows: vec![],
                })
            }
            Statement::Insert {
                table,
                columns,
                rows,
            } => self.insert_literals(table, columns.as_deref(), rows.clone()),
            Statement::Delete { table, where_pred } => {
                // A WHERE clause must be a predicate: `DELETE FROM t WHERE c1` on
                // a non-boolean column is rejected by the reference.
                if let Some(p) = where_pred.as_ref() {
                    self.validate_where_predicate(table, p)?;
                }
                // A subquery in the filter is evaluated once, before the scan.
                let where_pred = match where_pred {
                    Some(p) => Some(self.resolve_subquery_pred(p, &HashMap::new(), &[])?),
                    None => None,
                };
                let schema = self.table_schema(table)?;
                let pred = match &where_pred {
                    Some(p) => Some(planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                for (seq, row) in self.rows_for_predicate(table, where_pred.as_ref())? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        to_del.push(seq);
                    }
                }
                if !to_del.is_empty() {
                    let t = self.txn.begin();
                    self.commit_with_deletes(table, &to_del, 0, &[], t)?;
                    self.drop_text_index_rows(table, &to_del);
                    self.drop_vector_cache_rows(table, &to_del);
                }
                Ok(QueryResult::Deleted {
                    count: to_del.len(),
                })
            }
            Statement::Update {
                table,
                assignments,
                where_pred,
            } => {
                // The reference requires a filter on UPDATE, and it has to be a
                // predicate (`WHERE c1` on a non-boolean column is rejected).
                if where_pred.is_none() {
                    return Err(Error::Invalid(
                        "syntax: UPDATE requires a WHERE clause".into(),
                    ));
                }
                if let Some(p) = where_pred.as_ref() {
                    self.validate_where_predicate(table, p)?;
                }
                let where_pred = match where_pred {
                    Some(p) => Some(self.resolve_subquery_pred(p, &HashMap::new(), &[])?),
                    None => None,
                };
                let schema = self.table_schema(table)?;
                let types = self.column_types(table)?;
                let not_nulls = self
                    .storage
                    .catalog
                    .get_table_by_name(table)?
                    .map(|m| m.not_nulls)
                    .unwrap_or_default();
                let mut asg = Vec::new();
                for (c, e) in assignments {
                    let idx = planner::resolve_column(&schema, c)?;
                    // A boolean column only takes true/false, not 1/0.
                    let ty = types
                        .get(idx)
                        .map(|t| t.to_ascii_uppercase())
                        .unwrap_or_default();
                    if ty.starts_with("BOOLEAN") || ty.starts_with("BOOL") {
                        if let parser::ExprAst::Literal(l) = e {
                            use parser::Literal as L;
                            if matches!(l, L::Int(_) | L::Float(_)) {
                                return Err(Error::Invalid(
                                    "parser: Boolean type should be true or false".into(),
                                ));
                            }
                        }
                    }
                    // An array value cannot be stored in a scalar column.
                    if matches!(e, parser::ExprAst::Literal(parser::Literal::Array(_))) {
                        let structured = ty.starts_with("ARRAY")
                            || ty.starts_with("VECTOR")
                            || ty.starts_with("EMBEDDING")
                            || ty.starts_with("TENSOR")
                            || ty.starts_with("MULTIVECTOR")
                            || ty.starts_with("SPARSE")
                            || ty.starts_with("JSON")
                            || ty.starts_with("VARCHAR")
                            || ty.starts_with("TEXT");
                        if !structured {
                            return Err(Error::Invalid(format!(
                                "not supported type conversion: an array value cannot be stored in column '{c}'"
                            )));
                        }
                    }
                    if not_nulls.get(idx).copied().unwrap_or(false)
                        && matches!(e, parser::ExprAst::Literal(parser::Literal::Null))
                    {
                        return Err(Error::Invalid(format!(
                            "null value in NOT NULL column '{c}'"
                        )));
                    }
                    asg.push((idx, planner::compile_expr(e, &schema)?));
                }
                let pred = match &where_pred {
                    Some(p) => Some(planner::predicate_expr(p, &schema)?),
                    None => None,
                };
                let mut to_del = Vec::new();
                let mut new_rows = Vec::new();
                for (seq, row) in self.rows_for_predicate(table, where_pred.as_ref())? {
                    let matches = match &pred {
                        Some(e) => e.eval_predicate(&row),
                        None => true,
                    };
                    if matches {
                        let mut nr = row.clone();
                        for (idx, expr) in &asg {
                            if *idx < nr.len() {
                                nr[*idx] = expr.eval(&row);
                            }
                        }
                        to_del.push(seq);
                        new_rows.push(nr);
                    }
                }
                if !to_del.is_empty() {
                    // The reference deletes the old row and appends the
                    // rewritten one, so an updated row moves to the end and
                    // gets a fresh ROW_ID (`test_update` asserts that order).
                    // The id itself is allocated inside the commit
                    // (`SubmitForAllocation` runs between the conflict check and
                    // the WAL entry), so `base` only exists in `prepare`.
                    let t = self.txn.begin();
                    // The rewritten rows are handed to the block writer
                    // (`AppendInBlock`), not staged as `row:` keys: an UPDATE is
                    // the reference's delete-plus-append, and the appended rows
                    // live in the block like any other.
                    // The reference emits one frame holding the delete and then
                    // the append, both typed (`NewTxn::Update` -> `DeleteRows`
                    // then `Append`). The port's `build_commands` would order
                    // them the other way round and encode the rewritten rows as
                    // text, so both commands are supplied here.
                    let literals: Vec<Vec<Literal>> = new_rows
                        .iter()
                        .map(|r| r.iter().map(|v| value_to_literal(v.clone())).collect())
                        .collect();
                    let delete_ids = to_del.clone();
                    let types = types.clone();
                    let block = typed_append_block(&literals, &types);
                    let table_name = table.to_string();
                    let table_ident = self.table_ident(table)?;
                    let row_count = literals.len();
                    let base = self.commit_rewritten_rows(
                        table,
                        &to_del,
                        &new_rows,
                        t,
                        move |txn, base| {
                            if let Some((_, table_id, _)) = table_ident {
                                // A row the WAL encoder cannot express keeps
                                // its `DeleteV2` only: the new cells reach the
                                // block buffer, and no row record is kept.
                                if let Some(block) = block {
                                    txn.set_typed_commands(vec![
                                        self.delete_command(&table_name, &table_id, &delete_ids),
                                        self.append_command(
                                            &table_name,
                                            &table_id,
                                            base,
                                            row_count,
                                            block,
                                        ),
                                    ]);
                                }
                            }
                            Ok(())
                        },
                    )?;
                    // A table with no index has nothing to feed: rendering every
                    // cell to text for the hooks cost a `format!` per cell on
                    // every rewritten row.
                    if self.has_index_declaration(table) || self.any_in_memory_index() {
                        let schema = self.table_schema(table).unwrap_or_default();
                        self.update_secondary_indexes_batch(table, base, &new_rows, &schema);
                        // `PrepareCommitUpdate` commits delete + append, so every
                        // index kind follows the rewritten rows - the secondary
                        // tail included (`AppendIndex` dispatches over them all).
                        if self.table_has_memory_index(table) || self.has_index_declaration(table) {
                            for (k, nr) in new_rows.iter().enumerate() {
                                let row_texts: Vec<String> =
                                    nr.iter().map(|v| v.to_slt_string()).collect();
                                self.update_indexes_on_insert_text(
                                    table,
                                    base + k,
                                    &row_texts,
                                    &schema,
                                )?;
                                self.update_text_indexes(table, base + k, &row_texts, &schema);
                                self.update_vector_caches(table, base + k, &row_texts);
                            }
                            self.drop_secondary_rows(table, &to_del);
                            self.drop_text_index_rows(table, &to_del);
                            self.drop_vector_cache_rows(table, &to_del);
                        }
                    }
                }
                Ok(QueryResult::Updated {
                    count: to_del.len(),
                })
            }
            Statement::Select { .. } => {
                // The rows stay `Value`s: rendering every cell to text here only
                // for the protocol layer to parse it back was ~40% of a scan's
                // server time (`docs/SELECT_CHAIN.md`).
                let (columns, rows) = self.run_select(stmt)?;
                Ok(QueryResult::Rows { columns, rows })
            }
        }
    }

    /// Runs an optimized plan.
    ///
    /// `source_columns` is the unpruned column list the caller materialised its
    /// rows from (C++ keeps that binding in the scan and remaps down to the
    /// pruned list); `candidate` is the index scan the planner selected for this
    /// scan, which the scan operator applies before handing rows to the chain.
    #[allow(clippy::too_many_arguments)] // one argument per planner hint the plan carries
    pub(crate) fn run_plan(
        &self,
        plan: LogicalPlan,
        mut override_rows: Option<Vec<Vec<Value>>>,
        source_columns: Option<Vec<String>>,
        candidate: Option<planner::index_scan::IndexScanCandidate>,
        unmatchable: bool,
        fulltext_substring: bool,
        catalog: Option<planner::index_scan::IndexCatalog>,
        rough_filter_done: bool,
    ) -> Result<(Vec<String>, Vec<Vec<Value>>)> {
        // The index filter was built over the scan layout that still contains
        // the pushed-down columns; remember how those map onto the row before
        // `LazyLoad` drops them (C++ keeps the column bound in the index scan).
        let index_positions: Option<Vec<usize>> =
            candidate
                .as_ref()
                .and_then(|_| match source_columns.as_ref() {
                    Some(full) => plan_scan_of(&plan).map(|(_, columns)| {
                        columns
                            .iter()
                            .map(|column| {
                                full.iter()
                                    .position(|name| name == column)
                                    .unwrap_or(usize::MAX)
                            })
                            .collect()
                    }),
                    None => None,
                });
        // The column a full-text index filter reads, if the candidate has one:
        // its inverted index is what the evaluator must query.
        let fulltext_index_column: Option<String> = candidate
            .as_ref()
            .and_then(|candidate| fulltext_column(&candidate.index_filter))
            .and_then(|i| plan_scan_of(&plan).and_then(|(_, columns)| columns.get(i).cloned()));
        // The production optimizer: the C++ rule order (`optimizer_impl.cpp`).
        // The first two rules ran before the range lookup; the bound plan was
        // already re-stacked by the binder (`optimize::normalize`).
        // The first two rules ran before the index-scan range lookup (the
        // candidate has to exist there), so this pass starts at the pruner.
        let optimizer = match (catalog, rough_filter_done) {
            (_, true) => planner::optimize::Optimizer::after_rough_filter(),
            (Some(catalog), false) => planner::optimize::Optimizer::with_catalog(catalog, None),
            (None, false) => planner::optimize::Optimizer::with_default_rules(),
        };
        let plan = optimizer.optimize(plan)?;
        // `ResultCacheGetter` is registered last in the reference's optimizer
        // list, so it keys the *rewritten* nodes the scan actually produces.
        let begin_ts = self.current_ts();
        let cacheable = self.result_cache_active();
        let plan = if cacheable {
            planner::result_cache::ResultCacheGetter::rewrite(
                plan,
                self.plan_cache.as_ref(),
                begin_ts,
            )
        } else {
            plan
        };
        #[cfg(test)]
        LAST_PLAN_DEBUG.with(|slot| *slot.borrow_mut() = Some(format!("{plan:?}")));
        // A projection over a plain scan is a column selection (plus, at most, a
        // row predicate): answer it here, where every row costs one `Value`
        // clone instead of a trip through the `DataChunk` machinery.
        // A cached plan goes through the operators: the cache node the
        // optimizer inserted is what answers the repeat, and it only runs in
        // the operator chain.
        if !cacheable && candidate.is_none() && !unmatchable && !fulltext_substring {
            let mut rows = override_rows.take();
            if let Some(out) = self.direct_projection(&plan, &mut rows, source_columns.as_ref()) {
                return Ok(out);
            }
            override_rows = rows;
        }
        // Lower the plan to the operator chain and build the fragment's source.
        // `lower` is total: every `LogicalPlan` node has an operator.
        let crate::operator::lower::Lowered {
            operators,
            names,
            source,
        } = crate::operator::lower::lower(&plan)?;
        let spec = match source {
            crate::operator::lower::SourceKind::Values { columns, rows } => {
                crate::operator::scan::SourceSpec::Values { columns, rows }
            }
            crate::operator::lower::SourceKind::Table { table, columns } => {
                // The block prefilter answers the predicate's range/equality
                // comparisons: they are what a min/max record can refute.
                let prune = scan_predicate_of(&plan)
                    .map(|pred| prune_conditions(&pred, &columns))
                    .unwrap_or_default();
                crate::operator::scan::SourceSpec::Table(Box::new(
                    crate::operator::scan::TableScan {
                        table,
                        columns,
                        source_columns,
                        override_rows,
                        candidate,
                        index_positions,
                        fulltext_index_column,
                        fulltext_substring,
                        prune,
                    },
                ))
            }
            crate::operator::lower::SourceKind::Join {
                columns,
                left,
                right,
                left_columns,
                right_columns,
                left_key,
                right_key,
                left_width,
            } => {
                crate::operator::scan::SourceSpec::Join(Box::new(crate::operator::scan::JoinScan {
                    columns,
                    left,
                    right,
                    left_columns,
                    right_columns,
                    left_key,
                    right_key,
                    left_width,
                    left_rows: override_rows,
                    left_source_columns: source_columns,
                }))
            }
        };
        let mut source = crate::operator::scan::PhysicalSource::new(
            self,
            spec,
            crate::chunk::DEFAULT_VECTOR_SIZE,
        );
        source.set_unmatchable(unmatchable);
        crate::operator::run_operators(operators, names, &mut source)
    }
}

/// True when a `filter_text`/`filter_fulltext` call appears anywhere in the
/// predicate. Such a call is answered by a full-text index and may name one with
/// `column@index`; the columnar fast path evaluates the predicate row by row,
/// which drops the hint (and with it the chosen analyzer).
fn has_fulltext_call(p: &Predicate) -> bool {
    match p {
        Predicate::IsTrue(e) => expr_has_fulltext_call(e),
        Predicate::Cmp(l, _, r) => expr_has_fulltext_call(l) || expr_has_fulltext_call(r),
        Predicate::IsNull(e, _) => expr_has_fulltext_call(e),
        Predicate::Like(l, r, _, _) => expr_has_fulltext_call(l) || expr_has_fulltext_call(r),
        Predicate::In(e, list, _) => {
            expr_has_fulltext_call(e) || list.iter().any(expr_has_fulltext_call)
        }
        Predicate::Bool(l, _, r) => has_fulltext_call(l) || has_fulltext_call(r),
        _ => false,
    }
}

fn expr_has_fulltext_call(e: &ExprAst) -> bool {
    match e {
        ExprAst::Func { name, args } => {
            name.eq_ignore_ascii_case("filter_text")
                || name.eq_ignore_ascii_case("filter_fulltext")
                || args.iter().any(expr_has_fulltext_call)
        }
        ExprAst::Binary(l, _, r) | ExprAst::Cmp(l, _, r) => {
            expr_has_fulltext_call(l) || expr_has_fulltext_call(r)
        }
        ExprAst::And(l, r) | ExprAst::Or(l, r) => {
            expr_has_fulltext_call(l) || expr_has_fulltext_call(r)
        }
        ExprAst::Not(x) => expr_has_fulltext_call(x),
        _ => false,
    }
}

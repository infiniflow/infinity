//! Correlated subqueries as a dependent join.
//!
//! The reference's planner rewrites a correlated subquery into a mark join
//! (`SubqueryUnnest::unnest_subqueries`, `DependentJoinFlattener`,
//! `RewriteCorrelatedExpressions` -> `LogicalJoin{JoinType::kMark}`), but it has
//! no operator for the shape it builds: the C++ server aborts in
//! `FragmentTask::OnExecute` on `SELECT (SELECT max(x) FROM s WHERE s.a = o.a)
//! FROM o`, and answers `Unknown error` for the `WHERE` form, so there is no
//! reference *behaviour* to match. This module keeps the reference's model
//! without the missing operator:
//!
//! 1. `QueryBinder::bind_subquery` says which outer columns a nested query
//!    reads (`CorrelatedExpressionsDetector`), exactly as the reference does;
//! 2. the nested query becomes a hidden scan column `#_corr_<n>`, so `WHERE`,
//!    `HAVING`, `ORDER BY` and the projection all evaluate it through the
//!    ordinary column path (the same mechanism the search and `ROW_ID()`
//!    pseudo-columns use);
//! 3. the inner side runs **once per distinct key** and the result is joined
//!    back onto the outer rows - a dependent join with a cache, not one
//!    execution per outer row.
//!
//! Nested queries that read nothing from the enclosing query are left to the
//! constant folder ([`Engine::resolve_stmt_subqueries`]).

use std::collections::HashMap;

use ::common::{Error, Result};
use expression::Value;
use parser::{CmpOp, ExprAst, Literal, Predicate, SelectItem, Statement};

use crate::{subquery_sql_arg, substitute_outer_row};

// How many times the inner side was executed. The dependent join runs it once
// per distinct key, so a test can pin "not once per outer row" without timing
// anything.
#[cfg(test)]
thread_local! {
    pub(crate) static INNER_RUNS: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
}

#[cfg(test)]
fn note_inner_run() {
    INNER_RUNS.with(|runs| runs.set(runs.get() + 1));
}

/// One hidden column of the dependent join.
struct DependentColumn {
    /// `#_corr_<n>`: the column appended to the scan.
    name: String,
    /// The nested query, with the outer values still to be substituted.
    sql: String,
    /// The outer columns it reads: `(column name, outer schema index)`.
    keys: Vec<(String, usize)>,
    kind: Kind,
}

enum Kind {
    /// `(SELECT …)`: one value, `NULL` when the query returns no row.
    Scalar,
    /// `EXISTS (SELECT …)`: `1`/`0`.
    Exists,
    /// `x IN (SELECT …)` / `NOT IN`: the cached rows are probed with the
    /// row's own left operand, which is not part of the correlated key.
    In { left: Box<ExprAst>, negated: bool },
}

/// A `SELECT` whose correlated subqueries are hidden columns, plus the recipes
/// to fill those columns in.
pub(crate) struct DependentJoin {
    pub(crate) stmt: Statement,
    columns: Vec<DependentColumn>,
}

impl DependentJoin {
    /// The hidden column names, in the order [`Self::apply`] appends them.
    pub(crate) fn column_names(&self) -> Vec<String> {
        self.columns.iter().map(|c| c.name.clone()).collect()
    }

    /// Evaluates every column for every row and appends the values.
    ///
    /// `schema` is the outer relation's column list (before the hidden columns
    /// are appended); `keys` in each column index into it. The inner query is
    /// executed once per distinct key tuple and the rows are reused, which is
    /// what makes this a dependent join rather than a per-row re-execution.
    pub(crate) fn apply(
        &self,
        engine: &crate::Engine,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer_table: &str,
        schema: &[String],
        rows: &mut [Vec<Value>],
    ) -> Result<()> {
        let mut caches: Vec<HashMap<String, Result<Vec<Vec<Value>>>>> =
            (0..self.columns.len()).map(|_| HashMap::new()).collect();
        let mut appended: Vec<Vec<Value>> =
            vec![Vec::with_capacity(rows.len()); self.columns.len()];
        for row in rows.iter() {
            for (index, column) in self.columns.iter().enumerate() {
                let value =
                    column.value(engine, ctes, outer_table, schema, row, &mut caches[index])?;
                appended[index].push(value);
            }
        }
        for (row, index) in rows.iter_mut().zip(0..) {
            for values in &appended {
                row.push(values[index].clone());
            }
        }
        Ok(())
    }
}

impl DependentColumn {
    fn value(
        &self,
        engine: &crate::Engine,
        ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
        outer_table: &str,
        schema: &[String],
        row: &[Value],
        cache: &mut HashMap<String, Result<Vec<Vec<Value>>>>,
    ) -> Result<Value> {
        let key: String = self
            .keys
            .iter()
            .map(|(_, index)| format!("{:?}", row.get(*index).unwrap_or(&Value::Null)))
            .collect::<Vec<_>>()
            .join("\u{1f}");
        if !cache.contains_key(&key) {
            // Only the correlated columns are visible to the substitution, so
            // a sparse row with the rest left NULL is enough.
            let mut key_row = vec![Value::Null; schema.len()];
            for (_, index) in &self.keys {
                if let Some(value) = row.get(*index) {
                    key_row[*index] = value.clone();
                }
            }
            let sql = substitute_outer_row(&self.sql, &self.keys, &key_row, outer_table);
            #[cfg(test)]
            note_inner_run();
            let result = engine.run_subquery(&sql, ctes, &[]).map(|(_, rows)| rows);
            cache.insert(key.clone(), result);
        }
        let rows = cache
            .get(&key)
            .expect("just inserted")
            .as_ref()
            .map_err(|error| Error::Invalid(format!("{error}")))?;
        match &self.kind {
            Kind::Scalar => {
                if rows.len() > 1 {
                    return Err(Error::Invalid(format!(
                        "scalar subquery returned more than one row ({} rows)",
                        rows.len()
                    )));
                }
                Ok(rows
                    .first()
                    .and_then(|r| r.first())
                    .cloned()
                    .unwrap_or(Value::Null))
            }
            Kind::Exists => Ok(Value::Int(i64::from(!rows.is_empty()))),
            Kind::In { left, negated } => {
                let compiled = planner::compile_expr(left, schema)?;
                let probe = compiled.eval(row);
                let found = rows
                    .iter()
                    .filter_map(|r| r.first())
                    .any(|value| *value == probe);
                Ok(Value::Int(i64::from(found != *negated)))
            }
        }
    }
}

/// Rewrites every correlated subquery of `stmt` into a hidden column.
///
/// Returns `None` when nothing is correlated, in which case the statement is
/// left untouched and the constant folder keeps its current behaviour.
pub(crate) fn plan(
    engine: &crate::Engine,
    stmt: &Statement,
    outer: &[String],
    ctes: &HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
) -> Result<Option<DependentJoin>> {
    let mut builder = Builder {
        engine,
        outer,
        ctes,
        columns: Vec::new(),
    };
    let rewritten = builder.stmt(stmt)?;
    if builder.columns.is_empty() {
        return Ok(None);
    }
    Ok(Some(DependentJoin {
        stmt: rewritten,
        columns: builder.columns,
    }))
}

struct Builder<'a> {
    engine: &'a crate::Engine,
    outer: &'a [String],
    ctes: &'a HashMap<String, (Vec<String>, Vec<Vec<Value>>)>,
    columns: Vec<DependentColumn>,
}

impl Builder<'_> {
    /// The hidden column for one nested query; `None` when the query is not
    /// correlated (it stays where it is and the constant folder folds it).
    fn column(&mut self, sql: &str, kind: Kind) -> Result<Option<String>> {
        let Some(keys) = self
            .engine
            .correlated_sql_columns(sql, self.outer, self.ctes)?
        else {
            return Ok(None);
        };
        let name = format!("#_corr_{}", self.columns.len());
        self.columns.push(DependentColumn {
            name: name.clone(),
            sql: sql.to_string(),
            keys,
            kind,
        });
        Ok(Some(name))
    }

    fn stmt(&mut self, stmt: &Statement) -> Result<Statement> {
        let Statement::Select {
            items,
            where_pred,
            group_by,
            having,
            order_by,
            ..
        } = stmt
        else {
            return Ok(stmt.clone());
        };
        let mut out = stmt.clone();
        let Statement::Select {
            items: o_items,
            where_pred: o_where,
            group_by: o_group,
            having: o_having,
            order_by: o_order,
            ..
        } = &mut out
        else {
            unreachable!()
        };
        // Only an aggregating query restricts what a projected column may read;
        // a plain scan evaluates the hidden column per row.
        let grouped = !group_by.is_empty()
            || items
                .iter()
                .any(|item| matches!(item, SelectItem::Aggregate(_)));
        for (item, out) in items.iter().zip(o_items.iter_mut()) {
            let before = self.columns.len();
            *out = self.item(item)?;
            // A grouped query may only read a column that the grouping
            // determines. A hidden column is a function of the outer columns the
            // nested query reads, so it can join `GROUP BY` without changing the
            // groups - but only when those outer columns are themselves grouped.
            if grouped && !matches!(item, SelectItem::Aggregate(_)) {
                self.group_the_columns(before, group_by, o_group)?;
            }
        }
        if let Some(pred) = where_pred {
            *o_where = Some(self.pred(pred)?);
        }
        for (expr, out) in group_by.iter().zip(o_group.iter_mut()) {
            *out = self.expr(expr)?;
        }
        if let Some(pred) = having.as_deref() {
            let before = self.columns.len();
            *o_having = Some(Box::new(self.pred(pred)?));
            if grouped {
                self.group_the_columns(before, group_by, o_group)?;
            }
        }
        for (key, out) in order_by.iter().zip(o_order.iter_mut()) {
            if let Some(expr) = key.expr.as_ref() {
                let before = self.columns.len();
                out.expr = Some(self.expr(expr)?);
                if grouped {
                    self.group_the_columns(before, group_by, o_group)?;
                }
            }
        }
        Ok(out)
    }

    /// Adds the hidden columns created since `before` to `GROUP BY`, or refuses
    /// when the columns they depend on are not grouped: the value would then
    /// vary inside one group, and a grouped query has no single answer.
    fn group_the_columns(
        &self,
        before: usize,
        group_by: &[ExprAst],
        out: &mut Vec<ExprAst>,
    ) -> Result<()> {
        for column in &self.columns[before..] {
            if group_by.is_empty() {
                return Err(Error::Invalid(format!(
                    "correlated subquery in a grouped query must depend on the grouping columns ({})",
                    column.name
                )));
            }
            let grouped = column.keys.iter().all(|(name, _)| {
                group_by
                    .iter()
                    .any(|key| matches!(key, ExprAst::Column(c) if c.eq_ignore_ascii_case(name)))
            });
            if !grouped {
                return Err(Error::Invalid(format!(
                    "correlated subquery in a grouped query must depend on the grouping columns ({})",
                    column.name
                )));
            }
            out.push(ExprAst::Column(column.name.clone()));
        }
        Ok(())
    }

    fn item(&mut self, item: &SelectItem) -> Result<SelectItem> {
        Ok(match item {
            SelectItem::Expr(expr) => SelectItem::Expr(self.expr(expr)?),
            SelectItem::Func { name, args } => SelectItem::Func {
                name: name.clone(),
                args: args
                    .iter()
                    .map(|arg| self.expr(arg))
                    .collect::<Result<Vec<_>>>()?,
            },
            SelectItem::Aggregate(agg) => {
                let mut agg = agg.clone();
                for arg in agg.arg_exprs.iter_mut().flatten() {
                    *arg = self.expr(arg)?;
                }
                SelectItem::Aggregate(agg)
            }
            other => other.clone(),
        })
    }

    fn expr(&mut self, expr: &ExprAst) -> Result<ExprAst> {
        Ok(match expr {
            ExprAst::Func { name, args } if name.eq_ignore_ascii_case("SUBQUERY") => {
                let sql = subquery_sql_arg(args)?.to_string();
                match self.column(&sql, Kind::Scalar)? {
                    Some(column) => ExprAst::Column(column),
                    None => expr.clone(),
                }
            }
            ExprAst::Func { name, args } if name.eq_ignore_ascii_case("EXISTS") => {
                let sql = crate::exists_subquery_sql(args)?.to_string();
                match self.column(&sql, Kind::Exists)? {
                    Some(column) => ExprAst::Column(column),
                    None => expr.clone(),
                }
            }
            ExprAst::Func { name, args } => ExprAst::Func {
                name: name.clone(),
                args: args
                    .iter()
                    .map(|arg| self.expr(arg))
                    .collect::<Result<Vec<_>>>()?,
            },
            ExprAst::Binary(a, op, b) => {
                ExprAst::Binary(Box::new(self.expr(a)?), *op, Box::new(self.expr(b)?))
            }
            ExprAst::Cmp(a, op, b) => {
                ExprAst::Cmp(Box::new(self.expr(a)?), *op, Box::new(self.expr(b)?))
            }
            ExprAst::Not(a) => ExprAst::Not(Box::new(self.expr(a)?)),
            ExprAst::And(a, b) => ExprAst::And(Box::new(self.expr(a)?), Box::new(self.expr(b)?)),
            ExprAst::Or(a, b) => ExprAst::Or(Box::new(self.expr(a)?), Box::new(self.expr(b)?)),
            ExprAst::Column(_) | ExprAst::Literal(_) => expr.clone(),
        })
    }

    fn pred(&mut self, pred: &Predicate) -> Result<Predicate> {
        Ok(match pred {
            // `EXISTS (SELECT …)` in predicate position: the hidden column
            // holds 1/0, compared so the predicate stays a comparison (which
            // the planner's `WHERE` validation expects).
            Predicate::IsTrue(ExprAst::Func { name, args })
                if name.eq_ignore_ascii_case("EXISTS") =>
            {
                let sql = crate::exists_subquery_sql(args)?.to_string();
                match self.column(&sql, Kind::Exists)? {
                    Some(column) => Predicate::Cmp(
                        ExprAst::Column(column),
                        CmpOp::Eq,
                        ExprAst::Literal(Literal::Int(1)),
                    ),
                    None => pred.clone(),
                }
            }
            // `x IN (SELECT …)`: the inner rows are cached per key, the
            // membership test runs per row.
            Predicate::In(left, values, negated)
                if values.len() == 1 && is_subquery(&values[0]) =>
            {
                let sql = subquery_sql_arg(match &values[0] {
                    ExprAst::Func { args, .. } => args,
                    _ => unreachable!("checked by is_subquery"),
                })?
                .to_string();
                let kind = Kind::In {
                    left: Box::new(self.expr(left)?),
                    negated: *negated,
                };
                match self.column(&sql, kind)? {
                    Some(column) => Predicate::Cmp(
                        ExprAst::Column(column),
                        CmpOp::Eq,
                        ExprAst::Literal(Literal::Int(1)),
                    ),
                    None => pred.clone(),
                }
            }
            Predicate::Cmp(a, op, b) => Predicate::Cmp(self.expr(a)?, *op, self.expr(b)?),
            Predicate::IsTrue(expr) => Predicate::IsTrue(self.expr(expr)?),
            Predicate::IsNull(expr, negated) => Predicate::IsNull(self.expr(expr)?, *negated),
            Predicate::Like(a, pattern, escape, negated) => {
                Predicate::Like(self.expr(a)?, self.expr(pattern)?, *escape, *negated)
            }
            Predicate::In(a, values, negated) => Predicate::In(
                self.expr(a)?,
                values
                    .iter()
                    .map(|value| self.expr(value))
                    .collect::<Result<Vec<_>>>()?,
                *negated,
            ),
            Predicate::Bool(a, op, b) => {
                Predicate::Bool(Box::new(self.pred(a)?), *op, Box::new(self.pred(b)?))
            }
            Predicate::Not(a) => Predicate::Not(Box::new(self.pred(a)?)),
        })
    }
}

fn is_subquery(expr: &ExprAst) -> bool {
    matches!(expr, ExprAst::Func { name, .. } if name.eq_ignore_ascii_case("SUBQUERY"))
}

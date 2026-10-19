//! Query binding: turns parsed statement fragments into a logical plan.
//!
//! C++ `QueryBinder` also binds INSERT/DELETE/UPDATE/COMPACT statements and
//! table refs. In the Rust engine those statements are executed directly by
//! `infinity-executor` (their bound forms are not represented as a
//! `LogicalPlan`), so the binder here covers the SELECT path: FROM/JOIN table
//! refs, the select list, predicates and the clause ordering. It is the public
//! entry point that `plan_select` itself is built on.

use infinity_common::{Error, Result};
use infinity_expression::{Expr, Value};
use infinity_parser::{BoolOp, ExprAst, JoinClause, Literal, OrderKey, Predicate, SelectItem};

use crate::subquery::{
    BoundSubquery, CorrelatedExpressionsDetector, DepthColumn, SubqueryExpr, SubqueryPlan,
    SubqueryType,
};
use crate::{output_schema, plan_select, resolve_column, ColumnBinding, LogicalPlan};

/// C++ `BoundDeleteStatement`: the target table and the bound predicate.
#[derive(Debug, Clone, PartialEq)]
pub struct BoundDeleteStatement {
    pub table: String,
    pub predicate: Option<Expr>,
}

/// C++ `BoundUpdateStatement`: table, bound `(column, value)` assignments and
/// the bound predicate.
#[derive(Debug, Clone, PartialEq)]
pub struct BoundUpdateStatement {
    pub table: String,
    pub assignments: Vec<(usize, Expr)>,
    pub predicate: Option<Expr>,
}

/// C++ `BoundInsertStatement`: resolved target columns and literal rows.
#[derive(Debug, Clone, PartialEq)]
pub struct BoundInsertStatement {
    pub table: String,
    pub columns: Vec<String>,
    pub rows: Vec<Vec<Value>>,
}

/// C++ `BoundSelectStatement`: the select plan plus its output names.
#[derive(Debug, Clone, PartialEq)]
pub struct BoundSelectStatement {
    pub plan: LogicalPlan,
    pub output_names: Vec<String>,
}

impl BoundSelectStatement {
    fn new(plan: LogicalPlan) -> Self {
        Self {
            output_names: output_schema(&plan),
            plan,
        }
    }
}

/// C++ `QueryBinder`.
#[derive(Debug, Default, Clone, Copy)]
pub struct QueryBinder;

impl QueryBinder {
    pub fn new() -> Self {
        Self
    }

    /// C++ `BindSelect`: binds a parsed `SELECT`.
    #[allow(clippy::too_many_arguments)]
    pub fn bind_select(
        &self,
        items: &[SelectItem],
        tables: &[(String, Vec<String>)],
        join: Option<&JoinClause>,
        where_pred: Option<&Predicate>,
        group_by: &[ExprAst],
        having: Option<&Predicate>,
        order_by: &[OrderKey],
        limit: Option<u64>,
        offset: Option<u64>,
    ) -> Result<BoundSelectStatement> {
        let plan = plan_select(
            items, tables, join, where_pred, group_by, having, order_by, limit, offset,
        )?;
        Ok(BoundSelectStatement::new(plan))
    }

    /// C++ `BuildFromClause`: one scan per table, joined on the clause keys.
    pub fn bind_from(
        &self,
        tables: &[(String, Vec<String>)],
        join: Option<&JoinClause>,
    ) -> Result<LogicalPlan> {
        crate::build_source(tables, join)
    }

    /// C++ `BindInsert`: resolves the target column list and checks every row.
    pub fn bind_insert(
        &self,
        table: &str,
        columns: Option<&[String]>,
        rows: &[Vec<Literal>],
        schema: &[String],
    ) -> Result<BoundInsertStatement> {
        let columns: Vec<String> = match columns {
            Some(columns) => columns.to_vec(),
            None => schema.to_vec(),
        };
        for column in &columns {
            resolve_column(schema, column)?;
        }
        let mut bound_rows = Vec::with_capacity(rows.len());
        for row in rows {
            if row.len() != columns.len() {
                return Err(Error::Invalid(format!(
                    "insert row has {} values but {} columns were named",
                    row.len(),
                    columns.len()
                )));
            }
            bound_rows.push(row.iter().map(Value::from_literal).collect());
        }
        Ok(BoundInsertStatement {
            table: table.to_string(),
            columns,
            rows: bound_rows,
        })
    }

    /// C++ `BindDelete`: binds the optional predicate against the table schema.
    pub fn bind_delete(
        &self,
        table: &str,
        where_pred: Option<&Predicate>,
        schema: &[String],
    ) -> Result<BoundDeleteStatement> {
        Ok(BoundDeleteStatement {
            table: table.to_string(),
            predicate: where_pred
                .map(|p| self.bind_predicate(p, schema))
                .transpose()?,
        })
    }

    /// C++ `BindUpdate`: resolves assignment targets and binds every expression.
    pub fn bind_update(
        &self,
        table: &str,
        assignments: &[(String, ExprAst)],
        where_pred: Option<&Predicate>,
        schema: &[String],
    ) -> Result<BoundUpdateStatement> {
        let mut bound = Vec::with_capacity(assignments.len());
        for (column, expr) in assignments {
            bound.push((
                resolve_column(schema, column)?,
                self.bind_expression(expr, schema)?,
            ));
        }
        Ok(BoundUpdateStatement {
            table: table.to_string(),
            assignments: bound,
            predicate: where_pred
                .map(|p| self.bind_predicate(p, schema))
                .transpose()?,
        })
    }

    /// C++ `BuildTable`: a base-table scan.
    pub fn bind_table(&self, table: &str, columns: Vec<String>) -> LogicalPlan {
        LogicalPlan::Scan {
            table: table.to_string(),
            columns,
        }
    }

    /// C++ `BindExpression`: resolves names against a bound schema.
    pub fn bind_expression(&self, expr: &ExprAst, schema: &[String]) -> Result<Expr> {
        crate::compile_expr(expr, schema)
    }

    /// C++ `BindPredicate`: resolves a WHERE/HAVING predicate.
    pub fn bind_predicate(&self, pred: &Predicate, schema: &[String]) -> Result<Expr> {
        crate::predicate_expr(pred, schema)
    }

    /// C++ `QueryBinder::BindSubquery` + `SubqueryUnnest` pre-pass: binds a
    /// subquery/derived-table `SELECT` into the depth-annotated
    /// [`SubqueryPlan`], then asks [`CorrelatedExpressionsDetector`] whether it
    /// reads the enclosing query.
    ///
    /// A name that resolves in the subquery's own schema binds at depth 0; one
    /// that only the enclosing query knows binds at depth 1, which is exactly
    /// how the reference marks a correlated column.
    #[allow(clippy::too_many_arguments)]
    pub fn bind_subquery(
        &self,
        kind: SubqueryType,
        table: Option<&str>,
        local_columns: &[String],
        items: &[SelectItem],
        where_pred: Option<&Predicate>,
        outer_columns: &[String],
    ) -> Result<BoundSubquery> {
        let Some(table) = table else {
            return Err(Error::Invalid(
                "a subquery without FROM cannot be bound".into(),
            ));
        };
        let local_refs: Vec<&str> = local_columns.iter().map(String::as_str).collect();
        let mut plan = SubqueryPlan::scan(table, local_refs, 0);
        let mut correlated = Vec::new();
        if let Some(pred) = where_pred {
            let predicate = bind_subquery_predicate(
                pred,
                local_columns,
                table,
                outer_columns,
                &mut correlated,
            )?;
            plan = SubqueryPlan::filter(plan, predicate);
        }
        // Only plain column projections are represented; anything else keeps
        // the scan's own columns.
        let projected: Option<Vec<SubqueryExpr>> = items
            .iter()
            .map(|item| match item {
                SelectItem::Column(name) => Ok(Some(bind_subquery_column(
                    name,
                    local_columns,
                    table,
                    outer_columns,
                    &mut correlated,
                )?)),
                _ => Ok(None),
            })
            .collect::<Result<Vec<_>>>()?
            .into_iter()
            .collect();
        if let Some(exprs) = projected {
            if !exprs.is_empty() {
                let names: Vec<String> = items
                    .iter()
                    .map(|item| match item {
                        SelectItem::Column(name) => name.clone(),
                        _ => String::new(),
                    })
                    .collect();
                let name_refs: Vec<&str> = names.iter().map(String::as_str).collect();
                plan = SubqueryPlan::projection(plan, exprs, name_refs, 0);
            }
        }
        let mut detector = CorrelatedExpressionsDetector::new(Vec::new());
        detector.visit_plan(&plan)?;
        let bound = BoundSubquery::new(kind, plan);
        Ok(if detector.is_correlated() {
            bound.with_correlated(correlated)
        } else {
            bound
        })
    }
}

/// Binds one column reference of a subquery, recording outer references.
fn bind_subquery_column(
    name: &str,
    local: &[String],
    local_table: &str,
    outer: &[String],
    correlated: &mut Vec<DepthColumn>,
) -> Result<SubqueryExpr> {
    let (qualifier, base) = match name.rsplit_once('.') {
        Some((q, b)) => (Some(q.rsplit('.').next().unwrap_or(q)), b),
        None => (None, name),
    };
    // A name qualified by a table other than the subquery's own FROM cannot
    // be local, even when the bare column exists locally: that is the
    // shadowed outer-reference case. Only unqualified names (or names
    // qualified with the local table) may bind to the local scan.
    let local_table = local_table.rsplit('.').next().unwrap_or(local_table);
    let qualified_elsewhere = qualifier.is_some_and(|q| !q.eq_ignore_ascii_case(local_table));
    if !qualified_elsewhere {
        if let Some(i) = local.iter().position(|c| c == base) {
            return Ok(SubqueryExpr::col(ColumnBinding::new(0, i), 0));
        }
    }
    let column = match outer.iter().position(|c| c == base || c == name) {
        Some(i) => DepthColumn::outer(ColumnBinding::new(0, i)),
        None => {
            return Err(Error::Invalid(format!("unknown column '{name}'")));
        }
    };
    if !correlated.iter().any(|c| c.binding == column.binding) {
        correlated.push(column.clone());
    }
    Ok(SubqueryExpr::Column(column))
}

fn bind_subquery_expr(
    expr: &ExprAst,
    local: &[String],
    local_table: &str,
    outer: &[String],
    correlated: &mut Vec<DepthColumn>,
) -> Result<SubqueryExpr> {
    Ok(match expr {
        ExprAst::Column(name) => bind_subquery_column(name, local, local_table, outer, correlated)?,
        ExprAst::Literal(value) => SubqueryExpr::Literal(Value::from_literal(value)),
        ExprAst::Binary(a, op, b) => SubqueryExpr::Binary(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            *op,
            Box::new(bind_subquery_expr(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        ExprAst::Cmp(a, op, b) => SubqueryExpr::Cmp(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            *op,
            Box::new(bind_subquery_expr(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        ExprAst::Not(a) => SubqueryExpr::Not(Box::new(bind_subquery_expr(
            a,
            local,
            local_table,
            outer,
            correlated,
        )?)),
        ExprAst::And(a, b) => SubqueryExpr::And(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            Box::new(bind_subquery_expr(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        ExprAst::Or(a, b) => SubqueryExpr::Or(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            Box::new(bind_subquery_expr(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        ExprAst::Func { name, args } => SubqueryExpr::Func {
            name: name.clone(),
            args: args
                .iter()
                .map(|a| bind_subquery_expr(a, local, local_table, outer, correlated))
                .collect::<Result<Vec<_>>>()?,
        },
    })
}

fn bind_subquery_predicate(
    pred: &Predicate,
    local: &[String],
    local_table: &str,
    outer: &[String],
    correlated: &mut Vec<DepthColumn>,
) -> Result<SubqueryExpr> {
    Ok(match pred {
        Predicate::Cmp(a, op, b) => SubqueryExpr::Cmp(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            *op,
            Box::new(bind_subquery_expr(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        Predicate::IsTrue(e) => bind_subquery_expr(e, local, local_table, outer, correlated)?,
        Predicate::IsNull(e, neg) => SubqueryExpr::Func {
            name: if *neg { "is_not_null" } else { "is_null" }.into(),
            args: vec![bind_subquery_expr(
                e,
                local,
                local_table,
                outer,
                correlated,
            )?],
        },
        Predicate::Like(a, pat, _esc, neg) => SubqueryExpr::Func {
            name: if *neg { "not_like" } else { "like" }.into(),
            args: vec![
                bind_subquery_expr(a, local, local_table, outer, correlated)?,
                bind_subquery_expr(pat, local, local_table, outer, correlated)?,
            ],
        },
        Predicate::In(a, values, neg) => SubqueryExpr::In(
            Box::new(bind_subquery_expr(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            values
                .iter()
                .map(|v| bind_subquery_expr(v, local, local_table, outer, correlated))
                .collect::<Result<Vec<_>>>()?,
            *neg,
        ),
        Predicate::Bool(a, BoolOp::And, b) => SubqueryExpr::And(
            Box::new(bind_subquery_predicate(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            Box::new(bind_subquery_predicate(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        Predicate::Bool(a, BoolOp::Or, b) => SubqueryExpr::Or(
            Box::new(bind_subquery_predicate(
                a,
                local,
                local_table,
                outer,
                correlated,
            )?),
            Box::new(bind_subquery_predicate(
                b,
                local,
                local_table,
                outer,
                correlated,
            )?),
        ),
        Predicate::Not(a) => SubqueryExpr::Not(Box::new(bind_subquery_predicate(
            a,
            local,
            local_table,
            outer,
            correlated,
        )?)),
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_parser::{CmpOp, Literal, SelectItem};

    fn tables() -> Vec<(String, Vec<String>)> {
        vec![("t".to_string(), vec!["id".to_string(), "name".to_string()])]
    }

    /// C++ `QueryBinder::BindSelect`: the bound statement carries the plan and
    /// the output names the client sees.
    #[test]
    fn binds_a_select_with_where_and_limit() {
        let binder = QueryBinder::new();
        let bound = binder
            .bind_select(
                &[SelectItem::Column("id".into())],
                &tables(),
                None,
                Some(&Predicate::simple("id", CmpOp::Ge, Literal::Int(1))),
                &[],
                None,
                &[],
                Some(10),
                None,
            )
            .unwrap();
        assert_eq!(bound.output_names, vec!["id".to_string()]);
        match bound.plan {
            LogicalPlan::Limit { input, n } => {
                assert_eq!(n, 10);
                assert!(matches!(*input, LogicalPlan::Projection { .. }));
            }
            other => panic!("expected limit over projection, got {other:?}"),
        }
    }

    /// C++ `BuildFromClause` / `BuildJoin`: two tables become a join with the
    /// left/right key positions bound.
    #[test]
    fn binds_a_join_source() {
        let binder = QueryBinder::new();
        let tables = vec![
            ("a".to_string(), vec!["aid".to_string()]),
            ("b".to_string(), vec!["bid".to_string()]),
        ];
        let join = JoinClause {
            table: "b".into(),
            left: "aid".into(),
            right: "bid".into(),
            kind: infinity_parser::JoinKind::Inner,
        };
        match binder.bind_from(&tables, Some(&join)).unwrap() {
            LogicalPlan::Join {
                left_key,
                right_key,
                left_width,
                ..
            } => assert_eq!((left_key, right_key, left_width), (0, 0, 1)),
            other => panic!("expected join, got {other:?}"),
        }
    }

    /// C++ `BindInsert` / `BindDelete` / `BindUpdate`: DML binds names to
    /// positions and keeps the literal rows.
    #[test]
    fn binds_insert_delete_and_update() {
        let binder = QueryBinder::new();
        let schema = vec!["id".to_string(), "name".to_string()];

        let insert = binder
            .bind_insert(
                "t",
                Some(&["name".to_string()]),
                &[vec![Literal::Str("a".into())]],
                &schema,
            )
            .unwrap();
        assert_eq!(insert.columns, vec!["name".to_string()]);
        assert_eq!(insert.rows, vec![vec![Value::Str("a".into())]]);
        assert!(binder
            .bind_insert("t", None, &[vec![Literal::Int(1)]], &schema)
            .is_err());

        let delete = binder
            .bind_delete(
                "t",
                Some(&Predicate::simple("id", CmpOp::Eq, Literal::Int(2))),
                &schema,
            )
            .unwrap();
        assert_eq!(delete.table, "t");
        assert!(delete.predicate.is_some());

        let update = binder
            .bind_update(
                "t",
                &[(
                    "name".to_string(),
                    ExprAst::Literal(Literal::Str("b".into())),
                )],
                None,
                &schema,
            )
            .unwrap();
        assert_eq!(update.assignments.len(), 1);
        assert_eq!(update.assignments[0].0, 1);
        assert!(binder
            .bind_update(
                "t",
                &[("missing".to_string(), ExprAst::Literal(Literal::Int(1)))],
                None,
                &schema
            )
            .is_err());
    }

    /// `QueryBinder::bind_subquery` + `CorrelatedExpressionsDetector`: a
    /// predicate that names an enclosing column binds at depth 1 and is
    /// reported as correlated; one that only reads the subquery's own table is
    /// not.
    #[test]
    fn binds_subqueries_and_detects_correlation() {
        let binder = QueryBinder::new();
        let local = vec!["u".to_string()];
        let outer = vec!["t".to_string()];
        let items = [SelectItem::Column("u".into())];

        let uncorrelated = Predicate::Cmp(
            ExprAst::Column("u".into()),
            CmpOp::Eq,
            ExprAst::Literal(Literal::Int(1)),
        );
        let bound = binder
            .bind_subquery(
                SubqueryType::Exists,
                Some("s"),
                &local,
                &items,
                Some(&uncorrelated),
                &outer,
            )
            .unwrap();
        assert!(bound.correlated_columns.is_empty());

        let correlated = Predicate::Cmp(
            ExprAst::Column("u".into()),
            CmpOp::Eq,
            ExprAst::Column("t".into()),
        );
        let bound = binder
            .bind_subquery(
                SubqueryType::Exists,
                Some("s"),
                &local,
                &items,
                Some(&correlated),
                &outer,
            )
            .unwrap();
        assert_eq!(bound.correlated_columns.len(), 1);
        assert_eq!(bound.correlated_columns[0].depth, 1);
    }

    /// The predicate of a subquery whose single projection is a plain column:
    /// `Projection(Filter(Scan))`, so the `WHERE` predicate lives one level
    /// below the projection.
    fn subquery_predicate(plan: &SubqueryPlan) -> &SubqueryExpr {
        let SubqueryPlan::Projection { input, .. } = plan else {
            panic!("expected a projection, got {plan:?}");
        };
        let SubqueryPlan::Filter { predicate, .. } = input.as_ref() else {
            panic!("expected a filter, got {input:?}");
        };
        predicate
    }

    /// A qualifier naming another table is an outer reference even when the
    /// bare column also exists locally (round-4 review G2).
    #[test]
    fn a_qualified_outer_reference_is_not_shadowed_by_a_local_column() {
        let binder = QueryBinder::new();
        let local = vec!["a".to_string()];
        let outer = vec!["a".to_string()];
        let items = [SelectItem::Column("a".into())];
        let pred = Predicate::Cmp(
            ExprAst::Column("s.a".into()),
            CmpOp::Eq,
            ExprAst::Column("o.a".into()),
        );
        let bound = binder
            .bind_subquery(
                SubqueryType::Exists,
                Some("s"),
                &local,
                &items,
                Some(&pred),
                &outer,
            )
            .unwrap();
        assert_eq!(bound.correlated_columns.len(), 1, "{bound:?}");
        assert_eq!(bound.correlated_columns[0].depth, 1);
        // Correlation alone cannot distinguish the two bindings: `s.a` bound
        // as an outer reference would dedupe with `o.a` and still yield one
        // correlated column. Assert the sides themselves.
        let SubqueryExpr::Cmp(left, CmpOp::Eq, right) = subquery_predicate(&bound.plan) else {
            panic!("expected an equality, got {:?}", bound.plan);
        };
        assert_eq!(
            **left,
            SubqueryExpr::col(ColumnBinding::new(0, 0), 0),
            "`s.a` must read the subquery's own scan"
        );
        assert_eq!(
            **right,
            SubqueryExpr::col(ColumnBinding::new(0, 0), 1),
            "`o.a` must read the enclosing query"
        );
    }

    /// A qualifier that *is* the subquery's own table keeps the reference
    /// local; a database-qualified `db.s.a` normalises to the same table.
    #[test]
    fn a_qualified_local_reference_binds_to_the_subquery_scan() {
        let binder = QueryBinder::new();
        let local = vec!["a".to_string()];
        let outer = vec!["a".to_string()];
        let items = [SelectItem::Column("a".into())];
        for qualifier in ["s.a", "db.s.a"] {
            let pred = Predicate::Cmp(
                ExprAst::Column(qualifier.into()),
                CmpOp::Eq,
                ExprAst::Literal(Literal::Int(1)),
            );
            let bound = binder
                .bind_subquery(
                    SubqueryType::Exists,
                    Some("s"),
                    &local,
                    &items,
                    Some(&pred),
                    &outer,
                )
                .unwrap();
            assert!(bound.correlated_columns.is_empty(), "{bound:?}");
            let SubqueryExpr::Cmp(left, CmpOp::Eq, _) = subquery_predicate(&bound.plan) else {
                panic!("expected an equality, got {:?}", bound.plan);
            };
            assert_eq!(
                **left,
                SubqueryExpr::col(ColumnBinding::new(0, 0), 0),
                "`{qualifier}` must read the subquery's own scan"
            );
        }
    }

    /// C++ `BindExpression`: a select expression is resolved against the bound
    /// schema, so later stages only see positions.
    #[test]
    fn binds_an_expression_to_positions() {
        let binder = QueryBinder::new();
        let schema = vec!["id".to_string(), "name".to_string()];
        assert_eq!(
            binder
                .bind_expression(&ExprAst::Column("name".into()), &schema)
                .unwrap(),
            Expr::Column(1)
        );
        assert!(binder
            .bind_expression(&ExprAst::Column("missing".into()), &schema)
            .is_err());
    }
}

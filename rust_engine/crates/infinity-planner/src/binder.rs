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
use infinity_parser::{ExprAst, JoinClause, Literal, OrderKey, Predicate, SelectItem};

use crate::{output_schema, plan_select, resolve_column, LogicalPlan};

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

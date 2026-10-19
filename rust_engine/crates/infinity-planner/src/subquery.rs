//! Correlated-subquery detection, rewriting, dependent-join flattening and
//! unnesting.
//!
//! C++ binds a subquery into a logical plan whose `ColumnExpression`s carry a
//! correlation depth. The Rust `LogicalPlan` addresses columns positionally and
//! has no depth, so this module defines the depth-annotated bound subquery tree
//! (`SubqueryPlan` / `SubqueryExpr`) the four classes operate on. The executor
//! still evaluates subqueries row by row; this tree is the planner-side
//! representation used to detect, rewrite and flatten correlation.

use std::collections::HashMap;

use infinity_common::{Error, Result};
use infinity_expression::Value;
use infinity_parser::{BinOp, CmpOp};

use crate::ColumnBinding;

/// C++ `ColumnExpression::depth`: `0` is local, `1` refers to the enclosing
/// query. Larger depths are rejected, as in the reference.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DepthColumn {
    pub binding: ColumnBinding,
    pub depth: u32,
}

impl DepthColumn {
    pub fn local(binding: ColumnBinding) -> Self {
        Self { binding, depth: 0 }
    }

    pub fn outer(binding: ColumnBinding) -> Self {
        Self { binding, depth: 1 }
    }
}

/// A bound expression over a subquery's plan, including nested subqueries.
#[derive(Debug, Clone, PartialEq)]
pub enum SubqueryExpr {
    Column(DepthColumn),
    Literal(Value),
    Cmp(Box<SubqueryExpr>, CmpOp, Box<SubqueryExpr>),
    And(Box<SubqueryExpr>, Box<SubqueryExpr>),
    Or(Box<SubqueryExpr>, Box<SubqueryExpr>),
    Not(Box<SubqueryExpr>),
    /// `left [NOT] IN (values)` (C++ `InExpression`).
    In(Box<SubqueryExpr>, Vec<SubqueryExpr>, bool),
    Binary(Box<SubqueryExpr>, BinOp, Box<SubqueryExpr>),
    Func {
        name: String,
        args: Vec<SubqueryExpr>,
    },
    /// C++ `SubqueryExpression`.
    Subquery(Box<BoundSubquery>),
}

impl SubqueryExpr {
    pub fn col(binding: ColumnBinding, depth: u32) -> Self {
        SubqueryExpr::Column(DepthColumn { binding, depth })
    }

    pub fn eq(a: SubqueryExpr, b: SubqueryExpr) -> Self {
        SubqueryExpr::Cmp(Box::new(a), CmpOp::Eq, Box::new(b))
    }
}

/// C++ `SubqueryType`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SubqueryType {
    Scalar,
    Exists,
    NotExists,
    In,
    NotIn,
}

/// C++ `SubqueryExpression::bound_select_statement_ptr_` + correlated list.
#[derive(Debug, Clone, PartialEq)]
pub struct BoundSubquery {
    pub kind: SubqueryType,
    pub plan: SubqueryPlan,
    /// Left operand of `IN` (`expr_ptr->left_`).
    pub left: Option<SubqueryExpr>,
    pub correlated_columns: Vec<DepthColumn>,
}

impl BoundSubquery {
    pub fn new(kind: SubqueryType, plan: SubqueryPlan) -> Self {
        Self {
            kind,
            plan,
            left: None,
            correlated_columns: Vec::new(),
        }
    }

    pub fn with_correlated(mut self, columns: Vec<DepthColumn>) -> Self {
        self.correlated_columns = columns;
        self
    }
}

/// The depth-annotated logical plan of a bound subquery.
#[derive(Debug, Clone, PartialEq)]
pub enum SubqueryPlan {
    Scan {
        table: String,
        columns: Vec<String>,
        table_idx: usize,
    },
    Filter {
        input: Box<SubqueryPlan>,
        predicate: SubqueryExpr,
    },
    Projection {
        input: Box<SubqueryPlan>,
        exprs: Vec<SubqueryExpr>,
        names: Vec<String>,
        table_idx: usize,
    },
    Aggregate {
        input: Box<SubqueryPlan>,
        group_keys: Vec<SubqueryExpr>,
        aggs: Vec<SubqueryExpr>,
        table_idx: usize,
    },
    Join {
        left: Box<SubqueryPlan>,
        right: Box<SubqueryPlan>,
        conditions: Vec<SubqueryExpr>,
        mark: bool,
        table_idx: usize,
    },
    CrossProduct {
        left: Box<SubqueryPlan>,
        right: Box<SubqueryPlan>,
        table_idx: usize,
    },
    Limit {
        input: Box<SubqueryPlan>,
        n: u64,
    },
    Sort {
        input: Box<SubqueryPlan>,
        keys: Vec<(SubqueryExpr, bool)>,
    },
}

impl SubqueryPlan {
    pub fn scan(table: impl Into<String>, columns: Vec<&str>, table_idx: usize) -> Self {
        SubqueryPlan::Scan {
            table: table.into(),
            columns: columns.into_iter().map(str::to_string).collect(),
            table_idx,
        }
    }

    pub fn filter(input: SubqueryPlan, predicate: SubqueryExpr) -> Self {
        SubqueryPlan::Filter {
            input: Box::new(input),
            predicate,
        }
    }

    pub fn projection(
        input: SubqueryPlan,
        exprs: Vec<SubqueryExpr>,
        names: Vec<&str>,
        table_idx: usize,
    ) -> Self {
        SubqueryPlan::Projection {
            input: Box::new(input),
            exprs,
            names: names.into_iter().map(str::to_string).collect(),
            table_idx,
        }
    }

    pub fn aggregate(
        input: SubqueryPlan,
        group_keys: Vec<SubqueryExpr>,
        aggs: Vec<SubqueryExpr>,
        table_idx: usize,
    ) -> Self {
        SubqueryPlan::Aggregate {
            input: Box::new(input),
            group_keys,
            aggs,
            table_idx,
        }
    }

    pub fn cross(left: SubqueryPlan, right: SubqueryPlan, table_idx: usize) -> Self {
        SubqueryPlan::CrossProduct {
            left: Box::new(left),
            right: Box::new(right),
            table_idx,
        }
    }

    pub fn join(
        left: SubqueryPlan,
        right: SubqueryPlan,
        conditions: Vec<SubqueryExpr>,
        mark: bool,
        table_idx: usize,
    ) -> Self {
        SubqueryPlan::Join {
            left: Box::new(left),
            right: Box::new(right),
            conditions,
            mark,
            table_idx,
        }
    }

    pub fn limit(input: SubqueryPlan, n: u64) -> Self {
        SubqueryPlan::Limit {
            input: Box::new(input),
            n,
        }
    }

    /// The node's output width (C++ `GetOutputNames()->size()`).
    pub fn width(&self) -> usize {
        match self {
            SubqueryPlan::Scan { columns, .. } => columns.len(),
            SubqueryPlan::Filter { input, .. }
            | SubqueryPlan::Limit { input, .. }
            | SubqueryPlan::Sort { input, .. } => input.width(),
            SubqueryPlan::Projection { exprs, .. } => exprs.len(),
            SubqueryPlan::Aggregate {
                group_keys, aggs, ..
            } => group_keys.len() + aggs.len(),
            SubqueryPlan::Join { left, right, .. }
            | SubqueryPlan::CrossProduct { left, right, .. } => left.width() + right.width(),
        }
    }

    /// The operator's table index (used as the output binding's table part).
    pub fn table_idx(&self) -> usize {
        match self {
            SubqueryPlan::Scan { table_idx, .. }
            | SubqueryPlan::Projection { table_idx, .. }
            | SubqueryPlan::Aggregate { table_idx, .. }
            | SubqueryPlan::Join { table_idx, .. }
            | SubqueryPlan::CrossProduct { table_idx, .. } => *table_idx,
            SubqueryPlan::Filter { input, .. }
            | SubqueryPlan::Limit { input, .. }
            | SubqueryPlan::Sort { input, .. } => input.table_idx(),
        }
    }
}

fn visit_plan_exprs<F>(plan: &SubqueryPlan, f: &mut F) -> Result<()>
where
    F: FnMut(&SubqueryExpr) -> Result<()>,
{
    match plan {
        SubqueryPlan::Scan { .. } => {}
        SubqueryPlan::Filter { input, predicate } => {
            f(predicate)?;
            visit_plan_exprs(input, f)?;
        }
        SubqueryPlan::Projection { input, exprs, .. } => {
            for e in exprs {
                f(e)?;
            }
            visit_plan_exprs(input, f)?;
        }
        SubqueryPlan::Aggregate {
            input,
            group_keys,
            aggs,
            ..
        } => {
            for e in group_keys {
                f(e)?;
            }
            for e in aggs {
                f(e)?;
            }
            visit_plan_exprs(input, f)?;
        }
        SubqueryPlan::Join {
            left,
            right,
            conditions,
            ..
        } => {
            for e in conditions {
                f(e)?;
            }
            visit_plan_exprs(left, f)?;
            visit_plan_exprs(right, f)?;
        }
        SubqueryPlan::CrossProduct { left, right, .. } => {
            visit_plan_exprs(left, f)?;
            visit_plan_exprs(right, f)?;
        }
        SubqueryPlan::Limit { input, .. } => visit_plan_exprs(input, f)?,
        SubqueryPlan::Sort { input, keys } => {
            for (e, _) in keys {
                f(e)?;
            }
            visit_plan_exprs(input, f)?;
        }
    }
    Ok(())
}

fn map_plan_exprs<F>(plan: &mut SubqueryPlan, f: &mut F) -> Result<()>
where
    F: FnMut(&mut SubqueryExpr) -> Result<()>,
{
    match plan {
        SubqueryPlan::Scan { .. } => {}
        SubqueryPlan::Filter { input, predicate } => {
            f(predicate)?;
            map_plan_exprs(input, f)?;
        }
        SubqueryPlan::Projection { input, exprs, .. } => {
            for e in exprs {
                f(e)?;
            }
            map_plan_exprs(input, f)?;
        }
        SubqueryPlan::Aggregate {
            input,
            group_keys,
            aggs,
            ..
        } => {
            for e in group_keys {
                f(e)?;
            }
            for e in aggs {
                f(e)?;
            }
            map_plan_exprs(input, f)?;
        }
        SubqueryPlan::Join {
            left,
            right,
            conditions,
            ..
        } => {
            for e in conditions {
                f(e)?;
            }
            map_plan_exprs(left, f)?;
            map_plan_exprs(right, f)?;
        }
        SubqueryPlan::CrossProduct { left, right, .. } => {
            map_plan_exprs(left, f)?;
            map_plan_exprs(right, f)?;
        }
        SubqueryPlan::Limit { input, .. } => map_plan_exprs(input, f)?,
        SubqueryPlan::Sort { input, keys } => {
            for (e, _) in keys {
                f(e)?;
            }
            map_plan_exprs(input, f)?;
        }
    }
    Ok(())
}

// ---------------------------------------------------------------------------
// CorrelatedExpressionsDetector
// ---------------------------------------------------------------------------

/// C++ `CorrelatedExpressionsDetector`.
pub struct CorrelatedExpressionsDetector {
    pub correlated_column_exprs: Vec<ColumnBinding>,
    is_correlated: bool,
}

impl CorrelatedExpressionsDetector {
    pub fn new(correlated_column_exprs: Vec<ColumnBinding>) -> Self {
        Self {
            correlated_column_exprs,
            is_correlated: false,
        }
    }

    /// C++ `VisitNode`: checks the node's expressions and its children.
    pub fn visit_plan(&mut self, plan: &SubqueryPlan) -> Result<()> {
        visit_plan_exprs(plan, &mut |e| self.visit_expr(e))
    }

    /// C++ `VisitReplace(ColumnExpression)` / `(SubqueryExpression)`.
    pub fn visit_expr(&mut self, expr: &SubqueryExpr) -> Result<()> {
        match expr {
            SubqueryExpr::Column(column) => {
                if column.depth == 0 {
                    return Ok(());
                }
                if column.depth > 1 {
                    return Err(Error::Invalid(
                        "Column expression with depth > 1 is detected".into(),
                    ));
                }
                self.is_correlated = true;
                Ok(())
            }
            SubqueryExpr::Subquery(sub) => {
                if !sub.correlated_columns.is_empty() {
                    return Err(Error::Invalid(
                        "Not support nested correlated subquery in the subquery plan".into(),
                    ));
                }
                Ok(())
            }
            SubqueryExpr::Literal(_) => Ok(()),
            SubqueryExpr::Cmp(a, _, b)
            | SubqueryExpr::And(a, b)
            | SubqueryExpr::Or(a, b)
            | SubqueryExpr::Binary(a, _, b) => {
                self.visit_expr(a)?;
                self.visit_expr(b)
            }
            SubqueryExpr::Not(a) => self.visit_expr(a),
            SubqueryExpr::In(a, values, _) => {
                self.visit_expr(a)?;
                for value in values {
                    self.visit_expr(value)?;
                }
                Ok(())
            }
            SubqueryExpr::Func { args, .. } => {
                for a in args {
                    self.visit_expr(a)?;
                }
                Ok(())
            }
        }
    }

    pub fn is_correlated(&self) -> bool {
        self.is_correlated
    }
}

// ---------------------------------------------------------------------------
// RewriteCorrelatedExpressions
// ---------------------------------------------------------------------------

/// C++ `RewriteCorrelatedExpressions`.
pub struct RewriteCorrelatedExpressions {
    pub correlated_column_map: HashMap<ColumnBinding, usize>,
    pub base_binding: ColumnBinding,
}

impl RewriteCorrelatedExpressions {
    pub fn new(
        correlated_column_map: HashMap<ColumnBinding, usize>,
        base_binding: ColumnBinding,
    ) -> Self {
        Self {
            correlated_column_map,
            base_binding,
        }
    }

    /// C++ `VisitNode`.
    pub fn rewrite_plan(&self, plan: &mut SubqueryPlan) -> Result<()> {
        map_plan_exprs(plan, &mut |e| self.rewrite_expr(e))
    }

    /// C++ `VisitReplace(ColumnExpression)` / `(SubqueryExpression)`.
    pub fn rewrite_expr(&self, expr: &mut SubqueryExpr) -> Result<()> {
        match expr {
            SubqueryExpr::Column(column) => {
                if column.depth == 0 {
                    return Ok(());
                }
                if column.depth > 1 {
                    return Err(Error::Invalid(
                        "Correlated depth > 1 is not supported now.".into(),
                    ));
                }
                let offset = self
                    .correlated_column_map
                    .get(&column.binding)
                    .ok_or_else(|| {
                        Error::Unexpected("Correlated expression isn't found.".into())
                    })?;
                column.binding = ColumnBinding::new(
                    self.base_binding.table_idx,
                    self.base_binding.column_idx + offset,
                );
                column.depth = 0;
                Ok(())
            }
            SubqueryExpr::Subquery(sub) => {
                if !sub.correlated_columns.is_empty() {
                    return Err(Error::Invalid(
                        "Not support rewrite nested correlated subquery in the subquery plan"
                            .into(),
                    ));
                }
                Ok(())
            }
            SubqueryExpr::Literal(_) => Ok(()),
            SubqueryExpr::Cmp(a, _, b)
            | SubqueryExpr::And(a, b)
            | SubqueryExpr::Or(a, b)
            | SubqueryExpr::Binary(a, _, b) => {
                self.rewrite_expr(a)?;
                self.rewrite_expr(b)
            }
            SubqueryExpr::Not(a) => self.rewrite_expr(a),
            SubqueryExpr::In(a, values, _) => {
                self.rewrite_expr(a)?;
                for value in values {
                    self.rewrite_expr(value)?;
                }
                Ok(())
            }
            SubqueryExpr::Func { args, .. } => {
                for a in args {
                    self.rewrite_expr(a)?;
                }
                Ok(())
            }
        }
    }
}

fn plan_is_correlated(plan: &SubqueryPlan) -> Result<bool> {
    let mut detector = CorrelatedExpressionsDetector::new(Vec::new());
    detector.visit_plan(plan)?;
    Ok(detector.is_correlated())
}

// ---------------------------------------------------------------------------
// DependentJoinFlattener
// ---------------------------------------------------------------------------

/// C++ `DependentJoinFlattener`.
pub struct DependentJoinFlattener {
    pub correlated_column_exprs: Vec<DepthColumn>,
    pub correlated_column_map: HashMap<ColumnBinding, usize>,
    pub base_binding: ColumnBinding,
    pub correlated_expression_offset: usize,
    detected: bool,
}

impl DependentJoinFlattener {
    pub fn new(
        correlated_column_exprs: Vec<DepthColumn>,
        correlated_column_map: HashMap<ColumnBinding, usize>,
    ) -> Self {
        Self {
            correlated_column_exprs,
            correlated_column_map,
            base_binding: ColumnBinding::default(),
            correlated_expression_offset: 0,
            detected: false,
        }
    }

    /// C++ `DetectCorrelatedExpressions`.
    pub fn detect_correlated_expressions(&mut self, plan: &SubqueryPlan) -> Result<bool> {
        self.detected = true;
        plan_is_correlated(plan)
    }

    pub fn correlated_column_base_index(&self) -> usize {
        self.correlated_expression_offset
    }

    /// C++ `PushDependentJoin`.
    pub fn push_dependent_join(&mut self, plan: SubqueryPlan) -> Result<SubqueryPlan> {
        if !self.detected {
            return Err(Error::Invalid(
                "Logical node wasn't detected before.".into(),
            ));
        }
        self.push_internal(plan)
    }

    fn push_internal(&mut self, plan: SubqueryPlan) -> Result<SubqueryPlan> {
        if !plan_is_correlated(&plan)? {
            return self.build_no_correlated(plan);
        }
        match plan {
            SubqueryPlan::Aggregate {
                input,
                mut group_keys,
                aggs,
                table_idx,
            } => {
                let pushed = self.push_internal(*input)?;
                let mut node = SubqueryPlan::Aggregate {
                    input: Box::new(pushed),
                    group_keys: std::mem::take(&mut group_keys),
                    aggs,
                    table_idx,
                };
                RewriteCorrelatedExpressions::new(
                    self.correlated_column_map.clone(),
                    self.base_binding,
                )
                .rewrite_plan(&mut node)?;
                let SubqueryPlan::Aggregate {
                    group_keys: ref mut groups,
                    ..
                } = node
                else {
                    unreachable!()
                };
                let mut appended = Vec::new();
                for (idx, correlated) in self.correlated_column_exprs.iter().enumerate() {
                    appended.push(SubqueryExpr::Column(DepthColumn {
                        binding: ColumnBinding::new(
                            self.base_binding.table_idx,
                            self.base_binding.column_idx + idx,
                        ),
                        depth: 0,
                    }));
                    let _ = correlated;
                }
                groups.extend(appended);
                self.base_binding = ColumnBinding::new(
                    table_idx,
                    groups.len() - self.correlated_column_exprs.len(),
                );
                self.correlated_expression_offset = self.base_binding.column_idx;
                Ok(node)
            }
            SubqueryPlan::Filter { input, predicate } => {
                let pushed = self.push_internal(*input)?;
                let mut node = SubqueryPlan::Filter {
                    input: Box::new(pushed),
                    predicate,
                };
                RewriteCorrelatedExpressions::new(
                    self.correlated_column_map.clone(),
                    self.base_binding,
                )
                .rewrite_plan(&mut node)?;
                Ok(node)
            }
            SubqueryPlan::Projection {
                input,
                exprs,
                names,
                table_idx,
            } => {
                let pushed = self.push_internal(*input)?;
                let original_len = exprs.len();
                let mut node = SubqueryPlan::Projection {
                    input: Box::new(pushed),
                    exprs,
                    names,
                    table_idx,
                };
                RewriteCorrelatedExpressions::new(
                    self.correlated_column_map.clone(),
                    self.base_binding,
                )
                .rewrite_plan(&mut node)?;
                let SubqueryPlan::Projection { exprs, .. } = &mut node else {
                    unreachable!()
                };
                for idx in 0..self.correlated_column_exprs.len() {
                    exprs.push(SubqueryExpr::Column(DepthColumn {
                        binding: ColumnBinding::new(
                            self.base_binding.table_idx,
                            self.base_binding.column_idx + idx,
                        ),
                        depth: 0,
                    }));
                }
                self.base_binding = ColumnBinding::new(table_idx, original_len);
                self.correlated_expression_offset = original_len;
                Ok(node)
            }
            SubqueryPlan::CrossProduct {
                left,
                right,
                table_idx,
            } => {
                let left_correlated = plan_is_correlated(&left)?;
                let right_correlated = plan_is_correlated(&right)?;
                if !right_correlated {
                    let pushed = self.push_internal(*left)?;
                    return Ok(SubqueryPlan::CrossProduct {
                        left: Box::new(pushed),
                        right,
                        table_idx,
                    });
                }
                if !left_correlated {
                    let pushed = self.push_internal(*right)?;
                    return Ok(SubqueryPlan::CrossProduct {
                        left,
                        right: Box::new(pushed),
                        table_idx,
                    });
                }
                let left_pushed = self.push_internal(*left)?;
                let left_binding = self.base_binding;
                let right_pushed = self.push_internal(*right)?;
                let right_binding = self.base_binding;
                let mut conditions = Vec::new();
                for idx in 0..self.correlated_column_exprs.len() {
                    conditions.push(SubqueryExpr::eq(
                        SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(
                            left_binding.table_idx,
                            left_binding.column_idx + idx,
                        ))),
                        SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(
                            right_binding.table_idx,
                            right_binding.column_idx + idx,
                        ))),
                    ));
                }
                Ok(SubqueryPlan::Join {
                    left: Box::new(left_pushed),
                    right: Box::new(right_pushed),
                    conditions,
                    mark: false,
                    table_idx,
                })
            }
            SubqueryPlan::Join { .. } => {
                Err(Error::Invalid("Can't push down through join node".into()))
            }
            SubqueryPlan::Limit { .. } => {
                Err(Error::Invalid("Can't push down through limit node".into()))
            }
            SubqueryPlan::Sort { .. } => Err(Error::Invalid(
                "Can't push down through order by node".into(),
            )),
            SubqueryPlan::Scan { .. } => Err(Error::Invalid(
                "Can't push down through table scan node".into(),
            )),
        }
    }

    /// C++ `BuildNoCorrelatedInternal`: cross product the (now uncorrelated)
    /// subplan with a scan of the correlated columns.
    fn build_no_correlated(&mut self, plan: SubqueryPlan) -> Result<SubqueryPlan> {
        let first = self
            .correlated_column_exprs
            .first()
            .ok_or_else(|| Error::Invalid("No correlated column".into()))?;
        let table_idx = first.binding.table_idx;
        for correlated in &self.correlated_column_exprs[1..] {
            if correlated.binding.table_idx != table_idx {
                return Err(Error::Invalid(
                    "Correlated column are from different table.".into(),
                ));
            }
        }
        let columns: Vec<String> = self
            .correlated_column_exprs
            .iter()
            .map(|c| format!("c{}", c.binding.column_idx))
            .collect();
        let scan = SubqueryPlan::Scan {
            table: format!("correlated_{table_idx}"),
            columns,
            table_idx,
        };
        let offset = plan.width();
        let cross = SubqueryPlan::CrossProduct {
            left: Box::new(plan),
            right: Box::new(scan),
            table_idx,
        };
        self.base_binding = ColumnBinding::new(table_idx, 0);
        self.correlated_expression_offset = offset;
        Ok(cross)
    }
}

// ---------------------------------------------------------------------------
// SubqueryUnnest
// ---------------------------------------------------------------------------

/// C++ `BindContext` (the correlated-column part).
#[derive(Debug, Clone, Default)]
pub struct BindContext {
    pub correlated_column_exprs: Vec<DepthColumn>,
    pub correlated_column_map: HashMap<ColumnBinding, usize>,
    next_table_index: usize,
}

impl BindContext {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn generate_table_index(&mut self) -> usize {
        let idx = self.next_table_index;
        self.next_table_index += 1;
        idx
    }
}

/// C++ `SubqueryUnnest`.
pub struct SubqueryUnnest;

impl SubqueryUnnest {
    /// C++ `UnnestSubqueries`.
    pub fn unnest_subqueries(
        expr: &mut SubqueryExpr,
        root: &mut SubqueryPlan,
        context: &mut BindContext,
    ) -> Result<()> {
        if matches!(expr, SubqueryExpr::Subquery(_)) {
            let replaced = Self::unnest_subquery(expr, root, context)?;
            *expr = replaced;
        }
        Ok(())
    }

    /// C++ `UnnestSubquery`: builds the subplan and dispatches on correlation.
    pub fn unnest_subquery(
        expr: &mut SubqueryExpr,
        root: &mut SubqueryPlan,
        context: &mut BindContext,
    ) -> Result<SubqueryExpr> {
        let SubqueryExpr::Subquery(subquery) = expr else {
            return Ok(expr.clone());
        };
        let subquery = (**subquery).clone();
        if subquery.correlated_columns.is_empty() {
            Self::unnest_uncorrelated(&subquery, root, context)
        } else {
            Self::unnest_correlated(&subquery, root, context)
        }
    }

    /// C++ `UnnestUncorrelated`.
    pub fn unnest_uncorrelated(
        subquery: &BoundSubquery,
        root: &mut SubqueryPlan,
        context: &mut BindContext,
    ) -> Result<SubqueryExpr> {
        match subquery.kind {
            SubqueryType::Scalar => {
                let limited = SubqueryPlan::limit(subquery.plan.clone(), 1);
                let aggregate_index = context.generate_table_index();
                let aggregate = SubqueryPlan::aggregate(
                    limited,
                    Vec::new(),
                    vec![SubqueryExpr::Column(DepthColumn::local(
                        ColumnBinding::new(0, 0),
                    ))],
                    aggregate_index,
                );
                let cross_index = context.generate_table_index();
                *root = SubqueryPlan::cross(
                    std::mem::replace(
                        root,
                        SubqueryPlan::Scan {
                            table: String::new(),
                            columns: Vec::new(),
                            table_idx: 0,
                        },
                    ),
                    aggregate,
                    cross_index,
                );
                Ok(SubqueryExpr::Column(DepthColumn::local(
                    ColumnBinding::new(aggregate_index, 0),
                )))
            }
            SubqueryType::Exists | SubqueryType::NotExists => {
                Err(Error::Invalid("Plan EXISTS uncorrelated subquery".into()))
            }
            SubqueryType::In | SubqueryType::NotIn => {
                let left = subquery
                    .left
                    .clone()
                    .ok_or_else(|| Error::Invalid("IN subquery has no left operand".into()))?;
                let right = SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(
                    subquery.plan.table_idx(),
                    0,
                )));
                let condition = if subquery.kind == SubqueryType::NotIn {
                    SubqueryExpr::Cmp(Box::new(left), CmpOp::Ne, Box::new(right))
                } else {
                    SubqueryExpr::eq(left, right)
                };
                let mark_index = context.generate_table_index();
                let root_plan = std::mem::replace(
                    root,
                    SubqueryPlan::Scan {
                        table: String::new(),
                        columns: Vec::new(),
                        table_idx: 0,
                    },
                );
                *root = SubqueryPlan::join(
                    root_plan,
                    subquery.plan.clone(),
                    vec![condition],
                    true,
                    mark_index,
                );
                Ok(SubqueryExpr::Column(DepthColumn::local(
                    ColumnBinding::new(mark_index, 0),
                )))
            }
        }
    }

    /// C++ `UnnestCorrelated`.
    pub fn unnest_correlated(
        subquery: &BoundSubquery,
        root: &mut SubqueryPlan,
        context: &mut BindContext,
    ) -> Result<SubqueryExpr> {
        if subquery.correlated_columns.is_empty() {
            return Err(Error::Invalid("No correlated column".into()));
        }
        let table_index = subquery.correlated_columns[0].binding.table_idx;
        for correlated in &subquery.correlated_columns[1..] {
            if correlated.binding.table_idx != table_index {
                return Err(Error::Invalid(
                    "Correlated columns can be only from one table, now.".into(),
                ));
            }
        }
        context.correlated_column_exprs = subquery.correlated_columns.clone();
        context.correlated_column_map = subquery
            .correlated_columns
            .iter()
            .enumerate()
            .map(|(i, c)| (c.binding, i))
            .collect();

        let mut flattener = DependentJoinFlattener::new(
            context.correlated_column_exprs.clone(),
            context.correlated_column_map.clone(),
        );
        flattener.detect_correlated_expressions(&subquery.plan)?;
        let dependent = flattener.push_dependent_join(subquery.plan.clone())?;
        let base = flattener.correlated_column_base_index();
        let conditions = Self::generate_join_conditions(
            &subquery.correlated_columns,
            dependent.table_idx(),
            base,
        )?;
        let mut conditions = conditions;
        if matches!(subquery.kind, SubqueryType::In | SubqueryType::NotIn) {
            let left = subquery
                .left
                .clone()
                .ok_or_else(|| Error::Invalid("IN subquery has no left operand".into()))?;
            let right = SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(
                dependent.table_idx(),
                0,
            )));
            conditions.push(SubqueryExpr::In(
                Box::new(left),
                vec![right],
                subquery.kind == SubqueryType::NotIn,
            ));
        }
        let mark_index = context.generate_table_index();
        let root_plan = std::mem::replace(
            root,
            SubqueryPlan::Scan {
                table: String::new(),
                columns: Vec::new(),
                table_idx: 0,
            },
        );
        match subquery.kind {
            SubqueryType::Exists
            | SubqueryType::NotExists
            | SubqueryType::In
            | SubqueryType::NotIn => {
                *root = SubqueryPlan::join(root_plan, dependent, conditions, true, mark_index);
                let mark =
                    SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(mark_index, 0)));
                if subquery.kind == SubqueryType::NotExists {
                    Ok(SubqueryExpr::Not(Box::new(mark)))
                } else {
                    Ok(mark)
                }
            }
            SubqueryType::Scalar => {
                *root = SubqueryPlan::join(root_plan, dependent, conditions, false, mark_index);
                Ok(SubqueryExpr::Column(DepthColumn::local(
                    ColumnBinding::new(mark_index, 0),
                )))
            }
        }
    }

    /// C++ `GenerateJoinConditions`: one equality per correlated column.
    fn generate_join_conditions(
        correlated_columns: &[DepthColumn],
        subplan_table_idx: usize,
        correlated_base_index: usize,
    ) -> Result<Vec<SubqueryExpr>> {
        let mut conditions = Vec::new();
        for (idx, left) in correlated_columns.iter().enumerate() {
            conditions.push(SubqueryExpr::eq(
                SubqueryExpr::Column(DepthColumn::local(left.binding)),
                SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(
                    subplan_table_idx,
                    correlated_base_index + idx,
                ))),
            ));
        }
        Ok(conditions)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn outer_col() -> DepthColumn {
        DepthColumn::outer(ColumnBinding::new(0, 1))
    }

    fn subquery_plan() -> SubqueryPlan {
        SubqueryPlan::scan("s", vec!["s0"], 1)
    }

    /// C++ `CorrelatedExpressionsDetector`: depth-1 columns mark the plan
    /// correlated; depth > 1 and nested correlated subqueries are errors.
    #[test]
    fn correlation_detector_reports_depth() {
        let mut detector = CorrelatedExpressionsDetector::new(vec![ColumnBinding::new(0, 1)]);
        detector
            .visit_plan(&SubqueryPlan::filter(
                subquery_plan(),
                SubqueryExpr::Cmp(
                    Box::new(SubqueryExpr::col(ColumnBinding::new(1, 0), 0)),
                    CmpOp::Gt,
                    Box::new(SubqueryExpr::Literal(Value::Int(1))),
                ),
            ))
            .unwrap();
        assert!(!detector.is_correlated());

        let mut detector = CorrelatedExpressionsDetector::new(vec![ColumnBinding::new(0, 1)]);
        detector
            .visit_plan(&SubqueryPlan::filter(
                subquery_plan(),
                SubqueryExpr::eq(
                    SubqueryExpr::Column(outer_col()),
                    SubqueryExpr::Literal(Value::Int(1)),
                ),
            ))
            .unwrap();
        assert!(detector.is_correlated());

        let mut detector = CorrelatedExpressionsDetector::new(Vec::new());
        assert!(detector
            .visit_plan(&SubqueryPlan::filter(
                subquery_plan(),
                SubqueryExpr::eq(
                    SubqueryExpr::col(ColumnBinding::new(0, 1), 2),
                    SubqueryExpr::Literal(Value::Int(1)),
                ),
            ))
            .is_err());
    }

    /// C++ `RewriteCorrelatedExpressions`: depth 1 becomes a local column at
    /// `base_binding + offset`; a missing binding is an error.
    #[test]
    fn rewrite_maps_correlated_columns_to_the_base_binding() {
        let mut map = HashMap::new();
        map.insert(ColumnBinding::new(0, 1), 0);
        let rewriter = RewriteCorrelatedExpressions::new(map, ColumnBinding::new(9, 4));
        let mut plan = SubqueryPlan::filter(
            subquery_plan(),
            SubqueryExpr::eq(
                SubqueryExpr::Column(outer_col()),
                SubqueryExpr::Literal(Value::Int(1)),
            ),
        );
        rewriter.rewrite_plan(&mut plan).unwrap();
        match plan {
            SubqueryPlan::Filter { predicate, .. } => match predicate {
                SubqueryExpr::Cmp(a, _, _) => assert_eq!(
                    *a,
                    SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(9, 4)))
                ),
                other => panic!("expected cmp, got {other:?}"),
            },
            other => panic!("expected filter, got {other:?}"),
        }

        let rewriter = RewriteCorrelatedExpressions::new(HashMap::new(), ColumnBinding::new(9, 4));
        let mut plan = SubqueryPlan::filter(
            subquery_plan(),
            SubqueryExpr::Column(DepthColumn::outer(ColumnBinding::new(0, 2))),
        );
        assert!(rewriter.rewrite_plan(&mut plan).is_err());
    }

    /// C++ `DependentJoinFlattener`: an uncorrelated subplan becomes a cross
    /// product with the correlated-column scan, and the offset points past the
    /// subplan's output.
    #[test]
    fn flattener_cross_products_an_uncorrelated_subplan() {
        let mut flattener = DependentJoinFlattener::new(vec![outer_col()], HashMap::new());
        flattener
            .detect_correlated_expressions(&subquery_plan())
            .unwrap();
        let plan = flattener.push_dependent_join(subquery_plan()).unwrap();
        match plan {
            SubqueryPlan::CrossProduct { left, right, .. } => {
                assert_eq!(left.width(), 1);
                match *right {
                    SubqueryPlan::Scan { columns, .. } => assert_eq!(columns, vec!["c1"]),
                    other => panic!("expected correlated scan, got {other:?}"),
                }
            }
            other => panic!("expected cross product, got {other:?}"),
        }
        assert_eq!(flattener.correlated_column_base_index(), 1);
        assert_eq!(flattener.base_binding, ColumnBinding::new(0, 0));
    }

    /// C++ `DependentJoinFlattener`: a correlated projection is pushed and the
    /// correlated columns are appended to the projection list.
    #[test]
    fn flattener_appends_correlated_columns_to_a_projection() {
        let map: HashMap<ColumnBinding, usize> =
            [(ColumnBinding::new(0, 1), 0)].into_iter().collect();
        let mut flattener = DependentJoinFlattener::new(vec![outer_col()], map);
        let plan = SubqueryPlan::projection(
            SubqueryPlan::filter(
                subquery_plan(),
                SubqueryExpr::eq(
                    SubqueryExpr::Column(outer_col()),
                    SubqueryExpr::Literal(Value::Int(1)),
                ),
            ),
            vec![SubqueryExpr::col(ColumnBinding::new(1, 0), 0)],
            vec!["x"],
            2,
        );
        flattener.detect_correlated_expressions(&plan).unwrap();
        let pushed = flattener.push_dependent_join(plan).unwrap();
        match pushed {
            SubqueryPlan::Projection { exprs, .. } => {
                assert_eq!(exprs.len(), 2);
                assert_eq!(
                    exprs[1],
                    SubqueryExpr::Column(DepthColumn::local(ColumnBinding::new(0, 0)))
                );
            }
            other => panic!("expected projection, got {other:?}"),
        }
        assert_eq!(flattener.correlated_expression_offset, 1);
    }

    /// C++ `SubqueryUnnest::UnnestUncorrelated`: a scalar subquery becomes a
    /// cross product over `Limit(1)` + `first()`, and the returned expression
    /// refers to the aggregate output.
    #[test]
    fn unnest_uncorrelated_scalar_builds_limit_aggregate_cross() {
        let mut root = SubqueryPlan::scan("t", vec!["a"], 0);
        let mut context = BindContext::new();
        let mut expr = SubqueryExpr::Subquery(Box::new(BoundSubquery::new(
            SubqueryType::Scalar,
            subquery_plan(),
        )));
        SubqueryUnnest::unnest_subqueries(&mut expr, &mut root, &mut context).unwrap();
        assert!(matches!(root, SubqueryPlan::CrossProduct { .. }));
        match root {
            SubqueryPlan::CrossProduct { right, .. } => match *right {
                SubqueryPlan::Aggregate { input, aggs, .. } => {
                    assert!(matches!(*input, SubqueryPlan::Limit { n: 1, .. }));
                    assert_eq!(aggs.len(), 1);
                }
                other => panic!("expected aggregate, got {other:?}"),
            },
            _ => unreachable!(),
        }
        assert!(matches!(expr, SubqueryExpr::Column(_)));
    }

    /// C++ `UnnestUncorrelated`: an `IN` subquery becomes a mark join and the
    /// result is the mark column.
    #[test]
    fn unnest_uncorrelated_in_builds_a_mark_join() {
        let mut root = SubqueryPlan::scan("t", vec!["a"], 0);
        let mut context = BindContext::new();
        let mut sub = BoundSubquery::new(SubqueryType::In, subquery_plan());
        sub.left = Some(SubqueryExpr::col(ColumnBinding::new(0, 0), 0));
        let mut expr = SubqueryExpr::Subquery(Box::new(sub));
        SubqueryUnnest::unnest_subqueries(&mut expr, &mut root, &mut context).unwrap();
        match root {
            SubqueryPlan::Join {
                conditions, mark, ..
            } => {
                assert!(mark);
                assert_eq!(conditions.len(), 1);
            }
            other => panic!("expected mark join, got {other:?}"),
        }
        assert!(matches!(expr, SubqueryExpr::Column(_)));
    }

    /// C++ `UnnestUncorrelated`: EXISTS is rejected, matching the reference.
    #[test]
    fn unnest_uncorrelated_exists_is_rejected() {
        let mut root = SubqueryPlan::scan("t", vec!["a"], 0);
        let mut context = BindContext::new();
        let mut expr = SubqueryExpr::Subquery(Box::new(BoundSubquery::new(
            SubqueryType::Exists,
            subquery_plan(),
        )));
        assert!(SubqueryUnnest::unnest_subqueries(&mut expr, &mut root, &mut context).is_err());
    }

    /// C++ `UnnestCorrelated`: a correlated EXISTS becomes a mark join with an
    /// equality condition on the correlated column.
    #[test]
    fn unnest_correlated_exists_builds_mark_join_conditions() {
        let mut root = SubqueryPlan::scan("t", vec!["a"], 0);
        let mut context = BindContext::new();
        let sub = BoundSubquery::new(
            SubqueryType::Exists,
            SubqueryPlan::filter(
                subquery_plan(),
                SubqueryExpr::eq(
                    SubqueryExpr::Column(outer_col()),
                    SubqueryExpr::Literal(Value::Int(1)),
                ),
            ),
        )
        .with_correlated(vec![outer_col()]);
        let mut expr = SubqueryExpr::Subquery(Box::new(sub));
        SubqueryUnnest::unnest_subqueries(&mut expr, &mut root, &mut context).unwrap();
        match root {
            SubqueryPlan::Join {
                conditions, mark, ..
            } => {
                assert!(mark);
                assert_eq!(conditions.len(), 1);
            }
            other => panic!("expected mark join, got {other:?}"),
        }
    }
}

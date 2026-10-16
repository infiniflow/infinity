//! Physical `EXPLAIN` text (plan §6 Gate A). Reproduces the C++
//! `ExplainPhysicalPlan` printer for the supported plan shapes:
//!   * `CREATE TABLE` (a fixed 5-line block),
//!   * a single-table `SELECT` (`TABLE SCAN` with an optional `FILTER` and/or
//!     `SORT`, under a final `PROJECT`).
//!
//! Node ids follow the planner's creation order (leaf = 2, +1 upward) and the
//! projection's `table index` is the binding counter; column ids (`(#n)`) are
//! assigned bottom-up in first-appearance order.

use std::collections::HashMap;

use infinity_parser::{BinOp, BoolOp, CmpOp, ExprAst, OrderKey, Predicate, SelectItem};

/// A single-table `SELECT` reduced to what the printer needs.
pub struct SelectPlan<'a> {
    pub items: &'a [SelectItem],
    /// Qualified table name, e.g. `default_db.t`.
    pub table: &'a str,
    pub where_pred: Option<&'a Predicate>,
    pub order_by: &'a [OrderKey],
    /// `SELECT DISTINCT …` over plain columns (no aggregates).
    pub distinct: bool,
    pub schema: &'a [String],
}

/// The node header line: `PROJECT (3)` at the root, `   -> SORT (4)` below it.
fn header(depth: usize, name: &str, id: u64) -> String {
    if depth == 0 {
        format!(" {name} ({id})")
    } else {
        format!("{}-> {name} ({id})", " ".repeat(2 * depth - 1))
    }
}

/// An attribute line: `  - table index: #4`.
fn attr(depth: usize, text: &str) -> String {
    format!("{}- {text}", " ".repeat(2 * (depth + 1)))
}

/// Appends `name` to `out` (unique), keeping `__rowid` last.
fn add_col(out: &mut Vec<String>, name: &str) {
    if out.iter().any(|c| c == name) {
        return;
    }
    if name == "__rowid" {
        out.push(name.to_string());
    } else if let Some(pos) = out.iter().position(|c| c == "__rowid") {
        out.insert(pos, name.to_string());
    } else {
        out.push(name.to_string());
    }
}

/// Column names referenced by an expression, in order of first appearance.
fn expr_cols(e: &ExprAst, out: &mut Vec<String>) {
    match e {
        ExprAst::Column(c) => add_col(out, c),
        ExprAst::Literal(_) => {}
        ExprAst::Func { args, .. } => {
            for a in args {
                expr_cols(a, out);
            }
        }
        ExprAst::Binary(a, _, b) | ExprAst::Cmp(a, _, b) => {
            expr_cols(a, out);
            expr_cols(b, out);
        }
        ExprAst::Not(a) => expr_cols(a, out),
        ExprAst::And(a, b) | ExprAst::Or(a, b) => {
            expr_cols(a, out);
            expr_cols(b, out);
        }
    }
}

fn pred_cols(p: &Predicate, out: &mut Vec<String>) {
    match p {
        Predicate::Cmp(a, _, b) => {
            expr_cols(a, out);
            expr_cols(b, out);
        }
        Predicate::Bool(a, _, b) => {
            pred_cols(a, out);
            pred_cols(b, out);
        }
        Predicate::Not(a) => pred_cols(a, out),
        Predicate::IsNull(e, _) | Predicate::IsTrue(e) => expr_cols(e, out),
        Predicate::Like(a, b, ..) => {
            expr_cols(a, out);
            expr_cols(b, out);
        }
        Predicate::In(a, list, _) => {
            expr_cols(a, out);
            for e in list {
                expr_cols(e, out);
            }
        }
    }
}

fn bin_text(op: BinOp) -> &'static str {
    match op {
        BinOp::Add => "+",
        BinOp::Sub => "-",
        BinOp::Mul => "*",
        BinOp::Div => "/",
        BinOp::Mod => "%",
    }
}

fn cmp_text(op: CmpOp) -> &'static str {
    match op {
        CmpOp::Eq => "=",
        CmpOp::Ne => "!=",
        CmpOp::Lt => "<",
        CmpOp::Le => "<=",
        CmpOp::Gt => ">",
        CmpOp::Ge => ">=",
    }
}

fn bool_text(op: BoolOp) -> &'static str {
    match op {
        BoolOp::And => "AND",
        BoolOp::Or => "OR",
    }
}

/// Renders an expression, annotating each column with `(#id)`.
fn render_expr(e: &ExprAst, ids: &HashMap<String, usize>) -> String {
    match e {
        ExprAst::Column(c) => match ids.get(c) {
            Some(i) => format!("{c} (#{i})"),
            None => c.clone(),
        },
        ExprAst::Literal(l) => l.to_slt_string(),
        ExprAst::Func { name, args } => {
            let inner: Vec<String> = args.iter().map(|a| render_expr(a, ids)).collect();
            format!("{}({})", name.to_ascii_lowercase(), inner.join(", "))
        }
        ExprAst::Binary(a, op, b) => format!(
            "{} {} {}",
            render_expr(a, ids),
            bin_text(*op),
            render_expr(b, ids)
        ),
        ExprAst::Cmp(a, op, b) => format!(
            "{} {} {}",
            render_expr(a, ids),
            cmp_text(*op),
            render_expr(b, ids)
        ),
        ExprAst::Not(a) => format!("NOT {}", render_expr(a, ids)),
        ExprAst::And(a, b) => format!("{} AND {}", render_expr(a, ids), render_expr(b, ids)),
        ExprAst::Or(a, b) => format!("{} OR {}", render_expr(a, ids), render_expr(b, ids)),
    }
}

/// A comparison operand: a bare column is wrapped in `CAST(… AS BigInt)`.
fn render_cmp_operand(e: &ExprAst, ids: &HashMap<String, usize>) -> String {
    match e {
        ExprAst::Column(_) => format!("CAST({} AS BigInt)", render_expr(e, ids)),
        other => render_expr(other, ids),
    }
}

fn render_pred(p: &Predicate, ids: &HashMap<String, usize>) -> String {
    match p {
        Predicate::Cmp(a, op, b) => format!(
            "{} {} {}",
            render_cmp_operand(a, ids),
            cmp_text(*op),
            render_cmp_operand(b, ids)
        ),
        Predicate::Bool(a, op, b) => format!(
            "({}) {} ({})",
            render_pred(a, ids),
            bool_text(*op),
            render_pred(b, ids)
        ),
        Predicate::Not(a) => format!("NOT ({})", render_pred(a, ids)),
        Predicate::IsNull(e, neg) => format!(
            "{} IS {}NULL",
            render_expr(e, ids),
            if *neg { "NOT " } else { "" }
        ),
        Predicate::IsTrue(e) => render_expr(e, ids),
        Predicate::Like(a, b, _, neg) => format!(
            "{} {}LIKE {}",
            render_expr(a, ids),
            if *neg { "NOT " } else { "" },
            render_expr(b, ids)
        ),
        Predicate::In(a, list, neg) => {
            let items: Vec<String> = list.iter().map(|e| render_expr(e, ids)).collect();
            format!(
                "{} {}IN ({})",
                render_expr(a, ids),
                if *neg { "NOT " } else { "" },
                items.join(", ")
            )
        }
    }
}

/// True when any select item is an aggregate (those plans are not handled here).
pub fn has_aggregate(items: &[SelectItem]) -> bool {
    items
        .iter()
        .any(|it| matches!(it, SelectItem::Aggregate(_)))
}

/// A `SELECT … COUNT(DISTINCT col) … [GROUP BY keys]` plan: the C++ engine
/// builds two stacked aggregates (inner dedup, outer aggregate).
pub struct DistinctAggPlan<'a> {
    pub items: &'a [SelectItem],
    pub table: &'a str,
    pub group_by: &'a [ExprAst],
    pub order_by: &'a [OrderKey],
}

/// Recognises the supported distinct-aggregate shape: every aggregate is
/// `DISTINCT <plain column>` and every plain select item is a group key.
pub fn is_distinct_agg_plan(items: &[SelectItem], group_by: &[ExprAst]) -> bool {
    let keys: Vec<String> = group_by
        .iter()
        .filter_map(|e| match e {
            ExprAst::Column(c) => Some(c.clone()),
            _ => None,
        })
        .collect();
    if keys.len() != group_by.len() {
        return false;
    }
    let mut seen_agg = false;
    for it in items {
        match it {
            SelectItem::Aggregate(a) => {
                let plain = a.arg_exprs.first().map(|o| o.is_none()).unwrap_or(true);
                if !a.distinct || a.args.len() != 1 || !plain {
                    return false;
                }
                seen_agg = true;
            }
            SelectItem::Column(c) => {
                if !keys.iter().any(|k| k == c) {
                    return false;
                }
            }
            _ => return false,
        }
    }
    seen_agg
}

fn func_label(f: infinity_parser::AggFunc) -> String {
    f.as_str().to_ascii_uppercase()
}

/// Physical `EXPLAIN` for the stacked distinct-aggregate plan.
pub fn explain_distinct_agg(p: &DistinctAggPlan) -> Vec<String> {
    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");
    let keys: Vec<String> = p
        .group_by
        .iter()
        .map(|e| match e {
            ExprAst::Column(c) => c.clone(),
            other => render_expr(other, &HashMap::new()),
        })
        .collect();
    // distinct aggregate (func, column) pairs, in select order
    let aggs: Vec<(infinity_parser::AggFunc, String)> = p
        .items
        .iter()
        .filter_map(|it| match it {
            SelectItem::Aggregate(a) => Some((a.func, a.args[0].clone())),
            _ => None,
        })
        .collect();

    // column ids: group keys first, then the columns of the select items
    let mut ids: HashMap<String, usize> = HashMap::new();
    for k in &keys {
        if !ids.contains_key(k) {
            ids.insert(k.clone(), ids.len());
        }
    }
    for it in p.items {
        let c = match it {
            SelectItem::Column(c) => Some(c.clone()),
            SelectItem::Aggregate(a) => Some(a.args[0].clone()),
            _ => None,
        };
        if let Some(c) = c {
            if !ids.contains_key(&c) {
                ids.insert(c.clone(), ids.len());
            }
        }
    }
    let anno = |c: &str| match ids.get(c) {
        Some(i) => format!("{c} (#{i})"),
        None => c.to_string(),
    };

    let has_sort = !p.order_by.is_empty();
    let project_id: u64 = if has_sort { 6 } else { 5 };

    let mut lines = Vec::new();
    lines.push(header(0, "PROJECT", project_id));
    lines.push(attr(0, "table index: #8"));
    let exprs: Vec<String> = p
        .items
        .iter()
        .map(|it| match it {
            SelectItem::Column(c) => anno(c),
            SelectItem::Aggregate(a) => {
                // `count(DISTINCT c) (#<id>)` — the id trails the whole call
                let id = ids.get(&a.args[0]).copied().unwrap_or(0);
                format!("{}(DISTINCT {}) (#{id})", a.func.as_str(), a.args[0])
            }
            _ => String::new(),
        })
        .collect();
    lines.push(attr(0, &format!("expressions: [{}]", exprs.join(", "))));

    if has_sort {
        lines.push(header(1, "SORT", 5));
        let skeys: Vec<String> = p
            .order_by
            .iter()
            .map(|k| format!("{} {}", anno(&k.column), if k.asc { "ASC" } else { "DESC" }))
            .collect();
        lines.push(attr(1, &format!("expressions: [{}]", skeys.join(", "))));
        let labels: Vec<String> = p
            .items
            .iter()
            .map(|it| match it {
                SelectItem::Column(c) => c.clone(),
                SelectItem::Aggregate(a) => {
                    format!("{}({})", func_label(a.func), a.args[0])
                }
                _ => String::new(),
            })
            .collect();
        lines.push(attr(1, &format!("output columns: [{}]", labels.join(", "))));
    }

    let outer_d = if has_sort { 2 } else { 1 };
    lines.push(header(outer_d, "AGGREGATE", 4));
    lines.push(attr(outer_d, "aggregate table index: #6"));
    let agg_txt: Vec<String> = aggs
        .iter()
        .map(|(f, c)| format!("{}({})", func_label(*f), anno(c)))
        .collect();
    lines.push(attr(
        outer_d,
        &format!("aggregate: [{}]", agg_txt.join(", ")),
    ));
    if !keys.is_empty() {
        lines.push(attr(outer_d, "group by table index: #7"));
        let kt: Vec<String> = keys.iter().map(|k| anno(k)).collect();
        lines.push(attr(outer_d, &format!("group by: [{}]", kt.join(", "))));
    }

    let inner_d = outer_d + 1;
    lines.push(header(inner_d, "AGGREGATE", 3));
    lines.push(attr(inner_d, "aggregate table index: #5"));
    lines.push(attr(inner_d, "aggregate: []"));
    lines.push(attr(inner_d, "group by table index: #5"));
    let mut inner_cols = keys.clone();
    for (_, c) in &aggs {
        add_col(&mut inner_cols, c);
    }
    let it: Vec<String> = inner_cols.iter().map(|c| anno(c)).collect();
    lines.push(attr(inner_d, &format!("group by: [{}]", it.join(", "))));

    let scan_d = inner_d + 1;
    lines.push(header(scan_d, "TABLE SCAN", 2));
    lines.push(attr(scan_d, &format!("table name: {alias}({db}.{alias})")));
    lines.push(attr(scan_d, "table index: #1"));
    let mut out = keys.clone();
    for (_, c) in &aggs {
        add_col(&mut out, c);
    }
    add_col(&mut out, "__rowid");
    lines.push(attr(
        scan_d,
        &format!("output_columns: [{}]", out.join(", ")),
    ));
    lines
}

/// Produces the physical `EXPLAIN` text for a single-table `SELECT`.
pub fn explain_select(p: &SelectPlan) -> Vec<String> {
    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");

    // 1. column ids, assigned bottom-up (filter, sort, project).
    let mut ids: HashMap<String, usize> = HashMap::new();
    let mut filter_cols = Vec::new();
    if let Some(w) = p.where_pred {
        pred_cols(w, &mut filter_cols);
    }
    for c in &filter_cols {
        if !ids.contains_key(c) {
            ids.insert(c.clone(), ids.len());
        }
    }
    let sort_cols: Vec<String> = p.order_by.iter().map(|k| k.column.clone()).collect();
    for c in &sort_cols {
        if !ids.contains_key(c) {
            ids.insert(c.clone(), ids.len());
        }
    }
    let mut project_cols = Vec::new();
    for it in p.items {
        match it {
            SelectItem::Star => {
                for c in p.schema {
                    add_col(&mut project_cols, c);
                }
            }
            SelectItem::Column(c) => add_col(&mut project_cols, c),
            SelectItem::Expr(e) => expr_cols(e, &mut project_cols),
            _ => {}
        }
    }
    for c in &project_cols {
        if !ids.contains_key(c) {
            ids.insert(c.clone(), ids.len());
        }
    }

    // 2. the operator chain from the leaf upward.
    let has_filter = p.where_pred.is_some();
    let has_sort = !p.order_by.is_empty();
    let has_distinct = p.distinct && !has_filter && !has_sort;
    let mut ops: Vec<&str> = vec!["TABLE SCAN"];
    if has_filter {
        ops.push("FILTER");
    }
    if has_distinct {
        ops.push("AGGREGATE");
    }
    if has_sort {
        ops.push("SORT");
    }
    ops.push("PROJECT");
    let node_id = |i: usize| 2 + i as u64;
    let depth_of = |i: usize| ops.len() - 1 - i;

    // 3. output columns, bottom-up.
    let mut scan_out: Vec<String> = Vec::new();
    let required_by_parent: Vec<String> = if has_filter {
        filter_cols.clone()
    } else if has_distinct {
        project_cols.clone()
    } else if has_sort {
        sort_cols.clone()
    } else {
        project_cols.clone()
    };
    for c in &required_by_parent {
        add_col(&mut scan_out, c);
    }
    add_col(&mut scan_out, "__rowid");

    let mut lines: Vec<String> = Vec::new();
    let project_idx = ops.len() - 1;
    lines.push(header(0, "PROJECT", node_id(project_idx)));
    lines.push(attr(0, "table index: #4"));
    lines.push(attr(
        0,
        &format!("expressions: [{}]", project_exprs(p, &ids).join(", ")),
    ));

    let mut child_out = scan_out.clone();
    for (i, op) in ops.iter().enumerate().rev() {
        if i == project_idx {
            continue;
        }
        let d = depth_of(i);
        lines.push(header(d, op, node_id(i)));
        match *op {
            "SORT" => {
                let keys: Vec<String> = p
                    .order_by
                    .iter()
                    .map(|k| {
                        let col = match ids.get(&k.column) {
                            Some(id) => format!("{} (#{id})", k.column),
                            None => k.column.clone(),
                        };
                        format!("{col} {}", if k.asc { "ASC" } else { "DESC" })
                    })
                    .collect();
                lines.push(attr(d, &format!("expressions: [{}]", keys.join(", "))));
                let mut out = child_out.clone();
                for c in &sort_cols {
                    add_col(&mut out, c);
                }
                lines.push(attr(d, &format!("output columns: [{}]", out.join(", "))));
                child_out = out;
            }
            "AGGREGATE" => {
                lines.push(attr(d, "aggregate table index: #5"));
                lines.push(attr(d, "aggregate: []"));
                lines.push(attr(d, "group by table index: #5"));
                let gb: Vec<String> = project_cols
                    .iter()
                    .map(|c| match ids.get(c) {
                        Some(id) => format!("{c} (#{id})"),
                        None => c.clone(),
                    })
                    .collect();
                lines.push(attr(d, &format!("group by: [{}]", gb.join(", "))));
            }
            "FILTER" => {
                let f = p
                    .where_pred
                    .map(|w| render_pred(w, &ids))
                    .unwrap_or_default();
                lines.push(attr(d, &format!("filter: {f}")));
                let mut out = child_out.clone();
                for c in &filter_cols {
                    add_col(&mut out, c);
                }
                lines.push(attr(d, &format!("output columns: [{}]", out.join(", "))));
                child_out = out;
            }
            _ => {
                lines.push(attr(d, &format!("table name: {alias}({db}.{alias})")));
                lines.push(attr(d, "table index: #1"));
                lines.push(attr(
                    d,
                    &format!("output_columns: [{}]", child_out.join(", ")),
                ));
            }
        }
    }
    lines
}

fn project_exprs(p: &SelectPlan, ids: &HashMap<String, usize>) -> Vec<String> {
    let mut out = Vec::new();
    for it in p.items {
        match it {
            SelectItem::Star => {
                for c in p.schema {
                    out.push(match ids.get(c) {
                        Some(i) => format!("{c} (#{i})"),
                        None => c.clone(),
                    });
                }
            }
            SelectItem::Column(c) => out.push(match ids.get(c) {
                Some(i) => format!("{c} (#{i})"),
                None => c.clone(),
            }),
            SelectItem::Expr(e) => out.push(render_expr(e, ids)),
            SelectItem::Func { name, args } => {
                let inner: Vec<String> = args.iter().map(|a| render_expr(a, ids)).collect();
                out.push(format!(
                    "{}({})",
                    name.to_ascii_lowercase(),
                    inner.join(", ")
                ));
            }
            _ => {}
        }
    }
    out
}

/// The `EXPLAIN CREATE TABLE` block.
pub fn explain_create_table(
    db: &str,
    name: &str,
    columns: &[(String, String, Option<String>)],
) -> Vec<String> {
    let rendered: Vec<String> = columns
        .iter()
        .map(|(n, ty, default)| {
            let d = default.clone().unwrap_or_else(|| "Null".to_string());
            format!("{n} {ty} default {d}")
        })
        .collect();
    vec![
        " CREATE TABLE (2)".to_string(),
        attr(0, &format!("database name: {db}")),
        attr(0, &format!("table name: {name}")),
        attr(0, &format!("columns: [{}]", rendered.join(", "))),
        attr(0, "conflict type: Issue error"),
        attr(0, "output columns: [OK]"),
    ]
}

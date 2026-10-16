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

use infinity_parser::{BinOp, BoolOp, CmpOp, ExprAst, Literal, OrderKey, Predicate, SelectItem};

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
    /// `EXPLAIN LOGICAL`: enables the logical-plan arithmetic cast rule.
    pub logical: bool,
}

/// The node header line: `PROJECT (3)` at the root, `   -> SORT (4)` below it.
pub(crate) fn header(depth: usize, name: &str, id: u64) -> String {
    if depth == 0 {
        format!(" {name} ({id})")
    } else {
        format!("{}-> {name} ({id})", " ".repeat(2 * depth - 1))
    }
}

/// An attribute line: `  - table index: #4`.
pub(crate) fn attr(depth: usize, text: &str) -> String {
    format!("{}- {text}", " ".repeat(2 * (depth + 1)))
}

/// Appends `name` to `out` (unique), keeping `__rowid` last.
pub(crate) fn add_col(out: &mut Vec<String>, name: &str) {
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
pub(crate) fn expr_cols(e: &ExprAst, out: &mut Vec<String>) {
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

pub(crate) fn bin_text(op: BinOp) -> &'static str {
    match op {
        BinOp::Add => "+",
        BinOp::Sub => "-",
        BinOp::Mul => "*",
        BinOp::Div => "/",
        BinOp::Mod => "%",
    }
}

pub(crate) fn cmp_text(op: CmpOp) -> &'static str {
    match op {
        CmpOp::Eq => "=",
        CmpOp::Ne => "!=",
        CmpOp::Lt => "<",
        CmpOp::Le => "<=",
        CmpOp::Gt => ">",
        CmpOp::Ge => ">=",
    }
}

pub(crate) fn bool_text(op: BoolOp) -> &'static str {
    match op {
        BoolOp::And => "AND",
        BoolOp::Or => "OR",
    }
}

/// Renders an expression, annotating each column with `(#id)`.
pub(crate) fn render_expr(e: &ExprAst, ids: &HashMap<String, usize>) -> String {
    render_expr_m(e, ids, false)
}

/// An arithmetic operand. In the *logical* plan an `INTEGER` column that meets a
/// `BIGINT` literal is cast to BigInt (`CAST(c AS BigInt) + 1`); a column met by
/// another column is not (`c1 + c2`).
fn arith_operand(this: &ExprAst, other: &ExprAst, ids: &HashMap<String, usize>) -> String {
    if matches!(this, ExprAst::Column(_)) && matches!(other, ExprAst::Literal(Literal::Int(_))) {
        format!("CAST({} AS BigInt)", render_expr(this, ids))
    } else {
        render_expr_m(this, ids, true)
    }
}

/// Renders an expression, with `logical` enabling the logical-plan arithmetic
/// cast rule (see [`arith_operand`]).
pub(crate) fn render_expr_m(e: &ExprAst, ids: &HashMap<String, usize>, logical: bool) -> String {
    match e {
        ExprAst::Column(c) => match ids.get(c) {
            Some(i) => format!("{c} (#{i})"),
            None => c.clone(),
        },
        ExprAst::Literal(l) => l.to_slt_string(),
        ExprAst::Func { name, args }
            if name.eq_ignore_ascii_case("filter_fulltext")
                || name.eq_ignore_ascii_case("filter_text") =>
        {
            fulltext_filter_text(args)
        }
        ExprAst::Func { name, args } => {
            let inner: Vec<String> = args
                .iter()
                .map(|a| render_expr_m(a, ids, logical))
                .collect();
            format!("{}({})", name.to_ascii_lowercase(), inner.join(", "))
        }
        ExprAst::Binary(a, op, b) => {
            let (ra, rb) = if logical {
                (arith_operand(a, b, ids), arith_operand(b, a, ids))
            } else {
                (
                    render_expr_m(a, ids, logical),
                    render_expr_m(b, ids, logical),
                )
            };
            format!("{ra} {} {rb}", bin_text(*op))
        }
        ExprAst::Cmp(a, op, b) => format!(
            "{} {} {}",
            render_expr_m(a, ids, logical),
            cmp_text(*op),
            render_expr_m(b, ids, logical)
        ),
        ExprAst::Not(a) => format!("NOT {}", render_expr_m(a, ids, logical)),
        ExprAst::And(a, b) => format!(
            "{} AND {}",
            render_expr_m(a, ids, logical),
            render_expr_m(b, ids, logical)
        ),
        ExprAst::Or(a, b) => format!(
            "{} OR {}",
            render_expr_m(a, ids, logical),
            render_expr_m(b, ids, logical)
        ),
    }
}

/// A comparison operand: a bare column is wrapped in `CAST(… AS BigInt)`.
pub(crate) fn render_cmp_operand(e: &ExprAst, ids: &HashMap<String, usize>) -> String {
    match e {
        ExprAst::Column(_) => format!("CAST({} AS BigInt)", render_expr(e, ids)),
        other => render_expr(other, ids),
    }
}

pub(crate) fn render_pred_m(p: &Predicate, ids: &HashMap<String, usize>, logical: bool) -> String {
    match p {
        Predicate::Cmp(a, op, b) => format!(
            "{} {} {}",
            render_cmp_operand(a, ids),
            cmp_text(*op),
            render_cmp_operand(b, ids)
        ),
        Predicate::Bool(a, op, b) => format!(
            "{} {} {}",
            wrap_bool_operand(a, render_pred_m(a, ids, logical)),
            bool_text(*op),
            wrap_bool_operand(b, render_pred_m(b, ids, logical))
        ),
        Predicate::Not(a) => format!("NOT({})", render_pred_m(a, ids, logical)),
        Predicate::IsNull(e, neg) => format!(
            "{} IS {}NULL",
            render_expr_m(e, ids, logical),
            if *neg { "NOT " } else { "" }
        ),
        Predicate::IsTrue(e) => render_expr_m(e, ids, logical),
        Predicate::Like(a, b, _, neg) => format!(
            "{} {}LIKE {}",
            render_expr_m(a, ids, logical),
            if *neg { "NOT " } else { "" },
            render_expr_m(b, ids, logical)
        ),
        Predicate::In(a, list, neg) => {
            let items: Vec<String> = list
                .iter()
                .map(|e| render_expr_m(e, ids, logical))
                .collect();
            format!(
                "{} {}IN ({})",
                render_expr_m(a, ids, logical),
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

/// True when every aggregate has a single plain-column argument (the shapes the
/// printer renders). `COUNT(*)` and computed arguments are left unhandled.
pub fn aggregates_renderable(items: &[SelectItem]) -> bool {
    items.iter().all(|it| match it {
        SelectItem::Aggregate(a) => {
            a.args.len() == 1 && a.arg_exprs.first().map(|o| o.is_none()).unwrap_or(true)
        }
        _ => true,
    })
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
    let mut n_agg = 0;
    for it in items {
        match it {
            SelectItem::Aggregate(a) => {
                let plain = a.arg_exprs.first().map(|o| o.is_none()).unwrap_or(true);
                if !a.distinct || a.args.len() != 1 || !plain {
                    return false;
                }
                n_agg += 1;
            }
            SelectItem::Column(c) => {
                if !keys.iter().any(|k| k == c) {
                    return false;
                }
            }
            _ => return false,
        }
    }
    // the stacked-AGGREGATE form only applies to a single distinct aggregate
    n_agg == 1
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

/// Physical `EXPLAIN` for the hash/merge-hash aggregate plan (several
/// aggregates, or any non-distinct aggregate).
pub struct HashAggPlan<'a> {
    pub items: &'a [SelectItem],
    pub table: &'a str,
    pub group_by: &'a [ExprAst],
    pub order_by: &'a [OrderKey],
}

pub fn explain_hash_agg(p: &HashAggPlan) -> Vec<String> {
    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");
    let keys: Vec<String> = p
        .group_by
        .iter()
        .filter_map(|e| match e {
            ExprAst::Column(c) => Some(c.clone()),
            _ => None,
        })
        .collect();
    let aggs: Vec<(infinity_parser::AggFunc, String, bool)> = p
        .items
        .iter()
        .filter_map(|it| match it {
            SelectItem::Aggregate(a) => Some((a.func, a.args[0].clone(), a.distinct)),
            _ => None,
        })
        .collect();

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
    let mut lines = Vec::new();
    lines.push(header(0, "PROJECT", if has_sort { 6 } else { 5 }));
    lines.push(attr(0, "table index: #8"));
    let exprs: Vec<String> = p
        .items
        .iter()
        .map(|it| match it {
            SelectItem::Column(c) => anno(c),
            SelectItem::Aggregate(a) => {
                let id = ids.get(&a.args[0]).copied().unwrap_or(0);
                if a.distinct {
                    format!("{}(DISTINCT {}) (#{id})", a.func.as_str(), a.args[0])
                } else {
                    format!("{}({}) (#{id})", a.func.as_str(), a.args[0])
                }
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
        let mut labels: Vec<String> = keys.clone();
        for it in p.items {
            if let SelectItem::Aggregate(a) = it {
                let l = if a.distinct {
                    format!("{}(DISTINCT {})", func_label(a.func), a.args[0])
                } else {
                    format!("{}({})", func_label(a.func), a.args[0])
                };
                labels.push(l);
            }
        }
        lines.push(attr(1, &format!("output columns: [{}]", labels.join(", "))));
    }

    let merge_d = if has_sort { 2 } else { 1 };
    lines.push(header(merge_d, "MERGE HASH AGGREGATE", 4));
    if !keys.is_empty() {
        let kt: Vec<String> = keys.iter().map(|k| anno(k)).collect();
        lines.push(attr(merge_d, &format!("group by: [{}]", kt.join(", "))));
    }
    let agg_txt: Vec<String> = aggs
        .iter()
        .map(|(f, c, d)| {
            if *d {
                format!("{}(DISTINCT {})", func_label(*f), anno(c))
            } else {
                format!("{}({})", func_label(*f), anno(c))
            }
        })
        .collect();
    lines.push(attr(
        merge_d,
        &format!("aggregates: [{}]", agg_txt.join(", ")),
    ));

    let hash_d = merge_d + 1;
    lines.push(header(hash_d, "HASH AGGREGATE", 3));
    if !keys.is_empty() {
        let kt: Vec<String> = keys.iter().map(|k| anno(k)).collect();
        lines.push(attr(hash_d, &format!("group by: [{}]", kt.join(", "))));
    }
    let distinct_cols: Vec<String> = aggs
        .iter()
        .filter(|(_, _, d)| *d)
        .map(|(_, c, _)| anno(c))
        .collect();
    if !distinct_cols.is_empty() {
        lines.push(attr(
            hash_d,
            &format!("distinct columns: [{}]", distinct_cols.join(", ")),
        ));
    }
    let non_distinct: Vec<String> = aggs
        .iter()
        .filter(|(_, _, d)| !*d)
        .map(|(_, c, _)| anno(c))
        .collect();
    if !non_distinct.is_empty() {
        lines.push(attr(
            hash_d,
            &format!("non-distinct columns: [{}]", non_distinct.join(", ")),
        ));
    }

    let scan_d = hash_d + 1;
    lines.push(header(scan_d, "TABLE SCAN", 2));
    lines.push(attr(scan_d, &format!("table name: {alias}({db}.{alias})")));
    lines.push(attr(scan_d, "table index: #1"));
    let mut out = keys.clone();
    for (_, c, _) in &aggs {
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
        &format!(
            "expressions: [{}]",
            project_exprs(p.items, p.schema, p.logical, &ids).join(", ")
        ),
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
                    .map(|w| render_pred_m(w, &ids, p.logical))
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
                let key = if p.logical {
                    "output columns"
                } else {
                    "output_columns"
                };
                lines.push(attr(d, &format!("{key}: [{}]", child_out.join(", "))));
            }
        }
    }
    lines
}

fn project_exprs(
    items: &[SelectItem],
    schema: &[String],
    logical: bool,
    ids: &HashMap<String, usize>,
) -> Vec<String> {
    let mut out = Vec::new();
    for it in items {
        match it {
            SelectItem::Star => {
                for c in schema {
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
            SelectItem::Expr(e) => out.push(render_expr_m(e, ids, logical)),
            SelectItem::Func { name, args } => {
                let inner: Vec<String> = args
                    .iter()
                    .map(|a| render_expr_m(a, ids, logical))
                    .collect();
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

/// A `SELECT` whose `WHERE` can be pushed into an index scan.
pub struct IndexScanPlan<'a> {
    pub items: &'a [SelectItem],
    pub table: &'a str,
    pub where_pred: &'a Predicate,
    pub order_by: &'a [OrderKey],
    pub schema: &'a [String],
    /// Declared type text per column (schema order), to decide BigInt casts.
    pub types: &'a [String],
    /// Columns carrying any index.
    pub indexed: &'a [String],
}

/// True for the integer column types that a comparison implicitly widens to
/// `BigInt` in the reference plan (`CAST(col AS BigInt)`).
fn is_integer_type(text: &str) -> bool {
    let t = text.to_ascii_uppercase();
    t.contains("INT") && !t.contains("POINT") && !t.contains("EMBEDDING") && !t.contains("TENSOR")
}

/// Splits a predicate on top-level `AND`s.
fn split_and(p: &Predicate) -> Vec<&Predicate> {
    match p {
        Predicate::Bool(a, BoolOp::And, b) => {
            let mut v = split_and(a);
            v.extend(split_and(b));
            v
        }
        _ => vec![p],
    }
}

/// The full-text filter pseudo-function name and the column it targets, if `e`
/// is a `filter_fulltext`/`filter_text` call.
fn fulltext_filter(e: &ExprAst) -> Option<String> {
    match e {
        ExprAst::Func { name, args }
            if name.eq_ignore_ascii_case("filter_fulltext")
                || name.eq_ignore_ascii_case("filter_text") =>
        {
            match args.first() {
                Some(ExprAst::Literal(Literal::Str(s))) => Some(s.clone()),
                Some(ExprAst::Column(c)) => Some(c.clone()),
                _ => None,
            }
        }
        _ => None,
    }
}

/// Renders a `filter_fulltext`/`filter_text` call as `FILTER_FULLTEXT('a', 'b')`,
/// quoting every argument as the reference plan does.
fn fulltext_filter_text(args: &[ExprAst]) -> String {
    let quoted: Vec<String> = args
        .iter()
        .map(|a| match a {
            ExprAst::Literal(Literal::Str(s)) => format!("'{s}'"),
            ExprAst::Column(c) => format!("'{c}'"),
            other => render_expr(other, &HashMap::new()),
        })
        .collect();
    format!("FILTER_FULLTEXT({})", quoted.join(", "))
}

/// True when every column referenced by `p` is indexed and `p` uses neither
/// `NOT` nor `!=` (the conditions the engine refuses to push into an index).
fn is_index_eligible(p: &Predicate, indexed: &[String]) -> bool {
    fn has_neg(p: &Predicate) -> bool {
        match p {
            Predicate::Not(_) => true,
            Predicate::Bool(a, _, b) => has_neg(a) || has_neg(b),
            Predicate::Cmp(_, CmpOp::Ne, _) => true,
            _ => false,
        }
    }
    if has_neg(p) {
        return false;
    }
    // A full-text filter over an indexed column is directly pushable.
    if let Predicate::IsTrue(e) = p {
        if let Some(col) = fulltext_filter(e) {
            return indexed.iter().any(|x| x == &col);
        }
    }
    let mut cols = Vec::new();
    pred_cols(p, &mut cols);
    !cols.is_empty() && cols.iter().all(|c| indexed.iter().any(|x| x == c))
}

/// A `.slt` literal rendered the way the index filter shows it: strings and
/// JSON paths lose their quotes (`value1`, `$.year`).
fn lit_plain(l: &Literal) -> String {
    match l {
        Literal::Str(s) => s.clone(),
        other => other.to_slt_string(),
    }
}

/// Renders one operand of an index-filter comparison.
fn idx_operand(e: &ExprAst, ann: &dyn Fn(&str) -> String, cast: &dyn Fn(&str) -> bool) -> String {
    match e {
        ExprAst::Column(c) if cast(c) => format!("CAST({} AS BigInt)", ann(c)),
        ExprAst::Column(c) => ann(c),
        ExprAst::Func { name, args } if name.starts_with("json_extract") => {
            let col = match args.first() {
                Some(ExprAst::Column(c)) => ann(c),
                _ => String::new(),
            };
            let path = args
                .get(1)
                .and_then(|a| match a {
                    ExprAst::Literal(l) => Some(lit_plain(l)),
                    _ => None,
                })
                .unwrap_or_default();
            format!("({col} {name} {path})")
        }
        ExprAst::Literal(l) => lit_plain(l),
        other => render_expr(other, &HashMap::new()),
    }
}

/// True when `p` is a bare `filter_fulltext`/`filter_text` call.
fn is_fulltext_pred(p: &Predicate) -> bool {
    matches!(p, Predicate::IsTrue(e) if fulltext_filter(e).is_some())
}

/// Parenthesizes a boolean operand unless it is a full-text filter (the
/// reference plan leaves `FILTER_FULLTEXT(…)` bare inside `AND`/`OR`).
fn wrap_bool_operand(p: &Predicate, s: String) -> String {
    if is_fulltext_pred(p) {
        s
    } else {
        format!("({s})")
    }
}

/// Renders a json *predicate* function (`json_contains`, `json_exists_path`).
fn idx_json_pred(name: &str, args: &[ExprAst], ann: &dyn Fn(&str) -> String) -> Option<String> {
    let col = match args.first() {
        Some(ExprAst::Column(c)) => ann(c),
        _ => return None,
    };
    let second = args.get(1).and_then(|a| match a {
        ExprAst::Literal(l) => Some(lit_plain(l)),
        _ => None,
    });
    match (name, args.len()) {
        ("json_exists_path", 2) => Some(format!("{col} json_exists_path {}", second?)),
        ("json_contains", 2) => Some(format!("{col} json_contains {}", second?)),
        ("json_contains", 3) => {
            let third = match args.get(2) {
                Some(ExprAst::Literal(l)) => lit_plain(l),
                _ => return None,
            };
            Some(format!("json_contains({col}, {}, {third})", second?))
        }
        _ => None,
    }
}

/// Renders an index-filter predicate, qualifying column ids as `#1.<col>`.
fn idx_pred(p: &Predicate, ann: &dyn Fn(&str) -> String, cast: &dyn Fn(&str) -> bool) -> String {
    match p {
        Predicate::Cmp(a, op, b) => {
            format!(
                "{} {} {}",
                idx_operand(a, ann, cast),
                cmp_text(*op),
                idx_operand(b, ann, cast)
            )
        }
        Predicate::Bool(a, op, b) => format!(
            "{} {} {}",
            wrap_bool_operand(a, idx_pred(a, ann, cast)),
            bool_text(*op),
            wrap_bool_operand(b, idx_pred(b, ann, cast))
        ),
        Predicate::Not(a) => format!("NOT({})", idx_pred(a, ann, cast)),
        Predicate::IsTrue(e) => {
            if let ExprAst::Func { args, .. } = e {
                if fulltext_filter(e).is_some() {
                    return fulltext_filter_text(args);
                }
            }
            if let ExprAst::Func { name, args } = e {
                if let Some(s) = idx_json_pred(name, args, ann) {
                    return s;
                }
            }
            render_expr(e, &HashMap::new())
        }
        other => render_pred_m(other, &HashMap::new(), false),
    }
}

/// The physical `EXPLAIN` text for a plan whose filter pushes into an index
/// scan (`PROJECT [-> SORT] [-> FILTER] -> INDEX SCAN`). Returns `None` when no
/// conjunct is index-eligible.
pub fn explain_index_scan(p: &IndexScanPlan) -> Option<Vec<String>> {
    let conjuncts = split_and(p.where_pred);
    let mut idx: Vec<&Predicate> = Vec::new();
    let mut left: Vec<&Predicate> = Vec::new();
    for c in conjuncts {
        if is_index_eligible(c, p.indexed) {
            idx.push(c);
        } else {
            left.push(c);
        }
    }
    if idx.is_empty() {
        return None;
    }

    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");
    let ann = |c: &str| match p.schema.iter().position(|x| x == c) {
        Some(i) => format!("{c} (#1.{i})"),
        None => c.to_string(),
    };
    // A comparison widens an integer column to BigInt, but never a varchar/date.
    let cast = |c: &str| match p.schema.iter().position(|x| x == c) {
        Some(i) => p.types.get(i).map(|t| is_integer_type(t)).unwrap_or(false),
        None => false,
    };
    // index filter: one conjunct at top level, several `(a) AND (b)`.
    let index_filter =
        idx.iter()
            .map(|c| idx_pred(c, &ann, &cast))
            .fold(String::new(), |acc, s| {
                if acc.is_empty() {
                    s
                } else {
                    format!("({acc}) AND ({s})")
                }
            });

    // 1. column ids (bottom-up): leftover-filter columns, sort keys, projection.
    let mut leftover_cols = Vec::new();
    for c in &left {
        pred_cols(c, &mut leftover_cols);
    }
    let mut ids: HashMap<String, usize> = HashMap::new();
    for c in &leftover_cols {
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

    let has_sort = !p.order_by.is_empty();
    let has_filter = !left.is_empty();
    let project_id = if has_sort { 5 } else { 4 };
    let index_id = if has_sort { 7 } else { 6 };

    let mut lines = Vec::new();
    lines.push(header(0, "PROJECT", project_id));
    lines.push(attr(0, "table index: #4"));
    lines.push(attr(
        0,
        &format!(
            "expressions: [{}]",
            project_exprs(p.items, p.schema, false, &ids).join(", ")
        ),
    ));

    let mut depth = 0;
    if has_sort {
        depth = 1;
        lines.push(header(1, "SORT", 4));
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
        lines.push(attr(1, &format!("expressions: [{}]", keys.join(", "))));
        let mut out = leftover_cols.clone();
        for c in &sort_cols {
            add_col(&mut out, c);
        }
        add_col(&mut out, "__rowid");
        lines.push(attr(1, &format!("output columns: [{}]", out.join(", "))));
    }
    if has_filter {
        depth += 1;
        lines.push(header(depth, "FILTER", 3));
        let f = left
            .iter()
            .map(|c| render_pred_m(c, &ids, false))
            .collect::<Vec<_>>()
            .join(" AND ");
        lines.push(attr(depth, &format!("filter: {f}")));
        let mut out = leftover_cols.clone();
        add_col(&mut out, "__rowid");
        lines.push(attr(
            depth,
            &format!("output columns: [{}]", out.join(", ")),
        ));
    }

    let d = depth + 1;
    lines.push(header(d, "INDEX SCAN", index_id));
    lines.push(attr(d, &format!("table name: {alias}({db}.{alias})")));
    lines.push(attr(d, "table index: #1"));
    lines.push(attr(d, &format!("filter: {index_filter}")));
    lines.push(attr(d, "output_columns: [__rowid]"));
    Some(lines)
}

/// The logical-plan `DELETE` text (`EXPLAIN LOGICAL DELETE …`).
pub fn explain_logical_delete(table: &str, where_pred: Option<&Predicate>) -> Vec<String> {
    let qualified = if table.contains('.') {
        table.to_string()
    } else {
        format!("default_db.{table}")
    };
    let mut ids: HashMap<String, usize> = HashMap::new();
    let mut cols: Vec<String> = Vec::new();
    if let Some(w) = where_pred {
        pred_cols(w, &mut cols);
    }
    for c in &cols {
        if !ids.contains_key(c) {
            ids.insert(c.clone(), ids.len());
        }
    }
    let mut out = cols.clone();
    add_col(&mut out, "__rowid");

    let mut lines = vec![format!("DELETE FROM {qualified}")];
    if let Some(w) = where_pred {
        lines.push(header(1, "FILTER", 4));
        lines.push(attr(
            1,
            &format!("filter: {}", render_pred_m(w, &ids, true)),
        ));
        lines.push(attr(1, &format!("output columns: [{}]", out.join(", "))));
    }
    lines.push(header(
        if where_pred.is_some() { 2 } else { 1 },
        "TABLE SCAN",
        2,
    ));
    let alias = qualified.split('.').next_back().unwrap_or(&qualified);
    let db = qualified
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");
    lines.push(attr(
        if where_pred.is_some() { 2 } else { 1 },
        &format!("table name: {alias}({db}.{alias})"),
    ));
    lines.push(attr(
        if where_pred.is_some() { 2 } else { 1 },
        "table index: #1",
    ));
    lines.push(attr(
        if where_pred.is_some() { 2 } else { 1 },
        &format!("output columns: [{}]", out.join(", ")),
    ));
    lines
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

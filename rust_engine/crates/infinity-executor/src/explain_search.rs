//! Physical `EXPLAIN` for `SEARCH MATCH …` plans (plan §6 Gate A).
//!
//! Reproduces the C++ `ExplainPhysicalPlan` printers for the search operators:
//! `MATCH`, `KNN SCAN`, `MatchTensorScan`, `MatchSparseScan`, `FUSION` and an
//! optional `LIMIT`, under a final `PROJECT`.
//!
//! The conformance harness normalizes whitespace (like `sqllogictest-rs`), so
//! only the token stream — node names, attributes and their order — matters.

use std::collections::HashMap;

use infinity_parser::{ExprAst, Literal, MatchClause, Predicate, SearchClause, SelectItem};

use crate::explain::{add_col, attr, expr_cols, header, render_expr};

/// A `SELECT … SEARCH MATCH …` reduced to what the printer needs.
pub struct SearchPlan<'a> {
    pub items: &'a [SelectItem],
    /// Qualified table name, e.g. `default_db.t`.
    pub table: &'a str,
    pub search: &'a SearchClause,
    pub limit: Option<u64>,
    pub offset: Option<u64>,
    pub where_pred: Option<&'a Predicate>,
    /// Column names in schema order.
    pub schema: &'a [String],
    /// Declared type text per column (schema order).
    pub types: &'a [String],
    /// Columns that have any index (used to split index/leftover filters).
    pub indexed: &'a [String],
    /// The table was written in more than one batch, so the scan is planned as
    /// several segments behind a `MERGE` operator.
    pub multi: bool,
}

/// C++ `Embedding2String` / `%g`-style float rendering: shortest round-trip.
fn norm_num(s: &str) -> String {
    match s.trim().parse::<f64>() {
        Ok(f) => format!("{f}"),
        Err(_) => s.to_string(),
    }
}

/// Renders an array literal as `[a,b,c]` (no spaces).
fn vec_text(items: &[String]) -> String {
    let parts: Vec<String> = items.iter().map(|s| norm_num(s)).collect();
    format!("[{}]", parts.join(","))
}

/// Renders a sparse vector literal as `[i:v,i:v,…]` with 6-decimal values.
fn sparse_vec_text(items: &[String]) -> String {
    let parts: Vec<String> = items
        .iter()
        .map(|s| match s.split_once(':') {
            Some((i, v)) => {
                let f = v
                    .trim()
                    .parse::<f64>()
                    .map(|f| format!("{f:.6}"))
                    .unwrap_or_else(|_| v.to_string());
                format!("{i}:{f}")
            }
            None => s.clone(),
        })
        .collect();
    format!("[{}]", parts.join(","))
}

/// The lower-case element name of a `SPARSE(<elem>,<dim>)` type (`float`).
fn sparse_elem(type_text: &str) -> String {
    type_text
        .to_ascii_uppercase()
        .strip_prefix("SPARSE(")
        .and_then(|inner| inner.split(',').next())
        .map(|e| e.trim().to_ascii_lowercase())
        .unwrap_or_else(|| "float".to_string())
}

/// Renders a flat tensor literal as `[[row],[row],…]` with the given row width.
fn tensor_text(items: &[String], dim: usize) -> String {
    if dim == 0 || !items.len().is_multiple_of(dim) {
        return vec_text(items);
    }
    let rows: Vec<String> = items.chunks(dim).map(vec_text).collect();
    format!("[{}]", rows.join(","))
}

fn element_type(t: &str) -> &'static str {
    match t.to_ascii_lowercase().as_str() {
        "float" | "float32" | "f32" => "FLOAT32",
        "float16" | "fp16" | "f16" => "FLOAT16",
        "bfloat16" | "bf16" => "BFLOAT16",
        "int8" | "i8" => "INT8",
        "uint8" | "u8" => "UINT8",
        "int16" | "i16" => "INT16",
        "int32" | "i32" => "INT32",
        "int64" | "i64" => "INT64",
        "bit" => "BIT",
        _ => "FLOAT32",
    }
}

fn distance_type(t: &str) -> &'static str {
    match t.to_ascii_lowercase().as_str() {
        "l2" | "l2sq" => "L2",
        "ip" | "inner_product" => "IP",
        "cosine" => "COSINE",
        "hamming" => "HAMMING",
        _ => "L2",
    }
}

/// A predicate rendered with `CAST(col AS BigInt)` operands and a caller-supplied
/// column-id annotation. Used for the `index filter` / `leftover filter` lines.
fn render_pred_ids(p: &Predicate, annotate: &dyn Fn(&str) -> String) -> String {
    let operand = |e: &ExprAst| -> String {
        match e {
            ExprAst::Column(c) => format!("CAST({} AS BigInt)", annotate(c)),
            other => render_expr(other, &HashMap::new()),
        }
    };
    match p {
        Predicate::Cmp(a, op, b) => format!(
            "{} {} {}",
            operand(a),
            crate::explain::cmp_text(*op),
            operand(b)
        ),
        Predicate::Bool(a, op, b) => format!(
            "({}) {} ({})",
            render_pred_ids(a, annotate),
            crate::explain::bool_text(*op),
            render_pred_ids(b, annotate)
        ),
        Predicate::Not(a) => format!("NOT ({})", render_pred_ids(a, annotate)),
        Predicate::IsNull(e, neg) => format!(
            "{} IS {}NULL",
            render_expr(e, &HashMap::new()),
            if *neg { "NOT " } else { "" }
        ),
        Predicate::IsTrue(e) => render_expr(e, &HashMap::new()),
        Predicate::Like(a, b, _, neg) => format!(
            "{} {}LIKE {}",
            render_expr(a, &HashMap::new()),
            if *neg { "NOT " } else { "" },
            render_expr(b, &HashMap::new())
        ),
        Predicate::In(a, list, neg) => {
            let items: Vec<String> = list
                .iter()
                .map(|e| render_expr(e, &HashMap::new()))
                .collect();
            format!(
                "{} {}IN ({})",
                render_expr(a, &HashMap::new()),
                if *neg { "NOT " } else { "" },
                items.join(", ")
            )
        }
    }
}

/// A predicate rendered verbatim (no `CAST`), used inside a `MATCH … WHERE (…)`.
fn render_pred_plain(p: &Predicate) -> String {
    match p {
        Predicate::Cmp(a, op, b) => format!(
            "{} {} {}",
            render_expr(a, &HashMap::new()),
            crate::explain::cmp_text(*op),
            render_expr(b, &HashMap::new())
        ),
        Predicate::Bool(a, op, b) => format!(
            "{} {} {}",
            render_pred_plain(a),
            crate::explain::bool_text(*op),
            render_pred_plain(b)
        ),
        Predicate::Not(a) => format!("NOT {}", render_pred_plain(a)),
        other => render_pred_ids(other, &|c| c.to_string()),
    }
}

fn str_arg(e: &ExprAst) -> Option<String> {
    match e {
        ExprAst::Literal(Literal::Str(s)) => Some(s.clone()),
        _ => None,
    }
}

fn arr_arg(e: &ExprAst) -> Option<Vec<String>> {
    match e {
        ExprAst::Literal(Literal::Array(items)) => Some(items.clone()),
        _ => None,
    }
}

fn top_n_of(opts: &str) -> u64 {
    for part in opts.split([';', ',']) {
        let part = part.trim();
        if let Some(rest) = part.strip_prefix("topn=") {
            if let Ok(n) = rest.trim().parse::<u64>() {
                return n;
            }
        }
    }
    10
}

/// Renders one projection item of a search plan.
fn render_item(
    it: &SelectItem,
    ids: &HashMap<String, usize>,
    score_id: usize,
    rowid_id: usize,
) -> Option<String> {
    match it {
        SelectItem::Column(c) => Some(match ids.get(c) {
            Some(i) => format!("{c} (#{i})"),
            None => c.clone(),
        }),
        SelectItem::Func { name, args } => {
            let up = name.to_ascii_uppercase();
            match up.as_str() {
                "DISTANCE" => Some(format!("DISTANCE (#{score_id})")),
                "SCORE" => Some(format!("SCORE (#{score_id})")),
                "ROW_ID" => Some(format!("ROW_ID (#{rowid_id})")),
                "ROUND" => {
                    let inner = args
                        .first()
                        .map(|a| render_item_expr(a, score_id, rowid_id))
                        .unwrap_or_default();
                    let n = args
                        .get(1)
                        .map(|a| render_expr(a, &HashMap::new()))
                        .unwrap_or_default();
                    Some(format!("{inner} round {n}"))
                }
                _ => {
                    let inner: Vec<String> = args
                        .iter()
                        .map(|a| render_item_expr(a, score_id, rowid_id))
                        .collect();
                    Some(format!("{}({})", up, inner.join(", ")))
                }
            }
        }
        SelectItem::Expr(e) => Some(render_item_expr(e, score_id, rowid_id)),
        _ => None,
    }
}

fn render_item_expr(e: &ExprAst, score_id: usize, rowid_id: usize) -> String {
    match e {
        ExprAst::Func { name, args } => {
            let up = name.to_ascii_uppercase();
            match up.as_str() {
                "DISTANCE" => format!("DISTANCE (#{score_id})"),
                "SCORE" => format!("SCORE (#{score_id})"),
                "ROW_ID" => format!("ROW_ID (#{rowid_id})"),
                _ => {
                    let inner: Vec<String> = args
                        .iter()
                        .map(|a| render_item_expr(a, score_id, rowid_id))
                        .collect();
                    format!("{}({})", up, inner.join(", "))
                }
            }
        }
        other => render_expr(other, &HashMap::new()),
    }
}

/// Produces the physical `EXPLAIN` text for a `SEARCH MATCH` plan. Returns
/// `None` for shapes this printer does not yet model (fusion, merges, …).
pub fn explain_search(p: &SearchPlan) -> Option<Vec<String>> {
    if p.search.fusion.is_some() {
        return explain_fusion(p);
    }
    if p.search.matches.len() != 1 {
        return None;
    }
    let m = &p.search.matches[0];
    let kind = m.kind.to_ascii_uppercase();
    if !matches!(kind.as_str(), "TEXT" | "VECTOR" | "TENSOR" | "SPARSE") {
        return None;
    }
    if p.offset.is_some() {
        return None;
    }

    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");

    // A node-level `WHERE` (inside the match parens) takes precedence over the
    // statement's outer WHERE for the index/leftover filter split.
    let filter_src = m.filter.as_ref().or(p.where_pred);
    let mut filter_cols: Vec<String> = Vec::new();
    if let Some(w) = filter_src {
        pred_cols(w, &mut filter_cols);
    }

    // Leaf output: non-indexed filter columns, then the selected columns, then
    // the synthetic `__score` / `__rowid`.
    let mut regular: Vec<String> = Vec::new();
    for c in &filter_cols {
        if p.indexed.iter().any(|x| x == c) {
            continue;
        }
        add_col(&mut regular, c);
    }
    for it in p.items {
        if let SelectItem::Column(c) = it {
            add_col(&mut regular, c);
        }
    }
    let mut leaf_out = regular.clone();
    leaf_out.push("__score".to_string());
    leaf_out.push("__rowid".to_string());
    let mut ids: HashMap<String, usize> = HashMap::new();
    for (i, c) in regular.iter().enumerate() {
        ids.insert(c.clone(), i);
    }
    let score_id = regular.len();
    let rowid_id = regular.len() + 1;

    let proj: Vec<String> = p
        .items
        .iter()
        .filter_map(|it| render_item(it, &ids, score_id, rowid_id))
        .collect();

    let has_limit = p.limit.is_some();
    let mut lines: Vec<String> = Vec::new();
    lines.push(header(0, "PROJECT", if has_limit { 4 } else { 3 }));
    lines.push(attr(0, "table index: #4"));
    lines.push(attr(0, &format!("expressions: [{}]", proj.join(", "))));

    let leaf_d = if has_limit {
        lines.push(header(1, "LIMIT", 3));
        lines.push(attr(1, &format!("limit: {}", p.limit.unwrap_or(0))));
        lines.push(attr(
            1,
            &format!("output columns: [{}]", leaf_out.join(", ")),
        ));
        2
    } else {
        1
    };

    // index/leftover filter lines (shared by MATCH and MatchTensorScan).
    let idx_annotation = |c: &str| -> String {
        match p.schema.iter().position(|x| x == c) {
            Some(i) => format!("{c} (#1.{i})"),
            None => c.to_string(),
        }
    };
    let leaf_annotation = |c: &str| -> String {
        match ids.get(c) {
            Some(i) => format!("{c} (#{i})"),
            None => c.to_string(),
        }
    };
    let indexed_any = filter_cols.iter().any(|c| p.indexed.iter().any(|x| x == c));
    let index_filter: String = match filter_src {
        Some(w) if indexed_any => render_pred_ids(w, &idx_annotation),
        _ => "None".to_string(),
    };
    let leftover_filter: String = match filter_src {
        Some(w) if !indexed_any => render_pred_ids(w, &leaf_annotation),
        _ => "None".to_string(),
    };

    // A multi-segment table wraps the MatchTensorScan in a `MERGE MatchTensor`.
    if p.multi && kind == "TENSOR" {
        let col = m.column.clone();
        let vals = m.args.first().and_then(arr_arg).unwrap_or_default();
        let opts = m.args.get(3).and_then(str_arg).unwrap_or_default();
        let dim = p
            .schema
            .iter()
            .position(|c| c == &col)
            .and_then(|i| p.types.get(i))
            .and_then(|t| crate::type_dim_deep(t))
            .unwrap_or(vals.len());
        let mut expr = format!("MATCH TENSOR ({col}, {}, MAX_SIM", tensor_text(&vals, dim));
        if let Some(f) = m.filter.as_ref() {
            expr.push_str(&format!(", WHERE ({})", render_pred_plain(f)));
        } else if !opts.is_empty() {
            expr.push_str(&format!(", '{opts}'"));
        }
        expr.push(')');
        lines.push(header(leaf_d, "MERGE MatchTensor", 5));
        lines.push(attr(leaf_d, &format!("table name: {alias}({db}.{alias})")));
        lines.push(attr(leaf_d, "table index: #1"));
        lines.push(attr(leaf_d, &format!("MatchTensor expression: {expr}")));
        lines.push(attr(leaf_d, &format!("Top N: {}", top_n_of(&opts))));
        lines.push(attr(
            leaf_d,
            &format!("output columns: [{}]", leaf_out.join(", ")),
        ));
    }

    // A multi-segment table wraps the MatchSparseScan in a `MERGE MatchSparse`.
    if p.multi && kind == "SPARSE" {
        let col = m.column.clone();
        let vals = m.args.first().and_then(arr_arg).unwrap_or_default();
        let metric = m.args.get(1).and_then(str_arg).unwrap_or_default();
        let topn = match m.args.get(2) {
            Some(ExprAst::Literal(Literal::Int(i))) => *i,
            _ => 3,
        };
        let dim = p
            .schema
            .iter()
            .position(|c| c == &col)
            .and_then(|i| p.types.get(i))
            .and_then(|t| crate::type_dim_deep(t))
            .unwrap_or(0);
        let elem = p
            .schema
            .iter()
            .position(|c| c == &col)
            .and_then(|i| p.types.get(i))
            .map(|t| sparse_elem(t))
            .unwrap_or_else(|| "float".to_string());
        let metric_name = match metric.to_ascii_lowercase().as_str() {
            "ip" | "inner_product" => "INNER_PRODUCT".to_string(),
            other => other.to_ascii_uppercase(),
        };
        let expr = format!(
            "MATCH SPARSE ({col}, [Cast({} AS Sparse({elem},int8,{dim}))], {metric_name}, {topn}) WITH () USING INDEX ()",
            sparse_vec_text(&vals)
        );
        lines.push(header(leaf_d, "MERGE MatchSparse", 5));
        lines.push(attr(leaf_d, &format!("table name: {alias}({db}.{alias})")));
        lines.push(attr(leaf_d, "table index: #1"));
        lines.push(attr(leaf_d, &format!("MatchSparse expression: {expr}")));
        lines.push(attr(leaf_d, &format!("Top N: {topn}")));
        lines.push(attr(
            leaf_d,
            &format!("output columns: [{}]", leaf_out.join(", ")),
        ));
    }

    if !render_leaf(
        &mut lines,
        &kind,
        m,
        leaf_d,
        alias,
        db,
        &leaf_out,
        &index_filter,
        &leftover_filter,
        filter_src.map(|w| {
            if indexed_any {
                render_pred_ids(w, &idx_annotation)
            } else {
                render_pred_ids(w, &leaf_annotation)
            }
        }),
        p.schema,
        p.types,
        2,
    ) {
        return None;
    }

    Some(lines)
}

/// Appends the lines of one search-scan node (`MATCH`/`KNN SCAN`/
/// `MatchTensorScan`/`MatchSparseScan`). Returns `false` for an unknown kind.
#[allow(clippy::too_many_arguments)]
fn render_leaf(
    lines: &mut Vec<String>,
    kind: &str,
    m: &MatchClause,
    d: usize,
    alias: &str,
    db: &str,
    leaf_out: &[String],
    index_filter: &str,
    leftover_filter: &str,
    knn_filter: Option<String>,
    schema: &[String],
    types: &[String],
    id: u64,
) -> bool {
    match kind {
        "VECTOR" => {
            let col = m.column.clone();
            let vals = m.args.first().and_then(arr_arg).unwrap_or_default();
            let etype = m.args.get(1).and_then(str_arg).unwrap_or_default();
            let dist = m.args.get(2).and_then(str_arg).unwrap_or_default();
            lines.push(header(d, "KNN SCAN", id));
            lines.push(attr(d, &format!("table name: {alias}({db}.{alias})")));
            lines.push(attr(d, "table index: #4"));
            lines.push(attr(d, &format!("embedding info: {col}")));
            lines.push(attr(d, &format!("element type: {}", element_type(&etype))));
            lines.push(attr(d, &format!("dimension: {}", vals.len())));
            lines.push(attr(d, &format!("distance type: {}", distance_type(&dist))));
            lines.push(attr(d, &format!("query embedding: {}", vec_text(&vals))));
            if let Some(f) = knn_filter {
                lines.push(attr(d, &format!("filter: {f}")));
            }
            lines.push(attr(
                d,
                &format!("output columns: [{}]", leaf_out.join(", ")),
            ));
        }
        "TENSOR" => {
            let col = m.column.clone();
            let vals = m.args.first().and_then(arr_arg).unwrap_or_default();
            let opts = m.args.get(3).and_then(str_arg).unwrap_or_default();
            let dim = schema
                .iter()
                .position(|c| c == &col)
                .and_then(|i| types.get(i))
                .and_then(|t| crate::type_dim_deep(t))
                .unwrap_or(vals.len());
            let mut expr = format!("MATCH TENSOR ({col}, {}, MAX_SIM", tensor_text(&vals, dim));
            if let Some(f) = m.filter.as_ref() {
                expr.push_str(&format!(", WHERE ({})", render_pred_plain(f)));
            } else if !opts.is_empty() {
                expr.push_str(&format!(", '{opts}'"));
            }
            expr.push(')');
            lines.push(header(d, "MatchTensorScan", id));
            lines.push(attr(d, &format!("table name: {alias}({db}.{alias})")));
            lines.push(attr(d, "table index: #1"));
            lines.push(attr(d, &format!("MatchTensor expression: {expr}")));
            lines.push(attr(d, &format!("Top N: {}", top_n_of(&opts))));
            lines.push(attr(d, &format!("index filter: {index_filter}")));
            lines.push(attr(d, &format!("leftover filter: {leftover_filter}")));
            lines.push(attr(
                d,
                &format!("output columns: [{}]", leaf_out.join(", ")),
            ));
        }
        "SPARSE" => {
            lines.push(header(d, "MatchSparseScan", id));
            lines.push(attr(d, "table index: #1"));
            lines.push(attr(
                d,
                &format!("output columns: [{}]", leaf_out.join(", ")),
            ));
        }
        "TEXT" => {
            let field = m.column.clone();
            let query = m.args.first().and_then(str_arg).unwrap_or_default();
            let opts = m.args.get(1).and_then(str_arg).unwrap_or_default();
            lines.push(header(d, "MATCH", id));
            lines.push(attr(d, &format!("table name: {alias}({db}.{alias})")));
            lines.push(attr(d, "table index: #1"));
            lines.push(attr(
                d,
                &format!("match expression: MATCH TEXT ('{field}', '{query}', '{opts}')"),
            ));
            lines.push(attr(d, &format!("index filter: {index_filter}")));
            lines.push(attr(d, &format!("leftover filter: {leftover_filter}")));
            lines.push(attr(
                d,
                &format!("output columns: [{}]", leaf_out.join(", ")),
            ));
        }
        _ => return false,
    }
    true
}

/// The physical `EXPLAIN` text for a fused (`…, FUSION('…')`) search plan:
/// `PROJECT -> FUSION -> <child>, …`. The children output only the non-indexed
/// filter columns plus `__score`/`__rowid`; the projection re-derives the
/// selected columns.
fn explain_fusion(p: &SearchPlan) -> Option<Vec<String>> {
    if p.offset.is_some() || p.limit.is_some() {
        return None;
    }
    let matches = &p.search.matches;
    if matches.is_empty() {
        return None;
    }
    let alias = p.table.split('.').next_back().unwrap_or(p.table);
    let db = p
        .table
        .split_once('.')
        .map(|(d, _)| d)
        .unwrap_or("default_db");

    let mut filter_cols: Vec<String> = Vec::new();
    if let Some(w) = p.where_pred {
        pred_cols(w, &mut filter_cols);
    }
    let mut regular: Vec<String> = Vec::new();
    for c in &filter_cols {
        if p.indexed.iter().any(|x| x == c) {
            continue;
        }
        add_col(&mut regular, c);
    }
    for it in p.items {
        if let SelectItem::Column(c) = it {
            add_col(&mut regular, c);
        }
    }
    let mut child_out: Vec<String> = filter_cols
        .iter()
        .filter(|c| !p.indexed.iter().any(|x| x.as_str() == c.as_str()))
        .cloned()
        .collect();
    add_col(&mut child_out, "__score");
    add_col(&mut child_out, "__rowid");

    let mut ids: HashMap<String, usize> = HashMap::new();
    for (i, c) in regular.iter().enumerate() {
        ids.insert(c.clone(), i);
    }
    let score_id = regular.len();
    let rowid_id = regular.len() + 1;
    let proj: Vec<String> = p
        .items
        .iter()
        .filter_map(|it| render_item(it, &ids, score_id, rowid_id))
        .collect();

    let idx_annotation = |c: &str| -> String {
        match p.schema.iter().position(|x| x == c) {
            Some(i) => format!("{c} (#1.{i})"),
            None => c.to_string(),
        }
    };
    let leaf_annotation = |c: &str| -> String {
        match ids.get(c) {
            Some(i) => format!("{c} (#{i})"),
            None => c.to_string(),
        }
    };
    let indexed_any = filter_cols.iter().any(|c| p.indexed.iter().any(|x| x == c));
    let index_filter = match p.where_pred {
        Some(w) if indexed_any => render_pred_ids(w, &idx_annotation),
        _ => "None".to_string(),
    };
    let leftover_filter = match p.where_pred {
        Some(w) if !indexed_any => render_pred_ids(w, &leaf_annotation),
        _ => "None".to_string(),
    };
    let knn_filter = p.where_pred.map(|w| {
        if indexed_any {
            render_pred_ids(w, &idx_annotation)
        } else {
            render_pred_ids(w, &leaf_annotation)
        }
    });

    let fusion_name = p.search.fusion.clone().unwrap_or_default();
    let fusion_params = p.search.fusion_params.clone().unwrap_or_default();

    let mut lines = Vec::new();
    lines.push(header(0, "PROJECT", 5));
    lines.push(attr(0, "table index: #4"));
    lines.push(attr(0, &format!("expressions: [{}]", proj.join(", "))));
    lines.push(header(1, "FUSION", 4));
    lines.push(attr(
        1,
        &format!("fusion: #FUSION('{fusion_name}', '{fusion_params}')"),
    ));
    lines.push(attr(
        1,
        &format!("output columns: [{}]", child_out.join(", ")),
    ));
    for (i, m) in matches.iter().enumerate() {
        let kind = m.kind.to_ascii_uppercase();
        if !render_leaf(
            &mut lines,
            &kind,
            m,
            1,
            alias,
            db,
            &child_out,
            &index_filter,
            &leftover_filter,
            knn_filter.clone(),
            p.schema,
            p.types,
            2 + i as u64,
        ) {
            return None;
        }
    }
    Some(lines)
}

/// Collects the column names referenced by a predicate (first-appearance order).
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

use super::*;

#[cfg(test)]
thread_local! {
    /// Rows whose text a `MATCH` path tokenised, so a test can prove the
    /// column form reads the inverted index instead of the rows.
    pub static MATCH_ROWS_TOKENIZED: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
}

tokio::task_local! {
    /// The timestamp the current statement reads at (`NewTxn::BeginTS`,
    /// `current_ts + 1` at its start). Reads under it hide rows created after
    /// that point and keep rows deleted after it; everything else - the bottom
    /// halves, the background tasks, a caller that reads directly - runs with
    /// `u64::MAX` and sees the latest state.
    ///
    /// A **task** local, not a thread local. A statement awaits (WAL, allocator,
    /// bottom, CPU decode), so it is resumed on whichever runtime worker is
    /// free; a thread local set on the thread that started it is not the value
    /// the rest of the statement reads, and the guard's `Drop` on a *different*
    /// thread restores a stale value into that thread's slot. That leak is
    /// visible: a later statement on the same worker inherited an old
    /// `read_ts`, `append_view::covers` compared a fresh commit ts against it,
    /// and every row it had just inserted was filtered out of the scan - the
    /// table read as empty right after a small insert.
    pub static READ_TS: u64;
}

thread_local! {
    /// Set while a table is created from the SDK's physical type, where a
    /// sparse dimension of 0 is legal.
    pub(crate) static ALLOW_ZERO_SPARSE: std::cell::Cell<bool> = const { std::cell::Cell::new(false) };
}

thread_local! {
    /// (blocks loaded, postings decoded) for the current thread: the WAND prints
    /// them under INFINITY_WAND_STATS to show what a query actually pays for.
    pub(crate) static DECODE_STATS: std::cell::Cell<(u64, u64)> = const { std::cell::Cell::new((0, 0)) };
}

/// Result of executing one statement.
#[derive(Debug, Clone, PartialEq)]
pub enum QueryResult {
    CreatedTable {
        id: u64,
        name: String,
    },
    CreatedDatabase {
        name: String,
    },
    DroppedDatabase {
        name: String,
    },
    CreatedIndex {
        name: String,
        table: String,
        column: String,
    },
    DroppedTable {
        name: String,
        existed: bool,
    },
    DroppedIndex {
        name: String,
        existed: bool,
    },
    Inserted {
        count: usize,
    },
    Deleted {
        count: usize,
    },
    Updated {
        count: usize,
    },
    /// A result set. The rows stay as the engine's `Value`s: the text a client
    /// sees is rendered at the edge (the CLI, the `.slt` runner, the thrift /
    /// HTTP / postgres writers), so a scan no longer renders every cell to a
    /// `String` only for the protocol layer to parse it back into the column
    /// chunk it sends.
    Rows {
        columns: Vec<String>,
        rows: Vec<Vec<Value>>,
    },
}

/// Rows that are already display text, as `Value::Str` cells (which
/// `rows_text()` renders back unchanged).
pub(crate) fn text_rows(rows: Vec<Vec<String>>) -> Vec<Vec<Value>> {
    rows.into_iter()
        .map(|r| r.into_iter().map(Value::Str).collect())
        .collect()
}

/// The key that names a row delete in the transaction's command stream.
///
/// Nothing is ever written under it - the tombstone lives in the block's
/// `version` file - but the WAL builder reads the `del:` form as the signal to
/// emit the reference's `WalCmdDeleteV2` for that `(table, row id)`, and the
/// conflict check keys row deletes the same way.
pub(crate) fn wal_delete_key(table: &str, seq: usize) -> Vec<u8> {
    format!("del:{table}:{seq:016}").into_bytes()
}

/// The block mirror's append log: the cumulative row count after every append
/// transaction (`u64` each). The reference's `version` gets one `create` per
/// append (`BlockVersion::Append`), so the mirror needs the same boundaries.
pub(crate) fn appends_key(table: &str) -> Vec<u8> {
    format!("appends:{table}").into_bytes()
}

/// The commit timestamp of every append, one per entry of [`appends_key`].
///
/// A block's `version` file carries one `create` per append **stamped with
/// that append's commit timestamp**, so the mirror has to remember it; the
/// row-id log alone only knows the boundaries.
pub(crate) fn append_ts_key(table: &str) -> Vec<u8> {
    format!("append_ts:{table}").into_bytes()
}

/// Commit timestamp of the last `DumpIndex` that sealed this index, written
/// into the `idx_chunk|…|{chunk_id}` catalog value.
///
/// The reference stamps that value with the commit ts of the transaction whose
/// memory dump produced the chunk (`SegmentIndexMeta::AddChunkIndexID1`). The
/// port's chunks are re-published by the checkpoint mirror, so the transaction
/// that last sealed the index is the closest real ts it can name.
pub(crate) fn dump_ts_key(table: &str, index: &str) -> Vec<u8> {
    format!("dump_ts:{table}:{index}").into_bytes()
}

/// One byte per append, aligned with [`appends_key`]: `1` when the append came
/// from `COPY`/`IMPORT`, `0` when it came from `INSERT`/`UPDATE`.
///
/// The reference's null bitmap covers different bit ranges depending on that
/// provenance (see `ColumnBody::build_with`), so the block mirror needs it.
pub(crate) fn append_copy_key(table: &str) -> Vec<u8> {
    format!("append_copy:{table}").into_bytes()
}

/// The append/delete journals a table's block files record, rebuilt on first
/// use by [`Engine::ensure_journals`].
pub(crate) struct Journals {
    pub(crate) appends: Vec<u64>,
    pub(crate) append_ts: Vec<u64>,
    pub(crate) append_copy: Vec<u64>,
    pub(crate) deletes: Vec<u64>,
    pub(crate) delete_ts: Vec<u64>,
}

/// `rank_features` contribution from a row's JSON cell - the pre-index path,
/// kept only for the unit test that pins the JSON shape. The query path reads
/// the same weight back from the index's doc-payload column instead
/// (`RankFeatureDocIterator` equivalent).
#[cfg(test)]
pub(crate) fn rank_features_bonus(cell: &str, tags: &[(String, f64)]) -> f64 {
    let mut sum = 0.0;
    for (tag, weight) in tags {
        let key = format!("\"{tag}\"");
        let Some(pos) = cell.find(&key) else {
            continue;
        };
        let rest = cell[pos + key.len()..].trim_start();
        let Some(rest) = rest.strip_prefix(':') else {
            continue;
        };
        let num: String = rest
            .trim_start()
            .chars()
            .take_while(|c| c.is_ascii_digit() || "+-.eE".contains(*c))
            .collect();
        if let Ok(v) = num.parse::<f64>() {
            sum += v * weight;
        }
    }
    sum
}

/// Overwrites a search-result row's hidden `#_score` column (the last one).
pub(crate) fn set_score(row: &mut [Value], score: f64) {
    if let Some(last) = row.last_mut() {
        *last = Value::Float(score);
    }
}

/// Reads a search-result row's hidden `#_score` column (the last one).
pub(crate) fn row_score(row: &[Value]) -> f64 {
    match row.last() {
        Some(Value::Float(f)) => *f,
        Some(Value::Int(i)) => *i as f64,
        _ => 0.0,
    }
}

/// Whether a fusion child's scores are similarities (higher is better, so no
/// sign flip) rather than distances. Mirrors `PhysicalFusion`'s `min_heaps`:
/// `MATCH`/`MATCH TENSOR`/`MATCH SPARSE` are similarities, and an L2/Hamming
/// vector scan is a distance (`KnnExpression::IsKnnMinHeap`).
pub(crate) fn match_is_min_heap(m: &MatchClause) -> bool {
    if m.kind.eq_ignore_ascii_case("VECTOR") {
        let metric = arg_str(m.args.get(2)).unwrap_or_default();
        !matches!(
            metric.trim().to_ascii_lowercase().as_str(),
            "l2" | "hamming"
        )
    } else {
        true
    }
}

/// Renders an `f64` so that parsing the text back yields the same bits.
///
/// Rust's `Display` for floats is the shortest representation that round-trips,
/// which is what a stored row needs; six decimals (the `.slt` display form) is
/// not, and the reference stores the raw typed value.
///
/// A value that prints without a fraction (`1`, `-0`) gets one: the stored row
/// is parsed back by a numeric literal reader that tries integers first, which
/// would turn `-0` into an integer zero and lose the sign.
pub(crate) fn exact_float(f: f64) -> String {
    let text = format!("{f}");
    if f.is_finite() && !text.contains(['.', 'e', 'E']) {
        return format!("{text}.0");
    }
    text
}

/// Renders a numeric token with six decimals, falling back to the raw token.
pub(crate) fn fmt_float6(token: &str) -> String {
    match token.trim().parse::<f64>() {
        Ok(f) => format!("{f:.6}"),
        Err(_) => token.to_string(),
    }
}

/// Bracket depth at which each numeric leaf of a literal appears.
pub(crate) fn array_leaf_depths(text: &str) -> Vec<usize> {
    let mut depths = Vec::new();
    let mut depth = 0usize;
    let mut chars = text.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '[' => depth += 1,
            ']' => depth = depth.saturating_sub(1),
            c if c.is_ascii_digit()
                || ((c == '-' || c == '.')
                    && matches!(chars.peek(), Some(x) if x.is_ascii_digit())) =>
            {
                while matches!(chars.peek(), Some(x) if x.is_ascii_digit() || *x == '.') {
                    chars.next();
                }
                depths.push(depth);
            }
            _ => {}
        }
    }
    depths
}

/// The elements of a brace array literal `{a,b,c}` (as `.slt` text), in order,
/// or `None` when the value is not brace-delimited.
pub(crate) fn brace_array_elems(cell: &str) -> Option<Vec<String>> {
    let t = cell.trim();
    let inner = t.strip_prefix('{')?.strip_suffix('}')?;
    if inner.trim().is_empty() {
        return Some(Vec::new());
    }
    Some(
        split_top_level(inner)
            .iter()
            .map(|s| s.trim().to_string())
            .collect(),
    )
}

/// The innermost numeric groups of an array literal, in order, each as its list
/// of element tokens. A flat literal yields a single group.
pub(crate) fn innermost_groups(text: &str) -> Vec<Vec<String>> {
    let t = text.trim();
    let inner = t.strip_prefix('[').unwrap_or(t);
    let inner = inner.strip_suffix(']').unwrap_or(inner);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        split_top_level(inner)
            .iter()
            .flat_map(|g| innermost_groups(g))
            .collect()
    } else {
        let items: Vec<String> = inner
            .split(',')
            .map(|s| s.trim().to_string())
            .filter(|s| !s.is_empty())
            .collect();
        vec![items]
    }
}

/// Validates a tensor-like literal against its declared dimension: every
/// embedding must have exactly `dim` elements, and a flat literal must be a
/// whole multiple of `dim`. Non tensor-like columns always pass.
pub(crate) fn validate_tensor_dimension(type_text: &str, lit: &str) -> Result<()> {
    let up = type_text.to_ascii_uppercase();
    let embedding = up.starts_with("EMBEDDING") || up.starts_with("VECTOR");
    if !is_tensor_type(&up) && !embedding {
        return Ok(());
    }
    // Only array literals carry a dimension; NULL/default padding is left alone.
    if !lit.trim_start().starts_with('[') {
        return Ok(());
    }
    if up.starts_with("TENSORARRAY") {
        let depths = array_leaf_depths(lit);
        if !depths.is_empty() && depths.iter().any(|d| *d != 3) {
            return Err(Error::Invalid(
                "TENSORARRAY value must be nested as [[[…]]]".into(),
            ));
        }
    }
    let dim = match type_dim(type_text) {
        Some(d) if d > 0 => d,
        _ => return Ok(()),
    };
    let groups = innermost_groups(lit);
    if groups.len() == 1 {
        let n = groups[0].len();
        // An embedding is exactly `dim` wide; a tensor may hold k*dim elements.
        if n != 0 && embedding && n != dim {
            return Err(Error::Invalid(format!(
                "embedding dimension mismatch: expected {dim}, got {n}"
            )));
        }
        if n != 0 && !embedding && !n.is_multiple_of(dim) {
            return Err(Error::Invalid(format!(
                "tensor dimension mismatch: expected a multiple of {dim}, got {n}"
            )));
        }
    } else {
        for g in &groups {
            if g.len() != dim {
                return Err(Error::Invalid(format!(
                    "tensor dimension mismatch: expected {dim}, got {}",
                    g.len()
                )));
            }
        }
    }
    Ok(())
}

/// Truncates every numeric token in `s` to an integer.
pub(crate) fn truncate_numbers(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let mut num = String::new();
    for c in s.chars() {
        if c.is_ascii_digit() || c == '.' || c == '-' {
            num.push(c);
            continue;
        }
        if !num.is_empty() {
            match num.parse::<f64>() {
                Ok(f) => out.push_str(&(f.round() as i64).to_string()),
                Err(_) => out.push_str(&num),
            }
            num.clear();
        }
        out.push(c);
    }
    if !num.is_empty() {
        match num.parse::<f64>() {
            Ok(f) => out.push_str(&(f.round() as i64).to_string()),
            Err(_) => out.push_str(&num),
        }
    }
    out
}

pub(crate) fn brace_render(s: &str) -> String {
    let t = s.trim();
    if !t.starts_with('[') || !t.ends_with(']') {
        return t.to_string();
    }
    t.chars()
        .map(|c| match c {
            '[' => '{',
            ']' => '}',
            other => other,
        })
        .collect()
}

/// Renders a `Value` as a JSON field value (strings quoted, arrays/objects raw).
/// Renders a cell as JSON. A sparse column (`[i:v,…]`) becomes a JSON object so
/// the line stays valid JSON and round-trips through `COPY … FROM … JSONL`.
pub(crate) fn json_value_typed(v: &Value, type_text: &str) -> String {
    if is_sparse_type(type_text) {
        if let Value::Str(s) = v {
            let t = s.trim();
            if t.starts_with('[') {
                let pairs: Vec<String> = t
                    .trim_start_matches('[')
                    .trim_end_matches(']')
                    .split(',')
                    .filter(|x| !x.trim().is_empty())
                    .map(|kv| match kv.split_once(':') {
                        Some((i, val)) => format!("\"{}\" : {}", i.trim(), val.trim()),
                        None => format!("\"{}\" : 1", kv.trim()),
                    })
                    .collect();
                return format!("{{{}}}", pairs.join(","));
            }
        }
    }
    match v {
        Value::Str(s) => {
            let t = s.trim();
            if t.starts_with('[') || t.starts_with('{') {
                t.to_string()
            } else {
                let escaped = s.replace('\\', "\\\\").replace('"', "\\\"");
                format!("\"{escaped}\"")
            }
        }
        Value::Null => "null".to_string(),
        other => other.to_slt_string(),
    }
}

/// Maps an export path to a writable location that a later `COPY … FROM`
/// resolves: paths under the fixture root are redirected beneath
/// `INFINITY_DATA_DIR`, with parent directories created.
pub(crate) fn resolve_write_path(path: &str) -> std::path::PathBuf {
    let p = std::path::PathBuf::from(path);
    if let Ok(base) = std::env::var("INFINITY_DATA_DIR") {
        if let Some(rest) = path.strip_prefix("/var/infinity/test_data/") {
            let joined = std::path::PathBuf::from(&base).join(rest);
            if let Some(parent) = joined.parent() {
                let _ = std::fs::create_dir_all(parent);
            }
            return joined;
        }
    }
    if let Some(parent) = p.parent() {
        let _ = std::fs::create_dir_all(parent);
    }
    p
}

/// Renders a JSON scalar (or nested value) as a stored cell string.
pub(crate) fn json_scalar_text(v: &expression::json::Json) -> String {
    use expression::json::Json;
    match v {
        Json::Str(s) => s.clone(),
        Json::Int(i) => i.to_string(),
        Json::Float(f) => fmt_num(*f),
        Json::Bool(b) => b.to_string(),
        other => other.compact(),
    }
}

/// Recognises a bare `filter_text(…)`/`filter_fulltext(…)` predicate that needs
/// the full-text index to answer, returning what it asked for.
pub(crate) fn fulltext_index_call(p: &Predicate) -> Option<(String, String, String)> {
    let Predicate::IsTrue(ExprAst::Func { name, args }) = p else {
        return None;
    };
    let up = name.to_ascii_uppercase();
    if up != "FILTER_TEXT" && up != "FILTER_FULLTEXT" {
        return None;
    }
    let s = |i: usize| match args.get(i) {
        Some(ExprAst::Literal(Literal::Str(v))) => Some(v.clone()),
        _ => None,
    };
    let column = s(0)?;
    let (col, hint) = match column.split_once('@') {
        Some((c, i)) => (c.to_string(), Some(i.to_string())),
        None => (column, None),
    };
    Some((
        col,
        s(1)?,
        format!("{}\u{1}{}", s(2)?, hint.unwrap_or_default()),
    ))
}

pub(crate) fn literal_to_block_value(
    lit: &Literal,
    lt: &::common::LogicalType,
) -> Option<storage::data_block::BlockValue> {
    use ::common::LogicalType as LT;
    use storage::data_block::BlockValue;
    Some(match (lt, lit) {
        (_, Literal::Null) => BlockValue::Null,
        (LT::Boolean, Literal::Int(i)) => BlockValue::Bool(*i != 0),
        (LT::Boolean, Literal::Str(s)) => BlockValue::Bool(match s.trim() {
            "true" | "1" => true,
            "false" | "0" => false,
            _ => return None,
        }),
        (LT::TinyInt, Literal::Int(i)) => BlockValue::TinyInt(*i as i8),
        (LT::SmallInt, Literal::Int(i)) => BlockValue::SmallInt(*i as i16),
        (LT::Integer, Literal::Int(i)) => BlockValue::Int32(*i as i32),
        // An integer column truncates a fractional value, as the reference's
        // implicit cast does (`-6.3` -> `-6`).
        (LT::TinyInt, Literal::Float(f)) => BlockValue::TinyInt(*f as i8),
        (LT::SmallInt, Literal::Float(f)) => BlockValue::SmallInt(*f as i16),
        (LT::Integer, Literal::Float(f)) => BlockValue::Int32(*f as i32),
        (LT::BigInt, Literal::Int(i)) => BlockValue::Int(*i),
        (LT::BigInt, Literal::Float(f)) => BlockValue::Int(*f as i64),
        (LT::Float, Literal::Float(f)) => BlockValue::Float(*f as f32),
        (LT::Float, Literal::Int(i)) => BlockValue::Float(*i as f32),
        (LT::Double, Literal::Float(f)) => BlockValue::Double(*f),
        (LT::Double, Literal::Int(i)) => BlockValue::Double(*i as f64),
        (LT::Varchar, Literal::Str(s)) => BlockValue::Str(s.clone()),
        (LT::Date, Literal::Str(s)) => BlockValue::parse_text(s, LT::Date)?,
        (LT::Time, Literal::Str(s)) => BlockValue::parse_text(s, LT::Time)?,
        (LT::DateTime, Literal::Str(s)) => BlockValue::parse_text(s, LT::DateTime)?,
        (LT::Timestamp, Literal::Str(s)) => BlockValue::parse_text(s, LT::Timestamp)?,
        (LT::Json, Literal::Str(s)) => BlockValue::parse_text(s, LT::Json)?,
        (LT::Embedding(_), Literal::Vector(v)) => {
            BlockValue::Embedding(v.iter().map(|x| *x as f32).collect())
        }
        (LT::Embedding(_), Literal::Array(items)) => BlockValue::Embedding(
            items
                .iter()
                .map(|x| x.trim().parse::<f32>().ok())
                .collect::<Option<Vec<_>>>()?,
        ),
        _ => return None,
    })
}

/// Builds the reference's `DataBlock` columns from the insert's literals.
///
/// `None` means a column's value or type is outside what this path can
/// represent exactly (a `DECIMAL`'s scale, a tensor, a sparse vector, …); the
/// caller then keeps the text-column block, which replays but does not match
/// the reference's bytes.
pub(crate) fn typed_append_block(
    rows: &[Vec<Literal>],
    types: &[String],
) -> Option<Vec<storage::wal::block::BlockColumn>> {
    use storage::data_block::{Column, DataBlock};
    use storage::wal::block::{self, BlockColumn};
    use storage::wal::ddl_def::LogicalType as RefType;

    let declared: Vec<_> = types
        .iter()
        .map(|t| crate::wal_bridge::data_type_of_text(t))
        .collect::<Option<Vec<_>>>()?;
    let direct = |i: usize| {
        matches!(
            declared[i].logical_type,
            RefType::Tensor | RefType::TensorArray | RefType::MultiVector | RefType::Sparse
        )
    };
    if declared.len() != types.len() {
        return None;
    }
    for row in rows {
        if row.len() != declared.len() {
            return None;
        }
    }

    // The port's `DataBlock` has no tensor or sparse column, so those are built
    // straight into the reference form; everything else goes through it.
    let port_indexes: Vec<usize> = (0..types.len()).filter(|i| !direct(*i)).collect();
    let mut adapted: Vec<Option<BlockColumn>> = vec![None; types.len()];
    if !port_indexes.is_empty() {
        let mut columns = Vec::with_capacity(port_indexes.len());
        let mut logical_types = Vec::with_capacity(port_indexes.len());
        let mut port_types = Vec::with_capacity(port_indexes.len());
        for &i in &port_indexes {
            let lt = port_logical_type(&types[i])?;
            columns.push(Column::new(lt)?);
            logical_types.push(lt);
            port_types.push(declared[i].clone());
        }
        let mut block = DataBlock::new(columns, rows.len());
        for row in rows {
            for (slot, &i) in port_indexes.iter().enumerate() {
                block
                    .append_value(slot, literal_to_block_value(&row[i], &logical_types[slot])?)
                    .ok()?;
            }
        }
        block.finalize();
        for (slot, column) in block::from_data_block_with(&block, Some(&port_types))?
            .into_iter()
            .enumerate()
        {
            adapted[port_indexes[slot]] = Some(column);
        }
    }
    for i in 0..types.len() {
        if !direct(i) {
            continue;
        }
        let cells: Vec<&Literal> = rows.iter().map(|row| &row[i]).collect();
        adapted[i] = Some(match declared[i].logical_type {
            RefType::Sparse => sparse_column(&cells, &declared[i])?,
            _ => tensor_column(&cells, &declared[i])?,
        });
    }
    adapted.into_iter().collect()
}

/// `TENSOR` / `TENSORARRAY` / `MULTIVECTOR`: the value's numbers are laid out
/// row-major in the heap and the slot counts how many `dim`-sized vectors it
/// holds (`TensorType{embedding_num, file_offset}`).
pub(crate) fn tensor_column(
    cells: &[&Literal],
    data_type: &storage::wal::ddl_def::DataType,
) -> Option<storage::wal::block::BlockColumn> {
    use storage::wal::block::{BlockColumn, ColumnValues};
    let dim = match &data_type.type_info {
        storage::wal::ddl_def::TypeInfo::Embedding { dimension, .. } => {
            usize::try_from(*dimension).ok()?
        }
        _ => return None,
    };
    if dim == 0 {
        return None;
    }
    let mut values = Vec::with_capacity(cells.len());
    let mut nulls = Vec::new();
    for (row, cell) in cells.iter().enumerate() {
        if matches!(cell, Literal::Null) {
            nulls.push(row as u32);
            values.push(None);
            continue;
        }
        let numbers = literal_numbers(cell)?;
        if numbers.len() % dim != 0 {
            return None;
        }
        let raw: Vec<u8> = numbers
            .iter()
            .flat_map(|x| (*x as f32).to_le_bytes())
            .collect();
        values.push(Some((u16::try_from(numbers.len() / dim).ok()?, raw)));
    }
    let mut column = BlockColumn::new(data_type.clone(), ColumnValues::tensors(&values));
    column.nulls = nulls;
    Some(column)
}

/// The numbers of a tensor literal, in declaration order.
pub(crate) fn literal_numbers(lit: &Literal) -> Option<Vec<f64>> {
    match lit {
        Literal::Vector(v) => Some(v.clone()),
        Literal::Int(i) => Some(vec![*i as f64]),
        Literal::Float(f) => Some(vec![*f]),
        Literal::Str(s) => parse_number_list(s),
        Literal::Array(items) => {
            let mut out = Vec::new();
            for item in items {
                out.extend(parse_number_list(item)?);
            }
            Some(out)
        }
        Literal::Null => None,
    }
}

/// `"[1,2,3]"` (any nesting) -> its numbers in order.
pub(crate) fn parse_number_list(text: &str) -> Option<Vec<f64>> {
    let mut out = Vec::new();
    for token in text.split(['[', ']', ',']) {
        let token = token.trim();
        if token.is_empty() {
            continue;
        }
        out.push(token.parse::<f64>().ok()?);
    }
    Some(out)
}

/// `"[1:0.5,3:0.25]"` -> `(index, value)` pairs.
pub(crate) fn literal_sparse_pairs(lit: &Literal) -> Option<Vec<(i64, f64)>> {
    let items: Vec<String> = match lit {
        Literal::Array(items) => items.clone(),
        Literal::Str(s) => s
            .trim()
            .trim_start_matches('[')
            .trim_end_matches(']')
            .split(',')
            .map(|s| s.to_string())
            .collect(),
        _ => return None,
    };
    let mut out = Vec::with_capacity(items.len());
    for item in items {
        let (index, value) = item.trim().split_once(':')?;
        out.push((index.trim().parse().ok()?, value.trim().parse().ok()?));
    }
    Some(out)
}

pub(crate) fn pack_integer(
    out: &mut Vec<u8>,
    value: i64,
    kind: storage::wal::ddl_def::EmbeddingDataType,
) -> Option<()> {
    use storage::wal::ddl_def::EmbeddingDataType as E;
    match kind {
        E::Int8 | E::UInt8 => out.push(value as u8),
        E::Int16 => out.extend_from_slice(&(value as i16).to_le_bytes()),
        E::Int32 => out.extend_from_slice(&(value as i32).to_le_bytes()),
        E::Int64 => out.extend_from_slice(&value.to_le_bytes()),
        _ => return None,
    }
    Some(())
}

pub(crate) fn pack_float(
    out: &mut Vec<u8>,
    value: f64,
    kind: storage::wal::ddl_def::EmbeddingDataType,
) -> Option<()> {
    use storage::wal::ddl_def::EmbeddingDataType as E;
    match kind {
        E::Float => out.extend_from_slice(&(value as f32).to_le_bytes()),
        E::Double => out.extend_from_slice(&value.to_le_bytes()),
        E::Float16 => {
            out.extend_from_slice(&storage::format::f32_to_f16_bits(value as f32).to_le_bytes())
        }
        E::BFloat16 => {
            out.extend_from_slice(&(((value as f32).to_bits() >> 16) as u16).to_le_bytes())
        }
        _ => return None,
    }
    Some(())
}

/// Splits a table reference into the `(db_name, table_name)` pair the typed WAL
/// commands carry. A bare name belongs to `default_db`.
pub(crate) fn split_table_name(table: &str) -> (String, String) {
    match table.rsplit_once('.') {
        Some((db, name)) => (db.to_string(), name.to_string()),
        None => ("default_db".to_string(), table.to_string()),
    }
}

/// Rewrites a predicate's column positions through `pos` (scan index -> row
/// index), so a filter can run on the rows as they are laid out.
///
/// The scan hands its predicate down to the reader, which builds full-width
/// rows: without this the predicate would read the wrong columns whenever the
/// lowerer pruned some (the common case for a narrow projection).
pub(crate) fn remap_columns(expr: &Expr, pos: &[usize]) -> Expr {
    fn map(i: usize, pos: &[usize]) -> usize {
        pos.get(i).copied().unwrap_or(usize::MAX)
    }
    match expr {
        Expr::Column(i) => Expr::Column(map(*i, pos)),
        Expr::Literal(v) => Expr::Literal(v.clone()),
        Expr::Cmp(a, op, b) => Expr::Cmp(
            Box::new(remap_columns(a, pos)),
            *op,
            Box::new(remap_columns(b, pos)),
        ),
        Expr::And(a, b) => Expr::And(
            Box::new(remap_columns(a, pos)),
            Box::new(remap_columns(b, pos)),
        ),
        Expr::Or(a, b) => Expr::Or(
            Box::new(remap_columns(a, pos)),
            Box::new(remap_columns(b, pos)),
        ),
        Expr::Not(a) => Expr::Not(Box::new(remap_columns(a, pos))),
        Expr::IsNull(a, negated) => Expr::IsNull(Box::new(remap_columns(a, pos)), *negated),
        Expr::Like(a, b, esc, negated) => Expr::Like(
            Box::new(remap_columns(a, pos)),
            Box::new(remap_columns(b, pos)),
            *esc,
            *negated,
        ),
        Expr::In(a, list, negated) => Expr::In(
            Box::new(remap_columns(a, pos)),
            list.iter().map(|e| remap_columns(e, pos)).collect(),
            *negated,
        ),
        Expr::Binary(a, op, b) => Expr::Binary(
            Box::new(remap_columns(a, pos)),
            *op,
            Box::new(remap_columns(b, pos)),
        ),
        Expr::Func { name, args } => Expr::Func {
            name: name.clone(),
            args: args.iter().map(|e| remap_columns(e, pos)).collect(),
        },
        Expr::FullText(i, query, opts) => Expr::FullText(map(*i, pos), query.clone(), opts.clone()),
        Expr::Regex(i, pattern) => Expr::Regex(map(*i, pos), Box::new(remap_columns(pattern, pos))),
    }
}

/// The comparisons a block prefilter can answer, out of a scan predicate.
///
/// Only conjunctions are pushed down (`a AND b`): a disjunction could be
/// refuted per branch, but the filter operator keeps it, so leaving it out is
/// both sound and simple. An operator the prefilter cannot answer (`NE`, `LIKE`,
/// `IN`, …) makes its conjunct disappear — the scan then reads more blocks, not
/// fewer rows.
/// The columns a predicate reads.
///
/// A scan only has to decode the columns its projection and its predicate
/// touch; the rest of a wide row (an embedding, a tensor) costs nothing.
pub(crate) fn predicate_columns(expr: &Expr, out: &mut Vec<usize>) {
    match expr {
        Expr::Column(i) => out.push(*i),
        Expr::Literal(_) => {}
        Expr::Cmp(a, _, b)
        | Expr::And(a, b)
        | Expr::Or(a, b)
        | Expr::Like(a, b, ..)
        | Expr::Binary(a, _, b) => {
            predicate_columns(a, out);
            predicate_columns(b, out);
        }
        Expr::Not(a) | Expr::IsNull(a, _) => predicate_columns(a, out),
        Expr::In(a, list, _) => {
            predicate_columns(a, out);
            for e in list {
                predicate_columns(e, out);
            }
        }
        Expr::Func { args, .. } => {
            for e in args {
                predicate_columns(e, out);
            }
        }
        Expr::FullText(i, ..) => out.push(*i),
        Expr::Regex(i, _) => out.push(*i),
    }
}

pub(crate) fn prune_conditions(
    pred: &Expr,
    columns: &[String],
) -> Vec<crate::fast_rough_filter::PruneCond> {
    use crate::fast_rough_filter::{PruneCond, PruneOp};
    let mut out = Vec::new();
    let mut stack = vec![pred];
    while let Some(expr) = stack.pop() {
        match expr {
            Expr::And(a, b) => {
                stack.push(a);
                stack.push(b);
            }
            Expr::Cmp(left, op, right) => {
                let (column, value, op) = match (&**left, &**right) {
                    (Expr::Column(i), Expr::Literal(v)) => (*i, v, *op),
                    (Expr::Literal(v), Expr::Column(i)) => (
                        *i,
                        v,
                        match op {
                            parser::CmpOp::Lt => parser::CmpOp::Gt,
                            parser::CmpOp::Le => parser::CmpOp::Ge,
                            parser::CmpOp::Gt => parser::CmpOp::Lt,
                            parser::CmpOp::Ge => parser::CmpOp::Le,
                            other => *other,
                        },
                    ),
                    _ => continue,
                };
                let op = match op {
                    parser::CmpOp::Eq => PruneOp::Eq,
                    parser::CmpOp::Lt => PruneOp::Lt,
                    parser::CmpOp::Le => PruneOp::Le,
                    parser::CmpOp::Gt => PruneOp::Gt,
                    parser::CmpOp::Ge => PruneOp::Ge,
                    parser::CmpOp::Ne => continue,
                };
                let Some(name) = columns.get(column) else {
                    continue;
                };
                out.push(PruneCond {
                    column: name.clone(),
                    op,
                    value: value.clone(),
                });
            }
            _ => {}
        }
    }
    out
}

pub(crate) fn encode_row(row: &[Literal], types: &[String]) -> String {
    row.iter()
        .enumerate()
        .map(|(i, v)| encode_cell_stored(v, types.get(i)))
        .collect::<Vec<_>>()
        .join("\t")
}

/// The stored text of one cell, normalised by its declared type.
///
/// Split out of [`encode_row`] so a bulk `COPY` can render the text the block
/// writer needs and the value the append buffer needs in one pass: it used to
/// join the row into text and then parse that text back into `Value`s.
pub(crate) fn encode_cell_stored(v: &Literal, ty: Option<&String>) -> String {
    {
        let s = v.to_slt_string();
        match ty {
            // Rows are stored as text, but the reference stores the typed
            // value, so the text has to round-trip exactly: `to_slt_string`
            // renders six decimals for `.slt` *display* and would round a
            // value such as `1.0 / 3.0` at insert time. Display still goes
            // through `to_slt_string`; only storage takes this path.
            Some(t) if is_float_type(t) => match v {
                Literal::Float(f) => exact_float(*f),
                // A value that arrived as text keeps its own digits, but is
                // normalised through `f64` so `1` and `1.0` store alike.
                _ => match s.trim().parse::<f64>() {
                    Ok(f) => exact_float(f),
                    Err(_) => s,
                },
            },
            Some(t) if is_array_type(t) => {
                let rendered = brace_render(&s);
                if array_elem_is_int(t) {
                    truncate_numbers(&rendered)
                } else {
                    rendered
                }
            }
            // JSON columns are stored compactly (sorted keys, no spaces)
            Some(t) if is_json_type(t) => match expression::json::parse(&s) {
                Some(j) => j.compact(),
                None => s,
            },
            Some(t) if is_datetime_type(t) => {
                parser::normalize_datetime(&t.to_ascii_uppercase(), &s)
            }
            Some(t) if is_sparse_type(t) => {
                let float = t.to_ascii_uppercase().contains("FLOAT")
                    || t.to_ascii_uppercase().contains("DOUBLE");
                match reshape_sparse(&s, float) {
                    Some(v) => v,
                    None => s,
                }
            }
            Some(t) if is_embedding_type(t) => {
                if is_bit_type(t) {
                    pack_bit_vector(&s)
                } else if declared_elem_is_int(t) {
                    // an integer element type truncates the stored values
                    truncate_numbers(&s)
                } else {
                    s
                }
            }
            Some(t) if is_tensor_type(t) => {
                let d = type_dim(t).unwrap_or(0);
                if is_bit_type(t) {
                    // `pack_bit_leaves` preserves the literal's nesting, so a
                    // `TENSORARRAY` (whose literal is already triple-nested)
                    // needs no extra wrapper.
                    reshape_bit_tensor(&s, d)
                } else {
                    reshape_tensor(&s, d)
                }
            }
            _ => s,
        }
    }
}

/// The reference's CSV cell parser for integer columns:
/// `ColumnVector::AppendByStringView` → `DataType::StringToValue<TinyIntT>`.
///
/// `std::from_chars` into the column's own width leaves the destination at its
/// initial 0 when the digits are out of range, and - because it still consumed
/// every digit - the reference's `ptr != end` check does not fire. So an
/// out-of-range cell is stored as 0, not wrapped and not rejected. Returns the
/// replacement cell when it applies.
pub(crate) fn zero_out_of_range_int(type_text: &str, cell: &str) -> Option<String> {
    let upper = type_text.trim().to_ascii_uppercase();
    let head = upper.split(['(', ',', ' ']).next().unwrap_or("").trim();
    let (min, max) = match head {
        "TINYINT" | "INT8" => (i64::from(i8::MIN), i64::from(i8::MAX)),
        "SMALLINT" | "INT16" => (i64::from(i16::MIN), i64::from(i16::MAX)),
        "INT" | "INTEGER" | "INT32" => (i64::from(i32::MIN), i64::from(i32::MAX)),
        // BIGINT is the widest: nothing can overflow it.
        _ => return None,
    };
    let n: i64 = cell.trim().parse().ok()?;
    (n < min || n > max).then(|| "0".to_string())
}

/// Renders a column default for `SHOW TABLE … COLUMNS`, mirroring the C++
/// `default_expr_->ToString()`. An absent default prints as `Null`; a sparse
/// default reproduces the C++ renderer, which (for float sparses) prints the
/// value before the index.
pub(crate) fn render_column_default(def: Option<&String>, type_text: &str) -> String {
    let Some(d) = def else {
        return "Null".to_string();
    };
    if !is_sparse_type(type_text) {
        return d.clone();
    }
    let t = d.trim();
    if t.is_empty() || t == "[]" {
        return "(empty)".to_string();
    }
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if inner.contains(':') {
        let pairs = parse_sparse_str(t);
        let body: Vec<String> = pairs.iter().map(|(i, v)| format!("{v:.6}:{i}")).collect();
        format!("[{}]", body.join(","))
    } else {
        let idx: Vec<String> = inner
            .split(',')
            .map(|s| s.trim().to_string())
            .filter(|s| !s.is_empty())
            .collect();
        format!("[{}]", idx.join(","))
    }
}

/// Rejects an out-of-range or malformed value for a **known** config key
/// (unknown keys are tolerated). Mirrors the C++ `Config` range checks
/// (`src/unit_test/main/config_ut.cpp::TestOutofRangeValues`): config values are
/// validated, never silently clamped.
pub(crate) fn invalid_config_value(key: &str, value: &str) -> Option<String> {
    match key.to_ascii_uppercase().as_str() {
        "RESULT_CACHE" => {
            let ok = matches!(
                value.to_ascii_lowercase().as_str(),
                "on" | "off" | "clear" | "suspend"
            );
            (!ok).then(|| {
                format!("invalid value '{value}' for result_cache (expected on|off|clear|suspend)")
            })
        }
        "CACHE_RESULT_CAPACITY" => match value.parse::<u64>() {
            Ok(n) if n >= 1 => None,
            _ => Some(format!(
                "invalid value '{value}' for cache_result_capacity (expected a positive integer)"
            )),
        },
        "CHECKPOINT_INTERVAL" => match value.parse::<u64>() {
            Ok(_) => None,
            Err(_) => Some(format!(
                "invalid value '{value}' for CHECKPOINT_INTERVAL (expected a non-negative integer)"
            )),
        },
        _ => None,
    }
}

/// True when a result is small enough to keep in the result cache. Caching a
/// whole-table `SELECT *` retains (and clones on each hit) hundreds of MB.
pub(crate) fn cacheable_result(res: &QueryResult) -> bool {
    matches!(res, QueryResult::Rows { rows, .. } if rows.len() <= 4096)
}

/// True when a statement can change data or metadata and therefore invalidates
/// cached `SELECT` results.
pub(crate) fn invalidates_result_cache(stmt: &Statement) -> bool {
    matches!(
        stmt,
        Statement::Insert { .. }
            | Statement::InsertSelect { .. }
            | Statement::Delete { .. }
            | Statement::Update { .. }
            | Statement::CreateTable { .. }
            | Statement::CreateTableAs { .. }
            | Statement::CreateView { .. }
            | Statement::DropView { .. }
            | Statement::DropTable { .. }
            | Statement::AlterTableAdd { .. }
            | Statement::AlterTableDropColumn { .. }
            | Statement::RenameTable { .. }
            | Statement::CreateIndex { .. }
            | Statement::DropIndex { .. }
            | Statement::CreateDatabase { .. }
            | Statement::DropDatabase { .. }
            | Statement::CreateCollection { .. }
            | Statement::DropCollection { .. }
            | Statement::CreateSnapshot { .. }
            | Statement::CreateDatabaseSnapshot { .. }
            | Statement::CreateSystemSnapshot { .. }
            | Statement::DropSnapshot { .. }
            | Statement::RestoreSnapshot { .. }
            | Statement::RestoreDatabaseSnapshot { .. }
            | Statement::RestoreSystemSnapshot { .. }
            | Statement::Copy { from: true, .. }
    )
}

/// Evaluates a Lucene `minimum_should_match` spec against the number of
/// optional clauses, returning how many terms a document must match.
///
/// The spec is a whitespace-separated list of clauses evaluated in order; the
/// last one that applies wins. Each clause is `[n<]value` where `value` is a
/// non-negative count, a negative count (`optional - n`), or a percentage of
/// the optional count (negative percentages subtract from it).
pub(crate) fn minimum_should_match(spec: &str, optional: usize) -> usize {
    if optional == 0 {
        return 0;
    }
    let clause_value = |value: &str| -> Option<usize> {
        let (num, pct) = match value.strip_suffix('%') {
            Some(n) => (n, true),
            None => (value, false),
        };
        let n: i64 = num.trim().parse().ok()?;
        if pct {
            let scaled = (optional as f64 * n.unsigned_abs() as f64 / 100.0).floor() as i64;
            Some(if n < 0 {
                (optional as i64 - scaled).max(0) as usize
            } else {
                scaled.max(0) as usize
            })
        } else {
            Some(if n < 0 {
                (optional as i64 + n).max(0) as usize
            } else {
                n as usize
            })
        }
    };
    let mut required = 1usize.min(optional);
    for clause in spec.split_whitespace() {
        match clause.split_once('<') {
            Some((cond, rest)) => {
                let cond: i64 = match cond.trim().parse() {
                    Ok(v) => v,
                    Err(_) => continue,
                };
                if optional as i64 > cond {
                    match clause_value(rest.trim()) {
                        Some(v) => required = v.min(optional),
                        None => continue,
                    }
                } else {
                    required = optional;
                }
            }
            None => match clause_value(clause) {
                Some(v) => required = v.min(optional),
                None => continue,
            },
        }
    }
    required
}

/// True when the select list references the `ROW_ID()` pseudo-column.
pub(crate) fn uses_row_id(items: &[SelectItem]) -> bool {
    fn in_expr(e: &ExprAst) -> bool {
        match e {
            ExprAst::Func { name, args } => {
                name.eq_ignore_ascii_case("ROW_ID") || args.iter().any(in_expr)
            }
            ExprAst::Column(c) => c.eq_ignore_ascii_case("_row_id"),
            ExprAst::Binary(a, _, b)
            | ExprAst::Cmp(a, _, b)
            | ExprAst::And(a, b)
            | ExprAst::Or(a, b) => in_expr(a) || in_expr(b),
            ExprAst::Not(a) => in_expr(a),
            _ => false,
        }
    }
    items.iter().any(|it| match it {
        SelectItem::Func { name, .. } => name.eq_ignore_ascii_case("ROW_ID"),
        SelectItem::Column(name) => name.eq_ignore_ascii_case("_row_id"),
        SelectItem::Expr(e) => in_expr(e),
        _ => false,
    })
}

/// The name the reference prints for a column: the last identifier of a
/// qualified reference (`t.a` → `a`), without quoting.
pub(crate) fn bare_column_name(name: &str) -> String {
    let name = name.rsplit('.').next().unwrap_or(name).trim();
    name.trim_matches('"').trim_matches('`').to_string()
}

/// The first column an expression reads, if any.
pub(crate) fn expr_column(e: &ExprAst) -> Option<String> {
    match e {
        ExprAst::Column(c) => Some(bare_column_name(c)),
        ExprAst::Func { args, .. } => args.iter().find_map(expr_column),
        ExprAst::Binary(a, _, b)
        | ExprAst::Cmp(a, _, b)
        | ExprAst::And(a, b)
        | ExprAst::Or(a, b) => expr_column(a).or_else(|| expr_column(b)),
        ExprAst::Not(a) => expr_column(a),
        ExprAst::Literal(_) => None,
    }
}

/// The first column an `INSERT … SELECT`'s select list reads, if any.
///
/// `SELECT *` counts: the binder expands it to the source's columns, whose
/// names `expanded` carries in order.
pub(crate) fn insert_select_column(query: &Statement, expanded: &[String]) -> Option<String> {
    let Statement::Select { items, .. } = query else {
        return None;
    };
    for item in items {
        match item {
            SelectItem::Star => {
                return Some(expanded.first().cloned().unwrap_or_else(|| "*".to_string()))
            }
            SelectItem::Column(name) => return Some(bare_column_name(name)),
            SelectItem::Expr(e) => {
                if let Some(column) = expr_column(e) {
                    return Some(column);
                }
            }
            SelectItem::Func { args, .. } => {
                if let Some(column) = args.iter().find_map(expr_column) {
                    return Some(column);
                }
            }
            SelectItem::Aggregate(agg) => {
                for expr in agg.arg_exprs.iter().flatten() {
                    if let Some(column) = expr_column(expr) {
                        return Some(column);
                    }
                }
                // `COUNT(*)` reads no column; any other plain argument does.
                if let Some(name) = agg.args.first().filter(|a| a.trim() != "*") {
                    return Some(bare_column_name(name));
                }
            }
            SelectItem::Literal(_) => {}
        }
    }
    None
}

/// Rewrites `UNNEST(col)` to the name of the column the expansion appends.
///
/// The SDKs turn `filter("uc2 > 1")` into `WHERE unnest(c2) > 1` (the alias is
/// replaced by the output expression), but the filter runs *after* the
/// expansion, where the values live in a synthetic column.
pub(crate) fn rewrite_unnest_expr(e: &ExprAst, col: &str, alias: &str) -> ExprAst {
    match e {
        ExprAst::Func { name, args }
            if name.eq_ignore_ascii_case("UNNEST")
                && args.len() == 1
                && matches!(&args[0], ExprAst::Column(c) if c.eq_ignore_ascii_case(col)) =>
        {
            ExprAst::Column(alias.to_string())
        }
        ExprAst::Func { name, args } => ExprAst::Func {
            name: name.clone(),
            args: args
                .iter()
                .map(|a| rewrite_unnest_expr(a, col, alias))
                .collect(),
        },
        ExprAst::Binary(a, op, b) => ExprAst::Binary(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Cmp(a, op, b) => ExprAst::Cmp(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Not(a) => ExprAst::Not(Box::new(rewrite_unnest_expr(a, col, alias))),
        ExprAst::And(a, b) => ExprAst::And(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        ExprAst::Or(a, b) => ExprAst::Or(
            Box::new(rewrite_unnest_expr(a, col, alias)),
            Box::new(rewrite_unnest_expr(b, col, alias)),
        ),
        other => other.clone(),
    }
}

pub(crate) fn rewrite_unnest_pred(p: &Predicate, col: &str, alias: &str) -> Predicate {
    match p {
        Predicate::Cmp(a, op, b) => Predicate::Cmp(
            rewrite_unnest_expr(a, col, alias),
            *op,
            rewrite_unnest_expr(b, col, alias),
        ),
        Predicate::IsTrue(e) => Predicate::IsTrue(rewrite_unnest_expr(e, col, alias)),
        Predicate::IsNull(e, not) => Predicate::IsNull(rewrite_unnest_expr(e, col, alias), *not),
        Predicate::Like(a, b, esc, not) => Predicate::Like(
            rewrite_unnest_expr(a, col, alias),
            rewrite_unnest_expr(b, col, alias),
            *esc,
            *not,
        ),
        Predicate::In(e, list, not) => Predicate::In(
            rewrite_unnest_expr(e, col, alias),
            list.iter()
                .map(|x| rewrite_unnest_expr(x, col, alias))
                .collect(),
            *not,
        ),
        Predicate::Bool(a, op, b) => Predicate::Bool(
            Box::new(rewrite_unnest_pred(a, col, alias)),
            *op,
            Box::new(rewrite_unnest_pred(b, col, alias)),
        ),
        Predicate::Not(a) => Predicate::Not(Box::new(rewrite_unnest_pred(a, col, alias))),
    }
}

/// Runs `f` allowing a zero-dimension sparse column.
///
/// A guard, not a call/restore: the flag is thread-local and the execution
/// threads are reused by the pool, so a panic inside `f` that left it set would
/// let an unrelated statement accept a zero-dimension sparse afterwards.
pub fn with_zero_dimension_sparse<T>(f: impl FnOnce() -> T) -> T {
    struct Reset(bool);
    impl Drop for Reset {
        fn drop(&mut self) {
            ALLOW_ZERO_SPARSE.with(|c| c.set(self.0));
        }
    }
    let previous = ALLOW_ZERO_SPARSE.with(|c| c.replace(true));
    let _restore = Reset(previous);
    f()
}

/// Rejects `AND`/`OR`/`NOT` with non-boolean operands (e.g. `c1 + 0.1 AND …`).
pub(crate) fn validate_boolean_expr(e: &ExprAst) -> Result<()> {
    match e {
        ExprAst::And(a, b) | ExprAst::Or(a, b) => {
            for x in [a.as_ref(), b.as_ref()] {
                if !is_boolean_expr(x) {
                    return Err(Error::Invalid(
                        "boolean operator requires boolean operands".into(),
                    ));
                }
                validate_boolean_expr(x)?;
            }
            Ok(())
        }
        ExprAst::Not(a) => {
            if !is_boolean_expr(a) {
                return Err(Error::Invalid("NOT requires a boolean operand".into()));
            }
            validate_boolean_expr(a)
        }
        ExprAst::Binary(a, _, b) | ExprAst::Cmp(a, _, b) => {
            validate_boolean_expr(a)?;
            validate_boolean_expr(b)
        }
        ExprAst::Func { args, .. } => {
            for a in args {
                validate_boolean_expr(a)?;
            }
            Ok(())
        }
        _ => Ok(()),
    }
}

/// `DISTANCE()` requires a distance metric and `SIMILARITY()` a similarity
/// metric; reject the mismatch.
pub(crate) fn validate_search_pseudo_columns(s: &SearchClause, items: &[SelectItem]) -> Result<()> {
    let Some(m) = s.matches.first() else {
        return Ok(());
    };
    let is_sparse = m.kind.eq_ignore_ascii_case("SPARSE");
    let is_vector = m.kind.eq_ignore_ascii_case("VECTOR");
    if !is_vector && !is_sparse {
        return Ok(());
    }
    let metric = arg_str(m.args.get(if is_sparse { 1 } else { 2 }))
        .unwrap_or_else(|| if is_sparse { "ip".into() } else { "l2".into() })
        .to_ascii_lowercase();
    let distance_metric = matches!(metric.as_str(), "l2" | "hamming");
    for it in items {
        if let SelectItem::Func { name, .. } = it {
            let n = name.to_ascii_uppercase();
            if n == "DISTANCE" && !distance_metric {
                return Err(Error::Invalid(
                    "DISTANCE() requires a distance metric".into(),
                ));
            }
            if n == "SIMILARITY" && distance_metric {
                return Err(Error::Invalid(
                    "SIMILARITY() requires a similarity metric".into(),
                ));
            }
        }
    }
    Ok(())
}

/// Validates an INSERT value against its declared column type (dimension or
/// sparse index range).
pub(crate) fn validate_insert_value(type_text: &str, lit: &str) -> Result<()> {
    // A BOOLEAN cell must be `true`/`false`; the C++ CSV reader rejects anything
    // else with a parser error (`Boolean type should be true or false`).
    if type_text.trim().eq_ignore_ascii_case("BOOLEAN")
        || type_text.trim().eq_ignore_ascii_case("BOOL")
    {
        let v = lit.trim();
        // `NULL` arrives as literal text from the `INSERT` path, and the Thrift
        // wire encodes booleans as 1/0.
        if !v.is_empty()
            && !v.eq_ignore_ascii_case("true")
            && !v.eq_ignore_ascii_case("false")
            && !v.eq_ignore_ascii_case("null")
            && v != "1"
            && v != "0"
        {
            return Err(Error::Invalid(
                "parser: Boolean type should be true or false".into(),
            ));
        }
    }
    if is_sparse_type(type_text) {
        return validate_sparse_literal(type_text, lit);
    }
    // A scalar column rejects text that does not parse as its type: the
    // reference's `StringToValue` fails, surfacing as a conversion error.
    let upper = type_text.trim().to_ascii_uppercase();
    let v = lit.trim();
    if !v.is_empty() && !v.eq_ignore_ascii_case("null") {
        let integer = upper == "INT"
            || upper.starts_with("INTEGER")
            || upper.starts_with("TINYINT")
            || upper.starts_with("SMALLINT")
            || upper.starts_with("BIGINT")
            || upper.starts_with("HUGEINT");
        if integer && v.parse::<i64>().is_err() && v.parse::<f64>().is_err() {
            return Err(Error::Invalid(format!(
                "not supported type conversion: '{v}' to {}",
                upper
            )));
        }
        let floating = upper.starts_with("DOUBLE") || upper.starts_with("FLOAT");
        if floating && v.parse::<f64>().is_err() {
            return Err(Error::Invalid(format!(
                "not supported type conversion: '{v}' to {}",
                upper
            )));
        }
    }
    // A column that stores a vector/tensor/array cannot take a bare scalar;
    // the C++ engine rejects this as NOT_SUPPORTED.
    let v = lit.trim();
    if (is_embedding_type(type_text)
        || is_tensor_type(type_text)
        || is_array_type(type_text)
        || is_sparse_type(type_text))
        && !v.is_empty()
        && !v.eq_ignore_ascii_case("null")
        && !v.starts_with('[')
    {
        return Err(Error::Invalid(format!(
            "not supported: column type '{}' requires an array value",
            type_text.trim()
        )));
    }
    validate_tensor_dimension(type_text, lit)
}

/// C++ `DataType::SupportBloomFilter()` (`data_type.h:149`): the types
/// `CREATE TABLE … WITH (bloom_filter_columns = …)` accepts. `DECIMAL` and
/// `HUGEINT` are on the reference's list but its DDL rejects both column types,
/// so they cannot be reached.
pub(crate) fn supports_bloom_filter(type_text: &str) -> bool {
    let t = type_text.trim().to_ascii_uppercase();
    let head = t.split(['(', ',']).next().unwrap_or("").trim();
    matches!(
        head,
        "BOOLEAN"
            | "TINYINT"
            | "SMALLINT"
            | "INTEGER"
            | "INT"
            | "BIGINT"
            | "HUGEINT"
            | "DECIMAL"
            | "VARCHAR"
            | "DATE"
            | "TIME"
            | "DATETIME"
            | "TIMESTAMP"
    )
}

/// The per-column `build_bloom_filter_` flags from `CREATE TABLE … WITH
/// (bloom_filter_columns = 'c1, c2')` (`logical_planner_impl.cpp:690`).
///
/// The reference splits the value on `,`, trims spaces (only spaces), resolves
/// each name to a column id, sorts and dedupes the ids, and refuses a type that
/// cannot carry a bloom filter. A repeated property name is a syntax error and
/// unknown names are ignored.
pub(crate) fn bloom_filter_flags(
    opts: &[(String, String)],
    names: &[String],
    types: &[String],
    table: &str,
) -> Result<Vec<bool>> {
    let mut flags = vec![false; names.len()];
    let mut seen: std::collections::HashSet<&str> = Default::default();
    for (key, value) in opts {
        if !seen.insert(key.as_str()) {
            return Err(Error::Invalid(format!(
                "Duplicate table property param name: {key}"
            )));
        }
        if !key.eq_ignore_ascii_case("BLOOM_FILTER_COLUMNS") {
            continue;
        }
        let mut ids: Vec<usize> = Vec::new();
        for raw in value.split(',') {
            let name = raw.trim_matches(' ');
            let Some(id) = names.iter().position(|n| n == name) else {
                return Err(Error::Invalid(format!(
                    "Column {name} not found in table {table}"
                )));
            };
            ids.push(id);
        }
        ids.sort_unstable();
        ids.dedup();
        for id in ids {
            if !supports_bloom_filter(&types[id]) {
                return Err(Error::Invalid(format!(
                    "Bloom filter can't be created for {} type column {}",
                    describe_type(&types[id]),
                    names[id]
                )));
            }
            flags[id] = true;
        }
    }
    Ok(flags)
}

/// A sparse vector's indices must be `< bound`, non-negative and distinct.
pub(crate) fn validate_sparse_literal(type_text: &str, lit: &str) -> Result<()> {
    let t = lit.trim();
    if !t.starts_with('[') {
        return Ok(());
    }
    let bound = type_dim(type_text).unwrap_or(0);
    // An integer-element sparse column rejects fractional values.
    let up = type_text.to_ascii_uppercase();
    let int_elem = up
        .split('(')
        .nth(1)
        .map(|inner| {
            inner.trim_start().starts_with("INT") || inner.trim_start().starts_with("UINT")
        })
        .unwrap_or(false);
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    let mut seen: Vec<i64> = Vec::new();
    for item in inner.split(',') {
        let item = item.trim();
        if item.is_empty() {
            continue;
        }
        let idx_txt = item.split(':').next().unwrap_or(item).trim();
        let Ok(i) = idx_txt.parse::<i64>() else {
            return Err(Error::Invalid(format!("invalid sparse index '{idx_txt}'")));
        };
        if i < 0 {
            return Err(Error::Invalid(format!("sparse index {i} is negative")));
        }
        if bound > 0 && i >= bound as i64 {
            return Err(Error::Invalid(format!(
                "sparse index {i} is out of range (bound {bound})"
            )));
        }
        if int_elem {
            if let Some((_, val)) = item.split_once(':') {
                if val.trim().parse::<i64>().is_err() {
                    return Err(Error::Invalid(
                        "parser: sparse value must be an integer".into(),
                    ));
                }
            }
        }
        if seen.contains(&i) {
            return Err(Error::Invalid(format!("duplicate sparse index {i}")));
        }
        seen.push(i);
    }
    Ok(())
}

/// Reconstructs a `Literal` from a stored default's text form.
pub(crate) fn default_literal(s: &str) -> Literal {
    if s.eq_ignore_ascii_case("NULL") {
        Literal::Null
    } else if let Ok(i) = s.parse::<i64>() {
        Literal::Int(i)
    } else if let Ok(f) = s.parse::<f64>() {
        Literal::Float(f)
    } else {
        Literal::Str(s.to_string())
    }
}

/// Aligns INSERT values to the table schema: an explicit column list maps by
/// name; otherwise values are positional. Missing columns take their default.
pub(crate) fn align_insert_rows(
    schema: &[String],
    defaults: &[Option<String>],
    cols: Option<&[String]>,
    rows: &[Vec<Literal>],
) -> Vec<Vec<Literal>> {
    rows.iter()
        .map(|row| match cols {
            Some(names) => {
                let mut out: Vec<Literal> = (0..schema.len())
                    .map(|i| {
                        defaults
                            .get(i)
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null)
                    })
                    .collect();
                for (name, val) in names.iter().zip(row) {
                    if let Some(i) = schema.iter().position(|c| c == name) {
                        if i < out.len() {
                            out[i] = val.clone();
                        }
                    }
                }
                out
            }
            None => {
                let mut out = row.clone();
                while out.len() < schema.len() {
                    out.push(
                        defaults
                            .get(out.len())
                            .and_then(|d| d.as_deref())
                            .map(default_literal)
                            .unwrap_or(Literal::Null),
                    );
                }
                out
            }
        })
        .collect()
}

/// Converts a projected value to the [`Literal`] the insert path expects for
/// `type_text`. This is the implicit cast `INSERT … SELECT` performs: numeric
/// text becomes a number for a numeric target, while everything stored as a
/// string keeps its text form.
pub(crate) fn literal_from_value(value: &Value, type_text: &str) -> Literal {
    if is_int_type_text(type_text) {
        return match value {
            Value::Int(i) => Literal::Int(*i),
            Value::Float(f) => Literal::Float(*f),
            Value::Bool(b) => Literal::Int(i64::from(*b)),
            Value::Str(s) => match s.trim().parse::<i64>() {
                Ok(i) => Literal::Int(i),
                Err(_) => Literal::Str(s.clone()),
            },
            Value::Null => Literal::Null,
        };
    }
    if is_float_type(type_text) {
        return match value {
            Value::Int(i) => Literal::Int(*i),
            Value::Float(f) => Literal::Float(*f),
            Value::Bool(b) => Literal::Float(if *b { 1.0 } else { 0.0 }),
            Value::Str(s) => match s.trim().parse::<f64>() {
                Ok(f) => Literal::Float(f),
                Err(_) => Literal::Str(s.clone()),
            },
            Value::Null => Literal::Null,
        };
    }
    if is_bool_type_text(type_text) {
        return match value {
            Value::Bool(b) => Literal::Str(b.to_string()),
            Value::Int(i) => Literal::Str((*i != 0).to_string()),
            Value::Str(s) => Literal::Str(s.clone()),
            Value::Float(f) => Literal::Str((*f != 0.0).to_string()),
            Value::Null => Literal::Null,
        };
    }
    match value {
        Value::Int(i) => Literal::Str(i.to_string()),
        Value::Float(f) => Literal::Str(f.to_string()),
        Value::Bool(b) => Literal::Str(b.to_string()),
        Value::Str(s) => Literal::Str(s.clone()),
        Value::Null => Literal::Null,
    }
}

/// The SQL text carried by a parser-produced `SUBQUERY` pseudo-function.
pub(crate) fn subquery_sql_arg(args: &[ExprAst]) -> Result<&str> {
    match args.first() {
        Some(ExprAst::Literal(Literal::Str(sql))) => Ok(sql.as_str()),
        _ => Err(Error::Invalid("malformed subquery expression".into())),
    }
}

/// The SQL text of the subquery nested in `EXISTS(SUBQUERY(sql))`.
pub(crate) fn exists_subquery_sql(args: &[ExprAst]) -> Result<&str> {
    match args.first() {
        Some(ExprAst::Func { name, args }) if name.eq_ignore_ascii_case("SUBQUERY") => {
            subquery_sql_arg(args)
        }
        _ => Err(Error::Invalid("malformed EXISTS subquery".into())),
    }
}

/// A boolean-valued expression, used to splice `EXISTS` into an `ExprAst`
/// (`Literal` has no boolean variant; the reference AST encodes booleans as
/// `true`/`false` strings, which cannot feed `WHERE`).
pub(crate) fn boolean_expr(value: bool) -> ExprAst {
    ExprAst::Cmp(
        Box::new(ExprAst::Literal(Literal::Int(1))),
        CmpOp::Eq,
        Box::new(ExprAst::Literal(Literal::Int(if value { 1 } else { 0 }))),
    )
}

pub(crate) fn substitute_outer_row(
    sql: &str,
    columns: &[(String, usize)],
    row: &[Value],
    outer_table: &str,
) -> String {
    let sql = collapse_dots(sql);
    // A single-table subquery's column may be qualified with its own table,
    // which the binder resolves through the fully-qualified one; drop *that*
    // qualifier. A join keeps them, since they disambiguate. A qualifier that
    // names neither the enclosing relation nor the inner table is left alone:
    // dropping it would silently read the inner column instead.
    let inner_table = inner_table_of(&sql);
    let strip_inner = !sql.to_ascii_uppercase().contains(" JOIN ");
    let mut out = String::with_capacity(sql.len() + 16);
    let chars: Vec<char> = sql.chars().collect();
    let mut i = 0usize;
    while i < chars.len() {
        let c = chars[i];
        if c == '\'' {
            // Copy the quoted run verbatim.
            out.push(c);
            i += 1;
            while i < chars.len() {
                out.push(chars[i]);
                if chars[i] == '\'' {
                    i += 1;
                    break;
                }
                i += 1;
            }
            continue;
        }
        if c.is_alphanumeric() || c == '_' || c == '.' {
            let start = i;
            while i < chars.len()
                && (chars[i].is_alphanumeric() || chars[i] == '_' || chars[i] == '.')
            {
                i += 1;
            }
            let token: String = chars[start..i].iter().collect();
            match outer_column_value(&token, columns, row, outer_table) {
                Some(text) => out.push_str(&text),
                None if strip_inner
                    && token.matches('.').count() == 1
                    && inner_table
                        .as_deref()
                        .is_some_and(|table| qualifier_matches(&token, table)) =>
                {
                    out.push_str(token.rsplit_once('.').map(|(_, c)| c).unwrap_or(&token))
                }
                None => out.push_str(&token),
            }
            continue;
        }
        out.push(c);
        i += 1;
    }
    out
}

pub(crate) fn outer_literal(value: &Value) -> String {
    match value {
        Value::Null => "NULL".to_string(),
        Value::Int(i) => i.to_string(),
        Value::Float(f) => {
            // A float must stay a float: `-0` or `2` would re-parse as an int.
            let text = format!("{f:?}");
            if text.contains(['.', 'e', 'E']) {
                text
            } else {
                format!("{text}.0")
            }
        }
        Value::Bool(b) => b.to_string(),
        Value::Str(s) => format!("'{}'", s.replace('\'', "''")),
    }
}

/// The table a single-table subquery reads (the first `FROM <ident>`), for the
/// qualifier comparison in [`substitute_outer_row`].
pub(crate) fn inner_table_of(sql: &str) -> Option<String> {
    let upper = sql.to_ascii_uppercase();
    let at = upper.find("FROM ")? + "FROM ".len();
    let rest = &sql[at..];
    let end = rest
        .find(|c: char| !(c.is_alphanumeric() || c == '_' || c == '.'))
        .unwrap_or(rest.len());
    let table = &rest[..end];
    if table.is_empty() {
        None
    } else {
        Some(table.to_string())
    }
}

/// Whether `qualified` (`t.c`) is qualified with `table` (`t`, `db.t`, or
/// `db . t` before `collapse_dots`).
pub(crate) fn qualifier_matches(qualified: &str, table: &str) -> bool {
    let Some((qualifier, _)) = qualified.rsplit_once('.') else {
        return false;
    };
    qualifier.eq_ignore_ascii_case(table)
        || qualifier.eq_ignore_ascii_case(table.rsplit('.').next().unwrap_or(table))
}

/// Closes the spaces a nested query's text may carry around a qualifier dot
/// (`t . c` -> `t.c`) without touching a quoted string.
pub(crate) fn collapse_dots(sql: &str) -> String {
    let chars: Vec<char> = sql.chars().collect();
    let mut out: Vec<char> = Vec::with_capacity(chars.len());
    let mut i = 0usize;
    while i < chars.len() {
        if chars[i] == '\'' {
            while i < chars.len() {
                out.push(chars[i]);
                if chars[i] == '\'' && out.len() > 1 && out[out.len() - 2] != '\\' {
                    i += 1;
                    break;
                }
                i += 1;
            }
            continue;
        }
        if chars[i] == '.' {
            while out.last() == Some(&' ') {
                out.pop();
            }
            out.push('.');
            i += 1;
            while i < chars.len() && chars[i] == ' ' {
                i += 1;
            }
            continue;
        }
        out.push(chars[i]);
        i += 1;
    }
    out.into_iter().collect()
}

pub(crate) fn outer_column_value(
    token: &str,
    columns: &[(String, usize)],
    row: &[Value],
    outer_table: &str,
) -> Option<String> {
    let bare = match token.rsplit_once('.') {
        Some((qualifier, column)) => {
            // The outer table is fully qualified (`db.table`) while the query
            // may name it bare; either spelling identifies the same table.
            let bare_table = outer_table.rsplit('.').next().unwrap_or(outer_table);
            if !qualifier.eq_ignore_ascii_case(outer_table)
                && !qualifier.eq_ignore_ascii_case(bare_table)
            {
                return None;
            }
            column
        }
        None => token,
    };
    // Only the columns the binder reported as correlated: a bare name that the
    // inner table also has belongs to the inner query.
    let (_, index) = columns
        .iter()
        .find(|(name, _)| name.eq_ignore_ascii_case(bare))?;
    row.get(*index).map(outer_literal)
}

pub(crate) fn value_to_literal(value: Value) -> Literal {
    match value {
        Value::Int(i) => Literal::Int(i),
        Value::Float(f) => Literal::Float(f),
        Value::Str(s) => Literal::Str(s),
        Value::Bool(b) => Literal::Str(if b { "true" } else { "false" }.into()),
        Value::Null => Literal::Null,
    }
}

/// True when a `SEARCH MATCH` clause still carries an unresolved subquery in
/// one of its argument lists or its in-parenthesis filter. The search path
/// compiles those directly, without going through the planner.
pub(crate) fn search_has_subquery(search: &SearchClause) -> bool {
    search.matches.iter().any(|m| {
        m.args.iter().any(ExprAst::contains_subquery)
            || m.filter.as_ref().is_some_and(Predicate::contains_subquery)
    })
}

pub(crate) fn parse_value(cell: &str) -> Value {
    if let Ok(i) = cell.parse::<i64>() {
        Value::Int(i)
    } else if let Ok(f) = cell.parse::<f64>() {
        Value::Float(f)
    } else if cell.eq_ignore_ascii_case("null") {
        Value::Null
    } else if cell.eq_ignore_ascii_case("true") {
        Value::Bool(true)
    } else if cell.eq_ignore_ascii_case("false") {
        Value::Bool(false)
    } else {
        Value::Str(cell.to_string())
    }
}

impl std::hash::Hasher for IdHasher {
    fn finish(&self) -> u64 {
        self.0
    }

    fn write(&mut self, bytes: &[u8]) {
        // Only `u64` keys are used; keep the trait implementation total.
        let mut h = 0xcbf29ce484222325u64;
        for b in bytes {
            h = (h ^ u64::from(*b)).wrapping_mul(0x100000001b3);
        }
        self.0 = h;
    }

    fn write_u64(&mut self, i: u64) {
        self.0 = i;
    }
}

impl std::hash::Hasher for FxHasher {
    #[inline]
    fn finish(&self) -> u64 {
        self.hash
    }

    #[inline]
    fn write(&mut self, bytes: &[u8]) {
        let (chunks, tail) = bytes.as_chunks::<8>();
        for chunk in chunks {
            self.add(u64::from_le_bytes(*chunk));
        }
        if !tail.is_empty() {
            let mut last = [0u8; 8];
            last[..tail.len()].copy_from_slice(tail);
            self.add(u64::from_le_bytes(last));
        }
    }

    #[inline]
    fn write_u8(&mut self, i: u8) {
        self.add(u64::from(i));
    }

    #[inline]
    fn write_u32(&mut self, i: u32) {
        self.add(u64::from(i));
    }

    #[inline]
    fn write_u64(&mut self, i: u64) {
        self.add(i);
    }

    #[inline]
    fn write_usize(&mut self, i: usize) {
        self.add(i as u64);
    }
}

/// One ascending posting source of a term: a slice of the in-memory tail, a
/// spilled run, or one sealed segment's DocListReader.
///
/// The cursors drive these directly: nothing is materialised, so a query pays
/// for the documents it actually visits instead of O(df) per term. An enum
/// rather than a trait object because doc()/tf()/len() run once per visited
/// document inside the block-max WAND loop, where the indirect call was a
/// measurable share of the work.
pub(crate) enum Source<'a> {
    /// The unsealed segment's memory tail: (row, tf, positions) plus the
    /// document-length map.
    Mem {
        list: &'a [(u64, u32, u32, u32)],
        /// The term's position arena (see TermPostings).
        pos: &'a [u32],
        lens: &'a DocLenMap,
        at: usize,
        /// The rows the tail covers, for the concat test.
        range: (u64, u64),
    },
    /// A list the caller owns: the query-syntax path's bmw_search, and the
    /// tests. The arena holds the token offsets when the caller has them.
    Owned {
        list: Vec<(u64, u32, u32)>,
        pos: PosArena,
        at: usize,
    },
    /// A sealed segment or a spilled run: the reference's DocListReader,
    /// decoding one 128-document mini-block at a time from the mmap.
    Segment(storage::invertedindex::disk_segment_reader::TermDocCursor<'a>),
}

impl<'a> Source<'a> {
    /// The current posting in one dispatch: row, term frequency and document
    /// length. doc/tf/len each re-match the enum, and the segment's tf and len
    /// each re-run its ensure.
    pub(crate) fn posting(&mut self) -> Option<(u64, u32, u32)> {
        match self {
            Source::Mem { list, lens, at, .. } => list
                .get(*at)
                .map(|p| (p.0, p.1, lens.get(&p.0).copied().unwrap_or(1))),
            Source::Owned { list, at, .. } => list.get(*at).map(|p| (p.0, p.1, p.2)),
            Source::Segment(cur) => cur.posting(),
        }
    }

    pub(crate) fn doc(&mut self) -> Option<u64> {
        match self {
            Source::Mem { list, at, .. } => list.get(*at).map(|p| p.0),
            Source::Owned { list, at, .. } => list.get(*at).map(|p| p.0),
            Source::Segment(cur) => cur.doc(),
        }
    }

    pub(crate) fn tf(&mut self) -> u32 {
        match self {
            Source::Mem { list, at, .. } => list.get(*at).map(|p| p.1).unwrap_or(0),
            Source::Owned { list, at, .. } => list.get(*at).map(|p| p.1).unwrap_or(0),
            Source::Segment(cur) => cur.tf(),
        }
    }

    /// The current document's length (BM25 dl).
    pub(crate) fn len(&mut self) -> u32 {
        match self {
            Source::Mem { list, lens, at, .. } => list
                .get(*at)
                .and_then(|p| lens.get(&p.0).copied())
                .unwrap_or(1),
            Source::Owned { list, at, .. } => list.get(*at).map(|p| p.2).unwrap_or(1),
            Source::Segment(cur) => cur.len(),
        }
    }

    pub(crate) fn positions(&mut self) -> &[u32] {
        match self {
            Source::Mem { list, pos, at, .. } => match list.get(*at) {
                Some((_, _, start, count)) => pos
                    .get(*start as usize..(*start + *count) as usize)
                    .unwrap_or(&[]),
                None => &[],
            },
            Source::Owned { pos, at, .. } => pos.get(*at),
            Source::Segment(cur) => cur.positions(),
        }
    }

    /// Appends up to `max` postings to the caller's three columns, stopping at
    /// `until` (the block's last row id), and returns how many it took. A sealed
    /// chunk already holds them decoded, so that case copies the slices instead
    /// of going through posting() per entry.
    pub(crate) fn extend_block(
        &mut self,
        docs: &mut Vec<u64>,
        tfs: &mut Vec<u32>,
        lens: &mut Vec<u32>,
        max: usize,
        until: u64,
    ) -> usize {
        if let Source::Segment(cur) = self {
            // A block the cursor has not decoded yet has to be loaded first;
            // block() only reports what is already there.
            if cur.doc().is_none() {
                return 0;
            }
            let Some((rows, tfs_at, _)) = cur.block() else {
                return 0;
            };
            let n = rows
                .iter()
                .position(|row| *row > until)
                .unwrap_or(rows.len())
                .min(max);
            docs.extend_from_slice(&rows[..n]);
            tfs.extend_from_slice(&tfs_at[..n]);
            lens.reserve(n);
            for i in 0..n {
                lens.push(cur.doc_len_at(i));
            }
            cur.skip(n);
            return n;
        }
        let mut n = 0;
        while n < max {
            let Some((doc, tf, len)) = self.posting() else {
                break;
            };
            if doc > until {
                break;
            }
            docs.push(doc);
            tfs.push(tf);
            lens.push(len);
            self.advance(doc + 1);
            n += 1;
        }
        n
    }

    pub(crate) fn advance(&mut self, target: u64) {
        match self {
            Source::Mem { list, at, .. } => {
                while *at < list.len() && list[*at].0 < target {
                    *at += 1;
                }
            }
            Source::Owned { list, at, .. } => {
                while *at < list.len() && list[*at].0 < target {
                    *at += 1;
                }
            }
            Source::Segment(cur) => cur.advance(target),
        }
    }

    /// The row range this source covers: sealed segments and the memory tail
    /// know it, which is what the concat path needs.
    pub(crate) fn range(&self) -> Option<(u64, u64)> {
        match self {
            Source::Mem { range, .. } => Some(*range),
            Source::Owned { list, .. } => Some((list.first()?.0, list.last()?.0)),
            Source::Segment(cur) => cur.reader().row_range(),
        }
    }

    pub(crate) fn df(&self) -> usize {
        match self {
            Source::Mem { list, .. } => list.len(),
            Source::Owned { list, .. } => list.len(),
            Source::Segment(cur) => cur.df(),
        }
    }

    /// The bound of the block the source is inside, **as the writer stored it**:
    /// `(last row of the block, largest tf, ceil(tf / doc_len) * u16::MAX)`.
    ///
    /// A sealed segment or a spilled run reads it from the doc skip list
    /// (`PostingIterator::GetBlockMaxInfo` over `SkipListReader::GetBlockMaxInfo`);
    /// the memory tail computes it over the 128-posting run the current row
    /// belongs to, which is what the reference's `InMemDocListDecoder` does.
    /// `None` when the source has no posting left or the list has no skip item
    /// (shorter than one block) - the caller then falls back to computing the
    /// bound from the decoded postings.
    pub(crate) fn block_stats(&mut self) -> Option<(u64, u32, u16)> {
        match self {
            Source::Segment(cur) => {
                cur.doc()?;
                let last_row = cur.block().and_then(|(rows, _, _)| rows.last().copied())?;
                let (max_tf, pct) = cur.block_max()?;
                Some((last_row, max_tf, pct))
            }
            Source::Mem { list, lens, at, .. } => {
                // Blocks start at the list's head, 128 postings each, exactly the
                // reference's `MAX_DOC_PER_RECORD` runs (`doc_list_encoder`).
                let block = WAND_BLOCK;
                let start = *at / block * block;
                let end = (start + block).min(list.len());
                let slice = list.get(start..end)?;
                let mut max_tf = 0u32;
                let mut best_pct = 0u16;
                for (row, tf, _, _) in slice {
                    max_tf = max_tf.max(*tf);
                    let len = lens.get(row).copied().unwrap_or(1).max(1);
                    let pct = (f64::from(*tf) / f64::from(len) * f64::from(u16::MAX)).ceil() as u16;
                    best_pct = best_pct.max(pct.max(1));
                }
                Some((slice.last()?.0, max_tf, best_pct))
            }
            Source::Owned { list, at, .. } => {
                let block = WAND_BLOCK;
                let start = *at / block * block;
                let end = (start + block).min(list.len());
                let slice = list.get(start..end)?;
                let mut max_tf = 0u32;
                let mut best_pct = 0u16;
                for (_, tf, len) in slice {
                    max_tf = max_tf.max(*tf);
                    let pct = (f64::from(*tf) / f64::from((*len).max(1)) * f64::from(u16::MAX))
                        .ceil() as u16;
                    best_pct = best_pct.max(pct.max(1));
                }
                Some((slice.last()?.0, max_tf, best_pct))
            }
        }
    }
}

/// One term's postings across every source, merged ascending by row id with the
/// newest copy of a row winning (PostingIterator over the reference's
/// segments).
///
/// cur is the smallest document the sources are on; it is not consumed until
/// the caller asks for the next one, so positions() still belongs to it.
/// The order in which the sources can simply be concatenated: every source
/// must know its row range and no two ranges may overlap. Returns the
/// permutation, or None when a real merge is needed.
pub(crate) fn concat_order(sources: &[Source<'_>]) -> Option<Vec<usize>> {
    if sources.len() < 2 {
        return None;
    }
    let mut ranges: Vec<(usize, (u64, u64))> = Vec::with_capacity(sources.len());
    for (i, source) in sources.iter().enumerate() {
        ranges.push((i, source.range()?));
    }
    ranges.sort_by_key(|(_, (lo, _))| *lo);
    for pair in ranges.windows(2) {
        if pair[0].1 .1 >= pair[1].1 .0 {
            return None;
        }
    }
    Some(ranges.into_iter().map(|(i, _)| i).collect())
}

/// Reorders the sources into the given permutation.
pub(crate) fn reorder<T>(items: Vec<T>, order: &[usize]) -> Vec<T> {
    let mut slots: Vec<Option<T>> = items.into_iter().map(Some).collect();
    order
        .iter()
        .map(|i| slots[*i].take().expect("a permutation of the sources"))
        .collect()
}

pub(crate) struct MergeCursor<'a> {
    pub(crate) sources: Vec<Source<'a>>,
    /// Sources >= segment_from are sealed segments: a row in overridden has a
    /// newer copy in the unsealed segment, so its sealed copy is stale.
    pub(crate) segment_from: usize,
    pub(crate) overridden: Option<&'a std::collections::HashSet<u64>>,
    /// (row, tf, length, winning source)
    pub(crate) cur: Option<(u64, u32, u32, usize)>,
    /// The smallest document still has to be recomputed (a source moved).
    pub(crate) dirty: bool,
    /// Exactly one source and no stale copy to filter: forward every accessor
    /// to it instead of running the k-way merge.
    pub(crate) single: bool,
    /// The sources cover disjoint, ascending row ranges, so the merged list is
    /// their concatenation: next walks them in order and no comparison runs
    /// per posting (the common case after a table has sealed a few segments).
    pub(crate) concat: bool,
    /// Index of the source the concat path is reading.
    pub(crate) next: usize,
    /// The document doc() last returned (the concat path caches it).
    pub(crate) cached: Option<u64>,
    pub(crate) df: usize,
}

impl<'a> MergeCursor<'a> {
    pub(crate) fn new(
        sources: Vec<Source<'a>>,
        segment_from: usize,
        overridden: Option<&'a std::collections::HashSet<u64>>,
    ) -> Self {
        let df = sources.iter().map(|s| s.df()).sum();
        let no_stale = overridden.is_none_or(|o| o.is_empty());
        let single = sources.len() == 1 && (segment_from >= 1 || no_stale);
        // Ranges that do not overlap make the merge a concatenation. A stale
        // copy (an overridden row) spans ranges, so it rules the path out.
        let (sources, concat) = match (single || !no_stale, concat_order(&sources)) {
            (false, Some(order)) => (reorder(sources, &order), true),
            _ => (sources, false),
        };
        let mut cursor = Self {
            sources,
            segment_from,
            overridden,
            cur: None,
            dirty: true,
            single,
            concat,
            next: 0,
            cached: None,
            df,
        };
        if concat {
            cursor.cached = cursor.pull_concat();
        } else {
            cursor.settle();
        }
        cursor
    }

    /// Pulls the next document from the concatenation of the sources: walk the
    /// current source, and move to the next one when it runs out.
    pub(crate) fn pull_concat(&mut self) -> Option<u64> {
        loop {
            if let Some(doc) = self.sources[self.next].doc() {
                return Some(doc);
            }
            if self.next + 1 >= self.sources.len() {
                return None;
            }
            self.next += 1;
        }
    }

    /// Documents containing the term (the reference's doc_freq).
    pub(crate) fn df(&self) -> usize {
        self.df
    }

    /// Recomputes the smallest current document, dropping stale sealed copies.
    pub(crate) fn settle(&mut self) {
        if self.single || !self.dirty {
            return;
        }
        loop {
            let mut best: Option<(usize, u64, u32, u32)> = None;
            for (i, src) in self.sources.iter_mut().enumerate() {
                let Some(doc) = src.doc() else { continue };
                if best.is_none_or(|(_, bd, _, _)| doc < bd) {
                    best = Some((i, doc, src.tf(), src.len()));
                }
            }
            let Some((winner, doc, tf, len)) = best else {
                self.cur = None;
                self.dirty = false;
                return;
            };
            if winner >= self.segment_from && self.overridden.is_some_and(|o| o.contains(&doc)) {
                self.sources[winner].advance(doc + 1);
                continue;
            }
            self.cur = Some((doc, tf, len, winner));
            self.dirty = false;
            return;
        }
    }

    /// Appends postings to the caller's block up to the block's last row id
    /// (`until`), one call per source instead of one per posting. Returns false
    /// when the cursor has nothing left for this block.
    pub(crate) fn extend_block(
        &mut self,
        docs: &mut Vec<u64>,
        tfs: &mut Vec<u32>,
        lens: &mut Vec<u32>,
        max: usize,
        until: u64,
    ) -> bool {
        if self.single {
            return self.sources[0].extend_block(docs, tfs, lens, max, until) > 0;
        }
        if self.concat {
            let mut filled = 0;
            while filled < max {
                if self.cached.is_none() {
                    self.cached = self.pull_concat();
                }
                match self.cached {
                    None => break,
                    // The block ends before the next source's range.
                    Some(doc) if doc > until => break,
                    Some(_) => {}
                }
                let n = self.sources[self.next].extend_block(docs, tfs, lens, max - filled, until);
                if n == 0 {
                    // This source ran out: move to the next one.
                    self.cached = None;
                    if self.next + 1 >= self.sources.len() {
                        break;
                    }
                    self.next += 1;
                    continue;
                }
                filled += n;
            }
            // The block holds these postings now: the merge's current document
            // is the next one, not the one the cache still names.
            self.cached = None;
            return filled > 0;
        }
        let mut n = 0;
        while n < max {
            let Some((doc, tf, len)) = self.posting() else {
                break;
            };
            if doc > until {
                break;
            }
            docs.push(doc);
            tfs.push(tf);
            lens.push(len);
            self.advance(doc + 1);
            n += 1;
        }
        n > 0
    }

    /// The bound of the block the merged cursor is inside: the smallest of the
    /// sources' block ends, with the largest tf / ratio among the sources that
    /// still cover it. That is the reference's `MultiPostingDecoder`
    /// `GetBlockLastDocID` / `GetBlockMaxInfo` pair - a bound may never claim
    /// more documents than the earliest source's block ends at.
    ///
    /// `None` when any live source has no stored bound (a list shorter than one
    /// block) or when nothing is left: the caller recomputes from the postings.
    pub(crate) fn block_stats(&mut self) -> Option<(u64, u32, u16)> {
        if self.single {
            return self.sources[0].block_stats();
        }
        if self.concat {
            return self.sources[self.next].block_stats();
        }
        let mut stats: Vec<(u64, u32, u16)> = Vec::with_capacity(self.sources.len());
        for source in &mut self.sources {
            if source.doc().is_none() {
                continue;
            }
            stats.push(source.block_stats()?);
        }
        let last = stats.iter().map(|s| s.0).min()?;
        let max_tf = stats.iter().filter(|s| s.0 >= last).map(|s| s.1).max()?;
        let pct = stats.iter().filter(|s| s.0 >= last).map(|s| s.2).max()?;
        Some((last, max_tf, pct))
    }

    /// The current posting (row, tf, document length) in one call. The single
    /// source path - a term whose postings all live in one chunk or in the
    /// memory tail - goes straight to it, with no merge state and no per-field
    /// dispatch.
    pub(crate) fn posting(&mut self) -> Option<(u64, u32, u32)> {
        if self.single {
            return self.sources[0].posting();
        }
        if self.concat {
            if self.cached.is_none() {
                self.cached = self.pull_concat();
            }
            return self.sources[self.next].posting();
        }
        self.settle();
        self.cur.map(|(doc, tf, len, _)| (doc, tf, len))
    }

    /// The current document (None once every source is exhausted).
    pub(crate) fn doc(&mut self) -> Option<u64> {
        if self.single {
            return self.sources[0].doc();
        }
        if self.concat {
            if self.cached.is_none() {
                self.cached = self.pull_concat();
            }
            return self.cached;
        }
        self.settle();
        self.cur.map(|c| c.0)
    }

    /// The current document's term frequency.
    pub(crate) fn tf(&mut self) -> u32 {
        if self.single {
            return self.sources[0].tf();
        }
        if self.concat {
            let _ = self.doc();
            return self.sources[self.next].tf();
        }
        self.settle();
        self.cur.map(|c| c.1).unwrap_or(0)
    }

    /// The current document's token offsets.
    pub(crate) fn positions(&mut self) -> &[u32] {
        if self.single {
            return self.sources[0].positions();
        }
        if self.concat {
            let _ = self.doc();
            return self.sources[self.next].positions();
        }
        self.settle();
        match self.cur {
            Some((_, _, _, winner)) => self.sources[winner].positions(),
            None => &[],
        }
    }

    /// SeekDoc: position on the first document >= target.
    pub(crate) fn advance(&mut self, target: u64) {
        if self.single {
            self.sources[0].advance(target);
            return;
        }
        if self.concat {
            self.cached = None;
            // A source whose last row is behind the target cannot contribute:
            // step over it whole.
            while self.next + 1 < self.sources.len()
                && self.sources[self.next]
                    .range()
                    .is_some_and(|(_, hi)| hi < target)
            {
                self.next += 1;
            }
            self.sources[self.next].advance(target);
            return;
        }
        self.dirty = true;
        for src in &mut self.sources {
            if src.doc().is_some_and(|doc| doc < target) {
                src.advance(target);
            }
        }
        self.settle();
    }
}

/// The interface BlockMaxWandIterator drives: a unit is a term or a phrase, and
/// either way it can say which block it is on and what that block's largest
/// tf / tf-length ratio is.
#[allow(clippy::large_enum_variant)] // one per query term, built once and kept on the stack
pub(crate) enum WandStream<'a> {
    Term(TermStream<'a>),
    Phrase(PhraseCursor<'a>),
}

impl<'a> WandStream<'a> {
    /// Documents this unit can match (the reference's doc_freq); a phrase can
    /// match no more often than its rarest term.
    pub(crate) fn df(&self) -> u64 {
        match self {
            WandStream::Term(s) => s.df() as u64,
            WandStream::Phrase(s) => s.children.iter().map(|c| c.df() as u64).min().unwrap_or(0),
        }
    }

    pub(crate) fn doc(&mut self) -> Option<u64> {
        match self {
            WandStream::Term(s) => s.doc(),
            WandStream::Phrase(s) => s.doc(),
        }
    }

    /// The unit's frequency: an integer for a term, fractional for a sloppy
    /// phrase (`PhraseDocIterator::tf_` is a float in the reference).
    pub(crate) fn tf(&mut self) -> f64 {
        match self {
            WandStream::Term(s) => f64::from(s.tf()),
            WandStream::Phrase(s) => s.tf(),
        }
    }

    /// The current document's length (BM25 dl).
    pub(crate) fn len(&mut self) -> u32 {
        match self {
            WandStream::Term(s) => s.len(),
            WandStream::Phrase(s) => s.len(),
        }
    }

    /// NextShallow: reposition at document level without resolving the unit.
    /// A term has nothing to defer.
    pub(crate) fn seek_block(&mut self, target: u64) {
        match self {
            WandStream::Term(s) => s.advance(target),
            WandStream::Phrase(s) => s.seek_block(target),
        }
    }

    /// Identity of the block the cursor is on (a BlockMaxBM25Score cache key).
    pub(crate) fn block_id(&mut self) -> u64 {
        match self {
            WandStream::Term(s) => s.block_id(),
            WandStream::Phrase(s) => s.block_id(),
        }
    }

    /// (last document of the block, largest tf, tf/doc-length ratio *
    /// `u16::MAX`), from the writer's stored block bounds.
    pub(crate) fn block_stats(&mut self) -> (u64, f64, u16) {
        match self {
            WandStream::Term(s) => s.block_stats(),
            WandStream::Phrase(s) => s.block_stats(),
        }
    }
}

/// One query term's postings as the block-max WAND cursor sees them: a
/// MergeCursor plus the current 128-posting block. Blocks are aligned from the
/// start of the list, exactly like the reference's MAX_DOC_PER_RECORD runs, so
/// a block bound stays valid while the cursor walks inside it.
pub(crate) struct TermStream<'a> {
    pub(crate) merge: MergeCursor<'a>,
    /// The block, column-major: only the three fields a posting needs.
    pub(crate) docs: Vec<u64>,
    pub(crate) tfs: Vec<u32>,
    pub(crate) lens: Vec<u32>,
    pub(crate) at: usize,
    pub(crate) block_id: u64,
    pub(crate) loaded: bool,
    /// The block's bound as the source stored it (`block_max_tf` /
    /// `block_max_percentage` from the doc skip list, or the memory tail's
    /// equivalent): `(last row, max tf, ratio * u16::MAX)`. `None` for a list
    /// without a skip item, where the bound is computed from the postings.
    pub(crate) stored: Option<(u64, u32, u16)>,
    /// The block's largest tf and tf/doc-length ratio, computed on the first
    /// block_stats() call (a block the WAND never bounds never pays for it).
    pub(crate) last: u64,
    pub(crate) stats_ready: bool,
    pub(crate) max_tf: f64,
    pub(crate) max_pct: f64,
    /// Keep each posting's token offsets (phrase children need them).
    pub(crate) capture: bool,
    pub(crate) pos: PosArena,
}

/// The number of postings a block-max bound covers (MAX_DOC_PER_RECORD, the
/// same 128 crate::wand uses).
pub(crate) const WAND_BLOCK: usize = 128;

pub(crate) fn take_decode_stats() -> (u64, u64) {
    DECODE_STATS.with(|c| c.replace((0, 0)))
}

impl<'a> TermStream<'a> {
    pub(crate) fn new(merge: MergeCursor<'a>) -> Self {
        Self::with_positions(merge, false)
    }

    pub(crate) fn with_positions(merge: MergeCursor<'a>, capture: bool) -> Self {
        Self {
            merge,
            docs: Vec::new(),
            tfs: Vec::new(),
            lens: Vec::new(),
            at: 0,
            block_id: 0,
            loaded: false,
            stored: None,
            last: u64::MAX,
            stats_ready: false,
            max_tf: 0.0,
            max_pct: 0.0,
            capture,
            pos: PosArena::default(),
        }
    }

    /// A stream over a list the caller owns (the query-syntax path): rows,
    /// term frequencies and lengths.
    pub(crate) fn from_owned(list: Vec<(u64, u32, u32)>) -> Self {
        Self::new(MergeCursor::new(
            vec![Source::Owned {
                list,
                pos: PosArena::default(),
                at: 0,
            }],
            1,
            None,
        ))
    }

    /// A stream over one materialised list and its lengths (tests).
    #[cfg(test)]
    pub(crate) fn from_slice(list: &'a [(u64, u32)], lens: &'a [(u64, u32)]) -> Self {
        let owned: Vec<(u64, u32, u32)> = list
            .iter()
            .map(|(doc, tf)| {
                let len = lens
                    .iter()
                    .find(|(d, _)| d == doc)
                    .map(|(_, l)| *l)
                    .unwrap_or(1);
                (*doc, *tf, len)
            })
            .collect();
        Self::from_owned(owned)
    }

    /// A stream over an owned list with token offsets (phrase tests).
    #[cfg(test)]
    pub(crate) fn from_postings(list: Vec<(u64, u32, Vec<u32>)>, lens: &'a [(u64, u32)]) -> Self {
        let mut pos = PosArena::default();
        let owned: Vec<(u64, u32, u32)> = list
            .iter()
            .map(|(doc, tf, positions)| {
                pos.push(positions);
                let len = lens
                    .iter()
                    .find(|(d, _)| d == doc)
                    .map(|(_, l)| *l)
                    .unwrap_or(1);
                (*doc, *tf, len)
            })
            .collect();
        Self::with_positions(
            MergeCursor::new(
                vec![Source::Owned {
                    list: owned,
                    pos,
                    at: 0,
                }],
                1,
                None,
            ),
            true,
        )
    }

    /// Documents containing the term (the reference's doc_freq).
    pub(crate) fn df(&self) -> usize {
        self.merge.df()
    }

    /// Loads the next block: the postings up to the **source's** block end,
    /// recording the block's bound as it goes.
    ///
    /// The block is the reference's `MAX_DOC_PER_RECORD` run of the posting
    /// list, not a window of 128 postings starting wherever the cursor landed.
    /// The source knows where its block ends (a sealed chunk's decoded
    /// mini-block, the skip list item's last doc id), so the cursor stops there
    /// and the bound it hands out is exactly the writer's - a seek inside a
    /// block therefore keeps the same bound, as in the reference.
    pub(crate) fn load(&mut self) -> bool {
        if self.at < self.docs.len() {
            return true;
        }
        self.docs.clear();
        self.tfs.clear();
        self.lens.clear();
        self.pos.clear();
        self.at = 0;
        self.stats_ready = false;
        // The stored bound of the block the sources are on; `None` degrades to
        // one 128-posting window and a bound computed from the postings.
        self.stored = self.merge.block_stats();
        let until = self.stored.map(|s| s.0).unwrap_or(u64::MAX);
        if self.capture {
            // A phrase child needs each posting's offsets, which only the
            // per-posting path can hand back.
            while self.docs.len() < WAND_BLOCK {
                let Some((doc, tf, len)) = self.merge.posting() else {
                    break;
                };
                if doc > until {
                    break;
                }
                let positions = self.merge.positions();
                self.pos.push(positions);
                self.docs.push(doc);
                self.tfs.push(tf);
                self.lens.push(len);
                self.merge.advance(doc + 1);
            }
        } else {
            // Nothing is copied per posting here: a sealed chunk hands back its
            // decoded mini-block in one call, and the merge copies the columns.
            let mut left = WAND_BLOCK - self.docs.len();
            while left > 0
                && self.merge.extend_block(
                    &mut self.docs,
                    &mut self.tfs,
                    &mut self.lens,
                    left,
                    until,
                )
            {
                left = WAND_BLOCK.saturating_sub(self.docs.len());
            }
        }
        if self.docs.is_empty() {
            return false;
        }
        DECODE_STATS.with(|c| {
            let (blocks, postings) = c.get();
            c.set((blocks + 1, postings + self.docs.len() as u64));
        });
        self.last = *self.docs.last().expect("block is not empty");
        if self.loaded {
            self.block_id += 1;
        } else {
            self.loaded = true;
        }
        true
    }

    /// The current document (None once exhausted).
    pub(crate) fn doc(&mut self) -> Option<u64> {
        if !self.load() {
            return None;
        }
        self.docs.get(self.at).copied()
    }

    /// The current document's term frequency.
    pub(crate) fn tf(&mut self) -> u32 {
        if !self.load() {
            return 0;
        }
        self.tfs.get(self.at).copied().unwrap_or(0)
    }

    /// The current document's length (BM25 dl), from the segment ordinal the
    /// cursor decoded - no search from the row id.
    pub(crate) fn len(&mut self) -> u32 {
        if !self.load() {
            return 1;
        }
        self.lens.get(self.at).copied().unwrap_or(1)
    }

    /// The current document's token offsets.
    pub(crate) fn positions(&mut self) -> &[u32] {
        if !self.load() {
            return &[];
        }
        self.pos.get(self.at)
    }

    /// SeekDoc: position on the first document >= target.
    ///
    /// Inside the block this is a linear step. When the target lies past the
    /// block, the **merged cursor** is seeked to it first (`MergeCursor::advance`
    /// → the sealed segment's `TermDocCursor::advance`, which hops blocks
    /// through the doc skip list) and the next block is decoded from where it
    /// landed - the reference's `TermDocIterator::NextShallow` →
    /// `PostingIterator::SkipTo` path. Without it the block-max WAND would decode
    /// every block between the two positions one at a time.
    pub(crate) fn advance(&mut self, target: u64) {
        loop {
            if !self.load() {
                return;
            }
            let Some(doc) = self.docs.get(self.at).copied() else {
                return;
            };
            if doc >= target {
                return;
            }
            // The target is past this block: leave `at` at the end so `load`
            // refills, and let the cursor seek - it is the layer that owns the
            // skip list. Walking the block first would pay a step per posting
            // for a jump the skip list covers in one decode.
            if target > self.last {
                self.at = self.docs.len();
                self.merge.advance(target);
                continue;
            }
            self.at += 1;
        }
    }

    /// Identity of the loaded block: the cache key of BlockMaxBM25Score().
    pub(crate) fn block_id(&mut self) -> u64 {
        self.load();
        self.block_id
    }

    /// The block's (last doc, largest tf, tf/doc-length ratio * `u16::MAX`).
    ///
    /// The first two components are the ones the writer stored for this block:
    /// a sealed chunk's doc skip list item, or the memory tail's per-128-run
    /// maximum - the same source `TermDocIterator::BlockMaxBM25Score` reads
    /// through `GetBlockMaxInfo`. Only a list without a skip item falls back to
    /// computing the bound from the decoded postings.
    pub(crate) fn block_stats(&mut self) -> (u64, f64, u16) {
        if !self.load() {
            return (u64::MAX, 0.0, 0);
        }
        if let Some((last, max_tf, pct)) = self.stored {
            return (last, f64::from(max_tf), pct);
        }
        if !self.stats_ready {
            // The largest tf and the largest tf/doc-length ratio of the block.
            // The ratio's winner is picked by cross-multiplication so the scan
            // pays one floating division per block instead of one per posting.
            let mut max_tf = 0u32;
            let mut best_tf = 0u64;
            let mut best_len = 1u64;
            for (tf, len) in self.tfs.iter().zip(&self.lens) {
                let len = u64::from((*len).max(1));
                max_tf = max_tf.max(*tf);
                if u64::from(*tf) * best_len > best_tf * len {
                    best_tf = u64::from(*tf);
                    best_len = len;
                }
            }
            self.max_tf = f64::from(max_tf);
            self.max_pct = best_tf as f64 / best_len as f64;
            self.stats_ready = true;
        }
        let pct = (self.max_pct * f64::from(u16::MAX))
            .ceil()
            .clamp(1.0, f64::from(u16::MAX)) as u16;
        (self.last, self.max_tf, pct)
    }
}

/// A phrase unit as one WAND cursor: it lands only on documents where the
/// terms sit at consecutive offsets, and its block bound is the **minimum** of
/// the terms' block bounds - a phrase can occur no more often than its rarest
/// term (BlockMaxPhraseScore, term_doc_iterator_impl.cpp). That is what lets
/// the iterator skip a whole block without verifying each document.
pub(crate) struct PhraseCursor<'a> {
    pub(crate) children: Vec<TermStream<'a>>,
    pub(crate) unit_len: usize,
    /// `"a b"~2`'s slop (`PhraseQueryNode::GetSlop`): 0 requires the terms to be
    /// strictly adjacent, otherwise Lucene's sloppy-phrase rule applies
    /// (`phrase_doc_iterator_impl.cpp:238-368`).
    pub(crate) slop: u32,
    pub(crate) cur: Option<u64>,
    /// The phrase frequency (`tf`). Fractional for a sloppy phrase, whose
    /// frequency is the sum of `1/(1+matchLength)` over the acceptable
    /// solutions (`phrase_doc_iterator_impl.cpp:337-340`).
    pub(crate) cur_tf: f64,
    pub(crate) cur_len: u32,
    pub(crate) scratch: Vec<Vec<u32>>,
    /// cur is stale: seek_block repositioned the children without verifying any
    /// phrase occurrence.
    pub(crate) pending: bool,
    /// Documents whose offsets were decoded to answer a query (test only).
    #[cfg(test)]
    pub(crate) verified: usize,
}

impl<'a> PhraseCursor<'a> {
    pub(crate) fn new(children: Vec<TermStream<'a>>, slop: u32) -> Self {
        let unit_len = children.len();
        let mut cursor = Self {
            children,
            unit_len,
            slop,
            cur: None,
            cur_tf: 0.0,
            cur_len: 1,
            scratch: Vec::new(),
            pending: true,
            #[cfg(test)]
            verified: 0,
        };
        cursor.resolve();
        cursor
    }

    /// NextShallow (phrase_doc_iterator_impl.cpp:139): reposition the children
    /// on target at document level **without** verifying a phrase occurrence.
    /// The next resolve() pays for the verification, so a block the WAND skips
    /// never has its offsets decoded.
    pub(crate) fn seek_block(&mut self, target: u64) {
        for child in self.children.iter_mut() {
            if child.doc().is_some_and(|doc| doc < target) {
                child.advance(target);
            }
        }
        self.pending = true;
        self.cur = None;
    }

    /// The reference's Next(): verify offsets from the current position on and
    /// stop at the next document where the phrase occurs. A no-op when the
    /// cursor is already resolved.
    pub(crate) fn resolve(&mut self) {
        if !self.pending && self.cur.is_some() {
            return;
        }
        self.settle();
        self.pending = false;
    }

    /// Test-only: how many documents had their offsets decoded.
    #[cfg(test)]
    pub(crate) fn verified(&self) -> usize {
        self.verified
    }

    pub(crate) fn settle(&mut self) {
        loop {
            let mut top: Option<u64> = None;
            for child in self.children.iter_mut() {
                let Some(doc) = child.doc() else {
                    self.cur = None;
                    return;
                };
                top = Some(top.map_or(doc, |t: u64| t.max(doc)));
            }
            let Some(top) = top else {
                self.cur = None;
                return;
            };
            let mut aligned = true;
            for child in self.children.iter_mut() {
                if child.doc() != Some(top) {
                    child.advance(top);
                    if child.doc() != Some(top) {
                        aligned = false;
                    }
                }
            }
            if !aligned {
                continue;
            }
            // Phrase frequency at top, from the terms' offsets.
            #[cfg(test)]
            {
                self.verified += 1;
            }
            self.scratch.resize(self.unit_len, Vec::new());
            let (children, scratch) = (&mut self.children, &mut self.scratch);
            for (i, child) in children.iter_mut().enumerate() {
                scratch[i].clear();
                scratch[i].extend_from_slice(child.positions());
            }
            // `slop == 0` keeps the exact-adjacency path (integer frequency);
            // a sloppy phrase sums `1/(1+matchLength)` over its solutions.
            let freq = if self.slop == 0 {
                phrase_freq_lists(scratch, self.unit_len) as f64
            } else {
                phrase_freq_lists_sloppy(scratch, self.unit_len, self.slop)
            };
            if freq > 0.0 {
                self.cur = Some(top);
                self.cur_tf = freq;
                // Any child on the aligned document has the document's length.
                self.cur_len = self.children.first_mut().map(|c| c.len()).unwrap_or(1);
                return;
            }
            for child in self.children.iter_mut() {
                child.advance(top + 1);
            }
        }
    }

    pub(crate) fn doc(&mut self) -> Option<u64> {
        self.resolve();
        self.cur
    }

    pub(crate) fn tf(&mut self) -> f64 {
        self.resolve();
        self.cur_tf
    }

    /// The matched document's length (BM25 dl).
    pub(crate) fn len(&mut self) -> u32 {
        self.resolve();
        self.cur_len
    }

    pub(crate) fn block_id(&mut self) -> u64 {
        let mut id = 0u64;
        for child in self.children.iter_mut() {
            id = id.wrapping_mul(31).wrapping_add(child.block_id());
        }
        id
    }

    /// The bound covers [cur, min over the terms' block ends].
    pub(crate) fn block_stats(&mut self) -> (u64, f64, u16) {
        let mut last = u64::MAX;
        let mut max_tf = f64::INFINITY;
        let mut max_pct = u16::MAX;
        for child in self.children.iter_mut() {
            let (l, t, p) = child.block_stats();
            last = last.min(l);
            max_tf = max_tf.min(t);
            // A zero percentage means "no stored bound", i.e. unbounded: it may
            // not lower the phrase's bound.
            if p > 0 {
                max_pct = max_pct.min(p);
            }
        }
        if !max_tf.is_finite() || last == u64::MAX {
            return (u64::MAX, 0.0, 0);
        }
        (last, max_tf, max_pct)
    }
}

/// One searched field's clause units plus the corpus statistics BM25 needs.
pub(crate) struct WandField<'a> {
    pub n_docs: usize,
    pub total_len: u64,
    pub units: Vec<WandUnit<'a>>,
}

/// One clause unit of one field: a term or a phrase, with the idf and the
/// delta its score uses (delta_term for a term, delta_phrase for a phrase).
pub(crate) struct WandUnit<'a> {
    pub stream: WandStream<'a>,
    pub weight: f64,
    pub idf: f64,
    /// None uses the iterator's delta (its delta_term); a phrase passes its own
    /// delta_phrase.
    pub delta: Option<f64>,
}

/// Cap for one in-memory inverted index. The heavy import tests build several
/// indexes over 250k wide rows; without a cap the postings alone reached ~20 GB.
pub(crate) const TEXT_INDEX_BUDGET_BYTES: usize = 512 * 1024 * 1024;

/// The in-memory budget of one column's inverted index, overridable for tests.
///
/// Past this budget the resident tail is spilled to a sorted run (the
/// reference's `CommitOffline`) and indexing continues, so a chunk can be built
/// from several runs - the **offline** dump (`OfflineDump`, multi-source merge)
/// instead of the realtime one. 512 MiB is unreachable from a test, and the
/// cross-engine gate has to drive that path on a few tens of thousands of rows,
/// so it lowers the budget through the environment.
pub(crate) fn text_index_budget_bytes() -> usize {
    static CAP: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *CAP.get_or_init(|| {
        std::env::var("INFINITY_TEXT_INDEX_BUDGET_BYTES")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(TEXT_INDEX_BUDGET_BYTES)
    })
}

/// Size at which the current segment is sealed to disk. Only this much posting
/// data stays resident per column; sealed segments are read through `mmap`,
/// like the reference's on-disk segments.
pub(crate) const TEXT_SEGMENT_BYTES: usize = 64 * 1024 * 1024;

/// The segment budget, overridable for tests that need to exercise sealing.
pub(crate) fn text_segment_bytes() -> usize {
    static CAP: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *CAP.get_or_init(|| {
        std::env::var("INFINITY_TEXT_SEGMENT_BYTES")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(TEXT_SEGMENT_BYTES)
    })
}

/// The postings and corpus statistics a query sees: the sealed segments plus
/// the unsealed tail, with stale (re-indexed) rows removed.
pub(crate) struct MergedIndex {
    pub(crate) n_docs: usize,
    pub(crate) total_len: u64,
    pub(crate) postings: HashMap<String, Vec<(u64, u32)>>,
}

pub(crate) type SharedTextIndex = Arc<std::sync::RwLock<TextIndex>>;

pub(crate) struct VectorCache {
    pub(crate) dims: usize,
    pub(crate) rows: Vec<(usize, Vec<f64>)>,
    pub(crate) live: std::collections::HashSet<usize>,
}

pub(crate) type SharedVectorCache = Arc<std::sync::RwLock<VectorCache>>;

/// The in-memory form of a vector index: the rows written since the last dump.
/// The reference keeps one `HnswIndexInMem` per segment; a query searches this
/// segment and merges it with the sealed (on-disk) ones.
/// Bounds how many full-table scans (full-text or vector) run at once. Each
/// such scan materialises a large share of the table, so several concurrent
/// readers can exhaust the process's memory even when a single query fits.
pub(crate) struct ScanGate {
    pub(crate) active: Mutex<usize>,
    pub(crate) cv: std::sync::Condvar,
    pub(crate) limit: usize,
}

impl ScanGate {
    pub(crate) fn new(limit: usize) -> Self {
        Self {
            active: Mutex::new(0),
            cv: std::sync::Condvar::new(),
            limit: limit.max(1),
        }
    }

    pub(crate) fn enter(&self) -> ScanSlot<'_> {
        let mut active = self.active.lock().unwrap();
        while *active >= self.limit {
            active = self.cv.wait(active).unwrap();
        }
        *active += 1;
        ScanSlot(self)
    }
}

pub(crate) struct ScanSlot<'a>(&'a ScanGate);

impl Drop for ScanSlot<'_> {
    fn drop(&mut self) {
        let mut active = self.0.active.lock().unwrap();
        *active = active.saturating_sub(1);
        self.0.cv.notify_one();
    }
}

/// One multi-vector index in the reference's two forms: the sealed segment
/// files (`seg_*.mvec` / `seg_*.emvb`, mapped read-only) and the memory tail
/// holding rows inserted since the last dump. Building the tail at write time
/// and dumping it on `flush()` keeps the query path read-only.
pub(crate) struct MultiVectorIndex {
    pub(crate) dims: usize,
    pub(crate) dir: PathBuf,
    /// `ROW_ID -> token rows` for documents not yet dumped.
    pub(crate) tail: std::collections::BTreeMap<u64, Vec<Vec<f32>>>,
    /// One mmap reader per dumped chunk plus the row id of every document and
    /// the persisted residual PQ (the reference's stage-2 candidate filter).
    #[allow(clippy::type_complexity)]
    pub(crate) sealed: Vec<(
        Arc<storage::multivector_segment::MultiVectorSegmentReader>,
        Arc<Vec<u64>>,
        Option<Arc<storage::emvb_segment::EmvbPqIndex>>,
    )>,
    /// Chunk ordinal used in file names.
    pub(crate) next_chunk: u64,
    /// Bytes held by `tail`, for the unified memindex budget.
    pub(crate) tail_bytes: usize,
    /// Bumped on every tail mutation; the in-memory EMVB index is rebuilt when
    /// it lags (the reference's `EMVBIndexInMem`).
    pub(crate) tail_generation: u64,
    pub(crate) tail_emvb: std::sync::Mutex<Option<MvTailEmvb>>,
    /// PLAID options when the column carries a `USING PLAID` index.
    pub(crate) plaid_config: Option<PlaidConfig>,
    /// The reference's `PlaidIndex` over every document (`PlaidIndexInMem`
    /// after its build, kept in step by incremental adds).
    pub(crate) plaid: Option<index::plaid::PlaidIndex>,
    /// Row id of each PLAID document id.
    pub(crate) plaid_seqs: Vec<u64>,
    /// Rows this index has seen (parsed or not). `doc_count` only counts parsed
    /// documents, so a column holding a NULL/empty tensor could never look
    /// complete against the table's live rows and every search rebuilt the whole
    /// index.
    pub(crate) scanned_rows: usize,
    /// Set when a mutation dropped `plaid` (a rewritten row, or an incremental
    /// add that could not be covered), so the next search rebuilds it instead of
    /// serving from sealed brute force forever.
    pub(crate) plaid_dropped: bool,
    /// Set when PLAID cannot exist for this column at all: the estimate is over
    /// [`plaid_memory_budget`], or the index refused to build.
    ///
    /// This port has no buffer-manager/mmap search path, so the budget is
    /// "do not build" rather than "build and spill". A table past it is served
    /// by the sealed brute-force scan, which is correct and merely slower - but
    /// a rebuild is pointless: the next attempt would take the same decision on
    /// the same data. Without this flag the flag `plaid_dropped` set by a
    /// mutation would keep the index permanently incomplete, and every query
    /// would rescan the table, retrain the centroids and rewrite the chunk set.
    ///
    /// It is not persisted: raising the budget and restarting gets a fresh
    /// build, because the index is reconstructed from the rows on open.
    pub(crate) plaid_unavailable: bool,
    /// SMVE options when the column carries a `USING SMVE` index.
    pub(crate) smve_config: Option<SmveConfig>,
    /// The index's `pq_subspace_num` / `pq_subspace_bits`, which decide how the
    /// residual PQ splits the embedding.
    pub(crate) emvb_config: Option<EmvbConfig>,
    /// The reference's `SMVEIndexInMem` (a BMP over SMVE sparse codes).
    pub(crate) smve: Option<index::smve::SmveIndex>,
    /// Row id of each SMVE document id.
    pub(crate) smve_seqs: Vec<u64>,
}

/// `IndexOptions`' PLAID search knobs, overridable per query
/// (`SEARCH MATCH TENSOR (…, 'plaid_n_ivf_probe=32')`). The defaults are the
/// reference's (`default_values.cppm:154-157`).
#[derive(Clone, Copy, Debug)]
pub(crate) struct PlaidSearchOptions {
    pub(crate) n_ivf_probe: usize,
    pub(crate) centroid_score_threshold: f32,
    pub(crate) n_doc_to_score: usize,
    pub(crate) n_full_scores: usize,
}

impl PlaidSearchOptions {
    /// `PLAID_N_DOC_TO_SCORE_FACTOR` / `PLAID_N_FULL_SCORES_FACTOR` scale with
    /// `topn`, so the defaults are derived once `topn` is known.
    pub(crate) fn for_topn(topn: usize) -> Self {
        Self {
            n_ivf_probe: 8,
            centroid_score_threshold: -0.1,
            n_doc_to_score: topn.max(1) * 100,
            n_full_scores: topn.max(1) * 10,
        }
    }
}

/// The PLAID knobs of one query: the defaults above, overridden by the search
/// options the statement carries (`SEARCH MATCH TENSOR (…, 'plaid_n_ivf_probe=32')`,
/// `logical_match_tensor_scan_impl.cpp:116-152`).
pub(crate) fn plaid_search_options(
    lookup: impl Fn(&str) -> Option<String>,
    topn: usize,
) -> PlaidSearchOptions {
    let topn = topn.max(1);
    let mut options = PlaidSearchOptions::for_topn(topn);
    if let Some(v) = lookup("plaid_n_ivf_probe")
        .and_then(|v| v.trim().parse::<usize>().ok())
        .filter(|v| *v > 0)
    {
        options.n_ivf_probe = v;
    }
    if let Some(v) =
        lookup("plaid_centroid_score_threshold").and_then(|v| v.trim().parse::<f32>().ok())
    {
        options.centroid_score_threshold = v;
    }
    // The reference rejects `n_doc_to_score < topn` and
    // `n_full_scores > n_doc_to_score`; a value it would reject keeps the
    // default instead of failing the query.
    if let Some(v) = lookup("plaid_n_doc_to_score")
        .and_then(|v| v.trim().parse::<usize>().ok())
        .filter(|v| *v >= topn)
    {
        options.n_doc_to_score = v;
    }
    if let Some(v) = lookup("plaid_n_full_scores")
        .and_then(|v| v.trim().parse::<usize>().ok())
        .filter(|v| *v >= topn && *v <= options.n_doc_to_score)
    {
        options.n_full_scores = v;
    }
    options
}

/// `CREATE INDEX … USING PLAID WITH (nbits=…, n_centroids=…)`.
#[derive(Clone, Copy, Debug)]
pub(crate) struct PlaidConfig {
    pub(crate) nbits: u32,
    pub(crate) n_centroids: u32,
    pub(crate) colbertsar: bool,
}

/// `CREATE INDEX … USING EMVB WITH (pq_subspace_num=…, pq_subspace_bits=…)`.
#[derive(Debug, Clone, Copy)]
pub(crate) struct EmvbConfig {
    pub(crate) subspace_num: usize,
    pub(crate) subspace_bits: u32,
}

/// `CREATE INDEX … USING SMVE WITH (width=…, topk=…)`.
#[derive(Debug, Clone, Copy)]
pub(crate) struct SmveConfig {
    pub(crate) width: u32,
    pub(crate) topk: u32,
}

/// The reference's `EMVBIndexInMem` over the multi-vector tail.
pub(crate) struct MvTailEmvb {
    pub(crate) generation: u64,
    pub(crate) data: storage::emvb_segment::EmvbData,
    pub(crate) seqs: Vec<u64>,
}

/// C++ `EMVBIndexInMem`: `build_index_threshold_ = 32 * (1 << bits)` with
/// `residual_pq_subspace_bits = 8`.
pub(crate) const EMVB_BUILD_THRESHOLD: usize = 32 * 256;

/// The threshold, overridable for tests that need to exercise the build.
pub(crate) fn emvb_build_threshold() -> usize {
    static CAP: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *CAP.get_or_init(|| {
        std::env::var("INFINITY_EMVB_BUILD_THRESHOLD")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(EMVB_BUILD_THRESHOLD)
    })
}

/// Heap a PLAID index may take before this port declines to build it.
pub(crate) fn plaid_memory_budget() -> usize {
    static BUDGET: std::sync::OnceLock<usize> = std::sync::OnceLock::new();
    *BUDGET.get_or_init(|| {
        std::env::var("INFINITY_PLAID_MAX_BYTES")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(512 * 1024 * 1024)
    })
}

pub(crate) fn multivector_doc_bytes(doc: &[Vec<f32>]) -> usize {
    doc.iter().map(|t| t.len() * 4 + 4).sum()
}

/// `MaxSim` over one in-memory document: sum over query rows of the best dot
/// product against a stored row (the same formula as the mmap reader).
///
/// Delegated to [`storage::simd::maxsim_f32`], which keeps this crate's
/// `#![forbid(unsafe_code)]` intact: the width-8 kernel is `std::simd`, and the
/// dot products are what a multi-vector re-rank spends its time on
/// (`nq * nd * dim` multiply-adds per candidate).
pub(crate) fn maxsim_tokens(query: &[Vec<f32>], doc: &[Vec<f32>]) -> f32 {
    storage::simd::maxsim_f32(query, doc)
}

impl MultiVectorIndex {
    pub(crate) fn new(
        dir: PathBuf,
        dims: usize,
        plaid_config: Option<PlaidConfig>,
        smve_config: Option<SmveConfig>,
        emvb_config: Option<EmvbConfig>,
    ) -> Self {
        let next_chunk = std::fs::read_dir(&dir)
            .map(|d| {
                d.filter_map(|e| e.ok())
                    .filter(|e| e.path().extension().is_some_and(|x| x == "mvec"))
                    .count() as u64
            })
            .unwrap_or(0);
        Self {
            dims,
            dir,
            tail: Default::default(),
            sealed: Vec::new(),
            next_chunk,
            tail_bytes: 0,
            tail_generation: 0,
            tail_emvb: std::sync::Mutex::new(None),
            plaid_config,
            plaid: None,
            plaid_seqs: Vec::new(),
            scanned_rows: 0,
            plaid_dropped: false,
            plaid_unavailable: false,
            smve_config,
            emvb_config,
            smve: None,
            smve_seqs: Vec::new(),
        }
    }

    pub(crate) fn doc_count(&self) -> usize {
        self.tail.len()
            + self
                .sealed
                .iter()
                .map(|(r, _, _)| r.doc_count())
                .sum::<usize>()
    }

    pub(crate) fn insert(&mut self, seq: u64, doc: Vec<Vec<f32>>) {
        let replacing = self.tail.contains_key(&seq);
        // The PLAID document list is `plaid_seqs`, which a cold start restores
        // without the sealed readers, so coverage is measured against it rather
        // than against `doc_count()`.
        let covered = self
            .plaid
            .as_ref()
            .is_some_and(|p| p.doc_num() as usize == self.plaid_seqs.len());
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(multivector_doc_bytes(&old));
        }
        self.tail_bytes += multivector_doc_bytes(&doc);
        self.tail.insert(seq, doc.clone());
        self.tail_generation += 1;
        // The reference's `PlaidIndexInMem::Insert`: once a trained index
        // covers every document, new rows are added incrementally. A rewritten
        // row cannot be removed from PLAID, so the index is dropped and the
        // next rebuild recreates it.
        if let Some(index) = self.plaid.as_mut() {
            let flat: Vec<f32> = doc.iter().flatten().copied().collect();
            let added = !replacing
                && covered
                && index
                    .add_multiple_docs_embeddings(&flat, &[doc.len() as u32])
                    .is_ok();
            if added {
                self.plaid_seqs.push(seq);
            } else {
                self.plaid = None;
                self.plaid_seqs.clear();
                self.plaid_dropped = true;
            }
        }
        // `SMVEIndexInMem::BuildFromColumn` runs per inserted block; the BMP
        // accepts one document at a time.
        if let Some(index) = self.smve.as_mut() {
            let label = index.row_count() as u32;
            if replacing || index.add_document(label, &doc).is_err() {
                self.smve = None;
                self.smve_seqs.clear();
            } else {
                self.smve_seqs.push(seq);
            }
        }
    }

    /// Records that the write hook handed this index one row.
    ///
    /// [`Self::insert`] cannot do it: a NULL (or unparseable) tensor cell adds
    /// no document, and the row would then never count towards
    /// [`Self::is_complete`] - every search would rebuild the whole index,
    /// PLAID training included. The rebuild sets the counter to the rows it
    /// scanned instead, so this is only for the per-row write path.
    pub(crate) fn note_row(&mut self) {
        self.scanned_rows += 1;
    }

    pub(crate) fn remove(&mut self, seq: u64) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(multivector_doc_bytes(&old));
            self.tail_generation += 1;
            // Neither index supports per-document deletes; the search bitmask
            // hides dead rows, and a rewritten row needs a fresh build.
            self.plaid = None;
            self.plaid_seqs.clear();
            self.plaid_dropped = true;
            self.smve = None;
            self.smve_seqs.clear();
        }
    }

    /// Whether this column's queries can reach the EMVB chunks.
    ///
    /// A PLAID or SMVE index answers a `SEARCH MATCH TENSOR` from its own
    /// structures and returns before the EMVB prefilter runs, so a segment's
    /// `.emvb` file would never be read. Anything else - a bare
    /// multi-vector column, or a column indexed with EMVB itself - does reach
    /// it, and needs the chunk.
    pub(crate) fn wants_emvb_chunk(&self) -> bool {
        self.plaid.is_none() && self.smve.is_none()
    }

    /// Builds the reference's `PlaidIndexInMem` over every buffered document.
    ///
    /// Called before the tail is sealed (the reference builds it while the
    /// embeddings are still in `buffered_embeddings_`). Below
    /// `max(256, n_centroids * 32)` embeddings the reference leaves the index
    /// unbuilt and the caller falls back to an exhaustive scan of the range.
    pub(crate) fn build_plaid(&mut self) {
        if std::env::var_os("INFINITY_DEBUG_PLAN").is_some() {
            eprintln!(
                "DEBUG build_plaid cfg={:?} tail={}",
                self.plaid_config,
                self.tail.len()
            );
        }
        let Some(cfg) = self.plaid_config else {
            return;
        };
        let total: usize = self.tail.values().map(Vec::len).sum();
        let threshold = 256usize.max(cfg.n_centroids as usize * 32);
        if total < threshold {
            return;
        }
        let mut k = if cfg.n_centroids == 0 {
            index::plaid::PlaidIndex::compute_auto_n_centroids(total as u64)
        } else {
            cfg.n_centroids
        };
        while k > 8 && total < 32 * k as usize {
            k >>= 1;
        }
        k = k.div_ceil(8) * 8;
        if k as usize > total {
            return;
        }
        let mut flat = Vec::with_capacity(total * self.dims);
        let mut lens = Vec::with_capacity(self.tail.len());
        let mut seqs = Vec::with_capacity(self.tail.len());
        for (seq, doc) in &self.tail {
            seqs.push(*seq);
            lens.push(doc.len() as u32);
            for token in doc {
                flat.extend_from_slice(token);
            }
        }
        // The reference keeps its PLAID index in the buffer manager and maps
        // chunks on demand; this port holds it in the process, so a table whose
        // residual table alone would exceed the budget keeps answering from the
        // sealed chunks instead of taking the process down. `INFINITY_PLAID_MAX_BYTES`
        // overrides it.
        let packed_dim = (self.dims * cfg.nbits as usize).div_ceil(8);
        let residual_bytes = if cfg.colbertsar {
            0
        } else {
            total * packed_dim
        };
        let centroid_bytes = k as usize * self.dims * 4;
        let assignment_bytes = total * 4;
        let estimate = residual_bytes + centroid_bytes + assignment_bytes;
        if estimate > plaid_memory_budget() {
            // "Do not build", not "build and spill": this port has no
            // buffer-manager or mmap search path, so the column keeps answering
            // from the sealed brute-force scan. Recording the decision is what
            // stops every later query from rescanning the table, retraining the
            // centroids and rewriting the chunk set only to decline again.
            self.degrade_plaid("over the memory budget");
            return;
        }
        let Ok(mut index) =
            index::plaid::PlaidIndex::new(0, self.dims as u32, cfg.nbits, k, cfg.colbertsar)
        else {
            self.degrade_plaid("PlaidIndex::new rejected the configuration");
            return;
        };
        if index.train(k, &flat, total, 4).is_err() {
            // Not recorded as final: a failed training can be transient (heap
            // pressure), and the next build may succeed.
            return;
        }
        if index.add_multiple_docs_embeddings(&flat, &lens).is_err() {
            return;
        }
        self.plaid = Some(index);
        self.plaid_seqs = seqs;
        self.plaid_dropped = false;
        self.plaid_unavailable = false;
    }

    /// Marks PLAID as impossible for this column and falls back to the sealed
    /// scan for good (until the process restarts and the index is rebuilt).
    fn degrade_plaid(&mut self, why: &str) {
        self.plaid = None;
        self.plaid_seqs.clear();
        self.plaid_dropped = true;
        self.plaid_unavailable = true;
        if std::env::var_os("INFINITY_DEBUG_PLAN").is_some() {
            eprintln!("DEBUG plaid degraded for the process: {why}");
        }
    }

    /// Whether this index can answer every live row: it has scanned them all and
    /// no mutation has dropped the PLAID index since - unless PLAID cannot exist
    /// here at all, in which case the sealed scan is the final answer rather
    /// than a state to repair.
    pub(crate) fn is_complete(&self, live_rows: usize) -> bool {
        self.scanned_rows >= live_rows && (!self.plaid_dropped || self.plaid_unavailable)
    }

    /// Whether this index was built knowing about `kind` (`PLAID` / `SMVE`).
    /// `EMVB` has no config of its own and is always known.
    pub(crate) fn knows_kind(&self, kind: &str) -> bool {
        if kind.eq_ignore_ascii_case("PLAID") {
            self.plaid_config.is_some()
        } else if kind.eq_ignore_ascii_case("SMVE") {
            self.smve_config.is_some()
        } else {
            true
        }
    }

    /// C++: `SMVEIndexInMem::BuildFromColumn` over the buffered documents.
    pub(crate) fn build_smve(&mut self) {
        if std::env::var_os("INFINITY_DEBUG_PLAN").is_some() {
            eprintln!(
                "DEBUG build_smve cfg={:?} tail={}",
                self.smve_config,
                self.tail.len()
            );
        }
        let Some(cfg) = self.smve_config else {
            return;
        };
        let mut docs: Vec<(u32, Vec<Vec<f32>>)> = Vec::with_capacity(self.tail.len());
        let mut seqs = Vec::with_capacity(self.tail.len());
        for (i, (seq, doc)) in self.tail.iter().enumerate() {
            seqs.push(*seq);
            docs.push((i as u32, doc.clone()));
        }
        if docs.is_empty() {
            return;
        }
        let mut index = index::smve::SmveIndex::new(cfg.width, cfg.topk, self.dims as u32);
        if index.build_from_column(&docs).is_ok() {
            self.smve = Some(index);
            self.smve_seqs = seqs;
        }
    }

    /// C++: `EMVBIndexInMem` over the memory tail (`build_index_threshold_`).
    ///
    /// Not called: the tail is sealed right after it is filled, and the chunk
    /// writer serializes the raw tokens itself — building the memory-side EMVB
    /// first cost 1.0-1.5 s of a 20k-row create and its result was dropped by
    /// the seal (`MultiVectorIndex::seal`). Kept as the reference's shape for
    /// when a query path reads the tail index directly.
    #[allow(dead_code)]
    pub(crate) fn build_emvb_tail(&self) {
        let tail_embeddings: usize = self.tail.values().map(Vec::len).sum();
        if tail_embeddings < emvb_build_threshold() {
            return;
        }
        let mut guard = self.tail_emvb.lock().unwrap();
        if guard
            .as_ref()
            .map(|t| t.generation != self.tail_generation)
            .unwrap_or(true)
        {
            let mut docs: Vec<(u32, &[Vec<f32>])> = Vec::with_capacity(self.tail.len());
            let mut seqs = Vec::with_capacity(self.tail.len());
            for (i, (seq, doc)) in self.tail.iter().enumerate() {
                docs.push((i as u32, doc.as_slice()));
                seqs.push(*seq);
            }
            let (subspace_num, subspace_bits) = match self.emvb_config {
                Some(cfg) => (cfg.subspace_num, cfg.subspace_bits),
                // No `USING EMVB` definition: keep the inference `build_emvb`
                // makes for a bare call.
                None => (if self.dims.is_multiple_of(8) { 8 } else { 1 }, 8),
            };
            let data = storage::emvb_segment::build_emvb_with(
                self.dims,
                0,
                &docs,
                subspace_num,
                subspace_bits,
            );
            *guard = Some(MvTailEmvb {
                generation: self.tail_generation,
                data,
                seqs,
            });
        }
    }

    /// Dumps the memory tail into per-segment files (one `ROW_ID` segment per
    /// file, the reference's chunk dump) and keeps their mmap readers.
    pub(crate) fn seal(&mut self) {
        if self.tail.is_empty() {
            return;
        }
        // The reference's chunk is the memory index it dumps, so the two that
        // live in memory (SMVE, and EMVB over the tail) have to be built from
        // these documents **before** the tail is taken - the chunk writer
        // serializes them, it does not rebuild from the rows.
        // Nothing here builds the memory-side indexes: the tail is taken below
        // and dumped to segment files, and after that the search path reads
        // `self.sealed` only. Building them first is work whose result is
        // thrown away one line later (`build_emvb_tail` cost 1.0-1.5 s of a
        // 20k-row create, and `build_smve` another 4.2 s).
        #[allow(clippy::type_complexity)]
        let mut by_segment: std::collections::BTreeMap<u64, Vec<(u64, Vec<Vec<f32>>)>> =
            Default::default();
        for (seq, doc) in std::mem::take(&mut self.tail) {
            by_segment.entry(seq >> 32).or_default().push((seq, doc));
        }
        self.tail_bytes = 0;
        self.tail_generation += 1;
        for (segment, mut rows) in by_segment {
            rows.sort_by_key(|(seq, _)| *seq);
            let ids: Vec<u64> = rows.iter().map(|(seq, _)| *seq).collect();
            let docs: Vec<Vec<Vec<f32>>> = rows.into_iter().map(|(_, doc)| doc).collect();
            let name = format!("seg_{segment}_{}_{}.mvec", self.next_chunk, docs.len());
            self.next_chunk += 1;
            let path = self.dir.join(name);
            if storage::multivector_segment::write_multivector_segment(&path, self.dims, &docs)
                .is_err()
            {
                continue;
            }
            // The reference's EMVB chunk (coarse centroids, document tables and
            // inverted lists) lives beside the raw-token segment. It is what
            // `SEARCH MATCH TENSOR` prefilters through before scoring the
            // survivors from the raw tokens - but only when the query reaches
            // that path: a PLAID or SMVE index answers from its own structures
            // and returns before the EMVB chunks are consulted, so building one
            // for such a column was 1.5 s of a 20k-row create that no query
            // could read. Build it when nothing more specific will answer.
            let pq = if self.wants_emvb_chunk() {
                // Borrow the token rows so sealing does not duplicate the
                // vectors.
                let emvb: Vec<(u32, &[Vec<f32>])> = docs
                    .iter()
                    .zip(ids.iter())
                    .map(|(doc, seq)| ((*seq & 0xffff_ffff) as u32, doc.as_slice()))
                    .collect();
                let (subspace_num, subspace_bits) = match self.emvb_config {
                    Some(cfg) => (cfg.subspace_num, cfg.subspace_bits),
                    // No `USING EMVB` definition: the inference `build_emvb`
                    // makes for a column without one.
                    None => (if self.dims.is_multiple_of(8) { 8 } else { 1 }, 8),
                };
                let _ = storage::emvb_segment::write_emvb_segment_refs_with(
                    &path.with_extension("emvb"),
                    self.dims,
                    (segment & 0xffff_ffff) as u32,
                    &emvb,
                    subspace_num,
                    subspace_bits,
                );
                storage::emvb_segment::EmvbReader::open(&path.with_extension("emvb"))
                    .ok()
                    .and_then(|r| r.pq_index())
                    .map(Arc::new)
            } else {
                None
            };
            if let Ok(reader) = storage::multivector_segment::MultiVectorSegmentReader::open(&path)
            {
                self.sealed.push((Arc::new(reader), Arc::new(ids), pq));
            }
        }
    }

    /// Points the sealed scan at the chunks a previous process wrote, without
    /// writing anything. [`Self::seal`] writes the chunks and then keeps their
    /// readers; a reload (a PLAID chunk opened on `Engine::open`) needs only the
    /// second half, and it matters for correctness: [`Self::search`] falls
    /// through to the sealed scan when the PLAID candidates are all excluded by
    /// a filter, which is exactly what an empty sealed set would turn into an
    /// empty answer.
    ///
    /// `ids` is the live, parseable row list in document order - the same list
    /// the chunk writer used (`column_docs`), split per segment.
    pub(crate) fn load_sealed(&mut self, ids: &[u64]) {
        let Ok(entries) = std::fs::read_dir(&self.dir) else {
            return;
        };
        let mut chunks: Vec<(u64, std::path::PathBuf)> = entries
            .flatten()
            .filter_map(|entry| {
                let path = entry.path();
                if path.extension().is_none_or(|x| x != "mvec") {
                    return None;
                }
                // `seg_<segment>_<chunk>_<rows>.mvec`, the name `seal` writes.
                let stem = path.file_stem()?.to_str()?;
                let segment = stem.strip_prefix("seg_")?.split('_').next()?.parse().ok()?;
                Some((segment, path))
            })
            .collect();
        chunks.sort();
        for (segment, path) in chunks {
            let Ok(reader) = storage::multivector_segment::MultiVectorSegmentReader::open(&path)
            else {
                continue;
            };
            let segment_ids: Vec<u64> = ids
                .iter()
                .copied()
                .filter(|seq| *seq >> 32 == segment)
                .collect();
            // A chunk whose document count no longer matches the live rows was
            // written before a delete; the PLAID rebuild will rewrite it.
            if reader.doc_count() != segment_ids.len() {
                continue;
            }
            self.sealed
                .push((Arc::new(reader), Arc::new(segment_ids), None));
        }
    }

    /// Top-k over sealed chunks and the memory tail, skipping tombstoned rows.
    pub(crate) fn search(
        &self,
        query: &[Vec<f32>],
        k: usize,
        dead: &std::collections::HashSet<u64>,
        exact: Option<&dyn Fn(u64) -> Option<f32>>,
        plaid: &PlaidSearchOptions,
        allowed: Option<&std::collections::HashSet<u64>>,
    ) -> Vec<(u64, f32)> {
        // A built PLAID index answers the query directly: IVF probing, an
        // approximate pass over the candidates and an exact residual rerank
        // (the reference's `PlaidIndex::SearchWithBitmask`). Deleted rows are
        // excluded by the bitmask.
        if let Some(index) = &self.plaid {
            if index.doc_num() as usize == self.plaid_seqs.len() {
                let mut mask = index::plaid::Bitmask::new(index.doc_num() as usize);
                for (doc, seq) in self.plaid_seqs.iter().enumerate() {
                    // `CalculateFilterBitmask` + the tombstone mask: only rows the
                    // statement admits take part in the search.
                    if !dead.contains(seq) && allowed.is_none_or(|set| set.contains(seq)) {
                        mask.set(doc as u32);
                    }
                }
                let flat: Vec<f32> = query.iter().flatten().copied().collect();
                let topn = k.max(1);
                let result = index.search_with_bitmask(
                    &flat,
                    query.len(),
                    topn,
                    &mask,
                    plaid.n_ivf_probe,
                    plaid.centroid_score_threshold,
                    plaid.n_doc_to_score.max(topn),
                    plaid
                        .n_full_scores
                        .clamp(topn, plaid.n_doc_to_score.max(topn)),
                );
                let mut hits: Vec<(u64, f32)> = result
                    .ids
                    .iter()
                    .zip(&result.scores)
                    .filter_map(|(id, score)| {
                        self.plaid_seqs.get(*id as usize).map(|seq| (*seq, *score))
                    })
                    .collect();
                if !hits.is_empty() {
                    hits.sort_by(|a, b| {
                        b.1.partial_cmp(&a.1)
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.0.cmp(&b.0))
                    });
                    hits.truncate(k);
                    return hits;
                }
            }
        }
        // `SMVEIndexInMem::Search`: BMP over the sparse codes yields candidates
        // that the reference re-ranks with exact MaxSim from the column; the
        // `exact` callback supplies that score here.
        if let Some(index) = &self.smve {
            if index.row_count() == self.smve_seqs.len() {
                let permitted: Vec<bool> = self
                    .smve_seqs
                    .iter()
                    .map(|seq| !dead.contains(seq) && allowed.is_none_or(|set| set.contains(seq)))
                    .collect();
                let (scores, docs) = index.search(query, k.max(1), 5, Some(&permitted));
                let mut hits: Vec<(u64, f32)> = docs
                    .iter()
                    .zip(&scores)
                    .filter_map(|(doc, score)| {
                        self.smve_seqs.get(*doc as usize).map(|seq| {
                            let rank = exact.and_then(|f| f(*seq)).unwrap_or(*score);
                            (*seq, rank)
                        })
                    })
                    .filter(|(seq, _)| !dead.contains(seq))
                    .collect();
                if !hits.is_empty() {
                    hits.sort_by(|a, b| {
                        b.1.partial_cmp(&a.1)
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.0.cmp(&b.0))
                    });
                    hits.truncate(k);
                    return hits;
                }
            }
        }
        let mut hits: Vec<(u64, f32)> = Vec::new();
        for (reader, ids, pq) in &self.sealed {
            // The persisted residual codes rank the documents (EMVB stage 2);
            // the survivors are re-scored exactly from the raw tokens.
            let docs: Vec<usize> = match pq {
                Some(pq) if reader.doc_count() > 256 => {
                    pq.prefilter(query, (k * 4).max(64).min(reader.doc_count()))
                }
                _ => (0..reader.doc_count()).collect(),
            };
            for doc in docs {
                let Some(seq) = ids.get(doc) else { continue };
                if dead.contains(seq) || allowed.is_some_and(|set| !set.contains(seq)) {
                    continue;
                }
                hits.push((*seq, reader.maxsim(query, doc)));
            }
        }
        // The memory segment: once the tail holds enough embeddings the
        // reference builds an in-memory `EMVBIndex` and searches it; the
        // survivors are re-scored exactly from the raw tokens.
        let tail_embeddings: usize = self.tail.values().map(Vec::len).sum();
        if tail_embeddings >= emvb_build_threshold() {
            let mut guard = self.tail_emvb.lock().unwrap();
            if guard
                .as_ref()
                .map(|t| t.generation != self.tail_generation)
                .unwrap_or(true)
            {
                let mut docs: Vec<(u32, &[Vec<f32>])> = Vec::with_capacity(self.tail.len());
                let mut seqs = Vec::with_capacity(self.tail.len());
                for (i, (seq, doc)) in self.tail.iter().enumerate() {
                    docs.push((i as u32, doc.as_slice()));
                    seqs.push(*seq);
                }
                let data = storage::emvb_segment::build_emvb(self.dims, 0, &docs);
                *guard = Some(MvTailEmvb {
                    generation: self.tail_generation,
                    data,
                    seqs,
                });
            }
            if let Some(tail) = guard.as_ref() {
                for (doc, _) in tail.data.view().search(
                    query,
                    (k + dead.len()).max(1),
                    index::emvb::EmvbSearchOptions::for_topk(k.max(1)),
                ) {
                    let Some(seq) = tail.seqs.get(doc as usize) else {
                        continue;
                    };
                    if dead.contains(seq) || allowed.is_some_and(|set| !set.contains(seq)) {
                        continue;
                    }
                    if let Some(tokens) = self.tail.get(seq) {
                        hits.push((*seq, maxsim_tokens(query, tokens)));
                    }
                }
            }
            drop(guard);
        } else {
            for (seq, doc) in &self.tail {
                if dead.contains(seq)
                    || doc.is_empty()
                    || allowed.is_some_and(|set| !set.contains(seq))
                {
                    continue;
                }
                hits.push((*seq, maxsim_tokens(query, doc)));
            }
        }
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits
    }
}

/// One sparse (BMP) index in the reference's two forms: sealed `seg_*.bmp`
/// chunks written with `BMPAlg::SaveToPtr`'s layout, and the memory tail.
///
/// The `SPARSE … WITH (SORTED)` hint is recorded here, but this engine's BMP
/// writer has a single build path: the hint does not change the chunk bytes.
/// It survives a restart because it is re-read from the catalog
/// (`TableMeta::sparse_stores`) when the index is rebuilt.
pub(crate) struct SparseIndex {
    pub(crate) width: storage::bmp::IdxWidth,
    /// `WITH (block_size = N)`: documents per `BlockFwd` block.
    pub(crate) block_size: usize,
    /// The catalog's `SparseStoreType` for this column.
    pub(crate) store: CatalogSparseStore,
    pub(crate) dir: PathBuf,
    /// The term space declared by the column type (`SPARSE(elem,index,bound)`),
    /// or 0 when the column declares no bound. The reference sizes the BMP
    /// inverted table by the declared bound (`BMPAlg` is constructed with the
    /// column's dimension), not by the distinct terms a chunk happens to hold,
    /// and its reader walks the file without a length field: a term count that
    /// is short of the reference's shifts every following section.
    pub(crate) terms: usize,
    /// `ROW_ID -> (term, weight)` for documents not yet dumped.
    pub(crate) tail: std::collections::BTreeMap<u64, Vec<(i32, f32)>>,
    /// One mmap reader per dumped chunk, with the `ROW_ID` segment it covers.
    pub(crate) sealed: Vec<(u64, Arc<storage::bmp::BmpReader>)>,
    /// Monotonic suffix so a later seal never overwrites an earlier spill.
    pub(crate) next_chunk: u64,
    pub(crate) tail_bytes: usize,
    /// Bumped on every tail mutation; the in-memory BMP index below is rebuilt
    /// when it lags (the reference's `BMPAlg` held by the mem index).
    pub(crate) tail_generation: u64,
    pub(crate) tail_index: std::sync::Mutex<Option<SparseTailBmp>>,
    /// Rows this index has *seen*, whether or not they contributed postings.
    ///
    /// A NULL (or unparseable) sparse cell writes no term, so `doc_count` is
    /// always short of the table's row count on such a table - and a coverage
    /// check based on it never agrees, which rebuilt the whole index on every
    /// query. This counter is the one the check reads: the rebuild sets it to
    /// the rows it scanned (NULL included) and the write hook bumps it per row
    /// it is handed, so it can only run ahead of the truth, never behind it.
    pub(crate) scanned_rows: usize,
}

/// The in-memory `BMPAlg` over the sparse tail, plus the `ROW_ID` of each
/// indexed document (`BMPAlg` documents are dense ids).
pub(crate) struct SparseTailBmp {
    pub(crate) generation: u64,
    pub(crate) alg: index::sparse::BmpAlg,
    pub(crate) seqs: Vec<u64>,
}

/// C++ `BMPUtil::ParseBmpSearchOptions`: the per-query `MATCH SPARSE` options,
/// from the option string and the SQL `WITH (…)` clause (the latter wins, like
/// `KnnScanOptions::parse`). An `alpha`/`beta` outside `[0, 1]` is ignored,
/// exactly like the reference's warnings do.
///
/// The reference only reads `alpha`, `beta`, `use_tail` and `use_lock` here,
/// while its binder admits `alpha`, `beta`, `tail` and `threshold`: `tail` and
/// `threshold` therefore never change `BmpSearchOptions` (`tail` is a dead name
/// in the reference, `threshold` filters results elsewhere), and `use_tail` is
/// rejected before it gets here.
pub(crate) fn parse_bmp_search_options(
    with_opts: Option<&str>,
    params: &str,
) -> index::sparse::BmpSearchOptions {
    let mut options = index::sparse::BmpSearchOptions::default();
    let unit = |value: &str| -> Option<f32> {
        value
            .trim()
            .parse::<f32>()
            .ok()
            .filter(|v| (0.0..=1.0).contains(v))
    };
    for source in [Some(params), with_opts].into_iter().flatten() {
        // The SQL `WITH (…)` clause is comma-separated; the SDK's option
        // string is semicolon-separated (see `knn_scan::option`).
        for part in source.split([';', ',']) {
            let Some((key, value)) = part.split_once('=') else {
                continue;
            };
            if key.trim().eq_ignore_ascii_case("alpha") {
                if let Some(alpha) = unit(value) {
                    options.alpha = alpha;
                }
            } else if key.trim().eq_ignore_ascii_case("beta") {
                if let Some(beta) = unit(value) {
                    options.beta = beta;
                }
            }
        }
    }
    options
}

impl SparseIndex {
    pub(crate) fn new(
        dir: PathBuf,
        width: storage::bmp::IdxWidth,
        store: CatalogSparseStore,
        block_size: usize,
    ) -> Self {
        let next_chunk = std::fs::read_dir(&dir)
            .map(|d| {
                d.filter_map(|e| e.ok())
                    .filter(|e| e.path().extension().is_some_and(|x| x == "bmp"))
                    .count() as u64
            })
            .unwrap_or(0);
        Self {
            width,
            block_size,
            store,
            dir,
            terms: 0,
            tail: Default::default(),
            sealed: Vec::new(),
            next_chunk,
            tail_bytes: 0,
            tail_generation: 0,
            tail_index: std::sync::Mutex::new(None),
            scanned_rows: 0,
        }
    }

    /// Whether this index covers every row of a table with `live_rows` rows.
    ///
    /// `doc_count` cannot answer that: a NULL sparse cell has no postings, so a
    /// table holding one would look incomplete forever and be rebuilt on every
    /// query. The counter the rebuild and the write hook maintain can.
    pub(crate) fn is_complete(&self, live_rows: usize) -> bool {
        self.scanned_rows >= live_rows
    }

    /// Records that the write hook handed this index one row, whether or not
    /// the row had postings to add.
    pub(crate) fn note_row(&mut self) {
        self.scanned_rows += 1;
    }

    pub(crate) fn insert(&mut self, seq: u64, doc: Vec<(i32, f32)>) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(sparse_doc_bytes(&old));
        }
        self.tail_bytes += sparse_doc_bytes(&doc);
        self.tail.insert(seq, doc);
        self.tail_generation += 1;
    }

    pub(crate) fn remove(&mut self, seq: u64) {
        if let Some(old) = self.tail.remove(&seq) {
            self.tail_bytes = self.tail_bytes.saturating_sub(sparse_doc_bytes(&old));
            self.tail_generation += 1;
        }
    }

    pub(crate) fn doc_count(&self) -> usize {
        self.tail.len()
            + self
                .sealed
                .iter()
                .map(|(_, r)| r.doc_count())
                .sum::<usize>()
    }

    /// Dumps the memory tail into one BMP file per `ROW_ID` segment per seal.
    ///
    /// The file is *not* the published chunk: the reference's chunk is the
    /// block and it is extended by every dump that touches it (a 100 + 100 row
    /// insert into one block leaves **one** chunk of 200 rows), so the publish
    /// step rebuilds one chunk per block from the rows.
    pub(crate) fn seal(&mut self) {
        if self.tail.is_empty() {
            return;
        }
        #[allow(clippy::type_complexity)]
        let mut by_segment: std::collections::BTreeMap<u64, Vec<(u64, Vec<(i32, f32)>)>> =
            Default::default();
        for (seq, doc) in std::mem::take(&mut self.tail) {
            by_segment
                .entry(seq >> 32)
                .or_default()
                .push((seq & 0xffff_ffff, doc));
        }
        self.tail_bytes = 0;
        self.tail_generation += 1;
        for (segment, mut docs) in by_segment {
            docs.sort_by_key(|(row, _)| *row);
            let term_num = (docs
                .iter()
                .flat_map(|(_, e)| e.iter().map(|(t, _)| *t + 1))
                .max()
                .unwrap_or(0)
                .max(1) as usize)
                .max(self.terms);
            let name = format!("seg_{segment}_{}_{}.bmp", self.next_chunk, docs.len());
            self.next_chunk += 1;
            let path = self.dir.join(name);
            if storage::bmp::write_bmp_file(&path, self.block_size, self.width, &docs, term_num)
                .is_err()
            {
                continue;
            }
            if let Ok(reader) = storage::bmp::BmpReader::open(&path, self.width) {
                self.sealed.push((segment, Arc::new(reader)));
            }
        }
    }

    /// Top-k over sealed chunks and the memory tail, skipping tombstones.
    /// `options` is the query's `alpha` / `beta` / `use_tail`
    /// (`BMPUtil::ParseBmpSearchOptions`).
    pub(crate) fn search(
        &self,
        query: &[(i32, f32)],
        k: usize,
        dead: &std::collections::HashSet<u64>,
        options: index::sparse::BmpSearchOptions,
    ) -> Vec<(u64, f32)> {
        let mut hits: Vec<(u64, f32)> = Vec::new();
        for (segment, reader) in &self.sealed {
            // Ask for enough candidates that tombstoned rows cannot starve the
            // top-k; `search` clamps to the chunk's document count. The on-disk
            // reader runs the same `BMPAlgBase::SearchKnn` (`alpha`, `beta`,
            // `use_tail`) as the in-memory one.
            for (row, score) in reader.search(query, k + dead.len(), options) {
                let seq = (segment << 32) | u64::from(row as u32);
                if !dead.contains(&seq) {
                    hits.push((seq, score));
                }
            }
        }
        // The memory segment is the reference's in-memory `BMPAlg`: rebuild it
        // when the tail changed since the last query.
        let qv: Vec<(u32, f32)> = query.iter().map(|(t, v)| (*t as u32, *v)).collect();
        let mut guard = self.tail_index.lock().unwrap();
        if guard
            .as_ref()
            .map(|t| t.generation != self.tail_generation)
            .unwrap_or(true)
        {
            let mut alg = index::sparse::BmpAlg::new(index::sparse::BMP_BLOCK_SIZE);
            let mut seqs = Vec::with_capacity(self.tail.len());
            for (seq, doc) in &self.tail {
                let doc32: Vec<(u32, f32)> = doc.iter().map(|(t, v)| (*t as u32, *v)).collect();
                alg.add_doc(&doc32);
                seqs.push(*seq);
            }
            alg.build();
            *guard = Some(SparseTailBmp {
                generation: self.tail_generation,
                alg,
                seqs,
            });
        }
        if let Some(tail) = guard.as_ref() {
            // The memory index is a `BMPAlg` too, so `use_tail`/`beta` apply to
            // its own unsealed tail exactly as `bmp_search` does in the
            // reference.
            for (doc, score) in tail.alg.search_knn(&qv, k + dead.len(), options) {
                if let Some(seq) = tail.seqs.get(doc) {
                    if !dead.contains(seq) {
                        hits.push((*seq, score));
                    }
                }
            }
        }
        drop(guard);
        hits.sort_by(|a, b| {
            b.1.partial_cmp(&a.1)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.0.cmp(&b.0))
        });
        hits.truncate(k);
        hits
    }
}

/// The storage layer: the catalog and the KV store.
///
/// C++: `Storage` (`src/storage/storage.cppm`) — the owner of databases,
/// tables and their metadata. The `Engine` composes it.
pub struct Storage {
    pub(crate) catalog: Catalog,
    /// Serializes checkpoints against index dumps: the block mirror rewrites
    /// `seg_0` and the index chunks, so two checkpoints would race on their
    /// files, and a dump must not publish while one runs.
    ///
    /// A checkpoint takes it for **writing**, a `DumpMemIndex` for **reading**:
    /// a bulk FLUSH has many independent indexes to seal, and serialising them
    /// behind one mutex made the wall time grow with the table count.
    pub(crate) flush_lock: tokio::sync::RwLock<()>,
    /// The lifecycle mode (`Storage::current_storage_mode_`).
    pub(crate) mode: Mutex<StorageMode>,
}

/// `Storage`'s lifecycle mode.
///
/// C++: `enum class StorageMode` (`src/storage/wal/wal_manager.cppm:33-38`),
/// driven by `Storage::SetStorageMode` (`src/storage/storage_impl.cpp:805-877`).
///
/// In the reference every transition starts or stops subsystems: `InitToAdmin`
/// builds the WAL manager and the persistence layer, `AdminToReaderBottom` /
/// `AdminToWriter` build the buffer manager, the catalog, the transaction
/// manager, the memory-index tracer and the four background processors,
/// `AttachCatalog(ts)` opens a catalog snapshot bounded by a checkpoint
/// timestamp, and `RecoverMemIndex` reloads the memory indexes. This port does
/// all of that once, inside [`Engine::open_with_options`], and tears it down
/// with the `Engine`; there is no checkpoint-timestamp catalog view because the
/// catalog is read live from the `MetaStore`. Only the part a client can
/// observe is modelled here: which mode the storage is in, which transitions
/// are legal, and whether a statement may write. `storage_mode_ut.rs` records
/// this as a deliberate deviation.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum StorageMode {
    /// `kUnInitialized`: nothing is open. A mutating statement is fatal, as
    /// the reference's `UnrecoverableError("Uninitialized storage mode")` is.
    UnInitialized,
    /// `kAdmin`: a cluster-management node. Reads work, user writes do not.
    Admin,
    /// `kReadable`: a follower/learner replica. Reads and cleanup run, user
    /// writes do not.
    Readable,
    /// `kWritable`: a standalone or leader node.
    Writable,
}

impl StorageMode {
    /// Whether a user write (`INSERT`/`UPDATE`/`DELETE`/`IMPORT`/DDL/
    /// `FLUSH`/`DUMP INDEX`) is allowed. The reference's operators and its
    /// checkpoint task all require exactly `kWritable`
    /// (`physical_insert_impl.cpp:117`, `background_process_impl.cpp:82`).
    pub fn accepts_writes(self) -> bool {
        matches!(self, StorageMode::Writable)
    }

    /// Whether `CLEANUP` is allowed. The reference's cleanup task runs in
    /// `kWritable` or `kReadable` (`background_process_impl.cpp:109`).
    pub fn accepts_cleanup(self) -> bool {
        matches!(self, StorageMode::Writable | StorageMode::Readable)
    }
}

impl Storage {
    /// C++ `Storage::GetStorageMode` (`storage_impl.cpp:800-803`).
    pub fn get_storage_mode(&self) -> StorageMode {
        *self.mode.lock().unwrap()
    }

    /// C++ `Storage::SetStorageMode` (`storage_impl.cpp:805-877`).
    ///
    /// The transition table is the reference's: `kUnInitialized` only moves to
    /// `kAdmin`; `kAdmin` reaches everything else; `kReadable`/`kWritable`
    /// step back to `kAdmin`, drop to `kUnInitialized`, or swap with each
    /// other. Setting the current mode again is a no-op (`Set unchanged
    /// mode`). The reference calls `UnrecoverableError` on an illegal
    /// transition; this port returns the same message as an error so an admin
    /// RPC can report it instead of aborting the process.
    pub fn set_storage_mode(&self, target: StorageMode) -> Result<()> {
        let mut mode = self.mode.lock().unwrap();
        if *mode == target {
            return Ok(());
        }
        let legal = matches!(
            (*mode, target),
            (StorageMode::UnInitialized, StorageMode::Admin)
                | (
                    StorageMode::Admin,
                    StorageMode::UnInitialized | StorageMode::Readable | StorageMode::Writable
                )
                | (
                    StorageMode::Readable,
                    StorageMode::UnInitialized | StorageMode::Admin | StorageMode::Writable
                )
                | (
                    StorageMode::Writable,
                    StorageMode::UnInitialized | StorageMode::Admin | StorageMode::Readable
                )
        );
        if !legal {
            return Err(Error::Unrecoverable(format!(
                "Attempt to set storage mode from {mode:?} to {target:?}"
            )));
        }
        *mode = target;
        Ok(())
    }
}

/// Sealed high-cardinality chunk readers with their base row id.
pub(crate) type SecondaryDiskChunks =
    Vec<(u64, Arc<storage::secondary_segment::SecondarySegmentReader>)>;
/// Sealed low-cardinality (boolean) chunk readers with their base row id.
pub(crate) type SecondaryLowChunks = Vec<(u64, Arc<storage::secondary_lowcard::LowCardReader>)>;

/// Secondary-index registry of one table (the reference's index metas under
/// `src/storage/catalog/meta/` plus the sealed chunk readers), keyed by column.
#[derive(Default)]
pub(crate) struct SecondaryIndexes {
    /// Memory tails: rows written since the last chunk dump.
    ///
    /// Shared behind an `Arc` so a scan or a bulk insert can hold one column's
    /// tail across the table lock, and so the tail's own RCU handles reader /
    /// writer overlap (see `index::SecondaryIndex`).
    pub(crate) indexes: HashMap<String, Arc<SecondaryIndex>>,
    /// The declared type of each indexed column, which fixes its ordered key
    /// domain (`ConvertToOrderedType`): a tail key is the value's
    /// order-preserving encoding, so a range scan can slice it. Recorded when
    /// the index is created and read back when the tail is dumped into a chunk.
    pub(crate) declared_types: HashMap<String, String>,
    /// Low-cardinality (boolean) chunk readers with their base row id.
    pub(crate) low: HashMap<String, SecondaryLowChunks>,
    /// High-cardinality (integer/varchar) PGM chunk readers.
    pub(crate) disk: HashMap<String, SecondaryDiskChunks>,
    /// Dumped chunk metadata per column: `(base row id, row count)` — the
    /// `ChunkIndexMetaInfo` the reference stores next to each chunk.
    pub(crate) chunk_meta: HashMap<String, Vec<(u64, usize)>>,
    /// Columns whose memory map is a tail backed by on-disk chunks.
    pub(crate) disk_backed: std::collections::HashSet<String>,
    /// Functional indexes of this table, keyed by the expression the catalog
    /// stores as the index's column (`lower(name)`).
    pub(crate) functional: HashMap<String, FunctionalIndex>,
    /// Tails a dump has frozen but whose chunks are not registered yet.
    ///
    /// `SecondaryIndex::seal` empties the tail, and the chunk that holds its
    /// entries only becomes visible a few writes later. A reader landing in that
    /// window would see neither copy, so the dump keeps the frozen entries here
    /// and readers merge them until the chunk is registered.
    pub(crate) sealing: HashMap<String, Vec<std::sync::Arc<index::SealedTail>>>,
}

impl TableEntry {
    /// Appends rows to the append buffer, keeping it **ordered by row id** and
    /// `pending_pos` current.
    ///
    /// The block writer, the range scans and the checkpoint all walk
    /// `pending_rows` as the table's row order, so a batch that lands behind
    /// rows already buffered (two commits whose bottoms finished out of order,
    /// now possible because the insert path no longer serialises its commits)
    /// is merged in at its place instead of appended. Rows normally arrive after
    /// everything buffered, so the fast path is still a push.
    pub(crate) fn buffer_rows(&mut self, rows: impl IntoIterator<Item = (usize, Vec<Value>)>) {
        let mut batch: Vec<(usize, Vec<Value>)> = rows.into_iter().collect();
        if batch.is_empty() {
            return;
        }
        // The map is kept in step on every path. Leaving it alone on an append
        // was worth ~8 s on an 8 M-row `COPY`, but the buffer has to stay both
        // sorted and addressable under concurrent writers, and the append fast
        // path could not promise the first: two writers interleaving their
        // commits hand over a batch whose head is past the buffer's last id
        // and whose tail is not, and extending it left `pending_rows` unsorted.
        // A binary search over that answers "absent" for rows that are there.
        if self
            .pending_rows
            .last()
            .is_none_or(|(last, _)| *last < batch[0].0)
        {
            self.pending_rows.reserve(batch.len());
            let mut bytes = 0usize;
            for (seq, values) in batch {
                bytes += Self::row_bytes(&values);
                self.pending_pos.insert(seq, self.pending_rows.len());
                self.pending_rows.push((seq, values));
            }
            self.pending_bytes += bytes;
            return;
        }
        batch.sort_by_key(|(seq, _)| *seq);
        let mut bytes = 0usize;
        for (seq, values) in batch {
            bytes += Self::row_bytes(&values);
            let at = self
                .pending_rows
                .partition_point(|(buffered, _)| *buffered < seq);
            self.pending_rows.insert(at, (seq, values));
        }
        self.pending_bytes += bytes;
        self.pending_pos.clear();
        for (at, (seq, _)) in self.pending_rows.iter().enumerate() {
            self.pending_pos.insert(*seq, at);
        }
    }

    /// Drops the buffered rows `keep` rejects, keeping `pending_pos` current.
    pub(crate) fn retain_buffered(&mut self, mut keep: impl FnMut(&(usize, Vec<Value>)) -> bool) {
        self.pending_rows.retain(|row| keep(row));
        if self.pending_rows.is_empty() {
            // A full drain: `retain` and `clear` keep their backing storage, so
            // the emptied buffer would still hold `capacity` row slots and
            // `capacity` map buckets - memory the quota has just stopped
            // charging. Release it with the rows.
            self.pending_rows = Vec::new();
            self.pending_pos = std::collections::HashMap::new();
            self.pending_bytes = 0;
            return;
        }
        self.pending_pos.clear();
        // Rebuilt here rather than adjusted by the rows dropped: the walk is
        // one pass over what is left, which is what `retain` and the map
        // rebuild below already cost, and it cannot drift from the buffer.
        self.pending_bytes = self
            .pending_rows
            .iter()
            .map(|(_, values)| Self::row_bytes(values))
            .sum();
        for (at, (seq, _)) in self.pending_rows.iter().enumerate() {
            self.pending_pos.insert(*seq, at);
        }
    }

    /// The memory the unsealed rows of this table hold.
    ///
    /// The reference keeps them in the block's `BufferObject`, which its buffer
    /// manager accounts for and evicts under pressure. The port has no buffer
    /// manager, so this is the number that stands in for it: the rows
    /// themselves (a `Value` per cell, plus the `String` a text cell owns) and
    /// the `seq -> position` map that indexes them.
    ///
    /// A running total, not a sum over `pending_rows`: the quota reads it on
    /// the write path, where walking a million rows would cost more than the
    /// memory it reports. Approximate by design - it is a quota input, not a
    /// byte-exact report.
    pub(crate) fn unsealed_bytes(&self) -> usize {
        // Key, value and the control byte a `HashMap` entry carries, doubled to
        // cover the slack the table keeps for its load factor.
        self.pending_bytes + self.pending_pos.len() * (std::mem::size_of::<usize>() * 2 + 1) * 2
    }

    /// What one buffered row costs, for the running total.
    fn row_bytes(values: &[Value]) -> usize {
        let mut bytes = std::mem::size_of::<(usize, Vec<Value>)>() + std::mem::size_of_val(values);
        for value in values {
            if let Value::Str(text) = value {
                bytes += text.len();
            }
        }
        bytes
    }

    /// The buffered row `seq`, if the commit that appended it is still above
    /// the last checkpoint.
    pub(crate) fn buffered_row(&self, seq: usize) -> Option<&Vec<Value>> {
        // The map is only a fast path, and it is allowed to be stale: an append
        // does not update it (a bulk `COPY` appends and never looks a row up),
        // so every position past the append point is off. A hit is therefore
        // only trusted when the row at that position really is `seq`.
        if let Some(at) = self.pending_pos.get(&seq) {
            if let Some((buffered, values)) = self.pending_rows.get(*at) {
                if *buffered == seq {
                    return Some(values);
                }
            }
        }
        // The buffer is sorted by id, so the search answers every case: a miss
        // in the map, and a position the map has not caught up with. A linear
        // fallback covers a buffer that lost its order anyway (an append that
        // raced another writer): answering "absent" for a row that is there
        // loses it from the reader, which is worse than the scan.
        let at = self
            .pending_rows
            .partition_point(|(buffered, _)| *buffered < seq);
        if let Some((buffered, values)) = self.pending_rows.get(at) {
            if *buffered == seq {
                return Some(values);
            }
        }
        self.pending_rows
            .iter()
            .find(|(buffered, _)| *buffered == seq)
            .map(|(_, values)| values)
    }
}

/// Per-table write bookkeeping and row-id allocation.
///
/// C++ counterpart: the runtime state behind `TableMeta` / `SegmentMeta`
/// (`src/storage/catalog/meta/`) plus the block map (`BlockIndex`,
/// `src/storage/common/block_index.cppm`). Upstream removed the older
/// `TableEntry` / `SegmentEntry` / `BlockManager` classes (PR #2722), so this
/// type stands in for their catalog-level successors, not for a single class.
#[derive(Default)]
pub(crate) struct TableEntry {
    /// Number of write batches (`COPY`/`INSERT`) seen by this process.
    pub(crate) write_batches: usize,
    /// Row ids the C++ block mirror was written from, in block order.
    ///
    /// The mirror is a snapshot: rows appended after it was written are read
    /// from the append buffer, and rows deleted since it was written are hidden
    /// by their tombstones (`TableEntry`'s block snapshot is the Rust stand-in
    /// for the reference's per-block row ranges + delete bitmap).
    pub(crate) block_rows: Option<Arc<Vec<usize>>>,
    /// Rows a commit appended that the block **files** do not have yet.
    ///
    /// The reference's `AppendInBlock` appends to the block's `BufferObject`
    /// in the buffer manager, and the block file is written by the checkpoint
    /// (`BufferManager::Flush`), not by the commit. This is that buffer: every
    /// row the commits since the last checkpoint appended, by row id. A read
    /// merges it over the blocks, a flush writes it out and empties it.
    ///
    /// One buffer per table, inside the engine's `tables` map: every reader
    /// (and every statement on every thread) sees the same unsealed rows, the
    /// way the reference's buffer object is shared by all of its handle
    /// holders. A sealed block needs no such buffer - it is mapped (see
    /// `Context::block_maps`).
    pub(crate) pending_rows: Vec<(usize, Vec<Value>)>,
    /// The memory `pending_rows` holds, kept as a running total.
    ///
    /// The quota reads this on every write statement, and summing a million
    /// rows' strings there would put an O(rows) walk on the insert path - the
    /// exact cost this number exists to get rid of. Both mutators adjust it by
    /// the rows they add or drop.
    pub(crate) pending_bytes: usize,
    /// Bytes this table's in-memory index forms held when the process-wide
    /// total was last updated for it (see `Context::memindex_total`).
    pub(crate) memindex_bytes: usize,
    /// `seq -> position in pending_rows`.
    ///
    /// A point read asks for one row at a time (`BlockCursor::row`), and
    /// scanning the buffer per call made a 5k-row `DELETE` quadratic against a
    /// 20k-row buffer (34% of the statement). Kept in step by
    /// [`Self::buffer_rows`] / [`Self::retain_buffered`].
    pub(crate) pending_pos: std::collections::HashMap<usize, usize>,
    /// `(segment, offset, sealed)` of the row-id allocator.
    pub(crate) row_counter: Option<(u32, u32, bool)>,
    /// Cumulative row count after every append transaction of this table
    /// (`appends:{table}`), loaded on first use.
    pub(crate) append_totals: Option<Vec<u64>>,
    /// Commit timestamp of every append, one entry per `append_totals` entry
    /// (`append_ts:{table}`). The block mirror writes it into the block's
    /// `version` file, which is where the reference records each append's
    /// commit ts.
    pub(crate) append_ts: Option<Vec<u64>>,
    /// `1` for a **bulk** append (`COPY`/`IMPORT`, or the single append a
    /// compaction writes), `0` for an `INSERT`/`UPDATE`, one entry per
    /// `append_totals` entry (`append_copy:{table}`). It decides the null
    /// bitmap's coverage of the block the append opened.
    pub(crate) append_copy: Option<Vec<u64>>,
    /// Row ids deleted since the table was created (`dellog:{table}`), loaded
    /// on first use.
    pub(crate) deleted_ids: Option<Vec<u64>>,
    /// [`Self::deleted_ids`] as a set, so a single row's tombstone is one hash
    /// lookup instead of a linear scan of the journal.
    ///
    /// A bulk `DELETE`/`UPDATE` asks `is_deleted` once per row, and the scan
    /// made that quadratic in the journal's length (a 5k-row statement against
    /// a 50k-entry journal walked 250M entries). Built lazily on the first
    /// lookup, extended by `note_deleted_ids`, dropped whenever the journal is
    /// rebuilt (`ensure_journals`).
    pub(crate) deleted_set: Option<Arc<std::collections::HashSet<u64>>>,
    /// Commit timestamp of every delete, one entry per `deleted_ids` entry
    /// (`delete_ts:{table}`).
    pub(crate) delete_ts: Option<Vec<u64>>,
    /// Segments allocated by the row-id allocator, by id.
    pub(crate) segments: std::collections::BTreeMap<u32, SegmentEntry>,
    /// Full-text analyzer per column (`CREATE INDEX … WITH (analyzer=…)`).
    pub(crate) analyzers: HashMap<String, String>,
    /// Import extent per sealed segment `(segment, rows, first create ts)`,
    /// rebuilt from the block `version` records and the segment's prefilter.
    pub(crate) imports: Option<Vec<(u32, u64, u64)>>,
    /// Analyzer per index name: two indexes on one column may differ.
    pub(crate) analyzers_by_index: HashMap<String, String>,
    /// Every full-text index of a column, index name → analyzer, sorted by
    /// index name (`SearchDriver::GetAnalyzerName` walks exactly this shape).
    pub(crate) column_index_analyzers: HashMap<String, std::collections::BTreeMap<String, String>>,
    /// `CREATE INDEX … COMMENT '…'` per index name.
    pub(crate) comments: HashMap<String, String>,
    /// HNSW memory segment per vector column (`MemIndex::hnsw_index_`).
    pub(crate) hnsw_mem: HashMap<String, Arc<Mutex<crate::hnsw_mem::HnswIndexInMem>>>,
    /// HNSW bytes buffered per column since its last dump (memindex quota).
    pub(crate) index_pending_bytes: HashMap<String, usize>,
    /// Whether this table's HNSW indexes were probed since the last dump.
    pub(crate) hnsw_probed: bool,
    /// Sparse (BMP) indexes per column.
    pub(crate) sparse: HashMap<String, Arc<std::sync::RwLock<SparseIndex>>>,
    /// Multi-vector indexes per column.
    pub(crate) mv: HashMap<String, Arc<std::sync::RwLock<MultiVectorIndex>>>,
    /// Full-text inverted indexes per column.
    pub(crate) text: HashMap<String, SharedTextIndex>,
    /// Secondary indexes per column with their sealed chunk readers.
    pub(crate) secondary: SecondaryIndexes,
    /// HNSW chunks loaded from the C++ block mirror, per column (the
    /// reference's `TableIndexMeta` chain for the index, on-disk form).
    pub(crate) vector: HashMap<String, Vec<Arc<crate::blocks::LoadedVectorIndex>>>,
    /// IVF chunks loaded from the C++ mirror, per column.
    pub(crate) ivf: HashMap<String, Vec<Arc<index::ivf::IvfSegment>>>,
    /// IVF memory segment: rows written since the last mirror dump, merged
    /// with the sealed chunks at query time.
    pub(crate) ivf_mem: HashMap<String, Vec<(u64, Vec<f32>)>>,
    /// Block map of the C++-format mirror (C++ `BlockIndex`): block ids, row
    /// ranges and the decoded-block LRU shared with `Context::buffer`.
    pub(crate) block_index: Option<Arc<Mutex<storage::block_index::BlockIndex>>>,
    /// Live row count, maintained incrementally so the per-batch mirror check
    /// does not rescan every key. `None` means "recompute on demand".
    pub(crate) live_rows: Option<usize>,
    /// The block mirror is **behind** these rows: a commit appended or deleted
    /// something since the files were written.
    ///
    /// The reference writes a block once per append batch and never rewrites it
    /// afterwards (`BlockColumn::Append` + `SaveToFile`); a checkpoint that
    /// re-encoded an unchanged table would change bytes it leaves alone - the
    /// NULL bitmap is sized by the append window, so a rewrite from the final
    /// row set writes a different one. Keep the same rule: only a table that
    /// moved is rewritten.
    pub(crate) mirror_dirty: bool,
    /// Counts every change a reader would have to merge out of the buffer:
    /// an append, a delete, a schema/row-id rewrite. A mirror write captures it
    /// before it reads the rows and only clears `mirror_dirty` when the value is
    /// unchanged - a write that lands during the flush stays visible.
    pub(crate) mirror_epoch: u64,
    /// Rows (deleted ones included) the block files held after the last mirror
    /// write. A commit is incremental only when this count is known and this
    /// batch is the only thing that moved since: the reference appends into the
    /// blocks it wrote and leaves every other block alone, so the previous
    /// count is what tells "everything else is already on disk" apart from
    /// "another transaction reserved ids that are not".
    pub(crate) mirror_rows: Option<u64>,
    /// Decoded rows of a **scalar-only** table (the reference's in-memory
    /// `DataBlock`s). Served by `fetch_rows_with_ids` while it covers every
    /// live row; any mutation that rewrites rows drops it.
    pub(crate) row_cache: Option<RowCache>,
}

/// One segment of a table: its id and the highest row offset ever allocated.
///
/// C++: `SegmentMeta` (`src/storage/catalog/meta/segment_meta.cppm`); the
/// removed `SegmentEntry` was its runtime counterpart.
pub(crate) struct SegmentEntry {
    pub(crate) id: u32,
    /// Number of rows in the segment (`SegmentEntry::row_count`).
    pub(crate) rows: u32,
}

/// A table's decoded rows, bounded by [`ROW_CACHE_BUDGET_BYTES`].
pub(crate) struct RowCache {
    pub(crate) rows: Vec<(usize, Vec<Value>)>,
    pub(crate) bytes: usize,
}

pub(crate) const ROW_CACHE_BUDGET_BYTES: usize = 64 * 1024 * 1024;

thread_local! {
    /// [`READ_TS`] for a **pool thread**.
    ///
    /// A decode dispatched to the blocking pool runs on a thread with no task
    /// context, so the task-local cannot be read there. `scheduler` copies it
    /// into this thread-local before running the job (the same shape as
    /// `POOL_SESSION`), and [`read_ts`] prefers the task-local when one is set.
    static POOL_READ_TS: std::cell::Cell<u64> = const { std::cell::Cell::new(u64::MAX) };
}

/// The timestamp the current statement reads at (`u64::MAX` outside one).
pub(crate) fn read_ts() -> u64 {
    match READ_TS.try_with(|ts| *ts) {
        Ok(ts) => ts,
        Err(_) => POOL_READ_TS.with(|cell| cell.get()),
    }
}

/// Sets [`POOL_READ_TS`] for the duration of `f` (a blocking-pool job).
pub(crate) fn with_pool_read_ts<T>(ts: u64, f: impl FnOnce() -> T) -> T {
    struct Reset(u64);
    impl Drop for Reset {
        fn drop(&mut self) {
            POOL_READ_TS.with(|c| c.set(self.0));
        }
    }
    let previous = POOL_READ_TS.with(|c| c.replace(ts));
    let _restore = Reset(previous);
    f()
}

/// Sets [`POOL_READ_TS`] for the rest of a **synchronous** call, restoring the
/// previous value on drop.
///
/// For the synchronous entry points (`select_columns`) and the tests, which run
/// a whole statement on one thread and cannot open a task-local scope there.
/// Inside an async statement the task-local is what [`read_ts`] reads, so this
/// is only the value a pool thread or an off-runtime caller sees.
pub(crate) struct ReadTsGuard(u64);

impl ReadTsGuard {
    pub(crate) fn enter(ts: u64) -> Self {
        Self(POOL_READ_TS.with(|cell| cell.replace(ts)))
    }
}

impl Drop for ReadTsGuard {
    fn drop(&mut self) {
        POOL_READ_TS.with(|cell| cell.set(self.0));
    }
}

/// Runs `f` with `ts` as the statement's read timestamp.
///
/// A task-local scope rather than a guard: tokio restores the binding when the
/// future completes, including on a panic, and it follows the statement across
/// the runtime workers it is resumed on.
pub(crate) async fn with_read_ts<T, F>(ts: u64, f: F) -> T
where
    F: std::future::Future<Output = T>,
{
    READ_TS.scope(ts, f).await
}

/// How many times a statement re-runs after a retryable conflict
/// (`QueryContext::QueryStatement` loops without a bound; this only stops a
/// statement that conflicts with itself).
pub(crate) const RETRY_LIMIT: usize = 1000;

/// Decoded rows paired with their row ids (the executor's scan unit).
pub(crate) type TableRows = Vec<(usize, Vec<Value>)>;

pub(crate) fn value_bytes(v: &Value) -> usize {
    match v {
        Value::Str(s) => s.len() + 24,
        _ => 24,
    }
}

impl RowCache {
    pub(crate) fn push(&mut self, seq: usize, row: Vec<Value>) {
        self.bytes += 24 + row.iter().map(value_bytes).sum::<usize>();
        self.rows.push((seq, row));
    }
}

/// Only scalar columns are cached: array/tensor/sparse cells have several
/// textual representations, so their decoded form is not stable enough to
/// reuse across write and scan paths.
pub(crate) fn scalar_only(types: &[String]) -> bool {
    types.iter().all(|t| {
        let t = t.trim().to_ascii_uppercase();
        !is_blob_type(&t)
            && !t.contains("ARRAY")
            && (t.starts_with("INT")
                || t.starts_with("BIGINT")
                || t.starts_with("SMALLINT")
                || t.starts_with("TINYINT")
                || t.starts_with("FLOAT")
                || t.starts_with("DOUBLE")
                || t.starts_with("REAL")
                || t.starts_with("VARCHAR")
                || t.starts_with("BOOL"))
    })
}

/// Engine-wide runtime state.
///
/// C++: `InfinityContext` (`src/storage/infinity_context.cppm`) — the shared
/// configuration, buffer manager and service handles. `Engine` composes it
/// instead of owning these fields directly.
pub(crate) struct Context {
    /// One mapping per sealed block, shared by every reader in the process.
    ///
    /// A block file never changes while it is sealed, so the mapping *is* the
    /// block: the reference reaches the same bytes through a `BufferManager`
    /// copy, which costs memory and an LRU; here the pages stay in the page
    /// cache. The entry is dropped when the block's files are rewritten or
    /// deleted (`Engine::drop_block_maps`).
    pub(crate) block_maps:
        Mutex<std::collections::HashMap<storage::format::BlockId, Arc<MappedBlock>>>,
    /// The budget the index buffers are held to (`memindex_memory_quota` for
    /// the dump trigger is separate). Built in at 8 GiB: the reference's
    /// `[buffer] buffer_manager_size` is accepted by the config parser but
    /// ignored, and sealed blocks are mapped, not buffered, so they do not
    /// spend it.
    pub(crate) buffer_budget: std::sync::atomic::AtomicUsize,
    /// Mappings served from the shared map, and mappings opened for a block
    /// it did not hold: `SHOW VARIABLE memory_cache_miss` reports the ratio.
    pub(crate) block_map_hits: std::sync::atomic::AtomicU64,
    pub(crate) block_map_misses: std::sync::atomic::AtomicU64,
    /// session/global configuration set via `SET CONFIG …`
    pub(crate) config: Mutex<HashMap<String, String>>,
    /// `[buffer] memindex_memory_quota`, in bytes. `0` disables the dump
    /// trigger (the reference only dumps when a quota is configured).
    pub(crate) memindex_quota: std::sync::atomic::AtomicU64,
    /// Bytes every in-memory index form holds right now, kept by delta.
    ///
    /// The quota check used to walk every table and lock every index, so a
    /// batch paid O(all tables × indexes) just to ask "am I over?". The number
    /// is now maintained where it changes (appends, dumps, seals, drops) and
    /// recomputed in full at each checkpoint to correct any drift; a read is
    /// one atomic load.
    pub(crate) memindex_total: std::sync::atomic::AtomicUsize,
    /// `[storage] mem_index_capacity`: rows an in-memory index may hold before
    /// it is dumped into a sealed segment (the reference's
    /// `Config::MemIndexCapacity()`). `0` disables the row trigger.
    pub(crate) memindex_capacity: std::sync::atomic::AtomicUsize,
    /// `[storage] flush_dumps_mem_index`: `FLUSH DATA` also dumps every memory
    /// index into its chunk. Off by default, because the reference's
    /// `PhysicalFlush` only runs a checkpoint and the cross-engine byte gates
    /// compare directories after `flush_data()`.
    pub(crate) flush_dumps_mem_index: std::sync::atomic::AtomicBool,
    /// Set while a background `DumpMemIndexTask` proposed by the quota is
    /// queued or running (C++ `MemIndexTracer::proposed_dump_`). Prevents the
    /// per-row quota check from queueing one task per row.
    pub(crate) memindex_pending: Arc<std::sync::atomic::AtomicUsize>,
    /// Milliseconds of the last `memindex_bytes` check (see
    /// `Engine::enforce_memindex_quota`).
    pub(crate) last_quota_check_ms: std::sync::atomic::AtomicU64,
    /// Object store backing the C++-format data files (`storage_type =
    /// "minio"`). `None` keeps everything on the local disk.
    pub(crate) object_store: Mutex<Option<Arc<storage::s3::S3Client>>>,
    /// Concurrency limit for full-table text/vector scans.
    pub(crate) scan_gate: ScanGate,
    /// The decoded HNSW chunk budget (built in at 8 GiB; the reference's
    /// `[buffer] buffer_manager_size` is accepted but ignored). Loaded graphs
    /// beyond it are evicted, and a scan that no longer sees full label
    /// coverage falls back to the exact scan.
    pub(crate) hnsw_budget: std::sync::atomic::AtomicUsize,
    /// `query_count` global/session variable: statements executed so far.
    pub(crate) query_count: std::sync::atomic::AtomicU64,
    /// `connected_timestamp` session variable: when the engine was opened.
    pub(crate) connected_at: String,
    /// Commit timestamp of the last cleanup (`BGTaskProcessor::last_cleanup_ts_`,
    /// `src/storage/bg_task/background_process.cppm:53-65`). The cleanup
    /// trigger skips a tick when nothing committed since
    /// (`periodic_trigger_impl.cpp:48-59`).
    pub(crate) last_cleanup_ts: std::sync::atomic::AtomicU64,
}

/// The engine: catalog + transaction manager + KV store + a Tokio runtime.
pub struct Engine {
    /// Catalog + KV store (the reference's `Storage`).
    pub(crate) storage: Storage,
    pub(crate) txn: TxnManager,
    /// the engine's data directory (root of the C++-format block mirror)
    pub(crate) dir: PathBuf,
    /// C++ `Config::SnapshotDir()`: one directory per snapshot name.
    pub(crate) snapshot_dir: PathBuf,
    /// The persistence layer (`PersistenceManager`), when this engine publishes
    /// its data files through `pm|object|` instead of leaving `data/` private
    /// (`EngineOptions::object_dir`).
    pub(crate) persistence: Option<storage::persistence::PersistenceManager>,
    /// Bumped when `optimize_indexes` runs: a `DumpMemIndex` staged before the
    /// optimize cannot commit after it (the reference's index meta version).
    pub(crate) index_epoch: std::sync::atomic::AtomicU64,
    /// Engine-wide runtime state (the reference's `InfinityContext`).
    pub(crate) context: Context,
    /// cached `SELECT` results keyed by the statement text. A `std::sync::Mutex`
    /// for the same reason as `tables`: every access is one `get` / `len` /
    /// `clear`, never held across an `.await` (see §1.9 of
    /// `docs/THREADS_TO_TOKIO.md`).
    pub(crate) result_cache: Mutex<HashMap<String, Vec<QueryResult>>>,
    /// `ResultCacheManager`: plan-node results the optimizer's
    /// `ResultCacheGetter` reads and the scan fills (`result_cache` config).
    pub(crate) plan_cache: Arc<planner::result_cache::SharedCache>,
    /// the database new unqualified table names are resolved against
    pub(crate) current_db: Mutex<String>,
    /// Per-session database (plan §6.1): `session_id -> db`.
    ///
    /// The reference keeps its sessions in a process-wide map and each one
    /// carries its own `current_db`; two connections that `USE` different
    /// databases must not see each other's choice. `current_db` above stays as
    /// the default a session-less caller (a test, a background task) sees.
    pub(crate) sessions: Mutex<HashMap<i64, String>>,
    /// Ids handed out by `open_session`; never reused, like the reference's.
    /// Per-table write bookkeeping (C++ `TableEntry`), keyed by the
    /// fully-qualified lower-case table name.
    /// Per-table write bookkeeping. A `std::sync::Mutex`, deliberately: every
    /// use is "lock, read or write one entry, unlock" - the longest critical
    /// section is a `HashMap` lookup plus a clone, and **no caller holds it
    /// across an `.await`**. Making it `tokio::sync::Mutex` would force the 102
    /// synchronous accessors of this map (and through them `run_select`,
    /// `run_plan` and the operator chain) to become `async`, for a lock that is
    /// never contended for longer than a hash lookup. See
    /// `docs/THREADS_TO_TOKIO.md` §1.9.
    pub(crate) tables: Arc<Mutex<HashMap<String, TableEntry>>>,
    /// Serialises the schema work a DDL does before its transaction commits.
    ///
    /// `catalog.capture` has a single overlay slot, so two statements staging
    /// a layout rewrite at once would interleave, and the capture has to see a
    /// consistent catalog (a create also races the orphan sweep a snapshot
    /// restore triggers). A DDL whose effects are catalog writes only holds it
    /// for its whole commit; `ALTER … ADD/DROP COLUMN` holds it for the
    /// capture and applies its cells and column files after the conflict
    /// check (`ddl_commit_with_effects`), so those commit phases are ordered by
    /// the transaction manager's conflict matrix instead of this lock.
    pub(crate) ddl_lock: tokio::sync::Mutex<()>,
    /// Optional background task processor (`BGTaskProcessor`). When attached,
    /// the memindex quota queues a real `DumpMemIndexTask`; otherwise the
    /// quota is enforced synchronously.
    pub(crate) bg_processor: Mutex<Option<Arc<BGTaskProcessor>>>,
    /// Weak self handle handed to background tasks (avoids a cycle through
    /// `bg_processor`).
    pub(crate) self_weak: Mutex<std::sync::Weak<Engine>>,
    /// One gate per table, held for a whole write batch.
    ///
    /// The reference's `DumpMemIndex` transaction retries while an append holds
    /// the segment, so it always dumps whole batches; holding the table's gate
    /// for the batch gives the same chunk boundaries deterministically. It is
    /// per table, not one lock for the engine: two appends to different tables
    /// hold different gates and overlap, which is the granularity the
    /// reference's per-block `BlockLock` gives for writes that do not touch the
    /// same block (`storage/block/block_lock`). A dump holds every gate, in key
    /// order, so it sees whole batches.
    pub(crate) batch_locks: Mutex<HashMap<String, Arc<tokio::sync::Mutex<()>>>>,
    /// One gate per table: a transaction's bottom half (rows → index entries →
    /// block mirror) runs under its table's gate, so two transactions of the
    /// same table do their bottom work one after the other while transactions
    /// of different tables overlap - the order `BottomExecutor`'s
    /// `crc32(table_id) % n` queues give the reference
    /// (`bottom_executor_impl.cpp:32-78`).
    pub(crate) bottom_gates: Mutex<HashMap<String, Arc<tokio::sync::Mutex<()>>>>,
    /// One FIFO of bottom-half jobs **per table**, drained on the shared pool.
    ///
    /// The reference routes a transaction to `crc32(table_id) % pool_size`, so
    /// two tables that hash alike wait for each other; with a directory of a
    /// hundred thousand tables that collision is what limits cross-table
    /// parallelism. Here every table has its own queue and one job in flight,
    /// so the in-flight tables are bounded by the pool, not by a hash.
    pub(crate) bottom_queues: Mutex<BottomQueues>,
    /// `Storage::txn_allocator_` (`txn_allocator_impl.cpp:56`): one thread
    /// hands out every append's row ids, so they are contiguous in submission
    /// order. Started lazily; an engine without a stable `Arc` handle (most
    /// tests) allocates inline.
    pub(crate) allocator: Mutex<Option<Arc<Allocator>>>,
    /// How many allocation jobs the `TxnAllocator` thread has run. Lets a test
    /// assert the ids really came from it and not from the fallback.
    pub(crate) allocator_jobs: std::sync::atomic::AtomicUsize,
    /// Write batches since the last `malloc_trim` (see `insert_literals`).
    pub(crate) batches_since_trim: std::sync::atomic::AtomicUsize,
    pub(crate) vector_cache: Mutex<HashMap<String, SharedVectorCache>>,
    /// C++'s `SystemInfo::CPUUsage` sleeps 200 ms between two snapshots; an
    /// accessor must not block, so this engine keeps the previous sample and
    /// reports the delta since the last call instead.
    pub(crate) cpu_sample: Mutex<Option<CpuSample>>,
    /// Running queries and the finished-statement history.
    pub(crate) queries: Mutex<diagnostics::QueryRegistry>,
    /// `profile` global variable (C++ `NewCatalog::SetProfile`): while it is
    /// on, a finished statement is appended to the profile history. The
    /// running-query registry is kept either way, as `SHOW QUERIES` needs it.
    pub(crate) profile_enabled: std::sync::atomic::AtomicBool,
}

/// A `cpu_usage` sample: the process's utime+stime ticks and when they were
/// read.
#[derive(Debug, Clone, Copy)]
pub(crate) struct CpuSample {
    pub(crate) ticks: u64,
    pub(crate) at: std::time::Instant,
}

/// Linux `USER_HZ`, the unit of the `utime`/`stime` fields of `/proc/self/stat`
/// (`sysconf(_SC_CLK_TCK)`; 100 on every supported Linux target).
pub(crate) const CLOCK_TICKS_PER_SEC: f64 = 100.0;

/// Resident set size in bytes: `VmRSS` from `/proc/self/status`, the exact
/// field C++'s `SystemInfo::MemoryUsage` (`src/metrics/system_info_impl.cpp:305`)
/// parses, with `/proc/self/statm` (field 2 = resident pages) as the fallback
/// for a kernel that does not publish `VmRSS`. Both report the same quantity.
pub(crate) fn process_rss_bytes() -> u64 {
    if let Ok(status) = std::fs::read_to_string("/proc/self/status") {
        for line in status.lines() {
            if let Some(rest) = line.strip_prefix("VmRSS:") {
                let kb: u64 = rest
                    .split_whitespace()
                    .next()
                    .and_then(|v| v.parse().ok())
                    .unwrap_or(0);
                return kb * 1024;
            }
        }
    }
    let Ok(statm) = std::fs::read_to_string("/proc/self/statm") else {
        return 0;
    };
    let pages: u64 = statm
        .split_whitespace()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(0);
    // 4 KiB is the page size of every Linux target this engine builds for.
    pages * 4096
}

/// Open file descriptors, the way C++'s `SystemInfo::OpenFileCount`
/// (`src/metrics/system_info_impl.cpp:357`) counts entries in `/proc/<pid>/fd`.
pub(crate) fn open_file_count() -> u64 {
    std::fs::read_dir("/proc/self/fd")
        .map(|entries| entries.filter(|e| e.is_ok()).count() as u64)
        .unwrap_or(0)
}

/// The process's CPU time in clock ticks: `/proc/self/stat` fields 14 (utime)
/// and 15 (stime), the first two of the four C++ `cpu_cost_of_process` sums
/// (`utime + stime + cutime + cstime`); the children counters stay out because
/// this process forks nothing.
pub(crate) fn process_cpu_ticks() -> Option<u64> {
    let stat = std::fs::read_to_string("/proc/self/stat").ok()?;
    // The command name is parenthesised and may contain spaces, so the fields
    // after it start at the last ')'. Field 14 of the whole line is field 12
    // after the command name (state=3, ppid=4, …).
    let tail = &stat[stat.rfind(')')? + 1..];
    let fields: Vec<&str> = tail.split_whitespace().collect();
    let utime: u64 = fields.get(11)?.parse().ok()?;
    let stime: u64 = fields.get(12)?.parse().ok()?;
    Some(utime + stime)
}

/// Logical CPUs the kernel reports online (`nproc`), from `/proc/cpuinfo`'s
/// `processor` lines.
pub(crate) fn online_cpu_count() -> u64 {
    let Ok(cpuinfo) = std::fs::read_to_string("/proc/cpuinfo") else {
        return 1;
    };
    let count = cpuinfo
        .lines()
        .filter(|l| l.starts_with("processor"))
        .count() as u64;
    count.max(1)
}

/// What a pending snapshot restore recreates (the reference's `kRestoreTable`,
/// `kRestoreDatabase` and `kRestoreSystem` transaction types).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum RestoreScope {
    Table,
    Database,
    System,
}

/// A snapshot restore that has not been applied yet.
///
/// The reference runs a restore inside a `NewTxn`: nothing is visible until
/// `CommitTxn`, and `Rollback` discards it. [`Engine`] exposes the same
/// two-phase shape: the `begin_restore_*` methods validate that the snapshot
/// exists, the engine is only modified when [`PendingRestore::commit`] runs,
/// and dropping the handle (or calling [`PendingRestore::rollback`]) leaves it
/// untouched.
pub struct PendingRestore<'a> {
    pub(crate) engine: &'a Engine,
    pub(crate) scope: RestoreScope,
    pub(crate) name: String,
}

impl PendingRestore<'_> {
    /// Applies the restore, as `CommitTxn` does for a restore transaction.
    pub fn commit(self) -> Result<()> {
        match self.scope {
            // A synchronous adapter for the tests and `EngineTxn`; the
            // statement path awaits the snapshot functions directly.
            RestoreScope::Table => {
                crate::scheduler::block_on(self.engine.restore_table_snapshot(&self.name))
            }
            RestoreScope::Database => {
                crate::scheduler::block_on(self.engine.restore_database_snapshot(&self.name))
            }
            RestoreScope::System => {
                crate::scheduler::block_on(self.engine.restore_system_snapshot(&self.name))
            }
        }
    }

    /// Discards the restore, as `NewTxn::Rollback` does.
    pub fn rollback(self) {}
}

/// One append staged by an [`EngineTxn`]: the table, its first row id, the
/// append-log record the reservation created (settled with the commit ts), the
/// encoded rows and the literal rows.
pub(crate) struct StagedAppend {
    pub(crate) table: String,
    pub(crate) base: usize,
    pub(crate) record: usize,
    pub(crate) encoded: Vec<String>,
    pub(crate) rows: Vec<Vec<Literal>>,
}

/// The catalog state a staged `DumpMemIndex` transaction validated against.
#[derive(Debug, Clone, PartialEq, Eq)]
pub(crate) struct DumpVersion {
    pub(crate) table: catalog::TableMeta,
    pub(crate) index: catalog::IndexMeta,
    /// `Engine::index_epoch` when the dump was staged.
    pub(crate) optimize_epoch: u64,
}

/// A `DumpMemIndex` staged in an [`EngineTxn`] (C++ `NewTxn::DumpMemIndex`).
#[derive(Debug, Clone)]
pub(crate) struct StagedDump {
    pub(crate) table: String,
    pub(crate) index: String,
    pub(crate) version: DumpVersion,
}

/// A multi-statement transaction (the reference's `NewTxn`).
///
/// Rows and row-id deletes are staged in an [`txn::Txn`] and become
/// visible only at [`EngineTxn::commit`]. A snapshot created through
/// [`EngineTxn::create_table_snapshot`] therefore observes committed state
/// only — an uncommitted append or delete of another live transaction (or of
/// this one) is absent, which is the isolation rule
/// `TableSnapshotTest.test_create_snapshot_{insert,delete}_data` asserts.
///
/// `rollback` discards the staged writes. The row-id allocator is not rewound
/// (the reference frees the append range on rollback; no test here reuses ids
/// from a rolled-back append).
pub struct EngineTxn<'a> {
    pub(crate) engine: &'a Engine,
    /// The same engine, when it lives in an `Arc` (`Engine::open` records a
    /// `Weak` of itself): the bottom half can then be handed to a worker
    /// thread with an owned handle instead of a borrow, as the reference's
    /// `BottomExecutor` does. `None` for an engine built outside an `Arc`, and
    /// the bottom half runs on the caller's thread.
    #[allow(dead_code)] // read once the bottom half is dispatched to a worker
    pub(crate) handle: Option<Arc<Engine>>,
    pub(crate) txn: Option<Txn>,
    /// `(table, first row id, encoded rows, literal rows)` per append.
    pub(crate) appends: Vec<StagedAppend>,
    /// `(table, global row id)` per staged delete.
    pub(crate) deletes: Vec<(String, usize)>,
    /// `DumpMemIndex` requests applied only if their catalog version is still
    /// current at commit.
    pub(crate) dumps: Vec<StagedDump>,
}

/// `NewTxn::CommitBottom` / `NewTxnManager::CommitBottom`: the bottom half of a
/// write commit on its own, so it can run on a worker thread (the reference's
/// `BottomExecutor`) instead of the committing one.
///
/// Rows go in first, index entries after, then the block mirror; the commit is
/// published only once all of it is in place, so a reader never sees rows its
/// index does not know about yet.
/// Publishes a deferred commit when it goes out of scope.
///
/// The bottom half of a commit is what makes the rows, their index entries and
/// the block mirror visible; the reference publishes only after all of it
/// (`SetTxnCommitted`). Holding this guard across that work - rather than
/// calling `finish_bottom` on the success path - keeps the barrier from
/// stalling when the work fails half way.
pub(crate) struct BottomGuard<'a> {
    pub(crate) manager: &'a TxnManager,
    pub(crate) ts: TxnTimeStamp,
}

impl<'a> BottomGuard<'a> {
    pub(crate) fn new(manager: &'a TxnManager, ts: TxnTimeStamp) -> Self {
        Self { manager, ts }
    }
}

impl Drop for BottomGuard<'_> {
    fn drop(&mut self) {
        self.manager.finish_bottom(self.ts);
    }
}

pub(crate) async fn run_bottom(
    engine: &Engine,
    appends: &[StagedAppend],
    deletes: &[(String, usize)],
    dumps: &[StagedDump],
    txn: Txn,
) -> Result<()> {
    let mut visible: Option<BottomGuard<'_>> = None;
    if !txn.is_empty() {
        // The bottom half: rows first, index entries after (both are
        // inside `CommitBottom` in the reference). The commit becomes
        // visible only once the index entries are in place, so a reader
        // never sees rows their index does not know about yet.
        let ts = engine.txn.commit_deferred(txn).await?;
        visible = Some(BottomGuard::new(&engine.txn, ts));
        // The block's `version` carries each append's and each delete's
        // commit ts (`BlockVersion::Append` / `Delete`). Each staged append
        // names its own record: another transaction may have appended while
        // this one was committing, and "the last record" would be its.
        for append in appends {
            engine.settle_append_at(&append.table, append.record, ts);
        }
        // The mirror's `version` hides a deleted slot (`BlockVersion::Delete`),
        // so the ids join the table's delete log - the record the SQL path
        // writes - now that the commit is durable. An uncommitted delete leaves
        // it alone, which is what keeps it invisible to a scan and a snapshot.
        let mut delete_ids: std::collections::BTreeMap<&str, Vec<usize>> = Default::default();
        for (table, seq) in deletes {
            delete_ids.entry(table.as_str()).or_default().push(*seq);
        }
        let mut delete_start: std::collections::BTreeMap<&str, (usize, usize)> = Default::default();
        for (table, ids) in &delete_ids {
            let first = engine.note_deleted_ids(table, ids);
            delete_start.insert(table, (first, ids.len()));
        }
        for (table, (first, count)) in &delete_start {
            engine.settle_delete_at(table, *first, *count, ts);
        }
    } else if !dumps.is_empty() {
        // A dump is a write transaction of its own in the reference:
        // `DumpIndexProcessor::DoDump` opens one with
        // `BeginTxnShared(…, kDumpMemIndex)` and commits it, so it spends
        // a commit ts whether or not the index had a tail to seal (the
        // WAL keeps the `DumpIndex` entry). Spending it here is what keeps
        // every later statement's ts where the reference has it.
        let turn = engine.txn.reserve_commit_ts();
        let ts = turn.ts();
        engine.txn.commit_at(txn, ts).await?;
    }
    for append in appends {
        let table = append.table.as_str();
        if engine.any_in_memory_index() || engine.has_index_declaration(table) {
            let schema = engine.table_schema(table).unwrap_or_default();
            // Tokenising and inserting the full-text terms happens once for the
            // whole append batch (one lock acquisition per column) instead of
            // once per row.
            let mut texts: Vec<(usize, Vec<String>)> = Vec::with_capacity(append.rows.len());
            for (k, row) in append.rows.iter().enumerate() {
                engine.update_indexes_on_insert(table, append.base + k, row, &schema)?;
                texts.push((
                    append.base + k,
                    row.iter().map(|v| v.to_slt_string()).collect(),
                ));
            }
            let batch: Vec<(usize, &[String])> = texts
                .iter()
                .map(|(seq, cells)| (*seq, cells.as_slice()))
                .collect();
            engine
                .update_text_indexes_batch_async(table, &batch, &schema, None)
                .await;
            for (seq, cells) in &texts {
                engine.update_vector_caches(table, *seq, cells);
            }
            engine.dump_hnsw_over_capacity(table);
            engine.dump_secondary_over_capacity(table).await;
        }
        engine.bump_writes(table);
        engine.note_live_rows(table, append.rows.len() as isize);
        engine.append_row_cache(table, append.base, &append.encoded);
    }
    // A delete is a mark in the block version (`DeleteInBlock`): the rows keep
    // their index entries, which a reader drops by the version (or the delete
    // journal before the checkpoint) - the reference never unlinks a posting.
    // Keep the C++ block mirror current at commit, as the reference does
    // (`AppendInBlock`), so a crash never leaves a block the catalog
    // mentions without its files - the reference refuses to open such a
    // directory.
    let touched: std::collections::BTreeSet<&str> = appends
        .iter()
        .map(|append| append.table.as_str())
        .chain(deletes.iter().map(|(table, _)| table.as_str()))
        .collect();
    for table in touched {
        // A column the block format cannot express has no block to hold the
        // row, and the port keeps no row journal either: such a table's rows
        // stay in the append buffer (see `has_block_layout`).
        let defaults = engine.table_defaults(table).unwrap_or_default();
        let mut published: Vec<(usize, Vec<Value>)> = Vec::new();
        for append in appends.iter().filter(|append| append.table == table) {
            for (k, text) in append.encoded.iter().enumerate() {
                // Never drop a row because its text form would not parse: the
                // commit has already published it, and the block is the only
                // place its values can come from.
                match engine.parse_stored_row(table, &defaults, text.as_bytes()) {
                    Ok(row) => published.push((append.base + k, row)),
                    Err(_) => published.push((
                        append.base + k,
                        text.split('\t')
                            .map(|c| Value::Str(c.to_string()))
                            .collect(),
                    )),
                }
            }
        }
        engine.refresh_block_mirror_extra(table, &published)?;
    }
    // The catalog is still current: perform the sealed writes now.
    for dump in dumps {
        engine.dump_index(&dump.table, &dump.index)?;
    }
    // `NewTxnManager::CommitBottom`: publishing happens when `visible` drops,
    // on the error path too, so the clock never waits on a half-done bottom.
    drop(visible);
    Ok(())
}

/// The per-table FIFO of bottom-half jobs (`Engine::bottom_queues`).
#[derive(Default)]
pub(crate) struct BottomQueues {
    /// Queued jobs, oldest first, per table key.
    pub(crate) pending:
        std::collections::HashMap<String, std::collections::VecDeque<Arc<BottomWork>>>,
    /// Tables with a job running: at most one of theirs is on the pool.
    pub(crate) running: std::collections::HashSet<String>,
    /// Whether a pool is attached; without one the caller runs the job inline.
    pub(crate) attached: bool,
}

/// The job a table's worker runs for one statement's bottom half.
pub(crate) type BottomJob = Box<dyn FnOnce(&Engine) -> Result<()> + Send>;

/// The post-commit work of one statement, handed to a [`BottomExecutor`]
/// worker (`NewTxnManager::CommitBottom` for a single-table statement).
///
/// The completion handshake is a **tokio oneshot** (plan 6.12.4 step 2): the
/// caller does not need a shared `finished` flag plus a `Condvar`, only "tell me
/// when it is done, and give me the result". The result rides in the message, so
/// the state mutex disappears with the condvar; when the statement path is async
/// the `wait` becomes `await` without another change.
pub(crate) struct BottomWork {
    pub(crate) engine: Arc<Engine>,
    pub(crate) table_id: String,
    /// The tables' gates, locked on the worker: bottom halves of one table take
    /// their turn in submission (= commit) order.
    pub(crate) gates: Vec<Arc<tokio::sync::Mutex<()>>>,
    pub(crate) work: std::sync::Mutex<Option<BottomJob>>,
    /// The worker's end of the completion channel, taken by `commit_bottom`.
    pub(crate) done: std::sync::Mutex<Option<tokio::sync::oneshot::Sender<Result<()>>>>,
    /// The waiter's end, taken by `wait`.
    pub(crate) finished: std::sync::Mutex<Option<tokio::sync::oneshot::Receiver<Result<()>>>>,
}

impl BottomWork {
    pub(crate) async fn wait(&self) -> Result<()> {
        let receiver = {
            let mut slot = self.finished.lock().unwrap();
            match slot.take() {
                Some(receiver) => receiver,
                // Already answered (or never armed): nothing to wait for.
                None => return Ok(()),
            }
        };
        match receiver.await {
            Ok(result) => result,
            // The sender was dropped without sending: the worker died before
            // the bottom ran. Reporting success would hide a lost commit.
            Err(_) => Err(Error::Txn(
                "a bottom-half task finished without a result".into(),
            )),
        }
    }
}

impl storage::wal::bottom_executor::BottomTask for BottomWork {
    fn table_id(&self) -> &str {
        &self.table_id
    }

    fn is_committing(&self) -> bool {
        true
    }

    fn commit_bottom(&self) {
        #[cfg(test)]
        BOTTOM_WORKS.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        let work = self.work.lock().unwrap().take();
        let result = match work {
            Some(work) => {
                let _turn: Vec<tokio::sync::MutexGuard<'_, ()>> = self
                    .gates
                    .iter()
                    .map(|gate| crate::scheduler::block_on(gate.lock()))
                    .collect::<Vec<_>>();
                work(&self.engine)
            }
            None => Ok(()),
        };
        // A waiter that has already given up (`wait` was never called, or the
        // receiver dropped) simply loses the message; the bottom itself ran.
        if let Some(sender) = self.done.lock().unwrap().take() {
            let _ = sender.send(result);
        }
    }
}

/// Statements whose bottom half ran on the pool, for tests.
#[cfg(test)]
pub(crate) static BOTTOM_WORKS: std::sync::atomic::AtomicUsize =
    std::sync::atomic::AtomicUsize::new(0);

/// Statements whose bottom half ran on the pool, for tests.
#[cfg(test)]
pub(crate) fn bottom_work_count() -> usize {
    BOTTOM_WORKS.load(std::sync::atomic::Ordering::SeqCst)
}

impl<'a> EngineTxn<'a> {
    /// C++ `NewTxn::BeginTS`: the read timestamp this transaction sees.
    pub fn begin_ts(&self) -> u64 {
        self.txn.as_ref().map(|t| t.begin_ts).unwrap_or(0)
    }

    /// C++ `NewTxn::Append`: normalize the block like `INSERT` does, reserve
    /// row ids, and stage the encoded rows in the transaction.
    pub fn append(&mut self, table: &str, rows: Vec<Vec<Literal>>) -> Result<usize> {
        if rows.is_empty() {
            return Ok(0);
        }
        let table = self.engine.table_key(table);
        let meta = self
            .engine
            .storage
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let schema = meta.columns.clone();
        let mut rows = align_insert_rows(&schema, &meta.defaults, None, &rows);
        // Integer columns truncate a fractional literal, as the reference's
        // implicit cast does (mirrors `Engine::insert_literals`).
        for row in rows.iter_mut() {
            for (i, cell) in row.iter_mut().enumerate() {
                let int_col = meta
                    .types
                    .get(i)
                    .map(|t| {
                        let t = t.trim().to_ascii_uppercase();
                        t.starts_with("INT")
                            || t.starts_with("TINYINT")
                            || t.starts_with("SMALLINT")
                            || t.starts_with("BIGINT")
                    })
                    .unwrap_or(false);
                if int_col {
                    if let Literal::Float(f) = cell {
                        *cell = Literal::Int(*f as i64);
                    }
                }
            }
        }
        for row in &rows {
            for (i, nn) in meta.not_nulls.iter().enumerate() {
                if *nn && matches!(row.get(i), Some(Literal::Null) | None) {
                    return Err(Error::Invalid(format!(
                        "null value in NOT NULL column '{}'",
                        schema.get(i).cloned().unwrap_or_default()
                    )));
                }
            }
        }
        let range = self.engine.reserve_rows(&table, rows.len(), false)?;
        let base = range.base;
        let types = self.engine.column_types(&table)?;
        let mut encoded = Vec::with_capacity(rows.len());
        for row in rows.iter() {
            encoded.push(self.engine.encode_row_stored(&table, row, &types)?);
        }
        // `WalCmdAppendV2`: the rows ride in the log's `DataBlock` (the
        // reference's command), and the commit writes them straight into the
        // block - the engine keeps no `row:` key for either. A table no block
        // can hold, or a row the WAL encoder cannot express, contributes no
        // command: its rows reach the disk with the block buffer at the next
        // checkpoint.
        let table_id = meta.id.to_string();
        if let Some(block) =
            typed_append_block(&rows, &types).filter(|_| self.engine.has_block_layout(&table))
        {
            if let Some(txn) = self.txn.as_mut() {
                txn.set_typed_commands(vec![self.engine.append_command(
                    &table,
                    &table_id,
                    base,
                    rows.len(),
                    block,
                )]);
            }
        }
        let n = rows.len();
        if let Some(txn) = self.txn.as_mut() {
            // `NewTxn::Append`: the store type the conflict matrix reads.
            txn.set_op(TxnOp::Append {
                table: table.to_string(),
            });
        }
        self.appends.push(StagedAppend {
            table: table.to_string(),
            base,
            record: range.append,
            encoded,
            rows,
        });
        Ok(n)
    }

    /// C++ `NewTxn::Delete` over physical row ids: `(segment_id, offset)` maps
    /// to the engine's global sequence id (`segment << 32 | offset`). Ids that
    /// do not exist in committed state are skipped, as the reference's scan
    /// does.
    pub fn delete_row_ids(&mut self, table: &str, row_ids: &[(u32, u32)]) -> Result<usize> {
        let table = self.engine.table_key(table);
        let mut staged = 0;
        let mut ids: Vec<usize> = Vec::with_capacity(row_ids.len());
        for (segment_id, offset) in row_ids {
            let seq = ((*segment_id as usize) << 32) | (*offset as usize);
            if self.engine.row_is_present(&table, seq) {
                // A deferred tombstone: it rides in the WAL's `DeleteV2` and is
                // applied by the bottom half (`DeleteInBlock`), not as a key -
                // the reference has no `del:` key space.
                self.txn
                    .as_mut()
                    .expect("transaction is open")
                    .delete_deferred(wal_delete_key(&table, seq));
                self.deletes.push((table.clone(), seq));
                ids.push(seq);
                staged += 1;
            }
        }
        Ok(staged)
    }

    /// `NewTxn::CreateTableSnapshot`: reads committed state only, so this
    /// transaction's staged writes are invisible to the snapshot.
    pub fn create_table_snapshot(&self, db: &str, table: &str, name: &str) -> Result<()> {
        crate::scheduler::block_on(self.engine.create_table_snapshot(db, table, name))
    }

    /// C++ `NewTxn::DumpMemIndex`: stages the dump of `index` on `table`.
    ///
    /// The dump is applied at [`Self::commit`] only if the table and index
    /// catalogs are unchanged; a `DROP`/`RENAME`/`ALTER` that commits in
    /// between makes the commit fail with a transaction conflict, exactly as
    /// the reference's `CommitTxn` rejects a dump whose meta version moved.
    pub fn dump_index(&mut self, table: &str, index: &str) -> Result<()> {
        let table = self.engine.table_key(table);
        let version = self.engine.dump_version(&table, index)?;
        self.dumps.push(StagedDump {
            table,
            index: index.to_string(),
            version,
        });
        Ok(())
    }

    /// `CommitTxn`: applies the staged batch, then updates the in-memory index
    /// registries and row bookkeeping.
    pub fn commit(mut self) -> Result<()> {
        // A transaction is only a staged batch until here, so the mode is
        // checked at commit: a node that turned read-only (or uninitialized)
        // after the append must not publish it (`PhysicalInsert` checks the
        // same condition while executing the request).
        self.engine.ensure_writable()?;
        let Some(txn) = self.txn.take() else {
            return Ok(());
        };
        // Validate every staged dump before touching the engine: a DDL that
        // committed after the dump began (drop db/table/index, rename, alter)
        // bumps the table/index metadata and fails the whole transaction.
        for dump in &self.dumps {
            match self.engine.dump_version(&dump.table, &dump.index) {
                Ok(current) if current == dump.version => {}
                Ok(_) => {
                    return Err(Error::TxnConflict(format!(
                        "index '{}' on '{}' changed before the dump committed",
                        dump.index, dump.table
                    )))
                }
                Err(_) => {
                    return Err(Error::TxnConflict(format!(
                        "index '{}' on '{}' was dropped before the dump committed",
                        dump.index, dump.table
                    )))
                }
            }
        }
        // A table (or its database) dropped after this transaction began makes
        // the staged writes uncommittable, as `CommitTxn` fails when the
        // catalog entry the request targeted is gone (the reference's
        // `test_import_with_index_rollback_cleanup`).
        for table in self
            .appends
            .iter()
            .map(|append| &append.table)
            .chain(self.deletes.iter().map(|(t, _)| t))
        {
            if !self.engine.table_exists(table) {
                return Err(Error::NotFound(format!(
                    "table '{table}' was dropped before commit"
                )));
            }
        }
        // A commit changes visible rows; cached `SELECT` results must not
        // survive it (the SQL path clears the cache for mutating statements).
        self.engine.result_cache.lock().unwrap().clear();
        self.engine.plan_cache.clear();
        let mut delete_counts: std::collections::BTreeMap<String, usize> = Default::default();
        for (table, _) in &self.deletes {
            *delete_counts.entry(table.clone()).or_default() += 1;
        }
        for (table, count) in &delete_counts {
            self.engine.invalidate_row_cache(table);
            self.engine.note_live_rows(table, -(*count as isize));
        }
        // Same-table bottom halves take their turn in commit order; different
        // tables do not block each other (`BottomExecutor`'s sharded queues).
        // Acquiring before the timestamp keeps the turn order equal to the
        // commit order the barrier publishes in.
        let mut gate_keys: Vec<String> = self
            .appends
            .iter()
            .map(|append| self.engine.table_key(&append.table))
            .chain(
                self.deletes
                    .iter()
                    .map(|(table, _)| self.engine.table_key(table)),
            )
            .chain(
                self.dumps
                    .iter()
                    .map(|dump| self.engine.table_key(&dump.table)),
            )
            .collect();
        gate_keys.sort();
        gate_keys.dedup();
        let gates: Vec<std::sync::Arc<tokio::sync::Mutex<()>>> = gate_keys
            .iter()
            .map(|table| self.engine.bottom_gate(table))
            .collect();
        let appends = std::mem::take(&mut self.appends);
        let deletes = std::mem::take(&mut self.deletes);
        let dumps = std::mem::take(&mut self.dumps);
        // The reference submits the transaction to the `BottomExecutor` and
        // waits for the worker to call `CommitBottom`; without a pool (or
        // without an owned engine handle for a `'static` job) the bottom half
        // runs here.
        let pool = self
            .handle
            .clone()
            .filter(|_| self.engine.bottom_pool_attached());
        if let Some(engine) = pool {
            let table_id = appends
                .first()
                .map(|append| append.table.clone())
                .or_else(|| deletes.first().map(|(table, _)| table.clone()))
                .or_else(|| dumps.first().map(|dump| dump.table.clone()))
                .unwrap_or_default();
            let (done, finished) = tokio::sync::oneshot::channel();
            let task = std::sync::Arc::new(BottomWork {
                engine,
                table_id,
                gates,
                work: std::sync::Mutex::new(Some(Box::new(move |engine: &Engine| {
                    crate::scheduler::block_on(run_bottom(engine, &appends, &deletes, &dumps, txn))
                }))),
                done: std::sync::Mutex::new(Some(done)),
                finished: std::sync::Mutex::new(Some(finished)),
            });
            self.engine.submit_bottom(task.clone());
            return crate::scheduler::block_on(task.wait());
        }
        let _gates: Vec<tokio::sync::MutexGuard<'_, ()>> = gates
            .iter()
            .map(|gate| crate::scheduler::block_on(gate.lock()))
            .collect();
        crate::scheduler::block_on(run_bottom(self.engine, &appends, &deletes, &dumps, txn))
    }

    /// `NewTxn::Rollback`: discards every staged write and counts the rollback
    /// (`NewTxnManager::RollBackTxn` bumps `total_rollbacked_txn_count`, which
    /// `SHOW VARIABLE total_rollback_count` reports).
    pub fn rollback(mut self) {
        if let Some(txn) = self.txn.take() {
            let _ = self.engine.txn.rollback(txn);
        }
        self.appends.clear();
        self.deletes.clear();
        self.dumps.clear();
    }
}

/// `ExplainFragment::Explain` for a plan that is not a `SEARCH`: the root
/// fragment's title, its sink, then the operators and the source the physical
/// plan already lists (`explain_select` and friends), which is the order the
/// reference prints them in (`explain_fragment_impl.cpp:37-67`).
///
/// The fragment type follows `FragmentBuilder`: a table scan is a
/// `ParallelStream`, a plan without one is a `SerialMaterialize`. The sink and
/// source carry the next free node id, as the reference gives them ids of
/// their own while it builds the fragment.
pub(crate) fn fragment_text(lines: Vec<String>) -> Vec<String> {
    // A `SEARCH` plan already comes back rendered per fragment
    // (`explain_search::fragment_lines`).
    if lines
        .first()
        .map(|line| line.starts_with("FRAGMENT"))
        .unwrap_or(false)
    {
        return lines;
    }
    // `FragmentBuilder`: a table scan is a parallel stream, a blocking operator
    // above it turns the fragment into a parallel materialize, and a plan with
    // no scan at all (VALUES, a bare projection) materialises serially.
    let has_scan = lines
        .iter()
        .any(|line| line.to_ascii_uppercase().contains("SCAN"));
    let blocking = lines.iter().any(|line| {
        let line = line.trim_start();
        line.starts_with("-> TOP")
            || line.starts_with("-> AGGREGATE")
            || line.starts_with("-> SORT")
    });
    let fragment_type = match (has_scan, blocking) {
        (false, _) => "SerialMaterialize",
        (true, true) => "ParallelMaterialize",
        (true, false) => "ParallelStream",
    };
    // Node ids in these renderings are the planner's. `FragmentBuilder` takes
    // the sink three ids past the last operator and the source one past the
    // sink (`test/sql/explain` pins the same offsets: ops up to 4, sink 7,
    // source 8).
    let mut last_id = 0u64;
    for line in &lines {
        if let Some(start) = line.find('(') {
            if let Some(end) = line[start..].find(')') {
                if let Ok(id) = line[start + 1..start + end].parse::<u64>() {
                    last_id = last_id.max(id);
                }
            }
        }
    }
    let sink = last_id + 3;
    let source = sink + 1;
    let mut out = Vec::with_capacity(lines.len() + 3);
    out.push(format!("FRAGMENT (1, {fragment_type})"));
    out.push(format!("-> SINK ({sink}) Result"));
    for (i, line) in lines.into_iter().enumerate() {
        // The physical rendering indents its root operator; in a fragment every
        // operator hangs off the sink, so the root gets an arrow too.
        if i == 0 && line.starts_with(' ') && !line.starts_with("->") {
            out.push(format!("->{}", line));
        } else {
            out.push(line);
        }
    }
    out.push(format!("-> SOURCE ({source}) Table"));
    // `ExplainFragment::Explain` appends a blank element after each fragment
    // ("Insert blank elements after each Fragment for alignment",
    // `explain_fragment_impl.cpp:67`); a client sees it as an empty row.
    out.push(String::new());
    out
}

/// Deregisters a statement from the query registry when it returns, and records
/// its duration in the profile history while `profile` is on.
///
/// C++: the `DeferFn` that calls `SessionManager::RemoveQueryRecord` plus
/// `QueryProfiler`'s history enqueue (`query_context_impl.cpp:180-260`).
pub(crate) struct QueryGuard<'a> {
    pub(crate) engine: &'a Engine,
    pub(crate) query_id: u64,
    pub(crate) query_kind: String,
    pub(crate) query_text: String,
    pub(crate) started: std::time::Instant,
}

impl Drop for QueryGuard<'_> {
    fn drop(&mut self) {
        let mut registry = self.engine.queries.lock().unwrap();
        registry.remove_running(self.query_id);
        if self
            .engine
            .profile_enabled
            .load(std::sync::atomic::Ordering::Relaxed)
        {
            registry.push_finished(diagnostics::ProfileRow {
                query_id: self.query_id,
                query_kind: self.query_kind.clone(),
                query_text: self.query_text.clone(),
                duration_ns: self.started.elapsed().as_nanos().min(i64::MAX as u128) as i64,
            });
        }
    }
}

pub(crate) fn index_key(table: &str, column: &str) -> String {
    format!("{table}.{column}")
}

/// Indexes one document's tokens: length plus `(doc, tf, positions)` postings.
///
/// C++: `MemoryIndexer::Insert` followed by the spill `CommitOffline` performs
/// when a batch fills the memory budget. A column that outgrows the budget is
/// spilled to a sorted run and **keeps indexing**: it never goes dark, so a
/// query can always answer from the postings instead of re-tokenising rows.
pub(crate) fn index_row(index: &mut TextIndex, seq: u64, text: &str) {
    let doc = prepare_doc(seq, text, &index.tokenizer);
    index_row_prepared(index, &doc);
}

/// [`index_row`] with the document already tokenised (see
/// [`prepare_doc`]): the write lock is held only for the insertion.
pub(crate) fn index_row_prepared(index: &mut TextIndex, doc: &PreparedDoc) {
    let budget = index.budget;
    if index.mem.insert_prepared(doc, budget) {
        return;
    }
    if index.mem.spill(&index.dir) && index.mem.insert_prepared(doc, budget) {
        return;
    }
    // Either the run could not be written (no directory, disk error) or this
    // single document is larger than the whole budget. Dropping it would
    // silently lose results, so keep it in memory past the budget; the seal
    // path still writes it.
    index.mem.insert_prepared(doc, usize::MAX);
}

/// [`index_row_prepared`] for a whole batch: one budget check and at most one
/// spill per document, but the caller already holds the lock once for all of
/// them.
pub(crate) fn index_rows_prepared(index: &mut TextIndex, docs: &[PreparedDoc]) {
    for doc in docs {
        index_row_prepared(index, doc);
    }
}

/// Marks a document deleted. Its postings and length stay: the reference's
/// segments keep deleted documents until compaction, so corpus statistics must
/// still count them; the search paths filter them out by sequence id.
/// Seals one column's unsealed segment with the write lock **released** for the
/// file write.
///
/// The tail moves into `TextIndex::inflight` while it is written, so the rows
/// stay searchable and a concurrent query never waits for the FST build, the
/// posting encoding or the `fsync` (measured at ~0.9 s for a 20k-row tail, which
/// a lock-held seal put in front of every query on the column). A failed write
/// leaves the tail in flight for the next attempt instead of merging it back
/// into a tail other writers may have appended to.
pub(crate) fn seal_text_index_unlocked(shared: &SharedTextIndex) {
    let job = {
        let mut index = shared.write().unwrap();
        index.claim_seal()
    };
    let Some(job) = job else {
        return;
    };
    // No lock held: the segment files are written here.
    let started = std::time::Instant::now();
    let written = job.write();
    let elapsed_us = started.elapsed().as_micros() as u64;
    let mut index = shared.write().unwrap();
    index.last_seal_us = elapsed_us;
    match written {
        Some(segment) => index.complete_seal(job, segment),
        None => index.abort_seal(job),
    }
}

/// Runs one column's chunk merge with the write lock **released**.
///
/// The plan is made under the lock from the chunk headers, the files are
/// rewritten with no lock at all, and the swap back in checks that the chunk
/// list is still the one the plan was made from. A chunk list that moved (a seal
/// appended a chunk, another optimize replaced one) is left alone and the merged
/// files are dropped, so a long merge never blocks a query or a writer and never
/// loses a chunk that arrived meanwhile.
pub(crate) fn optimize_text_index_unlocked(shared: &SharedTextIndex) -> crate::Result<usize> {
    let (plan, dir) = {
        let index = shared.read().unwrap();
        let Some(plan) = index.plan_merge()? else {
            return Ok(0);
        };
        (plan, index.dir.clone())
    };
    // No lock held: whole chunks are rewritten here.
    let merged = TextIndex::merge_runs(&dir, &plan);
    let mut index = shared.write().unwrap();
    Ok(index.apply_merge(&plan, merged))
}

pub(crate) fn index_delete_row(index: &mut TextIndex, seq: u64) {
    index.mem.delete(seq);
    // A row whose text already went to a sealed segment keeps its old postings
    // there; the fresh text is added to the memory tail, so the sealed copy
    // must be ignored from now on.
    if !index.mem.docs.contains_key(&seq) && index.reader.contains(seq) {
        index.overridden.insert(seq);
    }
}

pub struct EngineOptions {
    /// When `false`, catalog DDL becomes durable only at a checkpoint
    /// (`Engine::flush`), reproducing `NEW_CONFIG_NOWAL_PATH`.
    pub wal_enabled: bool,
    /// Tokio exchange capacity per pipeline edge.
    pub exchange_capacity: usize,
    /// The reference's object directory (`pm|object|` + one file per object).
    ///
    /// `Some` makes the engine speak that layout: at open it materialises any
    /// `data/` file the records describe but the directory does not hold, and
    /// at every checkpoint it republishes `data/` as one object. `None` keeps
    /// the plain directory the other gates use.
    pub object_dir: Option<std::path::PathBuf>,
    /// C++ `[storage] snapshot_dir`. `None` puts snapshots under the data
    /// root, which is what an engine opened on a temporary directory wants.
    pub snapshot_dir: Option<std::path::PathBuf>,
}

impl Default for EngineOptions {
    fn default() -> Self {
        Self {
            wal_enabled: true,
            exchange_capacity: 4,
            object_dir: None,
            snapshot_dir: None,
        }
    }
}

pub(crate) type AllocJob = Box<dyn FnOnce(&Engine) + Send>;

/// Depth of the row allocator's queue.
///
/// The same size the background-task queue uses (`DEFAULT_BLOCKING_QUEUE_SIZE`).
/// An append waits for its answer, so the queue only holds jobs the allocator
/// has not picked up yet; the bound exists so a stalled allocator cannot make a
/// burst of appends grow it without limit.
pub(crate) const ALLOC_QUEUE_CAPACITY: usize = 1024;

/// A reserved run of row ids and the append record that describes it.
///
/// The record is what the block's `version` file needs (`BlockVersion::Append`
/// carries the append's commit ts), and it has to be named by index: another
/// statement may append between this one's allocation and its bottom half.
#[derive(Clone, Copy, Debug)]
pub(crate) struct RowRange {
    pub(crate) base: usize,
    pub(crate) append: usize,
}

impl RowRange {
    pub(crate) fn single(base: usize) -> Self {
        Self {
            base,
            append: usize::MAX,
        }
    }
}

/// `TxnAllocator`: the single thread that hands out row ids.
///
/// The queue is a bounded `tokio` channel, and **both ends are now async**: the
/// consumer is a runtime task that drains with `recv().await`, and the
/// committers - which are all async (`run_bottom`, `insert_literals_inner`,
/// `copy_from_inner`) - send with `send().await`. A job body only touches the
/// in-memory row counter, so nothing on this path needs a blocking thread.
pub(crate) struct Allocator {
    pub(crate) jobs: tokio::sync::mpsc::Sender<AllocJob>,
}

/// Where one `COPY` stands inside the segment it opened.
/// The reference's import claims whole segments up front, so nothing else can
/// allocate inside them; a port that re-read the table's shared `row_counter`
/// per batch would start its next block after another writer's rows.
#[derive(Clone, Debug)]
pub(crate) struct ImportCursor {
    pub(crate) segment: u32,
    pub(crate) offset: u32,
    /// Rows per batch, in stream order. The append records they become are
    /// written **inside the commit** (`PrepareCommitImport` records each block
    /// in the transaction), so a statement that never commits leaves none.
    pub(crate) batches: Vec<usize>,
}

/// How many of the **last** append records a bottom half has to stamp with its
/// commit ts (`None` when the transaction staged no appends of its own).
///
/// It was an enum with a `None` variant until every producer that appends now
/// stamps its own record - an unstamped record is a row `committed_rows` does
/// not see, which is exactly the bug that variant caused.
pub(crate) type AppendRecords = Option<usize>;

/// Renames the projection's output columns with the select-list aliases.
pub(crate) fn apply_select_aliases(plan: &mut LogicalPlan, aliases: &[Option<String>]) {
    match plan {
        LogicalPlan::Projection { names, .. } => {
            for (n, a) in names.iter_mut().zip(aliases.iter()) {
                if let Some(a) = a {
                    *n = a.clone();
                }
            }
        }
        LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. }
        | LogicalPlan::Filter { input, .. } => apply_select_aliases(input, aliases),
        _ => {}
    }
}

/// Parses a bare predicate string (`num != 98 AND num != 12`) by wrapping it
/// in a statement; used by the per-search `filter=` option.
pub(crate) fn parse_filter_predicate(text: &str) -> Result<parser::Predicate> {
    let sql = format!("SELECT 1 FROM t WHERE {text};");
    match parser::parse(&sql)?.as_slice() {
        [parser::Statement::Select {
            where_pred: Some(p),
            ..
        }] => {
            // The option string is opaque text parsed here, not by the row
            // resolver, so a subquery would compile to an unknown function and
            // evaluate to NULL (silently dropping matches). Reject it.
            if p.contains_subquery() {
                return Err(Error::Invalid(
                    "subqueries are not supported in a search filter".into(),
                ));
            }
            Ok(p.clone())
        }
        _ => Err(Error::Invalid(format!("invalid filter '{text}'"))),
    }
}

/// The predicate `Filter(Scan)` applies above a scan, if the plan has one.
pub(crate) fn scan_predicate_of(plan: &LogicalPlan) -> Option<Expr> {
    match plan {
        LogicalPlan::Filter { input, predicate } => match &**input {
            LogicalPlan::Scan { .. } => Some(predicate.clone()),
            _ => scan_predicate_of(input),
        },
        LogicalPlan::Projection { input, .. }
        | LogicalPlan::Aggregate { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => scan_predicate_of(input),
        _ => None,
    }
}

/// Maps a declared column type onto the index-scan `ColumnKind`.
pub(crate) fn column_kind_of(declared: &str) -> planner::index_scan::ColumnKind {
    use planner::index_scan::ColumnKind;
    let ty = declared.to_ascii_lowercase();
    let ty = ty.trim();
    if ty.starts_with("datetime") {
        ColumnKind::DateTime
    } else if ty.starts_with("timestamp") {
        ColumnKind::Timestamp
    } else if ty.starts_with("date") {
        ColumnKind::Date
    } else if ty.starts_with("time") {
        ColumnKind::Time
    } else if ty.starts_with("bool") {
        ColumnKind::Boolean
    } else if ty.starts_with("tinyint") {
        ColumnKind::TinyInt
    } else if ty.starts_with("smallint") {
        ColumnKind::SmallInt
    } else if ty.starts_with("bigint") {
        ColumnKind::BigInt
    } else if ty.starts_with("int") || ty.starts_with("integer") {
        ColumnKind::Integer
    } else if ty.starts_with("double") {
        ColumnKind::Double
    } else if ty.starts_with("float") {
        ColumnKind::Float
    } else if ty.starts_with("json") {
        ColumnKind::Json
    } else {
        // varchar and everything else the secondary index stores as text
        ColumnKind::Varchar
    }
}

/// The secondary-chunk key kind of a declared column type — the reference's
/// `ConvertToOrderedType<T>` (`secondary_index_data.cppm:38-118`): `FLOAT` and
/// `DOUBLE` keep their raw, ordered value (`KeepOrderedSelf`), `INT`/`UINT` are
/// order-mapped and everything else is hashed to a `u64` (`U64` here), which is
/// the reference's `ConvertToHashU64` for `VARCHAR`.
pub(crate) fn secondary_key_kind(declared: &str) -> storage::secondary_segment::KeyKind {
    use storage::secondary_segment::KeyKind;
    let ty = declared.trim().to_ascii_lowercase();
    if ty.starts_with("float") {
        KeyKind::F32
    } else if ty.starts_with("double") {
        KeyKind::F64
    } else if ty.starts_with("datetime") || ty.starts_with("timestamp") {
        // `ConvertToOrderedI64`: `GetEpochTime()`, an ordered `i64`.
        KeyKind::I64
    } else if ty.starts_with("date") || ty.starts_with("time") {
        // `ConvertToOrderedI32`: `DateT`/`TimeT::GetValue()`, an `i32`.
        KeyKind::I32
    } else if ty.starts_with("bigint") {
        // `KeepOrderedSelf<BigIntT>`: the raw `i64`.
        KeyKind::I64
    } else if ty.starts_with("smallint") {
        // `KeepOrderedSelf<SmallIntT>`: `int16_t`, a 2-byte key.
        KeyKind::I16
    } else if ty.starts_with("tinyint") {
        // `KeepOrderedSelf<TinyIntT>`: `int8_t`, a 1-byte key.
        KeyKind::I8
    } else if ty.starts_with("bool") {
        // `KeepOrderedSelf<BooleanT>`: a 1-byte key. Only ever written as a
        // low-cardinality chunk, the reference's own BOOL dispatch.
        KeyKind::Bool
    } else if ty.starts_with("json") {
        // `KeepOrderedString<JsonTermT>`: the column is expanded into one term
        // per path/scalar (`storage::json_index::flatten_json`), and only ever
        // stored as a low-cardinality chunk.
        KeyKind::Bytes
    } else if ty.starts_with("int") {
        // `KeepOrderedSelf<IntegerT>`: the raw `i32`. Storing the value
        // itself (not an order-mapped `u64`) is what makes the chunk
        // byte-identical to the reference's.
        KeyKind::I32
    } else {
        KeyKind::U64
    }
}

/// How a stored cell becomes a chunk's ordered key (`ConvertToOrdered`,
/// `secondary_index_data.cppm:37-72`).
///
/// This is *not* [`secondary_key_kind`]: `INTEGER` and `DATE` share the `i32`
/// chunk width but read the cell differently - the value itself versus the day
/// count - so every place that turns a cell into a key (or a key back into a
/// comparison) has to know which one it has.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) enum KeyDomain {
    /// `KeepOrderedSelf` over an integral column: the value is the key.
    Integer,
    /// `KeepOrderedSelf` over `FloatT`/`DoubleT`: the value is the key.
    Float,
    /// `ConvertToOrderedI32`/`I64`: the day count or the epoch time.
    Temporal,
    /// `ConvertToHashU64`: `std::hash` of the varchar/json text.
    Hashed,
}

pub(crate) fn key_domain(declared: &str) -> KeyDomain {
    let ty = declared.trim().to_ascii_lowercase();
    if ty.starts_with("float") || ty.starts_with("double") {
        KeyDomain::Float
    } else if ty.starts_with("date")
        || ty.starts_with("time")
        || ty.starts_with("datetime")
        || ty.starts_with("timestamp")
    {
        KeyDomain::Temporal
    } else if ty.starts_with("int")
        || ty.starts_with("uint")
        || ty.starts_with("tinyint")
        || ty.starts_with("smallint")
        || ty.starts_with("bigint")
    {
        // `INTEGER`, `BIGINT`, `TINYINT`, `SMALLINT` and the port's `UINT`.
        // The narrow spellings have to be listed: `"tinyint"` does not start
        // with `"int"`, and mis-classifying it as `Hashed` made the writer
        // store `std::hash(value)` while `secondary_key_kind` handed the reader
        // an `I8` bound, so every range on such a column came back empty.
        KeyDomain::Integer
    } else {
        KeyDomain::Hashed
    }
}

/// An integer column's chunk key. `INTEGER` and `BIGINT` keep the value itself
/// at the reference's width; anything else (a width the reference's
/// `ConvertToOrdered` does not spell, e.g. `TINYINT`) falls back to the
/// order-mapped `u64`.
pub(crate) fn integer_secondary_key(declared: &str, value: i64) -> Key {
    match secondary_key_kind(declared) {
        KeyKind::I32 => Key::I32(value as i32),
        KeyKind::I64 => Key::I64(value),
        KeyKind::I16 => Key::I16(value as i16),
        KeyKind::I8 => Key::I8(value as i8),
        KeyKind::Bool => Key::Bool(value != 0),
        KeyKind::U64 => Key::U64(ordered_i64_key(value)),
        // A float/double column has no integral key; the caller picked the
        // wrong conversion.
        KeyKind::F32 | KeyKind::F64 => Key::F64(value as f64),
        // A JSON column's key is a term, not an integer.
        KeyKind::Bytes => Key::U64(0),
    }
}

/// A cell's stored text as its column's ordered key — the reference's
/// `ConvertToOrderedKeyValue` for a value that reached the index as text.
///
/// This is what the memory tail stores (`Key::ordered_bytes`), so the tail is
/// ordered like the column: a numeric range is a `BTreeMap::range`, not a walk
/// that parses every key back into a number.
pub(crate) fn text_to_key(declared: &str, text: &str) -> Option<Key> {
    // `BooleanT` is its own ordered key: the low-cardinality writer stores the
    // value as a 0/1 key (`SecondaryIndexDataLowCardinalityImpl<bool>`).
    if declared.trim().to_ascii_lowercase().starts_with("bool") {
        return Some(Key::Bool(matches!(text.trim(), "true" | "1")));
    }
    let key_kind = secondary_key_kind(declared);
    if key_kind == KeyKind::Bytes {
        // A JSON cell has no single ordered key: it becomes several terms, see
        // [`text_to_tail_keys`].
        return None;
    }
    match key_domain(declared) {
        KeyDomain::Integer => Some(integer_secondary_key(
            declared,
            text.trim().parse::<i64>().ok()?,
        )),
        KeyDomain::Float => {
            let value = text.trim().parse::<f64>().ok()?;
            Some(match key_kind {
                KeyKind::F32 => Key::F32(value as f32),
                _ => Key::F64(value),
            })
        }
        KeyDomain::Temporal => Some(match key_kind {
            KeyKind::I32 => Key::I32(temporal_key(declared, text)? as i32),
            _ => Key::I64(temporal_key(declared, text)?),
        }),
        // `ConvertToHashU64`: a varchar (and, until the JSON term encoding is
        // ported, a json document) is keyed by `std::hash` of its text.
        KeyDomain::Hashed => Some(Key::U64(storage::secondary_segment::hash_varchar(text))),
    }
}

/// A stored cell as the **memory tail's keys**.
///
/// Every column contributes one key - the order-preserving encoding of its
/// value - except a JSON document, which the reference expands into one term
/// per path/scalar and writes into a low-cardinality chunk. Those terms are the
/// tail keys here, so a dump can hand them straight to the term array.
pub(crate) fn value_to_tail_keys(declared: &str, value: &Value) -> Vec<Vec<u8>> {
    // `SecondaryIndexInMemT::InsertInner` skips a row whose null bit is set
    // (`secondary_index_in_mem_impl.cpp:181-184`): a NULL is not a key, so a
    // predicate that admits it can only come from the row set, never the index.
    if matches!(value, Value::Null) {
        return Vec::new();
    }
    let text = match value {
        Value::Str(text) => text.clone(),
        other => other.to_slt_string(),
    };
    text_to_tail_keys(declared, &text)
}

/// [`value_to_tail_keys`] for a cell that already reached the index as text.
pub(crate) fn text_to_tail_keys(declared: &str, text: &str) -> Vec<Vec<u8>> {
    if secondary_key_kind(declared) == KeyKind::Bytes {
        return storage::json_index::flatten_json_terms(text)
            .into_iter()
            .map(String::into_bytes)
            .collect();
    }
    text_to_key(declared, text)
        .map(|key| key.ordered_bytes())
        .into_iter()
        .collect()
}

/// One indexed column of a table resolved for a batch of rows: where the value
/// sits in the row, which type fixes its key domain, and — for a functional
/// index — the scalar function to apply before encoding.
pub(crate) struct SecondaryTarget {
    pub(crate) pos: usize,
    pub(crate) declared: String,
    pub(crate) index: std::sync::Arc<SecondaryIndex>,
    pub(crate) func: Option<String>,
}

/// A functional secondary index (`SECONDARYFUNCTIONAL`): the catalog keeps the
/// expression as the index's column, and this record says how to evaluate it.
///
/// The reference stores the bound expression (`function_expression`) and the
/// base column (`column_names`) and evaluates it per column vector
/// (`NewTxn::ExecuteFunctionExpression`, `new_txn_index_impl.cpp:2032`); the
/// port re-evaluates the same scalar function row by row.
#[derive(Debug, Clone)]
pub(crate) struct FunctionalIndex {
    /// The column the function reads.
    pub(crate) base: String,
    /// The scalar function, lower-cased (`lower`, `length`, `abs`, …).
    pub(crate) func: String,
    /// The function's declared return type, which fixes the key domain
    /// (`func_return_type` in the reference's index record).
    pub(crate) returns: String,
}

/// The `(function, base column)` a functional index expression names:
/// `lower(name)` -> `("lower", "name")`.
///
/// `logical_planner_impl.cpp:941-960` accepts exactly one column argument, no
/// nested call and no other argument shape; anything else is rejected when the
/// index is created.
pub(crate) fn functional_parts(expr: &str) -> Option<(String, String)> {
    let expr = expr.trim();
    let open = expr.find('(')?;
    if !expr.ends_with(')') {
        return None;
    }
    let name = expr[..open].trim().to_ascii_lowercase();
    let inner = expr[open + 1..expr.len() - 1].trim();
    if name.is_empty() || inner.is_empty() || inner.contains('(') || inner.contains(',') {
        return None;
    }
    if !inner.chars().all(|c| c.is_ascii_alphanumeric() || c == '_') {
        return None;
    }
    Some((name, inner.to_ascii_lowercase()))
}

/// `func_return_type` of a functional index: the type the scalar function yields
/// for one column value, spelled as a declared type the key encoder understands.
///
/// The reference binds the expression and takes `FunctionExpression::Type()`
/// (`logical_planner_impl.cpp:976`); the port maps the functions whose result
/// type is fixed by the name (and, for `abs`, by the argument's type).
pub(crate) fn functional_return_type(func: &str, base_type: &str) -> Option<&'static str> {
    match func {
        "lower" | "upper" | "trim" | "ltrim" | "rtrim" | "replace" | "concat" => Some("VARCHAR"),
        "length" | "char_length" | "strpos" | "char_position" | "instr" => Some("BIGINT"),
        "abs" => {
            let base = base_type.trim().to_ascii_uppercase();
            Some(if base.starts_with("FLOAT") || base.starts_with("DOUBLE") {
                "DOUBLE"
            } else {
                "BIGINT"
            })
        }
        "round" | "floor" | "ceil" | "sqrt" | "exp" | "ln" | "log10" | "pow" | "power" => {
            Some("DOUBLE")
        }
        _ => None,
    }
}

/// One row's functional-index value: `func(base)` evaluated on the row.
pub(crate) fn functional_value(func: &str, pos: usize, row: &[Value]) -> Option<Value> {
    let expr = expression::Expr::Func {
        name: func.to_string(),
        args: vec![expression::Expr::Column(pos)],
    };
    let value = expr.eval(row);
    // `InsertInner` skips a NULL cell; a function that yields one has no key.
    (!matches!(value, Value::Null)).then_some(value)
}

/// A computed value as its column's ordered key: the same encoding
/// [`value_to_tail_keys`] applies, without the text round-trip (a computed
/// `DOUBLE` would lose precision on the way through the slt rendering).
pub(crate) fn value_to_key(declared: &str, value: &Value) -> Vec<Vec<u8>> {
    use storage::secondary_segment::{Key, KeyKind};
    if matches!(value, Value::Null) {
        return Vec::new();
    }
    let key = match value {
        Value::Str(text) => return text_to_tail_keys(declared, text),
        Value::Int(i) => integer_secondary_key(declared, *i),
        Value::Float(f) => match secondary_key_kind(declared) {
            KeyKind::F32 => Key::F32(*f as f32),
            _ => Key::F64(*f),
        },
        Value::Bool(flag) => Key::Bool(*flag),
        other => return text_to_tail_keys(declared, &other.to_slt_string()),
    };
    vec![key.ordered_bytes()]
}

/// The reference's ordered key for a date/time text (`ConvertToOrderedKeyValue`):
/// `DateT::GetValue()` is the day count since 1970, `TimeT::GetValue()` the
/// second of the day, and `DateTimeT`/`TimestampT::GetEpochTime()` is
/// `days * 86400 + seconds`.
pub(crate) fn temporal_key(declared: &str, text: &str) -> Option<i64> {
    let ty = declared.trim().to_ascii_lowercase();
    let text = text.trim();
    let mut date = ::common::types::DateT::default();
    if ty.starts_with("datetime") || ty.starts_with("timestamp") {
        let (date_text, time_text) = text.split_once(' ')?;
        date.from_string(date_text.trim()).ok()?;
        Some(i64::from(date.value) * 86_400 + i64::from(seconds_since_midnight(time_text)?))
    } else if ty.starts_with("time") {
        Some(i64::from(seconds_since_midnight(text)?))
    } else {
        date.from_string(text).ok()?;
        Some(i64::from(date.value))
    }
}

/// `HH:MM:SS[.ffffff]` -> seconds since midnight, the `TimeT` value.
pub(crate) fn seconds_since_midnight(text: &str) -> Option<i32> {
    let mut parts = text.trim().split(':');
    let hour: i32 = parts.next()?.trim().parse().ok()?;
    let minute: i32 = parts.next()?.trim().parse().ok()?;
    let second: i32 = parts.next()?.trim().split('.').next()?.parse().ok()?;
    if !(0..24).contains(&hour) || !(0..60).contains(&minute) || !(0..60).contains(&second) {
        return None;
    }
    Some(hour * 3600 + minute * 60 + second)
}

/// A range bound as a chunk key of `kind`.
pub(crate) fn bound_key(
    kind: storage::secondary_segment::KeyKind,
    declared: &str,
    value: &Value,
) -> Option<storage::secondary_segment::Key> {
    use storage::secondary_segment::{Key, KeyKind};
    match kind {
        KeyKind::U64 => match value {
            Value::Int(i) => Some(Key::U64(ordered_i64_key(*i))),
            _ => None,
        },
        // A float bound stays `f64`: `Key::order` widens an `F32` key, so a
        // bound that `f32` cannot represent must not clip a boundary row.
        KeyKind::F32 | KeyKind::F64 => value_as_f64(value).map(Key::F64),
        // An integer column's bound is the literal itself.
        KeyKind::I32 if key_domain(declared) == KeyDomain::Integer => match value {
            Value::Int(i) => Some(Key::I32(
                (*i).clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32,
            )),
            _ => None,
        },
        KeyKind::I64 if key_domain(declared) == KeyDomain::Integer => match value {
            Value::Int(i) => Some(Key::I64(*i)),
            _ => None,
        },
        // The planner's open ends are `i64::MIN`/`i64::MAX`, and those must
        // clamp to the *i32* limits: `i64::MAX as i32` is -1 and
        // `i64::MIN as i32` is 0, either of which would invert or empty the
        // interval.
        //
        // A strict bound (`d < '1970-01-01'`) is declined by `temporal_key`:
        // the planner rewrites it to `<=` its lexicographic predecessor, which
        // is not a date. The residual filter still answers it, so the rows stay
        // right and only the acceleration is lost.
        KeyKind::I32 => temporal_bound(declared, value)
            .map(|v| Key::I32(v.clamp(i64::from(i32::MIN), i64::from(i32::MAX)) as i32)),
        KeyKind::I64 => temporal_bound(declared, value).map(Key::I64),
        KeyKind::I8 => match value {
            Value::Int(i) => Some(Key::I8(
                (*i).clamp(i64::from(i8::MIN), i64::from(i8::MAX)) as i8
            )),
            _ => None,
        },
        KeyKind::I16 => match value {
            Value::Int(i) => Some(Key::I16(
                (*i).clamp(i64::from(i16::MIN), i64::from(i16::MAX)) as i16,
            )),
            _ => None,
        },
        // A BOOL column is a low-cardinality chunk; its predicate reaches the
        // index as an equality, but an interval still has to convert.
        KeyKind::Bool => match value {
            Value::Int(i) => Some(Key::Bool(*i != 0)),
            Value::Str(text) => Some(Key::Bool(matches!(
                text.trim().to_ascii_lowercase().as_str(),
                "true" | "1"
            ))),
            _ => None,
        },
        // A JSON path range is answered by the low-cardinality term array, not
        // by an ordered-key chunk.
        KeyKind::Bytes => None,
    }
}

/// A query bound as the memory tail's ordered bytes.
///
/// [`bound_key`] widens a float bound to `f64` so that a chunk's `f32`
/// comparison cannot clip a boundary row; the tail stores the column's own
/// width, so the bound is narrowed back here. Narrowing rounds monotonically, so
/// a row that satisfies the bound still falls inside the narrowed interval —
/// any extra candidates the rounding adds are removed by the residual filter.
pub(crate) fn tail_bound(
    kind: storage::secondary_segment::KeyKind,
    key: storage::secondary_segment::Key,
) -> Vec<u8> {
    use storage::secondary_segment::{Key, KeyKind};
    match (kind, key) {
        (KeyKind::F32, Key::F64(v)) => Key::F32(v as f32).ordered_bytes(),
        _ => key.ordered_bytes(),
    }
}

/// A date/time bound: the planner passes the literal's text for a finite bound
/// and `Value::Int(i64::MIN/MAX)` for an open end.
pub(crate) fn temporal_bound(declared: &str, value: &Value) -> Option<i64> {
    match value {
        Value::Int(i) => Some(*i),
        Value::Str(text) => temporal_key(declared, text),
        _ => None,
    }
}

/// Order-preserving map of a signed integer into `u64`: `i64::MIN` -> 0.
pub(crate) fn ordered_i64_key(value: i64) -> u64 {
    (value as u64) ^ (1u64 << 63)
}

/// A range bound as `f64`: the planner emits `Value::Float` for a float
/// column and `Value::Int` for an integer one, and both appear in a
/// comparison against a float chunk after a cast was unwound.
pub(crate) fn value_as_f64(value: &Value) -> Option<f64> {
    match value {
        Value::Float(f) => Some(*f),
        Value::Int(i) => Some(*i as f64),
        _ => None,
    }
}

/// The first base scan a (single-table) plan reads, with its pruned columns.
pub(crate) fn plan_scan_of(plan: &LogicalPlan) -> Option<(String, Vec<String>)> {
    match plan {
        LogicalPlan::Scan { table, columns } | LogicalPlan::IndexScan { table, columns, .. } => {
            Some((table.clone(), columns.clone()))
        }
        LogicalPlan::Filter { input, .. }
        | LogicalPlan::Projection { input, .. }
        | LogicalPlan::Aggregate { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => plan_scan_of(input),
        LogicalPlan::Join { .. } | LogicalPlan::Values { .. } => None,
    }
}

/// The index-scan node's candidate, when the plan has one.
pub(crate) fn plan_index_candidate(
    plan: &LogicalPlan,
) -> Option<std::sync::Arc<planner::index_scan::IndexScanCandidate>> {
    match plan {
        LogicalPlan::IndexScan { candidate, .. } => Some(candidate.clone()),
        LogicalPlan::Filter { input, .. }
        | LogicalPlan::Projection { input, .. }
        | LogicalPlan::Aggregate { input, .. }
        | LogicalPlan::Sort { input, .. }
        | LogicalPlan::Limit { input, .. }
        | LogicalPlan::Offset { input, .. } => plan_index_candidate(input),
        _ => None,
    }
}

/// Rewrites each row of the unpruned `full` layout into the pruned `columns`
/// layout (C++ `BindingRemapper` for the scan's output).
pub(crate) fn align_rows(
    rows: Vec<Vec<Value>>,
    columns: &[String],
    full: &[String],
) -> Vec<Vec<Value>> {
    if columns.len() == full.len() {
        return rows;
    }
    let Some(first) = rows.first() else {
        return rows;
    };
    if first.len() != full.len() {
        // The caller materialised a different layout (a projected read); leave
        // it alone.
        return rows;
    }
    let positions: Vec<Option<usize>> = columns
        .iter()
        .map(|column| full.iter().position(|name| name == column))
        .collect();
    if positions.iter().any(Option::is_none) {
        return rows;
    }
    rows.into_iter()
        .map(|row| {
            positions
                .iter()
                .map(|i| {
                    row.get(i.expect("checked above"))
                        .cloned()
                        .unwrap_or(Value::Null)
                })
                .collect()
        })
        .collect()
}

/// A secondary index over already-materialised rows: the evaluator supplies the
/// inclusive range, this reports the rows whose key falls inside it.
pub(crate) struct RowSecondaryProvider<'a> {
    pub(crate) rows: &'a [Vec<Value>],
    /// pruned scan position -> position in the materialised row
    pub(crate) positions: &'a [usize],
    /// The candidate's columns, in the evaluator's own order: `column_id` indexes
    /// this list, and the name is what the engine's index registry knows.
    pub(crate) columns: Option<&'a [String]>,
    /// The functional index the predicate was pushed down for, if any: the
    /// registry keys it by its expression (`sqrt(c1)`), while `column_id` only
    /// names the base column, whose values are not what the index stores.
    pub(crate) functional: Option<String>,
    pub(crate) engine: Option<&'a Engine>,
    pub(crate) table: &'a str,
    /// storage sequence id -> position in `rows`. The secondary index is keyed
    /// by the id, so a hit can only select a row the scan holds.
    pub(crate) row_positions: Option<&'a std::collections::HashMap<usize, u32>>,
}

impl planner::index_scan::SecondaryIndexProvider for RowSecondaryProvider<'_> {
    fn range_rows(&self, column_id: usize, start: &Value, end: &Value) -> Vec<u32> {
        // The column's own index answers first: the reference's
        // `IndexFilterEvaluatorSecondary` reads the chunk bitmaps
        // (`index_filter_evaluators_impl.cpp:912`) instead of re-reading rows.
        // A column whose key has no order (a hashed VARCHAR) or that carries no
        // index reports `None`, and the row walk below is the fallback.
        if let (Some(engine), Some(columns), Some(positions)) =
            (self.engine, self.columns, self.row_positions)
        {
            // A functional index is keyed by its expression, which is not the
            // base column `column_id` names.
            let column = self
                .functional
                .clone()
                .or_else(|| columns.get(column_id).cloned());
            if let Some(column) = column {
                if let Some(seqs) =
                    engine.index_range_lookup(self.table, &column, &[(start.clone(), end.clone())])
                {
                    let mut out = Vec::new();
                    for seq in seqs {
                        if let Some(position) = positions.get(&(seq as usize)) {
                            out.push(*position);
                        }
                    }
                    return out;
                }
            }
            // A functional predicate the index could not answer must not fall
            // through to the row walk: it compares the base column's own text
            // against bounds written for the function's output, which silently
            // drops every row whose text differs from its image (`lower`).
            if self.functional.is_some() {
                return (0..self.rows.len() as u32).collect();
            }
        }
        let mut out = Vec::new();
        let position = self.positions.get(column_id).copied().unwrap_or(column_id);
        for (i, row) in self.rows.iter().enumerate() {
            let Some(value) = row.get(position) else {
                continue;
            };
            // The evaluator only narrows the rows the residual predicate then
            // re-checks, so an inconclusive comparison (a NULL or a value the
            // index cannot order) must keep the row rather than drop it.
            let inside = match (value.partial_cmp_value(start), value.partial_cmp_value(end)) {
                (Some(low), Some(high)) => {
                    low != std::cmp::Ordering::Less && high != std::cmp::Ordering::Greater
                }
                _ => true,
            };
            if inside {
                out.push(i as u32);
            }
        }
        out
    }
}

/// A full-text index over already-materialised rows. `postings` answers from
/// the column's inverted index (`ColumnIndexReader::Lookup` in the reference),
/// exactly like `IndexFilterEvaluatorFulltext`, so the filter narrows the
/// candidates instead of waving every row through.
pub(crate) struct RowFulltextProvider<'a> {
    pub(crate) rows: &'a [Vec<Value>],
    pub(crate) column: usize,
    /// The storage sequence id of each row, aligned with `rows`. The inverted
    /// index is keyed by it, so a posting can only select a row the scan
    /// actually materialised (a stale posting must not admit a foreign row).
    pub(crate) row_ids: Option<&'a [usize]>,
    /// The column's inverted index and the tokeniser the filter asks for. The
    /// index was built with the analyzer its column was created with; a column
    /// may carry two full-text indexes, so only a matching tokeniser proves it
    /// can answer this filter.
    pub(crate) index: Option<&'a (SharedTextIndex, TextTokenizer)>,
    /// A sparse-gram index matches a literal run contiguously; a term index
    /// matches whole tokens.
    pub(crate) substring: bool,
}

impl RowFulltextProvider<'_> {
    /// C++ `ColumnIndexReader::Lookup(term, false)`: the row ids whose documents
    /// contain `term`, with stale copies of re-indexed rows removed.
    pub(crate) fn index_postings(&self, term: &str) -> Option<std::collections::HashSet<usize>> {
        let (index, tokenizer) = self.index?;
        self.row_ids?;
        let guard = index.read().ok()?;
        if &guard.tokenizer != tokenizer {
            return None;
        }
        let merged = guard.merge(&[term.to_string()]);
        Some(
            merged
                .postings
                .get(term)
                .map(|list| list.iter().map(|(doc, _)| *doc as usize).collect())
                .unwrap_or_default(),
        )
    }
}

impl planner::index_scan::FulltextIndex for RowFulltextProvider<'_> {
    fn postings(&self, term: &str) -> Vec<u32> {
        let needle = term.to_lowercase();
        // The term index answers from the postings; a sparse-gram index
        // (seeded by `RegexIndexRewrite`, or named with `col@index`) is a
        // literal-run test over the text.
        let postings = self.index_postings(&needle);
        let mut out = Vec::new();
        for (i, row) in self.rows.iter().enumerate() {
            let hit = if let (Some(postings), Some(ids)) = (&postings, self.row_ids) {
                ids.get(i).is_some_and(|id| postings.contains(id))
            } else if self.substring {
                let text = match row.get(self.column) {
                    Some(Value::Str(s)) => s.clone(),
                    Some(other) => other.to_slt_string(),
                    None => continue,
                };
                text.to_lowercase().contains(&needle)
            } else {
                // No usable index for this filter: keep the row and let the
                // residual predicate decide rather than dropping it.
                true
            };
            if hit {
                out.push(i as u32);
            }
        }
        out
    }
}

/// The options text of the first `filter_fulltext` expression in `expr`.
pub(crate) fn fulltext_options(expr: &Expr) -> Option<&str> {
    match expr {
        Expr::FullText(_, _, options) => Some(options.as_str()),
        Expr::And(a, b) | Expr::Or(a, b) => fulltext_options(a).or_else(|| fulltext_options(b)),
        Expr::Not(a) => fulltext_options(a),
        _ => None,
    }
}

/// The column a `filter_fulltext` expression reads, if `expr` contains one.
pub(crate) fn fulltext_column(expr: &Expr) -> Option<usize> {
    match expr {
        Expr::FullText(i, _, _) => Some(*i),
        Expr::And(a, b) | Expr::Or(a, b) => fulltext_column(a).or_else(|| fulltext_column(b)),
        Expr::Not(a) => fulltext_column(a),
        _ => None,
    }
}

/// Applies the index-scan candidate to the fetched rows (C++ `LogicalIndexScan`
/// yields exactly the rows its evaluator selects).
#[allow(clippy::too_many_arguments)] // one argument per planner hint the filter reads
pub(crate) fn apply_index_filter(
    rows: Vec<Vec<Value>>,
    row_ids: Option<&[usize]>,
    fulltext_index: Option<&(SharedTextIndex, TextTokenizer)>,
    candidate: &planner::index_scan::IndexScanCandidate,
    positions: &[usize],
    fulltext_substring: bool,
    engine: Option<&Engine>,
    table: &str,
    index_columns: Option<&[String]>,
) -> Result<Vec<Vec<Value>>> {
    if rows.is_empty() {
        return Ok(rows);
    }
    let row_count = rows.len() as u32;
    let row_positions: Option<std::collections::HashMap<usize, u32>> = row_ids.map(|ids| {
        ids.iter()
            .enumerate()
            .map(|(i, id)| (*id, i as u32))
            .collect()
    });
    // The evaluator's own expression decides which registered index answers a
    // functional predicate; `column_id` only names its base column.
    let functional = match (&candidate.evaluator, engine, index_columns) {
        (
            planner::index_scan::IndexFilterEvaluator::Secondary(secondary),
            Some(engine),
            Some(columns),
        ) => engine.functional_expression_for(table, &secondary.src_expr, columns),
        _ => None,
    };
    let secondary = RowSecondaryProvider {
        rows: &rows,
        positions,
        columns: index_columns,
        functional,
        engine,
        table,
        row_positions: row_positions.as_ref(),
    };
    let mut ctx = planner::index_scan::IndexEvalContext::new(row_count).with_secondary(&secondary);
    let fulltext = RowFulltextProvider {
        rows: &rows,
        column: fulltext_column(&candidate.index_filter)
            .and_then(|i| positions.get(i).copied())
            .unwrap_or(0),
        row_ids,
        index: fulltext_index,
        substring: fulltext_substring,
    };
    ctx = ctx.with_fulltext(&fulltext);
    let mask = candidate.evaluator.evaluate(&ctx)?;
    // The evaluator decides which rows the index scan yields (C++
    // `PhysicalIndexScan`): an all-false mask means the scan produces nothing,
    // even when the residual predicate would have accepted the row. Keeping
    // every row here turned the filter into a no-op.
    if mask.count_true() == 0 {
        return Ok(Vec::new());
    }
    Ok(rows
        .into_iter()
        .enumerate()
        .filter(|(i, _)| mask.is_true(*i as u32))
        .map(|(_, row)| row)
        .collect())
}

pub(crate) fn nested_loop_join(
    left: &[Vec<Value>],
    right: &[Vec<Value>],
    left_key: usize,
    right_key: usize,
    left_width: usize,
) -> Vec<Vec<Value>> {
    let mut out = Vec::new();
    for l in left {
        let lk = l.get(left_key);
        for r in right {
            if let (Some(a), Some(b)) = (lk, r.get(right_key)) {
                if a == b {
                    let mut row = l.clone();
                    row.extend(r.iter().cloned());
                    out.push(row);
                }
            }
        }
    }
    let _ = left_width;
    out
}

/// Resolves a data file path, mapping Infinity's fixture prefix
/// `/var/infinity/test_data/…` onto `INFINITY_DATA_DIR` (searched recursively).
pub(crate) fn resolve_data_path(path: &str) -> PathBuf {
    let p = PathBuf::from(path);
    if p.exists() {
        return p;
    }
    if let Ok(base) = std::env::var("INFINITY_DATA_DIR") {
        let base = PathBuf::from(base);
        if let Some(name) = p.file_name() {
            if let Some(found) = find_file(&base, name) {
                return found;
            }
        }
        let joined = base.join(path.trim_start_matches('/'));
        if joined.exists() {
            return joined;
        }
    }
    p
}

pub(crate) fn find_file(dir: &Path, name: &std::ffi::OsStr) -> Option<PathBuf> {
    for e in std::fs::read_dir(dir).ok()?.flatten() {
        let path = e.path();
        if path.is_dir() {
            if let Some(f) = find_file(&path, name) {
                return Some(f);
            }
        } else if path.file_name() == Some(name) {
            return Some(path);
        }
    }
    None
}

/// Splits one CSV line, honouring double-quoted fields (with `""` escapes).
pub(crate) fn csv_fields(line: &str, delim: char) -> Vec<String> {
    // Fast path: no quote anywhere means every field is a plain slice of the
    // line, so nothing has to be re-encoded character by character. A
    // `COPY` of a numeric/text file hits this for all 8.4 M rows.
    if !line.as_bytes().contains(&b'"') {
        // Exactly what the character loop below produces for this input: the
        // field text is copied verbatim (the loop only trims around a quote).
        return line.split(delim).map(|f| f.to_string()).collect();
    }
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_q = false;
    let mut chars = line.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '"' if in_q => {
                if chars.peek() == Some(&'"') {
                    cur.push('"');
                    chars.next();
                } else {
                    in_q = false;
                }
            }
            '"' => {
                // A quote only opens a field when it is the first character
                // (RFC 4180); elsewhere it is literal (e.g. prose in a
                // tab-delimited body column).
                if cur.is_empty() {
                    in_q = true;
                } else {
                    cur.push('"');
                }
            }
            c if c == delim && !in_q => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    out.push(cur);
    out
}

/// Normalises a CSV cell: vector-looking values are re-joined compactly
/// (`[0.1, 0.2]` → `[0.1,0.2]`) so vector rendering is stable.
/// [`normalize_cell`] without the copy when nothing changes.
///
/// A `COPY` of a plain numeric/text file leaves every cell untouched, so the
/// import can borrow the field straight out of the line instead of allocating
/// a `String` per cell - 16.7 M allocations on 8.4 M two-column rows.
pub(crate) fn normalize_cell_cow(s: &str) -> std::borrow::Cow<'_, str> {
    let t = s.trim();
    if t.starts_with('[') && t.ends_with(']') && t.len() >= 2 {
        // Split on commas/spaces, but peel any bracket characters off each token
        // first so numbers adjacent to a bracket are normalised too
        // (`[[0.0, 0.2]]` → `[[0,0.2]]`).
        let mut out = String::from("[");
        let mut first = true;
        for token in t[1..t.len() - 1]
            .split([',', ' '])
            .filter(|x| !x.is_empty())
        {
            if !first {
                out.push(',');
            }
            first = false;
            let lead = token.len() - token.trim_start_matches('[').len();
            let trail = token.len() - token.trim_end_matches(']').len();
            let core = &token[lead..token.len() - trail];
            for _ in 0..lead {
                out.push('[');
            }
            match core.parse::<f64>() {
                Ok(f) => out.push_str(&fmt_num(f)),
                Err(_) => out.push_str(core),
            }
            for _ in 0..trail {
                out.push(']');
            }
        }
        out.push(']');
        return std::borrow::Cow::Owned(out);
    }
    if t.len() == s.len() {
        std::borrow::Cow::Borrowed(s)
    } else {
        std::borrow::Cow::Owned(t.to_string())
    }
}

/// Decodes a COPY `DELIMITER` option (`\t`, `\n`, … or a literal char).
///
/// Mirrors zsv's `zsv_scanner_init`: a NUL delimiter (what an unset SDK
/// option turns into) and the illegal `\n`/`\r`/`"` all fall back to `,`.
pub(crate) fn decode_delimiter(v: &str) -> char {
    match v {
        "\\t" => '\t',
        _ => match v.chars().next() {
            Some(c) if c != '\0' && c != '\n' && c != '\r' && c != '"' => c,
            _ => ',',
        },
    }
}

/// Shortest float rendering (`1.0` → `1`, `0.40` → `0.4`).
/// C++ `std::format("{}", float)`: the shortest round-trip form, in scientific
/// notation when the decimal exponent is below -4 or at least the digit count
/// (`std::to_chars`'s general format). Rust's own `Display` never switches to an
/// exponent, which made a small fvecs element read `0.000052582283` where the
/// reference writes `5.2582283e-05`.
pub(crate) fn fmt_f32_cpp(v: f32) -> String {
    if v == 0.0 {
        return if v.is_sign_negative() { "-0" } else { "0" }.to_string();
    }
    if !v.is_finite() {
        return if v.is_nan() {
            "nan"
        } else if v > 0.0 {
            "inf"
        } else {
            "-inf"
        }
        .to_string();
    }
    fmt_cpp_float(&shortest_sci_f32(v), v.is_sign_negative())
}

/// The same for a `double` cell or a half-precision element widened to `f64`.
pub(crate) fn fmt_f64_cpp(v: f64) -> String {
    if v == 0.0 {
        return if v.is_sign_negative() { "-0" } else { "0" }.to_string();
    }
    if !v.is_finite() {
        return if v.is_nan() {
            "nan"
        } else if v > 0.0 {
            "inf"
        } else {
            "-inf"
        }
        .to_string();
    }
    fmt_cpp_float(&shortest_sci_f64(v), v.is_sign_negative())
}

/// Rewrites a shortest round-trip decimal (`0.47070312`, `5.2582283e-05`) as
/// `d.ddde<exp>` so [`fmt_cpp_float`] can lay it out like `std::to_chars`.
fn sci_from_decimal(text: &str) -> String {
    let text = text.strip_prefix('-').unwrap_or(text);
    let (mantissa, exponent) = match text.split_once(['e', 'E']) {
        Some((m, e)) => (m, e.parse::<i32>().unwrap_or(0)),
        None => (text, 0),
    };
    let (int_part, frac_part) = mantissa.split_once('.').unwrap_or((mantissa, ""));
    let mut digits = String::with_capacity(int_part.len() + frac_part.len());
    digits.push_str(int_part);
    digits.push_str(frac_part);
    let mut exponent = exponent + int_part.len() as i32 - 1;
    let leading = digits.chars().take_while(|c| *c == '0').count();
    if leading == digits.len() {
        return "0e0".to_string();
    }
    digits.drain(..leading);
    exponent -= leading as i32;
    while digits.len() > 1 && digits.ends_with('0') {
        digits.pop();
    }
    if digits.len() == 1 {
        format!("{digits}e{exponent}")
    } else {
        format!("{}.{}e{}", &digits[..1], &digits[1..], exponent)
    }
}

/// The shortest `f32` spelling as `d.ddde<exp>`.
///
/// Rust's own shortest form breaks a decimal tie upward (`0.470703125` →
/// `0.47070313`); `std::to_chars` uses Ryu, which rounds half to even and
/// writes `0.47070312`. The digits come from `ryu` so the two agree.
fn shortest_sci_f32(v: f32) -> String {
    let mut buffer = ryu::Buffer::new();
    sci_from_decimal(buffer.format_finite(v))
}

/// The same for a `double` (and for halves widened to `f64`).
fn shortest_sci_f64(v: f64) -> String {
    let mut buffer = ryu::Buffer::new();
    sci_from_decimal(buffer.format_finite(v))
}

/// Formats the shortest scientific spelling Rust produced (`1.5e-5`) the way
/// `std::to_chars`'s general format does: fixed notation unless the exponent is
/// below -4 or at least the number of significant digits.
fn fmt_cpp_float(sci: &str, negative: bool) -> String {
    let (mantissa, exponent) = sci.split_once('e').expect("exponent");
    let exponent: i32 = exponent.parse().expect("exponent");
    let digits: String = mantissa.chars().filter(char::is_ascii_digit).collect();
    let count = digits.len() as i32;
    if exponent < -4 || exponent >= count {
        let mut out = String::new();
        if negative {
            out.push('-');
        }
        out.push_str(&digits[..1]);
        if count > 1 {
            out.push('.');
            out.push_str(&digits[1..]);
        }
        out.push('e');
        out.push(if exponent < 0 { '-' } else { '+' });
        out.push_str(&format!("{:02}", exponent.abs()));
        return out;
    }
    let mut out = String::new();
    if negative {
        out.push('-');
    }
    if exponent >= 0 {
        let integer = (exponent + 1) as usize;
        if digits.len() <= integer {
            out.push_str(&digits);
            for _ in digits.len()..integer {
                out.push('0');
            }
        } else {
            out.push_str(&digits[..integer]);
            out.push('.');
            out.push_str(&digits[integer..]);
        }
    } else {
        out.push_str("0.");
        for _ in 0..(-exponent - 1) {
            out.push('0');
        }
        out.push_str(&digits);
    }
    out
}

pub(crate) fn fmt_num(f: f64) -> String {
    if f.fract() == 0.0 && f.abs() < 1e15 {
        format!("{}", f as i64)
    } else {
        format!("{f}")
    }
}

/// A positional integer argument (`SEARCH MATCH TEXT (col, query, <topn>, …)`).
/// Only an integer literal counts: a string like `'10'` there is an option.
pub(crate) fn arg_usize(e: &ExprAst) -> Option<usize> {
    match e {
        ExprAst::Literal(Literal::Int(n)) => usize::try_from(*n).ok(),
        _ => None,
    }
}

pub(crate) fn arg_str(e: Option<&ExprAst>) -> Option<String> {
    match e? {
        ExprAst::Literal(Literal::Str(s)) => Some(s.clone()),
        ExprAst::Column(c) => Some(c.clone()),
        _ => None,
    }
}

/// The bare column names of a `MATCH` target: a comma-separated list whose
/// entries may carry a `^boost` suffix (e.g. `doctitle,body^5`).
pub(crate) fn match_columns(target: &str) -> Vec<String> {
    target
        .split(',')
        // `col@index^5` names the column, the index to search it with, and an
        // optional boost.
        .map(|c| {
            let c = c.split('^').next().unwrap_or(c);
            c.split('@').next().unwrap_or(c).trim().to_string()
        })
        .filter(|c| !c.is_empty())
        .collect()
}

pub(crate) fn arg_int(e: &ExprAst) -> Option<i64> {
    match e {
        ExprAst::Literal(Literal::Int(i)) => Some(*i),
        ExprAst::Literal(Literal::Float(f)) => Some(*f as i64),
        _ => None,
    }
}

pub(crate) fn parse_vec_str(s: &str) -> Vec<f64> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    t.split([',', ' '])
        .filter(|x| !x.is_empty())
        .filter_map(|x| x.parse::<f64>().ok())
        .collect()
}

/// Splits a stored multi-vector value into `dims`-sized token rows. Bit
/// columns are scored as hamming and are not part of the MaxSim index.
pub(crate) fn parse_multivector_doc(text: &str, dims: usize) -> Option<Vec<Vec<f32>>> {
    let flat: Vec<f32> = text
        .trim()
        .trim_start_matches('[')
        .trim_end_matches(']')
        .split([',', '[', ']'])
        .filter_map(|x| x.trim().parse::<f32>().ok())
        .collect();
    if dims == 0 || flat.is_empty() || !flat.len().is_multiple_of(dims) {
        return None;
    }
    Some(flat.chunks_exact(dims).map(|c| c.to_vec()).collect())
}

/// Parses sparse items like `"0:1.0"` into `(index, value)` pairs.
pub(crate) fn parse_sparse(items: &[String]) -> Vec<(usize, f64)> {
    items
        .iter()
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

pub(crate) fn parse_sparse_str(s: &str) -> Vec<(usize, f64)> {
    let t = s.trim().trim_start_matches('[').trim_end_matches(']');
    if t.trim().is_empty() {
        return Vec::new();
    }
    t.split(',')
        .filter_map(|it| {
            let (i, v) = it.split_once(':')?;
            Some((
                i.trim().parse::<usize>().ok()?,
                v.trim().parse::<f64>().ok()?,
            ))
        })
        .collect()
}

/// Recursively extracts the leaf vectors of a (nested) tensor literal.
///
/// One pass over the bytes rather than a `split_top_level` per level: a
/// `tensorarray,100,float` cell is `100` tensors of `100 x 100` numbers, so the
/// recursive form allocated a `Vec<&str>` per group - a million of them - and
/// that was 96% of a five-row tensor scan.
pub(crate) fn parse_tensor_leaves(text: &str) -> Vec<Vec<f64>> {
    let bytes = text.as_bytes();
    let mut out: Vec<Vec<f64>> = Vec::new();
    let mut current: Vec<f64> = Vec::new();
    let mut i = 0usize;
    while i < bytes.len() {
        match bytes[i] {
            b'[' => {
                i += 1;
            }
            b']' => {
                // A leaf is one innermost `[…]` group: its numbers end here.
                // The outer closers find `current` already empty, so they are
                // no-ops - which is what keeps the grouping of a
                // `[[[…]]]` cell intact without a `split_top_level` per level.
                if !current.is_empty() {
                    out.push(std::mem::take(&mut current));
                }
                i += 1;
            }
            b',' | b' ' | b'\t' | b'\n' => {
                i += 1;
            }
            _ => {
                // A number runs to the next bracket or separator; it belongs to
                // the innermost group that is still open.
                let start = i;
                while i < bytes.len()
                    && !matches!(bytes[i], b'[' | b']' | b',' | b' ' | b'\t' | b'\n')
                {
                    i += 1;
                }
                if let Ok(v) = text[start..i].parse::<f64>() {
                    current.push(v);
                }
            }
        }
    }
    if !current.is_empty() {
        out.push(current);
    }
    out
}

/// Parses a stored bit tensor (`[[01110111],[11011011]]`) into per-embedding
/// `0.0`/`1.0` vectors, one per bracketed bit string.
pub(crate) fn parse_bit_leaves(text: &str) -> Vec<Vec<f64>> {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return Vec::new();
    }
    if inner.trim_start().starts_with('[') {
        let mut out = Vec::new();
        for g in split_top_level(inner) {
            out.extend(parse_bit_leaves(&g));
        }
        out
    } else {
        let v: Vec<f64> = inner
            .chars()
            .filter(|c| *c == '0' || *c == '1')
            .map(|c| if c == '1' { 1.0 } else { 0.0 })
            .collect();
        if v.is_empty() {
            Vec::new()
        } else {
            vec![v]
        }
    }
}

/// Splits `a,[b,c],d` on top-level commas (ignoring commas inside brackets).
pub(crate) fn split_top_level(s: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for c in s.chars() {
        match c {
            '[' => {
                depth += 1;
                cur.push(c);
            }
            ']' => {
                depth -= 1;
                cur.push(c);
            }
            ',' if depth == 0 => out.push(std::mem::take(&mut cur)),
            _ => cur.push(c),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur);
    }
    out
}

/// MaxSim: sum over query vectors of the best (max) dot against the doc vectors
/// of one tensor. Both operands are rounded to their declared element type and
/// accumulated in `f32`, mirroring the C++ engine's aligned-`f32` arithmetic.
/// For a `bit` column the query dot is an inner product over the set bits.
pub(crate) fn tensor_maxsim(q: &[Vec<f64>], d: &[Vec<f64>], col: ElemType, qry: ElemType) -> f64 {
    // Everything but a bit column is scored in `f32`, and the doc side is
    // reused by every query vector, so it is narrowed once here instead of
    // inside each of the `q.len() * d.len()` dot products. A
    // `tensorarray,100,float` scan runs 10 000 of those per tensor.
    if col != ElemType::Bit {
        let narrowed: Vec<Vec<f32>> = d
            .iter()
            .map(|dv| dv.iter().map(|v| quantize_elem(*v, col)).collect())
            .collect();
        let query: Vec<Vec<f32>> = q
            .iter()
            .map(|qv| qv.iter().map(|v| quantize_elem(*v, qry)).collect())
            .collect();
        return maxsim_f32(&query, &narrowed);
    }
    let mut sum = 0f32;
    for qv in q {
        let width = qv.len();
        if width == 0 {
            continue;
        }
        let mut best = f32::NEG_INFINITY;
        for dv in d {
            // A stored tensor row may concatenate several `dim`-sized vectors
            // (the C++ writer emits one flat list); compare each slice. This is
            // `dv.len() == width` or a whole multiple of it.
            if dv.len() < width || dv.len() % width != 0 {
                continue;
            }
            // `chunks` rather than a collected `Vec`: this loop runs once per
            // (query vector, doc vector) pair, and the allocation was a visible
            // part of a tensor match.
            for piece in dv.chunks(width) {
                let score = if col == ElemType::Bit {
                    if qry == ElemType::Bit {
                        // Hamming distance: higher score means more different.
                        let diff = qv
                            .iter()
                            .zip(piece)
                            .filter(|(a, b)| (**a != 0.0) != (**b != 0.0))
                            .count();
                        -(diff as f32)
                    } else {
                        let mut acc = 0f32;
                        for (a, b) in qv.iter().zip(piece) {
                            if *b != 0.0 {
                                acc += quantize_elem(*a, qry);
                            }
                        }
                        acc
                    }
                } else {
                    // Unreachable: a non-bit column took the `f32` path above.
                    // Kept as a defensive zero rather than a panic so a future
                    // element type cannot take the process down mid-scan.
                    0.0
                };
                if score > best {
                    best = score;
                }
            }
        }
        if best.is_finite() {
            sum += best;
        }
    }
    sum as f64
}

/// MaxSim over two already-narrowed operands.
///
/// Split from [`tensor_maxsim`] so the narrowing happens once per tensor: the
/// doc side is compared against every query vector, and a
/// `tensorarray,100,float` scan narrows 10 000 vectors per tensor otherwise.
fn maxsim_f32(q: &[Vec<f32>], d: &[Vec<f32>]) -> f64 {
    use std::simd::{f32x8, num::SimdFloat};

    // Flattened: a `Vec<Vec<f32>>` costs one pointer hop per row, and this runs
    // 10 000 dot products per tensor. The rows are all `width` wide for the
    // shapes this scores, so a flat buffer with `width`-sized windows is the
    // same data without the indirection.
    let width = q.first().map(|v| v.len()).unwrap_or(0);
    if width == 0 {
        return 0.0;
    }
    let q_flat: Vec<f32> = q.iter().flat_map(|v| v.iter().copied()).collect();
    let d_flat: Vec<f32> = d.iter().flat_map(|v| v.iter().copied()).collect();
    let q_rows = q_flat.len() / width;
    let d_rows = d_flat.len() / width;
    // The fast path is only valid when every row is exactly one `width`-sized
    // window: a stored tensor row may concatenate several of them (the C++
    // writer emits one flat list), and those have to go through `chunks`.
    let uniform = q.iter().all(|v| v.len() == width) && d.iter().all(|v| v.len() == width);
    if !uniform || d_rows == 0 {
        return maxsim_f32_ragged(q, d);
    }

    let dot = |a: &[f32], b: &[f32]| -> f32 {
        let mut acc = f32x8::splat(0.0);
        let mut i = 0usize;
        while i + 8 <= width {
            acc += f32x8::from_slice(&a[i..i + 8]) * f32x8::from_slice(&b[i..i + 8]);
            i += 8;
        }
        let mut sum = acc.reduce_sum();
        while i < width {
            sum += a[i] * b[i];
            i += 1;
        }
        sum
    };

    let mut sum = 0f32;
    for qi in 0..q_rows {
        let qv = &q_flat[qi * width..(qi + 1) * width];
        let mut best = f32::NEG_INFINITY;
        for di in 0..d_rows {
            let dv = &d_flat[di * width..(di + 1) * width];
            let score = dot(qv, dv);
            if score > best {
                best = score;
            }
        }
        if best.is_finite() {
            sum += best;
        }
    }
    sum as f64
}

/// [`maxsim_f32`] for a doc side whose rows are not all `width` wide.
fn maxsim_f32_ragged(q: &[Vec<f32>], d: &[Vec<f32>]) -> f64 {
    use std::simd::{f32x8, num::SimdFloat};

    let dot = |a: &[f32], b: &[f32]| -> f32 {
        let n = a.len().min(b.len());
        let mut acc = f32x8::splat(0.0);
        let mut i = 0usize;
        while i + 8 <= n {
            acc += f32x8::from_slice(&a[i..i + 8]) * f32x8::from_slice(&b[i..i + 8]);
            i += 8;
        }
        let mut sum = acc.reduce_sum();
        while i < n {
            sum += a[i] * b[i];
            i += 1;
        }
        sum
    };
    let mut sum = 0f32;
    for qv in q {
        let w = qv.len();
        if w == 0 {
            continue;
        }
        let mut best = f32::NEG_INFINITY;
        for dv in d {
            if dv.len() < w || dv.len() % w != 0 {
                continue;
            }
            for piece in dv.chunks(w) {
                let score = dot(qv, piece);
                if score > best {
                    best = score;
                }
            }
        }
        if best.is_finite() {
            sum += best;
        }
    }
    sum as f64
}

/// Scores one stored tensor value against the query. A column holding a
/// `TensorArray` (top-level groups are themselves bracketed) scores by the max
/// over its component tensors; a plain tensor uses MaxSim directly.
pub(crate) fn tensor_score(
    q: &[Vec<f64>],
    doc_text: &str,
    col: ElemType,
    qry: ElemType,
    tensor_array: bool,
) -> f64 {
    let leaves = |g: &str| {
        if col == ElemType::Bit {
            parse_bit_leaves(g)
        } else {
            parse_tensor_leaves(g)
        }
    };
    let t = doc_text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return 0.0;
    }
    let groups = split_top_level(inner);
    // A `TENSORARRAY` row holds several tensors, each scored independently; a
    // plain `TENSOR` row is one tensor whose stored form may already be split
    // into `dim`-sized groups.
    if tensor_array {
        groups
            .iter()
            .map(|g| tensor_maxsim(q, &leaves(g), col, qry))
            .fold(f64::NEG_INFINITY, f64::max)
    } else {
        tensor_maxsim(q, &leaves(doc_text), col, qry)
    }
}

/// Splits `s` into lower-cased tokens using the delimiters implied by a
/// full-text analyzer: `Some([])` splits on whitespace, `Some(chars)` splits on
/// those characters only, and `None` uses the standard (alphanumeric) tokeniser.
pub(crate) fn split_words(s: &str, delims: &Option<Vec<char>>) -> Vec<String> {
    let parts: Vec<String> = match delims {
        Some(d) if d.is_empty() => s.split_whitespace().map(str::to_string).collect(),
        Some(d) => s
            .split(|c: char| d.contains(&c))
            .map(str::to_string)
            .collect(),
        None => return standard_tokenize(s),
    };
    parts
        .into_iter()
        .map(|t| t.to_ascii_lowercase())
        .filter(|t| !t.is_empty())
        .collect()
}

/// The `standard` analyzer's tokenizer, mirroring `tokenizer_impl.cpp` byte for
/// byte. The C++ table is built per **byte**: ASCII alphanumerics and every byte
/// above 127 are `ALLOW` (so UTF-8 bytes stay inside a token, and a `—` does not
/// split one), ASCII whitespace is `SPACE`, and everything else is a one-byte
/// `DELIMITER` token that the analyzer drops. `/` is configured as `UNITE`
/// (`AddUnites("/")`), which the scanner *skips* while continuing the token — but
/// only after the first byte, so a leading `/` is kept. Consequences verified on
/// the engine: `2012/02/14` -> `20120214`, `x/y` -> `xy`,
/// `www.example.com/path` -> `www`+`example`+`compath`.
pub(crate) fn standard_tokenize(s: &str) -> Vec<String> {
    analyzer::tokenize(s, &analyzer::TokenizeConfig::default())
        .into_iter()
        .map(|(token, _, _)| token.to_ascii_lowercase())
        .filter(|token| !token.is_empty())
        .collect()
}

/// Query-side tokens: the standard analyzer's stem replaces the word.
pub(crate) fn tokenize_text(s: &str, tokenizer: &TextTokenizer) -> Vec<String> {
    // A column indexed with a real analyzer (`ik`, `rag-chinese`, `ngram`, …)
    // tokenises the query with that same analyzer (`Analyzer::Analyze`); the
    // delimiter families below are the ones the analyzer reproduces.
    if let Some(terms) = tokenizer.analyze(s) {
        return terms
            .into_iter()
            .map(|term| term.text)
            .filter(|term| !term.is_empty())
            .collect();
    }
    let delims = &tokenizer.delims;
    // The C++ default analyzer is `standard` (`index_full_text_impl.cpp:68`) — a
    // `StandardAnalyzer` with `STEM_LANG_ENGLISH` (`analyzer_pool_impl.cpp:266-289`)
    // — so the `None` (standard) case stems English and drops the word form.
    // `keyword`/`whitespace`(±chars) do not.
    if delims.is_none() {
        return tokenizer
            .standard(true)
            .analyze(s)
            .into_iter()
            .map(|t| t.text)
            .collect();
    }
    split_words(s, delims)
}

/// The **index-side** tokenizer. The C++ `standard` analyzer emits each token
/// *and* its English stem when the two differ; it does not replace the token.
/// The engine's own statistics prove it: for a five-row probe table the C++
/// lengths are 2,4,6,5,4 for token counts 2,3,4,4,2 — every extra term is a
/// stem that differs (`multiple`->`multipl`, `many`->`mani`, `words`->`word`,
/// `another`->`anoth`), and `doc_len_sum` is 21 rather than 15. Query terms stay
/// stem-only (a boolean query for `second text multiple` scores 3, not 4).
pub(crate) fn tokenize_index_text(s: &str, tokenizer: &TextTokenizer) -> Vec<String> {
    if let Some(terms) = tokenizer.analyze(s) {
        return terms
            .into_iter()
            .map(|term| term.text)
            .filter(|term| !term.is_empty())
            .collect();
    }
    let delims = &tokenizer.delims;
    if delims.is_some() {
        return split_words(s, delims);
    }
    tokenizer
        .standard(false)
        .analyze(s)
        .into_iter()
        .map(|t| t.text)
        .collect()
}

/// Index-side tokens **with their position**. The C++ analyzer emits a token's
/// original and its stem at the same offset (`common_analyzer_impl.cpp`
/// re-uses `offset_` for both), so both share a position; `offset_` itself
/// advances once per raw token. Phrase queries need this.
/// Feeds `(term, position)` for the terms this column indexes, borrowing the
/// term text so the caller can intern it without a `String` per term.
///
/// The dictionary-driven analyzers and the delimiter fields still produce a
/// whole `Vec` (they are not on the hot path); the standard analyzer streams.
pub(crate) fn for_each_index_term(
    text: &str,
    tokenizer: &TextTokenizer,
    mut f: impl FnMut(&str, u32, u16),
) {
    let streamed = tokenizer.for_each_term(text, &mut |term: analyzer::TermView| {
        f(term.text, term.word_offset, term.payload)
    });
    if !streamed {
        for (term, pos, payload) in tokenize_index_terms(text, tokenizer) {
            f(&term, pos, payload);
        }
    }
}

/// `(term, position, payload)`. The payload is `Term::payload_`
/// (`index_defines.cppm`): the `rankfeatures` analyzer encodes the feature
/// weight with `SmallFloat::Float122ToUInt16`, every other analyzer leaves it
/// zero. It ends up in the posting's doc-payload slot, which is what
/// `RankFeatureDocIterator` decodes.
pub(crate) fn tokenize_index_terms(s: &str, tokenizer: &TextTokenizer) -> Vec<(String, u32, u16)> {
    if let Some(terms) = tokenizer.analyze(s) {
        return terms
            .into_iter()
            .filter(|term| !term.text.is_empty())
            .map(|term| (term.text, term.word_offset, term.payload))
            .collect();
    }
    let delims = &tokenizer.delims;
    if delims.is_some() {
        return split_words(s, delims)
            .into_iter()
            .enumerate()
            .map(|(pos, word)| (word, pos as u32, 0))
            .collect();
    }
    tokenizer
        .standard(false)
        .analyze(s)
        .into_iter()
        .map(|t| (t.text, t.word_offset, t.payload))
        .collect()
}

pub(crate) fn tokenize_index_positions(s: &str, tokenizer: &TextTokenizer) -> Vec<(String, u32)> {
    tokenize_index_terms(s, tokenizer)
        .into_iter()
        .map(|(text, pos, _)| (text, pos))
        .collect()
}

/// Byte spans of the standard tokenizer's tokens, with the forms the analyzer
/// emits for each (the word and, when it differs, its stem).
pub(crate) fn standard_token_spans(text: &str) -> Vec<(usize, usize, String, Option<String>)> {
    pub(crate) const ALLOW: u8 = 0;
    pub(crate) const DELIM: u8 = 1;
    pub(crate) const SPACE: u8 = 2;
    fn kind(b: u8) -> u8 {
        if b > 127 || b.is_ascii_alphanumeric() {
            ALLOW
        } else if b.is_ascii_whitespace() {
            SPACE
        } else {
            // The highlighter's analyzer splits on `/` (measured: the expected
            // output marks `anarchism` inside `com/anarchism-a-…`), unlike the
            // index analyzer which unites it.
            DELIM
        }
    }
    let bytes = text.as_bytes();
    let mut out = Vec::new();
    let mut i = 0usize;
    while i < bytes.len() {
        while i < bytes.len() && kind(bytes[i]) == SPACE {
            i += 1;
        }
        if i == bytes.len() {
            break;
        }
        if kind(bytes[i]) == DELIM {
            i += 1;
            continue;
        }
        let start = i;
        let mut tok = vec![bytes[i]];
        i += 1;
        while i < bytes.len() {
            match kind(bytes[i]) {
                SPACE | DELIM => break,
                ALLOW => {
                    tok.push(bytes[i]);
                    i += 1;
                }
                _ => i += 1,
            }
        }
        let word = String::from_utf8_lossy(&tok).to_ascii_lowercase();
        let stem = stem_english(&word);
        let stem = if stem != word { Some(stem) } else { None };
        out.push((start, i, word, stem));
    }
    out
}

/// Annotates the query-term occurrences of `text` with `<em>…</em>`, keeping
/// only the sentences that contain a hit and joining them with `" ... "`
/// (`Highlighter::GetHighlight` / `ApplyHighlights`).
pub(crate) fn highlight_text(text: &str, qterms: &[String], delims: &Option<Vec<char>>) -> String {
    if qterms.is_empty() || text.is_empty() {
        return text.to_string();
    }
    // `Highlighter::GetASCIIWordHighlight` scans the document's token stream and
    // keeps the terms the query produced (`highlighter_impl.cpp:72-76`); the set
    // is the port's equivalent of the reference's `std::find` over the query
    // list, in O(1) per token instead of O(query terms).
    let query: std::collections::HashSet<&str> = qterms.iter().map(String::as_str).collect();
    let mut matches: Vec<(usize, usize)> = Vec::new();
    if delims.is_none() {
        for (start, end, word, stem) in standard_token_spans(text) {
            let hit =
                query.contains(word.as_str()) || stem.as_deref().is_some_and(|s| query.contains(s));
            if hit {
                matches.push((start, end));
            }
        }
    } else {
        // The delimiter families (keyword / whitespace[-chars]) highlight whole
        // tokens, exactly like `GetASCIIWordHighlight` with that analyzer: a
        // substring that is not a token of its own is not a match.
        for (start, end, word) in delim_token_spans(text, delims.as_deref().unwrap_or(&[])) {
            if query.contains(word.as_str()) {
                matches.push((start, end));
            }
        }
    }
    apply_highlight_matches(text, matches)
}

/// The delimiter analyzers' tokens with their byte spans:
/// `Some([])` (a `keyword` field) splits on whitespace, `Some(chars)` on those
/// characters, and every token is compared lower-cased - the same tokenisation
/// [`split_words`] does, with offsets kept so the highlighter can mark them.
pub(crate) fn delim_token_spans(text: &str, delims: &[char]) -> Vec<(usize, usize, String)> {
    let is_delim = |c: char| {
        if delims.is_empty() {
            c.is_whitespace()
        } else {
            delims.contains(&c)
        }
    };
    let mut out = Vec::new();
    let mut start: Option<usize> = None;
    for (at, ch) in text.char_indices() {
        if is_delim(ch) {
            if let Some(from) = start.take() {
                out.push((from, at, text[from..at].to_ascii_lowercase()));
            }
        } else if start.is_none() {
            start = Some(at);
        }
    }
    if let Some(from) = start {
        out.push((from, text.len(), text[from..].to_ascii_lowercase()));
    }
    out
}

/// `Highlighter::GetHighlight`: analyses the query and the document with
/// `analyzer`. ASCII query terms match the analyzer's own token stream
/// (`GetASCIIWordHighlight`); non-ASCII terms match as substrings
/// (`GetUnicodeWordHighlight`).
pub fn highlight_with_analyzer(
    query: &str,
    raw_text: &str,
    analyzer: &analyzer::Analyzer,
) -> String {
    if query.is_empty() || raw_text.is_empty() {
        return raw_text.to_string();
    }
    let query_terms: Vec<String> = analyzer_terms(analyzer, query)
        .into_iter()
        .map(|term| term.text)
        .collect();
    let (ascii, unicode): (Vec<&String>, Vec<&String>) =
        query_terms.iter().partition(|term| term.is_ascii());
    let ascii_set: std::collections::HashSet<&str> = ascii.iter().map(|t| t.as_str()).collect();
    let mut matches: Vec<(usize, usize)> = Vec::new();
    if !ascii_set.is_empty() {
        for term in analyzer_terms(analyzer, raw_text) {
            if ascii_set.contains(term.text.as_str()) {
                matches.push((term.word_offset as usize, term.end_offset as usize));
            }
        }
    }
    for term in unicode {
        let mut from = 0usize;
        while let Some(offset) = raw_text[from..].find(term.as_str()) {
            let start = from + offset;
            matches.push((start, start + term.len()));
            from = next_char_boundary(raw_text, start);
        }
    }
    apply_highlight_matches(raw_text, matches)
}

pub(crate) fn analyzer_terms(analyzer: &analyzer::Analyzer, text: &str) -> Vec<analyzer::Term> {
    analyzer.analyze(text)
}

/// The byte offset after the character starting at `at`; substring search
/// advances by one character so a multi-byte UTF-8 sequence is never split.
pub(crate) fn next_char_boundary(text: &str, at: usize) -> usize {
    text[at..]
        .chars()
        .next()
        .map_or(at + 1, |c| at + c.len_utf8())
}

/// `Highlighter::ApplyHighlights`: merges overlapping spans and keeps only the
/// sentences containing a hit, joining the kept sentences with `" ... "`.
pub(crate) fn apply_highlight_matches(text: &str, mut matches: Vec<(usize, usize)>) -> String {
    if matches.is_empty() {
        return text.to_string();
    }
    matches.sort_unstable();
    let mut merged: Vec<(usize, usize)> = Vec::new();
    for (a, b) in matches {
        match merged.last_mut() {
            Some((_, last_end)) if a <= *last_end => *last_end = (*last_end).max(b),
            _ => merged.push((a, b)),
        }
    }
    let mut out = String::new();
    let mut pos = 0usize;
    let mut prev_highlighted = true;
    for sentence in split_sentences(text) {
        let end = pos + sentence.len();
        let rel: Vec<(usize, usize)> = merged
            .iter()
            .filter(|(a, b)| *a >= pos && *b <= end)
            .map(|(a, b)| (a - pos, b - pos))
            .collect();
        let highlighted = !rel.is_empty();
        if highlighted {
            let mut last = 0usize;
            for (a, b) in rel {
                out.push_str(&sentence[last..a]);
                out.push_str("<em>");
                out.push_str(&sentence[a..b]);
                out.push_str("</em>");
                last = b;
            }
            out.push_str(&sentence[last..]);
        } else if prev_highlighted {
            out.push_str(" ... ");
        }
        prev_highlighted = highlighted;
        pos = end;
    }
    out
}

/// `SentenceSplitter` (`rag_analyzer_impl.cpp:627`): splits after
/// ` *[\.\?!]['")\]\]]* *`, keeping the terminator and its trailing quotes and
/// spaces in the sentence that precedes them.
pub(crate) fn split_sentences(text: &str) -> Vec<String> {
    let b = text.as_bytes();
    let mut out = Vec::new();
    let mut start = 0usize;
    while start < b.len() {
        let mut found: Option<(usize, usize)> = None;
        let mut k = start;
        while k < b.len() {
            let mut j = k;
            while j < b.len() && b[j] == b' ' {
                j += 1;
            }
            if j < b.len() && matches!(b[j], b'.' | b'?' | b'!') {
                let mut e = j + 1;
                while e < b.len() && matches!(b[e], b'\'' | b'"' | b')' | b']') {
                    e += 1;
                }
                while e < b.len() && b[e] == b' ' {
                    e += 1;
                }
                found = Some((k, e));
                break;
            }
            k += 1;
        }
        match found {
            Some((ms, me)) => {
                if ms > start {
                    out.push(text[start..me].to_string());
                }
                start = me;
            }
            None => {
                out.push(text[start..].to_string());
                break;
            }
        }
    }
    out
}

/// The C++ default analyzer's English stemmer (`STEM_LANG_ENGLISH`).
pub(crate) static ENGLISH_STEMMER: std::sync::OnceLock<rust_stemmers::Stemmer> =
    std::sync::OnceLock::new();

pub(crate) fn stem_english(token: &str) -> String {
    let s = ENGLISH_STEMMER
        .get_or_init(|| rust_stemmers::Stemmer::create(rust_stemmers::Algorithm::English));
    s.stem(token).to_string()
}

/// Counts the occurrences of a phrase at consecutive positions, straight from
/// the terms' position lists: a start position counts when `unit[k]` sits at
/// `start + k` in every term's list. This is the same count a position map
/// answers, without building the map - the phrase path scores one document at
/// a time and the map was an allocation per document.
pub(crate) fn phrase_freq_lists(lists: &[Vec<u32>], unit_len: usize) -> usize {
    if unit_len == 0 || lists.len() < unit_len {
        return 0;
    }
    let mut n = 0usize;
    for &start in &lists[0] {
        let mut ok = true;
        for (k, list) in lists.iter().enumerate().take(unit_len).skip(1) {
            if list.binary_search(&(start + k as u32)).is_err() {
                ok = false;
                break;
            }
        }
        if ok {
            n += 1;
        }
    }
    n
}

/// A sloppy phrase's frequency: `PhraseDocIterator::GetSloppyPhraseMatchData`
/// (`phrase_doc_iterator_impl.cpp:238-368`), Lucene's `SloppyPhraseMatcher`.
///
/// `lists[i]` is term `i`'s ascending position list in the document. A solution
/// picks one position per term; with `phrase_pos_i = term_pos_i - i` it is
/// acceptable when `max(phrase_pos) - min(phrase_pos) <= slop`, and it
/// contributes `1 / (1 + matchLength)` where `matchLength` is that spread. The
/// frequency is the sum over the solutions.
///
/// The reference walks the solutions as a DFS over the terms, starting each
/// level at the first position that can still reach the previous level's
/// `min_phrase_pos` (`lower_bound` in `:321-324`) and backtracking when the
/// spread exceeds the slop (`:297-300`).
pub(crate) fn phrase_freq_lists_sloppy(lists: &[Vec<u32>], unit_len: usize, slop: u32) -> f64 {
    if unit_len == 0 || lists.len() < unit_len || lists[0].is_empty() {
        return 0.0;
    }
    if slop == 0 {
        return phrase_freq_lists(lists, unit_len) as f64;
    }
    struct Step {
        pos_i: usize,
        term_pos_idx: usize,
        min_phrase_pos: i64,
        max_phrase_pos: i64,
    }
    let slop = i64::from(slop);
    let mut steps: Vec<Step> = Vec::with_capacity(unit_len);
    steps.push(Step {
        pos_i: 0,
        term_pos_idx: 0,
        min_phrase_pos: 0,
        max_phrase_pos: 0,
    });
    let mut tf = 0.0f64;
    while !steps.is_empty() {
        // `CalibrateLastStep`: the spread of the prefix ending at the last step.
        // `too_low` says the position is behind the window the prefix allows, so
        // the same term's *next* occurrence may still fit and only this level
        // advances; an exhausted list or an over-high position unwinds a level.
        let (ok, too_low) = {
            let at = steps.len() - 1;
            let pos_i = steps[at].pos_i;
            let term_pos_idx = steps[at].term_pos_idx;
            match lists[pos_i].get(term_pos_idx) {
                None => (false, false),
                Some(&position) => {
                    let phrase_pos = i64::from(position) - pos_i as i64;
                    if pos_i == 0 {
                        steps[at].min_phrase_pos = phrase_pos;
                        steps[at].max_phrase_pos = phrase_pos;
                        (true, false)
                    } else {
                        let (prev_min, prev_max) = {
                            let prev = &steps[at - 1];
                            (prev.min_phrase_pos, prev.max_phrase_pos)
                        };
                        // A position below `prev_max - slop` cannot join the
                        // prefix (the spread would exceed the slop), and only a
                        // **larger** position of this same term can, so the level
                        // advances instead of unwinding. The bound is the prefix's
                        // high end - a position between `prev_max - slop` and
                        // `prev_min` still fits, and unwinding there would drop
                        // every later occurrence that does.
                        if phrase_pos < prev_max - slop {
                            (false, true)
                        } else {
                            let min = phrase_pos.min(prev_min);
                            let max = phrase_pos.max(prev_max);
                            steps[at].min_phrase_pos = min;
                            steps[at].max_phrase_pos = max;
                            (max - min <= slop, false)
                        }
                    }
                }
            }
        };
        if ok {
            if steps.len() == unit_len {
                let step = steps.last().expect("non-empty");
                let match_length = (step.max_phrase_pos - step.min_phrase_pos) as f64;
                tf += 1.0 / (1.0 + match_length);
                steps.last_mut().expect("non-empty").term_pos_idx += 1;
            } else {
                let pos_i = steps.len();
                let begin = steps[pos_i - 1].min_phrase_pos - slop + pos_i as i64;
                let term_pos_idx =
                    lists[pos_i].partition_point(|&position| i64::from(position) < begin);
                steps.push(Step {
                    pos_i,
                    term_pos_idx,
                    min_phrase_pos: 0,
                    max_phrase_pos: 0,
                });
            }
        } else if too_low {
            // The position is behind the prefix's window: the next occurrence of
            // the same term is the only way forward, so advance this level and
            // keep the prefix calibrated.
            steps.last_mut().expect("non-empty").term_pos_idx += 1;
        } else {
            steps.pop();
            if steps.is_empty() {
                break;
            }
            if steps.len() != unit_len {
                steps.last_mut().expect("non-empty").term_pos_idx += 1;
            }
        }
    }
    tf
}

/// Splits a field-query group body into terms, keeping a quoted phrase together
/// and dropping the quote marks (`doc:"a b" doc2:c` -> `["doc:a b", "doc2:c"]`).
pub(crate) fn split_query_terms(s: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_quote = false;
    for c in s.chars() {
        match c {
            '"' => in_quote = !in_quote,
            c if c.is_whitespace() && !in_quote => {
                if !cur.is_empty() {
                    out.push(std::mem::take(&mut cur));
                }
            }
            c => cur.push(c),
        }
    }
    if !cur.is_empty() {
        out.push(cur);
    }
    out
}

/// Splits a `MATCH TEXT` query into scoring units, keeping quoted groups
/// together: `"second default text"` is one phrase unit that only matches where
/// its tokens sit at consecutive positions, while bare words are single-term
/// units. `term^2.0` boosts are stripped here — boosts are applied per field.
/// Splits a query into `(is_phrase, text, boost, slop)` units.
///
/// `"a b"~2` is one phrase unit with slop 2 (`PhraseQueryNode::slop_`); the
/// `~N` is consumed here instead of becoming a term, which the reference's
/// lexer does the same way (`search_driver`'s Lucene syntax). A `~` after a
/// bare term is left alone: this port has no fuzzy query, matching the
/// reference, whose `QueryNodeType` has no fuzzy node.
pub(crate) fn query_units(query: &str) -> Vec<(bool, String, f64, u32)> {
    let mut out = Vec::new();
    let mut cur = String::new();
    let mut in_quote = false;
    let mut was_quoted = false;
    let flush = |cur: &mut String, quoted: &mut bool, out: &mut Vec<(bool, String)>| {
        if !cur.is_empty() {
            out.push((*quoted, std::mem::take(cur)));
            *quoted = false;
        }
    };
    for c in query.chars() {
        match c {
            '"' => {
                flush(&mut cur, &mut was_quoted, &mut out);
                in_quote = !in_quote;
            }
            c if c.is_whitespace() && !in_quote => flush(&mut cur, &mut was_quoted, &mut out),
            c => {
                if in_quote {
                    was_quoted = true;
                }
                cur.push(c);
            }
        }
    }
    flush(&mut cur, &mut was_quoted, &mut out);
    let mut units: Vec<(bool, String, u32)> = Vec::with_capacity(out.len());
    for (quoted, text) in out {
        // `"a b"~2`: the slop belongs to the quoted unit before it. The lexer
        // leaves the `~2` as its own token, so it is folded back here.
        if let Some(digits) = text.strip_prefix('~') {
            if let (Ok(slop), Some(previous)) = (digits.parse::<u32>(), units.last_mut()) {
                if previous.0 {
                    previous.2 = slop;
                    continue;
                }
            }
        }
        units.push((quoted, text, 0));
    }
    units
        .into_iter()
        .map(|(quoted, text, slop)| {
            // `term^2.0` boosts that one unit.
            let (text, boost) = match text.split_once('^') {
                Some((text, boost)) => (text.to_string(), boost.parse::<f64>().unwrap_or(1.0)),
                None => (text, 1.0),
            };
            (quoted, text, boost, slop)
        })
        .collect()
}

/// One field named by a column-less query tree: its schema index, the
/// tokeniser implied by its analyzer, its per-term document frequency and its
/// average document length.
pub(crate) struct FieldGroupField {
    pub(crate) name: String,
    pub(crate) ci: usize,
    pub(crate) tokenizer: TextTokenizer,
    pub(crate) df: HashMap<String, usize>,
    pub(crate) avg_len: f64,
}

impl FieldGroupField {
    /// `keyword` / `whitespace` fields split on whitespace and score a constant
    /// boost per matching document (the named-column path's `keyword_field`).
    pub(crate) fn keyword(&self) -> bool {
        self.tokenizer.keyword()
    }
}

/// Depth-first list of the columns a query tree names (`GetQueryColumnsTerms`).
pub(crate) fn collect_query_columns(node: &QueryNode, out: &mut Vec<String>) {
    match node {
        QueryNode::Term(term) => {
            if !out.contains(&term.column) {
                out.push(term.column.clone());
            }
        }
        QueryNode::Phrase(phrase) => {
            if !out.contains(&phrase.column) {
                out.push(phrase.column.clone());
            }
        }
        QueryNode::And(children)
        | QueryNode::Or(children)
        | QueryNode::Not(children)
        | QueryNode::AndNot(children) => {
            for child in children {
                collect_query_columns(child, out);
            }
        }
    }
}

/// The tree's `(column, term)` leaves, or `None` when it negates.
///
/// A `NOT doc:x` clause matches the documents that do **not** hold `x`, so the
/// union of the leaves' postings would drop exactly the documents it selects;
/// such a query keeps the full scan.
pub(crate) fn query_leaf_terms(tree: &QueryNode) -> Option<Vec<(String, String)>> {
    fn walk(node: &QueryNode, out: &mut Vec<(String, String)>) -> bool {
        match node {
            QueryNode::Term(term) => {
                out.push((term.column.clone(), term.term.clone()));
                true
            }
            QueryNode::Phrase(phrase) => {
                for term in &phrase.terms {
                    out.push((phrase.column.clone(), term.clone()));
                }
                true
            }
            QueryNode::And(children) | QueryNode::Or(children) => {
                children.iter().all(|child| walk(child, out))
            }
            QueryNode::Not(_) | QueryNode::AndNot(_) => false,
        }
    }
    let mut out = Vec::new();
    walk(tree, &mut out).then_some(out)
}

/// Number of leaf terms in the tree, the denominator of
/// `minimum_should_match` when it is given as a percentage.
pub(crate) fn query_term_count(node: &QueryNode) -> usize {
    match node {
        QueryNode::Term(_) => 1,
        QueryNode::Phrase(phrase) => phrase.terms.len(),
        QueryNode::And(children)
        | QueryNode::Or(children)
        | QueryNode::Not(children)
        | QueryNode::AndNot(children) => children.iter().map(query_term_count).sum(),
    }
}

/// Phrase occurrences at consecutive positions. Both a word and its stem share
/// a position, so a position matches any of its forms.
pub(crate) fn field_phrase_freq(tokens: &[(String, u32)], terms: &[String]) -> usize {
    if terms.is_empty() {
        return 0;
    }
    let mut at: HashMap<u32, Vec<&str>> = HashMap::new();
    for (term, position) in tokens {
        at.entry(*position).or_default().push(term.as_str());
    }
    let mut count = 0usize;
    for (start, forms) in &at {
        if !forms.iter().any(|form| *form == terms[0]) {
            continue;
        }
        if terms.iter().enumerate().skip(1).all(|(k, term)| {
            at.get(&(start + k as u32))
                .map(|forms| forms.iter().any(|form| form == term))
                .unwrap_or(false)
        }) {
            count += 1;
        }
    }
    count
}

/// Scores one document against the parsed tree. Returns the score and the
/// number of matched leaf terms (for `minimum_should_match`), or `None` when
/// the document matches no branch.
pub(crate) fn eval_field_query_node(
    node: &QueryNode,
    fields: &[FieldGroupField],
    tokens: &[Vec<(String, u32)>],
    n_docs: usize,
    delta_term: f64,
    delta_phrase: f64,
    // `(k1, b)` from `bm25_param_k1` / `bm25_param_b`
    // (`bound_select_statement_impl.cpp:239-252`).
    bm25: (f64, f64),
) -> Option<(f64, usize)> {
    let (k1, b) = bm25;
    match node {
        QueryNode::Term(term) => {
            let fi = fields.iter().position(|field| field.name == term.column)?;
            let field = &fields[fi];
            let tf = tokens[fi]
                .iter()
                .filter(|(token, _)| *token == term.term)
                .count();
            if tf == 0 {
                return None;
            }
            let score = if field.keyword() {
                f64::from(term.boost)
            } else {
                let df = field.df.get(&term.term).copied().unwrap_or(0).max(1) as f64;
                let idf = (1.0 + (n_docs as f64 - df + 0.5) / (df + 0.5)).ln();
                let len = tokens[fi].len() as f64;
                let tf = tf as f64;
                let norm = (k1 + 1.0) * tf / (tf + k1 * (1.0 - b + b * len / field.avg_len));
                idf * (norm + delta_term) * f64::from(term.boost)
            };
            Some((score, 1))
        }
        QueryNode::Phrase(phrase) => {
            let fi = fields
                .iter()
                .position(|field| field.name == phrase.column)?;
            let field = &fields[fi];
            let freq = field_phrase_freq(&tokens[fi], &phrase.terms);
            if freq == 0 {
                return None;
            }
            let score = if field.keyword() {
                f64::from(phrase.boost)
            } else {
                let idf_sum: f64 = phrase
                    .terms
                    .iter()
                    .map(|term| {
                        let df = field.df.get(term).copied().unwrap_or(0).max(1) as f64;
                        (1.0 + (n_docs as f64 - df + 0.5) / (df + 0.5)).ln()
                    })
                    .sum();
                let len = tokens[fi].len() as f64;
                let tf = freq as f64;
                let norm = (k1 + 1.0) * tf / (tf + k1 * (1.0 - b + b * len / field.avg_len));
                idf_sum * (norm + delta_phrase) * f64::from(phrase.boost)
            };
            Some((score, phrase.terms.len()))
        }
        QueryNode::And(children) => {
            let (mut score, mut units) = (0.0, 0);
            for child in children {
                let (child_score, child_units) = eval_field_query_node(
                    child,
                    fields,
                    tokens,
                    n_docs,
                    delta_term,
                    delta_phrase,
                    bm25,
                )?;
                score += child_score;
                units += child_units;
            }
            Some((score, units))
        }
        QueryNode::Or(children) => {
            let (mut score, mut units) = (0.0, 0);
            let mut matched = false;
            for child in children {
                if let Some((child_score, child_units)) = eval_field_query_node(
                    child,
                    fields,
                    tokens,
                    n_docs,
                    delta_term,
                    delta_phrase,
                    bm25,
                ) {
                    matched = true;
                    score += child_score;
                    units += child_units;
                }
            }
            matched.then_some((score, units))
        }
        QueryNode::AndNot(children) => {
            let mut iter = children.iter();
            let (score, units) = eval_field_query_node(
                iter.next()?,
                fields,
                tokens,
                n_docs,
                delta_term,
                delta_phrase,
                bm25,
            )?;
            for child in iter {
                if eval_field_query_node(
                    child,
                    fields,
                    tokens,
                    n_docs,
                    delta_term,
                    delta_phrase,
                    bm25,
                )
                .is_some()
                {
                    return None;
                }
            }
            Some((score, units))
        }
        QueryNode::Not(_) => None,
    }
}

/// A `MATCH TEXT ('', '(doc:"a b")^4.5 (doc2:c doc2:d)^3', …)` clause names its
/// fields inside the query string. Returns one entry per group: its boost and
/// the `(field, term)` pairs. `None` when no group names a field, so ordinary
/// queries are unaffected.
#[allow(clippy::type_complexity)]
pub(crate) fn parse_field_query(q: &str) -> Option<Vec<(f64, Vec<(String, String)>)>> {
    let chars: Vec<char> = q.chars().collect();
    let mut i = 0;
    let mut groups: Vec<(f64, Vec<(String, String)>)> = Vec::new();
    let mut any_field = false;
    while i < chars.len() {
        while i < chars.len() && chars[i].is_whitespace() {
            i += 1;
        }
        if i >= chars.len() {
            break;
        }
        let body: String = if chars[i] == '(' {
            let start = i + 1;
            let mut depth = 1;
            i += 1;
            while i < chars.len() {
                match chars[i] {
                    '(' => depth += 1,
                    ')' => {
                        depth -= 1;
                        if depth == 0 {
                            break;
                        }
                    }
                    _ => {}
                }
                i += 1;
            }
            let inner: String = chars[start..i.min(chars.len())].iter().collect();
            if i < chars.len() {
                i += 1; // skip ')'
            }
            inner
        } else {
            let start = i;
            while i < chars.len() && !chars[i].is_whitespace() {
                i += 1;
            }
            chars[start..i].iter().collect()
        };
        // optional `^<boost>`
        let mut boost = 1.0f64;
        if i < chars.len() && chars[i] == '^' {
            i += 1;
            let start = i;
            while i < chars.len() && (chars[i].is_ascii_digit() || chars[i] == '.') {
                i += 1;
            }
            boost = chars[start..i]
                .iter()
                .collect::<String>()
                .parse()
                .unwrap_or(1.0);
        }
        let mut terms: Vec<(String, String)> = Vec::new();
        for raw in split_query_terms(&body) {
            match raw.split_once(':') {
                Some((f, t)) if !f.is_empty() => {
                    any_field = true;
                    terms.push((f.to_string(), t.trim().to_string()));
                }
                _ => terms.push((String::new(), raw)),
            }
        }
        groups.push((boost, terms));
    }
    if any_field {
        Some(groups)
    } else {
        None
    }
}

/// Parses a `topn=<n>` value out of a trailing parameter string.
pub(crate) fn parse_topn_param(p: &str) -> Option<usize> {
    let idx = p.to_ascii_lowercase().find("topn")?;
    let rest = &p[idx + 4..];
    let eq = rest.find('=')?;
    let digits: String = rest[eq + 1..]
        .trim_start()
        .chars()
        .take_while(|c| c.is_ascii_digit())
        .collect();
    digits.parse().ok()
}

/// Reads one `key=value` out of a `;`-separated option string
/// (`'topn=10;default_field=body'`). The key is matched case-insensitively and
/// the value is trimmed, as every other option reader in this module does.
pub(crate) fn search_option(params: &str, key: &str) -> Option<String> {
    params
        .split(';')
        .filter_map(|p| p.split_once('='))
        .find(|(k, _)| k.trim().eq_ignore_ascii_case(key))
        .map(|(_, v)| v.trim().to_string())
        .filter(|v| !v.is_empty())
}

/// Returns `(similarity, distance)` for the given metric.
pub(crate) fn vector_score(q: &[f64], v: &[f64], metric: &str) -> (f64, f64) {
    let dot: f64 = q.iter().zip(v).map(|(a, b)| a * b).sum();
    let nq: f64 = q.iter().map(|a| a * a).sum::<f64>().sqrt();
    let nv: f64 = v.iter().map(|a| a * a).sum::<f64>().sqrt();
    match metric {
        "ip" => (dot, -dot),
        "cosine" => {
            let c = if nq > 0.0 && nv > 0.0 {
                dot / (nq * nv)
            } else {
                0.0
            };
            (c, 1.0 - c)
        }
        "hamming" => {
            // Hamming distance: the number of differing positions.
            let d = q
                .iter()
                .zip(v)
                .filter(|(a, b)| (**a != 0.0) != (**b != 0.0))
                .count() as f64;
            (1.0 / (1.0 + d), d)
        }
        _ => {
            // `l2` reports the *squared* Euclidean distance (no square root).
            let d: f64 = q.iter().zip(v).map(|(a, b)| (a - b) * (a - b)).sum();
            (1.0 / (1.0 + d), d)
        }
    }
}

pub(crate) fn sort_rows(rows: &mut [Vec<Value>], keys: &[(expression::Expr, bool)]) {
    // A stable sort: rows that tie on every key keep their input order, which is
    // the C++ `CompareTwoRowAndPreferLeft` rule.
    rows.sort_by(|a, b| compare_rows(a, b, keys));
}

/// The ORDER BY comparison shared by the stage pipeline, `PhysicalSort` and
/// `PhysicalTop`, so every path orders rows identically.
pub(crate) fn compare_rows(
    a: &[Value],
    b: &[Value],
    keys: &[(expression::Expr, bool)],
) -> std::cmp::Ordering {
    for (key, asc) in keys {
        let x = key.eval(a);
        let y = key.eval(b);
        let (x, y) = (&x, &y);
        // NULLs order before every non-NULL (so ASC → NULLS FIRST, and the
        // reverse below gives DESC → NULLS LAST).
        let mut ord = match (x, y) {
            (Value::Null, Value::Null) => std::cmp::Ordering::Equal,
            (Value::Null, _) => std::cmp::Ordering::Less,
            (_, Value::Null) => std::cmp::Ordering::Greater,
            // Fall back to a textual comparison for mixed types so the order
            // stays total (e.g. an int beside a non-numeric string).
            _ => x
                .partial_cmp_value(y)
                .unwrap_or_else(|| x.to_slt_string().cmp(&y.to_slt_string())),
        };
        if !*asc {
            ord = ord.reverse();
        }
        if ord != std::cmp::Ordering::Equal {
            return ord;
        }
    }
    std::cmp::Ordering::Equal
}

/// Recursively collects the files under `dir`, sorted. A missing directory has
/// no files.
pub(crate) fn files_under(dir: &Path) -> Vec<PathBuf> {
    let mut out = Vec::new();
    let Ok(entries) = std::fs::read_dir(dir) else {
        return out;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            out.extend(files_under(&path));
        } else if path.is_file() {
            out.push(path);
        }
    }
    out.sort();
    out
}

impl BgTask for DumpMemIndexTask {
    fn name(&self) -> &str {
        "DumpMemIndexTask"
    }

    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            if let Some(engine) = self.engine.upgrade() {
                // A dump sees a whole write batch: the dump transaction
                // conflicts with an append or update that already took its
                // commit ts on the same segment and is retried, as the
                // reference's `DoDump` is. Every step inside is an `.await`;
                // the segment files it seals are written on the blocking pool,
                // leaf by leaf.
                if self.drain_all {
                    engine.dump_all_memory_indexes().await;
                    engine.dump_all_sealed_blocks(None).await;
                } else {
                    engine.dump_under_pressure(None).await;
                }
            }
            self.pending.store(0, std::sync::atomic::Ordering::Relaxed);
        })
    }
}

impl BgTask for CleanupTask {
    fn name(&self) -> &str {
        "CleanupTask"
    }

    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            if let Some(engine) = self.engine.upgrade() {
                if let Err(error) = engine.cleanup().await {
                    eprintln!("background cleanup failed: {error}");
                }
            }
        })
    }
}

impl BgTask for OptimizeTask {
    fn name(&self) -> &str {
        "OptimizeTask"
    }

    fn run(self: Box<Self>) -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send>> {
        Box::pin(async move {
            if let Some(engine) = self.engine.upgrade() {
                if let Err(error) = engine.optimize_indexes_async().await {
                    eprintln!("background optimize failed: {error}");
                }
            }
        })
    }
}

#[cfg(test)]
mod float_format_ut {
    use super::{fmt_f32_cpp, fmt_f64_cpp};

    /// `std::to_chars` (`std::format("{}")`) spells these; the fvecs fixture
    /// in `test/sql/dml/import` compares against the engine byte for byte.
    #[test]
    fn the_shortest_form_matches_std_to_chars() {
        assert_eq!(fmt_f32_cpp(1.0), "1");
        assert_eq!(fmt_f32_cpp(0.0), "0");
        assert_eq!(fmt_f32_cpp(-0.0), "-0");
        assert_eq!(fmt_f32_cpp(0.5), "0.5");
        assert_eq!(fmt_f32_cpp(5.2582283e-05), "5.2582283e-05");
        assert_eq!(fmt_f32_cpp(1234567.0), "1234567");
        assert_eq!(fmt_f32_cpp(1.5e9), "1.5e+09");
        assert_eq!(fmt_f64_cpp(0.470703125), "0.470703125");
    }

    /// `0.470703125` is exactly between `0.47070312` and `0.47070313`.
    /// Ryu (what `std::to_chars` uses) rounds the tie to even; Rust's own
    /// shortest form rounds it up, which made the fvecs gate disagree with the
    /// reference on that one element.
    #[test]
    // The literal is exact (`241 / 512`); clippy compares it with Rust's own
    // shortest form and thinks the extra digits are redundant.
    #[allow(clippy::excessive_precision)]
    fn a_decimal_tie_rounds_like_ryu() {
        assert_eq!(fmt_f32_cpp(0.470703125), "0.47070312");
    }
}

use super::*;

/// The declared dimension of a `TENSOR(FLOAT,d)` / `EMBEDDING(FLOAT,d)` column.
pub(crate) fn type_dim(type_text: &str) -> Option<usize> {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1)?.trim_end_matches(')');
    inner.split(',').rev().find_map(|s| s.trim().parse().ok())
}

/// The declared dimension of a (possibly nested) parameterised type, e.g.
/// `ARRAY(EMBEDDING(FLOAT,16385))` -> 16385.
pub(crate) fn type_dim_deep(type_text: &str) -> Option<usize> {
    let t = type_text.to_ascii_uppercase();
    let after = t.rsplit_once('(').map(|(_, r)| r).unwrap_or(&t);
    after
        .split(',')
        .rev()
        .find_map(|s| s.trim().trim_end_matches(')').trim().parse().ok())
}

pub(crate) fn is_tensor_type(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    t.starts_with("TENSOR") || t.starts_with("TENSORARRAY") || t.starts_with("MULTIVECTOR")
}

/// The numeric element type of a `TENSOR(...)` / `TENSORARRAY(...)` column or of
/// a `MATCH TENSOR` query. The C++ engine casts both the stored and query tensor
/// values into a common aligned `f32` buffer before the MaxSim dot products, so
/// the element type only matters for the precision each value is rounded to.
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub(crate) enum ElemType {
    Float32,
    Float64,
    Float16,
    BFloat16,
    Bit,
    Int8,
    UInt8,
    Int16,
    Int32,
    Int64,
}

pub(crate) fn elem_type_of(token: &str) -> ElemType {
    // The parser concatenates type tokens without spaces (`UNSIGNEDTINYINT`),
    // so compare against a whitespace-free uppercase form.
    let t: String = token
        .chars()
        .filter(|c| !c.is_whitespace())
        .collect::<String>()
        .to_ascii_uppercase();
    match t.as_str() {
        "FLOAT16" | "FP16" | "HALF" => ElemType::Float16,
        "BFLOAT16" | "BF16" => ElemType::BFloat16,
        "DOUBLE" | "FLOAT64" => ElemType::Float64,
        "BIT" => ElemType::Bit,
        "INT8" | "TINYINT" => ElemType::Int8,
        "UINT8" | "UNSIGNEDTINYINT" | "UTINYINT" | "UNSIGNEDINT8" => ElemType::UInt8,
        "INT16" | "SMALLINT" => ElemType::Int16,
        "INT32" | "INT" | "INTEGER" => ElemType::Int32,
        "INT64" | "BIGINT" | "LONG" => ElemType::Int64,
        _ => ElemType::Float32,
    }
}

/// The query element type named by a `MATCH …` argument, if it names one.
/// Like [`elem_type_of`] but rejects an unknown type name.
pub(crate) fn elem_type_named(token: &str) -> Option<ElemType> {
    let t: String = token
        .chars()
        .filter(|c| !c.is_whitespace())
        .collect::<String>()
        .to_ascii_uppercase();
    match t.as_str() {
        "FLOAT" | "FLOAT32" | "FLOAT16" | "FP16" | "HALF" | "BFLOAT16" | "BF16" | "DOUBLE"
        | "FLOAT64" | "BIT" | "INT8" | "TINYINT" | "UINT8" | "UNSIGNEDTINYINT" | "UTINYINT"
        | "UNSIGNEDINT8" | "INT16" | "SMALLINT" | "INT32" | "INT" | "INTEGER" | "INT64"
        | "BIGINT" | "LONG" => Some(elem_type_of(token)),
        _ => None,
    }
}

pub(crate) fn elem_type_of_optional(arg: Option<&ExprAst>) -> Option<ElemType> {
    arg_str(arg).map(|s| elem_type_of(&s))
}

/// The element type declared inside a `TENSOR(<elem>, <dim>)` type text.
pub(crate) fn tensor_elem_type(type_text: &str) -> ElemType {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("");
    elem_type_of(inner.split(',').next().unwrap_or(""))
}

/// Rounds an `f64` to the element type's precision, widened back to `f32` — the
/// conversion the C++ engine performs when filling its `f32` arithmetic buffer.
pub(crate) fn quantize_elem(x: f64, e: ElemType) -> f32 {
    let v = x as f32;
    match e {
        ElemType::Float16 => round_f16(v),
        ElemType::BFloat16 => round_bf16(v),
        _ => v,
    }
}

/// Round-to-nearest-even conversion of an `f32` to the nearest `f16` value,
/// returned as `f32`.
pub(crate) fn round_f16(x: f32) -> f32 {
    f16_bits_to_f32(f32_to_f16_bits(x))
}

/// Rounds an `f32` to IEEE-754 half precision and returns its bit pattern.
///
/// Shared with the C++ block writer (`cpp_blocks`), which has to pack a
/// `FLOAT16` column the way the reference does.
pub(crate) fn f32_to_f16_bits(x: f32) -> u16 {
    let b = x.to_bits();
    let sign = ((b >> 16) & 0x8000) as u16;
    let exp = ((b >> 23) & 0xff) as i32;
    let mant = b & 0x007f_ffff;
    if exp == 0xff {
        let m = if mant != 0 { 0x200 } else { 0 };
        return sign | 0x7c00 | m;
    }
    let new_exp = exp - 127 + 15;
    if new_exp >= 31 {
        return sign | 0x7c00;
    }
    if new_exp <= 0 {
        if new_exp < -10 {
            return sign;
        }
        let m = mant | 0x0080_0000;
        let shift = (14 - new_exp) as u32;
        let half = 1u32 << (shift - 1);
        let mut q = m >> shift;
        let rem = m & ((1u32 << shift) - 1);
        if rem > half || (rem == half && (q & 1) == 1) {
            q += 1;
        }
        return sign | q as u16;
    }
    let mut m = mant >> 13;
    let rem = mant & 0x1fff;
    let mut e = new_exp;
    if rem > 0x1000 || (rem == 0x1000 && (m & 1) == 1) {
        m += 1;
        if m == 0x400 {
            m = 0;
            e += 1;
            if e >= 31 {
                return sign | 0x7c00;
            }
        }
    }
    sign | ((e as u16) << 10) | (m as u16)
}

/// Decodes an IEEE-754 half-precision bit pattern into `f32`.
///
/// Shared with the C++ block reader (`cpp_blocks`), which has to render a
/// `FLOAT16` column the way the reference does.
pub(crate) fn f16_bits_to_f32(h: u16) -> f32 {
    let sign = ((h & 0x8000) as u32) << 16;
    let exp = ((h >> 10) & 0x1f) as u32;
    let mant = (h & 0x3ff) as u32;
    if exp == 0 {
        if mant == 0 {
            return f32::from_bits(sign);
        }
        let mut e: i32 = -14;
        let mut m = mant;
        while m & 0x400 == 0 {
            m <<= 1;
            e -= 1;
        }
        m &= 0x3ff;
        return f32::from_bits(sign | (((e + 127) as u32) << 23) | (m << 13));
    }
    if exp == 0x1f {
        return f32::from_bits(sign | 0x7f80_0000 | (mant << 13));
    }
    f32::from_bits(sign | ((exp + 127 - 15) << 23) | (mant << 13))
}

/// Conversion of an `f32` to `bfloat16` by dropping the low 16 mantissa bits.
/// The C++ `bfloat16_t` conversion used by the tensor scan truncates rather than
/// rounding to nearest, so this deliberately truncates to match its scores.
pub(crate) fn round_bf16(x: f32) -> f32 {
    f32::from_bits(x.to_bits() & 0xffff_0000)
}

pub(crate) fn is_float_type(type_text: &str) -> bool {
    matches!(
        type_text.to_ascii_uppercase().as_str(),
        "FLOAT" | "DOUBLE" | "REAL" | "FP16" | "BF16" | "FLOAT16" | "BFLOAT16"
    )
}

pub(crate) fn is_array_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().starts_with("ARRAY")
}

/// Whether an `ARRAY(<elem>)` type's element type is a float type.
pub(crate) fn array_elem_is_float(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("").trim_end_matches(')');
    is_float_type(inner.trim())
}

/// ARRAY columns render with braces: `[1,2]` → `{1,2}`, `[]` → `{}`.
/// Whether the element type declared inside a `TENSOR`/`EMBEDDING`-style type
/// text is an integer type (the first comma-separated parameter).
pub(crate) fn declared_elem_is_int(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("");
    array_elem_is_int(&format!("ARRAY({}", inner.split(',').next().unwrap_or("")))
}

/// Whether an `ARRAY(<elem>)` type has an integer element type.
pub(crate) fn array_elem_is_int(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    let inner = t.split('(').nth(1).unwrap_or("").trim_end_matches(')');
    matches!(
        inner.trim(),
        "INT"
            | "INTEGER"
            | "TINYINT"
            | "SMALLINT"
            | "BIGINT"
            | "INT8"
            | "INT16"
            | "INT32"
            | "INT64"
    )
}

pub(crate) fn is_json_type(type_text: &str) -> bool {
    type_text.eq_ignore_ascii_case("JSON")
}

/// Whether a column type renders as a canonical date/time string.
pub(crate) fn is_datetime_type(type_text: &str) -> bool {
    let u = type_text.to_ascii_uppercase();
    matches!(u.as_str(), "DATE" | "TIME" | "DATETIME" | "TIMESTAMP")
}

pub(crate) fn is_sparse_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().starts_with("SPARSE")
}

/// The bound a `SPARSE` column declares, in either spelling:
/// `SPARSE(elem,index,bound)` (the C++ layout text) or `sparse,bound,elem,index`
/// (the SQL spelling). `None` when the type declares no bound.
pub(crate) fn sparse_declared_bound(type_text: &str) -> Option<usize> {
    let text = type_text.trim();
    if let Some(inner) = text
        .strip_prefix("SPARSE(")
        .or_else(|| text.strip_prefix("sparse("))
        .and_then(|s| s.strip_suffix(')'))
    {
        let bound = inner.split(',').nth(2)?;
        return bound.trim().parse().ok().filter(|b| *b > 0);
    }
    let mut parts = text.split(',');
    let head = parts.next()?;
    if !head.trim().eq_ignore_ascii_case("sparse") {
        return None;
    }
    parts.next()?.trim().parse().ok().filter(|b| *b > 0)
}

/// Canonical sparse rendering: `(empty)` when there is nothing, otherwise
/// `[i:v,…]` sorted by index. Returns `None` for an invalid vector (negative or
/// duplicated indices).
pub(crate) fn reshape_sparse(s: &str, float: bool) -> Option<String> {
    let t = s.trim();
    if t.is_empty() || t == "[]" {
        return Some("(empty)".to_string());
    }
    // A `bit` sparse vector (and any index-only literal) carries just the
    // non-zero positions, e.g. `[10000,20000]`, with no `:` values.
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if !inner.contains(':') {
        let mut idx: Vec<usize> = Vec::new();
        for it in inner.split(',') {
            let it = it.trim();
            if it.is_empty() {
                continue;
            }
            idx.push(it.parse::<usize>().ok()?);
        }
        if idx.is_empty() {
            return None;
        }
        idx.sort_unstable();
        if idx.windows(2).any(|w| w[0] == w[1]) {
            return None; // duplicate index
        }
        let body: Vec<String> = idx.iter().map(|i| i.to_string()).collect();
        return Some(format!("[{}]", body.join(",")));
    }
    let pairs = parse_sparse_str(t);
    if pairs.is_empty() {
        // a non-empty literal that parses to nothing (e.g. a negative index)
        // is invalid
        return None;
    }
    let mut sorted = pairs.clone();
    sorted.sort_by_key(|(i, _)| *i);
    if sorted.windows(2).any(|w| w[0].0 == w[1].0) {
        return None; // duplicate index
    }
    let body: Vec<String> = sorted
        .iter()
        .map(|(i, v)| {
            if float {
                format!("{i}:{v:.6}")
            } else {
                format!("{i}:{}", fmt_num(*v))
            }
        })
        .collect();
    Some(format!("[{}]", body.join(",")))
}

/// Whether the type text declares a vector-like dimension
/// (`EMBEDDING`/`VECTOR`/`TENSOR`/`TENSORARRAY`/`MULTIVECTOR`/`SPARSE`).
pub(crate) fn declares_dimension(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    [
        "EMBEDDING",
        "VECTOR",
        "TENSOR",
        "TENSORARRAY",
        "MULTIVECTOR",
        "SPARSE",
    ]
    .iter()
    .any(|k| t.starts_with(k) || (t.starts_with("ARRAY(") && t.contains(k)))
}

pub(crate) fn is_bit_type(type_text: &str) -> bool {
    type_text.to_ascii_uppercase().contains("BIT")
}

/// Whether the type text is a fixed-width embedding column
/// (`EMBEDDING(...)` / `VECTOR(...)`).
pub(crate) fn is_embedding_type(type_text: &str) -> bool {
    let t = type_text.to_ascii_uppercase();
    t.starts_with("EMBEDDING") || t.starts_with("VECTOR")
}

/// Packs a flat bit embedding into a single bit string, e.g. `[0000000000000001]`.
pub(crate) fn pack_bit_vector(s: &str) -> String {
    let t = s.trim();
    let inner = t.trim_start_matches('[').trim_end_matches(']');
    if inner.trim().is_empty() {
        return "[]".to_string();
    }
    // Already a bit string (`0100000000000000`): packing is idempotent. Without
    // this, re-encoding a stored row (a KNN hit rebuilt from its text) parsed
    // the whole run as one number and collapsed the vector to `[1]`.
    let trimmed = inner.trim();
    if trimmed.len() > 1 && trimmed.chars().all(|c| c == '0' || c == '1') {
        return format!("[{trimmed}]");
    }
    let bits: String = inner
        .split([',', ' '])
        .filter(|x| !x.trim().is_empty())
        .map(|x| {
            if x.trim().parse::<f64>().map(|f| f != 0.0).unwrap_or(false) {
                '1'
            } else {
                '0'
            }
        })
        .collect();
    format!("[{bits}]")
}

/// Packs a bit tensor into rows of `dim` `0`/`1` characters.
pub(crate) fn reshape_bit_tensor(s: &str, dim: usize) -> String {
    let t = s.trim();
    if !t.starts_with('[') {
        return t.to_string();
    }
    // A flat input is first chunked into `dim`-sized rows; an already-nested
    // input keeps its structure.
    let text = if t[1..].starts_with('[') {
        t.to_string()
    } else {
        let flat = parse_vec_str(t);
        if flat.is_empty() {
            return "(empty)".to_string();
        }
        let d = if dim == 0 { flat.len() } else { dim };
        let groups: Vec<String> = flat
            .chunks(d)
            .map(|c| {
                format!(
                    "[{}]",
                    c.iter().map(|v| fmt_num(*v)).collect::<Vec<_>>().join(",")
                )
            })
            .collect();
        format!("[{}]", groups.join(","))
    };
    pack_bit_leaves(&text)
}

/// Replaces every numeric leaf list with a bracketed bit string, keeping nesting.
pub(crate) fn pack_bit_leaves(text: &str) -> String {
    let t = text.trim();
    let inner = t
        .strip_prefix('[')
        .unwrap_or(t)
        .strip_suffix(']')
        .unwrap_or(t);
    if inner.trim().is_empty() {
        return "[]".to_string();
    }
    if inner.trim_start().starts_with('[') {
        let groups: Vec<String> = split_top_level(inner)
            .iter()
            .map(|g| pack_bit_leaves(g))
            .collect();
        format!("[{}]", groups.join(","))
    } else {
        let trimmed = inner.trim();
        // Already a bit string (`10110001`): the packing is idempotent. Without
        // this, re-encoding a value that was packed once parses the whole run
        // as one number and collapses it to `[1]`, which is what a row reproject
        // after a block read would do.
        if trimmed.len() > 1 && trimmed.chars().all(|c| c == '0' || c == '1') {
            return format!("[{trimmed}]");
        }
        let bits: String = inner
            .split([',', ' '])
            .filter(|x| !x.trim().is_empty())
            .map(|x| {
                if x.trim().parse::<f64>().map(|f| f != 0.0).unwrap_or(false) {
                    '1'
                } else {
                    '0'
                }
            })
            .collect();
        format!("[{bits}]")
    }
}

/// Reshapes a flat tensor literal (`[a,b,c,d,e,f,g,h]`) into `[[a,b,c,d],[e,f,g,h]]`.
pub(crate) fn reshape_tensor(s: &str, dim: usize) -> String {
    let t = s.trim();
    if dim == 0 || !t.starts_with('[') {
        return t.to_string();
    }
    if t[1..].starts_with('[') {
        return t.to_string(); // already nested
    }
    let flat: Vec<f64> = parse_vec_str(t);
    if flat.is_empty() || !flat.len().is_multiple_of(dim) {
        return t.to_string();
    }
    let groups: Vec<String> = flat
        .chunks(dim)
        .map(|c| {
            format!(
                "[{}]",
                c.iter().map(|f| fmt_num(*f)).collect::<Vec<_>>().join(",")
            )
        })
        .collect();
    format!("[{}]", groups.join(","))
}

/// Normalises a column default to its stored form: an integer-element
/// array/embedding default keeps integer values (`[1.3,4.1,33.7]` → `[1,4,34]`).
pub(crate) fn normalize_default(type_text: &str, raw: &str) -> String {
    if (is_array_type(type_text) || is_embedding_type(type_text))
        && (array_elem_is_int(type_text) || declared_elem_is_int(type_text))
    {
        let types = vec![type_text.to_string()];
        encode_row(&[Literal::Str(raw.to_string())], &types)
    } else {
        raw.to_string()
    }
}

/// Encodes a row for storage, reshaping tensor columns by their declared dim.
/// Maps a declared type text to the port `LogicalType`, for the subset the
/// Append block encoder can represent. `None` means "use the text fallback".
pub(crate) fn port_logical_type(type_text: &str) -> Option<::common::LogicalType> {
    use ::common::LogicalType as LT;
    let doc = catalog::cpp_layout::data_type_of_text(type_text)?;
    let code = match expression::json::extract(&doc, ".data_type")? {
        expression::json::Json::Int(v) => *v,
        _ => return None,
    };
    Some(match code {
        0 => LT::Boolean,
        1 => LT::TinyInt,
        2 => LT::SmallInt,
        3 => LT::Integer,
        4 => LT::BigInt,
        6 => LT::Decimal,
        7 => LT::Float,
        8 => LT::Double,
        9 => LT::Varchar,
        10 => LT::Date,
        11 => LT::Time,
        12 => LT::DateTime,
        13 => LT::Timestamp,
        // The port's block model has no half-precision column: the declared
        // type still reaches the WAL encoder, which packs the value to `f16`.
        32 | 33 => LT::Float,
        35 => LT::Json,
        23 => {
            let dim = match expression::json::extract(&doc, ".type_info.dimension")? {
                expression::json::Json::Int(v) => *v as u32,
                _ => 0,
            };
            LT::Embedding(dim)
        }
        _ => return None,
    })
}

/// `SPARSE`: `(index, value)` pairs, sorted by index (the reference's default
/// `kSort` store type), indices then values in the heap.
pub(crate) fn sparse_column(
    cells: &[&Literal],
    data_type: &storage::wal::ddl_def::DataType,
) -> Option<storage::wal::block::BlockColumn> {
    use storage::wal::block::{BlockColumn, ColumnValues};
    use storage::wal::ddl_def::TypeInfo;
    let TypeInfo::Sparse {
        data_type: value_type,
        index_type,
        ..
    } = &data_type.type_info
    else {
        return None;
    };
    let index_width = storage::wal::block::element_width(*index_type)?;
    let value_width = storage::wal::block::element_width(*value_type)?;
    let mut values = Vec::with_capacity(cells.len());
    let mut nulls = Vec::new();
    for (row, cell) in cells.iter().enumerate() {
        if matches!(cell, Literal::Null) {
            nulls.push(row as u32);
            values.push(None);
            continue;
        }
        let mut pairs = literal_sparse_pairs(cell)?;
        pairs.sort_by_key(|(index, _)| *index);
        let mut indices = Vec::with_capacity(pairs.len() * index_width);
        let mut raw = Vec::with_capacity(pairs.len() * value_width);
        for (index, value) in &pairs {
            pack_integer(&mut indices, *index, *index_type)?;
            pack_float(&mut raw, *value, *value_type)?;
        }
        values.push(Some((pairs.len() as i64, indices, raw)));
    }
    let mut column = BlockColumn::new(data_type.clone(), ColumnValues::sparses(&values));
    column.nulls = nulls;
    Some(column)
}

/// The reference's column-type whitelist, as its error message.
///
/// `VerifyColumnDataType` accepts `Boolean`, `TinyInt`, `SmallInt`, `Integer`,
/// `BigInt`, `Float`, `Double`, `Float16`, `BFloat16`, `Varchar`, `Date`,
/// `Time`, `Timestamp`, `Json`, `DateTime`, `Embedding` (with a dimension
/// limit), `MultiVector`, `Tensor`, `TensorArray`, `Sparse` and `Array` of those.
/// Everything else — notably `HugeInt`, `Decimal`, `Interval` and `UUID` — is
/// rejected; `Array` recurses and reports the element's error too.
pub(crate) fn unsupported_column_type(type_text: &str) -> Option<String> {
    unsupported_column_display(type_text)
        .map(|display| format!("Not supported data type: {display}"))
}

/// The `DataType::ToString()` of a rejected column type (`None` when accepted).
pub(crate) fn unsupported_column_display(type_text: &str) -> Option<String> {
    let up = type_text.trim().to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    match base {
        "BOOL" | "BOOLEAN" | "TINYINT" | "INT8" | "SMALLINT" | "INT16" | "INT" | "INTEGER"
        | "INT32" | "BIGINT" | "INT64" | "FLOAT" | "REAL" | "FLOAT32" | "DOUBLE" | "FLOAT64"
        | "FLOAT16" | "BFLOAT16" | "VARCHAR" | "TEXT" | "STRING" | "DATE" | "TIME" | "DATETIME"
        | "TIMESTAMP" | "JSON" | "EMBEDDING" | "VECTOR" | "TENSOR" | "TENSORARRAY"
        | "MULTIVECTOR" | "SPARSE" => None,
        "ARRAY" => {
            // The element type is the inner text (`ARRAY(INT)`); the SDK's
            // spelling is `array,int`.
            let inner = match up.split_once('(') {
                Some((_, rest)) => rest.trim_end_matches(')').to_string(),
                None => up.split_once(',').map(|(_, rest)| rest.to_string())?,
            };
            let element = unsupported_column_display(&inner)?;
            let message = format!("Not supported data type: {element}");
            Some(format!(
                "Array({element}), error in element type: {message}"
            ))
        }
        // `Decimal(decimal(<precision>, <scale>))`: `DecimalInfo::ToString`
        // spells the parameters lower-case with a space after the comma.
        "DECIMAL" => match up.split_once('(') {
            Some((_, rest)) => {
                let inner = rest.trim_end_matches(')');
                let mut parts = inner.split(',').map(|p| p.trim());
                let precision: i64 = parts.next().unwrap_or("0").parse().unwrap_or(0);
                let scale: i64 = parts.next().unwrap_or("0").parse().unwrap_or(0);
                Some(format!("Decimal(decimal({precision}, {scale}))"))
            }
            None => Some("Decimal".to_string()),
        },
        "HUGEINT" | "INT128" => Some("HugeInt".to_string()),
        "UUID" => Some("UUID".to_string()),
        "INTERVAL" => Some("Interval".to_string()),
        "TUPLE" => Some("Tuple".to_string()),
        "POINT" => Some("Point".to_string()),
        "LINE" => Some("Line".to_string()),
        "LINESEG" | "LINE_SEGMENT" => Some("LineSegment".to_string()),
        "BOX" => Some("Box".to_string()),
        "CIRCLE" => Some("Circle".to_string()),
        other => Some(other.to_string()),
    }
}

/// Whether a predicate is a single `col = constant` (either order).
///
/// The reader filters rows while it builds them, which pays off only when most
/// of them will be rejected; this is the shape that says so.
pub(crate) fn is_equality_predicate(expr: &Expr) -> bool {
    match expr {
        Expr::Cmp(a, parser::CmpOp::Eq, b) => {
            (matches!(**a, Expr::Column(_)) && matches!(**b, Expr::Literal(_)))
                || (matches!(**b, Expr::Column(_)) && matches!(**a, Expr::Literal(_)))
        }
        _ => false,
    }
}

/// Human-readable column type, e.g. `Integer` or `Sparse(float,int16,30000)`.
pub(crate) fn describe_type(type_text: &str) -> String {
    let t = type_text.trim();
    if t.is_empty() {
        return "Invalid".to_string();
    }
    let up = t.to_ascii_uppercase();
    if up.starts_with("ARRAY(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        return format!("Array({})", describe_type(inner));
    }
    if up.starts_with("SPARSE(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let raw = parts
            .first()
            .map(|s| s.to_ascii_lowercase())
            .unwrap_or_default();
        // Integer element names are printed with their width suffix.
        let elem = match raw.as_str() {
            "int" | "integer" => "int32",
            "tinyint" => "int8",
            "smallint" => "int16",
            "bigint" => "int64",
            _ => raw.as_str(),
        };
        // The canonical spelling is `SPARSE(<elem>,<index>,<bound>)`; the
        // two-field `SPARSE(<elem>,<bound>)` leaves the index to be derived
        // from the bound (the reference's `SparseInfo::GetIndexType`).
        let (index_text, bound_text): (Option<&str>, &str) = if parts.len() >= 3 {
            (parts.get(1).copied(), parts.get(2).copied().unwrap_or("0"))
        } else {
            (None, parts.get(1).copied().unwrap_or("0"))
        };
        let bound: u64 = bound_text.parse().unwrap_or(0);
        let idx = match index_text {
            Some(name) => match name.to_ascii_lowercase().as_str() {
                "int8" | "tinyint" | "uint8" => "int8",
                "int16" | "smallint" => "int16",
                "int" | "integer" | "int32" => "int32",
                "int64" | "bigint" => "int64",
                _ => "int8",
            },
            // No declared index: the reference derives the width from the bound.
            None => {
                if bound <= 127 {
                    "int8"
                } else if bound <= 32767 {
                    "int16"
                } else if bound <= 2_147_483_647 {
                    "int32"
                } else {
                    "int64"
                }
            }
        };
        return format!("Sparse({elem},{idx},{bound})");
    }
    if up.starts_with("EMBEDDING(") {
        let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let elem = parts
            .first()
            .map(|s| s.to_ascii_lowercase())
            .unwrap_or_default();
        let dim = parts.get(1).map(|s| s.trim()).unwrap_or("");
        return format!("Embedding({elem},{dim})");
    }
    // Parameterised vector-ish types print their element type and dimension, so
    // callers can recover both from `SHOW TABLE … COLUMNS`.
    for (kw, name) in [
        ("TENSORARRAY(", "TensorArray"),
        ("MULTIVECTOR(", "MultiVector"),
        ("TENSOR(", "Tensor"),
    ] {
        if up.starts_with(kw) {
            let inner = t[t.find('(').unwrap() + 1..].trim_end_matches(')');
            let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
            let elem = parts
                .first()
                .map(|s| s.to_ascii_lowercase())
                .unwrap_or_default();
            let dim = parts.get(1).map(|s| s.trim()).unwrap_or("");
            return format!("{name}({elem},{dim})");
        }
    }
    let base = up.split('(').next().unwrap_or(&up);
    match base {
        "BOOL" | "BOOLEAN" => "Boolean".to_string(),
        "TINYINT" => "TinyInt".to_string(),
        "SMALLINT" => "SmallInt".to_string(),
        "INT" | "INTEGER" => "Integer".to_string(),
        "BIGINT" => "BigInt".to_string(),
        "HUGEINT" => "HugeInt".to_string(),
        "DECIMAL" => "Decimal".to_string(),
        "FLOAT" => "Float".to_string(),
        "DOUBLE" => "Double".to_string(),
        "FLOAT16" => "Float16".to_string(),
        "BFLOAT16" => "BFloat16".to_string(),
        "VARCHAR" | "TEXT" | "STRING" => "Varchar".to_string(),
        "DATE" => "Date".to_string(),
        "TIME" => "Time".to_string(),
        "DATETIME" => "DateTime".to_string(),
        "TIMESTAMP" => "Timestamp".to_string(),
        "INTERVAL" => "Interval".to_string(),
        "JSON" => "Json".to_string(),
        _ => {
            let mut c = base.to_ascii_lowercase();
            if let Some(f) = c.get_mut(0..1) {
                f.make_ascii_uppercase();
            }
            c
        }
    }
}

pub(crate) fn is_boolean_expr(e: &ExprAst) -> bool {
    match e {
        ExprAst::Cmp(..) | ExprAst::And(..) | ExprAst::Or(..) | ExprAst::Not(..) => true,
        // NULL is "unknown" and is valid wherever a boolean is expected; a
        // column may hold a boolean (its type is not visible here).
        ExprAst::Literal(crate::Literal::Null) | ExprAst::Column(_) => true,
        // `TRUE`/`FALSE` parse to lower-case string literals.
        ExprAst::Literal(crate::Literal::Str(v)) => {
            v.eq_ignore_ascii_case("true") || v.eq_ignore_ascii_case("false")
        }
        ExprAst::Func { name, .. } => [
            "IS_NULL",
            "IS_NOT_NULL",
            "LIKE",
            "CONTAINS",
            "REGEX",
            "FILTER_TEXT",
            "FILTER_FULLTEXT",
        ]
        .iter()
        .any(|n| name.eq_ignore_ascii_case(n)),
        _ => false,
    }
}

/// The catalog-side `SPARSE` storage hints for a parsed column list.
pub(crate) fn sparse_store_hints(columns: &[ColumnDef]) -> Vec<Option<CatalogSparseStore>> {
    columns
        .iter()
        .map(|c| {
            c.sparse_store.map(|s| match s {
                ParserSparseStore::Sort => CatalogSparseStore::Sort,
                ParserSparseStore::Sorted => CatalogSparseStore::Sorted,
            })
        })
        .collect()
}

/// Width of a sparse column's index element (`SPARSE(FLOAT, INT8)` etc.).
/// `WITH (block_size = N)` of a BMP index (`IndexBMP::Init`, `index_bmp_impl.cpp:43`).
/// The reference groups a term's postings into blocks of this many documents;
/// the default is `BMP_BLOCK_SIZE`.
pub(crate) fn index_block_size(im: &catalog::IndexMeta) -> usize {
    im.options
        .iter()
        .find(|(k, _)| k.trim().eq_ignore_ascii_case("block_size"))
        .and_then(|(_, v)| v.trim().parse::<usize>().ok())
        .filter(|n| (1..=256).contains(n))
        .unwrap_or(index::sparse::BMP_BLOCK_SIZE)
}

pub(crate) fn sparse_idx_width(type_text: &str) -> storage::bmp_cpp::IdxWidth {
    use storage::bmp_cpp::IdxWidth;
    let t = type_text.to_ascii_uppercase();
    if t.contains("INT8") || t.contains("TINYINT") {
        IdxWidth::I8
    } else if t.contains("INT16") || t.contains("SMALLINT") {
        IdxWidth::I16
    } else {
        IdxWidth::I32
    }
}

pub(crate) fn is_int_type_text(type_text: &str) -> bool {
    let base = type_text
        .trim()
        .to_ascii_uppercase()
        .split('(')
        .next()
        .unwrap_or("")
        .to_string();
    matches!(
        base.as_str(),
        "INT" | "INTEGER" | "TINYINT" | "SMALLINT" | "BIGINT" | "HUGEINT"
    )
}

pub(crate) fn is_bool_type_text(type_text: &str) -> bool {
    matches!(
        type_text.trim().to_ascii_uppercase().as_str(),
        "BOOL" | "BOOLEAN"
    )
}

pub(crate) fn sparse_doc_bytes(doc: &[(i32, f32)]) -> usize {
    doc.len() * 8
}

/// Only a plain secondary index materialises into the string-keyed
/// `SecondaryIndex` map. Vector/fulltext/sparse indexes are served by their own
/// search paths, and mirroring a 2048-dim vector into a per-row string key
/// would cost gigabytes on a large table.
pub(crate) fn is_secondary_kind(index_type: &str) -> bool {
    let t = index_type.trim();
    t.is_empty()
        || t.eq_ignore_ascii_case("SECONDARY")
        || t.eq_ignore_ascii_case("SECONDARYFUNCTIONAL")
}

/// Wide values of these columns are kept in the row sidecar instead of the KV
/// row (Gate C memory).
pub(crate) fn is_blob_type(t: &str) -> bool {
    is_embedding_type(t) || is_tensor_type(t) || is_sparse_type(t)
}

/// Inner product over the shared indices of two sparse vectors.
pub(crate) fn sparse_dot(q: &[(usize, f64)], v: &[(usize, f64)]) -> f64 {
    let mut sum = 0.0;
    let (mut i, mut j) = (0usize, 0usize);
    while i < q.len() && j < v.len() {
        match q[i].0.cmp(&v[j].0) {
            std::cmp::Ordering::Equal => {
                sum += q[i].1 * v[j].1;
                i += 1;
                j += 1;
            }
            std::cmp::Ordering::Less => i += 1,
            std::cmp::Ordering::Greater => j += 1,
        }
    }
    sum
}

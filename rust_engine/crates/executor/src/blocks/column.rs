use super::*;

/// Maps an element-type name to the SDK `ElementType` code. `None` for bit
/// elements, which this writer does not pack.
pub(crate) fn element_code(elem: &str) -> Option<i32> {
    Some(if elem.contains("UINT8") {
        1
    } else if elem.contains("INT8") || elem.contains("TINYINT") {
        2
    } else if elem.contains("INT16") {
        3
    } else if elem.contains("INT32") || elem == "INT" {
        4
    } else if elem.contains("INT64") {
        5
    } else if elem.contains("FLOAT64") || elem.contains("DOUBLE") {
        7
    } else if elem.contains("BFLOAT16") {
        9
    } else if elem.contains("FLOAT16") {
        8
    } else if elem.contains("BIT") {
        0 // Bit (packed to ceil(dim/8) bytes; see `embedding_size`)
    } else {
        6 // Float32
    })
}

/// The index width implied by the bound (`SparseInfo::GetIndexType`,
/// `src/parser/type/info/sparse_info.cpp:44-58`).
pub(crate) fn derived_index_kind(bound: u64) -> (i32, usize) {
    if bound <= 127 {
        (2, 1)
    } else if bound <= 32767 {
        (3, 2)
    } else if bound <= 2_147_483_647 {
        (4, 4)
    } else {
        (5, 8)
    }
}

/// Encodes one sparse value: appends `indices` then `values` to `outline` and
/// returns the `i64 nnz + i64 offset` slot.
pub(crate) fn encode_sparse_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (elem, _index_type, index_width) = sparse_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a sparse type: '{type_text}'")))?;
    let (indices, values) = parse_sparse_literal(value);
    let offset = outline.len() as u64;
    for i in &indices {
        match index_width {
            1 => outline.push(*i as i8 as u8),
            2 => outline.extend_from_slice(&(*i as i16).to_le_bytes()),
            4 => outline.extend_from_slice(&(*i as i32).to_le_bytes()),
            _ => outline.extend_from_slice(&i.to_le_bytes()),
        }
    }
    // A BIT data type stores no values (`SparseInfo::DataSize` returns 0 for
    // `kElemBit`, `sparse_info.h:61-66`): the column is a bitmap of the
    // present indices and the value is implicitly 1.
    if elem != 0 {
        for v in &values {
            storage::definition::format::push_embedding_element(outline, elem, *v);
        }
    }
    Ok(Some(
        storage::definition::format::sparse_slot(indices.len() as i64, offset).to_vec(),
    ))
}

/// Decodes one sparse slot out of the outline.
pub(crate) fn decode_sparse_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let (elem, _index_type, index_width) = sparse_shape(type_text)?;
    let nnz = i64::from_le_bytes(slot.get(..8)?.try_into().ok()?) as usize;
    let offset = u64::from_le_bytes(slot.get(8..16)?.try_into().ok()?) as usize;
    // A BIT data type stores no value bytes; the value is implicitly 1.
    let elem_width = if elem == 0 { 0 } else { element_width(elem) };
    let mut idx_at = offset;
    let mut val_at = offset + nnz * index_width;
    if nnz == 0 {
        // The stored text of an empty sparse vector is the reference's marker
        // (`reshape_sparse`): a `SELECT` prints it as-is.
        return Some("(empty)".to_string());
    }
    let mut parts = Vec::with_capacity(nnz);
    for _ in 0..nnz {
        let i = match index_width {
            1 => f64::from(*outline.get(idx_at)? as i8),
            2 => f64::from(i16::from_le_bytes(
                outline.get(idx_at..idx_at + 2)?.try_into().ok()?,
            )),
            4 => f64::from(i32::from_le_bytes(
                outline.get(idx_at..idx_at + 4)?.try_into().ok()?,
            )),
            _ => i64::from_le_bytes(outline.get(idx_at..idx_at + 8)?.try_into().ok()?) as f64,
        };
        idx_at += index_width;
        let v = if elem == 0 {
            1.0
        } else {
            storage::definition::format::read_embedding_element(
                outline.get(val_at..val_at + elem_width)?,
                elem,
            )?
        };
        val_at += elem_width;
        // A `kElemBit` column lists the present positions only, and a float
        // element prints with the reference's six decimals
        // (`SparseT::ToString` -> `std::to_string`); an integer one is exact.
        parts.push(if elem == 0 {
            format!("{}", i as i64)
        } else if elem >= 6 {
            format!("{}:{v:.6}", i as i64)
        } else {
            format!("{}:{}", i as i64, v as i64)
        });
    }
    Some(format!("[{}]", parts.join(",")))
}

/// Width of a fixed-width scalar's slot, `None` for variable-length ones.
pub(crate) fn fixed_scalar_width(col: i32) -> Option<usize> {
    match storage::definition::format::column_layout(col)? {
        ColumnLayout::Fixed(w) => Some(w),
        ColumnLayout::CompactBit => Some(1),
        ColumnLayout::VarLen(_) => None,
    }
}

/// Renders an engine-rendered value as a SQL literal for re-insertion.
///
/// Text-valued columns (varchar/json/date/time/datetime/timestamp) must be
/// quoted; numbers, booleans, arrays, tensors and sparse vectors are already in
/// literal form. Single quotes are doubled, which is correct SQL — note our own
/// parser does not currently accept `''`, so values containing an apostrophe
/// cannot be re-inserted until it does.
pub(crate) fn sql_literal(type_text: &str, value: &str) -> String {
    if value.eq_ignore_ascii_case("null") {
        return "NULL".to_string();
    }
    let up = type_text.to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    // A BIT embedding's stored text is the display form (`[10110001]`); SQL
    // input takes one number per element (`[1,0,1,1,0,0,0,1]`).
    if let Some((dim, elem)) = embedding_shape(&up) {
        let inner = value.trim().trim_start_matches('[').trim_end_matches(']');
        if elem == 0
            && dim > 0
            && !inner.contains(',')
            && inner.len() == dim
            && inner.chars().all(|c| c == '0' || c == '1')
        {
            let cells: Vec<&str> = inner
                .chars()
                .map(|c| if c == '1' { "1" } else { "0" })
                .collect();
            return format!("[{}]", cells.join(","));
        }
    }
    let quoted = matches!(
        base,
        "VARCHAR" | "TEXT" | "STRING" | "JSON" | "DATE" | "TIME" | "DATETIME" | "TIMESTAMP"
    );
    if quoted {
        format!("'{}'", value.replace('\'', "''"))
    } else if is_float_type_text(base)
        && !value.contains(['.', 'e', 'E'])
        && value.trim().parse::<i64>().is_ok()
    {
        // A float column's text may have lost its digits (`1`, `-0`). SQL
        // would read that as an integer literal, which drops the sign of a
        // negative zero and the type of the value; `.0` keeps it a float.
        format!("{value}.0")
    } else {
        value.to_string()
    }
}

/// Whether a base type name (already uppercased, without parameters) is one of
/// the engine's float types.
pub(crate) fn is_float_type_text(base: &str) -> bool {
    matches!(
        base,
        "FLOAT" | "DOUBLE" | "REAL" | "FP16" | "BF16" | "FLOAT16" | "BFLOAT16"
    )
}

/// Maps a declared type text to the `ColumnType` code this writer packs.
///
/// Returns `None` for shapes the packer does not cover yet
/// (array/tensor/sparse, and bit embeddings); callers report those instead of
/// writing a mis-encoded file.
pub fn column_type_of(type_text: &str) -> Option<i32> {
    let t = type_text.trim().to_ascii_uppercase();
    let base = t.split('(').next().unwrap_or("").trim();
    Some(match base {
        "BOOL" | "BOOLEAN" => 0,
        "TINYINT" | "INT8" => 1,
        "SMALLINT" | "INT16" => 2,
        "INT" | "INTEGER" | "INT32" => 3,
        "BIGINT" | "INT64" => 4,
        "FLOAT" | "REAL" | "FLOAT32" => 5,
        "DOUBLE" | "FLOAT64" => 6,
        // The 16-bit floats are two bytes wide (`float16_t` / `bfloat16_t`,
        // `src/parser/type/number/float16.h`).
        "FLOAT16" | "FP16" | "HALF" => 7,
        "BFLOAT16" | "BF16" => 8,
        "VARCHAR" | "TEXT" | "STRING" => 9,
        "DATE" => 16,
        "TIME" => 17,
        "DATETIME" | "TIMESTAMP" => 18,
        "JSON" => 22,
        "EMBEDDING" | "VECTOR" => {
            embedding_shape(type_text)?;
            10
        }
        "TENSOR" | "MULTIVECTOR" => {
            tensor_shape(type_text)?;
            if base == "TENSOR" {
                11
            } else {
                14
            }
        }
        "TENSORARRAY" => {
            tensorarray_shape(type_text)?;
            12
        }
        "SPARSE" => {
            sparse_shape(type_text)?;
            13
        }
        "ARRAY" => {
            array_shape(type_text)?;
            21
        }
        _ => return None,
    })
}

/// Which self-outlining encoder a column needs (its outline is built by the
/// caller rather than by the generic var-length packer).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) enum OutlineKind {
    None,
    Tensor,
    TensorArray,
    Sparse,
    Array,
}

pub(crate) fn outline_kind(type_text: &str) -> OutlineKind {
    if tensorarray_shape(type_text).is_some() {
        OutlineKind::TensorArray
    } else if tensor_shape(type_text).is_some() {
        OutlineKind::Tensor
    } else if sparse_shape(type_text).is_some() {
        OutlineKind::Sparse
    } else if array_shape(type_text).is_some() {
        OutlineKind::Array
    } else {
        OutlineKind::None
    }
}

/// The `.col` layout of a column, including embedding columns whose fixed width
/// depends on the declared dimension.
pub(crate) fn layout_of(type_text: &str, column_type: i32) -> Option<ColumnLayout> {
    if let Some((dim, elem)) = embedding_shape(type_text) {
        // A BIT vector packs to `ceil(dim/8)` bytes, not `dim` (`:253-262`).
        return Some(ColumnLayout::Fixed(embedding_size(elem, dim)));
    }
    if tensor_shape(type_text).is_some() {
        // Tensor / MultiVector: a `{count:16; offset:48}` slot into the outline.
        return Some(ColumnLayout::VarLen(8));
    }
    if tensorarray_shape(type_text).is_some() {
        // TensorArray: the same slot; the outline holds every tensor's body and
        // their `TensorT` meta array.
        return Some(ColumnLayout::VarLen(8));
    }
    if sparse_shape(type_text).is_some() {
        // Sparse: `i64 nnz` + `i64 offset`.
        return Some(ColumnLayout::VarLen(16));
    }
    if array_shape(type_text).is_some() {
        // Array: a `{count:16; offset:48}` slot into the outline.
        return Some(ColumnLayout::VarLen(8));
    }
    storage::definition::format::column_layout(column_type)
}

/// The text a cell is encoded from.
///
/// A float is rendered with `Display` - the shortest form that parses back to
/// the same bits - because the block stores the raw value: `to_slt_string`'s
/// six decimals would round the value (and the bytes) away from the reference's.
pub(crate) fn mirror_cell_text(value: &Value) -> String {
    match value {
        // A float cell has to stay recognisable as one: `format!("{f}")` renders
        // `4.0` as `4`, and the reader of this text (`parse_value`) answers an
        // `Int` for that, so a FLOAT column with an integral value came back as
        // an integer - and printed `4` where the reference prints `4.000000`.
        // `exact_float` keeps the shortest round-tripping digits and leaves the
        // `.0` in place.
        Value::Float(f) => crate::exact_float(*f),
        other => other.to_slt_string(),
    }
}

pub(crate) fn encode_value(
    type_text: &str,
    column_type: i32,
    value: &str,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let bad = || {
        Error::Invalid(format!(
            "value '{value}' is not valid for column type {column_type}"
        ))
    };
    if let Some((dim, elem)) = embedding_shape(type_text) {
        let inner = value.trim().trim_start_matches('[').trim_end_matches(']');
        if elem == 0 {
            // BIT: the engine (and the C++ `ToString`, `embedding_type.h:253-262`)
            // renders the vector as a bit string like `[10110001]`; also accept
            // the comma-separated numeric form. Pack to `ceil(dim/8)` bytes with
            // value `j` at byte `j/8`, bit `j%8` (LSB-first).
            let bits: Vec<bool> = if inner.contains(',') {
                inner
                    .split(',')
                    .filter_map(|x| x.trim().parse::<f64>().ok())
                    .map(|v| v != 0.0)
                    .collect()
            } else {
                inner.chars().map(|c| c == '1').collect()
            };
            let mut bytes = vec![0u8; embedding_size(elem, dim)];
            for k in 0..dim.min(bits.len()) {
                if bits[k] {
                    bytes[k / 8] |= 1 << (k % 8);
                }
            }
            return Ok(Some(bytes));
        }
        // `[a,b,c]` → `dim` elements, zero-padded/truncated, raw little-endian.
        let nums: Vec<f64> = inner
            .split(',')
            .filter_map(|x| x.trim().parse::<f64>().ok())
            .collect();
        let mut bytes = Vec::with_capacity(embedding_size(elem, dim));
        for k in 0..dim {
            push_embedding_element(&mut bytes, elem, nums.get(k).copied().unwrap_or(0.0));
        }
        return Ok(Some(bytes));
    }
    let bytes = match column_type {
        0 => {
            // `pack_column`'s compact-bit path keys off these textual forms.
            if value.eq_ignore_ascii_case("true") || value == "1" {
                b"1".to_vec()
            } else {
                b"0".to_vec()
            }
        }
        1 => vec![value.trim().parse::<i8>().map_err(|_| bad())? as u8],
        2 => value
            .trim()
            .parse::<i16>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        3 => value
            .trim()
            .parse::<i32>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        4 => value
            .trim()
            .parse::<i64>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        5 => value
            .trim()
            .parse::<f32>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        6 => value
            .trim()
            .parse::<f64>()
            .map_err(|_| bad())?
            .to_le_bytes()
            .to_vec(),
        // The 16-bit floats: an IEEE half and a bfloat whose raw bits are the
        // high half of an `f32` (`bfloat16_t`).
        7 => crate::f32_to_f16_bits(value.trim().parse::<f32>().map_err(|_| bad())?)
            .to_le_bytes()
            .to_vec(),
        8 => ((value.trim().parse::<f32>().map_err(|_| bad())?.to_bits() >> 16) as u16)
            .to_le_bytes()
            .to_vec(),
        16 => date_to_days(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        17 => time_to_secs(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        18 => {
            let (d, secs) = datetime_to_pair(value).ok_or_else(bad)?;
            let mut v = d.to_le_bytes().to_vec();
            v.extend_from_slice(&secs.to_le_bytes());
            v
        }
        // Json is stored as BSON, not text: the reference writes
        // `JsonManager::to_bson` - the value wrapped in `{"data": …}`, keys
        // sorted, small integers as `int32` (`json_manager_impl.cpp:139`).
        _ if type_text.trim().eq_ignore_ascii_case("json") => {
            ::common::json::parse(value).ok_or_else(bad)?.to_bson()
        }
        // Varchar keeps its UTF-8 bytes.
        _ => value.as_bytes().to_vec(),
    };
    Ok(Some(bytes))
}

/// The commit ts of the append that wrote row `offset`, from the append log.
///
/// `totals[i]` is the cumulative row count after append `i` and `ts[i]` its
/// commit ts; a row lands in the first append whose total passes it.
pub(crate) fn append_ts_of(offset: u64, totals: &[u64], ts: &[u64]) -> Option<u64> {
    let i = totals.partition_point(|total| *total <= offset);
    ts.get(i).copied().filter(|t| *t != 0)
}

/// The block a row id lands in: `(segment, block)` of the mirror's layout.
pub(crate) fn block_of(seq: usize) -> (u32, u16) {
    (
        (seq >> 32) as u32,
        ((seq & 0xffff_ffff) / DEFAULT_BLOCK_CAPACITY) as u16,
    )
}

/// The buffer a column of `declared` type is decoded into.
pub(crate) fn column_buffer(declared: &str) -> ColumnData {
    let t = declared.trim().to_ascii_uppercase();
    let base = t.split('(').next().unwrap_or("").trim();
    match base {
        "BOOL" | "BOOLEAN" => ColumnData::Bool(Vec::new()),
        "FLOAT" | "DOUBLE" | "FLOAT16" | "BFLOAT16" => ColumnData::Float(Vec::new()),
        "TINYINT" | "SMALLINT" | "INT" | "INTEGER" | "BIGINT" | "UINT8" | "UINT16" | "UINT32"
        | "UINT64" => ColumnData::Int(Vec::new()),
        _ => ColumnData::Text(crate::columnar::TextColumn::default()),
    }
}

pub(crate) fn decode_value(
    type_text: &str,
    column_type: i32,
    body: &[u8],
    row: usize,
    outline: &[u8],
) -> Option<String> {
    decode_value_shaped(
        type_text,
        column_type,
        body,
        row,
        outline,
        embedding_shape(type_text),
    )
}

/// [`decode_value`] with the `EMBEDDING`/`VECTOR` shape already parsed.
///
/// The shape belongs to the column, not the cell: a scan parses it once per
/// column (`decode_rows_of`) instead of once per cell, which also removes the
/// uppercased copy of the type text that `embedding_shape` allocates.
pub(crate) fn decode_value_shaped(
    type_text: &str,
    column_type: i32,
    body: &[u8],
    row: usize,
    outline: &[u8],
    shape: Option<(usize, i32)>,
) -> Option<String> {
    if let Some((dim, elem)) = shape {
        if elem == 0 {
            // BIT: emit the comma-separated form the engine's SQL accepts
            // (its *display* form is the bit string `[10110001]`, which
            // `encode_value` also accepts). Bit `j` at byte `j/8`, bit `j%8`.
            let at = row * embedding_size(elem, dim);
            let mut parts = String::with_capacity(dim);
            for k in 0..dim {
                let byte = body.get(at + k / 8)?;
                parts.push(if byte & (1 << (k % 8)) != 0 { '1' } else { '0' });
            }
            return Some(format!("[{parts}]"));
        }
        // Inline `dim` elements per row.
        let width = element_width(elem);
        let at = row * dim * width;
        let mut parts = Vec::with_capacity(dim);
        for k in 0..dim {
            let s = body.get(at + k * width..at + (k + 1) * width)?;
            parts.push(embedding_element_text(s, elem)?);
        }
        return Some(format!("[{}]", parts.join(",")));
    }
    match layout_of(type_text, column_type)? {
        ColumnLayout::CompactBit => {
            let bit = body.get(row / 8).map(|b| b & (1 << (row % 8)) != 0)?;
            // The engine renders booleans as `true`/`false`.
            Some(if bit { "true".into() } else { "false".into() })
        }
        ColumnLayout::Fixed(width) => {
            let at = row * width;
            let slot = body.get(at..at + width)?;
            Some(match column_type {
                1 => (slot[0] as i8).to_string(),
                2 => i16::from_le_bytes(slot.try_into().unwrap()).to_string(),
                3 => i32::from_le_bytes(slot.try_into().unwrap()).to_string(),
                4 => i64::from_le_bytes(slot.try_into().unwrap()).to_string(),
                // The cell text has to stay a *float*: `parse_value` answers an
                // `Int` for a spelling without a decimal point, so a FLOAT
                // column holding `0` would print `0` where the reference
                // prints `0.000000`. `exact_float` keeps the shortest
                // round-tripping digits and leaves the `.0` in place, so a
                // rewrite of the block reproduces the same bits.
                5 => crate::exact_float(f64::from(f32::from_le_bytes(slot.try_into().unwrap()))),
                6 => crate::exact_float(f64::from_le_bytes(slot.try_into().unwrap())),
                // The 16-bit floats are two bytes wide and render like the
                // other floats: an IEEE half (`float16_t`) and a bfloat whose
                // raw bits are the high half of an `f32` (`bfloat16_t`).
                7 => crate::exact_float(f64::from(crate::f16_bits_to_f32(u16::from_le_bytes(
                    slot.try_into().unwrap(),
                )))),
                8 => crate::exact_float(f64::from(f32::from_bits(
                    (u16::from_le_bytes(slot.try_into().unwrap()) as u32) << 16,
                ))),
                // Date/time values are stored as integers but must come back as
                // text, matching how the engine renders them.
                16 => storage::definition::format::days_to_date(i32::from_le_bytes(
                    slot.try_into().unwrap(),
                )),
                17 => storage::definition::format::secs_to_time(i32::from_le_bytes(
                    slot.try_into().unwrap(),
                )),
                18 => {
                    let d = i32::from_le_bytes(slot[..4].try_into().unwrap());
                    let t = i32::from_le_bytes(slot[4..8].try_into().unwrap());
                    format!(
                        "{} {}",
                        storage::definition::format::days_to_date(d),
                        storage::definition::format::secs_to_time(t)
                    )
                }
                _ => return None,
            })
        }
        ColumnLayout::VarLen(16) => {
            // Varchar/Json (`varchar.h`): word 0 starts with the 24-bit length.
            // A payload of at most `VARCHAR_INLINE_LENGTH` (13) is inlined in the
            // remaining 13 bytes; a longer one lives in the outline and the slot
            // carries the 5-byte prefix plus the offset.
            let at = row * 16;
            let w0 = u64::from_le_bytes(body.get(at..at + 8)?.try_into().ok()?);
            let len = (w0 & 0x00ff_ffff) as usize;
            let bytes = if len <= 13 {
                &body.get(at + 3..at + 16)?[..len]
            } else {
                let offset =
                    u64::from_le_bytes(body.get(at + 8..at + 16)?.try_into().ok()?) as usize;
                outline.get(offset..offset + len)?
            };
            // Json cells are BSON (`JsonManager::to_bson`), so a JSON column is
            // rendered back to its text form; varchar is already text.
            if is_json_column(type_text) {
                Some(match ::common::json::Json::from_bson(bytes) {
                    Some(json) => json.compact(),
                    None => text_or_lossy(bytes),
                })
            } else {
                Some(text_or_lossy(bytes))
            }
        }
        ColumnLayout::VarLen(_) => None,
    }
}

/// Decodes one cell of a text column straight into its buffer: no `Value`, no
/// intermediate `String` for varchar/json.
pub(crate) fn decode_text_cell(
    type_text: &str,
    column_type: i32,
    layout: ColumnLayout,
    body: &[u8],
    row: usize,
    outline: &[u8],
    text: &mut crate::columnar::TextColumn,
) -> Option<bool> {
    match layout {
        ColumnLayout::VarLen(16) => {
            let at = row * 16;
            let w0 = u64::from_le_bytes(body.get(at..at + 8)?.try_into().ok()?);
            let len = (w0 & 0x00ff_ffff) as usize;
            let bytes = if len <= 13 {
                &body.get(at + 3..at + 16)?[..len]
            } else {
                let offset =
                    u64::from_le_bytes(body.get(at + 8..at + 16)?.try_into().ok()?) as usize;
                outline.get(offset..offset + len)?
            };
            if type_text.trim().eq_ignore_ascii_case("json") {
                let cell = match ::common::json::Json::from_bson(bytes) {
                    Some(json) => json.compact(),
                    None => String::from_utf8_lossy(bytes).to_string(),
                };
                text.push(&cell);
                return Some(true);
            }
            let cell = std::str::from_utf8(bytes).unwrap_or("");
            // A varchar cell spelled `null` is the SQL NULL marker on the row
            // path (`parse_owned_value`); keep that reading.
            if cell.eq_ignore_ascii_case("null") {
                text.push("");
                Some(false)
            } else {
                text.push(cell);
                Some(true)
            }
        }
        ColumnLayout::Fixed(width) => {
            let at = row * width;
            let slot = body.get(at..at + width)?;
            let cell = match column_type {
                16 => storage::definition::format::days_to_date(i32::from_le_bytes(
                    slot.try_into().ok()?,
                )),
                17 => storage::definition::format::secs_to_time(i32::from_le_bytes(
                    slot.try_into().ok()?,
                )),
                18 => {
                    let d = i32::from_le_bytes(slot[..4].try_into().ok()?);
                    let t = i32::from_le_bytes(slot[4..8].try_into().ok()?);
                    format!(
                        "{} {}",
                        storage::definition::format::days_to_date(d),
                        storage::definition::format::secs_to_time(t)
                    )
                }
                _ => return None,
            };
            text.push(&cell);
            Some(true)
        }
        ColumnLayout::CompactBit => {
            let bit = body.get(row / 8).map(|b| b & (1 << (row % 8)) != 0)?;
            text.push(if bit { "true" } else { "false" });
            Some(true)
        }
        ColumnLayout::VarLen(_) => None,
    }
}

/// The `Value` of a fixed-width scalar slot, or `None` for a type this does not
/// decode (`DATE`/`TIME`/`DATETIME`, whose cell is rendered text).
pub(crate) fn decode_scalar_value(
    column_type: i32,
    width: usize,
    body: &[u8],
    row: usize,
) -> Option<Value> {
    let at = row.checked_mul(width)?;
    let slot = body.get(at..at + width)?;
    Some(match column_type {
        1 => Value::Int(slot[0] as i8 as i64),
        2 => Value::Int(i16::from_le_bytes(slot.try_into().ok()?) as i64),
        3 => Value::Int(i32::from_le_bytes(slot.try_into().ok()?) as i64),
        4 => Value::Int(i64::from_le_bytes(slot.try_into().ok()?)),
        5 => Value::Float(f64::from(f32::from_le_bytes(slot.try_into().ok()?))),
        6 => Value::Float(f64::from_le_bytes(slot.try_into().ok()?)),
        7 => Value::Float(f64::from(crate::f16_bits_to_f32(u16::from_le_bytes(
            slot.try_into().ok()?,
        )))),
        8 => Value::Float(f64::from(f32::from_bits(
            (u16::from_le_bytes(slot.try_into().ok()?) as u32) << 16,
        ))),
        _ => return None,
    })
}

/// [`crate::parse_value`] for a cell the decoder already owns, so a text cell
/// moves into `Value::Str` instead of being copied into it.
/// `JSON` / `json` / ` json `: the column's declared type, which the cell
/// decoder re-tests for every cell it builds.
pub(crate) fn is_json_column(type_text: &str) -> bool {
    let t = type_text.trim();
    t.len() == 4 && t.eq_ignore_ascii_case("json")
}

/// The cell's text. `String::from_utf8_lossy` walks the bytes as UTF-8 chunks
/// even when they are valid, which is every cell of a well-formed table; try
/// `str::from_utf8` first and keep the lossy form for the rest.
pub(crate) fn text_or_lossy(bytes: &[u8]) -> String {
    match std::str::from_utf8(bytes) {
        Ok(text) => text.to_string(),
        Err(_) => String::from_utf8_lossy(bytes).into_owned(),
    }
}

pub(crate) fn parse_owned_value(cell: String) -> Value {
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
        Value::Str(cell)
    }
}

/// The types the reference stores in a variable-length buffer
/// (`ColumnVector::GetVectorBufferType` -> `kVarBuffer`): the data of such a
/// column is preceded by an outline file, `col_{id}_out`.
pub(crate) fn is_var_buffer_type(type_code: i32) -> bool {
    use catalog::value_format::logical;
    matches!(
        type_code as i64,
        logical::VARCHAR
            | logical::ARRAY
            | logical::EMBEDDING
            | logical::TENSOR
            | logical::TENSORARRAY
            | logical::SPARSE
            | logical::MULTIVECTOR
            | logical::JSON
    )
}

/// One row at a time out of a table's blocks, in row-id order.
///
/// [`Engine::block_cursor`] builds it: the visible blocks are resolved once
/// (the same `visible_block_ids` the scan uses), and each block is decoded on
/// first touch and kept until the walk moves to the next one. That makes a
/// compaction - which reads every row of its source segments and writes them
/// into one new segment - one pass over the blocks instead of one table scan
/// per moved row.
/// A snapshot of one table's append log, for a reader pinned to a timestamp.
pub(crate) struct AppendView {
    pub(crate) totals: Vec<u64>,
    pub(crate) stamps: Vec<u64>,
    pub(crate) bases: Vec<(u32, u64)>,
}

impl AppendView {
    /// Whether the append that produced `seq` had committed by `read_ts`.
    ///
    /// The records are cumulative and in row-id order, so the record that covers
    /// the row is the first total past its ordinal. A record with no stamp is
    /// one a replay rebuilt (or one written before the log kept stamps): it
    /// counts as committed, because hiding rows whose append ts was never
    /// recorded would lose them on every reopen.
    pub(crate) fn covers(&self, seq: usize, read_ts: u64) -> bool {
        if self.totals.is_empty() {
            return true;
        }
        let segment = (seq >> 32) as u32;
        let mut ordinal = seq;
        let mut seen = 0usize;
        for (id, rows) in &self.bases {
            if *id == segment {
                ordinal = seen + (seq & 0xffff_ffff);
                break;
            }
            seen += *rows as usize;
        }
        let index = self
            .totals
            .partition_point(|total| (*total as usize) <= ordinal);
        match self.stamps.get(index).copied().unwrap_or(0) {
            0 => true,
            stamp => stamp <= read_ts,
        }
    }
}

/// A scan predicate the column reader applies itself: column `pos` between
/// `lo` and `hi` (inclusive; either end open).
///
/// The caller still applies the full predicate to the rows that survive, so a
/// bound one row too wide (`>` mapped to `>=`) costs nothing - while the rows
/// it excludes are never decoded into a `Value` at all.
pub(crate) struct ColumnBounds {
    pub pos: usize,
    pub lo: Option<Value>,
    pub hi: Option<Value>,
}

impl ColumnBounds {
    /// The bounds a `WHERE` predicate implies for one column, or `None` when it
    /// is not a conjunction of comparisons on a single column.
    ///
    /// `>` and `<` become inclusive bounds: the row they exclude is one row the
    /// residual predicate then drops, so the narrowing stays exact.
    pub(crate) fn from_expr(expr: &Expr) -> Option<Self> {
        fn axis(expr: &Expr) -> Option<(usize, CmpOp, &Value)> {
            match expr {
                Expr::Cmp(a, op, b) => match (&**a, &**b) {
                    (Expr::Column(pos), Expr::Literal(value)) => Some((*pos, *op, value)),
                    _ => None,
                },
                _ => None,
            }
        }
        if let Some((pos, op, value)) = axis(expr) {
            let mut bounds = ColumnBounds {
                pos,
                lo: None,
                hi: None,
            };
            match op {
                CmpOp::Eq => {
                    bounds.lo = Some(value.clone());
                    bounds.hi = Some(value.clone());
                }
                CmpOp::Gt | CmpOp::Ge => bounds.lo = Some(value.clone()),
                CmpOp::Lt | CmpOp::Le => bounds.hi = Some(value.clone()),
                _ => return None,
            }
            return Some(bounds);
        }
        if let Expr::And(left, right) = expr {
            let (lpos, lop, lvalue) = axis(left)?;
            let (rpos, rop, rvalue) = axis(right)?;
            if lpos != rpos {
                return None;
            }
            let mut bounds = ColumnBounds {
                pos: lpos,
                lo: None,
                hi: None,
            };
            for (op, value) in [(lop, lvalue), (rop, rvalue)] {
                match op {
                    CmpOp::Gt | CmpOp::Ge => bounds.lo = Some(value.clone()),
                    CmpOp::Lt | CmpOp::Le => bounds.hi = Some(value.clone()),
                    CmpOp::Eq => {
                        bounds.lo = Some(value.clone());
                        bounds.hi = Some(value.clone());
                    }
                    _ => return None,
                }
            }
            return Some(bounds);
        }
        None
    }
}

impl ColumnBounds {
    /// The bounds a `WHERE` predicate implies for one column, from the parsed
    /// form the DML statements carry (`UPDATE`/`DELETE` scan for their rows).
    pub(crate) fn from_predicate(schema: &[String], pred: &parser::Predicate) -> Option<Self> {
        use parser::{BoolOp, CmpOp, ExprAst, Predicate as P};
        fn axis(p: &P) -> Option<(&str, CmpOp, &parser::Literal)> {
            match p {
                P::Cmp(ExprAst::Column(name), op, ExprAst::Literal(value)) => {
                    Some((name.as_str(), *op, value))
                }
                _ => None,
            }
        }
        let position = |name: &str| schema.iter().position(|c| c == name);
        let mut bounds;
        if let Some((column, op, value)) = axis(pred) {
            bounds = ColumnBounds {
                pos: position(column)?,
                lo: None,
                hi: None,
            };
            match op {
                CmpOp::Eq => {
                    bounds.lo = Some(Value::from_literal(value));
                    bounds.hi = Some(Value::from_literal(value));
                }
                CmpOp::Gt | CmpOp::Ge => bounds.lo = Some(Value::from_literal(value)),
                CmpOp::Lt | CmpOp::Le => bounds.hi = Some(Value::from_literal(value)),
                _ => return None,
            }
            return Some(bounds);
        }
        if let P::Bool(left, BoolOp::And, right) = pred {
            let (lcolumn, lop, lvalue) = axis(left)?;
            let (rcolumn, rop, rvalue) = axis(right)?;
            if lcolumn != rcolumn {
                return None;
            }
            let mut bounds = ColumnBounds {
                pos: position(lcolumn)?,
                lo: None,
                hi: None,
            };
            for (op, value) in [(lop, lvalue), (rop, rvalue)] {
                match op {
                    CmpOp::Gt | CmpOp::Ge => bounds.lo = Some(Value::from_literal(value)),
                    CmpOp::Lt | CmpOp::Le => bounds.hi = Some(Value::from_literal(value)),
                    CmpOp::Eq => {
                        bounds.lo = Some(Value::from_literal(value));
                        bounds.hi = Some(Value::from_literal(value));
                    }
                    _ => return None,
                }
            }
            return Some(bounds);
        }
        None
    }
}

/// One block's column, read for a bounds check.
pub(crate) struct ColumnScan<'a> {
    pub(crate) data: &'a [u8],
    pub(crate) nulls: &'a [u8],
    pub(crate) layout: ColumnLayout,
    pub(crate) column_type: i32,
    pub(crate) lo: Option<Num>,
    pub(crate) hi: Option<Num>,
}

impl<'a> ColumnScan<'a> {
    /// Whether `slot` falls in the bounds. One 1-8 byte load and a compare; a
    /// NULL never matches (SQL three-valued logic drops it too).
    pub(crate) fn matches(&self, slot: usize) -> bool {
        if self.nulls.len() > slot / 8 && self.nulls[slot / 8] & (1 << (slot % 8)) == 0 {
            return false;
        }
        let width = match self.layout {
            ColumnLayout::Fixed(width) => width,
            // A bit-packed boolean has no byte slot to load, and a var-length
            // column's cell is not comparable without decoding it.
            ColumnLayout::CompactBit | ColumnLayout::VarLen(_) => return true,
        };
        let at = slot * width;
        let Some(bytes) = self.data.get(at..at + width) else {
            return true;
        };
        let in_range = |value: f64, lo: Option<Num>, hi: Option<Num>| -> bool {
            let low = match lo {
                Some(Num::F(f)) => value >= f,
                Some(Num::I(i)) => value >= i as f64,
                None => true,
            };
            let high = match hi {
                Some(Num::F(f)) => value <= f,
                Some(Num::I(i)) => value <= i as f64,
                None => true,
            };
            low && high
        };
        match self.column_type {
            // TINYINT/SMALLINT/INT/BIGINT (and DATE/TIME/DATETIME as ordered
            // counts), compared as integers so a 64-bit id stays exact.
            1 | 2 | 3 | 4 | 16 | 17 | 18 => {
                let value = match width {
                    1 => i64::from(bytes[0] as i8),
                    2 => i64::from(i16::from_le_bytes(bytes.try_into().unwrap())),
                    4 => i64::from(i32::from_le_bytes(bytes.try_into().unwrap())),
                    8 => i64::from_le_bytes(bytes.try_into().unwrap()),
                    _ => return true,
                };
                let low = match self.lo {
                    Some(Num::I(i)) => value >= i,
                    Some(Num::F(f)) => value as f64 >= f,
                    None => true,
                };
                let high = match self.hi {
                    Some(Num::I(i)) => value <= i,
                    Some(Num::F(f)) => value as f64 <= f,
                    None => true,
                };
                low && high
            }
            5 => in_range(
                f64::from(f32::from_le_bytes(bytes.try_into().unwrap())),
                self.lo,
                self.hi,
            ),
            6 => in_range(
                f64::from_le_bytes(bytes.try_into().unwrap()),
                self.lo,
                self.hi,
            ),
            // FLOAT16/BFLOAT16 and anything else keep every row: the residual
            // predicate decides.
            _ => true,
        }
    }
}

use super::*;

/// `EMBEDDING(<elem>,<dim>)` / `VECTOR(...)` → `(dimension, ElementType code)`.
pub fn embedding_shape(type_text: &str) -> Option<(usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    if !(up.starts_with("EMBEDDING(") || up.starts_with("VECTOR(")) {
        return None;
    }
    let inner = up.split_once('(')?.1.trim_end_matches(')');
    let mut parts = inner.split(',');
    let elem = parts.next()?.trim().to_string();
    let dim: usize = parts.next()?.trim().parse().ok()?;
    Some((dim, element_code(&elem)?))
}

/// `TENSOR(<elem>,<dim>)` / `MULTIVECTOR(<elem>,<dim>)` →
/// `(ColumnType, dimension, ElementType)`.
///
/// `TENSORARRAY` is excluded as a *clean* gap, not because it is ambiguous:
/// its slot (`TensorArrayT`, `tensor_array_type.h:28-36`) is the same
/// `{tensor_num:16; file_offset:48}`, and `file_offset_` points at
/// `tensor_num` 8-byte `TensorT` records (`SetTensorArray`,
/// `column_vector_impl.cpp:2792-2805`; `GetTensorArrayMeta`, `:2784-2790`),
/// each giving that tensor's own count and data offset. So the split *is*
/// derivable - implementing it needs a meta array written into the outline.
/// Bit elements are excluded for the same reason as embeddings.
pub fn tensor_shape(type_text: &str) -> Option<(i32, usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    let (column_type, rest) = if let Some(r) = up.strip_prefix("MULTIVECTOR(") {
        (14, r)
    } else {
        let r = up.strip_prefix("TENSOR(")?;
        (11, r)
    };
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: usize = parts.get(1)?.parse().ok()?;
    let code = element_code(parts.first()?)?;
    // A bit tensor packs `dim` bits per row into `ceil(dim/8)` bytes, the same
    // element packing an `EMBEDDING(BIT, dim)` uses.
    Some((column_type, dim, code))
}

/// `TENSORARRAY(<elem>,<dim>)` → `(ColumnType, dimension, ElementType)`.
///
/// The slot (`TensorArrayT`, `tensor_array_type.h:28-36`) is the same
/// `{tensor_num:16; file_offset:48}` word as a tensor's, but `file_offset_`
/// points at `tensor_num` 8-byte `TensorT` records (`SetTensorArray`,
/// `column_vector_impl.cpp:2792-2805`; `GetTensorArrayMeta`, `:2784-2790`),
/// each of which carries that tensor's own element count and data offset. The
/// mirror therefore writes every tensor's body first and their meta array last.
pub fn tensorarray_shape(type_text: &str) -> Option<(i32, usize, i32)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("TENSORARRAY(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let dim: usize = parts.get(1)?.parse().ok()?;
    let code = element_code(parts.first()?)?;
    Some((12, dim, code))
}

/// `SPARSE(<elem>,<bound>)` / `SPARSE(<elem>,<index>,<bound>)` →
/// `(element_type, index_type, index_width)`.
///
/// The two-component form derives the index width from the bound. The
/// three-component form is what the C++ catalog and the REST physical type
/// carry, and there the width is the *declared* one — a `sparse,8,float,int`
/// column stores 4-byte indices even though a bound of 8 would imply int8.
pub fn sparse_shape(type_text: &str) -> Option<(i32, i32, usize)> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("SPARSE(")?;
    let inner = rest.trim_end_matches(')');
    let parts: Vec<&str> = inner.split(',').map(str::trim).collect();
    let elem = element_code(parts.first()?)?;
    let (explicit_index, bound_text) = if parts.len() >= 3 {
        (Some(parts[1]), parts[2])
    } else {
        (None, *parts.get(1)?)
    };
    let bound: u64 = bound_text.parse().unwrap_or(0);
    let (index_type, width) = match explicit_index.and_then(sparse_index_kind) {
        Some(kind) => kind,
        None => derived_index_kind(bound),
    };
    Some((elem, index_type, width))
}

/// `int8`/`tinyint`/… → `(index_type, index_width)`.
pub(crate) fn sparse_index_kind(name: &str) -> Option<(i32, usize)> {
    Some(match name {
        "INT8" | "TINYINT" | "UINT8" => (2, 1),
        "INT16" | "SMALLINT" => (3, 2),
        "INT" | "INTEGER" | "INT32" => (4, 4),
        "INT64" | "BIGINT" => (5, 8),
        _ => return None,
    })
}

/// Parses the engine's sparse rendering `[i:val,…]` / `(empty)`.
pub fn parse_sparse_literal(cell: &str) -> (Vec<i64>, Vec<f64>) {
    let inner = cell.trim();
    if inner == "(empty)" || inner.is_empty() || inner == "[]" {
        return (Vec::new(), Vec::new());
    }
    let inner = inner.trim_start_matches('[').trim_end_matches(']');
    let mut idx = Vec::new();
    let mut val = Vec::new();
    for part in inner.split(',') {
        let part = part.trim();
        if part.is_empty() {
            continue;
        }
        match part.split_once(':') {
            Some((i, v)) => {
                if let (Ok(i), Ok(v)) = (i.trim().parse::<i64>(), v.trim().parse::<f64>()) {
                    idx.push(i);
                    val.push(v);
                }
            }
            // An index-only literal is a `SPARSE(BIT, …)` vector: every listed
            // position is present and carries the implicit value 1.
            None => {
                if let Ok(i) = part.parse::<i64>() {
                    idx.push(i);
                    val.push(1.0);
                }
            }
        }
    }
    (idx, val)
}

/// Element shape of an `ARRAY(...)` column.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ArrayElem {
    /// A fixed-width scalar element (its `ColumnType`).
    Fixed(i32),
    /// An `ARRAY(<fixed scalar>)` element; the value is the inner scalar's
    /// `ColumnType`. The outer elements are 8-byte `ArrayType` slots into the
    /// outline, each pointing at its own elements — `ArrayInfo::ElemSize` is the
    /// inner `DataType::Size()`, which for an array is the 8-byte slot.
    Nested(i32),
    /// A `Varchar`/`Json` element; the outer elements are 16-byte `VarcharT`
    /// slots into the outline (their payload inlines or outlines as usual).
    VarLen(i32),
}

/// `ARRAY(<scalar>)` / `ARRAY(ARRAY(<scalar>))` → its element shape.
///
/// Scalar leaves may be fixed-width or `Varchar`/`Json`; a second nesting level
/// must bottom out in a fixed-width scalar. Anything else returns `None` and is
/// reported as unsupported by the caller.
pub fn array_shape(type_text: &str) -> Option<ArrayElem> {
    let up = type_text.trim().to_ascii_uppercase();
    let rest = up.strip_prefix("ARRAY(")?;
    let inner = rest.trim_end_matches(')').trim();
    if let Some(scalar) = inner.strip_prefix("ARRAY(") {
        let col = column_type_of(scalar.trim_end_matches(')').trim())?;
        fixed_scalar_width(col)?;
        return Some(ArrayElem::Nested(col));
    }
    let col = column_type_of(inner)?;
    match storage::definition::format::column_layout(col)? {
        ColumnLayout::Fixed(_) | ColumnLayout::CompactBit => Some(ArrayElem::Fixed(col)),
        ColumnLayout::VarLen(16) if matches!(col, 9 | 22) => Some(ArrayElem::VarLen(col)),
        ColumnLayout::VarLen(_) => None,
    }
}

/// Parses the engine's array rendering `{1,2,3}` into its elements.
pub fn parse_braces(cell: &str) -> Vec<String> {
    let t = cell.trim();
    let inner = if t.starts_with('{') && t.ends_with('}') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    };
    if inner.trim().is_empty() {
        return Vec::new();
    }
    inner.split(',').map(|s| s.trim().to_string()).collect()
}

/// Splits the body of a `{{…},{…}}` rendering into its top-level groups, e.g.
/// `{0,3},{1}` → `["{0,3}", "{1}"]`. A `{}` group is kept (as an empty one).
pub(crate) fn split_top_braces(inner: &str) -> Vec<String> {
    let mut out = Vec::new();
    let mut depth = 0i32;
    let mut cur = String::new();
    for c in inner.chars() {
        match c {
            '{' => {
                depth += 1;
                cur.push(c);
            }
            '}' => {
                depth -= 1;
                cur.push(c);
                if depth == 0 {
                    out.push(cur.trim().to_string());
                    cur.clear();
                }
            }
            ',' if depth == 0 => {}
            _ => cur.push(c),
        }
    }
    if !cur.trim().is_empty() {
        out.push(cur.trim().to_string());
    }
    out
}

/// Parses the engine's nested-array rendering `{{0,3},{1}}` into one inner
/// element list per outer element.
pub(crate) fn parse_nested_braces(cell: &str) -> Vec<Vec<String>> {
    let t = cell.trim();
    let inner = if t.starts_with('{') && t.ends_with('}') && t.len() >= 2 {
        &t[1..t.len() - 1]
    } else {
        t
    };
    split_top_braces(inner)
        .iter()
        .map(|g| parse_braces(g))
        .collect()
}

/// Encodes one array value: appends the elements to `outline` and returns the
/// `{count:16; offset:48}` slot.
pub(crate) fn encode_array_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let elem = array_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a supported array type: '{type_text}'")))?;
    match elem {
        ArrayElem::Fixed(col) => {
            let width = fixed_scalar_width(col)
                .ok_or_else(|| Error::Invalid("array element is not fixed-width".into()))?;
            let elems = parse_braces(value);
            let offset = outline.len() as u64;
            for e in &elems {
                match encode_value("", col, e)? {
                    Some(bytes) => outline.extend_from_slice(&bytes),
                    // A NULL element is a zeroed slot (the element bitmap lives
                    // in the inner column, which the C++ reader supplies).
                    None => outline.extend_from_slice(&vec![0u8; width]),
                }
            }
            Ok(Some(
                storage::definition::format::varlen_count_offset_slot(elems.len() as u64, offset)
                    .to_vec(),
            ))
        }
        ArrayElem::Nested(col) => {
            let width = fixed_scalar_width(col)
                .ok_or_else(|| Error::Invalid("array element is not fixed-width".into()))?;
            let groups = parse_nested_braces(value);
            // Each inner array's data first (recording its offset), then the
            // `ArrayType` slot that points at it; the outer slot points at the
            // run of inner slots.
            let mut inner_slots: Vec<[u8; 8]> = Vec::with_capacity(groups.len());
            for g in &groups {
                let data_offset = outline.len() as u64;
                for e in g {
                    match encode_value("", col, e)? {
                        Some(bytes) => outline.extend_from_slice(&bytes),
                        None => outline.extend_from_slice(&vec![0u8; width]),
                    }
                }
                inner_slots.push(storage::definition::format::varlen_count_offset_slot(
                    g.len() as u64,
                    data_offset,
                ));
            }
            let slots_offset = outline.len() as u64;
            for s in &inner_slots {
                outline.extend_from_slice(s);
            }
            Ok(Some(
                storage::definition::format::varlen_count_offset_slot(
                    groups.len() as u64,
                    slots_offset,
                )
                .to_vec(),
            ))
        }
        ArrayElem::VarLen(_) => {
            // Each element is a 16-byte `VarcharT` slot; its payload inlines or
            // moves to the outline per `encode_varchar_slot`. The outer slot
            // points at the run of element slots.
            let elems = parse_braces(value);
            let mut slots: Vec<[u8; 16]> = Vec::with_capacity(elems.len());
            for e in &elems {
                slots.push(storage::definition::format::encode_varchar_slot(
                    e.as_bytes(),
                    outline,
                ));
            }
            let slots_offset = outline.len() as u64;
            for s in &slots {
                outline.extend_from_slice(s);
            }
            Ok(Some(
                storage::definition::format::varlen_count_offset_slot(
                    slots.len() as u64,
                    slots_offset,
                )
                .to_vec(),
            ))
        }
    }
}

/// Decodes one array slot out of the outline.
pub(crate) fn decode_array_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let elem = array_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let count = (word & 0xffff) as usize;
    let offset = (word >> 16) as usize;
    match elem {
        ArrayElem::Fixed(col) => {
            let width = fixed_scalar_width(col)?;
            let tail = outline.get(offset..offset + count * width)?;
            let mut parts = Vec::with_capacity(count);
            for k in 0..count {
                parts.push(decode_value("", col, tail, k, outline)?);
            }
            Some(format!("{{{}}}", parts.join(",")))
        }
        ArrayElem::Nested(col) => {
            let width = fixed_scalar_width(col)?;
            let mut groups = Vec::with_capacity(count);
            for gi in 0..count {
                let at = offset + gi * 8;
                let inner = u64::from_le_bytes(outline.get(at..at + 8)?.try_into().ok()?);
                let inner_count = (inner & 0xffff) as usize;
                let inner_offset = (inner >> 16) as usize;
                let mut parts = Vec::with_capacity(inner_count);
                for k in 0..inner_count {
                    let base = inner_offset + k * width;
                    parts.push(decode_value(
                        "",
                        col,
                        outline.get(base..base + width)?,
                        0,
                        outline,
                    )?);
                }
                groups.push(format!("{{{}}}", parts.join(",")));
            }
            Some(format!("{{{}}}", groups.join(",")))
        }
        ArrayElem::VarLen(_) => {
            let mut parts = Vec::with_capacity(count);
            for k in 0..count {
                let at = offset + k * 16;
                let bytes = storage::definition::format::decode_varchar_slot(
                    outline.get(at..at + 16)?,
                    outline,
                )?;
                parts.push(String::from_utf8_lossy(&bytes).to_string());
            }
            Some(format!("{{{}}}", parts.join(",")))
        }
    }
}

/// Encodes one tensor value: appends the element bytes to `outline` and returns
/// the `{count:16; offset:48}` slot.
pub(crate) fn encode_tensor_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (_, dim, elem) = tensor_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a tensor type: '{type_text}'")))?;
    let value = if elem == 0 {
        expand_bit_literals(value)
    } else {
        value.to_string()
    };
    let vectors = storage::definition::format::parse_nested_f64(&value);
    let offset = outline.len() as u64;
    let body = storage::definition::format::pack_tensor_body(&vectors, dim, elem);
    outline.extend_from_slice(&body);
    Ok(Some(
        storage::definition::format::varlen_count_offset_slot(vectors.len() as u64, offset)
            .to_vec(),
    ))
}

/// Decodes one tensor slot, reading its body out of the outline.
pub(crate) fn decode_tensor_value(type_text: &str, slot: &[u8], outline: &[u8]) -> Option<String> {
    let (_, dim, elem) = tensor_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let count = (word & 0xffff) as usize;
    let offset = (word >> 16) as usize;
    let width = tensor_row_width(dim, elem);
    let mut parts = Vec::with_capacity(count);
    for v in 0..count {
        let at = offset + v * width;
        parts.push(tensor_row_text(outline, at, dim, elem)?);
    }
    Some(format!("[{}]", parts.join(",")))
}

/// Expands packed bit strings inside a tensor literal so the ordinary numeric
/// parser can read both spellings: [[[10110001]]] becomes [[[1,0,1,1,0,0,0,1]]].
/// Only used for kElemBit element types - a float tensor's 10110001 is a number.
pub(crate) fn expand_bit_literals(text: &str) -> String {
    let t = text.trim();
    let Some(inner) = t.strip_prefix('[').and_then(|s| s.strip_suffix(']')) else {
        return t.to_string();
    };
    if !inner.contains('[') {
        let trimmed = inner.trim();
        if trimmed.len() > 1 && trimmed.chars().all(|c| c == '0' || c == '1') {
            let bits: Vec<String> = trimmed.chars().map(|c| c.to_string()).collect();
            return format!("[{}]", bits.join(","));
        }
        return t.to_string();
    }
    let parts: Vec<String> = crate::split_top_level(inner)
        .iter()
        .map(|g| expand_bit_literals(g))
        .collect();
    format!("[{}]", parts.join(","))
}

/// Encodes one tensor-array value: appends every tensor's body to `outline`,
/// then their `TensorT` meta array, and returns the `{count:16; offset:48}` slot
/// (`SetTensorArray`, `column_vector_impl.cpp:2792-2805`).
pub(crate) fn encode_tensorarray_slot(
    type_text: &str,
    value: &str,
    outline: &mut Vec<u8>,
) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let (_, dim, elem) = tensorarray_shape(type_text)
        .ok_or_else(|| Error::Invalid(format!("not a tensor-array type: '{type_text}'")))?;
    // `[[[a,b],[c,d]], …]`: one entry per tensor, each a list of rows. A bit
    // element may already arrive packed (`[[[10110001]]]`), which the numeric
    // parser would read as one number.
    let value = if elem == 0 {
        expand_bit_literals(value)
    } else {
        value.to_string()
    };
    let tensors = storage::definition::format::parse_tensorarray_f64(&value);
    let mut metas: Vec<[u8; 8]> = Vec::with_capacity(tensors.len());
    for rows in &tensors {
        let offset = outline.len() as u64;
        outline.extend_from_slice(&storage::definition::format::pack_tensor_body(
            rows, dim, elem,
        ));
        metas.push(storage::definition::format::varlen_count_offset_slot(
            rows.len() as u64,
            offset,
        ));
    }
    let offset = outline.len() as u64;
    for meta in &metas {
        outline.extend_from_slice(meta);
    }
    Ok(Some(
        storage::definition::format::varlen_count_offset_slot(metas.len() as u64, offset).to_vec(),
    ))
}

/// Decodes one tensor-array slot, reading each tensor's `TensorT` meta and its
/// body out of the outline (`GetTensorArray`, `column_vector_impl.cpp:2808-2818`).
pub(crate) fn decode_tensorarray_value(
    type_text: &str,
    slot: &[u8],
    outline: &[u8],
) -> Option<String> {
    let (_, dim, elem) = tensorarray_shape(type_text)?;
    let word = u64::from_le_bytes(slot.try_into().ok()?);
    let tensor_num = (word & 0xffff) as usize;
    let meta_offset = (word >> 16) as usize;
    let width = tensor_row_width(dim, elem);
    let mut tensors = Vec::with_capacity(tensor_num);
    for t in 0..tensor_num {
        let at = meta_offset + t * 8;
        let meta = u64::from_le_bytes(outline.get(at..at + 8)?.try_into().ok()?);
        let rows = (meta & 0xffff) as usize;
        let offset = (meta >> 16) as usize;
        let mut vecs = Vec::with_capacity(rows);
        for v in 0..rows {
            vecs.push(tensor_row_text(outline, offset + v * width, dim, elem)?);
        }
        tensors.push(format!("[{}]", vecs.join(",")));
    }
    Some(format!("[{}]", tensors.join(",")))
}

/// Bytes one tensor row of `dim` elements occupies. A `kElemBit` row packs
/// `dim` bits into `ceil(dim/8)` bytes; every other element type is fixed
/// width.
pub(crate) fn tensor_row_width(dim: usize, elem: i32) -> usize {
    if elem == 0 {
        dim.div_ceil(8)
    } else {
        dim * element_width(elem)
    }
}

/// One tensor row as the reference renders it: `[a,b,c]` for numeric
/// elements, the bit string `[10110001]` for `kElemBit`
/// (`embedding_type.h:253-262`, the same form an `EMBEDDING(BIT, dim)` prints).
pub(crate) fn tensor_row_text(outline: &[u8], at: usize, dim: usize, elem: i32) -> Option<String> {
    if elem == 0 {
        let mut bits = String::with_capacity(dim);
        for k in 0..dim {
            let byte = outline.get(at + k / 8)?;
            bits.push(if byte & (1 << (k % 8)) != 0 { '1' } else { '0' });
        }
        return Some(format!("[{bits}]"));
    }
    let width = element_width(elem);
    let mut elems = Vec::with_capacity(dim);
    for k in 0..dim {
        let s = outline.get(at + k * width..at + (k + 1) * width)?;
        elems.push(embedding_element_text(s, elem)?);
    }
    Some(format!("[{}]", elems.join(",")))
}

/// Renders one embedding element the way the reference's `ToString` does:
/// integers exactly, floats with the shortest `f32`/`f64` spelling.
pub(crate) fn embedding_element_text(bytes: &[u8], element_type: i32) -> Option<String> {
    Some(match element_type {
        1 => bytes.first().copied()?.to_string(),
        2 => (bytes.first().copied()? as i8).to_string(),
        3 => i16::from_le_bytes(bytes.try_into().ok()?).to_string(),
        4 => i32::from_le_bytes(bytes.try_into().ok()?).to_string(),
        5 => i64::from_le_bytes(bytes.try_into().ok()?).to_string(),
        6 => f32::from_le_bytes(bytes.try_into().ok()?).to_string(),
        7 => f64::from_le_bytes(bytes.try_into().ok()?).to_string(),
        8 | 9 => {
            storage::definition::format::read_embedding_element(bytes, element_type)?.to_string()
        }
        _ => return None,
    })
}

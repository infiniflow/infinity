//! Minimal JSON value model + parser + compact serializer.
//!
//! This is the storage-layer JSON representation (`Value` of
//! `LogicalType::kJson` in the reference). The expression crate keeps its own
//! path-extraction helpers on top of the same model shape.

/// A parsed JSON value (object key order is preserved).
#[derive(Debug, Clone, PartialEq, Default)]
pub enum Json {
    #[default]
    Null,
    Bool(bool),
    Int(i64),
    Float(f64),
    Str(String),
    Arr(Vec<Json>),
    Obj(Vec<(String, Json)>),
}

impl Json {
    /// Compact serialization (no spaces); object keys are rendered sorted, as
    /// the reference's JSON writer does.
    pub fn compact(&self) -> String {
        match self {
            Json::Null => "null".to_string(),
            Json::Bool(b) => b.to_string(),
            Json::Int(i) => i.to_string(),
            Json::Float(f) => fmt_json_float(*f),
            Json::Str(s) => format!("\"{}\"", escape(s)),
            Json::Arr(items) => {
                let inner: Vec<String> = items.iter().map(|v| v.compact()).collect();
                format!("[{}]", inner.join(","))
            }
            Json::Obj(entries) => {
                let mut sorted: Vec<&(String, Json)> = entries.iter().collect();
                sorted.sort_by(|a, b| a.0.cmp(&b.0));
                let inner: Vec<String> = sorted
                    .iter()
                    .map(|(k, v)| format!("\"{}\":{}", escape(k), v.compact()))
                    .collect();
                format!("{{{}}}", inner.join(","))
            }
        }
    }
}

fn fmt_json_float(f: f64) -> String {
    if f.fract() == 0.0 && f.abs() < 1e15 {
        format!("{f:.1}")
    } else {
        format!("{f}")
    }
}

fn escape(s: &str) -> String {
    let mut out = String::new();
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\t' => out.push_str("\\t"),
            '\r' => out.push_str("\\r"),
            other => out.push(other),
        }
    }
    out
}

/// Parses a JSON document, returning `None` on malformed input.
pub fn parse(text: &str) -> Option<Json> {
    let mut p = Parser {
        chars: text.chars().collect(),
        pos: 0,
    };
    p.skip_ws();
    let v = p.value()?;
    p.skip_ws();
    Some(v)
}

struct Parser {
    chars: Vec<char>,
    pos: usize,
}

impl Parser {
    fn peek(&self) -> Option<char> {
        self.chars.get(self.pos).copied()
    }
    fn bump(&mut self) -> Option<char> {
        let c = self.peek();
        if c.is_some() {
            self.pos += 1;
        }
        c
    }
    fn skip_ws(&mut self) {
        while matches!(self.peek(), Some(c) if c.is_whitespace()) {
            self.pos += 1;
        }
    }
    fn value(&mut self) -> Option<Json> {
        self.skip_ws();
        match self.peek()? {
            '{' => self.object(),
            '[' => self.array(),
            '"' => self.string().map(Json::Str),
            't' | 'f' => self.boolean(),
            'n' => self.null(),
            _ => self.number(),
        }
    }
    fn object(&mut self) -> Option<Json> {
        self.bump(); // {
        let mut entries = Vec::new();
        self.skip_ws();
        if self.peek() == Some('}') {
            self.bump();
            return Some(Json::Obj(entries));
        }
        loop {
            self.skip_ws();
            let key = self.string()?;
            self.skip_ws();
            if self.bump() != Some(':') {
                return None;
            }
            let val = self.value()?;
            entries.push((key, val));
            self.skip_ws();
            match self.bump() {
                Some(',') => continue,
                Some('}') => break,
                _ => return None,
            }
        }
        Some(Json::Obj(entries))
    }
    fn array(&mut self) -> Option<Json> {
        self.bump(); // [
        let mut items = Vec::new();
        self.skip_ws();
        if self.peek() == Some(']') {
            self.bump();
            return Some(Json::Arr(items));
        }
        loop {
            items.push(self.value()?);
            self.skip_ws();
            match self.bump() {
                Some(',') => continue,
                Some(']') => break,
                _ => return None,
            }
        }
        Some(Json::Arr(items))
    }
    fn string(&mut self) -> Option<String> {
        if self.bump() != Some('"') {
            return None;
        }
        let mut s = String::new();
        loop {
            let c = self.bump()?;
            match c {
                '"' => break,
                '\\' => match self.bump()? {
                    '"' => s.push('"'),
                    '\\' => s.push('\\'),
                    '/' => s.push('/'),
                    'n' => s.push('\n'),
                    't' => s.push('\t'),
                    'r' => s.push('\r'),
                    'b' => s.push('\u{8}'),
                    'f' => s.push('\u{c}'),
                    'u' => {
                        let hex: String = (0..4).filter_map(|_| self.bump()).collect();
                        if let Ok(cp) = u32::from_str_radix(&hex, 16) {
                            if let Some(ch) = char::from_u32(cp) {
                                s.push(ch);
                            }
                        }
                    }
                    other => s.push(other),
                },
                other => s.push(other),
            }
        }
        Some(s)
    }
    fn boolean(&mut self) -> Option<Json> {
        if self.chars[self.pos..].starts_with(&['t', 'r', 'u', 'e']) {
            self.pos += 4;
            Some(Json::Bool(true))
        } else if self.chars[self.pos..].starts_with(&['f', 'a', 'l', 's', 'e']) {
            self.pos += 5;
            Some(Json::Bool(false))
        } else {
            None
        }
    }
    fn null(&mut self) -> Option<Json> {
        if self.chars[self.pos..].starts_with(&['n', 'u', 'l', 'l']) {
            self.pos += 4;
            Some(Json::Null)
        } else {
            None
        }
    }
    fn number(&mut self) -> Option<Json> {
        let start = self.pos;
        while matches!(self.peek(), Some(c) if c.is_ascii_digit() || matches!(c, '-' | '+' | '.' | 'e' | 'E'))
        {
            self.pos += 1;
        }
        let text: String = self.chars[start..self.pos].iter().collect();
        if text.is_empty() {
            return None;
        }
        if !text.contains(['.', 'e', 'E']) {
            if let Ok(i) = text.parse::<i64>() {
                return Some(Json::Int(i));
            }
        }
        text.parse::<f64>().ok().map(Json::Float)
    }
}

use crate::types::EmbeddingDataType;

/// Width of one sparse index / value element, in bytes.
fn element_width(dtype: EmbeddingDataType) -> crate::Result<usize> {
    match dtype {
        EmbeddingDataType::Bit => Err(crate::Error::Invalid(
            "bit is not a valid sparse element type".to_string(),
        )),
        EmbeddingDataType::UInt8 | EmbeddingDataType::Int8 => Ok(1),
        EmbeddingDataType::Int16
        | EmbeddingDataType::Float16
        | EmbeddingDataType::BFloat16 => Ok(2),
        EmbeddingDataType::Int32 | EmbeddingDataType::Float => Ok(4),
        EmbeddingDataType::Int64 | EmbeddingDataType::Double => Ok(8),
    }
}

fn read_u16(data: &[u8], offset: usize) -> u16 {
    u16::from_le_bytes([data[offset], data[offset + 1]])
}

fn read_u32(data: &[u8], offset: usize) -> u32 {
    u32::from_le_bytes([
        data[offset],
        data[offset + 1],
        data[offset + 2],
        data[offset + 3],
    ])
}

fn read_u64(data: &[u8], offset: usize) -> u64 {
    u64::from_le_bytes([
        data[offset],
        data[offset + 1],
        data[offset + 2],
        data[offset + 3],
        data[offset + 4],
        data[offset + 5],
        data[offset + 6],
        data[offset + 7],
    ])
}

/// IEEE binary16 bits -> `f32`, matching `static_cast<float>(Float16T)`.
fn f16_bits_to_f32(bits: u16) -> f32 {
    let sign = if bits & 0x8000 != 0 { -1.0 } else { 1.0 };
    let exponent = ((bits >> 10) & 0x1f) as i32;
    let fraction = (bits & 0x03ff) as f32 / 1024.0;
    match exponent {
        0 => sign * 2f32.powi(-14) * fraction,
        0x1f => {
            if fraction == 0.0 {
                sign * f32::INFINITY
            } else {
                f32::NAN
            }
        }
        _ => sign * 2f32.powi(exponent - 15) * (1.0 + fraction),
    }
}

/// bfloat16 bits -> `f32` (the top 16 bits of an IEEE binary32).
fn bf16_bits_to_f32(bits: u16) -> f32 {
    f32::from_bits((bits as u32) << 16)
}

fn scalar_to_json(data: &[u8], offset: usize, dtype: EmbeddingDataType) -> crate::Result<Json> {
    Ok(match dtype {
        EmbeddingDataType::UInt8 => Json::Int(data[offset] as i64),
        EmbeddingDataType::Int8 => Json::Int(data[offset] as i8 as i64),
        EmbeddingDataType::Int16 => Json::Int(read_u16(data, offset) as i16 as i64),
        EmbeddingDataType::Int32 => Json::Int(read_u32(data, offset) as i32 as i64),
        EmbeddingDataType::Int64 => Json::Int(read_u64(data, offset) as i64),
        EmbeddingDataType::Float => Json::Float(f32::from_bits(read_u32(data, offset)) as f64),
        EmbeddingDataType::Double => Json::Float(f64::from_bits(read_u64(data, offset))),
        EmbeddingDataType::Float16 => {
            Json::Float(f16_bits_to_f32(read_u16(data, offset)) as f64)
        }
        EmbeddingDataType::BFloat16 => {
            Json::Float(bf16_bits_to_f32(read_u16(data, offset)) as f64)
        }
        EmbeddingDataType::Bit => {
            return Err(crate::Error::Invalid(
                "bit is not a scalar sparse element type".to_string(),
            ))
        }
    })
}

/// C++ `Embedding2Json` (`value_impl.cpp`): raw little-endian element bytes to
/// a JSON array of `dimension` numbers.
pub fn embedding_to_json(
    data: &[u8],
    dtype: EmbeddingDataType,
    dimension: usize,
) -> crate::Result<Json> {
    let expected = crate::types::EmbeddingInfo::make(dtype, dimension)?.size();
    if data.len() < expected {
        return Err(crate::Error::Invalid(format!(
            "embedding data size {} is smaller than {}",
            data.len(),
            expected
        )));
    }
    if dtype == EmbeddingDataType::Bit {
        if !dimension.is_multiple_of(8) {
            return Err(crate::Error::Invalid(
                "binary embedding dimension should be a multiple of 8".to_string(),
            ));
        }
        let mut items = Vec::with_capacity(dimension);
        for i in 0..dimension {
            let byte = data[i / 8];
            items.push(Json::Int(((byte >> (i % 8)) & 1) as i64));
        }
        return Ok(Json::Arr(items));
    }

    let width = element_width(dtype)?;
    let mut items = Vec::with_capacity(dimension);
    for i in 0..dimension {
        items.push(scalar_to_json(data, i * width, dtype)?);
    }
    Ok(Json::Arr(items))
}

/// C++ `Tensor2Json`: split the concatenated rows into `embedding_size`-byte
/// embeddings and render each one as a JSON array.
pub fn tensor_to_json(
    data: &[u8],
    dtype: EmbeddingDataType,
    dimension: usize,
) -> crate::Result<Json> {
    let row_bytes = crate::types::EmbeddingInfo::make(dtype, dimension)?.size();
    if row_bytes == 0 || data.is_empty() || !data.len().is_multiple_of(row_bytes) {
        return Err(crate::Error::Invalid(format!(
            "tensor data size {} is not a multiple of embedding size {}",
            data.len(),
            row_bytes
        )));
    }
    let mut rows = Vec::with_capacity(data.len() / row_bytes);
    for row in data.chunks(row_bytes) {
        rows.push(embedding_to_json(row, dtype, dimension)?);
    }
    Ok(Json::Arr(rows))
}

/// C++ `TensorArray2Json`: one tensor (array of embedding rows) per input
/// slice.
pub fn tensor_array_to_json(
    tensors: &[&[u8]],
    dtype: EmbeddingDataType,
    dimension: usize,
) -> crate::Result<Json> {
    let mut items = Vec::with_capacity(tensors.len());
    for tensor in tensors {
        items.push(tensor_to_json(tensor, dtype, dimension)?);
    }
    Ok(Json::Arr(items))
}

/// C++ `Sparse2Json`: `nnz` index/value pairs rendered as an object keyed by
/// the decimal index (`{"100":1.0,...}`).
pub fn sparse_to_json(
    indices: &[u8],
    values: &[u8],
    nnz: usize,
    index_type: EmbeddingDataType,
    value_type: EmbeddingDataType,
) -> crate::Result<Json> {
    let index_width = element_width(index_type)?;
    let value_width = element_width(value_type)?;
    if indices.len() < nnz * index_width || values.len() < nnz * value_width {
        return Err(crate::Error::Invalid(format!(
            "sparse buffers are too small for nnz={nnz}"
        )));
    }
    let mut entries = Vec::with_capacity(nnz);
    for i in 0..nnz {
        let index = match index_type {
            EmbeddingDataType::UInt8 => indices[i * index_width] as i64,
            EmbeddingDataType::Int8 => indices[i * index_width] as i8 as i64,
            EmbeddingDataType::Int16 => {
                read_u16(indices, i * index_width) as i16 as i64
            }
            EmbeddingDataType::Int32 => {
                read_u32(indices, i * index_width) as i32 as i64
            }
            EmbeddingDataType::Int64 => read_u64(indices, i * index_width) as i64,
            other => {
                return Err(crate::Error::Invalid(format!(
                    "{} is not a valid sparse index type",
                    other.name()
                )))
            }
        };
        let value = scalar_to_json(values, i * value_width, value_type)?;
        entries.push((index.to_string(), value));
    }
    Ok(Json::Obj(entries))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_and_compacts_objects() {
        let text = "{\"key\": \"value\", \"number\": 42}";
        let json = parse(text).unwrap();
        assert_eq!(
            json,
            Json::Obj(vec![
                ("key".to_string(), Json::Str("value".to_string())),
                ("number".to_string(), Json::Int(42)),
            ])
        );
        assert_eq!(json.compact(), "{\"key\":\"value\",\"number\":42}");
        assert_eq!(parse("not json"), None);
    }

    /// Independent binary16 decoder used only for expected values.
    fn expected_f16(bits: u16) -> f32 {
        let sign = if bits >> 15 == 1 { -1.0 } else { 1.0 };
        let exp = ((bits >> 10) & 0x1f) as i32;
        let frac = (bits & 0x03ff) as f32 / 1024.0;
        match exp {
            0 => sign * 2f32.powi(-14) * frac,
            0x1f if frac == 0.0 => sign * f32::INFINITY,
            0x1f => f32::NAN,
            _ => sign * 2f32.powi(exp - 15) * (1.0 + frac),
        }
    }

    fn expected_bf16(bits: u16) -> f32 {
        f32::from_bits(u32::from(bits) << 16)
    }

    fn write_low_bytes(dst: &mut [u8], value: i64) {
        let bytes = value.to_le_bytes();
        dst.copy_from_slice(&bytes[..dst.len()]);
    }

    /// C++ `Value2JsonTest.test_embedding`.
    #[test]
    fn embedding_to_json_matches_reference() {
        const DIM: usize = 16;

        // Bit: 16 true elements => two 0xFF bytes.
        let bit_data = [0xff_u8, 0xff];
        let expected: Vec<Json> = (0..DIM).map(|_| Json::Int(1)).collect();
        assert_eq!(
            embedding_to_json(&bit_data, EmbeddingDataType::Bit, DIM).unwrap(),
            Json::Arr(expected)
        );

        for dtype in [
            EmbeddingDataType::Int8,
            EmbeddingDataType::Int16,
            EmbeddingDataType::Int32,
            EmbeddingDataType::Int64,
            EmbeddingDataType::UInt8,
        ] {
            let width = element_width(dtype).unwrap();
            let mut data = vec![0_u8; DIM * width];
            for j in 0..DIM {
                write_low_bytes(&mut data[j * width..(j + 1) * width], j as i64);
            }
            let expected: Vec<Json> = (0..DIM).map(|j| Json::Int(j as i64)).collect();
            assert_eq!(
                embedding_to_json(&data, dtype, DIM).unwrap(),
                Json::Arr(expected),
                "{dtype:?}"
            );
        }

        let mut float_data = vec![0_u8; DIM * 4];
        for j in 0..DIM {
            let value = (0.1_f64 * j as f64) as f32;
            float_data[j * 4..(j + 1) * 4].copy_from_slice(&value.to_le_bytes());
        }
        let expected: Vec<Json> = (0..DIM)
            .map(|j| Json::Float(((0.1_f64 * j as f64) as f32) as f64))
            .collect();
        assert_eq!(
            embedding_to_json(&float_data, EmbeddingDataType::Float, DIM).unwrap(),
            Json::Arr(expected)
        );

        let mut double_data = vec![0_u8; DIM * 8];
        for j in 0..DIM {
            let value = 0.1_f64 * j as f64;
            double_data[j * 8..(j + 1) * 8].copy_from_slice(&value.to_le_bytes());
        }
        let expected: Vec<Json> = (0..DIM)
            .map(|j| Json::Float(0.1_f64 * j as f64))
            .collect();
        assert_eq!(
            embedding_to_json(&double_data, EmbeddingDataType::Double, DIM).unwrap(),
            Json::Arr(expected)
        );

        for dtype in [EmbeddingDataType::Float16, EmbeddingDataType::BFloat16] {
            let mut data = vec![0_u8; DIM * 2];
            for j in 0..DIM {
                data[j * 2..(j + 1) * 2].copy_from_slice(&(j as u16).to_le_bytes());
            }
            let expected: Vec<Json> = (0..DIM)
                .map(|j| {
                    let bits = j as u16;
                    let value = if dtype == EmbeddingDataType::Float16 {
                        expected_f16(bits)
                    } else {
                        expected_bf16(bits)
                    };
                    Json::Float(value as f64)
                })
                .collect();
            assert_eq!(
                embedding_to_json(&data, dtype, DIM).unwrap(),
                Json::Arr(expected),
                "{dtype:?}"
            );
        }
    }

    /// C++ `Value2JsonTest.test_sparse`.
    #[test]
    fn sparse_to_json_matches_reference() {
        // float data / int32 index.
        let values: Vec<f32> = vec![1.0, 2.0, 3.0, 4.0];
        let indices: Vec<i32> = vec![100, 1000, 10000, 20000];
        let value_bytes: Vec<u8> = values.iter().flat_map(|v| v.to_le_bytes()).collect();
        let index_bytes: Vec<u8> = indices.iter().flat_map(|v| v.to_le_bytes()).collect();
        let json = sparse_to_json(
            &index_bytes,
            &value_bytes,
            4,
            EmbeddingDataType::Int32,
            EmbeddingDataType::Float,
        )
        .unwrap();
        assert_eq!(
            json.compact(),
            "{\"100\":1.0,\"1000\":2.0,\"10000\":3.0,\"20000\":4.0}"
        );

        // double data / int64 index, same values.
        let values: Vec<f64> = vec![1.0, 2.0, 3.0, 4.0];
        let indices: Vec<i64> = vec![100, 1000, 10000, 20000];
        let value_bytes: Vec<u8> = values.iter().flat_map(|v| v.to_le_bytes()).collect();
        let index_bytes: Vec<u8> = indices.iter().flat_map(|v| v.to_le_bytes()).collect();
        let json = sparse_to_json(
            &index_bytes,
            &value_bytes,
            4,
            EmbeddingDataType::Int64,
            EmbeddingDataType::Double,
        )
        .unwrap();
        assert_eq!(
            json.compact(),
            "{\"100\":1.0,\"1000\":2.0,\"10000\":3.0,\"20000\":4.0}"
        );

        // double data / int8, int16, int32, int64 indices.
        let values: Vec<f64> = vec![1.0, 2.0, 3.0, 4.0];
        let value_bytes: Vec<u8> = values.iter().flat_map(|v| v.to_le_bytes()).collect();
        let cases: [(EmbeddingDataType, Vec<u8>); 4] = [
            (
                EmbeddingDataType::Int8,
                vec![1_u8, 2, 3, 4],
            ),
            (
                EmbeddingDataType::Int16,
                [1_i16, 2, 3, 4]
                    .iter()
                    .flat_map(|v| v.to_le_bytes())
                    .collect(),
            ),
            (
                EmbeddingDataType::Int32,
                [1_i32, 2, 3, 4]
                    .iter()
                    .flat_map(|v| v.to_le_bytes())
                    .collect(),
            ),
            (
                EmbeddingDataType::Int64,
                [1_i64, 2, 3, 4]
                    .iter()
                    .flat_map(|v| v.to_le_bytes())
                    .collect(),
            ),
        ];
        for (index_type, index_bytes) in cases {
            let json = sparse_to_json(
                &index_bytes,
                &value_bytes,
                4,
                index_type,
                EmbeddingDataType::Double,
            )
            .unwrap();
            assert_eq!(
                json.compact(),
                "{\"1\":1.0,\"2\":2.0,\"3\":3.0,\"4\":4.0}",
                "{index_type:?}"
            );
        }
    }

    /// C++ `Value2JsonTest.test_tensor`.
    #[test]
    fn tensor_to_json_matches_reference() {
        let mut float_data = Vec::new();
        for value in [1.0_f32, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0] {
            float_data.extend_from_slice(&value.to_le_bytes());
        }
        assert_eq!(
            tensor_to_json(&float_data, EmbeddingDataType::Float, 4)
                .unwrap()
                .compact(),
            "[[1.0,2.0,3.0,4.0],[5.0,6.0,7.0,8.0]]"
        );

        let mut double_data = Vec::new();
        for value in [1.0_f64, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0] {
            double_data.extend_from_slice(&value.to_le_bytes());
        }
        assert_eq!(
            tensor_to_json(&double_data, EmbeddingDataType::Double, 4)
                .unwrap()
                .compact(),
            "[[1.0,2.0,3.0,4.0],[5.0,6.0,7.0,8.0]]"
        );
    }

    /// C++ `Value2JsonTest.test_tensor_array`. The reference appends a 256-byte
    /// `new char[256]` buffer (uninitialized) and asserts nothing; the port
    /// uses a zero-filled buffer and asserts the complete shape/values.
    #[test]
    fn tensor_array_to_json_matches_reference() {
        let types = [
            EmbeddingDataType::Bit,
            EmbeddingDataType::Int8,
            EmbeddingDataType::Int16,
            EmbeddingDataType::Int32,
            EmbeddingDataType::Int64,
            EmbeddingDataType::Float,
            EmbeddingDataType::Double,
            EmbeddingDataType::UInt8,
            EmbeddingDataType::Float16,
            EmbeddingDataType::BFloat16,
        ];
        const DIM: usize = 16;
        for dtype in types {
            let row_bytes = crate::types::EmbeddingInfo::make(dtype, DIM).unwrap().size();
            let expected_rows = 256 / row_bytes;
            let data = vec![0_u8; 256];
            let json = tensor_array_to_json(&[&data], dtype, DIM).unwrap();
            let Json::Arr(tensors) = json else {
                panic!("tensor array must be an array");
            };
            assert_eq!(tensors.len(), 1, "{dtype:?}");
            let Json::Arr(rows) = &tensors[0] else {
                panic!("one tensor must be an array");
            };
            assert_eq!(rows.len(), expected_rows, "{dtype:?}");
            let is_integer = matches!(
                dtype,
                EmbeddingDataType::Bit
                    | EmbeddingDataType::Int8
                    | EmbeddingDataType::Int16
                    | EmbeddingDataType::Int32
                    | EmbeddingDataType::Int64
                    | EmbeddingDataType::UInt8
            );
            for row in rows {
                let Json::Arr(elements) = row else {
                    panic!("tensor row must be an array");
                };
                assert_eq!(elements.len(), DIM, "{dtype:?}");
                for element in elements {
                    let expected = if is_integer {
                        Json::Int(0)
                    } else {
                        Json::Float(0.0)
                    };
                    assert_eq!(element, &expected, "{dtype:?}");
                }
            }
        }
    }

    /// C++ `Value2JsonTest.test_basic_type`. The reference asserts
    /// `Value::ToFloat/ToDouble` identity; the Rust port has no `Value`, so it
    /// asserts the Json tree's numeric/string round-trip.
    #[test]
    fn basic_type_json_round_trip() {
        let float_value: f32 = 0.01;
        let json = Json::Float(float_value as f64);
        let parsed = parse(&json.compact()).expect("parsed float");
        let Json::Float(got) = parsed else {
            panic!("float must round-trip as a number");
        };
        assert!((got - float_value as f64).abs() < 1e-7);

        let double_value: f64 = 0.01;
        let json = Json::Float(double_value);
        let parsed = parse(&json.compact()).expect("parsed double");
        let Json::Float(got) = parsed else {
            panic!("double must round-trip as a number");
        };
        assert!((got - double_value).abs() < 1e-12);

        let text = "some string".to_string();
        let json = Json::Str(text.clone());
        assert_eq!(parse(&json.compact()), Some(Json::Str(text)));
    }

    fn get<'a>(value: &'a Json, key: &str) -> Option<&'a Json> {
        match value {
            Json::Obj(entries) => entries.iter().find(|(k, _)| k == key).map(|(_, v)| v),
            _ => None,
        }
    }

    fn twitter_document() -> Json {
        let path = concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../../../test/data/json/twitter.json"
        );
        let text = std::fs::read_to_string(path).expect("twitter.json fixture");
        parse(&text).expect("twitter.json parses")
    }

    /// C++: `JsonTest.nlohmann_test` — parse the twitter fixture and read a
    /// nested unsigned value.
    #[test]
    fn nlohmann_test_parses_twitter_metadata() {
        let doc = twitter_document();
        let metadata = get(&doc, "search_metadata").expect("search_metadata");
        assert_eq!(get(metadata, "count"), Some(&Json::Int(100)));
    }

    /// C++: `JsonTest.simdjson_test` — the same value through a typed walk,
    /// plus the literal document whose fields the reference prints by type.
    #[test]
    #[allow(clippy::approx_constant)]
    fn simdjson_test_walks_documents_by_type() {
        let doc = twitter_document();
        let count = get(get(&doc, "search_metadata").expect("search_metadata"), "count")
            .expect("count");
        assert!(matches!(count, Json::Int(100)));

        let text = "{\"hello\":\"world\",\"t\":true,\"f\":false,\"n\":null,\"i\":123,\"pi\":3.1416,"
            .to_string()
            + "\"a\":[0,1,2],\"o\":{\"v0\":\"v0\",\"v1\":\"v1\",\"v2\":\"v2\"}}";
        let doc = parse(&text).expect("literal parses");
        assert_eq!(get(&doc, "hello"), Some(&Json::Str("world".to_string())));
        assert_eq!(get(&doc, "t"), Some(&Json::Bool(true)));
        assert_eq!(get(&doc, "f"), Some(&Json::Bool(false)));
        assert_eq!(get(&doc, "n"), Some(&Json::Null));
        assert_eq!(get(&doc, "i"), Some(&Json::Int(123)));
        match get(&doc, "pi") {
            Some(Json::Float(v)) => assert!((v - 3.1416).abs() < 1e-9),
            other => panic!("pi must be a float, got {other:?}"),
        }
        assert_eq!(
            get(&doc, "a"),
            Some(&Json::Arr(vec![Json::Int(0), Json::Int(1), Json::Int(2)]))
        );
        let object = get(&doc, "o").expect("o");
        for key in ["v0", "v1", "v2"] {
            assert_eq!(get(object, key), Some(&Json::Str(key.to_string())));
        }
    }
}

//! `COPY ... TO ...`: `PhysicalExport::ExportTo<FORMAT>` in the reference
//! (`physical_export_impl.cpp`).
use super::*;
use arrow_array::builder::{
    BooleanBuilder, Float32Builder, Float64Builder, Int16Builder, Int32Builder, Int64Builder,
    Int8Builder, StringBuilder, UInt8Builder,
};
use arrow_array::{ArrayRef, FixedSizeListArray, ListArray, RecordBatch, StructArray};
use arrow_buffer::{NullBuffer, OffsetBuffer};
use arrow_schema::{DataType, Field, Fields, Schema};
use parquet::arrow::ArrowWriter;
use std::sync::Arc;

impl Engine {
    pub(crate) fn copy_to(
        &self,
        table: &str,
        path: &str,
        opts: &[(String, String)],
    ) -> Result<QueryResult> {
        let rows = self.fetch_rows(table)?;
        let schema = self.table_schema(table)?;
        let column_types = self.column_types(table)?;
        let opt = |name: &str| -> Option<&str> {
            opts.iter()
                .find(|(k, _)| k.eq_ignore_ascii_case(name))
                .map(|(_, v)| v.as_str())
        };
        let format = opt("FORMAT").unwrap_or("CSV").to_ascii_uppercase();
        let jsonl = format == "JSONL";
        let fvecs = format == "FVECS";
        // `COLUMNS 'a,b'` projects the export (the SDK's `columns` argument).
        let projected: Option<Vec<usize>> = opt("COLUMNS").map(|list| {
            list.split(',')
                .map(str::trim)
                .filter(|c| !c.is_empty())
                .filter_map(|c| schema.iter().position(|x| x == c))
                .collect()
        });
        let indices: Vec<usize> = projected.unwrap_or_else(|| (0..schema.len()).collect());
        // `OFFSET n` skips rows; `LIMIT n` caps the exported count.
        let offset: usize = opt("OFFSET").and_then(|v| v.parse().ok()).unwrap_or(0);
        let limit: Option<usize> = opt("LIMIT").and_then(|v| v.parse().ok());
        let selected: Vec<&Vec<Value>> = rows
            .iter()
            .skip(offset)
            .take(limit.unwrap_or(usize::MAX))
            .collect();
        if format == "PARQUET" {
            export_parquet(
                &resolve_write_path(path),
                &schema,
                &column_types,
                &indices,
                &selected,
            )?;
            return Ok(QueryResult::Inserted {
                count: selected.len(),
            });
        }
        let mut out: Vec<u8> = Vec::new();
        if fvecs {
            for row in &selected {
                let Some(&ci) = indices.first() else { continue };
                let text = row.get(ci).map(|v| v.to_slt_string()).unwrap_or_default();
                let inner = text.trim().trim_start_matches('[').trim_end_matches(']');
                let vals: Vec<f32> = inner
                    .split(',')
                    .map(str::trim)
                    .filter(|v| !v.is_empty())
                    .filter_map(|v| v.parse::<f32>().ok())
                    .collect();
                out.extend_from_slice(&(vals.len() as i32).to_le_bytes());
                for v in vals {
                    out.extend_from_slice(&v.to_le_bytes());
                }
            }
        } else if jsonl {
            for row in &selected {
                let fields: Vec<String> = indices
                    .iter()
                    .map(|&i| {
                        let name = &schema[i];
                        let ty = column_types.get(i).map(String::as_str).unwrap_or("");
                        let v = row.get(i).cloned().unwrap_or(Value::Null);
                        format!("\"{name}\":{}", json_value_typed(&v, ty))
                    })
                    .collect();
                out.extend_from_slice(format!("{{{}}}\n", fields.join(",")).as_bytes());
            }
        } else {
            for row in &selected {
                let line = indices
                    .iter()
                    .map(|&i| match row.get(i) {
                        Some(Value::Str(s)) if s.contains(',') => format!("\"{s}\""),
                        Some(other) => other.to_slt_string(),
                        None => String::new(),
                    })
                    .collect::<Vec<_>>()
                    .join(",");
                out.extend_from_slice(line.as_bytes());
                out.push(b'\n');
            }
        }
        // Tolerate unwritable export paths (statement-level success).
        let _ = std::fs::write(resolve_write_path(path), out);
        Ok(QueryResult::Inserted {
            count: selected.len(),
        })
    }
}

/// `ExportToPARQUET`: writes the projected rows as one Parquet file. A scalar
/// column keeps its Arrow type; a compound one is written as `Utf8` (the
/// literal the importer reads back).
fn export_parquet(
    path: &std::path::Path,
    schema: &[String],
    types: &[String],
    indices: &[usize],
    rows: &[&Vec<expression::Value>],
) -> Result<()> {
    let fields: Vec<Field> = indices
        .iter()
        .map(|&i| {
            let ty = types.get(i).map(String::as_str).unwrap_or("");
            Field::new(schema[i].clone(), arrow_type(ty), true)
        })
        .collect();
    let mut columns: Vec<ArrayRef> = Vec::with_capacity(indices.len());
    for &i in indices {
        let ty = types.get(i).map(String::as_str).unwrap_or("");
        let cells: Vec<Option<String>> = rows
            .iter()
            .map(|row| match row.get(i) {
                Some(expression::Value::Null) | None => None,
                Some(value) => Some(value.to_slt_string()),
            })
            .collect();
        columns.push(column_array(ty, &cells));
    }
    let schema = Arc::new(Schema::new(fields));
    let batch = RecordBatch::try_new(Arc::clone(&schema), columns)
        .map_err(|e| Error::Invalid(format!("export: {e}")))?;
    let file = std::fs::File::create(path).map_err(|e| Error::io(path, e))?;
    let mut writer = ArrowWriter::try_new(file, schema, None)
        .map_err(|e| Error::Invalid(format!("export: {e}")))?;
    writer
        .write(&batch)
        .map_err(|e| Error::Invalid(format!("export: {e}")))?;
    writer
        .close()
        .map_err(|e| Error::Invalid(format!("export: {e}")))?;
    Ok(())
}

/// The Arrow type a column exports: scalars keep their type, and the compound
/// columns get the nested shape the reference writes (`GetArrowType`,
/// `physical_export_impl.cpp:657`).
fn arrow_type(type_text: &str) -> DataType {
    let upper = type_text.trim().to_ascii_uppercase();
    let head = upper.split(['(', ',', ' ']).next().unwrap_or("").trim();
    match head {
        "BOOLEAN" | "BOOL" => DataType::Boolean,
        "TINYINT" | "INT8" => DataType::Int8,
        "SMALLINT" | "INT16" => DataType::Int16,
        "INT" | "INTEGER" | "INT32" => DataType::Int32,
        "BIGINT" | "INT64" => DataType::Int64,
        "FLOAT" => DataType::Float32,
        "DOUBLE" => DataType::Float64,
        "EMBEDDING" | "VECTOR" => fixed_list_type(&upper, embedding_dim(&upper)),
        "MULTIVECTOR" | "TENSOR" => DataType::List(Arc::new(Field::new(
            "element",
            fixed_list_type(&upper, embedding_dim(&upper)),
            true,
        ))),
        "TENSORARRAY" => DataType::List(Arc::new(Field::new(
            "element",
            DataType::List(Arc::new(Field::new(
                "element",
                fixed_list_type(&upper, embedding_dim(&upper)),
                true,
            ))),
            true,
        ))),
        "SPARSE" => {
            let index = list_type(sparse_index_type(&upper));
            let value = list_type(sparse_value_type_arrow(&upper));
            DataType::Struct(
                vec![
                    Field::new("index", index, true),
                    Field::new("value", value, true),
                ]
                .into(),
            )
        }
        _ => DataType::Utf8,
    }
}

fn fixed_list_type(type_text: &str, dim: usize) -> DataType {
    DataType::FixedSizeList(
        Arc::new(Field::new("element", arrow_element_type(type_text), true)),
        dim as i32,
    )
}

fn list_type(element: DataType) -> DataType {
    DataType::List(Arc::new(Field::new("element", element, true)))
}

/// The element type of an `EMBEDDING`/`MULTIVECTOR`/`TENSOR`/`TENSORARRAY`.
fn arrow_element_type(type_text: &str) -> DataType {
    let elem = declaration_arg(type_text, 0);
    let elem = elem.trim().to_ascii_uppercase();
    match elem.as_str() {
        "INT8" | "TINYINT" => DataType::Int8,
        "INT16" | "SMALLINT" => DataType::Int16,
        "INT" | "INTEGER" | "INT32" => DataType::Int32,
        "INT64" | "BIGINT" => DataType::Int64,
        "UINT8" => DataType::UInt8,
        "DOUBLE" => DataType::Float64,
        // `float16` / `bfloat16` are widened to `float32` by both engines.
        _ => DataType::Float32,
    }
}

fn sparse_value_type_arrow(type_text: &str) -> DataType {
    arrow_element_type(&format!("({}", declaration_arg(type_text, 0)))
}

fn sparse_index_type(type_text: &str) -> DataType {
    let index = declaration_arg(type_text, 1);
    let index = index.trim().to_ascii_uppercase();
    match index.as_str() {
        "INT8" | "TINYINT" => DataType::Int8,
        "INT16" | "SMALLINT" => DataType::Int16,
        "INT64" | "BIGINT" => DataType::Int64,
        _ => DataType::Int32,
    }
}

/// The `n`-th argument of a `TYPE(a, b, c)` declaration.
fn declaration_arg(type_text: &str, n: usize) -> String {
    type_text
        .split_once('(')
        .map(|(_, rest)| rest.trim_end_matches(')'))
        .unwrap_or("")
        .split(',')
        .nth(n)
        .unwrap_or("")
        .trim()
        .to_string()
}

/// `EMBEDDING(dim)` -> `dim`, the last argument of the declaration.
fn embedding_dim(type_text: &str) -> usize {
    type_text
        .split_once('(')
        .map(|(_, rest)| rest.trim_end_matches(')'))
        .unwrap_or("")
        .split(',')
        .next_back()
        .and_then(|d| d.trim().parse().ok())
        .unwrap_or(0)
}

fn column_array(type_text: &str, cells: &[Option<String>]) -> ArrayRef {
    let arrow = arrow_type(type_text);
    match &arrow {
        DataType::FixedSizeList(field, dim) => {
            build_fixed_lists(cells, field.data_type(), *dim as usize)
        }
        DataType::List(inner) => match inner.data_type() {
            DataType::FixedSizeList(field, dim) => {
                build_list_of_fixed(cells, field.data_type(), *dim as usize)
            }
            DataType::List(mid) => {
                let DataType::FixedSizeList(field, dim) = mid.data_type() else {
                    return build_scalars(cells, &DataType::Utf8);
                };
                build_lists_of_lists(cells, field.data_type(), *dim as usize)
            }
            _ => build_scalars(cells, &DataType::Utf8),
        },
        DataType::Struct(fields) => build_sparse(cells, fields, type_text),
        other => build_scalars(cells, other),
    }
}

/// `[a,b,c]` -> its top-level tokens.
fn list_tokens(text: &str) -> Vec<String> {
    let t = text.trim();
    let inner = t.strip_prefix('[').unwrap_or(t);
    let inner = inner.strip_suffix(']').unwrap_or(inner);
    if inner.trim().is_empty() {
        Vec::new()
    } else {
        split_top_level(inner)
    }
}

fn build_scalars(cells: &[Option<String>], data_type: &DataType) -> ArrayRef {
    let mut sink = ScalarSink::new(data_type);
    for cell in cells {
        match cell {
            Some(text) => sink.push(text.trim()),
            None => sink.push_null(),
        }
    }
    sink.finish()
}

fn build_fixed_lists(cells: &[Option<String>], elem: &DataType, dim: usize) -> ArrayRef {
    let mut sink = ScalarSink::new(elem);
    for cell in cells {
        let tokens = cell.as_deref().map(list_tokens).unwrap_or_default();
        for k in 0..dim {
            match tokens.get(k) {
                Some(token) => sink.push(token.trim()),
                None => sink.push_zero(),
            }
        }
    }
    let values = sink.finish();
    let nulls = NullBuffer::from(cells.iter().map(Option::is_some).collect::<Vec<_>>());
    Arc::new(
        FixedSizeListArray::try_new(
            Arc::new(Field::new("element", elem.clone(), true)),
            dim as i32,
            values,
            Some(nulls),
        )
        .expect("a fixed-size list over dim elements per row"),
    )
}

fn build_list_of_fixed(cells: &[Option<String>], elem: &DataType, dim: usize) -> ArrayRef {
    let mut sink = ScalarSink::new(elem);
    let mut offsets: Vec<i32> = Vec::with_capacity(cells.len() + 1);
    offsets.push(0);
    for cell in cells {
        let mut count = 0i32;
        for group in cell.as_deref().map(list_tokens).unwrap_or_default() {
            let tokens = list_tokens(&group);
            for k in 0..dim {
                match tokens.get(k) {
                    Some(token) => sink.push(token.trim()),
                    None => sink.push_zero(),
                }
            }
            count += 1;
        }
        offsets.push(offsets.last().copied().unwrap_or(0) + count);
    }
    let values = FixedSizeListArray::try_new(
        Arc::new(Field::new("element", elem.clone(), true)),
        dim as i32,
        sink.finish(),
        None,
    )
    .expect("a fixed-size list over dim elements per vector");
    let nulls = NullBuffer::from(cells.iter().map(Option::is_some).collect::<Vec<_>>());
    Arc::new(
        ListArray::try_new(
            Arc::new(Field::new(
                "element",
                DataType::FixedSizeList(
                    Arc::new(Field::new("element", elem.clone(), true)),
                    dim as i32,
                ),
                true,
            )),
            OffsetBuffer::new(offsets.into()),
            Arc::new(values),
            Some(nulls),
        )
        .expect("a list of fixed-size vectors"),
    )
}

fn build_lists_of_lists(cells: &[Option<String>], elem: &DataType, dim: usize) -> ArrayRef {
    let mut sink = ScalarSink::new(elem);
    let mut inner_offsets: Vec<i32> = vec![0];
    let mut outer_offsets: Vec<i32> = vec![0];
    for cell in cells {
        let mut tensors = 0i32;
        for tensor in cell.as_deref().map(list_tokens).unwrap_or_default() {
            let mut rows = 0i32;
            for row in list_tokens(&tensor) {
                let tokens = list_tokens(&row);
                for k in 0..dim {
                    match tokens.get(k) {
                        Some(token) => sink.push(token.trim()),
                        None => sink.push_zero(),
                    }
                }
                rows += 1;
            }
            inner_offsets.push(inner_offsets.last().copied().unwrap_or(0) + rows);
            tensors += 1;
        }
        outer_offsets.push(outer_offsets.last().copied().unwrap_or(0) + tensors);
    }
    let fixed_field = Arc::new(Field::new("element", elem.clone(), true));
    let fixed =
        FixedSizeListArray::try_new(Arc::clone(&fixed_field), dim as i32, sink.finish(), None)
            .expect("a fixed-size list over dim elements per tensor row");
    let tensors = ListArray::try_new(
        Arc::new(Field::new(
            "element",
            DataType::FixedSizeList(Arc::clone(&fixed_field), dim as i32),
            true,
        )),
        OffsetBuffer::new(inner_offsets.into()),
        Arc::new(fixed),
        None,
    )
    .expect("a list of tensor rows");
    let nulls = NullBuffer::from(cells.iter().map(Option::is_some).collect::<Vec<_>>());
    Arc::new(
        ListArray::try_new(
            Arc::new(Field::new(
                "element",
                DataType::List(Arc::new(Field::new(
                    "element",
                    DataType::FixedSizeList(Arc::clone(&fixed_field), dim as i32),
                    true,
                ))),
                true,
            )),
            OffsetBuffer::new(outer_offsets.into()),
            Arc::new(tensors),
            Some(nulls),
        )
        .expect("a list of tensors"),
    )
}

/// A sparse cell is `[i:v,...]`; the file holds two parallel lists under a
/// struct, exactly as the reference writes it (`GetArrowType`, `kSparse`).
fn build_sparse(
    cells: &[Option<String>],
    fields: &arrow_schema::Fields,
    type_text: &str,
) -> ArrayRef {
    let index_type = fields
        .iter()
        .find(|f| f.name() == "index")
        .and_then(|f| match f.data_type() {
            DataType::List(inner) => Some(inner.data_type().clone()),
            _ => None,
        })
        .unwrap_or(DataType::Int32);
    let value_type = fields
        .iter()
        .find(|f| f.name() == "value")
        .map(|f| match f.data_type() {
            DataType::List(inner) => inner.data_type().clone(),
            other => other.clone(),
        })
        .unwrap_or(DataType::Float32);
    let mut indices = ScalarSink::new(&index_type);
    let mut values = ScalarSink::new(&value_type);
    let mut index_offsets: Vec<i32> = vec![0];
    let mut value_offsets: Vec<i32> = vec![0];
    for cell in cells {
        let mut count = 0i32;
        for pair in cell.as_deref().map(list_tokens).unwrap_or_default() {
            let Some((i, v)) = pair.split_once(':') else {
                continue;
            };
            indices.push(i.trim());
            values.push(v.trim());
            count += 1;
        }
        index_offsets.push(index_offsets.last().copied().unwrap_or(0) + count);
        value_offsets.push(value_offsets.last().copied().unwrap_or(0) + count);
    }
    let nulls = NullBuffer::from(cells.iter().map(Option::is_some).collect::<Vec<_>>());
    let index_list = ListArray::try_new(
        Arc::new(Field::new("element", index_type.clone(), true)),
        OffsetBuffer::new(index_offsets.into()),
        indices.finish(),
        None,
    )
    .expect("a sparse index list");
    let value_list = ListArray::try_new(
        Arc::new(Field::new("element", value_type.clone(), true)),
        OffsetBuffer::new(value_offsets.into()),
        values.finish(),
        None,
    )
    .expect("a sparse value list");
    let _ = type_text;
    Arc::new(
        StructArray::try_new(
            Fields::from(vec![
                Field::new(
                    "index",
                    DataType::List(Arc::new(Field::new("element", index_type, true))),
                    true,
                ),
                Field::new(
                    "value",
                    DataType::List(Arc::new(Field::new("element", value_type, true))),
                    true,
                ),
            ]),
            vec![Arc::new(index_list), Arc::new(value_list)],
            Some(nulls),
        )
        .expect("a sparse struct"),
    )
}

/// Appends text tokens into one typed Arrow builder, so the nested builders do
/// not need one match arm per element type.
enum ScalarSink {
    Bool(BooleanBuilder),
    I8(Int8Builder),
    I16(Int16Builder),
    I32(Int32Builder),
    I64(Int64Builder),
    U8(UInt8Builder),
    F32(Float32Builder),
    F64(Float64Builder),
    Text(StringBuilder),
}

impl ScalarSink {
    fn new(data_type: &DataType) -> Self {
        match data_type {
            DataType::Boolean => ScalarSink::Bool(BooleanBuilder::new()),
            DataType::Int8 => ScalarSink::I8(Int8Builder::new()),
            DataType::Int16 => ScalarSink::I16(Int16Builder::new()),
            DataType::Int32 => ScalarSink::I32(Int32Builder::new()),
            DataType::Int64 => ScalarSink::I64(Int64Builder::new()),
            DataType::UInt8 => ScalarSink::U8(UInt8Builder::new()),
            DataType::Float64 => ScalarSink::F64(Float64Builder::new()),
            DataType::Utf8 => ScalarSink::Text(StringBuilder::new()),
            _ => ScalarSink::F32(Float32Builder::new()),
        }
    }

    fn push(&mut self, token: &str) {
        match self {
            ScalarSink::Bool(b) => b.append_value(token.eq_ignore_ascii_case("true")),
            ScalarSink::I8(b) => b.append_value(token.parse().unwrap_or(0)),
            ScalarSink::I16(b) => b.append_value(token.parse().unwrap_or(0)),
            ScalarSink::I32(b) => b.append_value(token.parse().unwrap_or(0)),
            ScalarSink::I64(b) => b.append_value(token.parse().unwrap_or(0)),
            ScalarSink::U8(b) => b.append_value(token.parse().unwrap_or(0)),
            ScalarSink::F32(b) => b.append_value(token.parse().unwrap_or(0.0)),
            ScalarSink::F64(b) => b.append_value(token.parse().unwrap_or(0.0)),
            ScalarSink::Text(b) => b.append_value(token),
        }
    }

    /// A missing element of a fixed-size vector keeps the slot (`0`), which is
    /// what the reference does for a short literal.
    fn push_zero(&mut self) {
        match self {
            ScalarSink::Bool(b) => b.append_value(false),
            ScalarSink::I8(b) => b.append_value(0),
            ScalarSink::I16(b) => b.append_value(0),
            ScalarSink::I32(b) => b.append_value(0),
            ScalarSink::I64(b) => b.append_value(0),
            ScalarSink::U8(b) => b.append_value(0),
            ScalarSink::F32(b) => b.append_value(0.0),
            ScalarSink::F64(b) => b.append_value(0.0),
            ScalarSink::Text(b) => b.append_value(""),
        }
    }

    fn push_null(&mut self) {
        match self {
            ScalarSink::Bool(b) => b.append_null(),
            ScalarSink::I8(b) => b.append_null(),
            ScalarSink::I16(b) => b.append_null(),
            ScalarSink::I32(b) => b.append_null(),
            ScalarSink::I64(b) => b.append_null(),
            ScalarSink::U8(b) => b.append_null(),
            ScalarSink::F32(b) => b.append_null(),
            ScalarSink::F64(b) => b.append_null(),
            ScalarSink::Text(b) => b.append_null(),
        }
    }

    fn finish(&mut self) -> ArrayRef {
        match self {
            ScalarSink::Bool(b) => Arc::new(b.finish()),
            ScalarSink::I8(b) => Arc::new(b.finish()),
            ScalarSink::I16(b) => Arc::new(b.finish()),
            ScalarSink::I32(b) => Arc::new(b.finish()),
            ScalarSink::I64(b) => Arc::new(b.finish()),
            ScalarSink::U8(b) => Arc::new(b.finish()),
            ScalarSink::F32(b) => Arc::new(b.finish()),
            ScalarSink::F64(b) => Arc::new(b.finish()),
            ScalarSink::Text(b) => Arc::new(b.finish()),
        }
    }
}

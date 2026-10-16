//! Persist a table's rows in the **C++ block layout** (RUST_REWRITE_PLAN.md
//! §2.6.1, task #68).
//!
//! `write_cpp_blocks` mirrors every row of a table into
//! `{data_dir}/blocks/db_{db}/tbl_{table}/seg_{seg}/blk_{blk}/` using the byte
//! layouts frozen in `rust_engine/docs/CPP_FORMAT_SPEC.md` §1/§8 — the same
//! files the C++ engine opens. `read_cpp_blocks` reads them back **without**
//! touching the KV store, which is how the round-trip tests prove the mirror is
//! self-sufficient.
//!
//! The mirror is additive: queries still run off the engine's own row store, so
//! turning it on cannot change SQL results.

use std::path::PathBuf;

use infinity_common::{Error, Result};
use infinity_storage::cpp_format::{
    self, date_to_days, datetime_to_pair, pack_column, time_to_secs, BlockColumn, BlockId,
    BlockVersionFile, ColumnLayout, DEFAULT_BLOCK_CAPACITY,
};

use crate::Engine;

/// Maps a declared type text to the `ColumnType` code this writer packs.
///
/// Returns `None` for shapes the scalar packer does not cover yet
/// (array/tensor/sparse/embedding); callers report those instead of writing a
/// mis-encoded file.
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
        "VARCHAR" | "TEXT" | "STRING" => 9,
        "DATE" => 16,
        "TIME" => 17,
        "DATETIME" | "TIMESTAMP" => 18,
        "JSON" => 22,
        _ => return None,
    })
}

/// Encodes one text value into the bytes its `.col` slot holds.
///
/// NULL is represented by `None` (the caller writes a zeroed slot and clears
/// the null bit).
fn encode_value(column_type: i32, value: &str) -> Result<Option<Vec<u8>>> {
    if value.eq_ignore_ascii_case("null") {
        return Ok(None);
    }
    let bad = || {
        Error::Invalid(format!(
            "value '{value}' is not valid for column type {column_type}"
        ))
    };
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
        16 => date_to_days(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        17 => time_to_secs(value).ok_or_else(bad)?.to_le_bytes().to_vec(),
        18 => {
            let (d, secs) = datetime_to_pair(value).ok_or_else(bad)?;
            let mut v = d.to_le_bytes().to_vec();
            v.extend_from_slice(&secs.to_le_bytes());
            v
        }
        // Varchar / Json keep their UTF-8 bytes.
        _ => value.as_bytes().to_vec(),
    };
    Ok(Some(bytes))
}

/// Decodes a slot back into the text form the engine renders.
fn decode_value(column_type: i32, body: &[u8], row: usize) -> Option<String> {
    match cpp_format::column_layout(column_type)? {
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
                3 | 16 | 17 => i32::from_le_bytes(slot.try_into().unwrap()).to_string(),
                4 => i64::from_le_bytes(slot.try_into().unwrap()).to_string(),
                5 => f32::from_le_bytes(slot.try_into().unwrap()).to_string(),
                6 => f64::from_le_bytes(slot.try_into().unwrap()).to_string(),
                _ => return None,
            })
        }
        ColumnLayout::VarLen(16) => {
            // Varchar/Json: word0 = 24-bit length | 5 prefix bytes,
            // word1 = outline offset.
            let at = row * 16;
            let w0 = u64::from_le_bytes(body.get(at..at + 8)?.try_into().ok()?);
            let len = (w0 & 0x00ff_ffff) as usize;
            if len <= 13 {
                // Inline payload: the first `len` of the 13 inline bytes.
                let inline = &body.get(at + 3..at + 16)?[..len];
                Some(String::from_utf8_lossy(inline).to_string())
            } else {
                // Longer payloads live in the outline; the reader needs the
                // outline file, so report the length instead of guessing.
                Some(format!("<outline:{}>", len))
            }
        }
        ColumnLayout::VarLen(_) => None,
    }
}

impl Engine {
    /// Root of the C++-format block mirror.
    fn blocks_root(&self) -> PathBuf {
        self.dir.join("blocks")
    }

    /// Qualifies an unqualified table name with the session database, which is
    /// how the catalog and row keys are spelled.
    fn qualified_table(&self, table: &str) -> String {
        if table.contains('.') {
            table.to_string()
        } else {
            format!("{}.{}", self.current_db.lock().unwrap(), table)
        }
    }

    /// Writes `table`'s rows into `{data_dir}/blocks/db_{db}/tbl_{id}/seg_0/…`
    /// in the C++ block layout, returning the number of rows written.
    ///
    /// Rows are chunked `DEFAULT_BLOCK_CAPACITY` (8192) per block, as C++ does.
    pub fn write_cpp_blocks(&self, table: &str) -> Result<usize> {
        let table = self.qualified_table(table);
        let meta = self
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block writer"
                    ))
                })
            })
            .collect::<Result<_>>()?;

        let rows = self.fetch_rows_with_ids(&table)?;
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();
        // Drop any previous mirror of this table's first segment so a rewrite
        // cannot leave stale blocks behind.
        let seg_dir = root.join(format!("db_{db_id}/tbl_{}/seg_0", meta.id));
        let _ = std::fs::remove_dir_all(&seg_dir);

        let mut written = 0usize;
        for (block_index, chunk) in rows.chunks(DEFAULT_BLOCK_CAPACITY).enumerate() {
            let mut block_columns = Vec::with_capacity(types.len());
            for (col, column_type) in types.iter().enumerate() {
                let mut slots: Vec<Option<Vec<u8>>> = Vec::with_capacity(chunk.len());
                for (_, row) in chunk {
                    let text = row.get(col).map(|v| v.to_slt_string()).unwrap_or_default();
                    slots.push(encode_value(*column_type, &text)?);
                }
                // The scalar packer wants unchanged width, so keep the encoded
                // column body (never the fixed `capacity` length) here: the
                // block writer pads to capacity itself.
                let bytes: Vec<Option<&[u8]>> = slots.iter().map(|s| s.as_deref()).collect();
                let (body, outline) = pack_column(*column_type, &bytes, DEFAULT_BLOCK_CAPACITY)
                    .map_err(|e| Error::Storage(format!("pack column {col}: {e}")))?;
                block_columns.push(BlockColumn {
                    column_id: col as u64,
                    body,
                    outline,
                });
            }
            let version = BlockVersionFile::new(
                &[(block_index as u64 + 1, chunk.len() as u32)],
                DEFAULT_BLOCK_CAPACITY,
            );
            let id = BlockId {
                db_id,
                table_id: meta.id,
                segment_id: 0,
                block_id: block_index as u16,
            };
            cpp_format::write_block(&root, id, &block_columns, &version).map_err(|e| {
                Error::Io {
                    path: root.join(cpp_format::block_dir(db_id, meta.id, 0, id.block_id)),
                    source: e,
                }
            })?;
            written += chunk.len();
        }
        Ok(written)
    }

    /// Mirrors every table into the C++ block layout.
    ///
    /// Called from [`Engine::flush`], so a normal clean shutdown leaves the data
    /// directory readable by the C++ engine. Tables whose columns are not yet
    /// packable (array/tensor/sparse/embedding) are skipped rather than
    /// mis-encoded.
    pub fn flush_cpp_blocks(&self) -> Result<()> {
        for meta in self.catalog.list_tables()? {
            if meta.types.iter().all(|t| column_type_of(t).is_some()) {
                self.write_cpp_blocks(&meta.name)?;
            }
        }
        Ok(())
    }

    /// Reads the block mirror back, returning `(columns, rows)`.
    ///
    /// This never consults the KV store, so a successful read proves the mirror
    /// carries the table on its own.
    pub fn read_cpp_blocks(&self, table: &str) -> Result<(Vec<String>, Vec<Vec<String>>)> {
        let table = self.qualified_table(table);
        let meta = self
            .catalog
            .get_table_by_name(&table)?
            .ok_or_else(|| Error::NotFound(format!("table '{table}'")))?;
        let types: Vec<i32> = meta
            .types
            .iter()
            .map(|t| {
                column_type_of(t).ok_or_else(|| {
                    Error::Invalid(format!(
                        "column type '{t}' is not supported by the C++ block reader"
                    ))
                })
            })
            .collect::<Result<_>>()?;
        let db_id = self.database_id_of(&table);
        let root = self.blocks_root();

        let mut rows: Vec<Vec<String>> = Vec::new();
        for block_index in 0..u16::MAX {
            let id = BlockId {
                db_id,
                table_id: meta.id,
                segment_id: 0,
                block_id: block_index,
            };
            let (version, columns) = match cpp_format::read_block(&root, id) {
                Ok(v) => v,
                Err(_) if block_index == 0 => {
                    return Err(Error::NotFound(format!("no blocks for table '{table}'")))
                }
                Err(_) => break,
            };
            let row_count: usize = version
                .created
                .last()
                .map(|c| c.cumulative_row_count as usize)
                .unwrap_or(0);
            for row in 0..row_count {
                let mut out = Vec::with_capacity(types.len());
                for (col, column_type) in types.iter().enumerate() {
                    let Some(raw) = columns.iter().find(|c| c.column_id == col as u64) else {
                        return Err(Error::Invalid(format!("block is missing column {col}")));
                    };
                    let layout = cpp_format::column_layout(*column_type)
                        .ok_or_else(|| Error::Invalid("unsupported column layout".into()))?;
                    let body = cpp_format::split_body_for_layout(
                        &raw.body,
                        DEFAULT_BLOCK_CAPACITY,
                        layout,
                    );
                    let value = if body.null_bitmap.len() > row / 8
                        && body.null_bitmap[row / 8] & (1 << (row % 8)) == 0
                    {
                        "null".to_string()
                    } else {
                        decode_value(*column_type, &body.data, row).unwrap_or_else(|| "null".into())
                    };
                    out.push(value);
                }
                rows.push(out);
            }
        }
        Ok((meta.columns.clone(), rows))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::QueryResult;

    fn row_strings(engine: &Engine, sql: &str) -> Vec<Vec<String>> {
        let mut out = Vec::new();
        for r in engine.execute(sql).unwrap() {
            if let QueryResult::Rows { rows, .. } = r {
                out = rows;
            }
        }
        out
    }

    #[test]
    fn type_text_mapping() {
        assert_eq!(column_type_of("INT"), Some(3));
        assert_eq!(column_type_of("integer"), Some(3));
        assert_eq!(column_type_of("VARCHAR"), Some(9));
        assert_eq!(column_type_of("DOUBLE"), Some(6));
        assert_eq!(column_type_of("JSON"), Some(22));
        assert_eq!(column_type_of("EMBEDDING(FLOAT,4)"), None);
        assert_eq!(column_type_of("TENSOR(FLOAT,4)"), None);
    }

    #[test]
    fn scalar_values_encode() {
        assert_eq!(encode_value(3, "42").unwrap().unwrap(), 42i32.to_le_bytes());
        assert_eq!(encode_value(4, "42").unwrap().unwrap(), 42i64.to_le_bytes());
        assert_eq!(
            encode_value(6, "1.5").unwrap().unwrap(),
            1.5f64.to_le_bytes()
        );
        // Booleans go through `pack_column`'s compact-bit path, which keys off
        // the textual forms `1`/`0`.
        assert_eq!(encode_value(0, "true").unwrap().unwrap(), b"1".to_vec());
        assert_eq!(encode_value(0, "false").unwrap().unwrap(), b"0".to_vec());
        assert_eq!(encode_value(9, "hi").unwrap().unwrap(), b"hi".to_vec());
        assert!(encode_value(3, "null").unwrap().is_none());
        assert!(encode_value(3, "abc").is_err());
        // 1970-01-02 is one day after the epoch.
        assert_eq!(
            encode_value(16, "1970-01-02").unwrap().unwrap(),
            1i32.to_le_bytes()
        );
    }

    #[test]
    fn blocks_roundtrip_a_real_table() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR, ok BOOL);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (1, 'a', true);")
            .unwrap();
        engine
            .execute("INSERT INTO t VALUES (2, 'bb', false);")
            .unwrap();

        assert_eq!(engine.write_cpp_blocks("t").unwrap(), 2);
        // The C++ file names exist under the mirror root (`tbl_{id}` uses the
        // catalog's table id, so discover it rather than hardcoding).
        let db_dir = dir.path().join("blocks").join("db_0");
        let tbl_dir = std::fs::read_dir(&db_dir)
            .unwrap()
            .filter_map(|e| e.ok())
            .map(|e| e.path())
            .find(|p| {
                p.file_name()
                    .is_some_and(|n| n.to_string_lossy().starts_with("tbl_"))
            })
            .expect("table directory");
        let block = tbl_dir.join("seg_0").join("blk_0");
        assert!(block.join("0.col").exists());
        assert!(block.join("1.col").exists());
        assert!(block.join("2.col").exists());
        assert!(block.join(cpp_format::VERSION_FILE).exists());

        // Read back purely from the block files.
        let (columns, rows) = engine.read_cpp_blocks("t").unwrap();
        assert_eq!(columns, vec!["id", "name", "ok"]);
        assert_eq!(
            rows,
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );

        // ... and the SQL view is unchanged by mirroring.
        assert_eq!(
            row_strings(&engine, "SELECT * FROM t;"),
            vec![
                vec!["1".to_string(), "a".to_string(), "true".to_string()],
                vec!["2".to_string(), "bb".to_string(), "false".to_string()],
            ]
        );
    }

    #[test]
    fn blocks_reject_unsupported_column_types() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE v (id INT, e EMBEDDING(FLOAT,2));")
            .unwrap();
        let err = engine.write_cpp_blocks("v").unwrap_err();
        assert!(err.to_string().contains("not supported"), "{err}");
    }

    #[test]
    fn reading_blocks_without_a_mirror_reports_not_found() {
        let dir = tempfile::tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        assert!(engine.read_cpp_blocks("t").is_err());
    }
}

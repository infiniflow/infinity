//! Writing the catalog in the **C++ layout**: `catalog_format` keys (§8.2) with
//! `value_format` column records (§8.6), so a directory written here is what the
//! C++ engine's catalog expects to read (RUST_REWRITE_PLAN.md §2.6.1 item 4).
//!
//! This is additive: the engine's own catalog storage is untouched, and
//! [`Catalog::write_cpp_layout`] / [`read_cpp_layout`] let us prove the layout
//! round-trips on a real catalog. The C++ global counters
//! (`next_database_id`/`next_table_id`) are written too.

use infinity_common::{Error, Result};
use infinity_expression::json::Json;
use infinity_storage::{catalog_format as key, MetaStore};

use crate::value_format::{
    self, array_type_info, data_type_json, embedding_type_info, logical, ColumnDefValue, IndexDefValue,
};
use crate::{Catalog, IndexMeta, TableMeta};

/// One table as read back from a C++-layout catalog.
#[derive(Debug, Clone, PartialEq)]
pub struct CppTable {
    pub db_name: String,
    pub db_id: u64,
    pub table_name: String,
    pub table_id: u64,
    pub columns: Vec<ColumnDefValue>,
    /// The `next_column_id` tag the C++ writer keeps per table.
    pub next_column_id: u64,
}

/// Maps a declared type text to the C++ `DataType::Serialize()` object.
///
/// Returns `None` for shapes this mapping does not cover yet.
pub fn data_type_of_text(type_text: &str) -> Option<Json> {
    let up = type_text.trim().to_ascii_uppercase();
    let base = up.split('(').next().unwrap_or("").trim();
    let plain = |lt: i64| Some(data_type_json(lt, None));
    match base {
        "BOOL" | "BOOLEAN" => plain(logical::BOOLEAN),
        "TINYINT" | "INT8" => plain(logical::TINYINT),
        "SMALLINT" | "INT16" => plain(logical::SMALLINT),
        "INT" | "INTEGER" | "INT32" => plain(logical::INTEGER),
        "BIGINT" | "INT64" => plain(logical::BIGINT),
        "FLOAT" | "REAL" | "FLOAT32" => plain(logical::FLOAT),
        "DOUBLE" | "FLOAT64" => plain(logical::DOUBLE),
        "FLOAT16" => plain(logical::FLOAT16),
        "BFLOAT16" => plain(logical::BFLOAT16),
        "VARCHAR" | "TEXT" | "STRING" => plain(logical::VARCHAR),
        "JSON" => plain(logical::JSON),
        "DATE" => plain(logical::DATE),
        "TIME" => plain(logical::TIME),
        "DATETIME" | "TIMESTAMP" => plain(logical::DATETIME),
        "DECIMAL" => {
            // `DECIMAL(p,s)`; the engine's type text carries the parameters.
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let p: i64 = parts.next()?.trim().parse().ok()?;
            let s: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                logical::DECIMAL,
                Some(value_format::decimal_type_info(p, s)),
            ))
        }
        "EMBEDDING" | "VECTOR" | "TENSOR" | "TENSORARRAY" | "MULTIVECTOR" => {
            let lt = match base {
                "EMBEDDING" | "VECTOR" => logical::EMBEDDING,
                "TENSOR" => logical::TENSOR,
                "TENSORARRAY" => logical::TENSORARRAY,
                _ => logical::MULTIVECTOR,
            };
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let elem = parts.next()?.trim().to_string();
            let dim: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                lt,
                Some(embedding_type_info(element_code(&elem)?, dim)),
            ))
        }
        "ARRAY" => {
            let inner = up
                .split_once('(')?
                .1
                .trim_end_matches(')')
                .trim()
                .to_string();
            let element = data_type_of_text(&inner)?;
            Some(data_type_json(
                logical::ARRAY,
                Some(array_type_info(element)),
            ))
        }
        "SPARSE" => {
            // `SPARSE(<elem>,<bound>)` -> `SparseInfo::Serialize()`
            // (`sparse_info.cpp:103-110`, `GetIndexType` `:44-58`).
            let inner = up.split_once('(')?.1.trim_end_matches(')');
            let mut parts = inner.split(',');
            let elem = parts.next()?.trim().to_string();
            let dim: i64 = parts.next()?.trim().parse().ok()?;
            Some(data_type_json(
                logical::SPARSE,
                Some(value_format::sparse_type_info(
                    element_code(&elem)?,
                    value_format::sparse_index_type(dim),
                    dim,
                    value_format::sparse_store::SORT,
                )),
            ))
        }
        _ => None,
    }
}

/// Element type name -> `EmbeddingDataType` ordinal.
fn element_code(elem: &str) -> Option<i64> {
    Some(if elem.contains("UINT8") {
        value_format::element::UINT8
    } else if elem.contains("INT8") || elem.contains("TINYINT") {
        value_format::element::INT8
    } else if elem.contains("INT16") {
        value_format::element::INT16
    } else if elem.contains("INT32") || elem == "INT" {
        value_format::element::INT32
    } else if elem.contains("INT64") {
        value_format::element::INT64
    } else if elem.contains("FLOAT64") || elem.contains("DOUBLE") {
        value_format::element::FLOAT64
    } else if elem.contains("BFLOAT16") {
        value_format::element::BFLOAT16
    } else if elem.contains("FLOAT16") {
        value_format::element::FLOAT16
    } else if elem.contains("BIT") {
        value_format::element::BIT
    } else {
        value_format::element::FLOAT32
    })
}

/// The database a qualified table name belongs to (`default_db` when bare).
fn db_of(table: &str) -> &str {
    if table.contains('.') {
        table.split('.').next().unwrap_or("default_db")
    } else {
        "default_db"
    }
}

/// The unqualified part of a table name.
fn table_of(table: &str) -> &str {
    table.rsplit('.').next().unwrap_or(table)
}

/// Builds the C++ `IndexBase`-family record for one engine index.
///
/// Returns `None` for a `USING` kind the C++ has no index class for (or for a
/// column that vanished); the caller then leaves no record rather than write a
/// half-formed one.
fn index_def_for(im: &IndexMeta, table: &CppTable) -> Option<IndexDefValue> {
    let col = table
        .columns
        .iter()
        .find(|c| c.column_name == im.column)?;
    let opt = |k: &str| -> Option<String> {
        im.options
            .iter()
            .find(|(n, _)| n.eq_ignore_ascii_case(k))
            .map(|(_, v)| v.clone())
    };
    let opt_word = |k: &str, default: &str| -> String {
        opt(k)
            .map(|v| v.trim().trim_matches(|c| c == '\'' || c == '"').to_string())
            .unwrap_or_else(|| default.to_string())
    };
    let opt_num = |k: &str, default: i64| -> i64 {
        opt(k).and_then(|v| v.trim().parse().ok()).unwrap_or(default)
    };
    match im.index_type.as_str() {
        "HNSW" => Some(IndexDefValue::hnsw(
            &im.name,
            &table.table_name,
            &im.column,
            value_format::metric_name_of(&opt_word("METRIC", "l2")),
            value_format::hnsw_encode_name_of(&opt_word("ENCODE", "plain")),
            value_format::hnsw_build::PLAIN,
            opt_num("M", value_format::HNSW_M),
            opt_num("EF_CONSTRUCTION", value_format::HNSW_EF_CONSTRUCTION),
            opt_num("BLOCK_SIZE", value_format::HNSW_BLOCK_SIZE),
        )),
        "FULLTEXT" => Some(IndexDefValue::full_text(
            &im.name,
            &table.table_name,
            &im.column,
            &opt_word("ANALYZER", "standard"),
        )),
        // An index with no `USING` kind is the C++ default, a secondary index.
        "SECONDARY" | "" => {
            let is_json =
                value_format::data_type_code(&col.column_type) == Some(value_format::logical::JSON);
            let low = opt("CARDINALITY")
                .map(|v| v.eq_ignore_ascii_case("low"))
                .unwrap_or(false);
            Some(IndexDefValue::secondary(
                &im.name,
                &table.table_name,
                &im.column,
                low,
                is_json,
            ))
        }
        "IVF" | "FLAT" => {
            let elem = value_format::type_info_int(&col.column_type, "embedding_type")
                .unwrap_or(value_format::element::FLOAT32);
            Some(IndexDefValue::ivf_plain(
                &im.name,
                &table.table_name,
                &im.column,
                value_format::metric_code_of(&opt_word("METRIC", "l2")),
                elem,
            ))
        }
        _ => None,
    }
}

impl Catalog {
    /// Writes every database, table and column into `store` using the C++
    /// key space and value encoding, returning how many records were written.
    ///
    /// The commit timestamp is a per-database monotonic counter, which is all a
    /// C++ reader needs (it scans a name's prefix and takes the newest version).
    pub fn write_cpp_layout(&self, store: &dyn MetaStore) -> Result<usize> {
        let dbs = self.list_databases()?;
        let tables = self.list_tables()?;
        let indexes = self.list_indexes()?;
        let mut written = 0usize;

        let next_table_id = tables.iter().map(|t| t.id).max().unwrap_or(0) + 1;
        let db_id_of =
            |name: &str| -> u64 { dbs.iter().position(|d| d == name).unwrap_or(0) as u64 };

        for (i, db) in dbs.iter().enumerate() {
            let ts = i as u64 + 1;
            let db_str = i.to_string();
            store.put(key::catalog_db_key(db, ts).as_bytes(), db_str.as_bytes())?;
            store.put(
                key::catalog_db_tag_key(&db_str, "next_table_id").as_bytes(),
                next_table_id.to_string().as_bytes(),
            )?;
            written += 2;
        }

        // Resolved table layout, kept so the index pass below can reach each
        // table's db id, table id and column types.
        let mut resolved: Vec<(String, String, u64, CppTable)> = Vec::new();

        for t in &tables {
            let db_name = db_of(&t.name);
            if !dbs.iter().any(|d| d == db_name) {
                continue; // unknown database: nothing to hang the table under
            }
            let db_id = db_id_of(db_name);
            let db_str = db_id.to_string();
            let tbl_str = t.id.to_string();
            let ts = t.id + 1; // monotonic per write
            let name = table_of(&t.name);

            store.put(
                key::catalog_table_key(&db_str, name, ts).as_bytes(),
                tbl_str.as_bytes(),
            )?;
            for (tag, value) in [
                ("next_column_id", t.columns.len() as u64),
                ("next_segment_id", 0),
                ("unsealed_segment_id", 0),
                ("comment", 0),
                // `next_index_id` starts at 0 (`table_meta_impl.cpp:281-282`);
                // the C++ reader takes it as the next id to hand out.
                ("next_index_id", 0),
            ] {
                store.put(
                    key::catalog_table_tag_key(&db_str, &tbl_str, tag).as_bytes(),
                    value.to_string().as_bytes(),
                )?;
                written += 1;
            }
            written += 1;

            let mut columns = Vec::new();
            for (i, col) in t.columns.iter().enumerate() {
                let type_text = t.types.get(i).cloned().unwrap_or_default();
                let Some(data_type) = data_type_of_text(&type_text) else {
                    return Err(Error::Catalog(format!(
                        "column '{col}' of '{}' has a type the C++ catalog encoding \
                         does not cover yet: '{type_text}'",
                        t.name
                    )));
                };
                let mut value = ColumnDefValue::new(data_type, i as i64, col.clone());
                value.column_comment = t.comments.get(i).cloned().flatten();
                store.put(
                    key::table_column_key(&db_str, &tbl_str, col, ts).as_bytes(),
                    &value_format::column_def_bytes(&value),
                )?;
                written += 1;
                columns.push(value);
            }
            resolved.push((
                db_str,
                tbl_str,
                ts,
                CppTable {
                    db_name: db_name.to_string(),
                    db_id,
                    table_name: name.to_string(),
                    table_id: t.id,
                    columns,
                    next_column_id: t.columns.len() as u64,
                },
            ));
        }

        // Index records: `idx|{db}|{tbl}|{name}|{ts}` -> index_id (a per-table
        // counter starting at 0), plus an `index_base` tag holding the C++
        // `IndexBase::Serialize()` JSON (`table_index_meta_impl.cpp:90-97`).
        for im in &indexes {
            let im_table = table_of(&im.table);
            let im_db = db_of(&im.table);
            let Some((db_str, tbl_str, ts, cpp)) = resolved
                .iter()
                .find(|(_, _, _, c)| c.table_name == im_table && c.db_name == im_db)
            else {
                continue; // the index's table is not in the written layout
            };
            let Some(def) = index_def_for(im, cpp) else {
                continue;
            };
            // `list_indexes` is ordered, so ids are stable across runs.
            let index_id = indexes
                .iter()
                .filter(|o| table_of(&o.table) == im_table && db_of(&o.table) == im_db)
                .position(|o| o.name == im.name)
                .unwrap_or(0);
            store.put(
                key::catalog_index_key(db_str, tbl_str, &im.name, *ts).as_bytes(),
                index_id.to_string().as_bytes(),
            )?;
            store.put(
                key::catalog_index_tag_key(db_str, tbl_str, &index_id.to_string(), "index_base")
                    .as_bytes(),
                &value_format::index_def_bytes(&def),
            )?;
            written += 2;
        }
        // Record how many ids each table handed out.
        for (db_str, tbl_str, _, cpp) in &resolved {
            let count = indexes
                .iter()
                .filter(|o| table_of(&o.table) == cpp.table_name && db_of(&o.table) == cpp.db_name)
                .count();
            if count > 0 {
                store.put(
                    key::catalog_table_tag_key(db_str, tbl_str, "next_index_id").as_bytes(),
                    count.to_string().as_bytes(),
                )?;
            }
        }

        store.put(
            key::NEXT_DATABASE_ID.as_bytes(),
            dbs.len().to_string().as_bytes(),
        )?;
        store.put(
            key::NEXT_TABLE_ID.as_bytes(),
            next_table_id.to_string().as_bytes(),
        )?;
        written += 2;
        store.flush()?;
        Ok(written)
    }
}

/// Reads a C++-layout catalog back, one entry per table.
pub fn read_cpp_layout(store: &dyn MetaStore) -> Result<Vec<CppTable>> {
    let mut dbs: Vec<(u64, String)> = Vec::new();
    // `catalog|db|` - note `catalog_db_prefix("")` would emit a doubled pipe.
    for (k, v) in store.scan_prefix(b"catalog|db|")? {
        let key = String::from_utf8_lossy(&k).to_string();
        // `catalog|db|{name}|{ts}` -> the name may itself contain '|', so take
        // the last two fields off the right.
        let mut it = key.rsplitn(3, '|');
        let _ts = it.next();
        let name = it.next().unwrap_or_default().to_string();
        let id = String::from_utf8_lossy(&v).parse().unwrap_or(0);
        dbs.push((id, name));
    }
    dbs.sort();

    let mut out = Vec::new();
    for (db_id, db_name) in &dbs {
        let prefix = key::catalog_db_table_prefix(&db_id.to_string());
        for (k, v) in store.scan_prefix(prefix.as_bytes())? {
            let key = String::from_utf8_lossy(&k).to_string();
            let mut it = key.rsplitn(2, '|');
            let _ts = it.next();
            let rest = it.next().unwrap_or_default().to_string();
            // `catalog|tbl|{db_id}|{table_name}` - the name is the 4th field.
            let table_name = rest.splitn(4, '|').nth(3).unwrap_or_default().to_string();
            let table_id: u64 = String::from_utf8_lossy(&v).parse().unwrap_or(0);

            let mut columns = Vec::new();
            let col_prefix = key::table_column_prefix(&db_id.to_string(), &table_id.to_string());
            for (_, cv) in store.scan_prefix(col_prefix.as_bytes())? {
                if let Some(col) = value_format::column_def_from_bytes(&cv) {
                    columns.push(col);
                }
            }
            columns.sort_by_key(|c| c.column_id);
            let next_column_id = store
                .get(
                    key::catalog_table_tag_key(
                        &db_id.to_string(),
                        &table_id.to_string(),
                        "next_column_id",
                    )
                    .as_bytes(),
                )?
                .and_then(|v| String::from_utf8_lossy(&v).parse().ok())
                .unwrap_or(columns.len() as u64);
            out.push(CppTable {
                db_name: db_name.clone(),
                db_id: *db_id,
                table_name,
                table_id,
                columns,
                next_column_id,
            });
        }
    }
    Ok(out)
}

/// Reads back the index records written by [`Catalog::write_cpp_layout`]:
/// every `idx|…|index_base` tag, decoded.
pub fn read_cpp_indexes(store: &dyn MetaStore) -> Result<Vec<IndexDefValue>> {
    let mut out = Vec::new();
    for (k, v) in store.scan_prefix(b"catalog|idx|")? {
        let key = String::from_utf8_lossy(&k).to_string();
        // `catalog|idx|{db}|{tbl}|{name}|{ts}` - six fields. The value is the
        // numeric index id that locates the `index_base` tag.
        let fields: Vec<&str> = key.split('|').collect();
        if fields.len() != 6 {
            continue;
        }
        let index_id = String::from_utf8_lossy(&v).to_string();
        let tag = key::catalog_index_tag_key(fields[2], fields[3], &index_id, "index_base");
        if let Some(bytes) = store.get(tag.as_bytes())? {
            if let Some(def) = value_format::index_def_from_bytes(&bytes) {
                out.push(def);
            }
        }
    }
    out.sort_by(|a, b| a.base.index_name.cmp(&b.base.index_name));
    Ok(out)
}

/// Rebuilds `TableMeta`s from a C++-layout catalog (for comparison with the
/// engine's own view).
pub fn read_cpp_tables_as_meta(store: &dyn MetaStore) -> Result<Vec<TableMeta>> {
    let mut out = Vec::new();
    for t in read_cpp_layout(store)? {
        out.push(TableMeta {
            id: t.table_id,
            name: format!("{}.{}", t.db_name, t.table_name),
            columns: t.columns.iter().map(|c| c.column_name.clone()).collect(),
            defaults: vec![None; t.columns.len()],
            not_nulls: vec![false; t.columns.len()],
            types: Vec::new(),
            comments: t.columns.iter().map(|c| c.column_comment.clone()).collect(),
        });
    }
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    use infinity_storage::FjallMetaStore;
    use std::sync::Arc;

    #[test]
    fn data_type_mapping_covers_the_common_shapes() {
        assert_eq!(
            data_type_of_text("INTEGER").unwrap().compact(),
            r#"{"data_type":3}"#
        );
        assert_eq!(
            data_type_of_text("EMBEDDING(FLOAT,4)").unwrap().compact(),
            r#"{"data_type":12,"type_info":{"dimension":4,"embedding_type":6}}"#
        );
        assert_eq!(
            data_type_of_text("ARRAY(INTEGER)").unwrap().compact(),
            r#"{"data_type":22,"type_info":{"data_type":3}}"#
        );
        assert_eq!(
            data_type_of_text("DECIMAL(18,2)").unwrap().compact(),
            r#"{"data_type":6,"type_info":{"precision":18,"scale":2}}"#
        );
        // Unknown shapes are reported instead of guessed.
        assert!(data_type_of_text("MYSTERY").is_none());
        assert!(data_type_of_text("EMBEDDING(BIT,8)").is_some());
        // SPARSE carries the `SparseInfo` type_info (index type from the bound).
        assert_eq!(
            data_type_of_text("SPARSE(FLOAT,100)").unwrap().compact(),
            r#"{"data_type":15,"type_info":{"data_type":6,"dimension":100,"index_type":2,"sort":0}}"#
        );
    }

    #[test]
    fn cpp_layout_roundtrips_a_catalog() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        let columns = ["id", "name", "v"].map(String::from).to_vec();
        let types = ["INTEGER", "VARCHAR", "EMBEDDING(FLOAT,3)"]
            .map(String::from)
            .to_vec();
        let t = cat
            .create_table_typed(
                "default_db.t",
                &columns,
                &[None, None, None],
                &[false, false, false],
                &types,
                &[None, None, None],
            )
            .unwrap();

        // Write into a *separate* store so the engine's own catalog is untouched.
        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        let written = cat.write_cpp_layout(store.as_ref()).unwrap();
        assert!(written >= 4, "wrote {written} records");

        // The C++ keys are there.
        assert_eq!(
            store
                .get(key::NEXT_TABLE_ID.as_bytes())
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string()),
            Some((t.id + 1).to_string())
        );
        assert!(!store
            .scan_prefix(key::catalog_db_table_prefix("0").as_bytes())
            .unwrap()
            .is_empty());

        let back = read_cpp_layout(store.as_ref()).unwrap();
        assert_eq!(back.len(), 1);
        let got = &back[0];
        assert_eq!(got.db_name, "default_db");
        assert_eq!(got.table_name, "t");
        assert_eq!(got.table_id, t.id);
        assert_eq!(
            got.columns
                .iter()
                .map(|c| c.column_name.clone())
                .collect::<Vec<_>>(),
            vec!["id", "name", "v"]
        );
        // The embedding column kept its type_info, byte for byte.
        assert_eq!(
            got.columns[2].column_type.compact(),
            r#"{"data_type":12,"type_info":{"dimension":3,"embedding_type":6}}"#
        );
        assert_eq!(got.next_column_id, 3);
    }

    #[test]
    fn unsupported_column_type_is_reported() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        cat.create_table_typed(
            "default_db.t",
            &["x".to_string()],
            &[None],
            &[false],
            &["MYSTERY".to_string()],
            &[None],
        )
        .unwrap();
        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        let err = cat.write_cpp_layout(store.as_ref()).unwrap_err();
        assert!(err.to_string().contains("does not cover"), "{err}");
    }

    #[test]
    fn index_records_are_written_in_the_cpp_layout() {
        let tmp = tempfile::tempdir().unwrap();
        let cat = Catalog::open(tmp.path()).unwrap();
        let columns = ["id", "v", "body"].map(String::from).to_vec();
        let types = ["INTEGER", "EMBEDDING(FLOAT,4)", "VARCHAR"]
            .map(String::from)
            .to_vec();
        cat.create_table_typed(
            "default_db.t",
            &columns,
            &[None, None, None],
            &[false, false, false],
            &types,
            &[None, None, None],
        )
        .unwrap();
        cat.create_index_typed(
            "idx_vec",
            "default_db.t",
            "v",
            "HNSW",
            &[("METRIC".to_string(), "l2".to_string())],
        )
        .unwrap();
        cat.create_index_typed(
            "idx_ft",
            "default_db.t",
            "body",
            "FULLTEXT",
            &[("ANALYZER".to_string(), "standard".to_string())],
        )
        .unwrap();

        let out = tempfile::tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(out.path(), "cpp").unwrap());
        cat.write_cpp_layout(store.as_ref()).unwrap();

        // The `next_index_id` tag counts both indexes.
        let db = cat.list_databases().unwrap();
        let db_id = db.iter().position(|d| d == "default_db").unwrap().to_string();
        let tables = cat.list_tables().unwrap();
        let tbl_id = tables[0].id.to_string();
        assert_eq!(
            store
                .get(key::catalog_table_tag_key(&db_id, &tbl_id, "next_index_id").as_bytes())
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string()),
            Some("2".to_string())
        );

        let defs = read_cpp_indexes(store.as_ref()).unwrap();
        assert_eq!(defs.len(), 2);
        let hnsw = defs.iter().find(|d| d.base.index_name == "idx_vec").unwrap();
        assert_eq!(hnsw.base.index_type, "HNSW");
        assert_eq!(hnsw.base.file_name, "t_idx_vec");
        assert_eq!(hnsw.base.column_names, vec!["v".to_string()]);
        let ft = defs.iter().find(|d| d.base.index_name == "idx_ft").unwrap();
        assert_eq!(ft.base.index_type, "FULLTEXT");
        assert_eq!(ft.base.file_name, "t_idx_ft");
        assert_eq!(ft.base.column_names, vec!["body".to_string()]);
    }
}

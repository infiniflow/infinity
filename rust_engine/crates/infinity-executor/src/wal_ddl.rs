//! Builds the reference's `TableDef` from the port's per-column catalog
//! records, so a DDL can hand a typed `WalCmdCreateTableV2` to the transaction
//! instead of a `KvBatch`.
//!
//! The per-column record (`tbl|col|{db}|{tbl}|{name}|{ts}`) is JSON:
//! ```json
//! {"column_id":0,"column_name":"id",
//!  "column_type":{"data_type":3},"default":{"type":4}}
//! ```
//! `data_type` is the `LogicalType` discriminant; `type_info` carries the
//! embedding/sparse/decimal payload. The default's `type` is a `LiteralType`
//! discriminant (`4` = null).
//!
//! Uses the crate's own JSON (`infinity_expression::json`) — the workspace
//! does not depend on `serde_json`.

use infinity_expression::json::{extract, parse, Json};
use infinity_storage::wal::ddl_def::{
    ColumnDef, ConstantExpr, DataType, EmbeddingDataType, LogicalType, SparseStoreType, TableDef,
    TypeInfo,
};

fn as_i64(j: Option<&Json>) -> Option<i64> {
    match j {
        Some(Json::Int(v)) => Some(*v),
        Some(Json::Float(v)) => Some(*v as i64),
        _ => None,
    }
}

fn as_str(j: Option<&Json>) -> Option<&str> {
    match j {
        Some(Json::Str(s)) => Some(s.as_str()),
        _ => None,
    }
}

/// One column's catalog record, decoded.
pub struct ColumnRecord {
    doc: Json,
}

impl ColumnRecord {
    /// Parses the stored JSON of one `tbl|col|…` record.
    pub fn from_json(text: &str) -> Option<Self> {
        Some(Self { doc: parse(text)? })
    }

    pub fn id(&self) -> i64 {
        as_i64(extract(&self.doc, ".column_id")).unwrap_or(0)
    }

    pub fn name(&self) -> String {
        as_str(extract(&self.doc, ".column_name"))
            .unwrap_or("")
            .to_string()
    }

    fn data_type(&self) -> i8 {
        as_i64(extract(&self.doc, ".column_type.data_type")).unwrap_or(36) as i8
    }

    fn type_info(&self, key: &str) -> Option<i64> {
        as_i64(extract(&self.doc, &format!(".column_type.type_info.{key}")))
    }

    pub fn to_column_def(&self) -> ColumnDef {
        ColumnDef {
            id: self.id(),
            column_type: self.column_type(),
            name: self.name(),
            constraints: vec![],
            comment: String::new(),
            default_expr: self.default_expr(),
            build_bloom_filter: false,
        }
    }

    fn column_type(&self) -> DataType {
        data_type_from_json(extract(&self.doc, ".column_type"))
    }

    fn default_expr(&self) -> ConstantExpr {
        let literal = as_i64(extract(&self.doc, ".default.type")).unwrap_or(4);
        let value = extract(&self.doc, ".default.value");
        match literal {
            0 => ConstantExpr::Boolean(matches!(value, Some(Json::Bool(true)))),
            1 => ConstantExpr::Double(as_i64(value).unwrap_or(0) as f64),
            2 => ConstantExpr::String(as_str(value).unwrap_or("").to_string()),
            3 => ConstantExpr::Integer(as_i64(value).unwrap_or(0)),
            _ => ConstantExpr::Null,
        }
    }
}

/// Builds a `TableDef` from the table's per-column records (ordered by id).
pub fn table_def_from_columns(
    schema_name: &str,
    table_name: &str,
    columns: &[ColumnRecord],
) -> TableDef {
    TableDef {
        schema_name: schema_name.to_string(),
        table_name: table_name.to_string(),
        table_comment: String::new(),
        columns: columns.iter().map(ColumnRecord::to_column_def).collect(),
    }
}

use infinity_storage::WalCmd;

/// Builds a reference `DataType` from the catalog's `column_type` object
/// (`{"data_type": <LogicalType>, "type_info": {...}}`).
pub fn data_type_from_json(doc: Option<&Json>) -> DataType {
    let doc = doc.cloned().unwrap_or(Json::Null);
    let logical_type =
        LogicalType::from_discriminant(as_i64(extract(&doc, ".data_type")).unwrap_or(36) as i8);
    let info = |key: &str| as_i64(extract(&doc, &format!(".type_info.{key}")));
    let type_info = match logical_type {
        LogicalType::Embedding
        | LogicalType::Tensor
        | LogicalType::TensorArray
        | LogicalType::MultiVector => TypeInfo::Embedding {
            element_type: EmbeddingDataType::from_discriminant(
                info("embedding_type").unwrap_or(10) as i8,
            ),
            dimension: info("dimension").unwrap_or(0) as i32,
        },
        LogicalType::Sparse => TypeInfo::Sparse {
            data_type: EmbeddingDataType::from_discriminant(info("data_type").unwrap_or(10) as i8),
            index_type: EmbeddingDataType::from_discriminant(info("index_type").unwrap_or(10) as i8),
            dimension: info("dimension").unwrap_or(0),
            store_type: SparseStoreType::from_discriminant(info("sort").unwrap_or(2) as i8),
        },
        LogicalType::Decimal => TypeInfo::Decimal {
            precision: info("precision").unwrap_or(0),
            scale: info("scale").unwrap_or(0),
        },
        _ => TypeInfo::None,
    };
    DataType {
        logical_type,
        type_info,
    }
}

/// [`data_type_from_json`] from the declared type text (`VECTOR,4,FLOAT`).
pub fn data_type_of_text(text: &str) -> Option<DataType> {
    let doc = infinity_catalog::cpp_layout::data_type_of_text(text)?;
    Some(data_type_from_json(Some(&doc)))
}

/// Detects a `CREATE TABLE` batch and turns it into the reference's typed
/// `WalCmdCreateTableV2`, so the WAL carries the reference bytes instead of a
/// `KvBatch`. Returns `None` for any other DDL (CreateIndex, Drop, …).
///
/// The catalog writes are still applied to the store by `commit_at`; replay
/// deliberately skips typed DDL (the catalog is durable at commit), so this
/// only changes the WAL's bytes.
pub fn create_table_command_from_batch(batch: &[(Vec<u8>, Option<Vec<u8>>)]) -> Option<WalCmd> {
    // (db_id, table_id, name)
    let mut table: Option<(String, String, String)> = None;
    let mut cols: Vec<(i64, ColumnRecord)> = Vec::new();
    let mut has_index = false;
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if let Some(rest) = key.strip_prefix("catalog|tbl|") {
            // catalog|tbl|{db_id}|{name}|{ts}; the table id is the value.
            let parts: Vec<&str> = rest.split('|').collect();
            if parts.len() >= 2 {
                let table_id = v
                    .as_ref()
                    .map(|b| String::from_utf8_lossy(b).into_owned())
                    .unwrap_or_default();
                table = Some((parts[0].to_string(), table_id, parts[1].to_string()));
            }
        } else if key.starts_with("catalog|idx|") {
            has_index = true;
        } else if let Some(_rest) = key.strip_prefix("tbl|col|") {
            // tbl|col|{db}|{tbl}|{name}|{ts}
            if let Some(Some(val)) = v.as_ref().map(Some) {
                if let Ok(text) = std::str::from_utf8(val) {
                    if let Some(rec) = ColumnRecord::from_json(text) {
                        cols.push((rec.id(), rec));
                    }
                }
            }
        }
    }
    if has_index || cols.is_empty() {
        return None;
    }
    let (db_id, table_id, table_name) = table?;
    cols.sort_by_key(|(id, _)| *id);
    let ordered: Vec<ColumnRecord> = cols.into_iter().map(|(_, r)| r).collect();
    let td = table_def_from_columns("default_db", &table_name, &ordered);
    Some(WalCmd::CreateTable {
        db_name: "default_db".to_string(),
        db_id,
        table_id,
        table_def: td,
    })
}

/// Maps the port's `idx|{db}|{tbl}|{idx_id}|index_base` JSON to a full
/// reference `IndexDef` (base + the per-type option fields the record carries).
///
/// IVF/Plaid/EMVB/SMVE/DiskAnn are not produced: their option structs are PODs
/// written by `memcpy` (padding-sensitive) and are not handled yet, so the
/// caller falls back to the `KvBatch`.
pub fn index_def_from_json(text: &str) -> Option<infinity_storage::wal::index_def::IndexDef> {
    use infinity_storage::wal::index_def::{
        HnswBuildType, HnswEncodeType, IndexBase, IndexDef, IndexType, MetricType,
    };
    let doc = parse(text)?;
    let type_text = as_str(extract(&doc, ".index_type"))?;
    let index_type = match type_text {
        "SECONDARY" => IndexType::Secondary,
        "FULLTEXT" => IndexType::FullText,
        "HNSW" => IndexType::Hnsw,
        "BMP" => IndexType::Bmp,
        _ => return None,
    };
    let column_names = match extract(&doc, ".column_names") {
        Some(Json::Arr(items)) => items
            .iter()
            .filter_map(|x| as_str(Some(x)).map(str::to_string))
            .collect(),
        _ => Vec::new(),
    };
    let base = IndexBase {
        index_type,
        index_name: as_str(extract(&doc, ".index_name"))?.to_string(),
        index_comment: as_str(extract(&doc, ".index_comment"))
            .unwrap_or("")
            .to_string(),
        file_name: as_str(extract(&doc, ".file_name"))
            .unwrap_or("")
            .to_string(),
        column_names,
    };
    Some(match index_type {
        IndexType::Secondary => IndexDef::Secondary {
            base,
            cardinality: as_i64(extract(&doc, ".cardinality").or_else(|| None))
                .map(|v| v as u8)
                .unwrap_or(0),
            is_json: matches!(extract(&doc, ".is_json_index"), Some(Json::Bool(true))),
        },
        IndexType::FullText => IndexDef::FullText {
            base,
            analyzer: as_str(extract(&doc, ".analyzer")).unwrap_or("").to_string(),
            flag: as_i64(extract(&doc, ".flag")).unwrap_or(0) as u8,
        },
        IndexType::Bmp => IndexDef::Bmp {
            base,
            block_size: as_i64(extract(&doc, ".block_size")).unwrap_or(0) as u64,
            compress_type: as_i64(extract(&doc, ".compress_type")).unwrap_or(0) as i8,
        },
        IndexType::Hnsw => IndexDef::Hnsw {
            base,
            metric_type: match as_str(extract(&doc, ".metric_type")).unwrap_or("") {
                "l2" => MetricType::L2,
                "ip" => MetricType::InnerProduct,
                "cosine" => MetricType::Cosine,
                _ => MetricType::Invalid,
            },
            encode_type: match as_str(extract(&doc, ".encode_type")).unwrap_or("plain") {
                "plain" => HnswEncodeType::Plain,
                "lvq" => HnswEncodeType::Lvq,
                "rabitq" => HnswEncodeType::Rabitq,
                _ => HnswEncodeType::Invalid,
            },
            build_type: match as_str(extract(&doc, ".build_type")).unwrap_or("plain") {
                "plain" => HnswBuildType::Plain,
                "lsg" => HnswBuildType::Lsg,
                _ => HnswBuildType::Invalid,
            },
            m: as_i64(extract(&doc, ".M")).unwrap_or(0) as u64,
            ef_construction: as_i64(extract(&doc, ".ef_construction")).unwrap_or(0) as u64,
            block_size: as_i64(extract(&doc, ".block_size")).unwrap_or(0) as u64,
            has_lsg: false,
        },
        _ => return None,
    })
}

/// Detects a `CREATE INDEX` batch for a **secondary** index and turns it into
/// the reference's `WalCmdCreateIndexV2`. Other index kinds need their own
/// option structs (`IndexIVFOption`, HNSW M/ef, …), which are not serialized
/// yet, so they fall back to the `KvBatch`.
///
/// `table_name` and `table_key` are not in the batch; the caller supplies them
/// (the reference's `table_key` is the table's own `catalog|tbl|…` key).
pub fn create_index_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    table_name: &str,
    table_key: &str,
) -> Option<WalCmd> {
    use infinity_storage::wal::index_def::IndexDef;
    if table_key.is_empty() {
        return None;
    }
    let mut db_id = String::new();
    let mut table_id = String::new();
    let mut index_id = String::new();
    let mut has_index = false;
    let mut index_base: Option<IndexDef> = None;
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if let Some(rest) = key.strip_prefix("catalog|idx|") {
            // catalog|idx|{db_id}|{table_id}|{index_name}|{ts}
            let parts: Vec<&str> = rest.split('|').collect();
            if parts.len() >= 3 {
                db_id = parts[0].to_string();
                table_id = parts[1].to_string();
                index_id = v
                    .as_ref()
                    .map(|b| String::from_utf8_lossy(b).into_owned())
                    .unwrap_or_default();
                has_index = true;
            }
        } else if key.ends_with("|index_base") {
            if let Some(Some(val)) = v.as_ref().map(Some) {
                if let Ok(text) = std::str::from_utf8(val) {
                    if let Some(def) = index_def_from_json(text) {
                        index_base = Some(def);
                    }
                }
            }
        }
    }
    if !has_index {
        return None;
    }
    let index_base = index_base?;
    Some(WalCmd::CreateIndex {
        db_name: "default_db".to_string(),
        db_id,
        table_name: table_name.to_string(),
        table_id,
        index_id,
        index_base,
        segment_index_infos: Vec::new(),
        table_key: table_key.to_string(),
    })
}

/// Detects a `CREATE DATABASE` batch (`catalog|db|{name}|{ts}` → db id) and
/// builds the reference's `WalCmdCreateDatabaseV2`.
pub fn create_database_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    target: &str,
) -> Option<WalCmd> {
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if let Some(rest) = key.strip_prefix("catalog|db|") {
            // catalog|db|{name}|{ts}
            let parts: Vec<&str> = rest.split('|').collect();
            if parts.len() >= 2 && parts[0] == target {
                let db_id = v
                    .as_ref()
                    .map(|b| String::from_utf8_lossy(b).into_owned())
                    .unwrap_or_default();
                if !db_id.is_empty() {
                    return Some(WalCmd::CreateDatabase {
                        db_name: parts[0].to_string(),
                        db_id,
                        db_comment: String::new(),
                    });
                }
            }
        }
    }
    None
}

/// Detects an `ADD COLUMNS` batch (its new `tbl|col|…` records) and builds the
/// reference's `WalCmdAddColumnsV2`. `base_index` is the table's column count
/// before the ADD, so the appended columns' positions are `base_index..`.
pub fn add_columns_command_from_batch(
    batch: &[(Vec<u8>, Option<Vec<u8>>)],
    db_id: &str,
    table_id: &str,
    table_name: &str,
    table_key: &str,
    base_index: u32,
    new_names: &[String],
) -> Option<WalCmd> {
    if table_key.is_empty() {
        return None;
    }
    let mut cols: Vec<(i64, ColumnRecord)> = Vec::new();
    for (k, v) in batch {
        let Ok(key) = std::str::from_utf8(k) else {
            continue;
        };
        if key.starts_with("tbl|col|") {
            if let Some(Some(val)) = v.as_ref().map(Some) {
                if let Ok(text) = std::str::from_utf8(val) {
                    if let Some(rec) = ColumnRecord::from_json(text) {
                        if new_names.iter().any(|n| n == &rec.name()) {
                            cols.push((rec.id(), rec));
                        }
                    }
                }
            }
        }
    }
    if cols.is_empty() {
        return None;
    }
    cols.sort_by_key(|(id, _)| *id);
    let defs: Vec<_> = cols.into_iter().map(|(_, r)| r.to_column_def()).collect();
    // The reference stores the new columns' **1-based** positions
    // (one existing column -> the first added column is index 2).
    let column_idx_list: Vec<u32> = (0..defs.len() as u32).map(|i| base_index + i + 1).collect();
    Some(WalCmd::AddColumns {
        db_name: "default_db".to_string(),
        db_id: db_id.to_string(),
        table_name: table_name.to_string(),
        table_id: table_id.to_string(),
        column_idx_list,
        column_defs: defs,
        table_key: table_key.to_string(),
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    // The exact JSON the port writes for `tt(id int, name varchar, v
    // vector,4,float, sp sparse,8,float,int, sc float, b bool)`.
    const RECORDS: &[&str] = &[
        r#"{"column_id":0,"column_name":"id","column_type":{"data_type":3},"default":{"type":4}}"#,
        r#"{"column_id":1,"column_name":"name","column_type":{"data_type":9},"default":{"type":4}}"#,
        r#"{"column_id":2,"column_name":"v","column_type":{"data_type":23,"type_info":{"dimension":4,"embedding_type":5}},"default":{"type":4}}"#,
        r#"{"column_id":3,"column_name":"sp","column_type":{"data_type":30,"type_info":{"data_type":5,"dimension":8,"index_type":3,"sort":0}},"default":{"type":4}}"#,
        r#"{"column_id":4,"column_name":"sc","column_type":{"data_type":7},"default":{"type":4}}"#,
        r#"{"column_id":5,"column_name":"b","column_type":{"data_type":0},"default":{"type":4}}"#,
    ];

    #[test]
    fn maps_the_port_catalog_records_to_a_table_def() {
        let cols: Vec<ColumnRecord> = RECORDS
            .iter()
            .map(|s| ColumnRecord::from_json(s).unwrap())
            .collect();
        let td = table_def_from_columns("default_db", "tt", &cols);
        assert_eq!(td.table_name, "tt");
        let kinds: Vec<_> = td
            .columns
            .iter()
            .map(|c| c.column_type.logical_type)
            .collect();
        assert_eq!(
            kinds,
            vec![
                LogicalType::Integer,
                LogicalType::Varchar,
                LogicalType::Embedding,
                LogicalType::Sparse,
                LogicalType::Float,
                LogicalType::Boolean,
            ]
        );
        assert_eq!(
            td.columns[2].column_type.type_info,
            TypeInfo::Embedding {
                element_type: EmbeddingDataType::Float,
                dimension: 4
            }
        );
        assert_eq!(
            td.columns[3].column_type.type_info,
            TypeInfo::Sparse {
                data_type: EmbeddingDataType::Float,
                index_type: EmbeddingDataType::Int32,
                dimension: 8,
                store_type: SparseStoreType::Sort
            }
        );
    }

    #[test]
    fn maps_the_port_index_record_to_an_index_base() {
        use infinity_storage::wal::index_def::IndexType;
        let text = r#"{"cardinality":"high","column_names":["name"],"file_name":"ix_sec","index_comment":"","index_name":"sec","index_type":"SECONDARY","is_json_index":false}"#;
        let d = index_def_from_json(text).unwrap();
        match d {
            infinity_storage::wal::index_def::IndexDef::Secondary { base, .. } => {
                assert_eq!(base.index_type, IndexType::Secondary);
                assert_eq!(base.index_name, "sec");
                assert_eq!(base.file_name, "ix_sec");
                assert_eq!(base.column_names, vec!["name".to_string()]);
            }
            other => panic!("expected Secondary, got {other:?}"),
        }
    }
}

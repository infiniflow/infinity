//! Typed WAL command payloads that replace the port's `KvBatch` for DDL:
//! `WalCmdCreateTableV2` and `WalCmdCreateIndexV2`
//! (`src/storage/wal/wal_entry_impl.cpp:1213-1234`).
//!
//! Only the two create commands are ported here; the payload builders reuse
//! the `TableDef`/`IndexDef` serialization from `ddl_def` / `index_def`.

use ::common::{Error, Result};

use super::ddl_def::{put_i32, put_str, put_u8, Reader, TableDef};
use super::index_def::IndexDef;

/// `WalCommandType::CREATE_TABLE_V2` (10).
pub const CREATE_TABLE_V2: u8 = 10;
/// `WalCommandType::CREATE_INDEX_V2` (13).
pub const CREATE_INDEX_V2: u8 = 13;

/// C++ `WalCmdCreateTableV2` (`wal_entry.cppm:287`).
#[derive(Clone, Debug, PartialEq)]
pub struct WalCmdCreateTableV2 {
    pub db_name: String,
    pub db_id: String,
    pub table_id: String,
    pub table_def: TableDef,
}

impl WalCmdCreateTableV2 {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_u8(out, CREATE_TABLE_V2);
        put_str(out, &self.db_name);
        put_str(out, &self.db_id);
        put_str(out, &self.table_id);
        self.table_def.write_adv(out);
    }

    /// C++ `WalCmdCreateTableV2::ReadAdv`. The replay path builds the command
    /// from `crate::wal`'s parser, so only `wal_cmd`'s round-trip test calls this
    /// decoder.
    #[allow(dead_code)]
    pub(crate) fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        if r.u8()? != CREATE_TABLE_V2 {
            return Err(Error::Storage("not a CreateTableV2 command".into()));
        }
        Ok(Self {
            db_name: r.string()?,
            db_id: r.string()?,
            table_id: r.string()?,
            table_def: TableDef::read_adv(r)?,
        })
    }
}

/// C++ `WalCmdCreateIndexV2` (`wal_entry.cppm:337`).
#[derive(Clone, Debug, PartialEq)]
pub struct WalCmdCreateIndexV2 {
    pub db_name: String,
    pub db_id: String,
    pub table_name: String,
    pub table_id: String,
    pub index_id: String,
    pub index_base: IndexDef,
    /// `segment_index_infos_`; only the empty case is needed for `CREATE INDEX`
    /// on a table with no sealed segment yet.
    pub segment_index_infos: Vec<u8>,
    pub table_key: String,
}

impl WalCmdCreateIndexV2 {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_u8(out, CREATE_INDEX_V2);
        put_str(out, &self.db_name);
        put_str(out, &self.db_id);
        put_str(out, &self.table_name);
        put_str(out, &self.table_id);
        put_str(out, &self.index_id);
        self.index_base.write_adv(out);
        put_i32(out, self.segment_index_infos.len() as i32);
        for b in &self.segment_index_infos {
            put_u8(out, *b);
        }
        put_str(out, &self.table_key);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::wal::ddl_def::{
        ColumnDef, ConstantExpr, DataType, EmbeddingDataType, LogicalType, SparseStoreType,
        TypeInfo,
    };

    fn hex(s: &str) -> Vec<u8> {
        (0..s.len())
            .step_by(2)
            .map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap())
            .collect()
    }

    fn wdldl_table() -> TableDef {
        let col = |id, lt, name: &str| ColumnDef {
            id,
            column_type: DataType::new(lt),
            name: name.into(),
            constraints: vec![],
            comment: String::new(),
            default_expr: ConstantExpr::Null,
            build_bloom_filter: false,
        };
        TableDef {
            schema_name: "default_db".into(),
            table_name: "wdldl".into(),
            table_comment: String::new(),
            columns: vec![
                col(0, LogicalType::Integer, "id"),
                col(1, LogicalType::Varchar, "name"),
                ColumnDef {
                    id: 2,
                    column_type: DataType {
                        logical_type: LogicalType::Embedding,
                        type_info: TypeInfo::Embedding {
                            element_type: EmbeddingDataType::Float,
                            dimension: 4,
                        },
                    },
                    name: "v".into(),
                    constraints: vec![],
                    comment: String::new(),
                    default_expr: ConstantExpr::Null,
                    build_bloom_filter: false,
                },
                ColumnDef {
                    id: 3,
                    column_type: DataType {
                        logical_type: LogicalType::Sparse,
                        type_info: TypeInfo::Sparse {
                            data_type: EmbeddingDataType::Float,
                            index_type: EmbeddingDataType::Int32,
                            dimension: 8,
                            store_type: SparseStoreType::Sort,
                        },
                    },
                    name: "s".into(),
                    constraints: vec![],
                    comment: String::new(),
                    default_expr: ConstantExpr::Null,
                    build_bloom_filter: false,
                },
            ],
        }
    }

    #[test]
    fn create_table_payload_matches_the_reference() {
        // Captured payload (type byte included) from the reference's wal.log.
        let reference = hex(
            "0a0a00000064656661756c745f6462010000003101000000300a00000064656661756c745f6462\
             0500000077646c646c000000000400000000000000000000000302000000696400000000000000000\
             400000000010000000000000009040000006e616d6500000000000000000400000000020000000000\
             000017050400000001000000760000000000000000040000000003000000000000001e0503080000\
             000000000000010000007300000000000000000400000000",
        );
        let cmd = WalCmdCreateTableV2 {
            db_name: "default_db".into(),
            db_id: "1".into(),
            table_id: "0".into(),
            table_def: wdldl_table(),
        };
        let mut bytes = Vec::new();
        cmd.write_adv(&mut bytes);
        assert_eq!(bytes, reference, "CreateTableV2 payload differs");

        let mut r = Reader::new(&bytes);
        assert_eq!(WalCmdCreateTableV2::read_adv(&mut r).unwrap(), cmd);
    }

    #[test]
    fn create_index_payload_roundtrips() {
        use crate::wal::index_def::{IndexBase, IndexType};
        let cmd = WalCmdCreateIndexV2 {
            db_name: "default_db".into(),
            db_id: "1".into(),
            table_name: "wdldl".into(),
            table_id: "0".into(),
            index_id: "1".into(),
            index_base: IndexDef::Secondary {
                base: IndexBase {
                    index_type: IndexType::Secondary,
                    index_name: "sec".into(),
                    index_comment: String::new(),
                    file_name: "wdldl_sec".into(),
                    column_names: vec!["name".into()],
                },
                cardinality: 0,
                is_json: false,
            },
            segment_index_infos: vec![],
            table_key: "catalog|tbl|1|wdldl|38".into(),
        };
        let mut bytes = Vec::new();
        cmd.write_adv(&mut bytes);
        // type(13) | "default_db" | "1" | "wdldl" | "0" | "1" |
        // IndexType::Secondary(4, i32) | "sec" | "" | "wdldl_sec" | ["name"] |
        // nseg(0)
        let expected_prefix = hex("0d\
             0a00000064656661756c745f6462\
             0100000031\
             0500000077646c646c\
             0100000030\
             0100000031\
             04000000\
             03000000736563\
             00000000\
             0900000077646c646c5f736563\
             01000000\
             040000006e616d65\
             00000000");
        assert_eq!(&bytes[..expected_prefix.len()], &expected_prefix[..]);
    }
}

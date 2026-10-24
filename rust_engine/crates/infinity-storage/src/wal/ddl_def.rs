//! Port of the DDL definition serialization the WAL's typed commands need:
//! `DataType`/`ColumnDef`/`TableDef` (`src/parser/type/data_type.cpp`,
//! `src/parser/definition/column_def.cpp`,
//! `src/storage/definition/table_def_impl.cpp`).
//!
//! Mechanical translation first: `write_adv`/`read_adv` mirror each C++
//! `WriteAdv`/`ReadAdv` field for field, with the same enum discriminants and
//! the same `[i32 len][bytes]` string framing (`WriteBufAdv`).

use infinity_common::{Error, Result};

// ---------------------------------------------------------------------------
// Enums (underlying types and discriminants copied from the C++ headers)
// ---------------------------------------------------------------------------

/// `LogicalType` (`parser/type/logical_type.h:43`, `int8_t`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum LogicalType {
    Boolean = 0,
    TinyInt = 1,
    SmallInt = 2,
    Integer = 3,
    BigInt = 4,
    HugeInt = 5,
    Decimal = 6,
    Float = 7,
    Double = 8,
    Varchar = 9,
    Date = 10,
    Time = 11,
    DateTime = 12,
    Timestamp = 13,
    Interval = 14,
    Array = 15,
    Tuple = 16,
    Point = 17,
    Line = 18,
    LineSeg = 19,
    Box = 20,
    Circle = 21,
    Uuid = 22,
    Embedding = 23,
    RowId = 24,
    Mixed = 25,
    Null = 26,
    Missing = 27,
    Tensor = 28,
    TensorArray = 29,
    Sparse = 30,
    EmptyArray = 31,
    Float16 = 32,
    BFloat16 = 33,
    MultiVector = 34,
    Json = 35,
    Invalid = 36,
}

impl LogicalType {
    /// Infallible discriminant -> variant (unknown -> `Invalid`), for callers
    /// outside this crate that rebuild a `DataType` from the catalog JSON.
    pub fn from_discriminant(v: i8) -> Self {
        Self::from_i8(v).unwrap_or(Self::Invalid)
    }
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Boolean,
            1 => Self::TinyInt,
            2 => Self::SmallInt,
            3 => Self::Integer,
            4 => Self::BigInt,
            5 => Self::HugeInt,
            6 => Self::Decimal,
            7 => Self::Float,
            8 => Self::Double,
            9 => Self::Varchar,
            10 => Self::Date,
            11 => Self::Time,
            12 => Self::DateTime,
            13 => Self::Timestamp,
            14 => Self::Interval,
            15 => Self::Array,
            16 => Self::Tuple,
            17 => Self::Point,
            18 => Self::Line,
            19 => Self::LineSeg,
            20 => Self::Box,
            21 => Self::Circle,
            22 => Self::Uuid,
            23 => Self::Embedding,
            24 => Self::RowId,
            25 => Self::Mixed,
            26 => Self::Null,
            27 => Self::Missing,
            28 => Self::Tensor,
            29 => Self::TensorArray,
            30 => Self::Sparse,
            31 => Self::EmptyArray,
            32 => Self::Float16,
            33 => Self::BFloat16,
            34 => Self::MultiVector,
            35 => Self::Json,
            36 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad LogicalType {v}"))),
        })
    }
}

/// `EmbeddingDataType` (`parser/type/complex/embedding_type.h:33`, `int8_t`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum EmbeddingDataType {
    Bit = 0,
    Int8 = 1,
    Int16 = 2,
    Int32 = 3,
    Int64 = 4,
    Float = 5,
    Double = 6,
    UInt8 = 7,
    Float16 = 8,
    BFloat16 = 9,
    Invalid = 10,
}

impl EmbeddingDataType {
    pub fn from_discriminant(v: i8) -> Self {
        Self::from_i8(v).unwrap_or(Self::Invalid)
    }
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Bit,
            1 => Self::Int8,
            2 => Self::Int16,
            3 => Self::Int32,
            4 => Self::Int64,
            5 => Self::Float,
            6 => Self::Double,
            7 => Self::UInt8,
            8 => Self::Float16,
            9 => Self::BFloat16,
            10 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad EmbeddingDataType {v}"))),
        })
    }
}

/// `SparseStoreType` (`parser/type/info/sparse_info.h:29`, `int8_t`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum SparseStoreType {
    Sort = 0,
    Sorted = 1,
    Invalid = 2,
}

impl SparseStoreType {
    pub fn from_discriminant(v: i8) -> Self {
        Self::from_i8(v).unwrap_or(Self::Invalid)
    }
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Sort,
            1 => Self::Sorted,
            2 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad SparseStoreType {v}"))),
        })
    }
}

/// `ConstraintType` (`parser/definition/column_def.h:37`, `char`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ConstraintType {
    Null = 0,
    NotNull = 1,
    PrimaryKey = 2,
    Unique = 3,
    Invalid = 4,
}

impl ConstraintType {
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Null,
            1 => Self::NotNull,
            2 => Self::PrimaryKey,
            3 => Self::Unique,
            4 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad ConstraintType {v}"))),
        })
    }
}

/// `LiteralType` (`parser/expr/constant_expr.h:34`, `int32_t`).
#[repr(i32)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum LiteralType {
    Boolean = 0,
    Double = 1,
    String = 2,
    Integer = 3,
    Null = 4,
    Date = 5,
    Time = 6,
    DateTime = 7,
    Timestamp = 8,
    IntegerArray = 9,
    DoubleArray = 10,
    SubArrayArray = 11,
    Interval = 12,
    LongSparseArray = 13,
    DoubleSparseArray = 14,
    EmptyArray = 15,
    CurlyBracketsArray = 16,
    Json = 17,
}

impl LiteralType {
    fn from_i32(v: i32) -> Result<Self> {
        Ok(match v {
            0 => Self::Boolean,
            1 => Self::Double,
            2 => Self::String,
            3 => Self::Integer,
            4 => Self::Null,
            5 => Self::Date,
            6 => Self::Time,
            7 => Self::DateTime,
            8 => Self::Timestamp,
            9 => Self::IntegerArray,
            10 => Self::DoubleArray,
            11 => Self::SubArrayArray,
            12 => Self::Interval,
            13 => Self::LongSparseArray,
            14 => Self::DoubleSparseArray,
            15 => Self::EmptyArray,
            16 => Self::CurlyBracketsArray,
            17 => Self::Json,
            _ => return Err(Error::Storage(format!("bad LiteralType {v}"))),
        })
    }
}

// ---------------------------------------------------------------------------
// Low-level `WriteBufAdv` / `ReadBufAdv` helpers
// ---------------------------------------------------------------------------

pub(crate) fn put_i32(out: &mut Vec<u8>, v: i32) {
    out.extend_from_slice(&v.to_le_bytes());
}
pub(crate) fn put_i64(out: &mut Vec<u8>, v: i64) {
    out.extend_from_slice(&v.to_le_bytes());
}
pub(crate) fn put_u8(out: &mut Vec<u8>, v: u8) {
    out.push(v);
}
pub(crate) fn put_u32(out: &mut Vec<u8>, v: u32) {
    out.extend_from_slice(&v.to_le_bytes());
}
pub(crate) fn put_f32(out: &mut Vec<u8>, v: f32) {
    put_u32(out, v.to_bits());
}
pub(crate) fn put_str(out: &mut Vec<u8>, s: &str) {
    put_i32(out, s.len() as i32);
    out.extend_from_slice(s.as_bytes());
}

pub(crate) struct Reader<'a> {
    buf: &'a [u8],
    pos: usize,
}

impl<'a> Reader<'a> {
    pub(crate) fn new(buf: &'a [u8]) -> Self {
        Self { buf, pos: 0 }
    }
    /// Bytes consumed so far (for advancing an upstream cursor).
    pub(crate) fn pos(&self) -> usize {
        self.pos
    }
    pub(crate) fn i32(&mut self) -> Result<i32> {
        let b = self.take(4)?;
        Ok(i32::from_le_bytes(b.try_into().unwrap()))
    }
    pub(crate) fn i64(&mut self) -> Result<i64> {
        let b = self.take(8)?;
        Ok(i64::from_le_bytes(b.try_into().unwrap()))
    }
    pub(crate) fn u8(&mut self) -> Result<u8> {
        let b = self.take(1)?;
        Ok(b[0])
    }
    pub(crate) fn u32(&mut self) -> Result<u32> {
        let b = self.take(4)?;
        Ok(u32::from_le_bytes(b.try_into().unwrap()))
    }
    pub(crate) fn f32(&mut self) -> Result<f32> {
        Ok(f32::from_bits(self.u32()?))
    }
    pub(crate) fn string(&mut self) -> Result<String> {
        let n = self.i32()? as usize;
        let b = self.take(n)?;
        String::from_utf8(b.to_vec()).map_err(|_| Error::Storage("bad utf8 in WAL string".into()))
    }
    pub(crate) fn take(&mut self, n: usize) -> Result<&'a [u8]> {
        let end = self.pos + n;
        if end > self.buf.len() {
            return Err(Error::Storage("WAL DDL payload truncated".into()));
        }
        let s = &self.buf[self.pos..end];
        self.pos = end;
        Ok(s)
    }
}

// ---------------------------------------------------------------------------
// TypeInfo / DataType
// ---------------------------------------------------------------------------

/// The per-type payload `DataType::WriteAdv` appends after the `LogicalType`.
#[derive(Clone, Debug, PartialEq)]
pub enum TypeInfo {
    None,
    Array(Box<DataType>),
    Decimal {
        precision: i64,
        scale: i64,
    },
    Embedding {
        element_type: EmbeddingDataType,
        dimension: i32,
    },
    Sparse {
        data_type: EmbeddingDataType,
        index_type: EmbeddingDataType,
        dimension: i64,
        store_type: SparseStoreType,
    },
}

/// `DataType` (`parser/type/data_type.cpp:320`).
#[derive(Clone, Debug, PartialEq)]
pub struct DataType {
    pub logical_type: LogicalType,
    pub type_info: TypeInfo,
}

impl DataType {
    pub fn new(logical_type: LogicalType) -> Self {
        Self {
            logical_type,
            type_info: TypeInfo::None,
        }
    }

    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_u8(out, self.logical_type as i8 as u8);
        match self.logical_type {
            LogicalType::Array => {
                if let TypeInfo::Array(elem) = &self.type_info {
                    elem.write_adv(out);
                }
            }
            LogicalType::Decimal => {
                let (precision, scale) = match &self.type_info {
                    TypeInfo::Decimal { precision, scale } => (*precision, *scale),
                    _ => (0, 0),
                };
                put_i64(out, precision);
                put_i64(out, scale);
            }
            LogicalType::Tensor
            | LogicalType::TensorArray
            | LogicalType::MultiVector
            | LogicalType::Embedding => {
                if let TypeInfo::Embedding {
                    element_type,
                    dimension,
                } = &self.type_info
                {
                    put_u8(out, *element_type as i8 as u8);
                    put_i32(out, *dimension);
                }
            }
            LogicalType::Sparse => {
                if let TypeInfo::Sparse {
                    data_type,
                    index_type,
                    dimension,
                    store_type,
                } = &self.type_info
                {
                    put_u8(out, *data_type as i8 as u8);
                    put_u8(out, *index_type as i8 as u8);
                    put_i64(out, *dimension);
                    put_u8(out, *store_type as i8 as u8);
                }
            }
            _ => {}
        }
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let logical_type = LogicalType::from_i8(r.u8()? as i8)?;
        let type_info = match logical_type {
            LogicalType::Array => TypeInfo::Array(Box::new(Self::read_adv(r)?)),
            LogicalType::Decimal => TypeInfo::Decimal {
                precision: r.i64()?,
                scale: r.i64()?,
            },
            LogicalType::Tensor
            | LogicalType::TensorArray
            | LogicalType::MultiVector
            | LogicalType::Embedding => TypeInfo::Embedding {
                element_type: EmbeddingDataType::from_i8(r.u8()? as i8)?,
                dimension: r.i32()?,
            },
            LogicalType::Sparse => TypeInfo::Sparse {
                data_type: EmbeddingDataType::from_i8(r.u8()? as i8)?,
                index_type: EmbeddingDataType::from_i8(r.u8()? as i8)?,
                dimension: r.i64()?,
                store_type: SparseStoreType::from_i8(r.u8()? as i8)?,
            },
            _ => TypeInfo::None,
        };
        Ok(Self {
            logical_type,
            type_info,
        })
    }
}

// ---------------------------------------------------------------------------
// ConstantExpr (only the literals a column default can be)
// ---------------------------------------------------------------------------

/// `ConstantExpr` (`parser/expr/constant_expr.cpp:247`).
#[derive(Clone, Debug, PartialEq)]
pub enum ConstantExpr {
    Null,
    Boolean(bool),
    Double(f64),
    String(String),
    Integer(i64),
}

impl ConstantExpr {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        match self {
            ConstantExpr::Null => put_i32(out, LiteralType::Null as i32),
            ConstantExpr::Boolean(b) => {
                put_i32(out, LiteralType::Boolean as i32);
                put_u8(out, if *b { 1 } else { 0 });
            }
            ConstantExpr::Double(d) => {
                put_i32(out, LiteralType::Double as i32);
                out.extend_from_slice(&d.to_le_bytes());
            }
            ConstantExpr::String(s) => {
                put_i32(out, LiteralType::String as i32);
                put_str(out, s);
            }
            ConstantExpr::Integer(i) => {
                put_i32(out, LiteralType::Integer as i32);
                put_i64(out, *i);
            }
        }
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let literal_type = LiteralType::from_i32(r.i32()?)?;
        Ok(match literal_type {
            LiteralType::Null | LiteralType::EmptyArray | LiteralType::CurlyBracketsArray => {
                ConstantExpr::Null
            }
            LiteralType::Boolean => ConstantExpr::Boolean(r.u8()? != 0),
            LiteralType::Double => {
                let b = r.take(8)?;
                ConstantExpr::Double(f64::from_le_bytes(b.try_into().unwrap()))
            }
            LiteralType::String => ConstantExpr::String(r.string()?),
            LiteralType::Integer => ConstantExpr::Integer(r.i64()?),
            other => {
                return Err(Error::Storage(format!(
                    "unsupported ConstantExpr literal type {other:?}"
                )))
            }
        })
    }
}

// ---------------------------------------------------------------------------
// ColumnDef / TableDef
// ---------------------------------------------------------------------------

/// `ColumnDef` (`parser/definition/column_def.cpp:182`).
#[derive(Clone, Debug, PartialEq)]
pub struct ColumnDef {
    pub id: i64,
    pub column_type: DataType,
    pub name: String,
    pub constraints: Vec<ConstraintType>,
    pub comment: String,
    pub default_expr: ConstantExpr,
    pub build_bloom_filter: bool,
}

impl ColumnDef {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_i64(out, self.id);
        self.column_type.write_adv(out);
        put_str(out, &self.name);
        put_i32(out, self.constraints.len() as i32);
        for c in &self.constraints {
            put_u8(out, *c as i8 as u8);
        }
        put_str(out, &self.comment);
        self.default_expr.write_adv(out);
        put_u8(out, if self.build_bloom_filter { 1 } else { 0 });
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let id = r.i64()?;
        let column_type = DataType::read_adv(r)?;
        let name = r.string()?;
        let ncons = r.i32()?;
        let mut constraints = Vec::with_capacity(ncons.max(0) as usize);
        for _ in 0..ncons {
            constraints.push(ConstraintType::from_i8(r.u8()? as i8)?);
        }
        let comment = r.string()?;
        let default_expr = ConstantExpr::read_adv(r)?;
        let build_bloom_filter = r.u8()? == 1;
        Ok(Self {
            id,
            column_type,
            name,
            constraints,
            comment,
            default_expr,
            build_bloom_filter,
        })
    }
}

/// `TableDef` (`storage/definition/table_def_impl.cpp:92`).
#[derive(Clone, Debug, PartialEq)]
pub struct TableDef {
    pub schema_name: String,
    pub table_name: String,
    pub table_comment: String,
    pub columns: Vec<ColumnDef>,
}

impl TableDef {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_str(out, &self.schema_name);
        put_str(out, &self.table_name);
        put_str(out, &self.table_comment);
        put_i32(out, self.columns.len() as i32);
        for cd in &self.columns {
            cd.write_adv(out);
        }
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let schema_name = r.string()?;
        let table_name = r.string()?;
        let table_comment = r.string()?;
        let ncols = r.i32()?;
        let mut columns = Vec::with_capacity(ncols.max(0) as usize);
        for _ in 0..ncols {
            columns.push(ColumnDef::read_adv(r)?);
        }
        Ok(Self {
            schema_name,
            table_name,
            table_comment,
            columns,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn table() -> TableDef {
        TableDef {
            schema_name: "default_db".into(),
            table_name: "wdldl".into(),
            table_comment: String::new(),
            columns: vec![
                ColumnDef {
                    id: 0,
                    column_type: DataType::new(LogicalType::Integer),
                    name: "id".into(),
                    constraints: vec![],
                    comment: String::new(),
                    default_expr: ConstantExpr::Null,
                    build_bloom_filter: false,
                },
                ColumnDef {
                    id: 1,
                    column_type: DataType::new(LogicalType::Varchar),
                    name: "name".into(),
                    constraints: vec![],
                    comment: String::new(),
                    default_expr: ConstantExpr::Null,
                    build_bloom_filter: false,
                },
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
    fn roundtrips_every_column_type() {
        let t = table();
        let mut bytes = Vec::new();
        t.write_adv(&mut bytes);
        let mut r = Reader::new(&bytes);
        assert_eq!(TableDef::read_adv(&mut r).unwrap(), t);
    }

    #[test]
    fn header_prefix_matches_the_captured_reference() {
        // `db_name/db_id/table_id` are written by `WalCmdCreateTableV2` before
        // the `TableDef`; the TableDef itself starts with the schema name.
        let t = table();
        let mut bytes = Vec::new();
        t.write_adv(&mut bytes);
        // schema "default_db" -> 0a000000 64656661756c745f6462
        assert_eq!(&bytes[..14], &hex("0a00000064656661756c745f6462")[..]);
        // then "wdldl" -> 050000 00 77646c646c
        assert_eq!(&bytes[14..23], &hex("0500000077646c646c")[..]);
        // empty table comment + 4 columns
        assert_eq!(&bytes[23..31], &hex("0000000004000000")[..]);
    }

    #[test]
    fn whole_table_def_matches_the_reference_bytes() {
        // Captured from the reference's `WalCmdCreateTableV2`
        // (`wal.log`, CREATE TABLE wdldl(id int, name varchar,
        //  v vector,4,float, s sparse,8,float,int)). The command payload is
        // [db_name][db_id][table_id][TableDef]; this decodes the TableDef with
        // our reader and re-encodes it, asserting the exact bytes back.
        let payload = hex(
            "0a00000064656661756c745f6462010000003101000000300a00000064656661756c745f6462\
             0500000077646c646c000000000400000000000000000000000302000000696400000000000000000\
             400000000010000000000000009040000006e616d6500000000000000000400000000020000000000\
             000017050400000001000000760000000000000000040000000003000000000000001e0503080000\
             000000000000010000007300000000000000000400000000",
        );
        let mut r = Reader::new(&payload);
        let _db = r.string().unwrap();
        let _db_id = r.string().unwrap();
        let _table_id = r.string().unwrap();
        let tstart = r.pos;
        let td = TableDef::read_adv(&mut r).unwrap();
        let consumed = &payload[tstart..r.pos];
        let mut re = Vec::new();
        td.write_adv(&mut re);
        assert_eq!(re, consumed, "the TableDef bytes differ from the reference");

        // Sanity: the decoded table matches what the schema says.
        assert_eq!(td.table_name, "wdldl");
        let names: Vec<_> = td.columns.iter().map(|c| c.name.as_str()).collect();
        assert_eq!(names, vec!["id", "name", "v", "s"]);
        assert_eq!(td.columns[0].column_type.logical_type, LogicalType::Integer);
        assert_eq!(td.columns[1].column_type.logical_type, LogicalType::Varchar);
        assert_eq!(
            td.columns[2].column_type.logical_type,
            LogicalType::Embedding
        );
        assert_eq!(td.columns[3].column_type.logical_type, LogicalType::Sparse);
    }

    fn hex(s: &str) -> Vec<u8> {
        (0..s.len())
            .step_by(2)
            .map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap())
            .collect()
    }
}

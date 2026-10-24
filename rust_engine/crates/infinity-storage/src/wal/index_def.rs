//! Port of the index-definition serialization (`IndexBase` and its subclasses)
//! that the WAL's `CreateIndexV2` carries.
//!
//! Sources: `src/storage/definition/index_base_impl.cpp:93`, and the
//! per-subclass `WriteAdv` in `index_{secondary,full_text,hnsw,ivf,bmp}_impl.cpp`.
//! Mechanical translation first: same field order, same enum widths.

use infinity_common::{Error, Result};

use super::ddl_def::{put_i32, put_i64, put_str, put_u8, Reader};

/// `IndexType` (`parser/statement/extra/create_index_info.h:30`). No explicit
/// underlying type in C++, so it is an `int` (4 bytes) on the wire.
#[repr(i32)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum IndexType {
    Ivf = 0,
    Hnsw = 1,
    Bmp = 2,
    FullText = 3,
    Secondary = 4,
    SecondaryFunctional = 5,
    Emvb = 6,
    Plaid = 7,
    Smve = 8,
    Invalid = 9,
    DiskAnn = 10,
}

impl IndexType {
    fn from_i32(v: i32) -> Result<Self> {
        Ok(match v {
            0 => Self::Ivf,
            1 => Self::Hnsw,
            2 => Self::Bmp,
            3 => Self::FullText,
            4 => Self::Secondary,
            5 => Self::SecondaryFunctional,
            6 => Self::Emvb,
            7 => Self::Plaid,
            8 => Self::Smve,
            9 => Self::Invalid,
            10 => Self::DiskAnn,
            _ => return Err(Error::Storage(format!("bad IndexType {v}"))),
        })
    }
}

/// `MetricType` (`storage/definition/index_base.cppm:32`, `int8_t`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum MetricType {
    Cosine = 0,
    InnerProduct = 1,
    L2 = 2,
    Invalid = 3,
}

impl MetricType {
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Cosine,
            1 => Self::InnerProduct,
            2 => Self::L2,
            3 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad MetricType {v}"))),
        })
    }
}

/// `HnswEncodeType` (`storage/definition/index_hnsw.cppm:28`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum HnswEncodeType {
    Plain = 0,
    Lvq = 1,
    Rabitq = 2,
    Invalid = 3,
}

impl HnswEncodeType {
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Plain,
            1 => Self::Lvq,
            2 => Self::Rabitq,
            3 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad HnswEncodeType {v}"))),
        })
    }
}

/// `HnswBuildType` (`storage/definition/index_hnsw.cppm:39`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum HnswBuildType {
    Plain = 0,
    Lsg = 1,
    Invalid = 2,
}

impl HnswBuildType {
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Plain,
            1 => Self::Lsg,
            2 => Self::Invalid,
            _ => return Err(Error::Storage(format!("bad HnswBuildType {v}"))),
        })
    }
}

/// The base every index shares (`IndexBase::WriteAdv`).
#[derive(Clone, Debug, PartialEq)]
pub struct IndexBase {
    pub index_type: IndexType,
    pub index_name: String,
    pub index_comment: String,
    pub file_name: String,
    pub column_names: Vec<String>,
}

impl IndexBase {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        put_i32(out, self.index_type as i32);
        put_str(out, &self.index_name);
        put_str(out, &self.index_comment);
        put_str(out, &self.file_name);
        put_i32(out, self.column_names.len() as i32);
        for c in &self.column_names {
            put_str(out, c);
        }
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let index_type = IndexType::from_i32(r.i32()?)?;
        let index_name = r.string()?;
        let index_comment = r.string()?;
        let file_name = r.string()?;
        let n = r.i32()?;
        let mut column_names = Vec::with_capacity(n.max(0) as usize);
        for _ in 0..n {
            column_names.push(r.string()?);
        }
        Ok(Self {
            index_type,
            index_name,
            index_comment,
            file_name,
            column_names,
        })
    }
}

/// A full index definition: the base plus the subclass's own fields.
#[derive(Clone, Debug, PartialEq)]
pub enum IndexDef {
    Secondary {
        base: IndexBase,
        /// `u8(secondary_index_cardinality_)`.
        cardinality: u8,
        /// `u8(is_json_index_)`.
        is_json: bool,
    },
    FullText {
        base: IndexBase,
        analyzer: String,
        /// `u8(flag_)`.
        flag: u8,
    },
    Bmp {
        base: IndexBase,
        /// `size_t block_size_`.
        block_size: u64,
        /// `static_cast<i8>(compress_type_)`.
        compress_type: i8,
    },
    Hnsw {
        base: IndexBase,
        metric_type: MetricType,
        encode_type: HnswEncodeType,
        build_type: HnswBuildType,
        m: u64,
        ef_construction: u64,
        block_size: u64,
        /// `lsg_config_` presence; the config body itself is not ported yet.
        has_lsg: bool,
    },
}

impl IndexDef {
    pub fn write_adv(&self, out: &mut Vec<u8>) {
        match self {
            IndexDef::Secondary {
                base,
                cardinality,
                is_json,
            } => {
                base.write_adv(out);
                put_u8(out, *cardinality);
                put_u8(out, if *is_json { 1 } else { 0 });
            }
            IndexDef::FullText {
                base,
                analyzer,
                flag,
            } => {
                base.write_adv(out);
                put_str(out, analyzer);
                put_u8(out, *flag);
            }
            IndexDef::Bmp {
                base,
                block_size,
                compress_type,
            } => {
                base.write_adv(out);
                put_i64(out, *block_size as i64);
                put_u8(out, *compress_type as u8);
            }
            IndexDef::Hnsw {
                base,
                metric_type,
                encode_type,
                build_type,
                m,
                ef_construction,
                block_size,
                has_lsg,
            } => {
                base.write_adv(out);
                put_u8(out, *metric_type as i8 as u8);
                // `HnswEncodeType`/`HnswBuildType` have no explicit underlying
                // type in C++, so they are `int` (4 bytes) on the wire.
                put_i32(out, *encode_type as i8 as i32);
                put_i32(out, *build_type as i8 as i32);
                put_i64(out, *m as i64);
                put_i64(out, *ef_construction as i64);
                put_i64(out, *block_size as i64);
                put_u8(out, if *has_lsg { 1 } else { 0 });
            }
        }
    }

    pub fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
        let base = IndexBase::read_adv(r)?;
        Ok(match base.index_type {
            IndexType::Secondary => IndexDef::Secondary {
                base,
                cardinality: r.u8()?,
                is_json: r.u8()? != 0,
            },
            IndexType::SecondaryFunctional => IndexDef::Secondary {
                base,
                cardinality: r.u8()?,
                is_json: r.u8()? != 0,
            },
            IndexType::FullText => IndexDef::FullText {
                base,
                analyzer: r.string()?,
                flag: r.u8()?,
            },
            IndexType::Bmp => IndexDef::Bmp {
                base,
                block_size: r.i64()? as u64,
                compress_type: r.u8()? as i8,
            },
            IndexType::Hnsw => {
                let metric_type = MetricType::from_i8(r.u8()? as i8)?;
                let encode_type = HnswEncodeType::from_i8(r.i32()? as i8)?;
                let build_type = HnswBuildType::from_i8(r.i32()? as i8)?;
                let m = r.i64()? as u64;
                let ef_construction = r.i64()? as u64;
                let block_size = r.i64()? as u64;
                let has_lsg = r.u8()? != 0;
                if has_lsg {
                    return Err(Error::Storage(
                        "HNSW LSG config body is not ported yet".into(),
                    ));
                }
                IndexDef::Hnsw {
                    base,
                    metric_type,
                    encode_type,
                    build_type,
                    m,
                    ef_construction,
                    block_size,
                    has_lsg,
                }
            }
            other => {
                return Err(Error::Storage(format!(
                    "index type {other:?} is not ported yet"
                )))
            }
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn base(t: IndexType, name: &str, col: &str) -> IndexBase {
        IndexBase {
            index_type: t,
            index_name: name.into(),
            index_comment: String::new(),
            file_name: format!("{name}_file"),
            column_names: vec![col.into()],
        }
    }

    fn roundtrip(def: IndexDef) {
        let mut bytes = Vec::new();
        def.write_adv(&mut bytes);
        let mut r = Reader::new(&bytes);
        assert_eq!(IndexDef::read_adv(&mut r).unwrap(), def);
    }

    #[test]
    fn secondary_is_the_base() {
        roundtrip(IndexDef::Secondary {
            base: base(IndexType::Secondary, "sec", "title"),
            cardinality: 0,
            is_json: false,
        });
    }

    #[test]
    fn fulltext_roundtrips() {
        roundtrip(IndexDef::FullText {
            base: base(IndexType::FullText, "ft", "body"),
            analyzer: "standard".into(),
            flag: 1,
        });
    }

    #[test]
    fn bmp_roundtrips() {
        roundtrip(IndexDef::Bmp {
            base: base(IndexType::Bmp, "sp_idx", "sp"),
            block_size: 16,
            compress_type: 0,
        });
    }

    #[test]
    fn hnsw_roundtrips() {
        roundtrip(IndexDef::Hnsw {
            base: base(IndexType::Hnsw, "vec_idx", "vec"),
            metric_type: MetricType::L2,
            encode_type: HnswEncodeType::Plain,
            build_type: HnswBuildType::Plain,
            m: 16,
            ef_construction: 20,
            block_size: 16,
            has_lsg: false,
        });
    }
}

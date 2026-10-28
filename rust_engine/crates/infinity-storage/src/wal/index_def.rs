//! Port of the index-definition serialization (`IndexBase` and its subclasses)
//! that the WAL's `CreateIndexV2` carries.
//!
//! Sources: `src/storage/definition/index_base_impl.cpp:93`, and the
//! per-subclass `WriteAdv` in `index_{secondary,full_text,hnsw,ivf,bmp}_impl.cpp`.
//! Mechanical translation first: same field order, same enum widths.

use infinity_common::{Error, Result};

use super::ddl_def::{
    put_f32, put_i32, put_i64, put_str, put_u32, put_u8, EmbeddingDataType, Reader,
};

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

/// `IndexIVFStorageOption::Type` (`storage/definition/index_ivf.cppm:39`, `i8`).
#[repr(i8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum IvfStorageType {
    Plain = 0,
    ScalarQuantization = 1,
    ProductQuantization = 2,
}

impl IvfStorageType {
    fn from_i8(v: i8) -> Result<Self> {
        Ok(match v {
            0 => Self::Plain,
            1 => Self::ScalarQuantization,
            2 => Self::ProductQuantization,
            _ => return Err(Error::Storage(format!("bad IVF storage type {v}"))),
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

    pub(crate) fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
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
    /// `IndexEMVB::WriteAdv` (`index_emvb_impl.cpp:116`): two `u32`s.
    Emvb {
        base: IndexBase,
        residual_pq_subspace_num: u32,
        residual_pq_subspace_bits: u32,
    },
    /// `IndexPLAID::WriteAdv` (`index_plaid_impl.cpp:120`): two `u32`s and a
    /// `bool` (one byte).
    Plaid {
        base: IndexBase,
        nbits: u32,
        n_centroids: u32,
        colbertsar_mode: bool,
    },
    /// `IndexSMVE::WriteAdv` (`index_smve_impl.cpp:81`): two `u32`s.
    Smve {
        base: IndexBase,
        width: u32,
        topk: u32,
    },
    /// `IndexIVF::WriteAdv` copies `IndexIVFOption` as one blob
    /// (`storage/definition/index_ivf_impl.cpp:48`). The struct is 32 bytes
    /// with five padding bytes (after the `i8` metric and after the two `i8`s
    /// of the storage option); the reference zeroes them at the copy, so the
    /// blob is deterministic and the port writes the same zeros.
    Ivf {
        base: IndexBase,
        metric_type: MetricType,
        centroids_num_ratio: f32,
        min_points_per_centroid: u32,
        max_points_per_centroid: u32,
        storage_type: IvfStorageType,
        plain_storage_data_type: EmbeddingDataType,
        scalar_quantization_bits: u32,
        product_quantization_subspace_num: u32,
        product_quantization_subspace_bits: u32,
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
            IndexDef::Emvb {
                base,
                residual_pq_subspace_num,
                residual_pq_subspace_bits,
            } => {
                base.write_adv(out);
                put_u32(out, *residual_pq_subspace_num);
                put_u32(out, *residual_pq_subspace_bits);
            }
            IndexDef::Plaid {
                base,
                nbits,
                n_centroids,
                colbertsar_mode,
            } => {
                base.write_adv(out);
                put_u32(out, *nbits);
                put_u32(out, *n_centroids);
                put_u8(out, if *colbertsar_mode { 1 } else { 0 });
            }
            IndexDef::Smve { base, width, topk } => {
                base.write_adv(out);
                put_u32(out, *width);
                put_u32(out, *topk);
            }
            IndexDef::Ivf {
                base,
                metric_type,
                centroids_num_ratio,
                min_points_per_centroid,
                max_points_per_centroid,
                storage_type,
                plain_storage_data_type,
                scalar_quantization_bits,
                product_quantization_subspace_num,
                product_quantization_subspace_bits,
            } => {
                base.write_adv(out);
                // `sizeof(IndexIVFOption)` = 32: metric (i8) then three padding
                // bytes, the centroid block (f32,u32,u32), the storage type and
                // element type (i8 each), two padding bytes, three u32s.
                put_u8(out, *metric_type as i8 as u8);
                out.extend_from_slice(&[0, 0, 0]);
                put_f32(out, *centroids_num_ratio);
                put_u32(out, *min_points_per_centroid);
                put_u32(out, *max_points_per_centroid);
                put_u8(out, *storage_type as i8 as u8);
                put_u8(out, *plain_storage_data_type as i8 as u8);
                out.extend_from_slice(&[0, 0]);
                put_u32(out, *scalar_quantization_bits);
                put_u32(out, *product_quantization_subspace_num);
                put_u32(out, *product_quantization_subspace_bits);
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

    pub(crate) fn read_adv(r: &mut Reader<'_>) -> Result<Self> {
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
            IndexType::Emvb => IndexDef::Emvb {
                base,
                residual_pq_subspace_num: r.u32()?,
                residual_pq_subspace_bits: r.u32()?,
            },
            IndexType::Plaid => IndexDef::Plaid {
                base,
                nbits: r.u32()?,
                n_centroids: r.u32()?,
                colbertsar_mode: r.u8()? != 0,
            },
            IndexType::Smve => IndexDef::Smve {
                base,
                width: r.u32()?,
                topk: r.u32()?,
            },
            IndexType::Ivf => {
                let metric_type = MetricType::from_i8(r.u8()? as i8)?;
                let _padding = r.take(3)?;
                let centroids_num_ratio = r.f32()?;
                let min_points_per_centroid = r.u32()?;
                let max_points_per_centroid = r.u32()?;
                let storage_type = IvfStorageType::from_i8(r.u8()? as i8)?;
                let plain_storage_data_type = EmbeddingDataType::from_discriminant(r.u8()? as i8);
                let _padding = r.take(2)?;
                let scalar_quantization_bits = r.u32()?;
                let product_quantization_subspace_num = r.u32()?;
                let product_quantization_subspace_bits = r.u32()?;
                IndexDef::Ivf {
                    base,
                    metric_type,
                    centroids_num_ratio,
                    min_points_per_centroid,
                    max_points_per_centroid,
                    storage_type,
                    plain_storage_data_type,
                    scalar_quantization_bits,
                    product_quantization_subspace_num,
                    product_quantization_subspace_bits,
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
    fn emvb_plaid_smve_roundtrip() {
        roundtrip(IndexDef::Emvb {
            base: base(IndexType::Emvb, "e", "c"),
            residual_pq_subspace_num: 2,
            residual_pq_subspace_bits: 8,
        });
        roundtrip(IndexDef::Plaid {
            base: base(IndexType::Plaid, "p", "c"),
            nbits: 8,
            n_centroids: 4,
            colbertsar_mode: true,
        });
        roundtrip(IndexDef::Smve {
            base: base(IndexType::Smve, "s", "c"),
            width: 2048,
            topk: 8,
        });
    }

    /// The IVF option is a 32-byte blob with five zero padding bytes; the tail
    /// must be zeros, not whatever the struct happened to hold.
    #[test]
    fn ivf_option_is_the_reference_blob() {
        let def = IndexDef::Ivf {
            base: base(IndexType::Ivf, "ivf_idx", "vec"),
            metric_type: MetricType::L2,
            centroids_num_ratio: 1.0,
            min_points_per_centroid: 32,
            max_points_per_centroid: 256,
            storage_type: IvfStorageType::Plain,
            plain_storage_data_type: EmbeddingDataType::Float,
            scalar_quantization_bits: 0,
            product_quantization_subspace_num: 0,
            product_quantization_subspace_bits: 0,
        };
        let mut bytes = Vec::new();
        def.write_adv(&mut bytes);
        let option = &bytes[bytes.len() - 32..];
        assert_eq!(option.len(), 32);
        assert_eq!(option[0], MetricType::L2 as i8 as u8);
        assert_eq!(&option[1..4], &[0, 0, 0]);
        assert_eq!(&option[16..18], &[0, 5]); // plain + float
        assert_eq!(&option[18..20], &[0, 0]);
        roundtrip(def);
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

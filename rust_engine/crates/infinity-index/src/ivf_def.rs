//! `IndexIVF` definition: parameter parsing and column validation.
//!
//! Port of `src/storage/definition/index_ivf_impl.cpp` (`IndexIVF::Make`,
//! `CheckIndexIVFCentroidOption`, `CheckIndexIVFStorageOption` and
//! `IndexIVF::ValidateColumnDataType`). The reference validates an index
//! definition against the target column before the index is created; the
//! checks (and their accepted value sets) are reproduced here.

use infinity_common::{Error, Result};

/// `MetricType` as far as the IVF definition cares.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Metric {
    #[default]
    Invalid,
    L2,
    Ip,
    Cos,
}

impl Metric {
    /// C++ `StringToMetricType` (`index_base_impl.cpp:56`); the caller has
    /// already lowercased the value.
    pub fn parse(text: &str) -> Self {
        match text {
            "cos" | "cosine" => Metric::Cos,
            "ip" => Metric::Ip,
            "l2" => Metric::L2,
            _ => Metric::Invalid,
        }
    }

    pub fn name(self) -> &'static str {
        match self {
            Metric::Cos => "cos",
            Metric::Ip => "ip",
            Metric::L2 => "l2",
            Metric::Invalid => "Invalid",
        }
    }
}

/// `EmbeddingDataType` (`embedding_type.h`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum EmbeddingDataType {
    #[default]
    Invalid,
    Bit,
    Int8,
    UInt8,
    Int16,
    Int32,
    Int64,
    Float,
    Double,
    Float16,
    BFloat16,
}

impl EmbeddingDataType {
    /// C++ `EmbeddingT::String2EmbeddingDataType`; the caller has already
    /// uppercased the value. Unknown names raise in the reference.
    pub fn parse(text: &str) -> Option<Self> {
        Some(match text {
            "BIT" => EmbeddingDataType::Bit,
            "INT8" | "I8" => EmbeddingDataType::Int8,
            "INT16" | "I16" => EmbeddingDataType::Int16,
            "INT32" | "I32" | "INT" | "INTEGER" => EmbeddingDataType::Int32,
            "INT64" => EmbeddingDataType::Int64,
            "FLOAT32" | "FLOAT" | "F32" => EmbeddingDataType::Float,
            "FLOAT64" | "DOUBLE" | "F64" => EmbeddingDataType::Double,
            "UINT8" | "U8" => EmbeddingDataType::UInt8,
            "FLOAT16" | "F16" => EmbeddingDataType::Float16,
            "BFLOAT16" | "BF16" => EmbeddingDataType::BFloat16,
            _ => return None,
        })
    }

    /// C++ `EmbeddingDataType2String`.
    pub fn name(self) -> &'static str {
        match self {
            EmbeddingDataType::Bit => "BIT",
            EmbeddingDataType::Int8 => "INT8",
            EmbeddingDataType::Int16 => "INT16",
            EmbeddingDataType::Int32 => "INT32",
            EmbeddingDataType::Int64 => "INT64",
            EmbeddingDataType::Float => "FLOAT32",
            EmbeddingDataType::Double => "FLOAT64",
            EmbeddingDataType::UInt8 => "UINT8",
            EmbeddingDataType::Float16 => "FLOAT16",
            EmbeddingDataType::BFloat16 => "BFLOAT16",
            EmbeddingDataType::Invalid => "INVALID",
        }
    }

    fn is_floating(self) -> bool {
        matches!(
            self,
            EmbeddingDataType::Float
                | EmbeddingDataType::Double
                | EmbeddingDataType::Float16
                | EmbeddingDataType::BFloat16
        )
    }
}

/// The target column's type, as needed by `ValidateColumnDataType`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ColumnType {
    /// `LogicalType::kEmbedding` with the element type and dimension.
    Embedding(EmbeddingDataType, usize),
    /// `LogicalType::kMultiVector` (accepted without a storage check).
    MultiVector,
    /// Any other logical type.
    Other,
}

impl ColumnType {
    fn describe(self) -> String {
        match self {
            ColumnType::Embedding(dt, dim) => format!("Embedding[{}:{}]", dt.name(), dim),
            ColumnType::MultiVector => "MultiVector".to_string(),
            ColumnType::Other => "Other".to_string(),
        }
    }
}

/// One named column of the table an index is created on.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct Column {
    pub name: String,
    pub column_type: ColumnType,
}

impl Column {
    pub fn embedding(name: &str, data_type: EmbeddingDataType, dimension: usize) -> Self {
        Self {
            name: name.to_string(),
            column_type: ColumnType::Embedding(data_type, dimension),
        }
    }

    pub fn other(name: &str) -> Self {
        Self {
            name: name.to_string(),
            column_type: ColumnType::Other,
        }
    }
}

/// `IndexIVFCentroidOption`.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct CentroidOption {
    /// `centroid_num = ratio * sqrt(embedding_num)`.
    pub centroids_num_ratio: f32,
    pub min_points_per_centroid: u32,
    pub max_points_per_centroid: u32,
}

impl Default for CentroidOption {
    fn default() -> Self {
        Self {
            centroids_num_ratio: 1.0,
            min_points_per_centroid: 32,
            max_points_per_centroid: 256,
        }
    }
}

/// `IndexIVFStorageOption::Type`.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum StorageType {
    #[default]
    Plain,
    ScalarQuantization,
    ProductQuantization,
}

/// `IndexIVFStorageOption`.
#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct StorageOption {
    pub storage_type: StorageType,
    /// `kPlain`: float32/float16/bfloat16/int8/uint8.
    pub plain_storage_data_type: EmbeddingDataType,
    /// `kScalarQuantization`: 4 or 8.
    pub scalar_quantization_bits: u32,
    /// `kProductQuantization`: a divisor of the embedding dimension.
    pub product_quantization_subspace_num: u32,
    /// `kProductQuantization`: in `[4, 16]`.
    pub product_quantization_subspace_bits: u32,
}

/// `IndexIVFOption`.
#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct IvfOption {
    pub metric: Metric,
    pub centroid_option: CentroidOption,
    pub storage_option: StorageOption,
}

/// An `IndexIVF` definition.
#[derive(Clone, Debug, PartialEq)]
pub struct IndexIvf {
    pub index_name: String,
    pub index_comment: String,
    pub file_name: String,
    pub column_names: Vec<String>,
    pub option: IvfOption,
}

fn invalid(message: impl Into<String>) -> Error {
    Error::Invalid(message.into())
}

fn parse_u32(name: &str, value: &str) -> Result<u32> {
    let value: i64 = value
        .trim()
        .parse()
        .map_err(|_| invalid(format!("invalid value for '{name}': {value}")))?;
    if !(0..=i64::from(u32::MAX)).contains(&value) {
        return Err(invalid(format!(
            "'{name}' parameter value '{value}' is out of range."
        )));
    }
    Ok(value as u32)
}

impl IndexIvf {
    /// C++ `IndexIVF::Make`. Parameter names are matched case-insensitively;
    /// a duplicate (after lowercasing) is an error.
    pub fn make(
        index_name: &str,
        index_comment: &str,
        file_name: &str,
        column_names: Vec<String>,
        params: &[(&str, &str)],
    ) -> Result<Self> {
        use std::collections::BTreeMap;

        let mut map: BTreeMap<String, String> = BTreeMap::new();
        for (name, value) in params {
            let lowered = name.to_ascii_lowercase();
            if map.insert(lowered, (*value).to_string()).is_some() {
                return Err(invalid("Duplicate parameter names"));
            }
        }

        let mut option = IvfOption::default();
        {
            let value = map
                .remove("metric")
                .ok_or_else(|| invalid("Missing parameter 'metric'."))?;
            option.metric = Metric::parse(&value.to_ascii_lowercase());
        }
        if let Some(value) = map.remove("centroids_num_ratio") {
            option.centroid_option.centroids_num_ratio = value
                .trim()
                .parse()
                .map_err(|_| invalid(format!("invalid centroids_num_ratio: {value}")))?;
        }
        if let Some(value) = map.remove("min_points_per_centroid") {
            option.centroid_option.min_points_per_centroid =
                parse_u32("min_points_per_centroid", &value)?;
        }
        if let Some(value) = map.remove("max_points_per_centroid") {
            option.centroid_option.max_points_per_centroid =
                parse_u32("max_points_per_centroid", &value)?;
        }
        if let Some(value) = map.remove("storage_type") {
            option.storage_option.storage_type = match value.to_ascii_lowercase().as_str() {
                "plain" => StorageType::Plain,
                "scalar_quantization" => StorageType::ScalarQuantization,
                "product_quantization" => StorageType::ProductQuantization,
                other => {
                    return Err(invalid(format!(
                        "Unrecognized storage_type '{other}', valid choices: 'plain', \
                         'scalar_quantization', 'product_quantization'"
                    )))
                }
            };
        }
        match option.storage_option.storage_type {
            StorageType::Plain => {
                if let Some(value) = map.remove("plain_storage_data_type") {
                    let upper = value.to_ascii_uppercase();
                    option.storage_option.plain_storage_data_type =
                        EmbeddingDataType::parse(&upper).ok_or_else(|| {
                            invalid(format!("Unexpected embedding type: {value}"))
                        })?;
                }
            }
            StorageType::ScalarQuantization => {
                let value = map
                    .remove("scalar_quantization_bits")
                    .ok_or_else(|| invalid("Missing parameter 'scalar_quantization_bits'."))?;
                option.storage_option.scalar_quantization_bits =
                    parse_u32("scalar_quantization_bits", &value)?;
            }
            StorageType::ProductQuantization => {
                let value = map
                    .remove("product_quantization_subspace_num")
                    .ok_or_else(|| {
                        invalid("Missing parameter 'product_quantization_subspace_num'.")
                    })?;
                let n = parse_u32("product_quantization_subspace_num", &value)?;
                if n < 1 {
                    return Err(invalid(format!(
                        "'product_quantization_subspace_num' parameter value '{n}' is out of range."
                    )));
                }
                option.storage_option.product_quantization_subspace_num = n;
                let value = map
                    .remove("product_quantization_subspace_bits")
                    .ok_or_else(|| {
                        invalid("Missing parameter 'product_quantization_subspace_bits'.")
                    })?;
                let bits: i64 = value.trim().parse().map_err(|_| {
                    invalid(format!(
                        "invalid product_quantization_subspace_bits: {value}"
                    ))
                })?;
                if !(4..=16).contains(&bits) {
                    return Err(invalid(format!(
                        "'product_quantization_subspace_bits' parameter value '{bits}' is out of range."
                    )));
                }
                option.storage_option.product_quantization_subspace_bits = bits as u32;
            }
        }
        if !map.is_empty() {
            let rest: Vec<String> = map.iter().map(|(k, v)| format!(" ({k}, {v})")).collect();
            return Err(invalid(format!(
                "Following parameters are left unused:{}.",
                rest.join("")
            )));
        }
        Ok(Self {
            index_name: index_name.to_string(),
            index_comment: index_comment.to_string(),
            file_name: file_name.to_string(),
            column_names,
            option,
        })
    }

    /// C++ `IndexIVF::ValidateColumnDataType`.
    pub fn validate_column(&self, table: &[Column], column_name: &str) -> Result<()> {
        let column = table
            .iter()
            .find(|c| c.name == column_name)
            .ok_or_else(|| invalid(format!("Column not exist: {column_name}")))?;
        let column_type = column.column_type;
        match column_type {
            ColumnType::Embedding(_, _) | ColumnType::MultiVector => {}
            ColumnType::Other => {
                return Err(invalid(format!(
                    "Attempt to create IVF index on column: {column_name}, data type: {}.",
                    column_type.describe()
                )))
            }
        }
        if self.option.metric == Metric::Invalid {
            return Err(invalid("Invalid metric type"));
        }
        check_centroid_option(&self.option.centroid_option)?;
        check_storage_option(&self.option.storage_option, column_type)
    }

    /// C++ `IndexIVF::BuildOtherParamsString`.
    pub fn build_other_params_string(&self) -> String {
        let centroid = &self.option.centroid_option;
        let storage = &self.option.storage_option;
        let storage_text = match storage.storage_type {
            // The reference prints swapped field labels for SQ/PQ; mirrored
            // verbatim so the output matches.
            StorageType::Plain => format!(
                "Type: Plain, plain_storage_data_type: {}",
                storage.plain_storage_data_type.name()
            ),
            StorageType::ProductQuantization => format!(
                "Type: ProductQuantization, scalar_quantization_bits: {}",
                storage.scalar_quantization_bits
            ),
            StorageType::ScalarQuantization => format!(
                "Type: ScalarQuantization, product_quantization_subspace_num: {}, \
                 product_quantization_subspace_bits: {}",
                storage.product_quantization_subspace_num,
                storage.product_quantization_subspace_bits
            ),
        };
        format!(
            "metric: {}, IndexIVFCentroidOption: [centroids_num_ratio: {}, \
             min_points_per_centroid: {}, max_points_per_centroid: {}], \
             IndexIVFStorageOption: [{}]",
            self.option.metric.name(),
            centroid.centroids_num_ratio,
            centroid.min_points_per_centroid,
            centroid.max_points_per_centroid,
            storage_text
        )
    }
}

fn check_centroid_option(option: &CentroidOption) -> Result<()> {
    if option.centroids_num_ratio < 0.0 {
        return Err(invalid(format!(
            "centroids_num_ratio parameter value '{}' is not positive.",
            option.centroids_num_ratio
        )));
    }
    if option.min_points_per_centroid > option.max_points_per_centroid {
        return Err(invalid(format!(
            "min_points_per_centroid ({}) is greater than max_points_per_centroid ({}).",
            option.min_points_per_centroid, option.max_points_per_centroid
        )));
    }
    Ok(())
}

fn check_storage_option(option: &StorageOption, column_type: ColumnType) -> Result<()> {
    let ColumnType::Embedding(embedding_data_type, dimension) = column_type else {
        // `kMultiVector` has no element-type check in the reference: the
        // storage option is only inspected for embedding columns.
        return Ok(());
    };
    match option.storage_type {
        StorageType::Plain => match option.plain_storage_data_type {
            EmbeddingDataType::Int8 | EmbeddingDataType::UInt8 => {
                if option.plain_storage_data_type != embedding_data_type {
                    return Err(invalid(format!(
                        "Invalid plain storage type: {} on Embedding column: {}",
                        option.plain_storage_data_type.name(),
                        column_type.describe()
                    )));
                }
            }
            EmbeddingDataType::Float | EmbeddingDataType::Float16 | EmbeddingDataType::BFloat16 => {
                match embedding_data_type {
                    dt if dt.is_floating() => {}
                    _ => {
                        return Err(invalid(format!(
                            "Attempt to use floating point plain storage on embedding column with \
                         type: {}.",
                            column_type.describe()
                        )))
                    }
                }
            }
            EmbeddingDataType::Double
            | EmbeddingDataType::Bit
            | EmbeddingDataType::Int16
            | EmbeddingDataType::Int32
            | EmbeddingDataType::Int64 => {
                return Err(invalid(format!(
                    "Invalid plain storage type: {}. Can only choose: int8 for int8, uint8 for \
                     uint8, float, float16 or bfloat16 for floating type",
                    option.plain_storage_data_type.name()
                )))
            }
            EmbeddingDataType::Invalid => match embedding_data_type {
                EmbeddingDataType::UInt8 | EmbeddingDataType::Int8 => {}
                dt if dt.is_floating() => {}
                _ => {
                    return Err(invalid(format!(
                        "Attempt to use floating point plain storage on embedding column with \
                         type: {}.",
                        column_type.describe()
                    )))
                }
            },
        },
        StorageType::ScalarQuantization | StorageType::ProductQuantization => {
            if !embedding_data_type.is_floating() {
                return Err(invalid(format!(
                    "Attempt to use scalar quantization on embedding column with type: {}. Can \
                     only use scalar quantization on float32, float16, bfloat16, float64 embedding.",
                    column_type.describe()
                )));
            }
            match option.storage_type {
                StorageType::ScalarQuantization => {
                    if option.scalar_quantization_bits != 4 && option.scalar_quantization_bits != 8
                    {
                        return Err(invalid(format!(
                            "scalar_quantization_bits can only be 4 or 8, now it is {}.",
                            option.scalar_quantization_bits
                        )));
                    }
                }
                StorageType::ProductQuantization => {
                    let n = option.product_quantization_subspace_num;
                    if n == 0 || n >= dimension as u32 || dimension % n as usize != 0 {
                        return Err(invalid(format!(
                            "product_quantization_subspace_num ({n}) is not divisor of \
                             embedding_dimension({dimension})!"
                        )));
                    }
                    let bits = option.product_quantization_subspace_bits;
                    if !(4..=16).contains(&bits) {
                        return Err(invalid(format!(
                            "product_quantization_subspace_bits can only be in range [4, 16], \
                             now it is {bits}."
                        )));
                    }
                }
                StorageType::Plain => unreachable!(),
            }
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    /// The reference's `BaseTableRef`: one BF16 embedding column, dimension 128.
    fn table() -> Vec<Column> {
        vec![Column::embedding(
            "col_ivf",
            EmbeddingDataType::BFloat16,
            128,
        )]
    }

    fn make(params: &[(&str, &str)]) -> Result<IndexIvf> {
        IndexIvf::make(
            "tmp_index_name",
            "test comment",
            "tmp_file_name",
            vec!["col_ivf".to_string()],
            params,
        )
    }

    /// C++: `IndexIVFTest.testplain` (`index_ivf_def_ut.cpp`).
    #[test]
    fn testplain() {
        let index = make(&[("meTric", "cos"), ("plain_storage_data_type", "f32")]).unwrap();
        index.validate_column(&table(), "col_ivf").unwrap();
        assert_eq!(index.option.metric, Metric::Cos);
        assert_eq!(
            index.option.storage_option.plain_storage_data_type,
            EmbeddingDataType::Float
        );
        assert!(index.build_other_params_string().contains("metric: cos"));

        let index = make(&[("meTric", "cos"), ("plain_storage_data_type", "bf16")]).unwrap();
        index.validate_column(&table(), "col_ivf").unwrap();
        assert_eq!(
            index.option.storage_option.plain_storage_data_type,
            EmbeddingDataType::BFloat16
        );

        // float-only column may not use an int8 plain store
        let index = make(&[("meTric", "cos"), ("plain_storage_data_type", "i8")]).unwrap();
        assert!(index.validate_column(&table(), "col_ivf").is_err());

        // unknown parameter
        let err = make(&[
            ("meTric", "cos"),
            ("plain_storage_data_type", "i8"),
            ("unused_param", "xxx"),
        ])
        .unwrap_err();
        assert!(err.to_string().contains("unused"), "{err}");
    }

    /// C++: `IndexIVFTest.testsq` (`index_ivf_def_ut.cpp`).
    #[test]
    fn testsq() {
        let index = make(&[
            ("metrIc", "L2"),
            ("centroids_num_ratio", "0.33"),
            ("storage_type", "Scalar_Quantization"),
            ("scalar_quantization_bits", "4"),
        ])
        .unwrap();
        index.validate_column(&table(), "col_ivf").unwrap();
        assert_eq!(index.option.metric, Metric::L2);
        assert_eq!(
            index.option.storage_option.storage_type,
            StorageType::ScalarQuantization
        );

        let index = make(&[
            ("metrIc", "L2"),
            ("centroids_num_ratio", "-0.33"),
            ("storage_type", "Scalar_Quantization"),
            ("scalar_quantization_bits", "4"),
        ])
        .unwrap();
        assert!(index.validate_column(&table(), "col_ivf").is_err());

        // parameter names collide case-insensitively
        let err = make(&[
            ("metrIc", "L2"),
            ("centroids_num_ratio", "0.33"),
            ("Centroids_num_ratio", "0.33"),
            ("storage_type", "Scalar_Quantization"),
            ("scalar_quantization_bits", "4"),
        ])
        .unwrap_err();
        assert!(err.to_string().contains("Duplicate"), "{err}");
    }

    /// C++: `IndexIVFTest.testpq` (`index_ivf_def_ut.cpp`).
    #[test]
    fn testpq() {
        let index = make(&[
            ("Metric", "ip"),
            ("storage_type", "product_quAntization"),
            ("product_quantization_subspace_num", "32"),
            ("product_quanTization_subspace_bits", "12"),
        ])
        .unwrap();
        index.validate_column(&table(), "col_ivf").unwrap();
        assert_eq!(index.option.metric, Metric::Ip);
        assert_eq!(
            index.option.storage_option.storage_type,
            StorageType::ProductQuantization
        );
        assert_eq!(
            index
                .option
                .storage_option
                .product_quantization_subspace_num,
            32
        );
        assert_eq!(
            index
                .option
                .storage_option
                .product_quantization_subspace_bits,
            12
        );

        // metric is mandatory
        let err = make(&[
            ("storage_type", "product_quAntization"),
            ("product_quantization_subspace_num", "32"),
            ("product_quanTization_subspace_bits", "12"),
        ])
        .unwrap_err();
        assert!(
            err.to_string().contains("Missing parameter 'metric'"),
            "{err}"
        );
    }
}

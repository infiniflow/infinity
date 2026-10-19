//! Base column/logical types.
//!
//! Mirrors the subset of `src/common/types/logical_type.cppm` needed by the
//! storage kernel. Extended incrementally as milestones land.

use std::fmt;

/// Monotonic transaction timestamp (Infinity `TxnTimeStamp`).
pub type TxnTimeStamp = u64;

/// Global row identifier (0 == invalid, mirroring Infinity's `INVALID_ROWID`).
pub type RowID = u64;

/// Invalid row id sentinel.
pub const INVALID_ROWID: RowID = u64::MAX;

/// C++: `DateT` (`src/parser/type/datetime/date_type.h`): days since
/// 1970-01-01.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct DateT {
    pub value: i32,
}

impl DateT {
    pub fn new(value: i32) -> Self {
        Self { value }
    }
}

/// C++: `TimeT` (`src/parser/type/datetime/time_type.h`): microseconds since
/// midnight.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct TimeT {
    pub value: i32,
}

impl TimeT {
    pub fn new(value: i32) -> Self {
        Self { value }
    }
}

/// C++: `DateTimeT`: a date and a time of day.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct DateTimeT {
    pub date: i32,
    pub time: i32,
}

impl DateTimeT {
    pub fn new(date: i32, time: i32) -> Self {
        Self { date, time }
    }
}

/// C++: `TimestampT`: a date and a time of day.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct TimestampT {
    pub date: i32,
    pub time: i32,
}

impl TimestampT {
    pub fn new(date: i32, time: i32) -> Self {
        Self { date, time }
    }
}

/// C++ `TimeUnit` (`src/parser/type/datetime/interval_type.h`).
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub enum TimeUnit {
    Year,
    Month,
    Day,
    Hour,
    Minute,
    Second,
    #[default]
    Invalid,
}

/// C++: `IntervalT` (`src/parser/type/datetime/interval_type.h`).
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct IntervalT {
    pub value: i32,
    pub unit: TimeUnit,
}

impl IntervalT {
    /// C++ `IntervalType(int32_t)`: the unit stays `kInvalidUnit`.
    pub fn new(value: i32) -> Self {
        Self {
            value,
            unit: TimeUnit::Invalid,
        }
    }

    pub fn with_unit(value: i32, unit: TimeUnit) -> Self {
        Self { value, unit }
    }

    pub fn reset(&mut self) {
        self.value = 0;
        self.unit = TimeUnit::Invalid;
    }
}

/// C++: `DecimalT` (`src/parser/type/decimal_type.h`): a scaled integer.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub struct DecimalT {
    pub value: i128,
    pub scale: u32,
}

impl DecimalT {
    pub fn new(value: i128, scale: u32) -> Self {
        Self { value, scale }
    }
}

/// C++: `HugeIntT`: a 128-bit signed integer as two 64-bit halves.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct HugeIntT {
    pub upper: i64,
    pub lower: i64,
}

impl HugeIntT {
    pub fn new(upper: i64, lower: i64) -> Self {
        Self { upper, lower }
    }

    /// Two's-complement 128-bit value (matching C++ `HugeIntT` layout).
    pub fn to_i128(self) -> i128 {
        ((self.upper as i128) << 64) | (self.lower as u64 as i128)
    }

    pub fn from_i128(value: i128) -> Self {
        Self {
            upper: (value >> 64) as i64,
            lower: value as i64,
        }
    }
}

/// C++: `UuidT` (`src/parser/type/complex/uuid_type.h`): 16 raw bytes.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub struct UuidT(pub [u8; UuidT::LENGTH]);

impl UuidT {
    pub const LENGTH: usize = 16;

    pub const fn new() -> Self {
        Self([0; Self::LENGTH])
    }

    pub fn set(&mut self, input: &[u8; Self::LENGTH]) {
        self.0 = *input;
    }

    pub fn reset(&mut self) {
        self.0 = [0; Self::LENGTH];
    }

    /// The C++ `ToString()` keeps the trailing NULs; `EXPECT_STREQ` therefore
    /// compares up to the first NUL.
    pub fn to_string_value(&self) -> String {
        let end = self.0.iter().position(|b| *b == 0).unwrap_or(Self::LENGTH);
        String::from_utf8_lossy(&self.0[..end]).into_owned()
    }
}

/// C++: `MixedValueType` (`src/parser/type/base_mixed_type.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum MixedValueType {
    #[default]
    Invalid,
    Integer,
    Float,
    ShortStr,
    LongStr,
    Tuple,
    Array,
    Null,
    Missing,
    Dummy,
}

impl MixedValueType {
    /// C++: `BaseMixedType::GetTypeName`.
    pub fn type_name(self) -> &'static str {
        match self {
            Self::Invalid => "Invalid",
            Self::Integer => "Integer",
            Self::Float => "Float",
            Self::ShortStr => "Short String",
            Self::LongStr => "Long String",
            Self::Tuple => "Tuple",
            Self::Array => "Array",
            Self::Null => "Null",
            Self::Missing => "Missing",
            Self::Dummy => "Dummy",
        }
    }
}

/// The payload of a [`MixedT`] value.
///
/// C++: the `MixedType` union (`src/parser/type/mixed_type.h`). The reference's
/// short/long string split mirrors its inline header (14 bytes); the payload is
/// otherwise an owned value, so Rust's move semantics replace the reference's
/// moved-from `kInvalid` state.
#[derive(Debug, Clone, Default, PartialEq)]
pub enum MixedValue {
    #[default]
    Invalid,
    Integer(i64),
    Float(f64),
    ShortStr(String),
    LongStr(String),
    /// C++ `TupleMixedType`: a fixed number of named slots.
    Tuple {
        count: usize,
        entries: Vec<(String, MixedT)>,
    },
    /// C++ `ArrayMixedType`: a fixed number of positional slots.
    Array {
        count: usize,
        entries: Vec<Option<MixedT>>,
    },
    Null,
    Missing,
    Dummy,
}

/// C++: `MixedT` (`src/parser/type/mixed_type.h`) — a heterogeneous value.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct MixedT {
    pub value: MixedValue,
}

/// C++ `ShortStrMixedType`'s inline capacity.
pub const MIXED_SHORT_STR_MAX: usize = 14;

impl MixedT {
    /// The textual constructor used by the column/block mirror.
    pub fn new(value: impl Into<String>) -> Self {
        Self::make_string(&value.into())
    }

    pub fn make_integer(value: i64) -> Self {
        Self {
            value: MixedValue::Integer(value),
        }
    }

    pub fn make_float(value: f64) -> Self {
        Self {
            value: MixedValue::Float(value),
        }
    }

    /// C++ `MixedType::MakeString`: up to 14 bytes stay inline (`kShortStr`).
    pub fn make_string(value: &str) -> Self {
        let value = value.to_string();
        Self {
            value: if value.len() <= MIXED_SHORT_STR_MAX {
                MixedValue::ShortStr(value)
            } else {
                MixedValue::LongStr(value)
            },
        }
    }

    pub fn make_tuple(values: Vec<MixedT>) -> Self {
        let count = values.len();
        let entries = values
            .into_iter()
            .enumerate()
            .map(|(i, v)| (format!("key{i}"), v))
            .collect();
        Self {
            value: MixedValue::Tuple { count, entries },
        }
    }

    /// C++ `MixedType::MakeTuple(count)`: a tuple with `count` empty slots. A
    /// zero count is a parser error in the reference.
    pub fn make_tuple_with_count(count: usize) -> crate::Result<Self> {
        if count == 0 {
            return Err(crate::Error::Invalid(
                "mixed tuple capacity must be positive".to_string(),
            ));
        }
        Ok(Self {
            value: MixedValue::Tuple {
                count,
                entries: Vec::new(),
            },
        })
    }

    pub fn make_array(values: Vec<MixedT>) -> Self {
        let count = values.len();
        Self {
            value: MixedValue::Array {
                count,
                entries: values.into_iter().map(Some).collect(),
            },
        }
    }

    /// C++ `MixedType::MakeArray(count)`.
    pub fn make_array_with_count(count: usize) -> crate::Result<Self> {
        if count == 0 {
            return Err(crate::Error::Invalid(
                "mixed array capacity must be positive".to_string(),
            ));
        }
        Ok(Self {
            value: MixedValue::Array {
                count,
                entries: (0..count).map(|_| None).collect(),
            },
        })
    }

    /// C++ `Insert<Type>IntoTuple`: the key must be new and a slot must remain.
    pub fn insert_into_tuple(&mut self, key: &str, value: MixedT) -> crate::Result<()> {
        let MixedValue::Tuple { count, entries } = &mut self.value else {
            return Err(crate::Error::Invalid(
                "value is not a mixed tuple".to_string(),
            ));
        };
        if entries.iter().any(|(k, _)| k == key) {
            return Err(crate::Error::Invalid(format!(
                "mixed tuple key '{key}' already exists"
            )));
        }
        if entries.len() >= *count {
            return Err(crate::Error::Invalid("mixed tuple is full".to_string()));
        }
        entries.push((key.to_string(), value));
        Ok(())
    }

    /// C++ `GetFromTuple`.
    pub fn get_from_tuple(&self, key: &str) -> Option<&MixedT> {
        match &self.value {
            MixedValue::Tuple { entries, .. } => {
                entries.iter().find(|(k, _)| k == key).map(|(_, v)| v)
            }
            _ => None,
        }
    }

    /// C++ `Insert<Type>IntoArray`.
    pub fn insert_into_array(&mut self, index: usize, value: MixedT) -> crate::Result<()> {
        let MixedValue::Array { count, entries } = &mut self.value else {
            return Err(crate::Error::Invalid(
                "value is not a mixed array".to_string(),
            ));
        };
        if index >= *count {
            return Err(crate::Error::Invalid(format!(
                "mixed array index {index} out of range {count}"
            )));
        }
        entries[index] = Some(value);
        Ok(())
    }

    /// C++ `GetByIndex`.
    pub fn get_by_index(&self, index: usize) -> Option<&MixedT> {
        match &self.value {
            MixedValue::Array { entries, .. } => entries.get(index)?.as_ref(),
            _ => None,
        }
    }

    /// The declared slot count of a tuple/array.
    pub fn slot_count(&self) -> Option<usize> {
        match &self.value {
            MixedValue::Tuple { count, .. } | MixedValue::Array { count, .. } => Some(*count),
            _ => None,
        }
    }

    /// C++ `MixedType::MakeNull`.
    pub fn make_null() -> Self {
        Self {
            value: MixedValue::Null,
        }
    }

    /// C++ `MixedType::MakeMiss`.
    pub fn make_missing() -> Self {
        Self {
            value: MixedValue::Missing,
        }
    }

    /// C++ `NullValue<MixedT>()`: the value's type tag is `kNull`.
    pub fn null() -> Self {
        Self::make_null()
    }

    /// C++ `MixedType::Reset`: back to `kInvalid`.
    pub fn reset(&mut self) {
        self.value = MixedValue::Invalid;
    }

    /// C++ `MixedType::type`.
    pub fn value_type(&self) -> MixedValueType {
        match self.value {
            MixedValue::Invalid => MixedValueType::Invalid,
            MixedValue::Integer(_) => MixedValueType::Integer,
            MixedValue::Float(_) => MixedValueType::Float,
            MixedValue::ShortStr(_) => MixedValueType::ShortStr,
            MixedValue::LongStr(_) => MixedValueType::LongStr,
            MixedValue::Tuple { .. } => MixedValueType::Tuple,
            MixedValue::Array { .. } => MixedValueType::Array,
            MixedValue::Null => MixedValueType::Null,
            MixedValue::Missing => MixedValueType::Missing,
            MixedValue::Dummy => MixedValueType::Dummy,
        }
    }

    pub fn as_integer(&self) -> Option<i64> {
        match self.value {
            MixedValue::Integer(v) => Some(v),
            _ => None,
        }
    }

    pub fn as_float(&self) -> Option<f64> {
        match self.value {
            MixedValue::Float(v) => Some(v),
            _ => None,
        }
    }

    pub fn as_text(&self) -> Option<&str> {
        match &self.value {
            MixedValue::ShortStr(s) | MixedValue::LongStr(s) => Some(s),
            _ => None,
        }
    }

    /// The rendered form used by the block mirror.
    pub fn text(&self) -> String {
        match &self.value {
            MixedValue::Invalid => String::new(),
            MixedValue::Integer(v) => v.to_string(),
            MixedValue::Float(v) => v.to_string(),
            MixedValue::ShortStr(s) | MixedValue::LongStr(s) => s.clone(),
            MixedValue::Null => "null".to_string(),
            MixedValue::Missing => "missing".to_string(),
            MixedValue::Dummy => "dummy".to_string(),
            MixedValue::Tuple { entries, .. } => {
                let inner: Vec<String> = entries.iter().map(|(_, v)| v.text()).collect();
                format!("[{}]", inner.join(","))
            }
            MixedValue::Array { entries, .. } => {
                let inner: Vec<String> = entries
                    .iter()
                    .map(|v| v.as_ref().map(MixedT::text).unwrap_or_default())
                    .collect();
                format!("[{}]", inner.join(","))
            }
        }
    }

    /// Approximate heap bytes (used by the column-vector accounting).
    pub fn heap_len(&self) -> usize {
        match &self.value {
            MixedValue::ShortStr(s) | MixedValue::LongStr(s) => s.len(),
            MixedValue::Tuple { entries, .. } => entries
                .iter()
                .map(|(k, v)| k.len() + v.heap_len() + 24)
                .sum::<usize>(),
            MixedValue::Array { entries, .. } => {
                entries
                    .iter()
                    .flatten()
                    .map(MixedT::heap_len)
                    .sum::<usize>()
                    + entries.len() * 24
            }
            _ => 0,
        }
    }
}

/// Logical column type (subset).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum LogicalType {
    Invalid,
    Boolean,
    TinyInt,
    SmallInt,
    Integer,
    BigInt,
    HugeInt,
    Float,
    Double,
    Decimal,
    Varchar,
    Date,
    Time,
    DateTime,
    Timestamp,
    Interval,
    Uuid,
    Array,
    /// A nested tuple (the C++ `kTuple`).
    Tuple,
    Mixed,
    /// Embedded vector of `dim` f32 elements.
    Embedding(u32),
    /// Internal global row id.
    RowID,
    Null,
    /// Internal missing value used by heterogeneous columns.
    Missing,
}

impl LogicalType {
    /// C++ `LogicalType2Str` (`src/parser/type/logical_type.cpp`).
    pub fn type_name(&self) -> &'static str {
        use LogicalType::*;
        match self {
            Invalid => "Invalid",
            Boolean => "Boolean",
            TinyInt => "TinyInt",
            SmallInt => "SmallInt",
            Integer => "Integer",
            BigInt => "BigInt",
            HugeInt => "HugeInt",
            Float => "Float",
            Double => "Double",
            Decimal => "Decimal",
            Varchar => "Varchar",
            Date => "Date",
            Time => "Time",
            DateTime => "DateTime",
            Timestamp => "Timestamp",
            Interval => "Interval",
            Uuid => "UUID",
            Array => "Array",
            Tuple => "Tuple",
            Mixed => "Heterogeneous",
            Embedding(_) => "Embedding",
            RowID => "RowID",
            Null => "Null",
            Missing => "Missing",
        }
    }

    /// C++ `DataType::TypeToString<T>()` for the scalar Rust types.
    pub fn scalar_type_name<T: 'static>() -> &'static str {
        use std::any::TypeId;
        if TypeId::of::<T>() == TypeId::of::<bool>() {
            "Boolean"
        } else if TypeId::of::<T>() == TypeId::of::<i8>() {
            "TinyInt"
        } else if TypeId::of::<T>() == TypeId::of::<i16>() {
            "SmallInt"
        } else if TypeId::of::<T>() == TypeId::of::<i32>() {
            "Integer"
        } else if TypeId::of::<T>() == TypeId::of::<i64>() {
            "BigInt"
        } else if TypeId::of::<T>() == TypeId::of::<HugeIntT>() {
            "HugeInt"
        } else if TypeId::of::<T>() == TypeId::of::<f32>() {
            "Float"
        } else if TypeId::of::<T>() == TypeId::of::<f64>() {
            "Double"
        } else if TypeId::of::<T>() == TypeId::of::<DecimalT>() {
            "Decimal"
        } else if TypeId::of::<T>() == TypeId::of::<String>() {
            "Varchar"
        } else if TypeId::of::<T>() == TypeId::of::<DateT>() {
            "Date"
        } else if TypeId::of::<T>() == TypeId::of::<TimeT>() {
            "Time"
        } else if TypeId::of::<T>() == TypeId::of::<DateTimeT>() {
            "DateTime"
        } else if TypeId::of::<T>() == TypeId::of::<TimestampT>() {
            "Timestamp"
        } else if TypeId::of::<T>() == TypeId::of::<IntervalT>() {
            "Interval"
        } else if TypeId::of::<T>() == TypeId::of::<UuidT>() {
            "UUID"
        } else {
            "Invalid"
        }
    }

    /// Fixed inline width in bytes, or `None` for variable-length types.
    pub fn fixed_width(&self) -> Option<usize> {
        use LogicalType::*;
        Some(match self {
            Boolean | TinyInt => 1,
            SmallInt => 2,
            Integer | Float => 4,
            BigInt | Double | DateTime | Timestamp | Tuple | RowID => 8,
            HugeInt => 16,
            Decimal => 16,
            Uuid => 16,
            Embedding(dim) => (*dim as usize) * 4,
            Varchar | Array | Mixed | Date | Time | Interval | Invalid | Null | Missing => {
                if matches!(self, Date | Time | Interval) {
                    4
                } else {
                    return None;
                }
            }
        })
    }

    pub fn is_variable_length(&self) -> bool {
        self.fixed_width().is_none()
    }
}

impl fmt::Display for LogicalType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.type_name())
    }
}

/// C++: `DecimalInfo` (`src/parser/type/info/decimal_info.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct DecimalInfo {
    precision: i64,
    scale: i64,
}

impl DecimalInfo {
    /// C++ `DecimalInfo::Make`: `(0, 0)` means the maximum `(38, 38)`.
    pub fn make(precision: i64, scale: i64) -> crate::Result<Self> {
        let (precision, scale) = if precision == 0 && scale == 0 {
            (38, 38)
        } else {
            (precision, scale)
        };
        if scale > precision {
            return Err(crate::Error::Invalid(format!(
                "parser: Decimal scale {scale} is greater than precision {precision}."
            )));
        }
        if precision > 38 {
            return Err(crate::Error::Invalid(format!(
                "parser: Decimal precision {precision} is greater than 38."
            )));
        }
        Ok(Self { precision, scale })
    }

    pub fn precision(&self) -> i64 {
        self.precision
    }

    pub fn scale(&self) -> i64 {
        self.scale
    }

    /// C++ `DecimalInfo::Size()`: 16 bytes regardless of precision.
    pub fn size(&self) -> usize {
        16
    }

    /// C++ `DecimalInfo::ToString()`.
    pub fn to_string_value(&self) -> String {
        format!("decimal({}, {})", self.precision, self.scale)
    }
}

/// C++: `EmbeddingDataType` (`src/parser/type/info/embedding_info.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EmbeddingDataType {
    Bit,
    UInt8,
    Int8,
    Int16,
    Int32,
    Int64,
    Float,
    Double,
    Float16,
    BFloat16,
}

impl EmbeddingDataType {
    pub fn name(&self) -> &'static str {
        match self {
            Self::Bit => "bit",
            Self::UInt8 => "uint8",
            Self::Int8 => "int8",
            Self::Int16 => "int16",
            Self::Int32 => "int32",
            Self::Int64 => "int64",
            Self::Float => "float",
            Self::Double => "double",
            Self::Float16 => "float16",
            Self::BFloat16 => "bfloat16",
        }
    }
}

/// C++ `PARSER_EMBEDDING_LIMIT` (`src/parser/parser_defaults.cppm`).
pub const PARSER_EMBEDDING_LIMIT: usize = 65536;

/// C++: `EmbeddingInfo` (`src/parser/type/info/embedding_info.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct EmbeddingInfo {
    data_type: EmbeddingDataType,
    dimension: usize,
}

impl EmbeddingInfo {
    /// C++ `EmbeddingInfo::Make`; dimensions above the parser limit are a
    /// `ParserException`.
    pub fn make(data_type: EmbeddingDataType, dimension: usize) -> crate::Result<Self> {
        if dimension > PARSER_EMBEDDING_LIMIT {
            return Err(crate::Error::Invalid(format!(
                "parser: Embedding dimension should less than {PARSER_EMBEDDING_LIMIT}"
            )));
        }
        Ok(Self {
            data_type,
            dimension,
        })
    }

    /// `EmbeddingType::EmbeddingSize`.
    pub fn size(&self) -> usize {
        let elem_bits = match self.data_type {
            EmbeddingDataType::Bit => 1,
            EmbeddingDataType::UInt8 | EmbeddingDataType::Int8 => 8,
            EmbeddingDataType::Int16 | EmbeddingDataType::Float16 | EmbeddingDataType::BFloat16 => {
                16
            }
            EmbeddingDataType::Int32 | EmbeddingDataType::Float => 32,
            EmbeddingDataType::Int64 | EmbeddingDataType::Double => 64,
        };
        (self.dimension * elem_bits).div_ceil(8)
    }

    pub fn data_type(&self) -> EmbeddingDataType {
        self.data_type
    }

    pub fn dimension(&self) -> usize {
        self.dimension
    }

    pub fn to_string_value(&self) -> String {
        format!("{},{}", self.data_type.name(), self.dimension)
    }
}

/// C++: `TypeInfo` (`src/parser/type/info/type_info.h`), the variants the
/// scalar engine needs.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TypeInfo {
    Decimal(DecimalInfo),
    Embedding(EmbeddingInfo),
}

impl TypeInfo {
    pub fn size(&self) -> usize {
        match self {
            TypeInfo::Decimal(info) => info.size(),
            TypeInfo::Embedding(info) => info.size(),
        }
    }

    pub fn to_string_value(&self) -> String {
        match self {
            TypeInfo::Decimal(info) => info.to_string_value(),
            TypeInfo::Embedding(info) => info.to_string_value(),
        }
    }
}

/// C++: `DataType` (`src/parser/type/data_type.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct DataType {
    ty: LogicalType,
    info: Option<TypeInfo>,
}

impl DataType {
    pub fn new(ty: LogicalType) -> Self {
        Self { ty, info: None }
    }

    pub fn with_info(ty: LogicalType, info: TypeInfo) -> Self {
        Self {
            ty,
            info: Some(info),
        }
    }

    pub fn ty(&self) -> LogicalType {
        self.ty
    }

    pub fn info(&self) -> Option<TypeInfo> {
        self.info
    }

    /// C++ `DataType::GetSizeInBytes` for the types this crate models.
    pub fn size_in_bytes(&self) -> usize {
        match self.info {
            Some(info) => info.size(),
            None => self.ty.fixed_width().unwrap_or(0),
        }
    }
}

impl fmt::Display for DataType {
    /// C++ `DataType::ToString`: `Type` or `Type(type_info)`.
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.info {
            Some(info) => write!(f, "{}({})", self.ty.type_name(), info.to_string_value()),
            None => write!(f, "{}", self.ty.type_name()),
        }
    }
}

/// C++ `NullValue<T>()` (`src/common/types/null_value.h`): the in-band sentinel
/// each fixed-width type reserves for NULL.
pub trait NullValue {
    fn null_value() -> Self;
}

impl NullValue for bool {
    fn null_value() -> Self {
        false
    }
}

macro_rules! int_null_value {
    ($ty:ty) => {
        impl NullValue for $ty {
            fn null_value() -> Self {
                <$ty>::MIN
            }
        }
    };
}
int_null_value!(i8);
int_null_value!(i16);
int_null_value!(i32);
int_null_value!(i64);

impl NullValue for HugeIntT {
    fn null_value() -> Self {
        HugeIntT::new(i64::MIN, i64::MIN)
    }
}

impl NullValue for f32 {
    fn null_value() -> Self {
        f32::NEG_INFINITY
    }
}

impl NullValue for f64 {
    fn null_value() -> Self {
        f64::NEG_INFINITY
    }
}

impl NullValue for DecimalT {
    fn null_value() -> Self {
        DecimalT::new(i128::MIN, 0)
    }
}

impl NullValue for DateT {
    fn null_value() -> Self {
        DateT::new(i32::MIN)
    }
}

impl NullValue for TimeT {
    fn null_value() -> Self {
        TimeT::new(i32::MIN)
    }
}

impl NullValue for DateTimeT {
    fn null_value() -> Self {
        DateTimeT::new(i32::MIN, i32::MIN)
    }
}

impl NullValue for TimestampT {
    fn null_value() -> Self {
        TimestampT::new(i32::MIN, i32::MIN)
    }
}

impl NullValue for IntervalT {
    fn null_value() -> Self {
        IntervalT::with_unit(i32::MIN, TimeUnit::Invalid)
    }
}

impl NullValue for MixedT {
    fn null_value() -> Self {
        MixedT::null()
    }
}

impl NullValue for UuidT {
    fn null_value() -> Self {
        UuidT::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn widths() {
        assert_eq!(LogicalType::Integer.fixed_width(), Some(4));
        assert_eq!(LogicalType::BigInt.fixed_width(), Some(8));
        assert_eq!(LogicalType::Embedding(3).fixed_width(), Some(12));
        assert!(LogicalType::Varchar.is_variable_length());
        assert!(LogicalType::Embedding(1).fixed_width().is_some());
    }

    // C++: src/unit_test/parser/type/data_type_ut.cpp:GetTypeName
    #[test]
    fn data_type_get_type_name() {
        let cases = [
            (LogicalType::Boolean, "Boolean"),
            (LogicalType::TinyInt, "TinyInt"),
            (LogicalType::SmallInt, "SmallInt"),
            (LogicalType::Integer, "Integer"),
            (LogicalType::BigInt, "BigInt"),
            (LogicalType::HugeInt, "HugeInt"),
            (LogicalType::Float, "Float"),
            (LogicalType::Double, "Double"),
            (LogicalType::Decimal, "Decimal"),
            (LogicalType::Varchar, "Varchar"),
            (LogicalType::Date, "Date"),
            (LogicalType::Time, "Time"),
            (LogicalType::DateTime, "DateTime"),
            (LogicalType::Timestamp, "Timestamp"),
            (LogicalType::Interval, "Interval"),
            (LogicalType::Array, "Array"),
            (LogicalType::Tuple, "Tuple"),
            (LogicalType::Uuid, "UUID"),
            (LogicalType::Embedding(0), "Embedding"),
            (LogicalType::RowID, "RowID"),
            (LogicalType::Null, "Null"),
            (LogicalType::Missing, "Missing"),
            (LogicalType::Invalid, "Invalid"),
        ];
        for (logical_type, name) in cases {
            assert_eq!(DataType::new(logical_type).to_string(), name);
        }
        // Geography (Point/Line/LineSegment/Box/Circle) is excluded by project
        // decision, so those rows are not asserted here.
    }

    // C++: src/unit_test/parser/type/data_type_ut.cpp:TypeToString
    #[test]
    fn data_type_type_to_string() {
        assert_eq!(LogicalType::scalar_type_name::<bool>(), "Boolean");
        assert_eq!(LogicalType::scalar_type_name::<i8>(), "TinyInt");
        assert_eq!(LogicalType::scalar_type_name::<i16>(), "SmallInt");
        assert_eq!(LogicalType::scalar_type_name::<i32>(), "Integer");
        assert_eq!(LogicalType::scalar_type_name::<i64>(), "BigInt");
        assert_eq!(LogicalType::scalar_type_name::<HugeIntT>(), "HugeInt");
        assert_eq!(LogicalType::scalar_type_name::<f32>(), "Float");
        assert_eq!(LogicalType::scalar_type_name::<f64>(), "Double");
        assert_eq!(LogicalType::scalar_type_name::<DecimalT>(), "Decimal");
        assert_eq!(LogicalType::scalar_type_name::<String>(), "Varchar");
        assert_eq!(LogicalType::scalar_type_name::<DateT>(), "Date");
        assert_eq!(LogicalType::scalar_type_name::<TimeT>(), "Time");
        assert_eq!(LogicalType::scalar_type_name::<DateTimeT>(), "DateTime");
        assert_eq!(LogicalType::scalar_type_name::<TimestampT>(), "Timestamp");
        assert_eq!(LogicalType::scalar_type_name::<IntervalT>(), "Interval");
        assert_eq!(LogicalType::scalar_type_name::<UuidT>(), "UUID");
    }

    // C++: src/unit_test/parser/type/null_value_ut.cpp:null_value_test1
    #[test]
    fn null_value_test1() {
        assert!(!bool::null_value());
        assert_eq!(i8::null_value(), i8::MIN);
        assert_eq!(i16::null_value(), i16::MIN);
        assert_eq!(i32::null_value(), i32::MIN);
        assert_eq!(i64::null_value(), i64::MIN);
        let huge = HugeIntT::null_value();
        assert_eq!(huge.lower, i64::MIN);
        assert_eq!(huge.upper, i64::MIN);
        assert_eq!(f32::null_value(), f32::NEG_INFINITY);
        assert_eq!(f64::null_value(), f64::NEG_INFINITY);
        let decimal = DecimalT::null_value();
        assert_eq!(decimal.value, i128::MIN);
        assert_eq!(DateT::null_value().value, i32::MIN);
        assert_eq!(TimeT::null_value().value, i32::MIN);
        assert_eq!(DateTimeT::null_value().date, i32::MIN);
        assert_eq!(DateTimeT::null_value().time, i32::MIN);
        assert_eq!(TimestampT::null_value().date, i32::MIN);
        assert_eq!(TimestampT::null_value().time, i32::MIN);
        let interval = IntervalT::null_value();
        assert_eq!(interval.unit, TimeUnit::Invalid);
        assert_eq!(interval.value, i32::MIN);
        assert_eq!(MixedT::null_value().value_type(), MixedValueType::Null);
    }

    // C++: src/unit_test/parser/type/complex/uuid_type_ut.cpp:Uuid
    #[test]
    fn uuid() {
        let uuid_str = *b"aabbccddeeffgghh";
        let mut uuid1 = UuidT::new();
        uuid1.set(&uuid_str);
        assert_eq!(uuid1.to_string_value(), "aabbccddeeffgghh");

        // Copy construction / assignment.
        let uuid2 = uuid1;
        assert_eq!(uuid2.to_string_value(), "aabbccddeeffgghh");
        let mut uuid3 = UuidT::new();
        assert_eq!(uuid3.to_string_value(), "");
        uuid3 = uuid2;
        assert_eq!(uuid3.to_string_value(), "aabbccddeeffgghh");

        // Rust moves do not reset the source (the C++ move-reset assertions are
        // ownership semantics rather than value semantics).
        let uuid5 = uuid3;
        assert_eq!(uuid5.to_string_value(), "aabbccddeeffgghh");
        assert_eq!(uuid1, uuid5);

        let mut uuid5 = uuid5;
        uuid5.reset();
        assert_eq!(uuid5.to_string_value(), "");
    }

    // C++: src/unit_test/parser/type/info/decimal_info_ut.cpp:decimal_info_A
    #[test]
    fn decimal_info_a() {
        assert!(DecimalInfo::make(39, 39).is_err());
        assert!(DecimalInfo::make(38, 39).is_err());
        let info = DecimalInfo::make(38, 38).unwrap();
        assert_eq!(info.scale(), 38);
        assert_eq!(info.precision(), 38);
        assert_eq!(info.size(), 16);
        assert_eq!(
            DataType::with_info(LogicalType::Decimal, TypeInfo::Decimal(info)).to_string(),
            "Decimal(decimal(38, 38))"
        );
    }

    // C++: src/unit_test/parser/type/info/embedding_info_ut.cpp:embedding_info_A
    #[test]
    fn embedding_info_a() {
        assert!(EmbeddingInfo::make(EmbeddingDataType::Float, PARSER_EMBEDDING_LIMIT + 1).is_err());
        let info = EmbeddingInfo::make(EmbeddingDataType::Bit, 256).unwrap();
        assert_eq!(info.size(), 32);
        assert_eq!(info.dimension(), 256);
        assert_eq!(info.data_type(), EmbeddingDataType::Bit);

        let info = EmbeddingInfo::make(EmbeddingDataType::Int64, PARSER_EMBEDDING_LIMIT).unwrap();
        assert_eq!(info.size(), PARSER_EMBEDDING_LIMIT * 8);
        assert_eq!(info.dimension(), PARSER_EMBEDDING_LIMIT);
        assert_eq!(info.data_type(), EmbeddingDataType::Int64);
    }
    /// `src/unit_test/parser/type/heterogeneous/base_mixed_type_ut.cpp::BaseMixedTypeTest/GetTypeName`.
    #[test]
    fn mixed_value_type_names_match_the_reference() {
        for (ty, name) in [
            (MixedValueType::Invalid, "Invalid"),
            (MixedValueType::Integer, "Integer"),
            (MixedValueType::LongStr, "Long String"),
            (MixedValueType::ShortStr, "Short String"),
            (MixedValueType::Tuple, "Tuple"),
            (MixedValueType::Array, "Array"),
            (MixedValueType::Null, "Null"),
            (MixedValueType::Missing, "Missing"),
            (MixedValueType::Dummy, "Dummy"),
        ] {
            assert_eq!(ty.type_name(), name);
        }
    }
    /// `src/unit_test/parser/type/heterogeneous/mixed_type_ut.cpp::mixed_integer`.
    #[test]
    fn mixed_integer_value_and_clone() {
        let mut a = MixedT::make_integer(10);
        assert_eq!(a.value_type(), MixedValueType::Integer);
        assert_eq!(a.as_integer(), Some(10));
        let b = a.clone();
        a.value = MixedValue::Integer(11);
        assert_eq!(a.as_integer(), Some(11));
        assert_eq!(b.as_integer(), Some(10));
    }

    /// `mixed_type_ut.cpp::mixed_float`.
    #[test]
    fn mixed_float_value() {
        let v = MixedT::make_float(3.25);
        assert_eq!(v.value_type(), MixedValueType::Float);
        assert_eq!(v.as_float(), Some(3.25));
    }

    /// `mixed_type_ut.cpp::mixed_short_str`: the 14-byte inline boundary and
    /// `Reset`.
    #[test]
    fn mixed_short_string_boundary() {
        let v = MixedT::make_string("Hello");
        assert_eq!(v.value_type(), MixedValueType::ShortStr);
        assert_eq!(v.as_text(), Some("Hello"));

        let v = MixedT::make_string("Hello a World!");
        assert_eq!(v.as_text().unwrap().len(), 14);
        assert_eq!(v.value_type(), MixedValueType::ShortStr);

        let mut v = v;
        v.reset();
        assert_eq!(v.value_type(), MixedValueType::Invalid);
    }

    /// `mixed_type_ut.cpp::mixed_long_str`.
    #[test]
    fn mixed_long_string() {
        let v = MixedT::make_string("Hello the world");
        assert_eq!(v.value_type(), MixedValueType::LongStr);
        assert_eq!(v.as_text(), Some("Hello the world"));
    }

    /// `mixed_type_ut.cpp::mixed_null` / `mixed_miss`.
    #[test]
    fn mixed_null_and_missing_tags() {
        assert_eq!(MixedT::make_null().value_type(), MixedValueType::Null);
        assert_eq!(MixedT::make_missing().value_type(), MixedValueType::Missing);
    }

    /// `mixed_type_ut.cpp::mixed_tuple1` / `mixed_array1`: nesting and rendering.
    #[test]
    fn mixed_tuple_and_array_nest() {
        let tuple = MixedT::make_tuple(vec![MixedT::make_integer(1), MixedT::make_string("abc")]);
        assert_eq!(tuple.value_type(), MixedValueType::Tuple);
        assert_eq!(tuple.text(), "[1,abc]");

        let array = MixedT::make_array(vec![tuple.clone(), MixedT::make_float(0.5)]);
        assert_eq!(array.value_type(), MixedValueType::Array);
        assert_eq!(array.text(), "[[1,abc],0.5]");
    }
    /// `mixed_type_ut.cpp::mixed_tuple2`: a 7-slot named tuple with a nested
    /// tuple under key6.
    #[test]
    fn mixed_named_tuple_matches_the_reference() {
        let mut tuple = MixedT::make_tuple_with_count(7).unwrap();
        assert_eq!(tuple.value_type(), MixedValueType::Tuple);
        assert_eq!(tuple.slot_count(), Some(7));
        // Inserting into a tuple through the array API is an error.
        assert!(tuple.insert_into_array(0, MixedT::make_integer(1)).is_err());

        tuple
            .insert_into_tuple("key1", MixedT::make_integer(100))
            .unwrap();
        tuple
            .insert_into_tuple("key2", MixedT::make_float(1.1))
            .unwrap();
        tuple
            .insert_into_tuple("key3", MixedT::make_string("Hello World !!"))
            .unwrap();
        tuple
            .insert_into_tuple("key4", MixedT::make_string("Hello World Hello World"))
            .unwrap();
        tuple
            .insert_into_tuple("key5", MixedT::make_null())
            .unwrap();

        assert_eq!(
            tuple.get_from_tuple("key1").unwrap().as_integer(),
            Some(100)
        );
        assert_eq!(tuple.get_from_tuple("key2").unwrap().as_float(), Some(1.1));
        assert_eq!(
            tuple.get_from_tuple("key3").unwrap().value_type(),
            MixedValueType::ShortStr
        );
        assert_eq!(
            tuple.get_from_tuple("key4").unwrap().value_type(),
            MixedValueType::LongStr
        );
        assert_eq!(
            tuple.get_from_tuple("key5").unwrap().value_type(),
            MixedValueType::Null
        );

        let mut key6 = MixedT::make_tuple_with_count(5).unwrap();
        key6.insert_into_tuple("key61", MixedT::make_integer(200))
            .unwrap();
        key6.insert_into_tuple("key62", MixedT::make_float(2.2))
            .unwrap();
        key6.insert_into_tuple("key63", MixedT::make_string("Hello"))
            .unwrap();
        key6.insert_into_tuple("key64", MixedT::make_missing())
            .unwrap();
        key6.insert_into_tuple("key65", MixedT::make_null())
            .unwrap();
        tuple.insert_into_tuple("key6", key6).unwrap();
        let nested = tuple.get_from_tuple("key6").unwrap();
        assert_eq!(nested.value_type(), MixedValueType::Tuple);
        assert_eq!(
            nested.get_from_tuple("key61").unwrap().as_integer(),
            Some(200)
        );
        // A full tuple rejects further inserts.
        tuple
            .insert_into_tuple("key7", MixedT::make_integer(1))
            .unwrap();
        assert!(tuple
            .insert_into_tuple("key8", MixedT::make_integer(1))
            .is_err());
    }

    /// `mixed_type_ut.cpp::mixed_tuple3`: duplicate keys, overflow and a
    /// zero-capacity tuple are errors.
    #[test]
    fn mixed_tuple_errors_match_the_reference() {
        assert!(MixedT::make_tuple_with_count(0).is_err());
        let mut tuple = MixedT::make_tuple_with_count(3).unwrap();
        tuple
            .insert_into_tuple("key1key1", MixedT::make_integer(300))
            .unwrap();
        assert!(tuple
            .insert_into_tuple("key1key1", MixedT::make_integer(400))
            .is_err());
        tuple
            .insert_into_tuple("key1key1key1key1key1key2", MixedT::make_string("ABC"))
            .unwrap();
        assert_eq!(
            tuple
                .get_from_tuple("key1key1key1key1key1key2")
                .unwrap()
                .as_text(),
            Some("ABC")
        );
        assert!(tuple
            .insert_into_tuple("key1key1key1key1key1key2", MixedT::make_integer(900))
            .is_err());
        tuple
            .insert_into_tuple(
                "key1key1key1key1key1key3",
                MixedT::make_string("ABCABCABCABCABCABCABCABCABC"),
            )
            .unwrap();
        assert_eq!(
            tuple
                .get_from_tuple("key1key1key1key1key1key3")
                .unwrap()
                .value_type(),
            MixedValueType::LongStr
        );
        assert!(tuple
            .insert_into_tuple("keyX", MixedT::make_string("ABC"))
            .is_err());
    }

    /// `mixed_type_ut.cpp::mixed_array1`: positional slots and bounds checks.
    #[test]
    fn mixed_array_matches_the_reference() {
        let mut array = MixedT::make_array_with_count(7).unwrap();
        assert_eq!(array.value_type(), MixedValueType::Array);
        assert_eq!(array.slot_count(), Some(7));
        // Inserting into an array through the tuple API is an error.
        assert!(array
            .insert_into_tuple("key1", MixedT::make_integer(10))
            .is_err());

        array
            .insert_into_array(0, MixedT::make_integer(300))
            .unwrap();
        array.insert_into_array(1, MixedT::make_float(1.5)).unwrap();
        array
            .insert_into_array(2, MixedT::make_string("HappyHalloween"))
            .unwrap();
        array
            .insert_into_array(3, MixedT::make_string("HappyHalloween OK"))
            .unwrap();
        array.insert_into_array(4, MixedT::make_null()).unwrap();

        assert_eq!(array.get_by_index(0).unwrap().as_integer(), Some(300));
        assert_eq!(array.get_by_index(1).unwrap().as_float(), Some(1.5));
        assert_eq!(
            array.get_by_index(2).unwrap().as_text(),
            Some("HappyHalloween")
        );
        assert_eq!(
            array.get_by_index(3).unwrap().value_type(),
            MixedValueType::LongStr
        );
        assert_eq!(
            array.get_by_index(4).unwrap().value_type(),
            MixedValueType::Null
        );
        assert!(array.get_by_index(5).is_none());
        assert!(array.insert_into_array(7, MixedT::make_integer(1)).is_err());
    }

    /// `mixed_type_ut.cpp::mixed_array2`: slots 5 and 6 hold a nested tuple and
    /// a nested array.
    #[test]
    fn mixed_array_nests_tuple_and_array() {
        let mut array = MixedT::make_array_with_count(7).unwrap();
        array
            .insert_into_array(0, MixedT::make_integer(300))
            .unwrap();

        let mut tuple5 = MixedT::make_tuple_with_count(5).unwrap();
        tuple5
            .insert_into_tuple("key51", MixedT::make_integer(250))
            .unwrap();
        tuple5
            .insert_into_tuple("key52", MixedT::make_float(2.5))
            .unwrap();
        tuple5
            .insert_into_tuple("key53", MixedT::make_string("ABCDEFGHIJ"))
            .unwrap();
        tuple5
            .insert_into_tuple("key54", MixedT::make_string("ABCDEFGHIJKLMNOPQRST"))
            .unwrap();
        tuple5
            .insert_into_tuple("key55", MixedT::make_null())
            .unwrap();
        array.insert_into_array(5, tuple5).unwrap();

        let mut nested = MixedT::make_array_with_count(5).unwrap();
        nested
            .insert_into_array(0, MixedT::make_integer(3000))
            .unwrap();
        nested
            .insert_into_array(1, MixedT::make_float(1.5))
            .unwrap();
        nested
            .insert_into_array(2, MixedT::make_string("MNOPQRSTUVWXYZ"))
            .unwrap();
        nested
            .insert_into_array(3, MixedT::make_string("LMNOPQRSTUVWXYZ"))
            .unwrap();
        nested.insert_into_array(4, MixedT::make_null()).unwrap();
        array.insert_into_array(6, nested).unwrap();

        let t = array.get_by_index(5).unwrap();
        assert_eq!(t.value_type(), MixedValueType::Tuple);
        assert_eq!(t.slot_count(), Some(5));
        assert_eq!(t.get_from_tuple("key51").unwrap().as_integer(), Some(250));
        assert_eq!(
            t.get_from_tuple("key54").unwrap().value_type(),
            MixedValueType::LongStr
        );

        let a = array.get_by_index(6).unwrap();
        assert_eq!(a.value_type(), MixedValueType::Array);
        assert_eq!(a.slot_count(), Some(5));
        assert_eq!(a.get_by_index(0).unwrap().as_integer(), Some(3000));
        assert_eq!(
            a.get_by_index(3).unwrap().value_type(),
            MixedValueType::LongStr
        );
    }

    /// `mixed_type_ut.cpp::mixed_array3`: inserts overwrite the slot; bad
    /// indices and a zero capacity are errors.
    #[test]
    fn mixed_array_overwrites_and_bounds() {
        let mut array = MixedT::make_array_with_count(2).unwrap();
        array
            .insert_into_array(0, MixedT::make_integer(300))
            .unwrap();
        array
            .insert_into_array(0, MixedT::make_string("XXXXXXXXXXXXXXXXXXXXXXXX"))
            .unwrap();
        assert_eq!(
            array.get_by_index(0).unwrap().value_type(),
            MixedValueType::LongStr
        );
        array.insert_into_array(0, MixedT::make_float(4.3)).unwrap();
        assert_eq!(array.get_by_index(0).unwrap().as_float(), Some(4.3));
        array.insert_into_array(1, MixedT::make_float(1.5)).unwrap();
        assert_eq!(array.get_by_index(1).unwrap().as_float(), Some(1.5));
        assert!(array.insert_into_array(2, MixedT::make_float(1.5)).is_err());
        assert!(MixedT::make_array_with_count(0).is_err());
    }
}

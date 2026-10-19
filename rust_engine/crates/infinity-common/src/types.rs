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

/// C++: `IntervalT` (`src/parser/type/datetime/interval_type.h`).
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord)]
pub struct IntervalT {
    pub value: i32,
}

impl IntervalT {
    pub fn new(value: i32) -> Self {
        Self { value }
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

/// C++: `MixedT` (`src/parser/type/mixed_type.h`): the heterogeneous value's
/// rendered form (the executor keeps typed values in `Value`).
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct MixedT(pub String);

impl MixedT {
    pub fn new(value: impl Into<String>) -> Self {
        Self(value.into())
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
    Mixed,
    /// Embedded vector of `dim` f32 elements.
    Embedding(u32),
    Null,
}

impl LogicalType {
    /// Fixed inline width in bytes, or `None` for variable-length types.
    pub fn fixed_width(&self) -> Option<usize> {
        use LogicalType::*;
        Some(match self {
            Boolean | TinyInt => 1,
            SmallInt => 2,
            Integer | Float => 4,
            BigInt | Double | DateTime | Timestamp => 8,
            HugeInt => 16,
            Decimal => 16,
            Uuid => 16,
            Embedding(dim) => (*dim as usize) * 4,
            Varchar | Array | Mixed | Date | Time | Interval | Invalid | Null => {
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
        write!(f, "{self:?}")
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
}

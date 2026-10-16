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

/// Logical column type (subset).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum LogicalType {
    Invalid,
    Boolean,
    TinyInt,
    SmallInt,
    Integer,
    BigInt,
    Float,
    Double,
    Varchar,
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
            BigInt | Double => 8,
            Embedding(dim) => (*dim as usize) * 4,
            Varchar | Invalid | Null => return None,
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

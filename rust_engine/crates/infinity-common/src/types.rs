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

/// C++: `PointType` (`src/parser/type/geo/point_type.h`).
#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct PointT {
    pub x: f64,
    pub y: f64,
}

impl PointT {
    pub fn new(x: f64, y: f64) -> Self {
        Self { x, y }
    }
}

/// C++: `LineType`: `ax + by + c = 0`.
#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct LineT {
    pub a: f64,
    pub b: f64,
    pub c: f64,
}

impl LineT {
    pub fn new(a: f64, b: f64, c: f64) -> Self {
        Self { a, b, c }
    }
}

/// C++: `LineSegmentType`: two endpoints.
#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct LineSegT {
    pub point1: PointT,
    pub point2: PointT,
}

impl LineSegT {
    pub fn new(point1: PointT, point2: PointT) -> Self {
        Self { point1, point2 }
    }
}

/// C++: `BoxType`: upper-left and lower-right corners.
#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct BoxT {
    pub upper_left: PointT,
    pub lower_right: PointT,
}

impl BoxT {
    pub fn new(upper_left: PointT, lower_right: PointT) -> Self {
        Self {
            upper_left,
            lower_right,
        }
    }
}

/// C++: `CircleType`: center and radius.
#[derive(Debug, Clone, Copy, Default, PartialEq)]
pub struct CircleT {
    pub center: PointT,
    pub radius: f64,
}

impl CircleT {
    pub fn new(center: PointT, radius: f64) -> Self {
        Self { center, radius }
    }
}

/// C++: `PathType`: a point list with an optional closing flag.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct PathT {
    pub points: Vec<PointT>,
    pub closed: bool,
}

impl PathT {
    pub fn new(points: Vec<PointT>, closed: bool) -> Self {
        Self { points, closed }
    }
}

/// C++: `PolygonType`: a point list and its bounding box.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct PolygonT {
    pub points: Vec<PointT>,
    pub bounding_box: BoxT,
}

impl PolygonT {
    pub fn new(points: Vec<PointT>) -> Self {
        let mut upper_left = PointT::new(f64::INFINITY, f64::INFINITY);
        let mut lower_right = PointT::new(f64::NEG_INFINITY, f64::NEG_INFINITY);
        for point in &points {
            upper_left.x = upper_left.x.min(point.x);
            upper_left.y = upper_left.y.min(point.y);
            lower_right.x = lower_right.x.max(point.x);
            lower_right.y = lower_right.y.max(point.y);
        }
        if points.is_empty() {
            upper_left = PointT::default();
            lower_right = PointT::default();
        }
        Self {
            points,
            bounding_box: BoxT::new(upper_left, lower_right),
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

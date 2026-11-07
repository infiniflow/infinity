//! Scalar cast kernel ported from `src/function/cast/**`.
//!
//! The C++ reference expresses casts with per-target template
//! specialisations. Rust has no full specialisation, so the same behaviour is
//! modelled with small traits plus a `TypeId` check for the "can't cast to the
//! same type" rule. Errors keep the reference's two severities:
//! `Recoverable` (`RecoverableException`) and `Unrecoverable`
//! (`UnrecoverableException`).

use std::any::TypeId;

use crate::types::{DataType, DecimalT, HugeIntT, LogicalType, TypeInfo};

/// C++ `RecoverableException` / `UnrecoverableException` on the cast path.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CastError {
    Recoverable(String),
    Unrecoverable(String),
}

impl CastError {
    pub fn is_recoverable(&self) -> bool {
        matches!(self, CastError::Recoverable(_))
    }

    pub fn is_unrecoverable(&self) -> bool {
        matches!(self, CastError::Unrecoverable(_))
    }

    fn not_support(from: LogicalType, to: LogicalType) -> Self {
        CastError::Recoverable(format!(
            "Attempt to cast from {} to {}",
            from.type_name(),
            to.type_name()
        ))
    }
}

/// Result of one element-wise numeric cast: the C++ `Run` returns `false` for
/// out-of-range values and throws `UnrecoverableException` for unsupported
/// combinations.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NumericCastError {
    OutOfRange,
    Unrecoverable,
}

/// The kind of bound cast function (`BoundCastFunc`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CastOp {
    Fixed,
    Varlen,
}

/// C++ `IntegerTryCastToFixlen`: sources are all Rust integer widths plus
/// `HugeIntT`.
pub trait IntSource: Copy + 'static {
    /// C++ has no `HugeIntT` source specialisations; every conversion throws.
    const IS_HUGE: bool = false;
    fn as_i128(self) -> i128;
}

impl IntSource for i8 {
    fn as_i128(self) -> i128 {
        i128::from(self)
    }
}
impl IntSource for i16 {
    fn as_i128(self) -> i128 {
        i128::from(self)
    }
}
impl IntSource for i32 {
    fn as_i128(self) -> i128 {
        i128::from(self)
    }
}
impl IntSource for i64 {
    fn as_i128(self) -> i128 {
        i128::from(self)
    }
}
impl IntSource for HugeIntT {
    const IS_HUGE: bool = true;
    fn as_i128(self) -> i128 {
        HugeIntT::to_i128(self)
    }
}

pub trait IntTarget: Sized + 'static {
    fn try_from_i128(value: i128) -> Result<Self, NumericCastError>;
}

macro_rules! int_target {
    ($ty:ty) => {
        impl IntTarget for $ty {
            fn try_from_i128(value: i128) -> Result<Self, NumericCastError> {
                if value < i128::from(<$ty>::MIN) || value > i128::from(<$ty>::MAX) {
                    return Err(NumericCastError::OutOfRange);
                }
                Ok(value as $ty)
            }
        }
    };
}
int_target!(i8);
int_target!(i16);
int_target!(i32);
int_target!(i64);

impl IntTarget for HugeIntT {
    fn try_from_i128(value: i128) -> Result<Self, NumericCastError> {
        Ok(HugeIntT::from_i128(value))
    }
}

impl IntTarget for f32 {
    fn try_from_i128(value: i128) -> Result<Self, NumericCastError> {
        Ok(value as f32)
    }
}

impl IntTarget for f64 {
    fn try_from_i128(value: i128) -> Result<Self, NumericCastError> {
        Ok(value as f64)
    }
}

impl IntTarget for DecimalT {
    fn try_from_i128(_: i128) -> Result<Self, NumericCastError> {
        // C++ `IntegerTryCastToFixlen::Run(..., DecimalT&)` is "Not implemented".
        Err(NumericCastError::Unrecoverable)
    }
}

/// C++ `IntegerTryCastToFixlen::Run`.
pub fn integer_try_cast_to_fixlen<S: IntSource, T: IntTarget>(
    source: S,
) -> Result<T, NumericCastError> {
    if S::IS_HUGE {
        return Err(NumericCastError::Unrecoverable);
    }
    if TypeId::of::<S>() == TypeId::of::<T>() {
        // C++ has no same-type specialisation: the generic `Run` throws.
        return Err(NumericCastError::Unrecoverable);
    }
    T::try_from_i128(source.as_i128())
}

/// C++ `IntegerTryCastToVarlen::Run`.
pub fn integer_try_cast_to_varlen<S: IntSource>(source: S) -> Result<String, NumericCastError> {
    if S::IS_HUGE {
        return Err(NumericCastError::Unrecoverable);
    }
    let text = source.as_i128().to_string();
    // TinyInt/SmallInt/Integer keep the inline-string length guard; BigInt
    // appends to the column vector and has no such limit.
    if TypeId::of::<S>() != TypeId::of::<i64>() && text.len() > VARCHAR_INLINE_LEN {
        return Err(NumericCastError::Unrecoverable);
    }
    Ok(text)
}

const VARCHAR_INLINE_LEN: usize = 13;

/// C++ `BindIntegerCast<SourceType>`.
pub fn bind_integer_cast(source: LogicalType, target: LogicalType) -> Result<CastOp, CastError> {
    if source == target {
        return Err(CastError::Unrecoverable(
            "Can't cast from the same type".into(),
        ));
    }
    match target {
        LogicalType::TinyInt
        | LogicalType::SmallInt
        | LogicalType::Integer
        | LogicalType::BigInt
        | LogicalType::HugeInt
        | LogicalType::Float
        | LogicalType::Double
        | LogicalType::Decimal => Ok(CastOp::Fixed),
        LogicalType::Varchar => Ok(CastOp::Varlen),
        _ => Err(CastError::not_support(source, target)),
    }
}

/// C++ `BindBoolCast`.
pub fn bind_bool_cast(source: LogicalType, target: LogicalType) -> Result<CastOp, CastError> {
    if source != LogicalType::Boolean {
        return Err(CastError::Unrecoverable(format!(
            "Expect boolean type, but it is {}",
            source.type_name()
        )));
    }
    match target {
        LogicalType::Varchar => Ok(CastOp::Varlen),
        _ => Err(CastError::Recoverable(format!(
            "Can't cast from Boolean to {}",
            target.type_name()
        ))),
    }
}

/// C++ `TryCastBoolean::Run`: there is no boolean-to-boolean cast.
pub fn bool_try_cast_bool(_source: bool) -> Result<bool, CastError> {
    Err(CastError::Recoverable(
        "No implementation to cast from Boolean to Boolean".into(),
    ))
}

/// C++ `BoolTryCastToVarlen` (currently commented out in the reference, kept
/// for the day it is enabled).
pub fn bool_try_cast_to_varlen(source: bool) -> String {
    if source { "true" } else { "false" }.to_string()
}

/// C++ `FloatTryCastToFixlen` / `FloatTryCastToVarlen` sources.
pub trait FloatSource: Copy + 'static {
    const IS_F32: bool = false;
    fn to_f64(self) -> f64;
}

impl FloatSource for f32 {
    const IS_F32: bool = true;
    fn to_f64(self) -> f64 {
        f64::from(self)
    }
}

impl FloatSource for f64 {
    fn to_f64(self) -> f64 {
        self
    }
}

pub trait FloatTarget: Sized + 'static {
    fn from_f64(value: f64, source_is_f32: bool) -> Result<Self, NumericCastError>;
}

macro_rules! float_to_int_target {
    ($ty:ty, $min:expr, $max:expr) => {
        impl FloatTarget for $ty {
            fn from_f64(value: f64, _source_is_f32: bool) -> Result<Self, NumericCastError> {
                if !($min..=$max).contains(&value) {
                    return Err(NumericCastError::OutOfRange);
                }
                Ok(value as $ty)
            }
        }
    };
}
float_to_int_target!(i8, -128.0, 127.0);
float_to_int_target!(i16, -32768.0, 32767.0);

impl FloatTarget for i32 {
    fn from_f64(value: f64, source_is_f32: bool) -> Result<Self, NumericCastError> {
        // The reference uses an exclusive upper bound for FloatT and an
        // inclusive one for DoubleT.
        let in_range = if source_is_f32 {
            (-2147483648.0..2147483648.0).contains(&value)
        } else {
            (-2147483648.0..=2147483647.0).contains(&value)
        };
        if !in_range {
            return Err(NumericCastError::OutOfRange);
        }
        Ok(value.round_ties_even() as i32)
    }
}

impl FloatTarget for i64 {
    fn from_f64(value: f64, source_is_f32: bool) -> Result<Self, NumericCastError> {
        if source_is_f32 {
            // `9223372036854775807.0f` rounds up to 2^63, so the FloatT
            // specialisation accepts exactly 2^63 (the C++ cast is UB there).
            if !(-9223372036854775808.0..=9223372036854775808.0).contains(&value) {
                return Err(NumericCastError::OutOfRange);
            }
            Ok(value.round_ties_even() as i64)
        } else {
            if !(-9223372036854775808.0..9223372036854775808.0).contains(&value) {
                return Err(NumericCastError::OutOfRange);
            }
            let target = value.round_ties_even() as i64;
            if value > 0.0 && target < 0 {
                return Err(NumericCastError::OutOfRange);
            }
            Ok(target)
        }
    }
}

impl FloatTarget for HugeIntT {
    fn from_f64(_: f64, _: bool) -> Result<Self, NumericCastError> {
        Err(NumericCastError::Unrecoverable)
    }
}

impl FloatTarget for DecimalT {
    fn from_f64(_: f64, _: bool) -> Result<Self, NumericCastError> {
        Err(NumericCastError::Unrecoverable)
    }
}

impl FloatTarget for f32 {
    fn from_f64(value: f64, _: bool) -> Result<Self, NumericCastError> {
        Ok(value as f32)
    }
}

impl FloatTarget for f64 {
    fn from_f64(value: f64, _: bool) -> Result<Self, NumericCastError> {
        Ok(value)
    }
}

/// C++ `FloatTryCastToFixlen::Run`.
pub fn float_try_cast_to_fixlen<S: FloatSource, T: FloatTarget>(
    source: S,
) -> Result<T, NumericCastError> {
    if TypeId::of::<S>() == TypeId::of::<T>() {
        return Err(NumericCastError::Unrecoverable);
    }
    T::from_f64(source.to_f64(), S::IS_F32)
}

/// C++ `FloatTryCastToVarlen::Run`: `std::to_string` always prints six
/// fractional digits.
pub fn float_try_cast_to_varlen<S: FloatSource>(source: S) -> String {
    format!("{:.6}", source.to_f64())
}

/// C++ `BindFloatCast<SourceType>`.
pub fn bind_float_cast(source: LogicalType, target: LogicalType) -> Result<CastOp, CastError> {
    if source == target {
        return Err(CastError::Unrecoverable(
            "Can't cast from the same type".into(),
        ));
    }
    match target {
        LogicalType::TinyInt
        | LogicalType::SmallInt
        | LogicalType::Integer
        | LogicalType::BigInt
        | LogicalType::Float
        | LogicalType::Double => Ok(CastOp::Fixed),
        LogicalType::Varchar => Ok(CastOp::Varlen),
        LogicalType::Decimal => Err(CastError::Unrecoverable(
            "Not implement cast from numeric to decimal128 type.".into(),
        )),
        LogicalType::Boolean | LogicalType::Embedding(_) => {
            Err(CastError::not_support(source, target))
        }
        _ => Err(CastError::Unrecoverable(format!(
            "Attempt to cast from {} to {}",
            source.type_name(),
            target.type_name()
        ))),
    }
}

/// C++ `BindDecimalCast`: every target is unimplemented and throws
/// `UnrecoverableException`.
pub fn bind_decimal_cast(source: LogicalType, target: LogicalType) -> Result<CastOp, CastError> {
    let _ = (source, target);
    Err(CastError::Unrecoverable("Not implemented".into()))
}

/// C++ `DecimalTryCastToFixlen` / `DecimalTryCastToVarlen`.
pub fn decimal_try_cast_to_fixlen<T>(_source: DecimalT) -> Result<T, NumericCastError> {
    Err(NumericCastError::Unrecoverable)
}

/// C++ `DecimalTryCastToVarlen`.
pub fn decimal_try_cast_to_varlen(_source: DecimalT) -> Result<String, NumericCastError> {
    Err(NumericCastError::Unrecoverable)
}

/// C++ `BindDateCast`.
pub fn bind_date_cast(target: LogicalType) -> Result<CastOp, CastError> {
    match target {
        LogicalType::DateTime | LogicalType::Timestamp => Ok(CastOp::Fixed),
        LogicalType::Varchar => Ok(CastOp::Varlen),
        _ => Err(CastError::Unrecoverable(format!(
            "Can't cast from Date type to {}",
            target.type_name()
        ))),
    }
}

/// The datetime family's element casts are all declared but "Not implemented"
/// in the reference.
pub fn datetime_try_cast_fixlen<T>(_source: impl std::fmt::Debug) -> Result<T, NumericCastError> {
    Err(NumericCastError::Unrecoverable)
}

pub fn datetime_try_cast_varlen(_source: impl std::fmt::Debug) -> Result<String, NumericCastError> {
    Err(NumericCastError::Unrecoverable)
}

/// C++ `BindTimeCast` / `BindIntervalCast` / `BindTimestampCast` /
/// `BindDateTimeCast`.
pub fn bind_temporal_cast(source: LogicalType, target: LogicalType) -> Result<CastOp, CastError> {
    let allowed = match source {
        LogicalType::Time | LogicalType::Interval => matches!(target, LogicalType::Varchar),
        LogicalType::Timestamp => matches!(
            target,
            LogicalType::Date | LogicalType::Time | LogicalType::DateTime | LogicalType::Varchar
        ),
        LogicalType::DateTime => matches!(
            target,
            LogicalType::Date | LogicalType::Time | LogicalType::Timestamp | LogicalType::Varchar
        ),
        _ => false,
    };
    if allowed {
        Ok(if target == LogicalType::Varchar {
            CastOp::Varlen
        } else {
            CastOp::Fixed
        })
    } else {
        Err(CastError::Unrecoverable(format!(
            "Can't cast from {} type to {}",
            source.type_name(),
            target.type_name()
        )))
    }
}

/// C++ `TryCastVarchar` targets.
pub trait VarcharTarget: Sized {
    fn parse_varchar(text: &str) -> Option<Self>;
}

impl VarcharTarget for bool {
    fn parse_varchar(text: &str) -> Option<Self> {
        if text.len() == 4 && text.eq_ignore_ascii_case("true") {
            Some(true)
        } else if text.len() == 5 && text.eq_ignore_ascii_case("false") {
            Some(false)
        } else {
            None
        }
    }
}

macro_rules! varchar_int_target {
    ($ty:ty) => {
        impl VarcharTarget for $ty {
            fn parse_varchar(text: &str) -> Option<Self> {
                text.parse::<$ty>().ok()
            }
        }
    };
}
varchar_int_target!(i8);
varchar_int_target!(i16);
varchar_int_target!(i32);
varchar_int_target!(i64);

macro_rules! varchar_float_target {
    ($ty:ty) => {
        impl VarcharTarget for $ty {
            fn parse_varchar(text: &str) -> Option<Self> {
                text.parse::<$ty>().ok()
            }
        }
    };
}
varchar_float_target!(f32);
varchar_float_target!(f64);

/// C++ `TryCastVarchar::Run` (also used by `TryCastVarcharVector`).
pub fn varchar_try_cast<T: VarcharTarget>(text: &str) -> Option<T> {
    T::parse_varchar(text)
}

/// C++ `BindEmbeddingCast`.
pub fn bind_embedding_cast(source: &DataType, target: &DataType) -> Result<CastOp, CastError> {
    let source_info = match (source.ty(), source.info()) {
        (LogicalType::Embedding(_), Some(TypeInfo::Embedding(info))) => info,
        _ => {
            return Err(CastError::Recoverable(format!(
                "Attempt to cast from {} to {}",
                source, target
            )))
        }
    };
    let target_info = match (target.ty(), target.info()) {
        (LogicalType::Embedding(_), Some(TypeInfo::Embedding(info))) => info,
        _ => {
            return Err(CastError::Recoverable(format!(
                "Attempt to cast from {} to {}",
                source, target
            )))
        }
    };
    if source_info.dimension() != target_info.dimension() {
        return Err(CastError::Recoverable(format!(
            "Attempt to cast from {} to {}",
            source, target
        )));
    }
    Ok(CastOp::Fixed)
}

/// C++ `EmbeddingTryCastToFixlen` for the f32 -> f64 element pair.
pub fn embedding_try_cast_f32_to_f64(values: &[f32]) -> Vec<f64> {
    values.iter().map(|v| f64::from(*v)).collect()
}

/// C++ `CastTable::GetCastCost` for the rows exercised by the unit tests.
pub fn cast_cost(source: LogicalType, target: LogicalType) -> i32 {
    if source == LogicalType::Boolean {
        return match target {
            LogicalType::Boolean => 0,
            LogicalType::Varchar => 100,
            _ => -1,
        };
    }
    -1
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::types::{
        DataType, DecimalT, EmbeddingDataType, EmbeddingInfo, HugeIntT, LogicalType, TypeInfo,
    };

    /// C++ `DEFAULT_VECTOR_SIZE`.
    const VECTOR_SIZE: i64 = 8192;

    fn dt(ty: LogicalType) -> DataType {
        DataType::new(ty)
    }

    fn huge(lower: i64) -> HugeIntT {
        HugeIntT::new(-((lower < 0) as i64), lower)
    }

    fn cast_int_column<S: IntSource, T: IntTarget>(values: &[S]) -> (bool, Vec<Option<T>>) {
        let mut all = true;
        let out = values
            .iter()
            .map(|&value| match integer_try_cast_to_fixlen::<S, T>(value) {
                Ok(cast) => Some(cast),
                Err(NumericCastError::OutOfRange) => {
                    all = false;
                    None
                }
                Err(NumericCastError::Unrecoverable) => {
                    panic!("unexpected unrecoverable integer cast")
                }
            })
            .collect();
        (all, out)
    }

    fn cast_float_column<S: FloatSource, T: FloatTarget>(values: &[S]) -> (bool, Vec<Option<T>>) {
        let mut all = true;
        let out = values
            .iter()
            .map(|&value| match float_try_cast_to_fixlen::<S, T>(value) {
                Ok(cast) => Some(cast),
                Err(NumericCastError::OutOfRange) => {
                    all = false;
                    None
                }
                Err(NumericCastError::Unrecoverable) => {
                    panic!("unexpected unrecoverable float cast")
                }
            })
            .collect();
        (all, out)
    }

    // C++: src/unit_test/function/cast/bool_cast_ut.cpp:bool_cast0
    #[test]
    fn bool_cast0() {
        // Boolean to Boolean throws.
        let err = bool_try_cast_bool(true).unwrap_err();
        assert!(err.is_recoverable());
    }

    // C++: src/unit_test/function/cast/bool_cast_ut.cpp:bool_cast1
    #[test]
    fn bool_cast1() {
        let column: Vec<bool> = (0..VECTOR_SIZE).map(|i| i % 2 == 0).collect();
        for (i, value) in column.iter().enumerate() {
            assert_eq!(*value, i as i64 % 2 == 0);
        }
        assert_eq!(
            bind_bool_cast(LogicalType::Boolean, LogicalType::Varchar),
            Ok(CastOp::Varlen)
        );
        let err = bind_bool_cast(LogicalType::Boolean, LogicalType::Boolean).unwrap_err();
        assert!(err.is_recoverable());
        assert_eq!(bool_try_cast_to_varlen(true), "true");
        assert_eq!(bool_try_cast_to_varlen(false), "false");
    }

    // C++: src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp:tiny_integer_cast0
    #[test]
    fn tiny_integer_cast0() {
        let source: i8 = 0;
        assert!(matches!(
            integer_try_cast_to_fixlen::<i8, i8>(source),
            Err(NumericCastError::Unrecoverable)
        ));

        for value in [i8::MIN, i8::MAX, 0] {
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, i16>(value),
                Ok(i16::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, i32>(value),
                Ok(i32::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, i64>(value),
                Ok(i64::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, HugeIntT>(value),
                Ok(huge(i64::from(value)))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, f32>(value),
                Ok(f32::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i8, f64>(value),
                Ok(f64::from(value))
            );
        }
        assert!(matches!(
            integer_try_cast_to_fixlen::<i8, DecimalT>(i8::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [i8::MIN, i8::MAX, 0, 9, 10, 99, -100, 100] {
            let text = integer_try_cast_to_varlen(value).unwrap();
            assert_eq!(text, value.to_string());
        }
    }

    // C++: src/unit_test/function/cast/integer_cast/tiny_integer_cast_ut.cpp:tiny_integer_cast1
    #[test]
    fn tiny_integer_cast1() {
        let column: Vec<i8> = (0..VECTOR_SIZE).map(|i| i as i8).collect();
        let (all, out) = cast_int_column::<i8, i16>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(i16::from(column[i])));
        }
        let (all, out) = cast_int_column::<i8, i32>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(i32::from(column[i])));
        }
        let (all, out) = cast_int_column::<i8, i64>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(i64::from(column[i])));
        }
        let (all, out) = cast_int_column::<i8, HugeIntT>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(huge(i64::from(column[i]))));
        }
        let (all, out) = cast_int_column::<i8, f32>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(f32::from(column[i])));
        }
        let (all, out) = cast_int_column::<i8, f64>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(f64::from(column[i])));
        }
        assert!(matches!(
            integer_try_cast_to_fixlen::<i8, DecimalT>(1),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in &column {
            assert_eq!(
                integer_try_cast_to_varlen(*value).unwrap(),
                value.to_string()
            );
        }
        assert!(
            bind_integer_cast(LogicalType::TinyInt, LogicalType::Timestamp)
                .unwrap_err()
                .is_recoverable()
        );
    }

    // C++: src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp:small_integer_cast0
    #[test]
    fn small_integer_cast0() {
        assert!(matches!(
            integer_try_cast_to_fixlen::<i16, i16>(0),
            Err(NumericCastError::Unrecoverable)
        ));
        // Narrowing range checks.
        assert_eq!(
            integer_try_cast_to_fixlen::<i16, i8>(i16::MIN),
            Err(NumericCastError::OutOfRange)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i16, i8>(i16::MAX),
            Err(NumericCastError::OutOfRange)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i16, i8>(i16::from(i8::MAX)),
            Ok(i8::MAX)
        );
        for value in [i16::MIN, i16::MAX, 0] {
            assert_eq!(
                integer_try_cast_to_fixlen::<i16, i32>(value),
                Ok(i32::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i16, i64>(value),
                Ok(i64::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i16, HugeIntT>(value),
                Ok(huge(i64::from(value)))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i16, f32>(value),
                Ok(f32::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i16, f64>(value),
                Ok(f64::from(value))
            );
        }
        assert!(matches!(
            integer_try_cast_to_fixlen::<i16, DecimalT>(i16::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [i16::MIN, i16::MAX, 0, 9, 10, 99, -100, 100] {
            assert_eq!(
                integer_try_cast_to_varlen(value).unwrap(),
                value.to_string()
            );
        }
    }

    // C++: src/unit_test/function/cast/integer_cast/small_integer_cast_ut.cpp:small_integer_cast1
    #[test]
    fn small_integer_cast1() {
        let column: Vec<i16> = (0..VECTOR_SIZE).map(|i| i as i16).collect();
        let (all, out) = cast_int_column::<i16, i8>(&column);
        // Values 0..=127 fit; the rest are out of range.
        assert!(!all);
        for (i, value) in out.iter().enumerate() {
            if column[i] <= i8::MAX as i16 {
                assert_eq!(*value, Some(column[i] as i8));
            } else {
                assert_eq!(*value, None);
            }
        }
        let (all, _) = cast_int_column::<i16, i32>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i16, i64>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i16, HugeIntT>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i16, f32>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i16, f64>(&column);
        assert!(all);
        assert!(matches!(
            integer_try_cast_to_fixlen::<i16, DecimalT>(1),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in &column {
            assert_eq!(
                integer_try_cast_to_varlen(*value).unwrap(),
                value.to_string()
            );
        }
        assert!(
            bind_integer_cast(LogicalType::SmallInt, LogicalType::Timestamp)
                .unwrap_err()
                .is_recoverable()
        );
    }

    // C++: src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp:integer_cast0
    #[test]
    fn integer_cast0() {
        assert!(matches!(
            integer_try_cast_to_fixlen::<i32, i32>(0),
            Err(NumericCastError::Unrecoverable)
        ));
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i8>(i32::MIN),
            Err(NumericCastError::OutOfRange)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i8>(i32::MAX),
            Err(NumericCastError::OutOfRange)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i8>(i32::from(i8::MAX)),
            Ok(i8::MAX)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i8>(i32::from(i8::MIN)),
            Ok(i8::MIN)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i16>(i32::MIN),
            Err(NumericCastError::OutOfRange)
        );
        assert_eq!(
            integer_try_cast_to_fixlen::<i32, i16>(i32::MAX),
            Err(NumericCastError::OutOfRange)
        );
        for value in [i32::MIN, i32::MAX, 0] {
            assert_eq!(
                integer_try_cast_to_fixlen::<i32, i64>(value),
                Ok(i64::from(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i32, HugeIntT>(value),
                Ok(huge(i64::from(value)))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i32, f32>(value),
                Ok(value as f32)
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i32, f64>(value),
                Ok(f64::from(value))
            );
        }
        assert!(matches!(
            integer_try_cast_to_fixlen::<i32, DecimalT>(i32::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [i32::MIN, i32::MAX, 0, 9, 10, 99, -100, 100] {
            assert_eq!(
                integer_try_cast_to_varlen(value).unwrap(),
                value.to_string()
            );
        }
    }

    // C++: src/unit_test/function/cast/integer_cast/integer_cast_ut.cpp:integer_cast1
    #[test]
    fn integer_cast1() {
        let column: Vec<i32> = (0..VECTOR_SIZE).map(|i| i as i32).collect();
        let (all, out) = cast_int_column::<i32, i8>(&column);
        assert!(!all);
        for (i, value) in out.iter().enumerate() {
            if column[i] <= i8::MAX as i32 {
                assert_eq!(*value, Some(column[i] as i8));
            } else {
                assert_eq!(*value, None);
            }
        }
        let (all, out) = cast_int_column::<i32, i16>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i16));
        }
        let (all, _) = cast_int_column::<i32, i64>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i32, HugeIntT>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i32, f32>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i32, f64>(&column);
        assert!(all);
        assert!(matches!(
            integer_try_cast_to_fixlen::<i32, DecimalT>(1),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in &column {
            assert_eq!(
                integer_try_cast_to_varlen(*value).unwrap(),
                value.to_string()
            );
        }
        assert!(
            bind_integer_cast(LogicalType::Integer, LogicalType::Timestamp)
                .unwrap_err()
                .is_recoverable()
        );
    }

    // C++: src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp:bigint_cast0
    #[test]
    fn bigint_cast0() {
        assert!(matches!(
            integer_try_cast_to_fixlen::<i64, i64>(0),
            Err(NumericCastError::Unrecoverable)
        ));
        for (value, expected) in [
            (i64::MIN, Err(NumericCastError::OutOfRange)),
            (i64::MAX, Err(NumericCastError::OutOfRange)),
            (i64::from(i8::MAX), Ok(i8::MAX)),
            (i64::from(i8::MIN), Ok(i8::MIN)),
            (0, Ok(0)),
        ] {
            assert_eq!(integer_try_cast_to_fixlen::<i64, i8>(value), expected);
        }
        for (value, expected) in [
            (i64::MIN, Err(NumericCastError::OutOfRange)),
            (i64::MAX, Err(NumericCastError::OutOfRange)),
            (0, Ok(0)),
            (i64::from(i16::MAX), Ok(i16::MAX)),
            (i64::from(i16::MIN), Ok(i16::MIN)),
        ] {
            assert_eq!(integer_try_cast_to_fixlen::<i64, i16>(value), expected);
        }
        for (value, expected) in [
            (i64::MIN, Err(NumericCastError::OutOfRange)),
            (i64::MAX, Err(NumericCastError::OutOfRange)),
            (0, Ok(0)),
            (i64::from(i32::MAX), Ok(i32::MAX)),
            (i64::from(i32::MIN), Ok(i32::MIN)),
        ] {
            assert_eq!(integer_try_cast_to_fixlen::<i64, i32>(value), expected);
        }
        for value in [i64::MIN, i64::MAX, 0] {
            assert_eq!(
                integer_try_cast_to_fixlen::<i64, HugeIntT>(value),
                Ok(huge(value))
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i64, f32>(value),
                Ok(value as f32)
            );
            assert_eq!(
                integer_try_cast_to_fixlen::<i64, f64>(value),
                Ok(value as f64)
            );
        }
        assert!(matches!(
            integer_try_cast_to_fixlen::<i64, DecimalT>(i64::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [i64::MIN, i64::MAX, 0, 9, 10, 99, -100, 100] {
            assert_eq!(
                integer_try_cast_to_varlen(value).unwrap(),
                value.to_string()
            );
        }
    }

    // C++: src/unit_test/function/cast/integer_cast/bigint_cast_ut.cpp:bigint_cast1
    #[test]
    fn bigint_cast1() {
        let column: Vec<i64> = (0..VECTOR_SIZE).collect();
        let (all, out) = cast_int_column::<i64, i8>(&column);
        assert!(!all);
        for (i, value) in out.iter().enumerate() {
            if column[i] <= i64::from(i8::MAX) {
                assert_eq!(*value, Some(column[i] as i8));
            } else {
                assert_eq!(*value, None);
            }
        }
        let (all, out) = cast_int_column::<i64, i16>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i16));
        }
        let (all, out) = cast_int_column::<i64, i32>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i32));
        }
        let (all, out) = cast_int_column::<i64, HugeIntT>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(HugeIntT::new(0, column[i])));
        }
        let (all, _) = cast_int_column::<i64, f32>(&column);
        assert!(all);
        let (all, _) = cast_int_column::<i64, f64>(&column);
        assert!(all);
        assert!(matches!(
            integer_try_cast_to_fixlen::<i64, DecimalT>(1),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in &column {
            assert_eq!(
                integer_try_cast_to_varlen(*value).unwrap(),
                value.to_string()
            );
        }
        assert!(
            bind_integer_cast(LogicalType::BigInt, LogicalType::Timestamp)
                .unwrap_err()
                .is_recoverable()
        );
    }

    // C++: src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp:hugeint_cast0
    #[test]
    fn hugeint_cast0() {
        let source = HugeIntT::new(0, 0);
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, HugeIntT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        let huge_source = HugeIntT::new(i64::MAX, i64::MAX);
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, i8>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, i16>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, i32>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, i64>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, f32>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, f64>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, DecimalT>(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_varlen(huge_source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/integer_cast/hugeint_cast_ut.cpp:hugeint_cast1
    #[test]
    fn hugeint_cast1() {
        let source = HugeIntT::new(0, 0);
        for target in [
            LogicalType::TinyInt,
            LogicalType::SmallInt,
            LogicalType::Integer,
            LogicalType::BigInt,
            LogicalType::Float,
            LogicalType::Double,
            LogicalType::Decimal,
            LogicalType::Varchar,
        ] {
            assert!(bind_integer_cast(LogicalType::HugeInt, target).is_ok());
        }
        assert!(
            bind_integer_cast(LogicalType::BigInt, LogicalType::Timestamp)
                .unwrap_err()
                .is_recoverable()
        );
        // Every element cast throws, exactly like the C++ column function.
        assert!(matches!(
            integer_try_cast_to_fixlen::<HugeIntT, i8>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            integer_try_cast_to_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/float/float_cast_ut.cpp:float_cast0
    #[test]
    fn float_cast0() {
        assert!(matches!(
            float_try_cast_to_fixlen::<f32, f32>(0.0),
            Err(NumericCastError::Unrecoverable)
        ));
        for (value, expected) in [
            (f32::MIN, None),
            (f32::MAX, None),
            (127.0, Some(127i8)),
            (-128.0, Some(-128i8)),
            (0.0, Some(0)),
            (128.0, None),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f32, i8>(value).ok(), expected);
        }
        for (value, expected) in [
            (f32::MIN, None),
            (f32::MAX, None),
            (0.0, Some(0i16)),
            (32767.0, Some(i16::MAX)),
            (-32768.0, Some(i16::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f32, i16>(value).ok(), expected);
        }
        for (value, expected) in [
            (f32::MIN, None),
            (f32::MAX, None),
            (0.0, Some(0i32)),
            (i32::MAX as f32, None),
            (i32::MIN as f32, Some(i32::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f32, i32>(value).ok(), expected);
        }
        for (value, expected) in [
            (f32::MIN, None),
            (f32::MAX, None),
            (0.0, Some(0i64)),
            (i64::MAX as f32, Some(i64::MAX)),
            (i64::MIN as f32, Some(i64::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f32, i64>(value).ok(), expected);
        }
        assert!(matches!(
            float_try_cast_to_fixlen::<f32, HugeIntT>(f32::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [f32::MIN, i32::MAX as f32, 0.0] {
            assert_eq!(
                float_try_cast_to_fixlen::<f32, f64>(value),
                Ok(f64::from(value))
            );
        }
        assert!(matches!(
            float_try_cast_to_fixlen::<f32, DecimalT>(f32::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [f32::MIN, f32::MAX, 0.0, 9.0, 10.0, 99.0, -100.0, 100.0] {
            assert_eq!(
                float_try_cast_to_varlen(value),
                format!("{:.6}", f64::from(value))
            );
        }
    }

    // C++: src/unit_test/function/cast/float/float_cast_ut.cpp:float_cast1
    #[test]
    fn float_cast1() {
        let column: Vec<f32> = (0..VECTOR_SIZE).map(|i| i as f32).collect();
        let (all, out) = cast_float_column::<f32, i8>(&column);
        assert!(!all);
        for (i, value) in out.iter().enumerate() {
            if column[i] <= 127.0 {
                assert_eq!(*value, Some(column[i] as i8));
            } else {
                assert_eq!(*value, None);
            }
        }
        let (all, out) = cast_float_column::<f32, i16>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i16));
        }
        let (all, _) = cast_float_column::<f32, i32>(&column);
        assert!(all);
        let (all, _) = cast_float_column::<f32, i64>(&column);
        assert!(all);
        assert!(matches!(
            float_try_cast_to_fixlen::<f32, HugeIntT>(1.0),
            Err(NumericCastError::Unrecoverable)
        ));
        let (all, out) = cast_float_column::<f32, f64>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(f64::from(column[i])));
        }
        for value in &column {
            assert_eq!(
                float_try_cast_to_varlen(*value),
                format!("{:.6}", f64::from(*value))
            );
        }
        assert!(matches!(
            bind_float_cast(LogicalType::Float, LogicalType::Timestamp),
            Err(CastError::Unrecoverable(_))
        ));
    }

    // C++: src/unit_test/function/cast/float/double_cast_ut.cpp:double_cast0
    #[test]
    fn double_cast0() {
        assert!(matches!(
            float_try_cast_to_fixlen::<f64, f64>(0.0),
            Err(NumericCastError::Unrecoverable)
        ));
        for (value, expected) in [
            (f64::MIN, None),
            (f64::MAX, None),
            (127.0, Some(127i8)),
            (-128.0, Some(-128i8)),
            (0.0, Some(0i8)),
            (128.0, None),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f64, i8>(value).ok(), expected);
        }
        for (value, expected) in [
            (f64::MIN, None),
            (f64::MAX, None),
            (0.0, Some(0i16)),
            (32767.0, Some(i16::MAX)),
            (-32768.0, Some(i16::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f64, i16>(value).ok(), expected);
        }
        for (value, expected) in [
            (f64::MIN, None),
            (f64::MAX, None),
            (0.0, Some(0i32)),
            (f64::from(i32::MAX), Some(i32::MAX)),
            (f64::from(i32::MIN), Some(i32::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f64, i32>(value).ok(), expected);
        }
        for (value, expected) in [
            (f64::MIN, None),
            (f64::MAX, None),
            (0.0, Some(0i64)),
            (i64::MAX as f64, None),
            (i64::MIN as f64, Some(i64::MIN)),
        ] {
            assert_eq!(float_try_cast_to_fixlen::<f64, i64>(value).ok(), expected);
        }
        assert!(matches!(
            float_try_cast_to_fixlen::<f64, HugeIntT>(f64::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
        for value in [f64::MIN, f64::from(i32::MAX), 0.0] {
            assert_eq!(
                float_try_cast_to_fixlen::<f64, f32>(value),
                Ok(value as f32)
            );
        }
        assert!(matches!(
            float_try_cast_to_fixlen::<f64, DecimalT>(f64::MIN),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/float/double_cast_ut.cpp:double_cast1
    #[test]
    fn double_cast1() {
        let column: Vec<f64> = (0..VECTOR_SIZE).map(|i| i as f64).collect();
        let (all, out) = cast_float_column::<f64, i8>(&column);
        assert!(!all);
        for (i, value) in out.iter().enumerate() {
            if column[i] <= 127.0 {
                assert_eq!(*value, Some(column[i] as i8));
            } else {
                assert_eq!(*value, None);
            }
        }
        let (all, out) = cast_float_column::<f64, i16>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i16));
        }
        let (all, out) = cast_float_column::<f64, i32>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i32));
        }
        let (all, out) = cast_float_column::<f64, i64>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as i64));
        }
        let (all, out) = cast_float_column::<f64, f32>(&column);
        assert!(all);
        for (i, value) in out.iter().enumerate() {
            assert_eq!(*value, Some(column[i] as f32));
        }
        for value in &column {
            assert_eq!(float_try_cast_to_varlen(*value), format!("{value:.6}"));
        }
        assert!(matches!(
            bind_float_cast(LogicalType::Double, LogicalType::Timestamp),
            Err(CastError::Unrecoverable(_))
        ));
    }

    // C++: src/unit_test/function/cast/decimal/decimal_cast_ut.cpp:decimal_cast0
    #[test]
    fn decimal_cast0() {
        let source = DecimalT::default();
        assert!(matches!(
            decimal_try_cast_to_fixlen::<i8>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<i16>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<i32>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<i64>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<HugeIntT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<f32>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<f64>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_fixlen::<DecimalT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            decimal_try_cast_to_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/decimal/decimal_cast_ut.cpp:decimal_cast1
    #[test]
    fn decimal_cast1() {
        for target in [
            LogicalType::TinyInt,
            LogicalType::SmallInt,
            LogicalType::Integer,
            LogicalType::BigInt,
            LogicalType::HugeInt,
            LogicalType::Float,
            LogicalType::Double,
            LogicalType::Decimal,
            LogicalType::Varchar,
            LogicalType::Timestamp,
        ] {
            assert!(matches!(
                bind_decimal_cast(LogicalType::Decimal, target),
                Err(CastError::Unrecoverable(_))
            ));
        }
    }

    // C++: src/unit_test/function/cast/varchar_cast_ut.cpp:varchar_cast0
    #[test]
    fn varchar_cast0() {
        assert_eq!(varchar_try_cast::<bool>("true"), Some(true));
        assert_eq!(varchar_try_cast::<bool>("false"), Some(false));
        assert_eq!(varchar_try_cast::<bool>("falsk"), None);
        assert_eq!(varchar_try_cast::<i8>("-128"), Some(-128));
        assert_eq!(varchar_try_cast::<i8>("127"), Some(127));
        assert_eq!(varchar_try_cast::<i8>("190"), None);
        assert_eq!(varchar_try_cast::<i8>("abc"), None);
    }

    // C++: src/unit_test/function/cast/varchar_cast_ut.cpp:varchar_cast1
    #[test]
    #[allow(clippy::excessive_precision)]
    fn varchar_cast1() {
        for (text, expected) in [
            ("100", 100i64),
            ("-100", -100),
            ("1000000000000000", 1_000_000_000_000_000),
            ("-1000000000000000", -1_000_000_000_000_000),
        ] {
            assert_eq!(varchar_try_cast::<i64>(text), Some(expected));
        }
        for (text, expected) in [
            ("100.01", 100.01f32),
            ("-100.02", -100.02),
            ("100000.345", 100000.345),
            ("-1000000000.4565544", -1000000000.4565544),
        ] {
            assert_eq!(varchar_try_cast::<f32>(text), Some(expected));
        }
        for (text, expected) in [
            ("100100100.01", 100100100.01f64),
            ("-100.02", -100.02),
            ("100000345345.345", 100000345345.345),
            ("-1000000000345345.4565544", -1000000000345345.4565544),
        ] {
            assert_eq!(varchar_try_cast::<f64>(text), Some(expected));
        }
    }

    // C++: src/unit_test/function/cast/datetime/date_cast_ut.cpp:date_cast0
    #[test]
    fn date_cast0() {
        assert!(matches!(
            datetime_try_cast_fixlen::<i8>(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_varlen(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::DateTimeT>(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::TimestampT>(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_varlen(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/date_cast_ut.cpp:date_cast1
    #[test]
    fn date_cast1() {
        assert!(bind_date_cast(LogicalType::Decimal)
            .unwrap_err()
            .is_unrecoverable());
        let column: Vec<crate::DateT> = (0..VECTOR_SIZE)
            .map(|i| crate::DateT::new(i as i32))
            .collect();
        for (i, value) in column.iter().enumerate() {
            assert_eq!(value.value, i as i32);
        }
        for target in [LogicalType::DateTime, LogicalType::Timestamp] {
            assert_eq!(bind_date_cast(target), Ok(CastOp::Fixed));
            assert!(matches!(
                datetime_try_cast_fixlen::<crate::DateTimeT>(crate::DateT::default()),
                Err(NumericCastError::Unrecoverable)
            ));
        }
        assert_eq!(bind_date_cast(LogicalType::Varchar), Ok(CastOp::Varlen));
        assert!(matches!(
            datetime_try_cast_varlen(crate::DateT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/datetime_cast_ut.cpp:datetime_cast0
    #[test]
    fn datetime_cast0() {
        let source = crate::DateTimeT::default();
        assert!(matches!(
            datetime_try_cast_fixlen::<i8>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::DateT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::TimeT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::TimestampT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/datetime_cast_ut.cpp:datetime_cast1
    #[test]
    fn datetime_cast1() {
        assert!(
            bind_temporal_cast(LogicalType::DateTime, LogicalType::Decimal)
                .unwrap_err()
                .is_unrecoverable()
        );
        for target in [
            LogicalType::Date,
            LogicalType::Time,
            LogicalType::Timestamp,
            LogicalType::Varchar,
        ] {
            assert!(bind_temporal_cast(LogicalType::DateTime, target).is_ok());
        }
    }

    // C++: src/unit_test/function/cast/datetime/time_cast_ut.cpp:date_cast0
    #[test]
    fn time_cast0() {
        let source = crate::TimeT::default();
        assert!(matches!(
            datetime_try_cast_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/time_cast_ut.cpp:date_cast1
    #[test]
    fn time_cast1() {
        assert!(bind_temporal_cast(LogicalType::Time, LogicalType::Decimal)
            .unwrap_err()
            .is_unrecoverable());
        assert_eq!(
            bind_temporal_cast(LogicalType::Time, LogicalType::Varchar),
            Ok(CastOp::Varlen)
        );
        let column: Vec<crate::TimeT> = (0..VECTOR_SIZE)
            .map(|i| crate::TimeT::new(i as i32))
            .collect();
        for (i, value) in column.iter().enumerate() {
            assert_eq!(value.value, i as i32);
        }
        assert!(matches!(
            datetime_try_cast_varlen(crate::TimeT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp:datetime_cast0
    #[test]
    fn timestamp_cast0() {
        let source = crate::TimestampT::default();
        assert!(matches!(
            datetime_try_cast_fixlen::<i8>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_varlen(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::DateT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::TimeT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
        assert!(matches!(
            datetime_try_cast_fixlen::<crate::DateTimeT>(source),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/timestamp_cast_ut.cpp:datetime_cast1
    #[test]
    fn timestamp_cast1() {
        assert!(
            bind_temporal_cast(LogicalType::Timestamp, LogicalType::Decimal)
                .unwrap_err()
                .is_unrecoverable()
        );
        for target in [
            LogicalType::Date,
            LogicalType::Time,
            LogicalType::DateTime,
            LogicalType::Varchar,
        ] {
            assert!(bind_temporal_cast(LogicalType::Timestamp, target).is_ok());
        }
    }

    // C++: src/unit_test/function/cast/datetime/interval_cast_ut.cpp:date_cast0
    #[test]
    fn interval_cast0() {
        assert!(matches!(
            datetime_try_cast_varlen(crate::IntervalT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/datetime/interval_cast_ut.cpp:date_cast1
    #[test]
    fn interval_cast1() {
        assert!(
            bind_temporal_cast(LogicalType::Interval, LogicalType::Decimal)
                .unwrap_err()
                .is_unrecoverable()
        );
        assert_eq!(
            bind_temporal_cast(LogicalType::Interval, LogicalType::Varchar),
            Ok(CastOp::Varlen)
        );
        let column: Vec<crate::IntervalT> = (0..VECTOR_SIZE)
            .map(|i| crate::IntervalT::new(i as i32))
            .collect();
        for (i, value) in column.iter().enumerate() {
            assert_eq!(value.value, i as i32);
        }
        assert!(matches!(
            datetime_try_cast_varlen(crate::IntervalT::default()),
            Err(NumericCastError::Unrecoverable)
        ));
    }

    // C++: src/unit_test/function/cast/cast_table_ut.cpp:casttable_boolean
    #[test]
    fn casttable_boolean() {
        for target in [
            LogicalType::Boolean,
            LogicalType::TinyInt,
            LogicalType::SmallInt,
            LogicalType::Integer,
            LogicalType::BigInt,
            LogicalType::HugeInt,
            LogicalType::Float,
            LogicalType::Double,
            LogicalType::Decimal,
            LogicalType::Varchar,
            LogicalType::Date,
            LogicalType::Time,
            LogicalType::DateTime,
            LogicalType::Timestamp,
            LogicalType::Interval,
            LogicalType::Uuid,
            LogicalType::Array,
            LogicalType::Tuple,
            LogicalType::Mixed,
            LogicalType::Embedding(0),
            LogicalType::RowID,
            LogicalType::Null,
            LogicalType::Missing,
            LogicalType::Invalid,
        ] {
            match target {
                LogicalType::Boolean => assert_eq!(cast_cost(LogicalType::Boolean, target), 0),
                LogicalType::Varchar => assert_eq!(cast_cost(LogicalType::Boolean, target), 100),
                _ => assert_eq!(cast_cost(LogicalType::Boolean, target), -1),
            }
        }
    }

    // C++: src/unit_test/function/cast/embedding_cast_ut.cpp:embedding_cast1
    #[test]
    fn embedding_cast1() {
        assert!(matches!(
            bind_embedding_cast(&dt(LogicalType::Decimal), &dt(LogicalType::Decimal)),
            Err(CastError::Recoverable(_))
        ));

        let source_info = EmbeddingInfo::make(EmbeddingDataType::Float, 16).unwrap();
        let source =
            DataType::with_info(LogicalType::Embedding(16), TypeInfo::Embedding(source_info));
        let rows: Vec<Vec<f32>> = (0..VECTOR_SIZE)
            .map(|i| (0..16).map(|j| i as f32 + j as f32 + 0.5).collect())
            .collect();
        assert_eq!(rows[0], (0..16).map(|j| j as f32 + 0.5).collect::<Vec<_>>());

        let target_info = EmbeddingInfo::make(EmbeddingDataType::Double, 16).unwrap();
        let target =
            DataType::with_info(LogicalType::Embedding(16), TypeInfo::Embedding(target_info));
        assert_eq!(bind_embedding_cast(&source, &target), Ok(CastOp::Fixed));
        let converted = embedding_try_cast_f32_to_f64(&rows[0]);
        let expected: Vec<f64> = (0..16).map(|j| j as f64 + 0.5).collect();
        assert_eq!(converted, expected);
    }
}

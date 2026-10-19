//! Typed scalar function sets, mirroring the reference's
//! `ScalarFunctionSet` / `ScalarFunction` and `RegisterAddFunction`
//! (`src/function/scalar/add_impl.cpp`).
//!
//! The executor's row evaluator is untyped (`Value::Int(i64)`); this module is
//! the typed view used by the scalar-function unit tests: overload resolution
//! produces the reference's `+(TinyInt, TinyInt)->TinyInt` signature, and
//! [`add`] performs the per-width checked arithmetic of
//! `ScalarFunction::BinaryFunctionWithFailure<..., AddFunction>` — an overflow
//! is a NULL result, not a wrapped value.

use std::fmt;

/// The reference's `LogicalType`, limited to the scalar overloads registered
/// for `+`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum ScalarType {
    TinyInt,
    SmallInt,
    Integer,
    BigInt,
    HugeInt,
    Float16,
    BFloat16,
    Float,
    Double,
    Decimal,
    Varchar,
    Date,
    Time,
    DateTime,
    Timestamp,
    Interval,
    /// C++ `kMixed`; `LogicalType2Str` renders it as `Heterogeneous`.
    Mixed,
}

impl ScalarType {
    /// C++ `LogicalType2Str`.
    pub fn type_name(self) -> &'static str {
        match self {
            ScalarType::TinyInt => "TinyInt",
            ScalarType::SmallInt => "SmallInt",
            ScalarType::Integer => "Integer",
            ScalarType::BigInt => "BigInt",
            ScalarType::HugeInt => "HugeInt",
            ScalarType::Float16 => "Float16",
            ScalarType::BFloat16 => "BFloat16",
            ScalarType::Float => "Float",
            ScalarType::Double => "Double",
            ScalarType::Decimal => "Decimal",
            ScalarType::Varchar => "Varchar",
            ScalarType::Date => "Date",
            ScalarType::Time => "Time",
            ScalarType::DateTime => "DateTime",
            ScalarType::Timestamp => "Timestamp",
            ScalarType::Interval => "Interval",
            ScalarType::Mixed => "Heterogeneous",
        }
    }
}

/// C++ `ScalarFunction`: a concrete overload of one function name.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ScalarFunction {
    pub name: String,
    pub arguments: Vec<ScalarType>,
    pub return_type: ScalarType,
}

impl fmt::Display for ScalarFunction {
    /// C++ `ScalarFunction::ToString`: `+(TinyInt, TinyInt)->TinyInt`.
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let args: Vec<&str> = self.arguments.iter().map(|t| t.type_name()).collect();
        write!(
            f,
            "{}({})->{}",
            self.name,
            args.join(", "),
            self.return_type.type_name()
        )
    }
}

/// C++ `ScalarFunctionSet`: the registered overloads of one function name.
#[derive(Debug, Clone, Default)]
pub struct ScalarFunctionSet {
    pub name: String,
    pub functions: Vec<ScalarFunction>,
}

impl ScalarFunctionSet {
    /// `RegisterAddFunction`: every `+` overload in registration order.
    pub fn add_function_set() -> Self {
        use ScalarType::*;
        let binary = |arguments: [ScalarType; 2], return_type: ScalarType| ScalarFunction {
            name: "+".to_string(),
            arguments: arguments.to_vec(),
            return_type,
        };
        Self {
            name: "+".to_string(),
            functions: vec![
                binary([TinyInt, TinyInt], TinyInt),
                binary([SmallInt, SmallInt], SmallInt),
                binary([Integer, Integer], Integer),
                binary([BigInt, BigInt], BigInt),
                binary([HugeInt, HugeInt], HugeInt),
                binary([Float16, Float16], Float16),
                binary([BFloat16, BFloat16], BFloat16),
                binary([Float, Float], Float),
                binary([Double, Double], Double),
                binary([Decimal, Decimal], Decimal),
                binary([Date, Interval], Date),
                binary([Interval, Date], Date),
                binary([Time, Interval], Time),
                binary([Interval, Time], Time),
                binary([DateTime, Interval], DateTime),
                binary([Interval, DateTime], DateTime),
                binary([Timestamp, Interval], Timestamp),
                binary([Interval, Timestamp], Timestamp),
                binary([Mixed, BigInt], Mixed),
                binary([BigInt, Mixed], Mixed),
                binary([Mixed, Double], Mixed),
                binary([Double, Mixed], Mixed),
                binary([Mixed, Mixed], Mixed),
            ],
        }
    }

    /// C++ `ScalarFunctionSet::GetMostMatchFunction`: the overload whose
    /// argument types match exactly.
    pub fn most_match_function(&self, arguments: &[ScalarType]) -> Option<&ScalarFunction> {
        self.functions
            .iter()
            .find(|function| function.arguments == arguments)
    }
}

/// One typed input/result column; a `None` slot is NULL.
#[derive(Debug, Clone, PartialEq)]
pub enum TypedColumn {
    TinyInt(Vec<Option<i8>>),
    SmallInt(Vec<Option<i16>>),
    Integer(Vec<Option<i32>>),
    BigInt(Vec<Option<i64>>),
    Float(Vec<Option<f32>>),
    Double(Vec<Option<f64>>),
}

impl TypedColumn {
    pub fn scalar_type(&self) -> ScalarType {
        match self {
            TypedColumn::TinyInt(_) => ScalarType::TinyInt,
            TypedColumn::SmallInt(_) => ScalarType::SmallInt,
            TypedColumn::Integer(_) => ScalarType::Integer,
            TypedColumn::BigInt(_) => ScalarType::BigInt,
            TypedColumn::Float(_) => ScalarType::Float,
            TypedColumn::Double(_) => ScalarType::Double,
        }
    }

    pub fn len(&self) -> usize {
        match self {
            TypedColumn::TinyInt(v) => v.len(),
            TypedColumn::SmallInt(v) => v.len(),
            TypedColumn::Integer(v) => v.len(),
            TypedColumn::BigInt(v) => v.len(),
            TypedColumn::Float(v) => v.len(),
            TypedColumn::Double(v) => v.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

/// C++ `AddFunction::Run` for every width with a registered overload:
/// `checked_add`, NULL when the result overflows or is not finite.
pub fn add(left: &TypedColumn, right: &TypedColumn) -> Option<TypedColumn> {
    Some(match (left, right) {
        (TypedColumn::TinyInt(a), TypedColumn::TinyInt(b)) => TypedColumn::TinyInt(
            a.iter()
                .zip(b)
                .map(|(x, y)| x.zip(*y).and_then(|(x, y)| x.checked_add(y)))
                .collect(),
        ),
        (TypedColumn::SmallInt(a), TypedColumn::SmallInt(b)) => TypedColumn::SmallInt(
            a.iter()
                .zip(b)
                .map(|(x, y)| x.zip(*y).and_then(|(x, y)| x.checked_add(y)))
                .collect(),
        ),
        (TypedColumn::Integer(a), TypedColumn::Integer(b)) => TypedColumn::Integer(
            a.iter()
                .zip(b)
                .map(|(x, y)| x.zip(*y).and_then(|(x, y)| x.checked_add(y)))
                .collect(),
        ),
        (TypedColumn::BigInt(a), TypedColumn::BigInt(b)) => TypedColumn::BigInt(
            a.iter()
                .zip(b)
                .map(|(x, y)| x.zip(*y).and_then(|(x, y)| x.checked_add(y)))
                .collect(),
        ),
        (TypedColumn::Float(a), TypedColumn::Float(b)) => TypedColumn::Float(
            a.iter()
                .zip(b)
                .map(|(x, y)| {
                    x.zip(*y)
                        .map(|(x, y)| x + y)
                        .filter(|sum| sum.is_finite())
                })
                .collect(),
        ),
        (TypedColumn::Double(a), TypedColumn::Double(b)) => TypedColumn::Double(
            a.iter()
                .zip(b)
                .map(|(x, y)| {
                    x.zip(*y)
                        .map(|(x, y)| x + y)
                        .filter(|sum| sum.is_finite())
                })
                .collect(),
        ),
        _ => return None,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    const ROW_COUNT: usize = 8192;

    /// C++: `AddFunctionsTest.add_func` (`add_functions_ut.cpp:56`).
    #[test]
    #[allow(clippy::approx_constant)]
    fn add_func() {
        let set = ScalarFunctionSet::add_function_set();
        assert_eq!(set.name, "+");

        // `GetMostMatchFunction(inputs).ToString()` for every overload the
        // reference asserts.
        let resolve = |args: &[ScalarType]| {
            set.most_match_function(args)
                .unwrap_or_else(|| panic!("no overload for {args:?}"))
                .to_string()
        };
        assert_eq!(
            resolve(&[ScalarType::TinyInt, ScalarType::TinyInt]),
            "+(TinyInt, TinyInt)->TinyInt"
        );
        assert_eq!(
            resolve(&[ScalarType::SmallInt, ScalarType::SmallInt]),
            "+(SmallInt, SmallInt)->SmallInt"
        );
        assert_eq!(
            resolve(&[ScalarType::Integer, ScalarType::Integer]),
            "+(Integer, Integer)->Integer"
        );
        assert_eq!(
            resolve(&[ScalarType::BigInt, ScalarType::BigInt]),
            "+(BigInt, BigInt)->BigInt"
        );
        assert_eq!(
            resolve(&[ScalarType::Float, ScalarType::Float]),
            "+(Float, Float)->Float"
        );
        assert_eq!(
            resolve(&[ScalarType::Double, ScalarType::Double]),
            "+(Double, Double)->Double"
        );
        assert_eq!(
            resolve(&[ScalarType::Decimal, ScalarType::Decimal]),
            "+(Decimal, Decimal)->Decimal"
        );
        assert_eq!(
            resolve(&[ScalarType::DateTime, ScalarType::Interval]),
            "+(DateTime, Interval)->DateTime"
        );
        assert_eq!(
            resolve(&[ScalarType::Interval, ScalarType::DateTime]),
            "+(Interval, DateTime)->DateTime"
        );
        assert_eq!(
            resolve(&[ScalarType::Timestamp, ScalarType::Interval]),
            "+(Timestamp, Interval)->Timestamp"
        );
        assert_eq!(
            resolve(&[ScalarType::Interval, ScalarType::Timestamp]),
            "+(Interval, Timestamp)->Timestamp"
        );
        assert_eq!(
            resolve(&[ScalarType::Mixed, ScalarType::BigInt]),
            "+(Heterogeneous, BigInt)->Heterogeneous"
        );
        assert_eq!(
            resolve(&[ScalarType::BigInt, ScalarType::Mixed]),
            "+(BigInt, Heterogeneous)->Heterogeneous"
        );
        assert_eq!(
            resolve(&[ScalarType::Mixed, ScalarType::Double]),
            "+(Heterogeneous, Double)->Heterogeneous"
        );
        assert_eq!(
            resolve(&[ScalarType::Double, ScalarType::Mixed]),
            "+(Double, Heterogeneous)->Heterogeneous"
        );

        // TinyInt: the reference appends `(i8)i`, then expects NULL exactly
        // when `(i16)(i8)i * 2` does not fit in i8.
        let tiny_values: Vec<Option<i8>> = (0..ROW_COUNT).map(|i| Some(i as i8)).collect();
        for (i, value) in tiny_values.iter().enumerate() {
            assert_eq!(*value, Some(i as i8), "TinyInt input row {i}");
        }
        let input = TypedColumn::TinyInt(tiny_values);
        assert_eq!(input.scalar_type(), ScalarType::TinyInt);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::TinyInt);
        let TypedColumn::TinyInt(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = i16::from(i as i8) * 2;
            if res < i16::from(i8::MIN) || res > i16::from(i8::MAX) {
                assert_eq!(*got, None, "TinyInt row {i} must be NULL");
            } else {
                assert_eq!(*got, Some(res as i8), "TinyInt row {i}");
            }
        }

        // SmallInt.
        let small_values: Vec<Option<i16>> = (0..ROW_COUNT).map(|i| Some(i as i16)).collect();
        for (i, value) in small_values.iter().enumerate() {
            assert_eq!(*value, Some(i as i16), "SmallInt input row {i}");
        }
        let input = TypedColumn::SmallInt(small_values);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::SmallInt);
        let TypedColumn::SmallInt(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = i32::from(i as i16) * 2;
            if res < i32::from(i16::MIN) || res > i32::from(i16::MAX) {
                assert_eq!(*got, None, "SmallInt row {i} must be NULL");
            } else {
                assert_eq!(*got, Some(res as i16), "SmallInt row {i}");
            }
        }

        // Integer.
        let int_values: Vec<Option<i32>> = (0..ROW_COUNT).map(|i| Some(i as i32)).collect();
        for (i, value) in int_values.iter().enumerate() {
            assert_eq!(*value, Some(i as i32), "Integer input row {i}");
        }
        let input = TypedColumn::Integer(int_values);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::Integer);
        let TypedColumn::Integer(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = i64::from(i as i32) * 2;
            if res < i64::from(i32::MIN) || res > i64::from(i32::MAX) {
                assert_eq!(*got, None, "Integer row {i} must be NULL");
            } else {
                assert_eq!(*got, Some(res as i32), "Integer row {i}");
            }
        }

        // BigInt.
        let big_values: Vec<Option<i64>> = (0..ROW_COUNT).map(|i| Some(i as i64)).collect();
        for (i, value) in big_values.iter().enumerate() {
            assert_eq!(*value, Some(i as i64), "BigInt input row {i}");
        }
        let input = TypedColumn::BigInt(big_values);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::BigInt);
        let TypedColumn::BigInt(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = (i as i64).checked_mul(2);
            assert_eq!(*got, res, "BigInt row {i}");
        }

        // Float: `std::isinf(res) || std::isnan(res)` -> NULL.
        let float_values: Vec<Option<f32>> = (0..ROW_COUNT).map(|i| Some(i as f32)).collect();
        for (i, value) in float_values.iter().enumerate() {
            assert_eq!(*value, Some(i as f32), "Float input row {i}");
        }
        let input = TypedColumn::Float(float_values);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::Float);
        let TypedColumn::Float(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = i as f32 * 2.0;
            if res.is_infinite() || res.is_nan() {
                assert_eq!(*got, None, "Float row {i} must be NULL");
            } else {
                assert_eq!(*got, Some(res), "Float row {i}");
            }
        }

        // Double.
        let double_values: Vec<Option<f64>> = (0..ROW_COUNT).map(|i| Some(i as f64)).collect();
        for (i, value) in double_values.iter().enumerate() {
            assert_eq!(*value, Some(i as f64), "Double input row {i}");
        }
        let input = TypedColumn::Double(double_values);
        let result = add(&input, &input).unwrap();
        assert_eq!(result.scalar_type(), ScalarType::Double);
        let TypedColumn::Double(out) = &result else {
            unreachable!()
        };
        for (i, got) in out.iter().enumerate() {
            let res = i as f64 * 2.0;
            if res.is_infinite() || res.is_nan() {
                assert_eq!(*got, None, "Double row {i} must be NULL");
            } else {
                assert_eq!(*got, Some(res), "Double row {i}");
            }
        }

        // The cast wraparound is explicit at the boundaries the reference
        // exercises: row 128 stores i8::MIN and row 255 stores -1.
        let TypedColumn::TinyInt(tiny) = TypedColumn::TinyInt(
            (0..ROW_COUNT).map(|i| Some(i as i8)).collect(),
        ) else {
            unreachable!()
        };
        assert_eq!(tiny[127], Some(127));
        assert_eq!(tiny[128], Some(-128));
        assert_eq!(tiny[255], Some(-1));
        assert_eq!(tiny.len(), ROW_COUNT);
    }
}

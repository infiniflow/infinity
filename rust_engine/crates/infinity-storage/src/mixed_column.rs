//! Ports of `src/unit_test/storage/column_vector/column_vector_mixed_ut_nimpl.cpp`
//! (`ColumnVectorMixedTest`, 24 cases, TEST_AUDIT rows 388-411).
//!
//! The reference file is guarded by `#if 0` ("Mixed type is not implemented").
//! The Rust `MixedT` model and the `Column::Mixed` vector exist, so the same
//! structural assertions (flat / constant / selection / slice) are meaningful
//! here. Assertions that only inspect the reference's storage plumbing
//! (`data_type_size_`, `buffer_`, `nulls_ptr_`, `initialized`) have no Rust
//! counterpart and are omitted, as in the existing integer reference suite.

use crate::data_block::{ColumnVector, ColumnVectorType, DEFAULT_VECTOR_SIZE};
use infinity_common::types::MixedValueType;
use infinity_common::MixedT;

fn fvalue(src: i64) -> f64 {
    src as f64 + src as f64 / 10.0
}

/// C++ `mixed_short_str_*`: `std::to_string(i)` (1-4 ASCII digits).
fn short_string_value(src: i64) -> String {
    src.to_string()
}

/// C++ `mixed_tuple_*` / `mixed_array_*`: `String(10, 'a' + i % 26)`.
fn short_str(src: i64) -> String {
    std::iter::repeat_n((b'a' + (src % 26) as u8) as char, 10).collect()
}

fn long_str(src: i64) -> String {
    std::iter::repeat_n((b'a' + (src % 26) as u8) as char, 20).collect()
}

fn new_flat() -> ColumnVector<MixedT> {
    let mut v = ColumnVector::<MixedT>::new();
    v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
    assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
    assert_eq!(v.size(), 0);
    assert!(v.get(0).is_none());
    assert!(v.set_vector_type(ColumnVectorType::Flat).is_err());
    assert!(v.set_value(3, Some(MixedT::make_integer(3))).is_err());
    v
}

/// `Initialize()` + `AppendValue` over the whole vector, checking the value and
/// the out-of-range read after every append.
fn filled(make: impl Fn(i64) -> MixedT) -> ColumnVector<MixedT> {
    let mut v = new_flat();
    for i in 0..DEFAULT_VECTOR_SIZE as i64 {
        let value = make(i);
        v.append_value(Some(value.clone())).unwrap();
        assert_eq!(v.get(i as usize), Some(&value));
        assert!(v.get(i as usize + 1).is_none());
    }
    v
}

/// The `_a` shape: flat append, `ShallowCopy`, `Reset` + re-`Initialize`,
/// `AppendByPtr`, then the constant `SetValueByIndex(0)`/`Finalize(1)` loop.
fn flat_a(make: impl Fn(i64) -> MixedT, verify: impl Fn(&MixedT, i64)) {
    let mut v = filled(&make);

    let mut clone = ColumnVector::<MixedT>::new();
    clone.shallow_copy(&v);
    assert_eq!(clone.size(), v.size());
    assert_eq!(clone.capacity(), v.capacity());
    for i in 0..DEFAULT_VECTOR_SIZE as i64 {
        let expected = make(i);
        assert_eq!(clone.get(i as usize), Some(&expected));
        verify(&expected, i);
    }

    v.reset();
    assert_eq!(v.capacity(), 0);
    assert_eq!(v.size(), 0);
    v.initialize(ColumnVectorType::Flat, DEFAULT_VECTOR_SIZE);
    assert_eq!(v.capacity(), DEFAULT_VECTOR_SIZE);
    assert_eq!(v.size(), 0);
    assert!(v.get(0).is_none());

    for i in 0..DEFAULT_VECTOR_SIZE as i64 {
        let value = make(i);
        // C++ `AppendByPtr`: the pointed-to value is copied into the vector.
        v.push(Some(value.clone()));
        assert_eq!(v.get(i as usize), Some(&value));
        assert!(v.get(i as usize + 1).is_none());
        verify(&value, i);
    }

    for i in 0..DEFAULT_VECTOR_SIZE as i64 {
        let mut constant = ColumnVector::<MixedT>::new();
        constant.initialize(ColumnVectorType::Constant, DEFAULT_VECTOR_SIZE);
        constant.set_value(0, v.get(i as usize).cloned()).unwrap();
        constant.finalize(1);
        let got = constant.get(0).expect("constant vector holds one value");
        assert_eq!(got, &make(i));
        verify(got, i);
        constant.reset();
    }
}

fn select_a(make: impl Fn(i64) -> MixedT, verify: impl Fn(&MixedT, i64)) {
    let source = filled(&make);
    let selection: Vec<usize> = (0..DEFAULT_VECTOR_SIZE / 2).map(|i| i * 2).collect();
    let mut target = ColumnVector::<MixedT>::new();
    target.initialize_selection(&source, &selection);
    assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
    for i in 0..(DEFAULT_VECTOR_SIZE / 2) as i64 {
        let src = 2 * i;
        let expected = make(src);
        assert_eq!(target.get(i as usize), Some(&expected));
        verify(&expected, src);
    }
}

fn slice_a(make: impl Fn(i64) -> MixedT, verify: impl Fn(&MixedT, i64)) {
    let source = filled(&make);
    let start = DEFAULT_VECTOR_SIZE / 4;
    let end = 3 * DEFAULT_VECTOR_SIZE / 4;
    let mut target = ColumnVector::<MixedT>::new();
    target.initialize_slice(&source, start, end);
    assert_eq!(target.size(), DEFAULT_VECTOR_SIZE / 2);
    assert_eq!(end - start, DEFAULT_VECTOR_SIZE / 2);
    for i in 0..(end - start) as i64 {
        let src = start as i64 + i;
        let expected = make(src);
        assert_eq!(target.get(i as usize), Some(&expected));
        verify(&expected, src);
    }
}

fn integer_make(src: i64) -> MixedT {
    MixedT::make_integer(src)
}

fn integer_verify(value: &MixedT, src: i64) {
    assert_eq!(value.value_type(), MixedValueType::Integer);
    assert_eq!(value.as_integer(), Some(src));
}

fn float_make(src: i64) -> MixedT {
    MixedT::make_float(src as f64)
}

fn float_verify(value: &MixedT, src: i64) {
    assert_eq!(value.value_type(), MixedValueType::Float);
    assert_eq!(value.as_float(), Some(src as f64));
}

fn short_string_make(src: i64) -> MixedT {
    MixedT::make_string(&short_string_value(src))
}

fn short_string_verify(value: &MixedT, src: i64) {
    let expected = short_string_value(src);
    assert_eq!(value.value_type(), MixedValueType::ShortStr);
    assert_eq!(value.as_text(), Some(expected.as_str()));
}

fn long_string_make(src: i64) -> MixedT {
    MixedT::make_string(&long_str(src))
}

fn long_string_verify(value: &MixedT, src: i64) {
    let expected = long_str(src);
    assert_eq!(value.value_type(), MixedValueType::LongStr);
    assert_eq!(value.as_text(), Some(expected.as_str()));
    let text = value.as_text().expect("long string text");
    assert_eq!(text.len(), 20);
    // C++ `LongStrMixedType::header` caches the first
    // `BaseMixedType::LONG_STR_HEADER` (5) bytes.
    assert!(text.starts_with(&expected[..5]));
}

fn null_make(_src: i64) -> MixedT {
    MixedT::make_null()
}

fn null_verify(value: &MixedT, _src: i64) {
    assert_eq!(value.value_type(), MixedValueType::Null);
}

fn missing_make(_src: i64) -> MixedT {
    MixedT::make_missing()
}

fn missing_verify(value: &MixedT, _src: i64) {
    assert_eq!(value.value_type(), MixedValueType::Missing);
}

fn tuple_make(src: i64) -> MixedT {
    let fv = fvalue(src);
    let short = short_str(src);
    let long = long_str(src);

    let mut tuple = MixedT::make_tuple_with_count(7).expect("7-slot tuple");
    tuple.insert_integer_into_tuple("key1", src).unwrap();
    tuple.insert_float_into_tuple("key2", fv).unwrap();
    tuple.insert_string_into_tuple("key3", &short).unwrap();
    tuple.insert_string_into_tuple("key4", &long).unwrap();
    tuple.insert_null_into_tuple("key5").unwrap();

    let mut nested = MixedT::make_tuple_with_count(5).expect("5-slot tuple");
    nested.insert_integer_into_tuple("key61", src).unwrap();
    nested.insert_float_into_tuple("key62", fv).unwrap();
    nested.insert_string_into_tuple("key63", &short).unwrap();
    nested.insert_string_into_tuple("key64", &long).unwrap();
    nested.insert_null_into_tuple("key65").unwrap();
    tuple.copy_into_tuple("key6", &nested).unwrap();

    let mut array = MixedT::make_array_with_count(5).expect("5-slot array");
    array.insert_integer_into_array(src, 0).unwrap();
    array.insert_float_into_array(fv, 1).unwrap();
    array.insert_string_into_array(&short, 2).unwrap();
    array.insert_string_into_array(&long, 3).unwrap();
    array.insert_null_into_array(4).unwrap();
    tuple.copy_into_tuple("key7", &array).unwrap();

    tuple
}

fn tuple_verify(value: &MixedT, src: i64) {
    let fv = fvalue(src);
    let short = short_str(src);
    let long = long_str(src);

    assert_eq!(value.value_type(), MixedValueType::Tuple);
    assert_eq!(value.slot_count(), Some(7));

    let key1 = value.get_from_tuple("key1").expect("key1");
    assert_eq!(key1.value_type(), MixedValueType::Integer);
    assert_eq!(key1.as_integer(), Some(src));

    let key2 = value.get_from_tuple("key2").expect("key2");
    assert_eq!(key2.value_type(), MixedValueType::Float);
    assert_eq!(key2.as_float(), Some(fv));

    let key3 = value.get_from_tuple("key3").expect("key3");
    assert_eq!(key3.value_type(), MixedValueType::ShortStr);
    assert_eq!(key3.as_text(), Some(short.as_str()));

    let key4 = value.get_from_tuple("key4").expect("key4");
    assert_eq!(key4.value_type(), MixedValueType::LongStr);
    assert_eq!(key4.as_text(), Some(long.as_str()));

    let key5 = value.get_from_tuple("key5").expect("key5");
    assert_eq!(key5.value_type(), MixedValueType::Null);

    let key6 = value.get_from_tuple("key6").expect("key6");
    assert_eq!(key6.value_type(), MixedValueType::Tuple);
    assert_eq!(key6.slot_count(), Some(5));
    let nested1 = key6.get_from_tuple("key61").expect("key61");
    assert_eq!(nested1.value_type(), MixedValueType::Integer);
    assert_eq!(nested1.as_integer(), Some(src));
    let nested2 = key6.get_from_tuple("key62").expect("key62");
    assert_eq!(nested2.value_type(), MixedValueType::Float);
    assert_eq!(nested2.as_float(), Some(fv));
    let nested3 = key6.get_from_tuple("key63").expect("key63");
    assert_eq!(nested3.value_type(), MixedValueType::ShortStr);
    assert_eq!(nested3.as_text(), Some(short.as_str()));
    let nested4 = key6.get_from_tuple("key64").expect("key64");
    assert_eq!(nested4.value_type(), MixedValueType::LongStr);
    assert_eq!(nested4.as_text(), Some(long.as_str()));
    let nested5 = key6.get_from_tuple("key65").expect("key65");
    assert_eq!(nested5.value_type(), MixedValueType::Null);

    let key7 = value.get_from_tuple("key7").expect("key7");
    assert_eq!(key7.value_type(), MixedValueType::Array);
    assert_eq!(key7.slot_count(), Some(5));
    let element0 = key7.get_by_index(0).expect("key7[0]");
    assert_eq!(element0.value_type(), MixedValueType::Integer);
    assert_eq!(element0.as_integer(), Some(src));
    let element1 = key7.get_by_index(1).expect("key7[1]");
    assert_eq!(element1.value_type(), MixedValueType::Float);
    assert_eq!(element1.as_float(), Some(fv));
    let element2 = key7.get_by_index(2).expect("key7[2]");
    assert_eq!(element2.value_type(), MixedValueType::ShortStr);
    assert_eq!(element2.as_text(), Some(short.as_str()));
    let element3 = key7.get_by_index(3).expect("key7[3]");
    assert_eq!(element3.value_type(), MixedValueType::LongStr);
    assert_eq!(element3.as_text(), Some(long.as_str()));
    let element4 = key7.get_by_index(4).expect("key7[4]");
    assert_eq!(element4.value_type(), MixedValueType::Null);
}

fn array_make(src: i64) -> MixedT {
    let fv = fvalue(src);
    let short = short_str(src);
    let long = long_str(src);

    let mut array = MixedT::make_array_with_count(7).expect("7-slot array");
    array.insert_integer_into_array(src, 0).unwrap();
    array.insert_float_into_array(fv, 1).unwrap();
    array.insert_string_into_array(&short, 2).unwrap();
    array.insert_string_into_array(&long, 3).unwrap();
    array.insert_null_into_array(4).unwrap();

    let mut tuple = MixedT::make_tuple_with_count(5).expect("5-slot tuple");
    tuple.insert_integer_into_tuple("key51", src).unwrap();
    tuple.insert_float_into_tuple("key52", fv).unwrap();
    tuple.insert_string_into_tuple("key53", &short).unwrap();
    tuple.insert_string_into_tuple("key54", &long).unwrap();
    tuple.insert_null_into_tuple("key55").unwrap();
    array.copy_into_array(5, &tuple).unwrap();

    let mut nested = MixedT::make_array_with_count(5).expect("5-slot array");
    nested.insert_integer_into_array(src, 0).unwrap();
    nested.insert_float_into_array(fv, 1).unwrap();
    nested.insert_string_into_array(&short, 2).unwrap();
    nested.insert_string_into_array(&long, 3).unwrap();
    nested.insert_null_into_array(4).unwrap();
    array.copy_into_array(6, &nested).unwrap();

    array
}

fn array_verify(value: &MixedT, src: i64) {
    let fv = fvalue(src);
    let short = short_str(src);
    let long = long_str(src);

    assert_eq!(value.value_type(), MixedValueType::Array);
    assert_eq!(value.slot_count(), Some(7));

    let element0 = value.get_by_index(0).expect("array[0]");
    assert_eq!(element0.value_type(), MixedValueType::Integer);
    assert_eq!(element0.as_integer(), Some(src));
    let element1 = value.get_by_index(1).expect("array[1]");
    assert_eq!(element1.value_type(), MixedValueType::Float);
    assert_eq!(element1.as_float(), Some(fv));
    let element2 = value.get_by_index(2).expect("array[2]");
    assert_eq!(element2.value_type(), MixedValueType::ShortStr);
    assert_eq!(element2.as_text(), Some(short.as_str()));
    let element3 = value.get_by_index(3).expect("array[3]");
    assert_eq!(element3.value_type(), MixedValueType::LongStr);
    assert_eq!(element3.as_text(), Some(long.as_str()));
    let element4 = value.get_by_index(4).expect("array[4]");
    assert_eq!(element4.value_type(), MixedValueType::Null);

    let tuple = value.get_by_index(5).expect("array[5]");
    assert_eq!(tuple.value_type(), MixedValueType::Tuple);
    assert_eq!(tuple.slot_count(), Some(5));
    let tuple1 = tuple.get_from_tuple("key51").expect("key51");
    assert_eq!(tuple1.value_type(), MixedValueType::Integer);
    assert_eq!(tuple1.as_integer(), Some(src));
    let tuple2 = tuple.get_from_tuple("key52").expect("key52");
    assert_eq!(tuple2.value_type(), MixedValueType::Float);
    assert_eq!(tuple2.as_float(), Some(fv));
    let tuple3 = tuple.get_from_tuple("key53").expect("key53");
    assert_eq!(tuple3.value_type(), MixedValueType::ShortStr);
    assert_eq!(tuple3.as_text(), Some(short.as_str()));
    let tuple4 = tuple.get_from_tuple("key54").expect("key54");
    assert_eq!(tuple4.value_type(), MixedValueType::LongStr);
    assert_eq!(tuple4.as_text(), Some(long.as_str()));
    let tuple5 = tuple.get_from_tuple("key55").expect("key55");
    assert_eq!(tuple5.value_type(), MixedValueType::Null);

    let nested = value.get_by_index(6).expect("array[6]");
    assert_eq!(nested.value_type(), MixedValueType::Array);
    assert_eq!(nested.slot_count(), Some(5));
    let nested0 = nested.get_by_index(0).expect("nested[0]");
    assert_eq!(nested0.value_type(), MixedValueType::Integer);
    assert_eq!(nested0.as_integer(), Some(src));
    let nested1 = nested.get_by_index(1).expect("nested[1]");
    assert_eq!(nested1.value_type(), MixedValueType::Float);
    assert_eq!(nested1.as_float(), Some(fv));
    let nested2 = nested.get_by_index(2).expect("nested[2]");
    assert_eq!(nested2.value_type(), MixedValueType::ShortStr);
    assert_eq!(nested2.as_text(), Some(short.as_str()));
    let nested3 = nested.get_by_index(3).expect("nested[3]");
    assert_eq!(nested3.value_type(), MixedValueType::LongStr);
    assert_eq!(nested3.as_text(), Some(long.as_str()));
    let nested4 = nested.get_by_index(4).expect("nested[4]");
    assert_eq!(nested4.value_type(), MixedValueType::Null);
}

// C++ `mixed_integer_*`.
#[test]
fn mixed_integer_a() {
    flat_a(integer_make, integer_verify);
}

#[test]
fn mixed_integer_column_vector_select() {
    select_a(integer_make, integer_verify);
}

#[test]
fn mixed_integer_column_slice_init() {
    slice_a(integer_make, integer_verify);
}

// C++ `mixed_float_*`.
#[test]
fn mixed_float_a() {
    flat_a(float_make, float_verify);
}

#[test]
fn mixed_float_column_vector_select() {
    select_a(float_make, float_verify);
}

#[test]
fn mixed_float_column_slice_init() {
    slice_a(float_make, float_verify);
}

// C++ `mixed_short_str_*`.
#[test]
fn mixed_short_str_a() {
    flat_a(short_string_make, short_string_verify);
}

#[test]
fn mixed_short_str_column_vector_select() {
    select_a(short_string_make, short_string_verify);
}

#[test]
fn mixed_short_str_column_slice_init() {
    slice_a(short_string_make, short_string_verify);
}

// C++ `mixed_long_str_*`.
#[test]
fn mixed_long_str_a() {
    flat_a(long_string_make, long_string_verify);
}

#[test]
fn mixed_long_str_column_vector_select() {
    select_a(long_string_make, long_string_verify);
}

#[test]
fn mixed_long_str_column_slice_init() {
    slice_a(long_string_make, long_string_verify);
}

// C++ `mixed_null_*`.
#[test]
fn mixed_null_a() {
    flat_a(null_make, null_verify);
}

#[test]
fn mixed_null_column_vector_select() {
    select_a(null_make, null_verify);
}

#[test]
fn mixed_null_column_slice_init() {
    // The reference reads the source vector here by mistake; the expected
    // values are identical, and verifying the target is the intended behavior.
    slice_a(null_make, null_verify);
}

// C++ `mixed_missing_*`.
#[test]
fn mixed_missing_a() {
    flat_a(missing_make, missing_verify);
}

#[test]
fn mixed_missing_column_vector_select() {
    select_a(missing_make, missing_verify);
}

#[test]
fn mixed_missing_column_slice_init() {
    // See `mixed_null_column_slice_init` on the reference's source/target slip.
    slice_a(missing_make, missing_verify);
}

// C++ `mixed_tuple_*`.
#[test]
fn mixed_tuple_a() {
    flat_a(tuple_make, tuple_verify);
}

#[test]
fn mixed_tuple_column_vector_select() {
    select_a(tuple_make, tuple_verify);
}

#[test]
fn mixed_tuple_column_slice_init() {
    slice_a(tuple_make, tuple_verify);
}

// C++ `mixed_array_*`.
#[test]
fn mixed_array_a() {
    flat_a(array_make, array_verify);
}

#[test]
fn mixed_array_column_vector_select() {
    select_a(array_make, array_verify);
}

#[test]
fn mixed_array_column_slice_init() {
    slice_a(array_make, array_verify);
}

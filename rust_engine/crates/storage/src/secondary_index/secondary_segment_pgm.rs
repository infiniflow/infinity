//! Secondary-index segment files, following the reference's
//! `SecondaryIndexDataBase::SaveIndexInner` (`secondary_index_data_impl.cpp:224`)
//! and the PGM serialization in `secondary_index_pgm.cppm:129`.
//!
//! Layout of one chunk (`chunk_{n}.idx`, no header):
//!
//! ```text
//! keys     n * key_width      ordered keys (the reference's OrderedKeyType)
//! offsets  n * u32            segment offsets, parallel to `keys`
//! pgm      u32 n
//!          first_key          key_width bytes
//!          u32 segment_count, segment_count * (K key, Floating slope, u32 intercept)
//!          u32 level_count,   level_count * u64
//! ```
//!
//! `key_width` and the `Segment` size depend on the column's key type, which is
//! what [`KeyKind`] carries — the reference instantiates `PGMIndex<K, 64, 4,
//! Floating>` per type (`secondary_index_pgm.cppm:33-45`):
//!
//! | key kind | `K` / `Floating` | key width | `Segment` |
//! |---|---|---|---|
//! | `U64` (INT/UINT/VARCHAR/JSON) | `u64` / `float` | 8 | `{u64,f32,u32}` = 16 |
//! | `F32` (FLOAT) | `float` / `float` | 4 | `{f32,f32,u32}` = 12 |
//! | `F64` (DOUBLE) | `double` / `double` | 8 | `{f64,f64,u32}` = 20 |
//! | `I32` (DATE/TIME) | `i32` / `float` | 4 | `{i32,f32,u32}` = 12 |
//! | `I64` (DATETIME/TIMESTAMP) | `i64` / `float` | 8 | `{i64,f32,u32}` = 16 |
//!
//! and `levels_offsets` is a `std::vector<size_t>`, i.e. 8 bytes per entry.
//!
//! The model itself is a `PGMIndex<K, 64, 4, Floating>`: the bottom level is a
//! segmentation with `epsilon = 64`, every level above it is segmented with
//! `epsilon_recursive = 4` over the first key of the level below, and each
//! level ends with the reference's sentinel segment (`key = K::MAX`,
//! `slope = 0`, `intercept = last_n`). `levels_offsets` holds the start of
//! each level in `segments[]` followed by `segments.len()`.
//!
//! Every key kind is *built* the way the reference builds it and is
//! byte-identical to it (see the tests). The fit is exact integer arithmetic
//! for an integral key ([`Pgm`]) and `double` arithmetic for a floating-point
//! one ([`PgmFloat`]); the latter is what the reference does once its
//! `LargeSigned<float|double>` maps to `double` (`piecewise_linear_model.hpp:40`).
//! The reader does not consult a typed chunk's model: it binary-searches the
//! key array, which is what makes it correct for a chunk the reference wrote.

use crate::mapped::MappedFile;
use memmap2::Mmap;
use std::cmp::Ordering;
use std::fs::File;
use std::path::Path;

/// libstdc++'s `std::hash<std::string_view>` (`_Hash_bytes`, MurmurHash64A with
/// seed `0xc70f6907`), which is what the reference stores for VARCHAR keys
/// (`secondary_index_data.cppm:106-110`).
pub fn hash_varchar(text: &str) -> u64 {
    const SEED: u64 = 0xc70f_6907;
    const M: u64 = 0xc6a4_a793_5bd1_e995;
    const R: u32 = 47;
    let data = text.as_bytes();
    let mut h = SEED ^ (data.len() as u64).wrapping_mul(M);
    let (chunks, tail) = data.as_chunks::<8>();
    for chunk in chunks {
        let mut k = u64::from_le_bytes(*chunk);
        k = k.wrapping_mul(M);
        k ^= k >> R;
        k = k.wrapping_mul(M);
        h ^= k;
        h = h.wrapping_mul(M);
    }
    for (i, byte) in tail.iter().enumerate() {
        h ^= u64::from(*byte) << (8 * i);
    }
    if !tail.is_empty() {
        h = h.wrapping_mul(M);
    }
    h ^= h >> R;
    h = h.wrapping_mul(M);
    h ^= h >> R;
    h
}

use crate::format::FormatError;

const PGM_EPSILON: u64 = 64;
const PGM_EPSILON_RECURSIVE: u64 = 4;

/// How a chunk stores its keys — the reference's `ConvertToOrderedType<T>`
/// (`storage/secondary_index/secondary_index_data.cppm:38-118`).
///
/// `U64` is the engine's order-mapped integer key, and the hash the reference
/// stores for `VARCHAR`/`JSON` (`ConvertToHashU64`). `F32`/`F64` are the
/// reference's `KeepOrderedSelf` types `FloatT`/`DoubleT`: the key array holds
/// the raw IEEE bit pattern and the key *compares as a float*, which is what
/// makes a chunk written by the reference readable here and vice versa.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum KeyKind {
    U64,
    F32,
    F64,
    /// `ConvertToOrderedI32` (`DateT`, `TimeT`): a raw signed 4-byte key, the
    /// day count for a DATE and the second of the day for a TIME.
    I32,
    /// `ConvertToOrderedI64` (`DateTimeT`, `TimestampT`): a raw signed 8-byte
    /// key, `days * 86400 + seconds` (`GetEpochTime`).
    I64,
    /// `KeepOrderedSelf<TinyIntT>`: `TinyIntT` is `int8_t`
    /// (`parser/type/internal_types.h:55`), so the key array is one byte wide.
    I8,
    /// `KeepOrderedSelf<SmallIntT>`: `int16_t`, two bytes.
    I16,
    /// `KeepOrderedSelf<BooleanT>`: `bool`, one byte. A BOOL column is always
    /// written as a low-cardinality chunk (`SecondaryIndexInMemT` dispatches
    /// BOOL onto the low path), so no high-cardinality chunk is ever produced
    /// for it - [`write_secondary_segment_typed`] refuses the kind.
    Bool,
    /// `KeepOrderedString<JsonTermT>`: a variable-length JSON term. A JSON
    /// column is low-cardinality only (the reference rejects a high-cardinality
    /// JSON index outright), so this kind has no key width and no PGM layout.
    Bytes,
}

impl KeyKind {
    /// `sizeof(OrderedKeyType)`.
    pub fn key_width(self) -> usize {
        match self {
            KeyKind::U64 | KeyKind::F64 | KeyKind::I64 => 8,
            KeyKind::F32 | KeyKind::I32 => 4,
            KeyKind::I16 => 2,
            KeyKind::I8 | KeyKind::Bool => 1,
            // Variable-length: a term is `u32 len` + bytes. Only the
            // low-cardinality writer sees it, and it never asks for a width.
            KeyKind::Bytes => 0,
        }
    }

    /// `sizeof(Segment)` for `PGMIndex<K, 64, 4, Floating>`: `{K key; Floating
    /// slope; u32 intercept;}` under `#pragma pack(push, 1)`, so there is no
    /// padding: `sizeof(K) + 4 + 4`, or `sizeof(K) + 8 + 4` for a `double` key.
    pub fn segment_width(self) -> usize {
        match self {
            // `{u64, f32, u32}`, `{i64, f32, u32}`
            KeyKind::U64 | KeyKind::I64 => 16,
            // `{f32, f32, u32}`, `{i32, f32, u32}`
            KeyKind::F32 | KeyKind::I32 => 12,
            // `{i16, f32, u32}`, `{i8, f32, u32}`, `{bool, f32, u32}`
            KeyKind::I16 => 10,
            KeyKind::I8 | KeyKind::Bool => 9,
            // `{f64, f64, u32}` — `Floating` is `double` only for `double` keys.
            KeyKind::F64 => 20,
            // No PGM: a JSON term array is low-cardinality only.
            KeyKind::Bytes => 0,
        }
    }

    /// Width of the tail's `first_key`, i.e. `sizeof(K)`.
    pub fn first_key_width(self) -> usize {
        self.key_width()
    }

    /// The size of an empty chunk's tail: `n`, `first_key`, `segment_count`,
    /// `level_count` and no arrays.
    pub fn empty_tail_len(self) -> usize {
        4 + self.first_key_width() + 4 + 4
    }
}

/// A chunk key. `U64` covers the order-mapped integer and the hashed
/// varchar/json key; `F32`/`F64` are raw float keys.
///
/// Comparison: two `U64` keys compare as integers, anything else compares as
/// `f64` (an `F32` widens exactly) using [`f64::total_cmp`]. Comparing calls
/// from *different* domains (an integer key against a float key) is only
/// meaningful for callers that already know the column kind; every in-tree
/// caller passes the column's own kind.
///
/// `total_cmp` and not a raw `to_bits` comparison: the sign bit is the high
/// bit, so comparing bit patterns as integers puts every negative float
/// *after* every positive one. `-3.5` must sort before `-1.0`.
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Key {
    U64(u64),
    F32(f32),
    F64(f64),
    I32(i32),
    I64(i64),
    /// `TinyIntT` (`int8_t`).
    I8(i8),
    /// `SmallIntT` (`int16_t`).
    I16(i16),
    /// `BooleanT` (`bool`): a low-cardinality key, 0 or 1.
    Bool(bool),
}

impl Key {
    pub fn kind(self) -> KeyKind {
        match self {
            Key::U64(_) => KeyKind::U64,
            Key::F32(_) => KeyKind::F32,
            Key::F64(_) => KeyKind::F64,
            Key::I32(_) => KeyKind::I32,
            Key::I64(_) => KeyKind::I64,
            Key::I8(_) => KeyKind::I8,
            Key::I16(_) => KeyKind::I16,
            Key::Bool(_) => KeyKind::Bool,
        }
    }

    pub fn order(&self, other: &Key) -> Ordering {
        match (self, other) {
            (Key::U64(a), Key::U64(b)) => a.cmp(b),
            (Key::I32(a), Key::I32(b)) => a.cmp(b),
            (Key::I64(a), Key::I64(b)) => a.cmp(b),
            (Key::I8(a), Key::I8(b)) => a.cmp(b),
            (Key::I16(a), Key::I16(b)) => a.cmp(b),
            (Key::Bool(a), Key::Bool(b)) => (*a as u8).cmp(&(*b as u8)),
            _ => self.as_f64().total_cmp(&other.as_f64()),
        }
    }

    pub fn as_f64(self) -> f64 {
        match self {
            Key::U64(v) => v as f64,
            Key::F32(v) => v as f64,
            Key::F64(v) => v,
            Key::I32(v) => f64::from(v),
            Key::I64(v) => v as f64,
            Key::I8(v) => f64::from(v),
            Key::I16(v) => f64::from(v),
            Key::Bool(v) => f64::from(u8::from(v)),
        }
    }

    /// The key as an **order-preserving** byte encoding: two keys of one kind
    /// compare byte-wise exactly as [`Key::order`] compares them.
    ///
    /// The in-memory tail (`index::SecondaryIndex`) is a `BTreeMap` over these
    /// bytes: that is what lets a range scan slice it the way the reference
    /// slices its ordered `RcuMultiMap`, instead of decoding every key. It is
    /// **not** the chunk layout - a chunk stores the key's raw little-endian
    /// `OrderedKeyType` bits, which is what the reference writes.
    pub fn ordered_bytes(self) -> Vec<u8> {
        match self {
            Key::U64(v) => v.to_be_bytes().to_vec(),
            Key::I64(v) => ordered_u64_bits(v as u64).to_be_bytes().to_vec(),
            Key::I32(v) => ((v as u32) ^ (1 << 31)).to_be_bytes().to_vec(),
            Key::F32(v) => ordered_u32_bits(v.to_bits()).to_be_bytes().to_vec(),
            Key::F64(v) => ordered_u64_bits(v.to_bits()).to_be_bytes().to_vec(),
            Key::I8(v) => vec![(v as u8) ^ 0x80],
            Key::I16(v) => ((v as u16) ^ 0x8000).to_be_bytes().to_vec(),
            Key::Bool(v) => vec![u8::from(v)],
        }
    }

    /// Inverse of [`Self::ordered_bytes`] for a column's key kind.
    pub fn from_ordered_bytes(kind: KeyKind, bytes: &[u8]) -> Option<Key> {
        Some(match kind {
            KeyKind::U64 => Key::U64(u64::from_be_bytes(bytes.try_into().ok()?)),
            KeyKind::I64 => {
                Key::I64(unordered_u64_bits(u64::from_be_bytes(bytes.try_into().ok()?)) as i64)
            }
            KeyKind::I32 => {
                Key::I32(((u32::from_be_bytes(bytes.try_into().ok()?)) ^ (1 << 31)) as i32)
            }
            KeyKind::F32 => Key::F32(f32::from_bits(unordered_u32_bits(u32::from_be_bytes(
                bytes.try_into().ok()?,
            )))),
            KeyKind::F64 => Key::F64(f64::from_bits(unordered_u64_bits(u64::from_be_bytes(
                bytes.try_into().ok()?,
            )))),
            KeyKind::I8 => Key::I8((*bytes.first()? ^ 0x80) as i8),
            KeyKind::I16 => {
                let raw = u16::from_be_bytes(bytes.get(..2)?.try_into().ok()?);
                Key::I16((raw ^ 0x8000) as i16)
            }
            KeyKind::Bool => Key::Bool(*bytes.first()? != 0),
            // A term is not a fixed-width ordered key; the JSON path keeps it
            // as [`crate::secondary_lowcard::LowCardKey::Bytes`].
            KeyKind::Bytes => return None,
        })
    }
}

/// Sortable-bits transform: maps `f64` bit patterns onto ascending `u64` order,
/// which is exactly `f64::total_cmp`'s order (`-0.0 < +0.0`, NaN last).
fn ordered_u64_bits(bits: u64) -> u64 {
    if bits & (1u64 << 63) != 0 {
        !bits
    } else {
        bits ^ (1u64 << 63)
    }
}

fn unordered_u64_bits(bits: u64) -> u64 {
    if bits & (1u64 << 63) != 0 {
        bits ^ (1u64 << 63)
    } else {
        !bits
    }
}

fn ordered_u32_bits(bits: u32) -> u32 {
    if bits & (1u32 << 31) != 0 {
        !bits
    } else {
        bits ^ (1u32 << 31)
    }
}

fn unordered_u32_bits(bits: u32) -> u32 {
    if bits & (1u32 << 31) != 0 {
        bits ^ (1u32 << 31)
    } else {
        !bits
    }
}

/// An integral key type the reference instantiates `PGMIndex<K, 64, 4, float>`
/// with (`ConvertToOrdered`, `secondary_index_data.cppm:37-72`): `u64` for a
/// hashed varchar/json key, `i32` for `IntegerT`/`DateT`/`TimeT` and `i64` for
/// `BigIntT`/`DateTimeT`/`TimestampT`.
///
/// Over an integral `K` the reference's fit is exact integer arithmetic
/// (`get_floating_point_segment`, `piecewise_linear_model.hpp:245-253`), so a
/// chunk of one of these kinds is byte-reproducible. Floating-point keys take
/// the `double` path instead — see [`FloatKey`].
trait IntegralKey: Copy + Default + PartialEq + Eq + PartialOrd + std::fmt::Debug {
    /// `PGMIndex::sentinel`: `numeric_limits<K>::max()`, since an integral `K`
    /// has no infinity (`pgm_index.hpp:84`).
    const SENTINEL: Self;
    /// The key the reference probes before appending that sentinel,
    /// `sentinel - 1` (`pgm_index.hpp:105`).
    const SENTINEL_PREV: Self;
    /// `*std::prev(last) + 1`, wrapping at `K`'s width.
    fn next(self) -> Self;
    /// The key widened for the corridor arithmetic, which the reference does in
    /// `int64_t` for a 32-bit key and `__int128` for a 64-bit one.
    fn as_i128(self) -> i128;
    /// `size_t(slope * double(make_unsigned_t<K>(k) - key))`: for a *signed*
    /// `K` the reference subtracts unsigned (`pgm_index.hpp:262-263`), so a
    /// segment key above the probed key wraps instead of going negative.
    fn approx_diff(self, key: Self) -> f64;
    /// The tail's `first_key`, in the reference's little-endian `K`.
    fn push_le(self, out: &mut Vec<u8>);
    /// Widens a [`Key`] of a narrower or wider kind into this one.
    fn from_key(key: Key) -> Self;
}

impl IntegralKey for u64 {
    const SENTINEL: Self = u64::MAX;
    const SENTINEL_PREV: Self = u64::MAX - 1;
    fn next(self) -> Self {
        self.wrapping_add(1)
    }
    fn as_i128(self) -> i128 {
        self as i128
    }
    fn approx_diff(self, key: Self) -> f64 {
        self.wrapping_sub(key) as f64
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::U64(v) => v,
            other => other.as_f64() as u64,
        }
    }
}

impl IntegralKey for i32 {
    const SENTINEL: Self = i32::MAX;
    const SENTINEL_PREV: Self = i32::MAX - 1;
    fn next(self) -> Self {
        self.wrapping_add(1)
    }
    fn as_i128(self) -> i128 {
        self as i128
    }
    fn approx_diff(self, key: Self) -> f64 {
        (self as u32).wrapping_sub(key as u32) as f64
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::I32(v) => v,
            other => other.as_f64() as i32,
        }
    }
}

impl IntegralKey for i16 {
    const SENTINEL: Self = i16::MAX;
    const SENTINEL_PREV: Self = i16::MAX - 1;
    fn next(self) -> Self {
        self.wrapping_add(1)
    }
    fn as_i128(self) -> i128 {
        self as i128
    }
    fn approx_diff(self, key: Self) -> f64 {
        (self as u16).wrapping_sub(key as u16) as f64
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::I16(v) => v,
            other => other.as_f64() as i16,
        }
    }
}

impl IntegralKey for i8 {
    const SENTINEL: Self = i8::MAX;
    const SENTINEL_PREV: Self = i8::MAX - 1;
    fn next(self) -> Self {
        self.wrapping_add(1)
    }
    fn as_i128(self) -> i128 {
        self as i128
    }
    fn approx_diff(self, key: Self) -> f64 {
        (self as u8).wrapping_sub(key as u8) as f64
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.push(self as u8);
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::I8(v) => v,
            other => other.as_f64() as i8,
        }
    }
}

impl IntegralKey for i64 {
    const SENTINEL: Self = i64::MAX;
    const SENTINEL_PREV: Self = i64::MAX - 1;
    fn next(self) -> Self {
        self.wrapping_add(1)
    }
    fn as_i128(self) -> i128 {
        self as i128
    }
    fn approx_diff(self, key: Self) -> f64 {
        (self as u64).wrapping_sub(key as u64) as f64
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::I64(v) => v,
            other => other.as_f64() as i64,
        }
    }
}

/// One piecewise-linear segment: a key, a slope and an intercept.
///
/// `(#pragma pack(1)) Segment { K key; Floating slope; uint32_t intercept; }`
/// with an integral `K` and `Floating = float` (`pgm_index.hpp:225-267`).
#[derive(Debug, Clone, Copy)]
struct PgmSegment<X> {
    key: X,
    slope: f32,
    intercept: u32,
}

impl<X: IntegralKey> PgmSegment<X> {
    /// `Segment::operator()`: `size_t(slope * double(k - key)) + intercept`.
    fn approx(&self, key: X) -> u64 {
        let stride = (self.slope as f64 * key.approx_diff(self.key)) as u64;
        stride.wrapping_add(self.intercept as u64)
    }
}

// ---------------------------------------------------------------------------
// `PGMIndex<K, 64, 4, Floating>` over floating-point keys
// ---------------------------------------------------------------------------

/// A floating-point key type the reference instantiates `PGMIndex<K, 64, 4,
/// Floating>` with (`KeepOrderedSelf`, `secondary_index_data.cppm:74-90`):
/// `float` for a `FLOAT` column, `double` for a `DOUBLE` one.
///
/// The fit runs in `double` for both, because `LargeSigned<T>` is `double`
/// whenever `T` is floating-point (`piecewise_linear_model.hpp:39-42`): an
/// `f32` key is widened first. Only the slope's *storage* differs — `Floating`
/// is `float` for an `f32` key and `double` for an `f64` one
/// (`secondary_index_pgm.cppm:33-45`).
trait FloatKey: Copy + PartialEq + PartialOrd + std::fmt::Debug {
    /// `PGMIndex::sentinel`: `+inf`, an IEEE key having an infinity
    /// (`pgm_index.hpp:84`).
    const SENTINEL: Self;
    /// `std::nextafter(self, +inf)`.
    fn next_after(self) -> Self;
    /// The key widened for the corridor arithmetic, i.e. `SX(x)`.
    fn as_f64(self) -> f64;
    /// `K(value)` — the reference narrows a fitted `double` back to `K` where
    /// it rebuilds a key.
    fn from_f64(value: f64) -> Self;
    /// `double(k - key)`: the subtraction runs in `K`, so an `f32` rounds
    /// before the widening (`pgm_index.hpp:280-284`).
    fn approx_delta(self, key: Self) -> f64;
    /// The fitted `double` slope narrowed to the `Segment`'s `Floating` field.
    fn narrow_slope(slope: f64) -> f64;
    /// The tail's `first_key` and a segment's key, in the reference's `K`.
    fn push_le(self, out: &mut Vec<u8>);
    /// A `Segment`'s slope in the reference's little-endian `Floating`.
    fn push_slope(slope: f64, out: &mut Vec<u8>);
    /// Widens a [`Key`] of any kind into this one.
    fn from_key(key: Key) -> Self;
}

/// `std::nextafter(x, +inf)` on the bit pattern: a positive magnitude grows, a
/// negative one shrinks, and `±0` steps to the smallest positive subnormal.
/// (`f32::next_up`/`f64::next_up` are newer than this crate's MSRV.)
fn next_up_f32(x: f32) -> f32 {
    if x.is_nan() || x == f32::INFINITY {
        return x;
    }
    if x == 0.0 {
        return f32::from_bits(1);
    }
    let bits = x.to_bits();
    if x > 0.0 {
        f32::from_bits(bits + 1)
    } else {
        f32::from_bits(bits - 1)
    }
}

fn next_up_f64(x: f64) -> f64 {
    if x.is_nan() || x == f64::INFINITY {
        return x;
    }
    if x == 0.0 {
        return f64::from_bits(1);
    }
    let bits = x.to_bits();
    if x > 0.0 {
        f64::from_bits(bits + 1)
    } else {
        f64::from_bits(bits - 1)
    }
}

impl FloatKey for f32 {
    const SENTINEL: Self = f32::INFINITY;
    fn next_after(self) -> Self {
        next_up_f32(self)
    }
    fn as_f64(self) -> f64 {
        f64::from(self)
    }
    fn from_f64(value: f64) -> Self {
        value as f32
    }
    fn approx_delta(self, key: Self) -> f64 {
        f64::from(self - key)
    }
    fn narrow_slope(slope: f64) -> f64 {
        f64::from(slope as f32)
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_bits().to_le_bytes());
    }
    fn push_slope(slope: f64, out: &mut Vec<u8>) {
        out.extend_from_slice(&(slope as f32).to_bits().to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::F32(v) => v,
            other => other.as_f64() as f32,
        }
    }
}

impl FloatKey for f64 {
    const SENTINEL: Self = f64::INFINITY;
    fn next_after(self) -> Self {
        next_up_f64(self)
    }
    fn as_f64(self) -> f64 {
        self
    }
    fn from_f64(value: f64) -> Self {
        value
    }
    fn approx_delta(self, key: Self) -> f64 {
        self - key
    }
    fn narrow_slope(slope: f64) -> f64 {
        slope
    }
    fn push_le(self, out: &mut Vec<u8>) {
        out.extend_from_slice(&self.to_bits().to_le_bytes());
    }
    fn push_slope(slope: f64, out: &mut Vec<u8>) {
        out.extend_from_slice(&slope.to_bits().to_le_bytes());
    }
    fn from_key(key: Key) -> Self {
        match key {
            Key::F64(v) => v,
            other => other.as_f64(),
        }
    }
}

/// `(#pragma pack(1)) Segment { K key; Floating slope; uint32_t intercept; }`
/// for a floating-point `K`. The slope is kept widened and already narrowed to
/// `Floating`, so it is bit-exact with the reference for either key width.
#[derive(Debug, Clone, Copy)]
struct PgmFloatSegment<K> {
    key: K,
    slope: f64,
    intercept: u32,
}

impl<K: FloatKey> PgmFloatSegment<K> {
    /// `Segment::operator()`, saturated the way the patched reference is:
    /// `size_t(x)` is undefined for NaN/inf/out-of-range `x`, and both callers
    /// clamp the result with `next(it)->intercept` afterwards.
    fn approx(&self, key: K) -> u64 {
        let estimate = self.slope * key.approx_delta(self.key);
        if !estimate.is_finite() || estimate >= u64::MAX as f64 {
            return u64::MAX;
        }
        let position = estimate as u64;
        if position > u64::MAX - self.intercept as u64 {
            return u64::MAX;
        }
        position + self.intercept as u64
    }
}

/// A product `dx * dy` kept as `mantissa * 2^exponent` instead of a `double`.
///
/// With keys spanning most of the double range the product of a horizontal and
/// a vertical difference overflows to infinity, and `inf - inf` is NaN. The
/// reference compares such products through this exponent form
/// (`piecewise_linear_model.hpp:44-97`); the fast path below still uses the
/// plain `double` product whenever both products are finite, so every ordinary
/// model is bit-for-bit the plain-`double` fit.
#[derive(Debug, Clone, Copy)]
struct ScaledProduct {
    /// `|mantissa|` in `[0.5, 1)`, or `0` for a zero product.
    mantissa: f64,
    exponent: i32,
}

/// `std::frexp`: `x = m * 2^e` with `|m|` in `[0.5, 1)`. `(x, 0)` for a
/// zero, NaN or infinite input, which is what the reference's fallback sees.
fn frexp(x: f64) -> (f64, i32) {
    if x == 0.0 || !x.is_finite() {
        return (x, 0);
    }
    let bits = x.to_bits();
    let biased = ((bits >> 52) & 0x7ff) as i32;
    let (mantissa, exponent) = if biased == 0 {
        // Subnormal: scaling by 2^54 makes it normal, and the exponent shifts
        // back by the same amount.
        let (m, e) = frexp(x * 18_014_398_509_481_984.0);
        (m, e - 54)
    } else {
        (
            f64::from_bits((bits & !(0x7ff << 52)) | (1022 << 52)),
            biased - 1022,
        )
    };
    (mantissa, exponent)
}

/// `std::ldexp(x, n)`: `x * 2^n`, exact within the normal range.
fn ldexp(x: f64, n: i32) -> f64 {
    let mut x = x;
    let mut n = n;
    while n > 1023 {
        x *= f64::from_bits((1023 + 1023) << 52);
        n -= 1023;
    }
    while n < -1022 {
        x *= f64::from_bits((1023 - 1022) << 52);
        n += 1022;
    }
    x * f64::from_bits(((1023 + n) as u64) << 52)
}

fn scale_product(dx: f64, dy: f64) -> ScaledProduct {
    let (dx_mantissa, dx_exponent) = frexp(dx);
    let (dy_mantissa, dy_exponent) = frexp(dy);
    let mut mantissa = dx_mantissa * dy_mantissa;
    let mut exponent = dx_exponent + dy_exponent;
    // Both mantissas are in `[0.5, 1)`, so their product is in `[0.25, 1)`:
    // renormalize so that the exponent orders like the magnitude.
    if mantissa.abs() < 0.5 {
        mantissa *= 2.0;
        exponent -= 1;
    }
    ScaledProduct { mantissa, exponent }
}

fn compare_scaled(lhs: ScaledProduct, rhs: ScaledProduct) -> i32 {
    if lhs.mantissa == 0.0 || rhs.mantissa == 0.0 {
        if lhs.mantissa == 0.0 && rhs.mantissa == 0.0 {
            return 0;
        }
        if lhs.mantissa == 0.0 {
            return if rhs.mantissa.is_sign_negative() {
                1
            } else {
                -1
            };
        }
        return if lhs.mantissa.is_sign_negative() {
            -1
        } else {
            1
        };
    }
    let lhs_negative = lhs.mantissa.is_sign_negative();
    if lhs_negative != rhs.mantissa.is_sign_negative() {
        return if lhs_negative { -1 } else { 1 };
    }
    let magnitude = if lhs.exponent != rhs.exponent {
        if lhs.exponent < rhs.exponent {
            -1
        } else {
            1
        }
    } else if lhs.mantissa != rhs.mantissa {
        if lhs.mantissa < rhs.mantissa {
            -1
        } else {
            1
        }
    } else {
        0
    };
    if lhs_negative {
        -magnitude
    } else {
        magnitude
    }
}

/// Compares `dx1 * dy1` with `dx2 * dy2`, falling back to [`compare_scaled`]
/// only when a product overflows: for finite products this is exactly the
/// plain `double` comparison.
fn compare_products(dx1: f64, dy1: f64, dx2: f64, dy2: f64) -> i32 {
    let lhs = dx1 * dy1;
    let rhs = dx2 * dy2;
    if lhs.is_finite() && rhs.is_finite() {
        return i32::from(lhs > rhs) - i32::from(lhs < rhs);
    }
    compare_scaled(scale_product(dx1, dy1), scale_product(dx2, dy2))
}

#[derive(Debug, Clone, Copy, PartialEq)]
struct FloatPoint<K> {
    x: K,
    y: u64,
}

/// `Slope { double dx; double dy; }` — `SX` and `SY` are both `double` once the
/// key is floating-point.
#[derive(Debug, Clone, Copy)]
struct FloatSlope {
    dx: f64,
    dy: f64,
}

impl FloatSlope {
    /// `Slope::compare`: `compare_products(p.dx, dy, dx, p.dy)`, i.e. the
    /// original `dy * p.dx` against `dx * p.dy`.
    fn compare(&self, other: &FloatSlope) -> i32 {
        compare_products(other.dx, self.dy, self.dx, other.dy)
    }

    fn lt(&self, other: &FloatSlope) -> bool {
        self.compare(other) < 0
    }

    fn gt(&self, other: &FloatSlope) -> bool {
        self.compare(other) > 0
    }

    /// `explicit operator double()`: `dy / dx`.
    fn to_f64(self) -> f64 {
        self.dy / self.dx
    }
}

fn float_point_diff<K: FloatKey>(a: FloatPoint<K>, b: FloatPoint<K>) -> FloatSlope {
    FloatSlope {
        dx: a.x.as_f64() - b.x.as_f64(),
        dy: a.y as f64 - b.y as f64,
    }
}

/// `cross(O, A, B) = (A - O) x (B - O)`, compared without overflow.
fn float_cross<K: FloatKey>(o: FloatPoint<K>, a: FloatPoint<K>, b: FloatPoint<K>) -> i32 {
    let oa = float_point_diff(a, o);
    let ob = float_point_diff(b, o);
    compare_products(oa.dx, ob.dy, ob.dx, oa.dy)
}

/// `OptimalPiecewiseLinearModel<K, size_t>` for a floating-point `K`.
struct FloatModel<K> {
    epsilon: u64,
    lower: Vec<FloatPoint<K>>,
    upper: Vec<FloatPoint<K>>,
    first_x: K,
    /// The rank of `first_x`. The reference keeps only `x` in its
    /// `CanonicalSegment`; the rank is needed for the degenerate-fit fallback
    /// in [`float_segment_from_canonical`].
    first_y: u64,
    last_x: K,
    lower_start: usize,
    upper_start: usize,
    points_in_hull: usize,
    rectangle: [FloatPoint<K>; 4],
}

impl<K: FloatKey> FloatModel<K> {
    fn new(epsilon: u64) -> Self {
        let zero = FloatPoint {
            x: K::from_f64(0.0),
            y: 0,
        };
        Self {
            epsilon,
            lower: Vec::new(),
            upper: Vec::new(),
            first_x: zero.x,
            first_y: 0,
            last_x: zero.x,
            lower_start: 0,
            upper_start: 0,
            points_in_hull: 0,
            rectangle: [zero; 4],
        }
    }

    /// `add_point`: returns `false` when `(x, y)` no longer fits the corridor.
    fn add_point(&mut self, x: K, y: u64) -> bool {
        debug_assert!(self.points_in_hull == 0 || x > self.last_x);
        self.last_x = x;
        let max_y = u64::MAX;
        let min_y = 0u64;
        let p1 = FloatPoint {
            x,
            y: if y >= max_y - self.epsilon {
                max_y
            } else {
                y + self.epsilon
            },
        };
        let p2 = FloatPoint {
            x,
            y: if y <= min_y + self.epsilon {
                min_y
            } else {
                y - self.epsilon
            },
        };

        if self.points_in_hull == 0 {
            self.first_x = x;
            self.first_y = y;
            self.rectangle[0] = p1;
            self.rectangle[1] = p2;
            self.upper.clear();
            self.lower.clear();
            self.upper.push(p1);
            self.lower.push(p2);
            self.upper_start = 0;
            self.lower_start = 0;
            self.points_in_hull += 1;
            return true;
        }

        if self.points_in_hull == 1 {
            self.rectangle[2] = p2;
            self.rectangle[3] = p1;
            self.upper.push(p1);
            self.lower.push(p2);
            self.points_in_hull += 1;
            return true;
        }

        let slope1 = float_point_diff(self.rectangle[2], self.rectangle[0]);
        let slope2 = float_point_diff(self.rectangle[3], self.rectangle[1]);
        let outside_line1 = float_point_diff(p1, self.rectangle[2]).lt(&slope1);
        let outside_line2 = float_point_diff(p2, self.rectangle[3]).gt(&slope2);

        if outside_line1 || outside_line2 {
            self.points_in_hull = 0;
            return false;
        }

        if float_point_diff(p1, self.rectangle[1]).lt(&slope2) {
            // Find extreme slope.
            let mut min = float_point_diff(self.lower[self.lower_start], p1);
            let mut min_i = self.lower_start;
            for (i, point) in self.lower.iter().enumerate().skip(self.lower_start + 1) {
                let val = float_point_diff(*point, p1);
                if val.gt(&min) {
                    break;
                }
                min = val;
                min_i = i;
            }

            self.rectangle[1] = self.lower[min_i];
            self.rectangle[3] = p1;
            self.lower_start = min_i;

            // Hull update.
            let mut end = self.upper.len();
            while end >= self.upper_start + 2
                && float_cross(self.upper[end - 2], self.upper[end - 1], p1) <= 0
            {
                end -= 1;
            }
            self.upper.truncate(end);
            self.upper.push(p1);
        }

        if float_point_diff(p2, self.rectangle[0]).gt(&slope1) {
            // Find extreme slope.
            let mut max = float_point_diff(self.upper[self.upper_start], p2);
            let mut max_i = self.upper_start;
            for (i, point) in self.upper.iter().enumerate().skip(self.upper_start + 1) {
                let val = float_point_diff(*point, p2);
                if val.lt(&max) {
                    break;
                }
                max = val;
                max_i = i;
            }

            self.rectangle[0] = self.upper[max_i];
            self.rectangle[2] = p2;
            self.upper_start = max_i;

            // Hull update.
            let mut end = self.lower.len();
            while end >= self.lower_start + 2
                && float_cross(self.lower[end - 2], self.lower[end - 1], p2) >= 0
            {
                end -= 1;
            }
            self.lower.truncate(end);
            self.lower.push(p2);
        }

        self.points_in_hull += 1;
        true
    }

    fn get_segment(&self) -> CanonicalFloatSegment<K> {
        if self.points_in_hull == 1 {
            CanonicalFloatSegment {
                rectangle: [
                    self.rectangle[0],
                    self.rectangle[1],
                    self.rectangle[0],
                    self.rectangle[1],
                ],
                first: self.first_x,
                first_y: self.first_y,
            }
        } else {
            CanonicalFloatSegment {
                rectangle: self.rectangle,
                first: self.first_x,
                first_y: self.first_y,
            }
        }
    }
}

/// `OptimalPiecewiseLinearModel<K, size_t>::CanonicalSegment` for a
/// floating-point `K`.
#[derive(Debug, Clone, Copy)]
struct CanonicalFloatSegment<K> {
    rectangle: [FloatPoint<K>; 4],
    first: K,
    /// The rank of `first`, which the reference does not keep (see
    /// [`float_segment_from_canonical`]).
    first_y: u64,
}

impl<K: FloatKey> CanonicalFloatSegment<K> {
    fn one_point(&self) -> bool {
        let r = &self.rectangle;
        r[0].x == r[2].x && r[0].y == r[2].y && r[1].x == r[3].x && r[1].y == r[3].y
    }

    /// `get_intersection()`: the point where the two corridor edges meet.
    ///
    /// Every horizontal term is scaled by the same power of two, so `b` — a
    /// ratio of two cross products — is unchanged while the products stay
    /// finite for keys that span most of the double range
    /// (`piecewise_linear_model.hpp:298-321`).
    ///
    /// The reference is compiled with `-ffp-contract=on` (clang's default),
    /// but only `get_floating_point_segment`'s `i_y - (i_x - origin) * slope`
    /// is actually contracted: the two cross products of `a` and `b` and the
    /// `p0.x + b * slope1.dx` of `i_x`/`i_y` are evaluated as written.
    ///
    /// Measured, not assumed — a 65-key linear run (step 10) and a 100-key one
    /// (step 0.5) disagree about which of the five sites is fused:
    /// fusing `a`/`b`/`i_x`/`i_y` gives intercepts 31 and **32** (the 100-key
    /// case then differs from the reference by one byte), while fusing only
    /// the intercept gives 31 and 31, and fusing nothing gives 32 and 31. See
    /// `float_intercept_matches_contracted_reference_fit` and
    /// `float_intercept_is_not_contracted_in_the_cross_products`.
    fn get_intersection(&self) -> (f64, f64) {
        let r = &self.rectangle;
        let (p0, p1, p2, p3) = (r[0], r[1], r[2], r[3]);
        let slope1 = float_point_diff(p2, p0);
        let slope2 = float_point_diff(p3, p1);

        if self.one_point() || slope1.compare(&slope2) == 0 {
            return (p0.x.as_f64(), p0.y as f64);
        }

        let p0p1 = float_point_diff(p1, p0);
        let (_, exponent) = frexp(p0p1.dx.abs().max(slope1.dx.abs()).max(slope2.dx.abs()));
        let scaled = |dx: f64| ldexp(dx, -exponent);
        let a = scaled(slope1.dx) * slope2.dy - slope1.dy * scaled(slope2.dx);
        let b = (scaled(p0p1.dx) * slope2.dy - p0p1.dy * scaled(slope2.dx)) / a;
        let i_x = p0.x.as_f64() + b * slope1.dx;
        let i_y = p0.y as f64 + b * slope1.dy;
        (i_x, i_y)
    }

    /// `get_slope_range()`: the two corridor slopes `dy / dx`.
    fn get_slope_range(&self) -> (f64, f64) {
        if self.one_point() {
            return (0.0, 1.0);
        }
        let r = &self.rectangle;
        (
            float_point_diff(r[2], r[0]).to_f64(),
            float_point_diff(r[3], r[1]).to_f64(),
        )
    }

    /// `get_floating_point_segment(origin)`: the slope is the midpoint of the
    /// corridor and the intercept is where that line predicts rank `0`.
    /// Returns `(slope, intercept)` before the `Segment` constructor narrows
    /// the slope to `Floating` and truncates the intercept to `uint32_t`.
    fn floating_point_segment(&self, origin: K) -> (f64, f64) {
        if self.one_point() {
            let mid = self.rectangle[0].y.wrapping_add(self.rectangle[1].y) / 2;
            return (0.0, mid as f64);
        }

        let (i_x, i_y) = self.get_intersection();
        let (min_slope, max_slope) = self.get_slope_range();
        let slope = (min_slope + max_slope) / 2.0;
        // `i_y - (i_x - origin) * slope`, with the subtraction fused into an FMA
        // like the reference build does.
        let intercept = (i_x - origin.as_f64()).mul_add(-slope, i_y);
        (slope, intercept)
    }
}

/// `Segment::Segment(const CanonicalSegment &cs)`.
fn float_segment_from_canonical<K: FloatKey>(cs: &CanonicalFloatSegment<K>) -> PgmFloatSegment<K> {
    let key = cs.first;
    let (slope, intercept) = cs.floating_point_segment(key);
    // The reference converts the fitted `double` to `SY = __int128`, i.e. it
    // truncates toward zero, and throws when the result does not fit `uint32_t`.
    // A fit leaves that range only when the corridor's slopes overflow: a
    // segment over a gap of one subnormal `ulp`, e.g. a single-row `DOUBLE`
    // chunk holding `0.0`, where `1 / ulp` and `-1 / ulp` are the two
    // infinities and their midpoint is NaN. The reference throws there and
    // writes no chunk at all, so a flat segment at the segment's own first rank
    // is the only thing left to write - and it keeps the search window on the
    // segment instead of propagating a NaN.
    let intercept = intercept.trunc();
    if intercept.is_finite() && (0.0..=u32::MAX as f64).contains(&intercept) {
        PgmFloatSegment {
            key,
            slope: K::narrow_slope(slope),
            intercept: intercept as u32,
        }
    } else {
        PgmFloatSegment {
            key,
            slope: 0.0,
            intercept: cs.first_y as u32,
        }
    }
}

/// `internal::make_segmentation(n, start, end, epsilon, in, out)` for a
/// floating-point key. The duplicate-run adjustment maps `nextafter(x, +inf)`
/// (not `x + 1`) to the run's rank, and the closing point is the next
/// representable key after the last one (`piecewise_linear_model.hpp:353-398`).
fn make_segmentation_float<K: FloatKey>(
    n: usize,
    start: usize,
    end: usize,
    epsilon: u64,
    input: &dyn Fn(usize) -> K,
    out: &mut dyn FnMut(CanonicalFloatSegment<K>),
) -> usize {
    let mut c = 0usize;
    let mut model = FloatModel::new(epsilon);

    fn add_point<K: FloatKey>(
        model: &mut FloatModel<K>,
        out: &mut dyn FnMut(CanonicalFloatSegment<K>),
        c: &mut usize,
        x: K,
        y: u64,
    ) {
        if !model.add_point(x, y) {
            out(model.get_segment());
            model.add_point(x, y);
            *c += 1;
        }
    }

    add_point(&mut model, out, &mut c, input(start), start as u64);
    for i in start + 1..end - 1 {
        if input(i) == input(i - 1) {
            // At the end of a run of duplicate keys `x` such that
            // `nextafter(x, +inf) != in(i + 1)`, map the keys in between to
            // rank `i`.
            let next = input(i).next_after();
            if next < input(i + 1) {
                add_point(&mut model, out, &mut c, next, i as u64);
            }
        } else {
            add_point(&mut model, out, &mut c, input(i), i as u64);
        }
    }
    if end >= start + 2 && input(end - 1) != input(end - 2) {
        add_point(&mut model, out, &mut c, input(end - 1), (end - 1) as u64);
    }
    if end == n {
        // Ensure values greater than the last one are mapped to `n`.
        add_point(&mut model, out, &mut c, input(n - 1).next_after(), n as u64);
    }

    out(model.get_segment());
    c + 1
}

/// A multi-level PGM over ascending floating-point keys, exactly as
/// `PGMIndex<K, 64, 4, Floating>`.
#[derive(Debug)]
struct PgmFloat<K> {
    n: u32,
    first_key: K,
    segments: Vec<PgmFloatSegment<K>>,
    levels_offsets: Vec<u64>,
}

impl<K: FloatKey> PgmFloat<K> {
    fn build(keys: &[K]) -> Self {
        if keys.is_empty() {
            // `PGMIndex::build` returns before pushing any segment or level.
            return Self {
                n: 0,
                first_key: K::from_f64(0.0),
                segments: Vec::new(),
                levels_offsets: Vec::new(),
            };
        }
        // `key_after_last`: the reference always takes the next representable
        // key after the *input's* last key, at every level (`pgm_index.hpp:107`).
        let key_after_last = keys[keys.len() - 1].next_after();
        let mut segments = Vec::new();
        let mut levels_offsets = vec![0u64];

        let bottom = |i: usize| keys[i];
        let mut last_n = Self::build_level(
            &mut segments,
            keys.len(),
            PGM_EPSILON,
            &bottom,
            key_after_last,
        );
        levels_offsets.push(segments.len() as u64);

        while PGM_EPSILON_RECURSIVE > 0 && last_n > 1 {
            let offset = levels_offsets[levels_offsets.len() - 2] as usize;
            let level_keys: Vec<K> = (0..last_n).map(|i| segments[offset + i].key).collect();
            let input = |i: usize| level_keys[i];
            last_n = Self::build_level(
                &mut segments,
                last_n,
                PGM_EPSILON_RECURSIVE,
                &input,
                key_after_last,
            );
            levels_offsets.push(segments.len() as u64);
        }

        Self {
            n: keys.len() as u32,
            first_key: keys[0],
            segments,
            levels_offsets,
        }
    }

    /// `PGMIndex::build`'s `build_level` lambda (`pgm_index.hpp:102`). The
    /// floating-point branch probes with the next representable key instead of
    /// `sentinel - 1`, which is `+inf` here: `size_t(+inf)` is undefined and
    /// would make the segment count compiler-dependent.
    fn build_level(
        segments: &mut Vec<PgmFloatSegment<K>>,
        last_n: usize,
        epsilon: u64,
        input: &dyn Fn(usize) -> K,
        key_after_last: K,
    ) -> usize {
        let mut n_segments =
            make_segmentation_float(last_n, 0, last_n, epsilon, input, &mut |cs| {
                segments.push(float_segment_from_canonical(&cs));
            });
        let last = *segments.last().expect("at least one segment");
        if last.key == K::SENTINEL {
            n_segments -= 1;
        } else {
            if last.approx(key_after_last) < last_n as u64 {
                // Ensure keys > the last one are mapped to `last_n`.
                segments.push(PgmFloatSegment {
                    key: key_after_last,
                    slope: 0.0,
                    intercept: last_n as u32,
                });
            }
            segments.push(PgmFloatSegment {
                key: K::SENTINEL,
                slope: 0.0,
                intercept: last_n as u32,
            });
        }
        n_segments
    }

    fn encode(&self) -> Vec<u8> {
        let mut out = Vec::new();
        out.extend_from_slice(&self.n.to_le_bytes());
        self.first_key.push_le(&mut out);
        out.extend_from_slice(&(self.segments.len() as u32).to_le_bytes());
        for s in &self.segments {
            s.key.push_le(&mut out);
            K::push_slope(s.slope, &mut out);
            out.extend_from_slice(&s.intercept.to_le_bytes());
        }
        out.extend_from_slice(&(self.levels_offsets.len() as u32).to_le_bytes());
        for offset in &self.levels_offsets {
            out.extend_from_slice(&offset.to_le_bytes());
        }
        out
    }
}

// ---------------------------------------------------------------------------
// `internal::OptimalPiecewiseLinearModel` (`piecewise_linear_model.hpp:44`)
// ---------------------------------------------------------------------------

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
struct Point<X> {
    x: X,
    y: u64,
}

/// `Slope { SX dx; SY dy; }` where `SX = SY = __int128` for 64-bit integral
/// keys/positions.
#[derive(Debug, Clone, Copy)]
struct Slope {
    dx: i128,
    dy: i128,
}

impl Slope {
    fn lt(&self, other: &Slope) -> bool {
        self.dy.wrapping_mul(other.dx) < self.dx.wrapping_mul(other.dy)
    }

    fn gt(&self, other: &Slope) -> bool {
        self.dy.wrapping_mul(other.dx) > self.dx.wrapping_mul(other.dy)
    }
}

fn point_diff<X: IntegralKey>(a: Point<X>, b: Point<X>) -> Slope {
    Slope {
        dx: a.x.as_i128() - b.x.as_i128(),
        dy: a.y as i128 - b.y as i128,
    }
}

/// `cross(O, A, B) = (A - O) x (B - O)` in `__int128`.
fn cross<X: IntegralKey>(o: Point<X>, a: Point<X>, b: Point<X>) -> i128 {
    let oa = point_diff(a, o);
    let ob = point_diff(b, o);
    oa.dx
        .wrapping_mul(ob.dy)
        .wrapping_sub(oa.dy.wrapping_mul(ob.dx))
}

/// `OptimalPiecewiseLinearModel<X, size_t>`.
struct OptimalModel<X> {
    epsilon: u64,
    lower: Vec<Point<X>>,
    upper: Vec<Point<X>>,
    first_x: X,
    last_x: X,
    lower_start: usize,
    upper_start: usize,
    points_in_hull: usize,
    rectangle: [Point<X>; 4],
}

impl<X: IntegralKey> OptimalModel<X> {
    fn new(epsilon: u64) -> Self {
        Self {
            epsilon,
            lower: Vec::new(),
            upper: Vec::new(),
            first_x: X::default(),
            last_x: X::default(),
            lower_start: 0,
            upper_start: 0,
            points_in_hull: 0,
            rectangle: [Point::default(); 4],
        }
    }

    /// `add_point`: returns `false` when `(x, y)` no longer fits the corridor.
    fn add_point(&mut self, x: X, y: u64) -> bool {
        debug_assert!(self.points_in_hull == 0 || x > self.last_x);
        self.last_x = x;
        let max_y = u64::MAX;
        let min_y = 0u64;
        let p1 = Point {
            x,
            y: if y >= max_y - self.epsilon {
                max_y
            } else {
                y + self.epsilon
            },
        };
        let p2 = Point {
            x,
            y: if y <= min_y + self.epsilon {
                min_y
            } else {
                y - self.epsilon
            },
        };

        if self.points_in_hull == 0 {
            self.first_x = x;
            self.rectangle[0] = p1;
            self.rectangle[1] = p2;
            self.upper.clear();
            self.lower.clear();
            self.upper.push(p1);
            self.lower.push(p2);
            self.upper_start = 0;
            self.lower_start = 0;
            self.points_in_hull += 1;
            return true;
        }

        if self.points_in_hull == 1 {
            self.rectangle[2] = p2;
            self.rectangle[3] = p1;
            self.upper.push(p1);
            self.lower.push(p2);
            self.points_in_hull += 1;
            return true;
        }

        let slope1 = point_diff(self.rectangle[2], self.rectangle[0]);
        let slope2 = point_diff(self.rectangle[3], self.rectangle[1]);
        let outside_line1 = point_diff(p1, self.rectangle[2]).lt(&slope1);
        let outside_line2 = point_diff(p2, self.rectangle[3]).gt(&slope2);

        if outside_line1 || outside_line2 {
            self.points_in_hull = 0;
            return false;
        }

        if point_diff(p1, self.rectangle[1]).lt(&slope2) {
            // Find extreme slope.
            let mut min = point_diff(self.lower[self.lower_start], p1);
            let mut min_i = self.lower_start;
            for (i, point) in self.lower.iter().enumerate().skip(self.lower_start + 1) {
                let val = point_diff(*point, p1);
                if val.gt(&min) {
                    break;
                }
                min = val;
                min_i = i;
            }

            self.rectangle[1] = self.lower[min_i];
            self.rectangle[3] = p1;
            self.lower_start = min_i;

            // Hull update.
            let mut end = self.upper.len();
            while end >= self.upper_start + 2
                && cross(self.upper[end - 2], self.upper[end - 1], p1) <= 0
            {
                end -= 1;
            }
            self.upper.truncate(end);
            self.upper.push(p1);
        }

        if point_diff(p2, self.rectangle[0]).gt(&slope1) {
            // Find extreme slope.
            let mut max = point_diff(self.upper[self.upper_start], p2);
            let mut max_i = self.upper_start;
            for (i, point) in self.upper.iter().enumerate().skip(self.upper_start + 1) {
                let val = point_diff(*point, p2);
                if val.lt(&max) {
                    break;
                }
                max = val;
                max_i = i;
            }

            self.rectangle[0] = self.upper[max_i];
            self.rectangle[2] = p2;
            self.upper_start = max_i;

            // Hull update.
            let mut end = self.lower.len();
            while end >= self.lower_start + 2
                && cross(self.lower[end - 2], self.lower[end - 1], p2) >= 0
            {
                end -= 1;
            }
            self.lower.truncate(end);
            self.lower.push(p2);
        }

        self.points_in_hull += 1;
        true
    }

    fn get_segment(&self) -> CanonicalSegment<X> {
        if self.points_in_hull == 1 {
            CanonicalSegment {
                rectangle: [
                    self.rectangle[0],
                    self.rectangle[1],
                    self.rectangle[0],
                    self.rectangle[1],
                ],
                first: self.first_x,
            }
        } else {
            CanonicalSegment {
                rectangle: self.rectangle,
                first: self.first_x,
            }
        }
    }
}

/// `OptimalPiecewiseLinearModel::CanonicalSegment`.
#[derive(Debug, Clone, Copy)]
struct CanonicalSegment<X> {
    rectangle: [Point<X>; 4],
    first: X,
}

impl<X: IntegralKey> CanonicalSegment<X> {
    fn one_point(&self) -> bool {
        let r = &self.rectangle;
        r[0].x == r[2].x && r[0].y == r[2].y && r[1].x == r[3].x && r[1].y == r[3].y
    }

    /// `get_floating_point_segment(origin)` for integral `X` and `Y`:
    /// returns `(slope, intercept)` before the `Segment` constructor narrows
    /// the intercept to `uint32_t` and the slope to `float`.
    fn floating_point_segment(&self, origin: X) -> (f64, i128) {
        let r = &self.rectangle;
        if self.one_point() {
            let mid = r[0].y.wrapping_add(r[1].y) / 2;
            return (0.0, mid as i128);
        }

        let slope = point_diff(r[3], r[1]);
        let intercept_n = slope.dy.wrapping_mul(origin.as_i128() - r[1].x.as_i128());
        let intercept_d = slope.dx;
        debug_assert!(intercept_d != 0);
        let sign = if (intercept_n < 0) ^ (intercept_d < 0) {
            -1i128
        } else {
            1i128
        };
        let rounding_term = sign * intercept_d / 2;
        let intercept = (intercept_n.wrapping_add(rounding_term)) / intercept_d + r[1].y as i128;
        (slope.dy as f64 / slope.dx as f64, intercept)
    }
}

/// `Segment::Segment(const CanonicalSegment &cs)`.
fn segment_from_canonical<X: IntegralKey>(cs: &CanonicalSegment<X>) -> PgmSegment<X> {
    let key = cs.first;
    let (slope, intercept) = cs.floating_point_segment(key);
    PgmSegment {
        key,
        slope: slope as f32,
        intercept: intercept as u32,
    }
}

/// `internal::make_segmentation(n, start, end, epsilon, in, out)`. The
/// reference uses `make_segmentation_par`, which falls back to this exact
/// serial routine whenever `n < 1 << 15` or OpenMP is disabled.
fn make_segmentation<X: IntegralKey>(
    n: usize,
    start: usize,
    end: usize,
    epsilon: u64,
    input: &dyn Fn(usize) -> X,
    out: &mut dyn FnMut(CanonicalSegment<X>),
) -> usize {
    let mut c = 0usize;
    let mut model = OptimalModel::new(epsilon);

    fn add_point<X: IntegralKey>(
        model: &mut OptimalModel<X>,
        out: &mut dyn FnMut(CanonicalSegment<X>),
        c: &mut usize,
        x: X,
        y: u64,
    ) {
        if !model.add_point(x, y) {
            out(model.get_segment());
            model.add_point(x, y);
            *c += 1;
        }
    }

    add_point(&mut model, out, &mut c, input(start), start as u64);
    for i in start + 1..end - 1 {
        if input(i) == input(i - 1) {
            // At the end of a run of duplicate keys `x` such that
            // `x + 1 != in(i + 1)`, map `x + 1 .. in(i + 1) - 1` to rank `i`.
            if input(i).next() < input(i + 1) {
                add_point(&mut model, out, &mut c, input(i).next(), i as u64);
            }
        } else {
            add_point(&mut model, out, &mut c, input(i), i as u64);
        }
    }
    if end >= start + 2 && input(end - 1) != input(end - 2) {
        add_point(&mut model, out, &mut c, input(end - 1), (end - 1) as u64);
    }
    if end == n {
        // Ensure values greater than the last one are mapped to `n`.
        add_point(&mut model, out, &mut c, input(n - 1).next(), n as u64);
    }

    out(model.get_segment());
    c + 1
}

/// A multi-level PGM over ascending keys, exactly as `PGMIndex<X, 64, 4, float>`
/// for an integral `X`.
#[derive(Debug, Default)]
struct Pgm<X> {
    n: u32,
    first_key: X,
    segments: Vec<PgmSegment<X>>,
    levels_offsets: Vec<u64>,
}

impl<X: IntegralKey> Pgm<X> {
    fn build(keys: &[X]) -> Self {
        if keys.is_empty() {
            return Self::default();
        }
        // The reference probes with `*std::prev(last) + 1`. For an 8- or
        // 16-bit key already at `numeric_limits<K>::max()` that addition
        // overflows (signed overflow is UB in the reference), and there is no
        // representable "one past the last key" to build the model over. Emit
        // an empty model instead: `lookup` then binary-searches the key array,
        // which is correct without the acceleration.
        if keys[keys.len() - 1] == X::SENTINEL {
            return Self {
                n: keys.len() as u32,
                first_key: keys[0],
                segments: Vec::new(),
                levels_offsets: Vec::new(),
            };
        }
        let last_key_plus_one = keys[keys.len() - 1].next();
        let mut segments = Vec::new();
        let mut levels_offsets = vec![0u64];

        let bottom = |i: usize| keys[i];
        let mut last_n = Self::build_level(
            &mut segments,
            keys.len(),
            PGM_EPSILON,
            &bottom,
            last_key_plus_one,
        );
        levels_offsets.push(segments.len() as u64);

        while PGM_EPSILON_RECURSIVE > 0 && last_n > 1 {
            // `in_fun_rec(i) = segments[levels_offsets[len - 2] + i].key`
            let offset = levels_offsets[levels_offsets.len() - 2] as usize;
            let level_keys: Vec<X> = (0..last_n).map(|i| segments[offset + i].key).collect();
            let input = |i: usize| level_keys[i];
            last_n = Self::build_level(
                &mut segments,
                last_n,
                PGM_EPSILON_RECURSIVE,
                &input,
                last_key_plus_one,
            );
            levels_offsets.push(segments.len() as u64);
        }

        Self {
            n: keys.len() as u32,
            first_key: keys[0],
            segments,
            levels_offsets,
        }
    }

    /// `PGMIndex::build`'s `build_level` lambda (`pgm_index.hpp:102`).
    fn build_level(
        segments: &mut Vec<PgmSegment<X>>,
        last_n: usize,
        epsilon: u64,
        input: &dyn Fn(usize) -> X,
        last_key_plus_one: X,
    ) -> usize {
        let mut n_segments = make_segmentation(last_n, 0, last_n, epsilon, input, &mut |cs| {
            segments.push(segment_from_canonical(&cs));
        });
        let last = *segments.last().expect("at least one segment");
        if last.key == X::SENTINEL {
            n_segments -= 1;
        } else {
            if last.approx(X::SENTINEL_PREV) < last_n as u64 {
                // Ensure keys > the last one are mapped to `last_n`.
                segments.push(PgmSegment {
                    key: last_key_plus_one,
                    slope: 0.0,
                    intercept: last_n as u32,
                });
            }
            segments.push(PgmSegment {
                key: X::SENTINEL,
                slope: 0.0,
                intercept: last_n as u32,
            });
        }
        n_segments
    }

    /// `segment_for_key`: the rightmost segment with `key <= k`, found by
    /// descending the recursive levels.
    fn segment_for_key(&self, key: X) -> usize {
        let height = self.levels_offsets.len() - 1;
        let mut it = self.levels_offsets[self.levels_offsets.len() - 2] as usize;
        let mut level = height as isize - 2;
        while level >= 0 {
            let level_begin = self.levels_offsets[level as usize] as usize;
            let next_intercept = self
                .segments
                .get(it + 1)
                .map_or(self.n as u64, |s| s.intercept as u64);
            let pos = self.segments[it].approx(key).min(next_intercept);
            let mut lo = level_begin + pgm_sub_eps(pos, PGM_EPSILON_RECURSIVE + 1) as usize;
            let level_end = (self.levels_offsets[level as usize + 1] as usize).saturating_sub(1);
            while lo + 1 < level_end && self.segments[lo + 1].key <= key {
                lo += 1;
            }
            it = lo;
            level -= 1;
        }
        it
    }

    /// `PGMIndex::search`: `(pos, lo, hi)` as `SecondaryIndexApproxPos`.
    fn search(&self, key: X) -> (usize, usize, usize) {
        if self.segments.is_empty() || self.levels_offsets.len() < 2 {
            return (0, 0, 0);
        }
        let k = if key >= self.first_key {
            key
        } else {
            self.first_key
        };
        let it = self.segment_for_key(k);
        let next_intercept = if it + 1 < self.segments.len() {
            self.segments[it + 1].intercept as u64
        } else {
            self.n as u64
        };
        let pos = self.segments[it].approx(k).min(next_intercept);
        let lo = pgm_sub_eps(pos, PGM_EPSILON);
        let hi = pgm_add_eps(pos, PGM_EPSILON, self.n as u64);
        (pos as usize, lo as usize, hi as usize)
    }

    fn encode(&self) -> Vec<u8> {
        let mut out = Vec::new();
        out.extend_from_slice(&self.n.to_le_bytes());
        self.first_key.push_le(&mut out);
        out.extend_from_slice(&(self.segments.len() as u32).to_le_bytes());
        for s in &self.segments {
            s.key.push_le(&mut out);
            out.extend_from_slice(&s.slope.to_le_bytes());
            out.extend_from_slice(&s.intercept.to_le_bytes());
        }
        out.extend_from_slice(&(self.levels_offsets.len() as u32).to_le_bytes());
        for offset in &self.levels_offsets {
            out.extend_from_slice(&offset.to_le_bytes());
        }
        out
    }
}

impl Pgm<u64> {
    /// Parses the PGM header at `at` and returns
    /// `(n, first_key, segment_count, level_count, offsets_at)`. Every field is
    /// bounds-checked against `buf`, so this is safe on arbitrary bytes.
    fn header(buf: &[u8], at: usize) -> Result<(u32, u64, usize, usize, usize), FormatError> {
        let read_u32 = |o: usize| -> Result<u32, FormatError> {
            if o + 4 > buf.len() {
                return Err(FormatError::Truncated {
                    need: o + 4,
                    got: buf.len(),
                });
            }
            Ok(u32::from_le_bytes(buf[o..o + 4].try_into().unwrap()))
        };
        if at + 16 > buf.len() {
            return Err(FormatError::Truncated {
                need: at + 16,
                got: buf.len(),
            });
        }
        let n = read_u32(at)?;
        let first_key = u64::from_le_bytes(buf[at + 4..at + 12].try_into().unwrap());
        let seg_count = read_u32(at + 12)? as usize;
        let levels_at = at + 16 + seg_count * 16;
        let level_count = read_u32(levels_at)? as usize;
        Ok((n, first_key, seg_count, level_count, levels_at + 4))
    }

    /// Reads the PGM at `at` in `buf`, requiring it to end exactly at `buf.len()`.
    fn decode(buf: &[u8], at: usize) -> Result<Self, FormatError> {
        let (n, first_key, seg_count, level_count, offsets_at) = Self::header(buf, at)?;
        let end = offsets_at + level_count * 8;
        if end != buf.len() {
            return Err(FormatError::Io(format!(
                "pgm tail ends at {end}, chunk is {} bytes",
                buf.len()
            )));
        }
        let segments_at = at + 16;

        let mut segments = Vec::with_capacity(seg_count);
        for i in 0..seg_count {
            let o = segments_at + i * 16;
            segments.push(PgmSegment {
                key: u64::from_le_bytes(buf[o..o + 8].try_into().unwrap()),
                slope: f32::from_le_bytes(buf[o + 8..o + 12].try_into().unwrap()),
                intercept: u32::from_le_bytes(buf[o + 12..o + 16].try_into().unwrap()),
            });
        }
        let mut levels_offsets = Vec::with_capacity(level_count);
        for i in 0..level_count {
            let o = offsets_at + i * 8;
            levels_offsets.push(u64::from_le_bytes(buf[o..o + 8].try_into().unwrap()));
        }

        Ok(Self {
            n,
            first_key,
            segments,
            levels_offsets,
        })
    }
}

/// `PGM_SUB_EPS`.
fn pgm_sub_eps(x: u64, epsilon: u64) -> u64 {
    x.saturating_sub(epsilon)
}

/// `PGM_ADD_EPS`.
fn pgm_add_eps(x: u64, epsilon: u64, size: u64) -> u64 {
    if x + epsilon + 2 >= size {
        size
    } else {
        x + epsilon + 2
    }
}

/// Writes one secondary-index chunk for `(key, offset)` pairs sorted by key.
///
/// This is the `U64` chunk the engine has always written (order-mapped integer
/// and hashed varchar/json keys). Use [`write_secondary_segment_typed`] for a
/// float/double column.
pub fn write_secondary_segment(path: &Path, entries: &[(u64, u32)]) -> Result<(), FormatError> {
    let keys: Vec<u64> = entries.iter().map(|(k, _)| *k).collect();
    let pgm = Pgm::build(&keys);
    let mut out = Vec::with_capacity(entries.len() * 12 + 64);
    for (k, _) in entries {
        out.extend_from_slice(&k.to_le_bytes());
    }
    for (_, off) in entries {
        out.extend_from_slice(&off.to_le_bytes());
    }
    out.extend_from_slice(&pgm.encode());

    write_chunk(path, &out)
}

/// Writes one secondary-index chunk with `kind`-sized keys; `entries` must be
/// sorted by [`Key::order`].
///
/// Every kind carries the reference's own PGM fit: exact integer arithmetic
/// over an integral key, `double` arithmetic over a floating-point one, so the
/// file is byte-reproducible for all of them. A `F32`/`F64` chunk stores the
/// raw IEEE bit pattern of each key, which is the reference's
/// `KeepOrderedSelf`.
pub fn write_secondary_segment_typed(
    path: &Path,
    kind: KeyKind,
    entries: &[(Key, u32)],
) -> Result<(), FormatError> {
    match kind {
        KeyKind::U64 => {
            let entries: Vec<(u64, u32)> = entries
                .iter()
                .map(|(key, off)| (u64::from_key(*key), *off))
                .collect();
            write_secondary_segment(path, &entries)
        }
        KeyKind::I32 => write_integral_segment::<i32>(path, entries),
        KeyKind::I64 => write_integral_segment::<i64>(path, entries),
        KeyKind::I16 => write_integral_segment::<i16>(path, entries),
        KeyKind::I8 => write_integral_segment::<i8>(path, entries),
        KeyKind::F32 => write_float_segment::<f32>(path, entries),
        KeyKind::F64 => write_float_segment::<f64>(path, entries),
        // A BOOL column never reaches the high-cardinality writer: the reference
        // dispatches BOOL onto the low-cardinality path unconditionally
        // (`SecondaryIndexInMemT`), and [`crate::secondary_lowcard`] is what
        // stores it.
        KeyKind::Bool => Err(FormatError::Io(
            "a BOOL secondary chunk is low-cardinality only".to_string(),
        )),
        KeyKind::Bytes => Err(FormatError::Io(
            "a JSON secondary chunk is low-cardinality only".to_string(),
        )),
    }
}

/// Writes an integral-key chunk: `[key][offset]…` followed by the reference's
/// own PGM tail over those keys.
fn write_integral_segment<X: IntegralKey>(
    path: &Path,
    entries: &[(Key, u32)],
) -> Result<(), FormatError> {
    let keys: Vec<X> = entries.iter().map(|(key, _)| X::from_key(*key)).collect();
    let pgm = Pgm::<X>::build(&keys);
    let mut out = Vec::with_capacity(entries.len() * (std::mem::size_of::<X>() + 4) + 64);
    for key in &keys {
        key.push_le(&mut out);
    }
    for (_, off) in entries {
        out.extend_from_slice(&off.to_le_bytes());
    }
    out.extend_from_slice(&pgm.encode());
    write_chunk(path, &out)
}

/// Writes a floating-point chunk: the raw `K` keys, their offsets and the
/// reference's own PGM tail over those keys.
fn write_float_segment<K: FloatKey>(
    path: &Path,
    entries: &[(Key, u32)],
) -> Result<(), FormatError> {
    let keys: Vec<K> = entries.iter().map(|(key, _)| K::from_key(*key)).collect();
    let pgm = PgmFloat::<K>::build(&keys);
    let mut out = Vec::with_capacity(entries.len() * (std::mem::size_of::<K>() + 4) + 64);
    for key in &keys {
        key.push_le(&mut out);
    }
    for (_, off) in entries {
        out.extend_from_slice(&off.to_le_bytes());
    }
    out.extend_from_slice(&pgm.encode());
    write_chunk(path, &out)
}

fn write_chunk(path: &Path, out: &[u8]) -> Result<(), FormatError> {
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// Structurally validates a `PGMWithExtraFunction<K>::Save` tail at `at`
/// (`secondary_index_pgm.cppm:129-150`) against `kind`'s element widths, and
/// requires it to end exactly at EOF. The model is not decoded: a typed chunk
/// is searched by binary search, so only the framing has to match.
fn validate_tail(buf: &[u8], at: usize, kind: KeyKind) -> Result<(), FormatError> {
    let short = |need: usize| FormatError::Truncated {
        need,
        got: buf.len(),
    };
    let read_u32 = |p: usize| -> Result<u32, FormatError> {
        if p + 4 > buf.len() {
            return Err(short(p + 4));
        }
        Ok(u32::from_le_bytes(buf[p..p + 4].try_into().unwrap()))
    };
    if at + 4 + kind.first_key_width() + 4 > buf.len() {
        return Err(short(at + 4 + kind.first_key_width() + 4));
    }
    let seg_at = at + 4 + kind.first_key_width();
    let seg_count = read_u32(seg_at)? as usize;
    let levels_at = seg_at + 4 + seg_count * kind.segment_width();
    let level_count = read_u32(levels_at)? as usize;
    let end = levels_at + 4 + level_count * 8;
    if end != buf.len() {
        return Err(short(end));
    }
    Ok(())
}

/// A chunk mapped read-only; lookups use the PGM then a binary search.
pub struct SecondarySegmentReader {
    map: MappedFile,
    row_count: usize,
    kind: KeyKind,
    /// Only a `U64` chunk decodes its model; a typed chunk is validated and
    /// then searched by binary search (`validate_tail`).
    pgm: Option<Pgm<u64>>,
}

impl SecondarySegmentReader {
    /// The chunk's row count, recovered from the model's tail (`n * keys`,
    /// `n * u32` offsets, then a model that ends at EOF).
    pub fn row_count(&self) -> usize {
        self.row_count
    }

    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        Self::open_with_kind(path, KeyKind::U64)
    }

    /// Opens a chunk whose key array holds `kind`-sized keys. The chunk has no
    /// header, so the kind has to come from the column's declared type.
    #[allow(unsafe_code)]
    pub fn open_with_kind(path: &Path, kind: KeyKind) -> Result<Self, FormatError> {
        if kind == KeyKind::Bytes {
            return Err(FormatError::Io(
                "a JSON secondary chunk is low-cardinality only".to_string(),
            ));
        }
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = MappedFile::from_mmap(
            unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?,
        );
        let min = kind.empty_tail_len();
        if map.len() < min {
            return Err(FormatError::Truncated {
                need: min,
                got: map.len(),
            });
        }
        // The chunk carries no header, so locate the PGM tail by trying the
        // plausible array lengths: `n * key_width` keys and `n * u32` offsets
        // precede a tail whose header repeats `n` and ends exactly at EOF.
        // With typed keys the tail's stride changes, which is why the width
        // comes from `kind` and not from a constant.
        let stride = kind.key_width() + 4;
        let mut row_count = None;
        // The chunk carries no header, so the row count is the `n` whose tail
        // header starts at `n * stride`, repeats `n`, repeats the chunk's own
        // first key and ends exactly at EOF. The first-key test is what keeps a
        // candidate from matching *inside* the offsets array: rows
        // `[10,10,10,20]` with offsets `[0,1,3,2]` let `n = 3` pass on the
        // repeated count alone (offset[2] is 3), and the reader then answered
        // three rows - one row's key never came back.
        let first_key = map.get(..kind.first_key_width()).map(|k| k.to_vec());
        for n in 0..=map.len() / stride {
            let pgm_at = n * stride;
            if pgm_at + 4 > map.len() {
                break;
            }
            let pgm_n = u32::from_le_bytes(map[pgm_at..pgm_at + 4].try_into().unwrap()) as usize;
            if pgm_n != n {
                continue;
            }
            if n > 0 {
                let key_at = map.get(pgm_at + 4..pgm_at + 4 + kind.first_key_width());
                if key_at != first_key.as_deref() {
                    continue;
                }
            }
            let ok = match kind {
                KeyKind::U64 => Pgm::header(&map, pgm_at)
                    .map(|(_, _, _, level_count, offsets_at)| {
                        offsets_at + level_count * 8 == map.len()
                    })
                    .unwrap_or(false),
                _ => validate_tail(&map, pgm_at, kind).is_ok(),
            };
            if ok {
                row_count = Some(n);
                break;
            }
        }
        let Some(row_count) = row_count else {
            return Err(FormatError::Truncated {
                need: map.len(),
                got: map.len(),
            });
        };
        let pgm = match kind {
            KeyKind::U64 => Some(Pgm::decode(&map, row_count * stride)?),
            _ => None,
        };
        Ok(Self {
            map,
            row_count,
            kind,
            pgm,
        })
    }

    pub fn len(&self) -> usize {
        self.row_count
    }

    pub fn is_empty(&self) -> bool {
        self.row_count == 0
    }

    /// The key kind this chunk was opened with.
    pub fn kind(&self) -> KeyKind {
        self.kind
    }

    fn key(&self, i: usize) -> Key {
        let at = i * self.kind.key_width();
        match self.kind {
            KeyKind::U64 => Key::U64(u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())),
            KeyKind::F32 => Key::F32(f32::from_bits(u32::from_le_bytes(
                self.map[at..at + 4].try_into().unwrap(),
            ))),
            KeyKind::F64 => Key::F64(f64::from_bits(u64::from_le_bytes(
                self.map[at..at + 8].try_into().unwrap(),
            ))),
            KeyKind::I32 => Key::I32(i32::from_le_bytes(self.map[at..at + 4].try_into().unwrap())),
            KeyKind::I64 => Key::I64(i64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())),
            KeyKind::I8 => Key::I8(self.map[at] as i8),
            KeyKind::I16 => Key::I16(i16::from_le_bytes(self.map[at..at + 2].try_into().unwrap())),
            KeyKind::Bool => Key::Bool(self.map[at] != 0),
            // Rejected by `open_with_kind`: a JSON chunk is low-cardinality.
            KeyKind::Bytes => Key::U64(0),
        }
    }

    fn offset(&self, i: usize) -> u32 {
        let at = self.row_count * self.kind.key_width() + i * 4;
        u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap())
    }

    /// All segment offsets whose key equals `key`.
    pub fn lookup(&self, key: Key) -> Vec<u32> {
        if self.row_count == 0 {
            return Vec::new();
        }
        // The reference searches [lower_bound_, upper_bound_) from the PGM and
        // then walks to the exact range; the model guarantees that a present
        // key lies inside that window. A typed chunk has no decoded model, so
        // it searches the ordered key array directly.
        let mut i = match (&self.pgm, key) {
            (Some(pgm), Key::U64(key)) if !pgm.segments.is_empty() => {
                let (_, lo, _hi) = pgm.search(key);
                self.lower_bound(lo.min(self.row_count), self.row_count, &Key::U64(key))
            }
            _ => self.lower_bound(0, self.row_count, &key),
        };
        let mut out = Vec::new();
        while i < self.row_count && self.key(i).order(&key) == Ordering::Equal {
            out.push(self.offset(i));
            i += 1;
        }
        // A model miss (key outside the predicted range) falls back to a full
        // binary search over the mapped keys.
        if out.is_empty() {
            let mut i = self.lower_bound(0, self.row_count, &key);
            while i < self.row_count && self.key(i).order(&key) == Ordering::Equal {
                out.push(self.offset(i));
                i += 1;
            }
        }
        out
    }

    /// All segment offsets whose key lies in the inclusive range
    /// `[start, end]`, in stored order (key order, then the order duplicates
    /// were written).
    ///
    /// The keys array is sorted, so the result is exactly the two binary
    /// searches below; no PGM hint is used because the model only bounds a
    /// *present* key, and a range query must also be correct for a bound that
    /// falls between two keys. A `start` after `end` is empty.
    pub fn range(&self, start: Key, end: Key) -> Vec<u32> {
        if self.row_count == 0 || start.order(&end) == Ordering::Greater {
            return Vec::new();
        }
        let begin = self.lower_bound(0, self.row_count, &start);
        let stop = self.upper_bound(begin, self.row_count, &end);
        (begin..stop).map(|i| self.offset(i)).collect()
    }

    /// `partition_point` over the mapped keys: first index in `[lo, hi)` whose
    /// key is `>= key`.
    fn lower_bound(&self, lo: usize, hi: usize, key: &Key) -> usize {
        let (mut lo, mut hi) = (lo, hi);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.key(mid).order(key) == Ordering::Less {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        lo
    }

    /// First index in `[lo, hi)` whose key is `> key`.
    fn upper_bound(&self, lo: usize, hi: usize, key: &Key) -> usize {
        let (mut lo, mut hi) = (lo, hi);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.key(mid).order(key) == Ordering::Greater {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        lo
    }

    /// Every `(key, offset)` pair, in stored order: key order, then the order
    /// duplicates were written. A merge reads a whole chunk this way
    /// (`InsertMergeData` over the reference's chunk buffers).
    pub fn pairs(&self) -> Vec<(Key, u32)> {
        (0..self.row_count)
            .map(|i| (self.key(i), self.offset(i)))
            .collect()
    }

    /// First key of the chunk (useful for range scans).
    pub fn first_key(&self) -> Key {
        if self.row_count == 0 {
            return Key::U64(0);
        }
        self.key(0)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A chunk whose offsets make a *smaller* row count look valid: the reader
    /// used to pick `n = 3` here (offset[2] is 3 and the bytes after it happened
    /// to look like a tail), so the last row's key never came back and
    /// `where c2 = 10` over three matching rows answered two.
    #[test]
    fn typed_chunk_keeps_every_row_when_an_offset_mimics_the_row_count() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("seg_0_0_4.idx");
        let entries = vec![
            (Key::I32(10), 0u32),
            (Key::I32(10), 1),
            (Key::I32(10), 3),
            (Key::I32(20), 2),
        ];
        write_secondary_segment_typed(&path, KeyKind::I32, &entries).unwrap();
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I32).unwrap();
        assert_eq!(
            reader.row_count(),
            4,
            "the tail header repeats the real count"
        );
        assert_eq!(reader.lookup(Key::I32(10)), vec![0, 1, 3]);
        assert_eq!(reader.lookup(Key::I32(20)), vec![2]);
    }

    fn u64_lookup(reader: &SecondarySegmentReader, key: u64) -> Vec<u32> {
        reader.lookup(Key::U64(key))
    }

    fn u64_range(reader: &SecondarySegmentReader, lo: u64, hi: u64) -> Vec<u32> {
        reader.range(Key::U64(lo), Key::U64(hi))
    }

    #[test]
    fn varchar_hash_matches_libstdcxx() {
        // Reference values from g++ `std::hash<std::string_view>`.
        assert_eq!(hash_varchar(""), 6142509188972423790);
        assert_eq!(hash_varchar("a"), 4993892634952068459);
        assert_eq!(hash_varchar("abc"), 3663726644998027833);
        assert_eq!(hash_varchar("hello world"), 5577293430985752569);
        assert_eq!(hash_varchar("default text"), 7918694999526961961);
    }

    /// PGM tail emitted by `PGMWithExtraFunction<u64>::Save` for the keys
    /// `3, 10, 17, ...` (500 keys, one level of segments).
    const CPP_LINEAR_TAIL: &[u8] = &[
        244, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 61, 12, 37, 62,
        0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 244, 1, 0, 0, 2, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
    ];

    /// PGM tail emitted by `PGMWithExtraFunction<u64>::Save` for the keys
    /// `2, 3, 6, 11, ...` (`i * i + 2`, 1000 keys). Three levels:
    /// `levels_offsets = [0, 3, 5]`.
    const CPP_QUADRATIC_TAIL: &[u8] = &[
        232, 3, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 64,
        0, 0, 0, 3, 4, 4, 0, 0, 0, 0, 0, 161, 134, 88, 58, 200, 1, 0, 0, 255, 255, 255, 255, 255,
        255, 255, 255, 0, 0, 0, 0, 232, 3, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 120, 96, 159, 55, 0, 0, 0,
        0, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
    ];

    fn linear_keys(n: u64) -> Vec<u64> {
        (0..n).map(|i| i * 7 + 3).collect()
    }

    fn quadratic_keys(n: u64) -> Vec<u64> {
        (0..n).map(|i| i * i + 2).collect()
    }

    /// The writer must emit the reference's `pgm_index_->Save` bytes verbatim.
    #[test]
    fn pgm_bytes_match_cpp_reference() {
        let keys = linear_keys(500);
        assert_eq!(Pgm::build(&keys).encode(), CPP_LINEAR_TAIL);

        let keys = quadratic_keys(1000);
        let pgm = Pgm::build(&keys);
        assert_eq!(pgm.levels_offsets, vec![0, 3, 5]);
        assert_eq!(pgm.encode(), CPP_QUADRATIC_TAIL);
    }

    /// The packed `Segment` array of a floating-point index, i.e. what the
    /// reference's `PGMIndex::segments` holds (`#pragma pack(1)`).
    fn float_segment_bytes<K: FloatKey>(pgm: &PgmFloat<K>) -> Vec<u8> {
        let mut out = Vec::new();
        for s in &pgm.segments {
            s.key.push_le(&mut out);
            K::push_slope(s.slope, &mut out);
            out.extend_from_slice(&s.intercept.to_le_bytes());
        }
        out
    }

    /// `0, 10, …, 990` for `FLOAT`, pinned by the reference's
    /// `TestPGMModel.LinearFloatKeys` (`src/unit_test/storage/secondary_index/
    /// test_pgm_ut.cpp`): the fit is not the ideal line, so the intercept is
    /// 31 rather than 0 and the guard key is `nextafter(990, +inf)`.
    ///
    /// `Segment` is `{f32, f32, u32}` = 12 bytes.
    const CPP_FLOAT_LINEAR_SEGMENTS: &[u8] = &[
        0x00, 0x00, 0x00, 0x00, 0xe9, 0xa2, 0x8b, 0x3d, 0x1f, 0x00, 0x00, 0x00, //
        0x01, 0x80, 0x77, 0x44, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
    ];

    /// The `DOUBLE` twin, pinned by `TestPGMModel.LinearDoubleKeys`. Its fit
    /// lands on intercept 32 — the same keys, a different `Floating`.
    ///
    /// `Segment` is `{f64, f64, u32}` = 20 bytes.
    const CPP_DOUBLE_LINEAR_SEGMENTS: &[u8] = &[
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0xd1, 0x45, 0x17, 0x5d, 0x74, 0xb1,
        0x3f, 0x20, 0x00, 0x00, 0x00, //
        0x01, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x8e, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x64, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x64, 0x00, 0x00, 0x00,
    ];

    #[test]
    fn float_pgm_bytes_match_cpp_reference() {
        let keys: Vec<f32> = (0..100).map(|i| i as f32 * 10.0).collect();
        let pgm = PgmFloat::build(&keys);
        // One fitted segment, the guard segment and the `+inf` sentinel; the
        // recursive level has a single segment, so there is only one level.
        assert_eq!(pgm.segments.len(), 3);
        assert_eq!(pgm.levels_offsets, vec![0, 3]);
        assert_eq!(float_segment_bytes(&pgm), CPP_FLOAT_LINEAR_SEGMENTS);
        assert_eq!(pgm.first_key.to_bits(), 0f32.to_bits());
        // `n`, `first_key`, the segment count, the segments, the level table.
        assert_eq!(pgm.encode().len(), 4 + 4 + 4 + 3 * 12 + 4 + 2 * 8);
    }

    #[test]
    fn double_pgm_bytes_match_cpp_reference() {
        let keys: Vec<f64> = (0..100).map(|i| i as f64 * 10.0).collect();
        let pgm = PgmFloat::build(&keys);
        assert_eq!(pgm.segments.len(), 3);
        assert_eq!(pgm.levels_offsets, vec![0, 3]);
        assert_eq!(float_segment_bytes(&pgm), CPP_DOUBLE_LINEAR_SEGMENTS);
        assert_eq!(pgm.encode().len(), 4 + 8 + 4 + 3 * 20 + 4 + 2 * 8);
    }

    /// The same `FLOAT` keys truncated to 65 rows, a length at which the fit is
    /// sensitive to the reference's FMA contraction: the shipped `-O3` build
    /// (`-ffp-contract=on`) reports intercept 31, a `-ffp-contract=off` build
    /// 32. `TestPGMModel` does not cover this shape, so these bytes were taken
    /// from the reference build itself.
    ///
    /// `Segment` is `{f32, f32, u32}` = 12 bytes.
    const CPP_FLOAT_LINEAR_CONTRACTED_SEGMENTS: &[u8] = &[
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x50, 0x3d, 0x1f, 0x00, 0x00, 0x00, //
        0x01, 0x00, 0x20, 0x44, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00,
    ];

    #[test]
    fn float_intercept_matches_contracted_reference_fit() {
        let keys: Vec<f32> = (0..65).map(|i| i as f32 * 10.0).collect();
        let pgm = PgmFloat::build(&keys);
        assert_eq!(pgm.segments.len(), 3);
        assert_eq!(
            float_segment_bytes(&pgm),
            CPP_FLOAT_LINEAR_CONTRACTED_SEGMENTS
        );
        assert_eq!(pgm.segments[0].intercept, 31);
    }

    /// Only the intercept is contracted, measured on a second dataset.
    ///
    /// 100 keys of `i * 0.5` (a `FLOAT` secondary key over a two-segment
    /// table) put the same first intercept one ulp away from the 65-key case:
    /// fusing the cross products of `a`/`b` gives 32 here while the reference
    /// writes 31, and the reference's own bytes are stable across runs. With
    /// only the intercept fused both datasets match.
    #[test]
    fn float_intercept_is_not_contracted_in_the_cross_products() {
        let keys: Vec<f32> = (0..100).map(|i| i as f32 * 0.5).collect();
        let pgm = PgmFloat::build(&keys);
        assert_eq!(pgm.segments[0].intercept, 31);
        assert_eq!(
            float_segment_bytes(&pgm),
            [
                0u8, 0, 0, 0, 163, 139, 174, 63, 31, 0, 0, 0, 1, 0, 70, 66, 0, 0, 0, 0, 100, 0, 0,
                0, 0, 0, 128, 127, 0, 0, 0, 0, 100, 0, 0, 0
            ]
        );
    }

    /// Keys at a magnitude where `last + 1` would round back to `last`: the
    /// guard segment's key must be the next *representable* key, otherwise it
    /// is unreachable and the model carries a dead entry
    /// (`TestPGMModel.GuardKeyFollowsLastKey`).
    #[test]
    fn float_guard_key_follows_last_key() {
        let f32_keys: Vec<f32> = (0..100).map(|i| 1e30f32 + i as f32 * 1e24f32).collect();
        let pgm = PgmFloat::build(&f32_keys);
        assert!(pgm.segments.len() >= 3);
        for pair in pgm.segments.windows(2) {
            assert!(pair[0].key < pair[1].key);
        }
        assert_eq!(
            pgm.segments[pgm.segments.len() - 2].key,
            next_up_f32(1e30f32 + 99.0 * 1e24f32)
        );

        let f64_keys: Vec<f64> = (0..100).map(|i| 1e300f64 + i as f64 * 1e285f64).collect();
        let pgm = PgmFloat::build(&f64_keys);
        assert!(pgm.segments.len() >= 3);
        for pair in pgm.segments.windows(2) {
            assert!(pair[0].key < pair[1].key);
        }
        assert_eq!(
            pgm.segments[pgm.segments.len() - 2].key,
            next_up_f64(1e300f64 + 99.0 * 1e285f64)
        );
    }

    #[test]
    fn pgm_lookup_matches_binary_search() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_0.idx");
        let entries: Vec<(u64, u32)> = (0..500u64).map(|i| (i * 7 + 3, i as u32)).collect();
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.len(), 500);
        for i in [0usize, 1, 63, 64, 250, 499] {
            let key = entries[i].0;
            let hits = u64_lookup(&reader, key);
            assert_eq!(hits, vec![entries[i].1], "key {key}");
        }
        assert!(u64_lookup(&reader, 1).is_empty());
        assert!(u64_lookup(&reader, u64::MAX).is_empty());
    }

    #[test]
    fn duplicate_keys_return_every_offset() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_1.idx");
        let entries = vec![(1u64, 10u32), (1, 11), (2, 12), (3, 13)];
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(u64_lookup(&reader, 1), vec![10, 11]);
        assert_eq!(u64_lookup(&reader, 3), vec![13]);
    }

    /// A chunk laid out by the reference (`keys`, `offsets`, then the PGM tail
    /// above) must open and answer lookups.
    #[test]
    fn opens_cpp_shaped_multi_level_chunk() {
        let keys = quadratic_keys(1000);
        let mut chunk = Vec::with_capacity(keys.len() * 12 + CPP_QUADRATIC_TAIL.len());
        for key in &keys {
            chunk.extend_from_slice(&key.to_le_bytes());
        }
        for i in 0..keys.len() as u32 {
            chunk.extend_from_slice(&i.to_le_bytes());
        }
        chunk.extend_from_slice(CPP_QUADRATIC_TAIL);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp.idx");
        std::fs::write(&path, &chunk).unwrap();

        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.len(), 1000);
        assert_eq!(reader.first_key(), Key::U64(2));
        assert_eq!(u64_lookup(&reader, 2), vec![0]);
        assert_eq!(u64_lookup(&reader, 250_002), vec![500]);
        assert_eq!(u64_lookup(&reader, 998_003), vec![999]);
        for (i, key) in keys.iter().enumerate() {
            assert_eq!(u64_lookup(&reader, *key), vec![i as u32], "key {key}");
        }
        assert!(u64_lookup(&reader, 4).is_empty());
        assert!(u64_lookup(&reader, 0).is_empty());
    }

    /// An empty index writes only the empty PGM header; the reference's
    /// `build()` returns before pushing any segment or level.
    #[test]
    fn empty_chunk_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("empty.idx");
        write_secondary_segment(&path, &[]).unwrap();
        assert_eq!(std::fs::read(&path).unwrap().len(), 20);
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.len(), 0);
        assert!(reader.is_empty());
        assert!(u64_lookup(&reader, 7).is_empty());
        assert!(u64_range(&reader, 0, u64::MAX).is_empty());
    }

    /// `range` is inclusive at both ends and reports the offsets in stored
    /// order.
    #[test]
    fn range_is_inclusive_at_both_ends() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_range.idx");
        let entries = vec![(1u64, 10u32), (2, 11), (3, 12), (5, 13), (8, 14)];
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();

        assert_eq!(u64_range(&reader, 2, 5), vec![11, 12, 13]);
        assert_eq!(u64_range(&reader, 2, 2), vec![11]);
        assert_eq!(u64_range(&reader, 1, 8), vec![10, 11, 12, 13, 14]);
        // Bounds that fall between two keys still cover the keys between them.
        assert_eq!(u64_range(&reader, 3, 4), vec![12]);
        assert_eq!(u64_range(&reader, 4, 7), vec![13]);
        // Both ends outside the key domain.
        assert!(u64_range(&reader, 0, 0).is_empty());
        assert!(u64_range(&reader, 9, 100).is_empty());
        // An unbounded upper end.
        assert_eq!(u64_range(&reader, 5, u64::MAX), vec![13, 14]);
        // An inverted range is empty, not the whole chunk.
        assert!(u64_range(&reader, 5, 2).is_empty());
    }

    /// Every offset of a duplicated key is reported by `range`, as by
    /// `lookup`.
    #[test]
    fn range_returns_every_offset_of_a_duplicated_key() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_range_dup.idx");
        let entries = vec![(1u64, 10u32), (2, 11), (2, 12), (2, 13), (3, 14)];
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(u64_range(&reader, 2, 2), vec![11, 12, 13]);
        assert_eq!(u64_range(&reader, 2, 3), vec![11, 12, 13, 14]);
        assert_eq!(u64_range(&reader, 1, 3), vec![10, 11, 12, 13, 14]);
    }

    /// The binary searches must also answer over a multi-level PGM chunk
    /// written by the reference.
    #[test]
    fn range_walks_a_cpp_shaped_multi_level_chunk() {
        let keys = quadratic_keys(1000);
        let mut chunk = Vec::with_capacity(keys.len() * 12 + CPP_QUADRATIC_TAIL.len());
        for key in &keys {
            chunk.extend_from_slice(&key.to_le_bytes());
        }
        for i in 0..keys.len() as u32 {
            chunk.extend_from_slice(&i.to_le_bytes());
        }
        chunk.extend_from_slice(CPP_QUADRATIC_TAIL);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp_range.idx");
        std::fs::write(&path, &chunk).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();

        // `i * i + 2` for i in 3..=5 is 11, 18, 27.
        assert_eq!(u64_range(&reader, 11, 27), vec![3, 4, 5]);
        assert_eq!(u64_range(&reader, 2, 2), vec![0]);
        // Keys 3 (i = 1) and 6 (i = 2) both fall inside.
        assert_eq!(u64_range(&reader, 3, 6), vec![1, 2]);
        // Everything, and nothing.
        assert_eq!(u64_range(&reader, 0, u64::MAX).len(), 1000);
        assert!(u64_range(&reader, 999_000, u64::MAX).is_empty());
    }

    fn f32_entries() -> Vec<(Key, u32)> {
        vec![
            (Key::F32(-3.5), 10),
            (Key::F32(-1.0), 11),
            (Key::F32(1.5), 12),
            (Key::F32(2.5), 13),
        ]
    }

    /// A `FLOAT` chunk stores 4-byte raw IEEE keys, not the 8-byte order-mapped
    /// key the integer path uses: this is the layout the reference's
    /// `ConvertToOrderedType<FloatT>` = `float` produces, and what the old
    /// reader could not parse.
    #[test]
    fn float_chunk_keeps_raw_ordered_keys() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_f32.idx");
        let entries = f32_entries();
        write_secondary_segment_typed(&path, KeyKind::F32, &entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();

        let n = entries.len();
        // Key region: n * 4 bytes of raw f32.
        let keys: Vec<u8> = entries
            .iter()
            .flat_map(|(key, _)| match key {
                Key::F32(v) => v.to_bits().to_le_bytes(),
                _ => unreachable!(),
            })
            .collect();
        assert_eq!(&bytes[..n * 4], &keys[..]);
        assert_eq!(
            &bytes[..4],
            &(-3.5f32).to_bits().to_le_bytes()[..],
            "raw f32 LE"
        );
        // Offsets follow, 4 bytes each.
        for (i, (_, off)) in entries.iter().enumerate() {
            let at = n * 4 + i * 4;
            assert_eq!(&bytes[at..at + 4], &off.to_le_bytes()[..]);
        }
        // The tail starts with `n`, then a 4-byte `first_key`.
        let tail_at = n * 4 + n * 4;
        assert_eq!(&bytes[tail_at..tail_at + 4], &(n as u32).to_le_bytes()[..]);
        assert_eq!(
            &bytes[tail_at + 4..tail_at + 8],
            &(-3.5f32).to_bits().to_le_bytes()[..]
        );
        // `fitted segments, a guard segment for keys after the last one, then
        // the +inf sentinel`, 12 bytes each.
        let seg_count =
            u32::from_le_bytes(bytes[tail_at + 8..tail_at + 12].try_into().unwrap()) as usize;
        assert_eq!(
            bytes.len(),
            tail_at + 4 + 4 + 4 + seg_count * 12 + 4 + 2 * 8
        );
        assert!(seg_count >= 2);

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F32).unwrap();
        assert_eq!(reader.len(), n);
        assert_eq!(reader.kind(), KeyKind::F32);
        assert_eq!(reader.first_key(), Key::F32(-3.5));
    }

    /// Same for `DOUBLE`, whose key is 8 bytes but *raw* (not order-mapped) and
    /// whose `Segment` is 24 bytes because `double slope` pads the struct.
    #[test]
    fn double_chunk_keeps_raw_ordered_keys() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_f64.idx");
        let entries = vec![
            (Key::F64(-3.5), 10u32),
            (Key::F64(1.5), 11),
            (Key::F64(2.5), 12),
        ];
        write_secondary_segment_typed(&path, KeyKind::F64, &entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        let n = entries.len();

        let keys: Vec<u8> = entries
            .iter()
            .flat_map(|(key, _)| match key {
                Key::F64(v) => v.to_bits().to_le_bytes(),
                _ => unreachable!(),
            })
            .collect();
        assert_eq!(&bytes[..n * 8], &keys[..]);
        let tail_at = n * 8 + n * 4;
        assert_eq!(&bytes[tail_at..tail_at + 4], &(n as u32).to_le_bytes()[..]);
        // 20 bytes each (`#pragma pack(1)`: no padding).
        let seg_count =
            u32::from_le_bytes(bytes[tail_at + 12..tail_at + 16].try_into().unwrap()) as usize;
        assert_eq!(
            bytes.len(),
            tail_at + 4 + 8 + 4 + seg_count * 20 + 4 + 2 * 8
        );
        assert!(seg_count >= 2);

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F64).unwrap();
        assert_eq!(reader.len(), n);
        assert_eq!(reader.first_key(), Key::F64(-3.5));
    }

    /// Typed keys compare as floats: negatives sort before positives (a raw
    /// `to_bits` integer comparison would invert them), both range bounds are
    /// inclusive, and a duplicated key yields every offset.
    #[test]
    fn float_lookup_and_range_compare_as_floats() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_f32_range.idx");
        let entries = vec![
            (Key::F32(-3.5), 10u32),
            (Key::F32(-1.0), 11),
            (Key::F32(1.0), 12),
            (Key::F32(1.0), 13),
            (Key::F32(2.0), 14),
            (Key::F32(3.5), 15),
        ];
        write_secondary_segment_typed(&path, KeyKind::F32, &entries).unwrap();
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F32).unwrap();
        let range = |lo: f32, hi: f32| reader.range(Key::F32(lo), Key::F32(hi));

        // The sign-bit trap: -3.5 must come before -1.0, and both before 1.0.
        assert_eq!(range(-4.0, -1.0), vec![10, 11]);
        assert_eq!(range(-3.5, -3.5), vec![10]);
        // Inclusive at both ends, and a duplicated key reports every offset.
        assert_eq!(range(1.0, 2.0), vec![12, 13, 14]);
        assert_eq!(reader.lookup(Key::F32(1.0)), vec![12, 13]);
        assert_eq!(reader.lookup(Key::F32(1.5)), Vec::<u32>::new());
        // Across zero, and past the last key.
        assert_eq!(range(-1.0, 1.0), vec![11, 12, 13]);
        assert!(range(4.0, 10.0).is_empty());
        assert!(range(2.0, 1.0).is_empty(), "inverted range");
    }

    /// A chunk laid out the way `PGMWithExtraFunction<float>::Save` writes it
    /// (`secondary_index_pgm.cppm:129`): `f32` keys, then offsets, then `n`,
    /// `first_key`, `{f32 key, f32 slope, u32 intercept}` segments and the
    /// level table. Opens and answers lookups and ranges.
    ///
    /// The model's slope/intercept are synthetic (0), not what
    /// `PGMIndex<float, 64, 4, float>` would fit: producing genuine reference
    /// output needs the C++ toolchain. The reader binary-searches a typed
    /// chunk, so what this fixture proves is that the *layout* (key width,
    /// segment width, level table) is the reference's.
    #[test]
    fn cpp_shaped_float_chunk_opens() {
        let keys = [-3.5f32, 1.5, 2.5];
        let mut chunk = Vec::new();
        for key in keys {
            chunk.extend_from_slice(&key.to_bits().to_le_bytes());
        }
        for off in 0..keys.len() as u32 {
            chunk.extend_from_slice(&off.to_le_bytes());
        }
        chunk.extend_from_slice(&(keys.len() as u32).to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_bits().to_le_bytes());
        chunk.extend_from_slice(&1u32.to_le_bytes()); // one segment
        chunk.extend_from_slice(&keys[0].to_bits().to_le_bytes());
        chunk.extend_from_slice(&0.0f32.to_bits().to_le_bytes());
        chunk.extend_from_slice(&0u32.to_le_bytes());
        chunk.extend_from_slice(&2u32.to_le_bytes()); // level table
        chunk.extend_from_slice(&0u64.to_le_bytes());
        chunk.extend_from_slice(&1u64.to_le_bytes());

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp_f32.idx");
        std::fs::write(&path, &chunk).unwrap();

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F32).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(Key::F32(1.5)), vec![1]);
        assert_eq!(reader.range(Key::F32(-4.0), Key::F32(2.0)), vec![0, 1]);
        assert_eq!(reader.range(Key::F32(-4.0), Key::F32(3.0)), vec![0, 1, 2]);

        // The kind is not in the file: reading the same bytes as u64 keys must
        // not silently succeed with garbage.
        assert!(SecondarySegmentReader::open(&path).is_err());
    }

    /// The `DOUBLE` twin of `cpp_shaped_float_chunk_opens`: 8-byte keys and
    /// 20-byte packed `{f64, f64, u32}` segments (`pgm_index.hpp:225`).
    #[test]
    fn cpp_shaped_double_chunk_opens() {
        let keys = [-3.5f64, 1.5, 2.5];
        let mut chunk = Vec::new();
        for key in keys {
            chunk.extend_from_slice(&key.to_bits().to_le_bytes());
        }
        for off in 0..keys.len() as u32 {
            chunk.extend_from_slice(&off.to_le_bytes());
        }
        chunk.extend_from_slice(&(keys.len() as u32).to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_bits().to_le_bytes());
        chunk.extend_from_slice(&1u32.to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_bits().to_le_bytes());
        chunk.extend_from_slice(&0.0f64.to_bits().to_le_bytes());
        chunk.extend_from_slice(&0u32.to_le_bytes());
        chunk.extend_from_slice(&2u32.to_le_bytes());
        chunk.extend_from_slice(&0u64.to_le_bytes());
        chunk.extend_from_slice(&1u64.to_le_bytes());
        assert_eq!(chunk.len(), 3 * 8 + 3 * 4 + 4 + 8 + 4 + 20 + 4 + 16);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp_f64.idx");
        std::fs::write(&path, &chunk).unwrap();

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F64).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(Key::F64(2.5)), vec![2]);
        assert_eq!(reader.range(Key::F64(-4.0), Key::F64(2.0)), vec![0, 1]);
    }

    /// `DATE`/`TIME` (`ConvertToOrderedI32`) is a raw signed 4-byte key and
    /// `DATETIME`/`TIMESTAMP` (`ConvertToOrderedI64`) a raw signed 8-byte one.
    /// Negative keys — dates before 1970 — must sort first, and both bounds of
    /// a range are inclusive.
    #[test]
    fn i32_chunk_keeps_raw_signed_keys() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_i32.idx");
        let entries = vec![
            (Key::I32(-20), 10u32),
            (Key::I32(0), 11),
            (Key::I32(19_000), 12),
        ];
        write_secondary_segment_typed(&path, KeyKind::I32, &entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        let n = entries.len();
        let keys: Vec<u8> = entries
            .iter()
            .flat_map(|(key, _)| match key {
                Key::I32(v) => v.to_le_bytes(),
                _ => unreachable!(),
            })
            .collect();
        assert_eq!(&bytes[..n * 4], &keys[..], "raw i32 LE, 4 bytes each");
        let tail_at = n * 4 + n * 4;
        assert_eq!(&bytes[tail_at..tail_at + 4], &(n as u32).to_le_bytes()[..]);
        assert_eq!(
            &bytes[tail_at + 4..tail_at + 8],
            &(-20i32).to_le_bytes()[..]
        );
        // 1 segment + sentinel, 12 bytes each (`{i32, f32, u32}`).
        assert_eq!(bytes.len(), tail_at + 4 + 4 + 4 + 2 * 12 + 4 + 2 * 8);

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I32).unwrap();
        assert_eq!(reader.len(), n);
        assert_eq!(reader.first_key(), Key::I32(-20));
        assert_eq!(reader.lookup(Key::I32(-20)), vec![10]);
        // The sign-bit trap again: -20 sorts before 0 and 19000.
        assert_eq!(reader.range(Key::I32(-20), Key::I32(0)), vec![10, 11]);
        assert_eq!(reader.range(Key::I32(0), Key::I32(19_000)), vec![11, 12]);
        // Nothing between 1 and 18999.
        assert!(reader.range(Key::I32(1), Key::I32(18_999)).is_empty());
        assert!(
            SecondarySegmentReader::open(&path).is_err(),
            "kind is required"
        );
    }

    /// The `DATETIME`/`TIMESTAMP` twin: 8-byte signed keys, 16-byte
    /// `{i64, f32, u32}` segments.
    #[test]
    fn i64_chunk_keeps_raw_signed_keys() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_i64.idx");
        let entries = vec![
            (Key::I64(-86_400), 10u32),
            (Key::I64(0), 11),
            (Key::I64(1_700_000_000), 12),
        ];
        write_secondary_segment_typed(&path, KeyKind::I64, &entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        let n = entries.len();
        let keys: Vec<u8> = entries
            .iter()
            .flat_map(|(key, _)| match key {
                Key::I64(v) => v.to_le_bytes(),
                _ => unreachable!(),
            })
            .collect();
        assert_eq!(&bytes[..n * 8], &keys[..], "raw i64 LE, 8 bytes each");
        let tail_at = n * 8 + n * 4;
        // 1 segment + sentinel, 16 bytes each (`{i64, f32, u32}`).
        assert_eq!(bytes.len(), tail_at + 4 + 8 + 4 + 2 * 16 + 4 + 2 * 8);

        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I64).unwrap();
        assert_eq!(reader.first_key(), Key::I64(-86_400));
        assert_eq!(reader.range(Key::I64(-86_400), Key::I64(0)), vec![10, 11]);
        assert_eq!(reader.lookup(Key::I64(1_700_000_000)), vec![12]);
    }

    /// C++-shaped `DATE`/`TIME` chunk (`PGMWithExtraFunction<i32>::Save`):
    /// 4-byte keys, 12-byte packed segments.
    #[test]
    fn cpp_shaped_i32_chunk_opens() {
        let keys = [-20i32, 0, 19_000];
        let mut chunk = Vec::new();
        for key in keys {
            chunk.extend_from_slice(&key.to_le_bytes());
        }
        for off in 0..keys.len() as u32 {
            chunk.extend_from_slice(&off.to_le_bytes());
        }
        chunk.extend_from_slice(&(keys.len() as u32).to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_le_bytes());
        chunk.extend_from_slice(&1u32.to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_le_bytes());
        chunk.extend_from_slice(&0.0f32.to_bits().to_le_bytes());
        chunk.extend_from_slice(&0u32.to_le_bytes());
        chunk.extend_from_slice(&2u32.to_le_bytes());
        chunk.extend_from_slice(&0u64.to_le_bytes());
        chunk.extend_from_slice(&1u64.to_le_bytes());
        assert_eq!(chunk.len(), 3 * 4 + 3 * 4 + 4 + 4 + 4 + 12 + 4 + 16);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp_i32.idx");
        std::fs::write(&path, &chunk).unwrap();
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I32).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(Key::I32(0)), vec![1]);
        assert_eq!(reader.range(Key::I32(-20), Key::I32(0)), vec![0, 1]);
    }

    /// C++-shaped `DATETIME`/`TIMESTAMP` chunk: 8-byte keys, 16-byte segments.
    #[test]
    fn cpp_shaped_i64_chunk_opens() {
        let keys = [-86_400i64, 0, 1_700_000_000];
        let mut chunk = Vec::new();
        for key in keys {
            chunk.extend_from_slice(&key.to_le_bytes());
        }
        for off in 0..keys.len() as u32 {
            chunk.extend_from_slice(&off.to_le_bytes());
        }
        chunk.extend_from_slice(&(keys.len() as u32).to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_le_bytes());
        chunk.extend_from_slice(&1u32.to_le_bytes());
        chunk.extend_from_slice(&keys[0].to_le_bytes());
        chunk.extend_from_slice(&0.0f32.to_bits().to_le_bytes());
        chunk.extend_from_slice(&0u32.to_le_bytes());
        chunk.extend_from_slice(&2u32.to_le_bytes());
        chunk.extend_from_slice(&0u64.to_le_bytes());
        chunk.extend_from_slice(&1u64.to_le_bytes());
        assert_eq!(chunk.len(), 3 * 8 + 3 * 4 + 4 + 8 + 4 + 16 + 4 + 16);

        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_cpp_i64.idx");
        std::fs::write(&path, &chunk).unwrap();
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I64).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(Key::I64(1_700_000_000)), vec![2]);
        assert_eq!(reader.range(Key::I64(-86_400), Key::I64(0)), vec![0, 1]);
    }

    /// An empty typed chunk round-trips. `PGMIndex::build` returns before it
    /// pushes a segment or a level (`pgm_index.hpp:97-99`), so the tail is
    /// `n = 0`, `first_key = 0`, no segments and no levels: 16 bytes for an
    /// `f32` key, the reference's own empty model.
    #[test]
    fn empty_typed_chunk_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("empty_f32.idx");
        write_secondary_segment_typed(&path, KeyKind::F32, &[]).unwrap();
        assert_eq!(std::fs::read(&path).unwrap().len(), 4 + 4 + 4 + 4);
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F32).unwrap();
        assert_eq!(reader.len(), 0);
        assert!(reader.lookup(Key::F32(1.0)).is_empty());
        assert!(reader.range(Key::F32(-1.0), Key::F32(1.0)).is_empty());
    }

    /// TINYINT/SMALLINT chunks store `int8_t`/`int16_t` keys - one and two bytes,
    /// the reference's `KeepOrderedSelf` width - and the PGM tail's stride
    /// follows it.
    #[test]
    fn narrow_integer_chunks_use_their_own_width() {
        let dir = tempfile::tempdir().unwrap();

        let i8_entries: Vec<(Key, u32)> = [(-128i8, 0u32), (-1, 1), (0, 2), (127, 3)]
            .into_iter()
            .map(|(key, offset)| (Key::I8(key), offset))
            .collect();
        let path = dir.path().join("i8.idx");
        write_secondary_segment_typed(&path, KeyKind::I8, &i8_entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        assert_eq!(bytes[0] as i8, -128);
        assert_eq!(bytes[1] as i8, -1);
        assert_eq!(bytes[3] as i8, 127);
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I8).unwrap();
        assert_eq!(reader.len(), 4);
        assert_eq!(reader.lookup(Key::I8(-1)), vec![1]);
        assert_eq!(reader.range(Key::I8(0), Key::I8(127)), vec![2, 3]);

        let i16_entries: Vec<(Key, u32)> = [(-32768i16, 0u32), (7, 1), (32767, 2)]
            .into_iter()
            .map(|(key, offset)| (Key::I16(key), offset))
            .collect();
        let path = dir.path().join("i16.idx");
        write_secondary_segment_typed(&path, KeyKind::I16, &i16_entries).unwrap();
        let bytes = std::fs::read(&path).unwrap();
        assert_eq!(i16::from_le_bytes([bytes[0], bytes[1]]), -32768);
        assert_eq!(i16::from_le_bytes([bytes[2], bytes[3]]), 7);
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::I16).unwrap();
        assert_eq!(reader.len(), 3);
        assert_eq!(reader.lookup(Key::I16(7)), vec![1]);
        assert_eq!(reader.range(Key::I16(-32768), Key::I16(0)), vec![0]);
    }

    /// BOOL and JSON chunks are low-cardinality only; the high-cardinality
    /// writer refuses them instead of writing a chunk the reference cannot read.
    #[test]
    fn bool_and_json_chunks_are_refused_by_the_pgm_writer() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("bool.idx");
        assert!(
            write_secondary_segment_typed(&path, KeyKind::Bool, &[(Key::Bool(true), 0)]).is_err()
        );
        let path = dir.path().join("json.idx");
        assert!(write_secondary_segment_typed(&path, KeyKind::Bytes, &[]).is_err());
    }
}

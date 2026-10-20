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
//! | `F64` (DOUBLE) | `double` / `double` | 8 | `{f64,f64,u32}` = 24 (4 bytes of padding) |
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
//! Only the `U64` model is *built* the way the reference builds it (and is
//! byte-identical to it, see the tests). A `F32`/`F64` chunk gets a flat model
//! — one segment per 64 keys with `slope = 0` and `intercept = first index` —
//! which is exact for any monotone key array and readable by the reference,
//! but is not the fit the reference would produce. The reader does not consult
//! the typed model at all: it binary-searches the key array, which is what
//! makes it correct for a chunk the reference wrote.

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
    let mut chunks = data.chunks_exact(8);
    for chunk in &mut chunks {
        let mut k = u64::from_le_bytes(chunk.try_into().unwrap());
        k = k.wrapping_mul(M);
        k ^= k >> R;
        k = k.wrapping_mul(M);
        h ^= k;
        h = h.wrapping_mul(M);
    }
    let tail = chunks.remainder();
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

use crate::cpp_format::FormatError;

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
}

impl KeyKind {
    /// `sizeof(OrderedKeyType)`.
    pub fn key_width(self) -> usize {
        match self {
            KeyKind::U64 | KeyKind::F64 => 8,
            KeyKind::F32 => 4,
        }
    }

    /// `sizeof(Segment)` for `PGMIndex<K, 64, 4, Floating>`: `{K key; Floating
    /// slope; u32 intercept;}`, padded to the struct's alignment. `f64` keys
    /// make it 24 bytes (4 bytes after the intercept).
    pub fn segment_width(self) -> usize {
        match self {
            KeyKind::U64 => 16,
            KeyKind::F32 => 12,
            KeyKind::F64 => 24,
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
}

impl Key {
    pub fn kind(self) -> KeyKind {
        match self {
            Key::U64(_) => KeyKind::U64,
            Key::F32(_) => KeyKind::F32,
            Key::F64(_) => KeyKind::F64,
        }
    }

    pub fn order(&self, other: &Key) -> Ordering {
        match (self, other) {
            (Key::U64(a), Key::U64(b)) => a.cmp(b),
            _ => self.as_f64().total_cmp(&other.as_f64()),
        }
    }

    pub fn as_f64(self) -> f64 {
        match self {
            Key::U64(v) => v as f64,
            Key::F32(v) => v as f64,
            Key::F64(v) => v,
        }
    }

    /// The raw little-endian bytes this key occupies in a chunk, at the key
    /// width of `kind`.
    fn to_le_bytes_in(self, kind: KeyKind) -> Vec<u8> {
        match kind {
            KeyKind::U64 => {
                let bits = match self {
                    Key::U64(v) => v,
                    other => other.as_f64() as u64,
                };
                bits.to_le_bytes().to_vec()
            }
            KeyKind::F32 => (self.as_f64() as f32).to_bits().to_le_bytes().to_vec(),
            KeyKind::F64 => self.as_f64().to_bits().to_le_bytes().to_vec(),
        }
    }
}

/// One piecewise-linear segment: a key, a slope and an intercept.
///
/// `(#pragma pack(1)) Segment { K key; Floating slope; uint32_t intercept; }`
/// with `K = u64` and `Floating = float` (`pgm_index.hpp:225-267`).
#[derive(Debug, Clone, Copy)]
struct PgmSegment {
    key: u64,
    slope: f32,
    intercept: u32,
}

impl PgmSegment {
    /// `Segment::operator()`: `size_t(slope * double(k - key)) + intercept`.
    /// For `K = u64` the reference subtracts as `uint64_t`, so a smaller key
    /// wraps exactly as here.
    fn approx(&self, key: u64) -> u64 {
        let stride = (self.slope as f64 * key.wrapping_sub(self.key) as f64) as u64;
        stride.wrapping_add(self.intercept as u64)
    }
}

// ---------------------------------------------------------------------------
// `internal::OptimalPiecewiseLinearModel` (`piecewise_linear_model.hpp:44`)
// ---------------------------------------------------------------------------

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Point {
    x: u64,
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

fn point_diff(a: Point, b: Point) -> Slope {
    Slope {
        dx: a.x as i128 - b.x as i128,
        dy: a.y as i128 - b.y as i128,
    }
}

/// `cross(O, A, B) = (A - O) x (B - O)` in `__int128`.
fn cross(o: Point, a: Point, b: Point) -> i128 {
    let oa = point_diff(a, o);
    let ob = point_diff(b, o);
    oa.dx
        .wrapping_mul(ob.dy)
        .wrapping_sub(oa.dy.wrapping_mul(ob.dx))
}

/// `OptimalPiecewiseLinearModel<u64, size_t>`.
struct OptimalModel {
    epsilon: u64,
    lower: Vec<Point>,
    upper: Vec<Point>,
    first_x: u64,
    last_x: u64,
    lower_start: usize,
    upper_start: usize,
    points_in_hull: usize,
    rectangle: [Point; 4],
}

impl OptimalModel {
    fn new(epsilon: u64) -> Self {
        Self {
            epsilon,
            lower: Vec::new(),
            upper: Vec::new(),
            first_x: 0,
            last_x: 0,
            lower_start: 0,
            upper_start: 0,
            points_in_hull: 0,
            rectangle: [Point { x: 0, y: 0 }; 4],
        }
    }

    /// `add_point`: returns `false` when `(x, y)` no longer fits the corridor.
    fn add_point(&mut self, x: u64, y: u64) -> bool {
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
            for i in self.lower_start + 1..self.lower.len() {
                let val = point_diff(self.lower[i], p1);
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
            for i in self.upper_start + 1..self.upper.len() {
                let val = point_diff(self.upper[i], p2);
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

    fn get_segment(&self) -> CanonicalSegment {
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
struct CanonicalSegment {
    rectangle: [Point; 4],
    first: u64,
}

impl CanonicalSegment {
    fn one_point(&self) -> bool {
        let r = &self.rectangle;
        r[0].x == r[2].x && r[0].y == r[2].y && r[1].x == r[3].x && r[1].y == r[3].y
    }

    /// `get_floating_point_segment(origin)` for integral `X` and `Y`:
    /// returns `(slope, intercept)` before the `Segment` constructor narrows
    /// the intercept to `uint32_t` and the slope to `float`.
    fn floating_point_segment(&self, origin: u64) -> (f64, i128) {
        let r = &self.rectangle;
        if self.one_point() {
            let mid = r[0].y.wrapping_add(r[1].y) / 2;
            return (0.0, mid as i128);
        }

        let slope = point_diff(r[3], r[1]);
        let intercept_n = slope.dy.wrapping_mul(origin as i128 - r[1].x as i128);
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
fn segment_from_canonical(cs: &CanonicalSegment) -> PgmSegment {
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
fn make_segmentation(
    n: usize,
    start: usize,
    end: usize,
    epsilon: u64,
    input: &dyn Fn(usize) -> u64,
    out: &mut dyn FnMut(CanonicalSegment),
) -> usize {
    let mut c = 0usize;
    let mut model = OptimalModel::new(epsilon);

    fn add_point(
        model: &mut OptimalModel,
        out: &mut dyn FnMut(CanonicalSegment),
        c: &mut usize,
        x: u64,
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
            if input(i).wrapping_add(1) < input(i + 1) {
                add_point(&mut model, out, &mut c, input(i).wrapping_add(1), i as u64);
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
        add_point(
            &mut model,
            out,
            &mut c,
            input(n - 1).wrapping_add(1),
            n as u64,
        );
    }

    out(model.get_segment());
    c + 1
}

/// A multi-level PGM over ascending keys, exactly as `PGMIndex<u64, 64, 4, float>`.
#[derive(Debug, Default)]
struct Pgm {
    n: u32,
    first_key: u64,
    segments: Vec<PgmSegment>,
    levels_offsets: Vec<u64>,
}

impl Pgm {
    fn build(keys: &[u64]) -> Self {
        if keys.is_empty() {
            return Self::default();
        }
        let last_key_plus_one = keys[keys.len() - 1].wrapping_add(1);
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
            let level_keys: Vec<u64> = (0..last_n).map(|i| segments[offset + i].key).collect();
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
        segments: &mut Vec<PgmSegment>,
        last_n: usize,
        epsilon: u64,
        input: &dyn Fn(usize) -> u64,
        last_key_plus_one: u64,
    ) -> usize {
        let mut n_segments = make_segmentation(last_n, 0, last_n, epsilon, input, &mut |cs| {
            segments.push(segment_from_canonical(&cs));
        });
        let last = *segments.last().expect("at least one segment");
        if last.key == u64::MAX {
            n_segments -= 1;
        } else {
            if last.approx(u64::MAX - 1) < last_n as u64 {
                // Ensure keys > the last one are mapped to `last_n`.
                segments.push(PgmSegment {
                    key: last_key_plus_one,
                    slope: 0.0,
                    intercept: last_n as u32,
                });
            }
            segments.push(PgmSegment {
                key: u64::MAX,
                slope: 0.0,
                intercept: last_n as u32,
            });
        }
        n_segments
    }

    /// `segment_for_key`: the rightmost segment with `key <= k`, found by
    /// descending the recursive levels.
    fn segment_for_key(&self, key: u64) -> usize {
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
    fn search(&self, key: u64) -> (usize, usize, usize) {
        if self.segments.is_empty() || self.levels_offsets.len() < 2 {
            return (0, 0, 0);
        }
        let k = key.max(self.first_key);
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
        out.extend_from_slice(&self.first_key.to_le_bytes());
        out.extend_from_slice(&(self.segments.len() as u32).to_le_bytes());
        for s in &self.segments {
            out.extend_from_slice(&s.key.to_le_bytes());
            out.extend_from_slice(&s.slope.to_le_bytes());
            out.extend_from_slice(&s.intercept.to_le_bytes());
        }
        out.extend_from_slice(&(self.levels_offsets.len() as u32).to_le_bytes());
        for offset in &self.levels_offsets {
            out.extend_from_slice(&offset.to_le_bytes());
        }
        out
    }

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
/// sorted by [`Key::cmp`].
///
/// A `U64` chunk is byte-identical to [`write_secondary_segment`]. A
/// `F32`/`F64` chunk stores the raw IEEE bit pattern of each key and a flat
/// PGM tail (see the module comment): the framing is the reference's, the
/// model is not its fit.
pub fn write_secondary_segment_typed(
    path: &Path,
    kind: KeyKind,
    entries: &[(Key, u32)],
) -> Result<(), FormatError> {
    if kind == KeyKind::U64 {
        let entries: Vec<(u64, u32)> = entries
            .iter()
            .map(|(key, off)| match key {
                Key::U64(v) => (*v, *off),
                other => (other.as_f64() as u64, *off),
            })
            .collect();
        return write_secondary_segment(path, &entries);
    }
    let mut out = Vec::with_capacity(entries.len() * (kind.key_width() + 4) + 64);
    for (key, _) in entries {
        out.extend_from_slice(&key.to_le_bytes_in(kind));
    }
    for (_, off) in entries {
        out.extend_from_slice(&off.to_le_bytes());
    }
    out.extend_from_slice(&flat_tail(kind, entries));
    write_chunk(path, &out)
}

fn write_chunk(path: &Path, out: &[u8]) -> Result<(), FormatError> {
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A flat, exact `PGMWithExtraFunction<K>::Save` tail for a typed chunk: one
/// segment per `PGM_EPSILON` keys with `slope = 0`, so the model predicts the
/// first index of the segment and the reference's `[pos - 64, pos + 64]` window
/// still covers every key of it. Appended: the reference's sentinel
/// `{key: K::MAX, slope: 0, intercept: n}`.
///
/// `K::MAX` (`+inf`) is not `u64::MAX`: a float key may legitimately be
/// `f32::MAX`/`f64::MAX`, so only infinity sorts past every stored key.
fn flat_tail(kind: KeyKind, entries: &[(Key, u32)]) -> Vec<u8> {
    let n = entries.len();
    let mut segments: Vec<(Key, f64, u32)> = Vec::new();
    let mut start = 0usize;
    while start < n {
        segments.push((entries[start].0, 0.0, start as u32));
        start += PGM_EPSILON as usize;
    }
    segments.push((Key::F64(f64::INFINITY), 0.0, n as u32));

    let mut out = Vec::new();
    // `PGMWithExtraFunction::Save` order: `n`, `first_key`, the segment count,
    // the segments, then the level table.
    out.extend_from_slice(&(n as u32).to_le_bytes());
    let first = entries.first().map_or(0.0, |(key, _)| key.as_f64());
    match kind {
        KeyKind::F32 => out.extend_from_slice(&(first as f32).to_bits().to_le_bytes()),
        _ => out.extend_from_slice(&first.to_bits().to_le_bytes()),
    }
    out.extend_from_slice(&(segments.len() as u32).to_le_bytes());
    match kind {
        KeyKind::F32 => {
            for (key, slope, intercept) in &segments {
                out.extend_from_slice(&(key.as_f64() as f32).to_bits().to_le_bytes());
                out.extend_from_slice(&(*slope as f32).to_bits().to_le_bytes());
                out.extend_from_slice(&intercept.to_le_bytes());
            }
        }
        // `{f64 key; f64 slope; u32 intercept}` is 24 bytes: 4 of padding.
        _ => {
            for (key, slope, intercept) in &segments {
                out.extend_from_slice(&key.as_f64().to_bits().to_le_bytes());
                out.extend_from_slice(&slope.to_bits().to_le_bytes());
                out.extend_from_slice(&intercept.to_le_bytes());
                out.extend_from_slice(&[0u8; 4]);
            }
        }
    }
    out.extend_from_slice(&2u32.to_le_bytes());
    out.extend_from_slice(&0u64.to_le_bytes());
    out.extend_from_slice(&(segments.len() as u64).to_le_bytes());
    out
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
    map: Mmap,
    row_count: usize,
    kind: KeyKind,
    /// Only a `U64` chunk decodes its model; a typed chunk is validated and
    /// then searched by binary search (`validate_tail`).
    pgm: Option<Pgm>,
}

impl SecondarySegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        Self::open_with_kind(path, KeyKind::U64)
    }

    /// Opens a chunk whose key array holds `kind`-sized keys. The chunk has no
    /// header, so the kind has to come from the column's declared type.
    #[allow(unsafe_code)]
    pub fn open_with_kind(path: &Path, kind: KeyKind) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
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
        for n in 0..=map.len() / stride {
            let pgm_at = n * stride;
            if pgm_at + 4 > map.len() {
                break;
            }
            let pgm_n = u32::from_le_bytes(map[pgm_at..pgm_at + 4].try_into().unwrap()) as usize;
            if pgm_n != n {
                continue;
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
        // 1 segment for the 4 keys + the sentinel, 12 bytes each; two levels.
        assert_eq!(bytes.len(), tail_at + 4 + 4 + 4 + 2 * 12 + 4 + 2 * 8);

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
        // 1 segment + sentinel, 24 bytes each.
        assert_eq!(bytes.len(), tail_at + 4 + 8 + 4 + 2 * 24 + 4 + 2 * 8);

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

    /// An empty typed chunk round-trips (`n = 0`, one sentinel segment).
    #[test]
    fn empty_typed_chunk_roundtrip() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("empty_f32.idx");
        write_secondary_segment_typed(&path, KeyKind::F32, &[]).unwrap();
        let reader = SecondarySegmentReader::open_with_kind(&path, KeyKind::F32).unwrap();
        assert_eq!(reader.len(), 0);
        assert!(reader.lookup(Key::F32(1.0)).is_empty());
        assert!(reader.range(Key::F32(-1.0), Key::F32(1.0)).is_empty());
    }
}

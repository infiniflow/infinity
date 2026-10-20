//! Secondary-index segment files, following the reference's
//! `SecondaryIndexDataBase::SaveIndexInner` (`secondary_index_data_impl.cpp:224`)
//! and the PGM serialization in `secondary_index_pgm.cppm:129`.
//!
//! Layout of one chunk (`chunk_{n}.idx`, no header):
//!
//! ```text
//! keys     n * u64            ordered keys (the reference's OrderedKeyType)
//! offsets  n * u32            segment offsets, parallel to `keys`
//! pgm      u32 n
//!          u64 first_key
//!          u32 segment_count, segment_count * (u64 key, f32 slope, u32 intercept)
//!          u32 level_count,   level_count * u64
//! ```
//!
//! `Segment` is written from `#pragma pack(push, 1) struct Segment { K key;
//! Floating slope; uint32_t intercept; }`, i.e. 16 bytes with no padding, and
//! `levels_offsets` is a `std::vector<size_t>`, i.e. 8 bytes per entry.
//!
//! The model itself is a `PGMIndex<K, 64, 4, float>`: the bottom level is a
//! segmentation with `epsilon = 64`, every level above it is segmented with
//! `epsilon_recursive = 4` over the first key of the level below, and each
//! level ends with the reference's sentinel segment (`key = u64::MAX`,
//! `slope = 0`, `intercept = last_n`). `levels_offsets` holds the start of
//! each level in `segments[]` followed by `segments.len()`.

use memmap2::Mmap;
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

    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| FormatError::Io(format!("{}: {e}", parent.display())))?;
    }
    std::fs::write(path, out).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))
}

/// A chunk mapped read-only; lookups use the PGM then a binary search.
pub struct SecondarySegmentReader {
    map: Mmap,
    row_count: usize,
    pgm: Pgm,
}

impl SecondarySegmentReader {
    #[allow(unsafe_code)]
    pub fn open(path: &Path) -> Result<Self, FormatError> {
        let file =
            File::open(path).map_err(|e| FormatError::Io(format!("{}: {e}", path.display())))?;
        // SAFETY: read-only mapping of an immutable chunk file.
        let map = unsafe { Mmap::map(&file) }.map_err(|e| FormatError::Io(e.to_string()))?;
        if map.len() < 20 {
            return Err(FormatError::Truncated {
                need: 20,
                got: map.len(),
            });
        }
        // The chunk carries no header, so locate the PGM tail by trying the
        // plausible array lengths: `n * u64` keys and `n * u32` offsets
        // precede a tail whose header repeats `n` and ends exactly at EOF.
        let mut row_count = None;
        for n in 0..=map.len() / 12 {
            let pgm_at = n * 12;
            if pgm_at + 16 > map.len() {
                break;
            }
            let pgm_n = u32::from_le_bytes(map[pgm_at..pgm_at + 4].try_into().unwrap()) as usize;
            if pgm_n != n {
                continue;
            }
            if let Ok((_, _, _, level_count, offsets_at)) = Pgm::header(&map, pgm_at) {
                if offsets_at + level_count * 8 == map.len() {
                    row_count = Some(n);
                    break;
                }
            }
        }
        let Some(row_count) = row_count else {
            return Err(FormatError::Truncated {
                need: map.len(),
                got: map.len(),
            });
        };
        let pgm = Pgm::decode(&map, row_count * 12)?;
        Ok(Self {
            map,
            row_count,
            pgm,
        })
    }

    pub fn len(&self) -> usize {
        self.row_count
    }

    pub fn is_empty(&self) -> bool {
        self.row_count == 0
    }

    fn key(&self, i: usize) -> u64 {
        let at = i * 8;
        u64::from_le_bytes(self.map[at..at + 8].try_into().unwrap())
    }

    fn offset(&self, i: usize) -> u32 {
        let at = self.row_count * 8 + i * 4;
        u32::from_le_bytes(self.map[at..at + 4].try_into().unwrap())
    }

    /// All segment offsets whose key equals `key`.
    pub fn lookup(&self, key: u64) -> Vec<u32> {
        if self.row_count == 0 || self.pgm.segments.is_empty() {
            return Vec::new();
        }
        // The reference searches [lower_bound_, upper_bound_) from the PGM and
        // then walks to the exact range; the model guarantees that a present
        // key lies inside that window.
        let (_, lo, _hi) = self.pgm.search(key);
        let mut i = self.lower_bound(lo.min(self.row_count), self.row_count, key);
        let mut out = Vec::new();
        while i < self.row_count && self.key(i) == key {
            out.push(self.offset(i));
            i += 1;
        }
        // A model miss (key outside the predicted range) falls back to a full
        // binary search over the mapped keys.
        if out.is_empty() {
            let mut i = self.lower_bound(0, self.row_count, key);
            while i < self.row_count && self.key(i) == key {
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
    /// falls between two keys. `start > end` is empty.
    pub fn range(&self, start: u64, end: u64) -> Vec<u32> {
        if self.row_count == 0 || start > end {
            return Vec::new();
        }
        let begin = self.lower_bound(0, self.row_count, start);
        // `lower_bound(end + 1)` is the first key past the range; `end` may be
        // `u64::MAX`, where there is no such key.
        let stop = match end.checked_add(1) {
            Some(after) => self.lower_bound(begin, self.row_count, after),
            None => self.row_count,
        };
        (begin..stop).map(|i| self.offset(i)).collect()
    }

    /// `partition_point` over the mapped keys: first index in `[lo, hi)` whose
    /// key is `>= key`.
    fn lower_bound(&self, lo: usize, hi: usize, key: u64) -> usize {
        let (mut lo, mut hi) = (lo, hi);
        while lo < hi {
            let mid = (lo + hi) / 2;
            if self.key(mid) < key {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        lo
    }

    /// First key (useful for range scans).
    pub fn first_key(&self) -> u64 {
        self.pgm.first_key
    }
}

#[cfg(test)]
mod tests {
    use super::*;

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
            let hits = reader.lookup(key);
            assert_eq!(hits, vec![entries[i].1], "key {key}");
        }
        assert!(reader.lookup(1).is_empty());
        assert!(reader.lookup(u64::MAX).is_empty());
    }

    #[test]
    fn duplicate_keys_return_every_offset() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("chunk_1.idx");
        let entries = vec![(1u64, 10u32), (1, 11), (2, 12), (3, 13)];
        write_secondary_segment(&path, &entries).unwrap();
        let reader = SecondarySegmentReader::open(&path).unwrap();
        assert_eq!(reader.lookup(1), vec![10, 11]);
        assert_eq!(reader.lookup(3), vec![13]);
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
        assert_eq!(reader.first_key(), 2);
        assert_eq!(reader.lookup(2), vec![0]);
        assert_eq!(reader.lookup(250_002), vec![500]);
        assert_eq!(reader.lookup(998_003), vec![999]);
        for (i, key) in keys.iter().enumerate() {
            assert_eq!(reader.lookup(*key), vec![i as u32], "key {key}");
        }
        assert!(reader.lookup(4).is_empty());
        assert!(reader.lookup(0).is_empty());
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
        assert!(reader.lookup(7).is_empty());
        assert!(reader.range(0, u64::MAX).is_empty());
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

        assert_eq!(reader.range(2, 5), vec![11, 12, 13]);
        assert_eq!(reader.range(2, 2), vec![11]);
        assert_eq!(reader.range(1, 8), vec![10, 11, 12, 13, 14]);
        // Bounds that fall between two keys still cover the keys between them.
        assert_eq!(reader.range(3, 4), vec![12]);
        assert_eq!(reader.range(4, 7), vec![13]);
        // Both ends outside the key domain.
        assert!(reader.range(0, 0).is_empty());
        assert!(reader.range(9, 100).is_empty());
        // An unbounded upper end.
        assert_eq!(reader.range(5, u64::MAX), vec![13, 14]);
        // An inverted range is empty, not the whole chunk.
        assert!(reader.range(5, 2).is_empty());
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
        assert_eq!(reader.range(2, 2), vec![11, 12, 13]);
        assert_eq!(reader.range(2, 3), vec![11, 12, 13, 14]);
        assert_eq!(reader.range(1, 3), vec![10, 11, 12, 13, 14]);
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
        assert_eq!(reader.range(11, 27), vec![3, 4, 5]);
        assert_eq!(reader.range(2, 2), vec![0]);
        // Keys 3 (i = 1) and 6 (i = 2) both fall inside.
        assert_eq!(reader.range(3, 6), vec![1, 2]);
        // Everything, and nothing.
        assert_eq!(reader.range(0, u64::MAX).len(), 1000);
        assert!(reader.range(999_000, u64::MAX).is_empty());
    }
}

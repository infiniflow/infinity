// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! CRC-32/ISO-HDLC (the `crc32fast` API, without SIMD).
//!
//! This crate stands in for `crc32fast` through `[patch.crates-io]`. The real
//! one compiles an AVX-512 path behind `#[target_feature(enable = "avx512f")]`,
//! which lands in the binary whatever `target-cpu` says: measured, 52 `zmm`
//! instructions when building for `x86-64-v3` and 26 for `x86-64-v2`. Some CI
//! runners advertise `avx512f` in CPUID and reject the instructions, so the
//! engine died with SIGILL before it could log a line.
//!
//! It reaches us through `parquet -> flate2` (Parquet import) as well as
//! directly, so the patch is the only way to keep AVX2 in the rest of the build
//! without carrying that path. Only the surface those two use is provided:
//! `hash` and `Hasher` (`new`, `update`, `finalize`, `reset`, `combine`), plus
//! the trait impls `flate2` needs (`Clone`, `Default`).
//!
//! The algorithm is slicing-by-8, the same as `crates/storage`'s `crc` module:
//! eight tables, eight bytes per step, no SIMD. Its output is byte-for-byte the
//! standard CRC-32/ISO-HDLC, which is what the check value below pins.

/// One table per byte position, so eight bytes are consumed per iteration.
const TABLES: [[u32; 256]; 8] = build_tables();

/// Builds the eight tables at compile time.
const fn build_tables() -> [[u32; 256]; 8] {
    let mut tables = [[0u32; 256]; 8];
    let mut i = 0;
    while i < 256 {
        let mut crc = i as u32;
        let mut bit = 0;
        while bit < 8 {
            crc = if crc & 1 != 0 {
                (crc >> 1) ^ 0xEDB8_8320
            } else {
                crc >> 1
            };
            bit += 1;
        }
        tables[0][i] = crc;
        i += 1;
    }
    let mut t = 1;
    while t < 8 {
        let mut i = 0;
        while i < 256 {
            let prev = tables[t - 1][i];
            tables[t][i] = (prev >> 8) ^ tables[0][(prev & 0xFF) as usize];
            i += 1;
        }
        t += 1;
    }
    tables
}

/// The slicing-by-8 core.
#[inline]
fn update(mut state: u32, bytes: &[u8]) -> u32 {
    let mut chunks = bytes.chunks_exact(8);
    for chunk in &mut chunks {
        let lo = state ^ u32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
        let hi = u32::from_le_bytes([chunk[4], chunk[5], chunk[6], chunk[7]]);
        state = TABLES[7][(lo & 0xFF) as usize]
            ^ TABLES[6][((lo >> 8) & 0xFF) as usize]
            ^ TABLES[5][((lo >> 16) & 0xFF) as usize]
            ^ TABLES[4][((lo >> 24) & 0xFF) as usize]
            ^ TABLES[3][(hi & 0xFF) as usize]
            ^ TABLES[2][((hi >> 8) & 0xFF) as usize]
            ^ TABLES[1][((hi >> 16) & 0xFF) as usize]
            ^ TABLES[0][((hi >> 24) & 0xFF) as usize];
    }
    for &byte in chunks.remainder() {
        state = TABLES[0][((state ^ u32::from(byte)) & 0xFF) as usize] ^ (state >> 8);
    }
    state
}

/// CRC-32/ISO-HDLC over `bytes`.
#[inline]
pub fn hash(bytes: &[u8]) -> u32 {
    let mut hasher = Hasher::new();
    hasher.update(bytes);
    hasher.finalize()
}

/// Incremental CRC-32/ISO-HDLC, the same interface `crc32fast::Hasher` has.
#[derive(Clone, Debug)]
pub struct Hasher {
    state: u32,
    /// Bytes fed so far. `combine` needs it: the CRC of a longer message is the
    /// CRC of its prefix advanced by the suffix's length.
    amount: u64,
}

impl Default for Hasher {
    fn default() -> Self {
        Self::new()
    }
}

impl Hasher {
    pub fn new() -> Self {
        Self {
            state: 0xFFFF_FFFF,
            amount: 0,
        }
    }

    pub fn update(&mut self, bytes: &[u8]) {
        self.state = update(self.state, bytes);
        self.amount += bytes.len() as u64;
    }

    pub fn finalize(self) -> u32 {
        self.state ^ 0xFFFF_FFFF
    }

    /// Starts a new checksum without dropping the allocation.
    pub fn reset(&mut self) {
        self.state = 0xFFFF_FFFF;
        self.amount = 0;
    }

    /// Appends `other`'s message to this one, the same contract as
    /// `crc32fast::Hasher::combine`.
    pub fn combine(&mut self, other: &Self) {
        // `combine` takes finished CRCs, while `state` is the pre-final-xor
        // register. Convert both ways so the public contract holds.
        let mine = self.state ^ 0xFFFF_FFFF;
        let theirs = other.clone().finalize();
        self.state = combine(mine, theirs, other.amount) ^ 0xFFFF_FFFF;
        self.amount += other.amount;
    }

}

/// `crc32_combine` from zlib: the CRC of `crc1`'s message followed by `len2`
/// bytes whose own CRC is `crc2`.
///
/// Advancing a CRC by `n` zero bytes is a linear map over GF(2), so it can be
/// applied with repeated squaring instead of touching `n` bytes. This is the
/// same routine `crc32fast` uses, which is why the result matches bit for bit.
fn combine(crc1: u32, crc2: u32, len2: u64) -> u32 {
    if len2 == 0 {
        return crc1;
    }
    let mut odd = [0u32; 32];
    odd[0] = 0xEDB8_8320;
    let mut row = 1u32;
    for slot in odd.iter_mut().skip(1) {
        *slot = row;
        row <<= 1;
    }
    let mut even = square(&odd);
    odd = square(&even);

    let mut crc1 = crc1;
    let mut len = len2;
    loop {
        even = square(&odd);
        if len & 1 != 0 {
            crc1 = times(&even, crc1);
        }
        len >>= 1;
        if len == 0 {
            break;
        }
        odd = square(&even);
        if len & 1 != 0 {
            crc1 = times(&odd, crc1);
        }
        len >>= 1;
        if len == 0 {
            break;
        }
    }
    crc1 ^ crc2
}

/// One GF(2) matrix square, from zlib's `gf2_matrix_square`.
fn square(matrix: &[u32; 32]) -> [u32; 32] {
    let mut out = [0u32; 32];
    for (n, slot) in out.iter_mut().enumerate() {
        *slot = times(matrix, matrix[n]);
    }
    out
}

/// One GF(2) matrix-vector product, from zlib's `gf2_matrix_times`.
fn times(matrix: &[u32; 32], mut vector: u32) -> u32 {
    let mut sum = 0u32;
    let mut n = 0;
    while vector != 0 {
        if vector & 1 != 0 {
            sum ^= matrix[n];
        }
        vector >>= 1;
        n += 1;
    }
    sum
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn standard_check_value() {
        // The canonical CRC-32/ISO-HDLC check value for "123456789".
        assert_eq!(hash(b"123456789"), 0xCBF4_3926);
        assert_eq!(hash(b""), 0x0000_0000);
    }

    #[test]
    fn incremental_matches_one_shot() {
        let data: Vec<u8> = (0..=255u8).cycle().take(1000).collect();
        let expected = hash(&data);
        for split in [0, 1, 4, 7, 8, 9, 100, 999, 1000] {
            let mut hasher = Hasher::new();
            hasher.update(&data[..split]);
            hasher.update(&data[split..]);
            assert_eq!(hasher.finalize(), expected, "split at {split}");
        }
    }

    /// `combine` must equal hashing both messages back to back, which is the
    /// property `flate2` relies on when it concatenates deflate streams.
    #[test]
    fn combine_matches_concatenation() {
        let first = b"the quick brown fox";
        let second = b" jumps over the lazy dog";
        let mut a = Hasher::new();
        a.update(first);
        let mut b = Hasher::new();
        b.update(second);
        a.combine(&b);

        let mut expected = Hasher::new();
        expected.update(first);
        expected.update(second);
        assert_eq!(a.finalize(), expected.finalize());
    }

    /// A zero-length `other` leaves the checksum alone.
    #[test]
    fn combine_with_empty_other_is_identity() {
        let mut a = Hasher::new();
        a.update(b"abc");
        let before = a.clone().finalize();
        a.combine(&Hasher::new());
        assert_eq!(a.finalize(), before);
    }

    #[test]
    fn reset_starts_over() {
        let mut hasher = Hasher::new();
        hasher.update(b"garbage");
        hasher.reset();
        hasher.update(b"123456789");
        assert_eq!(hasher.finalize(), 0xCBF4_3926);
    }
}

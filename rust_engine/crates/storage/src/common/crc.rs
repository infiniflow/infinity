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

//! CRC-32/IEEE — matches Infinity's `CRC32IEEE` (`src/common/utility/crc.cppm:49`,
//! poly `0xEDB88320`, init/`final_xor` = `0xFFFFFFFF`).
//!
//! This used to wrap `crc32fast`. That crate compiles an AVX-512 path
//! (`calculate_avx512`) behind `#[target_feature(enable = "avx512f")]`, which is
//! emitted into the binary whatever `target-cpu` says - measured, 52 `zmm`
//! instructions in a build for `x86-64-v3` and 26 even for `x86-64-v2`. Some CI
//! runners advertise `avx512f` in CPUID and then reject the instructions, so the
//! engine died with SIGILL before it could log a line.
//!
//! The CRC here is table-driven: eight tables, eight bytes per step
//! (`slicing-by-8`), no SIMD. That runs at a few GB/s, far more than the WAL
//! checksum and block verification need - neither is on the scan's hot path -
//! and it compiles to the same code on every CPU.

/// One table per byte position, so eight bytes are consumed per iteration.
///
/// Table 0 is the classic reflected CRC-32 table for `0xEDB88320`; each further
/// table is the previous one advanced by a byte, which is what lets the loop XOR
/// eight input bytes before touching the state.
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

/// Incremental CRC-32/IEEE, mirroring the `crc32fast::Hasher` interface the WAL
/// reader used: `new`, `update`, `finalize`.
#[derive(Clone)]
pub struct Crc32 {
    state: u32,
}

impl Default for Crc32 {
    fn default() -> Self {
        Self::new()
    }
}

impl Crc32 {
    pub fn new() -> Self {
        Self { state: 0xFFFF_FFFF }
    }

    pub fn update(&mut self, bytes: &[u8]) {
        self.state = update(self.state, bytes);
    }

    pub fn finalize(self) -> u32 {
        self.state ^ 0xFFFF_FFFF
    }
}

/// CRC-32/IEEE over `bytes`.
#[inline]
pub fn crc32_ieee(bytes: &[u8]) -> u32 {
    let mut crc = Crc32::new();
    crc.update(bytes);
    crc.finalize()
}

/// The slicing-by-8 core: XOR eight bytes into the state, then fold in the eight
/// table lookups.
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn standard_check_value() {
        // The canonical CRC-32/ISO-HDLC check value for "123456789".
        assert_eq!(crc32_ieee(b"123456789"), 0xCBF4_3926);
        assert_eq!(crc32_ieee(b""), 0x0000_0000);
    }

    #[test]
    fn differs_on_one_bit() {
        assert_ne!(crc32_ieee(b"abcdef"), crc32_ieee(b"abcdeg"));
    }

    /// The incremental form must equal the one-shot form for every split point,
    /// because the WAL reader feeds the header, a zeroed checksum field and the
    /// body as three separate updates.
    #[test]
    fn incremental_matches_one_shot() {
        let data: Vec<u8> = (0..=255u8).cycle().take(1000).collect();
        let expected = crc32_ieee(&data);
        for split in [0, 1, 4, 7, 8, 9, 100, 999, 1000] {
            let mut hasher = Crc32::new();
            hasher.update(&data[..split]);
            hasher.update(&data[split..]);
            assert_eq!(hasher.finalize(), expected, "split at {split}");
        }
        // Three parts, like the WAL reader's header / zeroed checksum / body.
        let mut hasher = Crc32::new();
        hasher.update(&data[..4]);
        hasher.update(&[0u8; 4]);
        hasher.update(&data[8..]);
        let mut one_shot = data[..4].to_vec();
        one_shot.extend_from_slice(&[0u8; 4]);
        one_shot.extend_from_slice(&data[8..]);
        assert_eq!(hasher.finalize(), crc32_ieee(&one_shot));
    }

    /// Every length from 0 to 300, so each `chunks_exact` remainder is covered,
    /// checked against the bit-at-a-time definition of the same polynomial.
    #[test]
    fn every_length_and_remainder() {
        let data: Vec<u8> = (0..300usize).map(|i| (i as u8).wrapping_mul(7)).collect();
        for len in 0..=300 {
            let slice = &data[..len];
            let mut slow = 0xFFFF_FFFFu32;
            for &byte in slice {
                slow ^= u32::from(byte);
                for _ in 0..8 {
                    slow = if slow & 1 != 0 {
                        (slow >> 1) ^ 0xEDB8_8320
                    } else {
                        slow >> 1
                    };
                }
            }
            assert_eq!(crc32_ieee(slice), slow ^ 0xFFFF_FFFF, "len {len}");
        }
    }
}

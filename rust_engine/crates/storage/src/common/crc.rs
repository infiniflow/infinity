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
//! poly `0xEDB88320`, init/`final_xor` = `0xFFFFFFFF`). `crc32fast` implements the
//! identical CRC-32/ISO-HDLC, so we wrap it to keep a single audited implementation.

/// CRC-32/IEEE over `bytes`.
#[inline]
pub fn crc32_ieee(bytes: &[u8]) -> u32 {
    crc32fast::hash(bytes)
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
}

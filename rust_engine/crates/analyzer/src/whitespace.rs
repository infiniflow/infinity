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

use super::*;

/// C++: `WhitespaceAnalyzer` (`whitespace_analyzer_impl.cpp`).
///
/// The `keyword` and `whitespace` families both construct this analyzer. With
/// no delimiters it splits on `std::isspace()`; otherwise it splits **only** on
/// the listed bytes (it does not additionally split on whitespace).
#[derive(Debug, Clone)]
pub struct WhitespaceAnalyzer {
    pub(crate) delimiters: Vec<u8>,
}

impl WhitespaceAnalyzer {
    /// C++: the constructor sorts and de-duplicates the delimiter bytes.
    pub fn new(delimiters: &str) -> Self {
        let mut delimiters = delimiters.as_bytes().to_vec();
        delimiters.sort_unstable();
        delimiters.dedup();
        Self { delimiters }
    }

    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut out = Vec::new();
        let mut offset = 0u32;
        // C++ emits raw byte ranges. A non-ASCII delimiter byte can split a
        // UTF-8 sequence, which a Rust `str` cannot represent; lossily decode
        // so the analyzer never panics on such a name.
        let mut push = |text: &[u8]| {
            out.push(Term {
                text: String::from_utf8_lossy(text).into_owned(),
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: false,
                payload: 0,
            });
            offset += 1;
        };
        if self.delimiters.is_empty() {
            for token in input.split_whitespace() {
                push(token.as_bytes());
            }
            return out;
        }
        let bytes = input.as_bytes();
        let mut start = 0usize;
        while start < bytes.len() {
            let found = bytes[start..]
                .iter()
                .position(|b| self.delimiters.binary_search(b).is_ok())
                .map(|position| start + position);
            match found {
                None => {
                    push(&bytes[start..]);
                    break;
                }
                Some(found) => {
                    if found > start {
                        push(&bytes[start..found]);
                    }
                    start = found + 1;
                }
            }
        }
        out
    }
}

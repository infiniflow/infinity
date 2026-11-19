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

//! On-disk posting formats, laid out like the reference src/storage/invertedindex/format/.
//!
//! The u32/u16 block codecs, the term metadata, the FST trailer helpers and the
//! file-name conventions each live in their own file now; this module only
//! stitches them together.

use std::simd::u32x4;

use crate::format::FormatError;

pub mod codec;
pub mod doc_list;
pub mod file_names;
pub mod fst;
pub mod position_list;
pub mod posting_slice;
pub mod skiplist;
pub mod term_meta;

pub use codec::*;
pub use doc_list::*;
pub use file_names::*;
pub use fst::*;
pub use position_list::*;
pub use posting_slice::*;
pub use skiplist::*;
pub use term_meta::*;

#[cfg(test)]
mod tests;

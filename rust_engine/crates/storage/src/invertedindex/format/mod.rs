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

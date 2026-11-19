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

//! The inverted index, laid out like the reference's
//! src/storage/invertedindex/: the on-disk formats under format/ and the
//! sealed segment reader next to it.
//!
//! Only the parts the engine reaches are here; the reference's fst/, ring and
//! in-memory indexer live elsewhere (see docs/STRUCTURE.md).

pub mod chunk_manifest;
pub mod common;
pub mod disk_segment_reader;
pub mod format;
pub mod fst;
pub mod index_merger;

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

//! Full-text retrieval, laid out like the reference's
//! src/storage/invertedindex/search/.
//!
//! The iterators and the ranker live here; the term streams they drive are
//! the engine's (lib.rs), which is the one deviation from the reference's
//! placement - see docs/STRUCTURE.md.

pub mod search;

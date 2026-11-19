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

//! Text analysis, laid out like the reference's `src/common/analyzer/`.
//!
//! A standalone crate so anything that has to tokenize the way the engine's
//! full-text index does can depend on it without storage, the catalog or the
//! executor: the executor links it, and an FFI cdylib or an SDK can too.
//!
//! Every family is here - `rag-<lang>[-fine]`, `standard-<language>`,
//! `chinese`/`traditional`/`ik`, `japanese`/`korean`, `ngram`, `whitespace`,
//! `sparsegram`, `keyword` and `rank_features` - and the dictionaries they need
//! come from the resource root handed to each `load` (the reference's
//! `/usr/share/infinity/resource`).

#![forbid(unsafe_code)]

use std::collections::{HashMap, HashSet};
use std::path::Path;
use std::sync::{Arc, Mutex, OnceLock};

pub mod chinese;
pub mod core;
pub mod darts_build;
pub mod ik;
pub mod mecab;
pub mod ngram;
pub mod opencc;
pub mod porter;
pub mod rag;
pub mod rank_features;
pub mod sparsegram;
pub mod standard;
pub mod tokenizer;
pub mod traditional;
pub mod whitespace;

pub use chinese::*;
pub use core::*;
pub use ik::*;
pub use mecab::*;
pub use ngram::*;
pub use opencc::*;
pub use rag::*;
pub use rank_features::*;
pub use sparsegram::*;
pub use standard::*;
pub use tokenizer::*;
pub use traditional::*;
pub use whitespace::*;

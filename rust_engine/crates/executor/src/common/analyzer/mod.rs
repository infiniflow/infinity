//! Text analysis, laid out like the reference's src/common/analyzer/.

pub(crate) mod ngram;
pub(crate) mod rank_features;
pub(crate) mod sparsegram;
pub(crate) mod standard;
pub(crate) mod tokenizer;
pub(crate) mod whitespace;
use std::collections::{HashMap, HashSet};
use std::path::Path;
use std::sync::{Arc, Mutex, OnceLock};
pub(crate) mod chinese;
pub(crate) mod core;
pub(crate) mod ik;
pub(crate) mod mecab;
pub(crate) mod opencc;
pub(crate) mod rag;
pub(crate) mod traditional;

pub(crate) use chinese::*;
pub(crate) use core::*;
pub(crate) use ik::*;
pub(crate) use mecab::*;
pub(crate) use ngram::*;
pub(crate) use opencc::*;
pub(crate) use rag::*;
pub(crate) use rank_features::*;
pub(crate) use sparsegram::*;
pub(crate) use standard::*;
pub(crate) use tokenizer::*;
pub(crate) use traditional::*;
pub(crate) use whitespace::*;

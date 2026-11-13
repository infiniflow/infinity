//! Full-text retrieval, laid out like the reference's
//! src/storage/invertedindex/search/.
//!
//! The iterators and the ranker live here; the term streams they drive are
//! the engine's (lib.rs), which is the one deviation from the reference's
//! placement - see docs/STRUCTURE.md.

pub mod search;

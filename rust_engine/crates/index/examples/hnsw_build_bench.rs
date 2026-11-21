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

//! Times HNSW graph construction alone (the part `bench_index_types.py`
//! measures as `INDEX hnsw build`), serially and bucket-parallel.
//!
//! ```text
//! hnsw_build_bench <sift|gist|<file.fvecs>> [rows] [threads] [plain|lvq]
//! ```
//!
//! **A real corpus is required.** A build time is only meaningful for the
//! corpus it was measured on, and a generated dataset has a different
//! neighbour structure from SIFT or GIST - its seconds say nothing about the
//! reference's numbers, so there is deliberately no built-in generator here.
//!
//! `sift` and `gist` resolve to the checked-in benchmark corpora
//! (`test/data/benchmark/{sift,gist}_1m/*_base.fvecs`), located from the
//! manifest directory so the binary works from any cwd. Their dimensions come
//! from the dataset, not from a flag, so a typo cannot reshape every record.
//!
//! `rows` 0 means the whole file. `encode` is `plain` (default, the setting the
//! reference's build number was measured with) or `lvq`.
//!
//! The bucket build fans out on **a runtime the caller provides** - the engine
//! installs its own scheduler runtime, and a standalone binary has to bring one
//! (the index crate never builds a pool of its own). Without it the buckets
//! would run one after another on this thread.

use std::time::Instant;

use index::vector::{HnswEncoding, HnswIndex, Metric};

/// Reads up to `limit` records of an `fvecs` file (`i32 dim` then `dim` f32).
///
/// A record whose dimension is not `dims` is skipped, so a file that does not
/// match the declared shape cannot silently reshape the graph.
fn read_fvecs(path: &std::path::Path, limit: usize, dims: usize) -> Vec<Vec<f32>> {
    let bytes = std::fs::read(path).unwrap_or_else(|e| panic!("read {}: {e}", path.display()));
    let record = 4 + dims * 4;
    let count = bytes.len() / record;
    let take = if limit == 0 { count } else { limit.min(count) };
    let mut out = Vec::with_capacity(take);
    for i in 0..take {
        let at = i * record;
        let d = i32::from_le_bytes(bytes[at..at + 4].try_into().unwrap()) as usize;
        if d != dims {
            continue;
        }
        let mut v = Vec::with_capacity(dims);
        for j in 0..dims {
            let p = at + 4 + j * 4;
            v.push(f32::from_le_bytes(bytes[p..p + 4].try_into().unwrap()));
        }
        out.push(v);
    }
    out
}

/// A checked-in corpus: its file and the dimension it actually holds.
///
/// Resolved from the manifest directory (`rust_engine/crates/index`), walking up
/// until the file exists, so the binary runs from any cwd.
fn corpus(name: &str) -> Option<(std::path::PathBuf, usize)> {
    let (relative, dims) = match name {
        "sift" => ("test/data/benchmark/sift_1m/sift_base.fvecs", 128usize),
        "gist" => ("test/data/benchmark/gist_1m/gist_base.fvecs", 960),
        _ => return None,
    };
    let mut at = std::path::PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    loop {
        let candidate = at.join(relative);
        if candidate.is_file() {
            return Some((candidate, dims));
        }
        if !at.pop() {
            return None;
        }
    }
}

#[tokio::main]
async fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let Some(source) = args.first().cloned() else {
        eprintln!(
            "usage: hnsw_build_bench <sift|gist|<file.fvecs>> [rows] [threads] [plain|lvq]\n\
             \n\
             A real corpus is required: build time is only meaningful for the\n\
             corpus it was measured on, and a generated dataset has a different\n\
             neighbour structure, so it cannot be compared with the reference."
        );
        std::process::exit(2);
    };
    let rows: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
    let threads: usize = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(8);
    let encoding = args.get(3).map(String::as_str).unwrap_or("plain");
    if !matches!(encoding, "plain" | "lvq") {
        eprintln!("encode must be `plain` or `lvq`, got `{encoding}`");
        std::process::exit(2);
    }

    // `sift`/`gist` carry their own dimension; a caller-supplied path needs one,
    // and it comes from the file's own first record rather than from a flag.
    let (path, dims, data) = match corpus(&source) {
        Some((path, dims)) => {
            let data = read_fvecs(&path, rows, dims);
            (path, dims, data)
        }
        None => {
            let path = std::path::PathBuf::from(&source);
            let bytes = std::fs::read(&path).unwrap_or_else(|e| {
                eprintln!("cannot read `{source}`: {e}");
                std::process::exit(2);
            });
            if bytes.len() < 4 {
                eprintln!("`{source}` is too short to hold an fvecs record");
                std::process::exit(2);
            }
            let dims = i32::from_le_bytes(bytes[..4].try_into().unwrap()) as usize;
            let data = read_fvecs(&path, rows, dims);
            (path, dims, data)
        }
    };
    if data.is_empty() {
        eprintln!("{} holds no records of dimension {dims}", path.display());
        std::process::exit(2);
    }

    let mut index =
        HnswIndex::with_encoding(dims, 16, 200, 64, Metric::L2, HnswEncoding::parse(encoding));
    let t = Instant::now();
    // The build runs inside the runtime, so its buckets spawn onto it and this
    // thread parks while they run - the same shape a statement has, where the
    // build sits on the blocking pool and the buckets join the scheduler.
    let added = index.add_batch(&data, threads).await;
    let build = t.elapsed();
    println!(
        "corpus {} rows {added} dims {dims} threads {threads} encode {encoding} \
         built in {:.4}s  levels {}",
        path.display(),
        build.as_secs_f64(),
        index.max_level() + 1
    );

    // Self-recall: every corpus vector's nearest neighbour is itself. A graph
    // that lost or mis-linked vertices shows up here, so a fast build cannot be
    // a broken one. The reference's number for this is 1.0.
    let t = Instant::now();
    let mut hits = 0usize;
    let probes = 200.min(data.len());
    for (i, q) in data.iter().take(probes).enumerate() {
        let got = index.search_with_ef(q, 10, 200);
        if got.first().is_some_and(|(id, _)| *id == i) {
            hits += 1;
        }
    }
    println!(
        "{probes} queries ef=200 in {:.4}s  top1 self-recall {:.3}",
        t.elapsed().as_secs_f64(),
        hits as f64 / probes as f64
    );
}

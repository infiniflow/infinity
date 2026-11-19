//! Scale harness for the two build paths (`SPANN_DESIGN.md` §0.0.1, R1/R2).
//!
//! One mode per process, so the peak RSS reported belongs to that build alone:
//!
//! ```text
//! cargo run --release -p index --example spann_scale -- bulk 200000 1536
//! cargo run --release -p index --example spann_scale -- incr 200000 1536
//! ```
//!
//! `bulk` is `SpannIndex::build_from_source` (the tree the engine publishes
//! today, §19); `incr` is `IncIndex::bootstrap` + one `assign` per row +
//! `into_spann` (the §0.0.3 shape). Peak RSS is sampled out of `ps` because the
//! crate has no libc dependency, and the source itself is measured first so the
//! printed "index" figure is the build's own footprint.

#![allow(clippy::too_many_arguments)]

use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::Arc;
use std::time::{Duration, Instant};

use index::spann::incr::IncIndex;
use index::spann::source::{row_source, Memory, VectorSource};
use index::spann::{Options, SpannIndex};

fn main() {
    let mut args = std::env::args().skip(1);
    let mode = args.next().unwrap_or_else(|| "both".to_string());
    let rows: usize = args.next().and_then(|a| a.parse().ok()).unwrap_or(100_000);
    let dim: usize = args.next().and_then(|a| a.parse().ok()).unwrap_or(128);
    let beam: usize = args.next().and_then(|a| a.parse().ok()).unwrap_or(32);

    let sampler = Sampler::start();
    let built: Memory = make_rows(rows, dim);
    let baseline_kb = Sampler::rss_kb();

    let opts = Options::default();
    match mode.as_str() {
        "bulk" | "both" => run_bulk(&built, &opts, rows, dim, baseline_kb),
        _ => {}
    }
    match mode.as_str() {
        "incr" | "both" => run_incr(&built, &opts, rows, dim, baseline_kb, beam),
        _ => {}
    }
    if mode.as_str() == "disk" {
        run_disk(&built, &opts, rows, dim, beam);
    }
    let peak = sampler.stop();
    println!(
        "peak rss {} MB (source {} MB, budget {} rows x {} dims)",
        peak / 1024,
        baseline_kb / 1024,
        rows,
        dim
    );
}

fn run_bulk(src: &Memory, opts: &Options, rows: usize, dim: usize, baseline_kb: usize) {
    let started = Instant::now();
    let index = SpannIndex::build_from_source(src, opts.clone()).expect("bulk build");
    let secs = started.elapsed().as_secs_f64();
    report("bulk", &index, src, rows, dim, secs, baseline_kb, 32);
}

fn run_incr(
    src: &Memory,
    opts: &Options,
    rows: usize,
    dim: usize,
    baseline_kb: usize,
    beam: usize,
) {
    let started = Instant::now();
    let mut tree = IncIndex::bootstrap(src, dim, opts.clone()).expect("bootstrap");
    let booted = started.elapsed().as_secs_f64();
    let boot_parts = tree.parts().len();
    let lookup = row_source(src);
    let mut marks = Vec::new();
    for row in 0..src.len() {
        tree.insert(src.id(row), src.vector(row)).expect("insert");
        if tree.pending_fixups() >= 64 {
            tree.run_fixups(Some(&lookup));
        }
        if src.len() >= 4 && row % (src.len() / 4).max(1) == 0 {
            marks.push((row, tree.splits(), tree.merges()));
        }
    }
    tree.run_fixups(Some(&lookup));
    let secs = started.elapsed().as_secs_f64();
    let (splits, merges) = (tree.splits(), tree.merges());
    let index = tree.into_spann().expect("publish");
    println!(
        "bootstrap {} partitions in {:.2}s; splits {} merges {}",
        boot_parts, booted, splits, merges
    );
    println!("  (row, splits, merges) every quarter: {marks:?}");
    report("incr", &index, src, rows, dim, secs, baseline_kb, beam);
}

/// The path production uses: publish to a chunk, reopen it mapped, query it.
/// The in-memory tree is dropped before the measurement so the reader's RSS and
/// latency are its own.
fn run_disk(src: &Memory, opts: &Options, rows: usize, dim: usize, beam: usize) {
    let started = Instant::now();
    let mut tree = IncIndex::bootstrap(src, dim, opts.clone()).expect("bootstrap");
    let lookup = row_source(src);
    for row in 0..src.len() {
        tree.insert(src.id(row), src.vector(row)).expect("insert");
        if tree.pending_fixups() >= 64 {
            tree.run_fixups(Some(&lookup));
        }
    }
    tree.run_fixups(Some(&lookup));
    let built = started.elapsed().as_secs_f64();
    let index = tree.into_spann().expect("publish");

    let probes: Vec<usize> = (0..100).map(|i| i * (src.len() / 100).max(1)).collect();
    let before = Instant::now();
    let mut hits = 0usize;
    for row in &probes {
        let got = index.search(src.vector(*row), 1, Some(beam));
        if got.first().map(|(id, _)| *id) == Some(src.id(*row)) {
            hits += 1;
        }
    }
    let heap_us = before.elapsed().as_secs_f64() * 1e6 / probes.len() as f64;

    let path = std::env::temp_dir().join(format!("spann_scale_{rows}_{dim}.spann"));
    let bytes = storage::knn_index::spann::write_to(&index, &path, 0).expect("write chunk");
    drop(index);

    let file = storage::knn_index::spann::FileIndex::open(&path, opts).expect("open chunk");
    let before = Instant::now();
    let mut mapped_hits = 0usize;
    for row in &probes {
        let got = file.search(src.vector(*row), 1, Some(beam));
        if got.first().map(|(id, _)| *id) == Some(src.id(*row)) {
            mapped_hits += 1;
        }
    }
    let mapped_us = before.elapsed().as_secs_f64() * 1e6 / probes.len() as f64;

    println!(
        "disk: built in {built:.1}s, chunk {:.0} MB; recall@1 heap {:.3} ({heap_us:.0} us/query) \
         mapped {:.3} ({mapped_us:.0} us/query)",
        bytes as f64 / (1 << 20) as f64,
        hits as f32 / probes.len() as f32,
        mapped_hits as f32 / probes.len() as f32,
    );
    let _ = std::fs::remove_file(&path);
}

fn report(
    mode: &str,
    index: &SpannIndex,
    src: &Memory,
    rows: usize,
    dim: usize,
    secs: f64,
    baseline_kb: usize,
    beam: usize,
) {
    let kb = Sampler::rss_kb();
    let recall = recall(index, src, beam);
    let mut depths = std::collections::BTreeMap::new();
    let mut leaves = 0usize;
    for part in index.partitions() {
        *depths.entry(part.level).or_insert(0usize) += 1;
        if part.children.is_empty() {
            leaves += 1;
        }
    }
    println!(
        "{mode}: rows {rows} dim {dim} in {secs:.1}s ({:.0} rows/s); partitions {} height {}; \
         bytes/row {} ({} MB codes+meta); recall@1 {:.3}; rss {} MB over source",
        rows as f64 / secs,
        index.partitions().len(),
        index.height(),
        index.mem_usage() / rows.max(1),
        index.mem_usage() / (1 << 20),
        recall,
        kb.saturating_sub(baseline_kb) / 1024
    );
    println!("  leaves {leaves} levels {depths:?}");
}

/// Self-query recall: every probe is a stored row, so the exact top-1 is that
/// row itself. This is the same measure the index's own tests use - it does not
/// prove ranking quality, it proves the routing + codes still find the row.
fn recall(index: &SpannIndex, src: &Memory, beam: usize) -> f32 {
    let rows = src.len();
    let probes = rows.clamp(1, 100);
    let stride = (rows / probes).max(1);
    let mut hits = 0usize;
    for i in 0..probes {
        let row = i * stride;
        let got = index.search(src.vector(row), 1, Some(beam));
        if got.first().map(|(id, _)| *id) == Some(src.id(row)) {
            hits += 1;
        }
    }
    hits as f32 / probes as f32
}

struct Sampler {
    stop: Arc<AtomicBool>,
    peak_kb: Arc<AtomicUsize>,
    handle: Option<std::thread::JoinHandle<()>>,
}

impl Sampler {
    fn start() -> Self {
        let stop = Arc::new(AtomicBool::new(false));
        let peak_kb = Arc::new(AtomicUsize::new(0));
        let (stop_worker, peak) = (stop.clone(), peak_kb.clone());
        let handle = std::thread::spawn(move || {
            let pid = std::process::id().to_string();
            while !stop_worker.load(Ordering::Relaxed) {
                peak.fetch_max(Self::read_kb(&pid), Ordering::Relaxed);
                std::thread::sleep(Duration::from_millis(40));
            }
        });
        Sampler {
            stop,
            peak_kb,
            handle: Some(handle),
        }
    }

    fn rss_kb() -> usize {
        Self::read_kb(&std::process::id().to_string())
    }

    fn read_kb(pid: &str) -> usize {
        let out = std::process::Command::new("ps")
            .args(["-o", "rss=", "-p", pid])
            .output();
        match out {
            Ok(out) => String::from_utf8_lossy(&out.stdout)
                .trim()
                .parse()
                .unwrap_or(0),
            Err(_) => 0,
        }
    }

    fn stop(mut self) -> usize {
        self.stop.store(true, Ordering::Relaxed);
        if let Some(handle) = self.handle.take() {
            let _ = handle.join();
        }
        self.peak_kb.load(Ordering::Relaxed)
    }
}

struct Lcg {
    state: u64,
}

impl Lcg {
    fn next(&mut self) -> f32 {
        self.state ^= self.state << 13;
        self.state ^= self.state >> 7;
        self.state ^= self.state << 17;
        (self.state >> 40) as f32 / (1u32 << 24) as f32
    }
}

/// Clustered rows: 64 centres, 0.25 sigma of noise, so the tree has something
/// real to find instead of uniform noise.
fn make_rows(n: usize, dim: usize) -> Memory {
    let clusters = 64usize;
    let mut rng = Lcg {
        state: 0x2545_F491_4F6C_DD1D,
    };
    let centres: Vec<Vec<f32>> = (0..clusters)
        .map(|_| (0..dim).map(|_| rng.next() * 2.0 - 1.0).collect())
        .collect();
    let rows: Vec<(u64, Vec<f32>)> = (0..n)
        .map(|i| {
            let centre = &centres[i % clusters];
            let v: Vec<f32> = (0..dim)
                .map(|d| centre[d] + (rng.next() * 2.0 - 1.0) * 0.25)
                .collect();
            (i as u64, v)
        })
        .collect();
    Memory::new(rows)
}

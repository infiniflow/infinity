//! The process's memory ceiling, and the pass that keeps the process under it.
//!
//! Four budgets already existed and none of them is the process's:
//! `buffer_budget` (the block mappings and the decoded index chunks),
//! `memindex_quota` (the index tails), `spann_page_budget` /
//! `spann_dirty_budget` (one delta write). The only place that compared a number
//! with the process was the cleanup task - and it ran only when a transaction
//! had committed since the last pass, so a read-only server that grew its page
//! cache was never reclaimed. This module is the one place that reads the
//! process's own figure and decides.
//!
//! The component budgets bound what the engine *asks for*; this one covers the
//! rest (the WAL, the page cache of files read without a mapping, the allocator)
//! and is the only one that can hand memory back while a query is running.
//!
//! ## What the feed is, per platform
//!
//! * **Linux**: the cgroup's own `memory.current`, which is the number the OOM
//!   killer uses; it includes the page cache, i.e. exactly the bytes the reclaim
//!   pass can give back. `memory.max` is the ceiling and `memory.high`, when it
//!   is set, is the kernel's own soft watermark - better than a percentage of
//!   ours.
//! * **macOS / Windows**: no cgroup. There is no per-process ceiling to read, so
//!   the ceiling comes from the configuration alone ([`Ceiling::source`] says
//!   `None` otherwise) and the usage is the process's resident size. Guessing a
//!   fraction of physical memory would turn a healthy process into one that
//!   rejects writes, which is worse than not governing at all.
//!
//! Windows is the one non-Linux target with a real hard limit (`JobObject`
//! `ProcessMemoryLimit`, which is what a Windows container sets); reading it is
//! the next step there, not a guess.

use std::sync::atomic::Ordering;

/// Below this share of the ceiling nothing is touched.
pub(crate) const LOW_PERCENT: u64 = 70;
/// Above this share a reclaim pass starts.
pub(crate) const HIGH_PERCENT: u64 = 85;
/// Above this share writes are refused once a pass has failed to get under it.
pub(crate) const CRITICAL_PERCENT: u64 = 95;

/// The ceiling the process is governed by, and where it came from.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) struct Ceiling {
    pub bytes: u64,
    pub source: &'static str,
}

impl Ceiling {
    pub(crate) fn disabled() -> Self {
        Self {
            bytes: 0,
            source: "none",
        }
    }
}

/// How full the process is.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[repr(u8)]
pub(crate) enum Pressure {
    Ok,
    /// Over the high watermark: reclaim on the next tick.
    High,
    /// Over the critical watermark: reclaim, and refuse new writes.
    Critical,
}

impl Pressure {
    pub(crate) fn blocks_writes(self) -> bool {
        matches!(self, Self::Critical)
    }

    fn from_byte(value: u8) -> Self {
        match value {
            2 => Self::Critical,
            1 => Self::High,
            _ => Self::Ok,
        }
    }
}

/// How long a measured pressure is reused.
///
/// The measurement is not free: on Linux it reads the cgroup's `memory.current`,
/// and on macOS the resident size comes from `ps` (a fork per call). Throttling
/// it is also correct - the watermarks are compared against a level that only
/// moves over seconds, and the reclaim pass that follows a refusal measures
/// again anyway.
const CHECK_INTERVAL_MS: u64 = 500;

fn now_millis() -> u64 {
    std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .map(|since| since.as_millis() as u64)
        .unwrap_or(0)
}

#[cfg(target_os = "linux")]
mod proc {
    use std::path::Path;

    /// A cgroup file holding one number, or `None` when it says `max` / holds
    /// the "unlimited" sentinel (cgroup v1 writes a huge page-aligned value).
    fn read_number(path: &Path) -> Option<u64> {
        let text = std::fs::read_to_string(path).ok()?;
        let text = text.trim();
        let value: u64 = text.parse().ok()?;
        if value >= 1 << 62 {
            return None;
        }
        Some(value)
    }

    /// The cgroup this process belongs to, from `/proc/self/cgroup`: `0::/path`
    /// on v2, the `memory` controller's line on v1.
    fn cgroup_path(v2: bool) -> Option<String> {
        let text = std::fs::read_to_string("/proc/self/cgroup").ok()?;
        for line in text.lines() {
            let mut parts = line.splitn(3, ':');
            let (Some(hierarchy), Some(controllers), Some(path)) =
                (parts.next(), parts.next(), parts.next())
            else {
                continue;
            };
            if (v2 && hierarchy == "0" && controllers.is_empty())
                || (!v2 && controllers.split(',').any(|c| c == "memory"))
            {
                return Some(path.trim().to_string());
            }
        }
        None
    }

    /// `/sys/fs/cgroup` joined with the process's own cgroup, falling back to the
    /// mount point when the namespace does not expose the path.
    fn dir(v2: bool) -> String {
        let mount = if v2 {
            "/sys/fs/cgroup".to_string()
        } else {
            "/sys/fs/cgroup/memory".to_string()
        };
        match cgroup_path(v2) {
            Some(path) if !path.is_empty() && path != "/" => format!("{mount}{path}"),
            _ => mount,
        }
    }

    /// One cgroup v1 file: the process's own cgroup first, then the mount root.
    ///
    /// v1 puts each controller in its own hierarchy and the limit on the
    /// process's cgroup inside it (`/proc/self/cgroup` names which), so reading
    /// `/sys/fs/cgroup/memory/memory.limit_in_bytes` alone reports the **root**
    /// cgroup - unlimited - and a v1 deployment would silently run ungoverned.
    fn v1(name: &str) -> Option<u64> {
        read_number(Path::new(&format!("{}/{name}", dir(false))))
            .or_else(|| read_number(Path::new(&format!("/sys/fs/cgroup/memory/{name}"))))
    }

    pub(super) fn ceiling() -> Option<u64> {
        if Path::new("/sys/fs/cgroup/memory.max").exists() {
            // v2: the limit may sit on any ancestor, and the effective one is the
            // smallest, so walk up from our own cgroup to the mount point.
            let mut here = dir(true);
            loop {
                if let Some(value) = read_number(Path::new(&format!("{here}/memory.max"))) {
                    return Some(value);
                }
                let parent = Path::new(&here).parent()?.display().to_string();
                if !parent.starts_with("/sys/fs/cgroup") || parent == here {
                    return None;
                }
                here = parent;
            }
        }
        v1("memory.limit_in_bytes")
    }

    pub(super) fn high() -> Option<u64> {
        read_number(Path::new(&format!("{}/memory.high", dir(true))))
            .or_else(|| v1("memory.soft_limit_in_bytes"))
    }

    pub(super) fn usage() -> Option<u64> {
        read_number(Path::new(&format!("{}/memory.current", dir(true))))
            .or_else(|| v1("memory.usage_in_bytes"))
    }
}

#[cfg(not(target_os = "linux"))]
mod proc {
    // No cgroup: macOS has no per-process ceiling (jetsam is iOS; a desktop
    // daemon is only ever slowed by swap), and Windows exposes one through a
    // `JobObject`, not a file. The ceiling comes from the configuration there.
    pub(super) fn ceiling() -> Option<u64> {
        None
    }

    pub(super) fn high() -> Option<u64> {
        None
    }

    pub(super) fn usage() -> Option<u64> {
        None
    }
}

/// The cgroup's ceiling, discovered once: it cannot change without a new
/// container, and `memory_pressure` runs on the write path.
///
/// `UNKNOWN` is 0 and `NONE` is `u64::MAX`, so the common "there is no cgroup
/// limit" case costs one atomic load after the first look.
static CGROUP_CEILING: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(0);

/// The cgroup's memory ceiling, when it has one.
pub(crate) fn cgroup_ceiling() -> Option<u64> {
    const NONE: u64 = u64::MAX;
    let cached = CGROUP_CEILING.load(Ordering::Relaxed);
    if cached == NONE {
        return None;
    }
    if cached > 0 {
        return Some(cached);
    }
    let found = proc::ceiling();
    CGROUP_CEILING.store(found.unwrap_or(NONE), Ordering::Relaxed);
    found
}

/// The cgroup's own usage figure - the one the kernel charges, page cache
/// included.
pub(crate) fn cgroup_usage() -> Option<u64> {
    proc::usage()
}

/// The cgroup's soft limit (`memory.high`): the kernel throttles at it instead
/// of killing, so it is the best high watermark when it is set.
pub(crate) fn cgroup_high() -> Option<u64> {
    proc::high()
}

/// `(low, high, critical)` for `ceiling`, with `memory.high` overriding the
/// high watermark when the kernel has one.
fn watermarks(ceiling: u64, kernel_high: Option<u64>) -> (u64, u64, u64) {
    let low = ceiling * LOW_PERCENT / 100;
    let critical = ceiling * CRITICAL_PERCENT / 100;
    let high = kernel_high
        .filter(|high| *high > low && *high < critical)
        .unwrap_or(ceiling * HIGH_PERCENT / 100);
    (low, high, critical)
}

impl super::Engine {
    /// The ceiling this engine runs under: the configuration's
    /// `buffer_manager_size` (`memory_limit`) when it is set, the cgroup's
    /// `memory.max` otherwise. A container that sized neither is not governed.
    pub(crate) fn memory_ceiling(&self) -> Ceiling {
        let configured = self
            .context
            .memory_limit
            .load(std::sync::atomic::Ordering::Relaxed);
        if configured > 0 {
            return Ceiling {
                bytes: configured,
                source: "config",
            };
        }
        match cgroup_ceiling() {
            // Nine tenths: the kernel kills at the limit, and the last tenth is
            // what a reclaim pass needs to actually get below it.
            Some(bytes) => Ceiling {
                bytes: bytes / 10 * 9,
                source: "cgroup",
            },
            None => Ceiling::disabled(),
        }
    }

    /// What the process is using, as the platform's own account of it: the
    /// cgroup's `memory.current` where there is one (page cache included, which
    /// is what the kernel kills on), the process's resident size otherwise.
    pub(crate) fn memory_usage(&self) -> u64 {
        cgroup_usage().unwrap_or_else(crate::engine::state::process_rss_bytes)
    }

    /// How full the process is against [`Self::memory_ceiling`], measured at most
    /// once per [`CHECK_INTERVAL_MS`].
    pub(crate) fn memory_pressure(&self) -> Pressure {
        let now = now_millis();
        let checked = self
            .context
            .memory_checked_at
            .load(std::sync::atomic::Ordering::Relaxed);
        if now.saturating_sub(checked) < CHECK_INTERVAL_MS {
            return Pressure::from_byte(
                self.context
                    .memory_pressure_level
                    .load(std::sync::atomic::Ordering::Relaxed),
            );
        }
        let pressure = self.measure_pressure();
        self.context
            .memory_checked_at
            .store(now, std::sync::atomic::Ordering::Relaxed);
        self.context
            .memory_pressure_level
            .store(pressure as u8, std::sync::atomic::Ordering::Relaxed);
        pressure
    }

    /// [`Self::memory_pressure`] without the throttle.
    fn measure_pressure(&self) -> Pressure {
        let ceiling = self.memory_ceiling();
        if ceiling.bytes == 0 {
            return Pressure::Ok;
        }
        let (_, high, critical) = watermarks(ceiling.bytes, cgroup_high());
        match self.memory_usage() {
            usage if usage >= critical => Pressure::Critical,
            usage if usage >= high => Pressure::High,
            _ => Pressure::Ok,
        }
    }

    /// Gives back up to `want` bytes, cheapest lever first: allocator arenas,
    /// the resident pages of the live mappings, then the block mappings
    /// themselves - and, when `forced`, the SPANN page bookkeeping and the
    /// caches that are pure copies. Returns what it released.
    ///
    /// Dropping a block mapping is the step that actually shrinks the process:
    /// `madvise(DONTNEED)` only drops the resident pages and leaves the mapping
    /// (and the decoded `version`'s heap) in place, so a table larger than the
    /// budget would still hold one mapping per block ever read. Readers already
    /// inside a scan keep their `Arc`; the next one remaps from the (still warm)
    /// page cache.
    ///
    /// `forced` is the critical watermark: it adds the two steps that cost the
    /// most to redo - the SPANN handles' own page bookkeeping and the caches
    /// that are pure copies (results, plans, the whole-column vector cache an
    /// exact scan builds when it has no index).
    pub(crate) fn reclaim_memory(&self, want: u64, forced: bool) -> u64 {
        storage::trim_allocator();
        let mut released = storage::mapped::release_mapped_pages(want as usize) as u64;
        if forced {
            released += self.spann_release_pages_forced() as u64;
            // Pure copies: a query that needs them recomputes them.
            self.result_cache.lock().unwrap().clear();
            self.plan_cache.clear();
            self.vector_cache.lock().unwrap().clear();
        }
        // The page advice reports the mappings' length, which is an upper bound
        // on what was actually resident (and a cgroup keeps charging a page
        // cache it has not reclaimed yet), so the decision to escalate is the
        // process's own figure - not the sum above. Escalate when the cheap
        // pass did not meet the target, or when the process is still over.
        let pressure = self.measure_pressure();
        if forced || pressure != Pressure::Ok || released < want {
            let over = self.memory_usage().saturating_sub(self.memory_low());
            let remaining = want.saturating_sub(released).max(over);
            released += self.shrink_block_maps(remaining as usize) as u64;
        }
        released
    }

    /// The low watermark: the level a pass reclaims down to. `0` when no
    /// ceiling governs the process.
    fn memory_low(&self) -> u64 {
        let ceiling = self.memory_ceiling();
        if ceiling.bytes == 0 {
            return 0;
        }
        watermarks(ceiling.bytes, cgroup_high()).0
    }

    /// Drops least-recently-used block mappings until `want` bytes are no
    /// longer mapped, returning what the cache gave up.
    ///
    /// The mapping itself is the cache entry, so this unmaps the files - the
    /// one lever that returns the process's address space and not just the
    /// resident pages. A reader in the middle of a scan keeps its `Arc`; the
    /// next one remaps from the page cache.
    pub(crate) fn shrink_block_maps(&self, want: usize) -> usize {
        if want == 0 {
            return 0;
        }
        self.context.block_maps.lock().unwrap().shrink_by(want)
    }

    /// One pass of the governor, run on the periodic tick - deliberately **not**
    /// behind `needs_cleanup`: a read-only server can grow its page cache without
    /// a single transaction. Returns the bytes it released.
    pub(crate) fn memory_tick(&self) -> u64 {
        let ceiling = self.memory_ceiling();
        if ceiling.bytes == 0 {
            return 0;
        }
        let usage = self.memory_usage();
        let (low, high, critical) = watermarks(ceiling.bytes, cgroup_high());
        if usage < high {
            return 0;
        }
        // Down to the low watermark, not by one page: a pass that stops at `high`
        // reruns on the next tick and thrashes the working set.
        let want = usage.saturating_sub(low);
        let released = self.reclaim_memory(want, usage >= critical);
        // Measure again rather than assuming the pass worked: when nothing can
        // be released (a ceiling below the working set) the level is still
        // critical, and the next write must see that.
        let after = self.measure_pressure();
        self.context
            .memory_pressure_level
            .store(after as u8, Ordering::Relaxed);
        self.context
            .memory_checked_at
            .store(now_millis(), Ordering::Relaxed);
        self.context
            .memory_reclaim_passes
            .fetch_add(1, Ordering::Relaxed);
        self.context
            .memory_reclaimed_bytes
            .fetch_add(released, Ordering::Relaxed);
        if released == 0 && usage >= critical {
            // Nothing left to give: the ceiling is smaller than the working set.
            // Said once per pass because the next lever is the operator's.
            eprintln!(
                "memory: {usage} bytes in use against a {} ceiling ({}) and nothing left to \
                 release; writes are refused until it drops",
                ceiling.bytes, ceiling.source
            );
        }
        released
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A ceiling below the working set is the one case the levers cannot fix, so
    /// it is the one that decides the process's fate: writes are refused (the
    /// front end renders that as an error) while reads keep working.
    #[test]
    fn a_write_is_refused_when_the_ceiling_is_below_the_working_set() {
        let dir = tempfile::tempdir().unwrap();
        let e = super::super::Engine::open(dir.path()).unwrap();
        e.execute_blocking("CREATE TABLE meml (id INT);").unwrap();
        // One byte: every pass runs the levers and still cannot get under it,
        // which is exactly the state that must not take the process down.
        e.set_memory_limit(1);
        assert!(e.memory_pressure().blocks_writes());
        let _ = e.memory_tick();
        assert_eq!(
            e.context.memory_reclaim_passes.load(Ordering::Relaxed),
            1,
            "the tick ran a pass"
        );
        let err = e
            .execute_blocking("INSERT INTO meml VALUES (1);")
            .unwrap_err()
            .to_string();
        assert!(err.contains("memory limit exceeded"), "{err}");
        // Reads allocate nothing new and stay available under the same ceiling.
        e.execute_blocking("SELECT COUNT(*) FROM meml;").unwrap();
    }

    /// A pass that cannot release enough by advising pages has to drop the
    /// mappings themselves: `madvise(DONTNEED)` leaves the mapping (and its
    /// address space) in place, and a table larger than the budget would keep
    /// one mapping per block ever read. Dropping the entry unmaps the block;
    /// the next scan maps it again from a page cache that is still warm.
    #[test]
    fn a_reclaim_pass_unmaps_blocks_when_dropping_pages_is_not_enough() {
        let dir = tempfile::tempdir().unwrap();
        let e = super::super::Engine::open(dir.path()).unwrap();
        e.execute_blocking("CREATE TABLE memu (id INT);").unwrap();
        e.execute_blocking("INSERT INTO memu VALUES (1);").unwrap();
        crate::scheduler::block_on(e.flush()).unwrap();
        let root = e.blocks_root();
        let id = e.visible_block_ids("default_db.memu")[0];
        let _ = e.block_map("default_db.memu", &root, id).unwrap();
        assert_eq!(e.buffer_objects().len(), 1);

        // A terabyte the page advice cannot produce on its own: the pass has to
        // go on to the block map.
        let released = e.reclaim_memory(1 << 40, false);
        assert!(released > 0);
        assert!(
            e.buffer_objects().is_empty(),
            "the block mapping was unmapped"
        );

        // The rows are untouched: the next read maps the block again.
        assert_eq!(
            e.execute_blocking("SELECT COUNT(*) FROM memu;")
                .unwrap()
                .len(),
            1
        );
    }

    /// The periodic tick is the path a read-only server relies on: with the
    /// process over its ceiling it gives the block mappings back even though no
    /// transaction committed since the last pass.
    #[test]
    fn a_tick_over_the_ceiling_gives_the_block_mappings_back() {
        let dir = tempfile::tempdir().unwrap();
        let e = super::super::Engine::open(dir.path()).unwrap();
        e.execute_blocking("CREATE TABLE memt (id INT);").unwrap();
        e.execute_blocking("INSERT INTO memt VALUES (1);").unwrap();
        crate::scheduler::block_on(e.flush()).unwrap();
        let root = e.blocks_root();
        let id = e.visible_block_ids("default_db.memt")[0];
        let _ = e.block_map("default_db.memt", &root, id).unwrap();
        assert_eq!(e.buffer_objects().len(), 1);

        // One byte: every measurement is critical, which is the state that must
        // reclaim rather than wait for a write.
        e.set_memory_limit(1);
        e.memory_tick();
        assert!(
            e.buffer_objects().is_empty(),
            "the tick unmapped the block instead of leaving it resident"
        );
        assert!(e.context.memory_reclaim_passes.load(Ordering::Relaxed) >= 1);
    }

    /// The whole-column vector cache is a heap copy of every vector in the
    /// column, so it is capped at a share of the buffer budget: past that the
    /// exact scan streams instead of building it, and the query still answers.
    #[test]
    fn a_vector_cache_beyond_its_share_is_not_built() {
        let dir = tempfile::tempdir().unwrap();
        let e = super::super::Engine::open(dir.path()).unwrap();
        e.execute_blocking("CREATE TABLE memv (id INT, v EMBEDDING(FLOAT,4));")
            .unwrap();
        e.execute_blocking("INSERT INTO memv VALUES (1,[0.0,0.0,0.0,0.0]),(2,[1.0,1.0,1.0,1.0]);")
            .unwrap();
        // A quarter of 64 bytes is smaller than one cached row.
        e.set_buffer_budget(64);
        assert!(
            e.vector_cache_for("default_db.memv", "v", 4).is_err(),
            "the column does not fit the cache's share"
        );
        assert!(
            e.vector_cache.lock().unwrap().is_empty(),
            "a refused build keeps nothing"
        );
        // The exact scan falls back to streaming and still answers.
        let mut out = Vec::new();
        for r in e
            .execute_blocking(
                "SELECT id FROM memv SEARCH MATCH VECTOR \
                 (v, [1.0,1.0,1.0,1.0], 'float', 'l2', 1);",
            )
            .unwrap()
        {
            if let crate::QueryResult::Rows { .. } = r {
                out = r.rows_text();
            }
        }
        assert_eq!(out, vec![vec!["2".to_string()]]);
    }

    /// The watermarks come from the ceiling, and a kernel `memory.high` wins over
    /// the percentage - it is the number the kernel itself throttles at.
    #[test]
    fn watermarks_follow_the_ceiling_and_the_kernels_soft_limit() {
        let (low, high, critical) = watermarks(1000, None);
        assert_eq!((low, high, critical), (700, 850, 950));
        let (low, high, critical) = watermarks(1000, Some(800));
        assert_eq!((low, high, critical), (700, 800, 950));
        // A `memory.high` outside (low, critical) is not a watermark this engine
        // can use, so the percentage stands.
        assert_eq!(watermarks(1000, Some(990)).1, 850);
        assert_eq!(watermarks(1000, Some(100)).1, 850);
    }
}

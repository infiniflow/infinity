//! What every index tells the maintenance layer (`CSPANN_DESIGN.md` §12.4).
//!
//! The quota check and the seal scheduler both need the same three answers from
//! a tail - how many bytes it holds, how many would stay resident after a seal,
//! and how expensive sealing it is. Before this trait each index kind answered
//! with its own ad-hoc getter (`index_pending_bytes`, `tail_bytes`,
//! `approx_bytes`, `mem_used`), and an index whose figure was never charged -
//! an HNSW graph built by `CREATE INDEX`, whose charge only the *write* path
//! raises - looked empty and could not be released.
//!
//! The trait is the accounting surface only. **Sealing is not here** on purpose:
//! writing a chunk needs the catalog, the transaction and the chunk writer,
//! which live in the executor; an index that could seal itself would have to
//! own half of them. §9.6's `MaintainQueue` lives in the executor for the same
//! reason (`engine/maintain.rs`).

/// The accounting and decision surface of one in-memory index tail.
pub trait IndexMaintainer: Send + Sync {
    /// `"HNSW"`, `"CSPANN"`, `"fulltext"`, … - the kind the scheduler reports.
    fn kind(&self) -> &'static str;

    /// Bytes the tail holds right now: what the byte ledger compares against
    /// the waterlines.
    fn pending_bytes(&self) -> usize;

    /// Bytes that stay resident **after** a seal. A mapped chunk is evictable
    /// page cache and counts as `0`; a rebuilt tree in heap counts in full.
    fn resident_bytes(&self) -> usize;

    /// Relative cost of sealing, `0.0..=1.0`. Under equal pressure the
    /// scheduler seals the cheap tail first, because it gives the same bytes
    /// back for less work.
    fn optimize_cost(&self) -> f32;

    /// Whether the writing thread may seal this tail synchronously
    /// (degrade-to-sync, §9.5's `hard` row) without a long stall.
    fn can_degrade(&self) -> bool;
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A tail that is only accounted for: the trait's contract is the numbers,
    /// so a stub is all a unit test needs.
    struct Stub {
        pending: usize,
        resident: usize,
    }

    impl IndexMaintainer for Stub {
        fn kind(&self) -> &'static str {
            "stub"
        }
        fn pending_bytes(&self) -> usize {
            self.pending
        }
        fn resident_bytes(&self) -> usize {
            self.resident
        }
        fn optimize_cost(&self) -> f32 {
            0.0
        }
        fn can_degrade(&self) -> bool {
            true
        }
    }

    #[test]
    fn the_trait_is_object_safe() {
        // The scheduler holds a heterogeneous list of tails, so the surface has
        // to be usable behind a reference.
        let tails: Vec<Box<dyn IndexMaintainer>> = vec![
            Box::new(Stub {
                pending: 1 << 20,
                resident: 0,
            }),
            Box::new(Stub {
                pending: 2 << 20,
                resident: 1 << 20,
            }),
        ];
        let total: usize = tails.iter().map(|t| t.pending_bytes()).sum();
        assert_eq!(total, 3 << 20);
        assert_eq!(tails[1].resident_bytes(), 1 << 20);
        assert!(tails.iter().all(|t| t.can_degrade()));
    }
}

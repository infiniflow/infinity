//! The executor's cspann glue: DDL option parsing and the build entry point
//! (`docs/CSPANN_DESIGN.md` §15).
//!
//! The index itself lives in `index::cspann` and knows nothing about SQL; this
//! module is the only place that turns `WITH (…)` text into `Options` and the
//! engine's row ids into the index's `(row, vector)` pairs.

use std::sync::Arc;

use common::{Error, Result};
use index::cspann::{CspannError, CspannIndex, Encode, Options};
use index::vector::Metric;

/// The `WITH (…)` keys `CREATE INDEX … USING CSPANN` accepts.
pub(crate) const OPTION_KEYS: &[&str] = &[
    "METRIC",
    "MAX_PARTITION_SIZE",
    "MIN_PARTITION_SIZE",
    "FILL_FACTOR",
    "ENCODE",
    "NUM_CENTROIDS",
    "KMEANS_ITERATIONS",
    "SEED",
    "BEAM",
];

/// Translates the DDL options into the builder's knobs.
pub(crate) fn options_from(opts: &[(String, String)]) -> Result<Options> {
    let mut out = Options::default();
    for (key, value) in opts {
        let key_upper = key.trim().to_ascii_uppercase();
        let value = value.trim().trim_matches('\'').trim_matches('"');
        match key_upper.as_str() {
            "METRIC" => {
                out.metric = match value.to_ascii_lowercase().as_str() {
                    "l2" => Metric::L2,
                    "ip" | "inner_product" => Metric::Ip,
                    "cosine" | "cos" => Metric::Cos,
                    other => {
                        return Err(Error::Invalid(format!(
                            "invalid metric '{other}' for a CSPANN index"
                        )))
                    }
                };
            }
            "MAX_PARTITION_SIZE" => out.max_partition_size = parse_usize(key, value)?,
            "MIN_PARTITION_SIZE" => out.min_partition_size = parse_usize(key, value)?,
            "FILL_FACTOR" => {
                out.fill_factor = value.parse::<f32>().map_err(|_| {
                    Error::Invalid(format!(
                        "invalid fill_factor '{value}' (expected 0.3..=1.0)"
                    ))
                })?;
            }
            "ENCODE" => {
                out.encode = Encode::parse(value).ok_or_else(|| {
                    Error::Invalid(format!(
                        "invalid encode '{value}' (expected 'rabitq' or 'none')"
                    ))
                })?;
            }
            "NUM_CENTROIDS" => out.root_fanout = parse_usize(key, value)?,
            "KMEANS_ITERATIONS" => out.kmeans_iterations = parse_usize(key, value)?,
            "SEED" => {
                out.seed = value.parse::<u64>().map_err(|_| {
                    Error::Invalid(format!("invalid seed '{value}' (expected a u64)"))
                })?;
            }
            "BEAM" => out.base_beam = parse_usize(key, value)?,
            other => {
                return Err(Error::Invalid(format!(
                    "invalid index parameter: unsupported CSPANN option '{other}'"
                )))
            }
        }
    }
    // A caller that only names `max_partition_size` must not trip over the
    // default `min_partition_size` (32): the floor cannot exceed the ceiling.
    if out.min_partition_size > out.max_partition_size {
        out.min_partition_size = (out.max_partition_size / 2).max(2);
    }
    out.validate().map_err(|e| Error::Invalid(e.to_string()))?;
    Ok(out)
}

fn parse_usize(key: &str, value: &str) -> Result<usize> {
    value
        .parse::<usize>()
        .map_err(|_| Error::Invalid(format!("invalid {key} '{value}' (expected an integer)")))
}

/// Builds the tree from `(row id, vector)` pairs.
pub(crate) fn build(
    rows: Vec<(u64, Vec<f32>)>,
    opts: &[(String, String)],
) -> Result<Arc<CspannIndex>> {
    let options = options_from(opts)?;
    CspannIndex::build(rows, options)
        .map(Arc::new)
        .map_err(|e| match e {
            CspannError::Empty => {
                Error::Invalid("cspann: the indexed column has no vectors".to_string())
            }
            other => Error::Invalid(other.to_string()),
        })
}

/// A queryable cspann index: the tree a `CREATE INDEX` built, or the chunk a
/// later checkpoint published (§6.4).
///
/// The two forms must answer identically — `storage::knn_index::cspann` has a
/// test that runs the same queries through both — so the rest of the engine only
/// ever sees this enum.
pub(crate) enum CspannHandle {
    /// Built in this process and not yet published to a chunk.
    Memory(Arc<CspannIndex>),
    /// Served straight out of `chunk_{n}.idx`: the slot bytes are page cache,
    /// which is what makes 50M rows fit in 8 GB (§19.3).
    File(Arc<storage::knn_index::cspann::FileIndex>),
}

impl CspannHandle {
    pub(crate) fn len(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.len(),
            CspannHandle::File(index) => index.len(),
        }
    }

    pub(crate) fn dim(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.dim(),
            CspannHandle::File(index) => index.dims(),
        }
    }

    pub(crate) fn search(&self, q: &[f32], k: usize, beam: Option<usize>) -> Vec<(u64, f32)> {
        match self {
            CspannHandle::Memory(index) => index.search(q, k, beam),
            CspannHandle::File(index) => index.search(q, k, beam),
        }
    }

    /// The row ids the tree holds, ascending - the coverage check's input
    /// (`docs/CSPANN_DESIGN.md` §9.7).
    pub(crate) fn row_ids(&self) -> Vec<u64> {
        match self {
            CspannHandle::Memory(index) => index.row_ids(),
            CspannHandle::File(index) => index.row_ids(),
        }
    }

    /// [`Self::search`] restricted to the rows a statement admits.
    ///
    /// The tree's descent is not filter-aware - a partition either holds rows
    /// the bitmask admits or it does not, and the reference compares the slot's
    /// `row_id` while it reads the slot (`§12.5`). This asks for a bigger
    /// candidate list first and keeps the admitted rows, which is the same
    /// answer for a filter whose selectivity is not tiny, and the caller's
    /// `uncovered` fallback covers the rest.
    pub(crate) fn search_filtered(
        &self,
        q: &[f32],
        k: usize,
        beam: Option<usize>,
        allowed: &std::collections::HashSet<usize>,
    ) -> Vec<(u64, f32)> {
        if k == 0 {
            return Vec::new();
        }
        let len = self.len().max(1);
        let selectivity = (allowed.len() as f64 / len as f64).max(1e-3);
        let budget = ((k as f64) / selectivity).ceil() as usize;
        // Never ask for more rows than the tree holds; never ask for fewer than
        // the caller needs.
        let budget = budget.min(len).max(k);
        self.search(q, budget, beam)
            .into_iter()
            .filter(|(id, _)| allowed.contains(&(*id as usize)))
            .take(k)
            .collect()
    }

    /// `(partitions, height)`, for the bench and the `SHOW INDEX` telemetry
    /// (the latter is not wired yet, hence the allowance).
    #[allow(dead_code)]
    pub(crate) fn shape(&self) -> (usize, u32) {
        match self {
            CspannHandle::Memory(index) => (index.partitions().len(), index.height()),
            CspannHandle::File(index) => (index.partitions(), index.height()),
        }
    }

    /// Heap bytes this handle holds. A file-backed index holds only its header
    /// and section table, so the answer is 0 and the slot pages show up as page
    /// cache instead (`RssFile`, §19.6).
    ///
    /// Only the bench reads it today; the `cspann_bytes` gauge (§16.3) will.
    #[allow(dead_code)]
    pub(crate) fn mem_usage(&self) -> usize {
        match self {
            CspannHandle::Memory(index) => index.mem_usage(),
            CspannHandle::File(_) => 0,
        }
    }

    #[allow(dead_code)]
    pub(crate) fn is_file_backed(&self) -> bool {
        matches!(self, CspannHandle::File(_))
    }
}

/// The candidate budget for one query (`§8.6`): a 1-bit code cannot order
/// near-tied distances, so the index is asked for ~10× the requested top-n and
/// the exact rescoring decides.
pub(crate) fn candidate_budget(topn: usize) -> usize {
    (topn * 10).max(64)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn pairs(items: &[(&str, &str)]) -> Vec<(String, String)> {
        items
            .iter()
            .map(|(k, v)| (k.to_string(), v.to_string()))
            .collect()
    }

    #[test]
    fn parses_every_documented_option() {
        let opts = options_from(&pairs(&[
            ("metric", "cosine"),
            ("max_partition_size", "64"),
            ("min_partition_size", "16"),
            ("fill_factor", "0.75"),
            ("encode", "rabitq"),
            ("num_centroids", "32"),
            ("kmeans_iterations", "4"),
            ("seed", "42"),
            ("beam", "6"),
        ]))
        .unwrap();
        assert_eq!(opts.metric, Metric::Cos);
        assert_eq!(opts.max_partition_size, 64);
        assert_eq!(opts.min_partition_size, 16);
        assert!((opts.fill_factor - 0.75).abs() < 1e-6);
        assert_eq!(opts.encode, Encode::Rabitq);
        assert_eq!(opts.root_fanout, 32);
        assert_eq!(opts.kmeans_iterations, 4);
        assert_eq!(opts.seed, 42);
        assert_eq!(opts.base_beam, 6);
    }

    #[test]
    fn defaults_are_the_designs_p0_values() {
        let opts = options_from(&[]).unwrap();
        assert_eq!(opts.metric, Metric::L2);
        assert_eq!(opts.max_partition_size, 128);
        assert_eq!(opts.base_beam, 8);
        assert_eq!(opts.encode, Encode::Rabitq);
    }

    #[test]
    fn rejects_unknown_and_out_of_range_options() {
        assert!(options_from(&pairs(&[("M", "16")])).is_err());
        assert!(options_from(&pairs(&[("METRIC", "hamming")])).is_err());
        assert!(options_from(&pairs(&[("BEAM", "0")])).is_err());
        assert!(options_from(&pairs(&[("FILL_FACTOR", "2.0")])).is_err());
        assert!(options_from(&pairs(&[("SEED", "abc")])).is_err());
    }

    #[test]
    fn builds_and_answers() {
        let rows: Vec<(u64, Vec<f32>)> = (0..200)
            .map(|i| {
                (
                    i as u64,
                    (0..32).map(|d| ((i * 7 + d * 3) % 29) as f32).collect(),
                )
            })
            .collect();
        let index = build(rows.clone(), &pairs(&[("max_partition_size", "32")])).unwrap();
        assert_eq!(index.len(), 200);
        let hits = index.search(&rows[0].1, 10, None);
        assert_eq!(hits.len(), 10);
        assert!(
            hits.iter().any(|(id, _)| *id == 0),
            "the row itself is a hit"
        );
    }

    #[test]
    fn empty_input_is_reported_as_invalid() {
        assert!(build(Vec::new(), &[]).is_err());
    }

    #[test]
    fn candidate_budget_is_an_order_of_magnitude_above_top_n() {
        assert_eq!(candidate_budget(10), 100);
        assert_eq!(candidate_budget(1), 64);
    }
}

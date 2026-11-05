//! The reference's EMVB search stages over a trained residual-PQ index.
//!
//! [`EmvbView`] borrows the persisted arrays of one EMVB segment (coarse
//! centroids, centroid assignments, the residual product quantizer and the
//! per-document layout) and runs the same three-stage search as the reference:
//!
//!   1. `find_candidate_docs` — per query token, keep the best `nprobe`
//!      centroids above `threshold_first`; their documents are the candidates.
//!   2. `compute_hit_frequency` — rank candidates by how many query tokens
//!      their embeddings' centroids cover (`n_doc_to_score`).
//!   3. `second_stage_filtering` — approximate MaxSim with the centroid inner
//!      products only (`out_second_stage`).
//!   4. `compute_topk_documents_selected` — refine with the residual PQ code
//!      and keep the top-k.
//!
//! C++: `EMVBIndex::GetQueryResult` / `EMVBSearch` (`src/storage/knn_index/
//! emvb/emvb_search_impl.cpp`).

/// Search options; the defaults mirror `PLAID_*` in
/// `src/common/default_values.cppm:154`.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct EmvbSearchOptions {
    pub centroid_nprobe: usize,
    pub threshold_first: f32,
    pub n_doc_to_score: usize,
    pub out_second_stage: usize,
    pub threshold_final: f32,
}

impl EmvbSearchOptions {
    /// `PLAID_N_IVF_PROBE = 8`, `PLAID_CENTROID_SCORE_THRESHOLD = -0.1`,
    /// `PLAID_N_DOC_TO_SCORE_FACTOR = 100`, `PLAID_N_FULL_SCORES_FACTOR = 10`.
    pub fn for_topk(topk: usize) -> Self {
        Self {
            centroid_nprobe: 8,
            threshold_first: -0.1,
            n_doc_to_score: topk.saturating_mul(100),
            out_second_stage: topk.saturating_mul(10),
            threshold_final: 0.0,
        }
    }
}

impl Default for EmvbSearchOptions {
    fn default() -> Self {
        Self::for_topk(1)
    }
}

/// Optimised-product-quantisation rotation of the residual codebook.
///
/// C++: `OPQ` (`src/storage/knn_index/emvb/product_quantizer.cppm`) and
/// `OPQ::Train` (`product_quantizer_impl.cpp:150`). The reference multiplies
/// row vectors on the right, so the encoder maps `x -> x·R`, and
/// `GetIPDistanceTable` maps the query the same way (`R^T·q` in column form),
/// which leaves every inner product unchanged. `EMVBIndex` always builds an
/// OPQ (`emvb_index_impl.cpp:62`), never a plain PQ.
#[derive(Clone, Debug, PartialEq)]
pub struct OpqRotation {
    dims: usize,
    /// `dims * dims`, row-major, applied as `x·R`.
    matrix: Vec<f32>,
}

impl OpqRotation {
    pub fn identity(dims: usize) -> Self {
        let mut matrix = vec![0.0f32; dims * dims];
        for i in 0..dims {
            matrix[i * dims + i] = 1.0;
        }
        Self { dims, matrix }
    }

    pub fn dims(&self) -> usize {
        self.dims
    }

    pub fn matrix(&self) -> &[f32] {
        &self.matrix
    }

    pub fn is_identity(&self) -> bool {
        let dims = self.dims;
        self.matrix.len() == dims * dims
            && self.matrix.iter().enumerate().all(|(i, v)| {
                let diagonal = dims > 0 && i / dims == i % dims;
                *v == if diagonal { 1.0 } else { 0.0 }
            })
    }

    /// `OPQ::Train(embedding_data, embedding_num, iter_cnt)`.
    ///
    /// `residuals` are the `token - coarse centroid` vectors; `k` is the
    /// residual codebook size per subspace (`1 << residual_pq_subspace_bits`)
    /// and `iter_cnt` the k-means iteration budget the index trains with
    /// (`EMVBIndex::Train(..., 20)`). Returns the rotation the reference would
    /// store in `matrix_R_`.
    pub fn train(
        dims: usize,
        subspace_num: usize,
        k: usize,
        residuals: &[&[f32]],
        iter_cnt: usize,
    ) -> Self {
        if dims == 0
            || subspace_num == 0
            || k < 2
            || !dims.is_multiple_of(subspace_num)
            || residuals.is_empty()
        {
            return Self::identity(dims);
        }
        // The reference trains on a random sample (`GetKMeansCentroids` caps it
        // at `256 * k` points); a fixed stride keeps the build deterministic.
        // The codebook only has to expose the subspace variance while `R` is
        // being learnt — the persistent codebook is retrained at full rate on
        // the rotated residuals afterwards — so the sample and the k-means
        // width are also capped to bound the build cost.
        const MAX_TRAIN_SAMPLE: usize = 2048;
        const MAX_KMEANS_WORK: usize = 3_000_000;
        let stride = residuals.len().div_ceil(MAX_TRAIN_SAMPLE).max(1);
        let sample: Vec<&[f32]> = residuals.iter().step_by(stride).copied().collect();
        if sample.len() < 2 {
            return Self::identity(dims);
        }
        let per_centroid = (sample.len() * dims).max(1);
        let k = k
            .min(sample.len())
            .min((MAX_KMEANS_WORK / per_centroid).max(2))
            .max(2);
        let sample: Vec<Vec<f32>> = sample
            .iter()
            .map(|r| {
                let mut v = r.to_vec();
                v.resize(dims, 0.0);
                v
            })
            .collect();

        // step 1. R = I
        let mut rotation = Self::identity(dims);
        // `loop_short` / `loop_middle` / `loop_long` from `OPQ::Train`
        // (`(iter_cnt + 1) / 2` there is `iter_cnt.div_ceil(2)`).
        let loop_short = 3.min(iter_cnt.div_ceil(2));
        let loop_middle = 3.max(iter_cnt.div_ceil(2));
        let loop_long = iter_cnt;
        let sub_dim = dims / subspace_num;
        let mut pq = ResidualPqModel::new(subspace_num, sub_dim, k.min(sample.len()));
        let mut transformed: Vec<Vec<f32>> = vec![Vec::with_capacity(dims); sample.len()];
        let mut codes = vec![0u8; sample.len() * subspace_num];
        let mut decoded = vec![0.0f32; sample.len() * dims];
        for i in 0..6 {
            let train_iter = if i < 3 {
                loop_short
            } else if i < 5 {
                loop_middle
            } else {
                loop_long
            };
            // transformed = X·R
            for (dst, src) in transformed.iter_mut().zip(&sample) {
                dst.clear();
                dst.extend_from_slice(&mul_row(&rotation.matrix, dims, src));
            }
            pq.train(&transformed, train_iter);
            for (j, x) in transformed.iter().enumerate() {
                pq.encode(x, &mut codes[j * subspace_num..(j + 1) * subspace_num]);
                pq.decode(
                    &codes[j * subspace_num..(j + 1) * subspace_num],
                    &mut decoded[j * dims..(j + 1) * dims],
                );
            }
            // A = X^T · decoded, then A = U·S·V^T and R = U·V^T.
            let mut a = vec![0.0f32; dims * dims];
            for (src, dec) in sample.iter().zip(decoded.chunks_exact(dims)) {
                for row in 0..dims {
                    let xi = src[row];
                    if xi == 0.0 {
                        continue;
                    }
                    for col in 0..dims {
                        a[row * dims + col] += xi * dec[col];
                    }
                }
            }
            let mut u = vec![0.0f32; dims * dims];
            let mut v = vec![0.0f32; dims * dims];
            jacobi_svd(dims, &mut a, &mut u, &mut v);
            // R = U·V^T (either factor's sign is free, and flipping a column of
            // both leaves `U·S·V^T` invariant, hence `U·V^T` too).
            let mut next = vec![0.0f32; dims * dims];
            for row in 0..dims {
                for col in 0..dims {
                    let mut acc = 0.0f32;
                    for j in 0..dims {
                        acc += u[row * dims + j] * v[col * dims + j];
                    }
                    next[row * dims + col] = acc;
                }
            }
            rotation.matrix = next;
        }
        rotation
    }

    /// `x·R`; a row vector times the rotation, as `matrixA_multiply_matrixB_output_to_C`
    /// does for both `AddEmbeddings` and `GetIPDistanceTable`.
    pub fn rotate(&self, v: &[f32]) -> Vec<f32> {
        if self.matrix.len() != self.dims * self.dims {
            return v.to_vec();
        }
        mul_row(&self.matrix, self.dims, v)
    }

    /// `OPQ::Save`'s trailing block: `dim * dim` raw `f32`s.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut out = Vec::with_capacity(self.matrix.len() * 4);
        for v in &self.matrix {
            out.extend_from_slice(&v.to_le_bytes());
        }
        out
    }

    /// `OPQ::Load`'s trailing block.
    pub fn from_bytes(dims: usize, bytes: &[u8]) -> Option<Self> {
        let n = dims.checked_mul(dims)?;
        if bytes.len() < n * 4 {
            return None;
        }
        let matrix = (0..n)
            .map(|i| f32::from_le_bytes(bytes[i * 4..i * 4 + 4].try_into().unwrap()))
            .collect();
        Some(Self { dims, matrix })
    }
}

/// `out = x·R` for a row vector `x`. C++: `matrixA_multiply_matrixB_output_to_C`.
fn mul_row(rotation: &[f32], dims: usize, x: &[f32]) -> Vec<f32> {
    let n = dims.min(x.len());
    let mut out = vec![0.0f32; dims];
    for (col, slot) in out.iter_mut().enumerate() {
        let mut acc = 0.0f32;
        for row in 0..n {
            acc += x[row] * rotation[row * dims + col];
        }
        *slot = acc;
    }
    out
}

/// Residual product quantizer used only while learning `R`.
///
/// C++: the `PQ` base of `OPQ` (`PQ::Train` / `PQ::EncodeEmbedding` /
/// `PQ::DecodeEmbedding`); a nearest-code lookup by inner product, which is the
/// same argmin the reference takes for L2.
struct ResidualPqModel {
    subspace_num: usize,
    sub_dim: usize,
    k: usize,
    /// `subspace_num * k * sub_dim`.
    centroids: Vec<f32>,
}

impl ResidualPqModel {
    fn new(subspace_num: usize, sub_dim: usize, k: usize) -> Self {
        Self {
            subspace_num,
            sub_dim,
            k: k.max(1),
            centroids: vec![0.0f32; subspace_num * k.max(1) * sub_dim],
        }
    }

    fn train(&mut self, data: &[Vec<f32>], iters: usize) {
        let (k, sub_dim) = (self.k, self.sub_dim);
        for sub in 0..self.subspace_num {
            let lo = sub * sub_dim;
            let base = sub * k * sub_dim;
            for c in 0..k {
                let idx = (c * data.len()) / k;
                let src = &data[idx.min(data.len() - 1)][lo..lo + sub_dim];
                self.centroids[base + c * sub_dim..base + (c + 1) * sub_dim].copy_from_slice(src);
            }
            let mut sums = vec![0.0f32; k * sub_dim];
            let mut counts = vec![0usize; k];
            for _ in 0..iters.max(1) {
                sums.fill(0.0);
                counts.fill(0);
                for x in data {
                    let part = &x[lo..lo + sub_dim];
                    let mut best = 0usize;
                    let mut best_d = f32::INFINITY;
                    for c in 0..k {
                        let d: f32 = self.centroids[base + c * sub_dim..base + (c + 1) * sub_dim]
                            .iter()
                            .zip(part)
                            .map(|(a, b)| (a - b) * (a - b))
                            .sum();
                        if d < best_d {
                            best_d = d;
                            best = c;
                        }
                    }
                    counts[best] += 1;
                    for (acc, v) in sums[best * sub_dim..(best + 1) * sub_dim]
                        .iter_mut()
                        .zip(part)
                    {
                        *acc += v;
                    }
                }
                for c in 0..k {
                    if counts[c] == 0 {
                        continue;
                    }
                    let inv = 1.0 / counts[c] as f32;
                    for i in 0..sub_dim {
                        self.centroids[base + c * sub_dim + i] = sums[c * sub_dim + i] * inv;
                    }
                }
            }
        }
    }

    fn encode(&self, x: &[f32], out: &mut [u8]) {
        for (sub, slot) in out.iter_mut().enumerate().take(self.subspace_num) {
            let lo = sub * self.sub_dim;
            let base = sub * self.k * self.sub_dim;
            let mut best = 0usize;
            let mut best_d = f32::INFINITY;
            for c in 0..self.k {
                let d: f32 = self.centroids[base + c * self.sub_dim..base + (c + 1) * self.sub_dim]
                    .iter()
                    .zip(&x[lo..lo + self.sub_dim])
                    .map(|(a, b)| (a - b) * (a - b))
                    .sum();
                if d < best_d {
                    best_d = d;
                    best = c;
                }
            }
            *slot = best as u8;
        }
    }

    fn decode(&self, codes: &[u8], out: &mut [f32]) {
        for (sub, &code) in codes.iter().enumerate().take(self.subspace_num) {
            let lo = sub * self.sub_dim;
            let base = (sub * self.k + code as usize) * self.sub_dim;
            out[lo..lo + self.sub_dim].copy_from_slice(&self.centroids[base..base + self.sub_dim]);
        }
    }
}

/// C++: `Eigen::JacobiSVDSolve` (`third_party/eigen-3.4.0/JacobiSVD.h`), the
/// variant `EMVBSVDSolve` (`eigen_svd_impl.cpp`) calls: a two-sided Jacobi SVD
/// of a square matrix, `A = U·S·V^T`, without sorting the singular values.
/// Negative diagonal entries are folded into the matching column of `U` so the
/// singular values come out non-negative.
fn jacobi_svd(dim: usize, a: &mut [f32], u: &mut [f32], v: &mut [f32]) {
    let precision = 2.0 * f32::EPSILON;
    let consider_as_zero = f32::MIN_POSITIVE;
    let scale = a.iter().fold(0.0f32, |acc, x| acc.max(x.abs()));
    let scale = if scale == 0.0 { 1.0 } else { scale };
    if scale != 1.0 {
        for x in a.iter_mut() {
            *x /= scale;
        }
    }
    u.fill(0.0);
    v.fill(0.0);
    for i in 0..dim {
        u[i * dim + i] = 1.0;
        v[i * dim + i] = 1.0;
    }
    let mut max_diag = (0..dim).fold(0.0f32, |acc, i| acc.max(a[i * dim + i].abs()));
    let mut finished = false;
    while !finished {
        finished = true;
        for p in 1..dim {
            for q in 0..p {
                let threshold = consider_as_zero.max(precision * max_diag);
                if a[p * dim + q].abs() > threshold || a[q * dim + p].abs() > threshold {
                    finished = false;
                    let (j_left, j_right) = real_2x2_jacobi_svd(a, dim, p, q);
                    apply_on_the_left(a, dim, p, q, j_left);
                    apply_on_the_right(u, dim, p, q, j_left.transpose());
                    apply_on_the_right(a, dim, p, q, j_right);
                    apply_on_the_right(v, dim, p, q, j_right);
                    max_diag = max_diag.max(a[p * dim + p].abs()).max(a[q * dim + q].abs());
                }
            }
        }
    }
    // Make the diagonal (the singular values) non-negative by flipping `U`.
    let negative: Vec<usize> = (0..dim).filter(|&i| a[i * dim + i] < 0.0).collect();
    for col in negative {
        for row in 0..dim {
            u[row * dim + col] = -u[row * dim + col];
        }
    }
}

/// C++: `JacobiRotation`.
#[derive(Clone, Copy)]
struct JacobiRotation {
    c: f32,
    s: f32,
}

impl JacobiRotation {
    fn mul(self, other: Self) -> Self {
        Self {
            c: self.c * other.c - self.s * other.s,
            s: self.c * other.s + self.s * other.c,
        }
    }

    fn transpose(self) -> Self {
        Self {
            c: self.c,
            s: -self.s,
        }
    }
}

/// C++: `JacobiRotation::makeJacobi`.
fn make_jacobi(x: f32, y: f32, z: f32) -> JacobiRotation {
    let deno = 2.0 * y.abs();
    if deno < f32::MIN_POSITIVE {
        return JacobiRotation { c: 1.0, s: 0.0 };
    }
    let tau = (x - z) / deno;
    let w = (1.0 + tau * tau).sqrt();
    let t = 1.0 / if tau > 0.0 { tau + w } else { tau - w };
    let n = 1.0 / (1.0 + t * t).sqrt();
    let s = if y.is_sign_negative() { t * n } else { -t * n };
    JacobiRotation { c: n, s }
}

/// C++: `Matrix::applyOnTheLeft` — rotates rows `p` and `q` in place.
fn apply_on_the_left(a: &mut [f32], dim: usize, p: usize, q: usize, j: JacobiRotation) {
    if j.c == 1.0 && j.s == 0.0 {
        return;
    }
    for k in 0..dim {
        let x = a[p * dim + k];
        let y = a[q * dim + k];
        a[p * dim + k] = j.c * x + j.s * y;
        a[q * dim + k] = -j.s * x + j.c * y;
    }
}

/// C++: `Matrix::applyOnTheRight` — rotates columns `p` and `q` in place.
fn apply_on_the_right(a: &mut [f32], dim: usize, p: usize, q: usize, j: JacobiRotation) {
    let jt = j.transpose();
    if jt.c == 1.0 && jt.s == 0.0 {
        return;
    }
    for k in 0..dim {
        let x = a[k * dim + p];
        let y = a[k * dim + q];
        a[k * dim + p] = jt.c * x + jt.s * y;
        a[k * dim + q] = -jt.s * x + jt.c * y;
    }
}

/// C++: `internal::real_2x2_jacobi_svd` (`src/misc/RealSvd2x2.h`).
fn real_2x2_jacobi_svd(
    a: &[f32],
    dim: usize,
    p: usize,
    q: usize,
) -> (JacobiRotation, JacobiRotation) {
    let mut m = [
        a[p * dim + p],
        a[p * dim + q],
        a[q * dim + p],
        a[q * dim + q],
    ];
    let t = m[0] + m[3];
    let d = m[2] - m[1];
    let rot1 = if d.abs() < f32::MIN_POSITIVE {
        JacobiRotation { c: 1.0, s: 0.0 }
    } else {
        let u = t / d;
        let tmp = (1.0 + u * u).sqrt();
        JacobiRotation {
            c: u / tmp,
            s: 1.0 / tmp,
        }
    };
    apply_on_the_left(&mut m, 2, 0, 1, rot1);
    let j_right = make_jacobi(m[0], m[1], m[3]);
    let j_left = rot1.mul(j_right.transpose());
    (j_left, j_right)
}

/// A borrowed EMVB segment.
pub struct EmvbView<'a> {
    pub dims: usize,
    pub subspace_num: usize,
    pub sub_dim: usize,
    pub k: usize,
    /// Coarse centroids, `n_centroids * dims`.
    pub centroids: &'a [f32],
    /// Residual codebook, `subspace_num * k * sub_dim`.
    pub codebook: &'a [f32],
    /// One codeword per (embedding, subspace).
    pub codes: &'a [u8],
    /// Coarse centroid of every embedding.
    pub assignments: &'a [u32],
    pub doc_lens: &'a [u32],
    pub doc_segment_offsets: &'a [u32],
    /// Documents per coarse centroid (the reference's `centroids_to_docid_`).
    pub centroids_to_docid: &'a [Vec<u32>],
    /// `OPQ`'s `matrix_R_` (`dims * dims`, row-major). The residual codebook
    /// lives in the rotated space, so the query is rotated the same way before
    /// the residual inner products; the coarse-centroid scores stay unrotated,
    /// exactly as in `EMVBSearch`. An identity matrix is equivalent to `None`.
    pub rotation: Option<&'a [f32]>,
}

impl EmvbView<'_> {
    fn n_centroids(&self) -> usize {
        if self.dims == 0 {
            0
        } else {
            self.centroids.len() / self.dims
        }
    }

    fn doc_offsets(&self) -> Vec<u32> {
        let mut offsets = Vec::with_capacity(self.doc_lens.len());
        let mut at = 0u32;
        for len in self.doc_lens {
            offsets.push(at);
            at += *len;
        }
        offsets
    }

    /// C++: `EMVBSearch::GetQueryResult`.
    ///
    /// Returns `(segment offset, score)` best-first.
    pub fn search(
        &self,
        query: &[Vec<f32>],
        top_n: usize,
        options: EmvbSearchOptions,
    ) -> Vec<(u32, f32)> {
        let n_centroids = self.n_centroids();
        let n_docs = self.doc_lens.len();
        if top_n == 0 || n_docs == 0 || query.is_empty() || n_centroids == 0 {
            return Vec::new();
        }
        let dims = self.dims;
        let nqt = query.len();
        let doc_offsets = self.doc_offsets();

        // Stage 1a: `q_t · centroid_c` for every query token.
        let mut scores: Vec<Vec<f32>> = Vec::with_capacity(nqt);
        for q in query {
            let mut row = vec![f32::NEG_INFINITY; n_centroids];
            if q.len() == dims {
                for (c, score) in row.iter_mut().enumerate() {
                    let base = c * dims;
                    *score = q
                        .iter()
                        .zip(&self.centroids[base..base + dims])
                        .map(|(a, b)| a * b)
                        .sum();
                }
            }
            scores.push(row);
        }

        // Stage 1b: `find_candidate_docs`.
        let nprobe = options.centroid_nprobe.clamp(1, n_centroids);
        let words = nqt.div_ceil(64);
        let mut masks = vec![vec![0u64; words]; n_centroids];
        let mut selected: Vec<Vec<u32>> = Vec::with_capacity(nqt);
        for (qt, row) in scores.iter().enumerate() {
            let mut filtered: Vec<usize> = (0..n_centroids)
                .filter(|&c| row[c] >= options.threshold_first)
                .collect();
            if filtered.is_empty() {
                // No centroid passes the threshold: take the closest one.
                let mut argmax = 0usize;
                for c in 1..n_centroids {
                    if row[c] > row[argmax] {
                        argmax = c;
                    }
                }
                selected.push(vec![argmax as u32]);
            } else {
                if filtered.len() > nprobe {
                    filtered.sort_by(|&a, &b| {
                        row[b]
                            .partial_cmp(&row[a])
                            .unwrap_or(std::cmp::Ordering::Equal)
                            .then(a.cmp(&b))
                    });
                    filtered.truncate(nprobe);
                }
                selected.push(filtered.into_iter().map(|c| c as u32).collect());
            }
            for &c in selected.last().expect("just pushed") {
                masks[c as usize][qt / 64] |= 1u64 << (qt % 64);
            }
        }

        // Candidate documents: the union of the selected centroids' doc lists.
        let mut seen_centroid = vec![false; n_centroids];
        let mut seen_doc = vec![false; n_docs];
        let mut candidates: Vec<u32> = Vec::new();
        for qsel in &selected {
            for &c in qsel {
                let c = c as usize;
                if seen_centroid[c] {
                    continue;
                }
                seen_centroid[c] = true;
                if let Some(docs) = self.centroids_to_docid.get(c) {
                    for &doc in docs {
                        let doc = doc as usize;
                        if doc < n_docs && !seen_doc[doc] {
                            seen_doc[doc] = true;
                            candidates.push(doc as u32);
                        }
                    }
                }
            }
        }

        // Stage 2: `compute_hit_frequency`.
        let n_doc_to_score = options.n_doc_to_score.max(top_n);
        let scored_docs: Vec<u32> = if candidates.len() <= n_doc_to_score {
            candidates
        } else {
            let mut hits: Vec<(u32, u32)> = candidates
                .iter()
                .map(|&doc| {
                    (
                        self.query_token_hits(doc as usize, &doc_offsets, &masks),
                        doc,
                    )
                })
                .collect();
            hits.sort_by(|a, b| b.0.cmp(&a.0).then(a.1.cmp(&b.1)));
            hits.truncate(n_doc_to_score);
            hits.into_iter().map(|(_, doc)| doc).collect()
        };

        // Stage 3: `second_stage_filtering` — MaxSim over centroid scores.
        let out_second_stage = options.out_second_stage.max(top_n);
        let mut stage2: Vec<(f32, u32)> = scored_docs
            .iter()
            .map(|&doc| {
                (
                    self.centroid_maxsim(doc as usize, &doc_offsets, &scores),
                    doc,
                )
            })
            .collect();
        stage2.sort_by(|a, b| {
            b.0.partial_cmp(&a.0)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.1.cmp(&b.1))
        });
        stage2.truncate(out_second_stage);

        // Stage 4: `compute_topk_documents_selected` — residual-PQ refine.
        let table = self.ip_table(query);
        let mut refined: Vec<(f32, u32)> = stage2
            .iter()
            .map(|&(_, doc)| {
                (
                    self.residual_maxsim(doc as usize, &doc_offsets, &scores, &table, options),
                    doc,
                )
            })
            .collect();
        refined.sort_by(|a, b| {
            b.0.partial_cmp(&a.0)
                .unwrap_or(std::cmp::Ordering::Equal)
                .then(a.1.cmp(&b.1))
        });
        refined.truncate(top_n);
        refined
            .into_iter()
            .map(|(score, doc)| {
                (
                    self.doc_segment_offsets
                        .get(doc as usize)
                        .copied()
                        .unwrap_or(doc),
                    score,
                )
            })
            .collect()
    }

    /// Number of distinct query tokens covered by the centroids of a document's
    /// embeddings (`compute_hit_frequency`'s `mask.count()`).
    fn query_token_hits(&self, doc: usize, offsets: &[u32], masks: &[Vec<u64>]) -> u32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let words = masks.first().map(Vec::len).unwrap_or(0);
        let mut mask = vec![0u64; words];
        for e in start..start + len {
            let Some(&c) = self.assignments.get(e) else {
                break;
            };
            if let Some(m) = masks.get(c as usize) {
                for (acc, w) in mask.iter_mut().zip(m) {
                    *acc |= *w;
                }
            }
        }
        mask.iter().map(|w| w.count_ones()).sum()
    }

    /// `second_stage_filtering`: `sum_qt max_j score[qt][centroid(embedding_j)]`.
    fn centroid_maxsim(&self, doc: usize, offsets: &[u32], scores: &[Vec<f32>]) -> f32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let mut total = 0.0f32;
        for row in scores {
            let mut best = f32::NEG_INFINITY;
            for e in start..start + len {
                let Some(&c) = self.assignments.get(e) else {
                    break;
                };
                if let Some(&s) = row.get(c as usize) {
                    if s > best {
                        best = s;
                    }
                }
            }
            if best != f32::NEG_INFINITY {
                total += best;
            }
        }
        total
    }

    /// `EMVBProductQuantizer::GetIPDistanceTable`: `table[qt][sub][code]`.
    ///
    /// `OPQ::GetIPDistanceTable` rotates the query first (`R^T·q` in column
    /// form, i.e. `q·R` for the row vectors used throughout this module); the
    /// codebook was trained on rotated residuals, so without the rotation the
    /// residual scores are meaningless.
    fn ip_table(&self, query: &[Vec<f32>]) -> Vec<Vec<Vec<f32>>> {
        let sn = self.subspace_num;
        let rotated: Option<Vec<Vec<f32>>> = self
            .rotation
            .filter(|r| r.len() == self.dims * self.dims)
            .map(|r| query.iter().map(|q| mul_row(r, self.dims, q)).collect());
        let query = rotated.as_deref().unwrap_or(query);
        let mut table = Vec::with_capacity(query.len());
        for q in query {
            let mut per_sub = Vec::with_capacity(sn);
            for sub in 0..sn {
                let lo = sub * self.sub_dim;
                let hi = (lo + self.sub_dim).min(self.dims);
                let mut row = vec![0.0f32; self.k];
                if lo < self.dims && q.len() == self.dims {
                    for (code, slot) in row.iter_mut().enumerate() {
                        let base = (sub * self.k + code) * self.sub_dim;
                        *slot = q[lo..hi]
                            .iter()
                            .zip(&self.codebook[base..base + (hi - lo)])
                            .map(|(a, b)| a * b)
                            .sum();
                    }
                }
                per_sub.push(row);
            }
            table.push(per_sub);
        }
        table
    }

    fn pq_ip(&self, table: &[Vec<Vec<f32>>], embedding: usize, qt: usize) -> f32 {
        let mut sum = 0.0f32;
        for sub in 0..self.subspace_num {
            let Some(&code) = self.codes.get(embedding * self.subspace_num + sub) else {
                break;
            };
            if let Some(&v) = table
                .get(qt)
                .and_then(|t| t.get(sub))
                .and_then(|r| r.get(code as usize))
            {
                sum += v;
            }
        }
        sum
    }

    /// `compute_topk_documents_selected`: per query token keep the best
    /// embedding score (centroid IP + residual PQ), then sum over tokens.
    fn residual_maxsim(
        &self,
        doc: usize,
        offsets: &[u32],
        scores: &[Vec<f32>],
        table: &[Vec<Vec<f32>>],
        options: EmvbSearchOptions,
    ) -> f32 {
        let len = self.doc_lens[doc] as usize;
        let start = offsets[doc] as usize;
        let mut total = 0.0f32;
        for (qt, row) in scores.iter().enumerate() {
            let mut eligible = Vec::new();
            let mut best = f32::NEG_INFINITY;
            for e in start..start + len {
                let Some(&c) = self.assignments.get(e) else {
                    break;
                };
                let centroid = row.get(c as usize).copied().unwrap_or(f32::NEG_INFINITY);
                if centroid >= options.threshold_final {
                    eligible.push((e, centroid));
                }
            }
            if eligible.is_empty() {
                // `filter_scores_output_ids` left nothing: exhaustive refine.
                for e in start..start + len {
                    let Some(&c) = self.assignments.get(e) else {
                        break;
                    };
                    let centroid = row.get(c as usize).copied().unwrap_or(0.0);
                    let s = centroid + self.pq_ip(table, e, qt);
                    if s > best {
                        best = s;
                    }
                }
            } else {
                for (e, centroid) in eligible {
                    let s = centroid + self.pq_ip(table, e, qt);
                    if s > best {
                        best = s;
                    }
                }
            }
            if best != f32::NEG_INFINITY {
                total += best;
            }
        }
        total
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A hand-built segment: two coarse centroids, one subspace with two
    /// codewords, so every score can be checked by hand.
    struct Fixture {
        centroids: Vec<f32>,
        codebook: Vec<f32>,
        codes: Vec<u8>,
        assignments: Vec<u32>,
        doc_lens: Vec<u32>,
        doc_segment_offsets: Vec<u32>,
        centroids_to_docid: Vec<Vec<u32>>,
    }

    fn fixture() -> Fixture {
        // dims = 2, subspace_num = 1, sub_dim = 2, k = 2.
        let centroids = vec![1.0, 0.0, 0.0, 1.0];
        // Codeword 0 = (0,0); codeword 1 = (0.5, 0.5).
        let codebook = vec![0.0, 0.0, 0.5, 0.5];
        // doc 0: two embeddings, both near centroid 0, residual codes 1 and 0.
        // doc 1: one embedding near centroid 1, residual code 0.
        let codes = vec![1, 0, 0];
        let assignments = vec![0, 0, 1];
        let doc_lens = vec![2, 1];
        let doc_segment_offsets = vec![0, 1];
        let centroids_to_docid = vec![vec![0], vec![1]];
        Fixture {
            centroids,
            codebook,
            codes,
            assignments,
            doc_lens,
            doc_segment_offsets,
            centroids_to_docid,
        }
    }

    fn view<'a>(f: &'a Fixture) -> EmvbView<'a> {
        EmvbView {
            dims: 2,
            subspace_num: 1,
            sub_dim: 2,
            k: 2,
            centroids: &f.centroids,
            codebook: &f.codebook,
            codes: &f.codes,
            assignments: &f.assignments,
            doc_lens: &f.doc_lens,
            doc_segment_offsets: &f.doc_segment_offsets,
            centroids_to_docid: &f.centroids_to_docid,
            rotation: None,
        }
    }

    #[test]
    fn ranks_the_document_whose_centroid_matches_the_query() {
        let f = fixture();
        let got = view(&f).search(&[vec![1.0, 0.0]], 2, EmvbSearchOptions::for_topk(2));
        assert_eq!(got.len(), 2);
        // Doc 0's embedding 0 sits on centroid 0 with residual code 1, so its
        // score is `1.0 + 0.5 = 1.5`; doc 1 only reaches centroid 1 (0.0).
        assert_eq!(got[0].0, 0);
        assert!((got[0].1 - 1.5).abs() < 1e-6, "score = {}", got[0].1);
        assert_eq!(got[1].0, 1);
    }

    #[test]
    fn empty_query_and_zero_topk_return_nothing() {
        let f = fixture();
        let v = view(&f);
        assert!(v.search(&[], 5, EmvbSearchOptions::default()).is_empty());
        assert!(v
            .search(&[vec![1.0, 0.0]], 0, EmvbSearchOptions::default())
            .is_empty());
    }

    #[test]
    fn threshold_first_falls_back_to_the_closest_centroid() {
        let f = fixture();
        let opts = EmvbSearchOptions {
            threshold_first: 100.0,
            ..EmvbSearchOptions::for_topk(2)
        };
        let got = view(&f).search(&[vec![1.0, 0.0]], 2, opts);
        // Nothing passes the threshold, so the closest centroid is used.
        assert_eq!(got[0].0, 0);
    }

    /// `ip_table` rotates the query into the PQ space before the residual
    /// inner products (`OPQ::matrix_R_`), while the coarse-centroid scores stay
    /// unrotated. The fixture makes that exact: the residual codeword is
    /// `(0, 0.5)` in the rotated space and `R` turns `(1, 0)` into `(0, 1)`, so
    /// the residual term is `0.5` when the query is rotated and `0` when it is
    /// not.
    #[test]
    fn opq_rotation_is_applied_to_the_residual_table() {
        // dims = 2, one subspace with two codewords.
        let centroids = vec![1.0f32, 0.0, 0.0, 1.0];
        let codebook = vec![0.0f32, 0.0, 0.0, 0.5];
        let codes = vec![1u8];
        let assignments = vec![0u32];
        let doc_lens = vec![1u32];
        let doc_segment_offsets = vec![0u32];
        let centroids_to_docid = vec![vec![0u32], vec![]];
        // `mul_row` applies `R^T` (out[col] = sum_row x[row] * R[row][col]),
        // so this quarter turn maps (1,0) to (0,1).
        let rotation: &'static [f32] = &[0.0, 1.0, -1.0, 0.0];
        let view = |rotation: Option<&'static [f32]>| EmvbView {
            dims: 2,
            subspace_num: 1,
            sub_dim: 2,
            k: 2,
            centroids: &centroids,
            codebook: &codebook,
            codes: &codes,
            assignments: &assignments,
            doc_lens: &doc_lens,
            doc_segment_offsets: &doc_segment_offsets,
            centroids_to_docid: &centroids_to_docid,
            rotation,
        };
        let query = vec![vec![1.0f32, 0.0]];
        let options = EmvbSearchOptions {
            centroid_nprobe: 2,
            threshold_first: -1.0,
            n_doc_to_score: 10,
            out_second_stage: 10,
            threshold_final: -1.0,
        };
        let rotated = view(Some(rotation)).search(&query, 1, options);
        assert_eq!(rotated.len(), 1);
        // centroid score 1.0 + residual (0,1)·(0,0.5) = 1.5
        assert!(
            (rotated[0].1 - 1.5).abs() < 1e-6,
            "score with the rotation = {}",
            rotated[0].1
        );
        let unrotated = view(None).search(&query, 1, options);
        // The same codes read with an unrotated query give a residual of 0.
        assert!(
            (unrotated[0].1 - 1.0).abs() < 1e-6,
            "score without the rotation = {}",
            unrotated[0].1
        );
    }

    /// C++: `EMVBTest.test_fakepq` (`test_emvb_ut.cpp`). A zero PQ distance
    /// table (`FakePQ`) reduces the final stage to the coarse-centroid MaxSim,
    /// so the ten documents of centroid 3 tie at `32 * 1.0` and come back in
    /// document order.
    #[test]
    fn fake_pq_ranks_the_matching_centroids_documents() {
        const DIM: usize = 128;
        const CENTROID_NUM: usize = 8;
        const DOCS_IN_ONE_CENTROID: usize = 10;
        const N_DOCS: usize = CENTROID_NUM * DOCS_IN_ONE_CENTROID;
        const EMBEDDING_IN_ONE_DOC: usize = 10;
        const QUERY_TOKEN_NUM: usize = 32;

        let doc_lens = vec![EMBEDDING_IN_ONE_DOC as u32; N_DOCS];
        // `doc_offsets` is implied by `doc_lens` in `EmvbView`.
        let mut assignments = vec![0u32; N_DOCS * EMBEDDING_IN_ONE_DOC];
        for doc in 0..N_DOCS {
            for emb in 0..EMBEDDING_IN_ONE_DOC {
                assignments[doc * EMBEDDING_IN_ONE_DOC + emb] = (doc / DOCS_IN_ONE_CENTROID) as u32;
            }
        }
        let mut centroids = vec![0.0f32; CENTROID_NUM * DIM];
        for c in 0..CENTROID_NUM {
            centroids[c * DIM + c] = 1.0;
        }
        let centroids_to_docid: Vec<Vec<u32>> = (0..CENTROID_NUM)
            .map(|c| {
                (0..DOCS_IN_ONE_CENTROID)
                    .map(|j| (c * DOCS_IN_ONE_CENTROID + j) as u32)
                    .collect()
            })
            .collect();
        let doc_segment_offsets: Vec<u32> = (0..N_DOCS as u32).collect();
        // FakePQ: one subspace whose only codeword is the zero vector, so every
        // residual inner product is zero.
        let codebook = vec![0.0f32; DIM];
        let codes = vec![0u8; N_DOCS * EMBEDDING_IN_ONE_DOC];
        let view = EmvbView {
            dims: DIM,
            subspace_num: 1,
            sub_dim: DIM,
            k: 1,
            centroids: &centroids,
            codebook: &codebook,
            codes: &codes,
            assignments: &assignments,
            doc_lens: &doc_lens,
            doc_segment_offsets: &doc_segment_offsets,
            centroids_to_docid: &centroids_to_docid,
            rotation: None,
        };
        let mut query = vec![vec![0.0f32; DIM]; QUERY_TOKEN_NUM];
        for q in &mut query {
            q[3] = 1.0;
        }
        let options = EmvbSearchOptions {
            centroid_nprobe: 1,
            threshold_first: 0.5,
            n_doc_to_score: 100,
            out_second_stage: 50,
            threshold_final: 0.8,
        };
        let got = view.search(&query, 10, options);
        assert_eq!(got.len(), 10);
        let mut doc_ids: Vec<u32> = got.iter().map(|(doc, _)| *doc).collect();
        doc_ids.sort_unstable();
        for (i, (_, score)) in got.iter().enumerate() {
            assert!((score - 32.0).abs() < 1e-5, "score {i} = {score}");
        }
        assert_eq!(doc_ids, (30..40).collect::<Vec<u32>>());
    }

    // ---- OPQ (`OPQ::Train` / `EMVBSVDSolve`) --------------------------------

    const OPQ_DIMS: usize = 8;
    const OPQ_SUBSPACE_NUM: usize = 2;
    const OPQ_BITS: u32 = 4;

    /// Deterministic pseudo-random in `[-0.5, 0.5)`.
    fn rng(state: &mut u64) -> f32 {
        *state ^= *state << 13;
        *state ^= *state >> 7;
        *state ^= *state << 17;
        ((*state >> 40) as f32) / ((1u64 << 24) as f32) - 0.5
    }

    /// An orthonormal basis: the normalised 8x8 Hadamard matrix.
    fn hadamard8() -> Vec<f32> {
        let mut h = vec![1.0f32; 64];
        let mut step = 1;
        while step < 8 {
            for i in 0..8 {
                for j in 0..8 {
                    if (i / step) % 2 == 1 && (j / step) % 2 == 1 {
                        h[i * 8 + j] = -h[i * 8 + j];
                    }
                }
            }
            step *= 2;
        }
        let norm = (8.0f32).sqrt();
        h.iter().map(|v| v / norm).collect()
    }

    /// Residuals whose covariance is anisotropic in a basis that is *not* the
    /// coordinate axes: `x = H · diag(scale) · z` with decreasing `scale`.
    /// The coordinate split into two 4-dim subspaces therefore cuts across the
    /// principal directions, which is what OPQ is meant to fix.
    fn anisotropic_residuals(n: usize) -> Vec<Vec<f32>> {
        let h = hadamard8();
        let scale = [4.0f32, 2.5, 1.5, 1.0, 0.7, 0.5, 0.35, 0.25];
        let mut state = 0x2545_F491_4F6C_DD1Du64;
        (0..n)
            .map(|_| {
                let z: Vec<f32> = (0..OPQ_DIMS).map(|_| rng(&mut state)).collect();
                (0..OPQ_DIMS)
                    .map(|k| {
                        (0..OPQ_DIMS)
                            .map(|j| h[k * OPQ_DIMS + j] * scale[j] * z[j])
                            .sum()
                    })
                    .collect()
            })
            .collect()
    }

    fn residual_pq(rotation: &OpqRotation, data: &[&[f32]]) -> ResidualPqModel {
        let k = 1usize << OPQ_BITS;
        let mut pq = ResidualPqModel::new(OPQ_SUBSPACE_NUM, OPQ_DIMS / OPQ_SUBSPACE_NUM, k);
        let rotated: Vec<Vec<f32>> = data.iter().map(|x| rotation.rotate(x)).collect();
        pq.train(&rotated, 20);
        pq
    }

    /// Mean squared reconstruction error of the rotated-then-quantised data.
    fn reconstruction_error(rotation: &OpqRotation, data: &[Vec<f32>]) -> f32 {
        let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
        let pq = residual_pq(rotation, &refs);
        let mut code = vec![0u8; OPQ_SUBSPACE_NUM];
        let mut decoded = vec![0.0f32; OPQ_DIMS];
        let mut total = 0.0f64;
        for x in data {
            let rotated = rotation.rotate(x);
            pq.encode(&rotated, &mut code);
            pq.decode(&code, &mut decoded);
            total += rotated
                .iter()
                .zip(&decoded)
                .map(|(a, b)| ((a - b) * (a - b)) as f64)
                .sum::<f64>();
        }
        (total / (data.len() * OPQ_DIMS) as f64) as f32
    }

    #[test]
    fn opq_trains_a_non_identity_rotation_and_beats_the_axis_aligned_baseline() {
        let data = anisotropic_residuals(512);
        let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
        let rotation =
            OpqRotation::train(OPQ_DIMS, OPQ_SUBSPACE_NUM, 1usize << OPQ_BITS, &refs, 20);
        assert!(!rotation.is_identity(), "OPQ left the rotation at identity");
        // The rotation the reference stores is orthogonal.
        for i in 0..OPQ_DIMS {
            for j in 0..OPQ_DIMS {
                let dot: f32 = (0..OPQ_DIMS)
                    .map(|k| {
                        rotation.matrix()[k * OPQ_DIMS + i] * rotation.matrix()[k * OPQ_DIMS + j]
                    })
                    .sum();
                let expected = if i == j { 1.0 } else { 0.0 };
                assert!(
                    (dot - expected).abs() < 1e-3,
                    "R^T R[{i}][{j}] = {dot}, expected {expected}"
                );
            }
        }
        let identity = OpqRotation::identity(OPQ_DIMS);
        let err_identity = reconstruction_error(&identity, &data);
        let err_rotated = reconstruction_error(&rotation, &data);
        assert!(
            err_rotated < 0.85 * err_identity,
            "OPQ error {err_rotated} should be clearly below identity error {err_identity}"
        );
    }

    /// The ported two-sided Jacobi SVD must satisfy `A = U·S·V^T` with
    /// non-negative `S`, which is what `EMVBSVDSolve` is used for.
    #[test]
    fn jacobi_svd_reconstructs_the_matrix() {
        let dim = 5;
        let mut a: Vec<f32> = (0..dim * dim)
            .map(|i| {
                let (r, c) = (i / dim, i % dim);
                // Non-symmetric, well conditioned.
                (r as f32 + 1.0) * 0.3 + (c as f32 + 1.0).powi(2) * 0.7
                    - if r == c { 3.0 } else { 0.0 }
            })
            .collect();
        let original = a.clone();
        let mut u = vec![0.0f32; dim * dim];
        let mut v = vec![0.0f32; dim * dim];
        jacobi_svd(dim, &mut a, &mut u, &mut v);
        // S = U^T A V must be diagonal and non-negative.
        let mut s = vec![0.0f32; dim * dim];
        for i in 0..dim {
            for j in 0..dim {
                let mut acc = 0.0f32;
                for r in 0..dim {
                    for c in 0..dim {
                        acc += u[r * dim + i] * original[r * dim + c] * v[c * dim + j];
                    }
                }
                s[i * dim + j] = acc;
            }
        }
        for i in 0..dim {
            assert!(s[i * dim + i] >= -1e-4, "sigma[{i}] = {}", s[i * dim + i]);
            for j in 0..dim {
                if i != j {
                    assert!(
                        s[i * dim + j].abs() < 1e-3,
                        "S[{i}][{j}] = {}",
                        s[i * dim + j]
                    );
                }
            }
        }
        // A = U·S·V^T
        for r in 0..dim {
            for c in 0..dim {
                let mut acc = 0.0f32;
                for i in 0..dim {
                    for j in 0..dim {
                        acc += u[r * dim + i] * s[i * dim + j] * v[c * dim + j];
                    }
                }
                assert!(
                    (acc - original[r * dim + c]).abs() < 1e-3,
                    "A[{r}][{c}] = {} vs reconstruction {acc}",
                    original[r * dim + c]
                );
            }
        }
    }

    #[test]
    fn opq_rotation_survives_save_and_load() {
        let data = anisotropic_residuals(256);
        let refs: Vec<&[f32]> = data.iter().map(|v| v.as_slice()).collect();
        let rotation =
            OpqRotation::train(OPQ_DIMS, OPQ_SUBSPACE_NUM, 1usize << OPQ_BITS, &refs, 20);
        // `OPQ::Save` / `OPQ::Load`: dim*dim raw f32s.
        let bytes = rotation.to_bytes();
        assert_eq!(bytes.len(), OPQ_DIMS * OPQ_DIMS * 4);
        let loaded = OpqRotation::from_bytes(OPQ_DIMS, &bytes).expect("rotation block");
        assert_eq!(loaded, rotation);
        for x in data.iter().take(8) {
            assert_eq!(loaded.rotate(x), rotation.rotate(x));
        }
    }

    /// The residual-PQ block of an EMVB chunk (`PQ::Save` / `OPQ::Save`):
    /// per-subspace codebook, `-0.5*||c||` table, the codes, `next_embedding_id`
    /// and the rotation. Only the parts OPQ touches are modelled here.
    struct MiniChunk {
        subspace_num: usize,
        codebook: Vec<f32>,
        norms: Vec<f32>,
        codes: Vec<u8>,
        rotations: Vec<u8>,
    }

    impl MiniChunk {
        fn encode(
            dims: usize,
            subspace_num: usize,
            bits: u32,
            docs: &[Vec<Vec<f32>>],
            r: &OpqRotation,
        ) -> Self {
            let k = 1usize << bits;
            let sub_dim = dims / subspace_num;
            let tokens: Vec<&[f32]> = docs.iter().flatten().map(|t| t.as_slice()).collect();
            let pq = residual_pq(r, &tokens);
            let rotated: Vec<Vec<f32>> = tokens.iter().map(|t| r.rotate(t)).collect();
            let mut norms = Vec::with_capacity(subspace_num * k);
            for sub in 0..subspace_num {
                for c in 0..k {
                    let base = (sub * k + c) * sub_dim;
                    let norm: f32 = pq.centroids[base..base + sub_dim]
                        .iter()
                        .map(|x| x * x)
                        .sum::<f32>()
                        .sqrt();
                    norms.push(-0.5 * norm);
                }
            }
            let mut codes = vec![0u8; tokens.len() * subspace_num];
            for (i, x) in rotated.iter().enumerate() {
                pq.encode(x, &mut codes[i * subspace_num..(i + 1) * subspace_num]);
            }
            Self {
                subspace_num,
                codebook: pq.centroids,
                norms,
                codes,
                rotations: r.to_bytes(),
            }
        }

        fn to_bytes(&self) -> Vec<u8> {
            let mut out = Vec::new();
            for v in self.codebook.iter().chain(&self.norms) {
                out.extend_from_slice(&v.to_le_bytes());
            }
            let n = self.codes.len() / self.subspace_num;
            out.extend_from_slice(&(n as u32).to_le_bytes());
            out.extend_from_slice(&self.codes);
            out.extend_from_slice(&(n as u32).to_le_bytes());
            out.extend_from_slice(&self.rotations);
            out
        }

        fn decode(
            dims: usize,
            subspace_num: usize,
            bits: u32,
            bytes: &[u8],
        ) -> Option<(Vec<f32>, Vec<u8>, OpqRotation)> {
            let k = 1usize << bits;
            let sub_dim = dims / subspace_num;
            let cb_len = subspace_num * k * sub_dim;
            let mut at = 0usize;
            let read = |at: &mut usize, n: usize| -> Option<Vec<f32>> {
                if bytes.len() < *at + n * 4 {
                    return None;
                }
                let v = (0..n)
                    .map(|i| {
                        f32::from_le_bytes(bytes[*at + i * 4..*at + i * 4 + 4].try_into().unwrap())
                    })
                    .collect();
                *at += n * 4;
                Some(v)
            };
            let codebook = read(&mut at, cb_len)?;
            let _norms = read(&mut at, subspace_num * k)?;
            if bytes.len() < at + 4 {
                return None;
            }
            let n = u32::from_le_bytes(bytes[at..at + 4].try_into().unwrap()) as usize;
            at += 4;
            if bytes.len() < at + n * subspace_num + 4 {
                return None;
            }
            let codes = bytes[at..at + n * subspace_num].to_vec();
            at += n * subspace_num + 4;
            let rotation = OpqRotation::from_bytes(dims, &bytes[at..])?;
            Some((codebook, codes, rotation))
        }
    }

    /// Splits the documents of the mini chunk into the `EmvbView` arrays. One
    /// coarse centroid at the origin keeps every stage below the residual
    /// refinements (which is the only part OPQ changes).
    struct MiniSegment {
        dims: usize,
        subspace_num: usize,
        sub_dim: usize,
        k: usize,
        centroids: Vec<f32>,
        codebook: Vec<f32>,
        codes: Vec<u8>,
        assignments: Vec<u32>,
        doc_lens: Vec<u32>,
        doc_segment_offsets: Vec<u32>,
        centroids_to_docid: Vec<Vec<u32>>,
        rotation: Vec<f32>,
    }

    impl MiniSegment {
        fn new(docs: &[Vec<Vec<f32>>], r: &OpqRotation) -> Self {
            let chunk = MiniChunk::encode(OPQ_DIMS, OPQ_SUBSPACE_NUM, OPQ_BITS, docs, r);
            let bytes = chunk.to_bytes();
            let (codebook, codes, rotation) =
                MiniChunk::decode(OPQ_DIMS, OPQ_SUBSPACE_NUM, OPQ_BITS, &bytes).expect("PQ block");
            let n_embeddings = docs.iter().map(|d| d.len()).sum::<usize>();
            Self {
                dims: OPQ_DIMS,
                subspace_num: OPQ_SUBSPACE_NUM,
                sub_dim: OPQ_DIMS / OPQ_SUBSPACE_NUM,
                k: 1usize << OPQ_BITS,
                centroids: vec![0.0f32; OPQ_DIMS],
                codebook,
                codes,
                assignments: vec![0u32; n_embeddings],
                doc_lens: docs.iter().map(|d| d.len() as u32).collect(),
                doc_segment_offsets: (0..docs.len() as u32).collect(),
                centroids_to_docid: vec![(0..docs.len() as u32).collect()],
                rotation: rotation.matrix().to_vec(),
            }
        }

        fn view(&self) -> EmvbView<'_> {
            EmvbView {
                dims: self.dims,
                subspace_num: self.subspace_num,
                sub_dim: self.sub_dim,
                k: self.k,
                centroids: &self.centroids,
                codebook: &self.codebook,
                codes: &self.codes,
                assignments: &self.assignments,
                doc_lens: &self.doc_lens,
                doc_segment_offsets: &self.doc_segment_offsets,
                centroids_to_docid: &self.centroids_to_docid,
                rotation: Some(&self.rotation),
            }
        }
    }

    /// Exact MaxSim over the raw tokens (`EMVBIndex`'s ground truth).
    fn exact_maxsim(docs: &[Vec<Vec<f32>>], query: &[Vec<f32>]) -> usize {
        let mut best = (f32::NEG_INFINITY, 0usize);
        for (doc, tokens) in docs.iter().enumerate() {
            let mut total = 0.0f32;
            for q in query {
                let best_token = tokens
                    .iter()
                    .map(|t| t.iter().zip(q).map(|(a, b)| a * b).sum::<f32>())
                    .fold(f32::NEG_INFINITY, f32::max);
                total += best_token;
            }
            if total > best.0 {
                best = (total, doc);
            }
        }
        best.1
    }

    #[test]
    fn opq_chunk_reranks_the_same_document_as_exact_maxsim() {
        let docs: Vec<Vec<Vec<f32>>> = anisotropic_residuals(12)
            .chunks(4)
            .map(|doc| doc.to_vec())
            .collect();
        let train: Vec<Vec<f32>> = docs.iter().flatten().cloned().collect();
        let refs: Vec<&[f32]> = train.iter().map(|v| v.as_slice()).collect();
        let rotation =
            OpqRotation::train(OPQ_DIMS, OPQ_SUBSPACE_NUM, 1usize << OPQ_BITS, &refs, 20);
        assert!(!rotation.is_identity());

        let segment = MiniSegment::new(&docs, &rotation);
        // A query token taken from document 2, so the exact path must pick it.
        let query = vec![docs[2][1].clone()];
        let expected = exact_maxsim(&docs, &query);
        assert_eq!(expected, 2);

        let got = segment
            .view()
            .search(&query, 3, EmvbSearchOptions::for_topk(3));
        assert!(!got.is_empty());
        assert_eq!(
            got[0].0 as usize,
            expected,
            "OPQ chunk ranked {:?}, exact MaxSim winner is {expected}",
            got.iter().map(|(d, s)| (*d, *s)).collect::<Vec<_>>()
        );
        // The top score tracks the exact inner product (the query is not
        // rotated with `R^-1`, so the comparison is only approximate).
        assert!(got[0].1 > 0.0, "score {} not positive", got[0].1);
    }

    #[test]
    fn opq_chunk_search_survives_a_save_load_round_trip() {
        let docs: Vec<Vec<Vec<f32>>> = anisotropic_residuals(12)
            .chunks(4)
            .map(|doc| doc.to_vec())
            .collect();
        let train: Vec<Vec<f32>> = docs.iter().flatten().cloned().collect();
        let refs: Vec<&[f32]> = train.iter().map(|v| v.as_slice()).collect();
        let rotation =
            OpqRotation::train(OPQ_DIMS, OPQ_SUBSPACE_NUM, 1usize << OPQ_BITS, &refs, 20);
        let segment = MiniSegment::new(&docs, &rotation);
        let query = vec![docs[1][0].clone()];
        let before = segment
            .view()
            .search(&query, 4, EmvbSearchOptions::for_topk(4));

        // Re-read the PQ block including `matrix_R_` from its bytes.
        let chunk = MiniChunk::encode(OPQ_DIMS, OPQ_SUBSPACE_NUM, OPQ_BITS, &docs, &rotation);
        let bytes = chunk.to_bytes();
        let (_, _, loaded) =
            MiniChunk::decode(OPQ_DIMS, OPQ_SUBSPACE_NUM, OPQ_BITS, &bytes).expect("PQ block");
        assert_eq!(loaded, rotation);
        let reloaded = MiniSegment {
            rotation: loaded.matrix().to_vec(),
            ..segment
        };
        let after = reloaded
            .view()
            .search(&query, 4, EmvbSearchOptions::for_topk(4));
        assert_eq!(
            before.iter().map(|(d, _)| *d).collect::<Vec<u32>>(),
            after.iter().map(|(d, _)| *d).collect::<Vec<u32>>()
        );
        for (a, b) in before.iter().zip(&after) {
            assert!((a.1 - b.1).abs() < 1e-6, "score drift {} -> {}", a.1, b.1);
        }
    }

    /// A chunk encoded in the rotated space must not be scoreable without the
    /// rotation: this is the audit's "a C++-written chunk cannot be scored with
    /// its rotation" failure mode.
    #[test]
    fn dropping_the_rotation_degrades_the_residual_scores() {
        let docs: Vec<Vec<Vec<f32>>> = anisotropic_residuals(12)
            .chunks(4)
            .map(|doc| doc.to_vec())
            .collect();
        let train: Vec<Vec<f32>> = docs.iter().flatten().cloned().collect();
        let refs: Vec<&[f32]> = train.iter().map(|v| v.as_slice()).collect();
        let rotation =
            OpqRotation::train(OPQ_DIMS, OPQ_SUBSPACE_NUM, 1usize << OPQ_BITS, &refs, 20);
        let segment = MiniSegment::new(&docs, &rotation);
        let query = vec![docs[2][1].clone()];
        let rotated_scores = segment
            .view()
            .search(&query, 3, EmvbSearchOptions::for_topk(3));
        let unrotated = MiniSegment {
            rotation: OpqRotation::identity(OPQ_DIMS).matrix().to_vec(),
            ..segment
        };
        let identity_scores = unrotated
            .view()
            .search(&query, 3, EmvbSearchOptions::for_topk(3));
        assert!(
            rotated_scores[0].1 > identity_scores[0].1,
            "rotating the query should score the matching document higher: {} vs {}",
            rotated_scores[0].1,
            identity_scores[0].1
        );
    }
}

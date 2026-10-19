//! Streaming merge of PLAID chunks.
//!
//! C++: `PlaidIndexDiskMerger` and `PlaidMergeManifest`
//! (`src/storage/knn_index/plaid/plaid_index_disk_merger.cppm` and
//! `_impl.cpp`). The reference loads chunk buffers through the buffer manager
//! (`BufferObj`); here the caller passes decoded [`PlaidIndex`] chunks, which is
//! the same boundary the executor's other chunk loaders use.

use std::fmt::Write as _;
use std::path::Path;

use infinity_common::{Error, Result};

use super::index::PlaidIndex;

/// C++: `PlaidMergeManifestChunk`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct PlaidMergeManifestChunk {
    pub chunk_id: u32,
    pub doc_count: u32,
    pub embedding_count: u64,
    pub file_size: u64,
    pub last_modified: u64,
    pub checksum: String,
}

/// C++: `PlaidMergeManifest`.
#[derive(Debug, Clone, PartialEq)]
pub struct PlaidMergeManifest {
    pub version: u32,
    pub total_docs: u32,
    pub total_embeddings: u64,
    pub embedding_dimension: u32,
    pub nbits: u32,
    pub n_centroids: u32,
    pub chunks: Vec<PlaidMergeManifestChunk>,
    pub last_merge_time: u64,
}

impl Default for PlaidMergeManifest {
    fn default() -> Self {
        Self {
            version: 1,
            total_docs: 0,
            total_embeddings: 0,
            embedding_dimension: 0,
            nbits: 0,
            n_centroids: 0,
            chunks: Vec::new(),
            last_merge_time: 0,
        }
    }
}

impl PlaidMergeManifest {
    /// C++: `PlaidMergeManifest::NeedsMerge`.
    pub fn needs_merge(&self, new_chunk: &PlaidMergeManifestChunk) -> bool {
        for existing in &self.chunks {
            if existing.chunk_id == new_chunk.chunk_id {
                return existing.doc_count != new_chunk.doc_count
                    || existing.embedding_count != new_chunk.embedding_count
                    || existing.file_size != new_chunk.file_size
                    || existing.last_modified != new_chunk.last_modified
                    || existing.checksum != new_chunk.checksum;
            }
        }
        true
    }

    /// C++: `PlaidMergeManifest::UpdateChunk`.
    pub fn update_chunk(&mut self, chunk: PlaidMergeManifestChunk) {
        if let Some(existing) = self
            .chunks
            .iter_mut()
            .find(|c| c.chunk_id == chunk.chunk_id)
        {
            *existing = chunk;
            return;
        }
        self.chunks.push(chunk);
    }

    /// C++: `PlaidMergeManifest::ToJson`.
    pub fn to_json(&self) -> String {
        let mut out = String::new();
        let _ = write!(
            out,
            "{{\n  \"version\": {},\n  \"total_docs\": {},\n  \"total_embeddings\": {},\n  \"embedding_dimension\": {},\n  \"nbits\": {},\n  \"n_centroids\": {},\n  \"last_merge_time\": {},\n  \"chunks\": [",
            self.version,
            self.total_docs,
            self.total_embeddings,
            self.embedding_dimension,
            self.nbits,
            self.n_centroids,
            self.last_merge_time
        );
        for (i, chunk) in self.chunks.iter().enumerate() {
            if i > 0 {
                out.push(',');
            }
            let _ = write!(
                out,
                "\n    {{\n      \"chunk_id\": {},\n      \"doc_count\": {},\n      \"embedding_count\": {},\n      \"file_size\": {},\n      \"last_modified\": {},\n      \"checksum\": \"{}\"\n    }}",
                chunk.chunk_id,
                chunk.doc_count,
                chunk.embedding_count,
                chunk.file_size,
                chunk.last_modified,
                json_escape(&chunk.checksum)
            );
        }
        out.push_str("\n  ]\n}\n");
        out
    }

    /// C++: `PlaidMergeManifest::FromJson`.
    pub fn from_json(text: &str) -> Result<Self> {
        let json = Json::parse(text)?;
        let mut manifest = Self {
            version: json.get_u32("version", 1),
            total_docs: json.get_u32("total_docs", 0),
            total_embeddings: json.get_u64("total_embeddings", 0),
            embedding_dimension: json.get_u32("embedding_dimension", 0),
            nbits: json.get_u32("nbits", 0),
            n_centroids: json.get_u32("n_centroids", 0),
            chunks: Vec::new(),
            last_merge_time: json.get_u64("last_merge_time", 0),
        };
        if let Some(chunks) = json.get("chunks").and_then(Json::as_array) {
            for chunk in chunks {
                manifest.chunks.push(PlaidMergeManifestChunk {
                    chunk_id: chunk.get_u32("chunk_id", 0),
                    doc_count: chunk.get_u32("doc_count", 0),
                    embedding_count: chunk.get_u64("embedding_count", 0),
                    file_size: chunk.get_u64("file_size", 0),
                    last_modified: chunk.get_u64("last_modified", 0),
                    checksum: chunk
                        .get("checksum")
                        .and_then(Json::as_str)
                        .unwrap_or("")
                        .to_string(),
                });
            }
        }
        Ok(manifest)
    }
}

/// C++: `PlaidMergeSizeInfo`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct PlaidMergeSizeInfo {
    pub total_docs: u32,
    pub total_embeddings: u64,
    pub packed_residuals_size: usize,
    pub ivf_total_entries: usize,
    pub estimated_file_size: usize,
}

/// C++: `PlaidIndexDiskMerger`.
pub struct PlaidIndexDiskMerger {
    temp_dir: String,
    output_path: String,
    n_centroids: u32,
    embedding_dim: u32,
    nbits: u32,
    doc_lens: Vec<u32>,
    doc_offsets: Vec<u32>,
    centroid_ids: Vec<u32>,
    packed_residuals: Vec<u8>,
    ivf_lists: Vec<Vec<u32>>,
    current_doc_count: u32,
    current_embedding_count: u64,
    initialized: bool,
    progress: MergeProgress,
}

/// C++: `PlaidIndexDiskMerger::MergeProgress`.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct MergeProgress {
    pub chunks_merged: u32,
    pub total_chunks: u32,
    pub bytes_written: u64,
    pub total_bytes: u64,
}

impl PlaidIndexDiskMerger {
    /// C++: `PlaidIndexDiskMerger::PlaidIndexDiskMerger`.
    pub fn new(
        temp_dir: &str,
        output_path: &str,
        n_centroids: u32,
        embedding_dim: u32,
        nbits: u32,
    ) -> Self {
        Self {
            temp_dir: temp_dir.to_string(),
            output_path: output_path.to_string(),
            n_centroids,
            embedding_dim,
            nbits,
            doc_lens: Vec::new(),
            doc_offsets: Vec::new(),
            centroid_ids: Vec::new(),
            packed_residuals: Vec::new(),
            ivf_lists: vec![Vec::new(); n_centroids as usize],
            current_doc_count: 0,
            current_embedding_count: 0,
            initialized: false,
            progress: MergeProgress::default(),
        }
    }

    pub fn temp_dir(&self) -> &str {
        &self.temp_dir
    }
    pub fn output_path(&self) -> &str {
        &self.output_path
    }

    /// C++: `PlaidIndexDiskMerger::Initialize`.
    pub fn initialize(&mut self, size_info: &PlaidMergeSizeInfo) {
        if self.initialized {
            return;
        }
        self.doc_lens.reserve(size_info.total_docs as usize);
        self.doc_offsets.reserve(size_info.total_docs as usize);
        self.centroid_ids
            .reserve(size_info.total_embeddings as usize);
        self.packed_residuals
            .reserve(size_info.packed_residuals_size);
        self.initialized = true;
        self.progress.total_chunks = 0;
        self.progress.total_bytes = size_info.estimated_file_size as u64;
    }

    /// C++: `PlaidIndexDiskMerger::MergeChunk`.
    pub fn merge_chunk(&mut self, chunk: &PlaidIndex, doc_offset: u32) -> Result<()> {
        let _ = doc_offset;
        if !self.initialized {
            return Err(Error::Invalid(
                "PlaidIndexDiskMerger not initialized".into(),
            ));
        }
        if chunk.doc_num() == 0 {
            return Ok(());
        }
        // The chunk must use this merger's quantization width (ColBERTSaR
        // chunks carry no residual codes).
        let packed_dim = (self.embedding_dim * self.nbits).div_ceil(8) as usize;
        let expected = chunk.total_embedding_num() as usize * packed_dim;
        if !chunk.is_colbertsar_mode() && chunk.packed_residuals().len() != expected {
            return Err(Error::Invalid(format!(
                "PlaidIndexDiskMerger::MergeChunk: packed residuals {} != expected {expected}",
                chunk.packed_residuals().len()
            )));
        }
        let current = self.centroid_ids.len() as u32;
        for (i, &len) in chunk.doc_lens().iter().enumerate() {
            self.doc_lens.push(len);
            self.doc_offsets.push(current + chunk.doc_offsets()[i]);
        }
        self.centroid_ids.extend_from_slice(chunk.centroid_ids());
        self.packed_residuals
            .extend_from_slice(chunk.packed_residuals());
        for i in 0..chunk.doc_num() {
            let global = self.current_doc_count + i;
            let len = chunk.doc_lens()[i as usize] as usize;
            let at = chunk.doc_offsets()[i as usize] as usize;
            for j in 0..len {
                let cid = chunk.centroid_ids()[at + j];
                if cid as usize >= self.ivf_lists.len() {
                    continue;
                }
                let list = &mut self.ivf_lists[cid as usize];
                if list.last() != Some(&global) {
                    list.push(global);
                }
            }
        }
        self.current_doc_count += chunk.doc_num();
        self.current_embedding_count += chunk.total_embedding_num() as u64;
        self.progress.chunks_merged += 1;
        self.progress.bytes_written += chunk.packed_residuals().len() as u64
            + (chunk.doc_num() as u64 * 2 + chunk.total_embedding_num() as u64) * 4;
        Ok(())
    }

    /// C++: `PlaidIndexDiskMerger::MergeChunksParallel` (the reference falls
    /// back to a sequential merge here as well).
    pub fn merge_chunks_parallel(
        &mut self,
        chunks_with_offsets: &[(u32, &PlaidIndex)],
        _num_threads: u32,
    ) -> Result<()> {
        for &(offset, chunk) in chunks_with_offsets {
            self.merge_chunk(chunk, offset)?;
        }
        Ok(())
    }

    /// C++: `PlaidIndexDiskMerger::MergeIncremental`.
    pub fn merge_incremental(
        &mut self,
        manifest: &PlaidMergeManifest,
        new_chunks: &[(PlaidMergeManifestChunk, &PlaidIndex)],
    ) -> Result<()> {
        for (meta, chunk) in new_chunks {
            if manifest.needs_merge(meta) {
                self.merge_chunk(chunk, meta.chunk_id)?;
            }
        }
        Ok(())
    }

    /// C++: `PlaidIndexDiskMerger::Finalize`.
    pub fn finalize(&mut self, output: &mut PlaidIndex) -> Result<()> {
        if !self.initialized {
            return Err(Error::Invalid(
                "PlaidIndexDiskMerger not initialized".into(),
            ));
        }
        let packed = std::mem::take(&mut self.packed_residuals);
        let ivf = std::mem::take(&mut self.ivf_lists);
        output.accept_merged_data(
            std::mem::take(&mut self.doc_lens),
            std::mem::take(&mut self.doc_offsets),
            std::mem::take(&mut self.centroid_ids),
            packed,
            ivf,
            self.current_doc_count,
            self.current_embedding_count as u32,
        )?;
        self.current_doc_count = 0;
        self.current_embedding_count = 0;
        self.initialized = false;
        self.ivf_lists = vec![Vec::new(); self.n_centroids as usize];
        Ok(())
    }

    /// C++: `PlaidIndexDiskMerger::Cancel`.
    pub fn cancel(&mut self) {
        self.doc_lens.clear();
        self.doc_offsets.clear();
        self.centroid_ids.clear();
        self.packed_residuals.clear();
        for list in &mut self.ivf_lists {
            list.clear();
        }
        self.current_doc_count = 0;
        self.current_embedding_count = 0;
        self.initialized = false;
    }

    /// C++: `PlaidIndexDiskMerger::GetProgress`.
    pub fn progress(&self) -> &MergeProgress {
        &self.progress
    }

    /// C++: `PlaidIndexDiskMerger::LoadManifest`.
    pub fn load_manifest(path: &str) -> Result<PlaidMergeManifest> {
        match std::fs::read_to_string(path) {
            Ok(text) => PlaidMergeManifest::from_json(&text),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(PlaidMergeManifest::default()),
            Err(e) => Err(Error::Invalid(format!("merge manifest read: {e}"))),
        }
    }

    /// C++: `PlaidIndexDiskMerger::SaveManifest`.
    pub fn save_manifest(path: &str, manifest: &PlaidMergeManifest) -> Result<()> {
        if let Some(dir) = Path::new(path).parent() {
            std::fs::create_dir_all(dir)
                .map_err(|e| Error::Invalid(format!("merge manifest dir: {e}")))?;
        }
        std::fs::write(path, manifest.to_json())
            .map_err(|e| Error::Invalid(format!("merge manifest write: {e}")))
    }

    /// C++: `PlaidIndexDiskMerger::CalculateMergeSize`.
    pub fn calculate_merge_size(
        chunks: &[&PlaidIndex],
        embedding_dim: u32,
        nbits: u32,
    ) -> PlaidMergeSizeInfo {
        let mut info = PlaidMergeSizeInfo::default();
        for chunk in chunks {
            info.total_docs += chunk.doc_num();
            info.total_embeddings += chunk.total_embedding_num() as u64;
            info.ivf_total_entries += chunk.doc_lens().iter().map(|&l| l as usize).sum::<usize>();
        }
        let packed_dim = (embedding_dim * nbits).div_ceil(8) as usize;
        info.packed_residuals_size = info.total_embeddings as usize * packed_dim;
        info.estimated_file_size = 8
            + 4 * 10
            + info.total_docs as usize * 4 * 2
            + info.total_embeddings as usize * 4
            + info.packed_residuals_size
            + info.ivf_total_entries * 4
            + 1024;
        info
    }
}

fn json_escape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            c => out.push(c),
        }
    }
    out
}

/// Minimal JSON reader for the manifest (no serde dependency in this crate).
#[derive(Debug, Clone, PartialEq)]
enum Json {
    Null,
    Bool(bool),
    Number(f64),
    Str(String),
    Array(Vec<Json>),
    Object(Vec<(String, Json)>),
}

impl Json {
    fn parse(text: &str) -> Result<Self> {
        let mut parser = JsonParser {
            bytes: text.as_bytes(),
            at: 0,
        };
        let value = parser.value()?;
        parser.skip_ws();
        if parser.at != parser.bytes.len() {
            return Err(Error::Invalid("merge manifest: trailing input".into()));
        }
        Ok(value)
    }
    fn get(&self, key: &str) -> Option<&Json> {
        self.as_object()?
            .iter()
            .find(|(k, _)| k == key)
            .map(|(_, v)| v)
    }
    fn get_u32(&self, key: &str, default: u32) -> u32 {
        self.get(key)
            .and_then(Json::as_f64)
            .map(|v| v as u32)
            .unwrap_or(default)
    }
    fn get_u64(&self, key: &str, default: u64) -> u64 {
        self.get(key)
            .and_then(Json::as_f64)
            .map(|v| v as u64)
            .unwrap_or(default)
    }
    fn as_object(&self) -> Option<&[(String, Json)]> {
        match self {
            Json::Object(o) => Some(o),
            _ => None,
        }
    }
    fn as_array(&self) -> Option<&[Json]> {
        match self {
            Json::Array(a) => Some(a),
            _ => None,
        }
    }
    fn as_str(&self) -> Option<&str> {
        match self {
            Json::Str(s) => Some(s),
            _ => None,
        }
    }
    fn as_f64(&self) -> Option<f64> {
        match self {
            Json::Number(n) => Some(*n),
            _ => None,
        }
    }
}

struct JsonParser<'a> {
    bytes: &'a [u8],
    at: usize,
}

impl JsonParser<'_> {
    fn skip_ws(&mut self) {
        while matches!(self.bytes.get(self.at), Some(b' ' | b'\t' | b'\n' | b'\r')) {
            self.at += 1;
        }
    }
    fn value(&mut self) -> Result<Json> {
        self.skip_ws();
        match self.bytes.get(self.at) {
            Some(b'{') => self.object(),
            Some(b'[') => self.array(),
            Some(b'"') => Ok(Json::Str(self.string()?)),
            Some(b't') => self.literal("true", Json::Bool(true)),
            Some(b'f') => self.literal("false", Json::Bool(false)),
            Some(b'n') => self.literal("null", Json::Null),
            Some(_) => self.number(),
            None => Err(Error::Invalid("merge manifest: unexpected end".into())),
        }
    }
    fn literal(&mut self, text: &str, value: Json) -> Result<Json> {
        if self.bytes[self.at..].starts_with(text.as_bytes()) {
            self.at += text.len();
            Ok(value)
        } else {
            Err(Error::Invalid("merge manifest: bad literal".into()))
        }
    }
    fn object(&mut self) -> Result<Json> {
        self.at += 1; // {
        let mut entries = Vec::new();
        self.skip_ws();
        if self.bytes.get(self.at) == Some(&b'}') {
            self.at += 1;
            return Ok(Json::Object(entries));
        }
        loop {
            self.skip_ws();
            let key = self.string()?;
            self.skip_ws();
            if self.bytes.get(self.at) != Some(&b':') {
                return Err(Error::Invalid("merge manifest: expected ':'".into()));
            }
            self.at += 1;
            let value = self.value()?;
            entries.push((key, value));
            self.skip_ws();
            match self.bytes.get(self.at) {
                Some(b',') => self.at += 1,
                Some(b'}') => {
                    self.at += 1;
                    return Ok(Json::Object(entries));
                }
                _ => return Err(Error::Invalid("merge manifest: expected ',' or '}'".into())),
            }
        }
    }
    fn array(&mut self) -> Result<Json> {
        self.at += 1; // [
        let mut items = Vec::new();
        self.skip_ws();
        if self.bytes.get(self.at) == Some(&b']') {
            self.at += 1;
            return Ok(Json::Array(items));
        }
        loop {
            items.push(self.value()?);
            self.skip_ws();
            match self.bytes.get(self.at) {
                Some(b',') => self.at += 1,
                Some(b']') => {
                    self.at += 1;
                    return Ok(Json::Array(items));
                }
                _ => return Err(Error::Invalid("merge manifest: expected ',' or ']'".into())),
            }
        }
    }
    fn string(&mut self) -> Result<String> {
        if self.bytes.get(self.at) != Some(&b'"') {
            return Err(Error::Invalid("merge manifest: expected string".into()));
        }
        self.at += 1;
        let mut out = String::new();
        loop {
            match self.bytes.get(self.at) {
                Some(b'"') => {
                    self.at += 1;
                    return Ok(out);
                }
                Some(b'\\') => {
                    self.at += 1;
                    let c = match self.bytes.get(self.at) {
                        Some(b'"') => '"',
                        Some(b'\\') => '\\',
                        Some(b'/') => '/',
                        Some(b'n') => '\n',
                        Some(b'r') => '\r',
                        Some(b't') => '\t',
                        _ => return Err(Error::Invalid("merge manifest: bad escape".into())),
                    };
                    out.push(c);
                    self.at += 1;
                }
                Some(&b) => {
                    out.push(b as char);
                    self.at += 1;
                }
                None => return Err(Error::Invalid("merge manifest: unterminated string".into())),
            }
        }
    }
    fn number(&mut self) -> Result<Json> {
        let start = self.at;
        while matches!(
            self.bytes.get(self.at),
            Some(b'0'..=b'9' | b'-' | b'+' | b'.' | b'e' | b'E')
        ) {
            self.at += 1;
        }
        let text = std::str::from_utf8(&self.bytes[start..self.at])
            .map_err(|_| Error::Invalid("merge manifest: bad number".into()))?;
        text.parse::<f64>()
            .map(Json::Number)
            .map_err(|_| Error::Invalid("merge manifest: bad number".into()))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn small_index() -> PlaidIndex {
        let mut rng = 42u64;
        let mut data = Vec::new();
        for _ in 0..512 * 8 {
            rng ^= rng << 13;
            rng ^= rng >> 7;
            rng ^= rng << 17;
            data.push(((rng >> 40) as f32 / (1u64 << 24) as f32) - 0.5);
        }
        let mut index = PlaidIndex::new(0, 8, 4, 8, false).unwrap();
        index.train(8, &data, 512, 2).unwrap();
        let mut docs = Vec::new();
        for _ in 0..4 {
            for _ in 0..32 {
                rng ^= rng << 13;
                rng ^= rng >> 7;
                rng ^= rng << 17;
                docs.push(((rng >> 40) as f32 / (1u64 << 24) as f32) - 0.5);
            }
        }
        index
            .add_multiple_docs_embeddings(&docs, &[4u32; 4])
            .unwrap();
        index
    }

    #[test]
    fn streams_chunks_into_the_output_index() {
        let index = small_index();
        let size = PlaidIndexDiskMerger::calculate_merge_size(&[&index, &index], 8, 4);
        assert_eq!(size.total_docs, 8);
        assert_eq!(size.total_embeddings, 32);
        let mut merger = PlaidIndexDiskMerger::new("/tmp", "out", 8, 8, 4);
        merger.initialize(&size);
        merger.merge_chunk(&index, 0).unwrap();
        merger.merge_chunk(&index, 4).unwrap();
        assert_eq!(merger.progress().chunks_merged, 2);
        let mut merged = PlaidIndex::new(0, 8, 4, 8, false).unwrap();
        merged
            .copy_centroids_from(
                index.centroids_data(),
                index.centroid_norms_neg_half(),
                8,
                index.quantizer(),
                false,
            )
            .unwrap();
        merger.finalize(&mut merged).unwrap();
        assert_eq!(merged.doc_num(), 8);
        assert_eq!(merged.total_embedding_num(), 32);
        assert_eq!(merged.doc_lens(), &[4, 4, 4, 4, 4, 4, 4, 4]);
        assert_eq!(merged.ivf_lists().len(), 8);
    }

    #[test]
    fn manifest_roundtrips_through_json() {
        let mut manifest = PlaidMergeManifest {
            total_docs: 10,
            total_embeddings: 40,
            embedding_dimension: 8,
            nbits: 4,
            n_centroids: 8,
            ..Default::default()
        };
        manifest.update_chunk(PlaidMergeManifestChunk {
            chunk_id: 3,
            doc_count: 4,
            embedding_count: 16,
            file_size: 1234,
            last_modified: 77,
            checksum: "ab\"c".to_string(),
        });
        let json = manifest.to_json();
        let parsed = PlaidMergeManifest::from_json(&json).unwrap();
        assert_eq!(parsed, manifest);
        assert!(parsed.needs_merge(&PlaidMergeManifestChunk {
            chunk_id: 4,
            ..Default::default()
        }));
        assert!(!parsed.needs_merge(&manifest.chunks[0]));
    }

    #[test]
    fn incremental_merge_skips_unchanged_chunks() {
        let index = small_index();
        let meta = PlaidMergeManifestChunk {
            chunk_id: 0,
            doc_count: index.doc_num(),
            embedding_count: index.total_embedding_num() as u64,
            ..Default::default()
        };
        let mut manifest = PlaidMergeManifest::default();
        manifest.update_chunk(meta.clone());
        let mut merger = PlaidIndexDiskMerger::new("", "", 8, 8, 4);
        merger.initialize(&PlaidMergeSizeInfo::default());
        merger
            .merge_incremental(&manifest, &[(meta.clone(), &index)])
            .unwrap();
        assert_eq!(merger.progress().chunks_merged, 0);
        let changed = PlaidMergeManifestChunk {
            chunk_id: 0,
            doc_count: 1,
            ..meta
        };
        let mut merger = PlaidIndexDiskMerger::new("", "", 8, 8, 4);
        merger.initialize(&PlaidMergeSizeInfo::default());
        merger
            .merge_incremental(&manifest, &[(changed, &index)])
            .unwrap();
        assert_eq!(merger.progress().chunks_merged, 1);
    }
}

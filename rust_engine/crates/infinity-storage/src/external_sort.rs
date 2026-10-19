//! External sort merger, ported from
//! `src/storage/invertedindex/common/external_sort_merger.cppm`.
//!
//! The reference writes sorted runs as
//! `[u32 size][u32 count][u64 next_run_pos][records...]`, where each record is
//! `[LenType len][len bytes]` and the sort key is the record's own leading
//! bytes ([`SortKey`]). [`SortMerger::run`] k-way merges the runs in place:
//! it writes `path + ".out"`, removes the input and renames the result.
//!
//! This port keeps the on-disk framing and the key orders; it does not keep the
//! reference's predictor/output threads (the merge is a single streaming pass
//! with one [`std::io::BufReader`] per run).

use std::cmp::{Ordering, Reverse};
use std::collections::BinaryHeap;
use std::fs::{self, File};
use std::io::{BufReader, BufWriter, Read, Seek, SeekFrom, Write};
use std::marker::PhantomData;

use crate::cpp_format::FormatError;

/// `LenType` of a run record: the width and byte order of the length prefix.
pub trait RecordLen {
    /// `sizeof(LenType)`.
    const WIDTH: usize;
    fn read(bytes: &[u8]) -> usize;
    fn write(out: &mut [u8], len: usize);
}

impl RecordLen for u8 {
    const WIDTH: usize = 1;

    fn read(bytes: &[u8]) -> usize {
        usize::from(bytes[0])
    }

    fn write(out: &mut [u8], len: usize) {
        out[0] = len as u8;
    }
}

impl RecordLen for u32 {
    const WIDTH: usize = 4;

    fn read(bytes: &[u8]) -> usize {
        u32::from_le_bytes(bytes[..4].try_into().expect("width checked")) as usize
    }

    fn write(out: &mut [u8], len: usize) {
        out[..4].copy_from_slice(&(len as u32).to_le_bytes());
    }
}

/// The reference's `KeyType`: a record's sort key decoded from its bytes.
pub trait SortKey: Ord + Sized {
    fn from_record(record: &[u8]) -> Option<Self>;
}

impl SortKey for u32 {
    fn from_record(record: &[u8]) -> Option<Self> {
        record
            .get(..4)
            .map(|bytes| u32::from_le_bytes(bytes.try_into().expect("width checked")))
    }
}

/// The reference's `TermTuple` (`external_sort_merger.cppm:55`):
/// `term + '\0' + u32 doc_id + u32 term_pos + u16 doc_payload`, ordered by
/// `(term, doc_id, term_pos)`; the payload is not part of the key.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TermTuple {
    pub term: Vec<u8>,
    pub doc_id: u32,
    pub term_pos: u32,
    pub doc_payload: u16,
}

impl TermTuple {
    /// `TermTuple::TermTuple(char *, u32)`.
    pub fn parse(record: &[u8]) -> Option<Self> {
        let term_len = record.iter().position(|byte| *byte == 0)?;
        let rest = record.get(term_len + 1..)?;
        if rest.len() < 10 {
            return None;
        }
        Some(Self {
            term: record[..term_len].to_vec(),
            doc_id: u32::from_le_bytes(rest[..4].try_into().expect("length checked")),
            term_pos: u32::from_le_bytes(rest[4..8].try_into().expect("length checked")),
            doc_payload: u16::from_le_bytes(rest[8..10].try_into().expect("length checked")),
        })
    }
}

impl Ord for TermTuple {
    fn cmp(&self, other: &Self) -> Ordering {
        self.term
            .cmp(&other.term)
            .then_with(|| self.doc_id.cmp(&other.doc_id))
            .then_with(|| self.term_pos.cmp(&other.term_pos))
    }
}

impl PartialOrd for TermTuple {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl SortKey for TermTuple {
    fn from_record(record: &[u8]) -> Option<Self> {
        Self::parse(record)
    }
}

/// `SortMerger<KeyType, LenType>`.
///
/// `group_size` is the reference's `MAX_GROUP_SIZE_` (one merge group);
/// `buffer_size` sizes each run's read buffer (`BS_SIZE_`). `output_num` is
/// accepted for signature parity but the single-pass port does not need the
/// reference's output threads.
pub struct SortMerger<K, L> {
    path: String,
    group_size: usize,
    buffer_size: usize,
    _marker: PhantomData<fn() -> (K, L)>,
}

impl<K, L> SortMerger<K, L>
where
    K: SortKey,
    L: RecordLen,
{
    pub fn new(
        path: impl Into<String>,
        group_size: u32,
        buffer_size: u32,
        _output_num: u32,
    ) -> Self {
        Self {
            path: path.into(),
            group_size: usize::try_from(group_size).unwrap_or(usize::MAX),
            buffer_size: usize::try_from(buffer_size).unwrap_or(8192),
            _marker: PhantomData,
        }
    }

    /// `SortMerger::Run`: merge every run of `path` into sorted order.
    pub fn run(&self) -> Result<(), FormatError> {
        let file_len = fs::metadata(&self.path)
            .map_err(|err| FormatError::Io(err.to_string()))?
            .len();
        let file = File::open(&self.path).map_err(|err| FormatError::Io(err.to_string()))?;
        let mut reader = BufReader::new(file);
        let mut count_bytes = [0u8; 8];
        reader
            .read_exact(&mut count_bytes)
            .map_err(|err| FormatError::Io(err.to_string()))?;
        let count = u64::from_le_bytes(count_bytes);

        let mut runs: Vec<RunReader> = Vec::new();
        let mut offset = 8u64;
        while runs.len() < self.group_size && offset < file_len {
            reader
                .seek(SeekFrom::Start(offset))
                .map_err(|err| FormatError::Io(err.to_string()))?;
            let mut header = [0u8; 16];
            reader
                .read_exact(&mut header)
                .map_err(|err| FormatError::Io(err.to_string()))?;
            let size = u64::from(u32::from_le_bytes(
                header[..4].try_into().expect("length checked"),
            ));
            let next = u64::from_le_bytes(header[8..16].try_into().expect("length checked"));
            let start = offset + 16;
            let end = start.saturating_add(size).min(file_len);
            runs.push(RunReader::open(
                &self.path,
                start,
                end,
                L::WIDTH,
                self.buffer_size,
            )?);
            if next <= offset || next >= file_len {
                break;
            }
            offset = next;
        }

        let mut heap: BinaryHeap<Reverse<(K, usize, Vec<u8>)>> = BinaryHeap::new();
        for (index, run) in runs.iter_mut().enumerate() {
            if let Some(record) = run.next_record()? {
                heap.push(Reverse((decode_key::<K>(&record)?, index, record)));
            }
        }

        let out_path = format!("{}.out", self.path);
        let out = File::create(&out_path).map_err(|err| FormatError::Io(err.to_string()))?;
        let mut out = BufWriter::new(out);
        out.write_all(&count.to_le_bytes())
            .map_err(|err| FormatError::Io(err.to_string()))?;
        let mut written = 0u64;
        while let Some(Reverse((_key, index, record))) = heap.pop() {
            let mut len = [0u8; 4];
            L::write(&mut len, record.len());
            out.write_all(&len[..L::WIDTH])
                .map_err(|err| FormatError::Io(err.to_string()))?;
            out.write_all(&record)
                .map_err(|err| FormatError::Io(err.to_string()))?;
            written += 1;
            if let Some(record) = runs[index].next_record()? {
                heap.push(Reverse((decode_key::<K>(&record)?, index, record)));
            }
        }
        out.flush()
            .map_err(|err| FormatError::Io(err.to_string()))?;
        if written != count {
            return Err(FormatError::Io(format!(
                "sort merge wrote {written} of {count} records"
            )));
        }
        drop(out);
        fs::remove_file(&self.path).map_err(|err| FormatError::Io(err.to_string()))?;
        fs::rename(&out_path, &self.path).map_err(|err| FormatError::Io(err.to_string()))?;
        Ok(())
    }
}

fn decode_key<K: SortKey>(record: &[u8]) -> Result<K, FormatError> {
    K::from_record(record).ok_or_else(|| {
        FormatError::Io(format!(
            "sort record of {} bytes has no decodable key",
            record.len()
        ))
    })
}

/// One run's records, streamed from the shared input file.
struct RunReader {
    reader: BufReader<File>,
    /// Bytes left in the run (records only, headers excluded).
    remaining: u64,
    len_width: usize,
}

impl RunReader {
    fn open(
        path: &str,
        start: u64,
        end: u64,
        len_width: usize,
        buffer_size: usize,
    ) -> Result<Self, FormatError> {
        let mut file = File::open(path).map_err(|err| FormatError::Io(err.to_string()))?;
        file.seek(SeekFrom::Start(start))
            .map_err(|err| FormatError::Io(err.to_string()))?;
        Ok(Self {
            reader: BufReader::with_capacity(buffer_size.max(1), file),
            remaining: end.saturating_sub(start),
            len_width,
        })
    }

    fn next_record(&mut self) -> Result<Option<Vec<u8>>, FormatError> {
        if self.remaining == 0 {
            return Ok(None);
        }
        let mut len_bytes = [0u8; 4];
        self.reader
            .read_exact(&mut len_bytes[..self.len_width])
            .map_err(|err| FormatError::Io(err.to_string()))?;
        let len = u64::try_from(RecordLenU32::read(&len_bytes[..self.len_width]))
            .map_err(|_| FormatError::Io("record length overflow".to_string()))?;
        let total = len + self.len_width as u64;
        if total > self.remaining {
            return Err(FormatError::Truncated {
                need: usize::try_from(total).unwrap_or(usize::MAX),
                got: usize::try_from(self.remaining).unwrap_or(usize::MAX),
            });
        }
        let mut record = vec![
            0u8;
            usize::try_from(len).map_err(|_| {
                FormatError::Io("record length does not fit in memory".to_string())
            })?
        ];
        self.reader
            .read_exact(&mut record)
            .map_err(|err| FormatError::Io(err.to_string()))?;
        self.remaining -= total;
        Ok(Some(record))
    }
}

/// Reads a length prefix of any supported width (the type parameter only says
/// which width the run uses, so the decoder is picked dynamically here).
struct RecordLenU32;

impl RecordLenU32 {
    fn read(bytes: &[u8]) -> usize {
        match bytes.len() {
            1 => usize::from(bytes[0]),
            _ => u32::from_le_bytes(bytes[..4].try_into().expect("width checked")) as usize,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Deterministic `rand()` stand-in (the reference seeds from libc `rand`,
    /// whose stream differs between platforms).
    struct Rng(u64);

    impl Rng {
        fn new(seed: u64) -> Self {
            Self(seed)
        }

        fn next(&mut self) -> u64 {
            self.0 = self
                .0
                .wrapping_mul(6_364_136_223_846_793_005)
                .wrapping_add(1_442_695_040_888_963_407);
            self.0
        }

        /// `rand() % bound`.
        fn below(&mut self, bound: u64) -> u64 {
            self.next() % bound
        }
    }

    /// `RandStr<T>`: a random-length record whose first `sizeof(T)` bytes are
    /// `key` and whose remaining bytes are `'a' + rand() % 26`.
    fn rand_str(rng: &mut Rng, key: u32) -> Vec<u8> {
        let base = 4usize;
        let mut len = usize::try_from(rng.below(100)).expect("small") + base;
        while len == base {
            len = usize::try_from(rng.below(100)).expect("small") + base;
        }
        let mut record = vec![0u8; len];
        record[..4].copy_from_slice(&key.to_le_bytes());
        for byte in &mut record[4..] {
            *byte = b'a' + u8::try_from(rng.below(26)).expect("small");
        }
        record
    }

    fn write_run_header(
        file: &mut File,
        header_pos: u64,
        size: u32,
        count: u32,
        next: u64,
    ) -> std::io::Result<()> {
        file.seek(SeekFrom::Start(header_pos))?;
        file.write_all(&size.to_le_bytes())?;
        file.write_all(&count.to_le_bytes())?;
        file.write_all(&next.to_le_bytes())?;
        Ok(())
    }

    /// `ExternalSortTest::CheckMerger<u32, u8>`.
    fn check_merger(size: u64, buffer_size: u32, rng: &mut Rng) {
        let path = std::env::temp_dir().join(format!(
            "infinity_extsort_{}_{}.tt",
            std::process::id(),
            size
        ));
        let path_str = path.to_str().expect("utf-8 temp path").to_string();
        let mut file = File::create(&path).expect("create");
        file.write_all(&size.to_le_bytes()).expect("count");

        let run_num = 100u64;
        assert_eq!(size % run_num, 0);
        let per_run = size / run_num;
        for run in 0..run_num {
            let header_pos = file.stream_position().expect("tell");
            file.seek(SeekFrom::Current(16)).expect("header");
            let mut bytes = 0u32;
            for j in 0..per_run {
                let key = u32::try_from(run * per_run + j).expect("key fits");
                let record = rand_str(rng, key);
                file.write_all(&(record.len() as u8).to_le_bytes())
                    .expect("len");
                file.write_all(&record).expect("record");
                bytes += 1 + record.len() as u32;
            }
            let next = file.stream_position().expect("tell");
            write_run_header(&mut file, header_pos, bytes, per_run as u32, next).expect("header");
            file.seek(SeekFrom::End(0)).expect("end");
        }
        file.sync_all().expect("sync");
        drop(file);

        SortMerger::<u32, u8>::new(&path_str, run_num as u32, buffer_size, 2)
            .run()
            .expect("merge");

        let mut file = File::open(&path).expect("open");
        let mut count = [0u8; 8];
        file.read_exact(&mut count).expect("count");
        let count = u64::from_le_bytes(count);
        assert_eq!(count, size);
        for i in 0..count {
            let mut len = [0u8; 1];
            file.read_exact(&mut len).expect("len");
            let mut record = vec![0u8; usize::from(len[0])];
            file.read_exact(&mut record).expect("record");
            let key = u32::from_le_bytes(record[..4].try_into().expect("width"));
            assert_eq!(key, u32::try_from(i).expect("key fits"), "record {i}");
        }
        fs::remove_file(&path).expect("cleanup");
    }

    /// `ExternalSortTest::test1`.
    #[test]
    fn test1() {
        let mut rng = Rng::new(0x05EE_D501);
        check_merger(1_000, 100_000, &mut rng);
        check_merger(10_000, 1_000_000, &mut rng);
    }

    /// `ExternalSortTest::CheckTermTuple`.
    #[test]
    fn test2() {
        let size = 100u64;
        let run_num = 10u32;
        let per_run = size / u64::from(run_num);
        let path =
            std::env::temp_dir().join(format!("infinity_extsort_term_{}.tt", std::process::id()));
        let path_str = path.to_str().expect("utf-8 temp path").to_string();
        let mut file = File::create(&path).expect("create");
        file.write_all(&size.to_le_bytes()).expect("count");

        for run in 0..run_num {
            let header_pos = file.stream_position().expect("tell");
            file.seek(SeekFrom::Current(16)).expect("header");
            let mut bytes = 0u32;
            for j in 0..per_run {
                let term = (u64::from(run) * per_run + j).to_string();
                let mut record = term.clone().into_bytes();
                record.push(0);
                record.extend_from_slice(&34_567u32.to_le_bytes());
                record.extend_from_slice(&run.to_le_bytes());
                record.extend_from_slice(&10_086u16.to_le_bytes());
                file.write_all(&(record.len() as u32).to_le_bytes())
                    .expect("len");
                file.write_all(&record).expect("record");
                bytes += 4 + record.len() as u32;
            }
            let next = file.stream_position().expect("tell");
            write_run_header(&mut file, header_pos, bytes, per_run as u32, next).expect("header");
            file.seek(SeekFrom::End(0)).expect("end");
        }
        file.sync_all().expect("sync");
        drop(file);

        SortMerger::<TermTuple, u32>::new(&path_str, run_num, 1_000_000, 2)
            .run()
            .expect("merge");

        let mut file = File::open(&path).expect("open");
        let mut count = [0u8; 8];
        file.read_exact(&mut count).expect("count");
        assert_eq!(u64::from_le_bytes(count), size);
        let mut terms: Vec<Vec<u8>> = Vec::new();
        for _ in 0..size {
            let mut len = [0u8; 4];
            file.read_exact(&mut len).expect("len");
            let mut record = vec![0u8; u32::from_le_bytes(len) as usize];
            file.read_exact(&mut record).expect("record");
            let tuple = TermTuple::parse(&record).expect("tuple");
            assert_eq!(tuple.doc_id, 34_567);
            assert_eq!(tuple.doc_payload, 10_086);
            terms.push(tuple.term);
        }
        assert!(terms.windows(2).all(|pair| pair[0] <= pair[1]));
        fs::remove_file(&path).expect("cleanup");
    }
}

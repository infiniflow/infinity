//! Buffered file and byte-slice I/O — the counterpart of the reference's
//! `src/storage/io/` module:
//!
//! * [`FileWriter`] / [`FileReader`] (`file_writer.cppm`, `file_reader.cppm`)
//!   with the reference's big-endian fixed-width integers and LEB128 varints.
//! * [`ByteSliceWriter`] / [`ByteSliceReader`] (`byte_slice_writer.cppm`,
//!   `byte_slice_reader.cppm`) over a chain of [`ByteSlice`]s. Unlike the file
//!   writer these store native little-endian integers.
//! * [`StreamReader`] (`stream_reader.cppm`), a line reader.
//!
//! The wire encodings are the observable contract of these classes and are
//! pinned by the ported unit tests. Byte slices grow in place the same way the
//! reference does (`GetIncrementedSliceSize`), so a chain's total size and
//! merge/partial-copy behaviour match.

use std::fs::File;
use std::io::{BufRead, BufReader};
use std::path::{Path, PathBuf};

use infinity_common::{Error, Result};

use crate::virtual_store::{FileAccessMode, FileHandle, VirtualStore};

/// C++ `ByteSlice::GetHeadSize()`: `sizeof(ByteSlice)` (pointer + size +
/// pointer) on the 64-bit reference build.
const SLICE_HEAD_SIZE: u32 = 24;
/// C++ `ByteSliceWriter::MIN_SLICE_SIZE`.
const MIN_SLICE_SIZE: u32 = SLICE_HEAD_SIZE;
/// C++ `DEFAULT_CHUNK_SIZE` (`default_values.cppm:95`).
const DEFAULT_CHUNK_SIZE: u32 = 10 * 1024 * 1024;

/// A contiguous slice of written bytes.
///
/// C++: `ByteSlice` (`src/common/memory/byte_slice.cppm`). The allocation
/// capacity is the slice's grown size; [`ByteSlice::size`] is how much of it
/// has been written.
#[derive(Clone, Debug, Default)]
pub struct ByteSlice {
    data: Vec<u8>,
    size: usize,
}

impl ByteSlice {
    fn with_capacity(capacity: usize) -> Self {
        Self {
            data: vec![0u8; capacity],
            size: 0,
        }
    }

    /// The bytes written to this slice (`data_[0..size_]`).
    pub fn data(&self) -> &[u8] {
        &self.data[..self.size]
    }

    /// C++ `ByteSlice::size_`.
    pub fn size(&self) -> usize {
        self.size
    }

    fn capacity(&self) -> usize {
        self.data.len()
    }
}

/// The chain of slices produced by a [`ByteSliceWriter`].
///
/// C++: `ByteSliceList` (`src/common/memory/byte_slice.cppm`).
#[derive(Clone, Debug, Default)]
pub struct ByteSliceList {
    slices: Vec<ByteSlice>,
    total_size: usize,
}

impl ByteSliceList {
    pub fn head(&self) -> Option<&ByteSlice> {
        self.slices.first()
    }

    pub fn slices(&self) -> &[ByteSlice] {
        &self.slices
    }

    /// C++ `ByteSliceList::GetTotalSize`.
    pub fn total_size(&self) -> usize {
        self.total_size
    }

    /// C++ `ByteSliceList::IncrementTotalSize`.
    pub fn increment_total_size(&mut self, n: usize) {
        self.total_size += n;
    }

    /// C++ `ByteSliceList::UpdateTotalSize`: recomputes the size from the
    /// slices and returns it.
    pub fn update_total_size(&mut self) -> usize {
        self.total_size = self.slices.iter().map(ByteSlice::size).sum();
        self.total_size
    }

    pub fn add(&mut self, slice: ByteSlice) {
        self.slices.push(slice);
    }

    pub fn clear(&mut self) {
        self.slices.clear();
        self.total_size = 0;
    }

    /// C++ `ByteSliceList::MergeWith`: moves every slice of `src` to the tail
    /// of this list. `src` is left empty, which is why a merged writer reports
    /// size 0.
    pub fn merge_with(&mut self, src: &mut ByteSliceList) {
        self.slices.append(&mut src.slices);
        self.total_size += src.total_size;
        src.total_size = 0;
    }
}

/// Forward iterator over a slice chain.
///
/// C++: `ByteSliceListIterator` (`src/common/memory/byte_slice.cppm`).
pub struct ByteSliceListIterator<'a> {
    list: &'a ByteSliceList,
    index: usize,
}

impl<'a> ByteSliceListIterator<'a> {
    pub fn new(list: &'a ByteSliceList) -> Self {
        Self { list, index: 0 }
    }

    /// C++ `SeekSlice(0)`: positions the iterator on the first slice.
    pub fn seek_slice(&mut self, index: usize) {
        self.index = index;
    }

    pub fn has_next(&self, _total_size: usize) -> bool {
        self.index < self.list.slices.len()
    }

    /// Returns `(data, size)` of the current slice and advances.
    #[allow(clippy::should_implement_trait)] // the C++ name is `Next`
    pub fn next(&mut self) -> Option<(&'a [u8], usize)> {
        let slice = self.list.slices.get(self.index)?;
        self.index += 1;
        Some((slice.data(), slice.size()))
    }
}

/// C++ `ByteSliceWriter` (`src/storage/io/byte_slice_writer.cppm`).
#[derive(Debug)]
pub struct ByteSliceWriter {
    list: ByteSliceList,
    last_slice_size: u32,
}

impl Default for ByteSliceWriter {
    fn default() -> Self {
        Self::new()
    }
}

impl ByteSliceWriter {
    pub fn new() -> Self {
        let mut list = ByteSliceList::default();
        list.add(ByteSlice::with_capacity(0));
        Self {
            list,
            last_slice_size: MIN_SLICE_SIZE - SLICE_HEAD_SIZE,
        }
    }

    /// C++ `GetIncrementedSliceSize`.
    fn incremented_slice_size(last: u32) -> u32 {
        let prev = last + SLICE_HEAD_SIZE;
        let slice_size = prev + (prev >> 2) - SLICE_HEAD_SIZE;
        let chunk_size = DEFAULT_CHUNK_SIZE - SLICE_HEAD_SIZE - 8;
        slice_size.min(chunk_size)
    }

    fn create_slice(&mut self, capacity: u32) -> usize {
        self.last_slice_size = capacity;
        self.list.add(ByteSlice::with_capacity(capacity as usize));
        self.list.slices.len() - 1
    }

    fn tail_index(&self) -> usize {
        self.list.slices.len() - 1
    }

    /// C++ `GetSliceForWrite<T>`: a typed value is never split across slices.
    fn reserve_contiguous(&mut self, n: usize) {
        let tail = self.tail_index();
        if self.list.slices[tail].size + n > self.last_slice_size as usize {
            let next = Self::incremented_slice_size(self.last_slice_size);
            self.create_slice(next);
        }
    }

    pub fn write_byte(&mut self, value: u8) {
        self.reserve_contiguous(1);
        let tail = self.tail_index();
        let slice = &mut self.list.slices[tail];
        slice.data[slice.size] = value;
        slice.size += 1;
        self.list.increment_total_size(1);
    }

    pub fn write_int16(&mut self, value: i16) {
        self.write_typed(&value.to_le_bytes());
    }

    pub fn write_int32(&mut self, value: i32) {
        self.write_typed(&value.to_le_bytes());
    }

    pub fn write_uint32(&mut self, value: u32) {
        self.write_typed(&value.to_le_bytes());
    }

    fn write_typed(&mut self, bytes: &[u8]) {
        self.reserve_contiguous(bytes.len());
        let tail = self.tail_index();
        let slice = &mut self.list.slices[tail];
        slice.data[slice.size..slice.size + bytes.len()].copy_from_slice(bytes);
        slice.size += bytes.len();
        self.list.increment_total_size(bytes.len());
    }

    /// C++ `Write(const void *, size_t)`: fills the tail slice and appends new
    /// slices as needed.
    pub fn write(&mut self, mut data: &[u8]) {
        while !data.is_empty() {
            let tail = self.tail_index();
            if self.list.slices[tail].size >= self.last_slice_size as usize {
                let next = Self::incremented_slice_size(self.last_slice_size);
                self.create_slice(next);
            }
            let tail = self.tail_index();
            let slice = &mut self.list.slices[tail];
            let room = slice.capacity() - slice.size;
            let copy = room.min(data.len());
            slice.data[slice.size..slice.size + copy].copy_from_slice(&data[..copy]);
            slice.size += copy;
            data = &data[copy..];
            self.list.increment_total_size(copy);
        }
    }

    /// C++ `WriteVInt`.
    pub fn write_vint(&mut self, value: u32) -> u32 {
        let mut i = value;
        let mut len = 1;
        while i & !0x7F != 0 {
            self.write_byte(((i & 0x7F) | 0x80) as u8);
            i >>= 7;
            len += 1;
        }
        self.write_byte((i & 0x7F) as u8);
        len
    }

    /// C++ `WriteVLong`.
    pub fn write_vlong(&mut self, value: u64) -> u32 {
        let mut i = value;
        let mut len = 1;
        while i & !0x7F != 0 {
            self.write_byte(((i & 0x7F) | 0x80) as u8);
            i >>= 7;
            len += 1;
        }
        self.write_byte((i & 0x7F) as u8);
        len
    }

    pub fn get_size(&self) -> usize {
        self.list.total_size()
    }

    pub fn slice_list(&self) -> &ByteSliceList {
        &self.list
    }

    /// C++ `Dump`: writes every slice to `file`.
    pub fn dump(&self, file: &mut FileWriter) -> Result<()> {
        for slice in &self.list.slices {
            file.write(slice.data())?;
        }
        Ok(())
    }

    /// C++ `Load`: replaces the chain with one slice holding `size` bytes read
    /// from `file`.
    pub fn load(&mut self, file: &mut FileReader, size: usize) -> Result<()> {
        let mut slice = ByteSlice::with_capacity(size);
        file.read_into(&mut slice.data)?;
        slice.size = size;
        self.list.clear();
        self.list.add(slice);
        self.list.increment_total_size(size);
        self.last_slice_size = size as u32;
        Ok(())
    }

    /// C++ `Reset`.
    pub fn reset(&mut self) {
        self.list.clear();
        self.last_slice_size = MIN_SLICE_SIZE - SLICE_HEAD_SIZE;
        self.list.add(ByteSlice::with_capacity(0));
    }

    /// C++ `Write(ByteSliceList &src)`: the source chain is consumed (its
    /// total size becomes 0).
    pub fn write_list(&mut self, src: &mut ByteSliceWriter) {
        self.list.merge_with(&mut src.list);
    }

    /// C++ `Write(const ByteSliceList &src, u32 start, u32 end)`.
    pub fn write_range(&mut self, src: &ByteSliceWriter, start: u32, end: u32) {
        assert!(
            start < end && end as usize <= src.get_size(),
            "Write past EOF"
        );
        let mut offset = 0usize;
        for slice in &src.list.slices {
            let slice_start = offset;
            let slice_end = offset + slice.size();
            offset = slice_end;
            if slice_end <= start as usize || slice_start >= end as usize {
                continue;
            }
            let from = (start as usize).max(slice_start) - slice_start;
            let to = (end as usize).min(slice_end) - slice_start;
            self.write(&slice.data()[from..to]);
        }
    }
}

/// C++ `ByteSliceReader` (`src/storage/io/byte_slice_reader.cppm`).
pub struct ByteSliceReader<'a> {
    list: &'a ByteSliceList,
    slice_index: usize,
    slice_offset: usize,
    global_offset: usize,
}

impl<'a> ByteSliceReader<'a> {
    pub fn new(list: &'a ByteSliceList) -> Result<Self> {
        if list.head().is_none() {
            return Err(Error::Storage("Read past EOF".into()));
        }
        Ok(Self {
            list,
            slice_index: 0,
            slice_offset: 0,
            global_offset: 0,
        })
    }

    fn current(&self) -> Option<&'a ByteSlice> {
        self.list.slices.get(self.slice_index)
    }

    /// Skips exhausted slices the way `ReadByte` follows `next_`.
    fn skip_empty(&mut self) -> Result<()> {
        loop {
            let Some(slice) = self.current() else {
                return Err(Error::Storage("Read past EOF".into()));
            };
            if self.slice_offset < slice.size {
                return Ok(());
            }
            self.slice_index += 1;
            self.slice_offset = 0;
        }
    }

    pub fn read_byte(&mut self) -> Result<u8> {
        self.skip_empty()?;
        let slice = self.current().expect("checked");
        let value = slice.data[self.slice_offset];
        self.slice_offset += 1;
        self.global_offset += 1;
        Ok(value)
    }

    fn read_bytes(&mut self, out: &mut [u8]) -> Result<()> {
        for byte in out.iter_mut() {
            *byte = self.read_byte()?;
        }
        Ok(())
    }

    pub fn read_int16(&mut self) -> Result<i16> {
        let mut b = [0u8; 2];
        self.read_bytes(&mut b)?;
        Ok(i16::from_le_bytes(b))
    }

    pub fn read_int32(&mut self) -> Result<i32> {
        let mut b = [0u8; 4];
        self.read_bytes(&mut b)?;
        Ok(i32::from_le_bytes(b))
    }

    pub fn read_uint32(&mut self) -> Result<u32> {
        let mut b = [0u8; 4];
        self.read_bytes(&mut b)?;
        Ok(u32::from_le_bytes(b))
    }

    pub fn read_int64(&mut self) -> Result<i64> {
        let mut b = [0u8; 8];
        self.read_bytes(&mut b)?;
        Ok(i64::from_le_bytes(b))
    }

    pub fn read_uint64(&mut self) -> Result<u64> {
        let mut b = [0u8; 8];
        self.read_bytes(&mut b)?;
        Ok(u64::from_le_bytes(b))
    }

    pub fn read_vint32(&mut self) -> Result<i32> {
        Ok(self.read_vuint32()? as i32)
    }

    pub fn read_vuint32(&mut self) -> Result<u32> {
        let mut byte = self.read_byte()?;
        let mut value = (byte & 0x7F) as u32;
        let mut shift = 7;
        while byte & 0x80 != 0 {
            byte = self.read_byte()?;
            value |= ((byte & 0x7F) as u32) << shift;
            shift += 7;
        }
        Ok(value)
    }

    pub fn read_vuint64(&mut self) -> Result<u64> {
        let mut byte = self.read_byte()?;
        let mut value = (byte & 0x7F) as u64;
        let mut shift = 7;
        while byte & 0x80 != 0 {
            byte = self.read_byte()?;
            value |= ((byte & 0x7F) as u64) << shift;
            shift += 7;
        }
        Ok(value)
    }

    /// C++ `Read`: copies at most `len` bytes across slice boundaries and
    /// returns how many were available.
    pub fn read(&mut self, out: &mut [u8]) -> usize {
        let mut done = 0;
        while done < out.len() {
            if self.skip_empty().is_err() {
                break;
            }
            let slice = self.current().expect("checked");
            let n = (slice.size - self.slice_offset).min(out.len() - done);
            out[done..done + n]
                .copy_from_slice(&slice.data[self.slice_offset..self.slice_offset + n]);
            done += n;
            self.slice_offset += n;
            self.global_offset += n;
        }
        done
    }

    pub fn tell(&self) -> usize {
        self.global_offset
    }

    /// C++ `End`.
    pub fn end(&self) -> bool {
        self.global_offset + 1 >= self.list.total_size()
    }
}

/// C++ `FileWriter` (`src/storage/io/file_writer.cppm`). Fixed-width integers
/// are **big-endian** here; the byte-slice writer is little-endian.
#[derive(Debug)]
pub struct FileWriter {
    path: PathBuf,
    buffer: Vec<u8>,
    offset: usize,
    total_written: usize,
    buffer_size: usize,
    handle: FileHandle,
}

impl FileWriter {
    pub fn new(path: impl AsRef<Path>, buffer_size: usize) -> Result<Self> {
        let path = path.as_ref().to_path_buf();
        let handle = VirtualStore::open(&path, FileAccessMode::Write)?;
        let buffer_size = buffer_size.max(1);
        Ok(Self {
            path,
            buffer: vec![0u8; buffer_size],
            offset: 0,
            total_written: 0,
            buffer_size,
            handle,
        })
    }

    pub fn write_byte(&mut self, b: u8) -> Result<()> {
        if self.offset == self.buffer_size {
            self.flush()?;
        }
        self.buffer[self.offset] = b;
        self.offset += 1;
        Ok(())
    }

    pub fn write_short(&mut self, i: i16) -> Result<()> {
        self.write_byte((i >> 8) as u8)?;
        self.write_byte(i as u8)
    }

    pub fn write_int(&mut self, i: i32) -> Result<()> {
        self.write_byte((i >> 24) as u8)?;
        self.write_byte((i >> 16) as u8)?;
        self.write_byte((i >> 8) as u8)?;
        self.write_byte(i as u8)
    }

    pub fn write_vint(&mut self, vi: i32) -> Result<()> {
        let mut i = vi as u32;
        while i & !0x7F != 0 {
            self.write_byte(((i & 0x7F) | 0x80) as u8)?;
            i >>= 7;
        }
        self.write_byte((i & 0x7F) as u8)
    }

    pub fn write_long(&mut self, i: i64) -> Result<()> {
        self.write_int((i >> 32) as i32)?;
        self.write_int(i as i32)
    }

    pub fn write_vlong(&mut self, vi: i64) -> Result<()> {
        let mut i = vi as u64;
        while i & !0x7F != 0 {
            self.write_byte(((i & 0x7F) | 0x80) as u8)?;
            i >>= 7;
        }
        self.write_byte((i & 0x7F) as u8)
    }

    pub fn write(&mut self, mut data: &[u8]) -> Result<()> {
        while !data.is_empty() {
            let room = self.buffer_size - self.offset;
            let n = room.min(data.len());
            self.buffer[self.offset..self.offset + n].copy_from_slice(&data[..n]);
            self.offset += n;
            data = &data[n..];
            if self.offset == self.buffer_size {
                self.flush()?;
            }
        }
        Ok(())
    }

    pub fn flush(&mut self) -> Result<()> {
        if self.offset == 0 {
            return Ok(());
        }
        self.handle.append(&self.buffer[..self.offset])?;
        self.total_written += self.offset;
        self.offset = 0;
        Ok(())
    }

    pub fn sync(&mut self) -> Result<()> {
        self.flush()?;
        self.handle.sync()
    }

    /// C++ `GetFileSize`: bytes already written to disk plus the buffered tail.
    pub fn file_size(&self) -> Result<i64> {
        Ok(self.handle.file_size()? as i64 + self.offset as i64)
    }

    /// C++ `TotalWrittenBytes`: flushed plus buffered bytes.
    pub fn total_written_bytes(&self) -> usize {
        self.total_written + self.offset
    }

    pub fn path(&self) -> &Path {
        &self.path
    }
}

/// C++ `FileReader` (`src/storage/io/file_reader.cppm`). Fixed-width integers
/// are big-endian.
#[derive(Debug)]
pub struct FileReader {
    path: PathBuf,
    data: Vec<u8>,
    pos: usize,
}

impl FileReader {
    pub fn new(path: impl AsRef<Path>, _buffer_size: usize) -> Result<Self> {
        let path = path.as_ref().to_path_buf();
        let data = std::fs::read(&path).map_err(|e| Error::io(&path, e))?;
        Ok(Self { path, data, pos: 0 })
    }

    fn take(&mut self, n: usize) -> Result<&[u8]> {
        if self.pos + n > self.data.len() {
            return Err(Error::Storage(format!(
                "No enough data reading from {}",
                self.path.display()
            )));
        }
        let start = self.pos;
        self.pos += n;
        Ok(&self.data[start..start + n])
    }

    pub fn read_byte(&mut self) -> Result<u8> {
        Ok(self.take(1)?[0])
    }

    pub fn read_short(&mut self) -> Result<i16> {
        let b = self.take(2)?;
        Ok(i16::from_be_bytes([b[0], b[1]]))
    }

    pub fn read_int(&mut self) -> Result<i32> {
        let b = self.take(4)?;
        Ok(i32::from_be_bytes([b[0], b[1], b[2], b[3]]))
    }

    pub fn read_vint(&mut self) -> Result<i32> {
        let mut byte = self.read_byte()?;
        let mut i = (byte & 0x7F) as u32;
        let mut shift = 7;
        while byte & 0x80 != 0 {
            byte = self.read_byte()?;
            i |= ((byte & 0x7F) as u32) << shift;
            shift += 7;
        }
        Ok(i as i32)
    }

    pub fn read_long(&mut self) -> Result<i64> {
        let hi = self.read_int()? as i64;
        let lo = self.read_int()? as i64 & 0xFFFF_FFFF;
        Ok((hi << 32) | lo)
    }

    pub fn read_vlong(&mut self) -> Result<i64> {
        let mut byte = self.read_byte()?;
        let mut i = (byte & 0x7F) as u64;
        let mut shift = 7;
        while byte & 0x80 != 0 {
            byte = self.read_byte()?;
            i |= ((byte & 0x7F) as u64) << shift;
            shift += 7;
        }
        Ok(i as i64)
    }

    /// C++ `Read(char *, size_t)`: exact read, erroring like
    /// `Status::DataIOError` when the file is short.
    pub fn read(&mut self, out: &mut [u8]) -> Result<()> {
        self.read_into(out)
    }

    fn read_into(&mut self, out: &mut [u8]) -> Result<()> {
        let n = out.len();
        out.copy_from_slice(self.take(n)?);
        Ok(())
    }

    pub fn finished(&self) -> bool {
        self.pos >= self.data.len()
    }

    /// C++ `GetFilePointer`.
    pub fn file_pointer(&self) -> u64 {
        self.pos as u64
    }

    /// C++ `Seek`.
    pub fn seek(&mut self, pos: u64) {
        self.pos = pos as usize;
    }

    pub fn file_size(&self) -> usize {
        self.data.len()
    }
}

/// C++ `StreamReader` (`src/storage/io/stream_reader.cppm`), a `getline`
/// wrapper.
pub struct StreamReader {
    reader: BufReader<File>,
}

impl StreamReader {
    pub fn new(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        let file = File::open(path).map_err(|e| Error::io(path, e))?;
        Ok(Self {
            reader: BufReader::new(file),
        })
    }

    /// C++ `ReadLine`: strips the trailing `\n`, leaves `\r` (as `getline`
    /// does). Returns false at end of file.
    pub fn read_line(&mut self, line: &mut String) -> Result<bool> {
        line.clear();
        let n = self
            .reader
            .read_line(line)
            .map_err(|e| Error::io("<stream>", e))?;
        if n == 0 {
            return Ok(false);
        }
        if line.ends_with('\n') {
            line.pop();
        }
        Ok(true)
    }

    /// C++ `Close`; dropping the reader closes the file.
    pub fn close(self) {}
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    fn payload_string(len: usize, byte: char) -> String {
        std::iter::repeat_n(byte, len).collect()
    }

    fn all_slice_bytes(list: &ByteSliceList) -> Vec<u8> {
        let mut out = Vec::with_capacity(list.total_size());
        for slice in list.slices() {
            out.extend_from_slice(slice.data());
        }
        out
    }

    /// C++ `ByteSliceReaderWriterTest::CheckList`.
    fn check_list_i16(list: &ByteSliceList, num_elem: usize) {
        assert_eq!(num_elem * 2, list.total_size());
        let bytes = all_slice_bytes(list);
        let mut n = 0;
        for i in 0..num_elem {
            let value = i16::from_le_bytes([bytes[n], bytes[n + 1]]);
            assert_eq!(value, i as i16);
            n += 2;
        }
    }

    /// C++: `byte_slice_write_read_ut.cpp:77` (`test1`).
    #[test]
    fn byte_slice_writer_tracks_size() {
        let mut writer = ByteSliceWriter::new();
        let mut size = 0usize;

        writer.write_byte(12);
        size += 1;
        assert_eq!(size, writer.get_size());

        writer.write_int16(120);
        size += 2;
        assert_eq!(size, writer.get_size());

        writer.write_int32(12);
        size += 4;
        assert_eq!(size, writer.get_size());

        writer.write_uint32(21);
        size += 4;
        assert_eq!(size, writer.get_size());
    }

    /// C++: `byte_slice_write_read_ut.cpp:103` (`test2`).
    #[test]
    fn byte_slice_writer_int16_roundtrip() {
        let mut writer = ByteSliceWriter::new();
        for i in 0..10000i16 {
            writer.write_int16(i);
        }
        assert_eq!(10000 * 2, writer.get_size());
        check_list_i16(writer.slice_list(), 10000);
    }

    /// C++: `byte_slice_write_read_ut.cpp:114` (`test3`).
    #[test]
    fn byte_slice_writer_string_roundtrip() {
        let mut writer = ByteSliceWriter::new();
        let text = payload_string(16000, 'x');
        writer.write(text.as_bytes());
        assert_eq!(text.len(), writer.get_size());
        let bytes = all_slice_bytes(writer.slice_list());
        assert_eq!(bytes.len(), 16000);
        assert!(bytes.iter().all(|b| *b == b'x'));
    }

    /// C++: `byte_slice_write_read_ut.cpp:123` (`test4`).
    #[test]
    fn byte_slice_writer_vint_roundtrip() {
        let mut writer = ByteSliceWriter::new();
        for i in 0..10000u32 {
            writer.write_vint(i);
        }
        let list = writer.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        for i in 0..10000i32 {
            assert_eq!(reader.read_vint32().unwrap(), i);
        }
    }

    /// C++: `byte_slice_write_read_ut.cpp:138` (`test5`).
    #[test]
    fn byte_slice_writer_vlong_roundtrip() {
        let mut writer = ByteSliceWriter::new();
        for i in 0..10000u64 {
            writer.write_vlong(i);
        }
        let list = writer.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        for i in 0..10000u64 {
            assert_eq!(reader.read_vuint64().unwrap(), i);
        }
    }

    /// C++: `byte_slice_write_read_ut.cpp:153` (`test6`).
    #[test]
    fn byte_slice_writer_dump_and_load() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test001.txt");

        let mut writer = ByteSliceWriter::new();
        for i in 0..1000i16 {
            writer.write_int16(i);
        }
        let size = writer.get_size();
        assert_eq!(1000 * 2, size);

        let mut file_writer = FileWriter::new(&path, 128000).unwrap();
        writer.dump(&mut file_writer).unwrap();
        file_writer.sync().unwrap();

        writer.reset();
        let mut file_reader = FileReader::new(&path, 128000).unwrap();
        writer.load(&mut file_reader, 1000 * 2).unwrap();

        let list = writer.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        for i in 0..1000i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }
    }

    /// C++: `byte_slice_write_read_ut.cpp:185` (`test7`).
    #[test]
    fn byte_slice_writer_merge_and_partial_copy() {
        let mut writer1 = ByteSliceWriter::new();
        for i in 500..1000i16 {
            writer1.write_int16(i);
        }
        assert_eq!(writer1.get_size(), 500 * 2);

        let mut writer2 = ByteSliceWriter::new();
        for i in 1000..1500i16 {
            writer2.write_int16(i);
        }
        assert_eq!(writer2.get_size(), 500 * 2);

        // head --> writer2's data --> writer1's data --> tail
        writer2.write_list(&mut writer1);
        assert_eq!(writer1.get_size(), 0);
        assert_eq!(writer2.get_size(), 1000 * 2);

        let list = writer2.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        for i in 1000..1500i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }
        for i in 500..1000i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }

        let mut writer3 = ByteSliceWriter::new();
        for i in 1500..2000i16 {
            writer3.write_int16(i);
        }
        assert_eq!(writer3.get_size(), 500 * 2);

        // head --> writer3's data --> writer2's first half + 100 bytes
        writer3.write_range(&writer2, 0, writer2.get_size() as u32 / 2 + 100);

        let list = writer3.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        for i in 1500..2000i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }
        for i in 1000..1500i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }
        for i in 500..550i16 {
            assert_eq!(reader.read_int16().unwrap(), i);
        }

        let mut list = writer3.slice_list().clone();
        assert_eq!(list.update_total_size(), (500 + 500 + 50) * 2);

        let mut iter = ByteSliceListIterator::new(&list);
        iter.seek_slice(0);
        let mut slices = 0;
        while iter.has_next(list.total_size()) {
            let (_data, _size) = iter.next().expect("slice");
            slices += 1;
        }
        assert!(slices > 0);
    }

    /// C++: `byte_slice_write_read_ut.cpp:263` (`TestDataConsistency`).
    #[test]
    fn byte_slice_reader_writer_data_consistency() {
        let mut writer = ByteSliceWriter::new();
        writer.write_byte(255);
        writer.write_int16(-12345);
        writer.write_int32(123456789);
        writer.write_uint32(987654321);

        let list = writer.slice_list().clone();
        let mut reader = ByteSliceReader::new(&list).unwrap();
        assert_eq!(reader.read_byte().unwrap(), 255);
        assert_eq!(reader.read_int16().unwrap(), -12345);
        assert_eq!(reader.read_int32().unwrap(), 123456789);
        assert_eq!(reader.read_uint32().unwrap(), 987654321);
    }

    /// C++: `file_write_read_ut.cpp:36` (`test1`).
    #[test]
    fn file_writer_read_prefix() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file1.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        for _ in 0..128 {
            writer.write(b"abc").unwrap();
        }
        writer.flush().unwrap();
        assert_eq!(writer.file_size().unwrap(), 128 * 3);
        assert_eq!(writer.total_written_bytes(), 128 * 3);

        let mut reader = FileReader::new(&path, 128).unwrap();
        let mut read = [0u8; 4];
        reader.read(&mut read).unwrap();
        assert_eq!(&read, b"abca");
        assert!(!reader.finished());
    }

    /// C++: `file_write_read_ut.cpp:58` (`test2`).
    #[test]
    fn file_writer_vint_roundtrip() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file2.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        for i in 0..128u32 {
            writer.write_vint(i as i32).unwrap();
        }
        writer.flush().unwrap();

        let mut reader = FileReader::new(&path, 128).unwrap();
        for i in 0..128u32 {
            assert_eq!(reader.read_vint().unwrap(), i as i32);
        }
    }

    /// C++: `file_write_read_ut.cpp:76` (`test3`).
    #[test]
    fn file_writer_mixed_vint_and_bytes() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file3.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        for i in 0..128u32 {
            writer.write_vint(i as i32).unwrap();
        }
        for _ in 0..3 {
            writer.write(b"abc").unwrap();
        }
        for i in 0..128u32 {
            writer.write_vint(i as i32).unwrap();
        }
        writer.flush().unwrap();

        let mut reader = FileReader::new(&path, 128).unwrap();
        for i in 0..128u32 {
            assert_eq!(reader.read_vint().unwrap(), i as i32);
        }
        let mut read = [0u8; 9];
        reader.read(&mut read).unwrap();
        assert_eq!(&read, b"abcabcabc");
        for i in 0..128u32 {
            assert_eq!(reader.read_vint().unwrap(), i as i32);
        }
    }

    /// C++: `file_write_read_ut.cpp:109` (`TestExceedWriterTotalSize`).
    #[test]
    fn file_writer_exceed_total_size() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file_write_bytes.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        for i in 0..1024i32 {
            writer.write_int(i).unwrap();
        }
        writer.write(b"abcabcabc").unwrap();
        writer.sync().unwrap();

        let mut reader = FileReader::new(&path, 128).unwrap();
        for i in 0..1024i32 {
            assert_eq!(reader.read_int().unwrap(), i);
        }
        let mut read = [0u8; 9];
        reader.read(&mut read).unwrap();
        assert_eq!(&read, b"abcabcabc");

        assert_eq!(writer.file_size().unwrap(), 4 * 1024 + 9);
        assert_eq!(writer.total_written_bytes(), 4 * 1024 + 9);
    }

    /// C++: `file_write_read_ut.cpp:136` (`TestFilePointerSeek`).
    #[test]
    fn file_reader_file_pointer_seek() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file_write_bytes.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        for i in 0..1024i32 {
            writer.write_int(i).unwrap();
        }
        writer.sync().unwrap();

        let mut reader = FileReader::new(&path, 128).unwrap();
        let mut mark = 0u64;
        for i in 0..1024i32 {
            if i == 254 {
                mark = reader.file_pointer();
            }
            assert_eq!(reader.read_int().unwrap(), i);
        }
        assert!(reader.finished());
        assert_eq!(mark, 254 * 4);

        reader.seek(mark);
        let mut expected = 254i32;
        while !reader.finished() {
            assert_eq!(reader.read_int().unwrap(), expected);
            expected += 1;
        }
        assert_eq!(expected, 1024);
    }

    /// C++: `file_write_read_ut.cpp:165` (`TestFileReadOverflowBuffer`).
    #[test]
    fn file_reader_read_overflow_buffer() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file_write_bytes.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        let text = "abc".repeat(1000);
        writer.write(text.as_bytes()).unwrap();
        writer.sync().unwrap();

        let mut read = vec![0u8; text.len()];
        let mut reader = FileReader::new(&path, 128).unwrap();
        reader.read(&mut read).unwrap();
        assert_eq!(read, text.as_bytes());
    }

    /// C++: `file_write_read_ut.cpp:186` (`TestFileIODataTypes`).
    #[test]
    fn file_reader_io_data_types() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_io_alltypes.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        writer.write_byte(b'a').unwrap();
        writer.write_int(4).unwrap();
        writer.write_vint(23).unwrap();
        writer.write_long(4566).unwrap();
        writer.write_vlong(45639).unwrap();
        writer.write_short(328).unwrap();
        let text = b"linelineline";
        writer.write(text).unwrap();
        writer.sync().unwrap();

        let mut reader = FileReader::new(&path, 128).unwrap();
        assert_eq!(reader.read_byte().unwrap(), b'a');
        assert_eq!(reader.read_int().unwrap(), 4);
        assert_eq!(reader.read_vint().unwrap(), 23);
        assert_eq!(reader.read_long().unwrap(), 4566);
        assert_eq!(reader.read_vlong().unwrap(), 45639);
        assert_eq!(reader.read_short().unwrap(), 328);
    }

    /// C++: `local_file_ut.cpp:36` (`test1`).
    #[test]
    fn local_file_read_write_mmap() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_local_file.txt");
        let mut handle = VirtualStore::open(&path, FileAccessMode::Write).unwrap();
        handle.append(b"abcdefghijklmnopqrstuvwxyz").unwrap();
        handle.seek(0).unwrap();

        let first = handle.read(26).unwrap();
        assert_eq!(first.len(), 26);
        for (i, byte) in first.iter().enumerate() {
            assert_eq!(*byte, b'a' + i as u8);
        }

        handle.seek(0).unwrap();
        let read = handle.read(26).unwrap();
        assert_eq!(read.len(), 26);
        for (i, byte) in read.iter().enumerate() {
            assert_eq!(*byte, b'a' + i as u8);
        }

        // Both are no-ops in the reference build too.
        let mmap_path = dir.path().join("test_mmap.txt");
        handle.mmap_read(&mmap_path).unwrap();
        handle.unmmap(&mmap_path).unwrap();
    }

    /// C++: `stream_reader_ut.cpp:23` (`TestBasicStreamIO`).
    #[test]
    fn stream_reader_reads_lines() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_streamio.abc");
        let mut writer = FileWriter::new(&path, 128).unwrap();
        let lines = [
            "hahahahha",
            "xixixixiix",
            "huhuhuhu",
            "xuxuxuxuxxu",
            "ddddd",
        ];
        for line in lines {
            writer.write(line.as_bytes()).unwrap();
            writer.write(b"\n").unwrap();
        }
        writer.sync().unwrap();

        let mut stream = StreamReader::new(&path).unwrap();
        let mut line = String::new();
        let mut i = 0;
        while stream.read_line(&mut line).unwrap() {
            assert_eq!(line, lines[i]);
            i += 1;
        }
        assert_eq!(i, 5);
    }
}

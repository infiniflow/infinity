//! `VarBuffer` / `VarBufferManager` — chunked variable-length rows for the
//! column vectors.
//!
//! C++: `src/storage/column_vector/var_buffer.cppm`, `var_buffer_impl.cpp`.
//! A `VarBuffer` is a list of chunks plus a prefix sum of their sizes; a row is
//! an `(offset, size)` pair into it. A `VarBufferManager` owns either an
//! in-memory buffer (`kBuffer`) or the one attached to an outline
//! `BufferObj` (`kNewCatalog`).
//!
//! Two reference details are reproduced:
//!
//! * `Append(..., free_success)` charges the appended bytes to the outline
//!   buffer's budget (`BufferObj::AddBufferSize`) and reports whether the pool
//!   could satisfy the request; the bytes are appended either way.
//! * `SetToCatalog` turns a plain in-memory buffer into an outline-backed one,
//!   and `save` writes the outline's file (the reference's `BufferObj::Save`).
//!   The outline holds the *serialized* buffer here — the Rust `BufferObj`
//!   payload is byte-typed, where the reference stores the `VarBuffer` object
//!   itself inside it.

use std::sync::Arc;

use infinity_common::{Error, Result};

use crate::buffer::{BufferHandle, BufferObj, BufferStatus};

/// C++: `VarBuffer`.
#[derive(Debug, Clone)]
pub struct VarBuffer {
    buffers: Vec<Vec<u8>>,
    /// `buffer_size_prefix_sum_`: `[0, len(chunk0), len(chunk0)+len(chunk1), …]`.
    prefix: Vec<usize>,
    /// C++ `buffer_obj_`: an append into an attached buffer charges its budget.
    buffer_obj: Option<Arc<BufferObj>>,
}

impl Default for VarBuffer {
    fn default() -> Self {
        Self::new()
    }
}

impl VarBuffer {
    pub fn new() -> Self {
        Self {
            buffers: Vec::new(),
            prefix: vec![0],
            buffer_obj: None,
        }
    }

    /// C++ `VarBuffer(BufferObj *buffer_obj)`: appends charge `buffer_obj`.
    pub fn with_buffer_obj(buffer_obj: Arc<BufferObj>) -> Self {
        Self {
            buffers: Vec::new(),
            prefix: vec![0],
            buffer_obj: Some(buffer_obj),
        }
    }

    /// C++ `VarBuffer::Append(data, size, free_success)` without the budget
    /// result — returns the chunk's offset.
    pub fn append(&mut self, data: &[u8]) -> usize {
        self.append_checked(data).0
    }

    /// C++ `VarBuffer::Append(data, size, &free_success)` — returns the chunk's
    /// offset and whether the outline buffer's budget request was satisfied.
    /// A buffer with no outline always reports success.
    pub fn append_checked(&mut self, data: &[u8]) -> (usize, bool) {
        let offset = *self.prefix.last().unwrap();
        self.buffers.push(data.to_vec());
        self.prefix.push(offset + data.len());
        let free_success = match &self.buffer_obj {
            Some(obj) => obj.add_buffer_size(data.len()).unwrap_or(false),
            None => true,
        };
        (offset, free_success)
    }

    /// C++ `VarBuffer::Get` — `None` for a zero-length row; an offset/size
    /// that leaves its chunk is unrecoverable.
    pub fn get(&self, offset: usize, size: usize) -> Result<Option<&[u8]>> {
        if size == 0 {
            return Ok(None);
        }
        let Some(next) = self.prefix.iter().position(|&p| p > offset) else {
            return Err(Error::Invalid(format!(
                "offset {offset} is out of range {}",
                self.prefix.last().copied().unwrap_or(0)
            )));
        };
        if next == 0 {
            return Err(Error::Invalid(format!(
                "prefix_sum[0] should be 0, but got {}",
                self.prefix[0]
            )));
        }
        let chunk = next - 1;
        let offset_in_buffer = offset - self.prefix[chunk];
        if offset_in_buffer + size > self.prefix[chunk + 1] - self.prefix[chunk] {
            return Err(Error::Invalid(format!(
                "offset {offset} and size {size} is out of range [{}, {})",
                self.prefix[chunk],
                self.prefix[chunk + 1]
            )));
        }
        let buffer = &self.buffers[chunk];
        Ok(Some(&buffer[offset_in_buffer..offset_in_buffer + size]))
    }

    /// C++ `VarBuffer::Write(ptr, size)` — concatenates whole chunks up to
    /// `limit` bytes.
    pub fn write(&self, limit: usize) -> Vec<u8> {
        let mut out = Vec::new();
        for (i, buffer) in self.buffers.iter().enumerate() {
            let buffer_size = self.prefix[i + 1] - self.prefix[i];
            if out.len() + buffer_size > limit {
                break;
            }
            out.extend_from_slice(buffer);
        }
        out
    }

    /// C++ `VarBuffer::Write(ptr, offset, size)`.
    pub fn write_range(&self, offset: usize, size: usize) -> Result<Vec<u8>> {
        Ok(self.get(offset, size)?.unwrap_or_default().to_vec())
    }

    /// C++ `VarBuffer::TotalSize`.
    pub fn total_size(&self) -> usize {
        *self.prefix.last().unwrap()
    }

    /// C++ `VarBuffer::GetSize(row_cnt)` — the prefix sum at `row_cnt`.
    pub fn get_size(&self, row_cnt: usize) -> usize {
        self.prefix
            .get(row_cnt)
            .copied()
            .unwrap_or_else(|| self.total_size())
    }
}

/// C++ `VarBufferManager::BufferType`.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum VarBufferKind {
    /// C++ `kBuffer`: an in-memory buffer with no outline object.
    Buffer,
    /// C++ `kNewCatalog`: the buffer is backed by an outline `BufferObj`.
    NewCatalog,
}

/// C++: `VarBufferManager` — a `VarBuffer` that may be attached to an outline
/// `BufferObj`, which pays for its growth out of the buffer-manager budget.
///
/// Once attached it holds the outline's [`BufferHandle`] for its lifetime, so
/// the payload stays pinned and the GC cannot free it out from under an
/// append — the reference's `std::optional<BufferHandle> buffer_handle_`.
pub struct VarBufferManager {
    kind: VarBufferKind,
    buffer: VarBuffer,
    outline: Option<Arc<BufferObj>>,
    buffer_handle: Option<BufferHandle>,
}

impl Default for VarBufferManager {
    fn default() -> Self {
        Self::new()
    }
}

impl VarBufferManager {
    /// C++ `VarBufferManager()` — a plain in-memory buffer.
    pub fn new() -> Self {
        Self {
            kind: VarBufferKind::Buffer,
            buffer: VarBuffer::new(),
            outline: None,
            buffer_handle: None,
        }
    }

    /// C++ `VarBufferManager(BufferObj *outline_buffer_obj)`.
    pub fn with_outline(outline: Arc<BufferObj>) -> Result<Self> {
        let mut manager = Self::new();
        manager.set_to_catalog(outline)?;
        Ok(manager)
    }

    pub fn kind(&self) -> VarBufferKind {
        self.kind
    }

    /// The outline buffer this manager charges, if attached.
    pub fn outline(&self) -> Option<&Arc<BufferObj>> {
        self.outline.as_ref()
    }

    /// C++ `VarBufferManager::SetToCatalog`: adopt an outline buffer, keeping
    /// the bytes appended so far.
    pub fn set_to_catalog(&mut self, outline: Arc<BufferObj>) -> Result<()> {
        if self.kind != VarBufferKind::Buffer {
            return Err(Error::Storage("Cannot convert to new catalog".into()));
        }
        self.kind = VarBufferKind::NewCatalog;
        self.buffer.buffer_obj = Some(Arc::clone(&outline));
        self.outline = Some(outline);
        self.sync_outline()?;
        Ok(())
    }

    /// C++ `VarBufferManager::Append` without the budget result.
    pub fn append(&mut self, data: &[u8]) -> Result<usize> {
        Ok(self.append_checked(data)?.0)
    }

    /// C++ `VarBufferManager::Append(data, size, &free_success)`: the offset
    /// of the new chunk and whether the outline's budget request succeeded.
    pub fn append_checked(&mut self, data: &[u8]) -> Result<(usize, bool)> {
        let appended = self.buffer.append_checked(data);
        if self.kind == VarBufferKind::NewCatalog {
            self.sync_outline()?;
        }
        Ok(appended)
    }

    /// C++ `VarBufferManager::Get`.
    pub fn get(&self, offset: usize, size: usize) -> Result<Option<&[u8]>> {
        self.buffer.get(offset, size)
    }

    /// C++ `VarBufferManager::Write(ptr)`.
    pub fn write(&self) -> Vec<u8> {
        self.buffer.write(usize::MAX)
    }

    /// C++ `VarBufferManager::Write(ptr, offset, size)`.
    pub fn write_range(&self, offset: usize, size: usize) -> Result<Vec<u8>> {
        self.buffer.write_range(offset, size)
    }

    pub fn total_size(&self) -> usize {
        self.buffer.total_size()
    }

    pub fn get_size(&self, row_cnt: usize) -> usize {
        self.buffer.get_size(row_cnt)
    }

    /// Write the chunks into the outline's payload and persist them
    /// (`BufferObj::Save`). A manager without an outline is already in memory,
    /// so there is nothing to save.
    pub fn save(&mut self) -> Result<bool> {
        let Some(outline) = self.outline.as_ref().map(Arc::clone) else {
            return Ok(false);
        };
        self.sync_outline()?;
        outline.set_data_size(self.buffer.total_size());
        outline.save()
    }

    /// Mirror the chunks into the outline payload, as the reference's
    /// `SetData(VarBuffer*)` does. A `New` outline adopts the payload without
    /// charging it — the bytes were charged by `AddBufferSize` as they were
    /// appended.
    fn sync_outline(&mut self) -> Result<()> {
        let Some(outline) = self.outline.as_ref().map(Arc::clone) else {
            return Ok(());
        };
        let bytes = self.buffer.write(usize::MAX);
        if self.buffer_handle.is_none() {
            if outline.status() == BufferStatus::New {
                outline.set_data(bytes)?;
            }
            self.buffer_handle = Some(outline.load()?);
        } else if let Some(handle) = self.buffer_handle.as_ref() {
            *handle.data_mut() = Some(bytes);
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::buffer::{BufferPool, BufferType, FileWorker};
    use std::path::PathBuf;

    /// `src/unit_test/storage/column_vector/var_buffer_ut.cpp::VarBufferTest/test1`.
    #[test]
    fn var_buffer_appends_and_roundtrips() {
        let data: Vec<u8> = (0..26).map(|i| b'a' + i).collect();
        let mut var_buffer = VarBuffer::new();

        assert_eq!(var_buffer.append(&data), 0);
        assert_eq!(var_buffer.append(&[]), 26);
        assert_eq!(var_buffer.append(&data), 26);

        let check = |var_buffer: &VarBuffer| {
            assert_eq!(var_buffer.get(0, 26).unwrap(), Some(data.as_slice()));
            assert_eq!(var_buffer.get(26, 0).unwrap(), None);
            assert_eq!(var_buffer.get(26, 26).unwrap(), Some(data.as_slice()));
            assert!(var_buffer.get(52, 26).is_err());
        };
        check(&var_buffer);

        let size = var_buffer.total_size();
        let written = var_buffer.write(size);
        assert_eq!(written.len(), size);
        assert_eq!(var_buffer.get_size(1), 26);
        assert_eq!(var_buffer.get_size(2), 26);
        assert_eq!(var_buffer.get_size(3), 52);
        // `Write(ptr, offset, size)` reads one row.
        assert_eq!(var_buffer.write_range(26, 26).unwrap(), data);

        let mut var_buffer2 = VarBuffer::new();
        var_buffer2.append(&written);
        check(&var_buffer2);
    }

    fn dirs() -> (tempfile::TempDir, PathBuf, PathBuf) {
        let dir = tempfile::tempdir().unwrap();
        let data_dir = dir.path().join("data");
        let temp_dir = dir.path().join("temp");
        std::fs::create_dir_all(&data_dir).unwrap();
        std::fs::create_dir_all(&temp_dir).unwrap();
        (dir, data_dir, temp_dir)
    }

    /// An append into an attached buffer charges its budget and reports the
    /// request's outcome, while the bytes are appended either way — the
    /// reference's `Append(data, size, &free_success)`.
    #[test]
    fn append_charges_the_outline_budget() {
        let (_dir, data_dir, temp_dir) = dirs();
        let pool = BufferPool::new(35);
        let outline = pool
            .allocate_file(FileWorker::var(&data_dir, &temp_dir, "", "outline", 0))
            .unwrap();
        let mut manager = VarBufferManager::with_outline(Arc::clone(&outline)).unwrap();
        // The manager pins the outline, so the GC can never free it and the
        // budget must come out of the pool's remaining space.
        assert_eq!(outline.rc(), 1);
        assert_eq!(outline.status(), BufferStatus::Loaded);

        let (offset, ok) = manager.append_checked(b"0123456789").unwrap();
        assert_eq!(offset, 0);
        assert!(ok);
        assert_eq!(pool.used_bytes(), 10);

        // The fourth append cannot be satisfied — but it is still appended and
        // still charged, exactly like the reference's `free_success`.
        manager.append(b"0123456789").unwrap();
        manager.append(b"0123456789").unwrap();
        let (offset, ok) = manager.append_checked(b"abcdefghij").unwrap();
        assert_eq!(offset, 30);
        assert!(!ok);
        assert_eq!(pool.used_bytes(), 40);
        assert_eq!(manager.total_size(), 40);
        assert_eq!(manager.get(30, 10).unwrap(), Some(b"abcdefghij".as_slice()));
    }

    /// C++ `VarBufferManager::SetToCatalog`: an in-memory buffer becomes
    /// outline-backed, and `save` writes the outline's file.
    #[test]
    fn set_to_catalog_attaches_an_outline_and_saves_it() {
        let (dir, data_dir, temp_dir) = dirs();
        let pool = BufferPool::new(1024);
        let mut manager = VarBufferManager::new();
        assert_eq!(manager.kind(), VarBufferKind::Buffer);
        manager.append(b"abc").unwrap();
        manager.append(b"de").unwrap();
        assert!(!manager.save().unwrap(), "no outline to save");

        let outline = pool
            .allocate_file(FileWorker::var(&data_dir, &temp_dir, "", "col_0", 0))
            .unwrap();
        manager.set_to_catalog(Arc::clone(&outline)).unwrap();
        assert_eq!(manager.kind(), VarBufferKind::NewCatalog);
        assert_eq!(manager.outline().unwrap().id(), outline.id());
        // The bytes appended before the conversion are still readable (a row
        // never spans two chunks, as in the reference).
        assert_eq!(manager.get(0, 3).unwrap(), Some(b"abc".as_slice()));
        assert_eq!(manager.get(3, 2).unwrap(), Some(b"de".as_slice()));
        assert_eq!(manager.total_size(), 5);
        // The conversion itself charged nothing; the outline's payload is a
        // mirror, not a second copy in the budget.
        assert_eq!(pool.used_bytes(), 0);

        manager.append(b"fghi").unwrap();
        assert!(manager.save().unwrap());
        assert_eq!(outline.kind(), BufferType::Persistent);
        let written = std::fs::read(data_dir.join("col_0")).unwrap();
        assert_eq!(written, b"abcdefghi");
        let _ = dir;
    }

    /// A buffer without an outline reports every append as satisfied.
    #[test]
    fn in_memory_appends_always_succeed() {
        let mut buffer = VarBuffer::new();
        let (offset, ok) = buffer.append_checked(b"xyz");
        assert_eq!(offset, 0);
        assert!(ok);
        assert_eq!(buffer.total_size(), 3);
    }

    /// `SetToCatalog` twice is the reference's "Cannot convert to new catalog".
    #[test]
    fn set_to_catalog_twice_is_rejected() {
        let (_dir, data_dir, temp_dir) = dirs();
        let pool = BufferPool::new(1024);
        let first = pool
            .allocate_file(FileWorker::var(&data_dir, &temp_dir, "", "a", 0))
            .unwrap();
        let second = pool
            .allocate_file(FileWorker::var(&data_dir, &temp_dir, "", "b", 0))
            .unwrap();
        let mut manager = VarBufferManager::new();
        manager.set_to_catalog(first).unwrap();
        assert!(manager.set_to_catalog(second).is_err());
    }
}

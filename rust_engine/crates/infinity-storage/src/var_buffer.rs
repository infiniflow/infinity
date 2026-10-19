//! `VarBuffer` — chunked variable-length rows for the column vectors.
//!
//! C++: `VarBuffer` / `VarBufferManager` (`src/storage/column_vector/var_buffer.cppm`,
//! `var_buffer_impl.cpp`). The Rust port keeps the chunk list and prefix sums;
//! the `BufferObj` sizing hook is not modelled.

use infinity_common::{Error, Result};

/// C++: `VarBuffer`.
#[derive(Debug, Clone, Default)]
pub struct VarBuffer {
    buffers: Vec<Vec<u8>>,
    /// `buffer_size_prefix_sum_`: `[0, len(chunk0), len(chunk0)+len(chunk1), …]`.
    prefix: Vec<usize>,
}

impl VarBuffer {
    pub fn new() -> Self {
        Self {
            buffers: Vec::new(),
            prefix: vec![0],
        }
    }

    /// C++: `VarBuffer::Append(data, size)` — returns the chunk's offset.
    pub fn append(&mut self, data: &[u8]) -> usize {
        let offset = *self.prefix.last().unwrap();
        self.buffers.push(data.to_vec());
        self.prefix.push(offset + data.len());
        offset
    }

    /// C++: `VarBuffer::Get` — `None` for a zero-length row; an offset/size
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

    /// C++: `VarBuffer::Write(ptr, size)` — concatenates whole chunks up to
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

    /// C++: `VarBuffer::TotalSize`.
    pub fn total_size(&self) -> usize {
        *self.prefix.last().unwrap()
    }

    /// C++: `VarBuffer::GetSize(row_cnt)` — the prefix sum at `row_cnt`.
    pub fn get_size(&self, row_cnt: usize) -> usize {
        self.prefix
            .get(row_cnt)
            .copied()
            .unwrap_or_else(|| self.total_size())
    }
}

/// C++: `VarBufferManager` — a `VarBuffer` that may spill to an outline
/// `BufferObj`. The Rust engine keeps outline bytes in memory, so the manager
/// is the buffer itself.
pub type VarBufferManager = VarBuffer;

#[cfg(test)]
mod tests {
    use super::*;

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

        let mut var_buffer2 = VarBuffer::new();
        var_buffer2.append(&written);
        check(&var_buffer2);
    }
}

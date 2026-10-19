//! Ring-buffer cursor port of `src/network/ring_buffer_iterator.cppm`.
//!
//! The C++ cursor holds a reference to a fixed `std::array<char, PG_MSG_BUFFER_SIZE>`
//! that several cursors alias. Rust cannot express aliasing `&mut` cursors without
//! `unsafe` (the crate forbids it), so the buffer is shared through
//! `Arc<RwLock<Vec<u8>>>` and the cursor only stores an index into it. All the
//! arithmetic that matters — modular advance, `Distance`, `Find`, `Copy` and the
//! three `CopyN` overloads — is a one-to-one port.

use std::sync::{Arc, RwLock};

/// `PG_MSG_BUFFER_SIZE` from `src/common/default_values.cppm`.
pub const PG_MSG_BUFFER_SIZE: usize = 4096;

fn read_guard(data: &RwLock<Vec<u8>>) -> std::sync::RwLockReadGuard<'_, Vec<u8>> {
    data.read().unwrap_or_else(|poisoned| poisoned.into_inner())
}

fn write_guard(data: &RwLock<Vec<u8>>) -> std::sync::RwLockWriteGuard<'_, Vec<u8>> {
    data.write()
        .unwrap_or_else(|poisoned| poisoned.into_inner())
}

/// A cursor over a ring buffer.
///
/// `position` is kept as a plain index like the C++ `u32 position_`; every
/// advance is taken modulo the buffer size, which is what makes the buffer wrap.
#[derive(Clone, Debug)]
pub struct RingBufferIterator {
    data: Arc<RwLock<Vec<u8>>>,
    position: u32,
}

impl RingBufferIterator {
    /// Wraps `data` in a cursor positioned at `position`.
    ///
    /// `data` must be exactly [`PG_MSG_BUFFER_SIZE`] bytes, matching the C++
    /// fixed-size array. The position is *not* normalised on construction, just
    /// like the C++ constructor.
    pub fn new(data: Arc<RwLock<Vec<u8>>>, position: u32) -> Self {
        assert_eq!(
            read_guard(&data).len(),
            PG_MSG_BUFFER_SIZE,
            "ring buffer must be PG_MSG_BUFFER_SIZE bytes"
        );
        Self { data, position }
    }

    /// Creates the backing buffer for a cursor.
    pub fn new_buffer() -> Arc<RwLock<Vec<u8>>> {
        Arc::new(RwLock::new(vec![0; PG_MSG_BUFFER_SIZE]))
    }

    pub fn position(&self) -> u32 {
        self.position
    }

    pub fn buffer(&self) -> Arc<RwLock<Vec<u8>>> {
        Arc::clone(&self.data)
    }

    /// C++ `equal`: same backing array *and* same position.
    pub fn equal(&self, other: &Self) -> bool {
        Arc::ptr_eq(&self.data, &other.data) && self.position == other.position
    }

    /// C++ `increment`.
    pub fn increment(&mut self) {
        self.increment_by(1);
    }

    /// C++ `increment(size_t n)`.
    pub fn increment_by(&mut self, n: usize) {
        self.position = ((self.position as usize + n) % PG_MSG_BUFFER_SIZE) as u32;
    }

    /// C++ `dereference`.
    pub fn dereference(&self) -> u8 {
        read_guard(&self.data)[self.position as usize]
    }

    /// Writes one byte at the current position without advancing.
    pub fn assign(&mut self, value: u8) {
        write_guard(&self.data)[self.position as usize] = value;
    }

    /// C++ `Distance`: a *signed* subtraction of the two raw indices. Note that
    /// this deliberately does not wrap: a cursor that has wrapped around yields
    /// a negative distance, exactly as in the C++ implementation.
    pub fn distance(begin: &Self, end: &Self) -> i64 {
        i64::from(end.position) - i64::from(begin.position)
    }

    /// C++ `Find`: advances `begin` until it equals `end` or `finding` is seen.
    pub fn find(begin: &Self, end: &Self, finding: u8) -> Self {
        let mut iter = begin.clone();
        while !iter.equal(end) {
            if iter.dereference() == finding {
                return iter;
            }
            iter.increment();
        }
        iter
    }

    /// C++ `Copy(begin, end, std::string&)`.
    pub fn copy_to_string(begin: &Self, end: &Self, output: &mut String) {
        output.reserve(Self::distance(begin, end).max(0) as usize);
        let mut iter = begin.clone();
        while !iter.equal(end) {
            output.push(iter.dereference() as char);
            iter.increment();
        }
    }

    /// C++ `CopyN(begin, len, char *output)`.
    pub fn copy_n_to_bytes(begin: &Self, len: usize) -> Vec<u8> {
        let mut iter = begin.clone();
        let mut output = Vec::with_capacity(len);
        for _ in 0..len {
            output.push(iter.dereference());
            iter.increment();
        }
        output
    }

    /// C++ `CopyN(begin, len, std::string&)`.
    pub fn copy_n_to_string(begin: &Self, len: usize) -> String {
        Self::copy_n_to_bytes(begin, len)
            .into_iter()
            .map(char::from)
            .collect()
    }

    /// C++ `CopyN(const char *src, size_t len, RingBufferIterator &result_iter)`.
    ///
    /// Writes `src` starting at `result.position` and wraps, leaving the cursor
    /// position untouched — the C++ code keeps the array index in a local.
    pub fn copy_n_from_slice(src: &[u8], result: &mut Self) {
        let mut buffer = write_guard(&result.data);
        let mut array_idx = result.position as usize;
        for &byte in src {
            buffer[array_idx] = byte;
            array_idx = (array_idx + 1) % PG_MSG_BUFFER_SIZE;
        }
    }

    /// The C++ `char *position_addr()` has no safe equivalent (it hands out a
    /// mutable pointer into the shared array), so callers get the index instead.
    pub fn position_index(&self) -> usize {
        self.position as usize
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn cursor(position: u32) -> RingBufferIterator {
        RingBufferIterator::new(RingBufferIterator::new_buffer(), position)
    }

    #[test]
    fn increment_wraps_at_buffer_end() {
        let mut iter = cursor((PG_MSG_BUFFER_SIZE - 1) as u32);
        iter.increment();
        assert_eq!(iter.position(), 0);

        iter.increment_by(PG_MSG_BUFFER_SIZE * 2 + 3);
        assert_eq!(iter.position(), 3);
    }

    #[test]
    fn equality_requires_the_same_buffer() {
        let a = cursor(7);
        let b = cursor(7);
        let c = a.clone();

        assert!(a.equal(&c));
        assert!(!a.equal(&b), "different backing buffers are never equal");

        let mut d = a.clone();
        d.increment();
        assert!(!a.equal(&d));
    }

    #[test]
    fn distance_is_signed_and_does_not_wrap() {
        let begin = cursor(10);
        let end = cursor(3);
        assert_eq!(RingBufferIterator::distance(&begin, &end), -7);

        let wrapped = cursor((PG_MSG_BUFFER_SIZE - 2) as u32);
        assert_eq!(RingBufferIterator::distance(&wrapped, &cursor(2)), -4092);
    }

    #[test]
    fn find_and_copy_walk_around_the_wrap() {
        let begin = cursor((PG_MSG_BUFFER_SIZE - 3) as u32);
        let mut writer = begin.clone();
        RingBufferIterator::copy_n_from_slice(b"aXbc", &mut writer);

        let end = {
            let mut iter = begin.clone();
            iter.increment_by(4);
            iter
        };

        let found = RingBufferIterator::find(&begin, &end, b'X');
        assert_eq!(found.position(), (PG_MSG_BUFFER_SIZE - 2) as u32);

        let mut text = String::new();
        RingBufferIterator::copy_to_string(&begin, &end, &mut text);
        assert_eq!(text, "aXbc");
    }

    #[test]
    fn copy_n_to_bytes_and_string_wrap() {
        let begin = cursor((PG_MSG_BUFFER_SIZE - 2) as u32);
        let mut writer = begin.clone();
        RingBufferIterator::copy_n_from_slice(b"wxyz", &mut writer);

        assert_eq!(RingBufferIterator::copy_n_to_bytes(&begin, 4), b"wxyz");
        assert_eq!(RingBufferIterator::copy_n_to_string(&begin, 4), "wxyz");
    }

    #[test]
    fn copy_n_from_slice_does_not_move_the_cursor() {
        let mut iter = cursor((PG_MSG_BUFFER_SIZE - 1) as u32);
        RingBufferIterator::copy_n_from_slice(b"ab", &mut iter);

        assert_eq!(iter.position(), (PG_MSG_BUFFER_SIZE - 1) as u32);
        assert_eq!(iter.dereference(), b'a');
        iter.increment();
        assert_eq!(iter.dereference(), b'b');
    }
}

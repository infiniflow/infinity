//! `Selection` — a u16 index vector used by the column-vector kernels.
//!
//! C++: `SelectionData` / `Selection` (`src/storage/column_vector/selection.cppm`).

use infinity_common::{Error, Result};

use crate::data_block::DEFAULT_VECTOR_SIZE;

/// C++: `SelectionData`.
#[derive(Debug, Clone, Default)]
pub struct SelectionData {
    pub data: Vec<u16>,
}

/// C++: `Selection`. An uninitialized selection reports `get(i) == i`, which is
/// how the reference's identity mapping works.
#[derive(Debug, Default)]
pub struct Selection {
    storage: Option<SelectionData>,
    latest_selection_idx: usize,
}

impl Selection {
    pub fn new() -> Self {
        Self::default()
    }

    /// C++: `Selection::Initialize(count = DEFAULT_VECTOR_SIZE)`. A count above
    /// `u16::MAX` is unrecoverable in the reference.
    pub fn initialize(&mut self, count: usize) -> Result<()> {
        if count > u16::MAX as usize {
            return Err(Error::Invalid(
                "Too large size for selection data.".to_string(),
            ));
        }
        self.storage = Some(SelectionData {
            data: vec![0; count],
        });
        self.latest_selection_idx = 0;
        Ok(())
    }

    /// C++: `Selection::Initialize()`.
    pub fn initialize_default(&mut self) -> Result<()> {
        self.initialize(DEFAULT_VECTOR_SIZE)
    }

    /// C++: `Selection::Set`.
    pub fn set(&mut self, selection_idx: usize, row_idx: usize) -> Result<()> {
        let storage = self
            .storage
            .as_mut()
            .ok_or_else(|| Error::Invalid("Selection container isn't initialized".to_string()))?;
        if selection_idx >= storage.data.len() {
            return Err(Error::Invalid(
                "Exceed the selection vector capacity.".to_string(),
            ));
        }
        storage.data[selection_idx] = u16::try_from(row_idx)
            .map_err(|_| Error::Invalid("Selection index exceeds u16".to_string()))?;
        Ok(())
    }

    /// C++: `Selection::Append`.
    pub fn append(&mut self, row_idx: usize) -> Result<()> {
        self.set(self.latest_selection_idx, row_idx)?;
        self.latest_selection_idx += 1;
        Ok(())
    }

    /// C++: `Selection::Get` — an uninitialized selection is the identity.
    pub fn get(&self, idx: usize) -> Result<usize> {
        let Some(storage) = &self.storage else {
            return Ok(idx);
        };
        if idx >= self.latest_selection_idx {
            return Err(Error::Invalid(
                "Exceed the last row of the selection vector.".to_string(),
            ));
        }
        Ok(storage.data[idx] as usize)
    }

    /// C++: `Selection::operator[]` (read).
    pub fn at(&self, idx: usize) -> Result<u16> {
        if idx >= self.latest_selection_idx {
            return Err(Error::Invalid(
                "Exceed the last row of the selection vector.".to_string(),
            ));
        }
        Ok(self
            .storage
            .as_ref()
            .map(|s| s.data[idx])
            .unwrap_or_default())
    }

    /// C++: `Selection::operator[]` (write).
    pub fn set_at(&mut self, idx: usize, value: u16) -> Result<()> {
        if idx >= self.latest_selection_idx {
            return Err(Error::Invalid(
                "Exceed the last row of the selection vector.".to_string(),
            ));
        }
        let storage = self
            .storage
            .as_mut()
            .ok_or_else(|| Error::Invalid("Selection container isn't initialized".to_string()))?;
        storage.data[idx] = value;
        Ok(())
    }

    /// C++: `Selection::Capacity`.
    pub fn capacity(&self) -> Result<usize> {
        self.storage
            .as_ref()
            .map(|s| s.data.len())
            .ok_or_else(|| Error::Invalid("Selection container isn't initialized".to_string()))
    }

    /// C++: `Selection::Size`.
    pub fn size(&self) -> Result<usize> {
        if self.storage.is_none() {
            return Err(Error::Invalid(
                "Selection container isn't initialized".to_string(),
            ));
        }
        Ok(self.latest_selection_idx)
    }

    /// C++: `Selection::Reset`.
    pub fn reset(&mut self) {
        self.storage = None;
        self.latest_selection_idx = 0;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// `src/unit_test/storage/column_vector/selection_ut.cpp::SelectionTest/test1`.
    #[test]
    fn selection_matches_the_reference() {
        let mut s = Selection::new();
        // Uninitialized: Append/Size/Capacity are errors, Get is the identity.
        assert!(s.append(1).is_err());
        assert!(s.size().is_err());
        assert!(s.capacity().is_err());
        assert!(s.initialize(u16::MAX as usize + 1).is_err());
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(s.get(i).unwrap(), i);
        }

        s.initialize_default().unwrap();
        assert_eq!(s.size().unwrap(), 0);
        assert_eq!(s.capacity().unwrap(), DEFAULT_VECTOR_SIZE);
        assert!(s.get(DEFAULT_VECTOR_SIZE).is_err());
        assert!(s.get(0).is_err());

        for i in 0..DEFAULT_VECTOR_SIZE {
            s.append(i * 2).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(s.get(i).unwrap(), 2 * i);
        }
        assert_eq!(s.size().unwrap(), DEFAULT_VECTOR_SIZE);
        for i in 0..DEFAULT_VECTOR_SIZE {
            s.set_at(i, (3 * i) as u16).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(s.get(i).unwrap(), 3 * i);
            assert_eq!(s.at(i).unwrap(), (3 * i) as u16);
        }
        assert!(s.get(DEFAULT_VECTOR_SIZE).is_err());
        for i in 0..DEFAULT_VECTOR_SIZE {
            s.set(i, 4 * i).unwrap();
        }
        for i in 0..DEFAULT_VECTOR_SIZE {
            assert_eq!(s.get(i).unwrap(), 4 * i);
        }
    }
}

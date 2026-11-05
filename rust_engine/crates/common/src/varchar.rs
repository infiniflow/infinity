//! `Varchar` storage layouts.
//!
//! C++: `src/parser/type/complex/varchar.h` declares the three structures
//! under `#pragma pack(1)`: a 13-byte inline buffer, a 5-byte prefix plus an
//! 8-byte file offset, and a 24-bit length followed by the union of the two.
//! The layout is part of the on-disk/vector format contract.

/// C++ `InlineVarchar`: `char data_[13]`.
#[repr(C, packed)]
#[derive(Clone, Copy, Default)]
pub struct InlineVarchar {
    pub data: [u8; 13],
}

/// C++ `VectorVarchar`: `char prefix_[5]; uint64_t file_offset_`.
#[repr(C, packed)]
#[derive(Clone, Copy, Default)]
pub struct VectorVarchar {
    pub prefix: [u8; 5],
    pub file_offset: u64,
}

/// C++ `Varchar`: `uint64_t length_ : 24` and a union of the two 13-byte
/// representations (16 bytes total under `#pragma pack(1)`).
#[repr(C, packed)]
#[derive(Clone, Copy, Default)]
pub struct Varchar {
    length: [u8; 3],
    data: [u8; 13],
}

impl Varchar {
    pub const INLINE_LENGTH: usize = 13;
    pub const PREFIX_LENGTH: usize = 5;
    pub const LENGTH_LIMIT: u32 = 16 * 1024 * 1024 - 1;

    pub fn new(length: u32) -> Self {
        let mut value = Self::default();
        value.set_length(length);
        value
    }

    /// The 24-bit `length_` bitfield.
    pub fn length(&self) -> u32 {
        u32::from(self.length[0])
            | (u32::from(self.length[1]) << 8)
            | (u32::from(self.length[2]) << 16)
    }

    pub fn set_length(&mut self, length: u32) {
        assert!(
            length <= Self::LENGTH_LIMIT,
            "varchar length exceeds 24 bits"
        );
        self.length = [
            (length & 0xff) as u8,
            ((length >> 8) & 0xff) as u8,
            ((length >> 16) & 0xff) as u8,
        ];
    }

    pub fn is_inlined(&self) -> bool {
        self.length() as usize <= Self::INLINE_LENGTH
    }

    pub fn inline_data(&self) -> &[u8; 13] {
        &self.data
    }
}

#[cfg(test)]
mod tests {
    use super::{InlineVarchar, Varchar, VectorVarchar};

    /// C++ `src/unit_test/parser/type/complex/varchar_ut.cpp:28
    /// VarcharTest.structure_check`.
    #[test]
    fn varchar_structure_check() {
        assert_eq!(std::mem::size_of::<InlineVarchar>(), 13);
        assert_eq!(std::mem::size_of::<VectorVarchar>(), 13);
        assert_eq!(std::mem::size_of::<Varchar>(), 16);
        assert_eq!(Varchar::LENGTH_LIMIT, 16 * 1024 * 1024 - 1);
    }
}

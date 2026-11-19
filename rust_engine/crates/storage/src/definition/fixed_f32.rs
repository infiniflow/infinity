//! A fixed-width `EMBEDDING` column slot, read as `f32`s **in place**.
//!
//! An `EMBEDDING(FLOAT, dim)` column is `ColumnLayout::Fixed(dim * 4)` bytes of
//! inline little-endian `f32` per row (`format::embedding_column_layout`), so a
//! row's vector already is the array an index build or an exact rescoring
//! wants: no text rendering, no copy, no scratch file. This is the accessor for
//! that shape, and it lives in `storage` because reinterpreting a block's bytes
//! as numbers is this crate's job (the executor forbids `unsafe`).

/// `dim` elements of `body_data`, starting at row `slot`.
///
/// `None` when the slot is outside the body, or when the body does not start on
/// an `f32` boundary - the caller then copies the cell instead of borrowing it
/// ([`copy_f32_slot`]). A block's column body is section-aligned in practice,
/// so the borrow is the normal path; the check exists because a misaligned read
/// would be undefined behaviour rather than a slow one.
#[allow(unsafe_code)]
pub fn fixed_f32_slot(body_data: &[u8], slot: usize, dim: usize) -> Option<&[f32]> {
    let width = dim.checked_mul(std::mem::size_of::<f32>())?;
    let at = slot.checked_mul(width)?;
    let bytes = body_data.get(at..at.checked_add(width)?)?;
    if !(bytes.as_ptr() as usize).is_multiple_of(std::mem::align_of::<f32>()) {
        return None;
    }
    // SAFETY: the pointer is aligned for `f32` (checked immediately above), the
    // slice is exactly `dim * 4` bytes (its bounds were checked above), it
    // borrows `body_data` for the returned lifetime, and every bit pattern is a
    // valid `f32`.
    Some(unsafe { std::slice::from_raw_parts(bytes.as_ptr().cast::<f32>(), dim) })
}

/// [`fixed_f32_slot`]'s fallback: the slot decoded into owned `f32`s.
pub fn copy_f32_slot(body_data: &[u8], slot: usize, dim: usize) -> Option<Vec<f32>> {
    let width = dim.checked_mul(std::mem::size_of::<f32>())?;
    let at = slot.checked_mul(width)?;
    let bytes = body_data.get(at..at.checked_add(width)?)?;
    Some(
        bytes
            .as_chunks::<{ std::mem::size_of::<f32>() }>()
            .0
            .iter()
            .map(|c| f32::from_le_bytes(*c))
            .collect(),
    )
}

#[cfg(test)]
mod tests {
    use super::*;

    fn body(rows: usize, dim: usize) -> Vec<u8> {
        let mut out = Vec::with_capacity(rows * dim * 4);
        for row in 0..rows {
            for d in 0..dim {
                out.extend_from_slice(&((row * 10 + d) as f32).to_le_bytes());
            }
        }
        out
    }

    #[test]
    fn a_slot_reads_back_the_floats_that_were_written() {
        let dim = 3;
        let bytes = body(2, dim);
        // The body a block hands over starts at a section boundary, which is
        // what makes the borrow legal; `Vec<u8>`'s allocation is aligned the
        // same way.
        let slot = fixed_f32_slot(&bytes, 1, dim).expect("an aligned body borrows");
        assert_eq!(slot, &[10.0, 11.0, 12.0]);
        // Past the last row of a short (tail) block: no read past the body.
        assert!(fixed_f32_slot(&bytes, 2, dim).is_none());
        assert!(copy_f32_slot(&bytes, 2, dim).is_none());
    }

    #[test]
    fn a_misaligned_body_falls_back_to_the_copy() {
        let dim = 3;
        let mut shifted = vec![0u8];
        shifted.extend_from_slice(&body(1, dim));
        let bytes = &shifted[1..];
        // `bytes` is byte-aligned here only by luck; skip the borrow check on a
        // platform/allocator that happens to align it.
        if let Some(slot) = fixed_f32_slot(bytes, 0, dim) {
            assert_eq!(slot, &[0.0, 1.0, 2.0]);
        } else {
            assert_eq!(copy_f32_slot(bytes, 0, dim).unwrap(), vec![0.0, 1.0, 2.0]);
        }
    }
}

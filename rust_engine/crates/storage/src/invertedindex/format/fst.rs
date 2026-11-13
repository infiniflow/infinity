use super::*;

/// Length of the FST region given the `u64` stored 12 bytes before the file end
/// (`dict_reader_impl.cpp:46-67`): `root_addr == 0` means an empty FST of 36
/// bytes, otherwise the FST ends at `root_addr + 21`.
pub fn fst_len_from_trailer(root_addr: u64) -> usize {
    if root_addr == 0 {
        36
    } else {
        root_addr as usize + 21
    }
}

/// FST format version written by the C++ builder (`fst/node.cppm:40`).
pub const FST_VERSION: u64 = 3;

/// Bytes of the FST trailer: `u64 len_`, `u64 root_addr`, `u32 checksum`.
pub const FST_TRAILER_LEN: usize = 20;

/// The masked CRC-32/IEEE the C++ FST writer stores and `Fst::Verify` checks
/// (`fst/writer.cppm:80-83`; `fst.cppm:134-144`):
/// `((crc >> 15) | (crc << 17)) + 0xa282ead8`.
pub fn fst_checksum(bytes: &[u8]) -> u32 {
    crate::crc::crc32_ieee(bytes)
        .rotate_right(15)
        .wrapping_add(0xa282_ead8)
}

/// The FST region starts with two `u64` header fields — `VERSION` then
/// `FstType` (`fst/node.cppm:40`; `fst.cppm:64-112`).
pub const FST_HEADER_LEN: usize = 16;

/// Reads the FST region's `(VERSION, FstType)` header (`fst.cppm:64-112`).
pub fn fst_header(fst: &[u8]) -> Result<(u64, u64), FormatError> {
    if fst.len() < FST_HEADER_LEN {
        return Err(FormatError::Truncated {
            need: FST_HEADER_LEN,
            got: fst.len(),
        });
    }
    let version = u64::from_le_bytes(fst[0..8].try_into().unwrap());
    let fst_type = u64::from_le_bytes(fst[8..16].try_into().unwrap());
    Ok((version, fst_type))
}

/// The FST sentinel for "no address" (`fst/node.cppm:43`).
pub const FST_EMPTY_ADDRESS: u64 = 0;

/// `PackSize`: the smallest number of bytes that can encode `n`
/// (`fst/bytes.cppm:107-128`).
pub fn fst_pack_size(n: u64) -> u8 {
    if n < 1 << 8 {
        1
    } else if n < 1 << 16 {
        2
    } else if n < 1 << 24 {
        3
    } else if n < 1 << 32 {
        4
    } else if n < 1 << 40 {
        5
    } else if n < 1 << 48 {
        6
    } else if n < 1 << 56 {
        7
    } else {
        8
    }
}

/// `PackUintIn`: append `n` as exactly `nbytes` little-endian bytes
/// (`fst/bytes.cppm:130-141`). `nbytes` must be in `1..=8`.
pub fn fst_pack_uint_in(out: &mut Vec<u8>, n: u64, nbytes: u8) {
    debug_assert!((1..=8).contains(&nbytes));
    out.extend_from_slice(&n.to_le_bytes()[..nbytes as usize]);
}

/// `PackUint`: append `n` in the smallest number of bytes, returning how many
/// were written (`fst/bytes.cppm:143-150`).
pub fn fst_pack_uint(out: &mut Vec<u8>, n: u64) -> u8 {
    let nbytes = fst_pack_size(n);
    fst_pack_uint_in(out, n, nbytes);
    nbytes
}

/// `UnpackUint`: read a little-endian integer of `nbytes` bytes
/// (`fst/bytes.cppm:152-...`). `nbytes` must be in `1..=8`.
pub fn fst_unpack_uint(buf: &[u8], nbytes: u8) -> Result<u64, FormatError> {
    let n = nbytes as usize;
    if !(1..=8).contains(&n) {
        return Err(FormatError::Truncated { need: n, got: 0 });
    }
    if buf.len() < n {
        return Err(FormatError::Truncated {
            need: n,
            got: buf.len(),
        });
    }
    let mut bytes = [0u8; 8];
    bytes[..n].copy_from_slice(&buf[..n]);
    Ok(u64::from_le_bytes(bytes))
}

/// `State::PackDeltaSize`: bytes needed to store `node_addr - trans_addr`
/// (`fst/node.cppm:319-327`). An empty transition address packs as the
/// sentinel `0`.
pub fn fst_pack_delta_size(node_addr: u64, trans_addr: u64) -> u8 {
    let delta = if trans_addr == FST_EMPTY_ADDRESS {
        FST_EMPTY_ADDRESS
    } else {
        node_addr.wrapping_sub(trans_addr)
    };
    fst_pack_size(delta)
}

/// `State::PackDelta` (`fst/node.cppm:303-307`): append the relative address
/// `node_addr - trans_addr`, returning the number of bytes written.
pub fn fst_pack_delta(out: &mut Vec<u8>, node_addr: u64, trans_addr: u64) -> u8 {
    let nbytes = fst_pack_delta_size(node_addr, trans_addr);
    let delta = if trans_addr == FST_EMPTY_ADDRESS {
        FST_EMPTY_ADDRESS
    } else {
        node_addr.wrapping_sub(trans_addr)
    };
    fst_pack_uint_in(out, delta, nbytes);
    nbytes
}

/// `State::UnpackDelta` (`fst/node.cppm:329-337`): recover an absolute address
/// from a packed relative one. The sentinel `0` maps back to `EMPTY_ADDRESS`.
pub fn fst_unpack_delta(buf: &[u8], nbytes: u8, node_addr: u64) -> Result<u64, FormatError> {
    let delta = fst_unpack_uint(buf, nbytes)?;
    if delta == FST_EMPTY_ADDRESS {
        Ok(FST_EMPTY_ADDRESS)
    } else {
        Ok(node_addr.wrapping_sub(delta))
    }
}

/// `COMMON_INPUTS[256]` (`fst/common_inputs.cppm:20-278`): byte -> rank.
pub const FST_COMMON_INPUTS: [u8; 256] = [
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 80, 117, 118, 79, 39, 30, 81, 75, 74,
    82, 57, 66, 16, 12, 2, 19, 20, 21, 27, 32, 29, 35, 36, 37, 34, 24, 73, 119, 23, 120, 40, 83,
    44, 48, 42, 43, 49, 46, 62, 61, 47, 69, 68, 58, 56, 55, 59, 51, 72, 54, 45, 52, 64, 65, 63, 71,
    67, 70, 77, 121, 78, 122, 31, 123, 4, 25, 9, 17, 1, 26, 22, 13, 7, 50, 38, 14, 15, 10, 3, 8,
    60, 6, 5, 0, 18, 33, 11, 41, 28, 53, 124, 125, 126, 76, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
    211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
    230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248,
    249, 250, 251, 252, 253, 254, 255,
];

/// `COMMON_INPUTS_INV[256]` (`fst/common_inputs.cppm:280-291`): rank-1 -> byte.
pub const FST_COMMON_INPUTS_INV: [u8; 256] = [
    116, 101, 47, 111, 97, 115, 114, 105, 112, 99, 110, 119, 46, 104, 108, 109, 45, 100, 117, 48,
    49, 50, 103, 61, 58, 98, 102, 51, 121, 53, 38, 95, 52, 118, 57, 54, 55, 56, 107, 37, 63, 120,
    67, 68, 65, 83, 70, 73, 66, 69, 106, 80, 84, 122, 82, 78, 77, 43, 76, 79, 113, 72, 71, 87, 85,
    86, 44, 89, 75, 74, 90, 88, 81, 59, 41, 40, 126, 91, 93, 36, 33, 39, 42, 64, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 34, 35, 60, 62, 92, 94, 96, 123, 124, 125, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154,
    155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
    174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211,
    212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    250, 251, 252, 253, 254, 255,
];

/// `State::CommonIdx` (`fst/node.cppm:282-290`): the six-bit index for `input`,
/// or `0` when its rank exceeds `max` (i.e. the input must be stored verbatim).
pub fn fst_common_idx(input: u8, max: u8) -> u8 {
    let val = FST_COMMON_INPUTS[input as usize].wrapping_add(1);
    if val > max {
        0
    } else {
        val
    }
}

/// `State::CommonInput` (`fst/node.cppm:292-301`): the byte for a stored index,
/// or `None` for the `0` sentinel meaning "input is stored verbatim".
pub fn fst_common_input(idx: u8) -> Option<u8> {
    if idx == 0 {
        None
    } else {
        Some(FST_COMMON_INPUTS_INV[idx as usize - 1])
    }
}

/// `TRANS_INDEX_THRESHOLD` (`fst/node.cppm:52`): a node with more transitions
/// than this adds a 256-byte byte -> index table.
pub const FST_TRANS_INDEX_THRESHOLD: usize = 32;

/// The `root_addr` from an FST's trailer (`fst.cppm:77`), 12 bytes before the
/// end. Does not verify the checksum.
pub fn fst_root_addr(fst: &[u8]) -> Result<u64, FormatError> {
    if fst.len() < 12 {
        return Err(FormatError::Truncated {
            need: 12,
            got: fst.len(),
        });
    }
    let at = fst.len() - 12;
    Ok(u64::from_le_bytes(fst[at..at + 8].try_into().unwrap()))
}

/// `PackSizes` (`fst/node.cppm:110-127`): the transition/output widths packed
/// into one byte (transition size in the high nibble).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub(crate) struct FstPackSizes {
    pub(crate) tsize: usize,
    pub(crate) osize: usize,
}

impl FstPackSizes {
    pub(crate) fn from_byte(b: u8) -> Self {
        Self {
            tsize: (b >> 4) as usize,
            osize: (b & 0x0f) as usize,
        }
    }
}

/// One decoded FST node (`fst/node.cppm:203-253`, `:755-795`).
pub(crate) struct FstNode<'a> {
    pub(crate) data: &'a [u8],
    /// Address of the state byte (`start_`).
    pub(crate) start: usize,
    /// The end of the node's data (`end_`), used as the delta base.
    pub(crate) end: usize,
    pub(crate) ntrans: usize,
    pub(crate) state: u8,
    pub(crate) sizes: FstPackSizes,
    pub(crate) is_final: bool,
    pub(crate) final_output: u64,
}

pub(crate) fn fst_err(need: usize, got: usize) -> FormatError {
    FormatError::Truncated { need, got }
}

/// Looks up `key` in a whole FST region (`Fst::Get`, `fst.cppm:156-181`),
/// returning its value when present.
pub fn fst_get(fst: &[u8], key: &[u8]) -> Result<Option<u64>, FormatError> {
    let mut addr = fst_root_addr(fst)?;
    let mut out = 0u64;
    for &b in key {
        let node = FstNode::new(fst, addr)?;
        let Some(ti) = node.find_input(b)? else {
            return Ok(None);
        };
        out = out.wrapping_add(node.output_at(ti)?);
        addr = node.trans_addr(ti)?;
    }
    let node = FstNode::new(fst, addr)?;
    if !node.is_final {
        return Ok(None);
    }
    Ok(Some(out.wrapping_add(node.final_output)))
}

/// All `(key, value)` pairs of an FST in lexicographic order (`FstStream`).
///
/// The reference walks the automaton lazily; this port materializes the same
/// traversal once (the FST is already fully resident in these chunks).
pub(crate) fn fst_entries(fst: &[u8]) -> Result<Vec<(Vec<u8>, u64)>, FormatError> {
    struct Frame {
        addr: u64,
        prefix: Vec<u8>,
        out: u64,
        next: usize,
    }
    let mut entries = Vec::new();
    let root = fst_root_addr(fst)?;
    let mut stack = vec![Frame {
        addr: root,
        prefix: Vec::new(),
        out: 0,
        next: 0,
    }];
    while let Some(frame) = stack.last_mut() {
        let i = frame.next;
        let node = FstNode::new(fst, frame.addr)?;
        if i >= node.ntrans {
            stack.pop();
            continue;
        }
        frame.next += 1;
        let mut prefix = frame.prefix.clone();
        prefix.push(node.input_at(i)?);
        let out = frame.out.wrapping_add(node.output_at(i)?);
        let addr = node.trans_addr(i)?;
        let child = FstNode::new(fst, addr)?;
        if child.is_final {
            entries.push((prefix.clone(), out.wrapping_add(child.final_output)));
        }
        if child.ntrans > 0 {
            stack.push(Frame {
                addr,
                prefix,
                out,
                next: 0,
            });
        }
    }
    Ok(entries)
}

/// `NONE_ADDRESS` (`fst/node.cppm:47`): never a real node address.
pub(crate) const FST_NONE_ADDRESS: usize = 1;

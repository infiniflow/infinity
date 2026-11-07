use super::*;

/// Appends `value` as an unsigned LEB128 varint (7 bits per byte, low bits
/// first, high bit = continuation).
pub fn write_vint(out: &mut Vec<u8>, mut value: u64) {
    loop {
        let byte = (value & 0x7f) as u8;
        value >>= 7;
        if value == 0 {
            out.push(byte);
            return;
        }
        out.push(byte | 0x80);
    }
}

/// Reads an unsigned LEB128 varint, returning `(value, bytes_consumed)`.
pub fn read_vint(buf: &[u8]) -> Result<(u64, usize), FormatError> {
    let mut value = 0u64;
    let mut shift = 0u32;
    for (i, byte) in buf.iter().enumerate() {
        // 10 groups of 7 bits fill a u64; a longer run is malformed.
        if shift >= 64 {
            return Err(FormatError::Truncated {
                need: i + 1,
                got: buf.len(),
            });
        }
        value |= u64::from(byte & 0x7f) << shift;
        if byte & 0x80 == 0 {
            return Ok((value, i + 1));
        }
        shift += 7;
    }
    Err(FormatError::Truncated {
        need: buf.len() + 1,
        got: buf.len(),
    })
}

/// Encodes `.len`: a bare little-endian `u32[row_cnt]`, no header
/// (`memory_indexer_impl.cpp:550-552`).
pub fn encode_column_lengths(lengths: &[u32]) -> Vec<u8> {
    let mut out = Vec::with_capacity(lengths.len() * 4);
    for l in lengths {
        out.extend_from_slice(&l.to_le_bytes());
    }
    out
}

/// Decodes `.len` (`column_length_io_impl.cpp:62-66`).
pub fn decode_column_lengths(buf: &[u8]) -> Result<Vec<u32>, FormatError> {
    if !buf.len().is_multiple_of(4) {
        return Err(FormatError::Truncated {
            need: (buf.len() / 4 + 1) * 4,
            got: buf.len(),
        });
    }
    Ok(buf
        .as_chunks::<4>()
        .0
        .iter()
        .map(|c| u32::from_le_bytes(*c))
        .collect())
}

/// Appends the trailer the C++ FST writer puts at the end of an FST region.
///
/// `Fst::Verify` computes `CRC32IEEE(data_ptr_, data_len_ - 4)` over the whole
/// FST buffer, so the checksum covers the `len_` and `root_addr` fields too and
/// must be computed after they are appended (only the 4-byte checksum itself is
/// excluded).
pub fn append_fst_trailer(fst: &mut Vec<u8>, len: u64, root_addr: u64) {
    fst.extend_from_slice(&len.to_le_bytes());
    fst.extend_from_slice(&root_addr.to_le_bytes());
    let checksum = fst_checksum(fst);
    fst.extend_from_slice(&checksum.to_le_bytes());
}

/// Reads the FST trailer and verifies the checksum, returning `(len_, root_addr)`.
pub fn verify_fst(buf: &[u8]) -> Result<(u64, u64), FormatError> {
    if buf.len() < FST_TRAILER_LEN {
        return Err(FormatError::Truncated {
            need: FST_TRAILER_LEN,
            got: buf.len(),
        });
    }
    let at = buf.len() - FST_TRAILER_LEN;
    let len = u64::from_le_bytes(buf[at..at + 8].try_into().unwrap());
    let root_addr = u64::from_le_bytes(buf[at + 8..at + 16].try_into().unwrap());
    let expected = u32::from_le_bytes(buf[at + 16..at + 20].try_into().unwrap());
    let got = fst_checksum(&buf[..buf.len() - 4]);
    if got != expected {
        return Err(FormatError::Checksum { expected, got });
    }
    Ok((len, root_addr))
}

impl<'a> FstNode<'a> {
    /// `State::NtransLen` (`fst/node.cppm:615-621`).
    pub(crate) fn ntrans_len(state: u8) -> usize {
        if state & 0x3f == 0 {
            1
        } else {
            0
        }
    }

    /// `StateOneTransNext`/`StateOneTrans` `InputLen` (`fst/node.cppm:361-366`,
    /// `:438-443`).
    pub(crate) fn input_len(state: u8) -> usize {
        if state & 0x3f == 0 {
            1
        } else {
            0
        }
    }

    /// The `Node::Node(addr, data_ptr)` constructor (`fst/node.cppm:755-795`).
    pub(crate) fn new(data: &'a [u8], addr: u64) -> Result<Self, FormatError> {
        if addr == FST_EMPTY_ADDRESS {
            // The empty node is final with a zero output.
            return Ok(Self {
                data,
                start: 0,
                end: 0,
                ntrans: 0,
                state: 0,
                sizes: FstPackSizes::default(),
                is_final: true,
                final_output: 0,
            });
        }
        let start = usize::try_from(addr).map_err(|_| fst_err(addr as usize, data.len()))?;
        if start >= data.len() {
            return Err(fst_err(start + 1, data.len()));
        }
        let data_len = start + 1;
        let state = data[start];
        let get = |i: usize| -> Result<u8, FormatError> {
            data.get(i).copied().ok_or(fst_err(i + 1, data.len()))
        };
        match state & 0b1100_0000 {
            0b1100_0000 => {
                // StateOneTransNext: one transition to the previously compiled
                // node.
                let il = Self::input_len(state);
                let end = data_len
                    .checked_sub(1 + il)
                    .ok_or(fst_err(data_len, data.len()))?;
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans: 1,
                    state,
                    sizes: FstPackSizes::default(),
                    is_final: false,
                    final_output: 0,
                })
            }
            0b1000_0000 => {
                // StateOneTrans: one transition with an output.
                let il = Self::input_len(state);
                let ntl = Self::ntrans_len(state);
                let sizes = FstPackSizes::from_byte(get(data_len - 1 - ntl - 1)?);
                let end = data_len
                    .checked_sub(1 + il + 1 + sizes.tsize + sizes.osize)
                    .ok_or(fst_err(data_len, data.len()))?;
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans: 1,
                    state,
                    sizes,
                    is_final: false,
                    final_output: 0,
                })
            }
            _ => {
                // StateAnyTrans.
                let ntl = Self::ntrans_len(state);
                let mut ntrans = (state & 0x3f) as usize;
                if ntrans == 0 {
                    let v = get(data_len - 2)?;
                    // "1" can never be a normal count, so it means 256.
                    ntrans = if v == 1 { 256 } else { v as usize };
                }
                let sizes = FstPackSizes::from_byte(get(data_len - 1 - ntl - 1)?);
                let index_size = if ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let total = ntrans + ntrans * sizes.tsize + index_size;
                let is_final = state & 0b0100_0000 != 0;
                let final_osize = if is_final { sizes.osize } else { 0 };
                let end = data_len
                    .checked_sub(1 + ntl + 1 + total + ntrans * sizes.osize + final_osize)
                    .ok_or(fst_err(data_len, data.len()))?;
                let final_output = if sizes.osize == 0 || !is_final {
                    0
                } else {
                    let at = data_len - 1 - ntl - 1 - total - ntrans * sizes.osize - sizes.osize;
                    fst_unpack_uint(&data[at..], sizes.osize as u8)?
                };
                Ok(Self {
                    data,
                    start,
                    end,
                    ntrans,
                    state,
                    sizes,
                    is_final,
                    final_output,
                })
            }
        }
    }

    /// The `i`th transition's input byte (`fst/node.cppm:370-378`, `:455-463`,
    /// `:672-676`).
    pub(crate) fn input_at(&self, i: usize) -> Result<u8, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => {
                // A common input, else the byte just before the state byte.
                if let Some(b) = fst_common_input(self.state & 0x3f) {
                    Ok(b)
                } else {
                    self.data
                        .get(self.start.wrapping_sub(1))
                        .copied()
                        .ok_or(fst_err(self.start, self.data.len()))
                }
            }
            0b1000_0000 => {
                if let Some(b) = fst_common_input(self.state & 0x3f) {
                    Ok(b)
                } else {
                    self.data
                        .get(self.start.wrapping_sub(1))
                        .copied()
                        .ok_or(fst_err(self.start, self.data.len()))
                }
            }
            _ => {
                let ntl = Self::ntrans_len(self.state);
                let index_size = if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let at =
                    self.start as isize - ntl as isize - 1 - index_size as isize - i as isize - 1;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                self.data
                    .get(at as usize)
                    .copied()
                    .ok_or(fst_err(at as usize + 1, self.data.len()))
            }
        }
    }

    /// The `i`th transition's output (`fst/node.cppm:465-472`, `:703-713`).
    pub(crate) fn output_at(&self, i: usize) -> Result<u64, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => Ok(0),
            0b1000_0000 => {
                // `OutputOf`
                if self.sizes.osize == 0 {
                    return Ok(0);
                }
                let il = Self::input_len(self.state);
                let at = self.start - il - 1 - self.sizes.tsize - self.sizes.osize;
                fst_unpack_uint(&self.data[at..], self.sizes.osize as u8)
            }
            _ => {
                if self.sizes.osize == 0 {
                    return Ok(0);
                }
                let ntl = Self::ntrans_len(self.state);
                let total = self.ntrans
                    + self.ntrans * self.sizes.tsize
                    + if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                        256
                    } else {
                        0
                    };
                let at = self.start as isize
                    - ntl as isize
                    - 1
                    - total as isize
                    - (i * self.sizes.osize) as isize
                    - self.sizes.osize as isize;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                fst_unpack_uint(&self.data[at as usize..], self.sizes.osize as u8)
            }
        }
    }

    /// The `i`th transition's target address (`fst/node.cppm:380`, `:474-478`,
    /// `:661-669`).
    pub(crate) fn trans_addr(&self, i: usize) -> Result<u64, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 => Ok(self.end as u64 - 1),
            0b1000_0000 => {
                let il = Self::input_len(self.state);
                let at = self.start - il - 1 - self.sizes.tsize;
                fst_unpack_delta(&self.data[at..], self.sizes.tsize as u8, self.end as u64)
            }
            _ => {
                let ntl = Self::ntrans_len(self.state);
                let index_size = if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    256
                } else {
                    0
                };
                let at = self.start as isize
                    - ntl as isize
                    - 1
                    - index_size as isize
                    - self.ntrans as isize
                    - (i * self.sizes.tsize) as isize
                    - self.sizes.tsize as isize;
                if at < 0 {
                    return Err(fst_err(self.start, self.data.len()));
                }
                fst_unpack_delta(
                    &self.data[at as usize..],
                    self.sizes.tsize as u8,
                    self.end as u64,
                )
            }
        }
    }

    /// `FindInput` (`fst/node.cppm:395-402`, `:493-500`, `:679-701`): the index
    /// of the transition whose input is `b`.
    pub(crate) fn find_input(&self, b: u8) -> Result<Option<usize>, FormatError> {
        match self.state & 0b1100_0000 {
            0b1100_0000 | 0b1000_0000 => {
                if self.input_at(0)? == b {
                    Ok(Some(0))
                } else {
                    Ok(None)
                }
            }
            _ => {
                if self.ntrans > FST_TRANS_INDEX_THRESHOLD {
                    let index_size = 256;
                    let start = self.start - Self::ntrans_len(self.state) - 1 - index_size;
                    let i = self.data[start + b as usize] as usize;
                    // The C++ compares `i > ntrans`; a value of `ntrans` or more
                    // marks an absent transition.
                    if i > self.ntrans {
                        Ok(None)
                    } else {
                        Ok(Some(i))
                    }
                } else {
                    if self.ntrans == 0 {
                        return Ok(None);
                    }
                    let ntl = Self::ntrans_len(self.state);
                    let start = self.start - ntl - 1 - self.ntrans;
                    for i in 0..self.ntrans {
                        if *self
                            .data
                            .get(start + i)
                            .ok_or(fst_err(start + i + 1, self.data.len()))?
                            == b
                        {
                            return Ok(Some(self.ntrans - i - 1));
                        }
                    }
                    Ok(None)
                }
            }
        }
    }
}

/// Lexicographically ordered iteration over an FST with optional bounds
/// (`FstStream`, `fst.cppm:231-330`).
pub struct FstStream {
    pub(crate) entries: Vec<(Vec<u8>, u64)>,
    pub(crate) start: usize,
    pub(crate) end: usize,
}

impl FstStream {
    /// `FstStream(fst)` with unbounded ends.
    pub fn new(fst: &[u8]) -> Result<Self, FormatError> {
        let entries = fst_entries(fst)?;
        let end = entries.len();
        Ok(Self {
            entries,
            start: 0,
            end,
        })
    }

    /// `FstStream::Reset(min, max)`: `min` is inclusive/exclusive as given,
    /// `max` is inclusive/exclusive as given; `None` is unbounded.
    pub fn reset(&mut self, min: Option<(&[u8], bool)>, max: Option<(&[u8], bool)>) {
        self.start = match min {
            None => 0,
            Some((key, inclusive)) => {
                let idx = self.entries.partition_point(|(k, _)| {
                    if inclusive {
                        k.as_slice() < key
                    } else {
                        k.as_slice() <= key
                    }
                });
                idx
            }
        };
        self.end = match max {
            None => self.entries.len(),
            Some((key, inclusive)) => self.entries.partition_point(|(k, _)| {
                if inclusive {
                    k.as_slice() <= key
                } else {
                    k.as_slice() < key
                }
            }),
        };
    }

    // `FstStream::Next` is the iterator's next item.
}

impl Iterator for FstStream {
    type Item = (Vec<u8>, u64);

    fn next(&mut self) -> Option<Self::Item> {
        if self.start >= self.end {
            return None;
        }
        let entry = self.entries[self.start].clone();
        self.start += 1;
        Some(entry)
    }
}

/// An FST output value (a `u64` with the prefix/cat/sub algebra,
/// `fst/node.cppm:67-98`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default, PartialOrd, Ord)]
pub(crate) struct FstOutput(u64);

impl FstOutput {
    pub(crate) fn value(self) -> u64 {
        self.0
    }
    pub(crate) fn is_zero(self) -> bool {
        self.0 == 0
    }
    /// `Prefix`: the minimum of the two.
    pub(crate) fn prefix(self, o: Self) -> Self {
        Self(self.0.min(o.0))
    }
    /// `Cat`: the sum of the two.
    pub(crate) fn cat(self, o: Self) -> Self {
        Self(self.0.wrapping_add(o.0))
    }
    /// `Sub`: the difference (asserted non-negative in the C++).
    pub(crate) fn sub(self, o: Self) -> Self {
        debug_assert!(self.0 >= o.0);
        Self(self.0.wrapping_sub(o.0))
    }
}

/// A transition (`fst/node.cppm:101-111`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub(crate) struct FstTransition {
    pub(crate) inp: u8,
    pub(crate) out: FstOutput,
    pub(crate) addr: usize,
}

impl Default for FstTransition {
    fn default() -> Self {
        Self {
            inp: 0,
            out: FstOutput::default(),
            addr: FST_NONE_ADDRESS,
        }
    }
}

/// A node under construction (`fst/node.cppm:143-199`), with the `Hash` the
/// registry uses.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub(crate) struct BuilderNode {
    pub(crate) is_final: bool,
    pub(crate) final_output: FstOutput,
    pub(crate) trans: Vec<FstTransition>,
}

impl BuilderNode {
    /// `BuilderNode::Hash` (`fst/node.cppm:165-183`): FNV-1a.
    pub(crate) fn hash(&self) -> u64 {
        const FNV_PRIME: u64 = 1_099_511_628_211;
        let mut h: u64 = 14_695_981_039_346_656_037;
        h = (h ^ self.is_final as u64).wrapping_mul(FNV_PRIME);
        h = (h ^ self.final_output.value()).wrapping_mul(FNV_PRIME);
        for t in &self.trans {
            h = (h ^ t.inp as u64).wrapping_mul(FNV_PRIME);
            h = (h ^ t.out.value()).wrapping_mul(FNV_PRIME);
            h = (h ^ t.addr as u64).wrapping_mul(FNV_PRIME);
        }
        h
    }
}

/// A direct-mapped node cache (`fst/registry.cppm:41-68`) used for suffix
/// sharing. A slot collides destructively, exactly as the C++ does.
pub(crate) struct Registry {
    pub(crate) table: Vec<(BuilderNode, usize)>,
    pub(crate) mask: usize,
}

impl Registry {
    pub(crate) fn new(table_size_shift: u32) -> Self {
        let n = 1usize << table_size_shift;
        Self {
            table: vec![(BuilderNode::default(), 0); n],
            mask: n - 1,
        }
    }

    /// `Registry::Find` (`fst/registry.cppm:51-61`): returns the slot and, if
    /// the slot already holds an equal node, its compiled address.
    pub(crate) fn find(&self, key: &BuilderNode) -> (usize, Option<usize>) {
        let slot = (key.hash() as usize) & self.mask;
        if self.table[slot].0 == *key {
            (slot, Some(self.table[slot].1))
        } else {
            (slot, None)
        }
    }

    /// `Registry::Insert` (`fst/registry.cppm:64-67`).
    pub(crate) fn insert(&mut self, slot: usize, key: BuilderNode, value: usize) {
        self.table[slot] = (key, value);
    }
}

/// One level of the unfinished-node stack (`fst/build.cppm:36-59`).
#[derive(Debug, Clone, Default)]
pub(crate) struct UnfinishedNode {
    pub(crate) node: BuilderNode,
    /// `last_`: `None` means `present_ == false`.
    pub(crate) last: Option<(u8, FstOutput)>,
}

impl UnfinishedNode {
    /// `LastCompiled`: append the pending transition pointing at `addr`.
    pub(crate) fn last_compiled(&mut self, addr: usize) {
        if let Some((inp, out)) = self.last.take() {
            self.node.trans.push(FstTransition { inp, out, addr });
        }
    }

    /// `AddOutputPrefix`: push a prefix onto the node's and its pending
    /// transition's outputs.
    pub(crate) fn add_output_prefix(&mut self, prefix: FstOutput) {
        if self.node.is_final {
            self.node.final_output = prefix.cat(self.node.final_output);
        }
        for t in &mut self.node.trans {
            t.out = prefix.cat(t.out);
        }
        if let Some((_, out)) = &mut self.last {
            *out = prefix.cat(*out);
        }
    }
}

/// `FstBuilder` (`fst/build.cppm:170-242`, `build_impl.cpp`).
///
/// Keys must be inserted in lexicographic order. All I/O goes to an internal
/// buffer, and the trailer is appended by `finish` (the C++ streams through a
/// `CountingWriter`; buffering here is equivalent because the trailer's
/// checksum covers every byte written before it).
pub struct FstBuilder {
    pub(crate) buf: Vec<u8>,
    pub(crate) unfinished: Vec<UnfinishedNode>,
    pub(crate) registry: Registry,
    pub(crate) last_key: Vec<u8>,
    pub(crate) last_addr: usize,
    pub(crate) len: usize,
}

impl Default for FstBuilder {
    fn default() -> Self {
        Self::new()
    }
}

impl FstBuilder {
    pub fn new() -> Self {
        let mut buf = Vec::new();
        buf.extend_from_slice(&FST_VERSION.to_le_bytes());
        buf.extend_from_slice(&0u64.to_le_bytes()); // FstType
        Self {
            buf,
            unfinished: vec![UnfinishedNode::default()],
            registry: Registry::new(16),
            last_key: Vec::new(),
            last_addr: FST_NONE_ADDRESS,
            len: 0,
        }
    }

    /// `FstBuilder::Insert` (`fst/build.cppm:223-226`).
    pub fn insert(&mut self, key: &[u8], val: u64) {
        self.check_last_key(key, true);
        self.insert_output(key, val);
    }

    /// `FstBuilder::Add` (`fst/build.cppm:209-212`): a key with a zero output.
    pub fn add(&mut self, key: &[u8]) {
        self.check_last_key(key, false);
        self.insert_output(key, 0);
    }

    /// `FstBuilder::Finish` (`build_impl.cpp:32-43`), returning the bytes.
    pub fn finish(mut self) -> Vec<u8> {
        self.compile_from(0);
        let root = self.unfinished.pop().expect("root node").node;
        let root_addr = self.compile(root);
        append_fst_trailer(&mut self.buf, self.len as u64, root_addr as u64);
        self.buf
    }

    /// `CheckLastKey` (`build_impl.cpp:106-122`).
    ///
    /// Note the C++ returns as soon as `last_` is empty *without storing the
    /// key*, so in this version `last_` stays empty forever and the ordering
    /// checks never fire. Reproduced verbatim so the builder's behaviour is
    /// identical (with a debug assert to catch genuinely bad callers).
    pub(crate) fn check_last_key(&mut self, key: &[u8], check_dupe: bool) {
        if self.last_key.is_empty() {
            return;
        }
        assert!(
            !(check_dupe && self.last_key.len() == key.len() && self.last_key == key),
            "FST duplicated key"
        );
        assert!(self.last_key.as_slice() <= key, "FST out of order key");
        self.last_key.clear();
        self.last_key.extend_from_slice(key);
    }

    /// `InsertOutput` (`build_impl.cpp:45-66`).
    pub(crate) fn insert_output(&mut self, key: &[u8], val: u64) {
        let mut out = FstOutput(val);
        if key.is_empty() {
            self.len = 1; // must be the first key
                          // SetRootOutput
            self.unfinished[0].node.is_final = true;
            self.unfinished[0].node.final_output = out;
            return;
        }
        let prefix_len = self.find_common_prefix_and_set_output(key, &mut out);
        if prefix_len == key.len() {
            // Duplicate value with no output.
            debug_assert!(out.is_zero());
            return;
        }
        self.len += 1;
        self.compile_from(prefix_len);
        self.add_suffix(&key[prefix_len..], out);
    }

    /// `FindCommonPrefixAndSetOutput` (`fst/build.cppm:121-139`).
    pub(crate) fn find_common_prefix_and_set_output(
        &mut self,
        key: &[u8],
        out: &mut FstOutput,
    ) -> usize {
        let n = self.unfinished.len();
        let common_len = key.len().min(n - 1);
        let mut i = 0;
        while i < common_len {
            let Some((inp, t_out)) = self.unfinished[i].last else {
                panic!("registry invariant: last transition must be present");
            };
            if inp != key[i] {
                break;
            }
            let common_pre = t_out.prefix(*out);
            let add_prefix = t_out.sub(common_pre);
            *out = out.sub(common_pre);
            if !add_prefix.is_zero() {
                self.unfinished[i].last = Some((inp, common_pre));
                self.unfinished[i + 1].add_output_prefix(add_prefix);
            }
            i += 1;
        }
        i
    }

    /// `AddSuffix` (`fst/build.cppm:93-107`).
    pub(crate) fn add_suffix(&mut self, bs: &[u8], out: FstOutput) {
        if bs.is_empty() {
            return;
        }
        let len = self.unfinished.len();
        assert!(self.unfinished[len - 1].last.is_none());
        self.unfinished[len - 1].last = Some((bs[0], out));
        self.unfinished
            .resize(len + bs.len() - 1, UnfinishedNode::default());
        for (i, &b) in bs.iter().enumerate().skip(1) {
            self.unfinished[len + i - 1].last = Some((b, FstOutput::default()));
        }
        // PushEmpty(true)
        self.unfinished.push(UnfinishedNode {
            node: BuilderNode {
                is_final: true,
                ..BuilderNode::default()
            },
            last: None,
        });
    }

    /// `TopLastFreeze` (`fst/build.cppm:87-91`).
    pub(crate) fn top_last_freeze(&mut self, addr: usize) {
        if let Some(top) = self.unfinished.last_mut() {
            top.last_compiled(addr);
        }
    }

    /// `CompileFrom` (`build_impl.cpp:68-81`).
    pub(crate) fn compile_from(&mut self, istate: usize) {
        let mut addr = FST_NONE_ADDRESS;
        let remained = self.unfinished.len() - (istate + 1);
        for _ in 0..remained {
            if addr != FST_NONE_ADDRESS {
                self.top_last_freeze(addr);
            }
            let node = self.unfinished.pop().expect("unfinished node").node;
            addr = self.compile(node);
        }
        self.top_last_freeze(addr);
    }

    /// `Compile` (`build_impl.cpp:83-95`).
    pub(crate) fn compile(&mut self, node: BuilderNode) -> usize {
        if node.is_final && node.trans.is_empty() && node.final_output.is_zero() {
            return FST_EMPTY_ADDRESS as usize;
        }
        let (slot, found) = self.registry.find(&node);
        if let Some(addr) = found {
            return addr;
        }
        let start_addr = self.buf.len();
        self.compile_node(&node, start_addr);
        let last_addr = self.buf.len() - 1;
        self.last_addr = last_addr;
        self.registry.insert(slot, node, last_addr);
        last_addr
    }

    /// `Node::Compile` (`fst/node.cppm:797-810`).
    pub(crate) fn compile_node(&mut self, node: &BuilderNode, addr: usize) {
        let ntrans = node.trans.len();
        if ntrans == 0 && node.is_final && node.final_output.is_zero() {
            return;
        }
        if ntrans != 1 || node.is_final {
            self.compile_any_trans(node, addr);
        } else {
            let t = node.trans[0];
            if t.addr == self.last_addr && t.out.is_zero() {
                self.compile_one_trans_next(t.inp);
            } else {
                self.compile_one_trans(addr, t);
            }
        }
    }

    /// `StateOneTransNext::Compile` (`fst/node.cppm:348-355`).
    pub(crate) fn compile_one_trans_next(&mut self, input: u8) {
        let idx = fst_common_idx(input, 0b0011_1111);
        if idx == 0 {
            self.buf.push(input);
        }
        self.buf.push(0b1100_0000 | idx);
    }

    /// `StateOneTrans::Compile` (`fst/node.cppm:413-432`).
    pub(crate) fn compile_one_trans(&mut self, addr: usize, trans: FstTransition) {
        let out = trans.out.value();
        let output_pack_size = if out != 0 {
            fst_pack_uint(&mut self.buf, out)
        } else {
            0
        };
        let trans_pack_size = fst_pack_delta(&mut self.buf, addr as u64, trans.addr as u64);
        // PackSizes: transition size in the high nibble, output in the low.
        let pack_sizes = (trans_pack_size << 4) | (output_pack_size & 0x0f);
        self.buf.push(pack_sizes);
        let idx = fst_common_idx(trans.inp, 0b0011_1111);
        if idx == 0 {
            self.buf.push(trans.inp);
        }
        self.buf.push(0b1000_0000 | idx);
    }

    /// `StateAnyTrans::Compile` (`fst/node.cppm:511-575`).
    pub(crate) fn compile_any_trans(&mut self, node: &BuilderNode, addr: usize) {
        let ntrans = node.trans.len();
        debug_assert!(ntrans <= 256);
        let mut tsize = 0u8;
        let mut osize = fst_pack_size(node.final_output.value());
        let mut any_outs = !node.final_output.is_zero();
        for t in &node.trans {
            tsize = tsize.max(fst_pack_delta_size(addr as u64, t.addr as u64));
            osize = osize.max(fst_pack_size(t.out.value()));
            any_outs = any_outs || !t.out.is_zero();
        }
        if !any_outs {
            osize = 0;
        }
        // state byte bits: final flag + ntrans (0 when > 0x3f)
        let is_final = node.is_final;
        let state_val = (if is_final { 0b0100_0000 } else { 0 })
            | (if ntrans <= 0b0011_1111 {
                ntrans as u8
            } else {
                0
            });

        if any_outs {
            if is_final {
                fst_pack_uint_in(&mut self.buf, node.final_output.value(), osize);
            }
            for t in node.trans.iter().rev() {
                fst_pack_uint_in(&mut self.buf, t.out.value(), osize);
            }
        }
        for t in node.trans.iter().rev() {
            let delta = if t.addr == FST_EMPTY_ADDRESS as usize {
                FST_EMPTY_ADDRESS
            } else {
                (addr as u64).wrapping_sub(t.addr as u64)
            };
            fst_pack_uint_in(&mut self.buf, delta, tsize);
        }
        for t in node.trans.iter().rev() {
            self.buf.push(t.inp);
        }
        if ntrans > FST_TRANS_INDEX_THRESHOLD {
            let mut index = [255u8; 256];
            for (i, t) in node.trans.iter().enumerate() {
                index[t.inp as usize] = i as u8;
            }
            self.buf.extend_from_slice(&index);
        }
        let pack_sizes = (tsize << 4) | (osize & 0x0f);
        self.buf.push(pack_sizes);
        if ntrans == 0 || ntrans > 0b0011_1111 {
            // StateNtrans() == 0, so an explicit count byte follows.
            let count = if ntrans == 256 { 1 } else { ntrans };
            fst_pack_uint_in(&mut self.buf, count as u64, 1);
        }
        self.buf.push(state_val);
    }
}

/// Builds an FST from `(key, value)` pairs, which must be sorted by key and
/// distinct (`FstBuilder`).
pub fn build_fst(entries: &[(Vec<u8>, u64)]) -> Vec<u8> {
    let mut b = FstBuilder::new();
    for (k, v) in entries {
        b.insert(k, *v);
    }
    b.finish()
}

/// `SIMDBinaryPacking::MiniBlockSize` (`simdbitpacking.h:36`).
pub const POSTING_MINI_BLOCK: usize = 128;

/// `HowManyMiniBlocks` (`simdbitpacking.h:37`): mini-blocks per bit-width header.
pub const POSTING_MINI_BLOCKS_PER_GROUP: usize = 16;

/// The bit width needed to store `v` (`maxbits`), `0` for zero.
pub(crate) fn max_bits(v: u32) -> u8 {
    (32 - v.leading_zeros()) as u8
}

/// Packs one 128-value mini-block at width `w` into `4*w` words (the lane
/// layout above).
///
/// The mirror of [`unpack_mini_block`]: one register carries the four lanes of
/// one word, so a group of four values costs one shift pair and two ORs.
pub(crate) fn pack_mini_block(values: &[u32], w: u8, out: &mut Vec<u8>) {
    let width = w as usize;
    if width == 0 {
        // Every value is zero and the block occupies no words.
        return;
    }
    // `words[4 * i..][..4]` is word `i` of the four lanes, i.e. one register.
    let mut words = vec![u32x4::splat(0); width];
    let full = values.len() / 4;
    let mut bit = 0usize;
    for group in 0..full {
        let value = u32x4::from_slice(&values[4 * group..]);
        let word_index = bit >> 5;
        let bit_off = (bit & 31) as u32;
        if bit_off == 0 {
            words[word_index] |= value;
        } else {
            words[word_index] |= value << u32x4::splat(bit_off);
            if word_index + 1 < width {
                words[word_index + 1] |= value >> u32x4::splat(32 - bit_off);
            }
        }
        bit += width;
    }
    // A mini-block is 128 values, so there is no remainder; keep the loop total
    // for any other length rather than writing a silently short block.
    for (lane, &value) in values[full * 4..].iter().enumerate() {
        let word_index = bit >> 5;
        let bit_off = (bit & 31) as u32;
        let mut word = words[word_index].to_array();
        word[lane] |= value << bit_off;
        words[word_index] = u32x4::from_array(word);
        if bit_off != 0 && word_index + 1 < width {
            let mut next = words[word_index + 1].to_array();
            next[lane] |= value >> (32 - bit_off);
            words[word_index + 1] = u32x4::from_array(next);
        }
    }
    for word in &words {
        for lane in word.to_array() {
            out.extend_from_slice(&lane.to_le_bytes());
        }
    }
}

/// Unpacks one 128-value mini-block of width `w` from `buf`, appending up to
/// `count` values.
///
/// SIMD-BP128 as the reference writes it through `SIMDBinaryPacking`: 128
/// values of `w` bits stored **vertically**, i.e. four lanes with each lane's
/// values packed back to back into its own bit stream (`words[lane],
/// words[lane + 4], words[lane + 8], ...`). One 128-bit register therefore
/// holds the same bit window of all four lanes - the four values at positions
/// `4k .. 4k + 3` - so a group costs one shift pair and one store, the shape of
/// the reference's `_mm_srli_epi32` / `_mm_or_si128` loop.
pub(crate) fn unpack_mini_block(
    buf: &[u8],
    w: u8,
    count: usize,
    out: &mut Vec<u32>,
) -> Result<(), FormatError> {
    let w = w as usize;
    let nwords = 4 * w;
    if buf.len() < nwords * 4 {
        return Err(FormatError::Truncated {
            need: nwords * 4,
            got: buf.len(),
        });
    }
    let base = out.len();
    out.resize(base + count, 0);
    if w == 0 {
        return Ok(());
    }
    // The 4 * w words are the four lanes interleaved word by word, so the four
    // words of one register hold the same bit window of every lane.
    let mut words = [0u32; 4 * 32];
    for (i, word) in words.iter_mut().enumerate().take(nwords) {
        *word = u32::from_le_bytes(buf[i * 4..i * 4 + 4].try_into().unwrap());
    }
    let mask = u32x4::splat(if w >= 32 { u32::MAX } else { (1u32 << w) - 1 });
    let mut bit = 0usize;
    for group in 0..count / 4 {
        let word_index = bit >> 5;
        let bit_off = (bit & 31) as u32;
        let lo = u32x4::from_slice(&words[4 * word_index..]);
        let value = if bit_off == 0 {
            lo
        } else if word_index + 1 < w {
            let hi = u32x4::from_slice(&words[4 * (word_index + 1)..]);
            (lo >> u32x4::splat(bit_off)) | (hi << u32x4::splat(32 - bit_off))
        } else {
            lo >> u32x4::splat(bit_off)
        };
        let start = base + 4 * group;
        out[start..start + 4].copy_from_slice(&(value & mask).to_array());
        bit += w;
    }
    // Only the last mini-block of a list can be short; its leftover values are
    // the low lanes of the position after the last full group.
    let done = count / 4 * 4;
    if done < count {
        let word_index = bit >> 5;
        let bit_off = (bit & 31) as u32;
        let wide = if w >= 32 {
            u64::from(u32::MAX)
        } else {
            (1u64 << w) - 1
        };
        for lane in 0..count - done {
            let lo = u64::from(words[lane + 4 * word_index]);
            let hi = if word_index + 1 < w {
                u64::from(words[lane + 4 * (word_index + 1)])
            } else {
                0
            };
            out[base + done + lane] = (((lo | (hi << 32)) >> bit_off) & wide) as u32;
        }
    }
    Ok(())
}

/// `VariableByte::encodeToByteArray` (`variablebyte.h:64-...`): 7-bit groups,
/// LSB first, high bit set on the last byte, then zero-padded to 32 bits.
pub(crate) fn vb_encode(values: &[u32], out: &mut Vec<u8>) {
    for &v in values {
        let mut val = v as u64;
        loop {
            let byte = (val & 0x7f) as u8;
            val >>= 7;
            if val == 0 {
                out.push(byte | 0x80);
                break;
            }
            out.push(byte);
        }
    }
    while !out.len().is_multiple_of(4) {
        out.push(0);
    }
}

/// `VariableByte::decodeArray`: reads `count` values, returning how many bytes
/// were consumed.
pub(crate) fn vb_decode(buf: &[u8], count: usize) -> Result<(Vec<u32>, usize), FormatError> {
    let mut out = Vec::with_capacity(count);
    let mut pos = 0usize;
    for _ in 0..count {
        let mut value = 0u64;
        let mut shift = 0u32;
        loop {
            let byte = *buf.get(pos).ok_or(FormatError::Truncated {
                need: pos + 1,
                got: buf.len(),
            })?;
            pos += 1;
            value |= ((byte & 0x7f) as u64) << shift;
            if byte & 0x80 != 0 {
                break;
            }
            shift += 7;
        }
        out.push(value as u32);
    }
    Ok((out, pos))
}

/// Encodes a u32 posting field with the C++ `SIMDBinaryPacking + VariableByte`
/// composite: whole 128-value blocks go through SIMD-BP128, the tail through
/// variable-byte.
pub fn encode_u32_block(values: &[u32]) -> Vec<u8> {
    let mut out = Vec::new();
    let simd_len = values.len() / POSTING_MINI_BLOCK * POSTING_MINI_BLOCK;
    // SIMDBinaryPacking always writes its own length first (0 for no blocks).
    out.extend_from_slice(&(simd_len as u32).to_le_bytes());
    let nblocks = simd_len / POSTING_MINI_BLOCK;
    let mut b = 0usize;
    while b < nblocks {
        let group = (nblocks - b).min(POSTING_MINI_BLOCKS_PER_GROUP);
        let mut widths = [0u8; POSTING_MINI_BLOCKS_PER_GROUP];
        for (i, w) in widths.iter_mut().enumerate().take(group) {
            let mb = &values[(b + i) * POSTING_MINI_BLOCK..(b + i + 1) * POSTING_MINI_BLOCK];
            *w = mb.iter().map(|v| max_bits(*v)).max().unwrap_or(0);
        }
        // Four words, each packing four widths big-endian.
        for g in 0..4 {
            let w = ((widths[4 * g] as u32) << 24)
                | ((widths[4 * g + 1] as u32) << 16)
                | ((widths[4 * g + 2] as u32) << 8)
                | widths[4 * g + 3] as u32;
            out.extend_from_slice(&w.to_le_bytes());
        }
        for (i, &w) in widths.iter().enumerate().take(group) {
            let mb = &values[(b + i) * POSTING_MINI_BLOCK..(b + i + 1) * POSTING_MINI_BLOCK];
            pack_mini_block(mb, w, &mut out);
        }
        b += group;
    }
    if values.len() > simd_len {
        vb_encode(&values[simd_len..], &mut out);
    }
    out
}

/// Decodes a u32 posting field, returning the decoded values and how many bytes
/// were consumed (`IntEncoder::Decode` needs `expected` because the SIMD/`VB`
/// split is not self-describing).
pub fn decode_u32_block(buf: &[u8], expected: usize) -> Result<(Vec<u32>, usize), FormatError> {
    if buf.len() < 4 {
        return Err(FormatError::Truncated {
            need: 4,
            got: buf.len(),
        });
    }
    let simd_len = u32::from_le_bytes(buf[0..4].try_into().unwrap()) as usize;
    let mut pos = 4usize;
    let mut out = Vec::with_capacity(expected);
    let nblocks = simd_len / POSTING_MINI_BLOCK;
    let mut b = 0usize;
    while out.len() < simd_len {
        let group = (nblocks - b).min(POSTING_MINI_BLOCKS_PER_GROUP);
        // Four header words carry the 16 widths.
        if pos + 16 > buf.len() {
            return Err(FormatError::Truncated {
                need: pos + 16,
                got: buf.len(),
            });
        }
        let mut widths = [0u8; POSTING_MINI_BLOCKS_PER_GROUP];
        for g in 0..4 {
            let w = u32::from_le_bytes(buf[pos + 4 * g..pos + 4 * g + 4].try_into().unwrap());
            widths[4 * g] = (w >> 24) as u8;
            widths[4 * g + 1] = (w >> 16) as u8;
            widths[4 * g + 2] = (w >> 8) as u8;
            widths[4 * g + 3] = w as u8;
        }
        pos += 16;
        for &w in widths.iter().take(group) {
            let count = (simd_len - out.len()).min(POSTING_MINI_BLOCK);
            let nbytes = 4 * w as usize * 4;
            if pos + nbytes > buf.len() {
                return Err(FormatError::Truncated {
                    need: pos + nbytes,
                    got: buf.len(),
                });
            }
            unpack_mini_block(&buf[pos..pos + nbytes], w, count, &mut out)?;
            pos += nbytes;
        }
        b += group;
    }
    // The remaining values are variable-byte encoded.
    if expected > simd_len {
        let (tail, used) = vb_decode(&buf[pos..], expected - simd_len)?;
        out.extend_from_slice(&tail);
        // `VariableByte` zero-pads to a 32-bit boundary, and the C++ reader
        // consumes the padded span.
        pos = (pos + used).div_ceil(4) * 4;
    }
    Ok((out, pos))
}

/// Decodes one `CompositeCodec<SIMDBinaryPacking, VariableByte>` payload whose
/// length is already known (the `IntEncoder<u32, SIMDBitPacking>` u8 frame): the
/// SIMD value count is the first word, and the variable-byte tail is
/// self-delimiting. `VariableByte` zero-pads to a 32-bit boundary; padding bytes
/// terminate no value, so decoding stops at the payload end without producing
/// spurious zeros (`variablebyte.h:202-284`).
pub(crate) fn decode_u32_block_all(payload: &[u8]) -> Result<Vec<u32>, FormatError> {
    if payload.len() < 4 {
        return Err(FormatError::Truncated {
            need: 4,
            got: payload.len(),
        });
    }
    let simd_len = u32::from_le_bytes(payload[0..4].try_into().unwrap()) as usize;
    let (mut out, used) = decode_u32_block(payload, simd_len)?;
    let mut pos = used;
    while pos < payload.len() {
        let mut value = 0u64;
        let mut shift = 0u32;
        loop {
            let Some(&byte) = payload.get(pos) else {
                // Trailing `needPaddingTo32Bits` zero bytes: no value.
                return Ok(out);
            };
            pos += 1;
            value |= u64::from(byte & 0x7f) << shift;
            if byte & 0x80 != 0 {
                out.push(value as u32);
                break;
            }
            shift += 7;
            if shift >= 64 {
                return Err(FormatError::Truncated {
                    need: pos,
                    got: payload.len(),
                });
            }
        }
    }
    Ok(out)
}

/// `NewPForDeltaCompressor::BLOCK_SIZE` (`new_pfordelta_compressor.h:51`).
pub const NPF_BLOCK_SIZE: usize = 128;

/// `S9Compressor`'s field widths and counts (`s9compressor.h:59-60`).
pub(crate) const S9_BIT_LENGTH: [u32; 9] = [1, 2, 3, 4, 5, 7, 9, 14, 28];

pub(crate) const S9_CODE_NUM: [usize; 9] = [28, 14, 9, 7, 5, 4, 3, 2, 1];

/// The six "special frame" markers (`firstExceptionPos`,
/// `new_pfordelta_compressor.h:55-64`).
pub(crate) const NPF_SAME_INLINE: u32 = 255;

pub(crate) const NPF_SAME_ALONE: u32 = 254;

pub(crate) const NPF_INCREASE_INLINE: u32 = 253;

pub(crate) const NPF_INCREASE_ALONE: u32 = 252;

pub(crate) const NPF_DECREASE_INLINE: u32 = 251;

pub(crate) const NPF_DECREASE_ALONE: u32 = 250;

/// Header bitfields (`NewPForDeltaCompressorHeader`, `:30-42`).
#[derive(Debug, Clone, Copy)]
pub(crate) struct NpfHeader {
    pub(crate) is_last: bool,
    pub(crate) exception_range: u32,
    pub(crate) frame_bits: u32,
    pub(crate) first_exception_pos: u32,
    pub(crate) data_num: usize,
}

pub(crate) fn npf_header(x: u32) -> NpfHeader {
    let raw = (x >> 12) & 0x1f;
    NpfHeader {
        is_last: x & 1 != 0,
        exception_range: (x >> 1) & 0x7ff,
        frame_bits: if raw == 31 { 32 } else { raw },
        first_exception_pos: (x >> 17) & 0xff,
        data_num: ((x >> 25) & 0x7f) as usize + 1,
    }
}

/// `S9Decode` (`new_pfordelta_compressor.h`): ORs the exception magnitudes back
/// into `dest`, starting at `first_exception_pos` (already `fep - 1`).
///
/// The stream alternates exception-value and gap fields; the top four bits of
/// each word pick a case (0..8 starts with a value, 16..24 with a gap), and
/// `head` carries the alternation across words.
pub(crate) fn s9_apply(
    dest: &mut [u16],
    words: &[u32],
    first_exception_pos: usize,
    frame_bits: u32,
) {
    let mut cur = first_exception_pos;
    let mut head = 0u32;
    for &val in words {
        let c = (val >> 28) + head;
        let idx = (c % 16) as usize;
        let b = S9_BIT_LENGTH[idx];
        let n = S9_CODE_NUM[idx];
        let mut want_value = c < 16;
        for j in 0..n {
            let field = (val >> ((n - 1 - j) as u32 * b)) & ((1u32 << b) - 1);
            if want_value {
                if cur < dest.len() {
                    dest[cur] |= (field.wrapping_shl(frame_bits) & 0xffff) as u16;
                }
                want_value = false;
            } else {
                cur += field as usize + 1;
                want_value = true;
            }
        }
        head = if want_value { 0 } else { 16 };
    }
}

/// Decodes a u16 posting field (`NewPForDeltaCompressor::Decompress`), returning
/// the values and how many bytes were consumed. `expected` is the value count
/// because the last block is only marked by a header bit.
pub fn decode_u16_block(buf: &[u8], expected: usize) -> Result<(Vec<u16>, usize), FormatError> {
    let mut pos = 0usize;
    let mut out: Vec<u16> = Vec::with_capacity(expected);
    loop {
        if pos + 4 > buf.len() {
            return Err(FormatError::Truncated {
                need: pos + 4,
                got: buf.len(),
            });
        }
        let header = npf_header(u32::from_le_bytes(buf[pos..pos + 4].try_into().unwrap()));
        pos += 4;
        let word = |i: usize| -> Result<u32, FormatError> {
            // `TryDecompressSpecialBlock` reads the "alone" value at
            // `src + HEADER_INT_SIZE`, i.e. the word *after* the header.
            let at = pos + 4 * i;
            buf.get(at..at + 4)
                .map(|s| u32::from_le_bytes(s.try_into().unwrap()))
                .ok_or(FormatError::Truncated {
                    need: at + 4,
                    got: buf.len(),
                })
        };
        let dn = header.data_num;
        let mut block = vec![0u16; dn];
        match header.first_exception_pos {
            NPF_SAME_INLINE => {
                block.fill(header.exception_range as u16);
            }
            NPF_SAME_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                block.fill(v);
            }
            NPF_INCREASE_INLINE => {
                for (i, v) in block.iter_mut().enumerate() {
                    *v = (header.exception_range as u16).wrapping_add(i as u16);
                }
            }
            NPF_INCREASE_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                for (i, d) in block.iter_mut().enumerate() {
                    *d = v.wrapping_add(i as u16);
                }
            }
            NPF_DECREASE_INLINE => {
                // `dest[dataNum-1-i] = value + i`, i.e. the last value is the base.
                for (i, v) in block.iter_mut().rev().enumerate() {
                    *v = (header.exception_range as u16).wrapping_add(i as u16);
                }
            }
            NPF_DECREASE_ALONE => {
                let v = word(0)? as u16;
                pos += 4;
                for (i, d) in block.iter_mut().rev().enumerate() {
                    *d = v.wrapping_add(i as u16);
                }
            }
            fep => {
                // General frame: `dn` packed values, then the exception words.
                let fb = header.frame_bits;
                let frame_words = (dn * fb as usize).div_ceil(32);
                let nbytes = frame_words * 4;
                if pos + nbytes > buf.len() {
                    return Err(FormatError::Truncated {
                        need: pos + nbytes,
                        got: buf.len(),
                    });
                }
                unpack_n_u16(&buf[pos..pos + nbytes], fb, dn, &mut block)?;
                pos += nbytes;
                if fep != 0 {
                    // `exceptionIntRange` words of S9-encoded exceptions.
                    let exc = header.exception_range as usize;
                    if pos + exc * 4 > buf.len() {
                        return Err(FormatError::Truncated {
                            need: pos + exc * 4,
                            got: buf.len(),
                        });
                    }
                    let words: Vec<u32> = (0..exc)
                        .map(|i| {
                            u32::from_le_bytes(
                                buf[pos + 4 * i..pos + 4 * i + 4].try_into().unwrap(),
                            )
                        })
                        .collect();
                    s9_apply(&mut block, &words, fep as usize - 1, fb);
                    pos += exc * 4;
                }
            }
        }
        out.extend_from_slice(&block);
        if header.is_last || out.len() >= expected {
            break;
        }
    }
    Ok((out, pos))
}

/// `unpack_N<T>` (`unpack.h`): `n` values of `bits` bits each, packed LSB-first
/// and read sequentially (the 32-value main loop plus the unaligned tail is
/// equivalent to one naive pass).
pub(crate) fn unpack_n_u16(
    buf: &[u8],
    bits: u32,
    n: usize,
    out: &mut [u16],
) -> Result<(), FormatError> {
    if bits == 0 {
        return Ok(());
    }
    for (j, o) in out.iter_mut().enumerate().take(n) {
        let bit = j as u64 * bits as u64;
        let word = (bit / 32) as usize;
        let off = (bit % 32) as u32;
        let at = word * 4;
        let lo = buf.get(at..at + 4).ok_or(FormatError::Truncated {
            need: at + 4,
            got: buf.len(),
        })?;
        let mut v = u32::from_le_bytes(lo.try_into().unwrap()) >> off;
        // A field may straddle a word boundary.
        if off + bits > 32 {
            let hi = buf.get(at + 4..at + 8).ok_or(FormatError::Truncated {
                need: at + 8,
                got: buf.len(),
            })?;
            let hi = u32::from_le_bytes(hi.try_into().unwrap());
            v |= hi << (32 - off);
        }
        *o = (v & ((1u32 << bits) - 1)) as u16;
    }
    Ok(())
}

/// `HighBitIdx` (`new_pfordelta_compressor.h`): the number of significant bits
/// (`0` for zero).
pub(crate) fn npf_high_bit(value: u16) -> u32 {
    16 - value.leading_zeros()
}

/// `MakeHeader` (`:670-687`).
pub(crate) fn npf_make_header(
    frame_bits: u32,
    first_exception_pos: u32,
    code_len: usize,
    exception_int_range: u32,
    is_last_block: bool,
) -> u32 {
    let fb = if frame_bits >= 32 { 31 } else { frame_bits };
    ((code_len as u32 - 1) & 0x7f) << 25
        | (first_exception_pos & 0xff) << 17
        | (fb & 0x1f) << 12
        | (exception_int_range & 0x7ff) << 1
        | u32::from(is_last_block)
}

/// `SelectCompressBits` (`:228-309`): the frame width and either the exception
/// count or a special-frame marker.
pub(crate) fn npf_select_bits(block: &[u16]) -> (u32, u32) {
    // `FRAME_BIT_MAP[i] == i` except for 31/32 which map to 32.
    let frame_bit_map = |i: usize| -> u32 {
        if i >= 31 {
            32
        } else {
            i as u32
        }
    };
    let mut num_bit_array = [0u32; 33];
    let first_value = block[0];
    let mut is_all_same = true;
    let mut is_increase = true;
    let mut is_decrease = true;
    let mut max_bits = 0u32;
    for (i, &v) in block.iter().enumerate() {
        is_all_same &= v == first_value;
        is_increase &= v == first_value.wrapping_add(i as u16);
        is_decrease &= v == first_value.wrapping_sub(i as u16);
        let hb = npf_high_bit(v);
        max_bits = max_bits.max(hb);
        num_bit_array[hb as usize] += 1;
    }
    if max_bits == 0 {
        return (0, 0);
    }
    let inline = |v: u16| -> u32 {
        if (v as u32) <= 2047 {
            0
        } else {
            1
        }
    };
    if is_all_same {
        return (0, NPF_SAME_INLINE - inline(first_value));
    }
    if is_increase {
        return (0, NPF_INCREASE_INLINE - inline(first_value));
    }
    if is_decrease {
        let base = block[block.len() - 1];
        return (0, NPF_DECREASE_INLINE - inline(base));
    }
    for j in 1..33 {
        num_bit_array[j] += num_bit_array[j - 1];
    }
    let max_exception_num = (block.len() as u32 * 10) / 100;
    let mut num_exceptions = 0u32;
    let mut best_frame_bits = 32u32;
    let mut best_total_bytes = 4 * block.len() as u32;
    let mut i = frame_bit_map(max_bits as usize) as i32;
    while i >= 0 {
        if frame_bit_map(i as usize) != i as u32 {
            i -= 1;
            continue;
        }
        let frame_bits = i as u32;
        let exception_num = block.len() as u32 - num_bit_array[i as usize];
        if exception_num > max_exception_num || max_bits > frame_bits + 28 {
            break;
        }
        let exception_bits = max_bits - frame_bits;
        let bytes_per_exception = if exception_bits <= 7 {
            2
        } else if exception_bits <= 9 {
            3
        } else if exception_bits <= 14 {
            4
        } else {
            8
        };
        let bytes_for_frame = (block.len() as u32 * frame_bits).div_ceil(32) * 4;
        let bytes_for_exception = (exception_num * bytes_per_exception).div_ceil(4) * 4;
        if bytes_for_exception + bytes_for_frame < best_total_bytes {
            best_total_bytes = bytes_for_exception + bytes_for_frame;
            best_frame_bits = frame_bits;
            num_exceptions = exception_num;
        }
        i -= 1;
    }
    (best_frame_bits, num_exceptions)
}

/// `Pack` (`:453-466`) / `pack_N<T>`: the LSB-first sequential inverse of
/// [`unpack_n_u16`].
pub(crate) fn npf_pack(out: &mut Vec<u8>, code: &[u16], frame_bits: u32) {
    if frame_bits == 0 {
        return;
    }
    let nwords = (code.len() * frame_bits as usize).div_ceil(32);
    let mut words = vec![0u32; nwords];
    let mask = if frame_bits >= 32 {
        u32::MAX
    } else {
        (1u32 << frame_bits) - 1
    };
    for (j, &v) in code.iter().enumerate() {
        let bit = j as u64 * frame_bits as u64;
        let w = (bit / 32) as usize;
        let off = (bit % 32) as u32;
        let masked = (v as u32) & mask;
        words[w] |= masked << off;
        if off + frame_bits > 32 && w + 1 < nwords {
            words[w + 1] |= masked >> (32 - off);
        }
    }
    for w in words {
        out.extend_from_slice(&w.to_le_bytes());
    }
}

/// `S9Compressor::Encode` (`s9compressor.h:68-110`).
pub(crate) fn s9_encode(src: &[u32]) -> Vec<u32> {
    let mut out = Vec::new();
    let mut cur = 0usize;
    while cur < src.len() {
        for k in 0..9usize {
            let cur_size = S9_CODE_NUM[k];
            if src.len() < cur + cur_size {
                continue;
            }
            let b = S9_BIT_LENGTH[k];
            let mut res = 0u32;
            let mut i = 0usize;
            while i < cur_size && src[cur + i] < (1u32 << b) {
                res = (res << b) + src[cur + i];
                i += 1;
            }
            if i == cur_size {
                out.push(res | ((k as u32) << 28));
                cur += cur_size;
                break;
            }
            assert!(k < 8, "S9 exception value exceeds 28 bits");
        }
    }
    out
}

/// `TransformToFrame` (`:414-451`).
#[allow(clippy::too_many_arguments)]
pub(crate) fn npf_transform_to_frame(
    out: &mut Vec<u8>,
    frame_bits: u32,
    first_exception_pos: usize,
    code: &[u16],
    exceptions: &[u32],
    exception_offsets: &[u32],
    num_exceptions: usize,
    is_last_block: bool,
) {
    let header = |exc_range: u32| {
        npf_make_header(
            frame_bits,
            first_exception_pos as u32,
            code.len(),
            exc_range,
            is_last_block,
        )
    };
    if first_exception_pos == 0 {
        out.extend_from_slice(&header(0).to_le_bytes());
        npf_pack(out, code, frame_bits);
        return;
    }
    // Interleave exception values and offsets, then S9-encode them.
    let mut datum = vec![0u32; num_exceptions * 2 - 1];
    for i in 0..num_exceptions {
        datum[2 * i] = exceptions[i];
    }
    for i in 0..num_exceptions - 1 {
        datum[2 * i + 1] = exception_offsets[i];
    }
    let exc_words = s9_encode(&datum);
    out.extend_from_slice(&header(exc_words.len() as u32).to_le_bytes());
    npf_pack(out, code, frame_bits);
    for w in exc_words {
        out.extend_from_slice(&w.to_le_bytes());
    }
}

/// `TransformToSpecialFrame` (`:380-405`).
pub(crate) fn npf_transform_to_special_frame(
    out: &mut Vec<u8>,
    block: &[u16],
    marker: u32,
    is_last_block: bool,
) {
    let store_alone = matches!(
        marker,
        NPF_SAME_ALONE | NPF_INCREASE_ALONE | NPF_DECREASE_ALONE
    );
    let mut value = block[0];
    if matches!(marker, NPF_DECREASE_INLINE | NPF_DECREASE_ALONE) {
        value = block[block.len() - 1];
    }
    let header = npf_make_header(
        0,
        marker,
        block.len(),
        if store_alone { 0 } else { value as u32 },
        is_last_block,
    );
    out.extend_from_slice(&header.to_le_bytes());
    if store_alone {
        out.extend_from_slice(&(value as u32).to_le_bytes());
    }
}

/// `CompressBlock` (`:321-368`).
pub(crate) fn npf_compress_block(
    out: &mut Vec<u8>,
    block: &[u16],
    frame_bits: u32,
    num_exceptions: u32,
    is_last_block: bool,
) {
    if num_exceptions == 0 {
        npf_transform_to_frame(out, frame_bits, 0, block, &[], &[], 0, is_last_block);
        return;
    }
    if matches!(
        num_exceptions,
        NPF_SAME_ALONE
            | NPF_SAME_INLINE
            | NPF_INCREASE_ALONE
            | NPF_INCREASE_INLINE
            | NPF_DECREASE_ALONE
            | NPF_DECREASE_INLINE
    ) {
        npf_transform_to_special_frame(out, block, num_exceptions, is_last_block);
        return;
    }
    // Split each value into the low `frame_bits` (the frame) and the rest (the
    // exception), recording the positions that need patching.
    let mask = if frame_bits >= 32 {
        u32::MAX
    } else {
        (1u32 << frame_bits) - 1
    };
    let max = (1u32 << frame_bits) as u16;
    let n = block.len();
    let mut code = block.to_vec();
    let mut miss = Vec::new();
    for (i, &v) in block.iter().enumerate() {
        if max <= v {
            miss.push(i);
        }
    }
    debug_assert_eq!(miss.len(), num_exceptions as usize);
    let mut exceptions = vec![0u32; num_exceptions as usize];
    let mut offsets = vec![0u32; num_exceptions.saturating_sub(1) as usize];
    for (k, &m) in miss.iter().enumerate() {
        exceptions[k] = (code[m] as u32) >> frame_bits;
        code[m] = ((code[m] as u32) & mask) as u16;
        if k > 0 {
            offsets[k - 1] = (m - miss[k - 1] - 1) as u32;
        }
    }
    let first_exception_pos = miss[0] + 1;
    let _ = n;
    npf_transform_to_frame(
        out,
        frame_bits,
        first_exception_pos,
        &code,
        &exceptions,
        &offsets,
        num_exceptions as usize,
        is_last_block,
    );
}

/// Encodes a u16 posting field (`NewPForDeltaCompressor::Compress(..., true)`).
pub fn encode_u16_block(values: &[u16]) -> Vec<u8> {
    let mut out = Vec::new();
    let total = values.len().div_ceil(NPF_BLOCK_SIZE);
    for blk in 0..total {
        let off = blk * NPF_BLOCK_SIZE;
        let len = (values.len() - off).min(NPF_BLOCK_SIZE);
        let block = &values[off..off + len];
        let (frame_bits, num_exceptions) = npf_select_bits(block);
        npf_compress_block(
            &mut out,
            block,
            frame_bits,
            num_exceptions,
            blk + 1 == total,
        );
    }
    out
}

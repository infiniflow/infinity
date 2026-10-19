//! Binary fuse filter (C++ `src/storage/fast_rough_filter/binary_fuse_filter.cppm`).
//!
//! The wrapper tracks a build timestamp and a ready flag exactly like the
//! reference: before a filter is built (or when the query snapshot predates the
//! build) every lookup answers `true`, so pruning never drops a visible row.
//! The filter itself is a faithful port of the arity-3 fuse-8 construction in
//! `third_party/xor_singleheader/include/binaryfusefilter.h`.

use std::sync::atomic::{AtomicBool, Ordering};

use infinity_common::{Error, Result};

/// `UNCOMMIT_TS` (`src/common/default_values.cppm:83`).
pub const UNCOMMIT_TS: u64 = u64::MAX;

/// `XOR_MAX_ITERATIONS`.
const MAX_ITERATIONS: usize = 100;

/// `binary_fuse8_t`.
#[derive(Clone, Debug)]
struct Fuse8 {
    seed: u64,
    segment_length: u32,
    segment_length_mask: u32,
    segment_count: u32,
    segment_count_length: u32,
    array_length: u32,
    fingerprints: Vec<u8>,
}

fn mulhi(a: u64, b: u64) -> u64 {
    ((a as u128 * b as u128) >> 64) as u64
}

fn murmur64(mut h: u64) -> u64 {
    h ^= h >> 33;
    h = h.wrapping_mul(0xff51_afd7_ed55_8ccd);
    h ^= h >> 33;
    h = h.wrapping_mul(0xc4ce_b9fe_1a85_ec53);
    h ^= h >> 33;
    h
}

fn mix_split(key: u64, seed: u64) -> u64 {
    murmur64(key.wrapping_add(seed))
}

fn fingerprint(hash: u64) -> u8 {
    (hash ^ (hash >> 32)) as u8
}

fn splitmix64(seed: &mut u64) -> u64 {
    *seed = seed.wrapping_add(0x9E37_79B9_7F4A_7C15);
    let mut z = *seed;
    z = (z ^ (z >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
    z = (z ^ (z >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
    z ^ (z >> 31)
}

fn calculate_segment_length(arity: u32, size: u32) -> u32 {
    if arity == 3 {
        1u32 << ((size as f64).ln() / 3.33f64.ln() + 2.25).floor() as u32
    } else if arity == 4 {
        1u32 << ((size as f64).ln() / 2.91f64.ln() - 0.5).floor() as u32
    } else {
        65536
    }
}

fn calculate_size_factor(arity: u32, size: u32) -> f64 {
    if arity == 3 {
        (1.125f64).max(0.875 + 0.25 * (1_000_000f64).ln() / (size as f64).ln())
    } else if arity == 4 {
        (1.075f64).max(0.77 + 0.305 * (600_000f64).ln() / (size as f64).ln())
    } else {
        2.0
    }
}

/// `binary_fuse8_hash`.
fn hash_index(index: u32, hash: u64, filter: &Fuse8) -> u32 {
    let mut h = mulhi(hash, filter.segment_count_length as u64);
    h = h.wrapping_add(index as u64 * filter.segment_length as u64);
    let hh = hash & ((1u64 << 36) - 1);
    h ^= (hh >> (36 - 18 * index)) & filter.segment_length_mask as u64;
    h as u32
}

/// `binary_fuse8_hash_batch`.
fn hash_batch(hash: u64, filter: &Fuse8) -> [u32; 3] {
    let hi = mulhi(hash, filter.segment_count_length as u64);
    let h0 = hi as u32;
    let h1 = h0 + filter.segment_length;
    let h2 = h1 + filter.segment_length;
    [
        h0,
        h1 ^ ((hash >> 18) as u32 & filter.segment_length_mask),
        h2 ^ (hash as u32 & filter.segment_length_mask),
    ]
}

fn mod3(x: u8) -> u8 {
    if x > 2 {
        x - 3
    } else {
        x
    }
}

impl Fuse8 {
    /// `binary_fuse8_allocate`.
    fn allocate(size: u32) -> Self {
        const ARITY: u32 = 3;
        let mut segment_length = if size == 0 {
            4
        } else {
            calculate_segment_length(ARITY, size)
        };
        if segment_length > 262_144 {
            segment_length = 262_144;
        }
        let segment_length_mask = segment_length - 1;
        let size_factor = if size <= 1 {
            0.0
        } else {
            calculate_size_factor(ARITY, size)
        };
        let capacity = if size <= 1 {
            0
        } else {
            (size as f64 * size_factor).round() as u32
        };
        // The reference relies on unsigned wrap-around for `size <= 1`; the
        // same intermediate values end at a 12-slot array.
        let init_segment_count = capacity
            .div_ceil(segment_length)
            .wrapping_sub(ARITY - 1);
        let array_length = init_segment_count
            .wrapping_add(ARITY - 1)
            .wrapping_mul(segment_length);
        let mut segment_count = array_length.div_ceil(segment_length);
        if segment_count < ARITY {
            segment_count = 1;
        } else {
            segment_count -= ARITY - 1;
        }
        let array_length = segment_count
            .wrapping_add(ARITY - 1)
            .wrapping_mul(segment_length);
        let segment_count_length = segment_count.wrapping_mul(segment_length);
        Self {
            seed: 0,
            segment_length,
            segment_length_mask,
            segment_count,
            segment_count_length,
            array_length,
            fingerprints: vec![0u8; array_length as usize],
        }
    }

    /// `binary_fuse8_populate`.
    fn populate(keys: &[u64], filter: &mut Fuse8) -> bool {
        let mut rng_counter = 0x726b_2b9d_438b_9d4d;
        filter.seed = splitmix64(&mut rng_counter);
        let capacity = filter.array_length as usize;
        let mut keys = keys.to_vec();
        let mut size = keys.len() as u32;
        let mut block_bits = 1u32;
        while (1u32 << block_bits) < filter.segment_count {
            block_bits += 1;
        }
        let block = 1u32 << block_bits;
        let mask_block = block - 1;
        let mut reverse_order = Vec::new();
        let mut reverse_h = Vec::new();

        for loop_index in 0..=MAX_ITERATIONS {
            if loop_index == MAX_ITERATIONS {
                filter.fingerprints.fill(0xff);
                return false;
            }
            reverse_order = vec![0u64; size as usize + 1];
            // Guard slot: a full segment must stop before running past the end
            // of the reverse-order array (C++ `reverseOrder[size] = 1`).
            reverse_order[size as usize] = 1;
            let mut alone = vec![0u32; capacity];
            let mut t2count = vec![0u8; capacity];
            reverse_h = vec![0u8; size as usize];
            let mut t2hash = vec![0u64; capacity];
            let mut start_pos = vec![0u32; block as usize];

            for (i, slot) in start_pos.iter_mut().enumerate() {
                *slot = ((i as u64 * size as u64) >> block_bits) as u32;
            }

            for key in keys.iter().take(size as usize) {
                let hash = murmur64(key.wrapping_add(filter.seed));
                let mut segment_index = (hash >> (64 - block_bits)) as u32;
                while reverse_order[start_pos[segment_index as usize] as usize] != 0 {
                    segment_index = segment_index.wrapping_add(1) & mask_block;
                }
                reverse_order[start_pos[segment_index as usize] as usize] = hash;
                start_pos[segment_index as usize] += 1;
            }

            let mut error = false;
            let mut duplicates = 0u32;
            for &hash in reverse_order.iter().take(size as usize) {
                let h0 = hash_index(0, hash, filter);
                t2count[h0 as usize] = t2count[h0 as usize].wrapping_add(4);
                t2hash[h0 as usize] ^= hash;
                let h1 = hash_index(1, hash, filter);
                t2count[h1 as usize] = t2count[h1 as usize].wrapping_add(4);
                t2count[h1 as usize] ^= 1;
                t2hash[h1 as usize] ^= hash;
                let h2 = hash_index(2, hash, filter);
                t2count[h2 as usize] = t2count[h2 as usize].wrapping_add(4);
                t2count[h2 as usize] ^= 2;
                t2hash[h2 as usize] ^= hash;
                if (t2hash[h0 as usize] & t2hash[h1 as usize] & t2hash[h2 as usize]) == 0
                    && ((t2hash[h0 as usize] == 0 && t2count[h0 as usize] == 8)
                        || (t2hash[h1 as usize] == 0 && t2count[h1 as usize] == 8)
                        || (t2hash[h2 as usize] == 0 && t2count[h2 as usize] == 8))
                {
                    duplicates += 1;
                    t2count[h0 as usize] = t2count[h0 as usize].wrapping_sub(4);
                    t2hash[h0 as usize] ^= hash;
                    t2count[h1 as usize] = t2count[h1 as usize].wrapping_sub(4);
                    t2count[h1 as usize] ^= 1;
                    t2hash[h1 as usize] ^= hash;
                    t2count[h2 as usize] = t2count[h2 as usize].wrapping_sub(4);
                    t2count[h2 as usize] ^= 2;
                    t2hash[h2 as usize] ^= hash;
                }
                error |= t2count[h0 as usize] < 4;
                error |= t2count[h1 as usize] < 4;
                error |= t2count[h2 as usize] < 4;
            }
            if error {
                filter.seed = splitmix64(&mut rng_counter);
                continue;
            }

            let mut qsize = 0usize;
            for (i, count) in t2count.iter().enumerate() {
                alone[qsize] = i as u32;
                qsize += usize::from((count >> 2) == 1);
            }
            let mut stacksize = 0usize;
            while qsize > 0 {
                qsize -= 1;
                let index = alone[qsize] as usize;
                if (t2count[index] >> 2) == 1 {
                    let hash = t2hash[index];
                    let h012 = [
                        hash_index(0, hash, filter),
                        hash_index(1, hash, filter),
                        hash_index(2, hash, filter),
                        hash_index(0, hash, filter),
                        hash_index(1, hash, filter),
                    ];
                    let found = t2count[index] & 3;
                    reverse_h[stacksize] = found;
                    reverse_order[stacksize] = hash;
                    stacksize += 1;

                    let other1 = h012[(found + 1) as usize] as usize;
                    alone[qsize] = other1 as u32;
                    qsize += usize::from((t2count[other1] >> 2) == 2);
                    t2count[other1] = t2count[other1].wrapping_sub(4);
                    t2count[other1] ^= mod3(found + 1);
                    t2hash[other1] ^= hash;

                    let other2 = h012[(found + 2) as usize] as usize;
                    alone[qsize] = other2 as u32;
                    qsize += usize::from((t2count[other2] >> 2) == 2);
                    t2count[other2] = t2count[other2].wrapping_sub(4);
                    t2count[other2] ^= mod3(found + 2);
                    t2hash[other2] ^= hash;
                }
            }

            if stacksize + duplicates as usize == size as usize {
                size = stacksize as u32;
                break;
            } else if duplicates > 0 {
                keys.sort_unstable();
                keys.dedup();
                size = keys.len() as u32;
            }
            filter.seed = splitmix64(&mut rng_counter);
        }

        for i in (0..size as usize).rev() {
            let hash = reverse_order[i];
            let xor2 = fingerprint(hash);
            let found = reverse_h[i];
            let h012 = [
                hash_index(0, hash, filter),
                hash_index(1, hash, filter),
                hash_index(2, hash, filter),
                hash_index(0, hash, filter),
                hash_index(1, hash, filter),
            ];
            filter.fingerprints[h012[found as usize] as usize] = xor2
                ^ filter.fingerprints[h012[found as usize + 1] as usize]
                ^ filter.fingerprints[h012[found as usize + 2] as usize];
        }
        true
    }

    /// `binary_fuse8_contain`.
    fn contain(&self, key: u64) -> bool {
        let hash = mix_split(key, self.seed);
        let f = fingerprint(hash);
        let [h0, h1, h2] = hash_batch(hash, self);
        f ^ self.fingerprints[h0 as usize]
            ^ self.fingerprints[h1 as usize]
            ^ self.fingerprints[h2 as usize]
            == 0
    }
}

/// C++ `BinaryFuse`: a lazily built fuse-8 filter with a build timestamp.
pub struct BinaryFuse {
    build_time: u64,
    finished_build_filter: AtomicBool,
    filter: Option<Fuse8>,
}

impl Default for BinaryFuse {
    fn default() -> Self {
        Self::new()
    }
}

impl BinaryFuse {
    pub fn new() -> Self {
        Self {
            build_time: UNCOMMIT_TS,
            finished_build_filter: AtomicBool::new(false),
            filter: None,
        }
    }

    /// C++ `BinaryFuse::Build`.
    pub fn build(&mut self, begin_ts: u64, data: &[u64]) -> Result<()> {
        if self.build_time != UNCOMMIT_TS
            || self.filter.is_some()
            || self.finished_build_filter.load(Ordering::Acquire)
        {
            return Err(Error::Unrecoverable(
                "Already have data, cannot allocate.".into(),
            ));
        }
        self.build_time = begin_ts;
        let mut filter = Fuse8::allocate(data.len() as u32);
        if !Fuse8::populate(data, &mut filter) {
            return Err(Error::Unrecoverable("Failed to populate data.".into()));
        }
        self.filter = Some(filter);
        self.finished_build_filter.store(true, Ordering::Release);
        Ok(())
    }

    /// C++ `BinaryFuse::Contain`.
    pub fn contain(&self, query_ts: u64, item: u64) -> bool {
        if !self.finished_build_filter.load(Ordering::Acquire) {
            return true;
        }
        if query_ts > self.build_time {
            self.filter.as_ref().is_some_and(|f| f.contain(item))
        } else {
            true
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_2000() {
        const NUM: u64 = 2000;
        let mut data = vec![0u64; NUM as usize];
        for (i, slot) in data.iter_mut().enumerate() {
            *slot = i as u64 * NUM;
        }
        let mut filter = BinaryFuse::new();
        // test build
        filter.build(0, &data).expect("build");
        // test contain
        for i in 0..NUM {
            let item = i * NUM;
            assert!(filter.contain(1, item), "missing {item}");
        }
        // test fake contain
        let mut fake_contain = 0u64;
        let mut total_cnt = 0u64;
        for i in 0..NUM * NUM {
            if i % NUM != 0 {
                total_cnt += 1;
                if filter.contain(1, i) {
                    fake_contain += 1;
                }
            }
        }
        let ratio = fake_contain as f64 / total_cnt as f64;
        assert!(ratio < 0.005, "false positive ratio {ratio}");
    }

    #[test]
    fn query_at_or_before_build_time_skips_the_filter() {
        let mut filter = BinaryFuse::new();
        filter.build(10, &[1, 2, 3]).expect("build");
        assert!(filter.contain(10, 999));
        assert!(!filter.contain(11, 999));
        assert!(filter.build(12, &[4]).is_err());
    }
}

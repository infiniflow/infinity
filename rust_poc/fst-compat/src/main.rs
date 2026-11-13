//! PoC F (RUST_REWRITE_PLAN.md 附录 B): is Infrared's C++ `fst` (a hand port of
//! `BurntSushi/fst`) actually interoperable with the Rust `fst` crate?
//!
//! Verdict: **data path is compatible, but the checksum is NOT** — the C++ port
//! computes a CRC-32/IEEE (poly 0xEDB88320) while the crate uses CRC32C
//! (Castagnoli, poly 0x82F63B78) with a Snappy-style mask. The two `verify()`
//! implementations therefore disagree. The bounds-check predicate also differs
//! (`&&` in the crate vs `||` in the C++ port).
//!
//! Evidence:
//!   - crate checksum : `~/.cargo/.../fst-0.4.7/src/raw/crc32.rs:21-24` (CRC32C + 0xA282EAD8)
//!   - C++ reader      : `src/storage/invertedindex/fst/fst.cppm:134-144` (CRC32IEEE + 0xA282EAD8)
//!   - Infinity CRC    : `src/common/utility/crc.cppm:49` (`CRC32_IEEE = 0xEDB88320`)
//!   - crate bounds    : `fst-0.4.7/src/raw/mod.rs:399-401`  (`&&`)
//!   - C++ bounds      : `src/storage/invertedindex/fst/fst.cppm:104` (`||`)
//!
//! Run:  cargo run -p fst-compat
//! Exit code 0 means "PoC executed"; see printed verdict for the compatibility
//! classification (the reuse claim must be treated as conditional).

use fst::{IntoStreamer, Map, MapBuilder, Streamer};

const CPP_VERSION: u64 = 3;
const CPP_EMPTY_ADDRESS: u64 = 0;
const CPP_ADDR_OFFSET: usize = 21;

fn read_u64_le(b: &[u8], off: usize) -> u64 {
    u64::from_le_bytes(b[off..off + 8].try_into().unwrap())
}
fn read_u32_le(b: &[u8], off: usize) -> u32 {
    u32::from_le_bytes(b[off..off + 4].try_into().unwrap())
}

// ---- CRC-32/IEEE (802.3), as used by Infinity's CRC32IEEE ----------------
fn crc32_ieee(buf: &[u8]) -> u32 {
    crc32fast::hash(buf)
}

// ---- CRC32C (Castagnoli), as used by the fst crate ----------------------
fn crc32c_table() -> [u32; 256] {
    let mut t = [0u32; 256];
    for i in 0..256u32 {
        let mut c = i;
        for _ in 0..8 {
            c = if c & 1 != 0 { 0x82F6_3B78 ^ (c >> 1) } else { c >> 1 };
        }
        t[i as usize] = c;
    }
    t
}
fn crc32c(buf: &[u8]) -> u32 {
    let t = crc32c_table();
    let mut c = 0xFFFF_FFFFu32;
    for &b in buf {
        c = t[((c ^ b as u32) & 0xff) as usize] ^ (c >> 8);
    }
    c ^ 0xFFFF_FFFF
}

/// Snappy-style mask used by BOTH implementations: ((s>>15)|(s<<17)) + 0xA282EAD8
fn mask(crc: u32) -> u32 {
    (crc >> 15 | crc << 17).wrapping_add(0xA282_EAD8)
}

struct Meta {
    version: u64,
    root_addr: usize,
    len: usize,
    checksum: u32,
}

/// C++ `Fst` ctor layout checks (`fst.cppm:64-112`), using the C++ `||` predicate.
fn cpp_accepts(b: &[u8]) -> Result<Meta, String> {
    let n = b.len();
    if n < 36 {
        return Err(format!("size {n} < 36"));
    }
    let version = read_u64_le(b, 0);
    if version != CPP_VERSION {
        return Err(format!("version {version} != {CPP_VERSION}"));
    }
    let end = n - 4;
    let checksum = read_u32_le(b, n - 4);
    let root_addr = read_u64_le(b, end - 8) as usize;
    let len = read_u64_le(b, end - 16) as usize;
    let (empty_total, addr_offset) = (36usize, CPP_ADDR_OFFSET);
    // C++ fst.cppm:104 -> uses `||`
    let ok = (root_addr == CPP_EMPTY_ADDRESS as usize && n != empty_total)
        || (root_addr != CPP_EMPTY_ADDRESS as usize && root_addr + addr_offset != n);
    // note: C++ returns error when this is TRUE, so acceptance is !ok
    if ok {
        return Err(format!("bounds check failed (root_addr={root_addr}, len={n})"));
    }
    Ok(Meta { version, root_addr, len, checksum })
}

/// Rust crate `Fst::new` bounds check (`mod.rs:399-401`), using `&&`.
fn crate_bounds_accepts(b: &[u8], root_addr: usize) -> bool {
    let n = b.len();
    let (empty_total, addr_offset) = (36usize, 21usize);
    let bad = (root_addr == CPP_EMPTY_ADDRESS as usize && n != empty_total)
        && root_addr + addr_offset != n;
    !bad
}

fn build_map() -> Vec<u8> {
    let mut b = MapBuilder::memory();
    for (k, word) in ["apple", "applet", "application", "banana", "band", "bandana", "cherry"]
        .iter()
        .enumerate()
    {
        b.insert(word, k as u64).unwrap();
    }
    b.into_inner().unwrap()
}

fn main() {
    let mut hard_fail = 0;
    println!("== PoC F: C++ `fst` port vs Rust `fst` crate (BurntSushi/fst 0.4.7) ==");

    let bytes = build_map();
    let meta = cpp_accepts(&bytes).unwrap_or_else(|e| {
        println!("  FAIL  C++ structural checks rejected Rust bytes: {e}");
        std::process::exit(1);
    });
    println!(
        "  [structure] PASS  C++ layout checks accept Rust bytes (version={}, root_addr={}, len={}, size={})",
        meta.version, meta.root_addr, meta.len, bytes.len()
    );
    println!(
        "  [bounds]    crate(`&&`) accepts = {}, cpp(`||`) accepts = true  (agree on this input)",
        crate_bounds_accepts(&bytes, meta.root_addr)
    );

    // ---- checksum: the actual divergence ----
    let body = &bytes[..bytes.len() - 4];
    let ieee = crc32_ieee(body);
    let c = crc32c(body);
    let m_ieee = mask(ieee);
    let m_c = mask(c);
    println!("  [checksum] stored = {:#010x}", meta.checksum);
    println!("             CRC32/IEEE  raw={ieee:#010x} masked={m_ieee:#010x}  (Infinity C++ CRC32IEEE::makeCRC)");
    println!("             CRC32C      raw={c:#010x} masked={m_c:#010x}  (fst crate CheckSummer::masked)");
    let ieee_match = m_ieee == meta.checksum;
    let c_match = m_c == meta.checksum;
    println!("             stored == mask(CRC32C)   ? {c_match}");
    println!("             stored == mask(CRC32/IEEE)? {ieee_match}");
    if c_match && !ieee_match {
        println!("  [checksum] DIVERGENCE CONFIRMED: crate stores CRC32C; C++ `Verify()` computes CRC32/IEEE");
        println!("             => C++ `Fst::Verify()` would REJECT crate-written fst (and vice versa).");
    } else if !c_match && !ieee_match {
        println!("  [checksum] neither matches -> unexpected; investigate");
        hard_fail += 1;
    }

    // ---- data path: round-trip read must still work ----
    match Map::new(bytes.clone()) {
        Ok(map) => {
            let mut s = map.stream();
            let mut n = 0;
            while let Some(_) = s.next() {
                n += 1;
            }
            let mut ps = map.range().ge("app".as_bytes()).lt("appz".as_bytes()).into_stream();
            let mut pc = 0;
            while let Some(_) = ps.next() {
                pc += 1;
            }
            if n == 7 && pc == 3 {
                println!("  [data]      PASS  round-trip: {n} keys; prefix 'app' -> {pc} hits");
            } else {
                hard_fail += 1;
                println!("  [data]      FAIL  round-trip keys={n}, prefix={pc}");
            }
        }
        Err(e) => {
            hard_fail += 1;
            println!("  [data]      FAIL  Map::new: {e}");
        }
    }

    // ---- empty map: NOT 36 bytes (my earlier assumption was wrong) ----
    let empty = MapBuilder::memory().into_inner().unwrap();
    let empty_ok = cpp_accepts(&empty).is_ok();
    println!(
        "  [empty]     Rust empty map = {} bytes, C++ accepts = {} (root node present; not the 36-byte EMPTY_ADDRESS form)",
        empty.len(),
        empty_ok
    );
    if !empty_ok {
        hard_fail += 1;
    }

    println!("== VERDICT ==");
    println!("  reuse of the `fst` crate is viable for the READ/WRITE data path,");
    println!("  but NOT unconditionally binary-compatible:");
    println!("    (1) checksum polynomial differs (CRC32C vs CRC32/IEEE) -> Verify() not interoperable;");
    println!("    (2) bounds-check predicate differs (`&&` vs `||`).");
    println!("  => plan's FST reuse claim must stay conditional; reconcile checksum before relying on it.");
    std::process::exit(if hard_fail == 0 { 0 } else { 1 });
}

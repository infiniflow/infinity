//! PoC D (RUST_REWRITE_PLAN.md 附录 B / §3 D6): Thrift `TBinaryProtocol` wire
//! interop, exercising the exact binary layouts the SDK golden relies on.
//!
//! Structs (from `thrift/infinity.thrift`):
//!   CommonResponse { 1: i64 error_code, 2: string error_msg, 3: i64 session_id }
//!   ColumnField    { 1: i32 column_type, 2: list<binary> column_vectors,
//!                    3: string column_name, 4: list<bool> bitmasks }
//!
//! Two checks:
//!   (a) BYTE-EXACT golden: bytes for CommonResponse(0,"ok",42) must equal a
//!       hand-computed TBinaryProtocol encoding.
//!   (b) CROSS-LANGUAGE: encode with Rust -> `rust_*.bin`; decode `py_*.bin`
//!       produced by the authoritative generated Python codec (`py_side.py`).
//!
//! Run:  cargo run -p thrift-golden -- <workdir>

use std::fs;
use std::path::Path;

use thrift::protocol::{
    TBinaryInputProtocol, TBinaryOutputProtocol, TFieldIdentifier, TInputProtocol,
    TListIdentifier, TOutputProtocol, TStructIdentifier, TType,
};

fn enc_common_response() -> thrift::Result<Vec<u8>> {
    let mut buf = Vec::new();
    {
        let mut o = TBinaryOutputProtocol::new(&mut buf, true);
        o.write_struct_begin(&TStructIdentifier::new("CommonResponse"))?;
        o.write_field_begin(&TFieldIdentifier::new("error_code", TType::I64, 1i16))?;
        o.write_i64(0)?;
        o.write_field_end()?;
        o.write_field_begin(&TFieldIdentifier::new("error_msg", TType::String, 2i16))?;
        o.write_string("ok")?;
        o.write_field_end()?;
        o.write_field_begin(&TFieldIdentifier::new("session_id", TType::I64, 3i16))?;
        o.write_i64(42)?;
        o.write_field_end()?;
        o.write_field_stop()?;
        o.write_struct_end()?;
    }
    Ok(buf)
}

fn dec_common_response(data: &[u8]) -> thrift::Result<String> {
    let mut i = TBinaryInputProtocol::new(data, true);
    i.read_struct_begin()?;
    let (mut ec, mut msg, mut sid) = (None, None, None);
    loop {
        let f = i.read_field_begin()?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => ec = Some(i.read_i64()?),
            Some(2) => msg = Some(i.read_string()?),
            Some(3) => sid = Some(i.read_i64()?),
            _ => i.skip(f.field_type)?,
        }
        i.read_field_end()?;
    }
    i.read_struct_end()?;
    Ok(format!(
        "{{\"error_code\":{},\"error_msg\":\"{}\",\"session_id\":{}}}",
        ec.unwrap_or(-1),
        msg.unwrap_or_default(),
        sid.unwrap_or(-1)
    ))
}

fn enc_column_field() -> thrift::Result<Vec<u8>> {
    let vec0: Vec<u8> = [7i32, 8i32].iter().flat_map(|v| v.to_le_bytes()).collect();
    let mut buf = Vec::new();
    {
        let mut o = TBinaryOutputProtocol::new(&mut buf, true);
        o.write_struct_begin(&TStructIdentifier::new("ColumnField"))?;
        o.write_field_begin(&TFieldIdentifier::new("column_type", TType::I32, 1i16))?;
        o.write_i32(1)?;
        o.write_field_end()?;
        o.write_field_begin(&TFieldIdentifier::new("column_vectors", TType::List, 2i16))?;
        o.write_list_begin(&TListIdentifier::new(TType::String, 1))?;
        o.write_bytes(&vec0)?;
        o.write_list_end()?;
        o.write_field_end()?;
        o.write_field_begin(&TFieldIdentifier::new("column_name", TType::String, 3i16))?;
        o.write_string("score")?;
        o.write_field_end()?;
        o.write_field_begin(&TFieldIdentifier::new("bitmasks", TType::List, 4i16))?;
        o.write_list_begin(&TListIdentifier::new(TType::Bool, 2))?;
        o.write_bool(true)?;
        o.write_bool(false)?;
        o.write_list_end()?;
        o.write_field_end()?;
        o.write_field_stop()?;
        o.write_struct_end()?;
    }
    Ok(buf)
}

fn dec_column_field(data: &[u8]) -> thrift::Result<String> {
    let mut i = TBinaryInputProtocol::new(data, true);
    i.read_struct_begin()?;
    let (mut ctype, mut name, mut ints, mut masks) = (None, None, vec![], vec![]);
    loop {
        let f = i.read_field_begin()?;
        if f.field_type == TType::Stop {
            break;
        }
        match f.id {
            Some(1) => ctype = Some(i.read_i32()?),
            Some(2) => {
                let li = i.read_list_begin()?;
                for _ in 0..li.size {
                    let b = i.read_bytes()?;
                    for c in b.chunks_exact(4) {
                        ints.push(i32::from_le_bytes([c[0], c[1], c[2], c[3]]));
                    }
                }
                i.read_list_end()?;
            }
            Some(3) => name = Some(i.read_string()?),
            Some(4) => {
                let li = i.read_list_begin()?;
                for _ in 0..li.size {
                    masks.push(i.read_bool()?);
                }
                i.read_list_end()?;
            }
            _ => i.skip(f.field_type)?,
        }
        i.read_field_end()?;
    }
    i.read_struct_end()?;
    let ints_s = ints.iter().map(|v| v.to_string()).collect::<Vec<_>>().join(",");
    let masks_s = masks.iter().map(|b| if *b { "true" } else { "false" }).collect::<Vec<_>>().join(",");
    Ok(format!(
        "{{\"column_type\":{},\"column_name\":\"{}\",\"ints\":[{}],\"bitmasks\":[{}]}}",
        ctype.unwrap_or(-1),
        name.unwrap_or_default(),
        ints_s,
        masks_s
    ))
}

/// Hand-computed TBinaryProtocol bytes for CommonResponse(0, "ok", 42).
fn expected_common_bytes() -> Vec<u8> {
    let mut v = Vec::new();
    v.push(0x0A); // I64
    v.extend_from_slice(&[0x00, 0x01]); // field id 1
    v.extend_from_slice(&0i64.to_be_bytes());
    v.push(0x0B); // STRING
    v.extend_from_slice(&[0x00, 0x02]); // field id 2
    v.extend_from_slice(&2i32.to_be_bytes());
    v.extend_from_slice(b"ok");
    v.push(0x0A);
    v.extend_from_slice(&[0x00, 0x03]); // field id 3
    v.extend_from_slice(&42i64.to_be_bytes());
    v.push(0x00); // STOP
    v
}

fn main() {
    let mut failed = 0;
    let dir = std::env::args().nth(1).unwrap_or_else(|| "/tmp/infinity-thrift-golden".into());
    fs::create_dir_all(&dir).unwrap();
    let dir = Path::new(&dir);
    println!("== PoC D: Thrift TBinaryProtocol interop ==");

    let cr = enc_common_response().unwrap();
    let exp = expected_common_bytes();
    let golden_ok = cr == exp;
    println!(
        "  [byte golden]  {}  CommonResponse bytes={} (expect {}), equal={}",
        if golden_ok { "PASS" } else { "FAIL" },
        cr.len(),
        exp.len(),
        golden_ok
    );
    if !golden_ok {
        failed += 1;
        println!("     got={:02x?}", cr);
        println!("     exp={:02x?}", exp);
    }

    // Self round-trip
    let rt = dec_common_response(&cr).unwrap();
    let want = "{\"error_code\":0,\"error_msg\":\"ok\",\"session_id\":42}";
    let rt_ok = rt == want;
    println!("  [round-trip]   {}  {rt}", if rt_ok { "PASS" } else { "FAIL" });
    if !rt_ok {
        failed += 1;
    }

    fs::write(dir.join("rust_common_response.bin"), &cr).unwrap();
    let cf = enc_column_field().unwrap();
    fs::write(dir.join("rust_column_field.bin"), &cf).unwrap();
    println!("  wrote rust_*.bin to {}", dir.display());

    for (file, dec) in [
        ("py_common_response.bin", dec_common_response as fn(&[u8]) -> thrift::Result<String>),
        ("py_column_field.bin", dec_column_field),
    ] {
        let p = dir.join(file);
        if p.exists() {
            match dec(&fs::read(&p).unwrap()) {
                Ok(s) => println!("RUST_DECODED {file} {s}"),
                Err(e) => {
                    failed += 1;
                    println!("RUST_DECODE_ERROR {file}: {e}");
                }
            }
        } else {
            println!("  (skip) {file} not present yet");
        }
    }

    println!("== PoC D result: {} ==", if failed == 0 { "PASS" } else { "FAIL" });
    std::process::exit(if failed == 0 { 0 } else { 1 });
}

//! Emits a Thrift `Connect` CALL message (TBinaryProtocol) to stdout as raw bytes,
//! so it can be decoded by the authoritative Python SDK codec:
//!   cargo run -q -p infinity-server --bin emit-connect-call | python3 py_decode.py
use std::io::Write;

use infinity_server::{write_connect_call, CURRENT_VERSION_INDEX};

fn main() {
    let mut w = std::io::stdout().lock();
    write_connect_call(&mut w, 1, CURRENT_VERSION_INDEX).expect("write");
    w.flush().expect("flush");
}

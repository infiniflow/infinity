//! Reads a `ListDatabase` CALL from stdin and writes a REPLY — a metadata RPC:
//!   cargo run -q -p infinity-server --bin handle-list-database < call.bin > reply.bin
use std::io::Write;

use infinity_server::{read_list_database_call, write_list_database_response};

fn main() {
    let mut stdin = std::io::stdin();
    let (seq, _sid) = read_list_database_call(&mut stdin).expect("decode ListDatabase call");
    let mut out = std::io::stdout().lock();
    write_list_database_response(&mut out, seq, &["default_db".to_string()]).expect("write reply");
    out.flush().expect("flush");
}

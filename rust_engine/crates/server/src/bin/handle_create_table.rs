//! Reads a Thrift `CreateTable` CALL from stdin, runs it against the engine, and
//! writes a `CommonResponse` REPLY to stdout — a real SDK→engine→SDK round-trip:
//!   cargo run -q -p infinity-server --bin handle-create-table < call.bin > reply.bin
use std::io::Write;

use executor::Engine;
use server::{read_create_table_call, run_create_table, write_reply_common};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-thrift-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");

    let mut stdin = std::io::stdin();
    let (seq, req) = read_create_table_call(&mut stdin).expect("decode CreateTable call");

    // The same primitive the Thrift `CreateTable` handler uses, so the fixture
    // builds `Statement::CreateTable` from the request rather than rendering
    // the column names into a statement string.
    let resp = executor::block_on(run_create_table(Some(&engine), &req));
    let _ = executor::block_on(engine.flush());

    let mut out = std::io::stdout().lock();
    write_reply_common(&mut out, "CreateTable", seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

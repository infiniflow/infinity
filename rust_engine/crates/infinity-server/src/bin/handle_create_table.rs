//! Reads a Thrift `CreateTable` CALL from stdin, runs it against the engine, and
//! writes a `CommonResponse` REPLY to stdout — a real SDK→engine→SDK round-trip:
//!   cargo run -q -p infinity-server --bin handle-create-table < call.bin > reply.bin
use std::io::Write;

use infinity_executor::Engine;
use infinity_server::{read_create_table_call, write_reply_common, CommonResponse};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-thrift-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");

    let mut stdin = std::io::stdin();
    let (seq, req) = read_create_table_call(&mut stdin).expect("decode CreateTable call");

    let cols: Vec<String> = req
        .columns
        .iter()
        .map(|c| format!("{} VARCHAR", c.name))
        .collect();
    let sql = format!("CREATE TABLE {} ({});", req.table_name, cols.join(", "));
    let resp = match engine.execute(&sql) {
        Ok(_) => CommonResponse {
            error_code: 0,
            error_msg: "OK".into(),
            session_id: req.session_id,
        },
        Err(e) => CommonResponse {
            error_code: 1,
            error_msg: e.to_string(),
            session_id: req.session_id,
        },
    };
    let _ = engine.flush();

    let mut out = std::io::stdout().lock();
    write_reply_common(&mut out, "CreateTable", seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

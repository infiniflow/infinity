//! Reads a Thrift `Insert` CALL from stdin, runs it against the engine (after a
//! fixture), and writes a `CommonResponse` REPLY — completing the DDL→DML path:
//!   cargo run -q -p infinity-server --bin handle-insert < call.bin > reply.bin
use std::io::Write;

use infinity_executor::Engine;
use infinity_server::{read_insert_call, write_reply_common, CommonResponse};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-insert-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");

    let mut stdin = std::io::stdin();
    let (seq, req) = read_insert_call(&mut stdin).expect("decode Insert call");
    let t = &req.table_name;

    let ncols = req
        .rows
        .first()
        .map(|(_, vs)| vs.len())
        .unwrap_or(1)
        .max(1);
    let cols: Vec<String> = (0..ncols).map(|i| format!("c{i} VARCHAR")).collect();
    let _ = engine.execute(&format!("CREATE TABLE {t} ({});", cols.join(", ")));

    let vals = req
        .rows
        .iter()
        .map(|(_, vs)| {
            format!(
                "({})",
                vs.iter()
                    .map(|v| v.to_sql())
                    .collect::<Vec<_>>()
                    .join(", ")
            )
        })
        .collect::<Vec<_>>()
        .join(", ");
    let sql = format!("INSERT INTO {t} VALUES {vals};");
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
    write_reply_common(&mut out, "Insert", seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

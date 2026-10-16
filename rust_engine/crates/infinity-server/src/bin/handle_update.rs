//! Reads an `Update` CALL from stdin, runs it against the engine (with a fixture),
//! and writes a `CommonResponse` REPLY:
//!   cargo run -q -p infinity-server --bin handle-update < call.bin > reply.bin
use std::io::Write;

use infinity_executor::Engine;
use infinity_server::{read_update_call, write_reply_common, CommonResponse};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-update-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");
    let (seq, req) = read_update_call(&mut std::io::stdin()).expect("decode Update call");
    let t = &req.table_name;
    let _ = engine.execute(&format!("CREATE TABLE {t} (id INT, name VARCHAR);"));
    let _ = engine.execute(&format!(
        "INSERT INTO {t} VALUES (1, 'a'), (2, 'b'), (3, 'c');"
    ));

    let resp = match engine.execute(&req.to_sql()) {
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
    let mut out = std::io::stdout().lock();
    write_reply_common(&mut out, "Update", seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

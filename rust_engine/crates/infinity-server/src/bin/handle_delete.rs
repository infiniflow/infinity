//! Reads a `Delete` CALL from stdin, runs it against the engine (with a fixture),
//! and writes a `DeleteResponse` REPLY:
//!   cargo run -q -p infinity-server --bin handle-delete < call.bin > reply.bin
use std::io::Write;

use infinity_executor::{Engine, QueryResult};
use infinity_server::{read_delete_call, write_delete_response};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-delete-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");
    let (seq, req) = read_delete_call(&mut std::io::stdin()).expect("decode Delete call");
    let t = &req.table_name;
    let _ = engine.execute(&format!("CREATE TABLE {t} (id INT, name VARCHAR);"));
    let _ = engine.execute(&format!(
        "INSERT INTO {t} VALUES (1, 'a'), (2, 'b'), (3, 'c');"
    ));

    let deleted = match engine.execute(&req.to_sql()) {
        Ok(rs) => rs
            .into_iter()
            .map(|r| {
                if let QueryResult::Deleted { count } = r {
                    count as i64
                } else {
                    0
                }
            })
            .sum::<i64>(),
        Err(_) => -1,
    };
    let mut out = std::io::stdout().lock();
    write_delete_response(&mut out, seq, 0, "OK", deleted.max(0)).expect("write reply");
    out.flush().expect("flush");
}

//! Reads a `ShowTable` CALL from stdin, runs it against the engine (with a
//! fixture), and writes a `ShowTableResponse` REPLY:
//!   cargo run -q -p infinity-server --bin handle-show-table < call.bin > reply.bin
use std::io::Write;

use infinity_executor::Engine;
use infinity_server::{read_show_table_call, write_show_table_response};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-show-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");
    let (seq, db, table, _sid) =
        read_show_table_call(&mut std::io::stdin()).expect("decode ShowTable");
    let _ = engine.execute(&format!("CREATE TABLE {table} (id INT, name VARCHAR);"));
    let _ = engine.execute(&format!("INSERT INTO {table} VALUES (1, 'a'), (2, 'b');"));

    let col_count = engine
        .catalog()
        .get_table_by_name(&table)
        .ok()
        .flatten()
        .map(|m| m.columns.len() as i64)
        .unwrap_or(0);
    let row_count = engine.row_count(&table).unwrap_or(0) as i64;

    let mut out = std::io::stdout().lock();
    write_show_table_response(&mut out, seq, &db, &table, 0, "OK", col_count, 1, row_count)
        .expect("write reply");
    out.flush().expect("flush");
}

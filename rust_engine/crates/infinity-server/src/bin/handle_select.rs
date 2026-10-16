//! Reads a Thrift `Select` CALL from stdin, runs `SELECT * FROM <table>` against
//! the engine (after a small fixture), and writes a `SelectResponse` REPLY to
//! stdout — a real query over the wire:
//!   cargo run -q -p infinity-server --bin handle-select < call.bin > reply.bin
use std::io::Write;

use infinity_executor::{Engine, QueryResult};
use infinity_server::{
    read_select_call, select_response_from, write_select_response, SelectResponseWire,
};

fn main() {
    let dir = std::env::temp_dir().join(format!("infinity-select-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open(&dir).expect("open engine");

    let mut stdin = std::io::stdin();
    let (seq, req) = read_select_call(&mut stdin).expect("decode Select call");
    let t = &req.table_name;

    // fixture so the query returns data
    let _ = engine.execute(&format!("CREATE TABLE {t} (id INT, name VARCHAR);"));
    let _ = engine.execute(&format!("INSERT INTO {t} VALUES (1, 'a');"));
    let _ = engine.execute(&format!("INSERT INTO {t} VALUES (2, 'b');"));

    let resp = match engine.execute(&req.to_sql()) {
        Ok(rs) => match rs.into_iter().next() {
            Some(QueryResult::Rows { columns, rows }) => select_response_from(&columns, &rows),
            other => SelectResponseWire {
                error_code: 1,
                error_msg: format!("unexpected result: {other:?}"),
                column_defs: vec![],
                column_fields: vec![],
                extra_result: String::new(),
            },
        },
        Err(e) => SelectResponseWire {
            error_code: 1,
            error_msg: e.to_string(),
            column_defs: vec![],
            column_fields: vec![],
            extra_result: String::new(),
        },
    };
    let _ = engine.flush();

    let mut out = std::io::stdout().lock();
    write_select_response(&mut out, seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

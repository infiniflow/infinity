//! Emits a `list<ColumnField>` (TBinaryProtocol) to stdout for cross-language
//! verification with the authoritative Python codec:
//!   cargo run -q -p infinity-server --bin emit-select-columns | python3 py_decode_columns.py
use std::io::Write;

use infinity_server::{rows_to_column_fields, write_column_fields};

fn main() {
    let cols = vec!["id".to_string(), "name".to_string()];
    let rows = vec![
        vec!["1".to_string(), "a".to_string()],
        vec!["2".to_string(), "NULL".to_string()],
    ];
    let fields = rows_to_column_fields(&cols, &rows);
    let mut w = std::io::stdout().lock();
    write_column_fields(&mut w, &fields).expect("write");
    w.flush().expect("flush");
}

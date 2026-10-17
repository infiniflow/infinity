//! Emits a full `Select` REPLY (`SelectResponse`) to stdout for cross-language
//! verification:
//!   cargo run -q -p infinity-server --bin emit-select-response | python3 py_decode_select_response.py
use std::io::Write;

use infinity_server::{
    rows_to_column_fields, write_select_response, ColumnDefWire, PhysicalWire, SelectResponseWire,
};

fn main() {
    let columns = vec!["id".to_string(), "name".to_string()];
    let rows = vec![
        vec!["1".to_string(), "a".to_string()],
        vec!["2".to_string(), "NULL".to_string()],
    ];
    let resp = SelectResponseWire {
        error_code: 0,
        error_msg: "OK".into(),
        column_defs: vec![
            ColumnDefWire {
                id: 0,
                name: "id".into(),
                default: None,
                not_null: false,
                logic_type: 5,
                physical: PhysicalWire::None,
            },
            ColumnDefWire {
                id: 1,
                name: "name".into(),
                default: None,
                not_null: false,
                logic_type: 2,
                physical: PhysicalWire::None,
            },
        ],
        column_fields: rows_to_column_fields(&columns, &rows),
        extra_result: String::new(),
    };
    let mut w = std::io::stdout().lock();
    write_select_response(&mut w, 1, &resp).expect("write");
    w.flush().expect("flush");
}

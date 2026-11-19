// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Emits a `list<ColumnField>` (TBinaryProtocol) to stdout for cross-language
//! verification with the authoritative Python codec:
//!   cargo run -q -p infinity-server --bin emit-select-columns | python3 py_decode_columns.py
use std::io::Write;

use server::{rows_to_column_fields, write_column_fields};

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

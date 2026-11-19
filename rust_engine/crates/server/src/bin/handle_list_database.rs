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

//! Reads a `ListDatabase` CALL from stdin and writes a REPLY — a metadata RPC:
//!   cargo run -q -p infinity-server --bin handle-list-database < call.bin > reply.bin
use std::io::Write;

use server::{read_list_database_call, write_list_database_response};

fn main() {
    let mut stdin = std::io::stdin();
    let (seq, _sid) = read_list_database_call(&mut stdin).expect("decode ListDatabase call");
    let mut out = std::io::stdout().lock();
    write_list_database_response(&mut out, seq, &["default_db".to_string()]).expect("write reply");
    out.flush().expect("flush");
}

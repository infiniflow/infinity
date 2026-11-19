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

//! Emits a Thrift `Connect` CALL message (TBinaryProtocol) to stdout as raw bytes,
//! so it can be decoded by the authoritative Python SDK codec:
//!   cargo run -q -p infinity-server --bin emit-connect-call | python3 py_decode.py
use std::io::Write;

use server::{write_connect_call, CURRENT_VERSION_INDEX};

fn main() {
    let mut w = std::io::stdout().lock();
    write_connect_call(&mut w, 1, CURRENT_VERSION_INDEX).expect("write");
    w.flush().expect("flush");
}

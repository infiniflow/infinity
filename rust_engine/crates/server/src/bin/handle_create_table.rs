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

//! Reads a Thrift `CreateTable` CALL from stdin, runs it against the engine, and
//! writes a `CommonResponse` REPLY to stdout — a real SDK→engine→SDK round-trip:
//!   cargo run -q -p infinity-server --bin handle-create-table < call.bin > reply.bin
use std::io::Write;

use executor::Engine;
use server::{read_create_table_call, run_create_table, write_reply_common};

fn main() {
    executor::scheduler().block_on_runtime(run());
}

async fn run() {
    let dir = std::env::temp_dir().join(format!("infinity-thrift-{}", std::process::id()));
    let _ = std::fs::remove_dir_all(&dir);
    let engine = Engine::open_async(&dir, Default::default())
        .await
        .expect("open engine");

    let mut stdin = std::io::stdin();
    let (seq, req) = read_create_table_call(&mut stdin).expect("decode CreateTable call");

    // The same primitive the Thrift `CreateTable` handler uses, so the fixture
    // builds `Statement::CreateTable` from the request rather than rendering
    // the column names into a statement string.
    let resp = (run_create_table(Some(&engine), &req)).await;
    let _ = (engine.flush()).await;

    let mut out = std::io::stdout().lock();
    write_reply_common(&mut out, "CreateTable", seq, &resp).expect("write reply");
    out.flush().expect("flush");
}

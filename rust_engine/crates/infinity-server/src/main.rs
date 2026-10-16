//! `infinity-server`: binds the Thrift-compatible TCP front end (plan §4 M4).
//!
//! Usage: `infinity-server [data_dir]`
//!   * `data_dir`  — engine data directory (default `/var/infinity`)
//!   * `INFINITY_BIND` — listen address (default `0.0.0.0:23817`)

use std::sync::Arc;

use infinity_executor::Engine;
use infinity_server::Server;

fn main() {
    let dir = std::env::args()
        .nth(1)
        .unwrap_or_else(|| "/var/infinity".to_string());
    let addr = std::env::var("INFINITY_BIND").unwrap_or_else(|_| "0.0.0.0:23817".to_string());

    let engine = Arc::new(Engine::open(&dir).expect("open engine"));
    let server = Server::bind_with_engine(&addr, engine).expect("bind server");
    eprintln!("infinity-server listening on {addr} (data dir: {dir})");
    server.run();
}

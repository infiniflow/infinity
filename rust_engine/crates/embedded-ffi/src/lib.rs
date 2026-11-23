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

//! The embedded engine: the database runs **inside the host process**.
//!
//! A local client (rgrep's daemon) does not want a server to install, a port to
//! pick or a supervisor to keep the two in step. It wants one process whose
//! lifetime is its own. This crate is that: `inf_embedded_start` opens the
//! engine on a directory, serves the ordinary thrift front end on a **unix
//! socket** inside that directory, and `inf_embedded_stop` closes both.
//!
//! Why a socket rather than a direct call: everything above the transport -
//! thrift framing, the session table, every statement arm, the panic boundary -
//! already exists in `server`, and a client that speaks thrift over a unix
//! socket is the *same* client code as one that speaks it over TCP (the Go SDK
//! only swaps its dialer). A function-call ABI would have meant a second
//! statement path to keep in step with the first.
//!
//! Ownership: the handle returned by `inf_embedded_start` owns the runtime
//! thread and the socket file; `inf_embedded_stop` joins that thread, removes
//! the file and releases the engine, after which the data directory can be
//! reopened. Every entry point catches panics - a panic must never unwind
//! across the FFI boundary.

#![allow(non_camel_case_types)]

use std::ffi::{c_char, c_int, CStr, CString};
use std::panic::{catch_unwind, AssertUnwindSafe};
use std::ptr;
use std::sync::mpsc;
use std::time::Duration;

use executor::Engine;
use server::Server;

/// A running engine plus the thread serving its socket.
pub struct InfEmbedded {
    /// The socket path, kept as a `CString` so `inf_embedded_addr` can hand out
    /// a pointer that lives as long as the handle.
    socket: CString,
    stop: Option<tokio::sync::oneshot::Sender<()>>,
    join: Option<std::thread::JoinHandle<()>>,
}

/// Hands `message` to the caller as a `malloc`'d string (released with
/// [`inf_embedded_free`]). A null `out` is tolerated so a caller that only wants
/// the return code can pass null.
unsafe fn set_err(out: *mut *mut c_char, message: String) {
    if out.is_null() {
        return;
    }
    match CString::new(message) {
        Ok(s) => *out = c_string_to_malloc(s),
        Err(_) => *out = ptr::null_mut(),
    }
}

/// `CString` -> `libc::malloc`'d copy, so the host releases it with `free()`
/// (or [`inf_embedded_free`]) and never has to know the Rust allocator.
fn c_string_to_malloc(s: CString) -> *mut c_char {
    let bytes = s.as_bytes_with_nul();
    unsafe {
        let p = libc::malloc(bytes.len()) as *mut c_char;
        if p.is_null() {
            return ptr::null_mut();
        }
        ptr::copy_nonoverlapping(bytes.as_ptr() as *const c_char, p, bytes.len());
        p
    }
}

unsafe fn c_str<'a>(p: *const c_char, what: &str) -> Result<&'a str, String> {
    if p.is_null() {
        return Err(format!("{what} is null"));
    }
    CStr::from_ptr(p)
        .to_str()
        .map_err(|_| format!("{what} is not valid UTF-8"))
}

/// Opens `data_dir` and serves the thrift API on `socket_path`.
///
/// Returns a handle, or null with `*err` set. The socket file exists when this
/// returns, so a caller may connect immediately; the accept loop starts on the
/// worker thread and the connection waits in the listen backlog.
///
/// # Safety
/// `data_dir` and `socket_path` must be NUL-terminated strings; `err` must be
/// null or point at a writable `char*`.
#[no_mangle]
pub unsafe extern "C" fn inf_embedded_start(
    data_dir: *const c_char,
    socket_path: *const c_char,
    err: *mut *mut c_char,
) -> *mut InfEmbedded {
    let result = catch_unwind(AssertUnwindSafe(|| {
        let data_dir = c_str(data_dir, "data_dir")?.to_string();
        let socket_path = c_str(socket_path, "socket_path")?.to_string();
        if socket_path.is_empty() {
            return Err("socket_path is empty".to_string());
        }

        let runtime = tokio::runtime::Builder::new_multi_thread()
            .worker_threads(2)
            .enable_all()
            .thread_name("infinity-embedded")
            .build()
            .map_err(|e| format!("runtime: {e}"))?;

        let engine = std::sync::Arc::new(
            runtime
                .block_on(Engine::open_async(&data_dir, Default::default()))
                .map_err(|e| format!("open {data_dir}: {e}"))?,
        );
        // `[buffer] memindex_memory_quota`, which the CLI sets and this host did
        // not: `0` disables the dump trigger, so a tail was never sealed and
        // every query rebuilt the index from the rows instead of mapping a
        // chunk. A multi-vector row is ~48 KB, so a 62 k-unit table kept 3 GiB
        // of vectors resident and one query peaked at 8.2 GiB and still had to
        // brute-force its way through, because the structure that would have
        // pruned it is what the dump builds.
        //
        // 1 GiB is a laptop's answer, not the reference's 4 GiB default: it
        // bounds the tail at ~20 k units and pays the same build work the query
        // would have paid, once per dump, during the import instead of on the
        // first search. The chunks it writes are what later opens and queries
        // map.
        engine.set_memindex_quota(1 << 30);
        // `install_engine_handle` records the running runtime, so the bind has
        // to happen inside its context.
        let _guard = runtime.enter();
        let bound = Server::bind_unix_with_engine(&socket_path, engine.clone())
            .map_err(|e| format!("bind {socket_path}: {e}"))?;
        drop(_guard);

        let (stop_tx, stop_rx) = tokio::sync::oneshot::channel::<()>();
        let socket = CString::new(socket_path.clone())
            .map_err(|_| "socket_path contains a NUL byte".to_string())?;
        let cleanup = socket_path.clone();
        let (ready_tx, ready_rx) = mpsc::channel::<()>();
        let join = std::thread::Builder::new()
            .name("infinity-embedded-serve".into())
            .spawn(move || {
                let _ = ready_tx.send(());
                runtime.block_on(async move {
                    tokio::select! {
                        _ = bound.serve() => {}
                        _ = stop_rx => {}
                    }
                    // The last chance to checkpoint. Without it the host's data
                    // directory opens by replaying every record the log holds
                    // beyond the newest frame - and the replay reads each
                    // segment whole, so the log's whole history paid for every
                    // open. Skipped when the session wrote nothing, so a
                    // read-only host does not rewrite the block mirror on its
                    // way out. The checkpoint also reclaims the segments it
                    // seals (`flush_storage`), which is what keeps the log at
                    // the size of one checkpoint.
                    if engine.has_work_since_checkpoint().await {
                        // Seal the tails the checkpoint leaves in memory, now that
                        // they are bounded: this host sets a memory quota, so what
                        // is left is at most one watermark (~64 MiB of vectors,
                        // seconds to dump) instead of the whole table. Without the
                        // seal the rows written since the last quota dump have no
                        // chunk, and the next query rebuilds the multi-vector index
                        // from the rows rather than mapping the chunks - 248 s for
                        // 62 k units against milliseconds once they are all there.
                        engine.set_flush_dumps_mem_index(true);
                        if let Err(e) = engine.flush().await {
                            eprintln!("embedded engine: final checkpoint failed: {e}");
                        }
                    }
                    // The secondary tails, always: a dump is a key-to-rows map
                    // (no row is read to write it) and sealing it is what lets
                    // the next open skip a scan of the whole table - measured
                    // at 94.5 s of a 102 s open for 62 k units with one
                    // `path` index and nothing sealed. Not gated on the
                    // checkpoint flag: a checkpoint the append budget already
                    // ran leaves those tails unsealed, and an empty one is
                    // skipped inside `dump_index_transaction`.
                    //
                    // The vector tails are the other trade and stay put:
                    // sealing one rebuilds that index from the rows, ~64 ms per
                    // unit, against the ~1-2 ms per unit `RecoverMemIndex`
                    // spends rebuilding the same rows on the next open.
                    engine.dump_secondary_indexes().await;
                    // `WalManager::Cleanup` is the only path that removes a
                    // sealed segment, and it is the background trigger that
                    // normally runs it. This host has none, so the stop does
                    // it: without that step the log keeps every segment it
                    // ever wrote, which is the disk half of the same
                    // problem the replay half had.
                    if let Err(e) = engine.cleanup().await {
                        eprintln!("embedded engine: final cleanup failed: {e}");
                    }
                });
                // The select drops `serve` mid-listen; the socket file is ours
                // either way (the normal path removes it too).
                let _ = std::fs::remove_file(&cleanup);
                runtime.shutdown_timeout(Duration::from_millis(500));
            })
            .map_err(|e| format!("spawn: {e}"))?;
        // Not a synchronisation point for readiness - the socket is already
        // bound - but it keeps the thread from being created lazily under the
        // first client's connect.
        let _ = ready_rx.recv_timeout(Duration::from_secs(5));

        Ok(Box::into_raw(Box::new(InfEmbedded {
            socket,
            stop: Some(stop_tx),
            join: Some(join),
        })))
    }));

    match result {
        Ok(Ok(handle)) => handle,
        Ok(Err(message)) => {
            set_err(err, message);
            ptr::null_mut()
        }
        Err(payload) => {
            set_err(err, format!("panic: {}", panic_message(payload)));
            ptr::null_mut()
        }
    }
}

/// Stops the engine and waits for the serving thread, which unlinks the socket.
///
/// Returns 0 on success. A handle is not usable afterwards, and passing null is
/// a no-op (so a caller may `stop` from a destructor without tracking whether
/// `start` succeeded).
///
/// # Safety
/// `handle` must come from [`inf_embedded_start`] and must not have been
/// stopped already.
#[no_mangle]
pub unsafe extern "C" fn inf_embedded_stop(handle: *mut InfEmbedded) -> c_int {
    if handle.is_null() {
        return 0;
    }
    let mut boxed = Box::from_raw(handle);
    if let Some(stop) = boxed.stop.take() {
        let _ = stop.send(());
    }
    if let Some(join) = boxed.join.take() {
        let _ = join.join();
    }
    // The serving thread unlinks the socket on its way out, and it does so only
    // while the path still names the file it bound. Removing it here as well
    // would delete the socket of an engine that bound the same path between
    // that unlink and this join.
    0
}

/// The socket path this handle serves, borrowed from the handle.
///
/// # Safety
/// `handle` must come from [`inf_embedded_start`] and be alive.
#[no_mangle]
pub unsafe extern "C" fn inf_embedded_addr(handle: *const InfEmbedded) -> *const c_char {
    match handle.as_ref() {
        Some(h) => h.socket.as_ptr(),
        None => ptr::null(),
    }
}

/// Releases a string this library returned.
///
/// # Safety
/// `s` must be a pointer this library allocated (an element of `err`, or
/// [`inf_embedded_addr`]'s value must **not** be passed here).
#[no_mangle]
pub unsafe extern "C" fn inf_embedded_free(s: *mut c_char) {
    if !s.is_null() {
        libc::free(s as *mut libc::c_void);
    }
}

fn panic_message(payload: Box<dyn std::any::Any + Send>) -> String {
    match payload.downcast_ref::<&str>() {
        Some(s) => (*s).to_string(),
        None => match payload.downcast_ref::<String>() {
            Some(s) => s.clone(),
            None => "unknown panic".to_string(),
        },
    }
}

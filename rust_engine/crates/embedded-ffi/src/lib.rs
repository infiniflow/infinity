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

        let engine = runtime
            .block_on(Engine::open_async(&data_dir, Default::default()))
            .map_err(|e| format!("open {data_dir}: {e}"))?;
        // `install_engine_handle` records the running runtime, so the bind has
        // to happen inside its context.
        let _guard = runtime.enter();
        let bound = Server::bind_unix_with_engine(&socket_path, std::sync::Arc::new(engine))
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

/// Stops the engine, removes the socket file and waits for the serving thread.
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
    // The thread removes the file on its way out; a file still there means the
    // thread was never started, and leaving it would break the next bind.
    if let Ok(path) = boxed.socket.to_str() {
        let _ = std::fs::remove_file(path);
    }
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

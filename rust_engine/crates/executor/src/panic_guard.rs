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

//! Request-level panic containment.
//!
//! A statement runs on the caller's task, in the client's session, and the
//! engine takes its table maps, catalog handles and index registries with
//! `lock().unwrap()`. A panic anywhere in that chain used to kill the task: the
//! client's connection was dropped mid-request instead of answered, and every
//! later statement that touched the same `Mutex` panicked again because the lock
//! was left poisoned.
//!
//! This module is the *boundary*: [`guard`] / [`guard_async`] poll (or call)
//! something under `catch_unwind` and hand back the panic payload instead of
//! unwinding, so the front end renders its ordinary error shape — the thrift
//! arm replies with an `error_code`, HTTP with a 500 envelope, PG with an `E`
//! frame — and the process keeps serving the next connection.
//!
//! **Restarting is the deployment's decision, not this module's.** A panic means
//! the instance may be holding a poisoned lock, and the reference engine treats
//! it as fatal; that is expressed where the process lifecycle lives
//! (`Restart=always`, a container `restartPolicy`), not by calling
//! `std::process::exit` from a library. The front ends deliberately do not know
//! about either policy: they only ever see "this request failed".
//!
//! `cfg!(feature = "testing")` adds `Engine::arm_injected_panic`, which makes the
//! next statement **on that engine** panic so the boundaries can be exercised end
//! to end. The arming is per engine on purpose: a process-wide flag is consumed by
//! whichever request happens to run next, which in a parallel test binary is
//! usually another test's.

use std::any::Any;
use std::future::Future;
use std::panic::AssertUnwindSafe;
use std::pin::Pin;
use std::task::{Context, Poll};

use ::common::Error;

/// The panic's message, whichever shape the payload has.
pub fn message(payload: &(dyn Any + Send)) -> String {
    if let Some(text) = payload.downcast_ref::<&str>() {
        (*text).to_string()
    } else if let Some(text) = payload.downcast_ref::<String>() {
        text.clone()
    } else {
        "panic payload is not a string".to_string()
    }
}

/// The error a panicked request answers with, after reporting the panic once.
pub fn to_error(payload: Box<dyn Any + Send>) -> Error {
    let text = message(payload.as_ref());
    eprintln!("infinity: request panicked: {text}");
    Error::Unrecoverable(format!("internal error: {text}"))
}

/// Runs `f`, returning the panic payload instead of unwinding.
///
/// `AssertUnwindSafe` is the honest assertion here: the caller's state may be
/// half-updated after a panic, which is exactly why the payload is reported to
/// the client rather than retried or resumed.
pub fn guard<T>(f: impl FnOnce() -> T) -> std::thread::Result<T> {
    std::panic::catch_unwind(AssertUnwindSafe(f))
}

/// A future that turns the inner future's panic into `Err`.
///
/// The inner future is boxed, so the projection needs no `unsafe` (the crate
/// forbids it) and no `pin-project` dependency.
pub struct Guarded<F: Future> {
    inner: Pin<Box<F>>,
}

impl<F: Future> Future for Guarded<F> {
    type Output = std::thread::Result<F::Output>;

    fn poll(mut self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output> {
        let inner = &mut self.inner;
        match std::panic::catch_unwind(AssertUnwindSafe(|| inner.as_mut().poll(cx))) {
            Ok(polled) => polled.map(Ok),
            Err(payload) => Poll::Ready(Err(payload)),
        }
    }
}

/// [`guard`] for an awaited request.
///
/// A panic is only caught where it happens: a statement that panics on another
/// thread (a `spawn_blocking` job) has to be contained there, which is why the
/// front ends drive statements on the handler task.
pub async fn guard_async<T, F>(fut: F) -> std::thread::Result<T>
where
    F: Future<Output = T>,
{
    Guarded {
        inner: Box::pin(fut),
    }
    .await
}

/// Per-statement check for [`Engine::arm_injected_panic`]; a no-op without the
/// feature.
///
/// The arming lives **on the engine**, not in a process-wide static: the server's
/// tests run in parallel (`cargo test` gives each test its own thread), and a
/// global flag let one test's arming fire inside another test's query - the
/// panic-isolation test failed because a `/configs` request had consumed it.
#[cfg(feature = "testing")]
pub(crate) fn inject_statement_panic(armed: &std::sync::atomic::AtomicBool) {
    if armed.swap(false, std::sync::atomic::Ordering::SeqCst) {
        panic!("injected panic: panic-isolation test");
    }
}

#[cfg(feature = "testing")]
impl crate::Engine {
    /// Makes the next statement on this engine panic. Test-only: the `testing`
    /// feature is never on in a shipped build.
    pub fn arm_injected_panic(&self) {
        self.armed_panic
            .store(true, std::sync::atomic::Ordering::SeqCst);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn a_panicking_call_returns_the_payload() {
        let out = guard::<()>(|| panic!("boom in guard"));
        let payload = out.expect_err("a panic must come back as a payload");
        assert!(message(payload.as_ref()).contains("boom in guard"));
        let Error::Unrecoverable(text) = to_error(payload) else {
            panic!("a panic must become an unrecoverable error");
        };
        assert!(text.contains("boom in guard"), "{text}");
    }

    #[test]
    fn a_plain_value_passes_through() {
        assert_eq!(guard(|| 7).unwrap(), 7);
        assert!(guard(|| 7u8).is_ok());
    }

    #[tokio::test]
    async fn a_panicking_future_returns_the_payload() {
        let out = guard_async::<(), _>(async {
            if std::hint::black_box(true) {
                panic!("boom in future");
            }
        })
        .await;
        let payload = out.expect_err("a panicking future must come back as a payload");
        assert!(message(payload.as_ref()).contains("boom in future"));
    }

    #[tokio::test]
    async fn a_healthy_future_passes_through() {
        let out = guard_async(async { 5 }).await;
        assert_eq!(out.unwrap(), 5);
    }
}

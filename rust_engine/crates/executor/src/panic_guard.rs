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
//! `cfg!(feature = "testing")` adds [`arm_injected_panic`], which makes the next
//! statement panic so the boundaries can be exercised end to end.

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

#[cfg(feature = "testing")]
mod injection {
    use std::sync::atomic::{AtomicBool, Ordering};

    static ARMED: AtomicBool = AtomicBool::new(false);

    /// Makes the next statement panic. Test-only: the `testing` feature is never
    /// on in a shipped build.
    pub fn arm_injected_panic() {
        ARMED.store(true, Ordering::SeqCst);
    }

    /// Consumes the arming, if any.
    pub(crate) fn take_injected_panic() -> bool {
        ARMED.swap(false, Ordering::SeqCst)
    }
}

#[cfg(feature = "testing")]
pub use injection::arm_injected_panic;

/// Per-statement check for [`arm_injected_panic`]; a no-op without the feature.
pub(crate) fn inject_statement_panic() {
    #[cfg(feature = "testing")]
    if injection::take_injected_panic() {
        panic!("injected panic: panic-isolation test");
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

    #[test]
    fn a_panicking_future_returns_the_payload() {
        let out = executor_runtime().block_on(guard_async::<(), _>(async {
            if std::hint::black_box(true) {
                panic!("boom in future");
            }
        }));
        let payload = out.expect_err("a panicking future must come back as a payload");
        assert!(message(payload.as_ref()).contains("boom in future"));
    }

    #[test]
    fn a_healthy_future_passes_through() {
        let out = executor_runtime().block_on(guard_async(async { 5 }));
        assert_eq!(out.unwrap(), 5);
    }

    fn executor_runtime() -> tokio::runtime::Runtime {
        tokio::runtime::Builder::new_current_thread()
            .build()
            .expect("runtime")
    }
}

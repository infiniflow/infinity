//! The engine's entry points into [`index::parallel`].
//!
//! The fan-out primitive spawns its tasks on the caller's runtime, or on the
//! engine's when the calling thread has none entered. The engine's runtime is
//! [`crate::scheduler::scheduler`], created and installed by `Engine::open`
//! before any engine work runs, so a fan-out on a thread with no runtime of its
//! own (a synchronous `Engine::execute_blocking`, a blocking-pool body) still
//! lands on the engine's pool instead of collapsing to the caller's thread.
//!
//! `index::parallel` builds no runtime of its own: outside the engine, the
//! caller brings one.
//!
//! On a serving path the scheduler already exists, so this costs one atomic
//! load.

/// Runs the **async** body `f` over `items` with at most `workers` in flight.
///
/// The body's waits stay `.await`s; whatever it hands to the blocking pool it
/// does so itself, per leaf.
pub(crate) async fn for_each_async<T, F, Fut>(
    items: Vec<T>,
    workers: usize,
    f: F,
) -> common::Result<()>
where
    T: Send + 'static,
    F: Fn(T) -> Fut + Send + Sync + 'static,
    Fut: std::future::Future<Output = common::Result<()>> + Send + 'static,
{
    let _ = crate::scheduler::scheduler();
    index::parallel::for_each_async(items, workers, f).await
}

/// Whether the caller is a **runtime worker** (driving tasks), as opposed to a
/// blocking-pool thread or a plain thread.
///
/// A synchronous fan-out from a worker is what deadlocks: `drive_on` hands the
/// worker's core to a replacement and parks it, so the items it spawned need
/// another worker; if every worker is parked the same way, none is left to
/// poll them. Callers that can also run inline (a block decode) should skip the
/// fan-out while this is true.
pub(crate) fn on_a_runtime_worker() -> bool {
    tokio::runtime::Handle::try_current()
        .map(|handle| {
            handle.runtime_flavor() == tokio::runtime::RuntimeFlavor::MultiThread
                && tokio::task::try_id().is_some()
        })
        .unwrap_or(false)
}

/// [`for_each_blocking`] that collects the values, in input order.
pub(crate) fn map_blocking<T, U, F>(items: Vec<T>, workers: usize, f: F) -> common::Result<Vec<U>>
where
    T: Send + 'static,
    U: Send + 'static,
    F: Fn(T) -> common::Result<U> + Send + Sync + 'static,
{
    let _ = crate::scheduler::scheduler();
    index::parallel::map_blocking(items, workers, f)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Mutex;

    /// No `fan-out` pool exists any more, so a fan-out with no runtime on the
    /// calling thread must land on the engine's `task-scheduler` workers - both
    /// its worker threads and its blocking pool carry that name.
    ///
    /// The name matters: the deleted fallback built a separate `fan-out` pool,
    /// so a body named anything else means a second runtime came back.
    #[test]
    fn a_fan_out_with_no_caller_runtime_uses_the_engines_workers() {
        let dir = tempfile::tempdir().unwrap();
        let _engine = crate::Engine::open(dir.path()).unwrap();
        let seen: std::sync::Arc<Mutex<Vec<String>>> = Default::default();
        let record = std::sync::Arc::clone(&seen);
        map_blocking((0..8).collect(), 4, move |item| {
            record.lock().unwrap().push(
                std::thread::current()
                    .name()
                    .unwrap_or("<unnamed>")
                    .to_string(),
            );
            Ok(item)
        })
        .unwrap();
        let names = seen.lock().unwrap().clone();
        assert_eq!(names.len(), 8, "every item runs");
        for name in &names {
            assert!(
                name.starts_with("task-scheduler"),
                "a fan-out body ran on '{name}', not on the engine's runtime: {names:?}"
            );
        }
    }
}

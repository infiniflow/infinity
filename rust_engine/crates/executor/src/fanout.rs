//! The engine's entry points into [`index::parallel`].
//!
//! The fan-out primitive spawns its tasks on a runtime; the engine's runtime
//! is [`crate::scheduler::scheduler`], which installs its handle the first time
//! it is built. Calling through here guarantees that happened, so a fan-out on
//! a thread with no runtime of its own (a synchronous `Engine::execute_blocking`
//! or a helper binary) still runs in parallel instead of collapsing to the
//! caller's thread.
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

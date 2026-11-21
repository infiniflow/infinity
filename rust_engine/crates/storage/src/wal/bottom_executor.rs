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

//! Port of `src/storage/wal/bottom_executor.{cppm,impl}`.
//!
//! Ported from `src/storage/wal/bottom_executor.cppm`, which declares the
//! worker pool that runs `NewTxn::CommitBottom`. The **pool** is not ported: the
//! engine already owns an equivalent one (`Engine::bottom_queues`, one FIFO per
//! table, drained by a task on the engine's runtime), so a second implementation
//! would only be a second place for the commit order to drift. What survives
//! here is the interface between them - the two questions the drain asks a
//! queued commit, and the work it runs.
//!
//! The reference routes a transaction to a worker by
//! `CRC32(table_id) % executor_size`, so commits of one table stay ordered on
//! one thread; `Engine::submit_bottom` keeps that ordering per table instead of
//! per shard, which is the same guarantee without the hash.
//!
//! This lives in `storage` rather than `executor` so the storage layer can name
//! the type it hands over without depending on the engine that implements it.

/// What a drain needs from a queued commit (`bottom_executor_impl.cpp:49-52`).
pub trait BottomTask: Send + Sync + 'static {
    /// `NewTxn::GetTableIdStr` — the routing key.
    fn table_id(&self) -> &str;
    /// `NewTxn::GetTxnState() == TxnState::kCommitting`.
    fn is_committing(&self) -> bool;
    /// `NewTxn::CommitBottom`.
    ///
    /// Returns a future: the bottom half awaits its own index and block writes,
    /// so the drain awaits it rather than driving it with a nested `block_on`
    /// (which would park the worker those writes need).
    fn commit_bottom(&self)
        -> std::pin::Pin<Box<dyn std::future::Future<Output = ()> + Send + '_>>;
}

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

//! The write-ahead log (reference: src/storage/wal/).

pub mod block;
pub mod bottom_executor;
pub mod ddl_def;
pub mod index_def;
pub mod log_file;
pub mod wal_cmd;
pub mod wal_entry;
pub mod wal_manager;

pub use wal_manager::WalManager;

// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module;

import stl;
import config;
import catalog;
import txn_manager;
import buffer_manager;
import wal_manager;
import background_process;
import compaction_process;
import periodic_trigger_thread;
import log_file;
import memindex_tracer;
import persistence_manager;

export module storage;

namespace infinity {

class CleanupInfoTracer;

export class Storage {
public:
    explicit Storage(Config *config_ptr);

    [[nodiscard]] inline Catalog *catalog() noexcept { return new_catalog_.get(); }

    [[nodiscard]] inline BufferManager *buffer_manager() noexcept { return buffer_mgr_.get(); }

    [[nodiscard]] inline BGMemIndexTracer *memindex_tracer() noexcept { return memory_index_tracer_.get(); }

    [[nodiscard]] inline TxnManager *txn_manager() const noexcept { return txn_mgr_.get(); }

    [[nodiscard]] inline WalManager *wal_manager() const noexcept { return wal_mgr_.get(); }

    [[nodiscard]] inline BGTaskProcessor *bg_processor() const noexcept { return bg_processor_.get(); }

    [[nodiscard]] inline PeriodicTriggerThread *periodic_trigger_thread() const noexcept { return periodic_trigger_thread_.get(); }

    [[nodiscard]] inline CompactionProcessor *compaction_processor() const noexcept { return compact_processor_.get(); }

    [[nodiscard]] inline CleanupInfoTracer *cleanup_info_tracer() const noexcept { return cleanup_info_tracer_.get(); }

    [[nodiscard]] inline PersistenceManager *persistence_manager() noexcept { return persistence_manager_.get(); }

    StorageMode GetStorageMode() const;
    void SetStorageMode(StorageMode mode);

    void AttachCatalog(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos);

    Config *config() const { return config_ptr_; }

    void CreateDefaultDB();
private:
    Config *config_ptr_{};
    UniquePtr<Catalog> new_catalog_{};
    UniquePtr<BufferManager> buffer_mgr_{};
    UniquePtr<BGMemIndexTracer> memory_index_tracer_{};
    UniquePtr<TxnManager> txn_mgr_{};
    UniquePtr<WalManager> wal_mgr_{};
    UniquePtr<PersistenceManager> persistence_manager_{};
    UniquePtr<BGTaskProcessor> bg_processor_{};
    UniquePtr<CompactionProcessor> compact_processor_{};
    UniquePtr<PeriodicTriggerThread> periodic_trigger_thread_{};
    UniquePtr<CleanupInfoTracer> cleanup_info_tracer_{};

    mutable std::mutex mutex_;
    StorageMode current_storage_mode_{StorageMode::kUnInitialized};
};

} // namespace infinity

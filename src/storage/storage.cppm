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
import wal_manager;
import log_file;
import status;
import buffer_manager;

export module storage;

namespace infinity {

class ResultCacheManager;
class NewCatalog;
class NewTxnManager;
class KVStore;
class KVInstance;
class PeriodicTriggerThread;
class CompactionProcessor;
class DumpIndexProcessor;
class MemIndexAppender;
class BGTaskProcessor;
class BGMemIndexTracer;
class ObjectStorageProcess;
class Config;
class PersistenceManager;

export enum class ReaderInitPhase {
    kInvalid,
    kPhase1,
    kPhase2,
};

export class Storage {
public:
    explicit Storage(Config *config_ptr);

    ~Storage();

    [[nodiscard]] inline NewCatalog *new_catalog() noexcept { return new_catalog_.get(); }

    [[nodiscard]] inline BufferManager *buffer_manager() noexcept { return buffer_mgr_.get(); }

    [[nodiscard]] inline BGMemIndexTracer *memindex_tracer() noexcept { return memory_index_tracer_.get(); }

    [[nodiscard]] inline NewTxnManager *new_txn_manager() const noexcept { return new_txn_mgr_.get(); }

    [[nodiscard]] inline WalManager *wal_manager() const noexcept { return wal_mgr_.get(); }

    [[nodiscard]] inline PersistenceManager *persistence_manager() noexcept { return persistence_manager_.get(); }

    [[nodiscard]] inline BGTaskProcessor *bg_processor() const noexcept { return bg_processor_.get(); }

    [[nodiscard]] inline ObjectStorageProcess *object_storage_processor() const noexcept { return object_storage_processor_.get(); }

    [[nodiscard]] inline PeriodicTriggerThread *periodic_trigger_thread() const noexcept { return periodic_trigger_thread_.get(); }

    [[nodiscard]] inline CompactionProcessor *compaction_processor() const noexcept { return compact_processor_.get(); }

    [[nodiscard]] inline DumpIndexProcessor *dump_index_processor() const noexcept { return dump_index_processor_.get(); }

    [[nodiscard]] inline MemIndexAppender *mem_index_appender() const noexcept { return mem_index_appender_.get(); }

    UniquePtr<KVInstance> KVInstance();

    [[nodiscard]] KVStore *kv_store() const { return kv_store_.get(); }

    [[nodiscard]] ResultCacheManager *result_cache_manager() const noexcept;

    [[nodiscard]] ResultCacheManager *GetResultCacheManagerPtr() const noexcept;

    StorageMode GetStorageMode() const;
    Status SetStorageMode(StorageMode mode);
    Status AdminToReaderBottom(TxnTimeStamp system_start_ts);

    // Used for admin
    Status InitToAdmin();
    Status UnInitFromAdmin();
    Status AdminToReader();
    Status AdminToWriter();

    // Used for follower and learner
    Status ReaderToAdmin();
    Status ReaderToWriter();
    Status UnInitFromReader();

    // Used for leader and standalone
    Status WriterToAdmin();
    Status WriterToReader();
    Status UnInitFromWriter();

    void AttachCatalog(TxnTimeStamp checkpoint_ts);
    void RecoverMemIndex();

    Config *config() const { return config_ptr_; }
    ReaderInitPhase reader_init_phase() const { return reader_init_phase_; }

    void CreateDefaultDB();

private:
    Config *config_ptr_{};
    UniquePtr<WalManager> wal_mgr_{};
    UniquePtr<ObjectStorageProcess> object_storage_processor_{};
    UniquePtr<PersistenceManager> persistence_manager_{};
    UniquePtr<ResultCacheManager> result_cache_manager_{};
    UniquePtr<BufferManager> buffer_mgr_{};
    UniquePtr<NewCatalog> new_catalog_{};
    UniquePtr<BGMemIndexTracer> memory_index_tracer_{};
    UniquePtr<NewTxnManager> new_txn_mgr_{};
    UniquePtr<BGTaskProcessor> bg_processor_{};
    UniquePtr<CompactionProcessor> compact_processor_{};
    UniquePtr<DumpIndexProcessor> dump_index_processor_{};
    UniquePtr<MemIndexAppender> mem_index_appender_{};
    UniquePtr<PeriodicTriggerThread> periodic_trigger_thread_{};
    UniquePtr<KVStore> kv_store_{};

    mutable std::mutex mutex_;
    StorageMode current_storage_mode_{StorageMode::kUnInitialized};
    ReaderInitPhase reader_init_phase_{ReaderInitPhase::kInvalid};
};

} // namespace infinity

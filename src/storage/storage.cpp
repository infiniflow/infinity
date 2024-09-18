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

#include <filesystem>
#include <functional>
#include <regex>
#include <string>

module storage;

import config;
import stl;
import buffer_manager;
import default_values;
import wal_manager;
import catalog;
import txn_manager;
import builtin_functions;
import local_file_system;
import third_party;
import logger;

import txn;
import infinity_exception;
import status;
import background_process;
import compaction_process;
import status;
import bg_task;
import periodic_trigger_thread;
import periodic_trigger;
import log_file;

import query_context;
import infinity_context;
import memindex_tracer;
import cleanup_scanner;
import persistence_manager;
import extra_ddl_info;

namespace infinity {

Storage::Storage(Config *config_ptr) : config_ptr_(config_ptr) {}

StorageMode Storage::GetStorageMode() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return current_storage_mode_;
}

void Storage::SetStorageMode(StorageMode target_mode) {
    StorageMode current_mode = GetStorageMode();
    if (current_mode == target_mode) {
        LOG_WARN(fmt::format("Set unchanged mode"));
        return;
    }
    cleanup_info_tracer_ = MakeUnique<CleanupInfoTracer>();
    switch (current_mode) {
        case StorageMode::kUnInitialized: {
            if (target_mode != StorageMode::kAdmin) {
                UnrecoverableError("Attempt to set storage mode from UnInit to UnInit");
            }

            // Construct wal manager
            if (wal_mgr_ != nullptr) {
                UnrecoverableError("WAL manager was initialized before.");
            }

            wal_mgr_ = MakeUnique<WalManager>(this,
                                              config_ptr_->WALDir(),
                                              config_ptr_->DataDir(),
                                              config_ptr_->WALCompactThreshold(),
                                              config_ptr_->DeltaCheckpointThreshold(),
                                              config_ptr_->FlushMethodAtCommit());
            LOG_INFO(fmt::format("Set storage from un-init mode to admin"));
            break;
        }
        case StorageMode::kAdmin: {
            if (target_mode == StorageMode::kAdmin) {
                UnrecoverableError("Attempt to set storage mode from Admin to Admin");
            }

            if (target_mode == StorageMode::kUnInitialized) {
                wal_mgr_.reset();
                LOG_INFO(fmt::format("Set storage from admin mode to un-init"));
                break;
            }

            // Construct buffer manager
            String persistence_dir = config_ptr_->PersistenceDir();
            if (!persistence_dir.empty()) {
                if (persistence_manager_ != nullptr) {
                    UnrecoverableError("persistence_manager was initialized before.");
                }
                i64 persistence_object_size_limit = config_ptr_->PersistenceObjectSizeLimit();
                persistence_manager_ = MakeUnique<PersistenceManager>(persistence_dir, config_ptr_->DataDir(), (SizeT)persistence_object_size_limit);
            }

            // Construct buffer manager
            if (buffer_mgr_ != nullptr) {
                UnrecoverableError("Buffer manager was initialized before.");
            }
            buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->BufferManagerSize(),
                                                    MakeShared<String>(config_ptr_->DataDir()),
                                                    MakeShared<String>(config_ptr_->TempDir()),
                                                    persistence_manager_.get(),
                                                    config_ptr_->LRUNum());
            buffer_mgr_->Start();

            // Must init catalog before txn manager.
            // Replay wal file wrap init catalog
            TxnTimeStamp system_start_ts = wal_mgr_->ReplayWalFile(target_mode);
            if (system_start_ts == 0) {
                // Init database, need to create default_db
                LOG_INFO(fmt::format("Init a new catalog"));
                new_catalog_ = Catalog::NewCatalog();
            }

            BuiltinFunctions builtin_functions(new_catalog_);
            builtin_functions.Init();
            // Catalog finish init here.
            if (bg_processor_ != nullptr) {
                UnrecoverableError("Background processor was initialized before.");
            }
            bg_processor_ = MakeUnique<BGTaskProcessor>(wal_mgr_.get(), new_catalog_.get());

            // Construct txn manager
            if (txn_mgr_ != nullptr) {
                UnrecoverableError("Transaction manager was initialized before.");
            }
            txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(),
                                              buffer_mgr_.get(),
                                              bg_processor_.get(),
                                              wal_mgr_.get(),
                                              new_catalog_->next_txn_id(),
                                              system_start_ts);
            txn_mgr_->Start();

            // start WalManager after TxnManager since it depends on TxnManager.
            wal_mgr_->Start();

            if (system_start_ts == 0 && target_mode == StorageMode::kWritable) {
                CreateDefaultDB();
            }

            if (memory_index_tracer_ != nullptr) {
                UnrecoverableError("Memory index tracer was initialized before.");
            }
            memory_index_tracer_ = MakeUnique<BGMemIndexTracer>(config_ptr_->MemIndexMemoryQuota(), new_catalog_.get(), txn_mgr_.get());

            new_catalog_->StartMemoryIndexCommit();
            new_catalog_->MemIndexRecover(buffer_mgr_.get(), system_start_ts);

            bg_processor_->Start();

            if (target_mode == StorageMode::kWritable) {
                if (compact_processor_ != nullptr) {
                    UnrecoverableError("compact processor was initialized before.");
                }

                compact_processor_ = MakeUnique<CompactionProcessor>(new_catalog_.get(), txn_mgr_.get());
                compact_processor_->Start();
            }

            if (periodic_trigger_thread_ != nullptr) {
                UnrecoverableError("periodic trigger was initialized before.");
            }
            periodic_trigger_thread_ = MakeUnique<PeriodicTriggerThread>();

            i64 compact_interval = config_ptr_->CompactInterval() > 0 ? config_ptr_->CompactInterval() : 0;
            i64 optimize_interval = config_ptr_->OptimizeIndexInterval() > 0 ? config_ptr_->OptimizeIndexInterval() : 0;
            i64 cleanup_interval = config_ptr_->CleanupInterval() > 0 ? config_ptr_->CleanupInterval() : 0;
            i64 full_checkpoint_interval_sec = config_ptr_->FullCheckpointInterval() > 0 ? config_ptr_->FullCheckpointInterval() : 0;
            i64 delta_checkpoint_interval_sec = config_ptr_->DeltaCheckpointInterval() > 0 ? config_ptr_->DeltaCheckpointInterval() : 0;

            if (target_mode == StorageMode::kWritable) {
                periodic_trigger_thread_->full_checkpoint_trigger_ =
                    MakeShared<CheckpointPeriodicTrigger>(full_checkpoint_interval_sec, wal_mgr_.get(), true);
                periodic_trigger_thread_->delta_checkpoint_trigger_ =
                    MakeShared<CheckpointPeriodicTrigger>(delta_checkpoint_interval_sec, wal_mgr_.get(), false);
                periodic_trigger_thread_->compact_segment_trigger_ =
                    MakeShared<CompactSegmentPeriodicTrigger>(compact_interval, compact_processor_.get());
                periodic_trigger_thread_->optimize_index_trigger_ =
                    MakeShared<OptimizeIndexPeriodicTrigger>(optimize_interval, compact_processor_.get());
            }

            periodic_trigger_thread_->cleanup_trigger_ =
                MakeShared<CleanupPeriodicTrigger>(cleanup_interval, bg_processor_.get(), new_catalog_.get(), txn_mgr_.get());
            bg_processor_->SetCleanupTrigger(periodic_trigger_thread_->cleanup_trigger_);

            if (target_mode == StorageMode::kWritable) {
                auto txn = txn_mgr_->BeginTxn(MakeUnique<String>("ForceCheckpointTask"));
                auto force_ckp_task = MakeShared<ForceCheckpointTask>(txn, true, system_start_ts);
                bg_processor_->Submit(force_ckp_task);
                force_ckp_task->Wait();
                txn_mgr_->CommitTxn(txn);
            }

            periodic_trigger_thread_->Start();
            break;
        }
        case StorageMode::kReadable: {
            if (target_mode == StorageMode::kReadable) {
                UnrecoverableError("Attempt to set storage mode from Readable to Readable");
            }

            if (target_mode == StorageMode::kUnInitialized or target_mode == StorageMode::kAdmin) {
                periodic_trigger_thread_->Stop();
                periodic_trigger_thread_.reset();

                if (compact_processor_ != nullptr) {
                    UnrecoverableError("Compact processor shouldn't be set before");
                }

                bg_processor_->Stop();
                bg_processor_.reset();

                new_catalog_.reset();

                memory_index_tracer_.reset();

                wal_mgr_->Stop();
                wal_mgr_.reset();
                if (target_mode == StorageMode::kAdmin) {
                    // wal_manager stop won't reset many member. We need to recreate the wal_manager object.
                    wal_mgr_ = MakeUnique<WalManager>(this,
                                                      config_ptr_->WALDir(),
                                                      config_ptr_->DataDir(),
                                                      config_ptr_->WALCompactThreshold(),
                                                      config_ptr_->DeltaCheckpointThreshold(),
                                                      config_ptr_->FlushMethodAtCommit());
                }

                txn_mgr_->Stop();
                txn_mgr_.reset();

                buffer_mgr_->Stop();
                buffer_mgr_.reset();

                persistence_manager_.reset();
            }

            if (target_mode == StorageMode::kWritable) {
                if (compact_processor_ != nullptr) {
                    UnrecoverableError("compact processor was initialized before.");
                }

                compact_processor_ = MakeUnique<CompactionProcessor>(new_catalog_.get(), txn_mgr_.get());
                compact_processor_->Start();

                periodic_trigger_thread_->Stop();
                i64 compact_interval = config_ptr_->CompactInterval() > 0 ? config_ptr_->CompactInterval() : 0;
                i64 optimize_interval = config_ptr_->OptimizeIndexInterval() > 0 ? config_ptr_->OptimizeIndexInterval() : 0;
                //                i64 cleanup_interval = config_ptr_->CleanupInterval() > 0 ? config_ptr_->CleanupInterval() : 0;
                i64 full_checkpoint_interval_sec = config_ptr_->FullCheckpointInterval() > 0 ? config_ptr_->FullCheckpointInterval() : 0;
                i64 delta_checkpoint_interval_sec = config_ptr_->DeltaCheckpointInterval() > 0 ? config_ptr_->DeltaCheckpointInterval() : 0;
                periodic_trigger_thread_->full_checkpoint_trigger_ = MakeShared<CheckpointPeriodicTrigger>(full_checkpoint_interval_sec, wal_mgr_.get(), true);
                periodic_trigger_thread_->delta_checkpoint_trigger_ = MakeShared<CheckpointPeriodicTrigger>(delta_checkpoint_interval_sec, wal_mgr_.get(), false);
                periodic_trigger_thread_->compact_segment_trigger_ = MakeShared<CompactSegmentPeriodicTrigger>(compact_interval, compact_processor_.get());
                periodic_trigger_thread_->optimize_index_trigger_ = MakeShared<OptimizeIndexPeriodicTrigger>(optimize_interval, compact_processor_.get());
                periodic_trigger_thread_->Start();
            }
            break;
        }
        case StorageMode::kWritable: {
            if (target_mode == StorageMode::kWritable) {
                UnrecoverableError("Attempt to set storage mode from Writable to Writable");
            }

            if (target_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Attempt to set storage mode from Writeable to UnInit");
            }

            if (target_mode == StorageMode::kUnInitialized or target_mode == StorageMode::kAdmin) {
                periodic_trigger_thread_->Stop();
                periodic_trigger_thread_.reset();

                compact_processor_->Stop(); // Different from Readable
                compact_processor_.reset(); // Different from Readable

                bg_processor_->Stop();
                bg_processor_.reset();

                new_catalog_.reset();

                memory_index_tracer_.reset();

                wal_mgr_->Stop();
                wal_mgr_.reset();
                if (target_mode == StorageMode::kAdmin) {
                    // wal_manager stop won't reset many member. We need to recreate the wal_manager object.
                    wal_mgr_ = MakeUnique<WalManager>(this,
                                                      config_ptr_->WALDir(),
                                                      config_ptr_->DataDir(),
                                                      config_ptr_->WALCompactThreshold(),
                                                      config_ptr_->DeltaCheckpointThreshold(),
                                                      config_ptr_->FlushMethodAtCommit());
                }

                txn_mgr_->Stop();
                txn_mgr_.reset();

                buffer_mgr_->Stop();
                buffer_mgr_.reset();

                persistence_manager_.reset();
            }

            if (target_mode == StorageMode::kReadable) {
                periodic_trigger_thread_->Stop();
                periodic_trigger_thread_.reset();

                compact_processor_->Stop(); // Different from Readable
                compact_processor_.reset(); // Different from Readable

                i64 cleanup_interval = config_ptr_->CleanupInterval() > 0 ? config_ptr_->CleanupInterval() : 0;

                periodic_trigger_thread_ = MakeUnique<PeriodicTriggerThread>();
                periodic_trigger_thread_->cleanup_trigger_ =
                    MakeShared<CleanupPeriodicTrigger>(cleanup_interval, bg_processor_.get(), new_catalog_.get(), txn_mgr_.get());
                bg_processor_->SetCleanupTrigger(periodic_trigger_thread_->cleanup_trigger_);
                periodic_trigger_thread_->Start();
            }
            break;
        }
    }

    std::unique_lock<std::mutex> lock(mutex_);
    current_storage_mode_ = target_mode;
}

void Storage::AttachCatalog(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos) {
    new_catalog_ = Catalog::LoadFromFiles(full_ckp_info, delta_ckp_infos, buffer_mgr_.get());
}

void Storage::CreateDefaultDB() {
    Txn *new_txn = txn_mgr_->BeginTxn(MakeUnique<String>("create db1"));

    // Txn1: Create db1, OK
    Status status = new_txn->CreateDatabase("default_db", ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError("Can't initial 'default_db'");
    }
    txn_mgr_->CommitTxn(new_txn);
}

} // namespace infinity

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

namespace infinity {

Storage::Storage(Config *config_ptr) : config_ptr_(config_ptr) {}

void Storage::Init() {
    // Construct buffer manager
    buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->BufferManagerSize(),
                                            MakeShared<String>(config_ptr_->DataDir()),
                                            MakeShared<String>(config_ptr_->TempDir()),
                                            config_ptr_->LRUNum());

    // Construct wal manager
    wal_mgr_ = MakeUnique<WalManager>(this,
                                      config_ptr_->WALDir(),
                                      config_ptr_->DataDir(),
                                      config_ptr_->WALCompactThreshold(),
                                      config_ptr_->DeltaCheckpointThreshold(),
                                      config_ptr_->FlushMethodAtCommit());

    // Must init catalog before txn manager.
    // Replay wal file wrap init catalog
    TxnTimeStamp system_start_ts = wal_mgr_->ReplayWalFile();

    BuiltinFunctions builtin_functions(new_catalog_);
    builtin_functions.Init();
    // Catalog finish init here.

    bg_processor_ = MakeUnique<BGTaskProcessor>(wal_mgr_.get(), new_catalog_.get());
    // Construct txn manager
    txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(),
                                      buffer_mgr_.get(),
                                      bg_processor_.get(),
                                      wal_mgr_.get(),
                                      new_catalog_->next_txn_id(),
                                      system_start_ts);


    compact_processor_ = MakeUnique<CompactionProcessor>(new_catalog_.get(), txn_mgr_.get());

    txn_mgr_->Start();
    // start WalManager after TxnManager since it depends on TxnManager.
    wal_mgr_->Start();

    new_catalog_->StartMemoryIndexCommit();
    new_catalog_->MemIndexRecover(buffer_mgr_.get());

    bg_processor_->Start();
    if (compact_processor_.get() != nullptr) {
        compact_processor_->Start();
    }

    auto txn = txn_mgr_->BeginTxn(MakeUnique<String>("ForceCheckpointTask"));
    auto force_ckp_task = MakeShared<ForceCheckpointTask>(txn, true);
    bg_processor_->Submit(force_ckp_task);
    force_ckp_task->Wait();
    txn_mgr_->CommitTxn(txn);

    {
        i64 compact_interval = config_ptr_->CompactInterval() > 0 ? config_ptr_->CompactInterval() : 0;
        i64 optimize_interval = config_ptr_->OptimizeIndexInterval() > 0 ? config_ptr_->OptimizeIndexInterval() : 0;
        i64 cleanup_interval = config_ptr_->CleanupInterval() > 0 ? config_ptr_->CleanupInterval() : 0;
        i64 full_checkpoint_interval_sec = config_ptr_->FullCheckpointInterval() > 0 ? config_ptr_->FullCheckpointInterval() : 0;
        i64 delta_checkpoint_interval_sec = config_ptr_->DeltaCheckpointInterval() > 0 ? config_ptr_->DeltaCheckpointInterval() : 0;

        periodic_trigger_thread_ = MakeUnique<PeriodicTriggerThread>();
        periodic_trigger_thread_->cleanup_trigger_ = MakeUnique<CleanupPeriodicTrigger>(cleanup_interval, bg_processor_.get(), new_catalog_.get(), txn_mgr_.get());
        periodic_trigger_thread_->full_checkpoint_trigger_ = MakeUnique<CheckpointPeriodicTrigger>(full_checkpoint_interval_sec, wal_mgr_.get(), true);
        periodic_trigger_thread_->delta_checkpoint_trigger_ = MakeUnique<CheckpointPeriodicTrigger>(delta_checkpoint_interval_sec, wal_mgr_.get(), false);
        periodic_trigger_thread_->compact_segment_trigger_ = MakeUnique<CompactSegmentPeriodicTrigger>(compact_interval, compact_processor_.get());
        periodic_trigger_thread_->optimize_index_trigger_ = MakeUnique<OptimizeIndexPeriodicTrigger>(optimize_interval, compact_processor_.get());

        periodic_trigger_thread_->Start();
    }
}

void Storage::UnInit() {
    LOG_INFO("Close storage ...\n");
    periodic_trigger_thread_->Stop();
    if (compact_processor_.get() != nullptr) {
        compact_processor_->Stop();
    }
    bg_processor_->Stop();
    wal_mgr_->Stop();
    txn_mgr_->Stop();

    txn_mgr_.reset();
    if (compact_processor_.get() != nullptr) {
        compact_processor_.reset();
    }
    bg_processor_.reset();
    wal_mgr_.reset();
    new_catalog_.reset();
    buffer_mgr_.reset();
    config_ptr_ = nullptr;
    LOG_INFO("Close storage successfully\n");
}

void Storage::AttachCatalog(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos, const String &data_dir) {
    new_catalog_ = Catalog::LoadFromFiles(data_dir, full_ckp_info, delta_ckp_infos, buffer_mgr_.get());
}

void Storage::InitNewCatalog() {
    LOG_INFO("Init new catalog");
    auto data_dir = config_ptr_->DataDir();
    SharedPtr<String> data_dir_ptr = MakeShared<String>(data_dir);
    new_catalog_ = Catalog::NewCatalog(std::move(data_dir_ptr), true);
}

} // namespace infinity

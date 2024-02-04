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
import backgroud_process;
import status;
import bg_task;

namespace infinity {

Storage::Storage(const Config *config_ptr) : config_ptr_(config_ptr) {}

void Storage::Init() {
    // Check the data dir to get latest catalog file.
    String catalog_dir = String(*config_ptr_->data_dir()) + "/" + String(CATALOG_FILE_DIR);

    // Construct buffer manager
    buffer_mgr_ = MakeUnique<BufferManager>(config_ptr_->buffer_pool_size(), config_ptr_->data_dir(), config_ptr_->temp_dir());

    // Construct wal manager
    wal_mgr_ = MakeUnique<WalManager>(this,
                                      Path(*config_ptr_->wal_dir()) / WAL_FILE_TEMP_FILE,
                                      config_ptr_->wal_size_threshold(),
                                      config_ptr_->full_checkpoint_interval_sec(),
                                      config_ptr_->delta_checkpoint_interval_sec(),
                                      config_ptr_->delta_checkpoint_interval_wal_bytes());

    // Must init catalog before txn manager.
    // Replay wal file wrap init catalog
    TxnTimeStamp system_start_ts = wal_mgr_->ReplayWalFile();

    bg_processor_ = MakeUnique<BGTaskProcessor>(wal_mgr_.get());
    // Construct txn manager
    txn_mgr_ = MakeUnique<TxnManager>(new_catalog_.get(),
                                      buffer_mgr_.get(),
                                      bg_processor_.get(),
                                      std::bind(&WalManager::PutEntry, wal_mgr_.get(), std::placeholders::_1),
                                      new_catalog_->next_txn_id_,
                                      system_start_ts);

    txn_mgr_->Start();
    // start WalManager after TxnManager since it depends on TxnManager.
    wal_mgr_->Start();

    bg_processor_->Start();

    BuiltinFunctions builtin_functions(new_catalog_);
    builtin_functions.Init();

    auto txn = txn_mgr_->CreateTxn();
    txn->Begin();
    SharedPtr<ForceCheckpointTask> force_ckp_task = MakeShared<ForceCheckpointTask>(txn);
    bg_processor_->Submit(force_ckp_task);
    force_ckp_task->Wait();
    txn_mgr_->CommitTxn(txn);
}

void Storage::UnInit() {
    fmt::print("Shutdown storage ...\n");
    bg_processor_->Stop();

    wal_mgr_->Stop();

    txn_mgr_.reset();
    bg_processor_.reset();
    wal_mgr_.reset();

    // Buffer Manager need to be destroyed before catalog. since buffer manage hold the raw pointer owned by catalog:
    // such as index definition and index base of IndexFileWorker
    buffer_mgr_.reset();
    new_catalog_.reset();

    config_ptr_ = nullptr;
    fmt::print("Shutdown storage successfully\n");
}

void Storage::AttachCatalog(const Vector<String> &catalog_files) {
    LOG_INFO(fmt::format("Attach catalogs from {} files", catalog_files.size()));
    for (const auto &catalog_file : catalog_files) {
        LOG_TRACE(fmt::format("Catalog file: {}", catalog_file.c_str()));
    }
    new_catalog_ = NewCatalog::LoadFromFiles(catalog_files, buffer_mgr_.get());
}

void Storage::InitNewCatalog() {
    LOG_INFO("Init new catalog");
    String catalog_dir = String(*config_ptr_->data_dir()) + "/" + String(CATALOG_FILE_DIR);
    LocalFileSystem fs;
    if (!fs.Exists(catalog_dir)) {
        fs.CreateDirectory(catalog_dir);
    }
    new_catalog_ = MakeUnique<NewCatalog>(MakeShared<String>(catalog_dir), true);
}

} // namespace infinity

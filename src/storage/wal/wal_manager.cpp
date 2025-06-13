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
#include <fstream>
#include <thread>

import stl;
import logger;
import txn_manager;
import new_txn_manager;
import new_txn;
import storage;
import virtual_store;
import third_party;
import table_entry_type;
import infinity_context;
import txn_store;
import data_access_state;
import status;
import bg_task;
import background_process;
import extra_ddl_info;

import infinity_exception;
import block_column_entry;
import compact_state_data;
import build_fast_rough_filter_task;
import catalog_delta_entry;
import column_def;

import db_meta;
import db_entry;
import table_meta;
import table_entry;
import segment_entry;
import block_entry;
import table_index_meta;
import table_index_entry;
import segment_index_entry;
import log_file;
import default_values;
import defer_op;
import index_base;
import base_table_ref;
import cluster_manager;
import admin_statement;
import cleanup_scanner;
import global_resource_usage;
import txn_state;
import meta_info;
import wal_entry;
import block_index;
import bottom_executor;

module wal_manager;

namespace infinity {

WalManager::WalManager(Storage *storage, String wal_dir, u64 wal_size_threshold, FlushOptionType flush_option)
    : cfg_wal_size_threshold_(wal_size_threshold), wal_dir_(wal_dir), wal_path_(wal_dir + "/" + WalFile::TempWalFilename()), storage_(storage),
      running_(false), flush_option_(flush_option), last_ckp_wal_size_(0), checkpoint_in_progress_(false), last_ckp_ts_(UNCOMMIT_TS) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("WalManager");
#endif
}

WalManager::WalManager(Storage *storage, String wal_dir, String data_dir, u64 wal_size_threshold, FlushOptionType flush_option)
    : cfg_wal_size_threshold_(wal_size_threshold), wal_dir_(wal_dir), wal_path_(wal_dir + "/" + WalFile::TempWalFilename()), data_path_(data_dir),
      storage_(storage), running_(false), flush_option_(flush_option), last_ckp_wal_size_(0), checkpoint_in_progress_(false),
      last_ckp_ts_(UNCOMMIT_TS) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("WalManager");
#endif
}

WalManager::~WalManager() {
    if (running_.load()) {
        Stop();
    }
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("WalManager");
#endif
}

void WalManager::Start() {
    LOG_INFO("WAL manager is starting...");
    bool expected = false;
    bool changed = running_.compare_exchange_strong(expected, true);
    if (!changed)
        return;
    if (!VirtualStore::Exists(wal_dir_)) {
        VirtualStore::MakeDirectory(wal_dir_);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        String error_message = fmt::format("Failed to open wal file: {}", wal_path_);
        UnrecoverableError(error_message);
    }
    LOG_INFO(fmt::format("Open wal file: {}", wal_path_));

    wal_size_ = 0;
    new_flush_thread_ = Thread([this] { NewFlush(); });
    // checkpoint_thread_ = Thread([this] { CheckpointTimer(); });

    bottom_executor_ = MakeUnique<BottomExecutor>();
    bottom_executor_->Start(storage_->config()->BottomExecutorWorker());
    LOG_INFO("WAL manager is started.");
}

void WalManager::Stop() {
    LOG_INFO("WAL manager is stopping...");

    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("WAL manager was stopped...");
        return;
    }

    bottom_executor_->Stop();

    LOG_TRACE("Begin to stop txn manager.");
    // Notify txn manager to stop.
    if (NewTxnManager *new_txn_mgr = storage_->new_txn_manager(); new_txn_mgr) {
        new_txn_mgr->Stop();
    } else {
        UnrecoverableError("WAL manager stop failed: txn manager is nullptr");
    }

    // pop all the entries in the queue. and notify the condition variable.
    new_wait_flush_.Enqueue(nullptr);

    LOG_TRACE("Stop the new flush thread");
    new_flush_thread_.join();

    ofs_.close();
    LOG_INFO("WAL manager is stopped.");
}

void WalManager::SubmitTxn(Vector<NewTxn *> &txn_array) {
    if (!running_.load()) {
        return;
    }
    new_wait_flush_.EnqueueBulk(txn_array);
}

TxnTimeStamp WalManager::LastCheckpointTS() const {
    std::lock_guard guard(last_ckp_ts_mutex_);
    return last_ckp_ts_ == UNCOMMIT_TS ? 0 : last_ckp_ts_;
}

void WalManager::SetLastCheckpointTS(TxnTimeStamp new_last_ckp_ts) {
    std::lock_guard guard(last_ckp_ts_mutex_);
    if (new_last_ckp_ts > last_ckp_ts_ or last_ckp_ts_ == UNCOMMIT_TS) {
        last_ckp_ts_ = new_last_ckp_ts;
    }
    return;
}

Vector<SharedPtr<String>> WalManager::GetDiffWalEntryString(TxnTimeStamp start_timestamp) const {

    Vector<SharedPtr<String>> log_strings;

    Vector<String> wal_list{};
    {
        auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir_);
        if (wal_infos.size() > 1) {
            std::sort(wal_infos.begin(), wal_infos.end(), [](const WalFileInfo &a, const WalFileInfo &b) {
                return a.max_commit_ts_ > b.max_commit_ts_;
            });
        }
        if (temp_wal_info.has_value()) {
            wal_list.push_back(temp_wal_info->path_);
            LOG_INFO(fmt::format("Find temp wal file: {}", temp_wal_info->path_));
        }
        for (const auto &wal_info : wal_infos) {
            wal_list.push_back(wal_info.path_);
            LOG_INFO(fmt::format("Find wal file: {}", wal_info.path_));
        }
        // e.g. wal_list = {wal.log , wal.log.100 , wal.log.50}
    }
    if (wal_list.empty()) {
        LOG_INFO(fmt::format("No checkpoint found, terminate replaying WAL"));
        return log_strings;
    }

    TxnTimeStamp max_checkpoint_ts = 0; // the max commit ts that has been checkpointed
    Vector<SharedPtr<WalEntry>> log_entries;
    String catalog_dir = "";

    {
        // if no checkpoint, max_checkpoint_ts is 0
        WalListIterator iterator(wal_list);
        // phase 1: find the max commit ts and catalog path
        LOG_INFO("Replay phase 1: find the max commit ts and catalog path");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }
            LOG_INFO(wal_entry->ToString());

            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_checkpoint_ts = checkpoint_cmd->max_commit_ts_;
                std::string catalog_path = fmt::format("{}/{}", data_path_, "catalog");
                catalog_dir = Path(fmt::format("{}/{}", catalog_path, checkpoint_cmd->catalog_name_)).parent_path().string();
                break;
            }
            log_entries.push_back(wal_entry);
        }
        LOG_INFO(fmt::format("Find checkpoint max commit ts: {}", max_checkpoint_ts));

        // phase 2: by the max commit ts, find the entries to replay
        LOG_INFO("Replay phase 2: by the max commit ts, find the entries to replay");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
                break;
            }
            // LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ > max_checkpoint_ts) {
                log_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }

    auto catalog_fileinfo = CatalogFile::ParseValidCheckpointFilenames(catalog_dir, max_checkpoint_ts);
    if (!catalog_fileinfo.has_value()) {
        String error_message = fmt::format("Wal Replay: Parse catalog file failed, catalog_dir: {}", catalog_dir);
        UnrecoverableError(error_message);
    }
    auto &[full_catalog_fileinfo, delta_catalog_fileinfo_array] = catalog_fileinfo.value();

    SharedPtr<WalEntry> ckp_wal_entry = MakeShared<WalEntry>();
    String full_ckp_filename = std::filesystem::path(full_catalog_fileinfo.path_).filename();
    ckp_wal_entry->cmds_.push_back(MakeShared<WalCmdCheckpoint>(full_catalog_fileinfo.max_commit_ts_, true, "catalog", full_ckp_filename));
    // Use max ckp commit ts
    ckp_wal_entry->commit_ts_ = std::max(ckp_wal_entry->commit_ts_, full_catalog_fileinfo.max_commit_ts_);
    for (const auto &delta_catalog_file : delta_catalog_fileinfo_array) {
        String delta_ckp_filename = std::filesystem::path(delta_catalog_file.path_).filename();
        ckp_wal_entry->cmds_.push_back(MakeShared<WalCmdCheckpoint>(delta_catalog_file.max_commit_ts_, false, "catalog", delta_ckp_filename));
        ckp_wal_entry->commit_ts_ = std::max(ckp_wal_entry->commit_ts_, delta_catalog_file.max_commit_ts_);
    }
    log_entries.emplace_back(ckp_wal_entry);

    std::reverse(log_entries.begin(), log_entries.end());
    log_strings.reserve(log_entries.size());

    for (const auto &log_entry : log_entries) {
        i32 exp_size = log_entry->GetSizeInBytes();

        // Serialize the log entry
        SharedPtr<String> buf_ptr = MakeShared<String>();
        buf_ptr->resize(exp_size);
        char *ptr = buf_ptr->data();
        log_entry->WriteAdv(ptr);
        i32 act_size = ptr - buf_ptr->data();
        if (exp_size != act_size) {
            String error_message = fmt::format("WalManager::Flush WalEntry estimated size {} differ with the actual one {}, entry {}",
                                               exp_size,
                                               act_size,
                                               log_entry->ToString());
            UnrecoverableError(error_message);
        }

        // LOG_TRACE(fmt::format("SYNC: {}", log_entry->ToString()));
        log_strings.emplace_back(buf_ptr);
    }

    return log_strings;
}

// Flush is scheduled regularly. It collects a batch of transactions, sync
// wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
// ~10s. So it's necessary to sync for a batch of transactions, and to
// checkpoint for a batch of sync.
void WalManager::NewFlush() {
    LOG_TRACE("WalManager::Flush log mainloop begin");

    Deque<NewTxn *> txn_batch{};
    ClusterManager *cluster_manager = nullptr;
    while (running_.load()) {
        new_wait_flush_.DequeueBulk(txn_batch);
        if (txn_batch.empty()) {
            LOG_WARN("WalManager::Dequeue empty batch logs");
            continue;
        }

        for (const auto &txn : txn_batch) {
            if (txn == nullptr) {
                // terminate entry
                LOG_INFO("Terminate WAL Manager flush thread");
                running_ = false;
                break;
            }
            TxnState txn_state = txn->GetTxnState();

            switch (txn_state) {
                case TxnState::kCommitting: {
                    break;
                }
                default: {
                    String error_message = fmt::format("NewTxnManager::Flush: txn state is {}, not committing", TxnState2Str(txn_state));
                    UnrecoverableError(error_message);
                }
            }

            // Empty WalEntry (read-only transactions) shouldn't go into WalManager.
            WalEntry *entry = txn->GetWALEntry();

            if (entry->cmds_.empty()) {
                continue;
            }

            if (txn->GetTxnType() == TransactionType::kNewCheckpoint) {
                LOG_INFO(fmt::format("Full or delta checkpoint begin at {}, cur txn commit_ts: {}, txn_id: {}, swap to new wal file",
                                     txn->BeginTS(),
                                     txn->CommitTS(),
                                     txn->TxnID()));
                this->SwapWalFile(max_commit_ts_, true);
            }

            for (const SharedPtr<WalCmd> &cmd : entry->cmds_) {
                LOG_TRACE(fmt::format("TXN: {}, WAL CMD: {}", entry->txn_id_, cmd->ToString()));
            }

            i32 exp_size = entry->GetSizeInBytes();
            SharedPtr<String> buf = MakeShared<String>(exp_size, 0);
            char *ptr = buf->data();
            entry->WriteAdv(ptr);
            i32 act_size = ptr - buf->data();
            if (exp_size != act_size) {
                String error_message = fmt::format("WalManager::Flush WalEntry estimated size {} differ with the actual one {}, entry {}",
                                                   exp_size,
                                                   act_size,
                                                   entry->ToString());
                UnrecoverableError(error_message);
            }
            ofs_.write(buf->data(), ptr - buf->data());

            if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader) {
                if (cluster_manager == nullptr) {
                    cluster_manager = InfinityContext::instance().cluster_manager();
                }
                cluster_manager->PrepareLogs(buf);
            }

            LOG_TRACE(fmt::format("WalManager::Flush done writing wal for txn_id {}, commit_ts {}", entry->txn_id_, entry->commit_ts_));

            UpdateCommitState(entry->commit_ts_, wal_size_ + act_size);
        }

        if (!running_.load()) {
            break;
        }

        switch (flush_option_) {
            case FlushOptionType::kFlushAtOnce: {
                ofs_.flush();
                break;
            }
            case FlushOptionType::kOnlyWrite: {
                ofs_.flush(); // FIXME: not flush, only write
                break;
            }
            case FlushOptionType::kFlushPerSecond: {
                ofs_.flush(); // FIXME: not flush, flush per second
                break;
            }
        }

        if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader) {
            cluster_manager->SyncLogs();
        }

        // Commit bottom
        for (const auto &txn : txn_batch) {
            bottom_executor_->Submit(txn);
            // TODO: if txn is checkpoint, swap WAL file
        }
        txn_batch.clear();

        // Check if the wal file is too large, swap to a new one.
        try {
            auto file_size = VirtualStore::GetFileSize(wal_path_);
            if (file_size > cfg_wal_size_threshold_) {
                LOG_INFO(fmt::format("WAL size: {} is larger than threshold: {}", file_size, cfg_wal_size_threshold_));
                this->SwapWalFile(max_commit_ts_, true);
            }
        } catch (RecoverableException &e) {
            LOG_ERROR(e.what());
        } catch (UnrecoverableException &e) {
            LOG_CRITICAL(e.what());
            throw e;
        }

        LOG_TRACE("WAL flush is finished.");
    }
    LOG_TRACE("WalManager::Flush mainloop end");
}

void WalManager::FlushLogByReplication(const Vector<String> &synced_logs, bool on_startup) {
    if (on_startup) {
        // To get max commit TS
        Vector<String> wal_list{};
        {
            auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir_);
            if (!wal_infos.empty()) {
                std::sort(wal_infos.begin(), wal_infos.end(), [](const WalFileInfo &a, const WalFileInfo &b) {
                    return a.max_commit_ts_ > b.max_commit_ts_;
                });
            }
            if (temp_wal_info.has_value()) {
                wal_list.push_back(temp_wal_info->path_);
                LOG_INFO(fmt::format("Find temp wal file: {}", temp_wal_info->path_));
            }
            for (const auto &wal_info : wal_infos) {
                wal_list.push_back(wal_info.path_);
                LOG_INFO(fmt::format("Find wal file: {}", wal_info.path_));
            }
            // e.g. wal_list = {wal.log , wal.log.100 , wal.log.50}
        }

        TxnTimeStamp max_commit_ts = 0;
        if (!wal_list.empty()) {
            WalListIterator iterator(wal_list);
            while (iterator.HasNext()) {
                auto wal_entry = iterator.Next();
                if (wal_entry.get() == nullptr) {
                    String error_message = "Found unexpected bad wal entry when got replicate logs";
                    LOG_ERROR(error_message);
                    break;
                }
                LOG_TRACE(wal_entry->ToString());
                max_commit_ts = std::max(max_commit_ts, wal_entry->commit_ts_);
            }
            LOG_INFO(fmt::format("Find checkpoint max commit ts: {}", max_commit_ts));
        }

        // Rename the old WAL file and open new WAL file, use the max_commit_ts. If duplicate filename, just remove current WAL file and create a new
        // one.
        LOG_INFO("FlushLogByReplication, rename old WAL file and open new WAL");
        SwapWalFile(max_commit_ts, false);
    }

    for (const String &synced_log : synced_logs) {
        ofs_.write(synced_log.c_str(), synced_log.size());
    }
    ofs_.flush();
}

bool WalManager::SetCheckpointing() {
    bool expect = false;
    if (checkpoint_in_progress_.compare_exchange_strong(expect, true)) {
        return true;
    }
    return false;
}

bool WalManager::UnsetCheckpoint() {
    bool expect = true;
    if (checkpoint_in_progress_.compare_exchange_strong(expect, false)) {
        return true;
    }
    return false;
}

bool WalManager::IsCheckpointing() const { return checkpoint_in_progress_; }

/*****************************************************************************
 * CHECKPOINT WAL FILE
 *****************************************************************************/
void WalManager::UpdateCommitState(TxnTimeStamp commit_ts, i64 wal_size) {
    std::scoped_lock lock(mutex2_);
    if (commit_ts <= max_commit_ts_ || wal_size < wal_size_ /*equal when replay because both are 0*/) {
        String error_message = fmt::format("WalManager::UpdateCommitState commit_ts {} <= max_commit_ts_ {} or wal_size {} <= wal_size_ {}",
                                           commit_ts,
                                           max_commit_ts_,
                                           wal_size,
                                           wal_size_);
        // UnrecoverableError(error_message);
        LOG_ERROR(error_message);
        return;
    }
    max_commit_ts_ = commit_ts;
    wal_size_ = wal_size;
}

Tuple<TxnTimeStamp, i64> WalManager::GetCommitState() {
    std::scoped_lock lock(mutex2_);
    return {max_commit_ts_, wal_size_};
}

void WalManager::SetLastCkpWalSize(i64 wal_size) {
    std::lock_guard guard(mutex2_);
    last_ckp_wal_size_ = wal_size;
}

i64 WalManager::GetLastCkpWalSize() {
    std::lock_guard guard(mutex2_);
    return last_ckp_wal_size_;
}

/**
 * @brief Swap the wal file to a new one.
 * We will swap a new wal file when the current wal file is too large.
 * Just rename the current wal file to a new one, and create a new wal file with
 * the original name. So we only focus on the current wal file: wal.log When
 * replaying the wal file, we will just start with the wal.log file.
 * @param max_commit_ts The max commit timestamp of the transactions in the
 * current wal file.
 */
void WalManager::SwapWalFile(const TxnTimeStamp max_commit_ts, bool error_if_duplicate) {
    if (max_commit_ts <= last_swap_wal_ts_) {
        LOG_WARN(fmt::format("Skip swap wal file, max_commit_ts: {} <= last_swap_wal_ts_: {}", max_commit_ts, last_swap_wal_ts_));
        return;
    }

    if (ofs_.is_open()) {
        ofs_.close();
    }

    String new_file_path = fmt::format("{}/{}", wal_dir_, WalFile::WalFilename(max_commit_ts));
    LOG_INFO(fmt::format("Wal {} swap to new path: {}, error_if_duplicate: {}", wal_path_, new_file_path, error_if_duplicate));

    if (VirtualStore::Exists(new_file_path)) {
        if (error_if_duplicate) {
            UnrecoverableError(fmt::format("File: {}, exists!", new_file_path));
        } else {
            LOG_INFO(fmt::format("Delete WAL file: {}", wal_path_));
            VirtualStore::DeleteFile(wal_path_);
        }
    } else {
        // Rename the current wal file to a new one.
        if (VirtualStore::Exists(wal_path_)) {
            VirtualStore::Rename(wal_path_, new_file_path);
        }
    }

    if (!VirtualStore::Exists(wal_dir_)) {
        VirtualStore::MakeDirectory(wal_dir_);
    }

    // Create a new wal file with the original name.
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        String error_message = fmt::format("Failed to open wal file: {}", wal_path_);
        UnrecoverableError(error_message);
    }

    last_swap_wal_ts_ = max_commit_ts;
    LOG_INFO(fmt::format("Open new wal file {}", wal_path_));
}

String WalManager::GetWalFilename() const { return wal_path_; }

/*****************************************************************************
 * REPLAY WAL FILE
 *****************************************************************************/
/**
 * @brief Replay the wal file.
 *  wal format: wal.log.<max_commit_ts>
 *  the wal.log file is the current wal file.
 *  the most recent wal.log file is the current wal file.
 *
 *  time min -------------------------------------------------------------------------------------------------------------------------------> time max
 *  ================ =========================  =================================================================================================
 *  |   wal.log.1  | |       wal.log.3       |  |                                wal.log                                                        |
 *  ---------------- -------------------------  --------------------------------------------------------------------------------------------------
 *  |[entry｜entry]| |  [entry｜entry｜entry] ｜ | [entry｜entry｜entry| <checkpoint{ max_commit_ts ; catalog_path }>｜entry?(bad checksum)｜entry]|
 *  ================ =========================  ==================================================================================================
 *                                                  ⬆️     ｜                                          ⬆️-------- ⬅️ ---------------------- ⬅️ phase 1:
 find the checkpoint entry
 *                                                  ｜      ｜                      ⬇️
 *                                                  ｜------｜------- ⬅️ -----------｜     phase 2: find the first entry which commit_ts low equal (<=)
 the max_commit_ts
 *                                                          ｜
 *                                                          ➡️ ------ |       ...  (jump checkpoint entry) ...       | continue => end
 *
 *                                                                    phase 3: replay the entries by the order of the wal.log
 *                                                                    - case 1: the entry is a checkpoint entry, jump it.
 *                                                                    - case 2: the entry is a normal entry, replay it.
 *                                                                    - case 3: the entry is a bad entry, stop replaying and end.



 *  phase 1(⬅️): - find the checkpoint entry in the wal.log file. ( Attention: From back to front find the first checkpoint entry)
 *           - get the max commit timestamp of the checkpoint entry.
 *           - get the catalog path of the checkpoint entry.
 *
 *  phase 2(⬅️): - find the first entry which commit_ts low equal (<=) the max_commit_ts
 *  phase 3(➡️): - replay the entries by the order of the wal.log
 * @return int64_t The max commit timestamp of the transactions in the wal file.
 *
 */

Tuple<TransactionID, TxnTimeStamp, TxnTimeStamp> WalManager::GetReplayEntries(StorageMode targe_storage_mode,
                                                                              Vector<SharedPtr<WalEntry>> &replay_entries) {
    Vector<String> wal_list{};
    {
        auto [temp_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir_);
        if (wal_infos.size() > 1) {
            std::sort(wal_infos.begin(), wal_infos.end(), [](const WalFileInfo &a, const WalFileInfo &b) {
                return a.max_commit_ts_ > b.max_commit_ts_;
            });
        }
        if (temp_wal_info.has_value()) {
            wal_list.push_back(temp_wal_info->path_);
            LOG_INFO(fmt::format("Find temp wal file: {}", temp_wal_info->path_));
        }
        for (const auto &wal_info : wal_infos) {
            wal_list.push_back(wal_info.path_);
            LOG_INFO(fmt::format("Find wal file: {}", wal_info.path_));
        }
        // e.g. wal_list = {wal.log , wal.log.100 , wal.log.50}
    }
    if (wal_list.empty()) {
        LOG_INFO(fmt::format("No checkpoint found, terminate replaying WAL"));
        return {0, 0, 0};
    }

    LOG_INFO("Start Wal Replay");
    // log the wal files.

    TransactionID max_transaction_id = 0;
    TxnTimeStamp max_checkpoint_ts = 0; // the max commit ts that has been checkpointed
    replay_entries.clear();
    TxnTimeStamp last_commit_ts = 0; // last wal commit ts

    { // if no checkpoint, max_checkpoint_ts is 0
        WalListIterator iterator(wal_list);
        // phase 1: find the max commit ts and catalog path
        LOG_INFO("Replay phase 1: find the max commit ts and catalog path");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }
            LOG_TRACE(wal_entry->ToString());

            WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_checkpoint_ts = checkpoint_cmd->max_commit_ts_;
                last_commit_ts = wal_entry->commit_ts_;
                max_transaction_id = wal_entry->txn_id_;
                break;
            }
            replay_entries.push_back(wal_entry);
        }
        LOG_INFO(fmt::format("Find and set checkpoint max commit ts: {}", max_checkpoint_ts));
        SetLastCheckpointTS(max_checkpoint_ts);

        // phase 2: by the max commit ts, find the entries to replay
        LOG_INFO("Replay phase 2: by the max commit ts, find the entries to replay");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
                // TODO: clean the wal entries, disk break will reach this place.
                // replay_entries.clear();
                break;
            }
            // LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ > max_checkpoint_ts) {
                LOG_TRACE(fmt::format("Found WAL entry, commit ts: {} > max checkpoint ts: {}", wal_entry->commit_ts_, max_checkpoint_ts));
                LOG_TRACE(wal_entry->ToString());
                replay_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }
    if (last_commit_ts == 0) {
        switch (targe_storage_mode) {
            case StorageMode::kWritable: {
                // once wal is not empty, a checkpoint should always be found in leader or standalone mode.
                String error_message = "WAL replay: No checkpoint found in wal";
                UnrecoverableError(error_message);
                break;
            }
            case StorageMode::kReadable: {
                return {0, 0, 0};
            }
            default: {
                String error_message = "Unreachable branch";
                UnrecoverableError(error_message);
            }
        }
    }
    LOG_INFO(fmt::format("Checkpoint found, replay the catalog, wal entry size: {}", replay_entries.size()));
    std::reverse(replay_entries.begin(), replay_entries.end());

    return {max_transaction_id, last_commit_ts, max_checkpoint_ts};
}

Tuple<TransactionID, TxnTimeStamp> WalManager::ReplayWalEntries(const Vector<SharedPtr<WalEntry>> &replay_entries) {
    // phase 3: replay the entries

    LOG_INFO(fmt::format("Replay phase 3: replay {} entries", replay_entries.size()));
    TransactionID last_txn_id = 0;
    TxnTimeStamp last_commit_ts = 0;

    NewTxnManager *txn_mgr = storage_->new_txn_manager();

    for (SizeT replay_count = 0; replay_count < replay_entries.size(); ++replay_count) {
        // if (replay_entries[replay_count]->commit_ts_ < max_checkpoint_ts) {
        //     String error_message = fmt::format("Wal Replay: Commit ts should be greater than max commit ts, commit_ts: {}, max_commit: {}",
        //                                        replay_entries[replay_count]->commit_ts_,
        //                                        max_checkpoint_ts);
        //     UnrecoverableError(error_message);
        // }
        last_commit_ts = replay_entries[replay_count]->commit_ts_;
        last_txn_id = replay_entries[replay_count]->txn_id_;

        const SharedPtr<WalEntry> &replay_entry = replay_entries[replay_count];
        LOG_DEBUG(replay_entry->ToString());
        // ReplayWalOptions options{.on_startup_ = false, .is_replay_ = true, .sync_from_leader_ = false};

        UniquePtr<NewTxn> replay_txn = txn_mgr->BeginReplayTxn(replay_entry);
        for (const auto &cmd : replay_entry->cmds_) {
            LOG_TRACE(fmt::format("Replay wal cmd: {}, commit ts: {}", cmd->ToString(), replay_entry->commit_ts_));

            Status status = replay_txn->ReplayWalCmd(cmd);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Fail to replay wal entry: {}", status.message()));
            }
        }

        txn_mgr->CommitReplayTxn(replay_txn.get());
    }

    LOG_INFO(fmt::format("Latest txn commit_ts: {}, latest txn id: {}", last_commit_ts, last_txn_id));
    // storage_->catalog()->next_txn_id_ = last_txn_id;
    // UpdateCommitState(last_commit_ts, 0);

    // TxnTimeStamp system_start_ts = last_commit_ts + 1;
    // LOG_INFO(fmt::format("System start ts: {}", system_start_ts));
    return {last_txn_id, last_commit_ts};
}

Vector<SharedPtr<WalEntry>> WalManager::CollectWalEntries() const {
    Vector<SharedPtr<WalEntry>> wal_entries;

    Vector<String> wal_list{};
    {
        auto [active_wal_info, wal_infos] = WalFile::ParseWalFilenames(wal_dir_);
        if (!wal_infos.empty()) {
            std::sort(wal_infos.begin(), wal_infos.end(), [](const WalFileInfo &a, const WalFileInfo &b) {
                return a.max_commit_ts_ > b.max_commit_ts_;
            });
        }
        if (active_wal_info.has_value()) {
            wal_list.push_back(active_wal_info->path_);
            LOG_TRACE(fmt::format("Find active WAL file: {}", active_wal_info->path_));
        }
        wal_list.reserve(wal_infos.size());
        for (const auto &wal_info : wal_infos) {
            wal_list.push_back(wal_info.path_);
            LOG_TRACE(fmt::format("Find WAL file: {}", wal_info.path_));
        }
        // e.g. wal_list = {wal.log , wal.log.100 , wal.log.50}
    }

    if (wal_list.empty()) {
        String error_message = "No WAL file found";
        UnrecoverableError(error_message);
    }

    TxnTimeStamp max_checkpoint_ts = 0;
    TxnTimeStamp system_start_ts = 0;

    {
        // if no checkpoint, max_checkpoint_ts is 0
        WalListIterator iterator(wal_list);
        // phase 1: find the max commit ts and catalog path
        LOG_TRACE("Check WAL: to find the max commit ts");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }

            LOG_TRACE(wal_entry->ToString());

            WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
            wal_entries.push_back(wal_entry);
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_checkpoint_ts = checkpoint_cmd->max_commit_ts_;
                system_start_ts = wal_entry->commit_ts_;
                break;
            }
        }

        if (system_start_ts == 0) {
            // once wal is not empty, a checkpoint should always be found.
            String error_message = "No checkpoint found in WAL";
            UnrecoverableError(error_message);
        }

        LOG_TRACE(fmt::format("Find checkpoint max commit ts: {}", max_checkpoint_ts));

        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }

            LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ > max_checkpoint_ts) {
                wal_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }

    std::reverse(wal_entries.begin(), wal_entries.end());
    return wal_entries;
}

} // namespace infinity

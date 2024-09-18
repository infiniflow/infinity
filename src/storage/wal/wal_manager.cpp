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
import txn;
import storage;
import local_file_system;
import third_party;
import catalog;
import table_entry_type;
import infinity_context;
import txn_store;
import data_access_state;
import status;
import bg_task;
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
import constant_expr;
import bind_context;
import value_expression;
import expression_binder;
import value;

module wal_manager;

namespace infinity {

WalManager::WalManager(Storage *storage,
                       String wal_dir,
                       u64 wal_size_threshold,
                       u64 delta_checkpoint_interval_wal_bytes,
                       FlushOptionType flush_option)
    : cfg_wal_size_threshold_(wal_size_threshold), cfg_delta_checkpoint_interval_wal_bytes_(delta_checkpoint_interval_wal_bytes), wal_dir_(wal_dir),
      wal_path_(wal_dir + "/" + WalFile::TempWalFilename()), storage_(storage), running_(false), flush_option_(flush_option), last_ckp_wal_size_(0),
      checkpoint_in_progress_(false), last_ckp_ts_(UNCOMMIT_TS), last_full_ckp_ts_(UNCOMMIT_TS) {}

WalManager::WalManager(Storage *storage,
                       String wal_dir,
                       String data_dir,
                       u64 wal_size_threshold,
                       u64 delta_checkpoint_interval_wal_bytes,
                       FlushOptionType flush_option)
    : cfg_wal_size_threshold_(wal_size_threshold), cfg_delta_checkpoint_interval_wal_bytes_(delta_checkpoint_interval_wal_bytes), wal_dir_(wal_dir),
      wal_path_(wal_dir + "/" + WalFile::TempWalFilename()), data_path_(data_dir), storage_(storage), running_(false), flush_option_(flush_option),
      last_ckp_wal_size_(0), checkpoint_in_progress_(false), last_ckp_ts_(UNCOMMIT_TS), last_full_ckp_ts_(UNCOMMIT_TS) {}

WalManager::~WalManager() {
    if (running_.load()) {
        Stop();
    }
}

void WalManager::Start() {
    LOG_INFO("WAL manager is starting...");
    bool expected = false;
    bool changed = running_.compare_exchange_strong(expected, true);
    if (!changed)
        return;
    LocalFileSystem fs;
    if (!fs.Exists(wal_dir_)) {
        fs.CreateDirectory(wal_dir_);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        String error_message = fmt::format("Failed to open wal file: {}", wal_path_);
        UnrecoverableError(error_message);
    }
    LOG_INFO(fmt::format("Open wal file: {}", wal_path_));

    wal_size_ = 0;
    flush_thread_ = Thread([this] { Flush(); });
    // checkpoint_thread_ = Thread([this] { CheckpointTimer(); });
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

    LOG_TRACE("WalManager::Stop begin to stop txn manager.");
    // Notify txn manager to stop.
    TxnManager *txn_mgr = storage_->txn_manager();
    txn_mgr->Stop();

    // pop all the entries in the queue. and notify the condition variable.
    wait_flush_.Enqueue(nullptr);

    // Wait for flush thread to stop
    LOG_TRACE("WalManager::Stop flush thread join");
    flush_thread_.join();

    ofs_.close();
    LOG_INFO("WAL manager is stopped.");
}

// Session request to persist an entry. Assuming txn_id of the entry has
// been initialized.
void WalManager::PutEntries(Vector<WalEntry *> wal_entries) {
    if (!running_.load()) {
        return;
    }
    wait_flush_.EnqueueBulk(wal_entries);
}

TxnTimeStamp WalManager::GetCheckpointedTS() { return last_ckp_ts_ == UNCOMMIT_TS ? 0 : last_ckp_ts_; }

Vector<SharedPtr<String>> WalManager::GetDiffWalEntryString(TxnTimeStamp start_timestamp) const {

    LocalFileSystem fs;
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

    Vector<SharedPtr<WalEntry>> log_entries;

    TxnTimeStamp max_commit_ts = 0; // the max commit ts that has be checkpointed
    String catalog_dir = "";

    {
        // if no checkpoint, max_commit_ts is 0
        WalListIterator iterator(wal_list);
        // phase 1: find the max commit ts and catalog path
        LOG_INFO("Replay phase 1: find the max commit ts and catalog path");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }

            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                std::string catalog_path = fmt::format("{}/{}", data_path_, "catalog");
                catalog_dir = Path(fmt::format("{}/{}", catalog_path, checkpoint_cmd->catalog_name_)).parent_path().string();
                break;
            }
            log_entries.push_back(wal_entry);
        }
        LOG_INFO(fmt::format("Find checkpoint max commit ts: {}", max_commit_ts));

        // phase 2: by the max commit ts, find the entries to replay
        LOG_INFO("Get all wal entries after CKP and start timestamp");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
                // TODO: clean the wal entries, disk break will reach this place.
                // replay_entries.clear();
                break;
            }
            LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ >= max_commit_ts and wal_entry->commit_ts_ > start_timestamp) {
                log_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }


    log_strings.reserve(log_entries.size());

    for(const auto& log_entry: log_entries) {

        i32 exp_size = log_entry->GetSizeInBytes();

        // Serialize the log entry
        SharedPtr<String> buf_ptr;
        buf_ptr->resize(exp_size);
        char *ptr = buf_ptr->data();
        log_entry->WriteAdv(ptr);
        i32 act_size = ptr - buf_ptr->data();
        if (exp_size != act_size) {
            String error_message = fmt::format("WalManager::Flush WalEntry estimated size {} differ with the actual one {}, entry {}", exp_size, act_size, log_entry->ToString());
            UnrecoverableError(error_message);
        }

        log_strings.emplace_back(buf_ptr);
    }

    return log_strings;
}

// Flush is scheduled regularly. It collects a batch of transactions, sync
// wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
// ~10s. So it's necessary to sync for a batch of transactions, and to
// checkpoint for a batch of sync.
void WalManager::Flush() {
    LOG_TRACE("WalManager::Flush log mainloop begin");

    Deque<WalEntry *> log_batch{};
    TxnManager *txn_mgr = storage_->txn_manager();
    while (running_.load()) {
        wait_flush_.DequeueBulk(log_batch);
        if (log_batch.empty()) {
            LOG_WARN("WalManager::Dequeue empty batch logs");
            continue;
        }

        for (const auto &entry : log_batch) {
            // Empty WalEntry (read-only transactions) shouldn't go into WalManager.
            if (entry == nullptr) {
                // terminate entry
                running_ = false;
                break;
            }

            if (entry->cmds_.empty()) {
                continue;
                // UnrecoverableError(fmt::format("WalEntry of txn_id {} commands is empty", entry->txn_id_));
            }
            if (txn_mgr->InCheckpointProcess(entry->commit_ts_)) {
                this->SwapWalFile(max_commit_ts_);
            }

            for (const SharedPtr<WalCmd> &cmd : entry->cmds_) {
                LOG_TRACE(fmt::format("WAL CMD: {}", cmd->ToString()));
            }

            i32 exp_size = entry->GetSizeInBytes();
            Vector<char> buf(exp_size);
            char *ptr = buf.data();
            entry->WriteAdv(ptr);
            i32 act_size = ptr - buf.data();
            if (exp_size != act_size) {
                String error_message = fmt::format("WalManager::Flush WalEntry estimated size {} differ with the actual one {}, entry {}",
                                                   exp_size,
                                                   act_size,
                                                   entry->ToString());
                UnrecoverableError(error_message);
            }
            ofs_.write(buf.data(), ptr - buf.data());
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

        for (const auto &entry : log_batch) {
            Txn *txn = txn_mgr->GetTxn(entry->txn_id_);
            if (txn != nullptr) {
                txn->CommitBottom();
            }
        }
        log_batch.clear();

        // Check if the wal file is too large, swap to a new one.
        try {
            LocalFileSystem fs;
            auto file_size = fs.GetFileSizeByPath(wal_path_);
            if (file_size > cfg_wal_size_threshold_) {
                this->SwapWalFile(max_commit_ts_);
            }
        } catch (RecoverableException &e) {
            LOG_ERROR(e.what());
        } catch (UnrecoverableException &e) {
            LOG_CRITICAL(e.what());
            throw e;
        }

        // Check if total wal is too large, do delta checkpoint
        i64 last_ckp_wal_size = GetLastCkpWalSize();
        if (wal_size_ - last_ckp_wal_size > i64(cfg_delta_checkpoint_interval_wal_bytes_)) {
            LOG_TRACE("Reach the WAL limit trigger the DELTA checkpoint");
            auto checkpoint_task = MakeShared<CheckpointTask>(false /*delta checkpoint*/);
            if (!this->TrySubmitCheckpointTask(std::move(checkpoint_task))) {
                LOG_TRACE("Skip delta checkpoint(size) because there is already a checkpoint task running.");
            }
        }
        LOG_TRACE("WAL flush is finished.");
    }
    LOG_TRACE("WalManager::Flush mainloop end");
}

bool WalManager::TrySubmitCheckpointTask(SharedPtr<CheckpointTaskBase> ckp_task) {
    bool expect = false;
    if (checkpoint_in_progress_.compare_exchange_strong(expect, true)) {
        storage_->bg_processor()->Submit(ckp_task);
        return true;
    }
    return false;
}

/*****************************************************************************
 * CHECKPOINT WAL FILE
 *****************************************************************************/

// Do checkpoint for transactions which lsn no larger than the given one.
void WalManager::Checkpoint(bool is_full_checkpoint) {
    TxnManager *txn_mgr = storage_->txn_manager();
    Txn *txn = txn_mgr->BeginTxn(MakeUnique<String>("Full or delta checkpoint"), true /*is_checkpoint*/);

    if (is_full_checkpoint) {
        FullCheckpointInner(txn);
    } else {
        DeltaCheckpointInner(txn);
    }

    txn_mgr->CommitTxn(txn);
}

void WalManager::Checkpoint(ForceCheckpointTask *ckp_task) {
    bool is_full_checkpoint = ckp_task->is_full_checkpoint_;

    if (is_full_checkpoint) {
        FullCheckpointInner(ckp_task->txn_);
    } else {
        DeltaCheckpointInner(ckp_task->txn_);
    }
}

void WalManager::FullCheckpointInner(Txn *txn) {
    DeferFn defer([&] { checkpoint_in_progress_.store(false); });

    TxnTimeStamp last_ckp_ts = last_ckp_ts_;
    TxnTimeStamp last_full_ckp_ts = last_full_ckp_ts_;
    auto [max_commit_ts, wal_size] = GetCommitState();
    max_commit_ts = std::min(max_commit_ts, txn->BeginTS()); // txn commit after txn->BeginTS() should be ignored
    // wal_size may be larger than the actual size. but it's ok. it only makes the swap of wal file a little bit later.

    if (max_commit_ts == last_full_ckp_ts) {
        LOG_TRACE(fmt::format("Skip full checkpoint because the max_commit_ts {} is the same as the last full checkpoint", max_commit_ts));
        return;
    }
    if ((last_full_ckp_ts != UNCOMMIT_TS && last_full_ckp_ts >= max_commit_ts) || (last_ckp_ts != UNCOMMIT_TS && last_ckp_ts > max_commit_ts)) {
        String error_msg = fmt::format("last_full_ckp_ts {} >= max_commit_ts {} or last_ckp_ts {} > max_commit_ts {}",
                                       last_full_ckp_ts,
                                       max_commit_ts,
                                       last_ckp_ts,
                                       max_commit_ts);
        UnrecoverableError(error_msg);
    }

    try {
        LOG_DEBUG(fmt::format("Full Checkpoint Txn txn_id: {}, begin_ts: {}, max_commit_ts {}", txn->TxnID(), txn->BeginTS(), max_commit_ts));
        txn->FullCheckpoint(max_commit_ts);
        SetLastCkpWalSize(wal_size);

        LOG_DEBUG(fmt::format("Full Checkpoint is done for commit_ts <= {}", max_commit_ts));
    } catch (RecoverableException &e) {
        LOG_ERROR(fmt::format("WalManager::Checkpoint failed: {}", e.what()));
    } catch (UnrecoverableException &e) {
        LOG_CRITICAL(fmt::format("WalManager::Checkpoint failed: {}", e.what()));
        throw e;
    }
    last_ckp_ts_ = max_commit_ts;
    WalFile::RecycleWalFile(max_commit_ts, wal_dir_);
    last_full_ckp_ts_ = max_commit_ts;
    const auto &catalog_dir = *storage_->catalog()->CatalogDir();
    CatalogFile::RecycleCatalogFile(max_commit_ts, catalog_dir);
}

void WalManager::DeltaCheckpointInner(Txn *txn) {
    DeferFn defer([&] { checkpoint_in_progress_.store(false); });

    TxnTimeStamp last_ckp_ts = last_ckp_ts_;
    auto [max_commit_ts, wal_size] = GetCommitState();
    max_commit_ts = std::min(max_commit_ts, txn->BeginTS()); // txn commit after txn->BeginTS() should be ignored
    // wal_size may be larger than the actual size. but it's ok. it only makes the swap of wal file a little bit later.

    if (max_commit_ts == last_ckp_ts) {
        LOG_TRACE(fmt::format("Skip delta checkpoint because the max_commit_ts {} is the same as the last checkpoint", max_commit_ts));
        return;
    }
    if (last_ckp_ts != UNCOMMIT_TS && last_ckp_ts >= max_commit_ts) {
        String error_message = fmt::format("last_ckp_ts {} >= max_commit_ts {}", last_ckp_ts, max_commit_ts);
        UnrecoverableError(error_message);
    }

    try {
        LOG_DEBUG(fmt::format("Delta Checkpoint Txn txn_id: {}, begin_ts: {}, max_commit_ts {}", txn->TxnID(), txn->BeginTS(), max_commit_ts));
        TxnTimeStamp new_max_commit_ts = 0;
        if (!txn->DeltaCheckpoint(last_ckp_ts, new_max_commit_ts)) {
            return;
        }
        if (new_max_commit_ts == 0) {
            UnrecoverableError(fmt::format("new_max_commit_ts = 0"));
        }
        max_commit_ts = new_max_commit_ts;
        SetLastCkpWalSize(wal_size);

        LOG_DEBUG(fmt::format("Delta Checkpoint is done for commit_ts <= {}", max_commit_ts));
    } catch (RecoverableException &e) {
        LOG_ERROR(fmt::format("WalManager::Checkpoint failed: {}", e.what()));
    } catch (UnrecoverableException &e) {
        LOG_CRITICAL(fmt::format("WalManager::Checkpoint failed: {}", e.what()));
        throw e;
    }
    last_ckp_ts_ = max_commit_ts;
    WalFile::RecycleWalFile(max_commit_ts, wal_dir_);
}

void WalManager::UpdateCommitState(TxnTimeStamp commit_ts, i64 wal_size) {
    std::scoped_lock lock(mutex2_);
    if (commit_ts <= max_commit_ts_ || wal_size < wal_size_ /*equal when replay because both are 0*/) {
        String error_message = fmt::format("WalManager::UpdateCommitState commit_ts {} <= max_commit_ts_ {} or wal_size {} <= wal_size_ {}",
                                           commit_ts,
                                           max_commit_ts_,
                                           wal_size,
                                           wal_size_);
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
void WalManager::SwapWalFile(const TxnTimeStamp max_commit_ts) {
    if (ofs_.is_open()) {
        ofs_.close();
    }

    String new_file_path = fmt::format("{}/{}", wal_dir_, WalFile::WalFilename(max_commit_ts));
    LOG_INFO(fmt::format("Wal {} swap to new path: {}", wal_path_, new_file_path));

    // Rename the current wal file to a new one.
    LocalFileSystem fs;
    fs.Rename(wal_path_, new_file_path);

    // Create a new wal file with the original name.
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        String error_message = fmt::format("Failed to open wal file: {}", wal_path_);
        UnrecoverableError(error_message);
    }
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
i64 WalManager::ReplayWalFile(StorageMode targe_storage_mode) {
    LocalFileSystem fs;

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
        return 0;
    }

    LOG_INFO("Start Wal Replay");
    // log the wal files.

    TxnTimeStamp max_commit_ts = 0; // the max commit ts that has be checkpointed
    Vector<SharedPtr<WalEntry>> replay_entries;
    String catalog_dir = "";
    TxnTimeStamp last_commit_ts = 0; // last wal commit ts

    { // if no checkpoint, max_commit_ts is 0
        WalListIterator iterator(wal_list);
        // phase 1: find the max commit ts and catalog path
        LOG_INFO("Replay phase 1: find the max commit ts and catalog path");
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }
            // LOG_TRACE(wal_entry->ToString());

            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                std::string catalog_path = fmt::format("{}/{}", data_path_, "catalog");
                catalog_dir = Path(fmt::format("{}/{}", catalog_path, checkpoint_cmd->catalog_name_)).parent_path().string();
                last_commit_ts = wal_entry->commit_ts_;
                break;
            }
            replay_entries.push_back(wal_entry);
        }
        LOG_INFO(fmt::format("Find checkpoint max commit ts: {}", max_commit_ts));

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
            LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ > max_commit_ts) {
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
                String error_message = "No checkpoint found in wal";
                UnrecoverableError(error_message);
                break;
            }
            case StorageMode::kReadable: {
                return 0;
            }
            default: {
                String error_message = "Unreachable branch";
                UnrecoverableError(error_message);
            }
        }
    }
    LOG_INFO(fmt::format("Checkpoint found, replay the catalog"));
    auto catalog_fileinfo = CatalogFile::ParseValidCheckpointFilenames(catalog_dir, max_commit_ts);
    if (!catalog_fileinfo.has_value()) {
        String error_message = fmt::format("Wal Replay: Parse catalog file failed, catalog_dir: {}", catalog_dir);
        UnrecoverableError(error_message);
    }
    auto &[full_catalog_fileinfo, delta_catalog_fileinfo_array] = catalog_fileinfo.value();
    storage_->AttachCatalog(full_catalog_fileinfo, delta_catalog_fileinfo_array);

    // phase 3: replay the entries
    LOG_INFO(fmt::format("Replay phase 3: replay {} entries", replay_entries.size()));
    std::reverse(replay_entries.begin(), replay_entries.end());
    TransactionID last_txn_id = 0;

    for (SizeT replay_count = 0; replay_count < replay_entries.size(); ++replay_count) {
        if (replay_entries[replay_count]->commit_ts_ < max_commit_ts) {
            String error_message = fmt::format("Wal Replay: Commit ts should be greater than max commit ts, commit_ts: {}, max_commit: {}",
                                               replay_entries[replay_count]->commit_ts_,
                                               max_commit_ts);
            UnrecoverableError(error_message);
        }
        last_commit_ts = replay_entries[replay_count]->commit_ts_;
        last_txn_id = replay_entries[replay_count]->txn_id_;

        LOG_DEBUG(replay_entries[replay_count]->ToString());
        ReplayWalEntry(*replay_entries[replay_count]);
    }

    LOG_INFO(fmt::format("Latest txn commit_ts: {}, latest txn id: {}", last_commit_ts, last_txn_id));
    storage_->catalog()->next_txn_id_ = last_txn_id;
    UpdateCommitState(last_commit_ts, 0);

    TxnTimeStamp system_start_ts = last_commit_ts + 1;
    LOG_INFO(fmt::format("System start ts: {}", system_start_ts));

    return system_start_ts;
}

Optional<Pair<FullCatalogFileInfo, Vector<DeltaCatalogFileInfo>>> WalManager::GetCatalogFiles() const {
    LocalFileSystem fs;

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

    if (wal_list.empty()) {
        String error_message = "No WAL file found";
        UnrecoverableError(error_message);
        return None;
    }

    TxnTimeStamp max_commit_ts = 0;
    String catalog_dir = "";
    TxnTimeStamp system_start_ts = 0;
    {
        WalListIterator iterator(wal_list);
        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }
            LOG_TRACE(wal_entry->ToString());

            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                std::string catalog_path = fmt::format("{}/{}", data_path_, "catalog");
                catalog_dir = Path(fmt::format("{}/{}", catalog_path, checkpoint_cmd->catalog_name_)).parent_path().string();
                system_start_ts = wal_entry->commit_ts_;
                break;
            }
        }
        LOG_INFO(fmt::format("Find checkpoint max commit ts: {}", max_commit_ts));
    }

    if (system_start_ts == 0) {
        // once wal is not empty, a checkpoint should always be found.
        String error_message = "No checkpoint found in wal";
        UnrecoverableError(error_message);
    }
    LOG_INFO(fmt::format("Checkpoint found, replay the catalog"));
    return CatalogFile::ParseValidCheckpointFilenames(catalog_dir, max_commit_ts);
}

Vector<SharedPtr<WalEntry>> WalManager::CollectWalEntries() const {
    LocalFileSystem fs;
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

    TxnTimeStamp max_commit_ts = 0;
    TxnTimeStamp system_start_ts = 0;

    {
        // if no checkpoint, max_commit_ts is 0
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

            WalCmdCheckpoint *checkpoint_cmd = nullptr;
            wal_entries.push_back(wal_entry);
            if (wal_entry->IsCheckPoint(checkpoint_cmd)) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                system_start_ts = wal_entry->commit_ts_;
                break;
            }
        }

        if (system_start_ts == 0) {
            // once wal is not empty, a checkpoint should always be found.
            String error_message = "No checkpoint found in WAL";
            UnrecoverableError(error_message);
        }

        LOG_TRACE(fmt::format("Find checkpoint max commit ts: {}", max_commit_ts));

        while (iterator.HasNext()) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                String error_message = "Found unexpected bad wal entry";
                UnrecoverableError(error_message);
            }

            LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts_ > max_commit_ts) {
                wal_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }

    std::reverse(wal_entries.begin(), wal_entries.end());
    return wal_entries;
}

void WalManager::ReplayWalEntry(const WalEntry &entry) {
    for (const auto &cmd : entry.cmds_) {
        LOG_TRACE(fmt::format("Replay wal cmd: {}, commit ts: {}", WalCmd::WalCommandTypeToString(cmd->GetType()).c_str(), entry.commit_ts_));
        switch (cmd->GetType()) {
            case WalCommandType::CREATE_DATABASE: {
                WalCmdCreateDatabaseReplay(*dynamic_cast<const WalCmdCreateDatabase *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DROP_DATABASE: {
                WalCmdDropDatabaseReplay(*dynamic_cast<const WalCmdDropDatabase *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::CREATE_TABLE: {
                WalCmdCreateTableReplay(*dynamic_cast<const WalCmdCreateTable *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DROP_TABLE: {
                WalCmdDropTableReplay(*dynamic_cast<const WalCmdDropTable *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::ALTER_INFO: {
                Status status = Status::NotSupport("WalCmdAlterInfo Replay Not implemented");
                RecoverableError(status);
                break;
            }
            case WalCommandType::CREATE_INDEX: {
                WalCmdCreateIndexReplay(*dynamic_cast<const WalCmdCreateIndex *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DROP_INDEX: {
                WalCmdDropIndexReplay(*dynamic_cast<const WalCmdDropIndex *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::IMPORT: {
                WalCmdImportReplay(*dynamic_cast<const WalCmdImport *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::APPEND: {
                WalCmdAppendReplay(*dynamic_cast<const WalCmdAppend *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DELETE: {
                WalCmdDeleteReplay(*dynamic_cast<const WalCmdDelete *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            // case WalCommandType::SET_SEGMENT_STATUS_SEALED:
            //     WalCmdSetSegmentStatusSealedReplay(*dynamic_cast<const WalCmdSetSegmentStatusSealed *>(cmd.get()), entry.txn_id_,
            //     entry.commit_ts_); break;
            // case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA:
            //     WalCmdUpdateSegmentBloomFilterDataReplay(*dynamic_cast<const WalCmdUpdateSegmentBloomFilterData *>(cmd.get()),
            //                                              entry.txn_id_,
            //                                              entry.commit_ts_);
            //     break;
            case WalCommandType::CHECKPOINT: {
                break;
            }
            case WalCommandType::COMPACT: {
                WalCmdCompactReplay(*static_cast<const WalCmdCompact *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::OPTIMIZE: {
                auto *optimize_cmd = const_cast<WalCmdOptimize *>(static_cast<const WalCmdOptimize *>(cmd.get()));
                WalCmdOptimizeReplay(*optimize_cmd, entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DUMP_INDEX: {
                WalCmdDumpIndexReplay(*static_cast<WalCmdDumpIndex *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::RENAME_TABLE: {
                WalCmdRenameTableReplay(*static_cast<WalCmdRenameTable *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::ADD_COLUMNS: {
                WalCmdAddColumnsReplay(*static_cast<WalCmdAddColumns *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            case WalCommandType::DROP_COLUMNS: {
                WalCmdDropColumnsReplay(*static_cast<WalCmdDropColumns *>(cmd.get()), entry.txn_id_, entry.commit_ts_);
                break;
            }
            default: {
                String error_message = "WalManager::ReplayWalEntry unknown wal command type";
                UnrecoverableError(error_message);
            }
        }
    }
}

void WalManager::WalCmdCreateDatabaseReplay(const WalCmdCreateDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    Catalog *catalog = storage_->catalog();
    auto db_dir = MakeShared<String>(cmd.db_dir_tail_);
    catalog->CreateDatabaseReplay(
        MakeShared<String>(cmd.db_name_),
        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
            return DBEntry::ReplayDBEntry(db_meta, false, db_dir, db_name, txn_id, begin_ts, commit_ts);
        },
        txn_id,
        0 /*begin_ts*/);
}
void WalManager::WalCmdCreateTableReplay(const WalCmdCreateTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto *db_entry = storage_->catalog()->GetDatabaseReplay(cmd.db_name_, txn_id, 0 /*begin_ts*/);
    auto table_dir = MakeShared<String>(*db_entry->db_entry_dir() + "/" + cmd.table_dir_tail_);
    SharedPtr<String> table_name = cmd.table_def_->table_name();
    db_entry->CreateTableReplay(
        table_name,
        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
            ColumnID next_column_id = cmd.table_def_->columns().size();
            return TableEntry::ReplayTableEntry(false,
                                                table_meta,
                                                table_dir,
                                                table_name,
                                                cmd.table_def_->columns(),
                                                TableEntryType::kTableEntry,
                                                txn_id,
                                                begin_ts,
                                                commit_ts,
                                                0 /*row_count*/,
                                                INVALID_SEGMENT_ID /*unsealed_id*/,
                                                0 /*next_segment_id*/,
                                                next_column_id);
        },
        txn_id,
        0 /*begin_ts*/);
}

void WalManager::WalCmdDropDatabaseReplay(const WalCmdDropDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    SharedPtr<String> data_path_ptr = MakeShared<String>(data_path_);
    storage_->catalog()->DropDatabaseReplay(
        cmd.db_name_,
        [&](DBMeta *db_meta, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
            return DBEntry::ReplayDBEntry(db_meta, true, data_path_ptr, db_name, txn_id, begin_ts, commit_ts);
        },
        txn_id,
        0 /*begin_ts*/);
}

void WalManager::WalCmdDropTableReplay(const WalCmdDropTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto *db_entry = storage_->catalog()->GetDatabaseReplay(cmd.db_name_, txn_id, 0 /*begin_ts*/);
    db_entry->DropTableReplay(
        cmd.table_name_,
        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
            return TableEntry::ReplayTableEntry(true,
                                                table_meta,
                                                nullptr,
                                                table_name,
                                                Vector<SharedPtr<ColumnDef>>{},
                                                TableEntryType::kTableEntry,
                                                txn_id,
                                                begin_ts,
                                                commit_ts,
                                                0 /*row_count*/,
                                                INVALID_SEGMENT_ID /*unsealed_id*/,
                                                0 /*next_segment_id*/,
                                                0 /*next_column_id*/);
        },
        txn_id,
        0 /*begin_ts*/);
}

void WalManager::WalCmdCreateIndexReplay(const WalCmdCreateIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    TxnTimeStamp begin_ts = 0; // TODO: FIX IT

    Catalog *catalog = storage_->catalog();
    auto *db_entry = catalog->GetDatabaseReplay(cmd.db_name_, txn_id, begin_ts);
    auto *table_entry = db_entry->GetTableReplay(cmd.table_name_, txn_id, begin_ts);

    auto index_entry_dir = MakeShared<String>(*table_entry->TableEntryDir() + "/" + cmd.index_dir_tail_);
    auto *table_index_entry = table_entry->CreateIndexReplay(
        cmd.index_base_->index_name_,
        [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
            return TableIndexEntry::ReplayTableIndexEntry(index_meta, false, cmd.index_base_, index_entry_dir, txn_id, begin_ts, commit_ts);
        },
        txn_id,
        begin_ts);

    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);
    auto base_table_ref = MakeShared<BaseTableRef>(table_entry, table_entry->GetBlockIndex(fake_txn.get()));
    table_index_entry->CreateIndexPrepare(base_table_ref.get(), fake_txn.get(), false, true);

    auto *txn_store = fake_txn->GetTxnTableStore(table_entry);
    for (const auto &[index_name, txn_index_store] : txn_store->txn_indexes_store()) {
        Catalog::CommitCreateIndex(txn_index_store.get(), commit_ts, true /*is_replay*/);
    }
    table_index_entry->Commit(commit_ts);
}

void WalManager::WalCmdDropIndexReplay(const WalCmdDropIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    TxnTimeStamp begin_ts = 0; // TODO: FIX IT

    Catalog *catalog = storage_->catalog();
    auto *db_entry = catalog->GetDatabaseReplay(cmd.db_name_, txn_id, begin_ts);
    auto *table_entry = db_entry->GetTableReplay(cmd.table_name_, txn_id, begin_ts);
    table_entry->DropIndexReplay(
        cmd.index_name_,
        [&](TableIndexMeta *index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
            auto index_base = MakeShared<IndexBase>(index_meta->index_name());
            auto index_entry = TableIndexEntry::NewTableIndexEntry(index_base, true, nullptr, index_meta, txn_id, begin_ts);
            index_entry->commit_ts_.store(commit_ts);
            return index_entry;
        },
        txn_id,
        begin_ts);
}

// use by import and compact, add a new segment
SharedPtr<SegmentEntry>
WalManager::ReplaySegment(TableEntry *table_entry, const WalSegmentInfo &segment_info, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto *buffer_mgr = storage_->buffer_manager();
    auto segment_entry = SegmentEntry::NewReplaySegmentEntry(table_entry,
                                                             segment_info.segment_id_,
                                                             SegmentStatus::kSealed,
                                                             segment_info.column_count_,
                                                             segment_info.row_count_,
                                                             segment_info.actual_row_count_,
                                                             segment_info.row_capacity_,
                                                             commit_ts, /*min_row_ts*/
                                                             commit_ts, /*max_row_ts*/
                                                             commit_ts, /*commit_ts*/
                                                             UNCOMMIT_TS /*first_delete_ts*/,
                                                             UNCOMMIT_TS /*deprecate_ts*/,
                                                             0 /*begin_ts*/, // FIXME
                                                             txn_id);
    auto *pm = InfinityContext::instance().persistence_manager();
    for (BlockID block_id = 0; block_id < (BlockID)segment_info.block_infos_.size(); ++block_id) {
        auto &block_info = segment_info.block_infos_[block_id];
        block_info.addr_serializer_.AddToPersistenceManager(pm);
        auto block_entry = BlockEntry::NewReplayBlockEntry(segment_entry.get(),
                                                           block_id,
                                                           block_info.row_count_,
                                                           block_info.row_capacity_,
                                                           commit_ts,             /*min_row_ts*/
                                                           commit_ts,             /*max_row_ts*/
                                                           commit_ts,             /*commit_ts*/
                                                           commit_ts,             /*checkpoint_ts*/
                                                           block_info.row_count_, /*checkpoint_row_count*/
                                                           buffer_mgr,
                                                           txn_id);
        for (ColumnID column_id = 0; column_id < (ColumnID)block_info.outline_infos_.size(); ++column_id) {
            const auto [next_idx, last_off] = block_info.outline_infos_[column_id];
            auto column_entry = BlockColumnEntry::NewReplayBlockColumnEntry(block_entry.get(), column_id, buffer_mgr, next_idx, last_off, commit_ts);
            block_entry->AddColumnReplay(std::move(column_entry), column_id); // reuse function from delta catalog.
        }
        segment_entry->AddBlockReplay(std::move(block_entry)); // reuse function from delta catalog.
    }
    return segment_entry;
}

void WalManager::WalCmdImportReplay(const WalCmdImport &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    auto segment_entry = this->ReplaySegment(table_entry, cmd.segment_info_, txn_id, commit_ts);
    table_entry->AddSegmentReplayWalImport(segment_entry);
}

void WalManager::WalCmdDeleteReplay(const WalCmdDelete &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);
    auto table_store = fake_txn->GetTxnTableStore(table_entry);
    table_store->Delete(cmd.row_ids_);
    Catalog::Delete(table_store->GetTableEntry(), fake_txn->TxnID(), (void *)table_store, fake_txn->CommitTS(), table_store->GetDeleteStateRef());
    Catalog::CommitWrite(table_store->GetTableEntry(), fake_txn->TxnID(), commit_ts, table_store->txn_segments(), table_store->GetDeleteStatePtr());
}

void WalManager::WalCmdCompactReplay(const WalCmdCompact &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    for (const auto &new_segment_info : cmd.new_segment_infos_) {
        auto segment_entry = this->ReplaySegment(table_entry, new_segment_info, txn_id, commit_ts);
        table_entry->AddSegmentReplayWalCompact(segment_entry);
    }

    for (const SegmentID segment_id : cmd.deprecated_segment_ids_) {
        auto segment_entry = table_entry->GetSegmentByID(segment_id, commit_ts);
        if (!segment_entry->TrySetCompacting(nullptr)) { // fake set because check
            String error_message = fmt::format("Replaying segment: {} from table: {} with status: {}, can't be compacted",
                                               segment_id,
                                               cmd.table_name_,
                                               ToString(segment_entry->status()));
            UnrecoverableError(error_message);
        }
        if (!segment_entry->SetNoDelete()) {
            String error_message =
                fmt::format("Replaying segment: {} from table: {} can't be set no delete, can't be compacted", segment_id, cmd.table_name_);
            UnrecoverableError(error_message);
        }
        segment_entry->SetDeprecated(commit_ts);
    }
}

void WalManager::WalCmdOptimizeReplay(WalCmdOptimize &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_index_entry, status] = storage_->catalog()->GetIndexByName(cmd.db_name_, cmd.table_name_, cmd.index_name_, txn_id, commit_ts);
    if (!status.ok()) {
        String error_message = fmt::format("Wal Replay: Get index failed {}", status.message());
        UnrecoverableError(error_message);
    }
    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);

    TableEntry *table_entry = table_index_entry->table_index_meta()->table_entry();
    auto *txn_store = fake_txn->GetTxnTableStore(table_entry);
    table_index_entry->OptIndex(txn_store, std::move(cmd.params_), true /*replay*/);
}

void WalManager::WalCmdDumpIndexReplay(WalCmdDumpIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_index_entry, status] = storage_->catalog()->GetIndexByName(cmd.db_name_, cmd.table_name_, cmd.index_name_, txn_id, commit_ts);
    auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
    auto *buffer_mgr = storage_->buffer_manager();
    if (!status.ok()) {
        String error_message = fmt::format("Wal Replay: Get index failed {}", status.message());
        UnrecoverableError(error_message);
    }
    SegmentIndexEntry *segment_index_entry = nullptr;
    {
        auto &index_by_segment = table_index_entry->index_by_segment();

        if (auto iter = index_by_segment.find(cmd.segment_id_); iter != index_by_segment.end()) {
            segment_index_entry = iter->second.get();
        } else {
            auto create_index_param =
                SegmentIndexEntry::GetCreateIndexParam(table_index_entry->table_index_def(), 0 /*segment_offset*/, table_index_entry->column_def());
            auto segment_index_entry_ptr = SegmentIndexEntry::NewReplaySegmentIndexEntry(table_index_entry,
                                                                                         table_entry,
                                                                                         cmd.segment_id_,
                                                                                         buffer_mgr,
                                                                                         commit_ts /*min_ts*/,
                                                                                         commit_ts /*max_ts*/,
                                                                                         0 /*next_chunk_id*/,
                                                                                         txn_id /*txn_id*/,
                                                                                         commit_ts /*begin_ts*/,
                                                                                         commit_ts);
            index_by_segment.emplace(cmd.segment_id_, segment_index_entry_ptr);
            segment_index_entry = segment_index_entry_ptr.get();
        }
    }
    auto *pm = InfinityContext::instance().persistence_manager();
    for (const auto &chunk_info : cmd.chunk_infos_) {
        chunk_info.addr_serializer_.AddToPersistenceManager(pm);
        segment_index_entry->AddChunkIndexEntryReplayWal(chunk_info.chunk_id_,
                                                         table_entry,
                                                         chunk_info.base_name_,
                                                         chunk_info.base_rowid_,
                                                         chunk_info.row_count_,
                                                         commit_ts,
                                                         chunk_info.deprecate_ts_,
                                                         buffer_mgr);
    }
    for (ChunkID old_chunk_id : cmd.deprecate_ids_) {
        auto *old_chunk = segment_index_entry->GetChunkIndexEntry(old_chunk_id);
        if (old_chunk != nullptr) {
            old_chunk->DeprecateChunk(commit_ts);
        }
    }
}

void WalManager::WalCmdRenameTableReplay(WalCmdRenameTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    //
}

void WalManager::WalCmdAddColumnsReplay(WalCmdAddColumns &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [db_entry, db_status] = storage_->catalog()->GetDatabase(cmd.db_name_, txn_id, commit_ts);
    if (!db_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get database failed {}", db_status.message());
        UnrecoverableError(error_message);
    }
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    SharedPtr<TableEntry> new_table_entry = table_entry->Clone();
    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);
    auto *txn_table_store = fake_txn->GetTxnTableStore(table_entry);
    Vector<Value> default_values;
    ExpressionBinder tmp_binder(nullptr);
    for (const auto &column_def : cmd.column_defs_) {
        if (!column_def->has_default_value()) {
            UnrecoverableError(fmt::format("Wal Replay: Add column {} without default value", column_def->name()));
        }

        auto default_expr = column_def->default_value();
        auto expr = tmp_binder.BuildValueExpr(*default_expr, nullptr, 0, false);
        auto value_expr = std::dynamic_pointer_cast<ValueExpression>(expr);

        default_values.push_back(value_expr->GetValue());
    }
    new_table_entry->AddColumns(cmd.column_defs_, default_values, txn_table_store);
    new_table_entry->commit_ts_ = commit_ts;
    db_entry->CreateTableReplay(
        table_entry->GetTableName(),
        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) { return new_table_entry; },
        txn_id,
        commit_ts);
}

void WalManager::WalCmdDropColumnsReplay(WalCmdDropColumns &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [db_entry, db_status] = storage_->catalog()->GetDatabase(cmd.db_name_, txn_id, commit_ts);
    if (!db_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get database failed {}", db_status.message());
        UnrecoverableError(error_message);
    }
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    SharedPtr<TableEntry> new_table_entry = table_entry->Clone();
    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);
    auto *txn_table_store = fake_txn->GetTxnTableStore(table_entry);
    new_table_entry->DropColumns(cmd.column_names_, txn_table_store);
    new_table_entry->commit_ts_ = commit_ts;
    db_entry->CreateTableReplay(
        table_entry->GetTableName(),
        [&](TableMeta *table_meta, const SharedPtr<String> &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) { return new_table_entry; },
        txn_id,
        commit_ts);
}

void WalManager::WalCmdAppendReplay(const WalCmdAppend &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
    if (!table_status.ok()) {
        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
        UnrecoverableError(error_message);
    }

    auto fake_txn = Txn::NewReplayTxn(storage_->buffer_manager(), storage_->txn_manager(), storage_->catalog(), txn_id, commit_ts);
    auto table_store = fake_txn->GetTxnTableStore(table_entry);
    table_store->Append(cmd.block_);

    auto append_state = MakeUnique<AppendState>(table_store->GetBlocks());
    table_store->SetAppendState(std::move(append_state));

    Catalog::Append(table_store->GetTableEntry(), fake_txn->TxnID(), table_store, commit_ts, storage_->buffer_manager(), true);
    Catalog::CommitWrite(table_store->GetTableEntry(), fake_txn->TxnID(), commit_ts, table_store->txn_segments(), nullptr);
}

// // TMP deprecated
// void WalManager::WalCmdSetSegmentStatusSealedReplay(const WalCmdSetSegmentStatusSealed &cmd, TransactionID txn_id, TxnTimeStamp commit_ts) {
//     auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
//     if (!table_status.ok()) {
//        String error_message = fmt::format("Wal Replay: Get table failed {}", table_status.message());
//        LOG_CRITICAL(error_message);
//        UnrecoverableError(error_message);
//     }
//     auto segment_entry = table_entry->GetSegmentByID(cmd.segment_id_, commit_ts);
//     if (!segment_entry) {
//         UnrecoverableError("Wal Replay: Get segment failed");
//     }
//     segment_entry->SetSealed();
//     segment_entry->LoadFilterBinaryData(cmd.segment_filter_binary_data_, cmd.block_filter_binary_data_);
// }

// void WalManager::WalCmdUpdateSegmentBloomFilterDataReplay(const WalCmdUpdateSegmentBloomFilterData &cmd,
//                                                           TransactionID txn_id,
//                                                           TxnTimeStamp commit_ts) {
//     auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name_, cmd.table_name_, txn_id, commit_ts);
//     if (!table_status.ok()) {
//         UnrecoverableError(fmt::format("Wal Replay: Get table failed {}", table_status.message()));
//     }
//     auto segment_entry = table_entry->GetSegmentByID(cmd.segment_id_, commit_ts);
//     if (!segment_entry) {
//         UnrecoverableError("Wal Replay: Get segment failed");
//     }
//     segment_entry->LoadFilterBinaryData(cmd.segment_filter_binary_data_, cmd.block_filter_binary_data_);
// }

} // namespace infinity

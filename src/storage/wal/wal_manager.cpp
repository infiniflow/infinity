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

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <thread>

import stl;
import logger;
import third_party;
import txn_manager;
import txn;
import storage;
import wal_entry;
import infinity_exception;

import catalog;

import table_entry_type;
import parser;
import txn_store;

import data_access_state;
import status;
import bg_task;

import infinity_exception;
// #include "statement/extra/extra_ddl_info.h"

module wal_manager;

namespace infinity {

// WalCommandType -> String
String WalManager::WalCommandTypeToString(WalCommandType type) {
    switch (type) {
        case WalCommandType::INVALID:
            return "INVALID";
        case WalCommandType::CREATE_DATABASE:
            return "CREATE_DATABASE";
        case WalCommandType::DROP_DATABASE:
            return "DROP_DATABASE";
        case WalCommandType::CREATE_TABLE:
            return "CREATE_TABLE";
        case WalCommandType::DROP_TABLE:
            return "DROP_TABLE";
        case WalCommandType::ALTER_INFO:
            return "ALTER_INFO";
        case WalCommandType::IMPORT:
            return "IMPORT";
        case WalCommandType::APPEND:
            return "APPEND";
        case WalCommandType::DELETE:
            return "DELETE";
        case WalCommandType::CHECKPOINT:
            return "CHECKPOINT";
        case WalCommandType::CREATE_INDEX:
            return "CREATE_INDEX";
        case WalCommandType::DROP_INDEX:
            return "DROP_INDEX";
        default: {
            Error<StorageException>("Not supported wal command type");
        }
    }
}

// using namespace std;
// namespace std::filesystem = std::filesystem;

WalManager::WalManager(Storage *storage,
                       String wal_path,
                       u64 wal_size_threshold,
                       u64 full_checkpoint_interval_sec,
                       u64 delta_checkpoint_interval_sec,
                       u64 delta_checkpoint_interval_wal_bytes)
    : wal_size_threshold_(wal_size_threshold), full_checkpoint_interval_sec_(full_checkpoint_interval_sec),
      delta_checkpoint_interval_sec_(delta_checkpoint_interval_sec), delta_checkpoint_interval_wal_bytes_(delta_checkpoint_interval_wal_bytes),
      wal_path_(Move(wal_path)), storage_(storage), running_(false) {}

WalManager::~WalManager() {
    Stop();
    que_.clear();
    que2_.clear();
}

void WalManager::Start() {
    bool expected = false;
    bool changed = running_.compare_exchange_strong(expected, true);
    if (!changed)
        return;
    Path wal_dir = Path(wal_path_).parent_path();
    if (!std::filesystem::exists(wal_dir)) {
        std::filesystem::create_directory(wal_dir);
    }
    // TODO: recovery from wal checkpoint
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        Error<StorageException>(Format("Failed to open wal file: {}", wal_path_));
    }
    auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch());
    int64_t now = seconds_since_epoch.count();
    full_ckp_when_ = now;
    delta_ckp_when_ = now;
    max_commit_ts_ = 0;
    wal_size_ = 0;
    flush_thread_ = std::thread([this] { Flush(); });
    checkpoint_thread_ = std::thread([this] { CheckpointTimer(); });
}

void WalManager::Stop() {
    bool expected = true;
    bool changed = running_.compare_exchange_strong(expected, false);
    if (!changed) {
        LOG_INFO("WalManager::Stop already stopped");
        return;
    }

    LOG_INFO("WalManager::Stop begin to stop txn manager");
    // Notify txn manager to stop.
    TxnManager *txn_mgr = storage_->txn_manager();
    txn_mgr->Stop();

    // pop all the entries in the queue. and notify the condition variable.
    std::lock_guard guard(mutex_);
    for (const auto &entry : que_) {
        Txn *txn = txn_mgr->GetTxn(entry->txn_id);
        if (txn != nullptr) {
            txn->CancelCommitBottom();
        }
    }
    que_.clear();

    // Wait for checkpoint thread to stop.
    LOG_INFO("WalManager::Stop checkpoint thread join");
    checkpoint_thread_.join();

    // Wait for flush thread to stop
    LOG_INFO("WalManager::Stop flush thread join");
    flush_thread_.join();

    ofs_.close();

    LOG_INFO("WalManager is stopped");
}

// Session request to persist an entry. Assuming txn_id of the entry has
// been initialized.
int WalManager::PutEntry(SharedPtr<WalEntry> entry) {
    if (!running_.load()) {
        return -1;
    }
    int rc = 0;
    mutex_.lock();
    if (running_.load()) {
        que_.push_back(entry);
        rc = -1;
    }
    mutex_.unlock();
    return rc;
}

void WalManager::SetWalState(TxnTimeStamp max_commit_ts, int64_t wal_size) {
    mutex2_.lock();
    this->max_commit_ts_ = max_commit_ts;
    this->wal_size_ = wal_size;
    mutex2_.unlock();
}

Tuple<TxnTimeStamp, int64_t> WalManager::GetWalState() {
    int64_t wal_size{0};
    TxnTimeStamp max_commit_ts{};
    mutex2_.lock();
    max_commit_ts = this->max_commit_ts_;
    wal_size = this->wal_size_;
    mutex2_.unlock();
    return {max_commit_ts, wal_size};
}

// Flush is scheduled regularly. It collects a batch of transactions, sync
// wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
// ~10s. So it's necessary to sync for a batch of transactions, and to
// checkpoint for a batch of sync.
void WalManager::Flush() {
    LOG_TRACE("WalManager::Flush mainloop begin");
    while (running_.load()) {
        mutex_.lock();
        que_.swap(que2_);
        mutex_.unlock();
        if (que2_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        auto [max_commit_ts, wal_size] = GetWalState();
        for (const auto &entry : que2_) {
            // Empty WalEntry (read-only transactions) shouldn't go into WalManager.
            if (entry->cmds.empty()) {
                Error<StorageException>(Format("WalEntry of txn_id {} commands is empty", entry->txn_id));
            }
            int32_t exp_size = entry->GetSizeInBytes();
            Vector<char> buf(exp_size);
            char *ptr = buf.data();
            entry->WriteAdv(ptr);
            int32_t act_size = ptr - buf.data();
            if (exp_size != act_size)
                LOG_ERROR(Format("WalManager::Flush WalEntry estimated size {} differ "
                                 "with the actual one {}",
                                 exp_size,
                                 act_size));
            ofs_.write(buf.data(), ptr - buf.data());
            LOG_TRACE(Format("WalManager::Flush done writing wal for txn_id {}, commit_ts {}", entry->txn_id, entry->commit_ts));
            if (entry->cmds[0]->GetType() != WalCommandType::CHECKPOINT) {
                max_commit_ts = entry->commit_ts;
                wal_size += act_size;
            }
        }
        ofs_.flush();
        TxnManager *txn_mgr = storage_->txn_manager();
        for (const auto &entry : que2_) {
            // Commit sequentially so they get visible in the same order with wal.
            Txn *txn = txn_mgr->GetTxn(entry->txn_id);
            if (txn != nullptr) {
                txn->CommitBottom();
            }
        }
        que2_.clear();

        // Check if the wal file is too large.
        try {
            auto file_size = std::filesystem::file_size(wal_path_);
            if (file_size > wal_size_threshold_) {
                this->SwapWalFile(max_commit_ts);
            }
        } catch (Exception &e) {
            LOG_ERROR(e.what());
        }
        SetWalState(max_commit_ts, wal_size);
    }
    LOG_TRACE("WalManager::Flush mainloop end");
}

/*****************************************************************************
 * CHECKPOINT WAL FILE
 *****************************************************************************/

void WalManager::CheckpointTimer() {
    LOG_TRACE("WalManager::Checkpoint mainloop begin");
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        storage_->bg_processor()->Submit(MakeShared<TryCheckpointTask>(true));
    }
}

// Do checkpoint for transactions which lsn no larger than the given one.
void WalManager::Checkpoint() {

    auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch());
    i64 now = seconds_since_epoch.count();
    auto [max_commit_ts, wal_size] = GetWalState();
    bool is_full_checkpoint = false;
    //        bool is_delta_checkpoint = false;
    if (now - full_ckp_when_ > i64(full_checkpoint_interval_sec_) && wal_size != full_ckp_wal_size_) {
        is_full_checkpoint = true;
    } else if ((now - delta_ckp_when_ > i64(delta_checkpoint_interval_sec_) && wal_size != delta_ckp_wal_size_) ||
               wal_size - delta_ckp_wal_size_ > i64(delta_checkpoint_interval_wal_bytes_)) {
        //            is_delta_checkpoint = true;
    } else {
        return;
    }

    TxnManager *txn_mgr = nullptr;
    Txn *txn = nullptr;
    try {
        txn_mgr = storage_->txn_manager();
        txn = txn_mgr->CreateTxn();
        txn->Begin();
        LOG_INFO(Format("created txn for checkpoint, txn_id: {}, begin_ts: {}, max_commit_ts {}", txn->TxnID(), txn->BeginTS(), max_commit_ts));

        txn->Checkpoint(max_commit_ts, is_full_checkpoint);
        txn_mgr->CommitTxn(txn);

        delta_ckp_when_ = now;
        delta_ckp_wal_size_ = wal_size;
        if (is_full_checkpoint) {
            full_ckp_when_ = now;
            full_ckp_wal_size_ = wal_size;
            full_ckp_commit_ts_ = max_commit_ts;
            RecycleWalFile(full_ckp_commit_ts_);
        }
        LOG_INFO(Format("WalManager::Checkpoint {} done for commit_ts <= {}", is_full_checkpoint ? "full" : "delta", max_commit_ts));
    } catch (Exception &e) {
        LOG_ERROR(Format("WalManager::Checkpoint failed: {}", e.what()));
    }
}

void WalManager::Checkpoint(ForceCheckpointTask *ckp_task) {

    auto [max_commit_ts, wal_size] = GetWalState();
    if (wal_size == full_ckp_wal_size_) {
        return;
    }

    bool is_full_checkpoint = true;

    LOG_INFO(Format("Start to full checkpoint, txn_id: {}, begin_ts: {}, max_commit_ts {}",
                    ckp_task->txn_->TxnID(),
                    ckp_task->txn_->BeginTS(),
                    max_commit_ts));

    ckp_task->txn_->Checkpoint(max_commit_ts, is_full_checkpoint);

    // This function doesn't change the ckp related variable, which won't affect the following checkpoint correctness.

    LOG_INFO(Format("Full Checkpoint is done for commit_ts <= {}", max_commit_ts));
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

    Path old_file_path = Path(wal_path_);

    String new_file_path = old_file_path.string() + '.' + ToStr(max_commit_ts);
    LOG_INFO(Format("Wal Swap to new path: {}", new_file_path.c_str()));

    // Rename the current wal file to a new one.
    std::filesystem::rename(wal_path_, new_file_path);

    // Create a new wal file with the original name.
    ofs_ = std::ofstream(wal_path_, std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        Error<StorageException>(Format("Failed to open wal file: {}", wal_path_));
    }
}

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
i64 WalManager::ReplayWalFile() {
    if (!std::filesystem::exists(wal_path_) || std::filesystem::file_size(wal_path_) == 0) {
        storage_->InitNewCatalog();
        return 0;
    }

    for (const auto &entry : std::filesystem::directory_iterator(Path(wal_path_).parent_path())) {
        if (entry.is_regular_file()) {
            wal_list_.push_back(entry.path().string());
        }
    }

    // e.g. wal_list_ = {wal.log , wal.log.100 , wal.log.50}
    std::sort(wal_list_.begin(), wal_list_.end(), [](const std::string &a, const std::string &b) {
        auto get_lastNumber = [](const std::string &s) {
            auto pos = s.find_last_of('.');
            if (pos != std::string::npos) {
                return std::stol(s.substr(pos + 1));
            } else {
                throw std::invalid_argument("No '.' found");
            }
        };
        bool is_a_wal_log = (a.length() >= 7 && a.substr(a.length() - 7) == "wal.log");
        bool is_b_wal_log = (b.length() >= 7 && b.substr(b.length() - 7) == "wal.log");

        if (is_a_wal_log) {
            return true;
        } else if (is_b_wal_log) {
            return false;
        }

        SizeT num_a = get_lastNumber(a);
        SizeT num_b = get_lastNumber(b);

        return num_a > num_b;
    });

    LOG_INFO("Start Wal Replay");
    // log the wal files.
    for (const auto &wal_file : wal_list_) {
        LOG_TRACE(Format("List wal file: {}", wal_file.c_str()));
    }
    LOG_INFO(Format("List wal file size: {}", wal_list_.size()));

    i64 max_commit_ts = 0;
    String catalog_path;
    Vector<SharedPtr<WalEntry>> replay_entries;
    Vector<String> checkpoint_catalog_paths;

    {
        WalListIterator iterator(wal_list_);
        // phase 1: find the max commit ts and catalog path
        LOG_INFO("Replay phase 1: find the max commit ts and catalog path");
        while (true) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                break;
            }

            LOG_TRACE(wal_entry->ToString());

            replay_entries.push_back(wal_entry);

            if (wal_entry->IsCheckPoint()) {

                checkpoint_catalog_paths.push_back(wal_entry->GetCheckpointInfo().second);

                if (wal_entry->IsFullCheckPoint()) {
                    auto [current_max_commit_ts, current_catalog_path] = wal_entry->GetCheckpointInfo();
                    if (current_max_commit_ts > max_commit_ts) {
                        max_commit_ts = current_max_commit_ts;
                        catalog_path = current_catalog_path;
                    }
                    LOG_TRACE(Format("Find checkpoint max commit ts: {}", max_commit_ts));
                    LOG_TRACE(Format("Find catalog path: {}", catalog_path));
                    break;
                } else {
                    // delta checkpoint
                    auto [current_max_commit_ts, current_catalog_path] = wal_entry->GetCheckpointInfo();
                    // if the current max commit ts is greater than the max commit ts, update the max commit ts and catalog path
                    if (current_max_commit_ts > max_commit_ts) {
                        max_commit_ts = current_max_commit_ts;
                        catalog_path = current_catalog_path;
                    }
                }
            }
        }
        LOG_INFO(Format("Find checkpoint max commit ts: {}", max_commit_ts));

        // phase 2: by the max commit ts, find the entries to replay
        LOG_INFO("Replay phase 2: by the max commit ts, find the entries to replay");
        while (true) {
            auto wal_entry = iterator.Next();
            if (wal_entry.get() == nullptr) {
                break;
            }

            LOG_TRACE(wal_entry->ToString());

            if (wal_entry->commit_ts > max_commit_ts) {
                replay_entries.push_back(wal_entry);
            } else {
                break;
            }
        }
    }

    // Note: Init a new catalog when not found any checkpoint wal entry
    // Indicates that the system has not done checkpoint in the previous.
    if (checkpoint_catalog_paths.empty()) {
        storage_->InitNewCatalog();
    } else {
        std::reverse(checkpoint_catalog_paths.begin(), checkpoint_catalog_paths.end());
        storage_->AttachCatalog(checkpoint_catalog_paths);
    }

    // phase 3: replay the entries
    LOG_INFO("Replay phase 3: replay the entries");
    std::reverse(replay_entries.begin(), replay_entries.end());
    i64 system_start_ts = 0;
    i64 last_txn_id = 0;
    SizeT replay_count = 0;
    for (; replay_count < replay_entries.size(); ++replay_count) {
        if (replay_entries[replay_count]->commit_ts > max_commit_ts) {
            break;
        }
    }

    for (; replay_count < replay_entries.size(); ++replay_count) {
        if (replay_entries[replay_count]->commit_ts <= max_commit_ts) {
            Error<StorageException>("Wal Replay: Commit ts should be greater than max commit ts");
        }
        system_start_ts = replay_entries[replay_count]->commit_ts;
        last_txn_id = replay_entries[replay_count]->txn_id;
        if (replay_entries[replay_count]->IsCheckPoint()) {
            LOG_TRACE(Format("Replay Skip checkpoint entry: {}", replay_entries[replay_count]->ToString()));
            continue;
        }
        ReplayWalEntry(*replay_entries[replay_count]);
        LOG_TRACE(replay_entries[replay_count]->ToString());
    }

    LOG_TRACE(Format("System start ts: {}, lastest txn id: {}", system_start_ts, last_txn_id));
    storage_->catalog()->next_txn_id_ = last_txn_id;
    return system_start_ts;
}
/*****************************************************************************
 * GC WAL FILE
 *****************************************************************************/

/**
 * @brief Gc the old wal files.
 * Only delete the wal.log.* files. the wal.log file is current wal file.
 * Check if the wal.log.* files are too old.
 * if * is little than the max_commit_ts, we will delete it.
 */
void WalManager::RecycleWalFile(TxnTimeStamp full_ckp_ts) {
    // Gc old wal files.
    LOG_INFO("WalManager::Checkpoint begin to gc wal files");
    if (std::filesystem::exists(wal_path_)) {
        for (const auto &entry : std::filesystem::directory_iterator(Path(wal_path_).parent_path())) {
            if (entry.is_regular_file() && entry.path().string().find("wal.log.") != std::string::npos) {
                auto suffix = entry.path().string().substr(entry.path().string().find_last_of('.') + 1);
                if (std::stoll(suffix) < i64(full_ckp_ts)) {
                    std::filesystem::remove(entry.path());
                    LOG_TRACE(Format("WalManager::Checkpoint delete wal file: {}", entry.path().string().c_str()));
                }
            }
        }
    }
    LOG_INFO("WalManager::Checkpoint end to gc wal files");
}
void WalManager::ReplayWalEntry(const WalEntry &entry) {
    for (const auto &cmd : entry.cmds) {
        LOG_TRACE(Format("Replay wal cmd: {}, commit ts: {}", WalManager::WalCommandTypeToString(cmd->GetType()).c_str(), entry.commit_ts));
        switch (cmd->GetType()) {
            case WalCommandType::CREATE_DATABASE:
                WalCmdCreateDatabaseReplay(*dynamic_cast<const WalCmdCreateDatabase *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::DROP_DATABASE:
                WalCmdDropDatabaseReplay(*dynamic_cast<const WalCmdDropDatabase *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::CREATE_TABLE:
                WalCmdCreateTableReplay(*dynamic_cast<const WalCmdCreateTable *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::DROP_TABLE:
                WalCmdDropTableReplay(*dynamic_cast<const WalCmdDropTable *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::ALTER_INFO:
                Error<NotImplementException>("WalCmdAlterInfo Replay Not implemented");
                break;
            case WalCommandType::CREATE_INDEX:
                WalCmdCreateIndexReplay(*dynamic_cast<const WalCmdCreateIndex *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::DROP_INDEX:
                WalCmdDropIndexReplay(*dynamic_cast<const WalCmdDropIndex *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::IMPORT:
                WalCmdImportReplay(*dynamic_cast<const WalCmdImport *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::APPEND:
                WalCmdAppendReplay(*dynamic_cast<const WalCmdAppend *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::DELETE:
                WalCmdDeleteReplay(*dynamic_cast<const WalCmdDelete *>(cmd.get()), entry.txn_id, entry.commit_ts);
                break;
            case WalCommandType::CHECKPOINT:
                break;
            default:
                Error<StorageException>("WalManager::ReplayWalEntry unknown wal command type");
        }
    }
}
void WalManager::WalCmdCreateDatabaseReplay(const WalCmdCreateDatabase &cmd, u64 txn_id, i64 commit_ts) {
    auto [db_entry, status] = storage_->catalog()->CreateDatabase(cmd.db_name, txn_id, commit_ts, storage_->txn_manager());
    if (!status.ok()) {
        Error<StorageException>("Wal Replay: Create database failed");
    }
    db_entry->Commit(commit_ts);
}
void WalManager::WalCmdCreateTableReplay(const WalCmdCreateTable &cmd, u64 txn_id, i64 commit_ts) {

    auto [table_entry, table_status] =
        storage_->catalog()->CreateTable(cmd.db_name, txn_id, commit_ts, cmd.table_def, ConflictType::kIgnore, storage_->txn_manager());

    if (!table_status.ok()) {
        Error<StorageException>("Wal Replay: Create table failed" + *table_status.msg_);
    }
    table_entry->Commit(commit_ts);
}

void WalManager::WalCmdDropDatabaseReplay(const WalCmdDropDatabase &cmd, u64 txn_id, i64 commit_ts) {
    auto [db_entry, status] = storage_->catalog()->DropDatabase(cmd.db_name, txn_id, commit_ts, nullptr);
    if (!status.ok()) {
        Error<StorageException>("Wal Replay: Drop database failed");
    }
    db_entry->Commit(commit_ts);
}

void WalManager::WalCmdDropTableReplay(const WalCmdDropTable &cmd, u64 txn_id, i64 commit_ts) {
    auto [table_entry, table_status] =
        storage_->catalog()->DropTableByName(cmd.db_name, cmd.table_name, ConflictType::kIgnore, txn_id, commit_ts, storage_->txn_manager());
    if (!table_status.ok()) {
        Error<StorageException>("Wal Replay: Drop table failed {}", table_status.message());
    }
    table_entry->Commit(commit_ts);
}

void WalManager::WalCmdCreateIndexReplay(const WalCmdCreateIndex &cmd, u64 txn_id, i64 commit_ts) {
    auto [table_entry, table_index_entry, index_def_entry_status] =
        storage_->catalog()->CreateIndex(cmd.db_name_, cmd.table_name_, cmd.index_def_, ConflictType::kError, txn_id, commit_ts, nullptr);
    if (!index_def_entry_status.ok()) {
        Error<StorageException>("Wal Replay: Create index failed");
    }

    auto fake_txn = MakeUnique<Txn>(storage_->txn_manager(), storage_->catalog(), txn_id);
    auto table_store = MakeShared<TxnTableStore>(table_entry, fake_txn.get());

    NewCatalog::CreateIndexFile(table_entry, table_store.get(), table_index_entry, commit_ts, storage_->buffer_manager());
    NewCatalog::CommitCreateIndex(table_store->txn_indexes_store_);
    table_index_entry->Commit(commit_ts);
}

void WalManager::WalCmdDropIndexReplay(const WalCmdDropIndex &cmd, u64 txn_id, i64 commit_ts) {
    auto [table_index_entry, index_status] =
        storage_->catalog()
            ->DropIndex(cmd.db_name_, cmd.table_name_, cmd.index_name_, ConflictType::kIgnore, txn_id, commit_ts, storage_->txn_manager());
    if (!index_status.ok()) {
        Error<StorageException>("Wal Replay: Drop index failed" + *index_status.msg_);
    }

    table_index_entry->Commit(commit_ts);
}

void WalManager::WalCmdImportReplay(const WalCmdImport &cmd, u64 txn_id, i64 commit_ts) {

    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name, cmd.table_name, txn_id, commit_ts);
    if (!table_status.ok()) {
        Error<StorageException>(Format("Wal Replay: Get table failed {}", table_status.message()));
    }

    auto segment_dir_ptr = MakeShared<String>(cmd.segment_dir);
    auto segment_entry = SegmentEntry::MakeReplaySegmentEntry(table_entry, cmd.segment_id, segment_dir_ptr, commit_ts);

    for (int id = 0; id < cmd.block_entries_size; ++id) {
        auto block_entry = MakeUnique<BlockEntry>(segment_entry.get(),
                                                  id,
                                                  0,
                                                  table_entry->ColumnCount(),
                                                  storage_->buffer_manager(),
                                                  cmd.row_counts_[id],
                                                  commit_ts,
                                                  commit_ts);

        segment_entry->AppendBlockEntry(Move(block_entry));
        segment_entry->IncreaseRowCount(cmd.row_counts_[id]);
    }

    NewCatalog::ImportSegment(table_entry, cmd.segment_id, segment_entry);
}
void WalManager::WalCmdDeleteReplay(const WalCmdDelete &cmd, u64 txn_id, i64 commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name, cmd.table_name, txn_id, commit_ts);
    if (!table_status.ok()) {
        Error<StorageException>(Format("Wal Replay: Get table failed {}", table_status.message()));
    }

    auto fake_txn = MakeUnique<Txn>(storage_->txn_manager(), storage_->catalog(), txn_id);
    auto table_store = MakeShared<TxnTableStore>(table_entry, fake_txn.get());
    table_store->Delete(cmd.row_ids);
    fake_txn->FakeCommit(commit_ts);
    NewCatalog::Delete(table_store->table_entry_, table_store->txn_->TxnID(), table_store->txn_->CommitTS(), table_store->delete_state_);
    NewCatalog::CommitDelete(table_store->table_entry_, table_store->txn_->TxnID(), table_store->txn_->CommitTS(), table_store->delete_state_);
}

void WalManager::WalCmdAppendReplay(const WalCmdAppend &cmd, u64 txn_id, i64 commit_ts) {
    auto [table_entry, table_status] = storage_->catalog()->GetTableByName(cmd.db_name, cmd.table_name, txn_id, commit_ts);
    if (!table_status.ok()) {
        Error<StorageException>(Format("Wal Replay: Get table failed {}", table_status.message()));
    }

    auto fake_txn = MakeUnique<Txn>(storage_->txn_manager(), storage_->catalog(), txn_id);

    auto table_store = MakeShared<TxnTableStore>(table_entry, fake_txn.get());
    table_store->Append(cmd.block);

    auto append_state = MakeUnique<AppendState>(table_store->blocks_);
    table_store->append_state_ = Move(append_state);

    fake_txn->FakeCommit(commit_ts);
    NewCatalog::Append(table_store->table_entry_, table_store->txn_->TxnID(), table_store.get(), storage_->buffer_manager());
    NewCatalog::CommitAppend(table_store->table_entry_, table_store->txn_->TxnID(), table_store->txn_->CommitTS(), table_store->append_state_.get());
}

} // namespace infinity

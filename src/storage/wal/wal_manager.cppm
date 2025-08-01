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

export module infinity_core:wal_manager;

import :stl;
// import :options;
import :blocking_queue;
import :log_file;
import :options;

namespace infinity {
// class FlushOptionType;

export class Storage;
class BGTaskProcessor;
class NewTxn;
class CheckpointTaskBase;
class ForceCheckpointTask;
class BottomExecutor;

struct WalEntry;

export enum class StorageMode {
    kUnInitialized,
    kAdmin,
    kReadable,
    kWritable,
};

export String ToString(StorageMode storage_mode) {
    switch (storage_mode) {
        case StorageMode::kUnInitialized: {
            return "Uninitialized";
        }
        case StorageMode::kAdmin: {
            return "Admin";
        }
        case StorageMode::kReadable: {
            return "Readable";
        }
        case StorageMode::kWritable: {
            return "Writable";
        }
    }
}

export struct ReplayWalOptions {
    bool on_startup_;
    bool is_replay_;
    bool sync_from_leader_;
};

export class WalManager {
public:
    WalManager(Storage *storage, String wal_dir, u64 wal_size_threshold, FlushOptionType flush_option);
    WalManager(Storage *storage, String wal_dir, String data_dir, u64 wal_size_threshold, FlushOptionType flush_option);

    ~WalManager();

    void Start();

    void Stop();

    void SubmitTxn(Vector<NewTxn *> &txn_batch);

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void NewFlush();

    void FlushLogByReplication(const Vector<String> &synced_logs, bool on_startup);

    bool SetCheckpointing();
    bool UnsetCheckpoint();
    bool IsCheckpointing() const;

    void SwapWalFile(TxnTimeStamp max_commit_ts, bool error_if_duplicate);

    String GetWalFilename() const;

    Tuple<TransactionID, TxnTimeStamp, TxnTimeStamp> GetReplayEntries(StorageMode targe_storage_mode, Vector<SharedPtr<WalEntry>> &replay_entries);

    Tuple<TransactionID, TxnTimeStamp> ReplayWalEntries(const Vector<SharedPtr<WalEntry>> &replay_entries);

    Vector<SharedPtr<WalEntry>> CollectWalEntries() const;

    TxnTimeStamp LastCheckpointTS() const;
    void SetLastCheckpointTS(TxnTimeStamp new_last_ckp_ts);

    Vector<SharedPtr<String>> GetDiffWalEntryString(TxnTimeStamp timestamp) const;
    void UpdateCommitState(TxnTimeStamp commit_ts, i64 wal_size);

public:

    Tuple<TxnTimeStamp, i64> GetCommitState();
    void SetLastCkpWalSize(i64 wal_size);

private:
    i64 GetLastCkpWalSize();

public:
    u64 cfg_wal_size_threshold_{};

    const String &wal_dir() const { return wal_dir_; }
    const String &data_path() const { return data_path_; }

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    String wal_dir_{};
    String wal_path_{};
    String data_path_{};

    Storage *storage_{};

    // WalManager state
    Atomic<bool> running_{};
    Thread new_flush_thread_{};

    // TxnManager and Flush thread access following members
    BlockingQueue<NewTxn *> new_wait_flush_{"WalManager"};

    // Only Flush thread access following members
    std::ofstream ofs_{};
    FlushOptionType flush_option_{FlushOptionType::kOnlyWrite};
    UniquePtr<BottomExecutor> bottom_executor_{nullptr};

    // Flush and Checkpoint threads access following members
    mutable std::mutex mutex2_{};
    TxnTimeStamp max_commit_ts_{};
    TxnTimeStamp last_swap_wal_ts_{};
    i64 wal_size_{};
    i64 last_ckp_wal_size_{};
    Atomic<bool> checkpoint_in_progress_{false};

    // Only Checkpoint/Cleanup thread access following members
    mutable std::mutex last_ckp_ts_mutex_{};
    TxnTimeStamp last_ckp_ts_{};
};

} // namespace infinity

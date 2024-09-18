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

export module wal_manager;

import stl;
import bg_task;
import wal_entry;
import options;
import catalog_delta_entry;
import blocking_queue;
import log_file;

namespace infinity {

class Storage;
class BGTaskProcessor;
class TableEntry;
class Txn;
struct SegmentEntry;

export enum class StorageMode {
    kUnInitialized,
    kAdmin,
    kReadable,
    kWritable,
};

export class WalManager {
public:
    WalManager(Storage *storage, String wal_dir, u64 wal_size_threshold, u64 delta_checkpoint_interval_wal_bytes, FlushOptionType flush_option);
    WalManager(Storage *storage, String wal_dir, String data_dir, u64 wal_size_threshold, u64 delta_checkpoint_interval_wal_bytes, FlushOptionType flush_option);

    ~WalManager();

    void Start();

    void Stop();

    // Session request to persist an entry. Assuming txn_id of the entry has
    // been initialized.
    void PutEntries(Vector<WalEntry *> wal_entries);

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void Flush();

    bool TrySubmitCheckpointTask(SharedPtr<CheckpointTaskBase> ckp_task);

    void Checkpoint(bool is_full_checkpoint);

    void Checkpoint(ForceCheckpointTask *ckp_task);

    void SwapWalFile(TxnTimeStamp max_commit_ts);

    String GetWalFilename() const;

    i64 ReplayWalFile(StorageMode targe_storage_mode);

    Optional<Pair<FullCatalogFileInfo, Vector<DeltaCatalogFileInfo>>> GetCatalogFiles() const;

    Vector<SharedPtr<WalEntry>> CollectWalEntries() const;

    void ReplayWalEntry(const WalEntry &entry);

    TxnTimeStamp GetCheckpointedTS();

    Vector<SharedPtr<String>> GetDiffWalEntryString(TxnTimeStamp timestamp) const;

private:
    // Checkpoint Helper
    void FullCheckpointInner(Txn *txn);
    void DeltaCheckpointInner(Txn *txn);
    void UpdateCommitState(TxnTimeStamp commit_ts, i64 wal_size);
    Tuple<TxnTimeStamp, i64> GetCommitState();
    i64 GetLastCkpWalSize();
    void SetLastCkpWalSize(i64 wal_size);

    void WalCmdCreateDatabaseReplay(const WalCmdCreateDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropDatabaseReplay(const WalCmdDropDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCreateTableReplay(const WalCmdCreateTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropTableReplay(const WalCmdDropTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCreateIndexReplay(const WalCmdCreateIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropIndexReplay(const WalCmdDropIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdAppendReplay(const WalCmdAppend &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);

    // import and compact helper
    SharedPtr<SegmentEntry> ReplaySegment(TableEntry *table_entry, const WalSegmentInfo &segment_info, TransactionID txn_id, TxnTimeStamp commit_ts);

    void WalCmdImportReplay(const WalCmdImport &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDeleteReplay(const WalCmdDelete &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    // void WalCmdSetSegmentStatusSealedReplay(const WalCmdSetSegmentStatusSealed &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    // void WalCmdUpdateSegmentBloomFilterDataReplay(const WalCmdUpdateSegmentBloomFilterData &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCompactReplay(const WalCmdCompact &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdOptimizeReplay(WalCmdOptimize &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDumpIndexReplay(WalCmdDumpIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);

    void WalCmdRenameTableReplay(WalCmdRenameTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdAddColumnsReplay(WalCmdAddColumns &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropColumnsReplay(WalCmdDropColumns &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);

public:
    u64 cfg_wal_size_threshold_{};
    u64 cfg_delta_checkpoint_interval_wal_bytes_{};

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
    Thread flush_thread_{};

    // TxnManager and Flush thread access following members
    BlockingQueue<WalEntry *> wait_flush_{};

    // Only Flush thread access following members
    std::ofstream ofs_{};
    FlushOptionType flush_option_{FlushOptionType::kOnlyWrite};

    // Flush and Checkpoint threads access following members
    mutable std::mutex mutex2_{};
    TxnTimeStamp max_commit_ts_{};
    i64 wal_size_{};
    i64 last_ckp_wal_size_{};
    Atomic<bool> checkpoint_in_progress_{false};

    // Only Checkpoint/Cleanup thread access following members
    TxnTimeStamp last_ckp_ts_{};
    TxnTimeStamp last_full_ckp_ts_{};
};

} // namespace infinity

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

namespace infinity {

class Storage;
class BGTaskProcessor;
class TableEntry;
class Txn;

export class WalManager {
public:
    WalManager(Storage *storage, String wal_dir, u64 wal_size_threshold, u64 delta_checkpoint_interval_wal_bytes, FlushOption flush_option);

    ~WalManager();

    void Start();

    void Stop();

    // Session request to persist an entry. Assuming txn_id of the entry has
    // been initialized.
    int PutEntry(SharedPtr<WalEntry> entry);

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void Flush();

    void Checkpoint(bool is_full_checkpoint, TxnTimeStamp max_commit_ts, i64 wal_size);

    void Checkpoint(ForceCheckpointTask *ckp_task, TxnTimeStamp max_commit_ts, i64 wal_size);

    void SwapWalFile(TxnTimeStamp max_commit_ts);

    i64 ReplayWalFile();

    void ReplayWalEntry(const WalEntry &entry);

    // Should only called in `Flush` thread
    i64 WalSize() const { return wal_size_; }

private:
    // Checkpoint Helper
    void CheckpointInner(bool is_full_checkpoint, Txn *txn, TxnTimeStamp max_commit_ts, i64 wal_size);

private:
    void SetLastCkpWalSize(i64 wal_size);
    i64 GetLastCkpWalSize();

    void WalCmdCreateDatabaseReplay(const WalCmdCreateDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropDatabaseReplay(const WalCmdDropDatabase &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCreateTableReplay(const WalCmdCreateTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropTableReplay(const WalCmdDropTable &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCreateIndexReplay(const WalCmdCreateIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDropIndexReplay(const WalCmdDropIndex &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdAppendReplay(const WalCmdAppend &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);

    void ReplaySegment(TableEntry *table_entry, const WalSegmentInfo &segment_info, TxnTimeStamp commit_ts);

    void WalCmdImportReplay(const WalCmdImport &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdDeleteReplay(const WalCmdDelete &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    // void WalCmdSetSegmentStatusSealedReplay(const WalCmdSetSegmentStatusSealed &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    // void WalCmdUpdateSegmentBloomFilterDataReplay(const WalCmdUpdateSegmentBloomFilterData &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);
    void WalCmdCompactReplay(const WalCmdCompact &cmd, TransactionID txn_id, TxnTimeStamp commit_ts);

public:
    u64 cfg_wal_size_threshold_{};
    u64 cfg_delta_checkpoint_interval_wal_bytes_{};

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    String wal_dir_{};
    String wal_path_{};

    Storage *storage_{};

    // WalManager state
    Atomic<bool> running_{};
    Thread flush_thread_{};

    // TxnManager and Flush thread access following members
    std::mutex mutex_{};
    Deque<SharedPtr<WalEntry>> que_{};

    // Only Flush thread access following members
    Deque<SharedPtr<WalEntry>> que2_{};
    StdOfStream ofs_{};
    TxnTimeStamp max_commit_ts_{};
    i64 wal_size_{};
    FlushOption flush_option_{FlushOption::kOnlyWrite};

    // Flush and Checkpoint threads access following members
    std::mutex mutex2_{};
    i64 last_ckp_wal_size_{};

    // Only Checkpoint thread access following members
};

} // namespace infinity

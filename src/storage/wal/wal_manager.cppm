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
import wal_entry;
import bg_task;

export module wal_manager;

namespace infinity {

class Storage;
class BGTaskProcessor;

class SeqGenerator {
public:
    // Begin with 1 to avoid distinguish uninitialized value and the minimal
    // valid value.
    explicit SeqGenerator(i64 begin = 1) : next_seq_(begin) {}
    i64 Generate() { return next_seq_.fetch_add(1); }
    i64 GetLast() { return next_seq_.load() - 1; }

private:
    Atomic<i64> next_seq_;
};

export class WalManager {
public:
    static String WalCommandTypeToString(WalCommandType type);

public:
    WalManager(Storage *storage,
               String wal_path,
               u64 wal_size_threshold,
               u64 full_checkpoint_interval_sec,
               u64 delta_checkpoint_interval_sec,
               u64 delta_checkpoint_interval_wal_bytes);

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

    // Checkpoint is scheduled regularly.
    // Checkpoint for transactions which lsn no larger than lsn_pend_chk_.
    void CheckpointTimer();

    void Checkpoint();

    void Checkpoint(ForceCheckpointTask* ckp_task);

    void SwapWalFile(TxnTimeStamp max_commit_ts);

    i64 ReplayWalFile();

    void ReplayWalEntry(const WalEntry &entry);

    void RecycleWalFile(TxnTimeStamp full_ckp_ts);

private:
    void SetWalState(TxnTimeStamp max_commit_ts, i64 wal_size);
    Tuple<TxnTimeStamp, i64> GetWalState();

    void WalCmdCreateDatabaseReplay(const WalCmdCreateDatabase &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdDropDatabaseReplay(const WalCmdDropDatabase &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdCreateTableReplay(const WalCmdCreateTable &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdDropTableReplay(const WalCmdDropTable &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdCreateIndexReplay(const WalCmdCreateIndex &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdDropIndexReplay(const WalCmdDropIndex &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdAppendReplay(const WalCmdAppend &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdImportReplay(const WalCmdImport &cmd, u64 txn_id, i64 commit_ts);
    void WalCmdDeleteReplay(const WalCmdDelete &cmd, u64 txn_id, i64 commit_ts);

public:
    u64 wal_size_threshold_{};
    u64 full_checkpoint_interval_sec_{};
    u64 delta_checkpoint_interval_sec_{};
    u64 delta_checkpoint_interval_wal_bytes_{};

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    String wal_path_{};
    Storage *storage_{};

    // WalManager state
    Atomic<bool> running_{};
    Thread flush_thread_{};
    Thread checkpoint_thread_{};

    // TxnManager and Flush thread access following members
    Mutex mutex_{};
    Deque<SharedPtr<WalEntry>> que_{};

    // Only Flush thread access following members
    Deque<SharedPtr<WalEntry>> que2_{};
    StdOfStream ofs_{};

    // Flush and Checkpoint threads access following members
    Mutex mutex2_{};
    TxnTimeStamp max_commit_ts_{};
    i64 wal_size_{};

    // Only Checkpoint thread access following members
    TxnTimeStamp full_ckp_commit_ts_{};
    i64 full_ckp_wal_size_{};
    i64 full_ckp_when_{};
    i64 delta_ckp_wal_size_{};
    i64 delta_ckp_when_{};

    Vector<String> wal_list_{};
};

} // namespace infinity

//
// Created by jinhai on 23-5-16.
//

module;

import stl;
import wal_entry;

export module wal_manager;

namespace infinity {

class Storage;

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
    // Checkpoint for transactions which's lsn no larger than lsn_pend_chk_.
    void Checkpoint();

    void SwapWalFile(TxnTimeStamp max_commit_ts);

    i64 ReplayWalFile();

    void ReplayWalEntry(const WalEntry &entry);

    void RecycleWalFile();
private:
    void SetWalState(TxnTimeStamp max_commit_ts, i64 wal_size);
    void GetWalState(TxnTimeStamp& max_commit_ts, i64& wal_size);

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
    String wal_path_;
    Storage *storage_;

    // WalManager state
    Atomic<bool> running_;
    Thread flush_thread_;
    Thread checkpoint_thread_;

    // TxnManager and Flush thread access following members
    Mutex mutex_;
    Deque<SharedPtr<WalEntry>> que_;

    // Only Flush thread access following members
    Deque<SharedPtr<WalEntry>> que2_;
    StdOfStream ofs_;

    // Flush and Checkpoint threads access following members
    Mutex mutex2_;
    TxnTimeStamp max_commit_ts_;
    i64 wal_size_;

    // Only Checkpoint thread access following members
    TxnTimeStamp ckp_commit_ts_{};
    TxnTimeStamp full_ckp_commit_ts_{};
    i64 full_ckp_wal_size_;
    i64 full_ckp_when_;
    i64 delta_ckp_wal_size_;
    i64 delta_ckp_when_;

    Vector<String> wal_list_;
};

} // namespace infinity

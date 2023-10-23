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
    SeqGenerator(i64 begin = 1) : next_seq_(begin) {}
    i64 Generate() { return next_seq_.fetch_add(1); }
    i64 GetLast() { return next_seq_.load() - 1; }

private:
    Atomic<i64> next_seq_;
};

export class WalManager {
public:
    WalManager(Storage *storage,
               const String &wal_path,
               u64 wal_size_threshold,
               u64 full_checkpoint_time_interval,
               u64 full_checkpoint_txn_interval,
               u64 delta_checkpoint_time_interval,
               u64 delta_checkpoint_txn_interval);

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

    void RecycleWalFile();

public:
    u64 wal_size_threshold_{};
    u64 full_checkpoint_time_interval_{};
    u64 full_checkpoint_txn_interval_{};
    u64 delta_checkpoint_time_interval_{};
    u64 delta_checkpoint_txn_interval_{};

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    String wal_path_;
    // WalManager state
    Atomic<bool> running_;
    Thread flush_thread_;
    Thread checkpoint_thread_;

    Mutex mutex_; // protect que_ and ofs_
    Queue<SharedPtr<WalEntry>> que_, que2_, que3_;
    StdOfStream ofs_;

    SeqGenerator lsn_gen_;
    Atomic<TxnTimeStamp> commit_ts_pend_;
    TxnTimeStamp commit_ts_done_{};

    i64 checkpoint_ts_{};

    Storage *storage_;

    Vector<String> wal_list_;
};

} // namespace infinity

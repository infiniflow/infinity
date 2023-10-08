//
// Created by jinhai on 23-5-16.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "storage/txn/txn_manager.h"
#include "common/constants/constants.h"
#include "wal_entry.h"
#include <atomic>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>

namespace infinity {

class Storage;

class SeqGenerator {
public:
    // Begin with 1 to avoid distinguish uninitialized value and the minimal
    // valid value.
    SeqGenerator(int64_t begin = 1) : next_seq_(begin) {}
    int64_t
    Generate() { return next_seq_.fetch_add(1); }
    int64_t
    GetLast() { return next_seq_.load() - 1; }

private:
    std::atomic<int64_t> next_seq_;
};

class WalManager {
public:
    WalManager(Storage* storage, const std::string& wal_path);

    ~WalManager();

    void
    Start();

    void
    Stop();

    // Session request to persist an entry. Assuming txn_id of the entry has
    // been initialized.
    int
    PutEntry(std::shared_ptr<WalEntry> entry);

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void
    Flush();

    // Checkpoint is scheduled regularly.
    // Checkpoint for transactions which's lsn no larger than lsn_pend_chk_.
    void
    Checkpoint();

    void
    SwapWALFile(int64_t max_commit_ts);

    int64_t
    ReplayWALFile();

private:
    // Concurrent writing WAL is disallowed. So put all WAL writing into a queue
    // and do serial writing.
    String wal_path_;
    std::atomic<bool> running_;
    std::thread flush_thread_;
    std::thread checkpoint_thread_;

    std::mutex mutex_; // protect que_ and ofs_
    std::queue<std::shared_ptr<WalEntry>> que_, que2_, que3_;
    std::ofstream ofs_;

    SeqGenerator lsn_gen_;
    std::atomic<TxnTimeStamp> commit_ts_pend_;
    TxnTimeStamp commit_ts_done_{};

    int64_t checkpoint_ts_{};

    Storage* storage_;

    Vector<String> wal_list_;

    bool wait_for_checkpoint_{false};
};

} // namespace infinity

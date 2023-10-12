//
// Created by jinhai on 23-5-16.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "storage/txn/txn_manager.h"
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
    int64_t Generate() { return next_seq_.fetch_add(1); }
    int64_t GetLast() { return next_seq_.load() - 1; }

private:
    std::atomic<int64_t> next_seq_;
};

class WalManager {
public:
    WalManager(Storage *storage,
               const std::string &wal_path,
               u64 wal_size_threshold,
               u64 full_checkpoint_interval_sec,
               u64 delta_checkpoint_interval_sec,
               u64 delta_checkpoint_interval_wal_bytes);

    ~WalManager();

    void Start();

    void Stop();

    // Session request to persist an entry. Assuming txn_id of the entry has
    // been initialized.
    int PutEntry(std::shared_ptr<WalEntry> entry);

    // Flush is scheduled regularly. It collects a batch of transactions, sync
    // wal and do parallel committing. Each sync cost ~1s. Each checkpoint cost
    // ~10s. So it's necessary to sync for a batch of transactions, and to
    // checkpoint for a batch of sync.
    void Flush();

    // Checkpoint is scheduled regularly.
    // Checkpoint for transactions which's lsn no larger than lsn_pend_chk_.
    void Checkpoint();

    void SwapWalFile(TxnTimeStamp max_commit_ts);

    int64_t ReplayWalFile();

    void RecycleWalFile();
private:
    void SetWalState(TxnTimeStamp max_commit_ts, int64_t wal_size);
    void GetWalState(TxnTimeStamp& max_commit_ts, int64_t& wal_size);

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
    std::atomic<bool> running_;
    std::thread flush_thread_;
    std::thread checkpoint_thread_;

    // TxnManager and Fush thread access following members
    std::mutex mutex_;
    std::deque<std::shared_ptr<WalEntry>> que_;

    // Only Flush thread access following members
    std::deque<std::shared_ptr<WalEntry>> que2_;
    std::ofstream ofs_;

    // Flush and Checkpoint threads access following members
    std::mutex mutex2_;
    TxnTimeStamp max_commit_ts_;
    int64_t wal_size_;

    // Only Checkpoint thread access following members
    TxnTimeStamp ckp_commit_ts_{};
    int64_t full_ckp_wal_size_;
    int64_t full_ckp_when_;
    int64_t delta_ckp_wal_size_;
    int64_t delta_ckp_when_;
    Vector<String> wal_list_;
};

} // namespace infinity

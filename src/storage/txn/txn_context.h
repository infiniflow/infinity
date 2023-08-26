//
// Created by jinhai on 23-6-17.
//


#pragma once

#include "common/types/internal_types.h"
#include "constants.h"

namespace infinity {

class TxnContext {
public:
    inline void
    BeginCommit(TxnTimeStamp begin_ts) {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if(state_ != TxnState::kNotStarted) {
            StorageError("Transaction isn't in NOT_STARTED status.")
        }
        begin_ts_ = begin_ts;
        state_ = TxnState::kStarted;
    }

    inline TxnTimeStamp
    GetBeginTS() {
        std::shared_lock<RWMutex> r_locker(rw_locker_);
        return begin_ts_;
    }

    inline TxnTimeStamp
    GetCommitTS() {
        std::shared_lock<RWMutex> r_locker(rw_locker_);
        return commit_ts_;
    }

    inline TxnState
    GetTxnState() {
        std::shared_lock<RWMutex> r_locker(rw_locker_);
        return state_;
    }

    inline void
    SetTxnRollbacking(TxnTimeStamp rollback_ts) {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if(state_ != TxnState::kStarted) {
            StorageError("Transaction isn't in STARTED status.")
        }
        state_ = TxnState::kRollbacking;
        commit_ts_ = rollback_ts;
    }

    inline void
    SetTxnRollbacked() {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if(state_ != TxnState::kRollbacking) {
            StorageError("Transaction isn't in ROLLBACKING status.")
        }
        state_ = TxnState::kRollbacked;
    }

    inline void
    SetTxnCommitted() {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if(state_ != TxnState::kCommitting) {
            StorageError("Transaction isn't in COMMITTING status.")
        }
        state_ = TxnState::kCommitted;
    }

    inline void
    SetTxnCommitting(TxnTimeStamp commit_ts) {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if(state_ != TxnState::kStarted) {
            StorageError("Transaction isn't in STARTED status.")
        }
        state_ = TxnState::kCommitting;
        commit_ts_ = commit_ts;
    }

private:
    RWMutex rw_locker_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnState state_{TxnState::kNotStarted};
};

}

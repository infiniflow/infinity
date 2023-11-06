//
// Created by jinhai on 23-6-17.
//

module;

import stl;
import txn_state;

import infinity_exception;

export module txn_context;

namespace infinity {

class Txn;

export class TxnContext {
public:
    friend class Txn;

    inline void BeginCommit(TxnTimeStamp begin_ts) {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        if (state_ != TxnState::kNotStarted) {
            Error<StorageException>("Transaction isn't in NOT_STARTED status.");
        }
        begin_ts_ = begin_ts;
        state_ = TxnState::kStarted;
    }

    inline TxnTimeStamp GetBeginTS() {
        SharedLock<RWMutex> r_locker(rw_locker_);
        return begin_ts_;
    }

    inline TxnTimeStamp GetCommitTS() {
        SharedLock<RWMutex> r_locker(rw_locker_);
        return commit_ts_;
    }

    inline TxnState GetTxnState() {
        SharedLock<RWMutex> r_locker(rw_locker_);
        return state_;
    }

    inline void SetTxnRollbacking(TxnTimeStamp rollback_ts) {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        if (state_ != TxnState::kStarted) {
            Error<StorageException>("Transaction isn't in STARTED status.");
        }
        state_ = TxnState::kRollbacking;
        commit_ts_ = rollback_ts;
    }

    inline void SetTxnRollbacked() {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        if (state_ != TxnState::kRollbacking && state_!= TxnState::kCommitting) {
            Error<StorageException>("Transaction isn't in ROLLBACKING status.");
        }
        state_ = TxnState::kRollbacked;
    }

    inline void SetTxnCommitted() {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        if (state_ != TxnState::kCommitting) {
            Error<StorageException>("Transaction isn't in COMMITTING status.");
        }
        state_ = TxnState::kCommitted;
    }

    inline void SetTxnCommitting(TxnTimeStamp commit_ts) {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        if (state_ != TxnState::kStarted) {
            Error<StorageException>("Transaction isn't in STARTED status.");
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

} // namespace infinity

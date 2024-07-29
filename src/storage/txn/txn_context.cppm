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

export module txn_context;

import stl;
import txn_state;
import third_party;
import infinity_exception;
import logger;

namespace infinity {

class Txn;

export class TxnContext {
public:
    friend class Txn;

    TxnContext(TxnTimeStamp begin_ts) : begin_ts_(begin_ts) {}

    inline TxnTimeStamp GetBeginTS() const { return begin_ts_; }

    inline TxnTimeStamp GetCommitTS() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return commit_ts_;
    }

    TxnTimeStamp GetCommittedTS() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return committed_ts_;
    }

    inline TxnState GetTxnState() {
        std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
        return state_;
    }

    inline void SetTxnRollbacking(TxnTimeStamp rollback_ts) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kCommitting && state_ != TxnState::kStarted) {
            String error_message = fmt::format("Transaction is in {} status, which can't rollback.", ToString(state_));
            UnrecoverableError(error_message);
        }
        state_ = TxnState::kRollbacking;
        commit_ts_ = rollback_ts; // update commit_ts ?
    }

    inline void SetTxnRollbacked() {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        // if (state_ != TxnState::kRollbacking) {
        //     UnrecoverableError(fmt::format("Transaction isn't in ROLLBACKING status. {}", ToString(state_)));
        // }
        state_ = TxnState::kRollbacked;
        committed_ts_ = commit_ts_;
    }

    inline void SetTxnCommitted(TxnTimeStamp committed_ts) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kCommitting) {
            String error_message = "Transaction isn't in COMMITTING status.";
            UnrecoverableError(error_message);
        }
        state_ = TxnState::kCommitted;
        committed_ts_ = committed_ts;
    }

    inline void SetTxnCommitting(TxnTimeStamp commit_ts) {
        std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
        if (state_ != TxnState::kStarted) {
            String error_message = "Transaction isn't in STARTED status.";
            UnrecoverableError(error_message);
        }
        state_ = TxnState::kCommitting;
        commit_ts_ = commit_ts;
    }

    void SetTxnType(TxnType type) { type_ = type; }

    TxnType GetTxnType() const { return type_; }

private:
    std::shared_mutex rw_locker_{};
    const TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnTimeStamp committed_ts_{};
    TxnState state_{TxnState::kStarted};
    TxnType type_{TxnType::kInvalid};
};

} // namespace infinity

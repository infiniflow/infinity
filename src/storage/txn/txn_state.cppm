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

export module txn_state;

import stl;
import logger;
import infinity_exception;

namespace infinity {

export enum class TxnState {
    kNotStarted,
    kStarted,
    kCommitting,
    kCommitted,
    kRollbacking,
    kRollbacked,
    kInvalid,
};

export inline String TxnState2Str(TxnState txn_state) {
    switch (txn_state) {
        case TxnState::kNotStarted: {
            return "Not Started";
        }
        case TxnState::kStarted: {
            return "Started";
        }
        case TxnState::kCommitting: {
            return "Committing";
        }
        case TxnState::kCommitted: {
            return "Committed";
        }
        case TxnState::kRollbacking: {
            return "Rollbacking";
        }
        case TxnState::kRollbacked: {
            return "Rollbacked";
        }
        default: {
            break;
        }
    }
    String error_message = "Invalid transaction state.";
    UnrecoverableError(error_message);
    return String();
}

export enum class TransactionType {
    kCheckpoint, // Develop know it's a checkpoint txn
    kRead,       // Developer know it's a read txn
    kNormal,     // Developer doesn't know what type is this txn
    kReplay,
    kRecovery,
    kNewCheckpoint,
};

export inline String TransactionType2Str(TransactionType txn_type) {
    switch (txn_type) {
        case TransactionType::kCheckpoint: {
            return "Checkpoint";
        }
        case TransactionType::kRead: {
            return "Read";
        }
        case TransactionType::kNormal: {
            return "Normal";
        }
        case TransactionType::kReplay: {
            return "Replay";
        }
        case TransactionType::kRecovery: {
            return "Recovery";
        }
        case TransactionType::kNewCheckpoint: {
            return "NewCheckpoint";
        }
    }
    return "Normal";
}

} // namespace infinity

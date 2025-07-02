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
    kInvalid,
    kCheckpoint, // Develop know it's a checkpoint txn
    kRead,       // Developer know it's a read txn
    kNormal,     // Developer doesn't know what type is this txn
    kReplay,
    kRecovery,
    kNewCheckpoint,
    kAppend,
    kUpdate,
    kImport,
    kDelete,
    kCompact,
    kCreateDB,
    kCreateTable,
    kRestoreTable,
    kRestoreDatabase,
    kDropDB,
    kDropTable,
    kRenameTable,
    kCreateIndex,
    kDropIndex,
    kDumpMemIndex,
    kOptimizeIndex,
    kAddColumn,
    kDropColumn,
    kCleanup
};

export inline String TransactionType2Str(TransactionType txn_type) {
    switch (txn_type) {
        case TransactionType::kCheckpoint: {
            return "Checkpoint";
        }
        case TransactionType::kRead: {
            return "Read";
        }
        case TransactionType::kAppend: {
            return "Append";
        }
        case TransactionType::kDelete: {
            return "Delete";
        }
        case TransactionType::kCreateIndex: {
            return "CreateIndex";
        }
        case TransactionType::kOptimizeIndex: {
            return "optimize_index";
        }
        case TransactionType::kCompact: {
            return "compact";
        }
        case TransactionType::kDumpMemIndex: {
            return "Dump mem index";
        }
        case TransactionType::kImport: {
            return "import";
        }
        case TransactionType::kUpdate: {
            return "update";
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
        case TransactionType::kCreateDB: {
            return "CreateDB";
        }
        case TransactionType::kCreateTable: {
            return "CreateTable";
        }
        case TransactionType::kRestoreTable: {
            return "RestoreTable";
        }
        case TransactionType::kRestoreDatabase: {
            return "RestoreDatabase";
        }
        case TransactionType::kRenameTable: {
            return "RenameTable";
        }
        case TransactionType::kDropDB: {
            return "DropDB";
        }
        case TransactionType::kDropTable: {
            return "DropTable";
        }
        case TransactionType::kDropIndex: {
            return "DropIndex";
        }
        case TransactionType::kAddColumn: {
            return "AddColumn";
        }
        case TransactionType::kDropColumn: {
            return "DropColumn";
        }
        case TransactionType::kCleanup: {
            return "Cleanup";
        }
        case TransactionType::kInvalid: {
            return "Invalid";
        }
    }
    return "Normal";
}

} // namespace infinity

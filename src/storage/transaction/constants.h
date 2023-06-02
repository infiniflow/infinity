//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

constexpr TxnTimeStamp UNCOMMIT_TS = std::numeric_limits<TxnTimeStamp>::max();

enum class TxnState {
    kStarted,
    kCommitting,
    kCommitted,
    kRollbacking,
    kRollbacked,
    kInvalid,
};

String
ToString(TxnState txn_state) {
    switch(txn_state) {
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
    }
    StorageError("Invalid transaction state.")
}

}

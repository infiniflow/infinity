//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "common/types/internal_types.h"

namespace infinity {

constexpr TxnTimeStamp UNCOMMIT_TS = std::numeric_limits<TxnTimeStamp>::max();
constexpr i64 INVALID_SEGMENT_ID = std::numeric_limits<i64>::max();
constexpr i64 INITIAL_SEGMENT_ID = -1;

constexpr SizeT KB = 1024;
constexpr SizeT MB = 1024 * KB;
constexpr SizeT GB = 1024 * MB;

enum class TxnState {
    kNotStarted,
    kStarted,
    kCommitting,
    kCommitted,
    kRollbacking,
    kRollbacked,
    kInvalid,
};

inline String
ToString(TxnState txn_state) {
    switch(txn_state) {
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
    }
    StorageError("Invalid transaction state.")
}

}

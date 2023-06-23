//
// Created by jinhai on 23-6-17.
//


#pragma once

#include "common/types/internal_types.h"
#include "constants.h"

namespace infinity {

struct TxnContext {
public:
    inline void
    RLock() {
        rw_locker_.lock_shared();
    }

    inline void
    RUnLock() {
        rw_locker_.unlock_shared();
    }

    inline void
    Lock() {
        rw_locker_.lock();
    }

    inline void
    UnLock() {
        rw_locker_.unlock();
    }

public:
    RWMutex rw_locker_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};
    TxnState state_{TxnState::kNotStarted};
};

}

//
// Created by jinhai on 23-9-30.
//

#pragma once

#include "ctpl_stl.h"
#include <shared_mutex>

namespace infinity {

using RWMutex = std::shared_mutex;
using ThreadPool = ctpl::thread_pool;

using Thread = std::thread;

using au64 = std::atomic_uint64_t;
using aptr = std::atomic_uintptr_t;

}

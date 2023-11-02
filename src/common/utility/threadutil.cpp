//
// Created by jinhai on 23-10-22.
//

module;

#include <thread>

import std;
import stl;

module threadutil;

namespace infinity {

bool ThreadUtil::pin(Thread &thread, const u16 cpu_id) {
#if defined(__APPLE__)
    return false;
#else
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_id, &cpu_set);

    if (pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpu_set) != 0) {
        std::cerr << "Can not pin thread!" << std::endl;
        return false;
    }
    return true;
#endif
}

}

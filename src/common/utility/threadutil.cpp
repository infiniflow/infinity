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

#include <thread>
#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>
#endif

import stl;

module threadutil;

namespace infinity {

#ifdef __APPLE__

namespace mac {
const auto SYSCTL_CORE_COUNT = "machdep.cpu.core_count";

struct cpu_set_t {
    uint32_t count;
};

inline void CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

inline void CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

inline int CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }

inline int sched_getaffinity(pid_t, size_t, cpu_set_t *cpu_set) {
    int32_t core_count = 0;
    size_t len = sizeof(core_count);
    int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
    if (ret) {
        printf("error while get core count %d\n", ret);
        return -1;
    }
    cpu_set->count = 0;
    for (int i = 0; i < core_count; i++) {
        cpu_set->count |= (1 << i);
    }

    return 0;
}

inline int pthread_setaffinity_np(pthread_t thread, size_t cpu_size, cpu_set_t *cpu_set) {
    thread_port_t mach_thread;
    int core = 0;

    for (core = 0; core < (int)(8 * cpu_size); core++) {
        if (CPU_ISSET(core, cpu_set))
            break;
    }
    // printf("binding to core %d\n", core);
    thread_affinity_policy_data_t policy = {core};
    mach_thread = pthread_mach_thread_np(thread);
    thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1);
    return 0;
}
} // namespace mac
#endif

bool ThreadUtil::pin(Thread &thread, const u16 cpu_id) {
#if defined(__APPLE__)
    mac::cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    auto nh = thread.native_handle();
    mac::pthread_setaffinity_np(nh, sizeof(mac::cpu_set_t), &cpuset);
    return true;
#else
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_id, &cpu_set);

    if (pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpu_set) != 0) {
        std::cerr << "Can not pin thread!" <<std::endl;
        return false;
    }
    return true;
#endif
}

} // namespace infinity

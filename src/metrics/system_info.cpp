// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#if defined(__APPLE__)

#include <cerrno>
#include <libproc.h>
#include <mach/host_info.h>
#include <mach/kern_return.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/mach_traps.h>
#include <mach/shared_region.h>
#include <mach/task.h>
#include <mach/thread_act.h>
#include <mach/thread_info.h>
#include <mach/vm_map.h>
#include <sys/sysctl.h>
#include <unistd.h>

#endif

#include <cstdio>
#include <dirent.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

module system_info;

import stl;
import defer_op;
import status;
import third_party;
import logger;
import infinity_exception;
import default_values;

namespace infinity {

namespace {

#if defined(__APPLE__)

#define OK 0
#define MSEC 1000L
#define USEC MSEC * 1000L

#define TICK2MSEC(s) ((u64)(s) * ((u64)MSEC / (double)ticks))

#define NSEC2MSEC(s) ((u64)(s) / ((u64)1000000L))
#define NSEC2USEC(s) ((u64)(s) / ((u64)1000L))

#define tval2msec(tval) ((tval.seconds * MSEC) + (tval.microseconds / 1000))

#define tv2msec(tv) (((u64)tv.tv_sec * MSEC) + (((u64)tv.tv_usec) / 1000))

u64 get_sysctl_value(const char *name) {
    size_t len = 0;
    if (sysctlbyname(name, nullptr, &len, nullptr, 0) != 0) {
        LOG_ERROR(fmt::format("get_sysctl_value {} ", name));
        return 0;
    }
    if (len == sizeof(u64)) {
        u64 ret = 0;
        if (sysctlbyname(name, &ret, &len, nullptr, 0) != 0) {
            LOG_ERROR(fmt::format("get_sysctl_value {} : failed to fetch value", name));
            return 0;
        }
        return ret;
    }

    if (len == sizeof(uint32_t)) {
        uint32_t ret;
        if (sysctlbyname("vm.pagesize", &ret, &len, nullptr, 0) != 0) {
            LOG_ERROR(fmt::format("get_sysctl_value {} : failed to fetch value", name));
            return 0;
        }
        return ret;
    }
    LOG_ERROR(fmt::format("get_sysctl_value {} : Unexpected variable size", name));
    return 0;
}

vm_statistics64 get_vmstat() {
    mach_port_t mach_port = mach_host_self();
    vm_statistics64 vmstat;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    const auto status = host_statistics64(mach_port, HOST_VM_INFO64, (host_info_t)&vmstat, &count);

    if (status != KERN_SUCCESS) {
        LOG_ERROR(fmt::format("get_pinfo(): sysctl"));
    }
    return vmstat;
}

struct proc_time_t {
    u64 start_time, user, sys, total;
};

int get_proc_times(u64 pid, proc_time_t *time) {
    unsigned int count;
    time_value_t utime = {0, 0}, stime = {0, 0};
    task_basic_info_data_t ti;
    task_thread_times_info_data_t tti;
    task_port_t task, self;
    kern_return_t status;

    struct proc_taskinfo pti;
    int sz = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &pti, sizeof(pti));

    if (sz == sizeof(pti)) {
        time->user = NSEC2MSEC(pti.pti_total_user);
        time->sys = NSEC2MSEC(pti.pti_total_system);
        time->total = time->user + time->sys;
        return OK;
    }

    self = mach_task_self();
    status = task_for_pid(self, pid, &task);
    if (status != KERN_SUCCESS) {
        return errno;
    }

    count = TASK_BASIC_INFO_COUNT;
    status = task_info(task, TASK_BASIC_INFO, (task_info_t)&ti, &count);
    if (status != KERN_SUCCESS) {
        if (task != self) {
            mach_port_deallocate(self, task);
        }
        return errno;
    }

    count = TASK_THREAD_TIMES_INFO_COUNT;
    status = task_info(task, TASK_THREAD_TIMES_INFO, (task_info_t)&tti, &count);
    if (status != KERN_SUCCESS) {
        if (task != self) {
            mach_port_deallocate(self, task);
        }
        return errno;
    }

    time_value_add(&utime, &ti.user_time);
    time_value_add(&stime, &ti.system_time);
    time_value_add(&utime, &tti.user_time);
    time_value_add(&stime, &tti.system_time);

    time->user = tval2msec(utime);
    time->sys = tval2msec(stime);
    time->total = time->user + time->sys;

    return OK;
}

u64 cpu_total_cost() {
    mach_port_t mach_port = mach_task_self();

    kern_return_t status;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    host_cpu_load_info_data_t cpuload;

    status = host_statistics(mach_port, HOST_CPU_LOAD_INFO, (host_info_t)&cpuload, &count);

    if (status != KERN_SUCCESS) {
        LOG_ERROR(fmt::format("get_cpu(): host_statistics"));
        return 0;
    }

    int ticks = sysconf(_SC_CLK_TCK);
    u64 user = TICK2MSEC(cpuload.cpu_ticks[CPU_STATE_USER]);
    u64 sys = TICK2MSEC(cpuload.cpu_ticks[CPU_STATE_SYSTEM]);
    u64 idle = TICK2MSEC(cpuload.cpu_ticks[CPU_STATE_IDLE]);
    u64 nice = TICK2MSEC(cpuload.cpu_ticks[CPU_STATE_NICE]);
    u64 total = user + nice + sys + idle;

    return total;
}

u64 cpu_cost_of_process(pid_t pid) {
    // get specific pid cpu use time
    proc_time_t proctime;
    int st = get_proc_times(pid, &proctime);
    if (st != OK) {
        return 0;
    }

    return proctime.user + proctime.sys;
}

i64 get_memory() {
    u64 total_memory = get_sysctl_value("hw.memsize");
    u64 page_size = get_sysctl_value("vm.pagesize");
    const auto vmstat = get_vmstat();

    u64 free = vmstat.free_count * page_size;
    u64 used = total_memory - free;

    u64 kern = vmstat.inactive_count * page_size;
    u64 actual_free = free + kern;
    u64 actual_used = used - kern;
    return (i64)(actual_free + actual_used) / 1024;
}

int get_open_fd_count(pid_t pid) {
    int rv = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, nullptr, 0);
    if (rv < 0) {
        return -1;
    }

    UniquePtr<char[]> buffer(new char[static_cast<size_t>(rv)]);
    rv = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, buffer.get(), rv);
    if (rv < 0) {
        return -1;
    }
    return static_cast<int>(static_cast<unsigned long>(rv) / PROC_PIDLISTFD_SIZE);
}

#elif defined(linux) || defined(__linux) || defined(__linux__)

const char* get_items(const char* buffer, u32 item) {
    // read from buffer by offset
    const char *p = buffer;

    i64 len = std::strlen(buffer);
    i64 count = 0;

    for (int i = 0; i < len; i++) {
        if (' ' == *p) {
            count++;
            if (count == item - 1) {
                p++;
                break;
            }
        }
        ++p;
    }

    return p;
}

u64 cpu_total_cost() {
    // different mode cpu cost
    u64 user_time{0};
    u64 nice_time{0};
    u64 system_time{0};
    u64 idle_time{0};

    std::ifstream file("/proc/stat");
    if (!file) {
        return 0;
    }

    String line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        String name;
        iss >> name >> user_time >> nice_time >> system_time >> idle_time;
    }
    return (user_time + nice_time + system_time + idle_time);
}

u64 cpu_cost_of_process(pid_t pid) {
    // get specific pid cpu use time
    u32 tmp_pid;
    u64 utime;  // user time
    u64 stime;  // kernel time
    u64 cutime; // all user time
    u64 cstime; // all dead time

    FILE *fd;
    char line_buff[1024] = {0};
    String file_name = fmt::format("/proc/{}/stat", pid);

    fd = std::fopen(file_name.c_str(), "r");
    if (nullptr == fd)
        return 0;

    fgets(line_buff, sizeof(line_buff), fd);

    sscanf(line_buff, "%u", &tmp_pid);
    const char *q = get_items(line_buff, 14);
    std::istringstream iss(q);
    iss >> utime >> stime >> cutime >> cstime;

    fclose(fd);

    return (utime + stime + cutime + cstime);
}
#endif
}

i64 SystemInfo::MemoryUsage() {
    i64 vm_rss_in_kb = 0;
    try {
#if defined(__APPLE__)
        vm_rss_in_kb = get_memory();
#elif defined(linux) || defined(__linux) || defined(__linux__)
        FILE* file = fopen("/proc/self/status", "r");
        DeferFn defer_fn([&] {
            fclose(file);
        });

        constexpr i64 line_length = 128;
        char line_rss[line_length];
        std::memset(line_rss, 0, line_length);

        while (fgets(line_rss, line_length, file) != NULL)
        {
            if (std::strncmp(line_rss, "VmRSS:", 6) == 0)
            {
                LOG_DEBUG(line_rss);
                String str(line_rss + 6);
                String kb;
                std::istringstream iss(str);
                iss >> vm_rss_in_kb >> kb;
            }
        }
#endif
    } catch (std::exception& e) {
        Status status = Status::FailToGetSysInfo(fmt::format("Can't get VmRSS: {}", e.what()));
        RecoverableError(status);
    }
    return vm_rss_in_kb * KB;
}


f64 SystemInfo::CPUUsage() {

    pid_t current_pid = getpid();
    u64 total_cpu_cost1 = cpu_total_cost();
    u64 cpu_cost_of_process1 = cpu_cost_of_process(current_pid);

    usleep(200000); // sleep 200ms to fetch two time point cpu snapshots

    u64 total_cpu_cost2 = cpu_total_cost();
    u64 cpu_cost_of_process2 = cpu_cost_of_process(current_pid);

    f64 cpu_usage = 0.0;
    if (total_cpu_cost2 - total_cpu_cost1 != 0) {
        cpu_usage = (cpu_cost_of_process2 - cpu_cost_of_process1) / f64(total_cpu_cost2 - total_cpu_cost1);
    }

    i32 cpu_count = std::thread::hardware_concurrency();
    cpu_usage *= cpu_count; // should multiply cpu num in multiple cpu machine

    return cpu_usage;
}

i64 SystemInfo::OpenFileCount() {
    int count = 0;
#if defined(__APPLE__)
    pid_t current_pid = getpid();
    count = get_open_fd_count(current_pid);
#elif defined(linux) || defined(__linux) || defined(__linux__)
    String dir_path;
    DIR *dir;
    struct dirent *entry;

    pid_t current_pid = getpid();
    dir_path = fmt::format("/proc/{}/fd", current_pid);
    dir = opendir(dir_path.c_str());
    if (dir == NULL) {
        Status status = Status::FailToGetSysInfo(fmt::format("Can't open dir: {}", dir_path));
        RecoverableError(status);
    }

    while ((entry = readdir(dir)) != NULL) {
        LOG_DEBUG(fmt::format("PID: {}, {}", current_pid, entry->d_name));
        if (entry->d_name[0] == '.') {
            continue;
        }
        count++;
    }

    closedir(dir);
#endif
    return count;
}

}

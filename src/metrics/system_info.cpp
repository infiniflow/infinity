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

#include <cstdio>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

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


u64 cpu_total_cost() {
    // different mode cpu cost
    u64 user_time;
    u64 nice_time;
    u64 system_time;
    u64 idle_time;

    FILE *fd;
    char buffer[1024] = {0};

    fd = fopen("/proc/stat", "r");
    if (nullptr == fd) {
        return 0;
    }

    fgets(buffer, sizeof(buffer), fd);
    char name[64] = {0};
    sscanf(buffer, "%s %lld %lld %lld %lld", name, &user_time, &nice_time, &system_time, &idle_time);
    fclose(fd);

    return (user_time + nice_time + system_time + idle_time);
}


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
    sscanf(q, "%lld %lld %lld %lld", &utime, &stime, &cutime, &cstime);
    fclose(fd);

    return (utime + stime + cutime + cstime);
}

}

i64 SystemInfo::MemoryUsage() {
    i64 vm_rss_in_kb = 0;
    try {
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

                char kb[line_length];
                sscanf(str.c_str(), "%lld %s", &vm_rss_in_kb, kb);
            }
        }
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
    String dir_path;
    int count = 0;
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
    return count;
}

}

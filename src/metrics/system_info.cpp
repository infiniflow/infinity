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
                sscanf(str.c_str(), "%ld %s", &vm_rss_in_kb, kb);
            }
        }
    } catch (std::exception& e) {
        Status status = Status::FailToGetSysInfo(fmt::format("Can't get VmRSS: {}", e.what()));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    return vm_rss_in_kb * KB;
}

f64 SystemInfo::CPUUsage() {


//    unsigned long totalcputime1, totalcputime2;
//    unsigned long procputime1, procputime2;
//
//    totalcputime1 = get_cpu_total_occupy();
//    procputime1 = get_cpu_proc_occupy(pid);
//
//    // FIXME: the 200ms is a magic number, works well
//    usleep(200000); // sleep 200ms to fetch two time point cpu usage snapshots sample for later calculation
//
//    totalcputime2 = get_cpu_total_occupy();
//    procputime2 = get_cpu_proc_occupy(pid);
//
//    float pcpu = 0.0;
//    if (0 != totalcputime2 - totalcputime1)
//        pcpu = (procputime2 - procputime1) / float(totalcputime2 - totalcputime1); // float number
//
//    int cpu_num = get_nprocs();
//    pcpu *= cpu_num; // should multiply cpu num in multiple cpu machine
//
//    return pcpu;

    return 0;
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
        LOG_ERROR(status.message());
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

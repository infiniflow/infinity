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
    return 0;
}

i64 SystemInfo::OpenFDCount() {
    return 0;
}

}

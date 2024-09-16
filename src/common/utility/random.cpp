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

#include <cassert>
#include <cstdlib>

module random;

import stl;
import third_party;
import logger;
import local_file_system;
import default_values;
import infinity_context;

namespace infinity {

namespace {
String available_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

String RandomString(SizeT len) {
    // LOG_WARN(fmt::format("Set seed: {}", seed));
    String ret(len, '\0');
    for (SizeT i = 0; i < len; i++) {
        SizeT rand_i = random() % available_chars.size();
        ret[i] = available_chars[rand_i];
    }
    return ret;
}

SharedPtr<String> DetermineRandomString(const String &parent_dir, const String &name) {
    assert(std::filesystem::path(parent_dir).is_absolute());
    LocalFileSystem fs;
    String rand, temp, result;
    int cnt = 0;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        srand(std::time(nullptr));
    }

    bool use_persistence_manager = InfinityContext::instance().persistence_manager();
    bool created = false;
    do {
        rand = RandomString(DEFAULT_RANDOM_NAME_LEN);
        result = fmt::format("{}_{}", rand, name);
        temp = LocalFileSystem::ConcatenateFilePath(parent_dir, result);
        ++cnt;
        if(!use_persistence_manager) {
            created = fs.CreateDirectoryNoExp(temp);
        } else {
            created = true;
        }
    } while (!created);
    LOG_DEBUG(fmt::format("Created directory {} in {} times", temp, cnt));
    return MakeShared<String>(std::move(result));
}

} // namespace infinity

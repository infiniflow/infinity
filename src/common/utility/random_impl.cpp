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

module infinity_core:random.impl;

import :random;
import :stl;
import :logger;
import :virtual_store;
import :default_values;
import :infinity_context;
import :status;

import third_party;

namespace infinity {

namespace {
std::string available_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

std::string RandomString(size_t len) {
    // LOG_WARN(fmt::format("Set seed: {}", seed));
    std::string ret(len, '\0');
    for (size_t i = 0; i < len; i++) {
        size_t rand_i = random() % available_chars.size();
        ret[i] = available_chars[rand_i];
    }
    return ret;
}

std::shared_ptr<std::string> DetermineRandomPath(const std::string &name) {
    std::string rand, result;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        srand(std::time(nullptr));
    }
    rand = RandomString(DEFAULT_RANDOM_NAME_LEN);
    result = fmt::format("{}_{}", rand, name);
    return std::make_shared<std::string>(std::move(result));
}

std::shared_ptr<std::string> DetermineRandomString(const std::string &parent_dir, const std::string &name) {
    assert(std::filesystem::path(parent_dir).is_absolute());
    std::string rand, temp, result;
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
        temp = VirtualStore::ConcatenatePath(parent_dir, result);
        ++cnt;
        if (!use_persistence_manager) {
            if (auto status = VirtualStore::MakeDirectory(temp); status.ok()) {
                created = true;
            } else {
                created = false;
            }
        } else {
            created = true;
        }
    } while (!created);
    LOG_DEBUG(fmt::format("Created directory {} in {} times", temp, cnt));
    return std::make_shared<std::string>(std::move(result));
}

} // namespace infinity

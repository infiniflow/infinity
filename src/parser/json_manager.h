// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#pragma once

import std.compat;
import third_party;

namespace infinity {

class JsonManager {
public:
    static nlohmann::json parse(const std::string &json_str);
    static nlohmann::json from_bson(const std::vector<uint8_t> &bson_data);
    static nlohmann::json dump(const nlohmann::json &json_obj);
    static std::vector<uint8_t> to_bson(const nlohmann::json &json_obj);
};

} // namespace infinity

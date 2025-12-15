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

module infinity_core:json_manager.impl;

import :json_manager;

namespace infinity {

bool JsonManager::valid_json(const std::string &valid_json) { return nlohmann::json::accept(valid_json); }

nlohmann::json JsonManager::parse(const std::string &json_str) { return nlohmann::json::parse(json_str); }

nlohmann::json JsonManager::from_bson(const std::vector<uint8_t> &bson_data) { return nlohmann::json::from_bson(bson_data); }

std::string JsonManager::dump(const nlohmann::json &json_obj) { return json_obj.dump(); }

std::vector<uint8_t> JsonManager::to_bson(const nlohmann::json &json_obj) { return nlohmann::json::to_bson(json_obj); }

} // namespace infinity
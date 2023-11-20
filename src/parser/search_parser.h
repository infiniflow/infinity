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

#pragma once
#include <string>
#include <utility>
#include <vector>
namespace infinity {

class SearchParser {
public:
    static void ParseFields(const std::string &fields_str, std::vector<std::pair<std::string, float>> &fields);
    static std::string FieldsToString(const std::vector<std::pair<std::string, float>> &fields);
    static void ParseOptions(const std::string &options_str, std::vector<std::pair<std::string, std::string>> &options);
    static std::string OptionsToString(const std::vector<std::pair<std::string, std::string>> &options);

private:
    static std::pair<std::string, float> ParseField(const std::string_view &field);
    static std::pair<std::string, std::string> ParseOption(const std::string_view &option);
};

} // namespace infinity
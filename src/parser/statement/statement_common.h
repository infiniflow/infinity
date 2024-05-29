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

#include <memory>
#include <string>
#include <vector>

namespace infinity {

enum class CopyFileType {
    kCSV,
    kJSON,
    kJSONL,
    kFVECS,
    kCSR,
    kInvalid,
};

inline std::shared_ptr<std::string> CopyFileTypeToStr(CopyFileType copy_file_type) {
    switch (copy_file_type) {
        case CopyFileType::kCSV:
            return std::make_shared<std::string>("CSV");
        case CopyFileType::kJSON:
            return std::make_shared<std::string>("JSON");
        case CopyFileType::kFVECS:
            return std::make_shared<std::string>("FVECS");
        case CopyFileType::kJSONL:
            return std::make_shared<std::string>("JSONL");
        case CopyFileType::kCSR:
            return std::make_shared<std::string>("CSR");
        case CopyFileType::kInvalid:
            return std::make_shared<std::string>("Invalid");
    }
}

struct TableName {
    char *schema_name_ptr_{nullptr};
    char *table_name_ptr_{nullptr};
};

struct InitParameter {
    std::string param_name_{};
    std::string param_value_{};
};

struct IfExistsInfo {
    bool exists_{false};
    bool if_exists_{false};
    std::string info_{};
};

struct IfNotExistsInfo {
    bool exists_{false};
    bool if_not_exists_{false};
    std::string info_{};
};
} // namespace infinity

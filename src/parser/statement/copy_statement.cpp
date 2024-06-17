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

#include "copy_statement.h"
#include <sstream>

namespace infinity {

CopyStatement::~CopyStatement() {
    if (expr_array_ != nullptr) {
        for (auto &expr_ptr : *expr_array_) {
            delete expr_ptr;
        }
        delete expr_array_;
        expr_array_ = nullptr;
    }
}

std::string CopyStatement::ToString() const {
    std::stringstream ss;
    std::string copy_direction;
    if (copy_from_) {
        copy_direction = " FROM ";
    } else {
        copy_direction = " TO ";
    }

    std::string file_format;
    switch (copy_file_type_) {
        case CopyFileType::kCSV: {
            file_format = "CSV";
            break;
        }
        case CopyFileType::kJSON: {
            file_format = "JSON";
            break;
        }
        case CopyFileType::kFVECS: {
            file_format = "FVECS";
            break;
        }
        case CopyFileType::kJSONL: {
            file_format = "JSONL";
            break;
        }
        case CopyFileType::kCSR: {
            file_format = "CSR";
            break;
        }
        case CopyFileType::kBVECS: {
            file_format = "BVECS";
            break;
        }
        case CopyFileType::kInvalid: {
            file_format = "Invalid";
            break;
        }
    }

    ss << "COPY: " << schema_name_ << "." << table_name_ << copy_direction << file_path_ << " WITH " << file_format << " delimiter: " << delimiter_;
    return ss.str();
}

} // namespace infinity

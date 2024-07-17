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

#include "create_index_info.h"
#include "parser_assert.h"

#include <sstream>

namespace infinity {

std::string IndexInfo::IndexTypeToString(IndexType index_type) {
    switch (index_type) {
        case IndexType::kIVFFlat: {
            return "IVFFlat";
        }
        case IndexType::kHnsw: {
            return "HNSW";
        }
        case IndexType::kFullText: {
            return "FULLTEXT";
        }
        case IndexType::kSecondary: {
            return "SECONDARY";
        }
        case IndexType::kEMVB: {
            return "EMVB";
        }
        case IndexType::kBMP: {
            return "BMP";
        }
        case IndexType::kDiskAnn: {
            return "DISKANN";
        }
        case IndexType::kInvalid: {
            ParserError("Invalid conflict type.");
        }
    }
    return {};
}

IndexType IndexInfo::StringToIndexType(const std::string &index_type_str) {
    if (index_type_str == "IVFFlat") {
        return IndexType::kIVFFlat;
    } else if (index_type_str == "HNSW") {
        return IndexType::kHnsw;
    } else if (index_type_str == "FULLTEXT") {
        return IndexType::kFullText;
    } else if (index_type_str == "SECONDARY") {
        return IndexType::kSecondary;
    } else if (index_type_str == "EMVB") {
        return IndexType::kEMVB;
    } else if (index_type_str == "BMP") {
        return IndexType::kBMP;
    } else if (index_type_str == "DISKANN") {
        return IndexType::kDiskAnn;
    } else {
        return IndexType::kInvalid;
    }
}

IndexInfo::~IndexInfo() {
    if (index_param_list_ != nullptr) {
        for (auto &index_param : *index_param_list_) {
            delete index_param;
            index_param = nullptr;
        }
        delete index_param_list_;
        index_param_list_ = nullptr;
    }
}

CreateIndexInfo::~CreateIndexInfo() {
    if (index_info_list_ != nullptr) {
        for (auto &index_info : *index_info_list_) {
            delete index_info;
            index_info = nullptr;
        }
        delete index_info_list_;
        index_info_list_ = nullptr;
    }
}

std::string CreateIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "CREATE INDEX ";
    switch (conflict_type_) {
        case ConflictType::kIgnore: {
            ss << "Ignore ";
            break;
        }
        case ConflictType::kError: {
            ss << "Error ";
            break;
        }
        case ConflictType::kReplace: {
            ss << "Replace ";
            break;
        }
        case ConflictType::kInvalid: {
            ParserError("Invalid conflict type.");
        }
    }
    if (index_name_.empty()) {
        ss << "DEFAULT_INDEX_NAME ";
    } else {
        ss << index_name_;
    }
    ss << " ON " << table_name_ << "(";
    size_t index_count = index_info_list_->size();
    if (index_count == 0) {
        ParserError("No index info.");
    }

    for (size_t idx = 0; idx < index_count; ++idx) {
        IndexInfo *index_info = (*index_info_list_)[idx];
        ss << index_info->column_name_;
        ss << " USING " << IndexInfo::IndexTypeToString(index_info->index_type_);
        if (index_info->index_param_list_ != nullptr && !index_info->index_param_list_->empty()) {
            ss << " WITH(";
            size_t param_count = index_info->index_param_list_->size();
            for (size_t param_idx = 0; param_idx < param_count; ++param_idx) {
                ss << (*index_info->index_param_list_)[param_idx]->param_name_ << " " << (*index_info->index_param_list_)[param_idx]->param_value_;
                if (param_idx != param_count - 1) {
                    ss << ", ";
                }
            }
            ss << ")";
        }
    }
    ss << ";";
    return ss.str();
}

} // namespace infinity

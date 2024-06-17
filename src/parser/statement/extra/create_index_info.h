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

#include "extra_ddl_info.h"
#include "statement/statement_common.h"
#include <string>
#include <vector>

namespace infinity {

enum class IndexType {
    kIVFFlat,
    kHnsw,
    kFullText,
    kSecondary,
    kEMVB,
    kInvalid,
};

struct IndexInfo {
    ~IndexInfo();
    IndexType index_type_{IndexType::kInvalid};
    std::string column_name_{};
    std::vector<InitParameter *> *index_param_list_{nullptr};

    static std::string IndexTypeToString(IndexType index_type);
    static IndexType StringToIndexType(const std::string& index_type_str);
};

class CreateIndexInfo final : public ExtraDDLInfo {
public:
    explicit CreateIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    ~CreateIndexInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{};
    std::string table_name_{};
    std::string index_name_{};

    std::vector<IndexInfo *> *index_info_list_{};
};

} // namespace infinity

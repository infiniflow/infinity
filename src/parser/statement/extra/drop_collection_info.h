//
// Created by jinhai on 23-2-24.
//

#pragma once

#include "extra_ddl_info.h"
#include "statement/statement_common.h"

namespace infinity {

class DropCollectionInfo final : public ExtraDDLInfo {
public:
    explicit DropCollectionInfo() : ExtraDDLInfo(DDLType::kCollection) {}

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string collection_name_{};
};

} // namespace infinity

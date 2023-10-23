//
// Created by jinhai on 23-3-3.
//

#pragma once

#include "extra_ddl_info.h"
#include "statement/statement_common.h"
#include <string>
#include <vector>

namespace infinity {

class CreateIndexInfo final : public ExtraDDLInfo {
public:
    explicit CreateIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    ~CreateIndexInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string index_name_{};
    std::string schema_name_{"default"};
    std::string table_name_{};
    std::string method_type_{};

    std::vector<std::string> *column_names_{nullptr};
    std::vector<InitParameter *> *index_para_list_{nullptr};
};

} // namespace infinity

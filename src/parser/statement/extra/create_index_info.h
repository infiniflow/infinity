//
// Created by jinhai on 23-3-3.
//

#pragma once

#include "common/types/alias/containers.h"
#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class CreateIndexInfo final : public ExtraDDLInfo {
public:
    explicit CreateIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    ~CreateIndexInfo() final;

    [[nodiscard]] String ToString() const final;

    String index_name_{};
    String schema_name_{"default"};
    String table_name_{};
    String method_type_{};

    Vector<String>* column_names_{nullptr};
    Vector<InitParameter*>* index_para_list_{nullptr};
};

} // namespace infinity

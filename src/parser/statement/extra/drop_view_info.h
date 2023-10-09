//
// Created by jinhai on 23-3-5.
//

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class DropViewInfo : public ExtraDDLInfo {
public:
    explicit DropViewInfo() : ExtraDDLInfo(DDLType::kView) {}

    [[nodiscard]] String ToString() const final;

    String schema_name_{"default"};
    String view_name_{};
};

} // namespace infinity

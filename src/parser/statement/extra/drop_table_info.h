//
// Created by jinhai on 23-3-5.
//

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class DropTableInfo : public ExtraDDLInfo {
public:
    explicit
    DropTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{"Default"};
    String table_name_{};
};

}

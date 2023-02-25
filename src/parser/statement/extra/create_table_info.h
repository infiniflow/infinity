//
// Created by jinhai on 23-2-25.
//

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class CreateTableInfo : public ExtraDDLInfo {
public:
    explicit
    CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    [[nodiscard]] virtual String
    ToString() const final;

    String schema_name_{"Default"};
    String table_name_{};
};

}

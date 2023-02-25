//
// Created by jinhai on 23-2-24.
//

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class DropCollectionInfo : public ExtraDDLInfo {
public:
    explicit
    DropCollectionInfo() : ExtraDDLInfo(DDLType::kCollection) {}

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{"Default"};
    String table_name_{};
};

}

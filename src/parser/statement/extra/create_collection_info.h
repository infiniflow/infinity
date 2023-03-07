//
// Created by jinhai on 23-2-23.
//

#pragma once

#include "extra_ddl_info.h"

namespace infinity {

class CreateCollectionInfo final : public ExtraDDLInfo {
public:
    explicit
    CreateCollectionInfo() : ExtraDDLInfo(DDLType::kCollection) {}

    [[nodiscard]] virtual String
    ToString() const final;

    String schema_name_{"Default"};
    String collection_name_{};
};

}

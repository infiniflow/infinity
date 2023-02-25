//
// Created by jinhai on 23-2-25.
//

#pragma once

#include "parser/base_statement.h"
#include "statement_common.h"

namespace infinity {

enum class CopyOptionType {
    kFormat,
    kDelimiter,
    kHeader,
};

struct CopyOption {
    CopyOptionType option_type_{CopyOptionType::kFormat};
    bool header_{false};
    CopyFileType file_type_{CopyFileType::kCSV};
    char delimiter_{','};
};

class CopyStatement : public BaseStatement {
public:
    CopyStatement() : BaseStatement(StatementType::kCopy) {}

    [[nodiscard]] String
    ToString() const final;

    bool copy_from_{false};
    String file_path_{};
    String table_name_{};
    String schema_name_{"Default"};
    bool header_{false};
    CopyFileType copy_file_type_{CopyFileType::kCSV};
    char delimiter_{','};

};

}

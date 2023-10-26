//
// Created by jinhai on 23-2-25.
//

#pragma once

#include "base_statement.h"
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

    [[nodiscard]] std::string ToString() const final;

    bool copy_from_{false};
    std::string file_path_{};
    std::string table_name_{};
    std::string schema_name_{"default"};
    bool header_{false};
    CopyFileType copy_file_type_{CopyFileType::kCSV};
    char delimiter_{','};
};

} // namespace infinity

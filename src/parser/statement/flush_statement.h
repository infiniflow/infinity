//
// Created by jinhai on 23-8-26.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/extra/create_schema_info.h"
#include "parser/statement/extra/create_collection_info.h"
#include "parser/statement/extra/create_table_info.h"
#include "parser/statement/extra/create_view_info.h"
#include "parser/statement/extra/create_index_info.h"


namespace infinity {

enum class FlushType {
    kData,
    kLog,
    kBuffer,
};

class FlushStatement : public BaseStatement {
public:
    explicit
    FlushStatement() : BaseStatement(StatementType::kFlush) {}

    [[nodiscard]] String
    ToString() const final;

    inline FlushType
    type() const {
        return type_;
    }

    FlushType type_;

};

}
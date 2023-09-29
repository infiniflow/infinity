//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser.h"
#include "parser_result.h"
#include "lexer.h"

#include <string>
#include <memory>

namespace infinity {

class SQLParser {

public:

    SQLParser();

    ~SQLParser();

    void
    Parse(const std::string& sql_text, std::shared_ptr<ParserResult>& result);

    void* scanner_{};
    YY_BUFFER_STATE state_;

};

}


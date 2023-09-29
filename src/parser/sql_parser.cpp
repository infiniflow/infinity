//
// Created by jinhai on 23-2-22.
//

#include "sql_parser.h"

#include "lexer.h"

#include <iostream>

namespace infinity {

SQLParser::SQLParser() {
    scanner_ = nullptr;
    state_ = nullptr;
    if(sqllex_init(&scanner_)) {
        std::cerr << "Init lexer error" << std::endl;
    }
}

SQLParser::~SQLParser() {
    sqllex_destroy(scanner_);
}

void
SQLParser::Parse(const String& sql_text, SharedPtr<ParserResult>& result) {

    state_ = sql_scan_string(sql_text.c_str(), scanner_);

    if(sqlparse(scanner_, result.get())) {
        std::cerr << "Parse error: " << sql_text << std::endl;
    }

    sql_delete_buffer(state_, scanner_);

}

}

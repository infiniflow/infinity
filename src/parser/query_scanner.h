#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "location.hh"
#include "query_parser.h"

namespace infinity {

class QueryScanner : public yyFlexLexer {
public:
    QueryScanner(std::istream *in) : yyFlexLexer(in){};
    virtual ~QueryScanner(){};

    // get rid of override virtual function warning
    using FlexLexer::yylex;

    virtual int yylex(QueryParser::semantic_type *const lval, QueryParser::location_type *location);
    // YY_DECL defined in lucene_lexer.l
    // Method body created by flex in lucene_lexer.y.cc

private:
    /* yylval ptr */
    QueryParser::semantic_type *yylval = nullptr;
};

} // namespace infinity


// Bison variant of [Lucene syntax](https://github.com/apache/lucene/blob/main/lucene/queryparser/src/java/org/apache/lucene/queryparser/flexible/standard/parser/StandardSyntaxParser.jj)

%language "c++"
%skeleton "lalr1.cc"
%require "3.8"
%debug
%defines
%define api.namespace {infinity}
%define api.parser.class {SearchParser}
%define api.value.type variant
%define api.token.raw
%define parse.assert
%define parse.error verbose

/* code snippet copied to generated search_parser.tab.hh */
%code requires{
    // unique_ptr<QueryNode> requires sizeof(QueryNode)
    #include "query_node.h"

    namespace infinity {
        class SearchDriver;
        class SearchScanner;
    }
}

%parse-param {SearchScanner &scanner} {SearchDriver &driver}

/* code snippet copied to generated search_parser.tab.cc */
%code{
    #include "search_driver.h"
    #include "search_scanner.h"

    #undef yylex
    #define yylex scanner.yylex

    #include <iostream>
    #include <sstream>
    #include <string>
    #include <vector>
    #include "query_node.h"

    // Avoid warnings with the error counter.
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #endif
}

/* terminal symbol */
%token                 END    0     "end of file"
%token                 AND
%token                 OR
%token                 NOT
%token                 LPAREN
%token                 RPAREN
%token                 OP_COLON
%token <float>         CARAT
%token <std::string>   STRING

/* nonterminal symbol */
%type <std::unique_ptr<QueryNode>>  topLevelQuery query clause term basic_filter_boost basic_filter

%locations

%%

// TopLevelQuery ::= Query <EOF>
// Query ::= DisjQuery ( DisjQuery )*
// DisjQuery ::= ConjQuery ( OR ConjQuery )*
// ConjQuery ::= Clause ( AND Clause )*
topLevelQuery
: query END {
    driver.result_ = std::move($1);
};

query
: clause { $$ = std::move($1); }
| query clause {
    auto query = std::make_unique<Or>();
    query->Add(std::move($1));
    query->Add(std::move($2));
    $$ = std::move(query);
}
| query OR clause {
    auto query = std::make_unique<Or>();
    query->Add(std::move($1));
    query->Add(std::move($3));
    $$ = std::move(query);
};

clause
: term { $$ = std::move($1); }
| clause AND term {
    auto query = std::make_unique<And>();
    query->Add(std::move($1));
    query->Add(std::move($3));
    $$ = std::move(query);
};

term
: basic_filter_boost { $$ = std::move($1); }
| NOT term {
    auto query = std::make_unique<Not>();
    query->Add(std::move($2));
    $$ = std::move(query);
}
| LPAREN query RPAREN { $$ = std::move($2); }
| LPAREN query RPAREN CARAT {
    $$ = std::move($2);
    $$->MultiplyWeight($4);
};

basic_filter_boost
: basic_filter {
    $$ = std::move($1);
}
| basic_filter CARAT {
    $$ = std::move($1);
    $1->MultiplyWeight($2);
};

basic_filter
: STRING {
    const std::string &field = driver.default_field_;
    if(field.empty()){
        error(@1, "driver.default_field is empty");
        YYERROR;
    }
    std::vector<std::string> terms;
    driver.Analyze(field, $1, terms);
    $$ = SearchDriver::BuildQueryNodeByFieldAndTerms(field, terms);
}
| STRING OP_COLON STRING {
    std::vector<std::string> terms;
    driver.Analyze($1, $3, terms);
    const std::string &field = $1;
    $$ = SearchDriver::BuildQueryNodeByFieldAndTerms(field, terms);
};

%%

namespace infinity{
void SearchParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity

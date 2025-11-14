// Bison variant of [Lucene syntax](https://github.com/apache/lucene/blob/main/lucene/queryparser/src/java/org/apache/lucene/queryparser/flexible/standard/parser/StandardSyntaxParser.jj)
// ElasticSearch query string examples: https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
// Infinity query string examples: https://github.com/infiniflow/infinity/blob/main/src/unit_test/parser/search_driver.cpp

%language "c++"
%skeleton "lalr1.cc"
%require "3.5"
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
    #include <memory>
    #include <string>

    // Import modules to get complete type definitions
    import infinity_core;

    namespace infinity {
        // Forward declaration for SearchScanner (defined in .h file)
        class SearchScanner;

        class InfString {
        public:
            // Bison requires default constructor for inplace new
            InfString() = default;
            InfString(std::string text, bool from_quoted) : text_{std::move(text)}, from_quoted_{from_quoted} {}
            std::string text_{};
            bool from_quoted_{};
        };
    }
}

%parse-param {SearchScanner &scanner} {const SearchDriver &driver} {const std::string &default_field} {std::unique_ptr<QueryNode> &parse_result}

/* code snippet copied to generated search_parser.tab.cc */
%code{
    #include <iostream>
    #include <sstream>
    #include <string>
    #include <vector>

    // Import modules for implementation
    import infinity_core;
    import search_scanner;

    #undef yylex
    #define yylex scanner.yylex

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
%token <unsigned long> TILDE
%token <float>         CARAT
%token <InfString>     STRING

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
    parse_result = std::move($1);
};

query
: clause { $$ = std::move($1); }
| query clause {
    assert(driver.operator_option_ == FulltextQueryOperatorOption::kInfinitySyntax);
    if (!($1)) {
        $$ = std::move($2);
    } else if (!($2)) {
        $$ = std::move($1);
    } else {
        auto q = std::make_unique<OrQueryNode>();
        q->Add(std::move($1));
        q->Add(std::move($2));
        $$ = std::move(q);
    }
}
| query OR clause {
    if (!($1)) {
        $$ = std::move($3);
    } else if (!($3)) {
        $$ = std::move($1);
    } else {
        auto q = std::make_unique<OrQueryNode>();
        q->Add(std::move($1));
        q->Add(std::move($3));
        $$ = std::move(q);
    }
};

clause
: term { $$ = std::move($1); }
| clause AND term {
    if (!($1)) {
        $$ = std::move($3);
    } else if (!($3)) {
        $$ = std::move($1);
    } else {
        auto query = std::make_unique<AndQueryNode>();
        query->Add(std::move($1));
        query->Add(std::move($3));
        $$ = std::move(query);
    }
};

term
: basic_filter_boost { $$ = std::move($1); }
| NOT term {
    if (!($2)) {
        $$ = nullptr;
    } else {
        auto query = std::make_unique<NotQueryNode>();
        query->Add(std::move($2));
        $$ = std::move(query);
    }
}
| LPAREN query RPAREN { $$ = std::move($2); }
| LPAREN query RPAREN CARAT {
    $$ = std::move($2);
    if ($$) {
        $$->MultiplyWeight($4);
    }
};

basic_filter_boost
: basic_filter {
    $$ = std::move($1);
}
| basic_filter CARAT {
    $$ = std::move($1);
    if ($$) {
        $$->MultiplyWeight($2);
    }
};

basic_filter
: STRING {
    const std::string &field = default_field;
    if(field.empty()){
        error(@1, "default_field is empty");
        YYERROR;
    }
    std::string text = SearchDriver::Unescape($1.text_);
    $$ = driver.AnalyzeAndBuildQueryNode(field, text, $1.from_quoted_);
}
| STRING OP_COLON STRING {
    std::string field = SearchDriver::Unescape($1.text_);
    std::string text = SearchDriver::Unescape($3.text_);
    $$ = driver.AnalyzeAndBuildQueryNode(field, text, $3.from_quoted_);
};
| STRING TILDE {
    const std::string &field = default_field;
    if(field.empty()){
        error(@1, "default_field is empty");
        YYERROR;
    }
    std::string text = SearchDriver::Unescape($1.text_);
    $$ = driver.AnalyzeAndBuildQueryNode(field, text, $1.from_quoted_, $2);
}
| STRING OP_COLON STRING TILDE {
    std::string field = SearchDriver::Unescape($1.text_);
    std::string text = SearchDriver::Unescape($3.text_);
    $$ = driver.AnalyzeAndBuildQueryNode(field, text, $3.from_quoted_, $4);
};

%%

namespace infinity{
void SearchParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity

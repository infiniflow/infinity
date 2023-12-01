
// Bison variant of [Lucene syntax](https://github.com/apache/lucene/blob/main/lucene/queryparser/src/java/org/apache/lucene/queryparser/flexible/standard/parser/StandardSyntaxParser.jj)

%language "c++"
%skeleton "lalr1.cc"
%require "3.8"
%debug
%defines
%define api.namespace {infinity}
%define api.parser.class {QueryParser}
%define api.value.type variant
%define api.token.raw
%define parse.assert
%define parse.error verbose

/* code snippet copied to generated query_parser.tab.hh */
%code requires{
    // unique_ptr<irs::filter> requires sizeof(irs::filter)
    #include "search/filter.hpp"

    namespace infinity {
        class QueryDriver;
        class QueryScanner;
    }
}

%parse-param {QueryScanner &scanner} {QueryDriver &driver}

/* code snippet copied to generated query_parser.tab.cc */
%code{
    #include "query_driver.h"
    #include "query_scanner.h"

    #undef yylex
    #define yylex scanner.yylex

    #include <iostream>
    #include <sstream>
    #include <string>
    #include "search/all_filter.hpp"
    #include "search/boolean_filter.hpp"
    #include "search/column_existence_filter.hpp"
    #include "search/granular_range_filter.hpp"
    #include "search/levenshtein_filter.hpp"
    #include "search/nested_filter.hpp"
    #include "search/ngram_similarity_filter.hpp"
    #include "search/phrase_filter.hpp"
    #include "search/prefix_filter.hpp"
    #include "search/proxy_filter.hpp"
    #include "search/range_filter.hpp"
    #include "search/same_position_filter.hpp"
    #include "search/term_filter.hpp"
    #include "search/terms_filter.hpp"
    #include "search/wildcard_filter.hpp"

    // Convert basic_string<char> to basic_string<unsigned char>
    static irs::bstring toBstring(const std::string &str) {
        irs::bstring converted_str((uint8_t *)str.c_str(), str.length());
        return converted_str;
    }
}

/* terminal symbol */
%token                 END    0     "end of file"
%token                 AND
%token                 OR
%token                 NOT
%token                 FN_PREFIX
%token                 LPAREN
%token                 RPAREN
%token                 OP_COLON
%token                 OP_EQUAL
%token                 OP_LESSTHAN
%token                 OP_LESSTHANEQ
%token                 OP_MORETHAN
%token                 OP_MORETHANEQ
%token                 RANGEIN_START
%token                 RANGEIN_END
%token                 RANGEEX_START
%token                 RANGEEX_END
%token                 RANGE_TO
%token <float>         CARAT
%token <int>           TILDE
%token <std::string> REGEXPTERM
%token <std::string> STRING

/* nonterminal symbol */
%type <std::unique_ptr<irs::filter>>  topLevelQuery query clause term basic_filter_boost basic_filter rangeExpr regexExpr fuzzyExpr
%type <int>                           opCompare rangeStart rangeEnd 
%type <std::string>                   boundVal

%locations

%%

// TopLevelQuery ::= Query <EOF>
// Query ::= DisjQuery ( DisjQuery )*
// DisjQuery ::= ConjQuery ( OR ConjQuery )*
// ConjQuery ::= ModClause ( AND ModClause )*
// ModClause ::= (Modifier)? Clause
// Modifier ::= <PLUS> | (<MINUS> | <NOT>)
topLevelQuery
: query END {
    driver.result = std::move($1);
};

query
: clause { $$ = std::move($1); }
| query clause {
    auto query = std::make_unique<irs::Or>();
    irs::filter::ptr fptr = std::move($1);
    query->add(std::move(fptr));
    fptr = std::move($2);
    query->add(std::move(fptr));
    $$ = std::move(query);
}
| query OR clause {
    auto query = std::make_unique<irs::Or>();
    irs::filter::ptr fptr = std::move($1);
    query->add(std::move(fptr));
    fptr = std::move($3);
    query->add(std::move(fptr));
    $$ = std::move(query);
};

clause
: term { $$ = std::move($1); }
| clause AND term {
    auto query = std::make_unique<irs::And>();
    irs::filter::ptr fptr = std::move($1);
    query->add(std::move(fptr));
    fptr = std::move($3);
    query->add(std::move(fptr));
    $$ = std::move(query);
};

term
: basic_filter_boost { $$ = std::move($1); }
| NOT term {
    auto query = std::make_unique<irs::And>();
    irs::filter::ptr fptr = std::move($2);
    query->add<irs::Not>().set_filter(std::move(fptr));
    $$ = std::move(query);
}
| LPAREN query RPAREN { $$ = std::move($2); }
| LPAREN query RPAREN CARAT {
    float new_boost = $2->boost() * $4;
    $2->boost(new_boost);
    $$ = std::move($2);
};

basic_filter_boost
: basic_filter {
    $$ = std::move($1);
}
| basic_filter CARAT {
    float new_boost = $1->boost() * $2;
    $1->boost(new_boost);
    $$ = std::move($1);
};

basic_filter
: STRING {
    std::string &field = driver.default_field;
    if(field.empty()){
        error(@1, "driver.default_field is empty");
        YYERROR;
    }
    std::vector<std::string> terms;
    driver.Analyze(field, $1, terms);
    if(terms.size()==1){
        auto query = std::make_unique<irs::by_term>();
        query->mutable_options()->term = toBstring(terms[0]);
        *query->mutable_field() = field;
        $$ = std::move(query);
    } else {
        auto query = std::make_unique<irs::Or>();
        for(size_t i=0; i<terms.size(); i++){
            auto subquery = std::make_unique<irs::by_term>();
            subquery->mutable_options()->term = toBstring(terms[i]);
            *subquery->mutable_field() = field;
            query->add(std::move(subquery));
        }
        $$ = std::move(query);
    }
}
| rangeExpr { $$ = std::move($1); }
| regexExpr { $$ = std::move($1); }
| fuzzyExpr { $$ = std::move($1); }
| STRING OP_COLON STRING {
    std::vector<std::string> terms;
    driver.Analyze($1, $3, terms);
    std::string &field = $1;
    if(terms.size()==1){
        auto query = std::make_unique<irs::by_term>();
        query->mutable_options()->term = toBstring(terms[0]);
        *query->mutable_field() = field;
        $$ = std::move(query);
    } else {
        auto query = std::make_unique<irs::Or>();
        for(size_t i=0; i<terms.size(); i++){
            auto subquery = std::make_unique<irs::by_term>();
            subquery->mutable_options()->term = toBstring(terms[i]);
            *subquery->mutable_field() = field;
            query->add(std::move(subquery));
        }
        $$ = std::move(query);
    }
}
| STRING OP_COLON rangeExpr {
    *dynamic_cast<irs::by_range*>($3.get())->mutable_field() = $1;
    $$ = std::move($3);
}
| STRING OP_COLON regexExpr {
    *dynamic_cast<irs::by_wildcard*>($3.get())->mutable_field() = $1;
    $$ = std::move($3);
}
| STRING OP_COLON fuzzyExpr {
    *dynamic_cast<irs::by_edit_distance*>($3.get())->mutable_field() = $1;
    $$ = std::move($3);
};

rangeExpr
: opCompare STRING {
    auto query = std::make_unique<irs::by_range>();
    auto &range = query->mutable_options()->range;
    switch($1){
        case int(token::OP_EQUAL):
            range.min = toBstring($2);
            range.max = toBstring($2);
            range.min_type = irs::BoundType::INCLUSIVE;
            range.max_type = irs::BoundType::INCLUSIVE;
            break;
        case int(token::OP_LESSTHAN):
            range.max = toBstring($2);
            range.max_type = irs::BoundType::EXCLUSIVE;
            break;
        case int(token::OP_MORETHAN):
            range.min = toBstring($2);
            range.min_type = irs::BoundType::EXCLUSIVE;
            break;
        case int(token::OP_LESSTHANEQ):
            range.max = toBstring($2);
            range.max_type = irs::BoundType::INCLUSIVE;
            break;
        case int(token::OP_MORETHANEQ):
            range.min = toBstring($2);
            range.min_type = irs::BoundType::INCLUSIVE;
            break;
        default:
            YYERROR;
    }
    $$ = std::move(query);
}
| rangeStart boundVal RANGE_TO boundVal rangeEnd {
    const std::string &beg = $2;
    const std::string &end = $4;
    auto query = std::make_unique<irs::by_range>();
    auto &range = query->mutable_options()->range;
    range.min = toBstring(beg);
    range.max = toBstring(end);
    if(beg.length()>0){
        if($1 == token::RANGEIN_START){
            range.min_type = irs::BoundType::INCLUSIVE;
        } else {
            range.min_type = irs::BoundType::EXCLUSIVE;
        }
    }
    if(end.length()>0){
        if($5 == token::RANGEIN_END){
            range.max_type = irs::BoundType::INCLUSIVE;
        } else {
            range.max_type = irs::BoundType::EXCLUSIVE;
        }
    }
    $$ = std::move(query);
};

opCompare
: OP_EQUAL { $$ = int(token::OP_EQUAL); }
| OP_LESSTHAN { $$ = int(token::OP_LESSTHAN); }
| OP_MORETHAN { $$ = int(token::OP_MORETHAN); }
| OP_LESSTHANEQ { $$ = int(token::OP_LESSTHANEQ); }
| OP_MORETHANEQ { $$ = int(token::OP_MORETHANEQ); }
;

rangeStart
: RANGEIN_START { $$ = int(token::RANGEIN_START); }
| RANGEEX_START { $$ = int(token::RANGEEX_START); }
;

rangeEnd
: RANGEIN_END { $$ = int(token::RANGEIN_END); }
| RANGEEX_END { $$ = int(token::RANGEEX_END); }
;

boundVal
: STRING {
    $$ = $1;
}
|{
    $$ = std::string("");
};

regexExpr
: REGEXPTERM {
    auto query = std::make_unique<irs::by_wildcard>();
    size_t leng = $1.length();
    query->mutable_options()->term = toBstring($1.substr(1, leng-2));
    $$ = std::move(query);
};

fuzzyExpr
: STRING TILDE {
    auto query = std::make_unique<irs::by_edit_distance>();
    auto* opts = query->mutable_options();
    opts->max_terms = 50;  // same as Lucene by default
    opts->max_distance = $2;
    opts->term = toBstring($1);
    $$ = std::move(query);
};

%%

namespace infinity{
void QueryParser::error(const location_type &l, const std::string &err_message) {
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
} //namespace infinity

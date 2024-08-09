%{
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

/* Implementation of yyFlexScanner */
#define SearchScannerDerived SearchScannerPlain
#include "search_scanner_derived.h"
#undef SearchScannerDerived
#undef  YY_DECL
#define YY_DECL int infinity::SearchScannerPlain::yylex(infinity::SearchParser::semantic_type * const lval, infinity::SearchParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = infinity::SearchParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option c++
%option yyclass="infinity::SearchScannerPlain"
%option noyywrap nounput batch debug noinput
%option prefix="SearchScannerPlain"
%option warn
%option never-interactive

ASC     [\x00-\x7f]
ASCN    [\x00-\t\v-\x7f]
U       [\x80-\xbf]
U2      [\xc2-\xdf]
U3      [\xe0-\xef]
U4      [\xf0-\xf4]
UANY    {ASC}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}
UANYN   {ASCN}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}
UONLY   {U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

-?[0-9]+("."[0-9]*)? |
-?"."[0-9]+ |
([a-zA-Z0-9_]|{UONLY})+        { yylval->build<InfString>(InfString(yytext, false)); return token::STRING; } // https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode

.|\n        /* ignore any other character */;

%%

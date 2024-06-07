%{
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

/* Implementation of yyFlexScanner */ 
#include "search_scanner.h"
#undef  YY_DECL
#define YY_DECL int infinity::SearchScanner::yylex(infinity::SearchParser::semantic_type * const lval, infinity::SearchParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = infinity::SearchParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

/* for temporary storage of quoted string */
static thread_local std::stringstream string_buffer;

%}

%option c++
%option yyclass="infinity::SearchScanner"
%option noyywrap nounput batch debug noinput
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

ESCAPABLE [\x20+\-=&|!(){}\[\]^"~*?:\\/]
ESCAPED    \\{ESCAPABLE}

%x SINGLE_QUOTED_STRING
%x DOUBLE_QUOTED_STRING

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;

            /* Note: special characters in pattern shall be double-quoted or escaped with backslash: " <^.+|/()[]{}" */
%}

[ \t\n]+        /* ignore \t\n and space */;

AND |
&& |
"+"      { return token::AND; }

OR |
"||"      { return token::OR; }

NOT |
! |
-        { return token::NOT; }

"("     { return token::LPAREN; }

")"     { return token::RPAREN; }

:        { return token::OP_COLON; }

"^"[0-9]+("."[0-9]*)? |
"^."[0-9]+         { yylval->build(std::strtof(yytext+1, NULL)); return token::CARAT; }

-?[0-9]+("."[0-9]*)? |
-?"."[0-9]+ |
([a-zA-Z0-9_]|{UONLY}|{ESCAPED})+        { yylval->build<InfString>(InfString(yytext, false)); return token::STRING; }  // https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode

\'                            { BEGIN SINGLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<SINGLE_QUOTED_STRING>\'\'    { string_buffer << '\''; }
<SINGLE_QUOTED_STRING>[^']*   { string_buffer << yytext; }
<SINGLE_QUOTED_STRING>\'      { BEGIN INITIAL; yylval->build<InfString>(InfString(string_buffer.str(), true)); return token::STRING; }
<SINGLE_QUOTED_STRING><<EOF>> { std::cerr << "[Lucene-Lexer-Error] Unterminated string" << std::endl; return 0; }

\"                            { BEGIN DOUBLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<DOUBLE_QUOTED_STRING>\"\"    { string_buffer << '\"'; }
<DOUBLE_QUOTED_STRING>[^"]*   { string_buffer << yytext; }
<DOUBLE_QUOTED_STRING>\"      { BEGIN INITIAL; yylval->build<InfString>(InfString(string_buffer.str(), true)); return token::STRING; }
<DOUBLE_QUOTED_STRING><<EOF>> { std::cerr << "[Lucene-Lexer-Error] Unterminated string" << std::endl; return 0; }

%%

%{
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

/* Implementation of yyFlexScanner */ 
#include "query_scanner.h"
#undef  YY_DECL
#define YY_DECL int infinity::QueryScanner::yylex(infinity::QueryParser::semantic_type * const lval, infinity::QueryParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = infinity::QueryParser::token;

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
%option yyclass="infinity::QueryScanner"
%option noyywrap nounput batch debug noinput
%option warn
%option never-interactive

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

fn:        { return token::FN_PREFIX; }

"("     { return token::LPAREN; }

")"     { return token::RPAREN; }

:        { return token::OP_COLON; }

=        { return token::OP_EQUAL; }

"<"        { return token::OP_LESSTHAN; }

"<="       { return token::OP_LESSTHANEQ; }

>        { return token::OP_MORETHAN; }

>=        { return token::OP_MORETHANEQ; }

"["        { return token::RANGEIN_START; }

"]"        { return token::RANGEIN_END; }

"{"        { return token::RANGEEX_START; }

"}"        { return token::RANGEEX_END; }

TO        { return token::RANGE_TO; }

"/"([^"/"]*)"/"        {
               /**
                * Section 10.1.7.1 of the 3.8.2 Bison Manual says the
                * following should work:
                * yylval.emplace(yytext);
                * but it doesn't.
                * ref: http://goo.gl/KLn0w2
                */
                yylval->build<std::string>(yytext); return token::REGEXPTERM; }

~[0-9]*   {
            int fuzzy = 1;
            if(yytext[1]!='\0'){
                fuzzy = std::strtol(yytext+1, NULL, 10);
            }
            yylval->build(fuzzy);
            return token::TILDE;
        }

"^"[0-9]+("."[0-9]*)? |
"^."[0-9]+         { yylval->build(std::strtof(yytext+1, NULL)); return token::CARAT; }

-?[0-9]+("."[0-9]*)? |
-?"."[0-9]+ |
[a-zA-Z0-9_]+        { yylval->build<std::string>(yytext); return token::STRING; }

\'                            { BEGIN SINGLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<SINGLE_QUOTED_STRING>\'\'    { string_buffer << '\''; }
<SINGLE_QUOTED_STRING>[^']*   { string_buffer << yytext; }
<SINGLE_QUOTED_STRING>\'      { BEGIN INITIAL; yylval->build<std::string>(string_buffer.str()); return token::STRING; }
<SINGLE_QUOTED_STRING><<EOF>> { std::cerr << "[Lucene-Lexer-Error] Unterminated string" << std::endl; return 0; }

\"                            { BEGIN DOUBLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<DOUBLE_QUOTED_STRING>\"\"    { string_buffer << '\"'; }
<DOUBLE_QUOTED_STRING>[^"]*   { string_buffer << yytext; }
<DOUBLE_QUOTED_STRING>\"      { BEGIN INITIAL; yylval->build<std::string>(string_buffer.str()); return token::STRING; }
<DOUBLE_QUOTED_STRING><<EOF>> { std::cerr << "[Lucene-Lexer-Error] Unterminated string" << std::endl; return 0; }

%%

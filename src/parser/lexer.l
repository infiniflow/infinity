%{

#include "parser.h"
#include "parser_result.h"
#include <sstream>

static thread_local std::stringstream string_buffer;

%}

%option reentrant
%option bison-bridge

%option noyywrap nounput
%option warn
%option never-interactive
%option batch
%option case-insensitive
%option bison-locations
%option verbose
%option debug

%option prefix="sql"

%x SINGLE_QUOTED_STRING

%%

[ \t\n]+        /* ignore \t\n and space */;

ALTER           { return ALTER; }
COLLECTION      { return COLLECTION; }
CREATE          { return CREATE; }
COPY            { return COPY; }
DROP            { return DROP; }
DELETE          { return DELETE; }
DELIMITER       { return DELIMITER; }
DESCRIBE        { return DESCRIBE; }
EXECUTE         { return EXECUTE; }
EXISTS          { return EXISTS; }
EXPLAIN         { return EXPLAIN; }
FORMAT          { return FORMAT; }
FROM            { return FROM; }
HEADER          { return HEADER; }
IF              { return IF; }
INSERT          { return INSERT; }
NOT             { return NOT; }
PREPARE         { return PREPARE; }
SCHEMA          { return SCHEMA; }
SELECT          { return SELECT; }
SET             { return SET; }
SHOW            { return SHOW; }
TO              { return TO; }
TABLE           { return TABLE; }
TABLES          { return TABLES; }
UPDATE          { return UPDATE; }
WITH            { return WITH; }

[-+*/(){},.;<>=^%:?[\]|]    { return yytext[0]; }

[0-9]+"."[0-9]* |
"."[0-9]*  {
    yylval->double_value = atof(yytext);
    return DOUBLE_VALUE;
}

[0-9]+ {
    errno = 0;
    yylval->long_value = strtoll(yytext, nullptr, 0);
    if (errno) {
        return fprintf(stderr, "[SQL-Lexer-Error] Integer cannot be parsed - is it out of range?");
        return 0;
    }
    return LONG_VALUE;
}

[A-Za-z][A-Za-z0-9_]* {
    yylval->str_value = yytext;
    return IDENTIFIER;
}

\'                            { BEGIN SINGLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<SINGLE_QUOTED_STRING>\'\'    { string_buffer << '\''; }
<SINGLE_QUOTED_STRING>[^']*   { string_buffer << yytext; }
<SINGLE_QUOTED_STRING>\'      { BEGIN INITIAL; yylval->str_value = strdup(string_buffer.str().c_str()); return STRING; }
<SINGLE_QUOTED_STRING><<EOF>> { fprintf(stderr, "[SQL-Lexer-Error] Unterminated string\n"); return 0; }

. { fprintf(stderr, "[SQL-Lexer-Error] Unknown Character: %c\n", yytext[0]); return 0; }

%%

int yyerror(const char *msg) {
    fprintf(stderr, "[Why here?] %s\n",msg); return 0;
}

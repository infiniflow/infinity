%{

#include "expression_parser.h"
#include "expression_parser_result.h"
#include <sstream>

static thread_local std::stringstream string_buffer;

%}

%option reentrant
%option bison-bridge

%option noyywrap nounput noinput
%option warn
%option never-interactive
%option batch
%option case-insensitive
%option bison-locations
%option verbose
%option debug

%option prefix="expression"

%x SINGLE_QUOTED_STRING

%%

[ \t\n]+        /* ignore \t\n and space */;

AND             { return AND; }
AS              { return AS; }
BETWEEN         { return BETWEEN; }
CAST            { return CAST; }
DISTINCT        { return DISTINCT; }
EXTRACT         { return EXTRACT; }
FALSE           { return FALSE; }
FROM            { return FROM; }
FUSION          { return FUSION; }
IN              { return IN; }
INTERVAL        { return INTERVAL; }
IS              { return IS; }
LIKE            { return LIKE; }
MATCH           { return MATCH; }
NOT             { return NOT; }
NULL            { return NULLABLE; }
OR              { return OR; }
QUERY           { return QUERY; }
TRUE            { return TRUE; }

"=="            { return EQUAL; }
"!="            { return NOT_EQ; }
"<>"            { return NOT_EQ; }
"<="            { return LESS_EQ; }
">="            { return GREATER_EQ; }

[-+*/(){},.;<>=^%:?[\]|]    { return yytext[0]; }

-?[0-9]+"."[0-9]* |
"."[0-9]*  {
    yylval->double_value = atof(yytext);
    return DOUBLE_VALUE;
}

-?[0-9]+ {
    errno = 0;
    yylval->long_value = strtoll(yytext, nullptr, 0);
    if (errno) {
        return fprintf(stderr, "[EXPRESSION-Lexer-Error] Integer cannot be parsed - is it out of range?");
        return 0;
    }
    return LONG_VALUE;
}

\"[^\"\n]+\" {
    // total length - 2 of quota + 1 null char
    long str_len = strlen(yytext) - 1;
    yylval->str_value = (char*)malloc(str_len);
    memset(yylval->str_value, 0, str_len);
    memcpy(yylval->str_value, (char*)(yytext + 1), str_len - 1);
    return IDENTIFIER;
}

[A-Za-z][A-Za-z0-9_]* {
    yylval->str_value = strdup(yytext);
    return IDENTIFIER;
}

\'                            { BEGIN SINGLE_QUOTED_STRING; string_buffer.clear(); string_buffer.str(""); }  // Clear strbuf manually, see #170
<SINGLE_QUOTED_STRING>\'\'    { string_buffer << '\''; }
<SINGLE_QUOTED_STRING>[^']*   { string_buffer << yytext; }
<SINGLE_QUOTED_STRING>\'      { BEGIN INITIAL; yylval->str_value = strdup(string_buffer.str().c_str()); return STRING; }
<SINGLE_QUOTED_STRING><<EOF>> { fprintf(stderr, "[EXPRESSION-Lexer-Error] Unterminated string\n"); return 0; }

. { fprintf(stderr, "[EXPRESSION-Lexer-Error] Unknown Character: %c\n", yytext[0]); return 0; }

%%

int expressionerror(const char *msg) {
    fprintf(stderr, "[Why here?] %s\n",msg); return 0;
}

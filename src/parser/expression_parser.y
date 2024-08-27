
%{
#include <stdint.h>
#include <stdio.h>
#include "expression_parser.h"
#include "expression_lexer.h"
#include "parser_helper.h"

void expressionerror(YYLTYPE * llocp, void* lexer, infinity::ExpressionParserResult* result, const char* msg);
%}

%code requires {

#include "expression.h"
#include "expression_parser_result.h"
#include "definition/column_def.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"

#include <vector>

#define YYSTYPE EXPRESSIONSTYPE
#define YYLTYPE EXPRESSIONLTYPE

struct EXPRESSION_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;
    int string_length;

    std::vector<void*> parameters;
};

#define EXPRESSIONLTYPE EXPRESSION_LTYPE
#define EXPRESSIONLTYPE_IS_DECLARED 1

#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line;      \
    yylloc->first_column = yylloc->last_column;  \
    for(int i = 0; yytext[i] != '\0'; ++ i) {   \
        yylloc->total_column++;                   \
        yylloc->string_length++;                  \
        if (yytext[i] == '\n') {                  \
            yylloc->last_line++;                    \
            yylloc->last_column = 0;                \
        } else {                                  \
            yylloc->last_column++;                  \
        }                                         \
    }
}


%define api.pure full

%define parse.trace
%define parse.error verbose

%define api.prefix {expression}

%lex-param {void *scanner}
%parse-param {void *scanner} {infinity::ExpressionParserResult* result}

%locations
%initial-action {
  // Initialize
  @$.first_column = 0;
  @$.last_column = 0;
  @$.first_line = 0;
  @$.last_line = 0;
  @$.total_column = 0;
  @$.string_length = 0;
};

%code{
// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
}

%union {
    bool    bool_value;
    char*   str_value;
    double  double_value;
    int64_t long_value;

    infinity::ColumnType              column_type_t;

    infinity::ParsedExpr*             expr_t;
    infinity::ConstantExpr*           const_expr_t;
    std::vector<infinity::ParsedExpr*>*    expr_array_t;
}

%destructor {
    fprintf(stderr, "destroy expression array\n");
    if (($$) != nullptr) {
        for (auto ptr : *($$)) {
            delete ptr;
        }
        delete ($$);
    }
} <expr_array_t>

%destructor {
    delete ($$);
} <expr_t>

%destructor {
    delete ($$);
} <const_expr_t>

%token <str_value>              IDENTIFIER STRING
%token <double_value>           DOUBLE_VALUE
%token <long_value>             LONG_VALUE

/* EXPRESSION keywords */

%token CREATE SELECT INSERT DROP UPDATE DELETE COPY SET EXPLAIN SHOW ALTER EXECUTE PREPARE UNION ALL INTERSECT COMPACT
%token EXCEPT FLUSH USE OPTIMIZE PROPERTIES
%token DATABASE TABLE COLLECTION TABLES INTO VALUES AST PIPELINE RAW LOGICAL PHYSICAL FRAGMENT VIEW INDEX ANALYZE VIEWS DATABASES SEGMENT SEGMENTS BLOCK BLOCKS COLUMNS INDEXES
%token GROUP BY HAVING AS NATURAL JOIN LEFT RIGHT OUTER FULL ON INNER CROSS DISTINCT WHERE ORDER LIMIT OFFSET ASC DESC
%token IF NOT EXISTS IN FROM TO WITH DELIMITER FORMAT HEADER CAST END CASE ELSE THEN WHEN
%token BOOLEAN INTEGER INT TINYINT SMALLINT BIGINT HUGEINT VARCHAR FLOAT DOUBLE REAL DECIMAL DATE TIME DATETIME
%token TIMESTAMP UUID POINT LINE LSEG BOX PATH POLYGON CIRCLE BLOB BITMAP EMBEDDING VECTOR BIT SPARSE
%token PRIMARY KEY UNIQUE NULLABLE IS
%token TRUE FALSE INTERVAL SECOND SECONDS MINUTE MINUTES HOUR HOURS DAY DAYS MONTH MONTHS YEAR YEARS
%token EQUAL NOT_EQ LESS_EQ GREATER_EQ BETWEEN AND OR EXTRACT LIKE
%token DATA LOG BUFFER
%token KNN USING SESSION GLOBAL OFF EXPORT PROFILE CONFIGS PROFILES STATUS VAR
%token SEARCH MATCH QUERY FUSION

%token NUMBER

/* nonterminal symbol */

%type <expr_t>                  expr expr_alias column_expr function_expr operand
%type <expr_t>                  in_expr between_expr
%type <expr_t>                  conjunction_expr cast_expr
%type <expr_t>                  match_expr query_expr fusion_expr
%type <const_expr_t>            constant_expr interval_expr
%type <const_expr_t>            long_array_expr unclosed_long_array_expr double_array_expr unclosed_double_array_expr
%type <expr_array_t>            expr_array
%type <column_type_t>           column_type

/*
 * Operator precedence, low to high
 */

%left       OR
%left       AND
%left       BETWEEN CASE WHEN THEN ELSE
%right      NOT

%nonassoc   '=' EQUAL NOT_EQ LIKE
%nonassoc   '<' '>' LESS_EQ GREATER_EQ

%nonassoc   IS
%left       '+' '-'
%left       '*' '/' '%'

%left       '[' ']'
%left       '(' ')'
%left       '.'

%%

input_pattern : expr_array {
    result->exprs_ptr_ = $1;
};

/*
 * EXPRESSION
 */

expr_array : expr_alias {
    $$ = new std::vector<infinity::ParsedExpr*>();
    $$->emplace_back($1);
}
| expr_array ',' expr_alias {
    $1->emplace_back($3);
    $$ = $1;
};

expr_alias : expr AS IDENTIFIER {
    $$ = $1;
    ParserHelper::ToLower($3);
    $$->alias_ = $3;
    free($3);
}
| expr {
    $$ = $1;
};

expr : operand
| in_expr
| between_expr
| conjunction_expr;

operand: '(' expr ')' {
   $$ = $2;
}
| constant_expr {
    $$ = $1;
}
| column_expr
| function_expr
| cast_expr
| match_expr
| query_expr
| fusion_expr

match_expr : MATCH '(' STRING ',' STRING ')' {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string($3);
    match_expr->matching_text_ = std::string($5);
    free($3);
    free($5);
    $$ = match_expr;
}
| MATCH '(' STRING ',' STRING ',' STRING ')' {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string($3);
    match_expr->matching_text_ = std::string($5);
    match_expr->options_text_ = std::string($7);
    free($3);
    free($5);
    free($7);
    $$ = match_expr;
}

query_expr : QUERY '(' STRING ')' {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string($3);
    free($3);
    $$ = match_expr;
}
| QUERY '(' STRING ',' STRING ')' {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string($3);
    match_expr->options_text_ = std::string($5);
    free($3);
    free($5);
    $$ = match_expr;
}

fusion_expr : FUSION '(' STRING ')' {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string($3);
    free($3);
    $$ = fusion_expr;
}
| FUSION '(' STRING ',' STRING ')' {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string($3);
    free($3);
    fusion_expr->SetOptions($5);
    free($5);
    $$ = fusion_expr;
}

function_expr : IDENTIFIER '(' ')' {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = nullptr;
    $$ = func_expr;
}
| IDENTIFIER '(' expr_array ')' {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = $3;
    $$ = func_expr;
}
| IDENTIFIER '(' DISTINCT expr_array ')' {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower($1);
    func_expr->func_name_ = $1;
    free($1);
    func_expr->arguments_ = $4;
    func_expr->distinct_ = true;
    $$ = func_expr;
}
| operand IS NOT NULLABLE {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
}
| operand IS NULLABLE {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    $$ = func_expr;
}
| NOT operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| '-' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| '+' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($2);
    $$ = func_expr;
}
| operand '-' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '+' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '*' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '/' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '%' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '=' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand EQUAL operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand NOT_EQ operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '<' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand '>' operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand LESS_EQ operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand GREATER_EQ operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| EXTRACT '(' STRING FROM operand ')' {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower($3);
    if(strcmp($3, "year") == 0) {
        func_expr->func_name_ = "extract_year";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp($3, "month") == 0) {
        func_expr->func_name_ = "extract_month";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp($3, "day") == 0) {
        func_expr->func_name_ = "extract_day";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp($3, "hour") == 0) {
        func_expr->func_name_ = "extract_hour";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp($3, "minute") == 0) {
        func_expr->func_name_ = "extract_minute";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp($3, "second") == 0) {
        func_expr->func_name_ = "extract_second";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else {
        delete func_expr;
        expressionerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    free($3);
    func_expr->arguments_->emplace_back($5);
    $$ = func_expr;
}
| operand LIKE operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| operand NOT LIKE operand {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($4);
    $$ = func_expr;
};

conjunction_expr: expr AND expr {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
}
| expr OR expr {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back($1);
    func_expr->arguments_->emplace_back($3);
    $$ = func_expr;
};

between_expr: operand BETWEEN operand AND operand {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = $1;
    between_expr->lower_bound_ = $3;
    between_expr->upper_bound_ = $5;
    $$ = between_expr;
}

in_expr: operand IN '(' expr_array ')' {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = $1;
    in_expr->arguments_ = $4;
    $$ = in_expr;
}
| operand NOT IN '(' expr_array ')' {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = $1;
    in_expr->arguments_ = $5;
    $$ = in_expr;
};

column_type :
BOOLEAN { $$ = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| TINYINT { $$ = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| SMALLINT { $$ = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| INTEGER { $$ = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| INT { $$ = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| BIGINT { $$ = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| HUGEINT { $$ = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| FLOAT { $$ = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| REAL  { $$ = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| DOUBLE { $$ = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| DATE { $$ = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| TIME { $$ = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| DATETIME { $$ = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| TIMESTAMP { $$ = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| UUID { $$ = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| POINT { $$ = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| LINE { $$ = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| LSEG { $$ = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| BOX { $$ = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
// | PATH { $$ = infinity::ColumnType{infinity::LogicalType::kPath, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
// | POLYGON { $$ = infinity::ColumnType{infinity::LogicalType::kPolygon, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| CIRCLE { $$ = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
// Variable types
| VARCHAR { $$ = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, $3, $5, infinity::EmbeddingDataType::kElemInvalid}; }
| DECIMAL '(' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, $3, 0, infinity::EmbeddingDataType::kElemInvalid}; }
| DECIMAL { $$ = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
// | BLOB '(' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kBlob, $3}; }
// | BITMAP '(' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kBitmap, $3}; }
| EMBEDDING '(' BIT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemBit}; }
| EMBEDDING '(' TINYINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
| EMBEDDING '(' SMALLINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
| EMBEDDING '(' INTEGER ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| EMBEDDING '(' INT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| EMBEDDING '(' BIGINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
| EMBEDDING '(' FLOAT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
| EMBEDDING '(' DOUBLE ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
| VECTOR '(' BIT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemBit}; }
| VECTOR '(' TINYINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
| VECTOR '(' SMALLINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
| VECTOR '(' INTEGER ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| VECTOR '(' INT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| VECTOR '(' BIGINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
| VECTOR '(' FLOAT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
| VECTOR '(' DOUBLE ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kEmbedding, $5, 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
| SPARSE '(' BIT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemBit}; }
| SPARSE '(' TINYINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
| SPARSE '(' SMALLINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
| SPARSE '(' INTEGER ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| SPARSE '(' INT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
| SPARSE '(' BIGINT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
| SPARSE '(' FLOAT ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
| SPARSE '(' DOUBLE ',' LONG_VALUE ')' { $$ = infinity::ColumnType{infinity::LogicalType::kSparse, $5, 0, 0, infinity::EmbeddingDataType::kElemDouble}; }

cast_expr: CAST '(' expr AS column_type ')' {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch($5.logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make($5.precision, $5.scale);
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($5.width);
//            break;
//        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make($5.embedding_type_, $5.width);
            break;
        }
        default: {
            break;
        }
    }
    infinity::CastExpr* cast_expr = new infinity::CastExpr($5.logical_type_, type_info_ptr);
    cast_expr->expr_ = $3;
    $$ = cast_expr;
};

column_expr : IDENTIFIER {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower($1);
    column_expr->names_.emplace_back($1);
    free($1);
    $$ = column_expr;
}
| column_expr '.' IDENTIFIER {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)$1;
    ParserHelper::ToLower($3);
    column_expr->names_.emplace_back($3);
    free($3);
    $$ = column_expr;
}
| '*' {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    $$ = column_expr;
}
| column_expr '.' '*' {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)$1;
    if(column_expr->star_) {
        expressionerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    $$ = column_expr;
};

constant_expr: STRING {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = $1;
    $$ = const_expr;
}
| TRUE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    $$ = const_expr;
}
| FALSE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    $$ = const_expr;
}
| DOUBLE_VALUE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| DATE STRING {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = $2;
    $$ = const_expr;
}
| TIME STRING {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = $2;
    $$ = const_expr;
}
| DATETIME STRING {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = $2;
    $$ = const_expr;
}
| TIMESTAMP STRING {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = $2;
    $$ = const_expr;
}
| INTERVAL interval_expr {
    $$ = $2;
}
| interval_expr {
    $$ = $1;
}
| long_array_expr {
    $$ = $1;
}
| double_array_expr {
    $$ = $1;
};

long_array_expr: unclosed_long_array_expr ']' {
    $$ = $1;
};

unclosed_long_array_expr: '[' LONG_VALUE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back($2);
    $$ = const_expr;
}
| unclosed_long_array_expr ',' LONG_VALUE {
    $1->long_array_.emplace_back($3);
    $$ = $1;
}

double_array_expr: unclosed_double_array_expr ']' {
    $$ = $1;
};

unclosed_double_array_expr: '[' DOUBLE_VALUE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back($2);
    $$ = const_expr;
}
| unclosed_double_array_expr ',' DOUBLE_VALUE {
    $1->double_array_.emplace_back($3);
    $$ = $1;
}

interval_expr: LONG_VALUE SECONDS {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE SECOND {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MINUTES {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MINUTE {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE HOURS {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE HOUR {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE DAYS {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE DAY {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MONTHS {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE MONTH {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE YEARS {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
}
| LONG_VALUE YEAR {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = $1;
    $$ = const_expr;
};

%%

void
expressionerror(YYLTYPE * llocp, void* lexer, infinity::ExpressionParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

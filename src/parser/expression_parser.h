/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED
#define YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef EXPRESSIONDEBUG
#if defined YYDEBUG
#if YYDEBUG
#define EXPRESSIONDEBUG 1
#else
#define EXPRESSIONDEBUG 0
#endif
#else /* ! defined YYDEBUG */
#define EXPRESSIONDEBUG 1
#endif /* ! defined YYDEBUG */
#endif /* ! defined EXPRESSIONDEBUG */
#if EXPRESSIONDEBUG
extern int expressiondebug;
#endif
/* "%code requires" blocks.  */
#line 10 "expression_parser.y"

#include "definition/column_def.h"
#include "expression.h"
#include "expression_parser_result.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

#define YYSTYPE EXPRESSIONSTYPE
#define YYLTYPE EXPRESSIONLTYPE

struct EXPRESSION_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;
    int string_length;

    std::vector<void *> parameters;
};

#define EXPRESSIONLTYPE EXPRESSION_LTYPE
#define EXPRESSIONLTYPE_IS_DECLARED 1

#define YY_USER_ACTION                                                                                                                               \
    yylloc->first_line = yylloc->last_line;                                                                                                          \
    yylloc->first_column = yylloc->last_column;                                                                                                      \
    for (int i = 0; yytext[i] != '\0'; ++i) {                                                                                                        \
        yylloc->total_column++;                                                                                                                      \
        yylloc->string_length++;                                                                                                                     \
        if (yytext[i] == '\n') {                                                                                                                     \
            yylloc->last_line++;                                                                                                                     \
            yylloc->last_column = 0;                                                                                                                 \
        } else {                                                                                                                                     \
            yylloc->last_column++;                                                                                                                   \
        }                                                                                                                                            \
    }

#line 102 "expression_parser.h"

/* Token type.  */
#ifndef EXPRESSIONTOKENTYPE
#define EXPRESSIONTOKENTYPE
enum expressiontokentype {
    IDENTIFIER = 258,
    STRING = 259,
    DOUBLE_VALUE = 260,
    LONG_VALUE = 261,
    CREATE = 262,
    SELECT = 263,
    INSERT = 264,
    DROP = 265,
    UPDATE = 266,
    DELETE = 267,
    COPY = 268,
    SET = 269,
    EXPLAIN = 270,
    SHOW = 271,
    ALTER = 272,
    EXECUTE = 273,
    PREPARE = 274,
    UNION = 275,
    ALL = 276,
    INTERSECT = 277,
    COMPACT = 278,
    EXCEPT = 279,
    FLUSH = 280,
    USE = 281,
    OPTIMIZE = 282,
    PROPERTIES = 283,
    DATABASE = 284,
    TABLE = 285,
    COLLECTION = 286,
    TABLES = 287,
    INTO = 288,
    VALUES = 289,
    AST = 290,
    PIPELINE = 291,
    RAW = 292,
    LOGICAL = 293,
    PHYSICAL = 294,
    FRAGMENT = 295,
    VIEW = 296,
    INDEX = 297,
    ANALYZE = 298,
    VIEWS = 299,
    DATABASES = 300,
    SEGMENT = 301,
    SEGMENTS = 302,
    BLOCK = 303,
    BLOCKS = 304,
    COLUMNS = 305,
    INDEXES = 306,
    GROUP = 307,
    BY = 308,
    HAVING = 309,
    AS = 310,
    NATURAL = 311,
    JOIN = 312,
    LEFT = 313,
    RIGHT = 314,
    OUTER = 315,
    FULL = 316,
    ON = 317,
    INNER = 318,
    CROSS = 319,
    DISTINCT = 320,
    WHERE = 321,
    ORDER = 322,
    LIMIT = 323,
    OFFSET = 324,
    ASC = 325,
    DESC = 326,
    IF = 327,
    NOT = 328,
    EXISTS = 329,
    IN = 330,
    FROM = 331,
    TO = 332,
    WITH = 333,
    DELIMITER = 334,
    FORMAT = 335,
    HEADER = 336,
    CAST = 337,
    END = 338,
    CASE = 339,
    ELSE = 340,
    THEN = 341,
    WHEN = 342,
    BOOLEAN = 343,
    INTEGER = 344,
    INT = 345,
    TINYINT = 346,
    SMALLINT = 347,
    BIGINT = 348,
    HUGEINT = 349,
    VARCHAR = 350,
    FLOAT = 351,
    DOUBLE = 352,
    REAL = 353,
    DECIMAL = 354,
    DATE = 355,
    TIME = 356,
    DATETIME = 357,
    TIMESTAMP = 358,
    UUID = 359,
    POINT = 360,
    LINE = 361,
    LSEG = 362,
    BOX = 363,
    PATH = 364,
    POLYGON = 365,
    CIRCLE = 366,
    BLOB = 367,
    BITMAP = 368,
    EMBEDDING = 369,
    VECTOR = 370,
    BIT = 371,
    SPARSE = 372,
    PRIMARY = 373,
    KEY = 374,
    UNIQUE = 375,
    NULLABLE = 376,
    IS = 377,
    TRUE = 378,
    FALSE = 379,
    INTERVAL = 380,
    SECOND = 381,
    SECONDS = 382,
    MINUTE = 383,
    MINUTES = 384,
    HOUR = 385,
    HOURS = 386,
    DAY = 387,
    DAYS = 388,
    MONTH = 389,
    MONTHS = 390,
    YEAR = 391,
    YEARS = 392,
    EQUAL = 393,
    NOT_EQ = 394,
    LESS_EQ = 395,
    GREATER_EQ = 396,
    BETWEEN = 397,
    AND = 398,
    OR = 399,
    EXTRACT = 400,
    LIKE = 401,
    DATA = 402,
    LOG = 403,
    BUFFER = 404,
    KNN = 405,
    USING = 406,
    SESSION = 407,
    GLOBAL = 408,
    OFF = 409,
    EXPORT = 410,
    PROFILE = 411,
    CONFIGS = 412,
    PROFILES = 413,
    STATUS = 414,
    VAR = 415,
    SEARCH = 416,
    MATCH = 417,
    QUERY = 418,
    FUSION = 419,
    NUMBER = 420
};
#endif

/* Value type.  */
#if !defined EXPRESSIONSTYPE && !defined EXPRESSIONSTYPE_IS_DECLARED
union EXPRESSIONSTYPE {
#line 85 "expression_parser.y"

    bool bool_value;
    char *str_value;
    double double_value;
    int64_t long_value;

    infinity::ParsedExpr *expr_t;
    infinity::ConstantExpr *const_expr_t;
    std::vector<infinity::ParsedExpr *> *expr_array_t;

#line 290 "expression_parser.h"
};
typedef union EXPRESSIONSTYPE EXPRESSIONSTYPE;
#define EXPRESSIONSTYPE_IS_TRIVIAL 1
#define EXPRESSIONSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if !defined EXPRESSIONLTYPE && !defined EXPRESSIONLTYPE_IS_DECLARED
typedef struct EXPRESSIONLTYPE EXPRESSIONLTYPE;
struct EXPRESSIONLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
#define EXPRESSIONLTYPE_IS_DECLARED 1
#define EXPRESSIONLTYPE_IS_TRIVIAL 1
#endif

int expressionparse(void *scanner, infinity::ExpressionParserResult *result);

#endif /* !YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED  */

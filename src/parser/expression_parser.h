/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED
# define YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef EXPRESSIONDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define EXPRESSIONDEBUG 1
#  else
#   define EXPRESSIONDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define EXPRESSIONDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined EXPRESSIONDEBUG */
#if EXPRESSIONDEBUG
extern int expressiondebug;
#endif
/* "%code requires" blocks.  */
#line 10 "expression_parser.y"


#include "expression.h"
#include "expression_parser_result.h"
#include "definition/column_def.h"
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

#line 103 "expression_parser.h"

/* Token kinds.  */
#ifndef EXPRESSIONTOKENTYPE
# define EXPRESSIONTOKENTYPE
  enum expressiontokentype
  {
    EXPRESSIONEMPTY = -2,
    EXPRESSIONEOF = 0,             /* "end of file"  */
    EXPRESSIONerror = 256,         /* error  */
    EXPRESSIONUNDEF = 257,         /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING = 259,                  /* STRING  */
    DOUBLE_VALUE = 260,            /* DOUBLE_VALUE  */
    LONG_VALUE = 261,              /* LONG_VALUE  */
    CREATE = 262,                  /* CREATE  */
    SELECT = 263,                  /* SELECT  */
    INSERT = 264,                  /* INSERT  */
    DROP = 265,                    /* DROP  */
    UPDATE = 266,                  /* UPDATE  */
    DELETE = 267,                  /* DELETE  */
    COPY = 268,                    /* COPY  */
    SET = 269,                     /* SET  */
    EXPLAIN = 270,                 /* EXPLAIN  */
    SHOW = 271,                    /* SHOW  */
    ALTER = 272,                   /* ALTER  */
    EXECUTE = 273,                 /* EXECUTE  */
    PREPARE = 274,                 /* PREPARE  */
    UNION = 275,                   /* UNION  */
    ALL = 276,                     /* ALL  */
    INTERSECT = 277,               /* INTERSECT  */
    COMPACT = 278,                 /* COMPACT  */
    EXCEPT = 279,                  /* EXCEPT  */
    FLUSH = 280,                   /* FLUSH  */
    USE = 281,                     /* USE  */
    OPTIMIZE = 282,                /* OPTIMIZE  */
    PROPERTIES = 283,              /* PROPERTIES  */
    DATABASE = 284,                /* DATABASE  */
    TABLE = 285,                   /* TABLE  */
    COLLECTION = 286,              /* COLLECTION  */
    TABLES = 287,                  /* TABLES  */
    INTO = 288,                    /* INTO  */
    VALUES = 289,                  /* VALUES  */
    AST = 290,                     /* AST  */
    PIPELINE = 291,                /* PIPELINE  */
    RAW = 292,                     /* RAW  */
    LOGICAL = 293,                 /* LOGICAL  */
    PHYSICAL = 294,                /* PHYSICAL  */
    FRAGMENT = 295,                /* FRAGMENT  */
    VIEW = 296,                    /* VIEW  */
    INDEX = 297,                   /* INDEX  */
    ANALYZE = 298,                 /* ANALYZE  */
    VIEWS = 299,                   /* VIEWS  */
    DATABASES = 300,               /* DATABASES  */
    SEGMENT = 301,                 /* SEGMENT  */
    SEGMENTS = 302,                /* SEGMENTS  */
    BLOCK = 303,                   /* BLOCK  */
    BLOCKS = 304,                  /* BLOCKS  */
    COLUMNS = 305,                 /* COLUMNS  */
    INDEXES = 306,                 /* INDEXES  */
    GROUP = 307,                   /* GROUP  */
    BY = 308,                      /* BY  */
    HAVING = 309,                  /* HAVING  */
    AS = 310,                      /* AS  */
    NATURAL = 311,                 /* NATURAL  */
    JOIN = 312,                    /* JOIN  */
    LEFT = 313,                    /* LEFT  */
    RIGHT = 314,                   /* RIGHT  */
    OUTER = 315,                   /* OUTER  */
    FULL = 316,                    /* FULL  */
    ON = 317,                      /* ON  */
    INNER = 318,                   /* INNER  */
    CROSS = 319,                   /* CROSS  */
    DISTINCT = 320,                /* DISTINCT  */
    WHERE = 321,                   /* WHERE  */
    ORDER = 322,                   /* ORDER  */
    LIMIT = 323,                   /* LIMIT  */
    OFFSET = 324,                  /* OFFSET  */
    ASC = 325,                     /* ASC  */
    DESC = 326,                    /* DESC  */
    IF = 327,                      /* IF  */
    NOT = 328,                     /* NOT  */
    EXISTS = 329,                  /* EXISTS  */
    IN = 330,                      /* IN  */
    FROM = 331,                    /* FROM  */
    TO = 332,                      /* TO  */
    WITH = 333,                    /* WITH  */
    DELIMITER = 334,               /* DELIMITER  */
    FORMAT = 335,                  /* FORMAT  */
    HEADER = 336,                  /* HEADER  */
    CAST = 337,                    /* CAST  */
    END = 338,                     /* END  */
    CASE = 339,                    /* CASE  */
    ELSE = 340,                    /* ELSE  */
    THEN = 341,                    /* THEN  */
    WHEN = 342,                    /* WHEN  */
    BOOLEAN = 343,                 /* BOOLEAN  */
    INTEGER = 344,                 /* INTEGER  */
    INT = 345,                     /* INT  */
    TINYINT = 346,                 /* TINYINT  */
    SMALLINT = 347,                /* SMALLINT  */
    BIGINT = 348,                  /* BIGINT  */
    HUGEINT = 349,                 /* HUGEINT  */
    VARCHAR = 350,                 /* VARCHAR  */
    FLOAT = 351,                   /* FLOAT  */
    DOUBLE = 352,                  /* DOUBLE  */
    REAL = 353,                    /* REAL  */
    DECIMAL = 354,                 /* DECIMAL  */
    DATE = 355,                    /* DATE  */
    TIME = 356,                    /* TIME  */
    DATETIME = 357,                /* DATETIME  */
    JSON = 358,                    /* JSON  */
    TIMESTAMP = 359,               /* TIMESTAMP  */
    UUID = 360,                    /* UUID  */
    POINT = 361,                   /* POINT  */
    LINE = 362,                    /* LINE  */
    LSEG = 363,                    /* LSEG  */
    BOX = 364,                     /* BOX  */
    PATH = 365,                    /* PATH  */
    POLYGON = 366,                 /* POLYGON  */
    CIRCLE = 367,                  /* CIRCLE  */
    BLOB = 368,                    /* BLOB  */
    BITMAP = 369,                  /* BITMAP  */
    EMBEDDING = 370,               /* EMBEDDING  */
    VECTOR = 371,                  /* VECTOR  */
    BIT = 372,                     /* BIT  */
    SPARSE = 373,                  /* SPARSE  */
    PRIMARY = 374,                 /* PRIMARY  */
    KEY = 375,                     /* KEY  */
    UNIQUE = 376,                  /* UNIQUE  */
    NULLABLE = 377,                /* NULLABLE  */
    IS = 378,                      /* IS  */
    TRUE = 379,                    /* TRUE  */
    FALSE = 380,                   /* FALSE  */
    INTERVAL = 381,                /* INTERVAL  */
    SECOND = 382,                  /* SECOND  */
    SECONDS = 383,                 /* SECONDS  */
    MINUTE = 384,                  /* MINUTE  */
    MINUTES = 385,                 /* MINUTES  */
    HOUR = 386,                    /* HOUR  */
    HOURS = 387,                   /* HOURS  */
    DAY = 388,                     /* DAY  */
    DAYS = 389,                    /* DAYS  */
    MONTH = 390,                   /* MONTH  */
    MONTHS = 391,                  /* MONTHS  */
    YEAR = 392,                    /* YEAR  */
    YEARS = 393,                   /* YEARS  */
    EQUAL = 394,                   /* EQUAL  */
    NOT_EQ = 395,                  /* NOT_EQ  */
    LESS_EQ = 396,                 /* LESS_EQ  */
    GREATER_EQ = 397,              /* GREATER_EQ  */
    BETWEEN = 398,                 /* BETWEEN  */
    AND = 399,                     /* AND  */
    OR = 400,                      /* OR  */
    EXTRACT = 401,                 /* EXTRACT  */
    LIKE = 402,                    /* LIKE  */
    DATA = 403,                    /* DATA  */
    LOG = 404,                     /* LOG  */
    BUFFER = 405,                  /* BUFFER  */
    KNN = 406,                     /* KNN  */
    USING = 407,                   /* USING  */
    SESSION = 408,                 /* SESSION  */
    GLOBAL = 409,                  /* GLOBAL  */
    OFF = 410,                     /* OFF  */
    EXPORT = 411,                  /* EXPORT  */
    PROFILE = 412,                 /* PROFILE  */
    CONFIGS = 413,                 /* CONFIGS  */
    PROFILES = 414,                /* PROFILES  */
    STATUS = 415,                  /* STATUS  */
    VAR = 416,                     /* VAR  */
    SEARCH = 417,                  /* SEARCH  */
    MATCH = 418,                   /* MATCH  */
    QUERY = 419,                   /* QUERY  */
    FUSION = 420,                  /* FUSION  */
    NUMBER = 421                   /* NUMBER  */
  };
  typedef enum expressiontokentype expressiontoken_kind_t;
#endif

/* Value type.  */
#if ! defined EXPRESSIONSTYPE && ! defined EXPRESSIONSTYPE_IS_DECLARED
union EXPRESSIONSTYPE
{
#line 85 "expression_parser.y"

    bool    bool_value;
    char*   str_value;
    double  double_value;
    int64_t long_value;

    infinity::ParsedExpr*             expr_t;
    infinity::ConstantExpr*           const_expr_t;
    std::vector<infinity::ParsedExpr*>*    expr_array_t;
    infinity::ColumnType*             column_type_t;

#line 298 "expression_parser.h"

};
typedef union EXPRESSIONSTYPE EXPRESSIONSTYPE;
# define EXPRESSIONSTYPE_IS_TRIVIAL 1
# define EXPRESSIONSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined EXPRESSIONLTYPE && ! defined EXPRESSIONLTYPE_IS_DECLARED
typedef struct EXPRESSIONLTYPE EXPRESSIONLTYPE;
struct EXPRESSIONLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define EXPRESSIONLTYPE_IS_DECLARED 1
# define EXPRESSIONLTYPE_IS_TRIVIAL 1
#endif




int expressionparse (void *scanner, infinity::ExpressionParserResult* result);


#endif /* !YY_EXPRESSION_EXPRESSION_PARSER_H_INCLUDED  */

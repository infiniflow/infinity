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

#ifndef YY_SQL_PARSER_H_INCLUDED
# define YY_SQL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef SQLDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define SQLDEBUG 1
#  else
#   define SQLDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define SQLDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined SQLDEBUG */
#if SQLDEBUG
extern int sqldebug;
#endif
/* "%code requires" blocks.  */
#line 11 "parser.y"


#include "expression.h"
#include "parser_result.h"
#include "defer_operation.h"
#include "statement/alter_statement.h"
#include "statement/copy_statement.h"
#include "statement/create_statement.h"
#include "statement/delete_statement.h"
#include "statement/drop_statement.h"
#include "statement/execute_statement.h"
#include "statement/explain_statement.h"
#include "statement/flush_statement.h"
#include "statement/optimize_statement.h"
#include "statement/insert_statement.h"
#include "statement/prepare_statement.h"
#include "statement/select_statement.h"
#include "statement/show_statement.h"
#include "statement/update_statement.h"
#include "statement/command_statement.h"
#include "statement/compact_statement.h"
#include "table_reference/base_table_reference.h"
#include "table_reference/join_reference.h"
#include "table_reference/cross_product_reference.h"
#include "table_reference/table_reference.h"
#include "table_reference/subquery_reference.h"
#include "parser_helper.h"

#include <vector>

#define YYSTYPE SQLSTYPE
#define YYLTYPE SQLLTYPE

struct SQL_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;
    int string_length;

    std::vector<void*> parameters;
};

#define SQLLTYPE SQL_LTYPE
#define SQLLTYPE_IS_DECLARED 1

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

#line 120 "parser.h"

/* Token kinds.  */
#ifndef SQLTOKENTYPE
# define SQLTOKENTYPE
  enum sqltokentype
  {
    SQLEMPTY = -2,
    SQLEOF = 0,                    /* "end of file"  */
    SQLerror = 256,                /* error  */
    SQLUNDEF = 257,                /* "invalid token"  */
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
    DESCRIBE = 275,                /* DESCRIBE  */
    UNION = 276,                   /* UNION  */
    ALL = 277,                     /* ALL  */
    INTERSECT = 278,               /* INTERSECT  */
    COMPACT = 279,                 /* COMPACT  */
    EXCEPT = 280,                  /* EXCEPT  */
    FLUSH = 281,                   /* FLUSH  */
    USE = 282,                     /* USE  */
    OPTIMIZE = 283,                /* OPTIMIZE  */
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
    GROUP = 304,                   /* GROUP  */
    BY = 305,                      /* BY  */
    HAVING = 306,                  /* HAVING  */
    AS = 307,                      /* AS  */
    NATURAL = 308,                 /* NATURAL  */
    JOIN = 309,                    /* JOIN  */
    LEFT = 310,                    /* LEFT  */
    RIGHT = 311,                   /* RIGHT  */
    OUTER = 312,                   /* OUTER  */
    FULL = 313,                    /* FULL  */
    ON = 314,                      /* ON  */
    INNER = 315,                   /* INNER  */
    CROSS = 316,                   /* CROSS  */
    DISTINCT = 317,                /* DISTINCT  */
    WHERE = 318,                   /* WHERE  */
    ORDER = 319,                   /* ORDER  */
    LIMIT = 320,                   /* LIMIT  */
    OFFSET = 321,                  /* OFFSET  */
    ASC = 322,                     /* ASC  */
    DESC = 323,                    /* DESC  */
    IF = 324,                      /* IF  */
    NOT = 325,                     /* NOT  */
    EXISTS = 326,                  /* EXISTS  */
    IN = 327,                      /* IN  */
    FROM = 328,                    /* FROM  */
    TO = 329,                      /* TO  */
    WITH = 330,                    /* WITH  */
    DELIMITER = 331,               /* DELIMITER  */
    FORMAT = 332,                  /* FORMAT  */
    HEADER = 333,                  /* HEADER  */
    CAST = 334,                    /* CAST  */
    END = 335,                     /* END  */
    CASE = 336,                    /* CASE  */
    ELSE = 337,                    /* ELSE  */
    THEN = 338,                    /* THEN  */
    WHEN = 339,                    /* WHEN  */
    BOOLEAN = 340,                 /* BOOLEAN  */
    INTEGER = 341,                 /* INTEGER  */
    INT = 342,                     /* INT  */
    TINYINT = 343,                 /* TINYINT  */
    SMALLINT = 344,                /* SMALLINT  */
    BIGINT = 345,                  /* BIGINT  */
    HUGEINT = 346,                 /* HUGEINT  */
    VARCHAR = 347,                 /* VARCHAR  */
    FLOAT = 348,                   /* FLOAT  */
    DOUBLE = 349,                  /* DOUBLE  */
    REAL = 350,                    /* REAL  */
    DECIMAL = 351,                 /* DECIMAL  */
    DATE = 352,                    /* DATE  */
    TIME = 353,                    /* TIME  */
    DATETIME = 354,                /* DATETIME  */
    TIMESTAMP = 355,               /* TIMESTAMP  */
    UUID = 356,                    /* UUID  */
    POINT = 357,                   /* POINT  */
    LINE = 358,                    /* LINE  */
    LSEG = 359,                    /* LSEG  */
    BOX = 360,                     /* BOX  */
    PATH = 361,                    /* PATH  */
    POLYGON = 362,                 /* POLYGON  */
    CIRCLE = 363,                  /* CIRCLE  */
    BLOB = 364,                    /* BLOB  */
    BITMAP = 365,                  /* BITMAP  */
    EMBEDDING = 366,               /* EMBEDDING  */
    VECTOR = 367,                  /* VECTOR  */
    BIT = 368,                     /* BIT  */
    PRIMARY = 369,                 /* PRIMARY  */
    KEY = 370,                     /* KEY  */
    UNIQUE = 371,                  /* UNIQUE  */
    NULLABLE = 372,                /* NULLABLE  */
    IS = 373,                      /* IS  */
    TRUE = 374,                    /* TRUE  */
    FALSE = 375,                   /* FALSE  */
    INTERVAL = 376,                /* INTERVAL  */
    SECOND = 377,                  /* SECOND  */
    SECONDS = 378,                 /* SECONDS  */
    MINUTE = 379,                  /* MINUTE  */
    MINUTES = 380,                 /* MINUTES  */
    HOUR = 381,                    /* HOUR  */
    HOURS = 382,                   /* HOURS  */
    DAY = 383,                     /* DAY  */
    DAYS = 384,                    /* DAYS  */
    MONTH = 385,                   /* MONTH  */
    MONTHS = 386,                  /* MONTHS  */
    YEAR = 387,                    /* YEAR  */
    YEARS = 388,                   /* YEARS  */
    EQUAL = 389,                   /* EQUAL  */
    NOT_EQ = 390,                  /* NOT_EQ  */
    LESS_EQ = 391,                 /* LESS_EQ  */
    GREATER_EQ = 392,              /* GREATER_EQ  */
    BETWEEN = 393,                 /* BETWEEN  */
    AND = 394,                     /* AND  */
    OR = 395,                      /* OR  */
    EXTRACT = 396,                 /* EXTRACT  */
    LIKE = 397,                    /* LIKE  */
    DATA = 398,                    /* DATA  */
    LOG = 399,                     /* LOG  */
    BUFFER = 400,                  /* BUFFER  */
    KNN = 401,                     /* KNN  */
    USING = 402,                   /* USING  */
    SESSION = 403,                 /* SESSION  */
    GLOBAL = 404,                  /* GLOBAL  */
    OFF = 405,                     /* OFF  */
    EXPORT = 406,                  /* EXPORT  */
    PROFILE = 407,                 /* PROFILE  */
    CONFIGS = 408,                 /* CONFIGS  */
    PROFILES = 409,                /* PROFILES  */
    STATUS = 410,                  /* STATUS  */
    VAR = 411,                     /* VAR  */
    SEARCH = 412,                  /* SEARCH  */
    MATCH = 413,                   /* MATCH  */
    QUERY = 414,                   /* QUERY  */
    FUSION = 415,                  /* FUSION  */
    NUMBER = 416                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 96 "parser.y"

    bool    bool_value;
    char*   str_value;
    double  double_value;
    int64_t long_value;

    infinity::BaseStatement*   base_stmt;
    infinity::SelectStatement* select_stmt;
    infinity::CopyStatement*   copy_stmt;
    infinity::InsertStatement* insert_stmt;
    infinity::UpdateStatement* update_stmt;
    infinity::DeleteStatement* delete_stmt;
    infinity::CreateStatement* create_stmt;
    infinity::DropStatement*   drop_stmt;
    infinity::PrepareStatement* prepare_stmt;
    infinity::ExecuteStatement* execute_stmt;
    infinity::AlterStatement*   alter_stmt;
    infinity::ShowStatement*    show_stmt;
    infinity::ExplainStatement* explain_stmt;
    infinity::FlushStatement*  flush_stmt;
    infinity::OptimizeStatement*  optimize_stmt;
    infinity::CommandStatement* command_stmt;
    infinity::CompactStatement* compact_stmt;

    std::vector<infinity::BaseStatement*>* stmt_array;

    std::vector<infinity::TableElement*>*  table_element_array_t;
    infinity::TableElement*           table_element_t;
    infinity::ColumnDef*              table_column_t;
    infinity::ColumnType              column_type_t;
    infinity::ConstraintType          column_constraint_t;
    std::unordered_set<infinity::ConstraintType>* column_constraints_t;
    std::vector<std::string>*         identifier_array_t;
    infinity::TableConstraint*        table_constraint_t;

    infinity::BaseTableReference*     table_reference_t;
    infinity::TableAlias *            table_alias_t;
    infinity::JoinType                join_type_t;

    infinity::OrderByExpr*            order_by_expr_t;
    std::vector<infinity::OrderByExpr*>*   order_by_expr_list_t;
    infinity::OrderType               order_by_type_t;

    infinity::WithExpr*               with_expr_t;
    std::vector<infinity::WithExpr*>*      with_expr_list_t;

    infinity::SetOperatorType         set_operator_t;

    infinity::ExplainType             explain_type_t;

    infinity::ParsedExpr*             expr_t;
    infinity::ConstantExpr*           const_expr_t;
    std::vector<infinity::ParsedExpr*>*    expr_array_t;
    std::vector<std::vector<infinity::ParsedExpr*>*>*    expr_array_list_t;

    std::vector<infinity::WhenThen*>*     case_check_array_t;

    infinity::UpdateExpr*             update_expr_t;
    std::vector<infinity::UpdateExpr*>*    update_expr_array_t;

    infinity::TableName* table_name_t;
    infinity::CopyOption* copy_option_t;
    std::vector<infinity::CopyOption*>* copy_option_array;

    infinity::InitParameter*        index_param_t;
    std::vector<infinity::InitParameter*>* index_param_list_t;
    std::vector<infinity::InitParameter*>* with_index_param_list_t;

    std::vector<infinity::IndexInfo*>* index_info_list_t;

    // infinity::IfExistsInfo*        if_exists_info_t;
    infinity::IfNotExistsInfo*     if_not_exists_info_t;

#line 372 "parser.h"

};
typedef union SQLSTYPE SQLSTYPE;
# define SQLSTYPE_IS_TRIVIAL 1
# define SQLSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined SQLLTYPE && ! defined SQLLTYPE_IS_DECLARED
typedef struct SQLLTYPE SQLLTYPE;
struct SQLLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define SQLLTYPE_IS_DECLARED 1
# define SQLLTYPE_IS_TRIVIAL 1
#endif




int sqlparse (void *scanner, infinity::ParserResult* result);


#endif /* !YY_SQL_PARSER_H_INCLUDED  */

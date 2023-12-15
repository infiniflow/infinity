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

#line 119 "parser.h"

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
    EXCEPT = 279,                  /* EXCEPT  */
    FLUSH = 280,                   /* FLUSH  */
    USE = 281,                     /* USE  */
    OPTIMIZE = 282,                /* OPTIMIZE  */
    DATABASE = 283,                /* DATABASE  */
    TABLE = 284,                   /* TABLE  */
    COLLECTION = 285,              /* COLLECTION  */
    TABLES = 286,                  /* TABLES  */
    INTO = 287,                    /* INTO  */
    VALUES = 288,                  /* VALUES  */
    AST = 289,                     /* AST  */
    PIPELINE = 290,                /* PIPELINE  */
    RAW = 291,                     /* RAW  */
    LOGICAL = 292,                 /* LOGICAL  */
    PHYSICAL = 293,                /* PHYSICAL  */
    FRAGMENT = 294,                /* FRAGMENT  */
    VIEW = 295,                    /* VIEW  */
    INDEX = 296,                   /* INDEX  */
    ANALYZE = 297,                 /* ANALYZE  */
    VIEWS = 298,                   /* VIEWS  */
    DATABASES = 299,               /* DATABASES  */
    SEGMENT = 300,                 /* SEGMENT  */
    SEGMENTS = 301,                /* SEGMENTS  */
    BLOCK = 302,                   /* BLOCK  */
    GROUP = 303,                   /* GROUP  */
    BY = 304,                      /* BY  */
    HAVING = 305,                  /* HAVING  */
    AS = 306,                      /* AS  */
    NATURAL = 307,                 /* NATURAL  */
    JOIN = 308,                    /* JOIN  */
    LEFT = 309,                    /* LEFT  */
    RIGHT = 310,                   /* RIGHT  */
    OUTER = 311,                   /* OUTER  */
    FULL = 312,                    /* FULL  */
    ON = 313,                      /* ON  */
    INNER = 314,                   /* INNER  */
    CROSS = 315,                   /* CROSS  */
    DISTINCT = 316,                /* DISTINCT  */
    WHERE = 317,                   /* WHERE  */
    ORDER = 318,                   /* ORDER  */
    LIMIT = 319,                   /* LIMIT  */
    OFFSET = 320,                  /* OFFSET  */
    ASC = 321,                     /* ASC  */
    DESC = 322,                    /* DESC  */
    IF = 323,                      /* IF  */
    NOT = 324,                     /* NOT  */
    EXISTS = 325,                  /* EXISTS  */
    IN = 326,                      /* IN  */
    FROM = 327,                    /* FROM  */
    TO = 328,                      /* TO  */
    WITH = 329,                    /* WITH  */
    DELIMITER = 330,               /* DELIMITER  */
    FORMAT = 331,                  /* FORMAT  */
    HEADER = 332,                  /* HEADER  */
    CAST = 333,                    /* CAST  */
    END = 334,                     /* END  */
    CASE = 335,                    /* CASE  */
    ELSE = 336,                    /* ELSE  */
    THEN = 337,                    /* THEN  */
    WHEN = 338,                    /* WHEN  */
    BOOLEAN = 339,                 /* BOOLEAN  */
    INTEGER = 340,                 /* INTEGER  */
    INT = 341,                     /* INT  */
    TINYINT = 342,                 /* TINYINT  */
    SMALLINT = 343,                /* SMALLINT  */
    BIGINT = 344,                  /* BIGINT  */
    HUGEINT = 345,                 /* HUGEINT  */
    CHAR = 346,                    /* CHAR  */
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
    SEARCH = 411,                  /* SEARCH  */
    MATCH = 412,                   /* MATCH  */
    QUERY = 413,                   /* QUERY  */
    FUSION = 414,                  /* FUSION  */
    NUMBER = 415                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 95 "parser.y"

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

#line 369 "parser.h"

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

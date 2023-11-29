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

#line 118 "parser.h"

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
    DATABASE = 282,                /* DATABASE  */
    TABLE = 283,                   /* TABLE  */
    COLLECTION = 284,              /* COLLECTION  */
    TABLES = 285,                  /* TABLES  */
    INTO = 286,                    /* INTO  */
    VALUES = 287,                  /* VALUES  */
    AST = 288,                     /* AST  */
    PIPELINE = 289,                /* PIPELINE  */
    RAW = 290,                     /* RAW  */
    LOGICAL = 291,                 /* LOGICAL  */
    PHYSICAL = 292,                /* PHYSICAL  */
    FRAGMENT = 293,                /* FRAGMENT  */
    VIEW = 294,                    /* VIEW  */
    INDEX = 295,                   /* INDEX  */
    ANALYZE = 296,                 /* ANALYZE  */
    VIEWS = 297,                   /* VIEWS  */
    DATABASES = 298,               /* DATABASES  */
    SEGMENT = 299,                 /* SEGMENT  */
    SEGMENTS = 300,                /* SEGMENTS  */
    BLOCK = 301,                   /* BLOCK  */
    GROUP = 302,                   /* GROUP  */
    BY = 303,                      /* BY  */
    HAVING = 304,                  /* HAVING  */
    AS = 305,                      /* AS  */
    NATURAL = 306,                 /* NATURAL  */
    JOIN = 307,                    /* JOIN  */
    LEFT = 308,                    /* LEFT  */
    RIGHT = 309,                   /* RIGHT  */
    OUTER = 310,                   /* OUTER  */
    FULL = 311,                    /* FULL  */
    ON = 312,                      /* ON  */
    INNER = 313,                   /* INNER  */
    CROSS = 314,                   /* CROSS  */
    DISTINCT = 315,                /* DISTINCT  */
    WHERE = 316,                   /* WHERE  */
    ORDER = 317,                   /* ORDER  */
    LIMIT = 318,                   /* LIMIT  */
    OFFSET = 319,                  /* OFFSET  */
    ASC = 320,                     /* ASC  */
    DESC = 321,                    /* DESC  */
    IF = 322,                      /* IF  */
    NOT = 323,                     /* NOT  */
    EXISTS = 324,                  /* EXISTS  */
    IN = 325,                      /* IN  */
    FROM = 326,                    /* FROM  */
    TO = 327,                      /* TO  */
    WITH = 328,                    /* WITH  */
    DELIMITER = 329,               /* DELIMITER  */
    FORMAT = 330,                  /* FORMAT  */
    HEADER = 331,                  /* HEADER  */
    CAST = 332,                    /* CAST  */
    END = 333,                     /* END  */
    CASE = 334,                    /* CASE  */
    ELSE = 335,                    /* ELSE  */
    THEN = 336,                    /* THEN  */
    WHEN = 337,                    /* WHEN  */
    BOOLEAN = 338,                 /* BOOLEAN  */
    INTEGER = 339,                 /* INTEGER  */
    INT = 340,                     /* INT  */
    TINYINT = 341,                 /* TINYINT  */
    SMALLINT = 342,                /* SMALLINT  */
    BIGINT = 343,                  /* BIGINT  */
    HUGEINT = 344,                 /* HUGEINT  */
    CHAR = 345,                    /* CHAR  */
    VARCHAR = 346,                 /* VARCHAR  */
    FLOAT = 347,                   /* FLOAT  */
    DOUBLE = 348,                  /* DOUBLE  */
    REAL = 349,                    /* REAL  */
    DECIMAL = 350,                 /* DECIMAL  */
    DATE = 351,                    /* DATE  */
    TIME = 352,                    /* TIME  */
    DATETIME = 353,                /* DATETIME  */
    TIMESTAMP = 354,               /* TIMESTAMP  */
    UUID = 355,                    /* UUID  */
    POINT = 356,                   /* POINT  */
    LINE = 357,                    /* LINE  */
    LSEG = 358,                    /* LSEG  */
    BOX = 359,                     /* BOX  */
    PATH = 360,                    /* PATH  */
    POLYGON = 361,                 /* POLYGON  */
    CIRCLE = 362,                  /* CIRCLE  */
    BLOB = 363,                    /* BLOB  */
    BITMAP = 364,                  /* BITMAP  */
    EMBEDDING = 365,               /* EMBEDDING  */
    VECTOR = 366,                  /* VECTOR  */
    BIT = 367,                     /* BIT  */
    PRIMARY = 368,                 /* PRIMARY  */
    KEY = 369,                     /* KEY  */
    UNIQUE = 370,                  /* UNIQUE  */
    NULLABLE = 371,                /* NULLABLE  */
    IS = 372,                      /* IS  */
    TRUE = 373,                    /* TRUE  */
    FALSE = 374,                   /* FALSE  */
    INTERVAL = 375,                /* INTERVAL  */
    SECOND = 376,                  /* SECOND  */
    SECONDS = 377,                 /* SECONDS  */
    MINUTE = 378,                  /* MINUTE  */
    MINUTES = 379,                 /* MINUTES  */
    HOUR = 380,                    /* HOUR  */
    HOURS = 381,                   /* HOURS  */
    DAY = 382,                     /* DAY  */
    DAYS = 383,                    /* DAYS  */
    MONTH = 384,                   /* MONTH  */
    MONTHS = 385,                  /* MONTHS  */
    YEAR = 386,                    /* YEAR  */
    YEARS = 387,                   /* YEARS  */
    EQUAL = 388,                   /* EQUAL  */
    NOT_EQ = 389,                  /* NOT_EQ  */
    LESS_EQ = 390,                 /* LESS_EQ  */
    GREATER_EQ = 391,              /* GREATER_EQ  */
    BETWEEN = 392,                 /* BETWEEN  */
    AND = 393,                     /* AND  */
    OR = 394,                      /* OR  */
    EXTRACT = 395,                 /* EXTRACT  */
    LIKE = 396,                    /* LIKE  */
    DATA = 397,                    /* DATA  */
    LOG = 398,                     /* LOG  */
    BUFFER = 399,                  /* BUFFER  */
    KNN = 400,                     /* KNN  */
    USING = 401,                   /* USING  */
    SESSION = 402,                 /* SESSION  */
    GLOBAL = 403,                  /* GLOBAL  */
    OFF = 404,                     /* OFF  */
    EXPORT = 405,                  /* EXPORT  */
    PROFILE = 406,                 /* PROFILE  */
    CONFIGS = 407,                 /* CONFIGS  */
    PROFILES = 408,                /* PROFILES  */
    SEARCH = 409,                  /* SEARCH  */
    MATCH = 410,                   /* MATCH  */
    QUERY = 411,                   /* QUERY  */
    FUSION = 412,                  /* FUSION  */
    NUMBER = 413                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 94 "parser.y"

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

#line 365 "parser.h"

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

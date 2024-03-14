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
    COMPACT = 279,                 /* COMPACT  */
    EXCEPT = 280,                  /* EXCEPT  */
    FLUSH = 281,                   /* FLUSH  */
    USE = 282,                     /* USE  */
    OPTIMIZE = 283,                /* OPTIMIZE  */
    PROPERTIES = 284,              /* PROPERTIES  */
    DATABASE = 285,                /* DATABASE  */
    TABLE = 286,                   /* TABLE  */
    COLLECTION = 287,              /* COLLECTION  */
    TABLES = 288,                  /* TABLES  */
    INTO = 289,                    /* INTO  */
    VALUES = 290,                  /* VALUES  */
    AST = 291,                     /* AST  */
    PIPELINE = 292,                /* PIPELINE  */
    RAW = 293,                     /* RAW  */
    LOGICAL = 294,                 /* LOGICAL  */
    PHYSICAL = 295,                /* PHYSICAL  */
    FRAGMENT = 296,                /* FRAGMENT  */
    VIEW = 297,                    /* VIEW  */
    INDEX = 298,                   /* INDEX  */
    ANALYZE = 299,                 /* ANALYZE  */
    VIEWS = 300,                   /* VIEWS  */
    DATABASES = 301,               /* DATABASES  */
    SEGMENT = 302,                 /* SEGMENT  */
    SEGMENTS = 303,                /* SEGMENTS  */
    BLOCK = 304,                   /* BLOCK  */
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
    TIMESTAMP = 358,               /* TIMESTAMP  */
    UUID = 359,                    /* UUID  */
    POINT = 360,                   /* POINT  */
    LINE = 361,                    /* LINE  */
    LSEG = 362,                    /* LSEG  */
    BOX = 363,                     /* BOX  */
    PATH = 364,                    /* PATH  */
    POLYGON = 365,                 /* POLYGON  */
    CIRCLE = 366,                  /* CIRCLE  */
    BLOB = 367,                    /* BLOB  */
    BITMAP = 368,                  /* BITMAP  */
    EMBEDDING = 369,               /* EMBEDDING  */
    VECTOR = 370,                  /* VECTOR  */
    BIT = 371,                     /* BIT  */
    PRIMARY = 372,                 /* PRIMARY  */
    KEY = 373,                     /* KEY  */
    UNIQUE = 374,                  /* UNIQUE  */
    NULLABLE = 375,                /* NULLABLE  */
    IS = 376,                      /* IS  */
    TRUE = 377,                    /* TRUE  */
    FALSE = 378,                   /* FALSE  */
    INTERVAL = 379,                /* INTERVAL  */
    SECOND = 380,                  /* SECOND  */
    SECONDS = 381,                 /* SECONDS  */
    MINUTE = 382,                  /* MINUTE  */
    MINUTES = 383,                 /* MINUTES  */
    HOUR = 384,                    /* HOUR  */
    HOURS = 385,                   /* HOURS  */
    DAY = 386,                     /* DAY  */
    DAYS = 387,                    /* DAYS  */
    MONTH = 388,                   /* MONTH  */
    MONTHS = 389,                  /* MONTHS  */
    YEAR = 390,                    /* YEAR  */
    YEARS = 391,                   /* YEARS  */
    EQUAL = 392,                   /* EQUAL  */
    NOT_EQ = 393,                  /* NOT_EQ  */
    LESS_EQ = 394,                 /* LESS_EQ  */
    GREATER_EQ = 395,              /* GREATER_EQ  */
    BETWEEN = 396,                 /* BETWEEN  */
    AND = 397,                     /* AND  */
    OR = 398,                      /* OR  */
    EXTRACT = 399,                 /* EXTRACT  */
    LIKE = 400,                    /* LIKE  */
    DATA = 401,                    /* DATA  */
    LOG = 402,                     /* LOG  */
    BUFFER = 403,                  /* BUFFER  */
    KNN = 404,                     /* KNN  */
    USING = 405,                   /* USING  */
    SESSION = 406,                 /* SESSION  */
    GLOBAL = 407,                  /* GLOBAL  */
    OFF = 408,                     /* OFF  */
    EXPORT = 409,                  /* EXPORT  */
    PROFILE = 410,                 /* PROFILE  */
    CONFIGS = 411,                 /* CONFIGS  */
    PROFILES = 412,                /* PROFILES  */
    STATUS = 413,                  /* STATUS  */
    VAR = 414,                     /* VAR  */
    SEARCH = 415,                  /* SEARCH  */
    MATCH = 416,                   /* MATCH  */
    QUERY = 417,                   /* QUERY  */
    FUSION = 418,                  /* FUSION  */
    NUMBER = 419                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 102 "parser.y"

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

#line 373 "parser.h"

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

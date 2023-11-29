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
    GROUP = 299,                   /* GROUP  */
    BY = 300,                      /* BY  */
    HAVING = 301,                  /* HAVING  */
    AS = 302,                      /* AS  */
    NATURAL = 303,                 /* NATURAL  */
    JOIN = 304,                    /* JOIN  */
    LEFT = 305,                    /* LEFT  */
    RIGHT = 306,                   /* RIGHT  */
    OUTER = 307,                   /* OUTER  */
    FULL = 308,                    /* FULL  */
    ON = 309,                      /* ON  */
    INNER = 310,                   /* INNER  */
    CROSS = 311,                   /* CROSS  */
    DISTINCT = 312,                /* DISTINCT  */
    WHERE = 313,                   /* WHERE  */
    ORDER = 314,                   /* ORDER  */
    LIMIT = 315,                   /* LIMIT  */
    OFFSET = 316,                  /* OFFSET  */
    ASC = 317,                     /* ASC  */
    DESC = 318,                    /* DESC  */
    IF = 319,                      /* IF  */
    NOT = 320,                     /* NOT  */
    EXISTS = 321,                  /* EXISTS  */
    IN = 322,                      /* IN  */
    FROM = 323,                    /* FROM  */
    TO = 324,                      /* TO  */
    WITH = 325,                    /* WITH  */
    DELIMITER = 326,               /* DELIMITER  */
    FORMAT = 327,                  /* FORMAT  */
    HEADER = 328,                  /* HEADER  */
    CAST = 329,                    /* CAST  */
    END = 330,                     /* END  */
    CASE = 331,                    /* CASE  */
    ELSE = 332,                    /* ELSE  */
    THEN = 333,                    /* THEN  */
    WHEN = 334,                    /* WHEN  */
    BOOLEAN = 335,                 /* BOOLEAN  */
    INTEGER = 336,                 /* INTEGER  */
    INT = 337,                     /* INT  */
    TINYINT = 338,                 /* TINYINT  */
    SMALLINT = 339,                /* SMALLINT  */
    BIGINT = 340,                  /* BIGINT  */
    HUGEINT = 341,                 /* HUGEINT  */
    CHAR = 342,                    /* CHAR  */
    VARCHAR = 343,                 /* VARCHAR  */
    FLOAT = 344,                   /* FLOAT  */
    DOUBLE = 345,                  /* DOUBLE  */
    REAL = 346,                    /* REAL  */
    DECIMAL = 347,                 /* DECIMAL  */
    DATE = 348,                    /* DATE  */
    TIME = 349,                    /* TIME  */
    DATETIME = 350,                /* DATETIME  */
    TIMESTAMP = 351,               /* TIMESTAMP  */
    UUID = 352,                    /* UUID  */
    POINT = 353,                   /* POINT  */
    LINE = 354,                    /* LINE  */
    LSEG = 355,                    /* LSEG  */
    BOX = 356,                     /* BOX  */
    PATH = 357,                    /* PATH  */
    POLYGON = 358,                 /* POLYGON  */
    CIRCLE = 359,                  /* CIRCLE  */
    BLOB = 360,                    /* BLOB  */
    BITMAP = 361,                  /* BITMAP  */
    EMBEDDING = 362,               /* EMBEDDING  */
    VECTOR = 363,                  /* VECTOR  */
    BIT = 364,                     /* BIT  */
    PRIMARY = 365,                 /* PRIMARY  */
    KEY = 366,                     /* KEY  */
    UNIQUE = 367,                  /* UNIQUE  */
    NULLABLE = 368,                /* NULLABLE  */
    IS = 369,                      /* IS  */
    TRUE = 370,                    /* TRUE  */
    FALSE = 371,                   /* FALSE  */
    INTERVAL = 372,                /* INTERVAL  */
    SECOND = 373,                  /* SECOND  */
    SECONDS = 374,                 /* SECONDS  */
    MINUTE = 375,                  /* MINUTE  */
    MINUTES = 376,                 /* MINUTES  */
    HOUR = 377,                    /* HOUR  */
    HOURS = 378,                   /* HOURS  */
    DAY = 379,                     /* DAY  */
    DAYS = 380,                    /* DAYS  */
    MONTH = 381,                   /* MONTH  */
    MONTHS = 382,                  /* MONTHS  */
    YEAR = 383,                    /* YEAR  */
    YEARS = 384,                   /* YEARS  */
    EQUAL = 385,                   /* EQUAL  */
    NOT_EQ = 386,                  /* NOT_EQ  */
    LESS_EQ = 387,                 /* LESS_EQ  */
    GREATER_EQ = 388,              /* GREATER_EQ  */
    BETWEEN = 389,                 /* BETWEEN  */
    AND = 390,                     /* AND  */
    OR = 391,                      /* OR  */
    EXTRACT = 392,                 /* EXTRACT  */
    LIKE = 393,                    /* LIKE  */
    DATA = 394,                    /* DATA  */
    LOG = 395,                     /* LOG  */
    BUFFER = 396,                  /* BUFFER  */
    KNN = 397,                     /* KNN  */
    USING = 398,                   /* USING  */
    SESSION = 399,                 /* SESSION  */
    GLOBAL = 400,                  /* GLOBAL  */
    OFF = 401,                     /* OFF  */
    EXPORT = 402,                  /* EXPORT  */
    PROFILE = 403,                 /* PROFILE  */
    CONFIGS = 404,                 /* CONFIGS  */
    PROFILES = 405,                /* PROFILES  */
    SEARCH = 406,                  /* SEARCH  */
    MATCH = 407,                   /* MATCH  */
    QUERY = 408,                   /* QUERY  */
    FUSION = 409,                  /* FUSION  */
    NUMBER = 410                   /* NUMBER  */
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

#line 362 "parser.h"

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

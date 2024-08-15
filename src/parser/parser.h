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
#include "statement/admin_statement.h"
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

#line 121 "parser.h"

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
    COLUMN = 305,                  /* COLUMN  */
    COLUMNS = 306,                 /* COLUMNS  */
    INDEXES = 307,                 /* INDEXES  */
    CHUNK = 308,                   /* CHUNK  */
    GROUP = 309,                   /* GROUP  */
    BY = 310,                      /* BY  */
    HAVING = 311,                  /* HAVING  */
    AS = 312,                      /* AS  */
    NATURAL = 313,                 /* NATURAL  */
    JOIN = 314,                    /* JOIN  */
    LEFT = 315,                    /* LEFT  */
    RIGHT = 316,                   /* RIGHT  */
    OUTER = 317,                   /* OUTER  */
    FULL = 318,                    /* FULL  */
    ON = 319,                      /* ON  */
    INNER = 320,                   /* INNER  */
    CROSS = 321,                   /* CROSS  */
    DISTINCT = 322,                /* DISTINCT  */
    WHERE = 323,                   /* WHERE  */
    ORDER = 324,                   /* ORDER  */
    LIMIT = 325,                   /* LIMIT  */
    OFFSET = 326,                  /* OFFSET  */
    ASC = 327,                     /* ASC  */
    DESC = 328,                    /* DESC  */
    IF = 329,                      /* IF  */
    NOT = 330,                     /* NOT  */
    EXISTS = 331,                  /* EXISTS  */
    IN = 332,                      /* IN  */
    FROM = 333,                    /* FROM  */
    TO = 334,                      /* TO  */
    WITH = 335,                    /* WITH  */
    DELIMITER = 336,               /* DELIMITER  */
    FORMAT = 337,                  /* FORMAT  */
    HEADER = 338,                  /* HEADER  */
    CAST = 339,                    /* CAST  */
    END = 340,                     /* END  */
    CASE = 341,                    /* CASE  */
    ELSE = 342,                    /* ELSE  */
    THEN = 343,                    /* THEN  */
    WHEN = 344,                    /* WHEN  */
    BOOLEAN = 345,                 /* BOOLEAN  */
    INTEGER = 346,                 /* INTEGER  */
    INT = 347,                     /* INT  */
    TINYINT = 348,                 /* TINYINT  */
    SMALLINT = 349,                /* SMALLINT  */
    BIGINT = 350,                  /* BIGINT  */
    HUGEINT = 351,                 /* HUGEINT  */
    VARCHAR = 352,                 /* VARCHAR  */
    FLOAT = 353,                   /* FLOAT  */
    DOUBLE = 354,                  /* DOUBLE  */
    REAL = 355,                    /* REAL  */
    DECIMAL = 356,                 /* DECIMAL  */
    DATE = 357,                    /* DATE  */
    TIME = 358,                    /* TIME  */
    DATETIME = 359,                /* DATETIME  */
    FLOAT16 = 360,                 /* FLOAT16  */
    BFLOAT16 = 361,                /* BFLOAT16  */
    UNSIGNED = 362,                /* UNSIGNED  */
    TIMESTAMP = 363,               /* TIMESTAMP  */
    UUID = 364,                    /* UUID  */
    POINT = 365,                   /* POINT  */
    LINE = 366,                    /* LINE  */
    LSEG = 367,                    /* LSEG  */
    BOX = 368,                     /* BOX  */
    PATH = 369,                    /* PATH  */
    POLYGON = 370,                 /* POLYGON  */
    CIRCLE = 371,                  /* CIRCLE  */
    BLOB = 372,                    /* BLOB  */
    BITMAP = 373,                  /* BITMAP  */
    EMBEDDING = 374,               /* EMBEDDING  */
    VECTOR = 375,                  /* VECTOR  */
    BIT = 376,                     /* BIT  */
    TEXT = 377,                    /* TEXT  */
    TENSOR = 378,                  /* TENSOR  */
    SPARSE = 379,                  /* SPARSE  */
    TENSORARRAY = 380,             /* TENSORARRAY  */
    IGNORE = 381,                  /* IGNORE  */
    PRIMARY = 382,                 /* PRIMARY  */
    KEY = 383,                     /* KEY  */
    UNIQUE = 384,                  /* UNIQUE  */
    NULLABLE = 385,                /* NULLABLE  */
    IS = 386,                      /* IS  */
    DEFAULT = 387,                 /* DEFAULT  */
    TRUE = 388,                    /* TRUE  */
    FALSE = 389,                   /* FALSE  */
    INTERVAL = 390,                /* INTERVAL  */
    SECOND = 391,                  /* SECOND  */
    SECONDS = 392,                 /* SECONDS  */
    MINUTE = 393,                  /* MINUTE  */
    MINUTES = 394,                 /* MINUTES  */
    HOUR = 395,                    /* HOUR  */
    HOURS = 396,                   /* HOURS  */
    DAY = 397,                     /* DAY  */
    DAYS = 398,                    /* DAYS  */
    MONTH = 399,                   /* MONTH  */
    MONTHS = 400,                  /* MONTHS  */
    YEAR = 401,                    /* YEAR  */
    YEARS = 402,                   /* YEARS  */
    EQUAL = 403,                   /* EQUAL  */
    NOT_EQ = 404,                  /* NOT_EQ  */
    LESS_EQ = 405,                 /* LESS_EQ  */
    GREATER_EQ = 406,              /* GREATER_EQ  */
    BETWEEN = 407,                 /* BETWEEN  */
    AND = 408,                     /* AND  */
    OR = 409,                      /* OR  */
    EXTRACT = 410,                 /* EXTRACT  */
    LIKE = 411,                    /* LIKE  */
    DATA = 412,                    /* DATA  */
    LOG = 413,                     /* LOG  */
    BUFFER = 414,                  /* BUFFER  */
    TRANSACTIONS = 415,            /* TRANSACTIONS  */
    TRANSACTION = 416,             /* TRANSACTION  */
    MEMINDEX = 417,                /* MEMINDEX  */
    USING = 418,                   /* USING  */
    SESSION = 419,                 /* SESSION  */
    GLOBAL = 420,                  /* GLOBAL  */
    OFF = 421,                     /* OFF  */
    EXPORT = 422,                  /* EXPORT  */
    PROFILE = 423,                 /* PROFILE  */
    CONFIGS = 424,                 /* CONFIGS  */
    CONFIG = 425,                  /* CONFIG  */
    PROFILES = 426,                /* PROFILES  */
    VARIABLES = 427,               /* VARIABLES  */
    VARIABLE = 428,                /* VARIABLE  */
    DELTA = 429,                   /* DELTA  */
    LOGS = 430,                    /* LOGS  */
    CATALOGS = 431,                /* CATALOGS  */
    CATALOG = 432,                 /* CATALOG  */
    SEARCH = 433,                  /* SEARCH  */
    MATCH = 434,                   /* MATCH  */
    MAXSIM = 435,                  /* MAXSIM  */
    QUERY = 436,                   /* QUERY  */
    QUERIES = 437,                 /* QUERIES  */
    FUSION = 438,                  /* FUSION  */
    ROWLIMIT = 439,                /* ROWLIMIT  */
    ADMIN = 440,                   /* ADMIN  */
    PERSISTENCE = 441,             /* PERSISTENCE  */
    OBJECT = 442,                  /* OBJECT  */
    OBJECTS = 443,                 /* OBJECTS  */
    FILES = 444,                   /* FILES  */
    NUMBER = 445                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 104 "parser.y"

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
    infinity::AdminStatement* admin_stmt;

    std::vector<infinity::BaseStatement*>* stmt_array;

    std::vector<infinity::TableElement*>*  table_element_array_t;
    infinity::TableElement*           table_element_t;
    infinity::ColumnDef*              table_column_t;
    infinity::ColumnType              column_type_t;
    infinity::ConstraintType          column_constraint_t;
    std::set<infinity::ConstraintType>* column_constraints_t;
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

    infinity::IndexInfo* index_info_t;

    // infinity::IfExistsInfo*        if_exists_info_t;
    infinity::IfNotExistsInfo*     if_not_exists_info_t;

    std::pair<int64_t, int64_t>*    int_sparse_ele_t;
    std::pair<int64_t, double>*     float_sparse_ele_t;

#line 406 "parser.h"

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

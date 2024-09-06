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
    LOCK = 279,                    /* LOCK  */
    UNLOCK = 280,                  /* UNLOCK  */
    EXCEPT = 281,                  /* EXCEPT  */
    FLUSH = 282,                   /* FLUSH  */
    USE = 283,                     /* USE  */
    OPTIMIZE = 284,                /* OPTIMIZE  */
    PROPERTIES = 285,              /* PROPERTIES  */
    DATABASE = 286,                /* DATABASE  */
    TABLE = 287,                   /* TABLE  */
    COLLECTION = 288,              /* COLLECTION  */
    TABLES = 289,                  /* TABLES  */
    INTO = 290,                    /* INTO  */
    VALUES = 291,                  /* VALUES  */
    AST = 292,                     /* AST  */
    PIPELINE = 293,                /* PIPELINE  */
    RAW = 294,                     /* RAW  */
    LOGICAL = 295,                 /* LOGICAL  */
    PHYSICAL = 296,                /* PHYSICAL  */
    FRAGMENT = 297,                /* FRAGMENT  */
    VIEW = 298,                    /* VIEW  */
    INDEX = 299,                   /* INDEX  */
    ANALYZE = 300,                 /* ANALYZE  */
    VIEWS = 301,                   /* VIEWS  */
    DATABASES = 302,               /* DATABASES  */
    SEGMENT = 303,                 /* SEGMENT  */
    SEGMENTS = 304,                /* SEGMENTS  */
    BLOCK = 305,                   /* BLOCK  */
    BLOCKS = 306,                  /* BLOCKS  */
    COLUMN = 307,                  /* COLUMN  */
    COLUMNS = 308,                 /* COLUMNS  */
    INDEXES = 309,                 /* INDEXES  */
    CHUNK = 310,                   /* CHUNK  */
    GROUP = 311,                   /* GROUP  */
    BY = 312,                      /* BY  */
    HAVING = 313,                  /* HAVING  */
    AS = 314,                      /* AS  */
    NATURAL = 315,                 /* NATURAL  */
    JOIN = 316,                    /* JOIN  */
    LEFT = 317,                    /* LEFT  */
    RIGHT = 318,                   /* RIGHT  */
    OUTER = 319,                   /* OUTER  */
    FULL = 320,                    /* FULL  */
    ON = 321,                      /* ON  */
    INNER = 322,                   /* INNER  */
    CROSS = 323,                   /* CROSS  */
    DISTINCT = 324,                /* DISTINCT  */
    WHERE = 325,                   /* WHERE  */
    ORDER = 326,                   /* ORDER  */
    LIMIT = 327,                   /* LIMIT  */
    OFFSET = 328,                  /* OFFSET  */
    ASC = 329,                     /* ASC  */
    DESC = 330,                    /* DESC  */
    IF = 331,                      /* IF  */
    NOT = 332,                     /* NOT  */
    EXISTS = 333,                  /* EXISTS  */
    IN = 334,                      /* IN  */
    FROM = 335,                    /* FROM  */
    TO = 336,                      /* TO  */
    WITH = 337,                    /* WITH  */
    DELIMITER = 338,               /* DELIMITER  */
    FORMAT = 339,                  /* FORMAT  */
    HEADER = 340,                  /* HEADER  */
    CAST = 341,                    /* CAST  */
    END = 342,                     /* END  */
    CASE = 343,                    /* CASE  */
    ELSE = 344,                    /* ELSE  */
    THEN = 345,                    /* THEN  */
    WHEN = 346,                    /* WHEN  */
    BOOLEAN = 347,                 /* BOOLEAN  */
    INTEGER = 348,                 /* INTEGER  */
    INT = 349,                     /* INT  */
    TINYINT = 350,                 /* TINYINT  */
    SMALLINT = 351,                /* SMALLINT  */
    BIGINT = 352,                  /* BIGINT  */
    HUGEINT = 353,                 /* HUGEINT  */
    VARCHAR = 354,                 /* VARCHAR  */
    FLOAT = 355,                   /* FLOAT  */
    DOUBLE = 356,                  /* DOUBLE  */
    REAL = 357,                    /* REAL  */
    DECIMAL = 358,                 /* DECIMAL  */
    DATE = 359,                    /* DATE  */
    TIME = 360,                    /* TIME  */
    DATETIME = 361,                /* DATETIME  */
    FLOAT16 = 362,                 /* FLOAT16  */
    BFLOAT16 = 363,                /* BFLOAT16  */
    UNSIGNED = 364,                /* UNSIGNED  */
    TIMESTAMP = 365,               /* TIMESTAMP  */
    UUID = 366,                    /* UUID  */
    POINT = 367,                   /* POINT  */
    LINE = 368,                    /* LINE  */
    LSEG = 369,                    /* LSEG  */
    BOX = 370,                     /* BOX  */
    PATH = 371,                    /* PATH  */
    POLYGON = 372,                 /* POLYGON  */
    CIRCLE = 373,                  /* CIRCLE  */
    BLOB = 374,                    /* BLOB  */
    BITMAP = 375,                  /* BITMAP  */
    EMBEDDING = 376,               /* EMBEDDING  */
    VECTOR = 377,                  /* VECTOR  */
    BIT = 378,                     /* BIT  */
    TEXT = 379,                    /* TEXT  */
    MULTIVECTOR = 380,             /* MULTIVECTOR  */
    TENSOR = 381,                  /* TENSOR  */
    SPARSE = 382,                  /* SPARSE  */
    TENSORARRAY = 383,             /* TENSORARRAY  */
    IGNORE = 384,                  /* IGNORE  */
    PRIMARY = 385,                 /* PRIMARY  */
    KEY = 386,                     /* KEY  */
    UNIQUE = 387,                  /* UNIQUE  */
    NULLABLE = 388,                /* NULLABLE  */
    IS = 389,                      /* IS  */
    DEFAULT = 390,                 /* DEFAULT  */
    TRUE = 391,                    /* TRUE  */
    FALSE = 392,                   /* FALSE  */
    INTERVAL = 393,                /* INTERVAL  */
    SECOND = 394,                  /* SECOND  */
    SECONDS = 395,                 /* SECONDS  */
    MINUTE = 396,                  /* MINUTE  */
    MINUTES = 397,                 /* MINUTES  */
    HOUR = 398,                    /* HOUR  */
    HOURS = 399,                   /* HOURS  */
    DAY = 400,                     /* DAY  */
    DAYS = 401,                    /* DAYS  */
    MONTH = 402,                   /* MONTH  */
    MONTHS = 403,                  /* MONTHS  */
    YEAR = 404,                    /* YEAR  */
    YEARS = 405,                   /* YEARS  */
    EQUAL = 406,                   /* EQUAL  */
    NOT_EQ = 407,                  /* NOT_EQ  */
    LESS_EQ = 408,                 /* LESS_EQ  */
    GREATER_EQ = 409,              /* GREATER_EQ  */
    BETWEEN = 410,                 /* BETWEEN  */
    AND = 411,                     /* AND  */
    OR = 412,                      /* OR  */
    EXTRACT = 413,                 /* EXTRACT  */
    LIKE = 414,                    /* LIKE  */
    DATA = 415,                    /* DATA  */
    LOG = 416,                     /* LOG  */
    BUFFER = 417,                  /* BUFFER  */
    TRANSACTIONS = 418,            /* TRANSACTIONS  */
    TRANSACTION = 419,             /* TRANSACTION  */
    MEMINDEX = 420,                /* MEMINDEX  */
    USING = 421,                   /* USING  */
    SESSION = 422,                 /* SESSION  */
    GLOBAL = 423,                  /* GLOBAL  */
    OFF = 424,                     /* OFF  */
    EXPORT = 425,                  /* EXPORT  */
    PROFILE = 426,                 /* PROFILE  */
    CONFIGS = 427,                 /* CONFIGS  */
    CONFIG = 428,                  /* CONFIG  */
    PROFILES = 429,                /* PROFILES  */
    VARIABLES = 430,               /* VARIABLES  */
    VARIABLE = 431,                /* VARIABLE  */
    DELTA = 432,                   /* DELTA  */
    LOGS = 433,                    /* LOGS  */
    CATALOGS = 434,                /* CATALOGS  */
    CATALOG = 435,                 /* CATALOG  */
    SEARCH = 436,                  /* SEARCH  */
    MATCH = 437,                   /* MATCH  */
    MAXSIM = 438,                  /* MAXSIM  */
    QUERY = 439,                   /* QUERY  */
    QUERIES = 440,                 /* QUERIES  */
    FUSION = 441,                  /* FUSION  */
    ROWLIMIT = 442,                /* ROWLIMIT  */
    ADMIN = 443,                   /* ADMIN  */
    PERSISTENCE = 444,             /* PERSISTENCE  */
    OBJECT = 445,                  /* OBJECT  */
    OBJECTS = 446,                 /* OBJECTS  */
    FILES = 447,                   /* FILES  */
    MEMORY = 448,                  /* MEMORY  */
    ALLOCATION = 449,              /* ALLOCATION  */
    NUMBER = 450                   /* NUMBER  */
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

#line 411 "parser.h"

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

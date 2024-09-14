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
    ADD = 281,                     /* ADD  */
    RENAME = 282,                  /* RENAME  */
    EXCEPT = 283,                  /* EXCEPT  */
    FLUSH = 284,                   /* FLUSH  */
    USE = 285,                     /* USE  */
    OPTIMIZE = 286,                /* OPTIMIZE  */
    PROPERTIES = 287,              /* PROPERTIES  */
    DATABASE = 288,                /* DATABASE  */
    TABLE = 289,                   /* TABLE  */
    COLLECTION = 290,              /* COLLECTION  */
    TABLES = 291,                  /* TABLES  */
    INTO = 292,                    /* INTO  */
    VALUES = 293,                  /* VALUES  */
    AST = 294,                     /* AST  */
    PIPELINE = 295,                /* PIPELINE  */
    RAW = 296,                     /* RAW  */
    LOGICAL = 297,                 /* LOGICAL  */
    PHYSICAL = 298,                /* PHYSICAL  */
    FRAGMENT = 299,                /* FRAGMENT  */
    VIEW = 300,                    /* VIEW  */
    INDEX = 301,                   /* INDEX  */
    ANALYZE = 302,                 /* ANALYZE  */
    VIEWS = 303,                   /* VIEWS  */
    DATABASES = 304,               /* DATABASES  */
    SEGMENT = 305,                 /* SEGMENT  */
    SEGMENTS = 306,                /* SEGMENTS  */
    BLOCK = 307,                   /* BLOCK  */
    BLOCKS = 308,                  /* BLOCKS  */
    COLUMN = 309,                  /* COLUMN  */
    COLUMNS = 310,                 /* COLUMNS  */
    INDEXES = 311,                 /* INDEXES  */
    CHUNK = 312,                   /* CHUNK  */
    GROUP = 313,                   /* GROUP  */
    BY = 314,                      /* BY  */
    HAVING = 315,                  /* HAVING  */
    AS = 316,                      /* AS  */
    NATURAL = 317,                 /* NATURAL  */
    JOIN = 318,                    /* JOIN  */
    LEFT = 319,                    /* LEFT  */
    RIGHT = 320,                   /* RIGHT  */
    OUTER = 321,                   /* OUTER  */
    FULL = 322,                    /* FULL  */
    ON = 323,                      /* ON  */
    INNER = 324,                   /* INNER  */
    CROSS = 325,                   /* CROSS  */
    DISTINCT = 326,                /* DISTINCT  */
    WHERE = 327,                   /* WHERE  */
    ORDER = 328,                   /* ORDER  */
    LIMIT = 329,                   /* LIMIT  */
    OFFSET = 330,                  /* OFFSET  */
    ASC = 331,                     /* ASC  */
    DESC = 332,                    /* DESC  */
    IF = 333,                      /* IF  */
    NOT = 334,                     /* NOT  */
    EXISTS = 335,                  /* EXISTS  */
    IN = 336,                      /* IN  */
    FROM = 337,                    /* FROM  */
    TO = 338,                      /* TO  */
    WITH = 339,                    /* WITH  */
    DELIMITER = 340,               /* DELIMITER  */
    FORMAT = 341,                  /* FORMAT  */
    HEADER = 342,                  /* HEADER  */
    CAST = 343,                    /* CAST  */
    END = 344,                     /* END  */
    CASE = 345,                    /* CASE  */
    ELSE = 346,                    /* ELSE  */
    THEN = 347,                    /* THEN  */
    WHEN = 348,                    /* WHEN  */
    BOOLEAN = 349,                 /* BOOLEAN  */
    INTEGER = 350,                 /* INTEGER  */
    INT = 351,                     /* INT  */
    TINYINT = 352,                 /* TINYINT  */
    SMALLINT = 353,                /* SMALLINT  */
    BIGINT = 354,                  /* BIGINT  */
    HUGEINT = 355,                 /* HUGEINT  */
    VARCHAR = 356,                 /* VARCHAR  */
    FLOAT = 357,                   /* FLOAT  */
    DOUBLE = 358,                  /* DOUBLE  */
    REAL = 359,                    /* REAL  */
    DECIMAL = 360,                 /* DECIMAL  */
    DATE = 361,                    /* DATE  */
    TIME = 362,                    /* TIME  */
    DATETIME = 363,                /* DATETIME  */
    FLOAT16 = 364,                 /* FLOAT16  */
    BFLOAT16 = 365,                /* BFLOAT16  */
    UNSIGNED = 366,                /* UNSIGNED  */
    TIMESTAMP = 367,               /* TIMESTAMP  */
    UUID = 368,                    /* UUID  */
    POINT = 369,                   /* POINT  */
    LINE = 370,                    /* LINE  */
    LSEG = 371,                    /* LSEG  */
    BOX = 372,                     /* BOX  */
    PATH = 373,                    /* PATH  */
    POLYGON = 374,                 /* POLYGON  */
    CIRCLE = 375,                  /* CIRCLE  */
    BLOB = 376,                    /* BLOB  */
    BITMAP = 377,                  /* BITMAP  */
    EMBEDDING = 378,               /* EMBEDDING  */
    VECTOR = 379,                  /* VECTOR  */
    BIT = 380,                     /* BIT  */
    TEXT = 381,                    /* TEXT  */
    MULTIVECTOR = 382,             /* MULTIVECTOR  */
    TENSOR = 383,                  /* TENSOR  */
    SPARSE = 384,                  /* SPARSE  */
    TENSORARRAY = 385,             /* TENSORARRAY  */
    IGNORE = 386,                  /* IGNORE  */
    PRIMARY = 387,                 /* PRIMARY  */
    KEY = 388,                     /* KEY  */
    UNIQUE = 389,                  /* UNIQUE  */
    NULLABLE = 390,                /* NULLABLE  */
    IS = 391,                      /* IS  */
    DEFAULT = 392,                 /* DEFAULT  */
    TRUE = 393,                    /* TRUE  */
    FALSE = 394,                   /* FALSE  */
    INTERVAL = 395,                /* INTERVAL  */
    SECOND = 396,                  /* SECOND  */
    SECONDS = 397,                 /* SECONDS  */
    MINUTE = 398,                  /* MINUTE  */
    MINUTES = 399,                 /* MINUTES  */
    HOUR = 400,                    /* HOUR  */
    HOURS = 401,                   /* HOURS  */
    DAY = 402,                     /* DAY  */
    DAYS = 403,                    /* DAYS  */
    MONTH = 404,                   /* MONTH  */
    MONTHS = 405,                  /* MONTHS  */
    YEAR = 406,                    /* YEAR  */
    YEARS = 407,                   /* YEARS  */
    EQUAL = 408,                   /* EQUAL  */
    NOT_EQ = 409,                  /* NOT_EQ  */
    LESS_EQ = 410,                 /* LESS_EQ  */
    GREATER_EQ = 411,              /* GREATER_EQ  */
    BETWEEN = 412,                 /* BETWEEN  */
    AND = 413,                     /* AND  */
    OR = 414,                      /* OR  */
    EXTRACT = 415,                 /* EXTRACT  */
    LIKE = 416,                    /* LIKE  */
    DATA = 417,                    /* DATA  */
    LOG = 418,                     /* LOG  */
    BUFFER = 419,                  /* BUFFER  */
    TRANSACTIONS = 420,            /* TRANSACTIONS  */
    TRANSACTION = 421,             /* TRANSACTION  */
    MEMINDEX = 422,                /* MEMINDEX  */
    USING = 423,                   /* USING  */
    SESSION = 424,                 /* SESSION  */
    GLOBAL = 425,                  /* GLOBAL  */
    OFF = 426,                     /* OFF  */
    EXPORT = 427,                  /* EXPORT  */
    PROFILE = 428,                 /* PROFILE  */
    CONFIGS = 429,                 /* CONFIGS  */
    CONFIG = 430,                  /* CONFIG  */
    PROFILES = 431,                /* PROFILES  */
    VARIABLES = 432,               /* VARIABLES  */
    VARIABLE = 433,                /* VARIABLE  */
    DELTA = 434,                   /* DELTA  */
    LOGS = 435,                    /* LOGS  */
    CATALOGS = 436,                /* CATALOGS  */
    CATALOG = 437,                 /* CATALOG  */
    SEARCH = 438,                  /* SEARCH  */
    MATCH = 439,                   /* MATCH  */
    MAXSIM = 440,                  /* MAXSIM  */
    QUERY = 441,                   /* QUERY  */
    QUERIES = 442,                 /* QUERIES  */
    FUSION = 443,                  /* FUSION  */
    ROWLIMIT = 444,                /* ROWLIMIT  */
    ADMIN = 445,                   /* ADMIN  */
    LEADER = 446,                  /* LEADER  */
    FOLLOWER = 447,                /* FOLLOWER  */
    LEARNER = 448,                 /* LEARNER  */
    CONNECT = 449,                 /* CONNECT  */
    STANDALONE = 450,              /* STANDALONE  */
    NODES = 451,                   /* NODES  */
    NODE = 452,                    /* NODE  */
    PERSISTENCE = 453,             /* PERSISTENCE  */
    OBJECT = 454,                  /* OBJECT  */
    OBJECTS = 455,                 /* OBJECTS  */
    FILES = 456,                   /* FILES  */
    MEMORY = 457,                  /* MEMORY  */
    ALLOCATION = 458,              /* ALLOCATION  */
    NUMBER = 459                   /* NUMBER  */
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
    std::vector<infinity::ColumnDef*>*     column_def_array_t;
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

#line 421 "parser.h"

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

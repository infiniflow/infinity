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
    VIEW = 294,                    /* VIEW  */
    INDEX = 295,                   /* INDEX  */
    VIEWS = 296,                   /* VIEWS  */
    DATABASES = 297,               /* DATABASES  */
    SEGMENT = 298,                 /* SEGMENT  */
    SEGMENTS = 299,                /* SEGMENTS  */
    BLOCK = 300,                   /* BLOCK  */
    BLOCKS = 301,                  /* BLOCKS  */
    COLUMN = 302,                  /* COLUMN  */
    COLUMNS = 303,                 /* COLUMNS  */
    INDEXES = 304,                 /* INDEXES  */
    CHUNK = 305,                   /* CHUNK  */
    GROUP = 306,                   /* GROUP  */
    BY = 307,                      /* BY  */
    HAVING = 308,                  /* HAVING  */
    AS = 309,                      /* AS  */
    NATURAL = 310,                 /* NATURAL  */
    JOIN = 311,                    /* JOIN  */
    LEFT = 312,                    /* LEFT  */
    RIGHT = 313,                   /* RIGHT  */
    OUTER = 314,                   /* OUTER  */
    FULL = 315,                    /* FULL  */
    ON = 316,                      /* ON  */
    INNER = 317,                   /* INNER  */
    CROSS = 318,                   /* CROSS  */
    DISTINCT = 319,                /* DISTINCT  */
    WHERE = 320,                   /* WHERE  */
    ORDER = 321,                   /* ORDER  */
    LIMIT = 322,                   /* LIMIT  */
    OFFSET = 323,                  /* OFFSET  */
    ASC = 324,                     /* ASC  */
    DESC = 325,                    /* DESC  */
    IF = 326,                      /* IF  */
    NOT = 327,                     /* NOT  */
    EXISTS = 328,                  /* EXISTS  */
    IN = 329,                      /* IN  */
    FROM = 330,                    /* FROM  */
    TO = 331,                      /* TO  */
    WITH = 332,                    /* WITH  */
    DELIMITER = 333,               /* DELIMITER  */
    FORMAT = 334,                  /* FORMAT  */
    HEADER = 335,                  /* HEADER  */
    HIGHLIGHT = 336,               /* HIGHLIGHT  */
    CAST = 337,                    /* CAST  */
    END = 338,                     /* END  */
    CASE = 339,                    /* CASE  */
    ELSE = 340,                    /* ELSE  */
    THEN = 341,                    /* THEN  */
    WHEN = 342,                    /* WHEN  */
    UNNEST = 343,                  /* UNNEST  */
    BOOLEAN = 344,                 /* BOOLEAN  */
    INTEGER = 345,                 /* INTEGER  */
    INT = 346,                     /* INT  */
    TINYINT = 347,                 /* TINYINT  */
    SMALLINT = 348,                /* SMALLINT  */
    BIGINT = 349,                  /* BIGINT  */
    HUGEINT = 350,                 /* HUGEINT  */
    VARCHAR = 351,                 /* VARCHAR  */
    FLOAT = 352,                   /* FLOAT  */
    DOUBLE = 353,                  /* DOUBLE  */
    REAL = 354,                    /* REAL  */
    DECIMAL = 355,                 /* DECIMAL  */
    DATE = 356,                    /* DATE  */
    TIME = 357,                    /* TIME  */
    DATETIME = 358,                /* DATETIME  */
    FLOAT16 = 359,                 /* FLOAT16  */
    BFLOAT16 = 360,                /* BFLOAT16  */
    UNSIGNED = 361,                /* UNSIGNED  */
    TIMESTAMP = 362,               /* TIMESTAMP  */
    UUID = 363,                    /* UUID  */
    POINT = 364,                   /* POINT  */
    LINE = 365,                    /* LINE  */
    LSEG = 366,                    /* LSEG  */
    BOX = 367,                     /* BOX  */
    PATH = 368,                    /* PATH  */
    POLYGON = 369,                 /* POLYGON  */
    CIRCLE = 370,                  /* CIRCLE  */
    BLOB = 371,                    /* BLOB  */
    BITMAP = 372,                  /* BITMAP  */
    EMBEDDING = 373,               /* EMBEDDING  */
    VECTOR = 374,                  /* VECTOR  */
    BIT = 375,                     /* BIT  */
    TEXT = 376,                    /* TEXT  */
    MULTIVECTOR = 377,             /* MULTIVECTOR  */
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
    COMMENT = 388,                 /* COMMENT  */
    TRUE = 389,                    /* TRUE  */
    FALSE = 390,                   /* FALSE  */
    INTERVAL = 391,                /* INTERVAL  */
    SECOND = 392,                  /* SECOND  */
    SECONDS = 393,                 /* SECONDS  */
    MINUTE = 394,                  /* MINUTE  */
    MINUTES = 395,                 /* MINUTES  */
    HOUR = 396,                    /* HOUR  */
    HOURS = 397,                   /* HOURS  */
    DAY = 398,                     /* DAY  */
    DAYS = 399,                    /* DAYS  */
    MONTH = 400,                   /* MONTH  */
    MONTHS = 401,                  /* MONTHS  */
    YEAR = 402,                    /* YEAR  */
    YEARS = 403,                   /* YEARS  */
    EQUAL = 404,                   /* EQUAL  */
    NOT_EQ = 405,                  /* NOT_EQ  */
    LESS_EQ = 406,                 /* LESS_EQ  */
    GREATER_EQ = 407,              /* GREATER_EQ  */
    BETWEEN = 408,                 /* BETWEEN  */
    AND = 409,                     /* AND  */
    OR = 410,                      /* OR  */
    EXTRACT = 411,                 /* EXTRACT  */
    LIKE = 412,                    /* LIKE  */
    DATA = 413,                    /* DATA  */
    LOG = 414,                     /* LOG  */
    BUFFER = 415,                  /* BUFFER  */
    TRANSACTIONS = 416,            /* TRANSACTIONS  */
    TRANSACTION = 417,             /* TRANSACTION  */
    MEMINDEX = 418,                /* MEMINDEX  */
    USING = 419,                   /* USING  */
    SESSION = 420,                 /* SESSION  */
    GLOBAL = 421,                  /* GLOBAL  */
    OFF = 422,                     /* OFF  */
    EXPORT = 423,                  /* EXPORT  */
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
    LEADER = 441,                  /* LEADER  */
    FOLLOWER = 442,                /* FOLLOWER  */
    LEARNER = 443,                 /* LEARNER  */
    CONNECT = 444,                 /* CONNECT  */
    STANDALONE = 445,              /* STANDALONE  */
    NODES = 446,                   /* NODES  */
    NODE = 447,                    /* NODE  */
    REMOVE = 448,                  /* REMOVE  */
    SNAPSHOT = 449,                /* SNAPSHOT  */
    SNAPSHOTS = 450,               /* SNAPSHOTS  */
    RECOVER = 451,                 /* RECOVER  */
    PERSISTENCE = 452,             /* PERSISTENCE  */
    OBJECT = 453,                  /* OBJECT  */
    OBJECTS = 454,                 /* OBJECTS  */
    FILES = 455,                   /* FILES  */
    MEMORY = 456,                  /* MEMORY  */
    ALLOCATION = 457,              /* ALLOCATION  */
    NUMBER = 458                   /* NUMBER  */
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
    std::vector<infinity::InsertRowExpr*>*    insert_row_list_t;

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

#line 420 "parser.h"

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

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
#include "statement/check_statement.h"
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

#line 122 "parser.h"

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
    ADD = 279,                     /* ADD  */
    RENAME = 280,                  /* RENAME  */
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
    VIEW = 292,                    /* VIEW  */
    INDEX = 293,                   /* INDEX  */
    VIEWS = 294,                   /* VIEWS  */
    DATABASES = 295,               /* DATABASES  */
    SEGMENT = 296,                 /* SEGMENT  */
    SEGMENTS = 297,                /* SEGMENTS  */
    BLOCK = 298,                   /* BLOCK  */
    BLOCKS = 299,                  /* BLOCKS  */
    COLUMN = 300,                  /* COLUMN  */
    COLUMNS = 301,                 /* COLUMNS  */
    INDEXES = 302,                 /* INDEXES  */
    CHUNK = 303,                   /* CHUNK  */
    SYSTEM = 304,                  /* SYSTEM  */
    GROUP = 305,                   /* GROUP  */
    BY = 306,                      /* BY  */
    HAVING = 307,                  /* HAVING  */
    AS = 308,                      /* AS  */
    NATURAL = 309,                 /* NATURAL  */
    JOIN = 310,                    /* JOIN  */
    LEFT = 311,                    /* LEFT  */
    RIGHT = 312,                   /* RIGHT  */
    OUTER = 313,                   /* OUTER  */
    FULL = 314,                    /* FULL  */
    ON = 315,                      /* ON  */
    INNER = 316,                   /* INNER  */
    CROSS = 317,                   /* CROSS  */
    DISTINCT = 318,                /* DISTINCT  */
    WHERE = 319,                   /* WHERE  */
    ORDER = 320,                   /* ORDER  */
    LIMIT = 321,                   /* LIMIT  */
    OFFSET = 322,                  /* OFFSET  */
    ASC = 323,                     /* ASC  */
    DESC = 324,                    /* DESC  */
    IF = 325,                      /* IF  */
    NOT = 326,                     /* NOT  */
    EXISTS = 327,                  /* EXISTS  */
    IN = 328,                      /* IN  */
    FROM = 329,                    /* FROM  */
    TO = 330,                      /* TO  */
    WITH = 331,                    /* WITH  */
    DELIMITER = 332,               /* DELIMITER  */
    FORMAT = 333,                  /* FORMAT  */
    HEADER = 334,                  /* HEADER  */
    HIGHLIGHT = 335,               /* HIGHLIGHT  */
    CAST = 336,                    /* CAST  */
    END = 337,                     /* END  */
    CASE = 338,                    /* CASE  */
    ELSE = 339,                    /* ELSE  */
    THEN = 340,                    /* THEN  */
    WHEN = 341,                    /* WHEN  */
    BOOLEAN = 342,                 /* BOOLEAN  */
    INTEGER = 343,                 /* INTEGER  */
    INT = 344,                     /* INT  */
    TINYINT = 345,                 /* TINYINT  */
    SMALLINT = 346,                /* SMALLINT  */
    BIGINT = 347,                  /* BIGINT  */
    HUGEINT = 348,                 /* HUGEINT  */
    VARCHAR = 349,                 /* VARCHAR  */
    FLOAT = 350,                   /* FLOAT  */
    DOUBLE = 351,                  /* DOUBLE  */
    REAL = 352,                    /* REAL  */
    DECIMAL = 353,                 /* DECIMAL  */
    DATE = 354,                    /* DATE  */
    TIME = 355,                    /* TIME  */
    DATETIME = 356,                /* DATETIME  */
    FLOAT16 = 357,                 /* FLOAT16  */
    BFLOAT16 = 358,                /* BFLOAT16  */
    UNSIGNED = 359,                /* UNSIGNED  */
    TIMESTAMP = 360,               /* TIMESTAMP  */
    UUID = 361,                    /* UUID  */
    POINT = 362,                   /* POINT  */
    LINE = 363,                    /* LINE  */
    LSEG = 364,                    /* LSEG  */
    BOX = 365,                     /* BOX  */
    PATH = 366,                    /* PATH  */
    POLYGON = 367,                 /* POLYGON  */
    CIRCLE = 368,                  /* CIRCLE  */
    BLOB = 369,                    /* BLOB  */
    BITMAP = 370,                  /* BITMAP  */
    ARRAY = 371,                   /* ARRAY  */
    TUPLE = 372,                   /* TUPLE  */
    EMBEDDING = 373,               /* EMBEDDING  */
    VECTOR = 374,                  /* VECTOR  */
    MULTIVECTOR = 375,             /* MULTIVECTOR  */
    TENSOR = 376,                  /* TENSOR  */
    SPARSE = 377,                  /* SPARSE  */
    TENSORARRAY = 378,             /* TENSORARRAY  */
    BIT = 379,                     /* BIT  */
    TEXT = 380,                    /* TEXT  */
    PRIMARY = 381,                 /* PRIMARY  */
    KEY = 382,                     /* KEY  */
    UNIQUE = 383,                  /* UNIQUE  */
    NULLABLE = 384,                /* NULLABLE  */
    IS = 385,                      /* IS  */
    DEFAULT = 386,                 /* DEFAULT  */
    COMMENT = 387,                 /* COMMENT  */
    IGNORE = 388,                  /* IGNORE  */
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
    LOGS = 429,                    /* LOGS  */
    CATALOGS = 430,                /* CATALOGS  */
    CATALOG = 431,                 /* CATALOG  */
    TASKS = 432,                   /* TASKS  */
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
    RESTORE = 452,                 /* RESTORE  */
    PERSISTENCE = 453,             /* PERSISTENCE  */
    OBJECT = 454,                  /* OBJECT  */
    OBJECTS = 455,                 /* OBJECTS  */
    FILES = 456,                   /* FILES  */
    MEMORY = 457,                  /* MEMORY  */
    ALLOCATION = 458,              /* ALLOCATION  */
    HISTORY = 459,                 /* HISTORY  */
    CHECK = 460,                   /* CHECK  */
    NUMBER = 461                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 105 "parser.y"

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
    infinity::CheckStatement* check_stmt;

    std::vector<infinity::BaseStatement*>* stmt_array;

    std::vector<infinity::TableElement*>*  table_element_array_t;
    std::vector<infinity::ColumnDef*>*     column_def_array_t;
    infinity::TableElement*           table_element_t;
    infinity::ColumnDef*              table_column_t;
    infinity::ColumnType*              column_type_t;
    std::vector<std::unique_ptr<infinity::ColumnType>>*     column_type_array_t;
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

#line 426 "parser.h"

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

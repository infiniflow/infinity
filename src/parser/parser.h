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
    SYSTEM = 306,                  /* SYSTEM  */
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
    HIGHLIGHT = 337,               /* HIGHLIGHT  */
    CAST = 338,                    /* CAST  */
    END = 339,                     /* END  */
    CASE = 340,                    /* CASE  */
    ELSE = 341,                    /* ELSE  */
    THEN = 342,                    /* THEN  */
    WHEN = 343,                    /* WHEN  */
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
    ARRAY = 373,                   /* ARRAY  */
    TUPLE = 374,                   /* TUPLE  */
    EMBEDDING = 375,               /* EMBEDDING  */
    VECTOR = 376,                  /* VECTOR  */
    MULTIVECTOR = 377,             /* MULTIVECTOR  */
    TENSOR = 378,                  /* TENSOR  */
    SPARSE = 379,                  /* SPARSE  */
    TENSORARRAY = 380,             /* TENSORARRAY  */
    BIT = 381,                     /* BIT  */
    TEXT = 382,                    /* TEXT  */
    PRIMARY = 383,                 /* PRIMARY  */
    KEY = 384,                     /* KEY  */
    UNIQUE = 385,                  /* UNIQUE  */
    NULLABLE = 386,                /* NULLABLE  */
    IS = 387,                      /* IS  */
    DEFAULT = 388,                 /* DEFAULT  */
    COMMENT = 389,                 /* COMMENT  */
    IGNORE = 390,                  /* IGNORE  */
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
    CONFIGS = 426,                 /* CONFIGS  */
    CONFIG = 427,                  /* CONFIG  */
    PROFILES = 428,                /* PROFILES  */
    VARIABLES = 429,               /* VARIABLES  */
    VARIABLE = 430,                /* VARIABLE  */
    DELTA = 431,                   /* DELTA  */
    LOGS = 432,                    /* LOGS  */
    CATALOGS = 433,                /* CATALOGS  */
    CATALOG = 434,                 /* CATALOG  */
    SEARCH = 435,                  /* SEARCH  */
    MATCH = 436,                   /* MATCH  */
    MAXSIM = 437,                  /* MAXSIM  */
    QUERY = 438,                   /* QUERY  */
    QUERIES = 439,                 /* QUERIES  */
    FUSION = 440,                  /* FUSION  */
    ROWLIMIT = 441,                /* ROWLIMIT  */
    ADMIN = 442,                   /* ADMIN  */
    LEADER = 443,                  /* LEADER  */
    FOLLOWER = 444,                /* FOLLOWER  */
    LEARNER = 445,                 /* LEARNER  */
    CONNECT = 446,                 /* CONNECT  */
    STANDALONE = 447,              /* STANDALONE  */
    NODES = 448,                   /* NODES  */
    NODE = 449,                    /* NODE  */
    REMOVE = 450,                  /* REMOVE  */
    SNAPSHOT = 451,                /* SNAPSHOT  */
    SNAPSHOTS = 452,               /* SNAPSHOTS  */
    RECOVER = 453,                 /* RECOVER  */
    RESTORE = 454,                 /* RESTORE  */
    PERSISTENCE = 455,             /* PERSISTENCE  */
    OBJECT = 456,                  /* OBJECT  */
    OBJECTS = 457,                 /* OBJECTS  */
    FILES = 458,                   /* FILES  */
    MEMORY = 459,                  /* MEMORY  */
    ALLOCATION = 460,              /* ALLOCATION  */
    HISTORY = 461,                 /* HISTORY  */
    NUMBER = 462                   /* NUMBER  */
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

#line 425 "parser.h"

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

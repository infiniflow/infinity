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
    EMBEDDING = 371,               /* EMBEDDING  */
    VECTOR = 372,                  /* VECTOR  */
    BIT = 373,                     /* BIT  */
    TEXT = 374,                    /* TEXT  */
    MULTIVECTOR = 375,             /* MULTIVECTOR  */
    TENSOR = 376,                  /* TENSOR  */
    SPARSE = 377,                  /* SPARSE  */
    TENSORARRAY = 378,             /* TENSORARRAY  */
    IGNORE = 379,                  /* IGNORE  */
    PRIMARY = 380,                 /* PRIMARY  */
    KEY = 381,                     /* KEY  */
    UNIQUE = 382,                  /* UNIQUE  */
    NULLABLE = 383,                /* NULLABLE  */
    IS = 384,                      /* IS  */
    DEFAULT = 385,                 /* DEFAULT  */
    TRUE = 386,                    /* TRUE  */
    FALSE = 387,                   /* FALSE  */
    INTERVAL = 388,                /* INTERVAL  */
    SECOND = 389,                  /* SECOND  */
    SECONDS = 390,                 /* SECONDS  */
    MINUTE = 391,                  /* MINUTE  */
    MINUTES = 392,                 /* MINUTES  */
    HOUR = 393,                    /* HOUR  */
    HOURS = 394,                   /* HOURS  */
    DAY = 395,                     /* DAY  */
    DAYS = 396,                    /* DAYS  */
    MONTH = 397,                   /* MONTH  */
    MONTHS = 398,                  /* MONTHS  */
    YEAR = 399,                    /* YEAR  */
    YEARS = 400,                   /* YEARS  */
    EQUAL = 401,                   /* EQUAL  */
    NOT_EQ = 402,                  /* NOT_EQ  */
    LESS_EQ = 403,                 /* LESS_EQ  */
    GREATER_EQ = 404,              /* GREATER_EQ  */
    BETWEEN = 405,                 /* BETWEEN  */
    AND = 406,                     /* AND  */
    OR = 407,                      /* OR  */
    EXTRACT = 408,                 /* EXTRACT  */
    LIKE = 409,                    /* LIKE  */
    DATA = 410,                    /* DATA  */
    LOG = 411,                     /* LOG  */
    BUFFER = 412,                  /* BUFFER  */
    TRANSACTIONS = 413,            /* TRANSACTIONS  */
    TRANSACTION = 414,             /* TRANSACTION  */
    MEMINDEX = 415,                /* MEMINDEX  */
    USING = 416,                   /* USING  */
    SESSION = 417,                 /* SESSION  */
    GLOBAL = 418,                  /* GLOBAL  */
    OFF = 419,                     /* OFF  */
    EXPORT = 420,                  /* EXPORT  */
    PROFILE = 421,                 /* PROFILE  */
    CONFIGS = 422,                 /* CONFIGS  */
    CONFIG = 423,                  /* CONFIG  */
    PROFILES = 424,                /* PROFILES  */
    VARIABLES = 425,               /* VARIABLES  */
    VARIABLE = 426,                /* VARIABLE  */
    DELTA = 427,                   /* DELTA  */
    LOGS = 428,                    /* LOGS  */
    CATALOGS = 429,                /* CATALOGS  */
    CATALOG = 430,                 /* CATALOG  */
    SEARCH = 431,                  /* SEARCH  */
    MATCH = 432,                   /* MATCH  */
    MAXSIM = 433,                  /* MAXSIM  */
    QUERY = 434,                   /* QUERY  */
    QUERIES = 435,                 /* QUERIES  */
    FUSION = 436,                  /* FUSION  */
    ROWLIMIT = 437,                /* ROWLIMIT  */
    ADMIN = 438,                   /* ADMIN  */
    LEADER = 439,                  /* LEADER  */
    FOLLOWER = 440,                /* FOLLOWER  */
    LEARNER = 441,                 /* LEARNER  */
    CONNECT = 442,                 /* CONNECT  */
    STANDALONE = 443,              /* STANDALONE  */
    NODES = 444,                   /* NODES  */
    NODE = 445,                    /* NODE  */
    PERSISTENCE = 446,             /* PERSISTENCE  */
    OBJECT = 447,                  /* OBJECT  */
    OBJECTS = 448,                 /* OBJECTS  */
    FILES = 449,                   /* FILES  */
    MEMORY = 450,                  /* MEMORY  */
    ALLOCATION = 451,              /* ALLOCATION  */
    NUMBER = 452                   /* NUMBER  */
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

#line 414 "parser.h"

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

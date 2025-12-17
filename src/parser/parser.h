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
#define YY_SQL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef SQLDEBUG
#if defined YYDEBUG
#if YYDEBUG
#define SQLDEBUG 1
#else
#define SQLDEBUG 0
#endif
#else /* ! defined YYDEBUG */
#define SQLDEBUG 1
#endif /* ! defined YYDEBUG */
#endif /* ! defined SQLDEBUG */
#if SQLDEBUG
extern int sqldebug;
#endif
/* "%code requires" blocks.  */
#line 9 "parser.y"

#include "expression.h"
#include "parser_helper.h"
#include "parser_result.h"
#include "statement/admin_statement.h"
#include "statement/alter_statement.h"
#include "statement/check_statement.h"
#include "statement/command_statement.h"
#include "statement/compact_statement.h"
#include "statement/copy_statement.h"
#include "statement/create_statement.h"
#include "statement/delete_statement.h"
#include "statement/drop_statement.h"
#include "statement/execute_statement.h"
#include "statement/explain_statement.h"
#include "statement/flush_statement.h"
#include "statement/insert_statement.h"
#include "statement/optimize_statement.h"
#include "statement/prepare_statement.h"
#include "statement/select_statement.h"
#include "statement/show_statement.h"
#include "statement/update_statement.h"
#include "table_reference/base_table_reference.h"
#include "table_reference/cross_product_reference.h"
#include "table_reference/join_reference.h"
#include "table_reference/subquery_reference.h"
#include "table_reference/table_reference.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

#define YYSTYPE SQLSTYPE
#define YYLTYPE SQLLTYPE

struct SQL_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;
    int string_length;

    std::vector<void *> parameters;
};

#define SQLLTYPE SQL_LTYPE
#define SQLLTYPE_IS_DECLARED 1

#define YY_USER_ACTION                                                                                                                               \
    yylloc->first_line = yylloc->last_line;                                                                                                          \
    yylloc->first_column = yylloc->last_column;                                                                                                      \
    for (int i = 0; yytext[i] != '\0'; ++i) {                                                                                                        \
        yylloc->total_column++;                                                                                                                      \
        yylloc->string_length++;                                                                                                                     \
        if (yytext[i] == '\n') {                                                                                                                     \
            yylloc->last_line++;                                                                                                                     \
            yylloc->last_column = 0;                                                                                                                 \
        } else {                                                                                                                                     \
            yylloc->last_column++;                                                                                                                   \
        }                                                                                                                                            \
    }

#line 124 "parser.h"

/* Token kinds.  */
#ifndef SQLTOKENTYPE
#define SQLTOKENTYPE
enum sqltokentype {
    SQLEMPTY = -2,
    SQLEOF = 0,         /* "end of file"  */
    SQLerror = 256,     /* error  */
    SQLUNDEF = 257,     /* "invalid token"  */
    IDENTIFIER = 258,   /* IDENTIFIER  */
    STRING = 259,       /* STRING  */
    DOUBLE_VALUE = 260, /* DOUBLE_VALUE  */
    LONG_VALUE = 261,   /* LONG_VALUE  */
    CREATE = 262,       /* CREATE  */
    SELECT = 263,       /* SELECT  */
    INSERT = 264,       /* INSERT  */
    DROP = 265,         /* DROP  */
    UPDATE = 266,       /* UPDATE  */
    DELETE = 267,       /* DELETE  */
    COPY = 268,         /* COPY  */
    SET = 269,          /* SET  */
    EXPLAIN = 270,      /* EXPLAIN  */
    SHOW = 271,         /* SHOW  */
    ALTER = 272,        /* ALTER  */
    EXECUTE = 273,      /* EXECUTE  */
    PREPARE = 274,      /* PREPARE  */
    UNION = 275,        /* UNION  */
    ALL = 276,          /* ALL  */
    INTERSECT = 277,    /* INTERSECT  */
    COMPACT = 278,      /* COMPACT  */
    ADD = 279,          /* ADD  */
    RENAME = 280,       /* RENAME  */
    DUMP = 281,         /* DUMP  */
    EXCEPT = 282,       /* EXCEPT  */
    FLUSH = 283,        /* FLUSH  */
    USE = 284,          /* USE  */
    OPTIMIZE = 285,     /* OPTIMIZE  */
    PROPERTIES = 286,   /* PROPERTIES  */
    DATABASE = 287,     /* DATABASE  */
    TABLE = 288,        /* TABLE  */
    COLLECTION = 289,   /* COLLECTION  */
    TABLES = 290,       /* TABLES  */
    INTO = 291,         /* INTO  */
    VALUES = 292,       /* VALUES  */
    VIEW = 293,         /* VIEW  */
    INDEX = 294,        /* INDEX  */
    TASKS = 295,        /* TASKS  */
    DATABASES = 296,    /* DATABASES  */
    SEGMENT = 297,      /* SEGMENT  */
    SEGMENTS = 298,     /* SEGMENTS  */
    BLOCK = 299,        /* BLOCK  */
    BLOCKS = 300,       /* BLOCKS  */
    COLUMN = 301,       /* COLUMN  */
    COLUMNS = 302,      /* COLUMNS  */
    INDEXES = 303,      /* INDEXES  */
    CHUNK = 304,        /* CHUNK  */
    CHUNKS = 305,       /* CHUNKS  */
    SYSTEM = 306,       /* SYSTEM  */
    GROUP = 307,        /* GROUP  */
    BY = 308,           /* BY  */
    HAVING = 309,       /* HAVING  */
    AS = 310,           /* AS  */
    NATURAL = 311,      /* NATURAL  */
    JOIN = 312,         /* JOIN  */
    LEFT = 313,         /* LEFT  */
    RIGHT = 314,        /* RIGHT  */
    OUTER = 315,        /* OUTER  */
    FULL = 316,         /* FULL  */
    ON = 317,           /* ON  */
    INNER = 318,        /* INNER  */
    CROSS = 319,        /* CROSS  */
    DISTINCT = 320,     /* DISTINCT  */
    WHERE = 321,        /* WHERE  */
    ORDER = 322,        /* ORDER  */
    LIMIT = 323,        /* LIMIT  */
    OFFSET = 324,       /* OFFSET  */
    ASC = 325,          /* ASC  */
    DESC = 326,         /* DESC  */
    IF = 327,           /* IF  */
    NOT = 328,          /* NOT  */
    EXISTS = 329,       /* EXISTS  */
    IN = 330,           /* IN  */
    FROM = 331,         /* FROM  */
    TO = 332,           /* TO  */
    WITH = 333,         /* WITH  */
    DELIMITER = 334,    /* DELIMITER  */
    FORMAT = 335,       /* FORMAT  */
    HEADER = 336,       /* HEADER  */
    HIGHLIGHT = 337,    /* HIGHLIGHT  */
    CAST = 338,         /* CAST  */
    END = 339,          /* END  */
    CASE = 340,         /* CASE  */
    ELSE = 341,         /* ELSE  */
    THEN = 342,         /* THEN  */
    WHEN = 343,         /* WHEN  */
    BOOLEAN = 344,      /* BOOLEAN  */
    JSON = 345,         /* JSON  */
    INTEGER = 346,      /* INTEGER  */
    INT = 347,          /* INT  */
    TINYINT = 348,      /* TINYINT  */
    SMALLINT = 349,     /* SMALLINT  */
    BIGINT = 350,       /* BIGINT  */
    HUGEINT = 351,      /* HUGEINT  */
    VARCHAR = 352,      /* VARCHAR  */
    FLOAT = 353,        /* FLOAT  */
    DOUBLE = 354,       /* DOUBLE  */
    REAL = 355,         /* REAL  */
    DECIMAL = 356,      /* DECIMAL  */
    DATE = 357,         /* DATE  */
    TIME = 358,         /* TIME  */
    DATETIME = 359,     /* DATETIME  */
    FLOAT16 = 360,      /* FLOAT16  */
    BFLOAT16 = 361,     /* BFLOAT16  */
    UNSIGNED = 362,     /* UNSIGNED  */
    TIMESTAMP = 363,    /* TIMESTAMP  */
    UUID = 364,         /* UUID  */
    POINT = 365,        /* POINT  */
    LINE = 366,         /* LINE  */
    LSEG = 367,         /* LSEG  */
    BOX = 368,          /* BOX  */
    PATH = 369,         /* PATH  */
    POLYGON = 370,      /* POLYGON  */
    CIRCLE = 371,       /* CIRCLE  */
    BLOB = 372,         /* BLOB  */
    BITMAP = 373,       /* BITMAP  */
    ARRAY = 374,        /* ARRAY  */
    TUPLE = 375,        /* TUPLE  */
    EMBEDDING = 376,    /* EMBEDDING  */
    VECTOR = 377,       /* VECTOR  */
    MULTIVECTOR = 378,  /* MULTIVECTOR  */
    TENSOR = 379,       /* TENSOR  */
    SPARSE = 380,       /* SPARSE  */
    TENSORARRAY = 381,  /* TENSORARRAY  */
    BIT = 382,          /* BIT  */
    TEXT = 383,         /* TEXT  */
    PRIMARY = 384,      /* PRIMARY  */
    KEY = 385,          /* KEY  */
    UNIQUE = 386,       /* UNIQUE  */
    NULLABLE = 387,     /* NULLABLE  */
    IS = 388,           /* IS  */
    DEFAULT = 389,      /* DEFAULT  */
    COMMENT = 390,      /* COMMENT  */
    IGNORE = 391,       /* IGNORE  */
    TRUE = 392,         /* TRUE  */
    FALSE = 393,        /* FALSE  */
    INTERVAL = 394,     /* INTERVAL  */
    SECOND = 395,       /* SECOND  */
    SECONDS = 396,      /* SECONDS  */
    MINUTE = 397,       /* MINUTE  */
    MINUTES = 398,      /* MINUTES  */
    HOUR = 399,         /* HOUR  */
    HOURS = 400,        /* HOURS  */
    DAY = 401,          /* DAY  */
    DAYS = 402,         /* DAYS  */
    MONTH = 403,        /* MONTH  */
    MONTHS = 404,       /* MONTHS  */
    YEAR = 405,         /* YEAR  */
    YEARS = 406,        /* YEARS  */
    JSON_EXTRACT = 407, /* JSON_EXTRACT  */
    EQUAL = 408,        /* EQUAL  */
    NOT_EQ = 409,       /* NOT_EQ  */
    LESS_EQ = 410,      /* LESS_EQ  */
    GREATER_EQ = 411,   /* GREATER_EQ  */
    BETWEEN = 412,      /* BETWEEN  */
    AND = 413,          /* AND  */
    OR = 414,           /* OR  */
    EXTRACT = 415,      /* EXTRACT  */
    LIKE = 416,         /* LIKE  */
    DATA = 417,         /* DATA  */
    LOG = 418,          /* LOG  */
    BUFFER = 419,       /* BUFFER  */
    TRANSACTIONS = 420, /* TRANSACTIONS  */
    TRANSACTION = 421,  /* TRANSACTION  */
    MEMINDEX = 422,     /* MEMINDEX  */
    USING = 423,        /* USING  */
    SESSION = 424,      /* SESSION  */
    GLOBAL = 425,       /* GLOBAL  */
    OFF = 426,          /* OFF  */
    EXPORT = 427,       /* EXPORT  */
    CONFIGS = 428,      /* CONFIGS  */
    CONFIG = 429,       /* CONFIG  */
    PROFILES = 430,     /* PROFILES  */
    VARIABLES = 431,    /* VARIABLES  */
    VARIABLE = 432,     /* VARIABLE  */
    LOGS = 433,         /* LOGS  */
    CATALOGS = 434,     /* CATALOGS  */
    CATALOG = 435,      /* CATALOG  */
    SEARCH = 436,       /* SEARCH  */
    MATCH = 437,        /* MATCH  */
    MAXSIM = 438,       /* MAXSIM  */
    QUERY = 439,        /* QUERY  */
    QUERIES = 440,      /* QUERIES  */
    FUSION = 441,       /* FUSION  */
    ROWLIMIT = 442,     /* ROWLIMIT  */
    ADMIN = 443,        /* ADMIN  */
    LEADER = 444,       /* LEADER  */
    FOLLOWER = 445,     /* FOLLOWER  */
    LEARNER = 446,      /* LEARNER  */
    CONNECT = 447,      /* CONNECT  */
    STANDALONE = 448,   /* STANDALONE  */
    NODES = 449,        /* NODES  */
    NODE = 450,         /* NODE  */
    REMOVE = 451,       /* REMOVE  */
    SNAPSHOT = 452,     /* SNAPSHOT  */
    SNAPSHOTS = 453,    /* SNAPSHOTS  */
    RECOVER = 454,      /* RECOVER  */
    RESTORE = 455,      /* RESTORE  */
    CACHES = 456,       /* CACHES  */
    CACHE = 457,        /* CACHE  */
    PERSISTENCE = 458,  /* PERSISTENCE  */
    OBJECT = 459,       /* OBJECT  */
    OBJECTS = 460,      /* OBJECTS  */
    FILES = 461,        /* FILES  */
    MEMORY = 462,       /* MEMORY  */
    ALLOCATION = 463,   /* ALLOCATION  */
    HISTORY = 464,      /* HISTORY  */
    CHECK = 465,        /* CHECK  */
    CLEAN = 466,        /* CLEAN  */
    CHECKPOINT = 467,   /* CHECKPOINT  */
    IMPORT = 468,       /* IMPORT  */
    NUMBER = 469        /* NUMBER  */
};
typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if !defined SQLSTYPE && !defined SQLSTYPE_IS_DECLARED
union SQLSTYPE {
#line 105 "parser.y"

    bool bool_value;
    char *str_value;
    double double_value;
    int64_t long_value;

    infinity::BaseStatement *base_stmt;
    infinity::SelectStatement *select_stmt;
    infinity::CopyStatement *copy_stmt;
    infinity::InsertStatement *insert_stmt;
    infinity::UpdateStatement *update_stmt;
    infinity::DeleteStatement *delete_stmt;
    infinity::CreateStatement *create_stmt;
    infinity::DropStatement *drop_stmt;
    infinity::PrepareStatement *prepare_stmt;
    infinity::ExecuteStatement *execute_stmt;
    infinity::AlterStatement *alter_stmt;
    infinity::ShowStatement *show_stmt;
    infinity::ExplainStatement *explain_stmt;
    infinity::FlushStatement *flush_stmt;
    infinity::OptimizeStatement *optimize_stmt;
    infinity::CommandStatement *command_stmt;
    infinity::CompactStatement *compact_stmt;
    infinity::AdminStatement *admin_stmt;
    infinity::CheckStatement *check_stmt;

    std::vector<infinity::BaseStatement *> *stmt_array;

    std::vector<infinity::TableElement *> *table_element_array_t;
    std::vector<infinity::ColumnDef *> *column_def_array_t;
    infinity::TableElement *table_element_t;
    infinity::ColumnDef *table_column_t;
    infinity::ColumnType *column_type_t;
    std::vector<std::unique_ptr<infinity::ColumnType>> *column_type_array_t;
    infinity::ConstraintType column_constraint_t;
    std::set<infinity::ConstraintType> *column_constraints_t;
    std::vector<std::string> *identifier_array_t;
    infinity::TableConstraint *table_constraint_t;

    infinity::BaseTableReference *table_reference_t;
    infinity::TableAlias *table_alias_t;
    infinity::JoinType join_type_t;

    infinity::OrderByExpr *order_by_expr_t;
    std::vector<infinity::OrderByExpr *> *order_by_expr_list_t;
    infinity::OrderType order_by_type_t;

    infinity::WithExpr *with_expr_t;
    std::vector<infinity::WithExpr *> *with_expr_list_t;

    infinity::SetOperatorType set_operator_t;

    infinity::ExplainType explain_type_t;

    infinity::ParsedExpr *expr_t;
    infinity::ConstantExpr *const_expr_t;
    std::vector<infinity::ParsedExpr *> *expr_array_t;
    std::vector<infinity::InsertRowExpr *> *insert_row_list_t;

    std::vector<infinity::WhenThen *> *case_check_array_t;

    infinity::UpdateExpr *update_expr_t;
    std::vector<infinity::UpdateExpr *> *update_expr_array_t;

    infinity::TableName *table_name_t;
    infinity::CopyOption *copy_option_t;
    std::vector<infinity::CopyOption *> *copy_option_array;

    infinity::InitParameter *index_param_t;
    std::vector<infinity::InitParameter *> *index_param_list_t;
    std::vector<infinity::InitParameter *> *with_index_param_list_t;

    infinity::IndexInfo *index_info_t;

    // infinity::IfExistsInfo*        if_exists_info_t;
    infinity::IfNotExistsInfo *if_not_exists_info_t;

    std::pair<int64_t, int64_t> *int_sparse_ele_t;
    std::pair<int64_t, double> *float_sparse_ele_t;

#line 436 "parser.h"
};
typedef union SQLSTYPE SQLSTYPE;
#define SQLSTYPE_IS_TRIVIAL 1
#define SQLSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if !defined SQLLTYPE && !defined SQLLTYPE_IS_DECLARED
typedef struct SQLLTYPE SQLLTYPE;
struct SQLLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
#define SQLLTYPE_IS_DECLARED 1
#define SQLLTYPE_IS_TRIVIAL 1
#endif

int sqlparse(void *scanner, infinity::ParserResult *result);

#endif /* !YY_SQL_PARSER_H_INCLUDED  */

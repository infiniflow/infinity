/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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

#line 123 "parser.h"

/* Token type.  */
#ifndef SQLTOKENTYPE
#define SQLTOKENTYPE
enum sqltokentype {
    IDENTIFIER = 258,
    STRING = 259,
    DOUBLE_VALUE = 260,
    LONG_VALUE = 261,
    CREATE = 262,
    SELECT = 263,
    INSERT = 264,
    DROP = 265,
    UPDATE = 266,
    DELETE = 267,
    COPY = 268,
    SET = 269,
    EXPLAIN = 270,
    SHOW = 271,
    ALTER = 272,
    EXECUTE = 273,
    PREPARE = 274,
    UNION = 275,
    ALL = 276,
    INTERSECT = 277,
    COMPACT = 278,
    ADD = 279,
    RENAME = 280,
    DUMP = 281,
    EXCEPT = 282,
    FLUSH = 283,
    USE = 284,
    OPTIMIZE = 285,
    PROPERTIES = 286,
    DATABASE = 287,
    TABLE = 288,
    COLLECTION = 289,
    TABLES = 290,
    INTO = 291,
    VALUES = 292,
    VIEW = 293,
    INDEX = 294,
    TASKS = 295,
    DATABASES = 296,
    SEGMENT = 297,
    SEGMENTS = 298,
    BLOCK = 299,
    BLOCKS = 300,
    COLUMN = 301,
    COLUMNS = 302,
    INDEXES = 303,
    CHUNK = 304,
    CHUNKS = 305,
    SYSTEM = 306,
    GROUP = 307,
    BY = 308,
    HAVING = 309,
    AS = 310,
    NATURAL = 311,
    JOIN = 312,
    LEFT = 313,
    RIGHT = 314,
    OUTER = 315,
    FULL = 316,
    ON = 317,
    INNER = 318,
    CROSS = 319,
    DISTINCT = 320,
    WHERE = 321,
    ORDER = 322,
    LIMIT = 323,
    OFFSET = 324,
    ASC = 325,
    DESC = 326,
    IF = 327,
    NOT = 328,
    EXISTS = 329,
    IN = 330,
    FROM = 331,
    TO = 332,
    WITH = 333,
    DELIMITER = 334,
    FORMAT = 335,
    HEADER = 336,
    HIGHLIGHT = 337,
    CAST = 338,
    END = 339,
    CASE = 340,
    ELSE = 341,
    THEN = 342,
    WHEN = 343,
    BOOLEAN = 344,
    INTEGER = 345,
    INT = 346,
    TINYINT = 347,
    SMALLINT = 348,
    BIGINT = 349,
    HUGEINT = 350,
    VARCHAR = 351,
    FLOAT = 352,
    DOUBLE = 353,
    REAL = 354,
    DECIMAL = 355,
    DATE = 356,
    TIME = 357,
    DATETIME = 358,
    FLOAT16 = 359,
    BFLOAT16 = 360,
    UNSIGNED = 361,
    TIMESTAMP = 362,
    UUID = 363,
    POINT = 364,
    LINE = 365,
    LSEG = 366,
    BOX = 367,
    PATH = 368,
    POLYGON = 369,
    CIRCLE = 370,
    BLOB = 371,
    BITMAP = 372,
    ARRAY = 373,
    TUPLE = 374,
    EMBEDDING = 375,
    VECTOR = 376,
    MULTIVECTOR = 377,
    TENSOR = 378,
    SPARSE = 379,
    TENSORARRAY = 380,
    BIT = 381,
    TEXT = 382,
    PRIMARY = 383,
    KEY = 384,
    UNIQUE = 385,
    NULLABLE = 386,
    IS = 387,
    DEFAULT = 388,
    COMMENT = 389,
    IGNORE = 390,
    TRUE = 391,
    FALSE = 392,
    INTERVAL = 393,
    SECOND = 394,
    SECONDS = 395,
    MINUTE = 396,
    MINUTES = 397,
    HOUR = 398,
    HOURS = 399,
    DAY = 400,
    DAYS = 401,
    MONTH = 402,
    MONTHS = 403,
    YEAR = 404,
    YEARS = 405,
    EQUAL = 406,
    NOT_EQ = 407,
    LESS_EQ = 408,
    GREATER_EQ = 409,
    BETWEEN = 410,
    AND = 411,
    OR = 412,
    EXTRACT = 413,
    LIKE = 414,
    DATA = 415,
    LOG = 416,
    BUFFER = 417,
    TRANSACTIONS = 418,
    TRANSACTION = 419,
    MEMINDEX = 420,
    USING = 421,
    SESSION = 422,
    GLOBAL = 423,
    OFF = 424,
    EXPORT = 425,
    CONFIGS = 426,
    CONFIG = 427,
    PROFILES = 428,
    VARIABLES = 429,
    VARIABLE = 430,
    LOGS = 431,
    CATALOGS = 432,
    CATALOG = 433,
    SEARCH = 434,
    MATCH = 435,
    MAXSIM = 436,
    QUERY = 437,
    QUERIES = 438,
    FUSION = 439,
    ROWLIMIT = 440,
    ADMIN = 441,
    LEADER = 442,
    FOLLOWER = 443,
    LEARNER = 444,
    CONNECT = 445,
    STANDALONE = 446,
    NODES = 447,
    NODE = 448,
    REMOVE = 449,
    SNAPSHOT = 450,
    SNAPSHOTS = 451,
    RECOVER = 452,
    RESTORE = 453,
    CACHES = 454,
    CACHE = 455,
    PERSISTENCE = 456,
    OBJECT = 457,
    OBJECTS = 458,
    FILES = 459,
    MEMORY = 460,
    ALLOCATION = 461,
    HISTORY = 462,
    CHECK = 463,
    CLEAN = 464,
    CHECKPOINT = 465,
    IMPORT = 466,
    NUMBER = 467
};
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

#line 428 "parser.h"
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

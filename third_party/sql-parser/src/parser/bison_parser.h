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

#ifndef YY_HSQL_BISON_PARSER_H_INCLUDED
# define YY_HSQL_BISON_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef HSQL_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define HSQL_DEBUG 1
#  else
#   define HSQL_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define HSQL_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined HSQL_DEBUG */
#if HSQL_DEBUG
extern int hsql_debug;
#endif
/* "%code requires" blocks.  */
#line 38 "bison_parser.y"

  // clang-format on
  // %code requires block

#include "../SQLParserResult.h"
#include "../sql/statements.h"
#include "parser_typedef.h"

// Auto update column and line number
#define YY_USER_ACTION                        \
  yylloc->first_line = yylloc->last_line;     \
  yylloc->first_column = yylloc->last_column; \
  for (int i = 0; yytext[i] != '\0'; i++) {   \
    yylloc->total_column++;                   \
    yylloc->string_length++;                  \
    if (yytext[i] == '\n') {                  \
      yylloc->last_line++;                    \
      yylloc->last_column = 0;                \
    } else {                                  \
      yylloc->last_column++;                  \
    }                                         \
  }

#line 81 "bison_parser.h"

/* Token kinds.  */
#ifndef HSQL_TOKENTYPE
# define HSQL_TOKENTYPE
  enum hsql_tokentype
  {
    SQL_HSQL_EMPTY = -2,
    SQL_YYEOF = 0,                 /* "end of file"  */
    SQL_HSQL_error = 256,          /* error  */
    SQL_HSQL_UNDEF = 257,          /* "invalid token"  */
    SQL_IDENTIFIER = 258,          /* IDENTIFIER  */
    SQL_STRING = 259,              /* STRING  */
    SQL_FLOATVAL = 260,            /* FLOATVAL  */
    SQL_INTVAL = 261,              /* INTVAL  */
    SQL_DEALLOCATE = 262,          /* DEALLOCATE  */
    SQL_PARAMETERS = 263,          /* PARAMETERS  */
    SQL_INTERSECT = 264,           /* INTERSECT  */
    SQL_TEMPORARY = 265,           /* TEMPORARY  */
    SQL_TIMESTAMP = 266,           /* TIMESTAMP  */
    SQL_DISTINCT = 267,            /* DISTINCT  */
    SQL_NVARCHAR = 268,            /* NVARCHAR  */
    SQL_RESTRICT = 269,            /* RESTRICT  */
    SQL_TRUNCATE = 270,            /* TRUNCATE  */
    SQL_ANALYZE = 271,             /* ANALYZE  */
    SQL_BETWEEN = 272,             /* BETWEEN  */
    SQL_CASCADE = 273,             /* CASCADE  */
    SQL_COLUMNS = 274,             /* COLUMNS  */
    SQL_CONTROL = 275,             /* CONTROL  */
    SQL_DEFAULT = 276,             /* DEFAULT  */
    SQL_EXECUTE = 277,             /* EXECUTE  */
    SQL_EXPLAIN = 278,             /* EXPLAIN  */
    SQL_INTEGER = 279,             /* INTEGER  */
    SQL_NATURAL = 280,             /* NATURAL  */
    SQL_PREPARE = 281,             /* PREPARE  */
    SQL_PRIMARY = 282,             /* PRIMARY  */
    SQL_SCHEMAS = 283,             /* SCHEMAS  */
    SQL_CHARACTER_VARYING = 284,   /* CHARACTER_VARYING  */
    SQL_REAL = 285,                /* REAL  */
    SQL_DECIMAL = 286,             /* DECIMAL  */
    SQL_SMALLINT = 287,            /* SMALLINT  */
    SQL_BIGINT = 288,              /* BIGINT  */
    SQL_SPATIAL = 289,             /* SPATIAL  */
    SQL_VARCHAR = 290,             /* VARCHAR  */
    SQL_VIRTUAL = 291,             /* VIRTUAL  */
    SQL_DESCRIBE = 292,            /* DESCRIBE  */
    SQL_BEFORE = 293,              /* BEFORE  */
    SQL_COLUMN = 294,              /* COLUMN  */
    SQL_CREATE = 295,              /* CREATE  */
    SQL_DELETE = 296,              /* DELETE  */
    SQL_DIRECT = 297,              /* DIRECT  */
    SQL_DOUBLE = 298,              /* DOUBLE  */
    SQL_ESCAPE = 299,              /* ESCAPE  */
    SQL_EXCEPT = 300,              /* EXCEPT  */
    SQL_EXISTS = 301,              /* EXISTS  */
    SQL_EXTRACT = 302,             /* EXTRACT  */
    SQL_CAST = 303,                /* CAST  */
    SQL_FORMAT = 304,              /* FORMAT  */
    SQL_GLOBAL = 305,              /* GLOBAL  */
    SQL_HAVING = 306,              /* HAVING  */
    SQL_IMPORT = 307,              /* IMPORT  */
    SQL_INSERT = 308,              /* INSERT  */
    SQL_ISNULL = 309,              /* ISNULL  */
    SQL_OFFSET = 310,              /* OFFSET  */
    SQL_RENAME = 311,              /* RENAME  */
    SQL_SCHEMA = 312,              /* SCHEMA  */
    SQL_SELECT = 313,              /* SELECT  */
    SQL_SORTED = 314,              /* SORTED  */
    SQL_TABLES = 315,              /* TABLES  */
    SQL_UNIQUE = 316,              /* UNIQUE  */
    SQL_UNLOAD = 317,              /* UNLOAD  */
    SQL_UPDATE = 318,              /* UPDATE  */
    SQL_VALUES = 319,              /* VALUES  */
    SQL_AFTER = 320,               /* AFTER  */
    SQL_ALTER = 321,               /* ALTER  */
    SQL_CROSS = 322,               /* CROSS  */
    SQL_DELTA = 323,               /* DELTA  */
    SQL_FLOAT = 324,               /* FLOAT  */
    SQL_GROUP = 325,               /* GROUP  */
    SQL_INDEX = 326,               /* INDEX  */
    SQL_INNER = 327,               /* INNER  */
    SQL_LIMIT = 328,               /* LIMIT  */
    SQL_LOCAL = 329,               /* LOCAL  */
    SQL_MERGE = 330,               /* MERGE  */
    SQL_MINUS = 331,               /* MINUS  */
    SQL_ORDER = 332,               /* ORDER  */
    SQL_OUTER = 333,               /* OUTER  */
    SQL_RIGHT = 334,               /* RIGHT  */
    SQL_TABLE = 335,               /* TABLE  */
    SQL_UNION = 336,               /* UNION  */
    SQL_USING = 337,               /* USING  */
    SQL_WHERE = 338,               /* WHERE  */
    SQL_CALL = 339,                /* CALL  */
    SQL_CASE = 340,                /* CASE  */
    SQL_CHAR = 341,                /* CHAR  */
    SQL_COPY = 342,                /* COPY  */
    SQL_DATE = 343,                /* DATE  */
    SQL_DATETIME = 344,            /* DATETIME  */
    SQL_DESC = 345,                /* DESC  */
    SQL_DROP = 346,                /* DROP  */
    SQL_ELSE = 347,                /* ELSE  */
    SQL_FILE = 348,                /* FILE  */
    SQL_FROM = 349,                /* FROM  */
    SQL_FULL = 350,                /* FULL  */
    SQL_HASH = 351,                /* HASH  */
    SQL_HINT = 352,                /* HINT  */
    SQL_INTO = 353,                /* INTO  */
    SQL_JOIN = 354,                /* JOIN  */
    SQL_LEFT = 355,                /* LEFT  */
    SQL_LIKE = 356,                /* LIKE  */
    SQL_LOAD = 357,                /* LOAD  */
    SQL_LONG = 358,                /* LONG  */
    SQL_NULL = 359,                /* NULL  */
    SQL_PLAN = 360,                /* PLAN  */
    SQL_SHOW = 361,                /* SHOW  */
    SQL_TEXT = 362,                /* TEXT  */
    SQL_THEN = 363,                /* THEN  */
    SQL_TIME = 364,                /* TIME  */
    SQL_VIEW = 365,                /* VIEW  */
    SQL_WHEN = 366,                /* WHEN  */
    SQL_WITH = 367,                /* WITH  */
    SQL_ADD = 368,                 /* ADD  */
    SQL_ALL = 369,                 /* ALL  */
    SQL_AND = 370,                 /* AND  */
    SQL_ASC = 371,                 /* ASC  */
    SQL_END = 372,                 /* END  */
    SQL_FOR = 373,                 /* FOR  */
    SQL_INT = 374,                 /* INT  */
    SQL_KEY = 375,                 /* KEY  */
    SQL_NOT = 376,                 /* NOT  */
    SQL_OFF = 377,                 /* OFF  */
    SQL_SET = 378,                 /* SET  */
    SQL_TOP = 379,                 /* TOP  */
    SQL_AS = 380,                  /* AS  */
    SQL_BY = 381,                  /* BY  */
    SQL_IF = 382,                  /* IF  */
    SQL_IN = 383,                  /* IN  */
    SQL_IS = 384,                  /* IS  */
    SQL_OF = 385,                  /* OF  */
    SQL_ON = 386,                  /* ON  */
    SQL_OR = 387,                  /* OR  */
    SQL_TO = 388,                  /* TO  */
    SQL_NO = 389,                  /* NO  */
    SQL_ARRAY = 390,               /* ARRAY  */
    SQL_CONCAT = 391,              /* CONCAT  */
    SQL_ILIKE = 392,               /* ILIKE  */
    SQL_SECOND = 393,              /* SECOND  */
    SQL_MINUTE = 394,              /* MINUTE  */
    SQL_HOUR = 395,                /* HOUR  */
    SQL_DAY = 396,                 /* DAY  */
    SQL_MONTH = 397,               /* MONTH  */
    SQL_YEAR = 398,                /* YEAR  */
    SQL_SECONDS = 399,             /* SECONDS  */
    SQL_MINUTES = 400,             /* MINUTES  */
    SQL_HOURS = 401,               /* HOURS  */
    SQL_DAYS = 402,                /* DAYS  */
    SQL_MONTHS = 403,              /* MONTHS  */
    SQL_YEARS = 404,               /* YEARS  */
    SQL_INTERVAL = 405,            /* INTERVAL  */
    SQL_TRUE = 406,                /* TRUE  */
    SQL_FALSE = 407,               /* FALSE  */
    SQL_BOOLEAN = 408,             /* BOOLEAN  */
    SQL_TRANSACTION = 409,         /* TRANSACTION  */
    SQL_BEGIN = 410,               /* BEGIN  */
    SQL_COMMIT = 411,              /* COMMIT  */
    SQL_ROLLBACK = 412,            /* ROLLBACK  */
    SQL_NOWAIT = 413,              /* NOWAIT  */
    SQL_SKIP = 414,                /* SKIP  */
    SQL_LOCKED = 415,              /* LOCKED  */
    SQL_SHARE = 416,               /* SHARE  */
    SQL_EQUALS = 417,              /* EQUALS  */
    SQL_NOTEQUALS = 418,           /* NOTEQUALS  */
    SQL_LESS = 419,                /* LESS  */
    SQL_GREATER = 420,             /* GREATER  */
    SQL_LESSEQ = 421,              /* LESSEQ  */
    SQL_GREATEREQ = 422,           /* GREATEREQ  */
    SQL_NOTNULL = 423,             /* NOTNULL  */
    SQL_UMINUS = 424               /* UMINUS  */
  };
  typedef enum hsql_tokentype hsql_token_kind_t;
#endif

/* Value type.  */
#if ! defined HSQL_STYPE && ! defined HSQL_STYPE_IS_DECLARED
union HSQL_STYPE
{
#line 98 "bison_parser.y"

  // clang-format on
  bool bval;
  char* sval;
  double fval;
  int64_t ival;
  uintmax_t uval;

  // statements
  hsql::AlterStatement* alter_stmt;
  hsql::CreateStatement* create_stmt;
  hsql::DeleteStatement* delete_stmt;
  hsql::DropStatement* drop_stmt;
  hsql::ExecuteStatement* exec_stmt;
  hsql::ExportStatement* export_stmt;
  hsql::ImportStatement* import_stmt;
  hsql::InsertStatement* insert_stmt;
  hsql::PrepareStatement* prep_stmt;
  hsql::SelectStatement* select_stmt;
  hsql::ShowStatement* show_stmt;
  hsql::SQLStatement* statement;
  hsql::TransactionStatement* transaction_stmt;
  hsql::UpdateStatement* update_stmt;

  hsql::Alias* alias_t;
  hsql::AlterAction* alter_action_t;
  hsql::ColumnDefinition* column_t;
  hsql::ColumnType column_type_t;
  hsql::ConstraintType column_constraint_t;
  hsql::DatetimeField datetime_field;
  hsql::DropColumnAction* drop_action_t;
  hsql::Expr* expr;
  hsql::GroupByDescription* group_t;
  hsql::ImportType import_type_t;
  hsql::JoinType join_type;
  hsql::LimitDescription* limit;
  hsql::OrderDescription* order;
  hsql::OrderType order_type;
  hsql::SetOperation* set_operator_t;
  hsql::TableConstraint* table_constraint_t;
  hsql::TableElement* table_element_t;
  hsql::TableName table_name;
  hsql::TableRef* table;
  hsql::UpdateClause* update_t;
  hsql::WithDescription* with_description_t;
  hsql::LockingClause* locking_t;

  std::vector<char*>* str_vec;
  std::unordered_set<hsql::ConstraintType>* column_constraint_set;
  std::vector<hsql::Expr*>* expr_vec;
  std::vector<hsql::OrderDescription*>* order_vec;
  std::vector<hsql::SQLStatement*>* stmt_vec;
  std::vector<hsql::TableElement*>* table_element_vec;
  std::vector<hsql::TableRef*>* table_vec;
  std::vector<hsql::UpdateClause*>* update_vec;
  std::vector<hsql::WithDescription*>* with_description_vec;
  std::vector<hsql::LockingClause*>* locking_clause_vec;

  std::pair<int64_t, int64_t>* ival_pair;

  hsql::RowLockMode lock_mode_t;
  hsql::RowLockWaitPolicy lock_wait_policy_t;

#line 331 "bison_parser.h"

};
typedef union HSQL_STYPE HSQL_STYPE;
# define HSQL_STYPE_IS_TRIVIAL 1
# define HSQL_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined HSQL_LTYPE && ! defined HSQL_LTYPE_IS_DECLARED
typedef struct HSQL_LTYPE HSQL_LTYPE;
struct HSQL_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define HSQL_LTYPE_IS_DECLARED 1
# define HSQL_LTYPE_IS_TRIVIAL 1
#endif




int hsql_parse (hsql::SQLParserResult* result, yyscan_t scanner);


#endif /* !YY_HSQL_BISON_PARSER_H_INCLUDED  */

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

#ifndef YY_SQL_HOME_JINHAI_DOCUMENTS_DEVELOPMENT_INFINITY_SRC_PARSER_PARSER_H_INCLUDED
# define YY_SQL_HOME_JINHAI_DOCUMENTS_DEVELOPMENT_INFINITY_SRC_PARSER_PARSER_H_INCLUDED
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
#line 11 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


#include "statement.h"
#include "parser_result.h"
#include "defer_operation.h"
#include "parser/table_reference/join_reference.h"
#include <vector>

using namespace infinity;

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


#line 101 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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
    DESCRIBE = 275,                /* DESCRIBE  */
    SCHEMA = 276,                  /* SCHEMA  */
    TABLE = 277,                   /* TABLE  */
    COLLECTION = 278,              /* COLLECTION  */
    TABLES = 279,                  /* TABLES  */
    GROUP = 280,                   /* GROUP  */
    BY = 281,                      /* BY  */
    HAVING = 282,                  /* HAVING  */
    AS = 283,                      /* AS  */
    NATURAL = 284,                 /* NATURAL  */
    JOIN = 285,                    /* JOIN  */
    LEFT = 286,                    /* LEFT  */
    RIGHT = 287,                   /* RIGHT  */
    OUTER = 288,                   /* OUTER  */
    FULL = 289,                    /* FULL  */
    ON = 290,                      /* ON  */
    INNER = 291,                   /* INNER  */
    CROSS = 292,                   /* CROSS  */
    DISTINCT = 293,                /* DISTINCT  */
    WHERE = 294,                   /* WHERE  */
    IF = 295,                      /* IF  */
    NOT = 296,                     /* NOT  */
    EXISTS = 297,                  /* EXISTS  */
    FROM = 298,                    /* FROM  */
    TO = 299,                      /* TO  */
    WITH = 300,                    /* WITH  */
    DELIMITER = 301,               /* DELIMITER  */
    FORMAT = 302,                  /* FORMAT  */
    HEADER = 303,                  /* HEADER  */
    BOOLEAN = 304,                 /* BOOLEAN  */
    INTEGER = 305,                 /* INTEGER  */
    TINYINT = 306,                 /* TINYINT  */
    SMALLINT = 307,                /* SMALLINT  */
    BIGINT = 308,                  /* BIGINT  */
    HUGEINT = 309,                 /* HUGEINT  */
    CHAR = 310,                    /* CHAR  */
    VARCHAR = 311,                 /* VARCHAR  */
    FLOAT = 312,                   /* FLOAT  */
    DOUBLE = 313,                  /* DOUBLE  */
    REAL = 314,                    /* REAL  */
    DECIMAL = 315,                 /* DECIMAL  */
    DATE = 316,                    /* DATE  */
    TIME = 317,                    /* TIME  */
    DATETIME = 318,                /* DATETIME  */
    TIMESTAMP = 319,               /* TIMESTAMP  */
    UUID = 320,                    /* UUID  */
    POINT = 321,                   /* POINT  */
    LINE = 322,                    /* LINE  */
    LSEG = 323,                    /* LSEG  */
    BOX = 324,                     /* BOX  */
    PATH = 325,                    /* PATH  */
    POLYGON = 326,                 /* POLYGON  */
    CIRCLE = 327,                  /* CIRCLE  */
    BLOB = 328,                    /* BLOB  */
    BITMAP = 329,                  /* BITMAP  */
    EMBEDDING = 330,               /* EMBEDDING  */
    VECTOR = 331,                  /* VECTOR  */
    BIT = 332,                     /* BIT  */
    PRIMARY = 333,                 /* PRIMARY  */
    KEY = 334,                     /* KEY  */
    UNIQUE = 335,                  /* UNIQUE  */
    NULLABLE = 336,                /* NULLABLE  */
    IS = 337,                      /* IS  */
    TRUE = 338,                    /* TRUE  */
    FALSE = 339,                   /* FALSE  */
    INTERVAL = 340,                /* INTERVAL  */
    SECOND = 341,                  /* SECOND  */
    SECONDS = 342,                 /* SECONDS  */
    MINUTE = 343,                  /* MINUTE  */
    MINUTES = 344,                 /* MINUTES  */
    HOUR = 345,                    /* HOUR  */
    HOURS = 346,                   /* HOURS  */
    DAY = 347,                     /* DAY  */
    DAYS = 348,                    /* DAYS  */
    MONTH = 349,                   /* MONTH  */
    MONTHS = 350,                  /* MONTHS  */
    YEAR = 351,                    /* YEAR  */
    YEARS = 352,                   /* YEARS  */
    NUMBER = 353                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 78 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"

    bool    bool_value;
    char*   str_value;
    double  double_value;
    int64_t long_value;

    BaseStatement*   base_stmt;
    SelectStatement* select_stmt;
    CopyStatement*   copy_stmt;
    InsertStatement* insert_stmt;
    UpdateStatement* update_stmt;
    DeleteStatement* delete_stmt;
    CreateStatement* create_stmt;
    DropStatement*   drop_stmt;
    PrepareStatement* prepare_stmt;
    ExecuteStatement* execute_stmt;
    AlterStatement*   alter_stmt;
    ShowStatement*    show_stmt;
    ExplainStatement* explain_stmt;
    SetStatement*     set_stmt;

    Vector<BaseStatement*>* stmt_array;

    Vector<TableElement*>*  table_element_array_t;
    TableElement*           table_element_t;
    ColumnDef*              table_column_t;
    ColumnType              column_type_t;
    ConstraintType          column_constraint_t;
    HashSet<ConstraintType>* column_constraints_t;
    Vector<String>*         identifier_array_t;
    TableConstraint*        table_constraint_t;

    BaseTableReference*     table_reference_t;
    TableAlias*             table_alias_t;
    JoinType                join_type_t;

    ParsedExpr*             parsed_expr_t;
    Vector<ParsedExpr*>*    expr_array_t;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;

#line 260 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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




int sqlparse (void *scanner, ParserResult* result);


#endif /* !YY_SQL_HOME_JINHAI_DOCUMENTS_DEVELOPMENT_INFINITY_SRC_PARSER_PARSER_H_INCLUDED  */

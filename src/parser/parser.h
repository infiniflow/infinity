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


#line 100 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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
    BOOL_VALUE = 262,              /* BOOL_VALUE  */
    CREATE = 263,                  /* CREATE  */
    SELECT = 264,                  /* SELECT  */
    INSERT = 265,                  /* INSERT  */
    DROP = 266,                    /* DROP  */
    UPDATE = 267,                  /* UPDATE  */
    DELETE = 268,                  /* DELETE  */
    COPY = 269,                    /* COPY  */
    SET = 270,                     /* SET  */
    EXPLAIN = 271,                 /* EXPLAIN  */
    SHOW = 272,                    /* SHOW  */
    ALTER = 273,                   /* ALTER  */
    EXECUTE = 274,                 /* EXECUTE  */
    PREPARE = 275,                 /* PREPARE  */
    DESCRIBE = 276,                /* DESCRIBE  */
    SCHEMA = 277,                  /* SCHEMA  */
    TABLE = 278,                   /* TABLE  */
    COLLECTION = 279,              /* COLLECTION  */
    TABLES = 280,                  /* TABLES  */
    GROUP = 281,                   /* GROUP  */
    BY = 282,                      /* BY  */
    HAVING = 283,                  /* HAVING  */
    AS = 284,                      /* AS  */
    NATURAL = 285,                 /* NATURAL  */
    JOIN = 286,                    /* JOIN  */
    LEFT = 287,                    /* LEFT  */
    RIGHT = 288,                   /* RIGHT  */
    OUTER = 289,                   /* OUTER  */
    FULL = 290,                    /* FULL  */
    ON = 291,                      /* ON  */
    INNER = 292,                   /* INNER  */
    CROSS = 293,                   /* CROSS  */
    DISTINCT = 294,                /* DISTINCT  */
    WHERE = 295,                   /* WHERE  */
    IF = 296,                      /* IF  */
    NOT = 297,                     /* NOT  */
    EXISTS = 298,                  /* EXISTS  */
    FROM = 299,                    /* FROM  */
    TO = 300,                      /* TO  */
    WITH = 301,                    /* WITH  */
    DELIMITER = 302,               /* DELIMITER  */
    FORMAT = 303,                  /* FORMAT  */
    HEADER = 304,                  /* HEADER  */
    BOOLEAN = 305,                 /* BOOLEAN  */
    INTEGER = 306,                 /* INTEGER  */
    TINYINT = 307,                 /* TINYINT  */
    SMALLINT = 308,                /* SMALLINT  */
    BIGINT = 309,                  /* BIGINT  */
    HUGEINT = 310,                 /* HUGEINT  */
    CHAR = 311,                    /* CHAR  */
    VARCHAR = 312,                 /* VARCHAR  */
    FLOAT = 313,                   /* FLOAT  */
    DOUBLE = 314,                  /* DOUBLE  */
    REAL = 315,                    /* REAL  */
    DECIMAL = 316,                 /* DECIMAL  */
    DATE = 317,                    /* DATE  */
    TIME = 318,                    /* TIME  */
    DATETIME = 319,                /* DATETIME  */
    TIMESTAMP = 320,               /* TIMESTAMP  */
    UUID = 321,                    /* UUID  */
    POINT = 322,                   /* POINT  */
    LINE = 323,                    /* LINE  */
    LSEG = 324,                    /* LSEG  */
    BOX = 325,                     /* BOX  */
    PATH = 326,                    /* PATH  */
    POLYGON = 327,                 /* POLYGON  */
    CIRCLE = 328,                  /* CIRCLE  */
    BLOB = 329,                    /* BLOB  */
    BITMAP = 330,                  /* BITMAP  */
    EMBEDDING = 331,               /* EMBEDDING  */
    VECTOR = 332,                  /* VECTOR  */
    BIT = 333,                     /* BIT  */
    PRIMARY = 334,                 /* PRIMARY  */
    KEY = 335,                     /* KEY  */
    UNIQUE = 336,                  /* UNIQUE  */
    NULLABLE = 337,                /* NULLABLE  */
    NUMBER = 338                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 77 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"

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

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;

#line 237 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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

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
#include "expression.h"
#include "parser_result.h"
#include "defer_operation.h"
#include "parser/table_reference/join_reference.h"
#include "parser/table_reference/cross_product_reference.h"
#include "parser/table_reference/table_reference.h"
#include "parser/table_reference/subquery_reference.h"

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

#line 105 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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
    UNION = 276,                   /* UNION  */
    ALL = 277,                     /* ALL  */
    INTERSECT = 278,               /* INTERSECT  */
    EXCEPT = 279,                  /* EXCEPT  */
    SCHEMA = 280,                  /* SCHEMA  */
    TABLE = 281,                   /* TABLE  */
    COLLECTION = 282,              /* COLLECTION  */
    TABLES = 283,                  /* TABLES  */
    GROUP = 284,                   /* GROUP  */
    BY = 285,                      /* BY  */
    HAVING = 286,                  /* HAVING  */
    AS = 287,                      /* AS  */
    NATURAL = 288,                 /* NATURAL  */
    JOIN = 289,                    /* JOIN  */
    LEFT = 290,                    /* LEFT  */
    RIGHT = 291,                   /* RIGHT  */
    OUTER = 292,                   /* OUTER  */
    FULL = 293,                    /* FULL  */
    ON = 294,                      /* ON  */
    INNER = 295,                   /* INNER  */
    CROSS = 296,                   /* CROSS  */
    DISTINCT = 297,                /* DISTINCT  */
    WHERE = 298,                   /* WHERE  */
    ORDER = 299,                   /* ORDER  */
    LIMIT = 300,                   /* LIMIT  */
    OFFSET = 301,                  /* OFFSET  */
    ASC = 302,                     /* ASC  */
    DESC = 303,                    /* DESC  */
    IF = 304,                      /* IF  */
    NOT = 305,                     /* NOT  */
    EXISTS = 306,                  /* EXISTS  */
    FROM = 307,                    /* FROM  */
    TO = 308,                      /* TO  */
    WITH = 309,                    /* WITH  */
    DELIMITER = 310,               /* DELIMITER  */
    FORMAT = 311,                  /* FORMAT  */
    HEADER = 312,                  /* HEADER  */
    BOOLEAN = 313,                 /* BOOLEAN  */
    INTEGER = 314,                 /* INTEGER  */
    TINYINT = 315,                 /* TINYINT  */
    SMALLINT = 316,                /* SMALLINT  */
    BIGINT = 317,                  /* BIGINT  */
    HUGEINT = 318,                 /* HUGEINT  */
    CHAR = 319,                    /* CHAR  */
    VARCHAR = 320,                 /* VARCHAR  */
    FLOAT = 321,                   /* FLOAT  */
    DOUBLE = 322,                  /* DOUBLE  */
    REAL = 323,                    /* REAL  */
    DECIMAL = 324,                 /* DECIMAL  */
    DATE = 325,                    /* DATE  */
    TIME = 326,                    /* TIME  */
    DATETIME = 327,                /* DATETIME  */
    TIMESTAMP = 328,               /* TIMESTAMP  */
    UUID = 329,                    /* UUID  */
    POINT = 330,                   /* POINT  */
    LINE = 331,                    /* LINE  */
    LSEG = 332,                    /* LSEG  */
    BOX = 333,                     /* BOX  */
    PATH = 334,                    /* PATH  */
    POLYGON = 335,                 /* POLYGON  */
    CIRCLE = 336,                  /* CIRCLE  */
    BLOB = 337,                    /* BLOB  */
    BITMAP = 338,                  /* BITMAP  */
    EMBEDDING = 339,               /* EMBEDDING  */
    VECTOR = 340,                  /* VECTOR  */
    BIT = 341,                     /* BIT  */
    PRIMARY = 342,                 /* PRIMARY  */
    KEY = 343,                     /* KEY  */
    UNIQUE = 344,                  /* UNIQUE  */
    NULLABLE = 345,                /* NULLABLE  */
    IS = 346,                      /* IS  */
    TRUE = 347,                    /* TRUE  */
    FALSE = 348,                   /* FALSE  */
    INTERVAL = 349,                /* INTERVAL  */
    SECOND = 350,                  /* SECOND  */
    SECONDS = 351,                 /* SECONDS  */
    MINUTE = 352,                  /* MINUTE  */
    MINUTES = 353,                 /* MINUTES  */
    HOUR = 354,                    /* HOUR  */
    HOURS = 355,                   /* HOURS  */
    DAY = 356,                     /* DAY  */
    DAYS = 357,                    /* DAYS  */
    MONTH = 358,                   /* MONTH  */
    MONTHS = 359,                  /* MONTHS  */
    YEAR = 360,                    /* YEAR  */
    YEARS = 361,                   /* YEARS  */
    EQUAL = 362,                   /* EQUAL  */
    NOT_EQ = 363,                  /* NOT_EQ  */
    LESS_EQ = 364,                 /* LESS_EQ  */
    GREATER_EQ = 365,              /* GREATER_EQ  */
    NUMBER = 366,                  /* NUMBER  */
    OR = 367,                      /* OR  */
    AND = 368                      /* AND  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 81 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"

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
    TableAlias *            table_alias_t;
    JoinType                join_type_t;

    OrderByExpr*            order_by_expr_t;
    Vector<OrderByExpr*>*   order_by_expr_list_t;
    OrderType               order_by_type_t;

    WithExpr*               with_expr_t;
    Vector<WithExpr*>*      with_expr_list_t;

    SetOperatorType         set_operator_t;

    ParsedExpr*             expr_t;
    Vector<ParsedExpr*>*    expr_array_t;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;

#line 287 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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

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
    INTO = 284,                    /* INTO  */
    VALUES = 285,                  /* VALUES  */
    AST = 286,                     /* AST  */
    PIPELINE = 287,                /* PIPELINE  */
    UNOPT = 288,                   /* UNOPT  */
    OPT = 289,                     /* OPT  */
    LOGICAL = 290,                 /* LOGICAL  */
    PHYSICAL = 291,                /* PHYSICAL  */
    GROUP = 292,                   /* GROUP  */
    BY = 293,                      /* BY  */
    HAVING = 294,                  /* HAVING  */
    AS = 295,                      /* AS  */
    NATURAL = 296,                 /* NATURAL  */
    JOIN = 297,                    /* JOIN  */
    LEFT = 298,                    /* LEFT  */
    RIGHT = 299,                   /* RIGHT  */
    OUTER = 300,                   /* OUTER  */
    FULL = 301,                    /* FULL  */
    ON = 302,                      /* ON  */
    INNER = 303,                   /* INNER  */
    CROSS = 304,                   /* CROSS  */
    DISTINCT = 305,                /* DISTINCT  */
    WHERE = 306,                   /* WHERE  */
    ORDER = 307,                   /* ORDER  */
    LIMIT = 308,                   /* LIMIT  */
    OFFSET = 309,                  /* OFFSET  */
    ASC = 310,                     /* ASC  */
    DESC = 311,                    /* DESC  */
    IF = 312,                      /* IF  */
    NOT = 313,                     /* NOT  */
    EXISTS = 314,                  /* EXISTS  */
    FROM = 315,                    /* FROM  */
    TO = 316,                      /* TO  */
    WITH = 317,                    /* WITH  */
    DELIMITER = 318,               /* DELIMITER  */
    FORMAT = 319,                  /* FORMAT  */
    HEADER = 320,                  /* HEADER  */
    BOOLEAN = 321,                 /* BOOLEAN  */
    INTEGER = 322,                 /* INTEGER  */
    TINYINT = 323,                 /* TINYINT  */
    SMALLINT = 324,                /* SMALLINT  */
    BIGINT = 325,                  /* BIGINT  */
    HUGEINT = 326,                 /* HUGEINT  */
    CHAR = 327,                    /* CHAR  */
    VARCHAR = 328,                 /* VARCHAR  */
    FLOAT = 329,                   /* FLOAT  */
    DOUBLE = 330,                  /* DOUBLE  */
    REAL = 331,                    /* REAL  */
    DECIMAL = 332,                 /* DECIMAL  */
    DATE = 333,                    /* DATE  */
    TIME = 334,                    /* TIME  */
    DATETIME = 335,                /* DATETIME  */
    TIMESTAMP = 336,               /* TIMESTAMP  */
    UUID = 337,                    /* UUID  */
    POINT = 338,                   /* POINT  */
    LINE = 339,                    /* LINE  */
    LSEG = 340,                    /* LSEG  */
    BOX = 341,                     /* BOX  */
    PATH = 342,                    /* PATH  */
    POLYGON = 343,                 /* POLYGON  */
    CIRCLE = 344,                  /* CIRCLE  */
    BLOB = 345,                    /* BLOB  */
    BITMAP = 346,                  /* BITMAP  */
    EMBEDDING = 347,               /* EMBEDDING  */
    VECTOR = 348,                  /* VECTOR  */
    BIT = 349,                     /* BIT  */
    PRIMARY = 350,                 /* PRIMARY  */
    KEY = 351,                     /* KEY  */
    UNIQUE = 352,                  /* UNIQUE  */
    NULLABLE = 353,                /* NULLABLE  */
    IS = 354,                      /* IS  */
    TRUE = 355,                    /* TRUE  */
    FALSE = 356,                   /* FALSE  */
    INTERVAL = 357,                /* INTERVAL  */
    SECOND = 358,                  /* SECOND  */
    SECONDS = 359,                 /* SECONDS  */
    MINUTE = 360,                  /* MINUTE  */
    MINUTES = 361,                 /* MINUTES  */
    HOUR = 362,                    /* HOUR  */
    HOURS = 363,                   /* HOURS  */
    DAY = 364,                     /* DAY  */
    DAYS = 365,                    /* DAYS  */
    MONTH = 366,                   /* MONTH  */
    MONTHS = 367,                  /* MONTHS  */
    YEAR = 368,                    /* YEAR  */
    YEARS = 369,                   /* YEARS  */
    EQUAL = 370,                   /* EQUAL  */
    NOT_EQ = 371,                  /* NOT_EQ  */
    LESS_EQ = 372,                 /* LESS_EQ  */
    GREATER_EQ = 373,              /* GREATER_EQ  */
    NUMBER = 374,                  /* NUMBER  */
    OR = 375,                      /* OR  */
    AND = 376                      /* AND  */
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

    ExplainType             explain_type_t;

    ParsedExpr*             expr_t;
    Vector<ParsedExpr*>*    expr_array_t;

    UpdateExpr*             update_expr_t;
    Vector<UpdateExpr*>*    update_expr_array_t;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;

#line 300 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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

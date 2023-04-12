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
#include "parser_helper.h"

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

#line 106 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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
    RAW = 288,                     /* RAW  */
    LOGICAL = 289,                 /* LOGICAL  */
    PHYSICAL = 290,                /* PHYSICAL  */
    VIEW = 291,                    /* VIEW  */
    INDEX = 292,                   /* INDEX  */
    ANALYZE = 293,                 /* ANALYZE  */
    VIEWS = 294,                   /* VIEWS  */
    GROUP = 295,                   /* GROUP  */
    BY = 296,                      /* BY  */
    HAVING = 297,                  /* HAVING  */
    AS = 298,                      /* AS  */
    NATURAL = 299,                 /* NATURAL  */
    JOIN = 300,                    /* JOIN  */
    LEFT = 301,                    /* LEFT  */
    RIGHT = 302,                   /* RIGHT  */
    OUTER = 303,                   /* OUTER  */
    FULL = 304,                    /* FULL  */
    ON = 305,                      /* ON  */
    INNER = 306,                   /* INNER  */
    CROSS = 307,                   /* CROSS  */
    DISTINCT = 308,                /* DISTINCT  */
    WHERE = 309,                   /* WHERE  */
    ORDER = 310,                   /* ORDER  */
    LIMIT = 311,                   /* LIMIT  */
    OFFSET = 312,                  /* OFFSET  */
    ASC = 313,                     /* ASC  */
    DESC = 314,                    /* DESC  */
    IF = 315,                      /* IF  */
    NOT = 316,                     /* NOT  */
    EXISTS = 317,                  /* EXISTS  */
    IN = 318,                      /* IN  */
    FROM = 319,                    /* FROM  */
    TO = 320,                      /* TO  */
    WITH = 321,                    /* WITH  */
    DELIMITER = 322,               /* DELIMITER  */
    FORMAT = 323,                  /* FORMAT  */
    HEADER = 324,                  /* HEADER  */
    CAST = 325,                    /* CAST  */
    END = 326,                     /* END  */
    CASE = 327,                    /* CASE  */
    ELSE = 328,                    /* ELSE  */
    THEN = 329,                    /* THEN  */
    WHEN = 330,                    /* WHEN  */
    BOOLEAN = 331,                 /* BOOLEAN  */
    INTEGER = 332,                 /* INTEGER  */
    TINYINT = 333,                 /* TINYINT  */
    SMALLINT = 334,                /* SMALLINT  */
    BIGINT = 335,                  /* BIGINT  */
    HUGEINT = 336,                 /* HUGEINT  */
    CHAR = 337,                    /* CHAR  */
    VARCHAR = 338,                 /* VARCHAR  */
    FLOAT = 339,                   /* FLOAT  */
    DOUBLE = 340,                  /* DOUBLE  */
    REAL = 341,                    /* REAL  */
    DECIMAL = 342,                 /* DECIMAL  */
    DATE = 343,                    /* DATE  */
    TIME = 344,                    /* TIME  */
    DATETIME = 345,                /* DATETIME  */
    TIMESTAMP = 346,               /* TIMESTAMP  */
    UUID = 347,                    /* UUID  */
    POINT = 348,                   /* POINT  */
    LINE = 349,                    /* LINE  */
    LSEG = 350,                    /* LSEG  */
    BOX = 351,                     /* BOX  */
    PATH = 352,                    /* PATH  */
    POLYGON = 353,                 /* POLYGON  */
    CIRCLE = 354,                  /* CIRCLE  */
    BLOB = 355,                    /* BLOB  */
    BITMAP = 356,                  /* BITMAP  */
    EMBEDDING = 357,               /* EMBEDDING  */
    VECTOR = 358,                  /* VECTOR  */
    BIT = 359,                     /* BIT  */
    PRIMARY = 360,                 /* PRIMARY  */
    KEY = 361,                     /* KEY  */
    UNIQUE = 362,                  /* UNIQUE  */
    NULLABLE = 363,                /* NULLABLE  */
    IS = 364,                      /* IS  */
    TRUE = 365,                    /* TRUE  */
    FALSE = 366,                   /* FALSE  */
    INTERVAL = 367,                /* INTERVAL  */
    SECOND = 368,                  /* SECOND  */
    SECONDS = 369,                 /* SECONDS  */
    MINUTE = 370,                  /* MINUTE  */
    MINUTES = 371,                 /* MINUTES  */
    HOUR = 372,                    /* HOUR  */
    HOURS = 373,                   /* HOURS  */
    DAY = 374,                     /* DAY  */
    DAYS = 375,                    /* DAYS  */
    MONTH = 376,                   /* MONTH  */
    MONTHS = 377,                  /* MONTHS  */
    YEAR = 378,                    /* YEAR  */
    YEARS = 379,                   /* YEARS  */
    EQUAL = 380,                   /* EQUAL  */
    NOT_EQ = 381,                  /* NOT_EQ  */
    LESS_EQ = 382,                 /* LESS_EQ  */
    GREATER_EQ = 383,              /* GREATER_EQ  */
    BETWEEN = 384,                 /* BETWEEN  */
    AND = 385,                     /* AND  */
    OR = 386,                      /* OR  */
    EXTRACT = 387,                 /* EXTRACT  */
    LIKE = 388,                    /* LIKE  */
    NUMBER = 389                   /* NUMBER  */
  };
  typedef enum sqltokentype sqltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SQLSTYPE && ! defined SQLSTYPE_IS_DECLARED
union SQLSTYPE
{
#line 82 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"

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

    Vector<WhenThen*>*     case_check_array_t;

    UpdateExpr*             update_expr_t;
    Vector<UpdateExpr*>*    update_expr_array_t;

    TableName* table_name_t;
    CopyOption* copy_option_t;
    Vector<CopyOption*>* copy_option_array;

#line 316 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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

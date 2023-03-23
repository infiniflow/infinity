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
    UNOPT = 288,                   /* UNOPT  */
    OPT = 289,                     /* OPT  */
    LOGICAL = 290,                 /* LOGICAL  */
    PHYSICAL = 291,                /* PHYSICAL  */
    VIEW = 292,                    /* VIEW  */
    INDEX = 293,                   /* INDEX  */
    ANALYZE = 294,                 /* ANALYZE  */
    VIEWS = 295,                   /* VIEWS  */
    GROUP = 296,                   /* GROUP  */
    BY = 297,                      /* BY  */
    HAVING = 298,                  /* HAVING  */
    AS = 299,                      /* AS  */
    NATURAL = 300,                 /* NATURAL  */
    JOIN = 301,                    /* JOIN  */
    LEFT = 302,                    /* LEFT  */
    RIGHT = 303,                   /* RIGHT  */
    OUTER = 304,                   /* OUTER  */
    FULL = 305,                    /* FULL  */
    ON = 306,                      /* ON  */
    INNER = 307,                   /* INNER  */
    CROSS = 308,                   /* CROSS  */
    DISTINCT = 309,                /* DISTINCT  */
    WHERE = 310,                   /* WHERE  */
    ORDER = 311,                   /* ORDER  */
    LIMIT = 312,                   /* LIMIT  */
    OFFSET = 313,                  /* OFFSET  */
    ASC = 314,                     /* ASC  */
    DESC = 315,                    /* DESC  */
    IF = 316,                      /* IF  */
    NOT = 317,                     /* NOT  */
    EXISTS = 318,                  /* EXISTS  */
    IN = 319,                      /* IN  */
    FROM = 320,                    /* FROM  */
    TO = 321,                      /* TO  */
    WITH = 322,                    /* WITH  */
    DELIMITER = 323,               /* DELIMITER  */
    FORMAT = 324,                  /* FORMAT  */
    HEADER = 325,                  /* HEADER  */
    CAST = 326,                    /* CAST  */
    END = 327,                     /* END  */
    CASE = 328,                    /* CASE  */
    ELSE = 329,                    /* ELSE  */
    THEN = 330,                    /* THEN  */
    WHEN = 331,                    /* WHEN  */
    BOOLEAN = 332,                 /* BOOLEAN  */
    INTEGER = 333,                 /* INTEGER  */
    TINYINT = 334,                 /* TINYINT  */
    SMALLINT = 335,                /* SMALLINT  */
    BIGINT = 336,                  /* BIGINT  */
    HUGEINT = 337,                 /* HUGEINT  */
    CHAR = 338,                    /* CHAR  */
    VARCHAR = 339,                 /* VARCHAR  */
    FLOAT = 340,                   /* FLOAT  */
    DOUBLE = 341,                  /* DOUBLE  */
    REAL = 342,                    /* REAL  */
    DECIMAL = 343,                 /* DECIMAL  */
    DATE = 344,                    /* DATE  */
    TIME = 345,                    /* TIME  */
    DATETIME = 346,                /* DATETIME  */
    TIMESTAMP = 347,               /* TIMESTAMP  */
    UUID = 348,                    /* UUID  */
    POINT = 349,                   /* POINT  */
    LINE = 350,                    /* LINE  */
    LSEG = 351,                    /* LSEG  */
    BOX = 352,                     /* BOX  */
    PATH = 353,                    /* PATH  */
    POLYGON = 354,                 /* POLYGON  */
    CIRCLE = 355,                  /* CIRCLE  */
    BLOB = 356,                    /* BLOB  */
    BITMAP = 357,                  /* BITMAP  */
    EMBEDDING = 358,               /* EMBEDDING  */
    VECTOR = 359,                  /* VECTOR  */
    BIT = 360,                     /* BIT  */
    PRIMARY = 361,                 /* PRIMARY  */
    KEY = 362,                     /* KEY  */
    UNIQUE = 363,                  /* UNIQUE  */
    NULLABLE = 364,                /* NULLABLE  */
    IS = 365,                      /* IS  */
    TRUE = 366,                    /* TRUE  */
    FALSE = 367,                   /* FALSE  */
    INTERVAL = 368,                /* INTERVAL  */
    SECOND = 369,                  /* SECOND  */
    SECONDS = 370,                 /* SECONDS  */
    MINUTE = 371,                  /* MINUTE  */
    MINUTES = 372,                 /* MINUTES  */
    HOUR = 373,                    /* HOUR  */
    HOURS = 374,                   /* HOURS  */
    DAY = 375,                     /* DAY  */
    DAYS = 376,                    /* DAYS  */
    MONTH = 377,                   /* MONTH  */
    MONTHS = 378,                  /* MONTHS  */
    YEAR = 379,                    /* YEAR  */
    YEARS = 380,                   /* YEARS  */
    EQUAL = 381,                   /* EQUAL  */
    NOT_EQ = 382,                  /* NOT_EQ  */
    LESS_EQ = 383,                 /* LESS_EQ  */
    GREATER_EQ = 384,              /* GREATER_EQ  */
    BETWEEN = 385,                 /* BETWEEN  */
    AND = 386,                     /* AND  */
    OR = 387,                      /* OR  */
    EXTRACT = 388,                 /* EXTRACT  */
    LIKE = 389,                    /* LIKE  */
    NUMBER = 390                   /* NUMBER  */
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

#line 317 "/home/jinhai/Documents/development/infinity/src/parser/parser.h"

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

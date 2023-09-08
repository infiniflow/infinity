/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         SQLSTYPE
#define YYLTYPE         SQLLTYPE
/* Substitute the variable and function names.  */
#define yyparse         sqlparse
#define yylex           sqllex
#define yyerror         sqlerror
#define yydebug         sqldebug
#define yynerrs         sqlnerrs

/* First part of user prologue.  */
#line 2 "/home/tangdh/workspace/infinity/src/parser/parser.y"

#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg);

#line 87 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_DOUBLE_VALUE = 5,               /* DOUBLE_VALUE  */
  YYSYMBOL_LONG_VALUE = 6,                 /* LONG_VALUE  */
  YYSYMBOL_CREATE = 7,                     /* CREATE  */
  YYSYMBOL_SELECT = 8,                     /* SELECT  */
  YYSYMBOL_INSERT = 9,                     /* INSERT  */
  YYSYMBOL_DROP = 10,                      /* DROP  */
  YYSYMBOL_UPDATE = 11,                    /* UPDATE  */
  YYSYMBOL_DELETE = 12,                    /* DELETE  */
  YYSYMBOL_COPY = 13,                      /* COPY  */
  YYSYMBOL_SET = 14,                       /* SET  */
  YYSYMBOL_EXPLAIN = 15,                   /* EXPLAIN  */
  YYSYMBOL_SHOW = 16,                      /* SHOW  */
  YYSYMBOL_ALTER = 17,                     /* ALTER  */
  YYSYMBOL_EXECUTE = 18,                   /* EXECUTE  */
  YYSYMBOL_PREPARE = 19,                   /* PREPARE  */
  YYSYMBOL_DESCRIBE = 20,                  /* DESCRIBE  */
  YYSYMBOL_UNION = 21,                     /* UNION  */
  YYSYMBOL_ALL = 22,                       /* ALL  */
  YYSYMBOL_INTERSECT = 23,                 /* INTERSECT  */
  YYSYMBOL_EXCEPT = 24,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 25,                     /* FLUSH  */
  YYSYMBOL_SCHEMA = 26,                    /* SCHEMA  */
  YYSYMBOL_TABLE = 27,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 28,                /* COLLECTION  */
  YYSYMBOL_TABLES = 29,                    /* TABLES  */
  YYSYMBOL_INTO = 30,                      /* INTO  */
  YYSYMBOL_VALUES = 31,                    /* VALUES  */
  YYSYMBOL_AST = 32,                       /* AST  */
  YYSYMBOL_PIPELINE = 33,                  /* PIPELINE  */
  YYSYMBOL_RAW = 34,                       /* RAW  */
  YYSYMBOL_LOGICAL = 35,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 36,                  /* PHYSICAL  */
  YYSYMBOL_VIEW = 37,                      /* VIEW  */
  YYSYMBOL_INDEX = 38,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 39,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 40,                     /* VIEWS  */
  YYSYMBOL_GROUP = 41,                     /* GROUP  */
  YYSYMBOL_BY = 42,                        /* BY  */
  YYSYMBOL_HAVING = 43,                    /* HAVING  */
  YYSYMBOL_AS = 44,                        /* AS  */
  YYSYMBOL_NATURAL = 45,                   /* NATURAL  */
  YYSYMBOL_JOIN = 46,                      /* JOIN  */
  YYSYMBOL_LEFT = 47,                      /* LEFT  */
  YYSYMBOL_RIGHT = 48,                     /* RIGHT  */
  YYSYMBOL_OUTER = 49,                     /* OUTER  */
  YYSYMBOL_FULL = 50,                      /* FULL  */
  YYSYMBOL_ON = 51,                        /* ON  */
  YYSYMBOL_INNER = 52,                     /* INNER  */
  YYSYMBOL_CROSS = 53,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 54,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 55,                     /* WHERE  */
  YYSYMBOL_ORDER = 56,                     /* ORDER  */
  YYSYMBOL_LIMIT = 57,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 58,                    /* OFFSET  */
  YYSYMBOL_ASC = 59,                       /* ASC  */
  YYSYMBOL_DESC = 60,                      /* DESC  */
  YYSYMBOL_IF = 61,                        /* IF  */
  YYSYMBOL_NOT = 62,                       /* NOT  */
  YYSYMBOL_EXISTS = 63,                    /* EXISTS  */
  YYSYMBOL_IN = 64,                        /* IN  */
  YYSYMBOL_FROM = 65,                      /* FROM  */
  YYSYMBOL_TO = 66,                        /* TO  */
  YYSYMBOL_WITH = 67,                      /* WITH  */
  YYSYMBOL_DELIMITER = 68,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 69,                    /* FORMAT  */
  YYSYMBOL_HEADER = 70,                    /* HEADER  */
  YYSYMBOL_CAST = 71,                      /* CAST  */
  YYSYMBOL_END = 72,                       /* END  */
  YYSYMBOL_CASE = 73,                      /* CASE  */
  YYSYMBOL_ELSE = 74,                      /* ELSE  */
  YYSYMBOL_THEN = 75,                      /* THEN  */
  YYSYMBOL_WHEN = 76,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 77,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 78,                   /* INTEGER  */
  YYSYMBOL_INT = 79,                       /* INT  */
  YYSYMBOL_TINYINT = 80,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 81,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 82,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 83,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 84,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 85,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 86,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 87,                    /* DOUBLE  */
  YYSYMBOL_REAL = 88,                      /* REAL  */
  YYSYMBOL_DECIMAL = 89,                   /* DECIMAL  */
  YYSYMBOL_DATE = 90,                      /* DATE  */
  YYSYMBOL_TIME = 91,                      /* TIME  */
  YYSYMBOL_DATETIME = 92,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 93,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 94,                      /* UUID  */
  YYSYMBOL_POINT = 95,                     /* POINT  */
  YYSYMBOL_LINE = 96,                      /* LINE  */
  YYSYMBOL_LSEG = 97,                      /* LSEG  */
  YYSYMBOL_BOX = 98,                       /* BOX  */
  YYSYMBOL_PATH = 99,                      /* PATH  */
  YYSYMBOL_POLYGON = 100,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 101,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 102,                     /* BLOB  */
  YYSYMBOL_BITMAP = 103,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 104,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 105,                   /* VECTOR  */
  YYSYMBOL_BIT = 106,                      /* BIT  */
  YYSYMBOL_PRIMARY = 107,                  /* PRIMARY  */
  YYSYMBOL_KEY = 108,                      /* KEY  */
  YYSYMBOL_UNIQUE = 109,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 110,                 /* NULLABLE  */
  YYSYMBOL_IS = 111,                       /* IS  */
  YYSYMBOL_TRUE = 112,                     /* TRUE  */
  YYSYMBOL_FALSE = 113,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 114,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 115,                   /* SECOND  */
  YYSYMBOL_SECONDS = 116,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 117,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 118,                  /* MINUTES  */
  YYSYMBOL_HOUR = 119,                     /* HOUR  */
  YYSYMBOL_HOURS = 120,                    /* HOURS  */
  YYSYMBOL_DAY = 121,                      /* DAY  */
  YYSYMBOL_DAYS = 122,                     /* DAYS  */
  YYSYMBOL_MONTH = 123,                    /* MONTH  */
  YYSYMBOL_MONTHS = 124,                   /* MONTHS  */
  YYSYMBOL_YEAR = 125,                     /* YEAR  */
  YYSYMBOL_YEARS = 126,                    /* YEARS  */
  YYSYMBOL_EQUAL = 127,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 128,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 129,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 130,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 131,                  /* BETWEEN  */
  YYSYMBOL_AND = 132,                      /* AND  */
  YYSYMBOL_OR = 133,                       /* OR  */
  YYSYMBOL_EXTRACT = 134,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 135,                     /* LIKE  */
  YYSYMBOL_DATA = 136,                     /* DATA  */
  YYSYMBOL_LOG = 137,                      /* LOG  */
  YYSYMBOL_BUFFER = 138,                   /* BUFFER  */
  YYSYMBOL_NUMBER = 139,                   /* NUMBER  */
  YYSYMBOL_140_ = 140,                     /* '='  */
  YYSYMBOL_141_ = 141,                     /* '<'  */
  YYSYMBOL_142_ = 142,                     /* '>'  */
  YYSYMBOL_143_ = 143,                     /* '+'  */
  YYSYMBOL_144_ = 144,                     /* '-'  */
  YYSYMBOL_145_ = 145,                     /* '*'  */
  YYSYMBOL_146_ = 146,                     /* '/'  */
  YYSYMBOL_147_ = 147,                     /* '%'  */
  YYSYMBOL_148_ = 148,                     /* '['  */
  YYSYMBOL_149_ = 149,                     /* ']'  */
  YYSYMBOL_150_ = 150,                     /* '('  */
  YYSYMBOL_151_ = 151,                     /* ')'  */
  YYSYMBOL_152_ = 152,                     /* '.'  */
  YYSYMBOL_153_ = 153,                     /* ';'  */
  YYSYMBOL_154_ = 154,                     /* ','  */
  YYSYMBOL_YYACCEPT = 155,                 /* $accept  */
  YYSYMBOL_input_pattern = 156,            /* input_pattern  */
  YYSYMBOL_statement_list = 157,           /* statement_list  */
  YYSYMBOL_statement = 158,                /* statement  */
  YYSYMBOL_explainable_statement = 159,    /* explainable_statement  */
  YYSYMBOL_create_statement = 160,         /* create_statement  */
  YYSYMBOL_table_element_array = 161,      /* table_element_array  */
  YYSYMBOL_table_element = 162,            /* table_element  */
  YYSYMBOL_table_column = 163,             /* table_column  */
  YYSYMBOL_column_type = 164,              /* column_type  */
  YYSYMBOL_column_constraints = 165,       /* column_constraints  */
  YYSYMBOL_column_constraint = 166,        /* column_constraint  */
  YYSYMBOL_table_constraint = 167,         /* table_constraint  */
  YYSYMBOL_identifier_array = 168,         /* identifier_array  */
  YYSYMBOL_delete_statement = 169,         /* delete_statement  */
  YYSYMBOL_insert_statement = 170,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 171, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 172,        /* explain_statement  */
  YYSYMBOL_explain_type = 173,             /* explain_type  */
  YYSYMBOL_update_statement = 174,         /* update_statement  */
  YYSYMBOL_update_expr_array = 175,        /* update_expr_array  */
  YYSYMBOL_update_expr = 176,              /* update_expr  */
  YYSYMBOL_drop_statement = 177,           /* drop_statement  */
  YYSYMBOL_copy_statement = 178,           /* copy_statement  */
  YYSYMBOL_select_statement = 179,         /* select_statement  */
  YYSYMBOL_select_with_paren = 180,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 181,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 182, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 183, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 184, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 185,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 186,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 187,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 188,            /* order_by_type  */
  YYSYMBOL_limit_expr = 189,               /* limit_expr  */
  YYSYMBOL_offset_expr = 190,              /* offset_expr  */
  YYSYMBOL_distinct = 191,                 /* distinct  */
  YYSYMBOL_from_clause = 192,              /* from_clause  */
  YYSYMBOL_where_clause = 193,             /* where_clause  */
  YYSYMBOL_having_clause = 194,            /* having_clause  */
  YYSYMBOL_group_by_clause = 195,          /* group_by_clause  */
  YYSYMBOL_set_operator = 196,             /* set_operator  */
  YYSYMBOL_table_reference = 197,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 198,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 199,     /* table_reference_name  */
  YYSYMBOL_table_name = 200,               /* table_name  */
  YYSYMBOL_table_alias = 201,              /* table_alias  */
  YYSYMBOL_with_clause = 202,              /* with_clause  */
  YYSYMBOL_with_expr_list = 203,           /* with_expr_list  */
  YYSYMBOL_with_expr = 204,                /* with_expr  */
  YYSYMBOL_join_clause = 205,              /* join_clause  */
  YYSYMBOL_join_type = 206,                /* join_type  */
  YYSYMBOL_show_statement = 207,           /* show_statement  */
  YYSYMBOL_flush_statement = 208,          /* flush_statement  */
  YYSYMBOL_expr_array = 209,               /* expr_array  */
  YYSYMBOL_expr_alias = 210,               /* expr_alias  */
  YYSYMBOL_expr = 211,                     /* expr  */
  YYSYMBOL_operand = 212,                  /* operand  */
  YYSYMBOL_function_expr = 213,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 214,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 215,             /* between_expr  */
  YYSYMBOL_in_expr = 216,                  /* in_expr  */
  YYSYMBOL_case_expr = 217,                /* case_expr  */
  YYSYMBOL_case_check_array = 218,         /* case_check_array  */
  YYSYMBOL_cast_expr = 219,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 220,            /* subquery_expr  */
  YYSYMBOL_column_expr = 221,              /* column_expr  */
  YYSYMBOL_constant_expr = 222,            /* constant_expr  */
  YYSYMBOL_interval_expr = 223,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 224,         /* copy_option_list  */
  YYSYMBOL_copy_option = 225,              /* copy_option  */
  YYSYMBOL_file_path = 226,                /* file_path  */
  YYSYMBOL_if_exists = 227,                /* if_exists  */
  YYSYMBOL_if_not_exists = 228,            /* if_not_exists  */
  YYSYMBOL_semicolon = 229                 /* semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL \
             && defined SQLSTYPE_IS_TRIVIAL && SQLSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  62
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   693

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  155
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  75
/* YYNRULES -- Number of rules.  */
#define YYNRULES  276
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  543

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   394


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   147,     2,     2,
     150,   151,   145,   143,   154,   144,   152,   146,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   153,
     141,   140,   142,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   148,     2,   149,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   370,   370,   374,   380,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   398,   399,   400,   401,   402,
     403,   404,   405,   406,   413,   426,   441,   465,   481,   498,
<<<<<<< HEAD
     518,   522,   528,   531,   537,   572,   606,   607,   608,   609,
     610,   611,   612,   613,   614,   615,   616,   617,   618,   619,
     620,   621,   622,   623,   624,   625,   626,   627,   629,   630,
     631,   632,   633,   634,   635,   636,   637,   638,   639,   640,
     641,   642,   643,   644,   645,   646,   647,   648,   649,   650,
     651,   652,   653,   672,   676,   686,   689,   692,   695,   699,
     704,   711,   717,   727,   743,   755,   768,   771,   778,   784,
     787,   790,   793,   796,   799,   802,   809,   822,   826,   831,
     844,   857,   872,   887,   902,   921,   962,  1007,  1010,  1013,
    1022,  1032,  1035,  1039,  1044,  1051,  1054,  1059,  1074,  1077,
    1081,  1085,  1090,  1096,  1099,  1102,  1106,  1110,  1112,  1116,
    1118,  1121,  1125,  1128,  1132,  1135,  1139,  1142,  1146,  1149,
    1153,  1156,  1159,  1162,  1170,  1173,  1188,  1188,  1190,  1204,
    1213,  1220,  1231,  1236,  1241,  1247,  1254,  1257,  1261,  1264,
    1269,  1281,  1288,  1302,  1305,  1308,  1311,  1314,  1317,  1320,
    1326,  1330,  1334,  1349,  1353,  1357,  1366,  1370,  1386,  1392,
    1396,  1397,  1398,  1399,  1400,  1402,  1405,  1411,  1412,  1413,
    1414,  1415,  1417,  1425,  1433,  1442,  1449,  1456,  1463,  1470,
    1477,  1485,  1493,  1501,  1509,  1517,  1525,  1533,  1541,  1549,
    1557,  1565,  1573,  1602,  1610,  1619,  1627,  1636,  1644,  1650,
    1657,  1663,  1670,  1675,  1682,  1689,  1697,  1721,  1727,  1733,
    1740,  1748,  1755,  1762,  1767,  1777,  1782,  1787,  1792,  1797,
    1802,  1807,  1810,  1814,  1820,  1826,  1832,  1838,  1844,  1850,
    1856,  1862,  1868,  1874,  1880,  1891,  1895,  1900,  1916,  1922,
    1928,  1932,  1933,  1935,  1936,  1938,  1939
=======
     518,   522,   528,   531,   537,   569,   600,   601,   602,   603,
     604,   605,   606,   607,   608,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,   619,   620,   621,   623,   624,
     625,   626,   627,   628,   629,   630,   631,   632,   633,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   666,   670,   680,   683,   686,   689,   693,
     698,   705,   711,   721,   737,   749,   762,   765,   772,   778,
     781,   784,   787,   790,   793,   796,   803,   816,   820,   825,
     838,   851,   866,   881,   896,   915,   956,  1001,  1004,  1007,
    1016,  1026,  1029,  1033,  1038,  1045,  1048,  1053,  1068,  1071,
    1075,  1079,  1084,  1090,  1093,  1096,  1100,  1104,  1106,  1110,
    1112,  1115,  1119,  1122,  1126,  1129,  1133,  1136,  1140,  1143,
    1147,  1150,  1153,  1156,  1164,  1167,  1182,  1182,  1184,  1198,
    1207,  1214,  1225,  1230,  1235,  1241,  1248,  1251,  1255,  1258,
    1263,  1275,  1282,  1296,  1299,  1302,  1305,  1308,  1311,  1314,
    1320,  1324,  1328,  1343,  1347,  1351,  1360,  1364,  1380,  1386,
    1390,  1391,  1392,  1393,  1394,  1396,  1399,  1405,  1406,  1407,
    1408,  1409,  1411,  1419,  1427,  1436,  1443,  1450,  1457,  1464,
    1471,  1479,  1487,  1495,  1503,  1511,  1519,  1527,  1535,  1543,
    1551,  1559,  1567,  1596,  1604,  1613,  1621,  1630,  1638,  1644,
    1651,  1657,  1664,  1669,  1676,  1683,  1691,  1712,  1718,  1724,
    1731,  1739,  1746,  1753,  1758,  1768,  1773,  1778,  1783,  1788,
    1793,  1798,  1801,  1805,  1811,  1817,  1823,  1829,  1835,  1841,
    1847,  1853,  1859,  1865,  1871,  1882,  1886,  1891,  1907,  1913,
    1919,  1923,  1924,  1926,  1927,  1929,  1930
>>>>>>> 74edf86 (add csv import)
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "STRING",
  "DOUBLE_VALUE", "LONG_VALUE", "CREATE", "SELECT", "INSERT", "DROP",
  "UPDATE", "DELETE", "COPY", "SET", "EXPLAIN", "SHOW", "ALTER", "EXECUTE",
  "PREPARE", "DESCRIBE", "UNION", "ALL", "INTERSECT", "EXCEPT", "FLUSH",
  "SCHEMA", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "AST",
  "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "VIEW", "INDEX", "ANALYZE",
  "VIEWS", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT",
  "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE",
  "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN",
  "FROM", "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END",
  "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "CHAR", "VARCHAR", "FLOAT", "DOUBLE",
  "REAL", "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS",
  "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS",
  "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN",
  "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "NUMBER", "'='",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('",
  "')'", "'.'", "';'", "','", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "table_element", "table_column", "column_type",
  "column_constraints", "column_constraint", "table_constraint",
  "identifier_array", "delete_statement", "insert_statement",
  "optional_identifier_array", "explain_statement", "explain_type",
  "update_statement", "update_expr_array", "update_expr", "drop_statement",
  "copy_statement", "select_statement", "select_with_paren",
  "select_without_paren", "select_clause_with_modifier",
  "select_clause_without_modifier_paren", "select_clause_without_modifier",
  "order_by_clause", "order_by_expr_list", "order_by_expr",
  "order_by_type", "limit_expr", "offset_expr", "distinct", "from_clause",
  "where_clause", "having_clause", "group_by_clause", "set_operator",
  "table_reference", "table_reference_unit", "table_reference_name",
  "table_name", "table_alias", "with_clause", "with_expr_list",
  "with_expr", "join_clause", "join_type", "show_statement",
  "flush_statement", "expr_array", "expr_alias", "expr", "operand",
  "function_expr", "conjunction_expr", "between_expr", "in_expr",
  "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-313)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-276)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      91,   190,    -9,   197,    22,   -36,    22,   239,    26,    22,
      41,    36,   -35,    52,   -83,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,   198,  -313,  -313,    70,  -313,  -313,    21,
      21,    21,    21,    21,    22,    25,    25,    25,    25,    25,
     -47,   145,    22,   -32,  -313,  -313,  -313,  -313,  -313,  -313,
     144,  -313,  -313,  -313,  -313,  -313,  -313,   122,    38,  -313,
      78,    96,  -313,    47,  -313,   237,  -313,  -313,    -5,   114,
    -313,   220,   244,   313,    22,    22,    22,   317,   172,   265,
     333,    22,    22,    22,    22,   340,   349,   298,   355,   355,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,   198,  -313,  -313,
     210,    36,  -313,  -313,  -313,  -313,    -5,  -313,  -313,  -313,
     327,   325,   311,   306,  -313,    -7,  -313,   172,   323,   375,
     -19,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   241,   -33,
    -313,   327,  -313,  -313,   318,   320,    70,  -313,   233,   240,
     238,  -313,  -313,   304,   341,   242,   243,   227,   390,  -313,
    -313,   393,   251,   345,   345,  -313,   252,   -37,  -313,   -20,
     398,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   253,  -313,
    -313,   327,   327,   348,  -313,   -35,    11,   364,    22,  -313,
    -116,   259,  -313,   327,   349,  -313,   -68,   260,   261,   262,
    -313,  -313,   136,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,   345,   283,   442,   367,   327,
     327,    62,   117,  -313,   304,  -313,   411,   137,   137,   285,
    -102,     2,   327,   298,   434,   327,   327,   -41,   288,   -60,
     345,   345,   345,   345,   345,   345,   345,   345,   345,   345,
     345,   345,   345,   345,     3,   289,  -313,   111,   -68,   327,
    -313,   198,   534,   334,   294,   -80,  -313,  -313,  -313,   -35,
     295,  -313,   443,   327,   -68,  -313,   223,   223,  -313,   327,
    -313,    54,   367,   312,   297,    29,   -56,   130,  -313,   327,
     327,   384,  -313,  -313,   -35,   296,   216,  -313,    17,  -313,
    -313,   410,  -313,  -313,   324,   302,   345,   252,   353,  -313,
     463,   463,   166,   166,   420,   463,   463,   166,   166,   137,
     137,  -313,  -313,  -313,  -313,  -313,   327,  -313,  -313,  -313,
     -68,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   314,   315,
    -313,  -313,  -313,   316,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,   319,   326,   328,   330,
     105,   337,   375,  -313,    11,   198,   375,  -313,    59,   464,
     470,  -313,   100,  -313,   148,   150,  -313,   332,  -313,   534,
     327,  -313,   327,   -64,    14,   345,   -14,     2,   421,  -313,
    -313,  -313,  -313,  -313,  -313,   428,  -313,   489,  -313,   451,
     453,   252,   463,   343,   167,  -313,   345,  -313,   495,   496,
     497,   500,   501,    94,   276,   408,   400,  -313,  -313,   105,
    -313,   375,   173,  -313,   175,  -313,  -313,  -313,  -313,   223,
    -313,  -313,  -313,   368,   -68,   -45,  -313,   327,   370,    17,
     216,     2,     2,   372,   327,   327,  -313,   369,   183,  -313,
    -313,   442,   373,   379,   184,   381,   383,   382,   392,   397,
     402,   403,   404,   405,   414,   419,   422,   424,   425,   426,
     427,   440,   441,  -313,  -313,  -313,   222,  -313,  -313,  -313,
    -313,  -313,   -68,  -313,  -313,  -313,   472,   375,   445,   -68,
    -313,  -313,  -313,  -313,  -313,   529,  -313,  -313,   531,   548,
     569,   590,   591,   594,   595,   596,   634,   635,   636,   637,
     638,   639,   640,   641,  -313,   327,   229,   498,   499,   502,
     503,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   -68,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,   105,     0,     0,
       0,     0,   167,     0,   276,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   118,   117,     0,     8,    14,   274,
     274,   274,   274,   274,     0,   272,   272,   272,   272,   272,
     160,     0,     0,     0,   100,   104,   101,   102,   103,    99,
     167,   180,   181,   182,   183,   184,   185,     0,   166,   168,
       0,     0,     1,   167,     2,   150,   152,   153,     0,   141,
     123,   129,     0,     0,     0,     0,     0,     0,    97,     0,
       0,     0,     0,     0,     0,     0,     0,   145,     0,     0,
      98,    15,    20,    22,    21,    16,    17,    19,    18,    23,
       0,     0,   122,   121,     4,   151,     0,   119,   120,   140,
       0,     0,   137,     0,    24,     0,    25,    97,     0,     0,
     167,   271,   110,   112,   111,   113,   114,   161,     0,   145,
     107,     0,    93,   270,     0,     0,     0,   169,     0,     0,
     241,   245,   248,   249,     0,     0,     0,     0,     0,   246,
     247,     0,     0,     0,     0,   243,   167,   143,   186,   189,
     190,   199,   194,   193,   192,   200,   201,   191,   198,   197,
     252,     0,     0,   139,   273,   167,     0,     0,     0,    91,
       0,     0,    95,     0,     0,   106,   144,     0,     0,     0,
     126,   125,     0,   254,   253,   256,   255,   258,   257,   260,
     259,   262,   261,   264,   263,     0,     0,   207,   167,     0,
       0,     0,     0,   250,     0,   251,     0,   209,   208,     0,
       0,     0,     0,   145,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   130,   135,   136,     0,
     124,    27,     0,     0,     0,     0,    30,    32,    33,   167,
       0,    96,     0,     0,   109,   108,     0,     0,   170,     0,
     202,     0,   167,     0,     0,     0,     0,     0,   232,     0,
       0,     0,   196,   195,   167,   142,   154,   156,   165,   157,
     187,   149,   188,   225,   226,     0,     0,   167,     0,   206,
     216,   217,   220,   221,     0,   223,   215,   218,   219,   211,
     210,   212,   213,   214,   242,   244,     0,   133,   134,   132,
     138,    36,    39,    40,    37,    38,    41,    42,    58,    60,
      43,    45,    44,    64,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,     0,     0,     0,
      34,     0,     0,    26,     0,    28,     0,    92,     0,     0,
       0,   269,     0,   265,     0,     0,   203,     0,   237,     0,
       0,   230,     0,     0,     0,     0,     0,     0,     0,   174,
     175,   176,   177,   173,   178,     0,   163,     0,   158,     0,
     147,   167,   224,     0,     0,   205,     0,   131,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    86,    87,    35,
      83,     0,     0,    31,     0,    94,   268,   267,   116,     0,
     115,   204,   238,     0,   234,     0,   233,     0,     0,   165,
     155,     0,     0,   162,     0,     0,   127,     0,     0,   239,
     228,   227,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    88,    85,    84,     0,    90,    29,   266,
     236,   231,   235,   222,   159,   171,     0,     0,   148,   146,
     240,   229,    59,    61,    63,     0,    65,    66,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    89,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   172,   164,    62,    70,    71,    68,
      69,    72,    73,    74,    67,    78,    79,    76,    77,    80,
      81,    82,    75
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -313,  -313,  -313,   585,  -313,   601,  -313,   321,  -313,   299,
    -313,   263,  -313,  -312,   602,   619,   553,  -313,  -313,   621,
    -313,   490,   623,   626,   -39,   665,   -12,   542,   573,    95,
    -313,  -313,   365,  -313,  -313,  -313,  -313,  -313,  -113,  -313,
    -313,  -313,  -313,   303,  -179,     9,   254,  -313,  -313,   581,
    -313,  -313,   642,   643,  -188,   462,  -130,  -108,  -313,  -313,
    -313,  -313,  -313,   474,  -313,  -313,  -313,  -313,   535,   423,
     268,   599,   146,   206,  -313
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    13,    14,    15,    90,    16,   255,   256,   257,   350,
     409,   410,   258,   180,    17,    18,   120,    19,    50,    20,
     129,   130,    21,    22,    23,    24,    25,    70,   107,    71,
     112,   245,   246,   319,   173,   250,   110,   223,   132,   436,
     390,    68,   285,   286,   287,   288,   388,    26,    58,    59,
     289,   385,    27,    28,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   212,   166,   167,   168,   169,   170,   362,
     363,   134,    80,    73,    64
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      61,   186,   298,    69,   271,    40,   314,    65,   426,    66,
      67,    97,   181,    41,   252,    43,   185,   211,    53,   370,
     386,    34,   131,   295,   224,    40,   220,   471,   221,    42,
     225,   226,    11,    88,    89,   261,   207,   175,   262,    57,
     412,   247,   248,    78,   414,   217,   218,  -275,    11,   283,
     299,    87,    62,   264,     1,    51,     2,     3,     4,     5,
       6,   387,     7,     8,   225,   226,    52,     9,   225,   226,
      63,   353,    10,   369,   354,   358,   225,   226,    69,   275,
     276,   365,    72,   115,   116,   117,    79,   225,   226,   427,
     123,   124,   125,   126,   296,   293,   294,   207,     1,   466,
       2,     3,     4,     5,     6,    85,     7,     8,   182,   394,
     291,     9,   225,   226,    11,    12,    10,   222,   253,   320,
     254,   184,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   251,   429,   210,   140,
     141,   142,   143,   176,   219,   106,   225,   226,   315,   373,
     374,     1,   284,     2,     3,     4,     5,     6,    11,    86,
       8,   225,   226,   108,     9,   506,   100,   405,   109,    10,
     317,   318,   447,   448,   449,   450,   451,    54,    55,    56,
     452,   453,    81,    82,    83,    84,   247,   260,   392,   278,
     269,   279,   101,   280,   225,   226,   274,    12,   144,   145,
     454,   139,   371,   438,   372,   366,   280,   146,   222,   147,
     415,    11,   406,   222,   407,   408,    29,    30,    31,    65,
     355,    66,    67,    35,    36,    37,   148,    32,    33,   102,
     140,   141,   142,   143,    38,    39,    74,    75,    76,    77,
     424,    12,   425,   225,   226,   376,   478,   103,   149,   150,
     151,   418,   475,   476,   419,   140,   141,   142,   143,   105,
     367,   378,  -179,   379,   380,   381,   382,   428,   383,   384,
     152,    44,    45,    46,    47,    48,   111,   229,    49,   153,
     154,   155,   241,   242,   243,   393,   156,   270,   441,   144,
     145,   359,   360,   361,    12,  -276,  -276,   472,   146,   420,
     147,   421,   419,   210,   222,   479,   113,  -276,  -276,   239,
     240,   241,   242,   243,   144,   145,   114,   148,   440,    11,
     118,   222,   119,   146,   467,   147,   468,   262,   121,   262,
     140,   141,   142,   143,   481,   484,   122,   222,   485,   149,
     150,   151,   148,   127,   140,   141,   142,   143,   140,   141,
     142,   143,   128,   131,   455,   456,   457,   458,   459,   133,
     136,   152,   460,   461,   149,   150,   151,   171,   172,   174,
     153,   154,   155,   504,   178,   524,   262,   156,   179,   437,
     525,   183,   462,   262,   190,   187,   152,   188,   192,   144,
     145,   191,   208,   209,   213,   153,   154,   155,   146,   214,
     147,   216,   156,   205,   206,   244,   249,   205,   259,   263,
     266,   267,   146,   268,   147,   281,   146,   148,   147,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   148,   273,   272,    11,   148,   282,   292,   297,   149,
     150,   151,   351,   316,   352,   356,   357,   296,   368,   375,
     377,   389,   391,   149,   150,   151,   225,   149,   150,   151,
     227,   152,   228,   395,   398,   399,   400,   431,   416,   401,
     153,   154,   155,   417,   432,   152,   402,   156,   403,   152,
     404,   229,   273,   422,   153,   154,   155,   411,   153,   154,
     155,   156,   433,   434,   439,   156,   435,   230,   231,   232,
     233,   442,   443,   444,   273,   235,   445,   446,   464,   229,
     236,   237,   238,   239,   240,   241,   242,   243,   463,   470,
     480,   473,   477,   505,   482,   230,   231,   232,   233,   234,
     483,   229,   486,   235,   487,   507,   488,   508,   236,   237,
     238,   239,   240,   241,   242,   243,   489,   230,   231,   232,
     233,   490,   396,   229,   509,   235,   491,   492,   493,   494,
     236,   237,   238,   239,   240,   241,   242,   243,   495,   230,
     231,   232,   233,   496,   229,   510,   497,   235,   498,   499,
     500,   501,   236,   237,   238,   239,   240,   241,   242,   243,
    -276,  -276,   232,   233,   502,   503,   511,   512,  -276,   222,
     513,   514,   515,  -276,   237,   238,   239,   240,   241,   242,
     243,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     516,   517,   518,   519,   520,   521,   522,   523,   104,   526,
     527,    91,    92,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   423,    93,
     177,    94,   465,    95,   265,   413,    96,    60,   189,   138,
     430,   397,   137,   474,   290,   277,   215,   469,   135,     0,
     364,     0,    98,    99
};

static const yytype_int16 yycheck[] =
{
      12,   131,    62,     8,   192,     3,     3,    21,    72,    23,
      24,    50,    31,     4,     3,     6,   129,   147,     9,    75,
       3,    30,    55,    64,    44,     3,   156,    72,    65,    65,
     132,   133,    67,    65,    66,   151,   144,    44,   154,     3,
     352,   171,   172,    34,   356,   153,   154,     0,    67,   151,
     110,    42,     0,   183,     7,    29,     9,    10,    11,    12,
      13,    44,    15,    16,   132,   133,    40,    20,   132,   133,
     153,   151,    25,    44,   154,   263,   132,   133,     8,   209,
     210,   269,    61,    74,    75,    76,    61,   132,   133,    75,
      81,    82,    83,    84,   135,   225,   226,   205,     7,   411,
       9,    10,    11,    12,    13,   152,    15,    16,   120,   297,
     223,    20,   132,   133,    67,   150,    25,   154,   107,   249,
     109,   154,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   175,   151,    76,     3,
       4,     5,     6,   150,   156,   150,   132,   133,   145,   279,
     280,     7,   150,     9,    10,    11,    12,    13,    67,    14,
      16,   132,   133,    68,    20,   477,    44,    62,    54,    25,
      59,    60,    78,    79,    80,    81,    82,   136,   137,   138,
      86,    87,    36,    37,    38,    39,   316,   178,   296,    72,
      54,    74,   154,    76,   132,   133,   208,   150,    62,    63,
     106,   106,    72,   391,    74,   151,    76,    71,   154,    73,
     151,    67,   107,   154,   109,   110,    26,    27,    28,    21,
     259,    23,    24,    26,    27,    28,    90,    37,    38,   151,
       3,     4,     5,     6,    37,    38,    30,    31,    32,    33,
     370,   150,   372,   132,   133,   284,   434,   151,   112,   113,
     114,   151,   431,   432,   154,     3,     4,     5,     6,    22,
     272,    45,    46,    47,    48,    49,    50,   375,    52,    53,
     134,    32,    33,    34,    35,    36,    56,   111,    39,   143,
     144,   145,   145,   146,   147,   297,   150,   151,   396,    62,
      63,    68,    69,    70,   150,   129,   130,   427,    71,   151,
      73,   151,   154,    76,   154,   435,    62,   141,   142,   143,
     144,   145,   146,   147,    62,    63,     3,    90,   151,    67,
       3,   154,   150,    71,   151,    73,   151,   154,    63,   154,
       3,     4,     5,     6,   151,   151,     3,   154,   154,   112,
     113,   114,    90,     3,     3,     4,     5,     6,     3,     4,
       5,     6,     3,    55,    78,    79,    80,    81,    82,     4,
     150,   134,    86,    87,   112,   113,   114,    42,    57,    63,
     143,   144,   145,   151,    51,   505,   154,   150,     3,   391,
     151,   140,   106,   154,   151,    67,   134,    67,   150,    62,
      63,   151,   150,   150,     4,   143,   144,   145,    71,     6,
      73,   150,   150,    62,    63,   152,    58,    62,    44,   150,
     150,   150,    71,   151,    73,     4,    71,    90,    73,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,    90,    62,   150,    67,    90,   151,     3,   150,   112,
     113,   114,   108,   154,   150,   150,     3,   135,   151,    65,
     154,    41,   150,   112,   113,   114,   132,   112,   113,   114,
      62,   134,    64,   110,   150,   150,   150,    46,     4,   150,
     143,   144,   145,     3,    46,   134,   150,   150,   150,   134,
     150,   111,    62,   151,   143,   144,   145,   150,   143,   144,
     145,   150,     3,    42,   151,   150,    43,   127,   128,   129,
     130,     6,     6,     6,    62,   135,     6,     6,   108,   111,
     140,   141,   142,   143,   144,   145,   146,   147,   110,   151,
     151,   151,   150,    51,   151,   127,   128,   129,   130,   131,
     151,   111,   151,   135,   151,     6,   154,     6,   140,   141,
     142,   143,   144,   145,   146,   147,   154,   127,   128,   129,
     130,   154,   132,   111,     6,   135,   154,   154,   154,   154,
     140,   141,   142,   143,   144,   145,   146,   147,   154,   127,
     128,   129,   130,   154,   111,     6,   154,   135,   154,   154,
     154,   154,   140,   141,   142,   143,   144,   145,   146,   147,
     127,   128,   129,   130,   154,   154,     6,     6,   135,   154,
       6,     6,     6,   140,   141,   142,   143,   144,   145,   146,
     147,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       6,     6,     6,     6,     6,     6,     6,     6,    63,   151,
     151,    50,    50,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   369,    50,
     117,    50,   409,    50,   184,   354,    50,    12,   136,   106,
     377,   316,   101,   429,   222,   211,   151,   419,    89,    -1,
     267,    -1,    50,    50
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      25,    67,   150,   156,   157,   158,   160,   169,   170,   172,
     174,   177,   178,   179,   180,   181,   202,   207,   208,    26,
      27,    28,    37,    38,    30,    26,    27,    28,    37,    38,
       3,   200,    65,   200,    32,    33,    34,    35,    36,    39,
     173,    29,    40,   200,   136,   137,   138,     3,   203,   204,
     180,   181,     0,   153,   229,    21,    23,    24,   196,     8,
     182,   184,    61,   228,   228,   228,   228,   228,   200,    61,
     227,   227,   227,   227,   227,   152,    14,   200,    65,    66,
     159,   160,   169,   170,   174,   177,   178,   179,   207,   208,
      44,   154,   151,   151,   158,    22,   150,   183,   184,    54,
     191,    56,   185,    62,     3,   200,   200,   200,     3,   150,
     171,    63,     3,   200,   200,   200,   200,     3,     3,   175,
     176,    55,   193,     4,   226,   226,   150,   204,   183,   184,
       3,     4,     5,     6,    62,    63,    71,    73,    90,   112,
     113,   114,   134,   143,   144,   145,   150,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   219,   220,   221,   222,
     223,    42,    57,   189,    63,    44,   150,   171,    51,     3,
     168,    31,   181,   140,   154,   193,   211,    67,    67,   182,
     151,   151,   150,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,    62,    63,   212,   150,   150,
      76,   211,   218,     4,     6,   223,   150,   212,   212,   181,
     211,    65,   154,   192,    44,   132,   133,    62,    64,   111,
     127,   128,   129,   130,   131,   135,   140,   141,   142,   143,
     144,   145,   146,   147,   152,   186,   187,   211,   211,    58,
     190,   179,     3,   107,   109,   161,   162,   163,   167,    44,
     200,   151,   154,   150,   211,   176,   150,   150,   151,    54,
     151,   209,   150,    62,   181,   211,   211,   218,    72,    74,
      76,     4,   151,   151,   150,   197,   198,   199,   200,   205,
     210,   193,     3,   211,   211,    64,   135,   150,    62,   110,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,     3,   145,   154,    59,    60,   188,
     211,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     164,   108,   150,   151,   154,   179,   150,     3,   209,    68,
      69,    70,   224,   225,   224,   209,   151,   181,   151,    44,
      75,    72,    74,   211,   211,    65,   179,   154,    45,    47,
      48,    49,    50,    52,    53,   206,     3,    44,   201,    41,
     195,   150,   212,   181,   209,   110,   132,   187,   150,   150,
     150,   150,   150,   150,   150,    62,   107,   109,   110,   165,
     166,   150,   168,   162,   168,   151,     4,     3,   151,   154,
     151,   151,   151,   164,   211,   211,    72,    75,   212,   151,
     198,    46,    46,     3,    42,    43,   194,   181,   209,   151,
     151,   212,     6,     6,     6,     6,     6,    78,    79,    80,
      81,    82,    86,    87,   106,    78,    79,    80,    81,    82,
      86,    87,   106,   110,   108,   166,   168,   151,   151,   225,
     151,    72,   211,   151,   201,   199,   199,   150,   209,   211,
     151,   151,   151,   151,   151,   154,   151,   151,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   151,    51,   168,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   211,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   155,   156,   157,   157,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   160,   160,   160,   160,   160,   160,
     161,   161,   162,   162,   163,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   165,   165,   166,   166,   166,   166,   167,
     167,   168,   168,   169,   170,   170,   171,   171,   172,   173,
     173,   173,   173,   173,   173,   173,   174,   175,   175,   176,
     177,   177,   177,   177,   177,   178,   178,   179,   179,   179,
     179,   180,   180,   181,   182,   183,   183,   184,   185,   185,
     186,   186,   187,   188,   188,   188,   189,   189,   190,   190,
     191,   191,   192,   192,   193,   193,   194,   194,   195,   195,
     196,   196,   196,   196,   197,   197,   198,   198,   199,   199,
     200,   200,   201,   201,   201,   201,   202,   202,   203,   203,
     204,   205,   205,   206,   206,   206,   206,   206,   206,   206,
     207,   207,   207,   208,   208,   208,   209,   209,   210,   210,
     211,   211,   211,   211,   211,   212,   212,   212,   212,   212,
     212,   212,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   214,   214,   215,   216,   216,
     217,   217,   217,   217,   218,   218,   219,   220,   220,   220,
     220,   221,   221,   221,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   224,   224,   225,   225,   225,
     226,   227,   227,   228,   228,   229,   229
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     7,     6,     7,     9,
       1,     3,     1,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       1,     4,     6,     4,     1,     4,     4,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     1,     2,     2,     1,     1,     2,     5,
       4,     1,     3,     4,     8,     5,     3,     0,     3,     1,
       1,     1,     1,     1,     1,     0,     5,     1,     3,     3,
       4,     4,     4,     4,     4,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     7,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     2,     2,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     3,     4,     5,     4,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     4,     3,     3,     5,     5,     6,
       4,     6,     3,     5,     4,     5,     6,     4,     5,     5,
       6,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     2,     2,     1,
       1,     2,     0,     3,     0,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = SQLEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == SQLEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, scanner, result, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use SQLerror or SQLUNDEF. */
#define YYERRCODE SQLUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if SQLDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, scanner, result); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, infinity::ParserResult* result)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (result);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, infinity::ParserResult* result)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, scanner, result);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, void *scanner, infinity::ParserResult* result)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), scanner, result);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner, result); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !SQLDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !SQLDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner, infinity::ParserResult* result)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (result);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_IDENTIFIER: /* IDENTIFIER  */
#line 223 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 1842 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1842 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 223 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 1850 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1850 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 150 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
<<<<<<< HEAD
#line 1864 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1864 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 140 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
<<<<<<< HEAD
#line 1878 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1878 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 216 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
<<<<<<< HEAD
#line 1889 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1889 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 227 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
<<<<<<< HEAD
#line 1898 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1898 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 227 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
<<<<<<< HEAD
#line 1907 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1907 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 187 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
<<<<<<< HEAD
#line 1921 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1921 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 180 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
<<<<<<< HEAD
#line 1932 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1932 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1942 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1942 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1952 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1952 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1962 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1962 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1972 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1972 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1982 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1982 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 1992 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 1992 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 170 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2006 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2006 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 170 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2020 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2020 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 246 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
<<<<<<< HEAD
#line 2030 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2030 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2038 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2038 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2046 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2046 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2055 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2063 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2063 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2071 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2071 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 160 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
<<<<<<< HEAD
#line 2085 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2085 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2094 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2094 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2103 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2103 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2112 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2112 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 207 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
<<<<<<< HEAD
#line 2125 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2125 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 236 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
<<<<<<< HEAD
#line 2134 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2134 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 197 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2148 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2148 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 197 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2162 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2162 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 252 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
<<<<<<< HEAD
#line 2172 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2172 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2181 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2181 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 160 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
<<<<<<< HEAD
#line 2195 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2195 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2203 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2203 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_expr: /* expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2211 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2211 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_operand: /* operand  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2219 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2219 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2227 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2227 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2235 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2235 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2243 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2243 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2251 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2251 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2259 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2259 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 264 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
<<<<<<< HEAD
#line 2272 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2272 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2280 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2280 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2288 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2288 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2296 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2296 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2304 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2304 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 232 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2312 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2312 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 223 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 2320 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2320 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner, infinity::ParserResult* result)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined SQLLTYPE_IS_TRIVIAL && SQLLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = SQLEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 70 "/home/tangdh/workspace/infinity/src/parser/parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

<<<<<<< HEAD
#line 2428 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2428 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == SQLEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= SQLEOF)
    {
      yychar = SQLEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == SQLerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = SQLUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = SQLEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* input_pattern: statement_list semicolon  */
#line 370 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
<<<<<<< HEAD
#line 2643 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2643 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 3: /* statement_list: statement  */
#line 374 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
<<<<<<< HEAD
#line 2654 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2654 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 380 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
<<<<<<< HEAD
#line 2665 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2665 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 5: /* statement: create_statement  */
#line 387 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
<<<<<<< HEAD
#line 2671 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2671 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 6: /* statement: drop_statement  */
#line 388 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
<<<<<<< HEAD
#line 2677 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2677 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 7: /* statement: copy_statement  */
#line 389 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
<<<<<<< HEAD
#line 2683 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2683 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 8: /* statement: show_statement  */
#line 390 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
<<<<<<< HEAD
#line 2689 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2689 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 9: /* statement: select_statement  */
#line 391 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
<<<<<<< HEAD
#line 2695 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2695 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 10: /* statement: delete_statement  */
#line 392 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
<<<<<<< HEAD
#line 2701 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2701 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 11: /* statement: update_statement  */
#line 393 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
<<<<<<< HEAD
#line 2707 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2707 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 12: /* statement: insert_statement  */
#line 394 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
<<<<<<< HEAD
#line 2713 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2713 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 13: /* statement: explain_statement  */
#line 395 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
<<<<<<< HEAD
#line 2719 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2719 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 14: /* statement: flush_statement  */
#line 396 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
<<<<<<< HEAD
#line 2725 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2725 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 15: /* explainable_statement: create_statement  */
#line 398 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
<<<<<<< HEAD
#line 2731 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2731 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 16: /* explainable_statement: drop_statement  */
#line 399 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
<<<<<<< HEAD
#line 2737 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2737 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 17: /* explainable_statement: copy_statement  */
#line 400 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
<<<<<<< HEAD
#line 2743 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2743 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 18: /* explainable_statement: show_statement  */
#line 401 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
<<<<<<< HEAD
#line 2749 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2749 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 19: /* explainable_statement: select_statement  */
#line 402 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
<<<<<<< HEAD
#line 2755 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2755 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 20: /* explainable_statement: delete_statement  */
#line 403 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
<<<<<<< HEAD
#line 2761 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2761 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 21: /* explainable_statement: update_statement  */
#line 404 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
<<<<<<< HEAD
#line 2767 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2767 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 22: /* explainable_statement: insert_statement  */
#line 405 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
<<<<<<< HEAD
#line 2773 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2773 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 23: /* explainable_statement: flush_statement  */
#line 406 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
<<<<<<< HEAD
#line 2779 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2779 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 24: /* create_statement: CREATE SCHEMA if_not_exists IDENTIFIER  */
#line 413 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                          {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
<<<<<<< HEAD
#line 2795 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2795 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 25: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 426 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                             {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateCollectionInfo> create_collection_info = std::make_shared<infinity::CreateCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        create_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    create_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.create_stmt)->create_info_ = create_collection_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
<<<<<<< HEAD
#line 2813 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2813 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 26: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 441 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                                    {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    for (infinity::TableElement*& element : *(yyvsp[-1].table_element_array_t)) {
        if(element->type_ == infinity::TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((infinity::ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((infinity::TableConstraint*)element);
        }
    }
    delete (yyvsp[-1].table_element_array_t);

    (yyval.create_stmt)->create_info_ = create_table_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-4].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
<<<<<<< HEAD
#line 2841 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2841 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 27: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 465 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                            {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);

    create_table_info->conflict_type_ = (yyvsp[-3].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    create_table_info->select_ = (yyvsp[0].select_stmt);
    (yyval.create_stmt)->create_info_ = create_table_info;
}
<<<<<<< HEAD
#line 2861 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2861 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 28: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 481 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                                                     {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateViewInfo> create_view_info = std::make_shared<infinity::CreateViewInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_view_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_view_info->view_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    create_view_info->view_columns_ = (yyvsp[-2].identifier_array_t);
    create_view_info->select_ = (yyvsp[0].select_stmt);
    create_view_info->conflict_type_ = (yyvsp[-4].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    (yyval.create_stmt)->create_info_ = create_view_info;
}
<<<<<<< HEAD
#line 2882 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2882 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 29: /* create_statement: CREATE INDEX if_not_exists IDENTIFIER ON table_name '(' identifier_array ')'  */
#line 498 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                                               {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    ParserHelper::ToLower((yyvsp[-5].str_value));
    create_index_info->index_name_ = (yyvsp[-5].str_value);
    free((yyvsp[-5].str_value));

    create_index_info->column_names_ = (yyvsp[-1].identifier_array_t);
    create_index_info->conflict_type_ = (yyvsp[-6].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    (yyval.create_stmt)->create_info_ = create_index_info;
}
<<<<<<< HEAD
#line 2906 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2906 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 30: /* table_element_array: table_element  */
#line 518 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
<<<<<<< HEAD
#line 2915 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2915 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 31: /* table_element_array: table_element_array ',' table_element  */
#line 522 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
<<<<<<< HEAD
#line 2924 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2924 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 32: /* table_element: table_column  */
#line 528 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
<<<<<<< HEAD
#line 2932 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2932 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 33: /* table_element: table_constraint  */
#line 531 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
<<<<<<< HEAD
#line 2940 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 2940 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)
    break;

  case 34: /* table_column: IDENTIFIER column_type  */
#line 537 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                       {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[0].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[0].column_type_t).precision, (yyvsp[0].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                delete (yyvsp[-1].str_value);
                YYERROR;
            }
            break;
        }
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[0].column_type_t).embedding_type_, (yyvsp[0].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[0].column_type_t).logical_type_, type_info_ptr);

    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
<<<<<<< HEAD
#line 2980 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* table_column: IDENTIFIER column_type column_constraints  */
#line 572 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 2977 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 35: /* table_column: IDENTIFIER column_type column_constraints  */
#line 569 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-1].column_type_t).logical_type_, type_info_ptr);

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-2].str_value);
    (yyval.table_column_t)->constraints_ = *(yyvsp[0].column_constraints_t);
    delete (yyvsp[0].column_constraints_t);
    free((yyvsp[-2].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
<<<<<<< HEAD
#line 3017 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: BOOLEAN  */
#line 606 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3023 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: TINYINT  */
#line 607 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: SMALLINT  */
#line 608 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3035 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: INTEGER  */
#line 609 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3041 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: INT  */
#line 610 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3047 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: BIGINT  */
#line 611 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3053 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: HUGEINT  */
#line 612 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3059 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: FLOAT  */
#line 613 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3065 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: REAL  */
#line 614 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3071 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DOUBLE  */
#line 615 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: DATE  */
#line 616 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3083 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: TIME  */
#line 617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3089 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: DATETIME  */
#line 618 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3095 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: TIMESTAMP  */
#line 619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3101 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: UUID  */
#line 620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3107 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: POINT  */
#line 621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3113 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: LINE  */
#line 622 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3119 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: LSEG  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3125 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: BOX  */
#line 624 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3131 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: PATH  */
#line 625 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPath}; }
#line 3137 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: POLYGON  */
#line 626 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPolygon}; }
#line 3143 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: CIRCLE  */
#line 627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3149 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: CHAR  */
#line 629 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3155 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 630 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3161 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 631 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3167 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 632 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3173 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3179 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 634 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3185 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: DECIMAL  */
#line 635 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3191 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 636 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3197 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 637 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3203 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 638 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3209 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 639 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3215 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 641 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3227 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 642 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 643 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3239 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3245 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 645 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3251 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 646 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3257 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 647 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3263 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 648 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3269 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 649 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3275 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3281 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 651 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3287 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 652 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3293 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 653 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3299 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* column_constraints: column_constraint  */
#line 672 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3011 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: BOOLEAN  */
#line 600 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3017 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: TINYINT  */
#line 601 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3023 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: SMALLINT  */
#line 602 "/home/tangdh/workspace/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3029 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: INTEGER  */
#line 603 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3035 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: INT  */
#line 604 "/home/tangdh/workspace/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3041 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: BIGINT  */
#line 605 "/home/tangdh/workspace/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3047 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: HUGEINT  */
#line 606 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3053 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: FLOAT  */
#line 607 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3059 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: REAL  */
#line 608 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3065 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DOUBLE  */
#line 609 "/home/tangdh/workspace/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3071 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: DATE  */
#line 610 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3077 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: TIME  */
#line 611 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3083 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: DATETIME  */
#line 612 "/home/tangdh/workspace/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3089 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: TIMESTAMP  */
#line 613 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3095 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: UUID  */
#line 614 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3101 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: POINT  */
#line 615 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3107 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: LINE  */
#line 616 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3113 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: LSEG  */
#line 617 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3119 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: BOX  */
#line 618 "/home/tangdh/workspace/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3125 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: PATH  */
#line 619 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPath}; }
#line 3131 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: POLYGON  */
#line 620 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPolygon}; }
#line 3137 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: CIRCLE  */
#line 621 "/home/tangdh/workspace/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3143 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: CHAR  */
#line 623 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3149 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 624 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3155 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 625 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3161 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 626 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3167 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 627 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3173 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 628 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3179 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: DECIMAL  */
#line 629 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3185 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 630 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3191 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 631 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3197 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 632 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3203 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 633 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3209 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 634 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3215 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 635 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3221 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 636 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3227 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 637 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3233 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 638 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3239 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 639 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3245 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 640 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3251 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 641 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3257 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 642 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3263 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 643 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3269 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 644 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3275 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 645 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3281 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 646 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3287 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 647 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3293 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 83: /* column_constraints: column_constraint  */
#line 666 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    (yyval.column_constraints_t) = new infinity::HashSet<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
<<<<<<< HEAD
#line 3308 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* column_constraints: column_constraints column_constraint  */
#line 676 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3302 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 84: /* column_constraints: column_constraints column_constraint  */
#line 670 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
<<<<<<< HEAD
#line 3322 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* column_constraint: PRIMARY KEY  */
#line 686 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3330 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 86: /* column_constraint: UNIQUE  */
#line 689 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3338 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 87: /* column_constraint: NULLABLE  */
#line 692 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3346 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 88: /* column_constraint: NOT NULLABLE  */
#line 695 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3354 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 699 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3316 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 85: /* column_constraint: PRIMARY KEY  */
#line 680 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3324 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 86: /* column_constraint: UNIQUE  */
#line 683 "/home/tangdh/workspace/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3332 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 87: /* column_constraint: NULLABLE  */
#line 686 "/home/tangdh/workspace/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3340 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 88: /* column_constraint: NOT NULLABLE  */
#line 689 "/home/tangdh/workspace/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3348 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 89: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 693 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
<<<<<<< HEAD
#line 3364 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 704 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3358 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 90: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 698 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
<<<<<<< HEAD
#line 3374 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* identifier_array: IDENTIFIER  */
#line 711 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3368 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 91: /* identifier_array: IDENTIFIER  */
#line 705 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 3385 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 717 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3379 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 92: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 711 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
<<<<<<< HEAD
#line 3396 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* delete_statement: DELETE FROM table_name where_clause  */
#line 727 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3390 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 93: /* delete_statement: DELETE FROM table_name where_clause  */
#line 721 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                       {
    (yyval.delete_stmt) = new infinity::DeleteStatement();

    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.delete_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.delete_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);
    (yyval.delete_stmt)->where_expr_ = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 3413 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' expr_array ')'  */
#line 743 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3407 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 94: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' expr_array ')'  */
#line 737 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                                             {
    (yyval.insert_stmt) = new infinity::InsertStatement();
    if((yyvsp[-5].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-5].table_name_t)->schema_name_ptr_;
        free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-5].table_name_t)->table_name_ptr_;
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);
    (yyval.insert_stmt)->columns_ = (yyvsp[-4].identifier_array_t);
    (yyval.insert_stmt)->values_ = (yyvsp[-1].expr_array_t);
}
<<<<<<< HEAD
#line 3430 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 755 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3424 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 95: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 749 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                        {
    (yyval.insert_stmt) = new infinity::InsertStatement();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);
    (yyval.insert_stmt)->columns_ = (yyvsp[-1].identifier_array_t);
    (yyval.insert_stmt)->select_ = (yyvsp[0].select_stmt);
}
<<<<<<< HEAD
#line 3447 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* optional_identifier_array: '(' identifier_array ')'  */
#line 768 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3455 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* optional_identifier_array: %empty  */
#line 771 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3463 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 778 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3441 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 96: /* optional_identifier_array: '(' identifier_array ')'  */
#line 762 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3449 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 97: /* optional_identifier_array: %empty  */
#line 765 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3457 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 98: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 772 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
<<<<<<< HEAD
#line 3473 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* explain_type: ANALYZE  */
#line 784 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3481 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* explain_type: AST  */
#line 787 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3489 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* explain_type: RAW  */
#line 790 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3497 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* explain_type: LOGICAL  */
#line 793 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3505 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* explain_type: PHYSICAL  */
#line 796 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3513 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 104: /* explain_type: PIPELINE  */
#line 799 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3521 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 105: /* explain_type: %empty  */
#line 802 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3529 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 809 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3467 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 99: /* explain_type: ANALYZE  */
#line 778 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3475 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 100: /* explain_type: AST  */
#line 781 "/home/tangdh/workspace/infinity/src/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3483 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 101: /* explain_type: RAW  */
#line 784 "/home/tangdh/workspace/infinity/src/parser/parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3491 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 102: /* explain_type: LOGICAL  */
#line 787 "/home/tangdh/workspace/infinity/src/parser/parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3499 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 103: /* explain_type: PHYSICAL  */
#line 790 "/home/tangdh/workspace/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3507 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 104: /* explain_type: PIPELINE  */
#line 793 "/home/tangdh/workspace/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3515 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 105: /* explain_type: %empty  */
#line 796 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3523 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 106: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 803 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                       {
    (yyval.update_stmt) = new infinity::UpdateStatement();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.update_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    (yyval.update_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
    (yyval.update_stmt)->where_expr_ = (yyvsp[0].expr_t);
    (yyval.update_stmt)->update_expr_array_ = (yyvsp[-1].update_expr_array_t);
}
<<<<<<< HEAD
#line 3546 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* update_expr_array: update_expr  */
#line 822 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3540 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 107: /* update_expr_array: update_expr  */
#line 816 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
<<<<<<< HEAD
#line 3555 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* update_expr_array: update_expr_array ',' update_expr  */
#line 826 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3549 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 108: /* update_expr_array: update_expr_array ',' update_expr  */
#line 820 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
<<<<<<< HEAD
#line 3564 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* update_expr: IDENTIFIER '=' expr  */
#line 831 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3558 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 109: /* update_expr: IDENTIFIER '=' expr  */
#line 825 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 3576 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 844 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3570 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 110: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 838 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                 {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
<<<<<<< HEAD
#line 3592 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 857 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3586 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 111: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 851 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropCollectionInfo> drop_collection_info = std::make_unique<infinity::DropCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = drop_collection_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
<<<<<<< HEAD
#line 3610 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* drop_statement: DROP TABLE if_exists table_name  */
#line 872 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3604 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 112: /* drop_statement: DROP TABLE if_exists table_name  */
#line 866 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropTableInfo> drop_table_info = std::make_unique<infinity::DropTableInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_table_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_table_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = drop_table_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
<<<<<<< HEAD
#line 3628 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* drop_statement: DROP VIEW if_exists table_name  */
#line 887 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3622 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 113: /* drop_statement: DROP VIEW if_exists table_name  */
#line 881 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                 {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropViewInfo> drop_view_info = std::make_unique<infinity::DropViewInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_view_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_view_info->view_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = drop_view_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
<<<<<<< HEAD
#line 3646 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* drop_statement: DROP INDEX if_exists table_name  */
#line 902 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3640 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 114: /* drop_statement: DROP INDEX if_exists table_name  */
#line 896 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropIndexInfo> drop_index_info = std::make_shared<infinity::DropIndexInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_index_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_index_info->index_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);

    (yyval.drop_stmt)->drop_info_ = drop_index_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
<<<<<<< HEAD
#line 3665 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 921 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3659 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 115: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 915 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                           {
    (yyval.copy_stmt) = new infinity::CopyStatement();

    // Copy To
    (yyval.copy_stmt)->copy_from_ = false;

    // table_name
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.copy_stmt)->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    (yyval.copy_stmt)->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    delete (yyvsp[-6].table_name_t);

    // file path
    (yyval.copy_stmt)->file_path_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));

    // copy options
    infinity::SizeT option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(infinity::SizeT idx = 0; idx < option_count; ++ idx) {
        infinity::CopyOption* option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
        switch(option_ptr->option_type_) {
            case infinity::CopyOptionType::kFormat: {
                (yyval.copy_stmt)->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case infinity::CopyOptionType::kDelimiter: {
                (yyval.copy_stmt)->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case infinity::CopyOptionType::kHeader: {
                (yyval.copy_stmt)->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
<<<<<<< HEAD
#line 3711 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 962 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3705 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 116: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 956 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                               {
    (yyval.copy_stmt) = new infinity::CopyStatement();

    // Copy From
    (yyval.copy_stmt)->copy_from_ = true;

    // table_name
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.copy_stmt)->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    (yyval.copy_stmt)->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    delete (yyvsp[-6].table_name_t);

    // file path
    (yyval.copy_stmt)->file_path_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));

    // copy options
    infinity::SizeT option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(infinity::SizeT idx = 0; idx < option_count; ++ idx) {
        infinity::CopyOption* option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
        switch(option_ptr->option_type_) {
            case infinity::CopyOptionType::kFormat: {
                (yyval.copy_stmt)->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case infinity::CopyOptionType::kDelimiter: {
                (yyval.copy_stmt)->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case infinity::CopyOptionType::kHeader: {
                (yyval.copy_stmt)->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
<<<<<<< HEAD
#line 3757 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* select_statement: select_without_paren  */
#line 1007 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3765 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* select_statement: select_with_paren  */
#line 1010 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3773 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1013 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3751 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 117: /* select_statement: select_without_paren  */
#line 1001 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3759 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 118: /* select_statement: select_with_paren  */
#line 1004 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3767 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 119: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1007 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
<<<<<<< HEAD
#line 3787 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1022 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3781 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 120: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1016 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
<<<<<<< HEAD
#line 3801 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* select_with_paren: '(' select_without_paren ')'  */
#line 1032 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3809 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* select_with_paren: '(' select_with_paren ')'  */
#line 1035 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3817 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1039 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3795 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 121: /* select_with_paren: '(' select_without_paren ')'  */
#line 1026 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3803 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 122: /* select_with_paren: '(' select_with_paren ')'  */
#line 1029 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3811 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 123: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1033 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
<<<<<<< HEAD
#line 3826 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1044 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3820 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 124: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1038 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
<<<<<<< HEAD
#line 3837 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1051 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3845 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1054 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3853 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1059 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3831 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 125: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1045 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3839 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 126: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1048 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3847 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 127: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1053 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                                  {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_list_ = (yyvsp[-4].expr_array_t);
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-5].bool_value);
    (yyval.select_stmt)->table_ref_ = (yyvsp[-3].table_reference_t);
    (yyval.select_stmt)->where_expr_ = (yyvsp[-2].expr_t);
    (yyval.select_stmt)->group_by_list_ = (yyvsp[-1].expr_array_t);
    (yyval.select_stmt)->having_expr_ = (yyvsp[0].expr_t);

    if((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
}
<<<<<<< HEAD
#line 3872 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1074 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3880 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* order_by_clause: %empty  */
#line 1077 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3888 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* order_by_expr_list: order_by_expr  */
#line 1081 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3866 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 128: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1068 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3874 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 129: /* order_by_clause: %empty  */
#line 1071 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3882 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 130: /* order_by_expr_list: order_by_expr  */
#line 1075 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
<<<<<<< HEAD
#line 3897 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1085 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3891 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 131: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1079 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
<<<<<<< HEAD
#line 3906 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* order_by_expr: expr order_by_type  */
#line 1090 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 3900 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 132: /* order_by_expr: expr order_by_type  */
#line 1084 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
<<<<<<< HEAD
#line 3916 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 133: /* order_by_type: ASC  */
#line 1096 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 3924 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 134: /* order_by_type: DESC  */
#line 1099 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
=======
#line 3910 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 133: /* order_by_type: ASC  */
#line 1090 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 3918 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 134: /* order_by_type: DESC  */
#line 1093 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 3926 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 135: /* order_by_type: %empty  */
#line 1096 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 3934 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 136: /* limit_expr: LIMIT expr  */
#line 1100 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3942 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 137: /* limit_expr: %empty  */
#line 1104 "/home/tangdh/workspace/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3948 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 138: /* offset_expr: OFFSET expr  */
#line 1106 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
>>>>>>> 74edf86 (add csv import)
}
#line 3956 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 135: /* order_by_type: %empty  */
#line 1102 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
  case 139: /* offset_expr: %empty  */
#line 1110 "/home/tangdh/workspace/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3962 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 140: /* distinct: DISTINCT  */
#line 1112 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3970 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 141: /* distinct: %empty  */
#line 1115 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
<<<<<<< HEAD
#line 3940 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* limit_expr: LIMIT expr  */
#line 1106 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3948 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* limit_expr: %empty  */
#line 1110 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3954 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* offset_expr: OFFSET expr  */
#line 1112 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
=======
#line 3978 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 142: /* from_clause: FROM table_reference  */
#line 1119 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
>>>>>>> 74edf86 (add csv import)
}
#line 3986 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 139: /* offset_expr: %empty  */
#line 1116 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3968 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* distinct: DISTINCT  */
#line 1118 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3976 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* distinct: %empty  */
#line 1121 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3984 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* from_clause: FROM table_reference  */
#line 1125 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3992 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* from_clause: %empty  */
#line 1128 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4000 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1132 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4008 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1135 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4016 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1139 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4024 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1142 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4032 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1146 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4040 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1149 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4048 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1153 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4056 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1156 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4064 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1159 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4072 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1162 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4080 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1170 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4088 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1173 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
  case 143: /* from_clause: %empty  */
#line 1122 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3994 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1126 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4002 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1129 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4010 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1133 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4018 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1136 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4026 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1140 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4034 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1143 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4042 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1147 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4050 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1150 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4058 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1153 "/home/tangdh/workspace/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4066 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1156 "/home/tangdh/workspace/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4074 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1164 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4082 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1167 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                           {
    infinity::CrossProductReference* cross_product_ref = nullptr;
    if((yyvsp[-2].table_reference_t)->type_ == infinity::TableRefType::kCrossProduct) {
        cross_product_ref = (infinity::CrossProductReference*)(yyvsp[-2].table_reference_t);
        cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
    } else {
        cross_product_ref = new infinity::CrossProductReference();
        cross_product_ref->tables_.emplace_back((yyvsp[-2].table_reference_t));
        cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
    }

    (yyval.table_reference_t) = cross_product_ref;
}
<<<<<<< HEAD
#line 4106 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1190 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4100 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1184 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                              {
    infinity::TableReference* table_ref = new infinity::TableReference();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        table_ref->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    table_ref->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    table_ref->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = table_ref;
}
<<<<<<< HEAD
#line 4124 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1204 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4118 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1198 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
<<<<<<< HEAD
#line 4135 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1213 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4129 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1207 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
<<<<<<< HEAD
#line 4147 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1220 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4141 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1214 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[-2].str_value));
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
<<<<<<< HEAD
#line 4161 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1231 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4155 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1225 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
<<<<<<< HEAD
#line 4171 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1236 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4165 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1230 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
<<<<<<< HEAD
#line 4181 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4175 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1235 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
<<<<<<< HEAD
#line 4192 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1247 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1254 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4208 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1257 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4216 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1261 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4186 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1241 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4194 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1248 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4202 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1251 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4210 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1255 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
<<<<<<< HEAD
#line 4225 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1264 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4219 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1258 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
<<<<<<< HEAD
#line 4234 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1269 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4228 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1263 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4246 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1281 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4240 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1275 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
<<<<<<< HEAD
#line 4258 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1288 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4252 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1282 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
<<<<<<< HEAD
#line 4271 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1302 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4279 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1305 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4287 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1308 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4295 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1311 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4303 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1314 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4311 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1317 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4319 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1320 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 4326 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 180: /* show_statement: SHOW TABLES  */
#line 1326 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4265 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1296 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4273 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1299 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4281 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1302 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4289 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1305 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4297 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1308 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4305 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1311 "/home/tangdh/workspace/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4313 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1314 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                {
}
#line 4320 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 180: /* show_statement: SHOW TABLES  */
#line 1320 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
<<<<<<< HEAD
#line 4335 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 181: /* show_statement: SHOW VIEWS  */
#line 1330 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4329 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 181: /* show_statement: SHOW VIEWS  */
#line 1324 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
<<<<<<< HEAD
#line 4344 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 182: /* show_statement: DESCRIBE table_name  */
#line 1334 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4338 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 182: /* show_statement: DESCRIBE table_name  */
#line 1328 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kColumns;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
<<<<<<< HEAD
#line 4360 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 183: /* flush_statement: FLUSH DATA  */
#line 1349 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4354 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 183: /* flush_statement: FLUSH DATA  */
#line 1343 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
<<<<<<< HEAD
#line 4369 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 184: /* flush_statement: FLUSH LOG  */
#line 1353 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4363 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 184: /* flush_statement: FLUSH LOG  */
#line 1347 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
<<<<<<< HEAD
#line 4378 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 185: /* flush_statement: FLUSH BUFFER  */
#line 1357 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4372 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 185: /* flush_statement: FLUSH BUFFER  */
#line 1351 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
<<<<<<< HEAD
#line 4387 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 186: /* expr_array: expr_alias  */
#line 1366 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4381 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 186: /* expr_array: expr_alias  */
#line 1360 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
<<<<<<< HEAD
#line 4396 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 187: /* expr_array: expr_array ',' expr_alias  */
#line 1370 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4390 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 187: /* expr_array: expr_array ',' expr_alias  */
#line 1364 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
<<<<<<< HEAD
#line 4405 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 188: /* expr_alias: expr AS IDENTIFIER  */
#line 1386 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4399 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 188: /* expr_alias: expr AS IDENTIFIER  */
#line 1380 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4416 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 189: /* expr_alias: expr  */
#line 1392 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4424 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 195: /* operand: '(' expr ')'  */
#line 1402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4432 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 196: /* operand: '(' select_without_paren ')'  */
#line 1405 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4410 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 189: /* expr_alias: expr  */
#line 1386 "/home/tangdh/workspace/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4418 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 195: /* operand: '(' expr ')'  */
#line 1396 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4426 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 196: /* operand: '(' select_without_paren ')'  */
#line 1399 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 4443 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: IDENTIFIER '(' ')'  */
#line 1417 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4437 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: IDENTIFIER '(' ')'  */
#line 1411 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4456 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1425 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4450 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1419 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4469 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 204: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1433 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4463 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 204: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1427 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4483 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 205: /* function_expr: operand IS NOT NULLABLE  */
#line 1442 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4477 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 205: /* function_expr: operand IS NOT NULLABLE  */
#line 1436 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4495 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 206: /* function_expr: operand IS NULLABLE  */
#line 1449 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4489 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 206: /* function_expr: operand IS NULLABLE  */
#line 1443 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4507 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 207: /* function_expr: NOT operand  */
#line 1456 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4501 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 207: /* function_expr: NOT operand  */
#line 1450 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4519 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 208: /* function_expr: '-' operand  */
#line 1463 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4513 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 208: /* function_expr: '-' operand  */
#line 1457 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4531 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 209: /* function_expr: '+' operand  */
#line 1470 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4525 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 209: /* function_expr: '+' operand  */
#line 1464 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4543 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 210: /* function_expr: operand '-' operand  */
#line 1477 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4537 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 210: /* function_expr: operand '-' operand  */
#line 1471 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4556 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 211: /* function_expr: operand '+' operand  */
#line 1485 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4550 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 211: /* function_expr: operand '+' operand  */
#line 1479 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4569 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 212: /* function_expr: operand '*' operand  */
#line 1493 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4563 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 212: /* function_expr: operand '*' operand  */
#line 1487 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4582 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 213: /* function_expr: operand '/' operand  */
#line 1501 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4576 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 213: /* function_expr: operand '/' operand  */
#line 1495 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4595 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 214: /* function_expr: operand '%' operand  */
#line 1509 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4589 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 214: /* function_expr: operand '%' operand  */
#line 1503 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4608 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 215: /* function_expr: operand '=' operand  */
#line 1517 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4602 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 215: /* function_expr: operand '=' operand  */
#line 1511 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4621 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 216: /* function_expr: operand EQUAL operand  */
#line 1525 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4615 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 216: /* function_expr: operand EQUAL operand  */
#line 1519 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4634 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 217: /* function_expr: operand NOT_EQ operand  */
#line 1533 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4628 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 217: /* function_expr: operand NOT_EQ operand  */
#line 1527 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4647 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 218: /* function_expr: operand '<' operand  */
#line 1541 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4641 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 218: /* function_expr: operand '<' operand  */
#line 1535 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4660 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 219: /* function_expr: operand '>' operand  */
#line 1549 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4654 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 219: /* function_expr: operand '>' operand  */
#line 1543 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4673 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 220: /* function_expr: operand LESS_EQ operand  */
#line 1557 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4667 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 220: /* function_expr: operand LESS_EQ operand  */
#line 1551 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4686 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 221: /* function_expr: operand GREATER_EQ operand  */
#line 1565 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4680 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 221: /* function_expr: operand GREATER_EQ operand  */
#line 1559 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4699 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 222: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 1573 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4693 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 222: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 1567 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    if(strcmp((yyvsp[-3].str_value), "year") == 0) {
        func_expr->func_name_ = "extract_year";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp((yyvsp[-3].str_value), "month") == 0) {
        func_expr->func_name_ = "extract_month";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp((yyvsp[-3].str_value), "day") == 0) {
        func_expr->func_name_ = "extract_day";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp((yyvsp[-3].str_value), "hour") == 0) {
        func_expr->func_name_ = "extract_hour";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp((yyvsp[-3].str_value), "minute") == 0) {
        func_expr->func_name_ = "extract_minute";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else if(strcmp((yyvsp[-3].str_value), "second") == 0) {
        func_expr->func_name_ = "extract_second";
        func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    } else {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    free((yyvsp[-3].str_value));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4733 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 223: /* function_expr: operand LIKE operand  */
#line 1602 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4727 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 223: /* function_expr: operand LIKE operand  */
#line 1596 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4746 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 224: /* function_expr: operand NOT LIKE operand  */
#line 1610 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4740 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 224: /* function_expr: operand NOT LIKE operand  */
#line 1604 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4759 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 225: /* conjunction_expr: expr AND expr  */
#line 1619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4753 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 225: /* conjunction_expr: expr AND expr  */
#line 1613 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4772 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 226: /* conjunction_expr: expr OR expr  */
#line 1627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4766 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 226: /* conjunction_expr: expr OR expr  */
#line 1621 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 4785 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 227: /* between_expr: operand BETWEEN operand AND operand  */
#line 1636 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4779 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 227: /* between_expr: operand BETWEEN operand AND operand  */
#line 1630 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
<<<<<<< HEAD
#line 4797 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 228: /* in_expr: operand IN '(' expr_array ')'  */
#line 1644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4791 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 228: /* in_expr: operand IN '(' expr_array ')'  */
#line 1638 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
<<<<<<< HEAD
#line 4808 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 229: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4802 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 229: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1644 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
<<<<<<< HEAD
#line 4819 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 230: /* case_expr: CASE expr case_check_array END  */
#line 1657 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4813 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 230: /* case_expr: CASE expr case_check_array END  */
#line 1651 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 4830 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 231: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1663 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4824 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 231: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1657 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 4842 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 232: /* case_expr: CASE case_check_array END  */
#line 1670 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4836 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 232: /* case_expr: CASE case_check_array END  */
#line 1664 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 4852 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 233: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1675 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4846 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 233: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1669 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 4863 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 234: /* case_check_array: WHEN expr THEN expr  */
#line 1682 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4857 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 234: /* case_check_array: WHEN expr THEN expr  */
#line 1676 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
<<<<<<< HEAD
#line 4875 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 235: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1689 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4869 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 235: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1683 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
<<<<<<< HEAD
#line 4887 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 236: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1697 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4881 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 236: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1691 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }
    infinity::CastExpr* cast_expr = new infinity::CastExpr((yyvsp[-1].column_type_t).logical_type_, type_info_ptr);
    cast_expr->expr_ = (yyvsp[-3].expr_t);
    (yyval.expr_t) = cast_expr;
}
<<<<<<< HEAD
#line 4915 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 237: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1721 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4906 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 237: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1712 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 4926 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 238: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1727 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4917 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 238: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1718 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 4937 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 239: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1733 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4928 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 239: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1724 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 4949 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 240: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1740 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4940 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 240: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1731 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 4961 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 241: /* column_expr: IDENTIFIER  */
#line 1748 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4952 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 241: /* column_expr: IDENTIFIER  */
#line 1739 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 4973 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 242: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1755 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4964 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 242: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1746 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 4985 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 243: /* column_expr: '*'  */
#line 1762 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4976 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 243: /* column_expr: '*'  */
#line 1753 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 4995 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 244: /* column_expr: column_expr '.' '*'  */
#line 1767 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 4986 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 244: /* column_expr: column_expr '.' '*'  */
#line 1758 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 5009 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 245: /* constant_expr: STRING  */
#line 1777 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5000 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 245: /* constant_expr: STRING  */
#line 1768 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5019 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 246: /* constant_expr: TRUE  */
#line 1782 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5010 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 246: /* constant_expr: TRUE  */
#line 1773 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 247: /* constant_expr: FALSE  */
#line 1787 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5020 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 247: /* constant_expr: FALSE  */
#line 1778 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5039 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 248: /* constant_expr: DOUBLE_VALUE  */
#line 1792 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5030 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 248: /* constant_expr: DOUBLE_VALUE  */
#line 1783 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5049 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 249: /* constant_expr: LONG_VALUE  */
#line 1797 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5040 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 249: /* constant_expr: LONG_VALUE  */
#line 1788 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5059 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 250: /* constant_expr: DATE STRING  */
#line 1802 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5050 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 250: /* constant_expr: DATE STRING  */
#line 1793 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5069 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 251: /* constant_expr: INTERVAL interval_expr  */
#line 1807 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 252: /* constant_expr: interval_expr  */
#line 1810 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5085 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 253: /* interval_expr: LONG_VALUE SECONDS  */
#line 1814 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5060 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 251: /* constant_expr: INTERVAL interval_expr  */
#line 1798 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5068 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 252: /* constant_expr: interval_expr  */
#line 1801 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5076 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 253: /* interval_expr: LONG_VALUE SECONDS  */
#line 1805 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5096 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 254: /* interval_expr: LONG_VALUE SECOND  */
#line 1820 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5087 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 254: /* interval_expr: LONG_VALUE SECOND  */
#line 1811 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5107 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 255: /* interval_expr: LONG_VALUE MINUTES  */
#line 1826 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5098 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 255: /* interval_expr: LONG_VALUE MINUTES  */
#line 1817 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5118 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 256: /* interval_expr: LONG_VALUE MINUTE  */
#line 1832 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5109 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 256: /* interval_expr: LONG_VALUE MINUTE  */
#line 1823 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5129 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 257: /* interval_expr: LONG_VALUE HOURS  */
#line 1838 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
=======
#line 5120 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 257: /* interval_expr: LONG_VALUE HOURS  */
#line 1829 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5131 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 258: /* interval_expr: LONG_VALUE HOUR  */
#line 1835 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5142 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 259: /* interval_expr: LONG_VALUE DAYS  */
#line 1841 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5153 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 260: /* interval_expr: LONG_VALUE DAY  */
#line 1847 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                 {
>>>>>>> 74edf86 (add csv import)
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5164 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 258: /* interval_expr: LONG_VALUE HOUR  */
#line 1844 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
=======
  case 261: /* interval_expr: LONG_VALUE MONTHS  */
#line 1853 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                    {
>>>>>>> 74edf86 (add csv import)
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5175 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 259: /* interval_expr: LONG_VALUE DAYS  */
#line 1850 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
=======
  case 262: /* interval_expr: LONG_VALUE MONTH  */
#line 1859 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
>>>>>>> 74edf86 (add csv import)
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5186 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 260: /* interval_expr: LONG_VALUE DAY  */
#line 1856 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
=======
  case 263: /* interval_expr: LONG_VALUE YEARS  */
#line 1865 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
>>>>>>> 74edf86 (add csv import)
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5197 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 261: /* interval_expr: LONG_VALUE MONTHS  */
#line 1862 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
=======
  case 264: /* interval_expr: LONG_VALUE YEAR  */
#line 1871 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                  {
>>>>>>> 74edf86 (add csv import)
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5208 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

<<<<<<< HEAD
  case 262: /* interval_expr: LONG_VALUE MONTH  */
#line 1868 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5195 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 263: /* interval_expr: LONG_VALUE YEARS  */
#line 1874 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5206 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 264: /* interval_expr: LONG_VALUE YEAR  */
#line 1880 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5217 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 265: /* copy_option_list: copy_option  */
#line 1891 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
  case 265: /* copy_option_list: copy_option  */
#line 1882 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
<<<<<<< HEAD
#line 5226 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 266: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1895 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5217 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 266: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1886 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
<<<<<<< HEAD
#line 5235 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 267: /* copy_option: FORMAT IDENTIFIER  */
#line 1900 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5226 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 267: /* copy_option: FORMAT IDENTIFIER  */
#line 1891 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                                {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    if (strcasecmp((yyvsp[0].str_value), "csv") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kCSV;
        free((yyvsp[0].str_value));
    } else if (strcasecmp((yyvsp[0].str_value), "json") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
<<<<<<< HEAD
#line 5256 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 268: /* copy_option: DELIMITER STRING  */
#line 1916 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5247 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 268: /* copy_option: DELIMITER STRING  */
#line 1907 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 5267 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 269: /* copy_option: HEADER  */
#line 1922 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 5258 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 269: /* copy_option: HEADER  */
#line 1913 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
<<<<<<< HEAD
#line 5277 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 270: /* file_path: STRING  */
#line 1928 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5285 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 271: /* if_exists: IF EXISTS  */
#line 1932 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5291 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 272: /* if_exists: %empty  */
#line 1933 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5297 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 273: /* if_not_exists: IF NOT EXISTS  */
#line 1935 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5303 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 274: /* if_not_exists: %empty  */
#line 1936 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5309 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 5313 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
=======
#line 5268 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 270: /* file_path: STRING  */
#line 1919 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5276 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 271: /* if_exists: IF EXISTS  */
#line 1923 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5282 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 272: /* if_exists: %empty  */
#line 1924 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5288 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 273: /* if_not_exists: IF NOT EXISTS  */
#line 1926 "/home/tangdh/workspace/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5294 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;

  case 274: /* if_not_exists: %empty  */
#line 1927 "/home/tangdh/workspace/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5300 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
    break;


#line 5304 "/home/tangdh/workspace/infinity/src/parser/parser.cpp"
>>>>>>> 74edf86 (add csv import)

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == SQLEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, scanner, result, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= SQLEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == SQLEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, scanner, result);
          yychar = SQLEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, scanner, result);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, result, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != SQLEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, result);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, scanner, result);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

<<<<<<< HEAD
#line 1942 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
=======
#line 1933 "/home/tangdh/workspace/infinity/src/parser/parser.y"
>>>>>>> 74edf86 (add csv import)


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
#line 2 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"

#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg);

#line 87 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
  YYSYMBOL_SCHEMA = 25,                    /* SCHEMA  */
  YYSYMBOL_TABLE = 26,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 27,                /* COLLECTION  */
  YYSYMBOL_TABLES = 28,                    /* TABLES  */
  YYSYMBOL_INTO = 29,                      /* INTO  */
  YYSYMBOL_VALUES = 30,                    /* VALUES  */
  YYSYMBOL_AST = 31,                       /* AST  */
  YYSYMBOL_PIPELINE = 32,                  /* PIPELINE  */
  YYSYMBOL_UNOPT = 33,                     /* UNOPT  */
  YYSYMBOL_OPT = 34,                       /* OPT  */
  YYSYMBOL_LOGICAL = 35,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 36,                  /* PHYSICAL  */
  YYSYMBOL_VIEW = 37,                      /* VIEW  */
  YYSYMBOL_INDEX = 38,                     /* INDEX  */
  YYSYMBOL_GROUP = 39,                     /* GROUP  */
  YYSYMBOL_BY = 40,                        /* BY  */
  YYSYMBOL_HAVING = 41,                    /* HAVING  */
  YYSYMBOL_AS = 42,                        /* AS  */
  YYSYMBOL_NATURAL = 43,                   /* NATURAL  */
  YYSYMBOL_JOIN = 44,                      /* JOIN  */
  YYSYMBOL_LEFT = 45,                      /* LEFT  */
  YYSYMBOL_RIGHT = 46,                     /* RIGHT  */
  YYSYMBOL_OUTER = 47,                     /* OUTER  */
  YYSYMBOL_FULL = 48,                      /* FULL  */
  YYSYMBOL_ON = 49,                        /* ON  */
  YYSYMBOL_INNER = 50,                     /* INNER  */
  YYSYMBOL_CROSS = 51,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 52,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 53,                     /* WHERE  */
  YYSYMBOL_ORDER = 54,                     /* ORDER  */
  YYSYMBOL_LIMIT = 55,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 56,                    /* OFFSET  */
  YYSYMBOL_ASC = 57,                       /* ASC  */
  YYSYMBOL_DESC = 58,                      /* DESC  */
  YYSYMBOL_IF = 59,                        /* IF  */
  YYSYMBOL_NOT = 60,                       /* NOT  */
  YYSYMBOL_EXISTS = 61,                    /* EXISTS  */
  YYSYMBOL_IN = 62,                        /* IN  */
  YYSYMBOL_FROM = 63,                      /* FROM  */
  YYSYMBOL_TO = 64,                        /* TO  */
  YYSYMBOL_WITH = 65,                      /* WITH  */
  YYSYMBOL_DELIMITER = 66,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 67,                    /* FORMAT  */
  YYSYMBOL_HEADER = 68,                    /* HEADER  */
  YYSYMBOL_CAST = 69,                      /* CAST  */
  YYSYMBOL_END = 70,                       /* END  */
  YYSYMBOL_CASE = 71,                      /* CASE  */
  YYSYMBOL_ELSE = 72,                      /* ELSE  */
  YYSYMBOL_THEN = 73,                      /* THEN  */
  YYSYMBOL_WHEN = 74,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 75,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 76,                   /* INTEGER  */
  YYSYMBOL_TINYINT = 77,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 78,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 79,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 80,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 81,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 82,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 83,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 84,                    /* DOUBLE  */
  YYSYMBOL_REAL = 85,                      /* REAL  */
  YYSYMBOL_DECIMAL = 86,                   /* DECIMAL  */
  YYSYMBOL_DATE = 87,                      /* DATE  */
  YYSYMBOL_TIME = 88,                      /* TIME  */
  YYSYMBOL_DATETIME = 89,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 90,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 91,                      /* UUID  */
  YYSYMBOL_POINT = 92,                     /* POINT  */
  YYSYMBOL_LINE = 93,                      /* LINE  */
  YYSYMBOL_LSEG = 94,                      /* LSEG  */
  YYSYMBOL_BOX = 95,                       /* BOX  */
  YYSYMBOL_PATH = 96,                      /* PATH  */
  YYSYMBOL_POLYGON = 97,                   /* POLYGON  */
  YYSYMBOL_CIRCLE = 98,                    /* CIRCLE  */
  YYSYMBOL_BLOB = 99,                      /* BLOB  */
  YYSYMBOL_BITMAP = 100,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 101,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 102,                   /* VECTOR  */
  YYSYMBOL_BIT = 103,                      /* BIT  */
  YYSYMBOL_PRIMARY = 104,                  /* PRIMARY  */
  YYSYMBOL_KEY = 105,                      /* KEY  */
  YYSYMBOL_UNIQUE = 106,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 107,                 /* NULLABLE  */
  YYSYMBOL_IS = 108,                       /* IS  */
  YYSYMBOL_TRUE = 109,                     /* TRUE  */
  YYSYMBOL_FALSE = 110,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 111,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 112,                   /* SECOND  */
  YYSYMBOL_SECONDS = 113,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 114,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 115,                  /* MINUTES  */
  YYSYMBOL_HOUR = 116,                     /* HOUR  */
  YYSYMBOL_HOURS = 117,                    /* HOURS  */
  YYSYMBOL_DAY = 118,                      /* DAY  */
  YYSYMBOL_DAYS = 119,                     /* DAYS  */
  YYSYMBOL_MONTH = 120,                    /* MONTH  */
  YYSYMBOL_MONTHS = 121,                   /* MONTHS  */
  YYSYMBOL_YEAR = 122,                     /* YEAR  */
  YYSYMBOL_YEARS = 123,                    /* YEARS  */
  YYSYMBOL_EQUAL = 124,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 125,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 126,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 127,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 128,                  /* BETWEEN  */
  YYSYMBOL_AND = 129,                      /* AND  */
  YYSYMBOL_OR = 130,                       /* OR  */
  YYSYMBOL_EXTRACT = 131,                  /* EXTRACT  */
  YYSYMBOL_NUMBER = 132,                   /* NUMBER  */
  YYSYMBOL_133_ = 133,                     /* '='  */
  YYSYMBOL_134_ = 134,                     /* '<'  */
  YYSYMBOL_135_ = 135,                     /* '>'  */
  YYSYMBOL_136_ = 136,                     /* '+'  */
  YYSYMBOL_137_ = 137,                     /* '-'  */
  YYSYMBOL_138_ = 138,                     /* '*'  */
  YYSYMBOL_139_ = 139,                     /* '/'  */
  YYSYMBOL_140_ = 140,                     /* '%'  */
  YYSYMBOL_141_ = 141,                     /* '['  */
  YYSYMBOL_142_ = 142,                     /* ']'  */
  YYSYMBOL_143_ = 143,                     /* '('  */
  YYSYMBOL_144_ = 144,                     /* ')'  */
  YYSYMBOL_145_ = 145,                     /* '.'  */
  YYSYMBOL_146_ = 146,                     /* ';'  */
  YYSYMBOL_147_ = 147,                     /* ','  */
  YYSYMBOL_YYACCEPT = 148,                 /* $accept  */
  YYSYMBOL_input_pattern = 149,            /* input_pattern  */
  YYSYMBOL_statement_list = 150,           /* statement_list  */
  YYSYMBOL_statement = 151,                /* statement  */
  YYSYMBOL_explainable_statement = 152,    /* explainable_statement  */
  YYSYMBOL_create_statement = 153,         /* create_statement  */
  YYSYMBOL_table_element_array = 154,      /* table_element_array  */
  YYSYMBOL_table_element = 155,            /* table_element  */
  YYSYMBOL_table_column = 156,             /* table_column  */
  YYSYMBOL_column_type = 157,              /* column_type  */
  YYSYMBOL_column_constraints = 158,       /* column_constraints  */
  YYSYMBOL_column_constraint = 159,        /* column_constraint  */
  YYSYMBOL_table_constraint = 160,         /* table_constraint  */
  YYSYMBOL_identifier_array = 161,         /* identifier_array  */
  YYSYMBOL_delete_statement = 162,         /* delete_statement  */
  YYSYMBOL_insert_statement = 163,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 164, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 165,        /* explain_statement  */
  YYSYMBOL_explain_type = 166,             /* explain_type  */
  YYSYMBOL_update_statement = 167,         /* update_statement  */
  YYSYMBOL_update_expr_array = 168,        /* update_expr_array  */
  YYSYMBOL_update_expr = 169,              /* update_expr  */
  YYSYMBOL_drop_statement = 170,           /* drop_statement  */
  YYSYMBOL_copy_statement = 171,           /* copy_statement  */
  YYSYMBOL_select_statement = 172,         /* select_statement  */
  YYSYMBOL_select_with_paren = 173,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 174,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 175, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 176, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 177, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 178,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 179,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 180,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 181,            /* order_by_type  */
  YYSYMBOL_limit_expr = 182,               /* limit_expr  */
  YYSYMBOL_offset_expr = 183,              /* offset_expr  */
  YYSYMBOL_distinct = 184,                 /* distinct  */
  YYSYMBOL_from_clause = 185,              /* from_clause  */
  YYSYMBOL_where_clause = 186,             /* where_clause  */
  YYSYMBOL_having_clause = 187,            /* having_clause  */
  YYSYMBOL_group_by_clause = 188,          /* group_by_clause  */
  YYSYMBOL_set_operator = 189,             /* set_operator  */
  YYSYMBOL_table_reference = 190,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 191,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 192,     /* table_reference_name  */
  YYSYMBOL_table_name = 193,               /* table_name  */
  YYSYMBOL_table_alias = 194,              /* table_alias  */
  YYSYMBOL_with_clause = 195,              /* with_clause  */
  YYSYMBOL_with_expr_list = 196,           /* with_expr_list  */
  YYSYMBOL_with_expr = 197,                /* with_expr  */
  YYSYMBOL_join_clause = 198,              /* join_clause  */
  YYSYMBOL_join_type = 199,                /* join_type  */
  YYSYMBOL_show_statement = 200,           /* show_statement  */
  YYSYMBOL_expr_array = 201,               /* expr_array  */
  YYSYMBOL_constant_expr_array = 202,      /* constant_expr_array  */
  YYSYMBOL_expr_alias = 203,               /* expr_alias  */
  YYSYMBOL_expr = 204,                     /* expr  */
  YYSYMBOL_operand = 205,                  /* operand  */
  YYSYMBOL_function_expr = 206,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 207,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 208,             /* between_expr  */
  YYSYMBOL_in_expr = 209,                  /* in_expr  */
  YYSYMBOL_case_expr = 210,                /* case_expr  */
  YYSYMBOL_case_check_array = 211,         /* case_check_array  */
  YYSYMBOL_cast_expr = 212,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 213,            /* subquery_expr  */
  YYSYMBOL_column_expr = 214,              /* column_expr  */
  YYSYMBOL_constant_expr = 215,            /* constant_expr  */
  YYSYMBOL_interval_expr = 216,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 217,         /* copy_option_list  */
  YYSYMBOL_copy_option = 218,              /* copy_option  */
  YYSYMBOL_file_path = 219,                /* file_path  */
  YYSYMBOL_if_exists = 220,                /* if_exists  */
  YYSYMBOL_if_not_exists = 221,            /* if_not_exists  */
  YYSYMBOL_semicolon = 222                 /* semicolon  */
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
#define YYFINAL  54
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   663

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  148
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  75
/* YYNRULES -- Number of rules.  */
#define YYNRULES  260
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  520

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   387


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
       2,     2,     2,     2,     2,     2,     2,   140,     2,     2,
     143,   144,   138,   136,   147,   137,   145,   139,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   146,
     134,   133,   135,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   141,     2,   142,     2,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   368,   368,   372,   378,   385,   386,   387,   388,   389,
     390,   391,   392,   393,   395,   396,   397,   398,   399,   400,
     401,   402,   409,   419,   434,   458,   474,   491,   510,   514,
     520,   523,   529,   567,   608,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,   626,   627,   628,   630,   631,   632,   633,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   649,   650,   669,   673,   683,   686,
     689,   692,   696,   701,   708,   713,   722,   736,   748,   760,
     763,   770,   776,   779,   782,   785,   788,   795,   808,   812,
     817,   829,   839,   854,   869,   882,   923,   968,   971,   974,
     983,   993,   996,  1000,  1005,  1012,  1015,  1020,  1035,  1038,
    1042,  1046,  1051,  1057,  1060,  1063,  1067,  1071,  1073,  1077,
    1079,  1082,  1086,  1089,  1093,  1096,  1100,  1103,  1107,  1110,
    1114,  1117,  1120,  1123,  1131,  1134,  1143,  1143,  1145,  1159,
    1168,  1174,  1183,  1187,  1192,  1199,  1202,  1206,  1209,  1214,
    1225,  1232,  1246,  1249,  1252,  1255,  1258,  1261,  1264,  1270,
    1274,  1290,  1294,  1299,  1303,  1308,  1313,  1317,  1318,  1319,
    1320,  1321,  1323,  1326,  1332,  1333,  1334,  1335,  1336,  1338,
    1345,  1352,  1360,  1367,  1374,  1381,  1388,  1396,  1404,  1412,
    1420,  1428,  1436,  1444,  1452,  1460,  1468,  1476,  1484,  1493,
    1501,  1510,  1520,  1527,  1535,  1541,  1548,  1553,  1560,  1567,
    1575,  1608,  1614,  1620,  1627,  1635,  1640,  1645,  1650,  1660,
    1665,  1670,  1675,  1680,  1685,  1690,  1693,  1697,  1703,  1709,
    1715,  1721,  1727,  1733,  1739,  1745,  1751,  1757,  1763,  1774,
    1778,  1783,  1796,  1802,  1808,  1812,  1813,  1815,  1816,  1818,
    1819
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
  "PREPARE", "DESCRIBE", "UNION", "ALL", "INTERSECT", "EXCEPT", "SCHEMA",
  "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "AST", "PIPELINE",
  "UNOPT", "OPT", "LOGICAL", "PHYSICAL", "VIEW", "INDEX", "GROUP", "BY",
  "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL",
  "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET",
  "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH",
  "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE", "ELSE", "THEN",
  "WHEN", "BOOLEAN", "INTEGER", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT",
  "CHAR", "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME",
  "DATETIME", "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH",
  "POLYGON", "CIRCLE", "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT",
  "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS", "TRUE", "FALSE",
  "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS",
  "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ",
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "NUMBER",
  "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'",
  "'('", "')'", "'.'", "';'", "','", "$accept", "input_pattern",
  "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "table_element",
  "table_column", "column_type", "column_constraints", "column_constraint",
  "table_constraint", "identifier_array", "delete_statement",
  "insert_statement", "optional_identifier_array", "explain_statement",
  "explain_type", "update_statement", "update_expr_array", "update_expr",
  "drop_statement", "copy_statement", "select_statement",
  "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "from_clause", "where_clause",
  "having_clause", "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "expr_array",
  "constant_expr_array", "expr_alias", "expr", "operand", "function_expr",
  "conjunction_expr", "between_expr", "in_expr", "case_expr",
  "case_check_array", "cast_expr", "subquery_expr", "column_expr",
  "constant_expr", "interval_expr", "copy_option_list", "copy_option",
  "file_path", "if_exists", "if_not_exists", "semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-328)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-260)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     165,   157,    -2,   133,    32,   -25,    32,   300,    16,    32,
      45,   -46,    50,   -92,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,  -328,   193,  -328,  -328,    56,  -328,    60,    60,    60,
      60,    60,    32,    73,    73,    73,    73,   -77,   126,    32,
      11,  -328,  -328,   120,   130,  -328,   258,  -328,  -328,   131,
      65,  -328,    74,    76,  -328,    46,  -328,   218,  -328,  -328,
      -1,   180,  -328,   197,   189,   261,    32,    32,    32,   263,
     129,   231,   274,    32,    32,   311,   312,   332,   284,   337,
     337,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
     193,  -328,   202,    45,  -328,  -328,  -328,  -328,    -1,  -328,
    -328,  -328,   242,   310,   306,   301,  -328,   -30,  -328,   129,
     328,   379,     4,  -328,  -328,  -328,  -328,  -328,  -328,   250,
     -43,  -328,   242,  -328,  -328,   319,   322,    56,  -328,   245,
     246,   248,  -328,  -328,   432,   331,   253,   254,   238,   394,
    -328,  -328,   393,   265,    25,    25,  -328,   257,   -49,  -328,
      -9,   278,  -328,  -328,  -328,  -328,  -328,  -328,  -328,   264,
    -328,  -328,   242,   242,   354,  -328,   -46,    12,   377,    32,
    -328,   -84,   277,  -328,   242,   332,  -328,    71,   279,   286,
     283,  -328,  -328,   144,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,   287,   356,   242,   242,
      40,   -27,  -328,   432,  -328,    18,    53,    53,   296,     9,
       3,   242,   284,   428,   242,   242,   380,   298,   -34,    25,
      25,    25,    25,    25,    25,    25,    25,    25,    25,    25,
      25,    25,     5,   303,  -328,    68,    71,   242,  -328,   193,
     441,   346,   309,    10,  -328,  -328,  -328,   -46,   318,  -328,
     459,    18,    71,  -328,   304,   304,  -328,   242,  -328,    90,
     356,   325,    -6,    22,   116,  -328,   242,   242,   400,  -328,
    -328,   -46,   323,   178,  -328,   429,  -328,  -328,   433,  -328,
    -328,   352,   339,   257,   381,  -328,   340,   340,   375,   375,
     320,   340,   375,   375,    53,    53,  -328,  -328,  -328,  -328,
    -328,   242,  -328,  -328,  -328,    71,  -328,  -328,  -328,  -328,
    -328,  -328,   347,   348,  -328,  -328,  -328,   349,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
     361,   362,   363,   364,   104,   365,   379,  -328,    12,   193,
     379,  -328,    92,  -328,   485,   500,  -328,   132,  -328,   172,
     177,  -328,   412,  -328,   441,   242,  -328,   242,   -21,    37,
      25,    -3,     3,   513,  -328,  -328,  -328,  -328,  -328,  -328,
     514,   556,  -328,   520,   521,   257,   417,   183,  -328,    25,
    -328,   557,   558,   559,   560,   561,   207,   217,   461,   464,
    -328,  -328,   104,  -328,   379,   195,  -328,   199,  -328,    18,
    -328,  -328,  -328,   304,  -328,  -328,  -328,   426,    71,     1,
    -328,   242,   299,   429,   178,     3,     3,   430,   242,   242,
    -328,   427,   210,  -328,  -328,   360,   431,   434,   211,   435,
     436,   425,   437,   438,   439,   440,   442,   443,   444,   445,
     446,   447,   448,   449,   450,  -328,  -328,  -328,   212,  -328,
    -328,  -328,  -328,  -328,  -328,    71,  -328,  -328,  -328,   525,
     379,   451,    71,  -328,  -328,  -328,  -328,  -328,   570,  -328,
    -328,   571,   575,   576,   577,   582,   593,   594,   595,   596,
     597,   598,   599,   600,   601,  -328,   242,   216,   465,   466,
     467,   468,   469,   470,   471,   472,   473,   474,   475,   476,
     477,   478,   479,    71,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     156,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   156,     0,   260,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   108,   107,     0,     8,   258,   258,   258,
     258,   258,     0,   256,   256,   256,   256,   150,     0,     0,
       0,    92,    96,     0,     0,    95,   156,   169,   170,     0,
     155,   157,     0,     0,     1,   156,     2,   140,   142,   143,
       0,   131,   113,   119,     0,     0,     0,     0,     0,     0,
      90,     0,     0,     0,     0,     0,     0,     0,   135,     0,
       0,    93,    94,    91,    14,    19,    21,    20,    15,    16,
      18,    17,     0,     0,   112,   111,     4,   141,     0,   109,
     110,   130,     0,     0,   127,     0,    22,     0,    23,    90,
       0,     0,   156,   255,   101,   103,   102,   104,   151,     0,
     135,    98,     0,    86,   254,     0,     0,     0,   158,     0,
       0,   225,   229,   232,   233,     0,     0,     0,     0,     0,
     230,   231,     0,     0,     0,     0,   227,   156,   133,   171,
     176,   177,   186,   181,   180,   179,   187,   188,   178,   185,
     184,   236,     0,     0,   129,   257,   156,     0,     0,     0,
      84,     0,     0,    88,     0,     0,    97,   134,     0,     0,
       0,   116,   115,     0,   238,   237,   240,   239,   242,   241,
     244,   243,   246,   245,   248,   247,     0,   156,     0,     0,
       0,     0,   234,     0,   235,     0,   195,   194,     0,     0,
       0,     0,   135,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   118,   120,   125,   126,     0,   114,    25,
       0,     0,     0,     0,    28,    30,    31,   156,     0,    89,
       0,     0,   100,    99,     0,     0,   159,     0,   189,     0,
     156,     0,     0,     0,     0,   216,     0,     0,     0,   183,
     182,   156,   132,   144,   146,   154,   147,   172,   139,   175,
     209,   210,     0,   156,     0,   193,   202,   203,   206,   207,
       0,   201,   204,   205,   197,   196,   198,   199,   200,   226,
     228,     0,   123,   124,   122,   128,    34,    37,    35,    36,
      38,    39,     0,     0,    40,    42,    41,    59,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
       0,     0,     0,     0,    32,     0,     0,    24,     0,    26,
       0,    85,     0,   173,     0,     0,   253,     0,   249,     0,
       0,   190,     0,   221,     0,     0,   214,     0,     0,     0,
       0,     0,     0,     0,   163,   164,   165,   166,   162,   167,
       0,     0,   148,     0,   137,   156,     0,     0,   192,     0,
     121,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    33,    76,     0,     0,    29,     0,    87,     0,
     252,   251,   106,     0,   105,   191,   222,     0,   218,     0,
     217,     0,     0,   154,   145,     0,     0,   152,     0,     0,
     117,     0,     0,   223,   212,   211,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    78,    77,     0,    83,
      27,   174,   250,   220,   215,   219,   208,   149,   160,     0,
       0,   138,   136,   224,   213,    55,    56,    58,     0,    60,
      61,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    82,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   161,   153,    57,    65,    63,    64,    66,
      67,    68,    62,    72,    70,    71,    73,    74,    75,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -328,  -328,  -328,   553,  -328,   578,  -328,   288,  -328,   271,
    -328,   235,  -328,  -327,   583,   584,   519,  -328,  -328,   585,
    -328,   457,   587,   588,   -44,   624,   -10,   509,   539,   -28,
    -328,  -328,   338,  -328,  -328,  -328,  -328,  -328,  -109,  -328,
    -328,  -328,  -328,   276,  -209,    33,   227,  -328,  -328,   548,
    -328,  -328,   602,  -166,  -328,   452,  -122,  -141,  -328,  -328,
    -328,  -328,  -328,   453,  -328,  -328,  -328,  -200,   501,   387,
     241,   565,   270,   228,  -328
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    12,    13,    14,    83,    15,   243,   244,   245,   334,
     392,   393,   246,   171,    16,    17,   112,    18,    46,    19,
     120,   121,    20,    21,    22,    23,    24,    62,    99,    63,
     104,   233,   234,   304,   164,   238,   102,   212,   123,   420,
     374,    60,   272,   273,   274,   275,   372,    25,    50,    51,
     276,   370,    26,   148,   342,   149,   150,   151,   152,   153,
     154,   155,   156,   201,   157,   158,   159,   160,   161,   347,
     348,   125,    72,    65,    56
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     177,    53,    90,   206,   207,   268,    37,    61,   299,   395,
     122,   176,   166,   397,   210,   240,   200,   259,    57,    10,
      58,    59,   132,   133,   134,   209,   284,    32,   131,   132,
     133,   134,   100,   213,   172,    37,   354,    38,    39,    40,
     235,   236,    48,   265,    47,   266,  -259,   267,    49,   410,
      54,   343,   252,     1,    55,     2,     3,     4,     5,     6,
     249,     7,     8,   250,    61,    70,     9,   448,    76,    10,
     130,   454,    78,   285,    79,    80,   262,   263,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   350,   280,   281,   137,   355,   138,    11,   211,   107,
     108,   109,   173,   278,   175,   139,   115,   116,   214,   215,
     411,    10,   139,   167,   199,   305,   241,   377,   242,    64,
     214,   215,   239,   214,   215,   302,   303,   140,   141,   142,
     214,   215,    71,   487,   140,   141,   142,   208,   214,   215,
      77,   413,    98,   300,   358,   359,   271,   131,   132,   133,
     134,   214,   215,   270,   337,    81,   143,   338,    33,    34,
      35,   144,   145,   146,   388,    82,   214,   215,   147,   214,
     215,    36,     1,    92,     2,     3,     4,     5,     6,   235,
       7,     8,    27,    28,    29,     9,   356,   261,   357,    11,
     267,   229,   230,   231,    30,    31,   257,   214,   215,   451,
     214,   215,   248,   339,   135,   136,   458,   459,   389,   422,
     390,   391,    93,   137,    57,   138,    58,    59,    94,   412,
      95,   363,  -168,   364,   365,   366,   367,   361,   368,   369,
      10,   139,   101,   408,   351,   409,   398,   211,   425,   399,
      97,   131,   132,   133,   134,   131,   132,   133,   134,   105,
     352,   103,   461,   140,   141,   142,    66,    67,    68,    69,
     131,   132,   133,   134,   106,     1,   110,     2,     3,     4,
       5,     6,   111,   376,     8,   143,   402,   114,     9,   403,
     144,   145,   146,   431,   432,   433,   434,   147,   258,   455,
     435,   436,   113,   438,   439,   440,   441,   462,   135,   136,
     442,   443,   135,   136,    73,    74,    75,   137,    11,   138,
     437,   137,   199,   138,   117,   118,   404,   135,   136,   403,
     444,   405,    10,    10,   211,   139,   137,   424,   138,   139,
     211,    41,    42,    43,    44,   119,    45,   122,   216,   449,
     217,   124,   250,   450,   139,   127,   250,   140,   141,   142,
     162,   140,   141,   142,   464,   467,   485,   211,   468,   250,
     504,   163,   165,   250,   503,   421,   140,   141,   142,   143,
     344,   345,   346,   143,   144,   145,   146,   169,   144,   145,
     146,   147,   170,   174,   178,   147,   218,   179,   143,   181,
     182,   183,   196,   144,   145,   146,   197,   198,   202,   203,
     147,    11,   219,   220,   221,   222,   223,   218,   205,   232,
     237,   224,   225,   226,   227,   228,   229,   230,   231,   247,
     251,    10,   254,   219,   220,   221,   222,   256,   218,   255,
     260,   279,   224,   225,   226,   227,   228,   229,   230,   231,
     269,   283,   282,   456,   219,   220,   221,   222,   218,   379,
     301,   335,   336,   224,   225,   226,   227,   228,   229,   230,
     231,   340,   341,   360,  -260,  -260,   221,   222,   218,   353,
     362,   371,   373,  -260,   225,   226,   227,   228,   229,   230,
     231,   214,   375,   218,   219,   220,   221,   222,   378,   400,
     381,   382,   383,   224,   225,   226,   227,   228,   229,   230,
     231,  -260,  -260,   401,   384,   385,   386,   387,   394,  -260,
    -260,   227,   228,   229,   230,   231,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   406,   415,   416,   417,
     418,   423,   419,   426,   427,   428,   429,   430,   445,   446,
     453,   463,   471,   460,   486,   465,   488,   489,   466,   469,
     470,   490,   491,   492,   472,   473,   474,   475,   493,   476,
     477,   478,   479,   480,   481,   482,   483,   484,   211,   494,
     495,   496,   497,   498,   499,   500,   501,   502,    96,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,    84,   407,   396,   447,   168,    85,
      86,    87,   253,    88,    89,    52,   180,   129,   414,   380,
     457,   128,   349,   204,   452,   126,     0,     0,    91,     0,
       0,     0,     0,   264,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   277
};

static const yytype_int16 yycheck[] =
{
     122,    11,    46,   144,   145,   205,     3,     8,     3,   336,
      53,   120,    42,   340,    63,     3,   138,   183,    21,    65,
      23,    24,     4,     5,     6,   147,    60,    29,     3,     4,
       5,     6,    60,    42,    30,     3,    42,     4,    63,     6,
     162,   163,     9,    70,    28,    72,     0,    74,     3,    70,
       0,   251,   174,     7,   146,     9,    10,    11,    12,    13,
     144,    15,    16,   147,     8,    32,    20,   394,   145,    65,
      98,    70,    39,   107,    63,    64,   198,   199,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   257,   214,   215,    69,    73,    71,   143,   147,    66,
      67,    68,   112,   212,   147,    87,    73,    74,   129,   130,
      73,    65,    87,   143,    74,   237,   104,   283,   106,    59,
     129,   130,   166,   129,   130,    57,    58,   109,   110,   111,
     129,   130,    59,   460,   109,   110,   111,   147,   129,   130,
      14,   144,   143,   138,   266,   267,   143,     3,     4,     5,
       6,   129,   130,   144,   144,    35,   131,   147,    25,    26,
      27,   136,   137,   138,    60,    35,   129,   130,   143,   129,
     130,    38,     7,    42,     9,    10,    11,    12,    13,   301,
      15,    16,    25,    26,    27,    20,    70,   197,    72,   143,
      74,   138,   139,   140,    37,    38,    52,   129,   130,   399,
     129,   130,   169,   247,    60,    61,   415,   416,   104,   375,
     106,   107,   147,    69,    21,    71,    23,    24,   144,   360,
     144,    43,    44,    45,    46,    47,    48,   271,    50,    51,
      65,    87,    52,   355,   144,   357,   144,   147,   379,   147,
      22,     3,     4,     5,     6,     3,     4,     5,     6,    60,
     260,    54,   418,   109,   110,   111,    28,    29,    30,    31,
       3,     4,     5,     6,     3,     7,     3,     9,    10,    11,
      12,    13,   143,   283,    16,   131,   144,     3,    20,   147,
     136,   137,   138,    76,    77,    78,    79,   143,   144,   411,
      83,    84,    61,    76,    77,    78,    79,   419,    60,    61,
      83,    84,    60,    61,    34,    35,    36,    69,   143,    71,
     103,    69,    74,    71,     3,     3,   144,    60,    61,   147,
     103,   144,    65,    65,   147,    87,    69,   144,    71,    87,
     147,    31,    32,    33,    34,     3,    36,    53,    60,   144,
      62,     4,   147,   144,    87,   143,   147,   109,   110,   111,
      40,   109,   110,   111,   144,   144,   144,   147,   147,   147,
     144,    55,    61,   147,   486,   375,   109,   110,   111,   131,
      66,    67,    68,   131,   136,   137,   138,    49,   136,   137,
     138,   143,     3,   133,    65,   143,   108,    65,   131,   144,
     144,   143,    61,   136,   137,   138,   143,   143,     4,     6,
     143,   143,   124,   125,   126,   127,   128,   108,   143,   145,
      56,   133,   134,   135,   136,   137,   138,   139,   140,    42,
     143,    65,   143,   124,   125,   126,   127,   144,   108,   143,
     143,     3,   133,   134,   135,   136,   137,   138,   139,   140,
     144,   143,    62,   144,   124,   125,   126,   127,   108,   129,
     147,   105,   143,   133,   134,   135,   136,   137,   138,   139,
     140,   143,     3,    63,   124,   125,   126,   127,   108,   144,
     147,    42,    39,   133,   134,   135,   136,   137,   138,   139,
     140,   129,   143,   108,   124,   125,   126,   127,   107,     4,
     143,   143,   143,   133,   134,   135,   136,   137,   138,   139,
     140,   126,   127,     3,   143,   143,   143,   143,   143,   134,
     135,   136,   137,   138,   139,   140,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   144,    44,    44,     3,
      40,   144,    41,     6,     6,     6,     6,     6,   107,   105,
     144,   144,   147,   143,    49,   144,     6,     6,   144,   144,
     144,     6,     6,     6,   147,   147,   147,   147,     6,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,     6,
       6,     6,     6,     6,     6,     6,     6,     6,    55,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,    46,   354,   338,   392,   109,    46,
      46,    46,   175,    46,    46,    11,   127,    98,   362,   301,
     413,    93,   255,   142,   403,    80,    -1,    -1,    46,    -1,
      -1,    -1,    -1,   200,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   211
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      65,   143,   149,   150,   151,   153,   162,   163,   165,   167,
     170,   171,   172,   173,   174,   195,   200,    25,    26,    27,
      37,    38,    29,    25,    26,    27,    38,     3,   193,    63,
     193,    31,    32,    33,    34,    36,   166,    28,   193,     3,
     196,   197,   173,   174,     0,   146,   222,    21,    23,    24,
     189,     8,   175,   177,    59,   221,   221,   221,   221,   221,
     193,    59,   220,   220,   220,   220,   145,    14,   193,    63,
      64,    35,    35,   152,   153,   162,   163,   167,   170,   171,
     172,   200,    42,   147,   144,   144,   151,    22,   143,   176,
     177,    52,   184,    54,   178,    60,     3,   193,   193,   193,
       3,   143,   164,    61,     3,   193,   193,     3,     3,     3,
     168,   169,    53,   186,     4,   219,   219,   143,   197,   176,
     177,     3,     4,     5,     6,    60,    61,    69,    71,    87,
     109,   110,   111,   131,   136,   137,   138,   143,   201,   203,
     204,   205,   206,   207,   208,   209,   210,   212,   213,   214,
     215,   216,    40,    55,   182,    61,    42,   143,   164,    49,
       3,   161,    30,   174,   133,   147,   186,   204,    65,    65,
     175,   144,   144,   143,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    61,   143,   143,    74,
     204,   211,     4,     6,   216,   143,   205,   205,   174,   204,
      63,   147,   185,    42,   129,   130,    60,    62,   108,   124,
     125,   126,   127,   128,   133,   134,   135,   136,   137,   138,
     139,   140,   145,   179,   180,   204,   204,    56,   183,   172,
       3,   104,   106,   154,   155,   156,   160,    42,   193,   144,
     147,   143,   204,   169,   143,   143,   144,    52,   144,   201,
     143,   174,   204,   204,   211,    70,    72,    74,   215,   144,
     144,   143,   190,   191,   192,   193,   198,   203,   186,     3,
     204,   204,    62,   143,    60,   107,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,     3,
     138,   147,    57,    58,   181,   204,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   157,   105,   143,   144,   147,   172,
     143,     3,   202,   215,    66,    67,    68,   217,   218,   217,
     201,   144,   174,   144,    42,    73,    70,    72,   204,   204,
      63,   172,   147,    43,    45,    46,    47,    48,    50,    51,
     199,    42,   194,    39,   188,   143,   174,   201,   107,   129,
     180,   143,   143,   143,   143,   143,   143,   143,    60,   104,
     106,   107,   158,   159,   143,   161,   155,   161,   144,   147,
       4,     3,   144,   147,   144,   144,   144,   157,   204,   204,
      70,    73,   205,   144,   191,    44,    44,     3,    40,    41,
     187,   174,   201,   144,   144,   205,     6,     6,     6,     6,
       6,    76,    77,    78,    79,    83,    84,   103,    76,    77,
      78,    79,    83,    84,   103,   107,   105,   159,   161,   144,
     144,   215,   218,   144,    70,   204,   144,   194,   192,   192,
     143,   201,   204,   144,   144,   144,   144,   144,   147,   144,
     144,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   144,    49,   161,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   204,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   148,   149,   150,   150,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   152,   152,   152,   152,   152,   152,
     152,   152,   153,   153,   153,   153,   153,   153,   154,   154,
     155,   155,   156,   156,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   158,   158,   159,   159,
     159,   159,   160,   160,   161,   161,   162,   163,   163,   164,
     164,   165,   166,   166,   166,   166,   166,   167,   168,   168,
     169,   170,   170,   170,   170,   171,   171,   172,   172,   172,
     172,   173,   173,   174,   175,   176,   176,   177,   178,   178,
     179,   179,   180,   181,   181,   181,   182,   182,   183,   183,
     184,   184,   185,   185,   186,   186,   187,   187,   188,   188,
     189,   189,   189,   189,   190,   190,   191,   191,   192,   192,
     193,   193,   194,   194,   194,   195,   195,   196,   196,   197,
     198,   198,   199,   199,   199,   199,   199,   199,   199,   200,
     200,   201,   201,   202,   202,   203,   203,   204,   204,   204,
     204,   204,   205,   205,   205,   205,   205,   205,   205,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   207,
     207,   208,   209,   209,   210,   210,   210,   210,   211,   211,
     212,   213,   213,   213,   213,   214,   214,   214,   214,   215,
     215,   215,   215,   215,   215,   215,   215,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   217,
     217,   218,   218,   218,   219,   220,   220,   221,   221,   222,
     222
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     7,     6,     7,     9,     1,     3,
       1,     1,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     6,     4,     1,
       4,     4,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     1,     2,     2,     1,
       1,     2,     5,     4,     1,     3,     4,     8,     5,     3,
       0,     3,     1,     2,     2,     1,     1,     5,     1,     3,
       3,     4,     4,     4,     4,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     7,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     5,     0,     2,     0,     1,     3,     5,
       4,     6,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     1,     3,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     3,
       4,     5,     4,     3,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       3,     5,     5,     6,     4,     6,     3,     5,     4,     5,
       6,     4,     5,     5,     6,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       3,     2,     2,     1,     1,     2,     0,     3,     0,     1,
       0
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
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, ParserResult* result)
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, ParserResult* result)
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
                 int yyrule, void *scanner, ParserResult* result)
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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner, ParserResult* result)
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
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1816 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1824 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 150 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1838 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 140 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 1852 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 216 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1863 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 227 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1872 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 227 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1881 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 187 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 1895 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 180 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 1906 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1916 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1926 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1936 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1946 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1956 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1966 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 170 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1980 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 170 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1994 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 246 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2004 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2012 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2020 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2037 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2045 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 160 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2059 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2068 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2086 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 207 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2099 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 236 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2108 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 197 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2122 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 197 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2136 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 252 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2146 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2155 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 160 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2169 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr_array: /* constant_expr_array  */
#line 160 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2183 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2191 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2199 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2207 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2215 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2223 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2231 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2239 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2247 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 264 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2260 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2268 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2276 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2284 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2292 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2300 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2308 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
yyparse (void *scanner, ParserResult* result)
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
#line 71 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2416 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
#line 368 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2631 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 372 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new Vector<BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2642 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 378 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2653 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 385 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2659 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 386 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2665 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 387 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2671 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 388 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2677 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 389 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2683 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 390 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2689 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 391 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2695 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 392 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2701 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 393 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2707 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 14: /* explainable_statement: create_statement  */
#line 395 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2713 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 15: /* explainable_statement: drop_statement  */
#line 396 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2719 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 16: /* explainable_statement: copy_statement  */
#line 397 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2725 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 17: /* explainable_statement: show_statement  */
#line 398 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2731 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 18: /* explainable_statement: select_statement  */
#line 399 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2737 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 19: /* explainable_statement: delete_statement  */
#line 400 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2743 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 20: /* explainable_statement: update_statement  */
#line 401 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2749 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 21: /* explainable_statement: insert_statement  */
#line 402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2755 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 22: /* create_statement: CREATE SCHEMA if_not_exists IDENTIFIER  */
#line 409 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                          {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.create_stmt)->create_info_ = std::move(create_schema_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2768 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 23: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 419 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                             {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateCollectionInfo> create_collection_info = std::make_unique<CreateCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        create_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    create_collection_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.create_stmt)->create_info_ = std::move(create_collection_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 2786 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 24: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 434 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateTableInfo> create_table_info = MakeUnique<CreateTableInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    for (TableElement*& element : *(yyvsp[-1].table_element_array_t)) {
        if(element->type_ == TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((TableConstraint*)element);
        }
    }
    delete (yyvsp[-1].table_element_array_t);

    (yyval.create_stmt)->create_info_ = std::move(create_table_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-4].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2814 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 25: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 458 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                            {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateTableInfo> create_table_info = MakeUnique<CreateTableInfo>();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);

    create_table_info->conflict_type_ = (yyvsp[-3].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    create_table_info->select_ = (yyvsp[0].select_stmt);
    (yyval.create_stmt)->create_info_ = std::move(create_table_info);
}
#line 2834 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 26: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 474 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                     {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateViewInfo> create_view_info = MakeUnique<CreateViewInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_view_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_view_info->view_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    create_view_info->view_columns_ = (yyvsp[-2].identifier_array_t);
    create_view_info->select_ = (yyvsp[0].select_stmt);
    create_view_info->conflict_type_ = (yyvsp[-4].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    (yyval.create_stmt)->create_info_ = std::move(create_view_info);
}
#line 2855 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 27: /* create_statement: CREATE INDEX if_not_exists IDENTIFIER ON table_name '(' identifier_array ')'  */
#line 491 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                               {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateIndexInfo> create_index_info = MakeUnique<CreateIndexInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    create_index_info->index_name_ = (yyvsp[-5].str_value);
    free((yyvsp[-5].str_value));

    create_index_info->column_names_ = (yyvsp[-1].identifier_array_t);
    create_index_info->conflict_type_ = (yyvsp[-6].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    (yyval.create_stmt)->create_info_ = std::move(create_index_info);
}
#line 2878 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 28: /* table_element_array: table_element  */
#line 510 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new Vector<TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 2887 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 29: /* table_element_array: table_element_array ',' table_element  */
#line 514 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 2896 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 30: /* table_element: table_column  */
#line 520 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 2904 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 31: /* table_element: table_constraint  */
#line 523 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 2912 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 32: /* table_column: IDENTIFIER column_type  */
#line 529 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[0].column_type_t).logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make((yyvsp[0].column_type_t).precision, (yyvsp[0].column_type_t).scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make((yyvsp[0].column_type_t).embedding_type_, (yyvsp[0].column_type_t).width);
            break;
        }
    }
    (yyval.table_column_t) = new ColumnDef((yyvsp[0].column_type_t).logical_type_, type_info_ptr);

    (yyval.table_column_t)->name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 2955 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 33: /* table_column: IDENTIFIER column_type column_constraints  */
#line 567 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
    }
    (yyval.table_column_t) = new ColumnDef((yyvsp[-1].column_type_t).logical_type_, type_info_ptr);

    (yyval.table_column_t)->name_ = (yyvsp[-2].str_value);
    (yyval.table_column_t)->constraints_ = (yyvsp[0].column_constraints_t);
    free((yyvsp[-2].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 2999 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 34: /* column_type: BOOLEAN  */
#line 608 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kBoolean}; }
#line 3005 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* column_type: TINYINT  */
#line 609 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kTinyInt}; }
#line 3011 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: SMALLINT  */
#line 610 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kSmallInt}; }
#line 3017 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: INTEGER  */
#line 611 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kInteger}; }
#line 3023 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: BIGINT  */
#line 612 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kBigInt}; }
#line 3029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: HUGEINT  */
#line 613 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kHugeInt}; }
#line 3035 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: FLOAT  */
#line 614 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3041 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: REAL  */
#line 615 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3047 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: DOUBLE  */
#line 616 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kDouble}; }
#line 3053 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: DATE  */
#line 617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kDate}; }
#line 3059 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: TIME  */
#line 618 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kTime}; }
#line 3065 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DATETIME  */
#line 619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kDateTime}; }
#line 3071 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: TIMESTAMP  */
#line 620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = ColumnType{LogicalType::kTimestamp}; }
#line 3077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: UUID  */
#line 621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kUuid}; }
#line 3083 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: POINT  */
#line 622 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kPoint}; }
#line 3089 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: LINE  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLine}; }
#line 3095 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: LSEG  */
#line 624 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLineSeg}; }
#line 3101 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: BOX  */
#line 625 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = ColumnType{LogicalType::kBox}; }
#line 3107 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: PATH  */
#line 626 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kPath}; }
#line 3113 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: POLYGON  */
#line 627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kPolygon}; }
#line 3119 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: CIRCLE  */
#line 628 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kCircle}; }
#line 3125 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 630 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kChar, (yyvsp[-1].long_value)}; }
#line 3131 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 631 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3137 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 632 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3143 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-1].long_value), 0}; }
#line 3149 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: DECIMAL  */
#line 634 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
#line 3155 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 635 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3161 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 636 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = ColumnType{LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3167 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 637 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3173 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 638 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3179 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 639 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3185 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3191 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 641 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3197 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 642 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3203 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 643 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3209 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3215 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 645 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 646 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3227 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 647 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 648 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3239 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 649 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3245 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3251 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_constraints: column_constraint  */
#line 669 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new HashSet<ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3260 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 77: /* column_constraints: column_constraints column_constraint  */
#line 673 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3274 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_constraint: PRIMARY KEY  */
#line 683 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = ConstraintType::kPrimaryKey;
}
#line 3282 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_constraint: UNIQUE  */
#line 686 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = ConstraintType::kUnique;
}
#line 3290 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_constraint: NULLABLE  */
#line 689 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = ConstraintType::kNull;
}
#line 3298 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_constraint: NOT NULLABLE  */
#line 692 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = ConstraintType::kNotNull;
}
#line 3306 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 696 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kPrimaryKey;
}
#line 3316 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 701 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kUnique;
}
#line 3326 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* identifier_array: IDENTIFIER  */
#line 708 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new Vector<String>();
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3336 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 713 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3346 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 86: /* delete_statement: DELETE FROM table_name where_clause  */
#line 722 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                       {
    (yyval.delete_stmt) = new DeleteStatement();

    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.delete_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.delete_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    (yyval.delete_stmt)->where_expr_ = (yyvsp[0].expr_t);
}
#line 3361 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 87: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' constant_expr_array ')'  */
#line 736 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                      {
    (yyval.insert_stmt) = new InsertStatement();
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
#line 3378 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 88: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 748 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                        {
    (yyval.insert_stmt) = new InsertStatement();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    (yyval.insert_stmt)->columns_ = (yyvsp[-1].identifier_array_t);
    (yyval.insert_stmt)->select_ = (yyvsp[0].select_stmt);
}
#line 3394 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* optional_identifier_array: '(' identifier_array ')'  */
#line 760 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3402 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* optional_identifier_array: %empty  */
#line 763 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3410 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 770 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyval.explain_stmt) = new ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3420 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* explain_type: AST  */
#line 776 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.explain_type_t) = ExplainType::kAst;
}
#line 3428 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* explain_type: UNOPT LOGICAL  */
#line 779 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.explain_type_t) = ExplainType::kUnOpt;
}
#line 3436 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* explain_type: OPT LOGICAL  */
#line 782 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    (yyval.explain_type_t) = ExplainType::kOpt;
}
#line 3444 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* explain_type: PHYSICAL  */
#line 785 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPhysical;
}
#line 3452 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* explain_type: PIPELINE  */
#line 788 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPipeline;
}
#line 3460 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 795 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                       {
    (yyval.update_stmt) = new UpdateStatement();
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
#line 3477 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* update_expr_array: update_expr  */
#line 808 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.update_expr_array_t) = new Vector<UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3486 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* update_expr_array: update_expr_array ',' update_expr  */
#line 812 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3495 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* update_expr: IDENTIFIER '=' expr  */
#line 817 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.update_expr_t) = new UpdateExpr();
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3506 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 829 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.drop_stmt)->drop_info_ = std::move(drop_schema_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3519 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 839 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.drop_stmt) = new DropStatement();
    std::unique_ptr<DropCollectionInfo> drop_collection_info = std::make_unique<DropCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_collection_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = std::move(drop_collection_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 3537 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* drop_statement: DROP TABLE if_exists table_name  */
#line 854 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.drop_stmt) = new DropStatement();
    std::unique_ptr<DropTableInfo> drop_table_info = std::make_unique<DropTableInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_table_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_table_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = std::move(drop_table_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 3555 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 104: /* drop_statement: DROP INDEX if_exists IDENTIFIER  */
#line 869 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.drop_stmt) = new DropStatement();
    std::unique_ptr<DropIndexInfo> drop_index_info = std::make_unique<DropIndexInfo>();
    drop_index_info->index_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.drop_stmt)->drop_info_ = std::move(drop_index_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3568 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 105: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 882 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                           {
    (yyval.copy_stmt) = new CopyStatement();

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
    SizeT option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(SizeT idx = 0; idx < option_count; ++ idx) {
        CopyOption* option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
        switch(option_ptr->option_type_) {
            case CopyOptionType::kFormat: {
                (yyval.copy_stmt)->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case CopyOptionType::kDelimiter: {
                (yyval.copy_stmt)->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case CopyOptionType::kHeader: {
                (yyval.copy_stmt)->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 3614 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 923 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyval.copy_stmt) = new CopyStatement();

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
    SizeT option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(SizeT idx = 0; idx < option_count; ++ idx) {
        CopyOption* option_ptr = (*(yyvsp[-1].copy_option_array))[idx];
        switch(option_ptr->option_type_) {
            case CopyOptionType::kFormat: {
                (yyval.copy_stmt)->copy_file_type_ = option_ptr->file_type_;
                break;
            }
            case CopyOptionType::kDelimiter: {
                (yyval.copy_stmt)->delimiter_ = option_ptr->delimiter_;
                break;
            }
            case CopyOptionType::kHeader: {
                (yyval.copy_stmt)->header_ = option_ptr->header_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 3660 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* select_statement: select_without_paren  */
#line 968 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3668 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* select_statement: select_with_paren  */
#line 971 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3676 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 974 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                     {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3690 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 983 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3704 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* select_with_paren: '(' select_without_paren ')'  */
#line 993 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3712 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* select_with_paren: '(' select_with_paren ')'  */
#line 996 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3720 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1000 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3729 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1005 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 3740 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1012 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3748 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1015 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3756 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1020 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                  {
    (yyval.select_stmt) = new SelectStatement();
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
#line 3775 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1035 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3783 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* order_by_clause: %empty  */
#line 1038 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3791 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* order_by_expr_list: order_by_expr  */
#line 1042 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new Vector<OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 3800 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1046 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 3809 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* order_by_expr: expr order_by_type  */
#line 1051 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 3819 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* order_by_type: ASC  */
#line 1057 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = kAsc;
}
#line 3827 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* order_by_type: DESC  */
#line 1060 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = kDesc;
}
#line 3835 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* order_by_type: %empty  */
#line 1063 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.order_by_type_t) = kAsc;
}
#line 3843 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* limit_expr: LIMIT expr  */
#line 1067 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3851 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* limit_expr: %empty  */
#line 1071 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3857 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* offset_expr: OFFSET expr  */
#line 1073 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3865 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* offset_expr: %empty  */
#line 1077 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3871 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* distinct: DISTINCT  */
#line 1079 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3879 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* distinct: %empty  */
#line 1082 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3887 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* from_clause: FROM table_reference  */
#line 1086 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3895 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 133: /* from_clause: %empty  */
#line 1089 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3903 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 134: /* where_clause: WHERE expr  */
#line 1093 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 135: /* where_clause: %empty  */
#line 1096 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3919 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* having_clause: HAVING expr  */
#line 1100 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3927 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* having_clause: %empty  */
#line 1103 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3935 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* group_by_clause: GROUP BY expr_array  */
#line 1107 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 3943 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 139: /* group_by_clause: %empty  */
#line 1110 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 3951 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* set_operator: UNION  */
#line 1114 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = SetOperatorType::kUnion;
}
#line 3959 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* set_operator: UNION ALL  */
#line 1117 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kUnionAll;
}
#line 3967 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* set_operator: INTERSECT  */
#line 1120 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kIntersect;
}
#line 3975 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* set_operator: EXCEPT  */
#line 1123 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = SetOperatorType::kExcept;
}
#line 3983 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 144: /* table_reference: table_reference_unit  */
#line 1131 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3991 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 145: /* table_reference: table_reference ',' table_reference_unit  */
#line 1134 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CrossProductReference* cross_product_ref = new CrossProductReference();
    cross_product_ref->left_ = (yyvsp[-2].table_reference_t);
    cross_product_ref->right_ = (yyvsp[0].table_reference_t);

    (yyval.table_reference_t) = cross_product_ref;
}
#line 4003 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 148: /* table_reference_name: table_name table_alias  */
#line 1145 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    TableReference* table_ref = new TableReference();
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
#line 4021 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 149: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1159 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    SubqueryReference* subquery_reference = new SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4032 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 150: /* table_name: IDENTIFIER  */
#line 1168 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4043 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1174 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* table_alias: AS IDENTIFIER  */
#line 1183 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4064 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1187 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4074 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* table_alias: %empty  */
#line 1192 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4082 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* with_clause: WITH with_expr_list  */
#line 1199 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4090 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 156: /* with_clause: %empty  */
#line 1202 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4098 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 157: /* with_expr_list: with_expr  */
#line 1206 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new Vector<WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4107 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1209 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4116 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1214 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                             {
    (yyval.with_expr_t) = new WithExpr();
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4127 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1225 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4139 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                   {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4152 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* join_type: INNER  */
#line 1246 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = JoinType::kInner;
}
#line 4160 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* join_type: LEFT  */
#line 1249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kLeft;
}
#line 4168 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* join_type: RIGHT  */
#line 1252 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kRight;
}
#line 4176 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* join_type: OUTER  */
#line 1255 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4184 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* join_type: FULL  */
#line 1258 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4192 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* join_type: CROSS  */
#line 1261 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kCross;
}
#line 4200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* join_type: %empty  */
#line 1264 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 4207 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* show_statement: SHOW TABLES  */
#line 1270 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kTables;
}
#line 4216 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* show_statement: DESCRIBE table_name  */
#line 1274 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kColumns;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4232 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* expr_array: expr_alias  */
#line 1290 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4241 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* expr_array: expr_array ',' expr_alias  */
#line 1294 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4250 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* constant_expr_array: constant_expr  */
#line 1299 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4259 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* constant_expr_array: constant_expr_array ',' constant_expr  */
#line 1303 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4268 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 175: /* expr_alias: expr AS IDENTIFIER  */
#line 1308 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4278 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 176: /* expr_alias: expr  */
#line 1313 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4286 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 182: /* operand: '(' expr ')'  */
#line 1323 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4294 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 183: /* operand: '(' select_without_paren ')'  */
#line 1326 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4305 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 189: /* function_expr: IDENTIFIER '(' ')'  */
#line 1338 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 4317 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 190: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1345 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 4329 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 191: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1352 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 4342 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 192: /* function_expr: operand IS NOT NULLABLE  */
#line 1360 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4354 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 193: /* function_expr: operand IS NULLABLE  */
#line 1367 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4366 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 194: /* function_expr: '-' operand  */
#line 1374 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "-";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4378 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 195: /* function_expr: '+' operand  */
#line 1381 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "+";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4390 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 196: /* function_expr: operand '-' operand  */
#line 1388 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4403 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 197: /* function_expr: operand '+' operand  */
#line 1396 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4416 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 198: /* function_expr: operand '*' operand  */
#line 1404 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4429 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 199: /* function_expr: operand '/' operand  */
#line 1412 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4442 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 200: /* function_expr: operand '%' operand  */
#line 1420 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4455 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 201: /* function_expr: operand '=' operand  */
#line 1428 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4468 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: operand EQUAL operand  */
#line 1436 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4481 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: operand NOT_EQ operand  */
#line 1444 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4494 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 204: /* function_expr: operand '<' operand  */
#line 1452 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4507 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 205: /* function_expr: operand '>' operand  */
#line 1460 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4520 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 206: /* function_expr: operand LESS_EQ operand  */
#line 1468 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4533 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 207: /* function_expr: operand GREATER_EQ operand  */
#line 1476 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4546 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 208: /* function_expr: EXTRACT '(' constant_expr FROM operand ')'  */
#line 1484 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "extract";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4559 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 209: /* conjunction_expr: expr AND expr  */
#line 1493 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4572 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 210: /* conjunction_expr: expr OR expr  */
#line 1501 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4585 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 211: /* between_expr: operand BETWEEN operand AND operand  */
#line 1510 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                  {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "between_and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4599 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 212: /* in_expr: operand IN '(' expr_array ')'  */
#line 1520 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4611 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 213: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1527 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-5].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4623 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 214: /* case_expr: CASE expr case_check_array END  */
#line 1535 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4634 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 215: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1541 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4646 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 216: /* case_expr: CASE case_check_array END  */
#line 1548 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4656 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 217: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1553 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4667 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 218: /* case_check_array: WHEN expr THEN expr  */
#line 1560 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    (yyval.case_check_array_t) = new Vector<CaseCheck*>();
    CaseCheck* case_check_ptr = new CaseCheck();
    case_check_ptr->when_ = (yyvsp[-2].expr_t);
    case_check_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(case_check_ptr);
}
#line 4679 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 219: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1567 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    CaseCheck* case_check_ptr = new CaseCheck();
    case_check_ptr->when_ = (yyvsp[-2].expr_t);
    case_check_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(case_check_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 4691 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 220: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1575 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            {
    SharedPtr<TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case LogicalType::kChar: {
            type_info_ptr = CharInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kVarchar: {
            type_info_ptr = VarcharInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kDecimal64: {
            type_info_ptr = Decimal64Info::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case LogicalType::kBlob: {
            type_info_ptr = BlobInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kBitmap: {
            type_info_ptr = BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
        case LogicalType::kEmbedding: {
            type_info_ptr = EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
    }
    CastExpr* cast_expr = new CastExpr((yyvsp[-1].column_type_t).logical_type_, type_info_ptr);
    cast_expr->expr_ = (yyvsp[-3].expr_t);
    (yyval.expr_t) = cast_expr;
}
#line 4728 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 221: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1608 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                   {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4739 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 222: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1614 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4750 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 223: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4762 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 224: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4774 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 225: /* column_expr: IDENTIFIER  */
#line 1635 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4784 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 226: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4794 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 227: /* column_expr: '*'  */
#line 1645 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4804 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 228: /* column_expr: column_expr '.' '*'  */
#line 1650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4818 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 229: /* constant_expr: STRING  */
#line 1660 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4828 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 230: /* constant_expr: TRUE  */
#line 1665 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
#line 4838 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 231: /* constant_expr: FALSE  */
#line 1670 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
#line 4848 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 232: /* constant_expr: DOUBLE_VALUE  */
#line 1675 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kFloat);
    const_expr->float_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
#line 4858 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 233: /* constant_expr: LONG_VALUE  */
#line 1680 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4868 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 234: /* constant_expr: DATE STRING  */
#line 1685 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4878 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 235: /* constant_expr: INTERVAL interval_expr  */
#line 1690 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4886 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 236: /* constant_expr: interval_expr  */
#line 1693 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4894 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 237: /* interval_expr: LONG_VALUE SECONDS  */
#line 1697 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4905 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 238: /* interval_expr: LONG_VALUE SECOND  */
#line 1703 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4916 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 239: /* interval_expr: LONG_VALUE MINUTES  */
#line 1709 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4927 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 240: /* interval_expr: LONG_VALUE MINUTE  */
#line 1715 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4938 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 241: /* interval_expr: LONG_VALUE HOURS  */
#line 1721 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4949 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 242: /* interval_expr: LONG_VALUE HOUR  */
#line 1727 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4960 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 243: /* interval_expr: LONG_VALUE DAYS  */
#line 1733 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4971 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 244: /* interval_expr: LONG_VALUE DAY  */
#line 1739 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4982 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 245: /* interval_expr: LONG_VALUE MONTHS  */
#line 1745 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4993 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 246: /* interval_expr: LONG_VALUE MONTH  */
#line 1751 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5004 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 247: /* interval_expr: LONG_VALUE YEARS  */
#line 1757 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5015 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 248: /* interval_expr: LONG_VALUE YEAR  */
#line 1763 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5026 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 249: /* copy_option_list: copy_option  */
#line 1774 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.copy_option_array) = new Vector<CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 5035 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 250: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1778 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 5044 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 251: /* copy_option: FORMAT IDENTIFIER  */
#line 1783 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kFormat;
    if (strcasecmp((yyvsp[0].str_value), "csv") == 0) {
        (yyval.copy_option_t)->file_type_ = CopyFileType::kCSV;
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file type");
        YYERROR;
    }
}
#line 5062 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 252: /* copy_option: DELIMITER STRING  */
#line 1796 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 5073 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 253: /* copy_option: HEADER  */
#line 1802 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 5083 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 254: /* file_path: STRING  */
#line 1808 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5091 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 255: /* if_exists: IF EXISTS  */
#line 1812 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5097 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 256: /* if_exists: %empty  */
#line 1813 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5103 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 257: /* if_not_exists: IF NOT EXISTS  */
#line 1815 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5109 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 258: /* if_not_exists: %empty  */
#line 1816 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5115 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 5119 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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

#line 1822 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d\n", msg, llocp->first_column);
}

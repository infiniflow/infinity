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
  YYSYMBOL_LIKE = 132,                     /* LIKE  */
  YYSYMBOL_NUMBER = 133,                   /* NUMBER  */
  YYSYMBOL_134_ = 134,                     /* '='  */
  YYSYMBOL_135_ = 135,                     /* '<'  */
  YYSYMBOL_136_ = 136,                     /* '>'  */
  YYSYMBOL_137_ = 137,                     /* '+'  */
  YYSYMBOL_138_ = 138,                     /* '-'  */
  YYSYMBOL_139_ = 139,                     /* '*'  */
  YYSYMBOL_140_ = 140,                     /* '/'  */
  YYSYMBOL_141_ = 141,                     /* '%'  */
  YYSYMBOL_142_ = 142,                     /* '['  */
  YYSYMBOL_143_ = 143,                     /* ']'  */
  YYSYMBOL_144_ = 144,                     /* '('  */
  YYSYMBOL_145_ = 145,                     /* ')'  */
  YYSYMBOL_146_ = 146,                     /* '.'  */
  YYSYMBOL_147_ = 147,                     /* ';'  */
  YYSYMBOL_148_ = 148,                     /* ','  */
  YYSYMBOL_YYACCEPT = 149,                 /* $accept  */
  YYSYMBOL_input_pattern = 150,            /* input_pattern  */
  YYSYMBOL_statement_list = 151,           /* statement_list  */
  YYSYMBOL_statement = 152,                /* statement  */
  YYSYMBOL_explainable_statement = 153,    /* explainable_statement  */
  YYSYMBOL_create_statement = 154,         /* create_statement  */
  YYSYMBOL_table_element_array = 155,      /* table_element_array  */
  YYSYMBOL_table_element = 156,            /* table_element  */
  YYSYMBOL_table_column = 157,             /* table_column  */
  YYSYMBOL_column_type = 158,              /* column_type  */
  YYSYMBOL_column_constraints = 159,       /* column_constraints  */
  YYSYMBOL_column_constraint = 160,        /* column_constraint  */
  YYSYMBOL_table_constraint = 161,         /* table_constraint  */
  YYSYMBOL_identifier_array = 162,         /* identifier_array  */
  YYSYMBOL_delete_statement = 163,         /* delete_statement  */
  YYSYMBOL_insert_statement = 164,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 165, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 166,        /* explain_statement  */
  YYSYMBOL_explain_type = 167,             /* explain_type  */
  YYSYMBOL_update_statement = 168,         /* update_statement  */
  YYSYMBOL_update_expr_array = 169,        /* update_expr_array  */
  YYSYMBOL_update_expr = 170,              /* update_expr  */
  YYSYMBOL_drop_statement = 171,           /* drop_statement  */
  YYSYMBOL_copy_statement = 172,           /* copy_statement  */
  YYSYMBOL_select_statement = 173,         /* select_statement  */
  YYSYMBOL_select_with_paren = 174,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 175,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 176, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 177, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 178, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 179,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 180,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 181,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 182,            /* order_by_type  */
  YYSYMBOL_limit_expr = 183,               /* limit_expr  */
  YYSYMBOL_offset_expr = 184,              /* offset_expr  */
  YYSYMBOL_distinct = 185,                 /* distinct  */
  YYSYMBOL_from_clause = 186,              /* from_clause  */
  YYSYMBOL_where_clause = 187,             /* where_clause  */
  YYSYMBOL_having_clause = 188,            /* having_clause  */
  YYSYMBOL_group_by_clause = 189,          /* group_by_clause  */
  YYSYMBOL_set_operator = 190,             /* set_operator  */
  YYSYMBOL_table_reference = 191,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 192,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 193,     /* table_reference_name  */
  YYSYMBOL_table_name = 194,               /* table_name  */
  YYSYMBOL_table_alias = 195,              /* table_alias  */
  YYSYMBOL_with_clause = 196,              /* with_clause  */
  YYSYMBOL_with_expr_list = 197,           /* with_expr_list  */
  YYSYMBOL_with_expr = 198,                /* with_expr  */
  YYSYMBOL_join_clause = 199,              /* join_clause  */
  YYSYMBOL_join_type = 200,                /* join_type  */
  YYSYMBOL_show_statement = 201,           /* show_statement  */
  YYSYMBOL_expr_array = 202,               /* expr_array  */
  YYSYMBOL_constant_expr_array = 203,      /* constant_expr_array  */
  YYSYMBOL_expr_alias = 204,               /* expr_alias  */
  YYSYMBOL_expr = 205,                     /* expr  */
  YYSYMBOL_operand = 206,                  /* operand  */
  YYSYMBOL_function_expr = 207,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 208,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 209,             /* between_expr  */
  YYSYMBOL_in_expr = 210,                  /* in_expr  */
  YYSYMBOL_case_expr = 211,                /* case_expr  */
  YYSYMBOL_case_check_array = 212,         /* case_check_array  */
  YYSYMBOL_cast_expr = 213,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 214,            /* subquery_expr  */
  YYSYMBOL_column_expr = 215,              /* column_expr  */
  YYSYMBOL_constant_expr = 216,            /* constant_expr  */
  YYSYMBOL_interval_expr = 217,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 218,         /* copy_option_list  */
  YYSYMBOL_copy_option = 219,              /* copy_option  */
  YYSYMBOL_file_path = 220,                /* file_path  */
  YYSYMBOL_if_exists = 221,                /* if_exists  */
  YYSYMBOL_if_not_exists = 222,            /* if_not_exists  */
  YYSYMBOL_semicolon = 223                 /* semicolon  */
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
#define YYFINAL  55
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   687

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  149
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  75
/* YYNRULES -- Number of rules.  */
#define YYNRULES  265
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  531

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   388


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
       2,     2,     2,     2,     2,     2,     2,   141,     2,     2,
     144,   145,   139,   137,   148,   138,   146,   140,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   147,
     135,   134,   136,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   142,     2,   143,     2,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132,   133
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   369,   369,   373,   379,   386,   387,   388,   389,   390,
     391,   392,   393,   394,   396,   397,   398,   399,   400,   401,
     402,   403,   410,   423,   438,   462,   478,   495,   515,   519,
     525,   528,   534,   573,   616,   617,   618,   619,   620,   621,
     622,   623,   624,   625,   626,   627,   628,   629,   630,   631,
     632,   633,   634,   635,   636,   638,   639,   640,   641,   642,
     643,   644,   645,   646,   647,   648,   649,   650,   651,   652,
     653,   654,   655,   656,   657,   658,   677,   681,   691,   694,
     697,   700,   704,   709,   716,   722,   732,   748,   760,   773,
     776,   783,   789,   792,   795,   798,   801,   808,   821,   825,
     830,   843,   856,   871,   886,   901,   920,   961,  1006,  1009,
    1012,  1021,  1031,  1034,  1038,  1043,  1050,  1053,  1058,  1073,
    1076,  1080,  1084,  1089,  1095,  1098,  1101,  1105,  1109,  1111,
    1115,  1117,  1120,  1124,  1127,  1131,  1134,  1138,  1141,  1145,
    1148,  1152,  1155,  1158,  1161,  1169,  1172,  1187,  1187,  1189,
    1203,  1212,  1219,  1230,  1235,  1240,  1246,  1253,  1256,  1260,
    1263,  1268,  1280,  1287,  1301,  1304,  1307,  1310,  1313,  1316,
    1319,  1325,  1329,  1345,  1349,  1354,  1358,  1363,  1369,  1373,
    1374,  1375,  1376,  1377,  1379,  1382,  1388,  1389,  1390,  1391,
    1392,  1394,  1402,  1410,  1419,  1426,  1433,  1440,  1447,  1454,
    1462,  1470,  1478,  1486,  1494,  1502,  1510,  1518,  1526,  1534,
    1542,  1550,  1558,  1566,  1575,  1583,  1592,  1600,  1607,  1615,
    1621,  1628,  1633,  1640,  1647,  1655,  1688,  1694,  1700,  1707,
    1715,  1722,  1729,  1734,  1744,  1749,  1754,  1759,  1764,  1769,
    1774,  1777,  1781,  1787,  1793,  1799,  1805,  1811,  1817,  1823,
    1829,  1835,  1841,  1847,  1858,  1862,  1867,  1880,  1886,  1892,
    1896,  1897,  1899,  1900,  1902,  1903
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
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "$accept", "input_pattern",
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

#define YYPACT_NINF (-313)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-265)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   237,   -19,   278,    31,   -23,    31,   173,    10,    31,
      97,   -45,    69,   -29,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,   195,  -313,  -313,   150,  -313,   115,   115,   115,
     115,   115,    31,   142,   142,   142,   142,   142,    46,   197,
      31,   119,  -313,  -313,   198,   203,  -313,    95,  -313,  -313,
     217,   118,  -313,   141,   156,  -313,    12,  -313,   273,  -313,
    -313,     7,   247,  -313,   253,   254,   340,    31,    31,    31,
     342,   216,   300,   360,    31,    31,    31,    31,   361,   362,
     313,   363,   363,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,   195,  -313,   227,    97,  -313,  -313,  -313,  -313,
       7,  -313,  -313,  -313,   248,   332,   321,   316,  -313,   -24,
    -313,   216,   329,   380,   133,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,   260,   -47,  -313,   248,  -313,  -313,   317,   323,
     150,  -313,   257,   261,   266,  -313,  -313,   488,   320,   268,
     269,   139,   410,  -313,  -313,   409,   272,   324,   324,  -313,
     231,   -54,  -313,   -13,   379,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,   279,  -313,  -313,   248,   248,   368,  -313,   -45,
       8,   384,    31,  -313,  -101,   283,  -313,   248,   362,  -313,
     192,   284,   292,   287,  -313,  -313,    86,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   324,
     293,   212,   373,   248,   248,    39,   199,  -313,   488,  -313,
      66,   -77,   -77,   295,    49,     4,   248,   313,   439,   248,
     248,   -49,   299,   -25,   324,   324,   324,   324,   324,   324,
     324,   324,   324,   324,   324,   324,   324,   324,    13,   296,
    -313,   127,   192,   248,  -313,   195,   487,   341,   301,   -60,
    -313,  -313,  -313,   -45,   304,  -313,   444,    66,   192,  -313,
     288,   288,  -313,   248,  -313,    16,   373,   318,   307,    36,
     -43,   215,  -313,   248,   248,   386,  -313,  -313,   -45,   306,
     353,  -313,    56,  -313,  -313,   417,  -313,  -313,   336,   322,
     324,   231,   366,  -313,   420,   420,   282,   282,   400,   420,
     420,   282,   282,   -77,   -77,  -313,  -313,  -313,  -313,  -313,
     248,  -313,  -313,  -313,   192,  -313,  -313,  -313,  -313,  -313,
    -313,   330,   331,  -313,  -313,  -313,   334,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   344,
     347,   348,   349,   154,   354,   380,  -313,     8,   195,   380,
    -313,    22,  -313,   463,   473,  -313,    42,  -313,    67,    84,
    -313,   355,  -313,   487,   248,  -313,   248,    51,    11,   324,
      27,     4,   445,  -313,  -313,  -313,  -313,  -313,  -313,   455,
    -313,   506,  -313,   470,   471,   231,   420,   377,    96,  -313,
     324,  -313,   517,   524,   525,   527,   536,   418,   514,   436,
     443,  -313,  -313,   154,  -313,   380,    98,  -313,   120,  -313,
      66,  -313,  -313,  -313,   288,  -313,  -313,  -313,   404,   192,
      92,  -313,   248,   345,    56,   353,     4,     4,   406,   248,
     248,  -313,   408,   134,  -313,  -313,   212,   449,   450,   136,
     451,   454,   403,   464,   465,   466,   467,   468,   472,   474,
     475,   476,   477,   478,   479,   480,  -313,  -313,  -313,   145,
    -313,  -313,  -313,  -313,  -313,  -313,   192,  -313,  -313,  -313,
     569,   380,   481,   192,  -313,  -313,  -313,  -313,  -313,   613,
    -313,  -313,   615,   624,   625,   626,   627,   628,   629,   630,
     631,   632,   633,   634,   635,   636,  -313,   248,   149,   498,
     499,   500,   501,   502,   503,   504,   505,   507,   508,   509,
     510,   511,   512,   513,   192,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   158,     0,   265,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   109,   108,     0,     8,   263,   263,   263,
     263,   263,     0,   261,   261,   261,   261,   261,   151,     0,
       0,     0,    92,    96,     0,     0,    95,   158,   171,   172,
       0,   157,   159,     0,     0,     1,   158,     2,   141,   143,
     144,     0,   132,   114,   120,     0,     0,     0,     0,     0,
       0,    90,     0,     0,     0,     0,     0,     0,     0,     0,
     136,     0,     0,    93,    94,    91,    14,    19,    21,    20,
      15,    16,    18,    17,     0,     0,   113,   112,     4,   142,
       0,   110,   111,   131,     0,     0,   128,     0,    22,     0,
      23,    90,     0,     0,   158,   260,   101,   103,   102,   104,
     105,   152,     0,   136,    98,     0,    86,   259,     0,     0,
       0,   160,     0,     0,   230,   234,   237,   238,     0,     0,
       0,     0,     0,   235,   236,     0,     0,     0,     0,   232,
     158,   134,   173,   178,   179,   188,   183,   182,   181,   189,
     190,   180,   187,   186,   241,     0,     0,   130,   262,   158,
       0,     0,     0,    84,     0,     0,    88,     0,     0,    97,
     135,     0,     0,     0,   117,   116,     0,   243,   242,   245,
     244,   247,   246,   249,   248,   251,   250,   253,   252,     0,
       0,   196,   158,     0,     0,     0,     0,   239,     0,   240,
       0,   198,   197,     0,     0,     0,     0,   136,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     121,   126,   127,     0,   115,    25,     0,     0,     0,     0,
      28,    30,    31,   158,     0,    89,     0,     0,   100,    99,
       0,     0,   161,     0,   191,     0,   158,     0,     0,     0,
       0,     0,   221,     0,     0,     0,   185,   184,   158,   133,
     145,   147,   156,   148,   174,   140,   177,   214,   215,     0,
       0,   158,     0,   195,   205,   206,   209,   210,     0,   212,
     204,   207,   208,   200,   199,   201,   202,   203,   231,   233,
       0,   124,   125,   123,   129,    34,    37,    35,    36,    38,
      39,     0,     0,    40,    42,    41,    59,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,     0,
       0,     0,     0,    32,     0,     0,    24,     0,    26,     0,
      85,     0,   175,     0,     0,   258,     0,   254,     0,     0,
     192,     0,   226,     0,     0,   219,     0,     0,     0,     0,
       0,     0,     0,   165,   166,   167,   168,   164,   169,     0,
     154,     0,   149,     0,   138,   158,   213,     0,     0,   194,
       0,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    79,    80,    33,    76,     0,     0,    29,     0,    87,
       0,   257,   256,   107,     0,   106,   193,   227,     0,   223,
       0,   222,     0,     0,   156,   146,     0,     0,   153,     0,
       0,   118,     0,     0,   228,   217,   216,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    81,    78,    77,     0,
      83,    27,   176,   255,   225,   220,   224,   211,   150,   162,
       0,     0,   139,   137,   229,   218,    55,    56,    58,     0,
      60,    61,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    82,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   163,   155,    57,    65,    63,    64,
      66,    67,    68,    62,    72,    70,    71,    73,    74,    75,
      69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -313,  -313,  -313,   595,  -313,   612,  -313,   314,  -313,   297,
    -313,   259,  -313,  -312,   616,   617,   554,  -313,  -313,   619,
    -313,   489,   621,   622,   -33,   659,   -11,   541,   572,    93,
    -313,  -313,   364,  -313,  -313,  -313,  -313,  -313,   -80,  -313,
    -313,  -313,  -313,   302,  -199,    -1,   251,  -313,  -313,   581,
    -313,  -313,   637,  -182,  -313,   461,  -124,  -102,  -313,  -313,
    -313,  -313,  -313,   482,  -313,  -313,  -313,  -208,   533,   419,
     265,   599,   276,   303,  -313
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    12,    13,    14,    85,    15,   249,   250,   251,   343,
     403,   404,   252,   174,    16,    17,   114,    18,    47,    19,
     123,   124,    20,    21,    22,    23,    24,    63,   101,    64,
     106,   239,   240,   313,   167,   244,   104,   217,   126,   431,
     384,    61,   279,   280,   281,   282,   382,    25,    51,    52,
     283,   379,    26,   151,   351,   152,   153,   154,   155,   156,
     157,   158,   159,   206,   160,   161,   162,   163,   164,   356,
     357,   128,    73,    66,    57
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      54,   180,   275,    39,   265,    41,   125,    38,    49,   215,
      32,   246,  -264,   289,    92,    62,   308,   205,   169,     1,
      10,     2,     3,     4,     5,     6,   214,     7,     8,   218,
     364,    71,     9,   406,    38,   292,   201,   408,    48,    80,
      40,   241,   242,   179,   255,   211,   212,   256,    58,   352,
      59,    60,     1,   258,     2,     3,     4,     5,     6,   380,
       7,     8,   235,   236,   237,     9,   109,   110,   111,    55,
     135,   136,   137,   117,   118,   119,   120,    10,   363,   269,
     270,   359,   293,   290,   422,   346,   219,   220,   347,   134,
     135,   136,   137,   459,   216,   287,   288,   201,   381,    11,
      50,   178,     1,   176,     2,     3,     4,     5,     6,   388,
      10,     8,   247,   204,   248,     9,   219,   220,    56,   314,
     170,   421,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   245,   285,   263,   213,
     219,   220,   134,   135,   136,   137,   138,   139,   278,   367,
     368,   100,   309,   142,   102,   140,    11,   141,    62,   498,
      10,   360,   465,   175,   216,   219,   220,   409,   219,   220,
     410,   254,   424,   142,    65,   143,   144,   145,   219,   220,
     219,   220,    81,    82,   311,   312,   241,   413,   386,    11,
     414,   268,    78,   133,   277,   143,   144,   145,    10,   138,
     139,    72,   462,   433,    42,    43,    44,    45,   140,    46,
     141,    79,   415,   204,   399,   414,    58,   146,    59,    60,
     348,   219,   220,   147,   148,   149,   142,   469,   470,   416,
     150,   264,   216,    83,   134,   135,   136,   137,    84,    11,
     419,   435,   420,   460,   216,   370,   256,   472,   143,   144,
     145,   134,   135,   136,   137,   361,   219,   220,   400,    94,
     401,   402,    27,    28,    29,   461,    95,   423,   256,   272,
     146,   273,   267,   274,    30,    31,   147,   148,   149,   475,
     387,   478,   216,   150,   479,   365,    96,   366,   436,   274,
     496,   138,   139,   256,   515,    99,    10,   256,   466,   103,
     140,    97,   141,    33,    34,    35,   473,   105,   138,   139,
      74,    75,    76,    77,   107,    36,    37,   140,   142,   141,
     223,   219,   220,   134,   135,   136,   137,   134,   135,   136,
     137,    67,    68,    69,    70,   142,   224,   225,   226,   227,
     143,   144,   145,   108,   229,   112,   230,   231,   232,   233,
     234,   235,   236,   237,   353,   354,   355,   143,   144,   145,
     113,   115,   146,   116,   121,   122,   125,   127,   147,   148,
     149,   130,   165,   514,   432,   150,   166,   168,   172,   146,
     199,   200,   181,   173,   199,   147,   148,   149,   182,   140,
     223,   141,   150,   140,   177,   141,   372,  -170,   373,   374,
     375,   376,   184,   377,   378,   267,   185,   142,  -265,  -265,
     186,   142,   202,   203,   207,   208,   210,  -265,  -265,   233,
     234,   235,   236,   237,   243,   238,   253,   257,   260,   143,
     144,   145,   262,   143,   144,   145,   261,   266,    10,   221,
     276,   222,   286,   291,   310,   345,   344,   350,   349,   369,
     290,   146,   362,   223,   371,   146,   383,   147,   148,   149,
     267,   147,   148,   149,   150,   219,   385,   411,   150,   224,
     225,   226,   227,   389,   392,   393,   412,   229,   394,   230,
     231,   232,   233,   234,   235,   236,   237,   223,   395,   426,
     467,   396,   397,   398,   442,   443,   444,   445,   405,   427,
     417,   446,   447,   224,   225,   226,   227,   228,   223,   428,
     429,   229,   430,   230,   231,   232,   233,   234,   235,   236,
     237,   448,   434,   437,   224,   225,   226,   227,   223,   390,
     438,   439,   229,   440,   230,   231,   232,   233,   234,   235,
     236,   237,   441,   456,  -265,  -265,   226,   227,   457,   464,
     471,   482,  -265,   474,  -265,   231,   232,   233,   234,   235,
     236,   237,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   340,   341,   342,
     449,   450,   451,   452,   476,   477,   480,   453,   454,   481,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   483,   484,   485,   486,   487,   455,   497,   499,
     488,   500,   489,   490,   491,   492,   493,   494,   495,   216,
     501,   502,   503,   504,   505,   506,   507,   508,   509,   510,
     511,   512,   513,   516,   517,   518,   519,   520,   521,   522,
     523,    98,   524,   525,   526,   527,   528,   529,   530,    86,
     418,   407,   458,    87,    88,   171,    89,   259,    90,    91,
      53,   183,   132,   425,   391,   468,   131,   284,   209,   463,
     358,   129,     0,     0,    93,     0,     0,   271
};

static const yytype_int16 yycheck[] =
{
      11,   125,   210,     4,   186,     6,    53,     3,     9,    63,
      29,     3,     0,    62,    47,     8,     3,   141,    42,     7,
      65,     9,    10,    11,    12,    13,   150,    15,    16,    42,
      73,    32,    20,   345,     3,    60,   138,   349,    28,    40,
      63,   165,   166,   123,   145,   147,   148,   148,    21,   257,
      23,    24,     7,   177,     9,    10,    11,    12,    13,     3,
      15,    16,   139,   140,   141,    20,    67,    68,    69,     0,
       4,     5,     6,    74,    75,    76,    77,    65,    42,   203,
     204,   263,   107,   132,    73,   145,   129,   130,   148,     3,
       4,     5,     6,   405,   148,   219,   220,   199,    42,   144,
       3,   148,     7,   114,     9,    10,    11,    12,    13,   291,
      65,    16,   104,    74,   106,    20,   129,   130,   147,   243,
     144,    70,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   169,   217,    52,   150,
     129,   130,     3,     4,     5,     6,    60,    61,   144,   273,
     274,   144,   139,    87,    61,    69,   144,    71,     8,   471,
      65,   145,    70,    30,   148,   129,   130,   145,   129,   130,
     148,   172,   145,    87,    59,   109,   110,   111,   129,   130,
     129,   130,    63,    64,    57,    58,   310,   145,   290,   144,
     148,   202,   146,   100,   145,   109,   110,   111,    65,    60,
      61,    59,   410,   385,    31,    32,    33,    34,    69,    36,
      71,    14,   145,    74,    60,   148,    21,   131,    23,    24,
     253,   129,   130,   137,   138,   139,    87,   426,   427,   145,
     144,   145,   148,    35,     3,     4,     5,     6,    35,   144,
     364,   145,   366,   145,   148,   278,   148,   429,   109,   110,
     111,     3,     4,     5,     6,   266,   129,   130,   104,    42,
     106,   107,    25,    26,    27,   145,   148,   369,   148,    70,
     131,    72,    60,    74,    37,    38,   137,   138,   139,   145,
     291,   145,   148,   144,   148,    70,   145,    72,   390,    74,
     145,    60,    61,   148,   145,    22,    65,   148,   422,    52,
      69,   145,    71,    25,    26,    27,   430,    54,    60,    61,
      34,    35,    36,    37,    60,    37,    38,    69,    87,    71,
     108,   129,   130,     3,     4,     5,     6,     3,     4,     5,
       6,    28,    29,    30,    31,    87,   124,   125,   126,   127,
     109,   110,   111,     3,   132,     3,   134,   135,   136,   137,
     138,   139,   140,   141,    66,    67,    68,   109,   110,   111,
     144,    61,   131,     3,     3,     3,    53,     4,   137,   138,
     139,   144,    40,   497,   385,   144,    55,    61,    49,   131,
      60,    61,    65,     3,    60,   137,   138,   139,    65,    69,
     108,    71,   144,    69,   134,    71,    43,    44,    45,    46,
      47,    48,   145,    50,    51,    60,   145,    87,   126,   127,
     144,    87,   144,   144,     4,     6,   144,   135,   136,   137,
     138,   139,   140,   141,    56,   146,    42,   144,   144,   109,
     110,   111,   145,   109,   110,   111,   144,   144,    65,    60,
     145,    62,     3,   144,   148,   144,   105,     3,   144,    63,
     132,   131,   145,   108,   148,   131,    39,   137,   138,   139,
      60,   137,   138,   139,   144,   129,   144,     4,   144,   124,
     125,   126,   127,   107,   144,   144,     3,   132,   144,   134,
     135,   136,   137,   138,   139,   140,   141,   108,   144,    44,
     145,   144,   144,   144,    76,    77,    78,    79,   144,    44,
     145,    83,    84,   124,   125,   126,   127,   128,   108,     3,
      40,   132,    41,   134,   135,   136,   137,   138,   139,   140,
     141,   103,   145,     6,   124,   125,   126,   127,   108,   129,
       6,     6,   132,     6,   134,   135,   136,   137,   138,   139,
     140,   141,     6,   107,   124,   125,   126,   127,   105,   145,
     144,   148,   132,   145,   134,   135,   136,   137,   138,   139,
     140,   141,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      76,    77,    78,    79,   145,   145,   145,    83,    84,   145,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   148,   148,   148,   148,   148,   103,    49,     6,
     148,     6,   148,   148,   148,   148,   148,   148,   148,   148,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   145,   145,   145,   145,   145,   145,   145,
     145,    56,   145,   145,   145,   145,   145,   145,   145,    47,
     363,   347,   403,    47,    47,   111,    47,   178,    47,    47,
      11,   130,   100,   371,   310,   424,    95,   216,   145,   414,
     261,    82,    -1,    -1,    47,    -1,    -1,   205
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      65,   144,   150,   151,   152,   154,   163,   164,   166,   168,
     171,   172,   173,   174,   175,   196,   201,    25,    26,    27,
      37,    38,    29,    25,    26,    27,    37,    38,     3,   194,
      63,   194,    31,    32,    33,    34,    36,   167,    28,   194,
       3,   197,   198,   174,   175,     0,   147,   223,    21,    23,
      24,   190,     8,   176,   178,    59,   222,   222,   222,   222,
     222,   194,    59,   221,   221,   221,   221,   221,   146,    14,
     194,    63,    64,    35,    35,   153,   154,   163,   164,   168,
     171,   172,   173,   201,    42,   148,   145,   145,   152,    22,
     144,   177,   178,    52,   185,    54,   179,    60,     3,   194,
     194,   194,     3,   144,   165,    61,     3,   194,   194,   194,
     194,     3,     3,   169,   170,    53,   187,     4,   220,   220,
     144,   198,   177,   178,     3,     4,     5,     6,    60,    61,
      69,    71,    87,   109,   110,   111,   131,   137,   138,   139,
     144,   202,   204,   205,   206,   207,   208,   209,   210,   211,
     213,   214,   215,   216,   217,    40,    55,   183,    61,    42,
     144,   165,    49,     3,   162,    30,   175,   134,   148,   187,
     205,    65,    65,   176,   145,   145,   144,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    60,
      61,   206,   144,   144,    74,   205,   212,     4,     6,   217,
     144,   206,   206,   175,   205,    63,   148,   186,    42,   129,
     130,    60,    62,   108,   124,   125,   126,   127,   128,   132,
     134,   135,   136,   137,   138,   139,   140,   141,   146,   180,
     181,   205,   205,    56,   184,   173,     3,   104,   106,   155,
     156,   157,   161,    42,   194,   145,   148,   144,   205,   170,
     144,   144,   145,    52,   145,   202,   144,    60,   175,   205,
     205,   212,    70,    72,    74,   216,   145,   145,   144,   191,
     192,   193,   194,   199,   204,   187,     3,   205,   205,    62,
     132,   144,    60,   107,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,     3,   139,
     148,    57,    58,   182,   205,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   158,   105,   144,   145,   148,   173,   144,
       3,   203,   216,    66,    67,    68,   218,   219,   218,   202,
     145,   175,   145,    42,    73,    70,    72,   205,   205,    63,
     173,   148,    43,    45,    46,    47,    48,    50,    51,   200,
       3,    42,   195,    39,   189,   144,   206,   175,   202,   107,
     129,   181,   144,   144,   144,   144,   144,   144,   144,    60,
     104,   106,   107,   159,   160,   144,   162,   156,   162,   145,
     148,     4,     3,   145,   148,   145,   145,   145,   158,   205,
     205,    70,    73,   206,   145,   192,    44,    44,     3,    40,
      41,   188,   175,   202,   145,   145,   206,     6,     6,     6,
       6,     6,    76,    77,    78,    79,    83,    84,   103,    76,
      77,    78,    79,    83,    84,   103,   107,   105,   160,   162,
     145,   145,   216,   219,   145,    70,   205,   145,   195,   193,
     193,   144,   202,   205,   145,   145,   145,   145,   145,   148,
     145,   145,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   145,    49,   162,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   205,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   149,   150,   151,   151,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   153,   153,   153,   153,   153,   153,
     153,   153,   154,   154,   154,   154,   154,   154,   155,   155,
     156,   156,   157,   157,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   159,   159,   160,   160,
     160,   160,   161,   161,   162,   162,   163,   164,   164,   165,
     165,   166,   167,   167,   167,   167,   167,   168,   169,   169,
     170,   171,   171,   171,   171,   171,   172,   172,   173,   173,
     173,   173,   174,   174,   175,   176,   177,   177,   178,   179,
     179,   180,   180,   181,   182,   182,   182,   183,   183,   184,
     184,   185,   185,   186,   186,   187,   187,   188,   188,   189,
     189,   190,   190,   190,   190,   191,   191,   192,   192,   193,
     193,   194,   194,   195,   195,   195,   195,   196,   196,   197,
     197,   198,   199,   199,   200,   200,   200,   200,   200,   200,
     200,   201,   201,   202,   202,   203,   203,   204,   204,   205,
     205,   205,   205,   205,   206,   206,   206,   206,   206,   206,
     206,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   208,   208,   209,   210,   210,   211,
     211,   211,   211,   212,   212,   213,   214,   214,   214,   214,
     215,   215,   215,   215,   216,   216,   216,   216,   216,   216,
     216,   216,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   218,   218,   219,   219,   219,   220,
     221,   221,   222,   222,   223,   223
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
       3,     4,     4,     4,     4,     4,     8,     8,     1,     1,
       3,     3,     3,     3,     2,     4,     3,     3,     7,     3,
       0,     1,     3,     2,     1,     1,     0,     2,     0,     2,
       0,     1,     0,     2,     0,     2,     0,     2,     0,     3,
       0,     1,     2,     1,     1,     1,     3,     1,     1,     2,
       4,     1,     3,     2,     1,     5,     0,     2,     0,     1,
       3,     5,     4,     6,     1,     1,     1,     1,     1,     1,
       0,     2,     2,     1,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     3,     4,     5,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     1,
       2,     0,     3,     0,     1,     0
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
#line 224 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1827 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 224 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1835 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 151 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1849 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 141 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 1863 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 217 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1874 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 228 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1883 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 228 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1892 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 188 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 1906 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 181 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 1917 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1927 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1937 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1947 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1957 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1967 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1977 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 171 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1991 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 171 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2005 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 247 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2015 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2023 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2031 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2040 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2048 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2056 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2070 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2079 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2088 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2097 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 208 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2110 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 237 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2119 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 198 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2133 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 198 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2147 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 253 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2157 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2166 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2180 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr_array: /* constant_expr_array  */
#line 161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2194 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2202 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2210 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2218 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2226 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2234 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2242 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2250 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2258 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 265 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2271 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2279 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2287 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2295 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2303 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2311 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 224 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2319 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 72 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2427 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
#line 369 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2642 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 373 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new Vector<BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2653 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 379 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2664 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 386 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2670 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 387 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2676 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 388 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2682 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 389 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2688 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 390 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2694 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 391 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2700 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 392 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2706 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 393 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2712 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 394 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2718 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 14: /* explainable_statement: create_statement  */
#line 396 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2724 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 15: /* explainable_statement: drop_statement  */
#line 397 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2730 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 16: /* explainable_statement: copy_statement  */
#line 398 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2736 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 17: /* explainable_statement: show_statement  */
#line 399 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2742 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 18: /* explainable_statement: select_statement  */
#line 400 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2748 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 19: /* explainable_statement: delete_statement  */
#line 401 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2754 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 20: /* explainable_statement: update_statement  */
#line 402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2760 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 21: /* explainable_statement: insert_statement  */
#line 403 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2766 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 22: /* create_statement: CREATE SCHEMA if_not_exists IDENTIFIER  */
#line 410 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                          {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = std::move(create_schema_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2782 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 23: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 423 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                             {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateCollectionInfo> create_collection_info = std::make_unique<CreateCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        create_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    create_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.create_stmt)->create_info_ = std::move(create_collection_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 2800 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 24: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 438 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2828 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 25: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 462 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2848 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 26: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 478 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2869 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 27: /* create_statement: CREATE INDEX if_not_exists IDENTIFIER ON table_name '(' identifier_array ')'  */
#line 495 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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

    ParserHelper::ToLower((yyvsp[-5].str_value));
    create_index_info->index_name_ = (yyvsp[-5].str_value);
    free((yyvsp[-5].str_value));

    create_index_info->column_names_ = (yyvsp[-1].identifier_array_t);
    create_index_info->conflict_type_ = (yyvsp[-6].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    (yyval.create_stmt)->create_info_ = std::move(create_index_info);
}
#line 2893 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 28: /* table_element_array: table_element  */
#line 515 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new Vector<TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 2902 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 29: /* table_element_array: table_element_array ',' table_element  */
#line 519 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 2911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 30: /* table_element: table_column  */
#line 525 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 2919 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 31: /* table_element: table_constraint  */
#line 528 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 2927 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 32: /* table_column: IDENTIFIER column_type  */
#line 534 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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

    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 2971 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 33: /* table_column: IDENTIFIER column_type column_constraints  */
#line 573 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3017 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 34: /* column_type: BOOLEAN  */
#line 616 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kBoolean}; }
#line 3023 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* column_type: TINYINT  */
#line 617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kTinyInt}; }
#line 3029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: SMALLINT  */
#line 618 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kSmallInt}; }
#line 3035 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: INTEGER  */
#line 619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kInteger}; }
#line 3041 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: BIGINT  */
#line 620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kBigInt}; }
#line 3047 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: HUGEINT  */
#line 621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kHugeInt}; }
#line 3053 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: FLOAT  */
#line 622 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3059 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: REAL  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3065 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: DOUBLE  */
#line 624 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kDouble}; }
#line 3071 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: DATE  */
#line 625 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kDate}; }
#line 3077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: TIME  */
#line 626 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kTime}; }
#line 3083 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DATETIME  */
#line 627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kDateTime}; }
#line 3089 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: TIMESTAMP  */
#line 628 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = ColumnType{LogicalType::kTimestamp}; }
#line 3095 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: UUID  */
#line 629 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kUuid}; }
#line 3101 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: POINT  */
#line 630 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kPoint}; }
#line 3107 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: LINE  */
#line 631 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLine}; }
#line 3113 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: LSEG  */
#line 632 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLineSeg}; }
#line 3119 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: BOX  */
#line 633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = ColumnType{LogicalType::kBox}; }
#line 3125 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: PATH  */
#line 634 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kPath}; }
#line 3131 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: POLYGON  */
#line 635 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kPolygon}; }
#line 3137 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: CIRCLE  */
#line 636 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kCircle}; }
#line 3143 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 638 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kChar, (yyvsp[-1].long_value)}; }
#line 3149 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 639 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3155 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3161 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 641 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-1].long_value), 0}; }
#line 3167 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: DECIMAL  */
#line 642 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
#line 3173 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 643 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3179 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = ColumnType{LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3185 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 645 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3191 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 646 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3197 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 647 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3203 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 648 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3209 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 649 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3215 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 651 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3227 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 652 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 653 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3239 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 654 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3245 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 655 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3251 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 656 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3257 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 657 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3263 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 658 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3269 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_constraints: column_constraint  */
#line 677 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new HashSet<ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3278 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 77: /* column_constraints: column_constraints column_constraint  */
#line 681 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3292 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_constraint: PRIMARY KEY  */
#line 691 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = ConstraintType::kPrimaryKey;
}
#line 3300 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_constraint: UNIQUE  */
#line 694 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = ConstraintType::kUnique;
}
#line 3308 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_constraint: NULLABLE  */
#line 697 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = ConstraintType::kNull;
}
#line 3316 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_constraint: NOT NULLABLE  */
#line 700 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = ConstraintType::kNotNull;
}
#line 3324 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 704 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kPrimaryKey;
}
#line 3334 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 709 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kUnique;
}
#line 3344 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* identifier_array: IDENTIFIER  */
#line 716 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new Vector<String>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3355 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 722 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3366 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 86: /* delete_statement: DELETE FROM table_name where_clause  */
#line 732 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                       {
    (yyval.delete_stmt) = new DeleteStatement();

    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.delete_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.delete_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);
    (yyval.delete_stmt)->where_expr_ = (yyvsp[0].expr_t);
}
#line 3383 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 87: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' constant_expr_array ')'  */
#line 748 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3400 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 88: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 760 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                        {
    (yyval.insert_stmt) = new InsertStatement();
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
#line 3417 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* optional_identifier_array: '(' identifier_array ')'  */
#line 773 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3425 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* optional_identifier_array: %empty  */
#line 776 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3433 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 783 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyval.explain_stmt) = new ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3443 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* explain_type: AST  */
#line 789 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.explain_type_t) = ExplainType::kAst;
}
#line 3451 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* explain_type: UNOPT LOGICAL  */
#line 792 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.explain_type_t) = ExplainType::kUnOpt;
}
#line 3459 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* explain_type: OPT LOGICAL  */
#line 795 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    (yyval.explain_type_t) = ExplainType::kOpt;
}
#line 3467 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* explain_type: PHYSICAL  */
#line 798 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPhysical;
}
#line 3475 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* explain_type: PIPELINE  */
#line 801 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPipeline;
}
#line 3483 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 808 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3500 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* update_expr_array: update_expr  */
#line 821 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.update_expr_array_t) = new Vector<UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3509 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* update_expr_array: update_expr_array ',' update_expr  */
#line 825 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3518 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* update_expr: IDENTIFIER '=' expr  */
#line 830 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.update_expr_t) = new UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3530 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 843 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = std::move(drop_schema_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3546 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 856 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.drop_stmt) = new DropStatement();
    std::unique_ptr<DropCollectionInfo> drop_collection_info = std::make_unique<DropCollectionInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_collection_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_collection_info->collection_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = std::move(drop_collection_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 3564 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* drop_statement: DROP TABLE if_exists table_name  */
#line 871 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3582 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 104: /* drop_statement: DROP VIEW if_exists table_name  */
#line 886 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyval.drop_stmt) = new DropStatement();
    std::unique_ptr<DropViewInfo> drop_view_info = std::make_unique<DropViewInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_view_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_view_info->view_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.drop_stmt)->drop_info_ = std::move(drop_view_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
    delete (yyvsp[0].table_name_t);
}
#line 3600 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 105: /* drop_statement: DROP INDEX if_exists table_name  */
#line 901 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropIndexInfo> drop_index_info = MakeUnique<DropIndexInfo>();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_index_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_index_info->index_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);

    (yyval.drop_stmt)->drop_info_ = std::move(drop_index_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3619 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 920 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3665 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 961 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3711 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* select_statement: select_without_paren  */
#line 1006 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3719 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* select_statement: select_with_paren  */
#line 1009 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3727 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1012 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                     {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3741 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1021 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3755 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* select_with_paren: '(' select_without_paren ')'  */
#line 1031 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3763 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* select_with_paren: '(' select_with_paren ')'  */
#line 1034 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3771 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1038 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3780 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1043 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 3791 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1050 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3799 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1053 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3807 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1058 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3826 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1073 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3834 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* order_by_clause: %empty  */
#line 1076 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3842 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* order_by_expr_list: order_by_expr  */
#line 1080 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new Vector<OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 3851 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1084 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 3860 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* order_by_expr: expr order_by_type  */
#line 1089 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 3870 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* order_by_type: ASC  */
#line 1095 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = kAsc;
}
#line 3878 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* order_by_type: DESC  */
#line 1098 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = kDesc;
}
#line 3886 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* order_by_type: %empty  */
#line 1101 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.order_by_type_t) = kAsc;
}
#line 3894 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* limit_expr: LIMIT expr  */
#line 1105 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3902 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* limit_expr: %empty  */
#line 1109 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3908 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* offset_expr: OFFSET expr  */
#line 1111 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3916 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* offset_expr: %empty  */
#line 1115 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3922 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* distinct: DISTINCT  */
#line 1117 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3930 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* distinct: %empty  */
#line 1120 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3938 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 133: /* from_clause: FROM table_reference  */
#line 1124 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3946 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 134: /* from_clause: %empty  */
#line 1127 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3954 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 135: /* where_clause: WHERE expr  */
#line 1131 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3962 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* where_clause: %empty  */
#line 1134 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3970 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* having_clause: HAVING expr  */
#line 1138 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3978 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* having_clause: %empty  */
#line 1141 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3986 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 139: /* group_by_clause: GROUP BY expr_array  */
#line 1145 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 3994 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* group_by_clause: %empty  */
#line 1148 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4002 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* set_operator: UNION  */
#line 1152 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = SetOperatorType::kUnion;
}
#line 4010 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* set_operator: UNION ALL  */
#line 1155 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kUnionAll;
}
#line 4018 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* set_operator: INTERSECT  */
#line 1158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kIntersect;
}
#line 4026 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 144: /* set_operator: EXCEPT  */
#line 1161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = SetOperatorType::kExcept;
}
#line 4034 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 145: /* table_reference: table_reference_unit  */
#line 1169 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4042 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 146: /* table_reference: table_reference ',' table_reference_unit  */
#line 1172 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CrossProductReference* cross_product_ref = nullptr;
    if((yyvsp[-2].table_reference_t)->type_ == TableRefType::kCrossProduct) {
        cross_product_ref = (CrossProductReference*)(yyvsp[-2].table_reference_t);
        cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
    } else {
        cross_product_ref = new CrossProductReference();
        cross_product_ref->tables_.emplace_back((yyvsp[-2].table_reference_t));
        cross_product_ref->tables_.emplace_back((yyvsp[0].table_reference_t));
    }

    (yyval.table_reference_t) = cross_product_ref;
}
#line 4060 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 149: /* table_reference_name: table_name table_alias  */
#line 1189 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4078 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 150: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    SubqueryReference* subquery_reference = new SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4089 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* table_name: IDENTIFIER  */
#line 1212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4101 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1219 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        ParserHelper::ToLower((yyvsp[-2].str_value));
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4115 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* table_alias: AS IDENTIFIER  */
#line 1230 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4125 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* table_alias: IDENTIFIER  */
#line 1235 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4135 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1240 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4146 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 156: /* table_alias: %empty  */
#line 1246 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4154 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 157: /* with_clause: WITH with_expr_list  */
#line 1253 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4162 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* with_clause: %empty  */
#line 1256 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4170 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* with_expr_list: with_expr  */
#line 1260 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new Vector<WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4179 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1263 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4188 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1268 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                             {
    (yyval.with_expr_t) = new WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1280 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4212 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1287 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                   {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4225 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* join_type: INNER  */
#line 1301 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = JoinType::kInner;
}
#line 4233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* join_type: LEFT  */
#line 1304 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kLeft;
}
#line 4241 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* join_type: RIGHT  */
#line 1307 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kRight;
}
#line 4249 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* join_type: OUTER  */
#line 1310 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4257 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* join_type: FULL  */
#line 1313 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4265 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* join_type: CROSS  */
#line 1316 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kCross;
}
#line 4273 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* join_type: %empty  */
#line 1319 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 4280 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* show_statement: SHOW TABLES  */
#line 1325 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kTables;
}
#line 4289 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* show_statement: DESCRIBE table_name  */
#line 1329 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4305 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* expr_array: expr_alias  */
#line 1345 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4314 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* expr_array: expr_array ',' expr_alias  */
#line 1349 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4323 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 175: /* constant_expr_array: constant_expr  */
#line 1354 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4332 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 176: /* constant_expr_array: constant_expr_array ',' constant_expr  */
#line 1358 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4341 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 177: /* expr_alias: expr AS IDENTIFIER  */
#line 1363 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4352 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 178: /* expr_alias: expr  */
#line 1369 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4360 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 184: /* operand: '(' expr ')'  */
#line 1379 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4368 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 185: /* operand: '(' select_without_paren ')'  */
#line 1382 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4379 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 191: /* function_expr: IDENTIFIER '(' ')'  */
#line 1394 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 4392 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 192: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 4405 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 193: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1410 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 4419 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 194: /* function_expr: operand IS NOT NULLABLE  */
#line 1419 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4431 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 195: /* function_expr: operand IS NULLABLE  */
#line 1426 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4443 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 196: /* function_expr: NOT operand  */
#line 1433 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "not";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4455 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 197: /* function_expr: '-' operand  */
#line 1440 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "-";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4467 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 198: /* function_expr: '+' operand  */
#line 1447 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "+";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4479 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 199: /* function_expr: operand '-' operand  */
#line 1454 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4492 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 200: /* function_expr: operand '+' operand  */
#line 1462 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4505 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 201: /* function_expr: operand '*' operand  */
#line 1470 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4518 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: operand '/' operand  */
#line 1478 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4531 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: operand '%' operand  */
#line 1486 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4544 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 204: /* function_expr: operand '=' operand  */
#line 1494 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4557 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 205: /* function_expr: operand EQUAL operand  */
#line 1502 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4570 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 206: /* function_expr: operand NOT_EQ operand  */
#line 1510 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4583 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 207: /* function_expr: operand '<' operand  */
#line 1518 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4596 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 208: /* function_expr: operand '>' operand  */
#line 1526 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4609 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 209: /* function_expr: operand LESS_EQ operand  */
#line 1534 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4622 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 210: /* function_expr: operand GREATER_EQ operand  */
#line 1542 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4635 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 211: /* function_expr: EXTRACT '(' constant_expr FROM operand ')'  */
#line 1550 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "extract";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4648 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 212: /* function_expr: operand LIKE operand  */
#line 1558 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4661 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 213: /* function_expr: operand NOT LIKE operand  */
#line 1566 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4674 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 214: /* conjunction_expr: expr AND expr  */
#line 1575 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4687 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 215: /* conjunction_expr: expr OR expr  */
#line 1583 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4700 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 216: /* between_expr: operand BETWEEN operand AND operand  */
#line 1592 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                  {
    BetweenExpr* between_expr = new BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 4712 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 217: /* in_expr: operand IN '(' expr_array ')'  */
#line 1600 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4724 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 218: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1607 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-5].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4736 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 219: /* case_expr: CASE expr case_check_array END  */
#line 1615 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4747 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 220: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4759 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 221: /* case_expr: CASE case_check_array END  */
#line 1628 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4769 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 222: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4780 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 223: /* case_check_array: WHEN expr THEN expr  */
#line 1640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    (yyval.case_check_array_t) = new Vector<WhenThen*>();
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 4792 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 224: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1647 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 4804 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 225: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1655 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4841 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 226: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1688 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                   {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4852 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 227: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1694 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4863 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 228: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1700 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4875 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 229: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1707 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4887 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 230: /* column_expr: IDENTIFIER  */
#line 1715 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    ColumnExpr* column_expr = new ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4899 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 231: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1722 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 232: /* column_expr: '*'  */
#line 1729 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4921 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 233: /* column_expr: column_expr '.' '*'  */
#line 1734 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4935 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 234: /* constant_expr: STRING  */
#line 1744 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4945 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 235: /* constant_expr: TRUE  */
#line 1749 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
#line 4955 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 236: /* constant_expr: FALSE  */
#line 1754 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
#line 4965 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 237: /* constant_expr: DOUBLE_VALUE  */
#line 1759 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
#line 4975 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 238: /* constant_expr: LONG_VALUE  */
#line 1764 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4985 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 239: /* constant_expr: DATE STRING  */
#line 1769 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4995 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 240: /* constant_expr: INTERVAL interval_expr  */
#line 1774 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5003 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 241: /* constant_expr: interval_expr  */
#line 1777 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5011 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 242: /* interval_expr: LONG_VALUE SECONDS  */
#line 1781 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5022 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 243: /* interval_expr: LONG_VALUE SECOND  */
#line 1787 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5033 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 244: /* interval_expr: LONG_VALUE MINUTES  */
#line 1793 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5044 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 245: /* interval_expr: LONG_VALUE MINUTE  */
#line 1799 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 246: /* interval_expr: LONG_VALUE HOURS  */
#line 1805 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5066 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 247: /* interval_expr: LONG_VALUE HOUR  */
#line 1811 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5077 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 248: /* interval_expr: LONG_VALUE DAYS  */
#line 1817 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5088 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 249: /* interval_expr: LONG_VALUE DAY  */
#line 1823 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5099 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 250: /* interval_expr: LONG_VALUE MONTHS  */
#line 1829 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5110 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 251: /* interval_expr: LONG_VALUE MONTH  */
#line 1835 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5121 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 252: /* interval_expr: LONG_VALUE YEARS  */
#line 1841 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5132 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 253: /* interval_expr: LONG_VALUE YEAR  */
#line 1847 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5143 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 254: /* copy_option_list: copy_option  */
#line 1858 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.copy_option_array) = new Vector<CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 5152 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 255: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1862 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 5161 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 256: /* copy_option: FORMAT IDENTIFIER  */
#line 1867 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 5179 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 257: /* copy_option: DELIMITER STRING  */
#line 1880 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 5190 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 258: /* copy_option: HEADER  */
#line 1886 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 5200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 259: /* file_path: STRING  */
#line 1892 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5208 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 260: /* if_exists: IF EXISTS  */
#line 1896 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5214 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 261: /* if_exists: %empty  */
#line 1897 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5220 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 262: /* if_not_exists: IF NOT EXISTS  */
#line 1899 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5226 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 263: /* if_not_exists: %empty  */
#line 1900 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5232 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 5236 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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

#line 1906 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

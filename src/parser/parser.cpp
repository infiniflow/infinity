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
  YYSYMBOL_ANALYZE = 39,                   /* ANALYZE  */
  YYSYMBOL_GROUP = 40,                     /* GROUP  */
  YYSYMBOL_BY = 41,                        /* BY  */
  YYSYMBOL_HAVING = 42,                    /* HAVING  */
  YYSYMBOL_AS = 43,                        /* AS  */
  YYSYMBOL_NATURAL = 44,                   /* NATURAL  */
  YYSYMBOL_JOIN = 45,                      /* JOIN  */
  YYSYMBOL_LEFT = 46,                      /* LEFT  */
  YYSYMBOL_RIGHT = 47,                     /* RIGHT  */
  YYSYMBOL_OUTER = 48,                     /* OUTER  */
  YYSYMBOL_FULL = 49,                      /* FULL  */
  YYSYMBOL_ON = 50,                        /* ON  */
  YYSYMBOL_INNER = 51,                     /* INNER  */
  YYSYMBOL_CROSS = 52,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 53,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 54,                     /* WHERE  */
  YYSYMBOL_ORDER = 55,                     /* ORDER  */
  YYSYMBOL_LIMIT = 56,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 57,                    /* OFFSET  */
  YYSYMBOL_ASC = 58,                       /* ASC  */
  YYSYMBOL_DESC = 59,                      /* DESC  */
  YYSYMBOL_IF = 60,                        /* IF  */
  YYSYMBOL_NOT = 61,                       /* NOT  */
  YYSYMBOL_EXISTS = 62,                    /* EXISTS  */
  YYSYMBOL_IN = 63,                        /* IN  */
  YYSYMBOL_FROM = 64,                      /* FROM  */
  YYSYMBOL_TO = 65,                        /* TO  */
  YYSYMBOL_WITH = 66,                      /* WITH  */
  YYSYMBOL_DELIMITER = 67,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 68,                    /* FORMAT  */
  YYSYMBOL_HEADER = 69,                    /* HEADER  */
  YYSYMBOL_CAST = 70,                      /* CAST  */
  YYSYMBOL_END = 71,                       /* END  */
  YYSYMBOL_CASE = 72,                      /* CASE  */
  YYSYMBOL_ELSE = 73,                      /* ELSE  */
  YYSYMBOL_THEN = 74,                      /* THEN  */
  YYSYMBOL_WHEN = 75,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 76,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 77,                   /* INTEGER  */
  YYSYMBOL_TINYINT = 78,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 79,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 80,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 81,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 82,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 83,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 84,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 85,                    /* DOUBLE  */
  YYSYMBOL_REAL = 86,                      /* REAL  */
  YYSYMBOL_DECIMAL = 87,                   /* DECIMAL  */
  YYSYMBOL_DATE = 88,                      /* DATE  */
  YYSYMBOL_TIME = 89,                      /* TIME  */
  YYSYMBOL_DATETIME = 90,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 91,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 92,                      /* UUID  */
  YYSYMBOL_POINT = 93,                     /* POINT  */
  YYSYMBOL_LINE = 94,                      /* LINE  */
  YYSYMBOL_LSEG = 95,                      /* LSEG  */
  YYSYMBOL_BOX = 96,                       /* BOX  */
  YYSYMBOL_PATH = 97,                      /* PATH  */
  YYSYMBOL_POLYGON = 98,                   /* POLYGON  */
  YYSYMBOL_CIRCLE = 99,                    /* CIRCLE  */
  YYSYMBOL_BLOB = 100,                     /* BLOB  */
  YYSYMBOL_BITMAP = 101,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 102,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 103,                   /* VECTOR  */
  YYSYMBOL_BIT = 104,                      /* BIT  */
  YYSYMBOL_PRIMARY = 105,                  /* PRIMARY  */
  YYSYMBOL_KEY = 106,                      /* KEY  */
  YYSYMBOL_UNIQUE = 107,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 108,                 /* NULLABLE  */
  YYSYMBOL_IS = 109,                       /* IS  */
  YYSYMBOL_TRUE = 110,                     /* TRUE  */
  YYSYMBOL_FALSE = 111,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 112,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 113,                   /* SECOND  */
  YYSYMBOL_SECONDS = 114,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 115,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 116,                  /* MINUTES  */
  YYSYMBOL_HOUR = 117,                     /* HOUR  */
  YYSYMBOL_HOURS = 118,                    /* HOURS  */
  YYSYMBOL_DAY = 119,                      /* DAY  */
  YYSYMBOL_DAYS = 120,                     /* DAYS  */
  YYSYMBOL_MONTH = 121,                    /* MONTH  */
  YYSYMBOL_MONTHS = 122,                   /* MONTHS  */
  YYSYMBOL_YEAR = 123,                     /* YEAR  */
  YYSYMBOL_YEARS = 124,                    /* YEARS  */
  YYSYMBOL_EQUAL = 125,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 126,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 127,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 128,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 129,                  /* BETWEEN  */
  YYSYMBOL_AND = 130,                      /* AND  */
  YYSYMBOL_OR = 131,                       /* OR  */
  YYSYMBOL_EXTRACT = 132,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 133,                     /* LIKE  */
  YYSYMBOL_NUMBER = 134,                   /* NUMBER  */
  YYSYMBOL_135_ = 135,                     /* '='  */
  YYSYMBOL_136_ = 136,                     /* '<'  */
  YYSYMBOL_137_ = 137,                     /* '>'  */
  YYSYMBOL_138_ = 138,                     /* '+'  */
  YYSYMBOL_139_ = 139,                     /* '-'  */
  YYSYMBOL_140_ = 140,                     /* '*'  */
  YYSYMBOL_141_ = 141,                     /* '/'  */
  YYSYMBOL_142_ = 142,                     /* '%'  */
  YYSYMBOL_143_ = 143,                     /* '['  */
  YYSYMBOL_144_ = 144,                     /* ']'  */
  YYSYMBOL_145_ = 145,                     /* '('  */
  YYSYMBOL_146_ = 146,                     /* ')'  */
  YYSYMBOL_147_ = 147,                     /* '.'  */
  YYSYMBOL_148_ = 148,                     /* ';'  */
  YYSYMBOL_149_ = 149,                     /* ','  */
  YYSYMBOL_YYACCEPT = 150,                 /* $accept  */
  YYSYMBOL_input_pattern = 151,            /* input_pattern  */
  YYSYMBOL_statement_list = 152,           /* statement_list  */
  YYSYMBOL_statement = 153,                /* statement  */
  YYSYMBOL_explainable_statement = 154,    /* explainable_statement  */
  YYSYMBOL_create_statement = 155,         /* create_statement  */
  YYSYMBOL_table_element_array = 156,      /* table_element_array  */
  YYSYMBOL_table_element = 157,            /* table_element  */
  YYSYMBOL_table_column = 158,             /* table_column  */
  YYSYMBOL_column_type = 159,              /* column_type  */
  YYSYMBOL_column_constraints = 160,       /* column_constraints  */
  YYSYMBOL_column_constraint = 161,        /* column_constraint  */
  YYSYMBOL_table_constraint = 162,         /* table_constraint  */
  YYSYMBOL_identifier_array = 163,         /* identifier_array  */
  YYSYMBOL_delete_statement = 164,         /* delete_statement  */
  YYSYMBOL_insert_statement = 165,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 166, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 167,        /* explain_statement  */
  YYSYMBOL_explain_type = 168,             /* explain_type  */
  YYSYMBOL_update_statement = 169,         /* update_statement  */
  YYSYMBOL_update_expr_array = 170,        /* update_expr_array  */
  YYSYMBOL_update_expr = 171,              /* update_expr  */
  YYSYMBOL_drop_statement = 172,           /* drop_statement  */
  YYSYMBOL_copy_statement = 173,           /* copy_statement  */
  YYSYMBOL_select_statement = 174,         /* select_statement  */
  YYSYMBOL_select_with_paren = 175,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 176,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 177, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 178, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 179, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 180,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 181,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 182,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 183,            /* order_by_type  */
  YYSYMBOL_limit_expr = 184,               /* limit_expr  */
  YYSYMBOL_offset_expr = 185,              /* offset_expr  */
  YYSYMBOL_distinct = 186,                 /* distinct  */
  YYSYMBOL_from_clause = 187,              /* from_clause  */
  YYSYMBOL_where_clause = 188,             /* where_clause  */
  YYSYMBOL_having_clause = 189,            /* having_clause  */
  YYSYMBOL_group_by_clause = 190,          /* group_by_clause  */
  YYSYMBOL_set_operator = 191,             /* set_operator  */
  YYSYMBOL_table_reference = 192,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 193,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 194,     /* table_reference_name  */
  YYSYMBOL_table_name = 195,               /* table_name  */
  YYSYMBOL_table_alias = 196,              /* table_alias  */
  YYSYMBOL_with_clause = 197,              /* with_clause  */
  YYSYMBOL_with_expr_list = 198,           /* with_expr_list  */
  YYSYMBOL_with_expr = 199,                /* with_expr  */
  YYSYMBOL_join_clause = 200,              /* join_clause  */
  YYSYMBOL_join_type = 201,                /* join_type  */
  YYSYMBOL_show_statement = 202,           /* show_statement  */
  YYSYMBOL_expr_array = 203,               /* expr_array  */
  YYSYMBOL_constant_expr_array = 204,      /* constant_expr_array  */
  YYSYMBOL_expr_alias = 205,               /* expr_alias  */
  YYSYMBOL_expr = 206,                     /* expr  */
  YYSYMBOL_operand = 207,                  /* operand  */
  YYSYMBOL_function_expr = 208,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 209,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 210,             /* between_expr  */
  YYSYMBOL_in_expr = 211,                  /* in_expr  */
  YYSYMBOL_case_expr = 212,                /* case_expr  */
  YYSYMBOL_case_check_array = 213,         /* case_check_array  */
  YYSYMBOL_cast_expr = 214,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 215,            /* subquery_expr  */
  YYSYMBOL_column_expr = 216,              /* column_expr  */
  YYSYMBOL_constant_expr = 217,            /* constant_expr  */
  YYSYMBOL_interval_expr = 218,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 219,         /* copy_option_list  */
  YYSYMBOL_copy_option = 220,              /* copy_option  */
  YYSYMBOL_file_path = 221,                /* file_path  */
  YYSYMBOL_if_exists = 222,                /* if_exists  */
  YYSYMBOL_if_not_exists = 223,            /* if_not_exists  */
  YYSYMBOL_semicolon = 224                 /* semicolon  */
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
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   702

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  150
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  75
/* YYNRULES -- Number of rules.  */
#define YYNRULES  267
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  532

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   389


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
       2,     2,     2,     2,     2,     2,     2,   142,     2,     2,
     145,   146,   140,   138,   149,   139,   147,   141,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   148,
     136,   135,   137,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   143,     2,   144,     2,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134
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
     776,   783,   789,   792,   795,   798,   801,   804,   807,   814,
     827,   831,   836,   849,   862,   877,   892,   907,   926,   967,
    1012,  1015,  1018,  1027,  1037,  1040,  1044,  1049,  1056,  1059,
    1064,  1079,  1082,  1086,  1090,  1095,  1101,  1104,  1107,  1111,
    1115,  1117,  1121,  1123,  1126,  1130,  1133,  1137,  1140,  1144,
    1147,  1151,  1154,  1158,  1161,  1164,  1167,  1175,  1178,  1193,
    1193,  1195,  1209,  1218,  1225,  1236,  1241,  1246,  1252,  1259,
    1262,  1266,  1269,  1274,  1286,  1293,  1307,  1310,  1313,  1316,
    1319,  1322,  1325,  1331,  1335,  1351,  1355,  1360,  1364,  1369,
    1375,  1379,  1380,  1381,  1382,  1383,  1385,  1388,  1394,  1395,
    1396,  1397,  1398,  1400,  1408,  1416,  1425,  1432,  1439,  1446,
    1453,  1460,  1468,  1476,  1484,  1492,  1500,  1508,  1516,  1524,
    1532,  1540,  1548,  1556,  1564,  1572,  1581,  1589,  1598,  1606,
    1613,  1621,  1627,  1634,  1639,  1646,  1653,  1661,  1694,  1700,
    1706,  1713,  1721,  1728,  1735,  1740,  1750,  1755,  1760,  1765,
    1770,  1775,  1780,  1783,  1787,  1793,  1799,  1805,  1811,  1817,
    1823,  1829,  1835,  1841,  1847,  1853,  1864,  1868,  1873,  1886,
    1892,  1898,  1902,  1903,  1905,  1906,  1908,  1909
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
  "UNOPT", "OPT", "LOGICAL", "PHYSICAL", "VIEW", "INDEX", "ANALYZE",
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM",
  "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "TINYINT", "SMALLINT",
  "BIGINT", "HUGEINT", "CHAR", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID", "POINT",
  "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE", "NULLABLE",
  "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','",
  "$accept", "input_pattern", "statement_list", "statement",
  "explainable_statement", "create_statement", "table_element_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "table_constraint", "identifier_array",
  "delete_statement", "insert_statement", "optional_identifier_array",
  "explain_statement", "explain_type", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
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

#define YYPACT_NINF (-314)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-267)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   266,    -9,   270,    67,     2,    67,   312,    66,    67,
      81,   -32,   121,    -7,  -314,  -314,  -314,  -314,  -314,  -314,
    -314,  -314,   220,  -314,  -314,   152,  -314,   111,   111,   111,
     111,   111,    67,   115,   115,   115,   115,   115,    32,   175,
      67,   211,  -314,  -314,   150,   160,  -314,  -314,   171,  -314,
    -314,   156,    55,  -314,    70,    77,  -314,    12,  -314,   224,
    -314,  -314,     7,   195,  -314,   207,   196,   279,    67,    67,
      67,   284,   149,   243,   306,    67,    67,    67,    67,   314,
     319,   215,   335,   335,  -314,  -314,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,   220,  -314,   209,    81,  -314,  -314,  -314,
    -314,     7,  -314,  -314,  -314,   249,   315,   322,   318,  -314,
     -33,  -314,   149,   327,   380,     5,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,   250,   -47,  -314,   249,  -314,  -314,   324,
     326,   152,  -314,   253,   254,   256,  -314,  -314,   479,   296,
     276,   280,   139,   407,  -314,  -314,   418,   281,   321,   321,
    -314,   202,   -46,  -314,   -30,   359,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,   282,  -314,  -314,   249,   249,   370,  -314,
     -32,     3,   387,    67,  -314,  -106,   292,  -314,   249,   319,
    -314,   108,   293,   294,   297,  -314,  -314,    86,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,
     321,   295,   381,   378,   249,   249,    63,    90,  -314,   479,
    -314,    58,    88,    88,   299,   -58,     6,   249,   215,   453,
     249,   249,   -19,   313,   -23,   321,   321,   321,   321,   321,
     321,   321,   321,   321,   321,   321,   321,   321,   321,    13,
     308,  -314,    24,   108,   249,  -314,   220,   488,   363,   336,
     -31,  -314,  -314,  -314,   -32,   338,  -314,   468,    58,   108,
    -314,   328,   328,  -314,   249,  -314,    18,   378,   347,   343,
     -14,   -44,   144,  -314,   249,   249,   427,  -314,  -314,   -32,
     333,   403,  -314,    56,  -314,  -314,   462,  -314,  -314,   373,
     348,   321,   202,   396,  -314,   401,   401,   421,   421,   337,
     401,   401,   421,   421,    88,    88,  -314,  -314,  -314,  -314,
    -314,   249,  -314,  -314,  -314,   108,  -314,  -314,  -314,  -314,
    -314,  -314,   360,   366,  -314,  -314,  -314,   367,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,
     368,   379,   386,   388,   105,   390,   380,  -314,     3,   220,
     380,  -314,   124,  -314,   511,   522,  -314,   134,  -314,   140,
     169,  -314,   398,  -314,   488,   249,  -314,   249,   -24,    46,
     321,    27,     6,   487,  -314,  -314,  -314,  -314,  -314,  -314,
     500,  -314,   543,  -314,   506,   508,   202,   401,   405,   174,
    -314,   321,  -314,   546,   547,   548,   549,   550,   181,   251,
     496,   499,  -314,  -314,   105,  -314,   380,   203,  -314,   204,
    -314,    58,  -314,  -314,  -314,   328,  -314,  -314,  -314,   460,
     108,    91,  -314,   249,   277,    56,   403,     6,     6,   463,
     249,   249,  -314,   461,   221,  -314,  -314,   381,   464,   465,
     223,   466,   467,   469,   470,   471,   472,   473,   474,   475,
     476,   477,   478,   480,   481,   482,   483,  -314,  -314,  -314,
     225,  -314,  -314,  -314,  -314,  -314,  -314,   108,  -314,  -314,
    -314,   559,   380,   484,   108,  -314,  -314,  -314,  -314,  -314,
     608,  -314,  -314,   609,   610,   611,   622,   628,   629,   630,
     631,   632,   633,   634,   635,   636,   637,  -314,   249,   230,
     498,   501,   502,   503,   504,   505,   507,   509,   510,   512,
     513,   514,   515,   516,   517,   108,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,
    -314,  -314
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     160,     0,     0,     0,     0,     0,     0,    98,     0,     0,
       0,   160,     0,   267,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   111,   110,     0,     8,   265,   265,   265,
     265,   265,     0,   263,   263,   263,   263,   263,   153,     0,
       0,     0,    93,    97,     0,     0,    96,    92,   160,   173,
     174,     0,   159,   161,     0,     0,     1,   160,     2,   143,
     145,   146,     0,   134,   116,   122,     0,     0,     0,     0,
       0,     0,    90,     0,     0,     0,     0,     0,     0,     0,
       0,   138,     0,     0,    94,    95,    91,    14,    19,    21,
      20,    15,    16,    18,    17,     0,     0,   115,   114,     4,
     144,     0,   112,   113,   133,     0,     0,   130,     0,    22,
       0,    23,    90,     0,     0,   160,   262,   103,   105,   104,
     106,   107,   154,     0,   138,   100,     0,    86,   261,     0,
       0,     0,   162,     0,     0,   232,   236,   239,   240,     0,
       0,     0,     0,     0,   237,   238,     0,     0,     0,     0,
     234,   160,   136,   175,   180,   181,   190,   185,   184,   183,
     191,   192,   182,   189,   188,   243,     0,     0,   132,   264,
     160,     0,     0,     0,    84,     0,     0,    88,     0,     0,
      99,   137,     0,     0,     0,   119,   118,     0,   245,   244,
     247,   246,   249,   248,   251,   250,   253,   252,   255,   254,
       0,     0,   198,   160,     0,     0,     0,     0,   241,     0,
     242,     0,   200,   199,     0,     0,     0,     0,   138,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     121,   123,   128,   129,     0,   117,    25,     0,     0,     0,
       0,    28,    30,    31,   160,     0,    89,     0,     0,   102,
     101,     0,     0,   163,     0,   193,     0,   160,     0,     0,
       0,     0,     0,   223,     0,     0,     0,   187,   186,   160,
     135,   147,   149,   158,   150,   176,   142,   179,   216,   217,
       0,     0,   160,     0,   197,   207,   208,   211,   212,     0,
     214,   206,   209,   210,   202,   201,   203,   204,   205,   233,
     235,     0,   126,   127,   125,   131,    34,    37,    35,    36,
      38,    39,     0,     0,    40,    42,    41,    59,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
       0,     0,     0,     0,    32,     0,     0,    24,     0,    26,
       0,    85,     0,   177,     0,     0,   260,     0,   256,     0,
       0,   194,     0,   228,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,   167,   168,   169,   170,   166,   171,
       0,   156,     0,   151,     0,   140,   160,   215,     0,     0,
     196,     0,   124,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    33,    76,     0,     0,    29,     0,
      87,     0,   259,   258,   109,     0,   108,   195,   229,     0,
     225,     0,   224,     0,     0,   158,   148,     0,     0,   155,
       0,     0,   120,     0,     0,   230,   219,   218,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    81,    78,    77,
       0,    83,    27,   178,   257,   227,   222,   226,   213,   152,
     164,     0,     0,   141,   139,   231,   220,    55,    56,    58,
       0,    60,    61,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    82,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   165,   157,    57,    65,    63,
      64,    66,    67,    68,    62,    72,    70,    71,    73,    74,
      75,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -314,  -314,  -314,   588,  -314,   598,  -314,   304,  -314,   290,
    -314,   260,  -314,  -313,   617,   618,   545,  -314,  -314,   619,
    -314,   489,   621,   623,   -34,   659,   -11,   541,   572,    36,
    -314,  -314,   364,  -314,  -314,  -314,  -314,  -314,  -113,  -314,
    -314,  -314,  -314,   302,   -95,    -1,   252,  -314,  -314,   580,
    -314,  -314,   638,  -183,  -314,   485,  -125,  -103,  -314,  -314,
    -314,  -314,  -314,   486,  -314,  -314,  -314,  -209,   532,   417,
     265,   599,   199,   334,  -314
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    12,    13,    14,    86,    15,   250,   251,   252,   344,
     404,   405,   253,   175,    16,    17,   115,    18,    48,    19,
     124,   125,    20,    21,    22,    23,    24,    64,   102,    65,
     107,   240,   241,   314,   168,   245,   105,   218,   127,   432,
     385,    62,   280,   281,   282,   283,   383,    25,    52,    53,
     284,   380,    26,   152,   352,   153,   154,   155,   156,   157,
     158,   159,   160,   207,   161,   162,   163,   164,   165,   357,
     358,   129,    74,    67,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      55,   181,   276,    39,   266,    41,   247,   126,    50,    38,
     170,   180,  -266,   219,    93,    63,   309,   206,   216,     1,
      32,     2,     3,     4,     5,     6,   215,     7,     8,   364,
     365,    72,     9,   407,    10,   176,   202,   409,   293,    81,
     256,   242,   243,   257,   290,   212,   213,   422,    59,   353,
      60,    61,     1,   259,     2,     3,     4,     5,     6,   381,
       7,     8,   136,   137,   138,     9,    40,   110,   111,   112,
      38,    10,   220,   221,   118,   119,   120,   121,    10,   270,
     271,   360,   312,   313,    51,   294,   220,   221,   278,   135,
     136,   137,   138,   460,    49,   288,   289,   202,   103,   382,
     220,   221,   179,   217,   177,   286,   220,   221,   248,   389,
     249,    10,   171,    11,   291,   347,   220,   221,   348,   315,
     423,    56,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   246,   134,   205,   264,
     214,    57,   135,   136,   137,   138,   143,   139,   140,   368,
     369,   279,   101,   310,   220,   221,   141,    11,   142,   499,
      63,   273,   466,   274,   361,   275,   400,   217,   144,   145,
     146,    66,   255,   425,   143,    73,   220,   221,     1,    79,
       2,     3,     4,     5,     6,    84,   242,     8,   387,    80,
      11,     9,   269,   220,   221,    85,   144,   145,   146,    95,
     139,   140,   463,   434,    96,   135,   136,   137,   138,   141,
     401,   142,   402,   403,   205,   366,    97,   367,   147,   275,
     349,   220,   221,    98,   148,   149,   150,   143,   236,   237,
     238,   151,   265,    75,    76,    77,    78,    10,   220,   221,
     420,    59,   421,    60,    61,   371,   100,   473,   104,   144,
     145,   146,   135,   136,   137,   138,   362,   108,   443,   444,
     445,   446,   106,   139,   140,   447,   448,   424,    10,   126,
     410,   147,   141,   411,   142,    82,    83,   148,   149,   150,
     414,   388,   109,   415,   151,   449,   416,   113,   437,   415,
     143,    27,    28,    29,   114,    33,    34,    35,   467,   135,
     136,   137,   138,    30,    31,   116,   474,    36,    37,   117,
     139,   140,   144,   145,   146,   417,    11,   122,   217,   141,
     436,   142,   123,   217,   135,   136,   137,   138,   450,   451,
     452,   453,   470,   471,   147,   454,   455,   143,   268,   128,
     148,   149,   150,    42,    43,    44,    45,   151,    46,   461,
     462,    47,   257,   257,   131,   456,   166,   200,   201,   144,
     145,   146,    68,    69,    70,    71,   141,   476,   142,   479,
     217,   497,   480,   515,   257,   433,   516,   173,   167,   257,
     169,   147,   200,   174,   143,   178,   224,   148,   149,   150,
     182,   141,   183,   142,   151,   354,   355,   356,   268,   185,
     186,   187,   225,   226,   227,   228,   144,   145,   146,   143,
     230,   208,   231,   232,   233,   234,   235,   236,   237,   238,
     222,   203,   223,   468,   209,   204,   211,   244,   147,   239,
     254,   144,   145,   146,   148,   149,   150,   258,   261,   262,
     267,   151,   268,   263,    10,   277,   224,   373,  -172,   374,
     375,   376,   377,   147,   378,   379,   287,   311,   292,   148,
     149,   150,   225,   226,   227,   228,   151,   391,   224,   345,
     230,   351,   231,   232,   233,   234,   235,   236,   237,   238,
     291,   346,   372,   350,   225,   226,   227,   228,   229,   363,
     224,   370,   230,   386,   231,   232,   233,   234,   235,   236,
     237,   238,   384,   220,   390,   393,   225,   226,   227,   228,
     224,   394,   395,   396,   230,   412,   231,   232,   233,   234,
     235,   236,   237,   238,   397,   413,  -267,  -267,   227,   228,
     224,   398,   427,   399,  -267,   406,  -267,   232,   233,   234,
     235,   236,   237,   238,   418,   428,   429,   430,  -267,  -267,
     431,   435,   438,   439,   440,   441,   442,  -267,  -267,   234,
     235,   236,   237,   238,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   457,   458,   465,   475,   472,   498,
     477,   478,   481,   482,   500,   501,   502,   503,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   504,   493,
     494,   495,   496,   217,   505,   506,   507,   508,   509,   510,
     511,   512,   513,   514,   517,    99,    87,   518,   519,   520,
     521,   522,   408,   523,   419,   524,   525,   172,   526,   527,
     528,   529,   530,   531,   459,    88,    89,    90,   260,    91,
      54,    92,   184,   133,   426,   392,   132,   469,   210,   359,
     464,     0,   130,     0,     0,     0,    94,     0,     0,     0,
       0,     0,   272,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   285
};

static const yytype_int16 yycheck[] =
{
      11,   126,   211,     4,   187,     6,     3,    54,     9,     3,
      43,   124,     0,    43,    48,     8,     3,   142,    64,     7,
      29,     9,    10,    11,    12,    13,   151,    15,    16,    43,
      74,    32,    20,   346,    66,    30,   139,   350,    61,    40,
     146,   166,   167,   149,    63,   148,   149,    71,    21,   258,
      23,    24,     7,   178,     9,    10,    11,    12,    13,     3,
      15,    16,     4,     5,     6,    20,    64,    68,    69,    70,
       3,    66,   130,   131,    75,    76,    77,    78,    66,   204,
     205,   264,    58,    59,     3,   108,   130,   131,   146,     3,
       4,     5,     6,   406,    28,   220,   221,   200,    62,    43,
     130,   131,   149,   149,   115,   218,   130,   131,   105,   292,
     107,    66,   145,   145,   133,   146,   130,   131,   149,   244,
      74,     0,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   170,   101,    75,    53,
     151,   148,     3,     4,     5,     6,    88,    61,    62,   274,
     275,   145,   145,   140,   130,   131,    70,   145,    72,   472,
       8,    71,    71,    73,   146,    75,    61,   149,   110,   111,
     112,    60,   173,   146,    88,    60,   130,   131,     7,   147,
       9,    10,    11,    12,    13,    35,   311,    16,   291,    14,
     145,    20,   203,   130,   131,    35,   110,   111,   112,    43,
      61,    62,   411,   386,   149,     3,     4,     5,     6,    70,
     105,    72,   107,   108,    75,    71,   146,    73,   132,    75,
     254,   130,   131,   146,   138,   139,   140,    88,   140,   141,
     142,   145,   146,    34,    35,    36,    37,    66,   130,   131,
     365,    21,   367,    23,    24,   279,    22,   430,    53,   110,
     111,   112,     3,     4,     5,     6,   267,    61,    77,    78,
      79,    80,    55,    61,    62,    84,    85,   370,    66,    54,
     146,   132,    70,   149,    72,    64,    65,   138,   139,   140,
     146,   292,     3,   149,   145,   104,   146,     3,   391,   149,
      88,    25,    26,    27,   145,    25,    26,    27,   423,     3,
       4,     5,     6,    37,    38,    62,   431,    37,    38,     3,
      61,    62,   110,   111,   112,   146,   145,     3,   149,    70,
     146,    72,     3,   149,     3,     4,     5,     6,    77,    78,
      79,    80,   427,   428,   132,    84,    85,    88,    61,     4,
     138,   139,   140,    31,    32,    33,    34,   145,    36,   146,
     146,    39,   149,   149,   145,   104,    41,    61,    62,   110,
     111,   112,    28,    29,    30,    31,    70,   146,    72,   146,
     149,   146,   149,   498,   149,   386,   146,    50,    56,   149,
      62,   132,    61,     3,    88,   135,   109,   138,   139,   140,
      66,    70,    66,    72,   145,    67,    68,    69,    61,   146,
     146,   145,   125,   126,   127,   128,   110,   111,   112,    88,
     133,     4,   135,   136,   137,   138,   139,   140,   141,   142,
      61,   145,    63,   146,     6,   145,   145,    57,   132,   147,
      43,   110,   111,   112,   138,   139,   140,   145,   145,   145,
     145,   145,    61,   146,    66,   146,   109,    44,    45,    46,
      47,    48,    49,   132,    51,    52,     3,   149,   145,   138,
     139,   140,   125,   126,   127,   128,   145,   130,   109,   106,
     133,     3,   135,   136,   137,   138,   139,   140,   141,   142,
     133,   145,   149,   145,   125,   126,   127,   128,   129,   146,
     109,    64,   133,   145,   135,   136,   137,   138,   139,   140,
     141,   142,    40,   130,   108,   145,   125,   126,   127,   128,
     109,   145,   145,   145,   133,     4,   135,   136,   137,   138,
     139,   140,   141,   142,   145,     3,   125,   126,   127,   128,
     109,   145,    45,   145,   133,   145,   135,   136,   137,   138,
     139,   140,   141,   142,   146,    45,     3,    41,   127,   128,
      42,   146,     6,     6,     6,     6,     6,   136,   137,   138,
     139,   140,   141,   142,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   108,   106,   146,   146,   145,    50,
     146,   146,   146,   146,     6,     6,     6,     6,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   149,     6,   149,
     149,   149,   149,   149,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   146,    57,    48,   146,   146,   146,
     146,   146,   348,   146,   364,   146,   146,   112,   146,   146,
     146,   146,   146,   146,   404,    48,    48,    48,   179,    48,
      11,    48,   131,   101,   372,   311,    96,   425,   146,   262,
     415,    -1,    83,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      -1,    -1,   206,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   217
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      66,   145,   151,   152,   153,   155,   164,   165,   167,   169,
     172,   173,   174,   175,   176,   197,   202,    25,    26,    27,
      37,    38,    29,    25,    26,    27,    37,    38,     3,   195,
      64,   195,    31,    32,    33,    34,    36,    39,   168,    28,
     195,     3,   198,   199,   175,   176,     0,   148,   224,    21,
      23,    24,   191,     8,   177,   179,    60,   223,   223,   223,
     223,   223,   195,    60,   222,   222,   222,   222,   222,   147,
      14,   195,    64,    65,    35,    35,   154,   155,   164,   165,
     169,   172,   173,   174,   202,    43,   149,   146,   146,   153,
      22,   145,   178,   179,    53,   186,    55,   180,    61,     3,
     195,   195,   195,     3,   145,   166,    62,     3,   195,   195,
     195,   195,     3,     3,   170,   171,    54,   188,     4,   221,
     221,   145,   199,   178,   179,     3,     4,     5,     6,    61,
      62,    70,    72,    88,   110,   111,   112,   132,   138,   139,
     140,   145,   203,   205,   206,   207,   208,   209,   210,   211,
     212,   214,   215,   216,   217,   218,    41,    56,   184,    62,
      43,   145,   166,    50,     3,   163,    30,   176,   135,   149,
     188,   206,    66,    66,   177,   146,   146,   145,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      61,    62,   207,   145,   145,    75,   206,   213,     4,     6,
     218,   145,   207,   207,   176,   206,    64,   149,   187,    43,
     130,   131,    61,    63,   109,   125,   126,   127,   128,   129,
     133,   135,   136,   137,   138,   139,   140,   141,   142,   147,
     181,   182,   206,   206,    57,   185,   174,     3,   105,   107,
     156,   157,   158,   162,    43,   195,   146,   149,   145,   206,
     171,   145,   145,   146,    53,   146,   203,   145,    61,   176,
     206,   206,   213,    71,    73,    75,   217,   146,   146,   145,
     192,   193,   194,   195,   200,   205,   188,     3,   206,   206,
      63,   133,   145,    61,   108,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,     3,
     140,   149,    58,    59,   183,   206,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   159,   106,   145,   146,   149,   174,
     145,     3,   204,   217,    67,    68,    69,   219,   220,   219,
     203,   146,   176,   146,    43,    74,    71,    73,   206,   206,
      64,   174,   149,    44,    46,    47,    48,    49,    51,    52,
     201,     3,    43,   196,    40,   190,   145,   207,   176,   203,
     108,   130,   182,   145,   145,   145,   145,   145,   145,   145,
      61,   105,   107,   108,   160,   161,   145,   163,   157,   163,
     146,   149,     4,     3,   146,   149,   146,   146,   146,   159,
     206,   206,    71,    74,   207,   146,   193,    45,    45,     3,
      41,    42,   189,   176,   203,   146,   146,   207,     6,     6,
       6,     6,     6,    77,    78,    79,    80,    84,    85,   104,
      77,    78,    79,    80,    84,    85,   104,   108,   106,   161,
     163,   146,   146,   217,   220,   146,    71,   206,   146,   196,
     194,   194,   145,   203,   206,   146,   146,   146,   146,   146,
     149,   146,   146,   149,   149,   149,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   149,   146,    50,   163,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   206,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   150,   151,   152,   152,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   154,   154,   154,   154,   154,   154,
     154,   154,   155,   155,   155,   155,   155,   155,   156,   156,
     157,   157,   158,   158,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   161,   161,
     161,   161,   162,   162,   163,   163,   164,   165,   165,   166,
     166,   167,   168,   168,   168,   168,   168,   168,   168,   169,
     170,   170,   171,   172,   172,   172,   172,   172,   173,   173,
     174,   174,   174,   174,   175,   175,   176,   177,   178,   178,
     179,   180,   180,   181,   181,   182,   183,   183,   183,   184,
     184,   185,   185,   186,   186,   187,   187,   188,   188,   189,
     189,   190,   190,   191,   191,   191,   191,   192,   192,   193,
     193,   194,   194,   195,   195,   196,   196,   196,   196,   197,
     197,   198,   198,   199,   200,   200,   201,   201,   201,   201,
     201,   201,   201,   202,   202,   203,   203,   204,   204,   205,
     205,   206,   206,   206,   206,   206,   207,   207,   207,   207,
     207,   207,   207,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   209,   209,   210,   211,
     211,   212,   212,   212,   212,   213,   213,   214,   215,   215,
     215,   215,   216,   216,   216,   216,   217,   217,   217,   217,
     217,   217,   217,   217,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   219,   219,   220,   220,
     220,   221,   222,   222,   223,   223,   224,   224
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
       0,     3,     1,     1,     2,     2,     1,     1,     0,     5,
       1,     3,     3,     4,     4,     4,     4,     4,     8,     8,
       1,     1,     3,     3,     3,     3,     2,     4,     3,     3,
       7,     3,     0,     1,     3,     2,     1,     1,     0,     2,
       0,     2,     0,     1,     0,     2,     0,     2,     0,     2,
       0,     3,     0,     1,     2,     1,     1,     1,     3,     1,
       1,     2,     4,     1,     3,     2,     1,     5,     0,     2,
       0,     1,     3,     5,     4,     6,     1,     1,     1,     1,
       1,     1,     0,     2,     2,     1,     3,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     3,     4,     5,     4,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     6,     3,     4,     3,     3,     5,     5,
       6,     4,     6,     3,     5,     4,     5,     6,     4,     5,
       5,     6,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     2,     2,
       1,     1,     2,     0,     3,     0,     1,     0
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
#line 1832 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 224 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1840 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 1854 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 1868 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 217 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1879 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 228 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1888 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 228 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1897 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 1911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 181 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 1922 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1932 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1942 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1952 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1962 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1972 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1982 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 1996 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2010 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 247 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2020 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2028 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2036 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2045 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2053 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2061 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2075 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2084 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2093 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2102 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2115 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 237 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2124 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2138 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2152 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 253 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2162 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2171 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2185 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2199 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2207 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2215 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2223 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2231 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2239 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2247 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2255 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2263 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2276 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2284 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2292 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2300 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2308 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 233 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2316 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 224 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2324 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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

#line 2432 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
#line 2647 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 373 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new Vector<BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2658 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 379 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2669 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 386 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2675 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 387 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2681 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 388 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2687 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 389 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2693 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 390 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2699 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 391 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2705 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 392 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2711 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 393 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2717 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 394 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2723 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 14: /* explainable_statement: create_statement  */
#line 396 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2729 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 15: /* explainable_statement: drop_statement  */
#line 397 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2735 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 16: /* explainable_statement: copy_statement  */
#line 398 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2741 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 17: /* explainable_statement: show_statement  */
#line 399 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2747 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 18: /* explainable_statement: select_statement  */
#line 400 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2753 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 19: /* explainable_statement: delete_statement  */
#line 401 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2759 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 20: /* explainable_statement: update_statement  */
#line 402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2765 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 21: /* explainable_statement: insert_statement  */
#line 403 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2771 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2787 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2805 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2833 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2853 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2874 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2898 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 28: /* table_element_array: table_element  */
#line 515 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new Vector<TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 2907 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 29: /* table_element_array: table_element_array ',' table_element  */
#line 519 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 2916 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 30: /* table_element: table_column  */
#line 525 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 2924 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 31: /* table_element: table_constraint  */
#line 528 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 2932 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 2976 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 3022 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 34: /* column_type: BOOLEAN  */
#line 616 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kBoolean}; }
#line 3028 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* column_type: TINYINT  */
#line 617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kTinyInt}; }
#line 3034 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: SMALLINT  */
#line 618 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kSmallInt}; }
#line 3040 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: INTEGER  */
#line 619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kInteger}; }
#line 3046 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: BIGINT  */
#line 620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kBigInt}; }
#line 3052 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: HUGEINT  */
#line 621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kHugeInt}; }
#line 3058 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: FLOAT  */
#line 622 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3064 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: REAL  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 3070 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: DOUBLE  */
#line 624 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kDouble}; }
#line 3076 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: DATE  */
#line 625 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kDate}; }
#line 3082 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: TIME  */
#line 626 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kTime}; }
#line 3088 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DATETIME  */
#line 627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kDateTime}; }
#line 3094 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: TIMESTAMP  */
#line 628 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = ColumnType{LogicalType::kTimestamp}; }
#line 3100 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: UUID  */
#line 629 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kUuid}; }
#line 3106 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: POINT  */
#line 630 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kPoint}; }
#line 3112 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: LINE  */
#line 631 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLine}; }
#line 3118 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: LSEG  */
#line 632 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLineSeg}; }
#line 3124 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: BOX  */
#line 633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = ColumnType{LogicalType::kBox}; }
#line 3130 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: PATH  */
#line 634 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kPath}; }
#line 3136 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: POLYGON  */
#line 635 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kPolygon}; }
#line 3142 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: CIRCLE  */
#line 636 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kCircle}; }
#line 3148 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 638 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kChar, (yyvsp[-1].long_value)}; }
#line 3154 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 639 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3160 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3166 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 641 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-1].long_value), 0}; }
#line 3172 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: DECIMAL  */
#line 642 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
#line 3178 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 643 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3184 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = ColumnType{LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3190 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 645 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3196 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 646 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3202 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 647 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3208 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 648 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3214 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 649 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3220 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 650 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3226 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 651 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3232 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 652 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3238 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 653 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3244 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 654 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3250 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 655 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3256 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 656 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3262 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 657 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3268 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 658 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3274 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_constraints: column_constraint  */
#line 677 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new HashSet<ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3283 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 3297 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_constraint: PRIMARY KEY  */
#line 691 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = ConstraintType::kPrimaryKey;
}
#line 3305 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_constraint: UNIQUE  */
#line 694 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = ConstraintType::kUnique;
}
#line 3313 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_constraint: NULLABLE  */
#line 697 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = ConstraintType::kNull;
}
#line 3321 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_constraint: NOT NULLABLE  */
#line 700 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = ConstraintType::kNotNull;
}
#line 3329 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 704 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kPrimaryKey;
}
#line 3339 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 709 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kUnique;
}
#line 3349 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* identifier_array: IDENTIFIER  */
#line 716 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new Vector<String>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3360 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 722 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3371 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 3388 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 3405 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 3422 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* optional_identifier_array: '(' identifier_array ')'  */
#line 773 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3430 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* optional_identifier_array: %empty  */
#line 776 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3438 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 783 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyval.explain_stmt) = new ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3448 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* explain_type: ANALYZE  */
#line 789 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    (yyval.explain_type_t) = ExplainType::kAnalyze;
}
#line 3456 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* explain_type: AST  */
#line 792 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    (yyval.explain_type_t) = ExplainType::kAst;
}
#line 3464 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* explain_type: UNOPT LOGICAL  */
#line 795 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.explain_type_t) = ExplainType::kUnOpt;
}
#line 3472 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* explain_type: OPT LOGICAL  */
#line 798 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    (yyval.explain_type_t) = ExplainType::kOpt;
}
#line 3480 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* explain_type: PHYSICAL  */
#line 801 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPhysical;
}
#line 3488 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* explain_type: PIPELINE  */
#line 804 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPipeline;
}
#line 3496 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* explain_type: %empty  */
#line 807 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.explain_type_t) = ExplainType::kPhysical;
}
#line 3504 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 814 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3521 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* update_expr_array: update_expr  */
#line 827 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.update_expr_array_t) = new Vector<UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3530 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* update_expr_array: update_expr_array ',' update_expr  */
#line 831 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3539 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* update_expr: IDENTIFIER '=' expr  */
#line 836 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.update_expr_t) = new UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3551 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 849 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = std::move(drop_schema_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3567 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 104: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 862 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3585 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 105: /* drop_statement: DROP TABLE if_exists table_name  */
#line 877 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3603 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* drop_statement: DROP VIEW if_exists table_name  */
#line 892 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3621 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* drop_statement: DROP INDEX if_exists table_name  */
#line 907 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3640 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 926 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3686 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 967 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3732 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* select_statement: select_without_paren  */
#line 1012 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3740 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* select_statement: select_with_paren  */
#line 1015 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3748 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1018 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                     {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3762 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1027 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3776 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* select_with_paren: '(' select_without_paren ')'  */
#line 1037 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3784 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* select_with_paren: '(' select_with_paren ')'  */
#line 1040 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3792 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1044 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3801 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1049 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 3812 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1056 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3820 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1059 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3828 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 1064 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3847 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1079 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3855 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* order_by_clause: %empty  */
#line 1082 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3863 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* order_by_expr_list: order_by_expr  */
#line 1086 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new Vector<OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 3872 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1090 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 3881 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* order_by_expr: expr order_by_type  */
#line 1095 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 3891 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* order_by_type: ASC  */
#line 1101 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = kAsc;
}
#line 3899 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* order_by_type: DESC  */
#line 1104 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = kDesc;
}
#line 3907 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* order_by_type: %empty  */
#line 1107 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.order_by_type_t) = kAsc;
}
#line 3915 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* limit_expr: LIMIT expr  */
#line 1111 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3923 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* limit_expr: %empty  */
#line 1115 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3929 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* offset_expr: OFFSET expr  */
#line 1117 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3937 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* offset_expr: %empty  */
#line 1121 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3943 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 133: /* distinct: DISTINCT  */
#line 1123 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3951 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 134: /* distinct: %empty  */
#line 1126 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3959 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 135: /* from_clause: FROM table_reference  */
#line 1130 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3967 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* from_clause: %empty  */
#line 1133 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3975 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* where_clause: WHERE expr  */
#line 1137 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3983 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* where_clause: %empty  */
#line 1140 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3991 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 139: /* having_clause: HAVING expr  */
#line 1144 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3999 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* having_clause: %empty  */
#line 1147 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4007 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* group_by_clause: GROUP BY expr_array  */
#line 1151 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4015 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* group_by_clause: %empty  */
#line 1154 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4023 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* set_operator: UNION  */
#line 1158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = SetOperatorType::kUnion;
}
#line 4031 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 144: /* set_operator: UNION ALL  */
#line 1161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kUnionAll;
}
#line 4039 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 145: /* set_operator: INTERSECT  */
#line 1164 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kIntersect;
}
#line 4047 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 146: /* set_operator: EXCEPT  */
#line 1167 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = SetOperatorType::kExcept;
}
#line 4055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 147: /* table_reference: table_reference_unit  */
#line 1175 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4063 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 148: /* table_reference: table_reference ',' table_reference_unit  */
#line 1178 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4081 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* table_reference_name: table_name table_alias  */
#line 1195 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4099 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1209 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    SubqueryReference* subquery_reference = new SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4110 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* table_name: IDENTIFIER  */
#line 1218 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4122 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1225 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        ParserHelper::ToLower((yyvsp[-2].str_value));
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4136 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* table_alias: AS IDENTIFIER  */
#line 1236 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4146 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 156: /* table_alias: IDENTIFIER  */
#line 1241 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4156 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 157: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1246 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4167 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* table_alias: %empty  */
#line 1252 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4175 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* with_clause: WITH with_expr_list  */
#line 1259 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4183 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* with_clause: %empty  */
#line 1262 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4191 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* with_expr_list: with_expr  */
#line 1266 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new Vector<WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1269 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4209 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1274 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                             {
    (yyval.with_expr_t) = new WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1286 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1293 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                   {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4246 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* join_type: INNER  */
#line 1307 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = JoinType::kInner;
}
#line 4254 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* join_type: LEFT  */
#line 1310 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kLeft;
}
#line 4262 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* join_type: RIGHT  */
#line 1313 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kRight;
}
#line 4270 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* join_type: OUTER  */
#line 1316 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4278 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* join_type: FULL  */
#line 1319 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4286 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* join_type: CROSS  */
#line 1322 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kCross;
}
#line 4294 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* join_type: %empty  */
#line 1325 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 4301 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* show_statement: SHOW TABLES  */
#line 1331 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kTables;
}
#line 4310 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* show_statement: DESCRIBE table_name  */
#line 1335 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4326 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 175: /* expr_array: expr_alias  */
#line 1351 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4335 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 176: /* expr_array: expr_array ',' expr_alias  */
#line 1355 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4344 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 177: /* constant_expr_array: constant_expr  */
#line 1360 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4353 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 178: /* constant_expr_array: constant_expr_array ',' constant_expr  */
#line 1364 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4362 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 179: /* expr_alias: expr AS IDENTIFIER  */
#line 1369 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4373 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 180: /* expr_alias: expr  */
#line 1375 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4381 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 186: /* operand: '(' expr ')'  */
#line 1385 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4389 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 187: /* operand: '(' select_without_paren ')'  */
#line 1388 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4400 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 193: /* function_expr: IDENTIFIER '(' ')'  */
#line 1400 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 4413 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 194: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1408 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 4426 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 195: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1416 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    FunctionExpr* func_expr = new FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 4440 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 196: /* function_expr: operand IS NOT NULLABLE  */
#line 1425 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4452 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 197: /* function_expr: operand IS NULLABLE  */
#line 1432 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4464 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 198: /* function_expr: NOT operand  */
#line 1439 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "not";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4476 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 199: /* function_expr: '-' operand  */
#line 1446 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "-";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4488 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 200: /* function_expr: '+' operand  */
#line 1453 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "+";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4500 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 201: /* function_expr: operand '-' operand  */
#line 1460 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4513 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: operand '+' operand  */
#line 1468 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4526 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: operand '*' operand  */
#line 1476 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4539 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 204: /* function_expr: operand '/' operand  */
#line 1484 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4552 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 205: /* function_expr: operand '%' operand  */
#line 1492 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4565 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 206: /* function_expr: operand '=' operand  */
#line 1500 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4578 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 207: /* function_expr: operand EQUAL operand  */
#line 1508 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4591 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 208: /* function_expr: operand NOT_EQ operand  */
#line 1516 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4604 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 209: /* function_expr: operand '<' operand  */
#line 1524 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4617 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 210: /* function_expr: operand '>' operand  */
#line 1532 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4630 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 211: /* function_expr: operand LESS_EQ operand  */
#line 1540 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4643 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 212: /* function_expr: operand GREATER_EQ operand  */
#line 1548 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4656 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 213: /* function_expr: EXTRACT '(' constant_expr FROM operand ')'  */
#line 1556 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "extract";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4669 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 214: /* function_expr: operand LIKE operand  */
#line 1564 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4682 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 215: /* function_expr: operand NOT LIKE operand  */
#line 1572 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4695 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 216: /* conjunction_expr: expr AND expr  */
#line 1581 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4708 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 217: /* conjunction_expr: expr OR expr  */
#line 1589 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4721 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 218: /* between_expr: operand BETWEEN operand AND operand  */
#line 1598 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                  {
    BetweenExpr* between_expr = new BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 4733 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 219: /* in_expr: operand IN '(' expr_array ')'  */
#line 1606 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4745 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 220: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1613 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "not_in";
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->arguments_->emplace_back((yyvsp[-5].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4757 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 221: /* case_expr: CASE expr case_check_array END  */
#line 1621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4768 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 222: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 1627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4780 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 223: /* case_expr: CASE case_check_array END  */
#line 1634 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 4790 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 224: /* case_expr: CASE case_check_array ELSE expr END  */
#line 1639 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    CaseExpr* case_expr = new CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 4801 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 225: /* case_check_array: WHEN expr THEN expr  */
#line 1646 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      {
    (yyval.case_check_array_t) = new Vector<WhenThen*>();
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 4813 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 226: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 1653 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    WhenThen* when_then_ptr = new WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 4825 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 227: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 1661 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4862 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 228: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1694 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                   {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4873 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 229: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1700 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4884 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 230: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1706 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4896 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 231: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1713 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4908 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 232: /* column_expr: IDENTIFIER  */
#line 1721 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    ColumnExpr* column_expr = new ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4920 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 233: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1728 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4932 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 234: /* column_expr: '*'  */
#line 1735 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4942 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 235: /* column_expr: column_expr '.' '*'  */
#line 1740 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4956 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 236: /* constant_expr: STRING  */
#line 1750 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4966 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 237: /* constant_expr: TRUE  */
#line 1755 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
#line 4976 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 238: /* constant_expr: FALSE  */
#line 1760 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
#line 4986 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 239: /* constant_expr: DOUBLE_VALUE  */
#line 1765 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
#line 4996 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 240: /* constant_expr: LONG_VALUE  */
#line 1770 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5006 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 241: /* constant_expr: DATE STRING  */
#line 1775 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 5016 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 242: /* constant_expr: INTERVAL interval_expr  */
#line 1780 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5024 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 243: /* constant_expr: interval_expr  */
#line 1783 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5032 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 244: /* interval_expr: LONG_VALUE SECONDS  */
#line 1787 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5043 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 245: /* interval_expr: LONG_VALUE SECOND  */
#line 1793 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5054 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 246: /* interval_expr: LONG_VALUE MINUTES  */
#line 1799 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5065 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 247: /* interval_expr: LONG_VALUE MINUTE  */
#line 1805 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5076 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 248: /* interval_expr: LONG_VALUE HOURS  */
#line 1811 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5087 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 249: /* interval_expr: LONG_VALUE HOUR  */
#line 1817 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5098 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 250: /* interval_expr: LONG_VALUE DAYS  */
#line 1823 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5109 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 251: /* interval_expr: LONG_VALUE DAY  */
#line 1829 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5120 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 252: /* interval_expr: LONG_VALUE MONTHS  */
#line 1835 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5131 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 253: /* interval_expr: LONG_VALUE MONTH  */
#line 1841 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5142 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 254: /* interval_expr: LONG_VALUE YEARS  */
#line 1847 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5153 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 255: /* interval_expr: LONG_VALUE YEAR  */
#line 1853 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 5164 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 256: /* copy_option_list: copy_option  */
#line 1864 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.copy_option_array) = new Vector<CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 5173 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 257: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1868 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 5182 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 258: /* copy_option: FORMAT IDENTIFIER  */
#line 1873 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 5200 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 259: /* copy_option: DELIMITER STRING  */
#line 1886 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 5211 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 260: /* copy_option: HEADER  */
#line 1892 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 5221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 261: /* file_path: STRING  */
#line 1898 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5229 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 262: /* if_exists: IF EXISTS  */
#line 1902 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 5235 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 263: /* if_exists: %empty  */
#line 1903 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5241 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 264: /* if_not_exists: IF NOT EXISTS  */
#line 1905 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 5247 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 265: /* if_not_exists: %empty  */
#line 1906 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 5253 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 5257 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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

#line 1912 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

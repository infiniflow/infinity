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
  YYSYMBOL_BOOLEAN = 69,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 70,                   /* INTEGER  */
  YYSYMBOL_TINYINT = 71,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 72,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 73,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 74,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 75,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 76,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 77,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 78,                    /* DOUBLE  */
  YYSYMBOL_REAL = 79,                      /* REAL  */
  YYSYMBOL_DECIMAL = 80,                   /* DECIMAL  */
  YYSYMBOL_DATE = 81,                      /* DATE  */
  YYSYMBOL_TIME = 82,                      /* TIME  */
  YYSYMBOL_DATETIME = 83,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 84,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 85,                      /* UUID  */
  YYSYMBOL_POINT = 86,                     /* POINT  */
  YYSYMBOL_LINE = 87,                      /* LINE  */
  YYSYMBOL_LSEG = 88,                      /* LSEG  */
  YYSYMBOL_BOX = 89,                       /* BOX  */
  YYSYMBOL_PATH = 90,                      /* PATH  */
  YYSYMBOL_POLYGON = 91,                   /* POLYGON  */
  YYSYMBOL_CIRCLE = 92,                    /* CIRCLE  */
  YYSYMBOL_BLOB = 93,                      /* BLOB  */
  YYSYMBOL_BITMAP = 94,                    /* BITMAP  */
  YYSYMBOL_EMBEDDING = 95,                 /* EMBEDDING  */
  YYSYMBOL_VECTOR = 96,                    /* VECTOR  */
  YYSYMBOL_BIT = 97,                       /* BIT  */
  YYSYMBOL_PRIMARY = 98,                   /* PRIMARY  */
  YYSYMBOL_KEY = 99,                       /* KEY  */
  YYSYMBOL_UNIQUE = 100,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 101,                 /* NULLABLE  */
  YYSYMBOL_IS = 102,                       /* IS  */
  YYSYMBOL_TRUE = 103,                     /* TRUE  */
  YYSYMBOL_FALSE = 104,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 105,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 106,                   /* SECOND  */
  YYSYMBOL_SECONDS = 107,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 108,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 109,                  /* MINUTES  */
  YYSYMBOL_HOUR = 110,                     /* HOUR  */
  YYSYMBOL_HOURS = 111,                    /* HOURS  */
  YYSYMBOL_DAY = 112,                      /* DAY  */
  YYSYMBOL_DAYS = 113,                     /* DAYS  */
  YYSYMBOL_MONTH = 114,                    /* MONTH  */
  YYSYMBOL_MONTHS = 115,                   /* MONTHS  */
  YYSYMBOL_YEAR = 116,                     /* YEAR  */
  YYSYMBOL_YEARS = 117,                    /* YEARS  */
  YYSYMBOL_EQUAL = 118,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 119,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 120,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 121,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 122,                  /* BETWEEN  */
  YYSYMBOL_AND = 123,                      /* AND  */
  YYSYMBOL_OR = 124,                       /* OR  */
  YYSYMBOL_NUMBER = 125,                   /* NUMBER  */
  YYSYMBOL_CASE = 126,                     /* CASE  */
  YYSYMBOL_WHEN = 127,                     /* WHEN  */
  YYSYMBOL_THEN = 128,                     /* THEN  */
  YYSYMBOL_ELSE = 129,                     /* ELSE  */
  YYSYMBOL_130_ = 130,                     /* '='  */
  YYSYMBOL_131_ = 131,                     /* '<'  */
  YYSYMBOL_132_ = 132,                     /* '>'  */
  YYSYMBOL_133_ = 133,                     /* '+'  */
  YYSYMBOL_134_ = 134,                     /* '-'  */
  YYSYMBOL_135_ = 135,                     /* '*'  */
  YYSYMBOL_136_ = 136,                     /* '/'  */
  YYSYMBOL_137_ = 137,                     /* '%'  */
  YYSYMBOL_138_ = 138,                     /* '['  */
  YYSYMBOL_139_ = 139,                     /* ']'  */
  YYSYMBOL_140_ = 140,                     /* '('  */
  YYSYMBOL_141_ = 141,                     /* ')'  */
  YYSYMBOL_142_ = 142,                     /* '.'  */
  YYSYMBOL_143_ = 143,                     /* ';'  */
  YYSYMBOL_144_ = 144,                     /* ','  */
  YYSYMBOL_YYACCEPT = 145,                 /* $accept  */
  YYSYMBOL_input_pattern = 146,            /* input_pattern  */
  YYSYMBOL_statement_list = 147,           /* statement_list  */
  YYSYMBOL_statement = 148,                /* statement  */
  YYSYMBOL_explainable_statement = 149,    /* explainable_statement  */
  YYSYMBOL_create_statement = 150,         /* create_statement  */
  YYSYMBOL_table_element_array = 151,      /* table_element_array  */
  YYSYMBOL_table_element = 152,            /* table_element  */
  YYSYMBOL_table_column = 153,             /* table_column  */
  YYSYMBOL_column_type = 154,              /* column_type  */
  YYSYMBOL_column_constraints = 155,       /* column_constraints  */
  YYSYMBOL_column_constraint = 156,        /* column_constraint  */
  YYSYMBOL_table_constraint = 157,         /* table_constraint  */
  YYSYMBOL_identifier_array = 158,         /* identifier_array  */
  YYSYMBOL_delete_statement = 159,         /* delete_statement  */
  YYSYMBOL_insert_statement = 160,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 161, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 162,        /* explain_statement  */
  YYSYMBOL_explain_type = 163,             /* explain_type  */
  YYSYMBOL_update_statement = 164,         /* update_statement  */
  YYSYMBOL_update_expr_array = 165,        /* update_expr_array  */
  YYSYMBOL_update_expr = 166,              /* update_expr  */
  YYSYMBOL_drop_statement = 167,           /* drop_statement  */
  YYSYMBOL_copy_statement = 168,           /* copy_statement  */
  YYSYMBOL_select_statement = 169,         /* select_statement  */
  YYSYMBOL_select_with_paren = 170,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 171,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 172, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 173, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 174, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 175,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 176,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 177,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 178,            /* order_by_type  */
  YYSYMBOL_limit_expr = 179,               /* limit_expr  */
  YYSYMBOL_offset_expr = 180,              /* offset_expr  */
  YYSYMBOL_distinct = 181,                 /* distinct  */
  YYSYMBOL_from_clause = 182,              /* from_clause  */
  YYSYMBOL_where_clause = 183,             /* where_clause  */
  YYSYMBOL_having_clause = 184,            /* having_clause  */
  YYSYMBOL_group_by_clause = 185,          /* group_by_clause  */
  YYSYMBOL_set_operator = 186,             /* set_operator  */
  YYSYMBOL_table_reference = 187,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 188,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 189,     /* table_reference_name  */
  YYSYMBOL_table_name = 190,               /* table_name  */
  YYSYMBOL_table_alias = 191,              /* table_alias  */
  YYSYMBOL_with_clause = 192,              /* with_clause  */
  YYSYMBOL_with_expr_list = 193,           /* with_expr_list  */
  YYSYMBOL_with_expr = 194,                /* with_expr  */
  YYSYMBOL_join_clause = 195,              /* join_clause  */
  YYSYMBOL_join_type = 196,                /* join_type  */
  YYSYMBOL_show_statement = 197,           /* show_statement  */
  YYSYMBOL_expr_array = 198,               /* expr_array  */
  YYSYMBOL_constant_expr_array = 199,      /* constant_expr_array  */
  YYSYMBOL_expr_alias = 200,               /* expr_alias  */
  YYSYMBOL_expr = 201,                     /* expr  */
  YYSYMBOL_operand = 202,                  /* operand  */
  YYSYMBOL_function_expr = 203,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 204,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 205,             /* between_expr  */
  YYSYMBOL_in_expr = 206,                  /* in_expr  */
  YYSYMBOL_subquery_expr = 207,            /* subquery_expr  */
  YYSYMBOL_column_expr = 208,              /* column_expr  */
  YYSYMBOL_constant_expr = 209,            /* constant_expr  */
  YYSYMBOL_interval_expr = 210,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 211,         /* copy_option_list  */
  YYSYMBOL_copy_option = 212,              /* copy_option  */
  YYSYMBOL_file_path = 213,                /* file_path  */
  YYSYMBOL_if_exists = 214,                /* if_exists  */
  YYSYMBOL_if_not_exists = 215,            /* if_not_exists  */
  YYSYMBOL_semicolon = 216                 /* semicolon  */
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
#define YYFINAL  52
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   560

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  145
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  72
/* YYNRULES -- Number of rules.  */
#define YYNRULES  247
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  479

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   384


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
       2,     2,     2,     2,     2,     2,     2,   137,     2,     2,
     140,   141,   135,   133,   144,   134,   142,   136,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   143,
     131,   130,   132,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   138,     2,   139,     2,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   349,   349,   353,   359,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   376,   377,   378,   379,   380,   381,
     382,   383,   390,   400,   415,   439,   455,   471,   489,   493,
     499,   502,   508,   546,   587,   588,   589,   590,   591,   592,
     593,   594,   595,   596,   597,   598,   599,   600,   601,   602,
     603,   604,   605,   606,   607,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,   626,   627,   628,   629,   648,   652,   662,   665,
     668,   671,   675,   680,   687,   692,   701,   715,   727,   739,
     742,   749,   755,   758,   761,   764,   767,   774,   786,   790,
     795,   806,   816,   834,   875,   920,   923,   926,   935,   945,
     948,   952,   957,   964,   967,   972,   987,   990,   994,   998,
    1003,  1009,  1012,  1015,  1019,  1023,  1025,  1029,  1031,  1034,
    1038,  1041,  1045,  1048,  1052,  1055,  1059,  1062,  1066,  1069,
    1072,  1075,  1083,  1086,  1095,  1095,  1097,  1111,  1120,  1126,
    1135,  1139,  1144,  1151,  1154,  1158,  1161,  1166,  1177,  1184,
    1198,  1201,  1204,  1207,  1210,  1213,  1216,  1222,  1226,  1242,
    1246,  1251,  1255,  1260,  1265,  1269,  1270,  1271,  1272,  1273,
    1275,  1278,  1284,  1285,  1286,  1288,  1295,  1302,  1310,  1317,
    1324,  1331,  1338,  1346,  1354,  1362,  1370,  1378,  1386,  1394,
    1402,  1410,  1418,  1426,  1435,  1443,  1452,  1462,  1469,  1477,
    1483,  1489,  1496,  1504,  1509,  1514,  1519,  1529,  1534,  1539,
    1544,  1549,  1554,  1559,  1563,  1569,  1575,  1581,  1587,  1593,
    1599,  1605,  1611,  1617,  1623,  1629,  1640,  1644,  1649,  1662,
    1668,  1674,  1678,  1679,  1681,  1682,  1684,  1685
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
  "DELIMITER", "FORMAT", "HEADER", "BOOLEAN", "INTEGER", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "CHAR", "VARCHAR", "FLOAT", "DOUBLE",
  "REAL", "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS",
  "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS",
  "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN",
  "AND", "OR", "NUMBER", "CASE", "WHEN", "THEN", "ELSE", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'",
  "'.'", "';'", "','", "$accept", "input_pattern", "statement_list",
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
  "with_expr", "join_clause", "join_type", "show_statement", "expr_array",
  "constant_expr_array", "expr_alias", "expr", "operand", "function_expr",
  "conjunction_expr", "between_expr", "in_expr", "subquery_expr",
  "column_expr", "constant_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-264)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-247)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     263,   220,    -6,    62,    29,   -23,    29,   181,    24,    29,
      54,   -52,    74,   -40,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,    97,  -264,  -264,    93,  -264,    46,    46,    46,
      46,    46,    29,    57,    57,   -20,   114,    29,   -17,  -264,
    -264,    95,   102,  -264,   186,  -264,  -264,   103,    -5,  -264,
       9,    11,  -264,    18,  -264,   137,  -264,  -264,    -2,   109,
    -264,   128,   132,   187,    29,    29,    29,   191,    65,   150,
     225,    29,   232,   233,   197,   257,   257,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,    97,  -264,   142,    54,
    -264,  -264,  -264,  -264,    -2,  -264,  -264,  -264,   120,   245,
     231,   234,  -264,   -28,  -264,    65,   247,   294,    21,  -264,
    -264,  -264,  -264,   168,   -38,  -264,   120,  -264,  -264,   242,
     244,    93,  -264,   167,   170,   172,  -264,  -264,  -264,   249,
     177,   315,  -264,  -264,   316,   129,   129,  -264,   104,   -42,
    -264,   -32,   169,  -264,  -264,  -264,  -264,  -264,   182,  -264,
     120,   120,   267,  -264,   -52,    17,   283,    29,  -264,   -59,
     189,  -264,   120,   233,  -264,   -10,   190,   192,   200,  -264,
    -264,    38,   202,   262,  -264,   346,  -264,   130,   130,   203,
      25,     4,   120,   197,   340,   120,   120,   297,   205,   -25,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,     5,   217,  -264,    23,   -10,   120,  -264,
      97,   345,   261,   222,   -48,  -264,  -264,  -264,   -52,   223,
    -264,   361,    50,   -10,  -264,   226,   226,  -264,   120,  -264,
      16,   262,   224,  -264,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,  -264,   -52,   235,   427,
    -264,   338,  -264,  -264,   342,  -264,  -264,   259,   254,   104,
     282,  -264,   236,   236,   276,   276,   216,   236,   276,   276,
     130,   130,  -264,  -264,  -264,  -264,  -264,   120,  -264,  -264,
    -264,   -10,  -264,  -264,  -264,  -264,  -264,  -264,   255,   258,
    -264,  -264,  -264,   260,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,   264,   265,   266,   306,
      76,   307,   294,  -264,    17,    97,   294,  -264,    42,  -264,
     380,   382,  -264,    43,  -264,    47,    59,  -264,   310,  -264,
     -12,     4,   355,  -264,  -264,  -264,  -264,  -264,  -264,   357,
     399,  -264,   408,   423,   104,   324,    77,  -264,   129,  -264,
     460,   461,   462,   470,   473,   243,   372,   379,   383,  -264,
    -264,    76,  -264,   294,    78,  -264,    86,  -264,    50,  -264,
    -264,  -264,   226,  -264,  -264,  -264,   338,   427,     4,     4,
     341,   120,   120,  -264,   343,   108,  -264,  -264,   256,   344,
     347,   115,   348,   349,   339,   350,   351,   352,   353,   354,
     356,   358,   359,   360,   362,   363,   364,   365,  -264,  -264,
    -264,   136,  -264,  -264,  -264,  -264,  -264,  -264,   437,   294,
     366,   -10,  -264,  -264,  -264,  -264,  -264,   481,  -264,  -264,
     485,   486,   487,   493,   495,   499,   505,   506,   507,   508,
     509,   510,   511,   512,  -264,   120,   140,   378,   381,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   -10,  -264,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     154,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   154,     0,   247,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   106,   105,     0,     8,   245,   245,   245,
     245,   245,     0,   243,   243,   148,     0,     0,     0,    92,
      96,     0,     0,    95,   154,   167,   168,     0,   153,   155,
       0,     0,     1,   154,     2,   138,   140,   141,     0,   129,
     111,   117,     0,     0,     0,     0,     0,     0,    90,     0,
       0,     0,     0,     0,   133,     0,     0,    93,    94,    91,
      14,    19,    21,    20,    15,    16,    18,    17,     0,     0,
     110,   109,     4,   139,     0,   107,   108,   128,     0,     0,
     125,     0,    22,     0,    23,    90,     0,     0,   154,   242,
     101,   102,   149,     0,   133,    98,     0,    86,   241,     0,
       0,     0,   156,     0,     0,   213,   217,   220,   221,     0,
       0,     0,   218,   219,     0,     0,     0,   215,   154,   131,
     169,   174,   175,   184,   179,   178,   177,   176,   183,   182,
       0,     0,   127,   244,   154,     0,     0,     0,    84,     0,
       0,    88,     0,     0,    97,   132,     0,     0,     0,   114,
     113,     0,     0,   154,   222,     0,   223,   191,   190,     0,
       0,     0,     0,   133,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,   118,   123,   124,     0,   112,
      25,     0,     0,     0,     0,    28,    30,    31,   154,     0,
      89,     0,     0,   100,    99,     0,     0,   157,     0,   185,
       0,   154,     0,   225,   224,   227,   226,   229,   228,   231,
     230,   233,   232,   235,   234,   181,   180,   154,   130,   142,
     144,   152,   145,   170,   137,   173,   204,   205,     0,   154,
       0,   189,   198,   199,   202,   203,     0,   197,   200,   201,
     193,   192,   194,   195,   196,   214,   216,     0,   121,   122,
     120,   126,    34,    37,    35,    36,    38,    39,     0,     0,
      40,    42,    41,    59,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,     0,     0,     0,     0,
      32,     0,     0,    24,     0,    26,     0,    85,     0,   171,
       0,     0,   240,     0,   236,     0,     0,   186,     0,   209,
       0,     0,     0,   161,   162,   163,   164,   160,   165,     0,
       0,   146,     0,   135,   154,     0,     0,   188,     0,   119,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    79,
      80,    33,    76,     0,     0,    29,     0,    87,     0,   239,
     238,   104,     0,   103,   187,   210,   152,   143,     0,     0,
     150,     0,     0,   115,     0,     0,   211,   207,   206,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    81,    78,
      77,     0,    83,    27,   172,   237,   147,   158,     0,     0,
     136,   134,   212,   208,    55,    56,    58,     0,    60,    61,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    82,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   159,   151,    57,    65,    63,    64,    66,    67,
      68,    62,    72,    70,    71,    73,    74,    75,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -264,  -264,  -264,   467,  -264,   477,  -264,   209,  -264,  -264,
    -264,   163,  -264,  -263,   494,   496,   434,  -264,  -264,   497,
    -264,   397,   498,   500,   -43,   532,   -11,   424,   452,   -19,
    -264,  -264,   270,  -264,  -264,  -264,  -264,  -264,   -88,  -264,
    -264,  -264,  -264,   218,  -211,    13,   174,  -264,  -264,   459,
    -264,  -264,   513,  -155,  -264,   369,  -114,  -132,  -264,  -264,
    -264,  -264,  -264,  -264,  -217,  -264,   326,   183,   478,   519,
     212,  -264
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    12,    13,    14,    79,    15,   214,   215,   216,   310,
     361,   362,   217,   159,    16,    17,   108,    18,    44,    19,
     114,   115,    20,    21,    22,    23,    24,    60,    95,    61,
     100,   204,   205,   280,   152,   209,    98,   183,   117,   383,
     343,    58,   248,   249,   250,   251,   341,    25,    48,    49,
     252,   339,    26,   139,   318,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   176,   323,   324,   119,    70,
      63,    54
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      51,    86,   165,   177,   178,   319,    59,    35,   275,    55,
     184,    56,    57,    10,   154,   116,   230,    36,  -246,    38,
     211,   181,    46,    32,   180,     1,   164,     2,     3,     4,
       5,     6,    35,     7,     8,   260,   206,   207,     9,    96,
      37,   125,   126,   127,   128,    68,    75,    76,   223,   364,
      74,   160,    45,   366,   126,   127,   128,    47,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   256,   257,   326,    52,   124,   261,   103,   104,   105,
     278,   279,   220,    10,   111,   221,    10,    33,    11,    34,
     228,   185,   186,   313,   281,   254,   314,   161,   129,   130,
     411,    59,   182,    53,   346,    62,   163,   125,   126,   127,
     128,   210,   155,   185,   186,   212,    69,   213,    55,   131,
      56,    57,    72,   125,   126,   127,   128,   179,    73,   376,
      77,   131,   125,   126,   127,   128,   357,    78,    94,    89,
     276,   132,   133,   134,   247,    88,   185,   186,   185,   186,
      90,   414,    91,   132,   133,   134,   446,   327,    11,    93,
     182,    97,   232,   206,   129,   130,   246,   417,   418,    10,
     219,   135,   136,   137,   358,   315,   359,   360,   138,   229,
     129,   130,    99,   367,   371,   131,   368,   372,   373,   385,
     102,   372,   101,     1,   106,     2,     3,     4,     5,     6,
     374,   131,     8,   182,   330,   107,     9,   132,   133,   134,
     131,   109,    39,    40,    41,    42,   388,    43,   387,   412,
     328,   182,   221,   132,   133,   134,   420,   413,   110,   187,
     221,   188,   132,   133,   134,   112,   113,   135,   136,   137,
      64,    65,    66,    67,   138,    27,    28,    29,   345,   423,
     116,    10,   182,   135,   136,   137,   426,    30,    31,   427,
     138,   118,   135,   136,   137,   200,   201,   202,   421,   138,
       1,   189,     2,     3,     4,     5,     6,   444,     7,     8,
     221,   463,   121,     9,   221,   150,   151,   190,   191,   192,
     193,   194,   320,   321,   322,   153,   157,   158,   162,   195,
     196,   197,   198,   199,   200,   201,   202,   166,   169,   167,
     172,   170,   171,   394,   395,   396,   397,   173,   189,   174,
     398,   399,   175,   208,   203,   218,    11,    10,    10,   222,
     225,   462,   226,   384,   190,   191,   192,   193,   189,   348,
     400,   227,   231,   255,   245,   259,   195,   196,   197,   198,
     199,   200,   201,   202,  -247,  -247,   192,   193,   189,   258,
     311,   277,   312,   316,   317,   329,  -247,   196,   197,   198,
     199,   200,   201,   202,   190,   191,   192,   193,   189,   331,
     340,   342,   185,   347,   369,   370,   195,   196,   197,   198,
     199,   200,   201,   202,   344,   350,  -247,  -247,   351,   378,
     352,   379,   380,    11,   353,   354,   355,  -247,  -247,   198,
     199,   200,   201,   202,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   401,   402,   403,   404,   356,   363,   381,   405,
     406,   375,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   382,   386,   389,   390,   391,   407,
     332,  -166,   333,   334,   335,   336,   392,   337,   338,   393,
     408,   419,   409,   430,   422,   424,   445,   447,   425,   428,
     429,   448,   449,   450,   431,   432,   433,   434,   435,   451,
     436,   452,   437,   438,   439,   453,   440,   441,   442,   443,
     182,   454,   455,   456,   457,   458,   459,   460,   461,   464,
      92,    80,   465,   365,   410,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,    81,   156,
      82,    83,    84,    50,    85,   168,   123,   349,   122,   377,
     416,   253,   325,    71,   120,   415,     0,    87,     0,     0,
     224
};

static const yytype_int16 yycheck[] =
{
      11,    44,   116,   135,   136,   222,     8,     3,     3,    21,
      42,    23,    24,    65,    42,    53,   171,     4,     0,     6,
       3,    63,     9,    29,   138,     7,   114,     9,    10,    11,
      12,    13,     3,    15,    16,    60,   150,   151,    20,    58,
      63,     3,     4,     5,     6,    32,    63,    64,   162,   312,
      37,    30,    28,   316,     4,     5,     6,     3,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   185,   186,   228,     0,    94,   101,    64,    65,    66,
      57,    58,   141,    65,    71,   144,    65,    25,   140,    27,
      52,   123,   124,   141,   208,   183,   144,   108,    60,    61,
     363,     8,   144,   143,   259,    59,   144,     3,     4,     5,
       6,   154,   140,   123,   124,    98,    59,   100,    21,    81,
      23,    24,   142,     3,     4,     5,     6,   138,    14,   141,
      35,    81,     3,     4,     5,     6,    60,    35,   140,   144,
     135,   103,   104,   105,   140,    42,   123,   124,   123,   124,
     141,   368,   141,   103,   104,   105,   419,   141,   140,    22,
     144,    52,   173,   277,    60,    61,   141,   378,   379,    65,
     157,   133,   134,   135,    98,   218,   100,   101,   140,   141,
      60,    61,    54,   141,   141,    81,   144,   144,   141,   344,
       3,   144,    60,     7,     3,     9,    10,    11,    12,    13,
     141,    81,    16,   144,   247,   140,    20,   103,   104,   105,
      81,    61,    31,    32,    33,    34,   348,    36,   141,   141,
     231,   144,   144,   103,   104,   105,   381,   141,     3,    60,
     144,    62,   103,   104,   105,     3,     3,   133,   134,   135,
      28,    29,    30,    31,   140,    25,    26,    27,   259,   141,
      53,    65,   144,   133,   134,   135,   141,    37,    38,   144,
     140,     4,   133,   134,   135,   135,   136,   137,   382,   140,
       7,   102,     9,    10,    11,    12,    13,   141,    15,    16,
     144,   141,   140,    20,   144,    40,    55,   118,   119,   120,
     121,   122,    66,    67,    68,    61,    49,     3,   130,   130,
     131,   132,   133,   134,   135,   136,   137,    65,   141,    65,
      61,   141,   140,    70,    71,    72,    73,   140,   102,     4,
      77,    78,     6,    56,   142,    42,   140,    65,    65,   140,
     140,   445,   140,   344,   118,   119,   120,   121,   102,   123,
      97,   141,   140,     3,   141,   140,   130,   131,   132,   133,
     134,   135,   136,   137,   118,   119,   120,   121,   102,    62,
      99,   144,   140,   140,     3,   141,   130,   131,   132,   133,
     134,   135,   136,   137,   118,   119,   120,   121,   102,   144,
      42,    39,   123,   101,     4,     3,   130,   131,   132,   133,
     134,   135,   136,   137,   140,   140,   120,   121,   140,    44,
     140,    44,     3,   140,   140,   140,   140,   131,   132,   133,
     134,   135,   136,   137,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    70,    71,    72,    73,   140,   140,    40,    77,
      78,   141,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    41,   141,     6,     6,     6,    97,
      43,    44,    45,    46,    47,    48,     6,    50,    51,     6,
     101,   140,    99,   144,   141,   141,    49,     6,   141,   141,
     141,     6,     6,     6,   144,   144,   144,   144,   144,     6,
     144,     6,   144,   144,   144,     6,   144,   144,   144,   144,
     144,     6,     6,     6,     6,     6,     6,     6,     6,   141,
      53,    44,   141,   314,   361,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,    44,   105,
      44,    44,    44,    11,    44,   121,    94,   277,    89,   331,
     376,   182,   226,    34,    76,   372,    -1,    44,    -1,    -1,
     163
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    15,    16,    20,
      65,   140,   146,   147,   148,   150,   159,   160,   162,   164,
     167,   168,   169,   170,   171,   192,   197,    25,    26,    27,
      37,    38,    29,    25,    27,     3,   190,    63,   190,    31,
      32,    33,    34,    36,   163,    28,   190,     3,   193,   194,
     170,   171,     0,   143,   216,    21,    23,    24,   186,     8,
     172,   174,    59,   215,   215,   215,   215,   215,   190,    59,
     214,   214,   142,    14,   190,    63,    64,    35,    35,   149,
     150,   159,   160,   164,   167,   168,   169,   197,    42,   144,
     141,   141,   148,    22,   140,   173,   174,    52,   181,    54,
     175,    60,     3,   190,   190,   190,     3,   140,   161,    61,
       3,   190,     3,     3,   165,   166,    53,   183,     4,   213,
     213,   140,   194,   173,   174,     3,     4,     5,     6,    60,
      61,    81,   103,   104,   105,   133,   134,   135,   140,   198,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
      40,    55,   179,    61,    42,   140,   161,    49,     3,   158,
      30,   171,   130,   144,   183,   201,    65,    65,   172,   141,
     141,   140,    61,   140,     4,     6,   210,   202,   202,   171,
     201,    63,   144,   182,    42,   123,   124,    60,    62,   102,
     118,   119,   120,   121,   122,   130,   131,   132,   133,   134,
     135,   136,   137,   142,   176,   177,   201,   201,    56,   180,
     169,     3,    98,   100,   151,   152,   153,   157,    42,   190,
     141,   144,   140,   201,   166,   140,   140,   141,    52,   141,
     198,   140,   171,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   141,   141,   140,   187,   188,
     189,   190,   195,   200,   183,     3,   201,   201,    62,   140,
      60,   101,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,     3,   135,   144,    57,    58,
     178,   201,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
     154,    99,   140,   141,   144,   169,   140,     3,   199,   209,
      66,    67,    68,   211,   212,   211,   198,   141,   171,   141,
     169,   144,    43,    45,    46,    47,    48,    50,    51,   196,
      42,   191,    39,   185,   140,   171,   198,   101,   123,   177,
     140,   140,   140,   140,   140,   140,   140,    60,    98,   100,
     101,   155,   156,   140,   158,   152,   158,   141,   144,     4,
       3,   141,   144,   141,   141,   141,   141,   188,    44,    44,
       3,    40,    41,   184,   171,   198,   141,   141,   202,     6,
       6,     6,     6,     6,    70,    71,    72,    73,    77,    78,
      97,    70,    71,    72,    73,    77,    78,    97,   101,    99,
     156,   158,   141,   141,   209,   212,   191,   189,   189,   140,
     198,   201,   141,   141,   141,   141,   141,   144,   141,   141,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   141,    49,   158,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   201,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   145,   146,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   149,   149,   149,   149,   149,   149,
     149,   149,   150,   150,   150,   150,   150,   150,   151,   151,
     152,   152,   153,   153,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   155,   155,   156,   156,
     156,   156,   157,   157,   158,   158,   159,   160,   160,   161,
     161,   162,   163,   163,   163,   163,   163,   164,   165,   165,
     166,   167,   167,   168,   168,   169,   169,   169,   169,   170,
     170,   171,   172,   173,   173,   174,   175,   175,   176,   176,
     177,   178,   178,   178,   179,   179,   180,   180,   181,   181,
     182,   182,   183,   183,   184,   184,   185,   185,   186,   186,
     186,   186,   187,   187,   188,   188,   189,   189,   190,   190,
     191,   191,   191,   192,   192,   193,   193,   194,   195,   195,
     196,   196,   196,   196,   196,   196,   196,   197,   197,   198,
     198,   199,   199,   200,   200,   201,   201,   201,   201,   201,
     202,   202,   202,   202,   202,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   204,   204,   205,   206,   206,   207,
     207,   207,   207,   208,   208,   208,   208,   209,   209,   209,
     209,   209,   209,   209,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   211,   211,   212,   212,
     212,   213,   214,   214,   215,   215,   216,   216
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
       3,     4,     4,     8,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,     7,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     3,     0,     1,     2,
       1,     1,     1,     3,     1,     1,     2,     4,     1,     3,
       2,     5,     0,     2,     0,     1,     3,     5,     4,     6,
       1,     1,     1,     1,     1,     1,     0,     2,     2,     1,
       3,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       3,     3,     1,     1,     1,     3,     4,     5,     4,     3,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     5,     5,     6,     4,
       5,     5,     6,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     2,     2,
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
#line 214 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1770 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 214 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1778 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 148 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1792 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 138 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 1806 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 207 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1817 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 218 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1826 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 218 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1835 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 178 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 1849 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1859 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1869 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1879 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1889 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1899 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 249 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 1909 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 168 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1923 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 168 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1937 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 237 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 1947 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1955 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1963 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1972 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1980 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1988 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2002 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2011 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2020 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 198 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2042 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 227 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2051 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 188 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2065 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 188 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2079 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 243 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->statement_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2089 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2098 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2112 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr_array: /* constant_expr_array  */
#line 158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2126 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2134 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2142 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2150 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2158 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2166 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2174 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2182 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2190 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2198 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2206 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2214 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 214 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2222 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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

#line 2330 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
#line 349 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2545 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 353 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new Vector<BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2556 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 359 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2567 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 366 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2573 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 367 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2579 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 368 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2585 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 369 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2591 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 370 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2597 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 371 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2603 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 372 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2609 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 373 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2615 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 374 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2621 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 14: /* explainable_statement: create_statement  */
#line 376 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2627 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 15: /* explainable_statement: drop_statement  */
#line 377 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2633 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 16: /* explainable_statement: copy_statement  */
#line 378 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2639 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 17: /* explainable_statement: show_statement  */
#line 379 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2645 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 18: /* explainable_statement: select_statement  */
#line 380 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2651 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 19: /* explainable_statement: delete_statement  */
#line 381 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2657 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 20: /* explainable_statement: update_statement  */
#line 382 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2663 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 21: /* explainable_statement: insert_statement  */
#line 383 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2669 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 22: /* create_statement: CREATE SCHEMA if_not_exists IDENTIFIER  */
#line 390 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                          {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.create_stmt)->create_info_ = std::move(create_schema_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2682 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 23: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 400 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2700 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 24: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 415 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2728 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 25: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 439 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2748 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 26: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 455 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
}
#line 2768 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 27: /* create_statement: CREATE INDEX if_not_exists IDENTIFIER ON table_name '(' identifier_array ')'  */
#line 471 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
}
#line 2790 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 28: /* table_element_array: table_element  */
#line 489 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new Vector<TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 2799 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 29: /* table_element_array: table_element_array ',' table_element  */
#line 493 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 2808 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 30: /* table_element: table_column  */
#line 499 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 2816 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 31: /* table_element: table_constraint  */
#line 502 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 2824 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 32: /* table_column: IDENTIFIER column_type  */
#line 508 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2867 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 33: /* table_column: IDENTIFIER column_type column_constraints  */
#line 546 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 34: /* column_type: BOOLEAN  */
#line 587 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kBoolean}; }
#line 2917 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* column_type: TINYINT  */
#line 588 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kTinyInt}; }
#line 2923 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: SMALLINT  */
#line 589 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kSmallInt}; }
#line 2929 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: INTEGER  */
#line 590 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kInteger}; }
#line 2935 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: BIGINT  */
#line 591 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kBigInt}; }
#line 2941 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: HUGEINT  */
#line 592 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kHugeInt}; }
#line 2947 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: FLOAT  */
#line 593 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 2953 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: REAL  */
#line 594 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 2959 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: DOUBLE  */
#line 595 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kDouble}; }
#line 2965 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: DATE  */
#line 596 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kDate}; }
#line 2971 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: TIME  */
#line 597 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kTime}; }
#line 2977 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: DATETIME  */
#line 598 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kDateTime}; }
#line 2983 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: TIMESTAMP  */
#line 599 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = ColumnType{LogicalType::kTimestamp}; }
#line 2989 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: UUID  */
#line 600 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kUuid}; }
#line 2995 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: POINT  */
#line 601 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kPoint}; }
#line 3001 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: LINE  */
#line 602 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLine}; }
#line 3007 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: LSEG  */
#line 603 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLineSeg}; }
#line 3013 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: BOX  */
#line 604 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = ColumnType{LogicalType::kBox}; }
#line 3019 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: PATH  */
#line 605 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kPath}; }
#line 3025 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: POLYGON  */
#line 606 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kPolygon}; }
#line 3031 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: CIRCLE  */
#line 607 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kCircle}; }
#line 3037 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 609 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kChar, (yyvsp[-1].long_value)}; }
#line 3043 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 610 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3049 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 611 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 612 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-1].long_value), 0}; }
#line 3061 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: DECIMAL  */
#line 613 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
#line 3067 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 614 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3073 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 615 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = ColumnType{LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3079 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 616 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3085 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3091 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 618 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3097 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 619 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3103 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 620 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3109 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 621 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3115 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 622 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3121 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 3127 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 624 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 3133 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 625 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 3139 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 626 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 3145 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 627 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 3151 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 628 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 3157 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 629 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 3163 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* column_constraints: column_constraint  */
#line 648 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new HashSet<ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3172 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 77: /* column_constraints: column_constraints column_constraint  */
#line 652 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3186 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* column_constraint: PRIMARY KEY  */
#line 662 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = ConstraintType::kPrimaryKey;
}
#line 3194 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* column_constraint: UNIQUE  */
#line 665 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = ConstraintType::kUnique;
}
#line 3202 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* column_constraint: NULLABLE  */
#line 668 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = ConstraintType::kNull;
}
#line 3210 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* column_constraint: NOT NULLABLE  */
#line 671 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = ConstraintType::kNotNull;
}
#line 3218 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 675 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kPrimaryKey;
}
#line 3228 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 680 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kUnique;
}
#line 3238 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* identifier_array: IDENTIFIER  */
#line 687 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new Vector<String>();
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3248 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 692 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3258 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 86: /* delete_statement: DELETE FROM table_name where_clause  */
#line 701 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                       {
    (yyval.delete_stmt) = new DeleteStatement();

    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.delete_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.delete_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    (yyval.delete_stmt)->where_expr_ = (yyvsp[0].expr_t);
}
#line 3273 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 87: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES '(' constant_expr_array ')'  */
#line 715 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                      {
    (yyval.insert_stmt) = new InsertStatement();
    if((yyvsp[-5].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-5].table_name_t)->schema_name_ptr_;
        free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-5].table_name_t)->table_name_ptr_;
    free((yyvsp[-5].table_name_t)->table_name_ptr_);

    (yyval.insert_stmt)->columns_ = (yyvsp[-4].identifier_array_t);
    (yyval.insert_stmt)->values_ = (yyvsp[-1].expr_array_t);
}
#line 3290 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 88: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 727 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3306 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* optional_identifier_array: '(' identifier_array ')'  */
#line 739 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3314 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* optional_identifier_array: %empty  */
#line 742 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3322 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 749 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyval.explain_stmt) = new ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3332 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* explain_type: AST  */
#line 755 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.explain_type_t) = ExplainType::kAst;
}
#line 3340 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* explain_type: UNOPT LOGICAL  */
#line 758 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    (yyval.explain_type_t) = ExplainType::kUnOpt;
}
#line 3348 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* explain_type: OPT LOGICAL  */
#line 761 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    (yyval.explain_type_t) = ExplainType::kOpt;
}
#line 3356 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* explain_type: PHYSICAL  */
#line 764 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPhysical;
}
#line 3364 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* explain_type: PIPELINE  */
#line 767 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.explain_type_t) = ExplainType::kPipeline;
}
#line 3372 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 774 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                       {
    (yyval.update_stmt) = new UpdateStatement();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.update_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    (yyval.update_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.update_stmt)->where_expr_ = (yyvsp[0].expr_t);
    (yyval.update_stmt)->update_expr_array_ = (yyvsp[-1].update_expr_array_t);
}
#line 3388 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* update_expr_array: update_expr  */
#line 786 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.update_expr_array_t) = new Vector<UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3397 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* update_expr_array: update_expr_array ',' update_expr  */
#line 790 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3406 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* update_expr: IDENTIFIER '=' expr  */
#line 795 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.update_expr_t) = new UpdateExpr();
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3416 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 806 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.drop_stmt)->drop_info_ = std::move(drop_schema_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 3429 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 816 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3447 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 834 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3493 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 104: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 875 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3539 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 105: /* select_statement: select_without_paren  */
#line 920 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3547 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* select_statement: select_with_paren  */
#line 923 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3555 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 926 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                     {
    (yyvsp[-2].select_stmt)->set_op_ = (yyvsp[-1].set_operator_t);
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3569 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 935 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    (yyvsp[-2].select_stmt)->set_op_ = (yyvsp[-1].set_operator_t);
    SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 3583 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* select_with_paren: '(' select_without_paren ')'  */
#line 945 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3591 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* select_with_paren: '(' select_with_paren ')'  */
#line 948 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3599 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 952 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3608 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 957 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 3619 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 964 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3627 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 967 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3635 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* select_clause_without_modifier: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause  */
#line 972 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3654 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 987 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3662 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* order_by_clause: %empty  */
#line 990 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3670 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* order_by_expr_list: order_by_expr  */
#line 994 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new Vector<OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 3679 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 998 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 3688 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* order_by_expr: expr order_by_type  */
#line 1003 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 3698 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* order_by_type: ASC  */
#line 1009 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = kAsc;
}
#line 3706 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* order_by_type: DESC  */
#line 1012 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = kDesc;
}
#line 3714 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* order_by_type: %empty  */
#line 1015 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.order_by_type_t) = kAsc;
}
#line 3722 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* limit_expr: LIMIT expr  */
#line 1019 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3730 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* limit_expr: %empty  */
#line 1023 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3736 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* offset_expr: OFFSET expr  */
#line 1025 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3744 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* offset_expr: %empty  */
#line 1029 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3750 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* distinct: DISTINCT  */
#line 1031 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3758 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* distinct: %empty  */
#line 1034 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3766 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* from_clause: FROM table_reference  */
#line 1038 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3774 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* from_clause: %empty  */
#line 1041 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3782 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* where_clause: WHERE expr  */
#line 1045 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3790 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 133: /* where_clause: %empty  */
#line 1048 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3798 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 134: /* having_clause: HAVING expr  */
#line 1052 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3806 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 135: /* having_clause: %empty  */
#line 1055 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3814 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* group_by_clause: GROUP BY expr_array  */
#line 1059 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 3822 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* group_by_clause: %empty  */
#line 1062 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 3830 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* set_operator: UNION  */
#line 1066 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    (yyval.set_operator_t) = SetOperatorType::kUnion;
}
#line 3838 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 139: /* set_operator: UNION ALL  */
#line 1069 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kUnionAll;
}
#line 3846 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* set_operator: INTERSECT  */
#line 1072 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    (yyval.set_operator_t) = SetOperatorType::kIntersect;
}
#line 3854 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* set_operator: EXCEPT  */
#line 1075 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.set_operator_t) = SetOperatorType::kExcept;
}
#line 3862 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* table_reference: table_reference_unit  */
#line 1083 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3870 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* table_reference: table_reference ',' table_reference_unit  */
#line 1086 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CrossProductReference* cross_product_ref = new CrossProductReference();
    cross_product_ref->left_ = (yyvsp[-2].table_reference_t);
    cross_product_ref->right_ = (yyvsp[0].table_reference_t);

    (yyval.table_reference_t) = cross_product_ref;
}
#line 3882 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 146: /* table_reference_name: table_name table_alias  */
#line 1097 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3900 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 147: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1111 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    SubqueryReference* subquery_reference = new SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 3911 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 148: /* table_name: IDENTIFIER  */
#line 1120 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 3922 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 149: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1126 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 3934 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 150: /* table_alias: AS IDENTIFIER  */
#line 1135 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 3943 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1139 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 3953 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* table_alias: %empty  */
#line 1144 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 3961 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* with_clause: WITH with_expr_list  */
#line 1151 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 3969 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* with_clause: %empty  */
#line 1154 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 3977 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* with_expr_list: with_expr  */
#line 1158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new Vector<WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 3986 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 156: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1161 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 3995 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 157: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1166 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                             {
    (yyval.with_expr_t) = new WithExpr();
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->statement_ = (yyvsp[-1].select_stmt);
}
#line 4006 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1177 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4018 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1184 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                   {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4031 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* join_type: INNER  */
#line 1198 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = JoinType::kInner;
}
#line 4039 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* join_type: LEFT  */
#line 1201 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kLeft;
}
#line 4047 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* join_type: RIGHT  */
#line 1204 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kRight;
}
#line 4055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* join_type: OUTER  */
#line 1207 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4063 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* join_type: FULL  */
#line 1210 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 4071 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* join_type: CROSS  */
#line 1213 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kCross;
}
#line 4079 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* join_type: %empty  */
#line 1216 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 4086 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* show_statement: SHOW TABLES  */
#line 1222 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kTables;
}
#line 4095 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* show_statement: DESCRIBE table_name  */
#line 1226 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4111 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* expr_array: expr_alias  */
#line 1242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4120 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* expr_array: expr_array ',' expr_alias  */
#line 1246 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4129 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* constant_expr_array: constant_expr  */
#line 1251 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4138 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* constant_expr_array: constant_expr_array ',' constant_expr  */
#line 1255 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4147 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* expr_alias: expr AS IDENTIFIER  */
#line 1260 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4157 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* expr_alias: expr  */
#line 1265 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4165 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 180: /* operand: '(' expr ')'  */
#line 1275 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4173 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 181: /* operand: '(' select_without_paren ')'  */
#line 1278 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4184 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 185: /* function_expr: IDENTIFIER '(' ')'  */
#line 1288 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 4196 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 186: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1295 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 4208 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 187: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1302 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 4221 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 188: /* function_expr: operand IS NOT NULLABLE  */
#line 1310 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4233 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 189: /* function_expr: operand IS NULLABLE  */
#line 1317 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4245 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 190: /* function_expr: '-' operand  */
#line 1324 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "-";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4257 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 191: /* function_expr: '+' operand  */
#line 1331 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "+";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 4269 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 192: /* function_expr: operand '-' operand  */
#line 1338 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4282 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 193: /* function_expr: operand '+' operand  */
#line 1346 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4295 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 194: /* function_expr: operand '*' operand  */
#line 1354 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4308 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 195: /* function_expr: operand '/' operand  */
#line 1362 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4321 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 196: /* function_expr: operand '%' operand  */
#line 1370 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4334 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 197: /* function_expr: operand '=' operand  */
#line 1378 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4347 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 198: /* function_expr: operand EQUAL operand  */
#line 1386 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4360 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 199: /* function_expr: operand NOT_EQ operand  */
#line 1394 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4373 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 200: /* function_expr: operand '<' operand  */
#line 1402 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4386 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 201: /* function_expr: operand '>' operand  */
#line 1410 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4399 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 202: /* function_expr: operand LESS_EQ operand  */
#line 1418 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4412 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 203: /* function_expr: operand GREATER_EQ operand  */
#line 1426 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4425 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 204: /* conjunction_expr: expr AND expr  */
#line 1435 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4438 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 205: /* conjunction_expr: expr OR expr  */
#line 1443 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4451 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 206: /* between_expr: operand BETWEEN operand AND operand  */
#line 1452 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                  {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "between_and";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 4465 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 207: /* in_expr: operand IN '(' expr_array ')'  */
#line 1462 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
     FunctionExpr* func_expr = new FunctionExpr();
     func_expr->func_name_ = "in";
     func_expr->arguments_ = (yyvsp[-1].expr_array_t);
     func_expr->arguments_->emplace_back((yyvsp[-4].expr_t));
     (yyval.expr_t) = func_expr;
 }
#line 4477 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 208: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 1469 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
     FunctionExpr* func_expr = new FunctionExpr();
     func_expr->func_name_ = "not_in";
     func_expr->arguments_ = (yyvsp[-1].expr_array_t);
     func_expr->arguments_->emplace_back((yyvsp[-5].expr_t));
     (yyval.expr_t) = func_expr;
 }
#line 4489 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 209: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 1477 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                   {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4500 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 210: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 1483 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4511 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 211: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 1489 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4523 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 212: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 1496 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    SubqueryExpr* subquery_expr = new SubqueryExpr();
    subquery_expr->subquery_type_ = SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4535 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 213: /* column_expr: IDENTIFIER  */
#line 1504 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4545 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 214: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1509 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 4555 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 215: /* column_expr: '*'  */
#line 1514 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4565 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 216: /* column_expr: column_expr '.' '*'  */
#line 1519 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 4579 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 217: /* constant_expr: STRING  */
#line 1529 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4589 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 218: /* constant_expr: TRUE  */
#line 1534 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
#line 4599 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 219: /* constant_expr: FALSE  */
#line 1539 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
#line 4609 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 220: /* constant_expr: DOUBLE_VALUE  */
#line 1544 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kFloat);
    const_expr->float_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
#line 4619 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 221: /* constant_expr: LONG_VALUE  */
#line 1549 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4629 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 222: /* constant_expr: DATE STRING  */
#line 1554 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 4639 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 223: /* constant_expr: INTERVAL interval_expr  */
#line 1559 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4647 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 224: /* interval_expr: LONG_VALUE SECONDS  */
#line 1563 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4658 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 225: /* interval_expr: LONG_VALUE SECOND  */
#line 1569 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4669 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 226: /* interval_expr: LONG_VALUE MINUTES  */
#line 1575 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4680 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 227: /* interval_expr: LONG_VALUE MINUTE  */
#line 1581 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4691 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 228: /* interval_expr: LONG_VALUE HOURS  */
#line 1587 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4702 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 229: /* interval_expr: LONG_VALUE HOUR  */
#line 1593 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4713 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 230: /* interval_expr: LONG_VALUE DAYS  */
#line 1599 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4724 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 231: /* interval_expr: LONG_VALUE DAY  */
#line 1605 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4735 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 232: /* interval_expr: LONG_VALUE MONTHS  */
#line 1611 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4746 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 233: /* interval_expr: LONG_VALUE MONTH  */
#line 1617 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4757 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 234: /* interval_expr: LONG_VALUE YEARS  */
#line 1623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4768 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 235: /* interval_expr: LONG_VALUE YEAR  */
#line 1629 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4779 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 236: /* copy_option_list: copy_option  */
#line 1640 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.copy_option_array) = new Vector<CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 4788 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 237: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1644 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 4797 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 238: /* copy_option: FORMAT IDENTIFIER  */
#line 1649 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 4815 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 239: /* copy_option: DELIMITER STRING  */
#line 1662 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 4826 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 240: /* copy_option: HEADER  */
#line 1668 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 4836 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 241: /* file_path: STRING  */
#line 1674 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 4844 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 242: /* if_exists: IF EXISTS  */
#line 1678 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 4850 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 243: /* if_exists: %empty  */
#line 1679 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 4856 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 244: /* if_not_exists: IF NOT EXISTS  */
#line 1681 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 4862 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 245: /* if_not_exists: %empty  */
#line 1682 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 4868 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 4872 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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

#line 1688 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d\n", msg, llocp->first_column);
}

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
  YYSYMBOL_SCHEMA = 21,                    /* SCHEMA  */
  YYSYMBOL_TABLE = 22,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 23,                /* COLLECTION  */
  YYSYMBOL_TABLES = 24,                    /* TABLES  */
  YYSYMBOL_GROUP = 25,                     /* GROUP  */
  YYSYMBOL_BY = 26,                        /* BY  */
  YYSYMBOL_HAVING = 27,                    /* HAVING  */
  YYSYMBOL_AS = 28,                        /* AS  */
  YYSYMBOL_NATURAL = 29,                   /* NATURAL  */
  YYSYMBOL_JOIN = 30,                      /* JOIN  */
  YYSYMBOL_LEFT = 31,                      /* LEFT  */
  YYSYMBOL_RIGHT = 32,                     /* RIGHT  */
  YYSYMBOL_OUTER = 33,                     /* OUTER  */
  YYSYMBOL_FULL = 34,                      /* FULL  */
  YYSYMBOL_ON = 35,                        /* ON  */
  YYSYMBOL_INNER = 36,                     /* INNER  */
  YYSYMBOL_CROSS = 37,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 38,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 39,                     /* WHERE  */
  YYSYMBOL_ORDER = 40,                     /* ORDER  */
  YYSYMBOL_LIMIT = 41,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 42,                    /* OFFSET  */
  YYSYMBOL_ASC = 43,                       /* ASC  */
  YYSYMBOL_DESC = 44,                      /* DESC  */
  YYSYMBOL_IF = 45,                        /* IF  */
  YYSYMBOL_NOT = 46,                       /* NOT  */
  YYSYMBOL_EXISTS = 47,                    /* EXISTS  */
  YYSYMBOL_FROM = 48,                      /* FROM  */
  YYSYMBOL_TO = 49,                        /* TO  */
  YYSYMBOL_WITH = 50,                      /* WITH  */
  YYSYMBOL_DELIMITER = 51,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 52,                    /* FORMAT  */
  YYSYMBOL_HEADER = 53,                    /* HEADER  */
  YYSYMBOL_BOOLEAN = 54,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 55,                   /* INTEGER  */
  YYSYMBOL_TINYINT = 56,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 57,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 58,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 59,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 60,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 61,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 62,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 63,                    /* DOUBLE  */
  YYSYMBOL_REAL = 64,                      /* REAL  */
  YYSYMBOL_DECIMAL = 65,                   /* DECIMAL  */
  YYSYMBOL_DATE = 66,                      /* DATE  */
  YYSYMBOL_TIME = 67,                      /* TIME  */
  YYSYMBOL_DATETIME = 68,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 69,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 70,                      /* UUID  */
  YYSYMBOL_POINT = 71,                     /* POINT  */
  YYSYMBOL_LINE = 72,                      /* LINE  */
  YYSYMBOL_LSEG = 73,                      /* LSEG  */
  YYSYMBOL_BOX = 74,                       /* BOX  */
  YYSYMBOL_PATH = 75,                      /* PATH  */
  YYSYMBOL_POLYGON = 76,                   /* POLYGON  */
  YYSYMBOL_CIRCLE = 77,                    /* CIRCLE  */
  YYSYMBOL_BLOB = 78,                      /* BLOB  */
  YYSYMBOL_BITMAP = 79,                    /* BITMAP  */
  YYSYMBOL_EMBEDDING = 80,                 /* EMBEDDING  */
  YYSYMBOL_VECTOR = 81,                    /* VECTOR  */
  YYSYMBOL_BIT = 82,                       /* BIT  */
  YYSYMBOL_PRIMARY = 83,                   /* PRIMARY  */
  YYSYMBOL_KEY = 84,                       /* KEY  */
  YYSYMBOL_UNIQUE = 85,                    /* UNIQUE  */
  YYSYMBOL_NULLABLE = 86,                  /* NULLABLE  */
  YYSYMBOL_IS = 87,                        /* IS  */
  YYSYMBOL_TRUE = 88,                      /* TRUE  */
  YYSYMBOL_FALSE = 89,                     /* FALSE  */
  YYSYMBOL_INTERVAL = 90,                  /* INTERVAL  */
  YYSYMBOL_SECOND = 91,                    /* SECOND  */
  YYSYMBOL_SECONDS = 92,                   /* SECONDS  */
  YYSYMBOL_MINUTE = 93,                    /* MINUTE  */
  YYSYMBOL_MINUTES = 94,                   /* MINUTES  */
  YYSYMBOL_HOUR = 95,                      /* HOUR  */
  YYSYMBOL_HOURS = 96,                     /* HOURS  */
  YYSYMBOL_DAY = 97,                       /* DAY  */
  YYSYMBOL_DAYS = 98,                      /* DAYS  */
  YYSYMBOL_MONTH = 99,                     /* MONTH  */
  YYSYMBOL_MONTHS = 100,                   /* MONTHS  */
  YYSYMBOL_YEAR = 101,                     /* YEAR  */
  YYSYMBOL_YEARS = 102,                    /* YEARS  */
  YYSYMBOL_EQUAL = 103,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 104,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 105,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 106,               /* GREATER_EQ  */
  YYSYMBOL_NUMBER = 107,                   /* NUMBER  */
  YYSYMBOL_OR = 108,                       /* OR  */
  YYSYMBOL_AND = 109,                      /* AND  */
  YYSYMBOL_110_ = 110,                     /* '='  */
  YYSYMBOL_111_ = 111,                     /* '<'  */
  YYSYMBOL_112_ = 112,                     /* '>'  */
  YYSYMBOL_113_ = 113,                     /* '+'  */
  YYSYMBOL_114_ = 114,                     /* '-'  */
  YYSYMBOL_115_ = 115,                     /* '*'  */
  YYSYMBOL_116_ = 116,                     /* '/'  */
  YYSYMBOL_117_ = 117,                     /* '%'  */
  YYSYMBOL_118_ = 118,                     /* '['  */
  YYSYMBOL_119_ = 119,                     /* ']'  */
  YYSYMBOL_120_ = 120,                     /* '('  */
  YYSYMBOL_121_ = 121,                     /* ')'  */
  YYSYMBOL_122_ = 122,                     /* '.'  */
  YYSYMBOL_123_ = 123,                     /* ';'  */
  YYSYMBOL_124_ = 124,                     /* ','  */
  YYSYMBOL_YYACCEPT = 125,                 /* $accept  */
  YYSYMBOL_input_pattern = 126,            /* input_pattern  */
  YYSYMBOL_statement_list = 127,           /* statement_list  */
  YYSYMBOL_statement = 128,                /* statement  */
  YYSYMBOL_create_statement = 129,         /* create_statement  */
  YYSYMBOL_table_element_array = 130,      /* table_element_array  */
  YYSYMBOL_table_element = 131,            /* table_element  */
  YYSYMBOL_table_column = 132,             /* table_column  */
  YYSYMBOL_column_type = 133,              /* column_type  */
  YYSYMBOL_column_constraints = 134,       /* column_constraints  */
  YYSYMBOL_column_constraint = 135,        /* column_constraint  */
  YYSYMBOL_table_constraint = 136,         /* table_constraint  */
  YYSYMBOL_identifier_array = 137,         /* identifier_array  */
  YYSYMBOL_drop_statement = 138,           /* drop_statement  */
  YYSYMBOL_copy_statement = 139,           /* copy_statement  */
  YYSYMBOL_select_statement = 140,         /* select_statement  */
  YYSYMBOL_select_with_paren = 141,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 142,     /* select_without_paren  */
  YYSYMBOL_select_clause = 143,            /* select_clause  */
  YYSYMBOL_order_by_clause = 144,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 145,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 146,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 147,            /* order_by_type  */
  YYSYMBOL_limit_expr = 148,               /* limit_expr  */
  YYSYMBOL_offset_expr = 149,              /* offset_expr  */
  YYSYMBOL_distinct = 150,                 /* distinct  */
  YYSYMBOL_from_clause = 151,              /* from_clause  */
  YYSYMBOL_where_clause = 152,             /* where_clause  */
  YYSYMBOL_having_clause = 153,            /* having_clause  */
  YYSYMBOL_group_by_clause = 154,          /* group_by_clause  */
  YYSYMBOL_table_reference = 155,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 156,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 157,     /* table_reference_name  */
  YYSYMBOL_table_name = 158,               /* table_name  */
  YYSYMBOL_table_alias = 159,              /* table_alias  */
  YYSYMBOL_with_clause = 160,              /* with_clause  */
  YYSYMBOL_with_expr_list = 161,           /* with_expr_list  */
  YYSYMBOL_with_expr = 162,                /* with_expr  */
  YYSYMBOL_join_clause = 163,              /* join_clause  */
  YYSYMBOL_join_type = 164,                /* join_type  */
  YYSYMBOL_show_statement = 165,           /* show_statement  */
  YYSYMBOL_expr_array = 166,               /* expr_array  */
  YYSYMBOL_expr_alias = 167,               /* expr_alias  */
  YYSYMBOL_expr = 168,                     /* expr  */
  YYSYMBOL_function_expr = 169,            /* function_expr  */
  YYSYMBOL_column_expr = 170,              /* column_expr  */
  YYSYMBOL_constant_expr = 171,            /* constant_expr  */
  YYSYMBOL_interval_expr = 172,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 173,         /* copy_option_list  */
  YYSYMBOL_copy_option = 174,              /* copy_option  */
  YYSYMBOL_file_path = 175,                /* file_path  */
  YYSYMBOL_if_exists = 176,                /* if_exists  */
  YYSYMBOL_if_not_exists = 177,            /* if_not_exists  */
  YYSYMBOL_semicolon = 178                 /* semicolon  */
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
#define YYFINAL  33
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   408

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  189
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  357

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   364


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   117,     2,     2,
     120,   121,   115,   113,   124,   114,   122,   116,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   123,
     111,   110,   112,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   118,     2,   119,     2,     2,     2,     2,     2,     2,
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
     105,   106,   107,   108,   109
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   312,   312,   316,   322,   329,   330,   331,   332,   334,
     341,   351,   366,   401,   405,   411,   414,   420,   462,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     519,   520,   521,   522,   523,   524,   525,   526,   527,   528,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   541,   542,   543,   544,   545,   546,   547,   548,   549,
     550,   569,   573,   582,   585,   588,   591,   595,   600,   607,
     612,   623,   633,   651,   703,   779,   782,   786,   789,   793,
     800,   817,   820,   824,   828,   833,   839,   842,   846,   850,
     852,   856,   858,   861,   865,   868,   872,   875,   879,   882,
     886,   889,   897,   900,   909,   909,   911,   931,   937,   946,
     950,   955,   962,   965,   969,   972,   977,   995,  1002,  1016,
    1019,  1022,  1025,  1028,  1031,  1034,  1040,  1044,  1060,  1064,
    1069,  1074,  1078,  1081,  1082,  1083,  1085,  1092,  1099,  1107,
    1114,  1121,  1128,  1135,  1143,  1151,  1159,  1167,  1175,  1183,
    1191,  1199,  1207,  1215,  1223,  1232,  1237,  1242,  1247,  1257,
    1262,  1267,  1272,  1277,  1282,  1287,  1291,  1297,  1303,  1309,
    1315,  1321,  1327,  1333,  1339,  1345,  1351,  1357,  1368,  1372,
    1377,  1389,  1395,  1401,  1405,  1406,  1408,  1409,  1411,  1412
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
  "PREPARE", "DESCRIBE", "SCHEMA", "TABLE", "COLLECTION", "TABLES",
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "FROM", "TO",
  "WITH", "DELIMITER", "FORMAT", "HEADER", "BOOLEAN", "INTEGER", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "CHAR", "VARCHAR", "FLOAT", "DOUBLE",
  "REAL", "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS",
  "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS",
  "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "NUMBER",
  "OR", "AND", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'['", "']'", "'('", "')'", "'.'", "';'", "','", "$accept",
  "input_pattern", "statement_list", "statement", "create_statement",
  "table_element_array", "table_element", "table_column", "column_type",
  "column_constraints", "column_constraint", "table_constraint",
  "identifier_array", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
  "select_clause", "order_by_clause", "order_by_expr_list",
  "order_by_expr", "order_by_type", "limit_expr", "offset_expr",
  "distinct", "from_clause", "where_clause", "having_clause",
  "group_by_clause", "table_reference", "table_reference_unit",
  "table_reference_name", "table_name", "table_alias", "with_clause",
  "with_expr_list", "with_expr", "join_clause", "join_type",
  "show_statement", "expr_array", "expr_alias", "expr", "function_expr",
  "column_expr", "constant_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-226)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-189)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       0,    25,    94,    42,    -3,    42,    52,    -6,    85,    -4,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,    88,  -226,    76,
      76,    76,    78,    78,     6,    13,  -226,  -226,    96,    29,
    -226,    17,    28,  -226,     2,  -226,   116,  -226,   115,   159,
      42,    42,   120,   165,    42,   166,   172,   172,    57,    52,
    -226,  -226,  -226,  -226,    37,   144,  -226,    72,  -226,  -226,
    -226,  -226,  -226,  -226,   151,   152,    88,  -226,    90,  -226,
    -226,  -226,   199,  -226,  -226,   205,    37,    37,  -226,    37,
     -40,  -226,   -23,  -226,   106,  -226,  -226,    14,    92,   111,
      84,    20,  -226,   168,  -226,    43,    43,    83,    42,    37,
     194,   233,   -27,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,     3,   222,   157,   122,   -64,
    -226,  -226,  -226,   113,   113,  -226,    37,  -226,   -55,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,   119,   193,  -226,   224,  -226,  -226,    37,   228,
    -226,   169,  -226,   103,   103,   -38,   -38,   103,   -38,   -38,
      43,    43,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,   136,   137,  -226,  -226,  -226,   153,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,   154,   188,   189,   190,   -32,   195,   251,  -226,    14,
     267,   269,  -226,   -26,  -226,   -19,    -8,  -226,    42,   283,
    -226,  -226,  -226,  -226,  -226,  -226,   284,   313,  -226,   134,
     291,   292,  -226,   312,   314,   315,   316,   317,    74,   249,
     238,   241,  -226,  -226,   -32,  -226,   251,  -226,    18,  -226,
    -226,  -226,  -226,   113,  -226,  -226,   193,    42,    42,   206,
      37,    37,   287,   207,   208,    22,   209,   211,   210,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   223,
     225,   226,  -226,  -226,  -226,    23,  -226,   330,  -226,  -226,
     300,   251,   227,   134,   320,   307,  -226,  -226,  -226,   346,
    -226,  -226,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,  -226,  -226,    37,    24,
      37,    37,   325,   247,   248,   250,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   134,  -226,
     246,  -226,    68,   134,    37,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,    37,  -226,  -226,  -226,   134,  -226
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     113,     0,     0,     0,     0,     0,     0,   113,     0,   189,
       3,     5,     6,     7,     9,    76,    75,     0,     8,   187,
     187,   187,   185,   185,   107,     0,   126,   127,     0,   112,
     114,     0,     0,     1,   113,     2,    93,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      78,    77,     4,    92,     0,     0,    10,     0,    11,   184,
      71,    72,   108,   183,     0,     0,     0,   115,   155,   159,
     162,   163,     0,   160,   161,     0,     0,     0,   157,     0,
      95,   128,   131,   135,   134,   133,   186,     0,     0,     0,
       0,     0,   164,     0,   165,   142,   141,     0,     0,     0,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      13,    15,    16,     0,     0,   116,     0,   136,     0,   167,
     166,   169,   168,   171,   170,   173,   172,   175,   174,   177,
     176,   132,    94,   102,   104,   111,   105,   129,     0,   101,
     130,     0,   140,   149,   150,   153,   154,   148,   151,   152,
     144,   143,   145,   146,   147,   156,   158,    19,    22,    20,
      21,    23,    24,     0,     0,    25,    27,    26,    44,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     0,     0,     0,     0,    17,     0,     0,    12,     0,
       0,     0,   182,     0,   178,     0,     0,   137,     0,     0,
     120,   121,   122,   123,   119,   124,     0,     0,   106,    96,
       0,    99,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    64,    65,    18,    61,     0,    69,     0,    14,
     181,   180,    74,     0,    73,   138,   103,     0,     0,   109,
       0,     0,    82,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    66,    63,    62,     0,    68,     0,   179,   117,
       0,     0,   100,    98,     0,    89,    40,    41,    43,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    67,    70,     0,     0,
       0,     0,    91,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   118,   110,
      81,    83,     0,    88,     0,    80,    42,    50,    48,    49,
      51,    52,    53,    47,    57,    55,    56,    58,    59,    60,
      54,     0,    86,    87,    85,    90,    84
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -226,  -226,  -226,   338,  -226,  -226,   186,  -226,  -226,  -226,
     155,  -226,  -225,  -226,  -226,  -226,   379,   380,   322,  -226,
    -226,    39,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,   183,  -177,    60,  -226,  -226,  -226,   343,  -226,  -226,
    -226,   -87,   294,   -76,  -226,  -226,  -226,  -226,   270,   156,
     361,   372,    86,  -226
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     8,     9,    10,    11,   119,   120,   121,   195,   234,
     235,   122,   238,    12,    13,    14,    15,    16,    37,   285,
     330,   331,   354,   312,   335,    54,   100,   149,   252,   221,
     142,   143,   144,   145,   218,    17,    29,    30,   146,   216,
      18,    80,    81,    82,    83,    84,    85,    94,   203,   204,
      64,    43,    39,    35
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      95,    96,  -188,    97,   128,   101,   165,     1,    98,     1,
       2,   275,     2,     3,   230,     3,     4,   116,     4,   151,
       5,    26,     5,    68,    69,    70,    71,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   206,
      68,    69,    70,    71,     6,    24,    19,    20,    21,   102,
       6,   231,     6,   232,   233,    28,   309,   198,   126,   152,
     199,    46,    47,    25,   102,    27,   207,  -189,  -189,    99,
     279,   280,   219,  -189,  -189,   110,   111,   112,   113,   114,
     103,   104,   105,   106,    99,    33,    72,   107,   108,   109,
     110,   111,   112,   113,   114,   242,    36,   117,   243,   118,
      57,    58,   244,    72,    61,   243,    40,    41,    73,    74,
      75,   352,   353,   245,     7,    22,    99,    23,   166,    34,
       7,    38,     7,    42,    48,    73,    74,    75,    45,   258,
     259,   260,   261,    76,    77,    78,   262,   263,    50,   276,
      79,   127,   277,   288,   306,   329,   289,   277,   277,    51,
      76,    77,    78,    49,    53,   102,   264,    79,   112,   113,
     114,    55,    56,   282,   200,   201,   202,    59,    60,    62,
     102,   103,   104,   105,   106,   283,    63,    66,   107,   108,
     109,   110,   111,   112,   113,   114,   103,   104,   105,   106,
     102,    86,    87,   107,   108,   109,   110,   111,   112,   113,
     114,    88,    89,    92,   141,   125,  -189,  -189,   105,   106,
      91,    93,   123,  -189,   108,   109,   110,   111,   112,   113,
     114,   102,   209,  -125,   210,   211,   212,   213,   115,   214,
     215,   124,   328,   148,   332,   333,   150,   103,   104,   105,
     106,   196,   197,   208,   107,   108,   109,   110,   111,   112,
     113,   114,   217,   220,   237,   222,   223,   224,   355,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   240,   241,   225,   226,   332,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   265,   266,   267,   268,   227,   228,
     229,   269,   270,   247,   248,   236,   249,   250,   253,   251,
     254,   255,   256,   257,   272,   273,   281,   284,   286,   287,
     290,   271,   291,   307,   292,   308,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   310,   303,   311,   304,
     305,    99,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   334,   336,   337,
     351,   338,    52,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   239,    31,    32,    90,   274,
     356,   246,    67,   147,   205,    44,     0,     0,     0,   278,
       0,     0,     0,     0,     0,     0,     0,     0,    65
};

static const yytype_int16 yycheck[] =
{
      76,    77,     0,    79,    91,    28,     3,     7,    48,     7,
      10,   236,    10,    13,    46,    13,    16,     3,    16,    46,
      20,    24,    20,     3,     4,     5,     6,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   126,
       3,     4,     5,     6,    50,     3,    21,    22,    23,    87,
      50,    83,    50,    85,    86,     3,   281,   121,    38,    86,
     124,    48,    49,     3,    87,     5,   121,   105,   106,   124,
     247,   248,   148,   111,   112,   113,   114,   115,   116,   117,
     103,   104,   105,   106,   124,     0,    66,   110,   111,   112,
     113,   114,   115,   116,   117,   121,     8,    83,   124,    85,
      40,    41,   121,    66,    44,   124,    20,    21,    88,    89,
      90,    43,    44,   121,   120,    21,   124,    23,   115,   123,
     120,    45,   120,    45,    28,    88,    89,    90,   122,    55,
      56,    57,    58,   113,   114,   115,    62,    63,   121,   121,
     120,   121,   124,   121,   121,   121,   124,   124,   124,   121,
     113,   114,   115,   124,    38,    87,    82,   120,   115,   116,
     117,    46,     3,   250,    51,    52,    53,    47,     3,     3,
      87,   103,   104,   105,   106,   251,     4,   120,   110,   111,
     112,   113,   114,   115,   116,   117,   103,   104,   105,   106,
      87,    47,   120,   110,   111,   112,   113,   114,   115,   116,
     117,    50,    50,     4,   121,   121,   103,   104,   105,   106,
     120,     6,   120,   110,   111,   112,   113,   114,   115,   116,
     117,    87,    29,    30,    31,    32,    33,    34,   122,    36,
      37,   120,   308,    39,   310,   311,     3,   103,   104,   105,
     106,    84,   120,   124,   110,   111,   112,   113,   114,   115,
     116,   117,    28,    25,     3,    86,   120,   120,   334,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,     4,     3,   120,   120,   351,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    55,    56,    57,    58,   120,   120,
     120,    62,    63,    30,    30,   120,     3,    26,     6,    27,
       6,     6,     6,     6,    86,    84,   120,    40,   121,   121,
     121,    82,   121,     3,   124,    35,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,    26,   124,    41,   124,
     124,   124,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,    42,   121,   121,
     124,   121,    34,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   199,     7,     7,    66,   234,
     351,   208,    49,    99,   124,    23,    -1,    -1,    -1,   243,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    10,    13,    16,    20,    50,   120,   126,   127,
     128,   129,   138,   139,   140,   141,   142,   160,   165,    21,
      22,    23,    21,    23,     3,   158,    24,   158,     3,   161,
     162,   141,   142,     0,   123,   178,     8,   143,    45,   177,
     177,   177,    45,   176,   176,   122,    48,    49,    28,   124,
     121,   121,   128,    38,   150,    46,     3,   158,   158,    47,
       3,   158,     3,     4,   175,   175,   120,   162,     3,     4,
       5,     6,    66,    88,    89,    90,   113,   114,   115,   120,
     166,   167,   168,   169,   170,   171,    47,   120,    50,    50,
     143,   120,     4,     6,   172,   168,   168,   168,    48,   124,
     151,    28,    87,   103,   104,   105,   106,   110,   111,   112,
     113,   114,   115,   116,   117,   122,     3,    83,    85,   130,
     131,   132,   136,   120,   120,   121,    38,   121,   166,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   121,   155,   156,   157,   158,   163,   167,    39,   152,
       3,    46,    86,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,     3,   115,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,   133,    84,   120,   121,   124,
      51,    52,    53,   173,   174,   173,   166,   121,   124,    29,
      31,    32,    33,    34,    36,    37,   164,    28,   159,   168,
      25,   154,    86,   120,   120,   120,   120,   120,   120,   120,
      46,    83,    85,    86,   134,   135,   120,     3,   137,   131,
       4,     3,   121,   124,   121,   121,   156,    30,    30,     3,
      26,    27,   153,     6,     6,     6,     6,     6,    55,    56,
      57,    58,    62,    63,    82,    55,    56,    57,    58,    62,
      63,    82,    86,    84,   135,   137,   121,   124,   174,   157,
     157,   120,   166,   168,    40,   144,   121,   121,   121,   124,
     121,   121,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   121,     3,    35,   137,
      26,    41,   148,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   168,   121,
     145,   146,   168,   168,    42,   149,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   124,    43,    44,   147,   168,   146
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   125,   126,   127,   127,   128,   128,   128,   128,   128,
     129,   129,   129,   130,   130,   131,   131,   132,   132,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   134,   134,   135,   135,   135,   135,   136,   136,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   141,   142,
     143,   144,   144,   145,   145,   146,   147,   147,   148,   148,
     149,   149,   150,   150,   151,   151,   152,   152,   153,   153,
     154,   154,   155,   155,   156,   156,   157,   158,   158,   159,
     159,   159,   160,   160,   161,   161,   162,   163,   163,   164,
     164,   164,   164,   164,   164,   164,   165,   165,   166,   166,
     167,   167,   168,   168,   168,   168,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   170,   170,   170,   170,   171,
     171,   171,   171,   171,   171,   171,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   173,   173,
     174,   174,   174,   175,   176,   176,   177,   177,   178,   178
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       4,     4,     7,     1,     3,     1,     1,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     6,     4,     1,     4,     4,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     1,     2,     2,     1,     1,     2,     5,     4,     1,
       3,     4,     4,     8,     8,     1,     1,     3,     3,     2,
      10,     3,     0,     1,     3,     2,     1,     1,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     1,     3,     1,     1,     2,     1,     3,     2,
       5,     0,     2,     0,     1,     3,     5,     4,     6,     1,
       1,     1,     1,     1,     1,     0,     2,     2,     1,     3,
       3,     1,     3,     1,     1,     1,     3,     4,     5,     4,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       2,     2,     1,     1,     2,     0,     3,     0,     1,     0
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
#line 194 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1632 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 194 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1640 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 138 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1654 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 128 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 1668 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 187 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 1679 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 198 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 1688 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1702 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 158 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 1716 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 217 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 1726 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1734 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1742 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1751 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1759 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1767 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 148 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 1781 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1790 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1799 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1808 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 178 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 1821 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 207 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 1830 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 168 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 1844 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 168 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 1858 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->statement_;
    delete ((*yyvaluep).with_expr_t);
}
#line 1868 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 212 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 1877 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 148 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 1891 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1899 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1907 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1915 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1923 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1931 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 203 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 1939 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 194 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1947 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
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
#line 67 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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
#line 312 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2270 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 316 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new Vector<BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2281 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 322 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2292 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 329 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2298 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 330 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2304 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 331 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2310 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 332 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2316 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 334 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2322 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 10: /* create_statement: CREATE SCHEMA if_not_exists IDENTIFIER  */
#line 341 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                          {
    (yyval.create_stmt) = new CreateStatement();
    UniquePtr<CreateSchemaInfo> create_schema_info = MakeUnique<CreateSchemaInfo>();
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.create_stmt)->create_info_ = std::move(create_schema_info);
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2335 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 11: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 351 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2353 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 12: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 366 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    if(result->IsError()) {
        printf("Error happened, release memory\n");
        if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
            free((yyvsp[-3].table_name_t)->schema_name_ptr_);
        }
        free((yyvsp[-3].table_name_t)->table_name_ptr_);
        delete((yyvsp[-3].table_name_t));
        delete((yyvsp[-1].table_element_array_t));
        YYERROR;
    }

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
#line 2392 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 13: /* table_element_array: table_element  */
#line 401 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    {
    (yyval.table_element_array_t) = new Vector<TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 2401 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 14: /* table_element_array: table_element_array ',' table_element  */
#line 405 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 2410 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 15: /* table_element: table_column  */
#line 411 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 2418 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 16: /* table_element: table_constraint  */
#line 414 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 2426 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 17: /* table_column: IDENTIFIER column_type  */
#line 420 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    if(result->IsError()) {
        free((yyvsp[-1].str_value));
    }

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
#line 2473 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 18: /* table_column: IDENTIFIER column_type column_constraints  */
#line 462 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            {
    if(result->IsError()) {
        free((yyvsp[-2].str_value));
        delete((yyvsp[0].column_constraints_t));
    }

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
#line 2522 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 19: /* column_type: BOOLEAN  */
#line 508 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kBoolean}; }
#line 2528 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 20: /* column_type: TINYINT  */
#line 509 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kTinyInt}; }
#line 2534 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 21: /* column_type: SMALLINT  */
#line 510 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kSmallInt}; }
#line 2540 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 22: /* column_type: INTEGER  */
#line 511 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kInteger}; }
#line 2546 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 23: /* column_type: BIGINT  */
#line 512 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kBigInt}; }
#line 2552 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 24: /* column_type: HUGEINT  */
#line 513 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kHugeInt}; }
#line 2558 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 25: /* column_type: FLOAT  */
#line 514 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 2564 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 26: /* column_type: REAL  */
#line 515 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kFloat}; }
#line 2570 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 27: /* column_type: DOUBLE  */
#line 516 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kDouble}; }
#line 2576 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 28: /* column_type: DATE  */
#line 517 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kDate}; }
#line 2582 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 29: /* column_type: TIME  */
#line 518 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kTime}; }
#line 2588 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 30: /* column_type: DATETIME  */
#line 519 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           { (yyval.column_type_t) = ColumnType{LogicalType::kDateTime}; }
#line 2594 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 31: /* column_type: TIMESTAMP  */
#line 520 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
            { (yyval.column_type_t) = ColumnType{LogicalType::kTimestamp}; }
#line 2600 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 32: /* column_type: UUID  */
#line 521 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kUuid}; }
#line 2606 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 33: /* column_type: POINT  */
#line 522 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        { (yyval.column_type_t) = ColumnType{LogicalType::kPoint}; }
#line 2612 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 34: /* column_type: LINE  */
#line 523 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLine}; }
#line 2618 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 35: /* column_type: LSEG  */
#line 524 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kLineSeg}; }
#line 2624 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 36: /* column_type: BOX  */
#line 525 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      { (yyval.column_type_t) = ColumnType{LogicalType::kBox}; }
#line 2630 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 37: /* column_type: PATH  */
#line 526 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       { (yyval.column_type_t) = ColumnType{LogicalType::kPath}; }
#line 2636 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 38: /* column_type: POLYGON  */
#line 527 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kPolygon}; }
#line 2642 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 39: /* column_type: CIRCLE  */
#line 528 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         { (yyval.column_type_t) = ColumnType{LogicalType::kCircle}; }
#line 2648 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 40: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 530 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kChar, (yyvsp[-1].long_value)}; }
#line 2654 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 41: /* column_type: VARCHAR '(' LONG_VALUE ')'  */
#line 531 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 2660 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 42: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 532 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 2666 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 43: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 533 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, (yyvsp[-1].long_value), 0}; }
#line 2672 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 44: /* column_type: DECIMAL  */
#line 534 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
          { (yyval.column_type_t) = ColumnType{LogicalType::kDecimal64, 0, 0, 0}; }
#line 2678 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 45: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 535 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          { (yyval.column_type_t) = ColumnType{LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 2684 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 46: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 536 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            { (yyval.column_type_t) = ColumnType{LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 2690 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 47: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 537 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 2696 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 48: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 538 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 2702 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 49: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 539 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                            { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 2708 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 50: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 540 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 2714 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 51: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 541 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 2720 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 52: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 542 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 2726 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 53: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 543 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                          { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 2732 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 54: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 544 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                    { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemBit}; }
#line 2738 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 55: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 545 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt8}; }
#line 2744 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 56: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 546 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt16}; }
#line 2750 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 57: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 547 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt32}; }
#line 2756 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 58: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 548 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemInt64}; }
#line 2762 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 59: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 549 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                      { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemFloat}; }
#line 2768 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 60: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 550 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       { (yyval.column_type_t) = ColumnType{LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, kElemDouble}; }
#line 2774 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 61: /* column_constraints: column_constraint  */
#line 569 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.column_constraints_t) = new HashSet<ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 2783 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 62: /* column_constraints: column_constraints column_constraint  */
#line 573 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 2796 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 63: /* column_constraint: PRIMARY KEY  */
#line 582 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.column_constraint_t) = ConstraintType::kPrimaryKey;
}
#line 2804 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 64: /* column_constraint: UNIQUE  */
#line 585 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.column_constraint_t) = ConstraintType::kUnique;
}
#line 2812 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 65: /* column_constraint: NULLABLE  */
#line 588 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
    (yyval.column_constraint_t) = ConstraintType::kNull;
}
#line 2820 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 66: /* column_constraint: NOT NULLABLE  */
#line 591 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    (yyval.column_constraint_t) = ConstraintType::kNotNull;
}
#line 2828 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 67: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 595 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                        {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kPrimaryKey;
}
#line 2838 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 68: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 600 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_constraint_t) = new TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = ConstraintType::kUnique;
}
#line 2848 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 69: /* identifier_array: IDENTIFIER  */
#line 607 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              {
    (yyval.identifier_array_t) = new Vector<String>();
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 2858 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 70: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 612 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 2868 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 71: /* drop_statement: DROP SCHEMA if_exists IDENTIFIER  */
#line 623 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.drop_stmt) = new DropStatement();
    UniquePtr<DropSchemaInfo> drop_schema_info = MakeUnique<DropSchemaInfo>();
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.drop_stmt)->drop_info_ = std::move(drop_schema_info);
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? ConflictType::kIgnore : ConflictType::kError;
}
#line 2881 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 72: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 633 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 2899 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 73: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 651 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                           {
    if(result->IsError()) {
        if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
            free((yyvsp[-6].table_name_t)->schema_name_ptr_);
        }
        free((yyvsp[-6].table_name_t)->table_name_ptr_);
        delete((yyvsp[-6].table_name_t));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-1].copy_option_array);
        YYERROR;
    }

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
#line 2956 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 74: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 703 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                               {
    if(result->IsError()) {
        if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
            free((yyvsp[-6].table_name_t)->schema_name_ptr_);
        }
        free((yyvsp[-6].table_name_t)->table_name_ptr_);
        delete((yyvsp[-6].table_name_t));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-1].copy_option_array);
        YYERROR;
    }

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
#line 3013 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 75: /* select_statement: select_without_paren  */
#line 779 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3021 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 76: /* select_statement: select_with_paren  */
#line 782 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 77: /* select_with_paren: '(' select_without_paren ')'  */
#line 786 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3037 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 78: /* select_with_paren: '(' select_with_paren ')'  */
#line 789 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 3045 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 79: /* select_without_paren: with_clause select_clause  */
#line 793 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                 {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);

    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 3055 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 80: /* select_clause: SELECT distinct expr_array from_clause where_clause group_by_clause having_clause order_by_clause limit_expr offset_expr  */
#line 800 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                                                                         {
    (yyval.select_stmt) = new SelectStatement();
    (yyval.select_stmt)->select_list_ = (yyvsp[-7].expr_array_t);
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-8].bool_value);
    (yyval.select_stmt)->table_ref_ = (yyvsp[-6].table_reference_t);
    (yyval.select_stmt)->where_expr_ = (yyvsp[-5].expr_t);
    (yyval.select_stmt)->group_by_list_ = (yyvsp[-4].expr_array_t);
    (yyval.select_stmt)->having_expr_ = (yyvsp[-3].expr_t);
    (yyval.select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyval.select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyval.select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    if((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
}
#line 3076 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 81: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 817 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 3084 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 82: /* order_by_clause: %empty  */
#line 820 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 3092 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 83: /* order_by_expr_list: order_by_expr  */
#line 824 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new Vector<OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 3101 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 84: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 828 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 3110 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 85: /* order_by_expr: expr order_by_type  */
#line 833 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyval.order_by_expr_t) = new OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 3120 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 86: /* order_by_type: ASC  */
#line 839 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.order_by_type_t) = kAsc;
}
#line 3128 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 87: /* order_by_type: DESC  */
#line 842 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.order_by_type_t) = kDesc;
}
#line 3136 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 88: /* limit_expr: LIMIT expr  */
#line 846 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3144 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 89: /* limit_expr: %empty  */
#line 850 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3150 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 90: /* offset_expr: OFFSET expr  */
#line 852 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3158 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 91: /* offset_expr: %empty  */
#line 856 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
{   (yyval.expr_t) = nullptr; }
#line 3164 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 92: /* distinct: DISTINCT  */
#line 858 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 3172 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 93: /* distinct: %empty  */
#line 861 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.bool_value) = false;
}
#line 3180 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 94: /* from_clause: FROM table_reference  */
#line 865 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3188 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 95: /* from_clause: %empty  */
#line 868 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 3196 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 96: /* where_clause: WHERE expr  */
#line 872 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3204 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 97: /* where_clause: %empty  */
#line 875 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3212 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 98: /* having_clause: HAVING expr  */
#line 879 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3220 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 99: /* having_clause: %empty  */
#line 882 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 3228 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 100: /* group_by_clause: GROUP BY expr_array  */
#line 886 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 3236 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 101: /* group_by_clause: %empty  */
#line 889 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 3244 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 102: /* table_reference: table_reference_unit  */
#line 897 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 3252 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 103: /* table_reference: table_reference ',' table_reference_unit  */
#line 900 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                           {
    CrossProductReference* cross_product_ref = new CrossProductReference();
    cross_product_ref->left_ = (yyvsp[-2].table_reference_t);
    cross_product_ref->right_ = (yyvsp[0].table_reference_t);

    (yyval.table_reference_t) = cross_product_ref;
}
#line 3264 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 106: /* table_reference_name: table_name table_alias  */
#line 911 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3282 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 107: /* table_name: IDENTIFIER  */
#line 931 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 3293 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 108: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 937 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new TableName();
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 3305 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 109: /* table_alias: AS IDENTIFIER  */
#line 946 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 3314 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 110: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 950 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    (yyval.table_alias_t) = new TableAlias();
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 3324 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 111: /* table_alias: %empty  */
#line 955 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 3332 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 112: /* with_clause: WITH with_expr_list  */
#line 962 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 3340 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 113: /* with_clause: %empty  */
#line 965 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 3348 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 114: /* with_expr_list: with_expr  */
#line 969 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                          {
    (yyval.with_expr_list_t) = new Vector<WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 3357 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 115: /* with_expr_list: with_expr_list ',' with_expr  */
#line 972 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 3366 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 116: /* with_expr: IDENTIFIER AS '(' select_clause ')'  */
#line 977 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                               {
    (yyval.with_expr_t) = new WithExpr();
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->statement_ = (yyvsp[-1].select_stmt);
}
#line 3377 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 117: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 995 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                    {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 3389 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 118: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1002 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                                                   {
    JoinReference* join_reference = new JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 3402 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 119: /* join_type: INNER  */
#line 1016 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    (yyval.join_type_t) = JoinType::kInner;
}
#line 3410 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 120: /* join_type: LEFT  */
#line 1019 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kLeft;
}
#line 3418 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 121: /* join_type: RIGHT  */
#line 1022 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kRight;
}
#line 3426 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 122: /* join_type: OUTER  */
#line 1025 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 3434 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 123: /* join_type: FULL  */
#line 1028 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.join_type_t) = JoinType::kFull;
}
#line 3442 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 124: /* join_type: CROSS  */
#line 1031 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    (yyval.join_type_t) = JoinType::kCross;
}
#line 3450 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 125: /* join_type: %empty  */
#line 1034 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
}
#line 3457 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 126: /* show_statement: SHOW TABLES  */
#line 1040 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyval.show_stmt) = new ShowStatement();
    (yyval.show_stmt)->show_type_ = ShowStmtType::kTables;
}
#line 3466 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 127: /* show_statement: DESCRIBE table_name  */
#line 1044 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
#line 3482 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 128: /* expr_array: expr_alias  */
#line 1060 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                        {
    (yyval.expr_array_t) = new Vector<ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 3491 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 129: /* expr_array: expr_array ',' expr_alias  */
#line 1064 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 3500 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 130: /* expr_alias: expr AS IDENTIFIER  */
#line 1069 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 3510 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 131: /* expr_alias: expr  */
#line 1074 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3518 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 132: /* expr: '(' expr ')'  */
#line 1078 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
      (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 3526 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 136: /* function_expr: IDENTIFIER '(' ')'  */
#line 1085 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 3538 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 137: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1092 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 3550 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 138: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1099 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                         {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 3563 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 139: /* function_expr: expr IS NOT NULLABLE  */
#line 1107 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3575 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 140: /* function_expr: expr IS NULLABLE  */
#line 1114 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3587 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 141: /* function_expr: '-' expr  */
#line 1121 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "-";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 3599 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 142: /* function_expr: '+' expr  */
#line 1128 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
           {
  FunctionExpr* func_expr = new FunctionExpr();
  func_expr->func_name_ = "+";
  func_expr->arguments_ = new Vector<ParsedExpr*>();
  func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
  (yyval.expr_t) = func_expr;
}
#line 3611 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 143: /* function_expr: expr '-' expr  */
#line 1135 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3624 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 144: /* function_expr: expr '+' expr  */
#line 1143 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3637 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 145: /* function_expr: expr '*' expr  */
#line 1151 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3650 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 146: /* function_expr: expr '/' expr  */
#line 1159 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3663 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 147: /* function_expr: expr '%' expr  */
#line 1167 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3676 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 148: /* function_expr: expr '=' expr  */
#line 1175 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3689 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 149: /* function_expr: expr EQUAL expr  */
#line 1183 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3702 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 150: /* function_expr: expr NOT_EQ expr  */
#line 1191 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3715 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 151: /* function_expr: expr '<' expr  */
#line 1199 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3728 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 152: /* function_expr: expr '>' expr  */
#line 1207 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3741 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 153: /* function_expr: expr LESS_EQ expr  */
#line 1215 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3754 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 154: /* function_expr: expr GREATER_EQ expr  */
#line 1223 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                       {
    FunctionExpr* func_expr = new FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new Vector<ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 3767 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 155: /* column_expr: IDENTIFIER  */
#line 1232 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 3777 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 156: /* column_expr: column_expr '.' IDENTIFIER  */
#line 1237 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                             {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 3787 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 157: /* column_expr: '*'  */
#line 1242 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
      {
    ColumnExpr* column_expr = new ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 3797 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 158: /* column_expr: column_expr '.' '*'  */
#line 1247 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ColumnExpr* column_expr = (ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 3811 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 159: /* constant_expr: STRING  */
#line 1257 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                      {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 3821 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 160: /* constant_expr: TRUE  */
#line 1262 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
       {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.expr_t) = const_expr;
}
#line 3831 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 161: /* constant_expr: FALSE  */
#line 1267 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
        {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.expr_t) = const_expr;
}
#line 3841 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 162: /* constant_expr: DOUBLE_VALUE  */
#line 1272 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
               {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kFloat);
    const_expr->float_value_ = (yyvsp[0].double_value);
    (yyval.expr_t) = const_expr;
}
#line 3851 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 163: /* constant_expr: LONG_VALUE  */
#line 1277 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
             {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3861 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 164: /* constant_expr: DATE STRING  */
#line 1282 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
              {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.expr_t) = const_expr;
}
#line 3871 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 165: /* constant_expr: INTERVAL interval_expr  */
#line 1287 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 3879 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 166: /* interval_expr: LONG_VALUE SECONDS  */
#line 1291 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3890 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 167: /* interval_expr: LONG_VALUE SECOND  */
#line 1297 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3901 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 168: /* interval_expr: LONG_VALUE MINUTES  */
#line 1303 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3912 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 169: /* interval_expr: LONG_VALUE MINUTE  */
#line 1309 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3923 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 170: /* interval_expr: LONG_VALUE HOURS  */
#line 1315 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3934 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 171: /* interval_expr: LONG_VALUE HOUR  */
#line 1321 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3945 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 172: /* interval_expr: LONG_VALUE DAYS  */
#line 1327 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3956 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 173: /* interval_expr: LONG_VALUE DAY  */
#line 1333 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                 {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3967 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 174: /* interval_expr: LONG_VALUE MONTHS  */
#line 1339 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                    {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3978 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 175: /* interval_expr: LONG_VALUE MONTH  */
#line 1345 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 3989 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 176: /* interval_expr: LONG_VALUE YEARS  */
#line 1351 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4000 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 177: /* interval_expr: LONG_VALUE YEAR  */
#line 1357 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                  {
    ConstantExpr* const_expr = new ConstantExpr(LiteralType::kInterval);
    const_expr->interval_type_ = IntervalExprType::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.expr_t) = const_expr;
}
#line 4011 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 178: /* copy_option_list: copy_option  */
#line 1368 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                               {
    (yyval.copy_option_array) = new Vector<CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 4020 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 179: /* copy_option_list: copy_option_list ',' copy_option  */
#line 1372 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 4029 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 180: /* copy_option: FORMAT IDENTIFIER  */
#line 1377 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
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
    }
}
#line 4046 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 181: /* copy_option: DELIMITER STRING  */
#line 1389 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 4057 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 182: /* copy_option: HEADER  */
#line 1395 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
         {
    (yyval.copy_option_t) = new CopyOption();
    (yyval.copy_option_t)->option_type_ = CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 4067 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 183: /* file_path: STRING  */
#line 1401 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 4075 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 184: /* if_exists: IF EXISTS  */
#line 1405 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                     { (yyval.bool_value) = true; }
#line 4081 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 185: /* if_exists: %empty  */
#line 1406 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 4087 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 186: /* if_not_exists: IF NOT EXISTS  */
#line 1408 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
                              { (yyval.bool_value) = true; }
#line 4093 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;

  case 187: /* if_not_exists: %empty  */
#line 1409 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"
  { (yyval.bool_value) = false; }
#line 4099 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"
    break;


#line 4103 "/home/jinhai/Documents/development/infinity/src/parser/parser.cpp"

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

#line 1415 "/home/jinhai/Documents/development/infinity/src/parser/parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = String(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d\n", msg, llocp->first_column);
}

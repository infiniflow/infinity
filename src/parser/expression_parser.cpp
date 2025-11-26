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
#define YYSTYPE EXPRESSIONSTYPE
#define YYLTYPE EXPRESSIONLTYPE
/* Substitute the variable and function names.  */
#define yyparse expressionparse
#define yylex expressionlex
#define yyerror expressionerror
#define yydebug expressiondebug
#define yynerrs expressionnerrs

/* First part of user prologue.  */
#line 2 "expression_parser.y"

#include "expression_parser.h"
#include "expression_lexer.h"
#include "parser_helper.h"

void expressionerror(YYLTYPE *llocp, void *lexer, infinity::ExpressionParserResult *result, const char *msg);

#line 86 "expression_parser.cpp"

#ifndef YY_CAST
#ifdef __cplusplus
#define YY_CAST(Type, Val) static_cast<Type>(Val)
#define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type>(Val)
#else
#define YY_CAST(Type, Val) ((Type)(Val))
#define YY_REINTERPRET_CAST(Type, Val) ((Type)(Val))
#endif
#endif
#ifndef YY_NULLPTR
#if defined __cplusplus
#if 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#else
#define YY_NULLPTR ((void *)0)
#endif
#endif

#include "expression_parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t {
    YYSYMBOL_YYEMPTY = -2,
    YYSYMBOL_YYEOF = 0,                        /* "end of file"  */
    YYSYMBOL_YYerror = 1,                      /* error  */
    YYSYMBOL_YYUNDEF = 2,                      /* "invalid token"  */
    YYSYMBOL_IDENTIFIER = 3,                   /* IDENTIFIER  */
    YYSYMBOL_STRING = 4,                       /* STRING  */
    YYSYMBOL_DOUBLE_VALUE = 5,                 /* DOUBLE_VALUE  */
    YYSYMBOL_LONG_VALUE = 6,                   /* LONG_VALUE  */
    YYSYMBOL_CREATE = 7,                       /* CREATE  */
    YYSYMBOL_SELECT = 8,                       /* SELECT  */
    YYSYMBOL_INSERT = 9,                       /* INSERT  */
    YYSYMBOL_DROP = 10,                        /* DROP  */
    YYSYMBOL_UPDATE = 11,                      /* UPDATE  */
    YYSYMBOL_DELETE = 12,                      /* DELETE  */
    YYSYMBOL_COPY = 13,                        /* COPY  */
    YYSYMBOL_SET = 14,                         /* SET  */
    YYSYMBOL_EXPLAIN = 15,                     /* EXPLAIN  */
    YYSYMBOL_SHOW = 16,                        /* SHOW  */
    YYSYMBOL_ALTER = 17,                       /* ALTER  */
    YYSYMBOL_EXECUTE = 18,                     /* EXECUTE  */
    YYSYMBOL_PREPARE = 19,                     /* PREPARE  */
    YYSYMBOL_UNION = 20,                       /* UNION  */
    YYSYMBOL_ALL = 21,                         /* ALL  */
    YYSYMBOL_INTERSECT = 22,                   /* INTERSECT  */
    YYSYMBOL_COMPACT = 23,                     /* COMPACT  */
    YYSYMBOL_EXCEPT = 24,                      /* EXCEPT  */
    YYSYMBOL_FLUSH = 25,                       /* FLUSH  */
    YYSYMBOL_USE = 26,                         /* USE  */
    YYSYMBOL_OPTIMIZE = 27,                    /* OPTIMIZE  */
    YYSYMBOL_PROPERTIES = 28,                  /* PROPERTIES  */
    YYSYMBOL_DATABASE = 29,                    /* DATABASE  */
    YYSYMBOL_TABLE = 30,                       /* TABLE  */
    YYSYMBOL_COLLECTION = 31,                  /* COLLECTION  */
    YYSYMBOL_TABLES = 32,                      /* TABLES  */
    YYSYMBOL_INTO = 33,                        /* INTO  */
    YYSYMBOL_VALUES = 34,                      /* VALUES  */
    YYSYMBOL_AST = 35,                         /* AST  */
    YYSYMBOL_PIPELINE = 36,                    /* PIPELINE  */
    YYSYMBOL_RAW = 37,                         /* RAW  */
    YYSYMBOL_LOGICAL = 38,                     /* LOGICAL  */
    YYSYMBOL_PHYSICAL = 39,                    /* PHYSICAL  */
    YYSYMBOL_FRAGMENT = 40,                    /* FRAGMENT  */
    YYSYMBOL_VIEW = 41,                        /* VIEW  */
    YYSYMBOL_INDEX = 42,                       /* INDEX  */
    YYSYMBOL_ANALYZE = 43,                     /* ANALYZE  */
    YYSYMBOL_VIEWS = 44,                       /* VIEWS  */
    YYSYMBOL_DATABASES = 45,                   /* DATABASES  */
    YYSYMBOL_SEGMENT = 46,                     /* SEGMENT  */
    YYSYMBOL_SEGMENTS = 47,                    /* SEGMENTS  */
    YYSYMBOL_BLOCK = 48,                       /* BLOCK  */
    YYSYMBOL_BLOCKS = 49,                      /* BLOCKS  */
    YYSYMBOL_COLUMNS = 50,                     /* COLUMNS  */
    YYSYMBOL_INDEXES = 51,                     /* INDEXES  */
    YYSYMBOL_GROUP = 52,                       /* GROUP  */
    YYSYMBOL_BY = 53,                          /* BY  */
    YYSYMBOL_HAVING = 54,                      /* HAVING  */
    YYSYMBOL_AS = 55,                          /* AS  */
    YYSYMBOL_NATURAL = 56,                     /* NATURAL  */
    YYSYMBOL_JOIN = 57,                        /* JOIN  */
    YYSYMBOL_LEFT = 58,                        /* LEFT  */
    YYSYMBOL_RIGHT = 59,                       /* RIGHT  */
    YYSYMBOL_OUTER = 60,                       /* OUTER  */
    YYSYMBOL_FULL = 61,                        /* FULL  */
    YYSYMBOL_ON = 62,                          /* ON  */
    YYSYMBOL_INNER = 63,                       /* INNER  */
    YYSYMBOL_CROSS = 64,                       /* CROSS  */
    YYSYMBOL_DISTINCT = 65,                    /* DISTINCT  */
    YYSYMBOL_WHERE = 66,                       /* WHERE  */
    YYSYMBOL_ORDER = 67,                       /* ORDER  */
    YYSYMBOL_LIMIT = 68,                       /* LIMIT  */
    YYSYMBOL_OFFSET = 69,                      /* OFFSET  */
    YYSYMBOL_ASC = 70,                         /* ASC  */
    YYSYMBOL_DESC = 71,                        /* DESC  */
    YYSYMBOL_IF = 72,                          /* IF  */
    YYSYMBOL_NOT = 73,                         /* NOT  */
    YYSYMBOL_EXISTS = 74,                      /* EXISTS  */
    YYSYMBOL_IN = 75,                          /* IN  */
    YYSYMBOL_FROM = 76,                        /* FROM  */
    YYSYMBOL_TO = 77,                          /* TO  */
    YYSYMBOL_WITH = 78,                        /* WITH  */
    YYSYMBOL_DELIMITER = 79,                   /* DELIMITER  */
    YYSYMBOL_FORMAT = 80,                      /* FORMAT  */
    YYSYMBOL_HEADER = 81,                      /* HEADER  */
    YYSYMBOL_CAST = 82,                        /* CAST  */
    YYSYMBOL_END = 83,                         /* END  */
    YYSYMBOL_CASE = 84,                        /* CASE  */
    YYSYMBOL_ELSE = 85,                        /* ELSE  */
    YYSYMBOL_THEN = 86,                        /* THEN  */
    YYSYMBOL_WHEN = 87,                        /* WHEN  */
    YYSYMBOL_BOOLEAN = 88,                     /* BOOLEAN  */
    YYSYMBOL_INTEGER = 89,                     /* INTEGER  */
    YYSYMBOL_INT = 90,                         /* INT  */
    YYSYMBOL_TINYINT = 91,                     /* TINYINT  */
    YYSYMBOL_SMALLINT = 92,                    /* SMALLINT  */
    YYSYMBOL_BIGINT = 93,                      /* BIGINT  */
    YYSYMBOL_HUGEINT = 94,                     /* HUGEINT  */
    YYSYMBOL_VARCHAR = 95,                     /* VARCHAR  */
    YYSYMBOL_FLOAT = 96,                       /* FLOAT  */
    YYSYMBOL_DOUBLE = 97,                      /* DOUBLE  */
    YYSYMBOL_REAL = 98,                        /* REAL  */
    YYSYMBOL_DECIMAL = 99,                     /* DECIMAL  */
    YYSYMBOL_DATE = 100,                       /* DATE  */
    YYSYMBOL_TIME = 101,                       /* TIME  */
    YYSYMBOL_DATETIME = 102,                   /* DATETIME  */
    YYSYMBOL_TIMESTAMP = 103,                  /* TIMESTAMP  */
    YYSYMBOL_UUID = 104,                       /* UUID  */
    YYSYMBOL_POINT = 105,                      /* POINT  */
    YYSYMBOL_LINE = 106,                       /* LINE  */
    YYSYMBOL_LSEG = 107,                       /* LSEG  */
    YYSYMBOL_BOX = 108,                        /* BOX  */
    YYSYMBOL_PATH = 109,                       /* PATH  */
    YYSYMBOL_POLYGON = 110,                    /* POLYGON  */
    YYSYMBOL_CIRCLE = 111,                     /* CIRCLE  */
    YYSYMBOL_BLOB = 112,                       /* BLOB  */
    YYSYMBOL_BITMAP = 113,                     /* BITMAP  */
    YYSYMBOL_EMBEDDING = 114,                  /* EMBEDDING  */
    YYSYMBOL_VECTOR = 115,                     /* VECTOR  */
    YYSYMBOL_BIT = 116,                        /* BIT  */
    YYSYMBOL_SPARSE = 117,                     /* SPARSE  */
    YYSYMBOL_PRIMARY = 118,                    /* PRIMARY  */
    YYSYMBOL_KEY = 119,                        /* KEY  */
    YYSYMBOL_UNIQUE = 120,                     /* UNIQUE  */
    YYSYMBOL_NULLABLE = 121,                   /* NULLABLE  */
    YYSYMBOL_IS = 122,                         /* IS  */
    YYSYMBOL_TRUE = 123,                       /* TRUE  */
    YYSYMBOL_FALSE = 124,                      /* FALSE  */
    YYSYMBOL_INTERVAL = 125,                   /* INTERVAL  */
    YYSYMBOL_SECOND = 126,                     /* SECOND  */
    YYSYMBOL_SECONDS = 127,                    /* SECONDS  */
    YYSYMBOL_MINUTE = 128,                     /* MINUTE  */
    YYSYMBOL_MINUTES = 129,                    /* MINUTES  */
    YYSYMBOL_HOUR = 130,                       /* HOUR  */
    YYSYMBOL_HOURS = 131,                      /* HOURS  */
    YYSYMBOL_DAY = 132,                        /* DAY  */
    YYSYMBOL_DAYS = 133,                       /* DAYS  */
    YYSYMBOL_MONTH = 134,                      /* MONTH  */
    YYSYMBOL_MONTHS = 135,                     /* MONTHS  */
    YYSYMBOL_YEAR = 136,                       /* YEAR  */
    YYSYMBOL_YEARS = 137,                      /* YEARS  */
    YYSYMBOL_EQUAL = 138,                      /* EQUAL  */
    YYSYMBOL_NOT_EQ = 139,                     /* NOT_EQ  */
    YYSYMBOL_LESS_EQ = 140,                    /* LESS_EQ  */
    YYSYMBOL_GREATER_EQ = 141,                 /* GREATER_EQ  */
    YYSYMBOL_BETWEEN = 142,                    /* BETWEEN  */
    YYSYMBOL_AND = 143,                        /* AND  */
    YYSYMBOL_OR = 144,                         /* OR  */
    YYSYMBOL_EXTRACT = 145,                    /* EXTRACT  */
    YYSYMBOL_LIKE = 146,                       /* LIKE  */
    YYSYMBOL_DATA = 147,                       /* DATA  */
    YYSYMBOL_LOG = 148,                        /* LOG  */
    YYSYMBOL_BUFFER = 149,                     /* BUFFER  */
    YYSYMBOL_KNN = 150,                        /* KNN  */
    YYSYMBOL_USING = 151,                      /* USING  */
    YYSYMBOL_SESSION = 152,                    /* SESSION  */
    YYSYMBOL_GLOBAL = 153,                     /* GLOBAL  */
    YYSYMBOL_OFF = 154,                        /* OFF  */
    YYSYMBOL_EXPORT = 155,                     /* EXPORT  */
    YYSYMBOL_PROFILE = 156,                    /* PROFILE  */
    YYSYMBOL_CONFIGS = 157,                    /* CONFIGS  */
    YYSYMBOL_PROFILES = 158,                   /* PROFILES  */
    YYSYMBOL_STATUS = 159,                     /* STATUS  */
    YYSYMBOL_VAR = 160,                        /* VAR  */
    YYSYMBOL_SEARCH = 161,                     /* SEARCH  */
    YYSYMBOL_MATCH = 162,                      /* MATCH  */
    YYSYMBOL_QUERY = 163,                      /* QUERY  */
    YYSYMBOL_FUSION = 164,                     /* FUSION  */
    YYSYMBOL_NUMBER = 165,                     /* NUMBER  */
    YYSYMBOL_166_ = 166,                       /* '='  */
    YYSYMBOL_167_ = 167,                       /* '<'  */
    YYSYMBOL_168_ = 168,                       /* '>'  */
    YYSYMBOL_169_ = 169,                       /* '+'  */
    YYSYMBOL_170_ = 170,                       /* '-'  */
    YYSYMBOL_171_ = 171,                       /* '*'  */
    YYSYMBOL_172_ = 172,                       /* '/'  */
    YYSYMBOL_173_ = 173,                       /* '%'  */
    YYSYMBOL_174_ = 174,                       /* ','  */
    YYSYMBOL_175_ = 175,                       /* '('  */
    YYSYMBOL_176_ = 176,                       /* ')'  */
    YYSYMBOL_177_ = 177,                       /* '.'  */
    YYSYMBOL_178_ = 178,                       /* ']'  */
    YYSYMBOL_179_ = 179,                       /* '['  */
    YYSYMBOL_YYACCEPT = 180,                   /* $accept  */
    YYSYMBOL_input_pattern = 181,              /* input_pattern  */
    YYSYMBOL_expr_array = 182,                 /* expr_array  */
    YYSYMBOL_expr_alias = 183,                 /* expr_alias  */
    YYSYMBOL_expr = 184,                       /* expr  */
    YYSYMBOL_operand = 185,                    /* operand  */
    YYSYMBOL_match_expr = 186,                 /* match_expr  */
    YYSYMBOL_query_expr = 187,                 /* query_expr  */
    YYSYMBOL_fusion_expr = 188,                /* fusion_expr  */
    YYSYMBOL_function_expr = 189,              /* function_expr  */
    YYSYMBOL_conjunction_expr = 190,           /* conjunction_expr  */
    YYSYMBOL_between_expr = 191,               /* between_expr  */
    YYSYMBOL_in_expr = 192,                    /* in_expr  */
    YYSYMBOL_column_expr = 193,                /* column_expr  */
    YYSYMBOL_constant_expr = 194,              /* constant_expr  */
    YYSYMBOL_long_array_expr = 195,            /* long_array_expr  */
    YYSYMBOL_unclosed_long_array_expr = 196,   /* unclosed_long_array_expr  */
    YYSYMBOL_double_array_expr = 197,          /* double_array_expr  */
    YYSYMBOL_unclosed_double_array_expr = 198, /* unclosed_double_array_expr  */
    YYSYMBOL_interval_expr = 199               /* interval_expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

/* Unqualified %code blocks.  */
#line 78 "expression_parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 327 "expression_parser.cpp"

#ifdef short
#undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
#include <limits.h> /* INFRINGES ON USER NAME SPACE */
#if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#define YY_STDINT_H
#endif
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
#undef UINT_LEAST8_MAX
#undef UINT_LEAST16_MAX
#define UINT_LEAST8_MAX 255
#define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
#if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#define YYPTRDIFF_T __PTRDIFF_TYPE__
#define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
#elif defined PTRDIFF_MAX
#ifndef ptrdiff_t
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#endif
#define YYPTRDIFF_T ptrdiff_t
#define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
#else
#define YYPTRDIFF_T long
#define YYPTRDIFF_MAXIMUM LONG_MAX
#endif
#endif

#ifndef YYSIZE_T
#ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
#elif defined size_t
#define YYSIZE_T size_t
#elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#define YYSIZE_T size_t
#else
#define YYSIZE_T unsigned
#endif
#endif

#define YYSIZE_MAXIMUM YY_CAST(YYPTRDIFF_T, (YYPTRDIFF_MAXIMUM < YY_CAST(YYSIZE_T, -1) ? YYPTRDIFF_MAXIMUM : YY_CAST(YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST(YYPTRDIFF_T, sizeof(X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_(Msgid) dgettext("bison-runtime", Msgid)
#endif
#endif
#ifndef YY_
#define YY_(Msgid) Msgid
#endif
#endif

#ifndef YY_ATTRIBUTE_PURE
#if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_PURE __attribute__((__pure__))
#else
#define YY_ATTRIBUTE_PURE
#endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
#if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define YY_ATTRIBUTE_UNUSED
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
#define YY_USE(E) ((void)(E))
#else
#define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && !defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
#if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")
#else
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                                                                                          \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#endif
#define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
#define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
#define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && !defined __ICC && 6 <= __GNUC__
#define YY_IGNORE_USELESS_CAST_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")
#define YY_IGNORE_USELESS_CAST_END _Pragma("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_END
#endif

#define YY_ASSERT(E) ((void)(0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

#ifdef YYSTACK_USE_ALLOCA
#if YYSTACK_USE_ALLOCA
#ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
#elif defined __BUILTIN_VA_ARG_INCR
#include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#elif defined _AIX
#define YYSTACK_ALLOC __alloca
#elif defined _MSC_VER
#include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#define alloca _alloca
#else
#define YYSTACK_ALLOC alloca
#if !defined _ALLOCA_H && !defined EXIT_SUCCESS
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
/* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#endif
#endif
#endif

#ifdef YYSTACK_ALLOC
/* Pacify GCC's 'empty if-body' warning.  */
#define YYSTACK_FREE(Ptr)                                                                                                                            \
    do { /* empty */                                                                                                                                 \
        ;                                                                                                                                            \
    } while (0)
#ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
   and a page size can be as small as 4096 bytes.  So we cannot safely
   invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
   to allow for a few compiler-allocated temporary stack slots.  */
#define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#endif
#else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
#ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#endif
#if (defined __cplusplus && !defined EXIT_SUCCESS && !((defined YYMALLOC || defined malloc) && (defined YYFREE || defined free)))
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#if !defined malloc && !defined EXIT_SUCCESS
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#ifndef YYFREE
#define YYFREE free
#if !defined free && !defined EXIT_SUCCESS
void free(void *); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#endif
#endif /* 1 */

#if (!defined yyoverflow && (!defined __cplusplus || (defined EXPRESSIONLTYPE_IS_TRIVIAL && EXPRESSIONLTYPE_IS_TRIVIAL &&                            \
                                                      defined EXPRESSIONSTYPE_IS_TRIVIAL && EXPRESSIONSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc {
    yy_state_t yyss_alloc;
    YYSTYPE yyvs_alloc;
    YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
#define YYSTACK_GAP_MAXIMUM (YYSIZEOF(union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
#define YYSTACK_BYTES(N) ((N) * (YYSIZEOF(yy_state_t) + YYSIZEOF(YYSTYPE) + YYSIZEOF(YYLTYPE)) + 2 * YYSTACK_GAP_MAXIMUM)

#define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
#define YYSTACK_RELOCATE(Stack_alloc, Stack)                                                                                                         \
    do {                                                                                                                                             \
        YYPTRDIFF_T yynewbytes;                                                                                                                      \
        YYCOPY(&yyptr->Stack_alloc, Stack, yysize);                                                                                                  \
        Stack = &yyptr->Stack_alloc;                                                                                                                 \
        yynewbytes = yystacksize * YYSIZEOF(*Stack) + YYSTACK_GAP_MAXIMUM;                                                                           \
        yyptr += yynewbytes / YYSIZEOF(*yyptr);                                                                                                      \
    } while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
#ifndef YYCOPY
#if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY(Dst, Src, Count) __builtin_memcpy(Dst, Src, YY_CAST(YYSIZE_T, (Count)) * sizeof(*(Src)))
#else
#define YYCOPY(Dst, Src, Count)                                                                                                                      \
    do {                                                                                                                                             \
        YYPTRDIFF_T yyi;                                                                                                                             \
        for (yyi = 0; yyi < (Count); yyi++)                                                                                                          \
            (Dst)[yyi] = (Src)[yyi];                                                                                                                 \
    } while (0)
#endif
#endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 338

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 180
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 20
/* YYNRULES -- Number of rules.  */
#define YYNRULES 86
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 162

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 420

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] = {
    0,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   173, 2,   2,   175, 176, 171, 169, 174, 170, 177, 172, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   167, 166, 168, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   179, 2,   178, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   2,   3,   4,   5,
    6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
    35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,
    93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165};

#if EXPRESSIONDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {0,   164, 164, 172, 176, 181, 187, 191, 192, 193, 194, 196, 199, 202, 203, 204, 205, 206, 208, 216, 227, 233,
                                       242, 248, 257, 265, 273, 282, 289, 296, 303, 310, 317, 325, 333, 341, 349, 357, 365, 373, 381, 389, 397, 405,
                                       413, 443, 451, 460, 468, 477, 485, 491, 498, 505, 512, 517, 527, 532, 537, 542, 547, 552, 557, 562, 567, 572,
                                       575, 578, 581, 585, 589, 594, 599, 603, 608, 613, 619, 625, 631, 637, 643, 649, 655, 661, 667, 673, 679};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST(yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name(yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {"\"end of file\"",
                                      "error",
                                      "\"invalid token\"",
                                      "IDENTIFIER",
                                      "STRING",
                                      "DOUBLE_VALUE",
                                      "LONG_VALUE",
                                      "CREATE",
                                      "SELECT",
                                      "INSERT",
                                      "DROP",
                                      "UPDATE",
                                      "DELETE",
                                      "COPY",
                                      "SET",
                                      "EXPLAIN",
                                      "SHOW",
                                      "ALTER",
                                      "EXECUTE",
                                      "PREPARE",
                                      "UNION",
                                      "ALL",
                                      "INTERSECT",
                                      "COMPACT",
                                      "EXCEPT",
                                      "FLUSH",
                                      "USE",
                                      "OPTIMIZE",
                                      "PROPERTIES",
                                      "DATABASE",
                                      "TABLE",
                                      "COLLECTION",
                                      "TABLES",
                                      "INTO",
                                      "VALUES",
                                      "AST",
                                      "PIPELINE",
                                      "RAW",
                                      "LOGICAL",
                                      "PHYSICAL",
                                      "FRAGMENT",
                                      "VIEW",
                                      "INDEX",
                                      "ANALYZE",
                                      "VIEWS",
                                      "DATABASES",
                                      "SEGMENT",
                                      "SEGMENTS",
                                      "BLOCK",
                                      "BLOCKS",
                                      "COLUMNS",
                                      "INDEXES",
                                      "GROUP",
                                      "BY",
                                      "HAVING",
                                      "AS",
                                      "NATURAL",
                                      "JOIN",
                                      "LEFT",
                                      "RIGHT",
                                      "OUTER",
                                      "FULL",
                                      "ON",
                                      "INNER",
                                      "CROSS",
                                      "DISTINCT",
                                      "WHERE",
                                      "ORDER",
                                      "LIMIT",
                                      "OFFSET",
                                      "ASC",
                                      "DESC",
                                      "IF",
                                      "NOT",
                                      "EXISTS",
                                      "IN",
                                      "FROM",
                                      "TO",
                                      "WITH",
                                      "DELIMITER",
                                      "FORMAT",
                                      "HEADER",
                                      "CAST",
                                      "END",
                                      "CASE",
                                      "ELSE",
                                      "THEN",
                                      "WHEN",
                                      "BOOLEAN",
                                      "INTEGER",
                                      "INT",
                                      "TINYINT",
                                      "SMALLINT",
                                      "BIGINT",
                                      "HUGEINT",
                                      "VARCHAR",
                                      "FLOAT",
                                      "DOUBLE",
                                      "REAL",
                                      "DECIMAL",
                                      "DATE",
                                      "TIME",
                                      "DATETIME",
                                      "TIMESTAMP",
                                      "UUID",
                                      "POINT",
                                      "LINE",
                                      "LSEG",
                                      "BOX",
                                      "PATH",
                                      "POLYGON",
                                      "CIRCLE",
                                      "BLOB",
                                      "BITMAP",
                                      "EMBEDDING",
                                      "VECTOR",
                                      "BIT",
                                      "SPARSE",
                                      "PRIMARY",
                                      "KEY",
                                      "UNIQUE",
                                      "NULLABLE",
                                      "IS",
                                      "TRUE",
                                      "FALSE",
                                      "INTERVAL",
                                      "SECOND",
                                      "SECONDS",
                                      "MINUTE",
                                      "MINUTES",
                                      "HOUR",
                                      "HOURS",
                                      "DAY",
                                      "DAYS",
                                      "MONTH",
                                      "MONTHS",
                                      "YEAR",
                                      "YEARS",
                                      "EQUAL",
                                      "NOT_EQ",
                                      "LESS_EQ",
                                      "GREATER_EQ",
                                      "BETWEEN",
                                      "AND",
                                      "OR",
                                      "EXTRACT",
                                      "LIKE",
                                      "DATA",
                                      "LOG",
                                      "BUFFER",
                                      "KNN",
                                      "USING",
                                      "SESSION",
                                      "GLOBAL",
                                      "OFF",
                                      "EXPORT",
                                      "PROFILE",
                                      "CONFIGS",
                                      "PROFILES",
                                      "STATUS",
                                      "VAR",
                                      "SEARCH",
                                      "MATCH",
                                      "QUERY",
                                      "FUSION",
                                      "NUMBER",
                                      "'='",
                                      "'<'",
                                      "'>'",
                                      "'+'",
                                      "'-'",
                                      "'*'",
                                      "'/'",
                                      "'%'",
                                      "','",
                                      "'('",
                                      "')'",
                                      "'.'",
                                      "']'",
                                      "'['",
                                      "$accept",
                                      "input_pattern",
                                      "expr_array",
                                      "expr_alias",
                                      "expr",
                                      "operand",
                                      "match_expr",
                                      "query_expr",
                                      "fusion_expr",
                                      "function_expr",
                                      "conjunction_expr",
                                      "between_expr",
                                      "in_expr",
                                      "column_expr",
                                      "constant_expr",
                                      "long_array_expr",
                                      "unclosed_long_array_expr",
                                      "double_array_expr",
                                      "unclosed_double_array_expr",
                                      "interval_expr",
                                      YY_NULLPTR};

static const char *yysymbol_name(yysymbol_kind_t yysymbol) { return yytname[yysymbol]; }
#endif

#define YYPACT_NINF (-167)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    29,   -166, -167, -167, 201,  29,   44,   45,   46,   47,   -167, -167, 41,   -123, -110, -109, -108, 29,   29,   -167, 29,   39,   68,   -105,
    -167, -49,  83,   -167, -167, -167, -167, -167, -167, -167, -106, -167, -167, -163, -167, -156, -167, -3,   -167, -167, -167, -167, -167, -167,
    -167, -167, -167, -167, -167, -167, 153,  -167, -167, -167, -167, 201,  -167, 69,   70,   71,   88,   -148, -148, -130, -167, -167, -167, 29,
    90,   29,   29,   -55,  -103, -61,  29,   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,   4,    95,   -167, 91,
    -167, 29,   -167, -155, -43,  28,   -68,  -146, -145, -167, -167, -167, -167, -35,  -66,  29,   29,   -14,  -167, 138,  138,  -114, -114, 96,
    138,  138,  -114, -114, -148, -148, -167, -167, -167, -167, -167, -167, -167, -138, -167, 29,   107,  108,  -167, 109,  -167, 29,   138,  -137,
    -167, 29,   -167, 43,   -134, -62,  -59,  -133, -167, 153,  -167, 111,  -167, -167, -167, -167, -58,  -167};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] = {0,  52, 56, 59, 60, 0,  0,  0,  0,  0,  57, 58, 0,  0,  0,  0,  0,  0,  0,  54, 0,  0,  0,  2,  3,  6,  7,
                                       15, 16, 17, 14, 10, 9,  8,  13, 12, 67, 0,  68, 0,  66, 0,  76, 75, 78, 77, 80, 79, 82, 81, 84, 83, 86, 85,
                                       29, 61, 62, 63, 64, 0,  65, 0,  0,  0,  0,  31, 30, 0,  73, 70, 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  69, 0,  72, 0,  24, 0,  0,  0,  0,  0,  0,  11, 4,  5,
                                       47, 48, 0,  0,  0,  0,  28, 38, 39, 42, 43, 0,  45, 37, 40, 41, 33, 32, 34, 35, 36, 53, 55, 71, 74, 0,  25,
                                       0,  0,  0,  20, 0,  22, 0,  46, 0,  27, 0,  26, 0,  0,  0,  0,  0,  50, 49, 44, 0,  18, 21, 23, 51, 0,  19};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {-167, -167, -36,  48,   -10,  -1,   -167, -167, -167, -167,
                                       -167, -167, -167, -167, -167, -167, -167, -167, -167, 112};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] = {0, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    1,   2,   3,   4,   54,  99,  72,  129, 77, 41,  67,  93,  113, 73,  74,  94,  65,  66,  95,  71,  110, 134, 96,  89,  90,  91,  -1,  -1,  137,
    139, 138, 140, 1,   2,   3,   4,   71,  71, 146, 152, 155, 71,  156, 159, 68,  69,  105, 59,  55,  56,  57,  58,  61,  -1,  -1,  87,  88,  89,
    90,  91,  114, 133, 97,  108, 109, 62,  63, 64,  70,  71,  5,   92,  112, 101, 102, 103, 143, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
    125, 126, 127, 128, 111, 104, 107, 73,  74, 132, 6,   7,   8,   9,   131, 5,   111, 135, 151, 136, 144, 73,  141, 142, 148, 149, 150, 157, 160,
    100, 158, 161, 106, 10,  11,  12,  0,   60, 0,   0,   0,   0,   6,   7,   8,   9,   0,   147, 0,   0,   0,   0,   0,   0,   0,   13,  0,   153,
    0,   0,   0,   0,   0,   0,   0,   10,  11, 12,  0,   75,  0,   76,  14,  15,  16,  0,   0,   0,   77,  17,  18,  19,  100, 0,   0,   20,  98,
    13,  130, 21,  0,   0,   0,   0,   78,  79, 80,  81,  0,   0,   0,   0,   83,  0,   14,  15,  16,  0,   0,   0,   0,   17,  18,  19,  0,   0,
    0,   20,  77,  0,   0,   21,  84,  85,  86, 87,  88,  89,  90,  91,  0,   77,  154, 0,   78,  79,  80,  81,  82,  100, 0,   0,   83,  0,   0,
    0,   0,   78,  79,  80,  81,  0,   145, 0,  0,   83,  0,   0,   0,   0,   0,   0,   84,  85,  86,  87,  88,  89,  90,  91,  0,   0,   0,   77,
    0,   84,  85,  86,  87,  88,  89,  90,  91, 0,   0,   0,   0,   0,   77,  -1,  -1,  80,  81,  0,   0,   0,   0,   -1,  0,   0,   0,   0,   0,
    0,   78,  79,  80,  81,  0,   0,   0,   0,  83,  0,   0,   0,   0,   -1,  85,  86,  87,  88,  89,  90,  91,  0,   0,   0,   0,   0,   0,   0,
    84,  85,  86,  87,  88,  89,  90,  91,  42, 43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53};

static const yytype_int16 yycheck[] = {
    3,   4,   5,   6,   5,   41,  55,  3,   122, 175, 20,  174, 73,  143, 144, 178, 17,  18,  174, 174, 75,  176, 178, 171, 172, 173, 140, 141, 174,
    174, 176, 176, 3,   4,   5,   6,   174, 174, 176, 176, 174, 174, 176, 176, 5,   6,   176, 6,   4,   4,   4,   4,   175, 167, 168, 169, 170, 171,
    172, 173, 121, 97,  65,  73,  74,  175, 175, 175, 0,   174, 73,  177, 175, 4,   4,   4,   112, 78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
    88,  89,  90,  91,  146, 4,   3,   143, 144, 5,   100, 101, 102, 103, 6,   73,  146, 76,  141, 174, 121, 143, 175, 111, 4,   4,   4,   176, 4,
    73,  176, 176, 71,  123, 124, 125, -1,  12,  -1,  -1,  -1,  -1,  100, 101, 102, 103, -1,  135, -1,  -1,  -1,  -1,  -1,  -1,  -1,  145, -1,  145,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  123, 124, 125, -1,  73,  -1,  75,  162, 163, 164, -1,  -1,  -1,  122, 169, 170, 171, 73,  -1,  -1,  175, 176,
    145, 171, 179, -1,  -1,  -1,  -1,  138, 139, 140, 141, -1,  -1,  -1,  -1,  146, -1,  162, 163, 164, -1,  -1,  -1,  -1,  169, 170, 171, -1,  -1,
    -1,  175, 122, -1,  -1,  179, 166, 167, 168, 169, 170, 171, 172, 173, -1,  122, 176, -1,  138, 139, 140, 141, 142, 73,  -1,  -1,  146, -1,  -1,
    -1,  -1,  138, 139, 140, 141, -1,  143, -1,  -1,  146, -1,  -1,  -1,  -1,  -1,  -1,  166, 167, 168, 169, 170, 171, 172, 173, -1,  -1,  -1,  122,
    -1,  166, 167, 168, 169, 170, 171, 172, 173, -1,  -1,  -1,  -1,  -1,  122, 138, 139, 140, 141, -1,  -1,  -1,  -1,  146, -1,  -1,  -1,  -1,  -1,
    -1,  138, 139, 140, 141, -1,  -1,  -1,  -1,  146, -1,  -1,  -1,  -1,  166, 167, 168, 169, 170, 171, 172, 173, -1,  -1,  -1,  -1,  -1,  -1,  -1,
    166, 167, 168, 169, 170, 171, 172, 173, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] = {
    0,   3,   4,   5,   6,   73,  100, 101, 102, 103, 123, 124, 125, 145, 162, 163, 164, 169, 170, 171, 175, 179, 181, 182, 183, 184, 185,
    186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 175, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
    185, 4,   4,   4,   4,   6,   199, 175, 175, 175, 175, 185, 185, 184, 5,   6,   0,   174, 55,  143, 144, 73,  75,  122, 138, 139, 140,
    141, 142, 146, 166, 167, 168, 169, 170, 171, 172, 173, 177, 174, 178, 174, 178, 65,  176, 182, 73,  4,   4,   4,   4,   176, 183, 3,
    184, 184, 75,  146, 175, 73,  121, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 185, 3,   171, 6,   5,   182, 176,
    76,  174, 174, 176, 174, 176, 175, 185, 182, 121, 143, 176, 185, 4,   4,   4,   182, 176, 185, 176, 174, 176, 176, 176, 176, 4,   176};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] = {0,   180, 181, 182, 182, 183, 183, 184, 184, 184, 184, 185, 185, 185, 185, 185, 185, 185, 186, 186, 187, 187,
                                    188, 188, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189,
                                    189, 189, 189, 190, 190, 191, 192, 192, 193, 193, 193, 193, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194,
                                    194, 194, 194, 195, 196, 196, 197, 198, 198, 199, 199, 199, 199, 199, 199, 199, 199, 199, 199, 199, 199};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] = {0, 2, 1, 1, 3, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 6, 8, 4, 6, 4, 6, 3, 4, 5, 4, 3,
                                   2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 3, 4, 3, 3, 5, 5, 6, 1, 3, 1, 3, 1, 1,
                                   1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 3, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

enum { YYENOMEM = -2 };

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = EXPRESSIONEMPTY)

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
#define YYNOMEM goto yyexhaustedlab

#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                                                                                                       \
    do                                                                                                                                               \
        if (yychar == EXPRESSIONEMPTY) {                                                                                                             \
            yychar = (Token);                                                                                                                        \
            yylval = (Value);                                                                                                                        \
            YYPOPSTACK(yylen);                                                                                                                       \
            yystate = *yyssp;                                                                                                                        \
            goto yybackup;                                                                                                                           \
        } else {                                                                                                                                     \
            yyerror(&yylloc, scanner, result, YY_("syntax error: cannot back up"));                                                                  \
            YYERROR;                                                                                                                                 \
        }                                                                                                                                            \
    while (0)

/* Backward compatibility with an undocumented macro.
   Use EXPRESSIONerror or EXPRESSIONUNDEF. */
#define YYERRCODE EXPRESSIONUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                                                                                                              \
    do                                                                                                                                               \
        if (N) {                                                                                                                                     \
            (Current).first_line = YYRHSLOC(Rhs, 1).first_line;                                                                                      \
            (Current).first_column = YYRHSLOC(Rhs, 1).first_column;                                                                                  \
            (Current).last_line = YYRHSLOC(Rhs, N).last_line;                                                                                        \
            (Current).last_column = YYRHSLOC(Rhs, N).last_column;                                                                                    \
        } else {                                                                                                                                     \
            (Current).first_line = (Current).last_line = YYRHSLOC(Rhs, 0).last_line;                                                                 \
            (Current).first_column = (Current).last_column = YYRHSLOC(Rhs, 0).last_column;                                                           \
        }                                                                                                                                            \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])

/* Enable debugging if requested.  */
#if EXPRESSIONDEBUG

#ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#define YYFPRINTF fprintf
#endif

#define YYDPRINTF(Args)                                                                                                                              \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            YYFPRINTF Args;                                                                                                                          \
    } while (0)

/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YYLOCATION_PRINT

#if defined YY_LOCATION_PRINT

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YYLOCATION_PRINT(File, Loc) YY_LOCATION_PRINT(File, *(Loc))

#elif defined EXPRESSIONLTYPE_IS_TRIVIAL && EXPRESSIONLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int yy_location_print_(FILE *yyo, YYLTYPE const *const yylocp) {
    int res = 0;
    int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
    if (0 <= yylocp->first_line) {
        res += YYFPRINTF(yyo, "%d", yylocp->first_line);
        if (0 <= yylocp->first_column)
            res += YYFPRINTF(yyo, ".%d", yylocp->first_column);
    }
    if (0 <= yylocp->last_line) {
        if (yylocp->first_line < yylocp->last_line) {
            res += YYFPRINTF(yyo, "-%d", yylocp->last_line);
            if (0 <= end_col)
                res += YYFPRINTF(yyo, ".%d", end_col);
        } else if (0 <= end_col && yylocp->first_column < end_col)
            res += YYFPRINTF(yyo, "-%d", end_col);
    }
    return res;
}

#define YYLOCATION_PRINT yy_location_print_

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT(File, Loc) YYLOCATION_PRINT(File, &(Loc))

#else

#define YYLOCATION_PRINT(File, Loc) ((void)0)
/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT YYLOCATION_PRINT

#endif
#endif /* !defined YYLOCATION_PRINT */

#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                                                                                                \
    do {                                                                                                                                             \
        if (yydebug) {                                                                                                                               \
            YYFPRINTF(stderr, "%s ", Title);                                                                                                         \
            yy_symbol_print(stderr, Kind, Value, Location, scanner, result);                                                                         \
            YYFPRINTF(stderr, "\n");                                                                                                                 \
        }                                                                                                                                            \
    } while (0)

/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void yy_symbol_value_print(FILE *yyo,
                                  yysymbol_kind_t yykind,
                                  YYSTYPE const *const yyvaluep,
                                  YYLTYPE const *const yylocationp,
                                  void *scanner,
                                  infinity::ExpressionParserResult *result) {
    FILE *yyoutput = yyo;
    YY_USE(yyoutput);
    YY_USE(yylocationp);
    YY_USE(scanner);
    YY_USE(result);
    if (!yyvaluep)
        return;
    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YY_USE(yykind);
    YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void yy_symbol_print(FILE *yyo,
                            yysymbol_kind_t yykind,
                            YYSTYPE const *const yyvaluep,
                            YYLTYPE const *const yylocationp,
                            void *scanner,
                            infinity::ExpressionParserResult *result) {
    YYFPRINTF(yyo, "%s %s (", yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name(yykind));

    YYLOCATION_PRINT(yyo, yylocationp);
    YYFPRINTF(yyo, ": ");
    yy_symbol_value_print(yyo, yykind, yyvaluep, yylocationp, scanner, result);
    YYFPRINTF(yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void yy_stack_print(yy_state_t *yybottom, yy_state_t *yytop) {
    YYFPRINTF(stderr, "Stack now");
    for (; yybottom <= yytop; yybottom++) {
        int yybot = *yybottom;
        YYFPRINTF(stderr, " %d", yybot);
    }
    YYFPRINTF(stderr, "\n");
}

#define YY_STACK_PRINT(Bottom, Top)                                                                                                                  \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            yy_stack_print((Bottom), (Top));                                                                                                         \
    } while (0)

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void yy_reduce_print(yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void *scanner, infinity::ExpressionParserResult *result) {
    int yylno = yyrline[yyrule];
    int yynrhs = yyr2[yyrule];
    int yyi;
    YYFPRINTF(stderr, "Reducing stack by rule %d (line %d):\n", yyrule - 1, yylno);
    /* The symbols being reduced.  */
    for (yyi = 0; yyi < yynrhs; yyi++) {
        YYFPRINTF(stderr, "   $%d = ", yyi + 1);
        yy_symbol_print(stderr,
                        YY_ACCESSING_SYMBOL(+yyssp[yyi + 1 - yynrhs]),
                        &yyvsp[(yyi + 1) - (yynrhs)],
                        &(yylsp[(yyi + 1) - (yynrhs)]),
                        scanner,
                        result);
        YYFPRINTF(stderr, "\n");
    }
}

#define YY_REDUCE_PRINT(Rule)                                                                                                                        \
    do {                                                                                                                                             \
        if (yydebug)                                                                                                                                 \
            yy_reduce_print(yyssp, yyvsp, yylsp, Rule, scanner, result);                                                                             \
    } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !EXPRESSIONDEBUG */
#define YYDPRINTF(Args) ((void)0)
#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
#define YY_STACK_PRINT(Bottom, Top)
#define YY_REDUCE_PRINT(Rule)
#endif /* !EXPRESSIONDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Context of a parse error.  */
typedef struct {
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
static int yypcontext_expected_tokens(const yypcontext_t *yyctx, yysymbol_kind_t yyarg[], int yyargn) {
    /* Actual size of YYARG. */
    int yycount = 0;
    int yyn = yypact[+*yyctx->yyssp];
    if (!yypact_value_is_default(yyn)) {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        /* Stay within bounds of both yycheck and yytname.  */
        int yychecklim = YYLAST - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        int yyx;
        for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror && !yytable_value_is_error(yytable[yyx + yyn])) {
                if (!yyarg)
                    ++yycount;
                else if (yycount == yyargn)
                    return 0;
                else
                    yyarg[yycount++] = YY_CAST(yysymbol_kind_t, yyx);
            }
    }
    if (yyarg && yycount == 0 && 0 < yyargn)
        yyarg[0] = YYSYMBOL_YYEMPTY;
    return yycount;
}

#ifndef yystrlen
#if defined __GLIBC__ && defined _STRING_H
#define yystrlen(S) (YY_CAST(YYPTRDIFF_T, strlen(S)))
#else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T yystrlen(const char *yystr) {
    YYPTRDIFF_T yylen;
    for (yylen = 0; yystr[yylen]; yylen++)
        continue;
    return yylen;
}
#endif
#endif

#ifndef yystpcpy
#if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
#else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *yystpcpy(char *yydest, const char *yysrc) {
    char *yyd = yydest;
    const char *yys = yysrc;

    while ((*yyd++ = *yys++) != '\0')
        continue;

    return yyd - 1;
}
#endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T yytnamerr(char *yyres, const char *yystr) {
    if (*yystr == '"') {
        YYPTRDIFF_T yyn = 0;
        char const *yyp = yystr;
        for (;;)
            switch (*++yyp) {
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
    do_not_strip_quotes:;
    }

    if (yyres)
        return yystpcpy(yyres, yystr) - yyres;
    else
        return yystrlen(yystr);
}
#endif

static int yy_syntax_error_arguments(const yypcontext_t *yyctx, yysymbol_kind_t yyarg[], int yyargn) {
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
    if (yyctx->yytoken != YYSYMBOL_YYEMPTY) {
        int yyn;
        if (yyarg)
            yyarg[yycount] = yyctx->yytoken;
        ++yycount;
        yyn = yypcontext_expected_tokens(yyctx, yyarg ? yyarg + 1 : yyarg, yyargn - 1);
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
static int yysyntax_error(YYPTRDIFF_T *yymsg_alloc, char **yymsg, const yypcontext_t *yyctx) {
    enum { YYARGS_MAX = 5 };
    /* Internationalized format string. */
    const char *yyformat = YY_NULLPTR;
    /* Arguments of yyformat: reported tokens (one for the "unexpected",
       one per "expected"). */
    yysymbol_kind_t yyarg[YYARGS_MAX];
    /* Cumulated lengths of YYARG.  */
    YYPTRDIFF_T yysize = 0;

    /* Actual size of YYARG. */
    int yycount = yy_syntax_error_arguments(yyctx, yyarg, YYARGS_MAX);
    if (yycount == YYENOMEM)
        return YYENOMEM;

    switch (yycount) {
#define YYCASE_(N, S)                                                                                                                                \
    case N:                                                                                                                                          \
        yyformat = S;                                                                                                                                \
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
    yysize = yystrlen(yyformat) - 2 * yycount + 1;
    {
        int yyi;
        for (yyi = 0; yyi < yycount; ++yyi) {
            YYPTRDIFF_T yysize1 = yysize + yytnamerr(YY_NULLPTR, yytname[yyarg[yyi]]);
            if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                yysize = yysize1;
            else
                return YYENOMEM;
        }
    }

    if (*yymsg_alloc < yysize) {
        *yymsg_alloc = 2 * yysize;
        if (!(yysize <= *yymsg_alloc && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
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
            if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount) {
                yyp += yytnamerr(yyp, yytname[yyarg[yyi++]]);
                yyformat += 2;
            } else {
                ++yyp;
                ++yyformat;
            }
    }
    return 0;
}

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void yydestruct(const char *yymsg,
                       yysymbol_kind_t yykind,
                       YYSTYPE *yyvaluep,
                       YYLTYPE *yylocationp,
                       void *scanner,
                       infinity::ExpressionParserResult *result) {
    YY_USE(yyvaluep);
    YY_USE(yylocationp);
    YY_USE(scanner);
    YY_USE(result);
    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT(yymsg, yykind, yyvaluep, yylocationp);

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    switch (yykind) {
        case YYSYMBOL_expr_array: /* expr_array  */
#line 96 "expression_parser.y"
        {
            fprintf(stderr, "destroy expression array\n");
            if ((((*yyvaluep).expr_array_t)) != nullptr) {
                for (auto ptr : *(((*yyvaluep).expr_array_t))) {
                    delete ptr;
                }
                delete (((*yyvaluep).expr_array_t));
            }
        }
#line 1563 "expression_parser.cpp"
        break;

        case YYSYMBOL_expr_alias: /* expr_alias  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1571 "expression_parser.cpp"
        break;

        case YYSYMBOL_expr: /* expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1579 "expression_parser.cpp"
        break;

        case YYSYMBOL_operand: /* operand  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1587 "expression_parser.cpp"
        break;

        case YYSYMBOL_match_expr: /* match_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1595 "expression_parser.cpp"
        break;

        case YYSYMBOL_query_expr: /* query_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1603 "expression_parser.cpp"
        break;

        case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1611 "expression_parser.cpp"
        break;

        case YYSYMBOL_function_expr: /* function_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1619 "expression_parser.cpp"
        break;

        case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1627 "expression_parser.cpp"
        break;

        case YYSYMBOL_between_expr: /* between_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1635 "expression_parser.cpp"
        break;

        case YYSYMBOL_in_expr: /* in_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1643 "expression_parser.cpp"
        break;

        case YYSYMBOL_column_expr: /* column_expr  */
#line 106 "expression_parser.y"
        {
            delete (((*yyvaluep).expr_t));
        }
#line 1651 "expression_parser.cpp"
        break;

        case YYSYMBOL_constant_expr: /* constant_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1659 "expression_parser.cpp"
        break;

        case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1667 "expression_parser.cpp"
        break;

        case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1675 "expression_parser.cpp"
        break;

        case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1683 "expression_parser.cpp"
        break;

        case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1691 "expression_parser.cpp"
        break;

        case YYSYMBOL_interval_expr: /* interval_expr  */
#line 110 "expression_parser.y"
        {
            delete (((*yyvaluep).const_expr_t));
        }
#line 1699 "expression_parser.cpp"
        break;

        default:
            break;
    }
    YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/*----------.
| yyparse.  |
`----------*/

int yyparse(void *scanner, infinity::ExpressionParserResult *result) {
    /* Lookahead token kind.  */
    int yychar;

    /* The semantic value of the lookahead symbol.  */
    /* Default value used for initialization, for pacifying older GCCs
       or non-GCC compilers.  */
    YY_INITIAL_VALUE(static YYSTYPE yyval_default;)
    YYSTYPE yylval YY_INITIAL_VALUE(= yyval_default);

    /* Location data for the lookahead symbol.  */
    static YYLTYPE yyloc_default
#if defined EXPRESSIONLTYPE_IS_TRIVIAL && EXPRESSIONLTYPE_IS_TRIVIAL
        = {1, 1, 1, 1}
#endif
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

#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

    /* The number of symbols on the RHS of the reduced rule.
       Keep to zero when no symbol should be popped.  */
    int yylen = 0;

    YYDPRINTF((stderr, "Starting parse\n"));

    yychar = EXPRESSIONEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 68 "expression_parser.y"
    {
        // Initialize
        yylloc.first_column = 0;
        yylloc.last_column = 0;
        yylloc.first_line = 0;
        yylloc.last_line = 0;
        yylloc.total_column = 0;
        yylloc.string_length = 0;
    }

#line 1807 "expression_parser.cpp"

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
    YYDPRINTF((stderr, "Entering state %d\n", yystate));
    YY_ASSERT(0 <= yystate && yystate < YYNSTATES);
    YY_IGNORE_USELESS_CAST_BEGIN
    *yyssp = YY_CAST(yy_state_t, yystate);
    YY_IGNORE_USELESS_CAST_END
    YY_STACK_PRINT(yyss, yyssp);

    if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
        YYNOMEM;
#else
    {
        /* Get the current used size of the three stacks, in elements.  */
        YYPTRDIFF_T yysize = yyssp - yyss + 1;

#if defined yyoverflow
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
            yyoverflow(YY_("memory exhausted"),
                       &yyss1,
                       yysize * YYSIZEOF(*yyssp),
                       &yyvs1,
                       yysize * YYSIZEOF(*yyvsp),
                       &yyls1,
                       yysize * YYSIZEOF(*yylsp),
                       &yystacksize);
            yyss = yyss1;
            yyvs = yyvs1;
            yyls = yyls1;
        }
#else /* defined YYSTACK_RELOCATE */
        /* Extend the stack our own way.  */
        if (YYMAXDEPTH <= yystacksize)
            YYNOMEM;
        yystacksize *= 2;
        if (YYMAXDEPTH < yystacksize)
            yystacksize = YYMAXDEPTH;

        {
            yy_state_t *yyss1 = yyss;
            union yyalloc *yyptr = YY_CAST(union yyalloc *, YYSTACK_ALLOC(YY_CAST(YYSIZE_T, YYSTACK_BYTES(yystacksize))));
            if (!yyptr)
                YYNOMEM;
            YYSTACK_RELOCATE(yyss_alloc, yyss);
            YYSTACK_RELOCATE(yyvs_alloc, yyvs);
            YYSTACK_RELOCATE(yyls_alloc, yyls);
#undef YYSTACK_RELOCATE
            if (yyss1 != yyssa)
                YYSTACK_FREE(yyss1);
        }
#endif

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;
        yylsp = yyls + yysize - 1;

        YY_IGNORE_USELESS_CAST_BEGIN
        YYDPRINTF((stderr, "Stack size increased to %ld\n", YY_CAST(long, yystacksize)));
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
    if (yypact_value_is_default(yyn))
        goto yydefault;

    /* Not known => get a lookahead token if don't already have one.  */

    /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
    if (yychar == EXPRESSIONEMPTY) {
        YYDPRINTF((stderr, "Reading a token\n"));
        yychar = yylex(&yylval, &yylloc, scanner);
    }

    if (yychar <= EXPRESSIONEOF) {
        yychar = EXPRESSIONEOF;
        yytoken = YYSYMBOL_YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
    } else if (yychar == EXPRESSIONerror) {
        /* The scanner already issued an error message, process directly
           to error recovery.  But do not keep the error token as
           lookahead, it is too special and may lead us to an endless
           loop in error recovery. */
        yychar = EXPRESSIONUNDEF;
        yytoken = YYSYMBOL_YYerror;
        yyerror_range[1] = yylloc;
        goto yyerrlab1;
    } else {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
    }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
        goto yydefault;
    yyn = yytable[yyn];
    if (yyn <= 0) {
        if (yytable_value_is_error(yyn))
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    }

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus)
        yyerrstatus--;

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);
    yystate = yyn;
    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END
    *++yylsp = yylloc;

    /* Discard the shifted token.  */
    yychar = EXPRESSIONEMPTY;
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
    yyval = yyvsp[1 - yylen];

    /* Default location. */
    YYLLOC_DEFAULT(yyloc, (yylsp - yylen), yylen);
    yyerror_range[1] = yyloc;
    YY_REDUCE_PRINT(yyn);
    switch (yyn) {
        case 2: /* input_pattern: expr_array  */
#line 164 "expression_parser.y"
        {
            result->exprs_ptr_ = (yyvsp[0].expr_array_t);
        }
#line 2022 "expression_parser.cpp"
        break;

        case 3: /* expr_array: expr_alias  */
#line 172 "expression_parser.y"
        {
            (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr *>();
            (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
        }
#line 2031 "expression_parser.cpp"
        break;

        case 4: /* expr_array: expr_array ',' expr_alias  */
#line 176 "expression_parser.y"
        {
            (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
        }
#line 2040 "expression_parser.cpp"
        break;

        case 5: /* expr_alias: expr AS IDENTIFIER  */
#line 181 "expression_parser.y"
        {
            (yyval.expr_t) = (yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
            free((yyvsp[0].str_value));
        }
#line 2051 "expression_parser.cpp"
        break;

        case 6: /* expr_alias: expr  */
#line 187 "expression_parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].expr_t);
        }
#line 2059 "expression_parser.cpp"
        break;

        case 11: /* operand: '(' expr ')'  */
#line 196 "expression_parser.y"
        {
            (yyval.expr_t) = (yyvsp[-1].expr_t);
        }
#line 2067 "expression_parser.cpp"
        break;

        case 12: /* operand: constant_expr  */
#line 199 "expression_parser.y"
        {
            (yyval.expr_t) = (yyvsp[0].const_expr_t);
        }
#line 2075 "expression_parser.cpp"
        break;

        case 18: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 208 "expression_parser.y"
        {
            infinity::MatchExpr *match_expr = new infinity::MatchExpr();
            match_expr->fields_ = std::string((yyvsp[-3].str_value));
            match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-3].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = match_expr;
        }
#line 2088 "expression_parser.cpp"
        break;

        case 19: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 216 "expression_parser.y"
        {
            infinity::MatchExpr *match_expr = new infinity::MatchExpr();
            match_expr->fields_ = std::string((yyvsp[-5].str_value));
            match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
            match_expr->options_text_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-5].str_value));
            free((yyvsp[-3].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = match_expr;
        }
#line 2103 "expression_parser.cpp"
        break;

        case 20: /* query_expr: QUERY '(' STRING ')'  */
#line 227 "expression_parser.y"
        {
            infinity::MatchExpr *match_expr = new infinity::MatchExpr();
            match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = match_expr;
        }
#line 2114 "expression_parser.cpp"
        break;

        case 21: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 233 "expression_parser.y"
        {
            infinity::MatchExpr *match_expr = new infinity::MatchExpr();
            match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
            match_expr->options_text_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-3].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = match_expr;
        }
#line 2127 "expression_parser.cpp"
        break;

        case 22: /* fusion_expr: FUSION '(' STRING ')'  */
#line 242 "expression_parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 2138 "expression_parser.cpp"
        break;

        case 23: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 248 "expression_parser.y"
        {
            infinity::FusionExpr *fusion_expr = new infinity::FusionExpr();
            fusion_expr->method_ = std::string((yyvsp[-3].str_value));
            free((yyvsp[-3].str_value));
            fusion_expr->SetOptions((yyvsp[-1].str_value));
            free((yyvsp[-1].str_value));
            (yyval.expr_t) = fusion_expr;
        }
#line 2151 "expression_parser.cpp"
        break;

        case 24: /* function_expr: IDENTIFIER '(' ')'  */
#line 257 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-2].str_value));
            func_expr->func_name_ = (yyvsp[-2].str_value);
            free((yyvsp[-2].str_value));
            func_expr->arguments_ = nullptr;
            (yyval.expr_t) = func_expr;
        }
#line 2164 "expression_parser.cpp"
        break;

        case 25: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 265 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            func_expr->func_name_ = (yyvsp[-3].str_value);
            free((yyvsp[-3].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = func_expr;
        }
#line 2177 "expression_parser.cpp"
        break;

        case 26: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 273 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-4].str_value));
            func_expr->func_name_ = (yyvsp[-4].str_value);
            free((yyvsp[-4].str_value));
            func_expr->arguments_ = (yyvsp[-1].expr_array_t);
            func_expr->distinct_ = true;
            (yyval.expr_t) = func_expr;
        }
#line 2191 "expression_parser.cpp"
        break;

        case 27: /* function_expr: operand IS NOT NULLABLE  */
#line 282 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_not_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2203 "expression_parser.cpp"
        break;

        case 28: /* function_expr: operand IS NULLABLE  */
#line 289 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "is_null";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2215 "expression_parser.cpp"
        break;

        case 29: /* function_expr: NOT operand  */
#line 296 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2227 "expression_parser.cpp"
        break;

        case 30: /* function_expr: '-' operand  */
#line 303 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2239 "expression_parser.cpp"
        break;

        case 31: /* function_expr: '+' operand  */
#line 310 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2251 "expression_parser.cpp"
        break;

        case 32: /* function_expr: operand '-' operand  */
#line 317 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "-";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2264 "expression_parser.cpp"
        break;

        case 33: /* function_expr: operand '+' operand  */
#line 325 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "+";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2277 "expression_parser.cpp"
        break;

        case 34: /* function_expr: operand '*' operand  */
#line 333 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "*";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2290 "expression_parser.cpp"
        break;

        case 35: /* function_expr: operand '/' operand  */
#line 341 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "/";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2303 "expression_parser.cpp"
        break;

        case 36: /* function_expr: operand '%' operand  */
#line 349 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "%";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2316 "expression_parser.cpp"
        break;

        case 37: /* function_expr: operand '=' operand  */
#line 357 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2329 "expression_parser.cpp"
        break;

        case 38: /* function_expr: operand EQUAL operand  */
#line 365 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2342 "expression_parser.cpp"
        break;

        case 39: /* function_expr: operand NOT_EQ operand  */
#line 373 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<>";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2355 "expression_parser.cpp"
        break;

        case 40: /* function_expr: operand '<' operand  */
#line 381 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2368 "expression_parser.cpp"
        break;

        case 41: /* function_expr: operand '>' operand  */
#line 389 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2381 "expression_parser.cpp"
        break;

        case 42: /* function_expr: operand LESS_EQ operand  */
#line 397 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "<=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2394 "expression_parser.cpp"
        break;

        case 43: /* function_expr: operand GREATER_EQ operand  */
#line 405 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = ">=";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2407 "expression_parser.cpp"
        break;

        case 44: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 413 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            ParserHelper::ToLower((yyvsp[-3].str_value));
            if (strcmp((yyvsp[-3].str_value), "year") == 0) {
                func_expr->func_name_ = "extract_year";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "month") == 0) {
                func_expr->func_name_ = "extract_month";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "day") == 0) {
                func_expr->func_name_ = "extract_day";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "hour") == 0) {
                func_expr->func_name_ = "extract_hour";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "minute") == 0) {
                func_expr->func_name_ = "extract_minute";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else if (strcmp((yyvsp[-3].str_value), "second") == 0) {
                func_expr->func_name_ = "extract_second";
                func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            } else {
                delete func_expr;
                expressionerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            free((yyvsp[-3].str_value));
            func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2442 "expression_parser.cpp"
        break;

        case 45: /* function_expr: operand LIKE operand  */
#line 443 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2455 "expression_parser.cpp"
        break;

        case 46: /* function_expr: operand NOT LIKE operand  */
#line 451 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "not_like";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2468 "expression_parser.cpp"
        break;

        case 47: /* conjunction_expr: expr AND expr  */
#line 460 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "and";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2481 "expression_parser.cpp"
        break;

        case 48: /* conjunction_expr: expr OR expr  */
#line 468 "expression_parser.y"
        {
            infinity::FunctionExpr *func_expr = new infinity::FunctionExpr();
            func_expr->func_name_ = "or";
            func_expr->arguments_ = new std::vector<infinity::ParsedExpr *>();
            func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
            func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
            (yyval.expr_t) = func_expr;
        }
#line 2494 "expression_parser.cpp"
        break;

        case 49: /* between_expr: operand BETWEEN operand AND operand  */
#line 477 "expression_parser.y"
        {
            infinity::BetweenExpr *between_expr = new infinity::BetweenExpr();
            between_expr->value_ = (yyvsp[-4].expr_t);
            between_expr->lower_bound_ = (yyvsp[-2].expr_t);
            between_expr->upper_bound_ = (yyvsp[0].expr_t);
            (yyval.expr_t) = between_expr;
        }
#line 2506 "expression_parser.cpp"
        break;

        case 50: /* in_expr: operand IN '(' expr_array ')'  */
#line 485 "expression_parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(true);
            in_expr->left_ = (yyvsp[-4].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 2517 "expression_parser.cpp"
        break;

        case 51: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 491 "expression_parser.y"
        {
            infinity::InExpr *in_expr = new infinity::InExpr(false);
            in_expr->left_ = (yyvsp[-5].expr_t);
            in_expr->arguments_ = (yyvsp[-1].expr_array_t);
            (yyval.expr_t) = in_expr;
        }
#line 2528 "expression_parser.cpp"
        break;

        case 52: /* column_expr: IDENTIFIER  */
#line 498 "expression_parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 2540 "expression_parser.cpp"
        break;

        case 53: /* column_expr: column_expr '.' IDENTIFIER  */
#line 505 "expression_parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            ParserHelper::ToLower((yyvsp[0].str_value));
            column_expr->names_.emplace_back((yyvsp[0].str_value));
            free((yyvsp[0].str_value));
            (yyval.expr_t) = column_expr;
        }
#line 2552 "expression_parser.cpp"
        break;

        case 54: /* column_expr: '*'  */
#line 512 "expression_parser.y"
        {
            infinity::ColumnExpr *column_expr = new infinity::ColumnExpr();
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 2562 "expression_parser.cpp"
        break;

        case 55: /* column_expr: column_expr '.' '*'  */
#line 517 "expression_parser.y"
        {
            infinity::ColumnExpr *column_expr = (infinity::ColumnExpr *)(yyvsp[-2].expr_t);
            if (column_expr->star_) {
                expressionerror(&yyloc, scanner, result, "Invalid column expression format");
                YYERROR;
            }
            column_expr->star_ = true;
            (yyval.expr_t) = column_expr;
        }
#line 2576 "expression_parser.cpp"
        break;

        case 56: /* constant_expr: STRING  */
#line 527 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
            const_expr->str_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2586 "expression_parser.cpp"
        break;

        case 57: /* constant_expr: TRUE  */
#line 532 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = true;
            (yyval.const_expr_t) = const_expr;
        }
#line 2596 "expression_parser.cpp"
        break;

        case 58: /* constant_expr: FALSE  */
#line 537 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
            const_expr->bool_value_ = false;
            (yyval.const_expr_t) = const_expr;
        }
#line 2606 "expression_parser.cpp"
        break;

        case 59: /* constant_expr: DOUBLE_VALUE  */
#line 542 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
            const_expr->double_value_ = (yyvsp[0].double_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2616 "expression_parser.cpp"
        break;

        case 60: /* constant_expr: LONG_VALUE  */
#line 547 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
            const_expr->integer_value_ = (yyvsp[0].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2626 "expression_parser.cpp"
        break;

        case 61: /* constant_expr: DATE STRING  */
#line 552 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2636 "expression_parser.cpp"
        break;

        case 62: /* constant_expr: TIME STRING  */
#line 557 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2646 "expression_parser.cpp"
        break;

        case 63: /* constant_expr: DATETIME STRING  */
#line 562 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2656 "expression_parser.cpp"
        break;

        case 64: /* constant_expr: TIMESTAMP STRING  */
#line 567 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
            const_expr->date_value_ = (yyvsp[0].str_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2666 "expression_parser.cpp"
        break;

        case 65: /* constant_expr: INTERVAL interval_expr  */
#line 572 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 2674 "expression_parser.cpp"
        break;

        case 66: /* constant_expr: interval_expr  */
#line 575 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 2682 "expression_parser.cpp"
        break;

        case 67: /* constant_expr: long_array_expr  */
#line 578 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 2690 "expression_parser.cpp"
        break;

        case 68: /* constant_expr: double_array_expr  */
#line 581 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
        }
#line 2698 "expression_parser.cpp"
        break;

        case 69: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 585 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 2706 "expression_parser.cpp"
        break;

        case 70: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 589 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
            const_expr->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 2716 "expression_parser.cpp"
        break;

        case 71: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 594 "expression_parser.y"
        {
            (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 2725 "expression_parser.cpp"
        break;

        case 72: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 599 "expression_parser.y"
        {
            (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
        }
#line 2733 "expression_parser.cpp"
        break;

        case 73: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 603 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
            const_expr->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = const_expr;
        }
#line 2743 "expression_parser.cpp"
        break;

        case 74: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 608 "expression_parser.y"
        {
            (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
            (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
        }
#line 2752 "expression_parser.cpp"
        break;

        case 75: /* interval_expr: LONG_VALUE SECONDS  */
#line 613 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2763 "expression_parser.cpp"
        break;

        case 76: /* interval_expr: LONG_VALUE SECOND  */
#line 619 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kSecond;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2774 "expression_parser.cpp"
        break;

        case 77: /* interval_expr: LONG_VALUE MINUTES  */
#line 625 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2785 "expression_parser.cpp"
        break;

        case 78: /* interval_expr: LONG_VALUE MINUTE  */
#line 631 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMinute;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2796 "expression_parser.cpp"
        break;

        case 79: /* interval_expr: LONG_VALUE HOURS  */
#line 637 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2807 "expression_parser.cpp"
        break;

        case 80: /* interval_expr: LONG_VALUE HOUR  */
#line 643 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kHour;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2818 "expression_parser.cpp"
        break;

        case 81: /* interval_expr: LONG_VALUE DAYS  */
#line 649 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2829 "expression_parser.cpp"
        break;

        case 82: /* interval_expr: LONG_VALUE DAY  */
#line 655 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kDay;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2840 "expression_parser.cpp"
        break;

        case 83: /* interval_expr: LONG_VALUE MONTHS  */
#line 661 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2851 "expression_parser.cpp"
        break;

        case 84: /* interval_expr: LONG_VALUE MONTH  */
#line 667 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kMonth;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2862 "expression_parser.cpp"
        break;

        case 85: /* interval_expr: LONG_VALUE YEARS  */
#line 673 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2873 "expression_parser.cpp"
        break;

        case 86: /* interval_expr: LONG_VALUE YEAR  */
#line 679 "expression_parser.y"
        {
            infinity::ConstantExpr *const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
            const_expr->interval_type_ = infinity::TimeUnit::kYear;
            const_expr->integer_value_ = (yyvsp[-1].long_value);
            (yyval.const_expr_t) = const_expr;
        }
#line 2884 "expression_parser.cpp"
        break;

#line 2888 "expression_parser.cpp"

        default:
            break;
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
    YY_SYMBOL_PRINT("-> $$ =", YY_CAST(yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

    YYPOPSTACK(yylen);
    yylen = 0;

    *++yyvsp = yyval;
    *++yylsp = yyloc;

    /* Now 'shift' the result of the reduction.  Determine what state
       that goes to, based on the state we popped back to and the rule
       number reduced by.  */
    {
        const int yylhs = yyr1[yyn] - YYNTOKENS;
        const int yyi = yypgoto[yylhs] + *yyssp;
        yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp ? yytable[yyi] : yydefgoto[yylhs]);
    }

    goto yynewstate;

/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yychar == EXPRESSIONEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE(yychar);
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus) {
        ++yynerrs;
        {
            yypcontext_t yyctx = {yyssp, yytoken, &yylloc};
            char const *yymsgp = YY_("syntax error");
            int yysyntax_error_status;
            yysyntax_error_status = yysyntax_error(&yymsg_alloc, &yymsg, &yyctx);
            if (yysyntax_error_status == 0)
                yymsgp = yymsg;
            else if (yysyntax_error_status == -1) {
                if (yymsg != yymsgbuf)
                    YYSTACK_FREE(yymsg);
                yymsg = YY_CAST(char *, YYSTACK_ALLOC(YY_CAST(YYSIZE_T, yymsg_alloc)));
                if (yymsg) {
                    yysyntax_error_status = yysyntax_error(&yymsg_alloc, &yymsg, &yyctx);
                    yymsgp = yymsg;
                } else {
                    yymsg = yymsgbuf;
                    yymsg_alloc = sizeof yymsgbuf;
                    yysyntax_error_status = YYENOMEM;
                }
            }
            yyerror(&yylloc, scanner, result, yymsgp);
            if (yysyntax_error_status == YYENOMEM)
                YYNOMEM;
        }
    }

    yyerror_range[1] = yylloc;
    if (yyerrstatus == 3) {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        if (yychar <= EXPRESSIONEOF) {
            /* Return failure if at end of input.  */
            if (yychar == EXPRESSIONEOF)
                YYABORT;
        } else {
            yydestruct("Error: discarding", yytoken, &yylval, &yylloc, scanner, result);
            yychar = EXPRESSIONEMPTY;
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
    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);
    yystate = *yyssp;
    goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
    yyerrstatus = 3; /* Each real token shifted decrements this.  */

    /* Pop stack until we find a state that shifts the error token.  */
    for (;;) {
        yyn = yypact[yystate];
        if (!yypact_value_is_default(yyn)) {
            yyn += YYSYMBOL_YYerror;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror) {
                yyn = yytable[yyn];
                if (0 < yyn)
                    break;
            }
        }

        /* Pop the current state because it cannot handle the error token.  */
        if (yyssp == yyss)
            YYABORT;

        yyerror_range[1] = *yylsp;
        yydestruct("Error: popping", YY_ACCESSING_SYMBOL(yystate), yyvsp, yylsp, scanner, result);
        YYPOPSTACK(1);
        yystate = *yyssp;
        YY_STACK_PRINT(yyss, yyssp);
    }

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END

    yyerror_range[2] = yylloc;
    ++yylsp;
    YYLLOC_DEFAULT(*yylsp, yyerror_range, 2);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT("Shifting", YY_ACCESSING_SYMBOL(yyn), yyvsp, yylsp);

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
    yyerror(&yylloc, scanner, result, YY_("memory exhausted"));
    yyresult = 2;
    goto yyreturnlab;

/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
    if (yychar != EXPRESSIONEMPTY) {
        /* Make sure we have latest lookahead translation.  See comments at
           user semantic actions for why this is necessary.  */
        yytoken = YYTRANSLATE(yychar);
        yydestruct("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc, scanner, result);
    }
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    YYPOPSTACK(yylen);
    YY_STACK_PRINT(yyss, yyssp);
    while (yyssp != yyss) {
        yydestruct("Cleanup: popping", YY_ACCESSING_SYMBOL(+*yyssp), yyvsp, yylsp, scanner, result);
        YYPOPSTACK(1);
    }
#ifndef yyoverflow
    if (yyss != yyssa)
        YYSTACK_FREE(yyss);
#endif
    if (yymsg != yymsgbuf)
        YYSTACK_FREE(yymsg);
    return yyresult;
}

#line 686 "expression_parser.y"

void expressionerror(YYLTYPE *llocp, void *lexer, infinity::ExpressionParserResult *result, const char *msg) {
    if (result->IsError())
        return;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
    fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
#line 2 "parser.y"

#include <stdint.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg);

#line 87 "parser.cpp"

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
  YYSYMBOL_COMPACT = 24,                   /* COMPACT  */
  YYSYMBOL_EXCEPT = 25,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 26,                     /* FLUSH  */
  YYSYMBOL_USE = 27,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 28,                  /* OPTIMIZE  */
  YYSYMBOL_DATABASE = 29,                  /* DATABASE  */
  YYSYMBOL_TABLE = 30,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 31,                /* COLLECTION  */
  YYSYMBOL_TABLES = 32,                    /* TABLES  */
  YYSYMBOL_INTO = 33,                      /* INTO  */
  YYSYMBOL_VALUES = 34,                    /* VALUES  */
  YYSYMBOL_AST = 35,                       /* AST  */
  YYSYMBOL_PIPELINE = 36,                  /* PIPELINE  */
  YYSYMBOL_RAW = 37,                       /* RAW  */
  YYSYMBOL_LOGICAL = 38,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 39,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 40,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 41,                      /* VIEW  */
  YYSYMBOL_INDEX = 42,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 43,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 44,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 45,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 46,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 47,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 48,                     /* BLOCK  */
  YYSYMBOL_GROUP = 49,                     /* GROUP  */
  YYSYMBOL_BY = 50,                        /* BY  */
  YYSYMBOL_HAVING = 51,                    /* HAVING  */
  YYSYMBOL_AS = 52,                        /* AS  */
  YYSYMBOL_NATURAL = 53,                   /* NATURAL  */
  YYSYMBOL_JOIN = 54,                      /* JOIN  */
  YYSYMBOL_LEFT = 55,                      /* LEFT  */
  YYSYMBOL_RIGHT = 56,                     /* RIGHT  */
  YYSYMBOL_OUTER = 57,                     /* OUTER  */
  YYSYMBOL_FULL = 58,                      /* FULL  */
  YYSYMBOL_ON = 59,                        /* ON  */
  YYSYMBOL_INNER = 60,                     /* INNER  */
  YYSYMBOL_CROSS = 61,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 62,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 63,                     /* WHERE  */
  YYSYMBOL_ORDER = 64,                     /* ORDER  */
  YYSYMBOL_LIMIT = 65,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 66,                    /* OFFSET  */
  YYSYMBOL_ASC = 67,                       /* ASC  */
  YYSYMBOL_DESC = 68,                      /* DESC  */
  YYSYMBOL_IF = 69,                        /* IF  */
  YYSYMBOL_NOT = 70,                       /* NOT  */
  YYSYMBOL_EXISTS = 71,                    /* EXISTS  */
  YYSYMBOL_IN = 72,                        /* IN  */
  YYSYMBOL_FROM = 73,                      /* FROM  */
  YYSYMBOL_TO = 74,                        /* TO  */
  YYSYMBOL_WITH = 75,                      /* WITH  */
  YYSYMBOL_DELIMITER = 76,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 77,                    /* FORMAT  */
  YYSYMBOL_HEADER = 78,                    /* HEADER  */
  YYSYMBOL_CAST = 79,                      /* CAST  */
  YYSYMBOL_END = 80,                       /* END  */
  YYSYMBOL_CASE = 81,                      /* CASE  */
  YYSYMBOL_ELSE = 82,                      /* ELSE  */
  YYSYMBOL_THEN = 83,                      /* THEN  */
  YYSYMBOL_WHEN = 84,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 85,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 86,                   /* INTEGER  */
  YYSYMBOL_INT = 87,                       /* INT  */
  YYSYMBOL_TINYINT = 88,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 89,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 90,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 91,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 92,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 93,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 94,                    /* DOUBLE  */
  YYSYMBOL_REAL = 95,                      /* REAL  */
  YYSYMBOL_DECIMAL = 96,                   /* DECIMAL  */
  YYSYMBOL_DATE = 97,                      /* DATE  */
  YYSYMBOL_TIME = 98,                      /* TIME  */
  YYSYMBOL_DATETIME = 99,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 100,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 101,                     /* UUID  */
  YYSYMBOL_POINT = 102,                    /* POINT  */
  YYSYMBOL_LINE = 103,                     /* LINE  */
  YYSYMBOL_LSEG = 104,                     /* LSEG  */
  YYSYMBOL_BOX = 105,                      /* BOX  */
  YYSYMBOL_PATH = 106,                     /* PATH  */
  YYSYMBOL_POLYGON = 107,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 108,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 109,                     /* BLOB  */
  YYSYMBOL_BITMAP = 110,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 111,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 112,                   /* VECTOR  */
  YYSYMBOL_BIT = 113,                      /* BIT  */
  YYSYMBOL_PRIMARY = 114,                  /* PRIMARY  */
  YYSYMBOL_KEY = 115,                      /* KEY  */
  YYSYMBOL_UNIQUE = 116,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 117,                 /* NULLABLE  */
  YYSYMBOL_IS = 118,                       /* IS  */
  YYSYMBOL_TRUE = 119,                     /* TRUE  */
  YYSYMBOL_FALSE = 120,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 121,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 122,                   /* SECOND  */
  YYSYMBOL_SECONDS = 123,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 124,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 125,                  /* MINUTES  */
  YYSYMBOL_HOUR = 126,                     /* HOUR  */
  YYSYMBOL_HOURS = 127,                    /* HOURS  */
  YYSYMBOL_DAY = 128,                      /* DAY  */
  YYSYMBOL_DAYS = 129,                     /* DAYS  */
  YYSYMBOL_MONTH = 130,                    /* MONTH  */
  YYSYMBOL_MONTHS = 131,                   /* MONTHS  */
  YYSYMBOL_YEAR = 132,                     /* YEAR  */
  YYSYMBOL_YEARS = 133,                    /* YEARS  */
  YYSYMBOL_EQUAL = 134,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 135,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 136,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 137,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 138,                  /* BETWEEN  */
  YYSYMBOL_AND = 139,                      /* AND  */
  YYSYMBOL_OR = 140,                       /* OR  */
  YYSYMBOL_EXTRACT = 141,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 142,                     /* LIKE  */
  YYSYMBOL_DATA = 143,                     /* DATA  */
  YYSYMBOL_LOG = 144,                      /* LOG  */
  YYSYMBOL_BUFFER = 145,                   /* BUFFER  */
  YYSYMBOL_KNN = 146,                      /* KNN  */
  YYSYMBOL_USING = 147,                    /* USING  */
  YYSYMBOL_SESSION = 148,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 149,                   /* GLOBAL  */
  YYSYMBOL_OFF = 150,                      /* OFF  */
  YYSYMBOL_EXPORT = 151,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 152,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 153,                  /* CONFIGS  */
  YYSYMBOL_PROFILES = 154,                 /* PROFILES  */
  YYSYMBOL_STATUS = 155,                   /* STATUS  */
  YYSYMBOL_VAR = 156,                      /* VAR  */
  YYSYMBOL_SEARCH = 157,                   /* SEARCH  */
  YYSYMBOL_MATCH = 158,                    /* MATCH  */
  YYSYMBOL_QUERY = 159,                    /* QUERY  */
  YYSYMBOL_FUSION = 160,                   /* FUSION  */
  YYSYMBOL_NUMBER = 161,                   /* NUMBER  */
  YYSYMBOL_162_ = 162,                     /* '='  */
  YYSYMBOL_163_ = 163,                     /* '<'  */
  YYSYMBOL_164_ = 164,                     /* '>'  */
  YYSYMBOL_165_ = 165,                     /* '+'  */
  YYSYMBOL_166_ = 166,                     /* '-'  */
  YYSYMBOL_167_ = 167,                     /* '*'  */
  YYSYMBOL_168_ = 168,                     /* '/'  */
  YYSYMBOL_169_ = 169,                     /* '%'  */
  YYSYMBOL_170_ = 170,                     /* '['  */
  YYSYMBOL_171_ = 171,                     /* ']'  */
  YYSYMBOL_172_ = 172,                     /* '('  */
  YYSYMBOL_173_ = 173,                     /* ')'  */
  YYSYMBOL_174_ = 174,                     /* '.'  */
  YYSYMBOL_175_ = 175,                     /* ';'  */
  YYSYMBOL_176_ = 176,                     /* ','  */
  YYSYMBOL_YYACCEPT = 177,                 /* $accept  */
  YYSYMBOL_input_pattern = 178,            /* input_pattern  */
  YYSYMBOL_statement_list = 179,           /* statement_list  */
  YYSYMBOL_statement = 180,                /* statement  */
  YYSYMBOL_explainable_statement = 181,    /* explainable_statement  */
  YYSYMBOL_create_statement = 182,         /* create_statement  */
  YYSYMBOL_table_element_array = 183,      /* table_element_array  */
  YYSYMBOL_table_element = 184,            /* table_element  */
  YYSYMBOL_table_column = 185,             /* table_column  */
  YYSYMBOL_column_type = 186,              /* column_type  */
  YYSYMBOL_column_constraints = 187,       /* column_constraints  */
  YYSYMBOL_column_constraint = 188,        /* column_constraint  */
  YYSYMBOL_table_constraint = 189,         /* table_constraint  */
  YYSYMBOL_identifier_array = 190,         /* identifier_array  */
  YYSYMBOL_delete_statement = 191,         /* delete_statement  */
  YYSYMBOL_insert_statement = 192,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 193, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 194,        /* explain_statement  */
  YYSYMBOL_explain_type = 195,             /* explain_type  */
  YYSYMBOL_update_statement = 196,         /* update_statement  */
  YYSYMBOL_update_expr_array = 197,        /* update_expr_array  */
  YYSYMBOL_update_expr = 198,              /* update_expr  */
  YYSYMBOL_drop_statement = 199,           /* drop_statement  */
  YYSYMBOL_copy_statement = 200,           /* copy_statement  */
  YYSYMBOL_select_statement = 201,         /* select_statement  */
  YYSYMBOL_select_with_paren = 202,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 203,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 204, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 205, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 206, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 207,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 208,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 209,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 210,            /* order_by_type  */
  YYSYMBOL_limit_expr = 211,               /* limit_expr  */
  YYSYMBOL_offset_expr = 212,              /* offset_expr  */
  YYSYMBOL_distinct = 213,                 /* distinct  */
  YYSYMBOL_from_clause = 214,              /* from_clause  */
  YYSYMBOL_search_clause = 215,            /* search_clause  */
  YYSYMBOL_where_clause = 216,             /* where_clause  */
  YYSYMBOL_having_clause = 217,            /* having_clause  */
  YYSYMBOL_group_by_clause = 218,          /* group_by_clause  */
  YYSYMBOL_set_operator = 219,             /* set_operator  */
  YYSYMBOL_table_reference = 220,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 221,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 222,     /* table_reference_name  */
  YYSYMBOL_table_name = 223,               /* table_name  */
  YYSYMBOL_table_alias = 224,              /* table_alias  */
  YYSYMBOL_with_clause = 225,              /* with_clause  */
  YYSYMBOL_with_expr_list = 226,           /* with_expr_list  */
  YYSYMBOL_with_expr = 227,                /* with_expr  */
  YYSYMBOL_join_clause = 228,              /* join_clause  */
  YYSYMBOL_join_type = 229,                /* join_type  */
  YYSYMBOL_show_statement = 230,           /* show_statement  */
  YYSYMBOL_flush_statement = 231,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 232,       /* optimize_statement  */
  YYSYMBOL_command_statement = 233,        /* command_statement  */
  YYSYMBOL_expr_array = 234,               /* expr_array  */
  YYSYMBOL_expr_array_list = 235,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 236,               /* expr_alias  */
  YYSYMBOL_expr = 237,                     /* expr  */
  YYSYMBOL_operand = 238,                  /* operand  */
  YYSYMBOL_knn_expr = 239,                 /* knn_expr  */
  YYSYMBOL_match_expr = 240,               /* match_expr  */
  YYSYMBOL_query_expr = 241,               /* query_expr  */
  YYSYMBOL_fusion_expr = 242,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 243,         /* sub_search_array  */
  YYSYMBOL_function_expr = 244,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 245,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 246,             /* between_expr  */
  YYSYMBOL_in_expr = 247,                  /* in_expr  */
  YYSYMBOL_case_expr = 248,                /* case_expr  */
  YYSYMBOL_case_check_array = 249,         /* case_check_array  */
  YYSYMBOL_cast_expr = 250,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 251,            /* subquery_expr  */
  YYSYMBOL_column_expr = 252,              /* column_expr  */
  YYSYMBOL_constant_expr = 253,            /* constant_expr  */
  YYSYMBOL_array_expr = 254,               /* array_expr  */
  YYSYMBOL_long_array_expr = 255,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 256, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 257,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 258, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 259,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 260,         /* copy_option_list  */
  YYSYMBOL_copy_option = 261,              /* copy_option  */
  YYSYMBOL_file_path = 262,                /* file_path  */
  YYSYMBOL_if_exists = 263,                /* if_exists  */
  YYSYMBOL_if_not_exists = 264,            /* if_not_exists  */
  YYSYMBOL_semicolon = 265,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 266,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 267,    /* with_index_param_list  */
  YYSYMBOL_index_param_list = 268,         /* index_param_list  */
  YYSYMBOL_index_param = 269,              /* index_param  */
  YYSYMBOL_index_info_list = 270           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 95 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 399 "parser.cpp"

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
#define YYFINAL  83
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   906

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  177
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  347
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  676

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   416


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
       2,     2,     2,     2,     2,     2,     2,   169,     2,     2,
     172,   173,   167,   165,   176,   166,   174,   168,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   175,
     163,   162,   164,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   170,     2,   171,     2,     2,     2,     2,     2,     2,
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
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   473,   473,   477,   483,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   520,   537,
     552,   576,   592,   610,   639,   643,   649,   652,   658,   693,
     727,   728,   729,   730,   731,   732,   733,   734,   735,   736,
     737,   738,   739,   740,   741,   742,   743,   744,   745,   748,
     750,   751,   752,   753,   756,   757,   758,   759,   760,   761,
     762,   763,   764,   765,   766,   767,   768,   769,   770,   771,
     790,   794,   804,   807,   810,   813,   817,   822,   829,   835,
     845,   861,   895,   908,   911,   918,   924,   927,   930,   933,
     936,   939,   942,   945,   952,   965,   969,   974,   987,  1000,
    1015,  1030,  1045,  1068,  1109,  1154,  1157,  1160,  1169,  1179,
    1182,  1186,  1191,  1213,  1216,  1221,  1237,  1240,  1244,  1248,
    1253,  1259,  1262,  1265,  1269,  1273,  1275,  1279,  1281,  1284,
    1288,  1291,  1295,  1300,  1304,  1307,  1311,  1314,  1318,  1321,
    1325,  1328,  1331,  1334,  1342,  1345,  1360,  1360,  1362,  1376,
    1385,  1390,  1399,  1404,  1409,  1415,  1422,  1425,  1429,  1432,
    1437,  1449,  1456,  1470,  1473,  1476,  1479,  1482,  1485,  1488,
    1494,  1498,  1502,  1506,  1510,  1514,  1518,  1522,  1529,  1540,
    1551,  1563,  1576,  1591,  1595,  1599,  1607,  1622,  1628,  1633,
    1639,  1645,  1653,  1659,  1665,  1671,  1677,  1685,  1691,  1697,
    1713,  1717,  1722,  1726,  1753,  1759,  1763,  1764,  1765,  1766,
    1767,  1769,  1772,  1778,  1781,  1782,  1783,  1784,  1785,  1786,
    1787,  1788,  1790,  1957,  1965,  1976,  1982,  1991,  1997,  2007,
    2011,  2015,  2019,  2023,  2027,  2031,  2035,  2040,  2048,  2056,
    2065,  2072,  2079,  2086,  2093,  2100,  2108,  2116,  2124,  2132,
    2140,  2148,  2156,  2164,  2172,  2180,  2188,  2196,  2226,  2234,
    2243,  2251,  2260,  2268,  2274,  2281,  2287,  2294,  2299,  2306,
    2313,  2321,  2345,  2351,  2357,  2364,  2372,  2379,  2386,  2391,
    2401,  2406,  2411,  2416,  2421,  2426,  2431,  2436,  2441,  2446,
    2449,  2452,  2455,  2459,  2462,  2466,  2470,  2475,  2480,  2484,
    2489,  2494,  2500,  2506,  2512,  2518,  2524,  2530,  2536,  2542,
    2548,  2554,  2560,  2571,  2575,  2580,  2602,  2612,  2618,  2622,
    2623,  2625,  2626,  2628,  2629,  2641,  2649,  2653,  2656,  2660,
    2664,  2669,  2674,  2682,  2689,  2700,  2748,  2797
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
  "PREPARE", "DESCRIBE", "UNION", "ALL", "INTERSECT", "COMPACT", "EXCEPT",
  "FLUSH", "USE", "OPTIMIZE", "DATABASE", "TABLE", "COLLECTION", "TABLES",
  "INTO", "VALUES", "AST", "PIPELINE", "RAW", "LOGICAL", "PHYSICAL",
  "FRAGMENT", "VIEW", "INDEX", "ANALYZE", "VIEWS", "DATABASES", "SEGMENT",
  "SEGMENTS", "BLOCK", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN",
  "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT",
  "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT",
  "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT", "HEADER",
  "CAST", "END", "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER",
  "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT",
  "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP",
  "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE",
  "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY",
  "UNIQUE", "NULLABLE", "IS", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "KNN", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE",
  "CONFIGS", "PROFILES", "STATUS", "VAR", "SEARCH", "MATCH", "QUERY",
  "FUSION", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
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
  "offset_expr", "distinct", "from_clause", "search_clause",
  "where_clause", "having_clause", "group_by_clause", "set_operator",
  "table_reference", "table_reference_unit", "table_reference_name",
  "table_name", "table_alias", "with_clause", "with_expr_list",
  "with_expr", "join_clause", "join_type", "show_statement",
  "flush_statement", "optimize_statement", "command_statement",
  "expr_array", "expr_array_list", "expr_alias", "expr", "operand",
  "knn_expr", "match_expr", "query_expr", "fusion_expr",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "array_expr", "long_array_expr",
  "unclosed_long_array_expr", "double_array_expr",
  "unclosed_double_array_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon",
  "if_not_exists_info", "with_index_param_list", "index_param_list",
  "index_param", "index_info_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-597)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-337)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     215,   174,    53,   207,    98,    49,    98,    65,   342,   156,
      37,    75,   182,   127,    98,   129,    43,   -51,   165,    18,
    -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,   230,  -597,
    -597,   172,  -597,  -597,  -597,  -597,   142,   142,   142,   142,
      45,    98,   149,   149,   149,   149,   149,    24,   209,    98,
     160,   218,   242,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
     682,  -597,  -597,  -597,    95,    97,  -597,  -597,   261,    98,
     211,    98,  -597,  -597,  -597,  -597,  -597,   224,   102,  -597,
     288,   147,   148,  -597,    38,  -597,   307,  -597,  -597,    -1,
     274,  -597,   291,   271,   356,    98,    98,    98,   357,   305,
     193,   301,   385,    98,    98,    98,   388,   392,   411,   352,
     421,   421,    13,    29,  -597,  -597,  -597,  -597,  -597,  -597,
    -597,   230,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
     436,  -597,  -597,   280,   129,   421,  -597,  -597,  -597,  -597,
      -1,  -597,  -597,  -597,   452,   393,   395,   390,  -597,   -15,
    -597,   193,  -597,    98,   459,     8,  -597,  -597,  -597,  -597,
    -597,   404,  -597,   302,   -48,  -597,   452,  -597,  -597,   394,
     399,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
    -597,   417,   172,  -597,  -597,   303,   304,   309,  -597,  -597,
     412,   486,   310,   312,   313,   471,   482,   483,   490,  -597,
    -597,   489,   324,   325,   326,   327,   328,   584,   584,  -597,
     282,   347,   -52,  -597,    15,   627,  -597,  -597,  -597,  -597,
    -597,  -597,  -597,  -597,  -597,  -597,  -597,   329,  -597,  -597,
       6,  -597,    41,  -597,   452,   452,   435,  -597,   -51,    17,
     450,   332,  -597,     0,   336,  -597,    98,   452,   411,  -597,
     175,   337,   338,   509,   343,  -597,  -597,   203,  -597,  -597,
    -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
     584,   339,   700,   443,   452,   452,    -3,   195,  -597,  -597,
    -597,  -597,   412,  -597,   516,   452,   517,   520,   521,   179,
     179,  -597,  -597,   354,   -50,     2,   452,   369,   525,   452,
     452,   -42,   358,   -14,   584,   584,   584,   584,   584,   584,
     584,   584,   584,   584,   584,   584,   584,   584,     5,  -597,
     523,  -597,   527,   370,  -597,   -45,   175,   452,  -597,   230,
     794,   432,   376,    11,  -597,  -597,  -597,   -51,   459,   381,
    -597,   551,   452,   379,  -597,   175,  -597,   353,   353,  -597,
    -597,   452,  -597,    26,   443,   416,   386,    21,   -28,   201,
    -597,   452,   452,   487,   -79,   387,    71,   116,  -597,  -597,
     -51,   398,   345,  -597,    28,  -597,  -597,    32,   352,  -597,
    -597,   422,   396,   584,   347,   445,  -597,   709,   709,   272,
     272,   637,   709,   709,   272,   272,   179,   179,  -597,  -597,
    -597,  -597,  -597,  -597,  -597,   452,  -597,  -597,  -597,   175,
    -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
    -597,   397,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
    -597,  -597,   405,   406,    69,   407,   459,  -597,    17,   230,
     122,   459,  -597,   123,   408,   560,   563,  -597,   135,  -597,
     167,   169,  -597,   403,  -597,   794,   452,  -597,   452,   -55,
     -24,   584,   400,   577,  -597,   578,  -597,   587,    -9,     2,
     538,  -597,  -597,  -597,  -597,  -597,  -597,   540,  -597,   592,
    -597,  -597,  -597,  -597,  -597,   420,   548,   347,   709,   426,
     181,  -597,   584,  -597,   594,   173,   245,   484,   488,  -597,
    -597,    69,  -597,   459,   198,  -597,   455,   213,  -597,   452,
    -597,  -597,  -597,   353,  -597,  -597,  -597,   431,   175,   -41,
    -597,   452,   505,   433,  -597,  -597,   220,   440,   441,    28,
     345,     2,     2,   444,    32,   558,   564,   447,   231,  -597,
    -597,   700,   243,   449,   453,   454,   457,   458,   460,   461,
     462,   467,   472,   473,   474,   479,   481,   485,   499,  -597,
    -597,  -597,   247,  -597,   618,   512,   251,  -597,  -597,  -597,
     175,  -597,   622,  -597,   624,  -597,  -597,  -597,  -597,   572,
     459,  -597,  -597,  -597,  -597,   452,   452,  -597,  -597,  -597,
    -597,   629,   654,   656,   658,   660,   670,   671,   673,   674,
     679,   680,   681,   684,   694,   695,   705,   706,  -597,   613,
     710,  -597,   539,   541,   452,   275,   542,   175,   543,   544,
     546,   547,   549,   550,   553,   554,   555,   556,   559,   561,
     562,   565,   566,   567,   568,   552,  -597,   613,   717,  -597,
     175,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,
    -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,  -597,   728,
    -597,   557,   574,   277,  -597,   731,   153,  -597,   728,   573,
    -597,  -597,  -597,  -597,   613,  -597
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,     0,     0,     0,     0,     0,     0,   167,     0,   334,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   116,
     115,     0,     8,    14,    15,    16,   332,   332,   332,   332,
     332,     0,   330,   330,   330,   330,   330,   160,     0,     0,
       0,     0,     0,    97,   101,    98,    99,   100,   102,    96,
     167,   181,   182,   180,     0,     0,   183,   184,     0,     0,
     188,     0,   193,   194,   195,   197,   196,     0,   166,   168,
       0,     0,     0,     1,   167,     2,   150,   152,   153,     0,
     139,   121,   127,     0,     0,     0,     0,     0,     0,     0,
      94,     0,     0,     0,     0,     0,     0,     0,     0,   145,
       0,     0,     0,     0,    95,    17,    22,    24,    23,    18,
      19,    21,    20,    25,    26,    27,   185,   186,   187,   192,
       0,   189,   209,     0,     0,     0,   120,   119,     4,   151,
       0,   117,   118,   138,     0,     0,   135,     0,    28,     0,
      29,    94,   335,     0,     0,   167,   329,   108,   110,   109,
     111,     0,   161,     0,   145,   105,     0,    90,   328,     0,
       0,   201,   203,   202,   199,   200,   206,   208,   207,   204,
     205,   190,     0,   169,   198,     0,     0,   286,   290,   293,
     294,     0,     0,     0,     0,     0,     0,     0,     0,   291,
     292,     0,     0,     0,     0,     0,     0,     0,     0,   288,
       0,   167,   141,   210,   215,   216,   228,   229,   230,   231,
     225,   220,   219,   218,   226,   227,   217,   224,   223,   301,
       0,   302,     0,   300,     0,     0,   137,   331,   167,     0,
       0,     0,    88,     0,     0,    92,     0,     0,     0,   104,
     144,     0,     0,     0,     0,   124,   123,     0,   312,   311,
     314,   313,   316,   315,   318,   317,   320,   319,   322,   321,
       0,     0,   252,   167,     0,     0,     0,     0,   295,   296,
     297,   298,     0,   299,     0,     0,     0,     0,     0,   254,
     253,   309,   306,     0,     0,     0,     0,   143,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   305,
       0,   308,     0,   126,   128,   133,   134,     0,   122,    31,
       0,     0,     0,     0,    34,    36,    37,   167,     0,    33,
      93,     0,     0,    91,   112,   107,   106,     0,     0,   191,
     170,     0,   247,     0,   167,     0,     0,     0,     0,     0,
     277,     0,     0,     0,     0,     0,     0,     0,   222,   221,
     167,   140,   154,   156,   165,   157,   211,     0,   145,   214,
     270,   271,     0,     0,   167,     0,   251,   261,   262,   265,
     266,     0,   268,   260,   263,   264,   256,   255,   257,   258,
     259,   287,   289,   307,   310,     0,   131,   132,   130,   136,
      40,    43,    44,    41,    42,    45,    46,    60,    47,    49,
      48,    63,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    38,     0,     0,    30,     0,    32,
       0,     0,    89,     0,     0,     0,     0,   327,     0,   323,
       0,     0,   248,     0,   282,     0,     0,   275,     0,     0,
       0,     0,     0,     0,   235,     0,   237,     0,     0,     0,
       0,   174,   175,   176,   177,   173,   178,     0,   163,     0,
     158,   239,   240,   241,   242,   142,   149,   167,   269,     0,
       0,   250,     0,   129,     0,     0,     0,     0,     0,    83,
      84,    39,    80,     0,     0,    35,   347,     0,   212,     0,
     326,   325,   114,     0,   113,   249,   283,     0,   279,     0,
     278,     0,     0,     0,   303,   304,     0,     0,     0,   165,
     155,     0,     0,   162,     0,     0,   147,     0,     0,   284,
     273,   272,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      82,    81,     0,    87,     0,     0,     0,   324,   281,   276,
     280,   267,     0,   233,     0,   236,   238,   159,   171,     0,
       0,   243,   244,   245,   246,     0,     0,   125,   285,   274,
      62,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    86,   338,
       0,   213,     0,     0,     0,     0,   148,   146,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   345,   338,     0,   234,
     172,   164,    61,    67,    68,    65,    66,    69,    70,    71,
      64,    75,    76,    73,    74,    77,    78,    79,    72,     0,
     346,     0,   341,     0,   339,     0,     0,   337,     0,     0,
     342,   344,   343,   340,   338,   232
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -597,  -597,  -597,   663,  -597,   688,  -597,   314,  -597,   298,
    -597,   257,  -597,  -334,   699,   707,   609,  -597,  -597,   708,
    -597,   518,   715,   718,   -57,   760,   -17,   598,   641,   -32,
    -597,  -597,   377,  -597,  -597,  -597,  -597,  -597,  -597,  -158,
    -597,  -597,  -597,  -597,   315,  -225,    22,   254,  -597,  -597,
     651,  -597,  -597,   726,   727,   737,   738,  -255,  -597,   492,
    -165,  -164,  -368,  -367,  -366,  -364,  -597,  -597,  -597,  -597,
    -597,  -597,   531,  -597,  -597,  -597,  -597,  -597,   346,  -597,
     348,  -597,   608,   463,   299,   -43,    31,   232,  -597,  -597,
    -596,  -597,   145,  -597
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   114,    21,   333,   334,   335,   434,
     501,   502,   336,   243,    22,    23,   155,    24,    60,    25,
     164,   165,    26,    27,    28,    29,    30,    91,   141,    92,
     146,   323,   324,   408,   236,   328,   144,   297,   378,   167,
     587,   536,    89,   371,   372,   373,   374,   480,    31,    78,
      79,   375,   477,    32,    33,    34,    35,   212,   343,   213,
     214,   215,   216,   217,   218,   219,   485,   220,   221,   222,
     223,   224,   277,   225,   226,   227,   228,   523,   229,   230,
     231,   232,   233,   448,   449,   169,   102,    94,    85,    99,
     636,   663,   664,   339
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      82,   250,   353,   121,   440,    47,   249,    90,   401,   481,
     482,   483,    86,   484,    87,   166,    88,   171,   172,   173,
     330,   295,   406,   407,    15,   520,    48,   272,    50,   276,
     382,   478,    70,   176,   177,   178,    76,   238,  -333,   569,
      47,   660,   244,   289,   290,     1,   294,     2,     3,     4,
       5,     6,     7,     8,     9,   456,   385,   142,    10,   521,
     299,   300,    11,   100,    12,    13,    14,   298,   170,   325,
     326,   109,   174,   455,   103,   104,   105,   106,   675,    69,
     479,   275,   345,    15,   299,   300,    41,   443,   179,   299,
     300,   129,   184,   132,   299,   300,   451,   462,   299,   300,
     383,    47,   504,   386,  -336,    71,   272,   507,   186,   357,
     358,   299,   300,    15,    93,   299,   300,   149,   150,   151,
     364,    17,    49,   369,   296,   158,   159,   160,   248,   490,
      75,   331,    77,   332,   380,   381,   299,   300,   245,   497,
     387,   388,   389,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   299,   300,   670,   239,   671,   672,
     299,   300,   409,   175,   529,    83,   581,   582,   583,   562,
     584,   140,   402,   340,   370,   241,   341,   319,   203,   180,
      90,   329,   320,   498,   437,   499,   500,   438,    61,    16,
     204,   205,   206,    84,   293,    80,   459,   460,   107,   452,
      62,    63,   296,    36,    37,    38,   187,   188,   189,   190,
      17,    93,   321,    51,    52,    39,    40,   322,   101,   488,
     486,   112,     1,   108,     2,     3,     4,     5,     6,     7,
       8,     9,   538,   110,   111,    10,    42,    43,    44,    11,
     325,    12,    13,    14,   464,   113,   615,   465,    45,    46,
     126,    86,   127,    87,   566,    88,   356,   130,   131,   543,
     544,   545,   546,   547,   128,   351,   548,   549,   344,    95,
      96,    97,    98,   191,   192,   360,   133,   361,   134,   362,
     439,   457,   193,   458,   194,   362,   550,   291,   292,   466,
      15,   518,   467,   519,   135,   506,   508,   522,   341,   296,
     195,   196,   197,   198,    64,    65,   578,   579,   512,    66,
      67,   513,    68,   468,   299,   300,   187,   188,   189,   190,
     136,   137,   199,   200,   201,    72,    73,    74,   541,   139,
     616,   551,   552,   553,   554,   555,   143,   453,   556,   557,
     514,   147,   515,   513,   202,   296,   315,   316,   317,   203,
     187,   188,   189,   190,   540,   145,   570,   296,   558,   148,
     152,   204,   205,   206,   153,   154,    16,   489,   207,   208,
     209,   563,   156,   210,   341,   211,   352,    53,    54,    55,
      56,    57,    58,   191,   192,    59,   565,    17,   157,   341,
     303,   161,   193,   573,   194,   162,   574,   275,   470,  -179,
     471,   472,   473,   474,   589,   475,   476,   296,  -337,  -337,
     195,   196,   197,   198,   163,   166,   590,   191,   192,   591,
     608,   617,    15,   341,   611,   168,   193,   296,   194,   445,
     446,   447,   199,   200,   201,  -337,  -337,   313,   314,   315,
     316,   317,   181,   234,   195,   196,   197,   198,   641,   640,
     667,   341,   182,   668,   202,   187,   188,   189,   190,   203,
     235,   237,   242,   246,   247,   253,   199,   200,   201,   251,
     537,   204,   205,   206,   252,   278,   255,   256,   207,   208,
     209,   257,   273,   210,   274,   211,   279,   280,   202,   187,
     188,   189,   190,   203,   281,   282,   284,   285,   286,   287,
     288,   327,   337,   318,   338,   204,   205,   206,   342,   347,
     348,   354,   207,   208,   209,   349,   350,   210,    15,   211,
     363,   365,   191,   192,   366,   367,   377,   368,   379,   403,
     384,   193,   404,   194,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   405,   435,   436,   195,
     196,   197,   198,   441,   442,   444,   270,   271,   383,   454,
     461,   299,   491,   463,   510,   193,   511,   194,   487,   494,
     210,   199,   200,   201,   469,   355,   516,   495,   496,   503,
     509,   526,   527,   195,   196,   197,   198,   187,   188,   189,
     190,   528,   531,   202,   532,   533,   534,   535,   203,   539,
     542,   559,   564,   560,   568,   199,   200,   201,   585,   572,
     204,   205,   206,   575,   576,   586,   580,   207,   208,   209,
     588,   609,   210,   303,   211,   592,   612,   202,   613,   593,
     594,   614,   203,   595,   596,   618,   597,   598,   599,   304,
     305,   306,   307,   600,   204,   205,   206,   309,   601,   602,
     603,   207,   208,   209,   270,   604,   210,   605,   211,   610,
     619,   606,   620,   193,   621,   194,   622,   310,   311,   312,
     313,   314,   315,   316,   317,   607,   623,   624,   571,   625,
     626,   195,   196,   197,   198,   627,   628,   629,   635,     1,
     630,     2,     3,     4,     5,     6,     7,   301,     9,   302,
     631,   632,    10,   199,   200,   201,    11,   355,    12,    13,
      14,   633,   634,   637,   639,   638,   642,   643,   296,   644,
     645,   661,   646,   647,   659,   202,   648,   649,   650,   651,
     203,   662,   652,   665,   653,   654,   666,   669,   655,   656,
     657,   658,   204,   205,   206,   303,   674,   138,   115,   207,
     208,   209,   505,   517,   210,   303,   211,    15,   561,   116,
     240,   304,   305,   306,   307,   308,   346,   117,   118,   309,
     355,   304,   305,   306,   307,   119,   492,    81,   120,   309,
     254,   185,   493,   577,   530,   183,   122,   123,   376,   310,
     311,   312,   313,   314,   315,   316,   317,   124,   125,   310,
     311,   312,   313,   314,   315,   316,   317,   359,   524,   283,
     525,   450,   567,   673,     0,     0,     0,     0,   303,     0,
       0,     0,     0,     0,     0,     0,     0,   303,     0,     0,
       0,     0,     0,    16,   304,   305,   306,   307,     0,     0,
       0,     0,   309,  -337,  -337,   306,   307,     0,     0,     0,
       0,  -337,     0,     0,    17,     0,     0,     0,     0,     0,
       0,     0,   310,   311,   312,   313,   314,   315,   316,   317,
       0,  -337,   311,   312,   313,   314,   315,   316,   317,   410,
     411,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   428,   429,   430,
       0,     0,   431,     0,     0,   432,   433
};

static const yytype_int16 yycheck[] =
{
      17,   166,   257,    60,   338,     3,   164,     8,     3,   377,
     377,   377,    21,   377,    23,    63,    25,     4,     5,     6,
       3,    73,    67,    68,    75,    80,     4,   191,     6,   194,
      72,     3,    10,     4,     5,     6,    14,    52,     0,    80,
       3,   637,    34,   207,   208,     7,   211,     9,    10,    11,
      12,    13,    14,    15,    16,    83,    70,    89,    20,    83,
     139,   140,    24,    41,    26,    27,    28,    52,   111,   234,
     235,    49,    59,    52,    43,    44,    45,    46,   674,    42,
      52,    84,   247,    75,   139,   140,    33,   342,    59,   139,
     140,    69,   135,    71,   139,   140,   351,   176,   139,   140,
     142,     3,   436,   117,    59,    30,   270,   441,   140,   274,
     275,   139,   140,    75,    69,   139,   140,    95,    96,    97,
     285,   172,    73,   173,   176,   103,   104,   105,   176,   384,
       3,   114,     3,   116,   299,   300,   139,   140,   155,    70,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   139,   140,     3,   172,     5,     6,
     139,   140,   327,   150,   173,     0,   534,   534,   534,   503,
     534,   172,   167,   173,   172,   153,   176,   171,   146,   150,
       8,   238,   176,   114,   173,   116,   117,   176,    32,   151,
     158,   159,   160,   175,   211,   152,   361,   362,   174,   173,
      44,    45,   176,    29,    30,    31,     3,     4,     5,     6,
     172,    69,   171,   148,   149,    41,    42,   176,    69,   383,
     378,     3,     7,    14,     9,    10,    11,    12,    13,    14,
      15,    16,   487,    73,    74,    20,    29,    30,    31,    24,
     405,    26,    27,    28,   173,     3,   580,   176,    41,    42,
     155,    21,   155,    23,   509,    25,   273,    46,    47,    86,
      87,    88,    89,    90,     3,    62,    93,    94,   246,    37,
      38,    39,    40,    70,    71,    80,    52,    82,   176,    84,
     337,    80,    79,    82,    81,    84,   113,     5,     6,   173,
      75,   456,   176,   458,     6,   173,   173,   461,   176,   176,
      97,    98,    99,   100,   148,   149,   531,   532,   173,   153,
     154,   176,   156,   370,   139,   140,     3,     4,     5,     6,
     173,   173,   119,   120,   121,   143,   144,   145,   492,    22,
     585,    86,    87,    88,    89,    90,    62,   354,    93,    94,
     173,    70,   173,   176,   141,   176,   167,   168,   169,   146,
       3,     4,     5,     6,   173,    64,   521,   176,   113,     3,
       3,   158,   159,   160,    59,   172,   151,   384,   165,   166,
     167,   173,    71,   170,   176,   172,   173,    35,    36,    37,
      38,    39,    40,    70,    71,    43,   173,   172,     3,   176,
     118,     3,    79,   173,    81,     3,   176,    84,    53,    54,
      55,    56,    57,    58,   173,    60,    61,   176,   136,   137,
      97,    98,    99,   100,     3,    63,   173,    70,    71,   176,
     173,   586,    75,   176,   173,     4,    79,   176,    81,    76,
      77,    78,   119,   120,   121,   163,   164,   165,   166,   167,
     168,   169,     6,    50,    97,    98,    99,   100,   173,   614,
     173,   176,   172,   176,   141,     3,     4,     5,     6,   146,
      65,    71,     3,    59,   162,    48,   119,   120,   121,    75,
     487,   158,   159,   160,    75,     4,   173,   173,   165,   166,
     167,   172,   172,   170,   172,   172,     4,     4,   141,     3,
       4,     5,     6,   146,     4,     6,   172,   172,   172,   172,
     172,    66,    52,   174,   172,   158,   159,   160,   172,   172,
     172,   172,   165,   166,   167,     6,   173,   170,    75,   172,
       4,     4,    70,    71,     4,     4,   157,   173,     3,     6,
     172,    79,     5,    81,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   176,   115,   172,    97,
      98,    99,   100,   172,     3,   176,    70,    71,   142,   173,
      73,   139,   117,   176,     4,    79,     3,    81,   172,   172,
     170,   119,   120,   121,   176,    70,   173,   172,   172,   172,
     172,     4,     4,    97,    98,    99,   100,     3,     4,     5,
       6,     4,    54,   141,    54,     3,   176,    49,   146,   173,
       6,   117,   147,   115,   173,   119,   120,   121,    50,   176,
     158,   159,   160,   173,   173,    51,   172,   165,   166,   167,
     173,     3,   170,   118,   172,   176,     4,   141,     4,   176,
     176,    59,   146,   176,   176,     6,   176,   176,   176,   134,
     135,   136,   137,   176,   158,   159,   160,   142,   176,   176,
     176,   165,   166,   167,    70,   176,   170,   176,   172,   147,
       6,   176,     6,    79,     6,    81,     6,   162,   163,   164,
     165,   166,   167,   168,   169,   176,     6,     6,   173,     6,
       6,    97,    98,    99,   100,     6,     6,     6,    75,     7,
       6,     9,    10,    11,    12,    13,    14,    70,    16,    72,
       6,     6,    20,   119,   120,   121,    24,    70,    26,    27,
      28,     6,     6,     3,   173,   176,   173,   173,   176,   173,
     173,     4,   173,   173,   172,   141,   173,   173,   173,   173,
     146,     3,   173,   176,   173,   173,   162,     6,   173,   173,
     173,   173,   158,   159,   160,   118,   173,    84,    60,   165,
     166,   167,   438,   455,   170,   118,   172,    75,   501,    60,
     151,   134,   135,   136,   137,   138,   248,    60,    60,   142,
      70,   134,   135,   136,   137,    60,   139,    17,    60,   142,
     182,   140,   405,   529,   469,   134,    60,    60,   296,   162,
     163,   164,   165,   166,   167,   168,   169,    60,    60,   162,
     163,   164,   165,   166,   167,   168,   169,   276,   462,   201,
     462,   348,   513,   668,    -1,    -1,    -1,    -1,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,    -1,    -1,
      -1,    -1,    -1,   151,   134,   135,   136,   137,    -1,    -1,
      -1,    -1,   142,   134,   135,   136,   137,    -1,    -1,    -1,
      -1,   142,    -1,    -1,   172,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   162,   163,   164,   165,   166,   167,   168,   169,
      -1,   162,   163,   164,   165,   166,   167,   168,   169,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,   108,    -1,    -1,   111,   112
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      20,    24,    26,    27,    28,    75,   151,   172,   178,   179,
     180,   182,   191,   192,   194,   196,   199,   200,   201,   202,
     203,   225,   230,   231,   232,   233,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   223,    73,
     223,   148,   149,    35,    36,    37,    38,    39,    40,    43,
     195,    32,    44,    45,   148,   149,   153,   154,   156,    42,
     223,    30,   143,   144,   145,     3,   223,     3,   226,   227,
     152,   202,   203,     0,   175,   265,    21,    23,    25,   219,
       8,   204,   206,    69,   264,   264,   264,   264,   264,   266,
     223,    69,   263,   263,   263,   263,   263,   174,    14,   223,
      73,    74,     3,     3,   181,   182,   191,   192,   196,   199,
     200,   201,   230,   231,   232,   233,   155,   155,     3,   223,
      46,    47,   223,    52,   176,     6,   173,   173,   180,    22,
     172,   205,   206,    62,   213,    64,   207,    70,     3,   223,
     223,   223,     3,    59,   172,   193,    71,     3,   223,   223,
     223,     3,     3,     3,   197,   198,    63,   216,     4,   262,
     262,     4,     5,     6,    59,   150,     4,     5,     6,    59,
     150,     6,   172,   227,   262,   205,   206,     3,     4,     5,
       6,    70,    71,    79,    81,    97,    98,    99,   100,   119,
     120,   121,   141,   146,   158,   159,   160,   165,   166,   167,
     170,   172,   234,   236,   237,   238,   239,   240,   241,   242,
     244,   245,   246,   247,   248,   250,   251,   252,   253,   255,
     256,   257,   258,   259,    50,    65,   211,    71,    52,   172,
     193,   223,     3,   190,    34,   203,    59,   162,   176,   216,
     237,    75,    75,    48,   204,   173,   173,   172,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      70,    71,   238,   172,   172,    84,   237,   249,     4,     4,
       4,     4,     6,   259,   172,   172,   172,   172,   172,   238,
     238,     5,     6,   203,   237,    73,   176,   214,    52,   139,
     140,    70,    72,   118,   134,   135,   136,   137,   138,   142,
     162,   163,   164,   165,   166,   167,   168,   169,   174,   171,
     176,   171,   176,   208,   209,   237,   237,    66,   212,   201,
       3,   114,   116,   183,   184,   185,   189,    52,   172,   270,
     173,   176,   172,   235,   223,   237,   198,   172,   172,     6,
     173,    62,   173,   234,   172,    70,   203,   237,   237,   249,
      80,    82,    84,     4,   237,     4,     4,     4,   173,   173,
     172,   220,   221,   222,   223,   228,   236,   157,   215,     3,
     237,   237,    72,   142,   172,    70,   117,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,     3,   167,     6,     5,   176,    67,    68,   210,   237,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   108,   111,   112,   186,   115,   172,   173,   176,   201,
     190,   172,     3,   234,   176,    76,    77,    78,   260,   261,
     260,   234,   173,   203,   173,    52,    83,    80,    82,   237,
     237,    73,   176,   176,   173,   176,   173,   176,   201,   176,
      53,    55,    56,    57,    58,    60,    61,   229,     3,    52,
     224,   239,   240,   241,   242,   243,   216,   172,   238,   203,
     234,   117,   139,   209,   172,   172,   172,    70,   114,   116,
     117,   187,   188,   172,   190,   184,   173,   190,   173,   172,
       4,     3,   173,   176,   173,   173,   173,   186,   237,   237,
      80,    83,   238,   254,   255,   257,     4,     4,     4,   173,
     221,    54,    54,     3,   176,    49,   218,   203,   234,   173,
     173,   238,     6,    86,    87,    88,    89,    90,    93,    94,
     113,    86,    87,    88,    89,    90,    93,    94,   113,   117,
     115,   188,   190,   173,   147,   173,   234,   261,   173,    80,
     237,   173,   176,   173,   176,   173,   173,   224,   222,   222,
     172,   239,   240,   241,   242,    50,    51,   217,   173,   173,
     173,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   173,     3,
     147,   173,     4,     4,    59,   190,   234,   237,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,    75,   267,     3,   176,   173,
     237,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   172,
     267,     4,     3,   268,   269,   176,   162,   173,   176,     6,
       3,     5,     6,   269,   173,   267
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   177,   178,   179,   179,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   182,   182,
     182,   182,   182,   182,   183,   183,   184,   184,   185,   185,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     187,   187,   188,   188,   188,   188,   189,   189,   190,   190,
     191,   192,   192,   193,   193,   194,   195,   195,   195,   195,
     195,   195,   195,   195,   196,   197,   197,   198,   199,   199,
     199,   199,   199,   200,   200,   201,   201,   201,   201,   202,
     202,   203,   204,   205,   205,   206,   207,   207,   208,   208,
     209,   210,   210,   210,   211,   211,   212,   212,   213,   213,
     214,   214,   215,   215,   216,   216,   217,   217,   218,   218,
     219,   219,   219,   219,   220,   220,   221,   221,   222,   222,
     223,   223,   224,   224,   224,   224,   225,   225,   226,   226,
     227,   228,   228,   229,   229,   229,   229,   229,   229,   229,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   231,   231,   231,   232,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     234,   234,   235,   235,   236,   236,   237,   237,   237,   237,
     237,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   239,   240,   240,   241,   241,   242,   242,   243,
     243,   243,   243,   243,   243,   243,   243,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     245,   245,   246,   247,   247,   248,   248,   248,   248,   249,
     249,   250,   251,   251,   251,   251,   252,   252,   252,   252,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   254,   254,   255,   256,   256,   257,   258,
     258,   259,   259,   259,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   260,   260,   261,   261,   261,   262,   263,
     263,   264,   264,   265,   265,   266,   266,   267,   267,   268,
     268,   269,   269,   269,   269,   270,   270,   270
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       7,     6,     7,     6,     1,     3,     1,     1,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     1,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       1,     2,     2,     1,     1,     2,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     1,     3,     3,     4,     4,
       4,     4,     6,     8,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,     8,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     2,     3,     3,     3,     2,     3,
       4,     6,     3,     2,     2,     2,     2,     2,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     3,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,    13,     6,     8,     4,     6,     4,     6,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     2,     2,     1,     1,     2,
       0,     3,     0,     1,     0,     2,     0,     4,     0,     1,
       3,     1,     3,     3,     3,     6,     7,     3
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
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2010 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2018 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 219 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2032 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 209 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2046 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 298 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2057 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2066 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2075 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 269 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2089 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 262 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2100 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2110 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2120 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2130 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2140 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2150 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2160 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 252 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2174 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 252 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2188 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2198 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2206 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2214 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2223 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2231 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2239 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2247 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 229 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2261 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2270 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2279 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2288 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 289 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2301 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 322 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2310 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 279 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2324 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 279 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2338 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 338 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2348 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2357 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 229 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2371 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 239 "parser.y"
            {
    fprintf(stderr, "destroy expression array list\n");
    if ((((*yyvaluep).expr_array_list_t)) != nullptr) {
        for (auto arr_ptr : *(((*yyvaluep).expr_array_list_t))) {
            for (auto ptr : *arr_ptr) {
                delete ptr;
            }
            delete (arr_ptr);
        }
        delete (((*yyvaluep).expr_array_list_t));
    }
}
#line 2388 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2396 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2404 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2412 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2420 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2428 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2436 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2444 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 229 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2458 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2466 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2482 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2490 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2498 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2511 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2519 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2527 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2543 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2551 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2559 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2567 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2575 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2583 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2599 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2610 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 185 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2624 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 175 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 2638 "parser.cpp"
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
#line 85 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2746 "parser.cpp"

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
#line 473 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2961 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 477 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2972 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 483 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2983 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 490 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2989 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 491 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2995 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 492 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3001 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 493 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3007 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 494 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3013 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 495 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3019 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 496 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3025 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3031 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 498 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3037 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 499 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3043 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 500 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3049 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3055 "parser.cpp"
    break;

  case 17: /* explainable_statement: create_statement  */
#line 503 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3061 "parser.cpp"
    break;

  case 18: /* explainable_statement: drop_statement  */
#line 504 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3067 "parser.cpp"
    break;

  case 19: /* explainable_statement: copy_statement  */
#line 505 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3073 "parser.cpp"
    break;

  case 20: /* explainable_statement: show_statement  */
#line 506 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3079 "parser.cpp"
    break;

  case 21: /* explainable_statement: select_statement  */
#line 507 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3085 "parser.cpp"
    break;

  case 22: /* explainable_statement: delete_statement  */
#line 508 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3091 "parser.cpp"
    break;

  case 23: /* explainable_statement: update_statement  */
#line 509 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3097 "parser.cpp"
    break;

  case 24: /* explainable_statement: insert_statement  */
#line 510 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3103 "parser.cpp"
    break;

  case 25: /* explainable_statement: flush_statement  */
#line 511 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3109 "parser.cpp"
    break;

  case 26: /* explainable_statement: optimize_statement  */
#line 512 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3115 "parser.cpp"
    break;

  case 27: /* explainable_statement: command_statement  */
#line 513 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3121 "parser.cpp"
    break;

  case 28: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 520 "parser.y"
                                                            {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    if(create_schema_info->schema_name_.empty()) {
        yyerror(&yyloc, scanner, result, "Empty database name is given.");
        YYERROR;
    }

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3141 "parser.cpp"
    break;

  case 29: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 537 "parser.y"
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
#line 3159 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 552 "parser.y"
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
#line 3187 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 576 "parser.y"
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
#line 3207 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 592 "parser.y"
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
#line 3228 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 610 "parser.y"
                                                                {
    std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    create_index_info->index_name_ = (yyvsp[-3].if_not_exists_info_t)->info_;
    if ((yyvsp[-3].if_not_exists_info_t)->exists_) {
        create_index_info->conflict_type_ = (yyvsp[-3].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    } else {
        create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
    }
    delete (yyvsp[-3].if_not_exists_info_t);

    create_index_info->index_info_list_ = (yyvsp[0].index_info_list_t);

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3261 "parser.cpp"
    break;

  case 34: /* table_element_array: table_element  */
#line 639 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3270 "parser.cpp"
    break;

  case 35: /* table_element_array: table_element_array ',' table_element  */
#line 643 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3279 "parser.cpp"
    break;

  case 36: /* table_element: table_column  */
#line 649 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3287 "parser.cpp"
    break;

  case 37: /* table_element: table_constraint  */
#line 652 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3295 "parser.cpp"
    break;

  case 38: /* table_column: IDENTIFIER column_type  */
#line 658 "parser.y"
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
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
//            break;
//        }
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
#line 3335 "parser.cpp"
    break;

  case 39: /* table_column: IDENTIFIER column_type column_constraints  */
#line 693 "parser.y"
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
//            break;
//        }
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
#line 3372 "parser.cpp"
    break;

  case 40: /* column_type: BOOLEAN  */
#line 727 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3378 "parser.cpp"
    break;

  case 41: /* column_type: TINYINT  */
#line 728 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3384 "parser.cpp"
    break;

  case 42: /* column_type: SMALLINT  */
#line 729 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3390 "parser.cpp"
    break;

  case 43: /* column_type: INTEGER  */
#line 730 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3396 "parser.cpp"
    break;

  case 44: /* column_type: INT  */
#line 731 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3402 "parser.cpp"
    break;

  case 45: /* column_type: BIGINT  */
#line 732 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3408 "parser.cpp"
    break;

  case 46: /* column_type: HUGEINT  */
#line 733 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3414 "parser.cpp"
    break;

  case 47: /* column_type: FLOAT  */
#line 734 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3420 "parser.cpp"
    break;

  case 48: /* column_type: REAL  */
#line 735 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3426 "parser.cpp"
    break;

  case 49: /* column_type: DOUBLE  */
#line 736 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3432 "parser.cpp"
    break;

  case 50: /* column_type: DATE  */
#line 737 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3438 "parser.cpp"
    break;

  case 51: /* column_type: TIME  */
#line 738 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3444 "parser.cpp"
    break;

  case 52: /* column_type: DATETIME  */
#line 739 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3450 "parser.cpp"
    break;

  case 53: /* column_type: TIMESTAMP  */
#line 740 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3456 "parser.cpp"
    break;

  case 54: /* column_type: UUID  */
#line 741 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3462 "parser.cpp"
    break;

  case 55: /* column_type: POINT  */
#line 742 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3468 "parser.cpp"
    break;

  case 56: /* column_type: LINE  */
#line 743 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3474 "parser.cpp"
    break;

  case 57: /* column_type: LSEG  */
#line 744 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3480 "parser.cpp"
    break;

  case 58: /* column_type: BOX  */
#line 745 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3486 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 748 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3492 "parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 750 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3498 "parser.cpp"
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 751 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3504 "parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 752 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3510 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL  */
#line 753 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3516 "parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 756 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3522 "parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3528 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 758 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3534 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 759 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3540 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 760 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3546 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3552 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 762 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3558 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 763 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3564 "parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 764 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3570 "parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 765 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3576 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 766 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3582 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 767 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3588 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 768 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3594 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 769 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3600 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 770 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3606 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 771 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3612 "parser.cpp"
    break;

  case 80: /* column_constraints: column_constraint  */
#line 790 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3621 "parser.cpp"
    break;

  case 81: /* column_constraints: column_constraints column_constraint  */
#line 794 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3635 "parser.cpp"
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 804 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3643 "parser.cpp"
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 807 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3651 "parser.cpp"
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 810 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3659 "parser.cpp"
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 813 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3667 "parser.cpp"
    break;

  case 86: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 817 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3677 "parser.cpp"
    break;

  case 87: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 822 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3687 "parser.cpp"
    break;

  case 88: /* identifier_array: IDENTIFIER  */
#line 829 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3698 "parser.cpp"
    break;

  case 89: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 835 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3709 "parser.cpp"
    break;

  case 90: /* delete_statement: DELETE FROM table_name where_clause  */
#line 845 "parser.y"
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
#line 3726 "parser.cpp"
    break;

  case 91: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 861 "parser.y"
                                                                                          {
    bool is_error{false};
    for (auto expr_array : *(yyvsp[0].expr_array_list_t)) {
        for (auto expr : *expr_array) {
            if(expr->type_ != infinity::ParsedExprType::kConstant) {
                yyerror(&yyloc, scanner, result, ("Value list has non-constant expression: " + expr->ToString()).c_str());
                is_error = true;
            }
        }
    }
    if(is_error) {
        for (auto expr_array : *(yyvsp[0].expr_array_list_t)) {
            for (auto expr : *expr_array) {
                delete expr;
            }
            delete (expr_array);
        }
        delete (yyvsp[0].expr_array_list_t);
        delete (yyvsp[-3].table_name_t);
        delete (yyvsp[-2].identifier_array_t);
        YYERROR;
    }

    (yyval.insert_stmt) = new infinity::InsertStatement();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
    (yyval.insert_stmt)->columns_ = (yyvsp[-2].identifier_array_t);
    (yyval.insert_stmt)->values_ = (yyvsp[0].expr_array_list_t);
}
#line 3765 "parser.cpp"
    break;

  case 92: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 895 "parser.y"
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
#line 3782 "parser.cpp"
    break;

  case 93: /* optional_identifier_array: '(' identifier_array ')'  */
#line 908 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3790 "parser.cpp"
    break;

  case 94: /* optional_identifier_array: %empty  */
#line 911 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3798 "parser.cpp"
    break;

  case 95: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 918 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3808 "parser.cpp"
    break;

  case 96: /* explain_type: ANALYZE  */
#line 924 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3816 "parser.cpp"
    break;

  case 97: /* explain_type: AST  */
#line 927 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3824 "parser.cpp"
    break;

  case 98: /* explain_type: RAW  */
#line 930 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3832 "parser.cpp"
    break;

  case 99: /* explain_type: LOGICAL  */
#line 933 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3840 "parser.cpp"
    break;

  case 100: /* explain_type: PHYSICAL  */
#line 936 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3848 "parser.cpp"
    break;

  case 101: /* explain_type: PIPELINE  */
#line 939 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3856 "parser.cpp"
    break;

  case 102: /* explain_type: FRAGMENT  */
#line 942 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3864 "parser.cpp"
    break;

  case 103: /* explain_type: %empty  */
#line 945 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3872 "parser.cpp"
    break;

  case 104: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 952 "parser.y"
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
#line 3889 "parser.cpp"
    break;

  case 105: /* update_expr_array: update_expr  */
#line 965 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3898 "parser.cpp"
    break;

  case 106: /* update_expr_array: update_expr_array ',' update_expr  */
#line 969 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3907 "parser.cpp"
    break;

  case 107: /* update_expr: IDENTIFIER '=' expr  */
#line 974 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3919 "parser.cpp"
    break;

  case 108: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 987 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3935 "parser.cpp"
    break;

  case 109: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1000 "parser.y"
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
#line 3953 "parser.cpp"
    break;

  case 110: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1015 "parser.y"
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
#line 3971 "parser.cpp"
    break;

  case 111: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1030 "parser.y"
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
#line 3989 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1045 "parser.y"
                                                {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropIndexInfo> drop_index_info = std::make_shared<infinity::DropIndexInfo>();

    (yyval.drop_stmt)->drop_info_ = drop_index_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-3].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;

    drop_index_info->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        drop_index_info->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    drop_index_info->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4012 "parser.cpp"
    break;

  case 113: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1068 "parser.y"
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
    size_t option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(size_t idx = 0; idx < option_count; ++ idx) {
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
#line 4058 "parser.cpp"
    break;

  case 114: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1109 "parser.y"
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
    size_t option_count = (*(yyvsp[-1].copy_option_array)).size();
    for(size_t idx = 0; idx < option_count; ++ idx) {
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
#line 4104 "parser.cpp"
    break;

  case 115: /* select_statement: select_without_paren  */
#line 1154 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4112 "parser.cpp"
    break;

  case 116: /* select_statement: select_with_paren  */
#line 1157 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4120 "parser.cpp"
    break;

  case 117: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1160 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4134 "parser.cpp"
    break;

  case 118: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1169 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4148 "parser.cpp"
    break;

  case 119: /* select_with_paren: '(' select_without_paren ')'  */
#line 1179 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4156 "parser.cpp"
    break;

  case 120: /* select_with_paren: '(' select_with_paren ')'  */
#line 1182 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4164 "parser.cpp"
    break;

  case 121: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1186 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4173 "parser.cpp"
    break;

  case 122: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1191 "parser.y"
                                                                                                   {
    if((yyvsp[-1].expr_t) == nullptr and (yyvsp[0].expr_t) != nullptr) {
        delete (yyvsp[-3].select_stmt);
        delete (yyvsp[-2].order_by_expr_list_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Offset expression isn't valid without Limit expression");
        YYERROR;
    }
    if((yyvsp[-3].select_stmt)->search_expr_ != nullptr and ((yyvsp[-2].order_by_expr_list_t) != nullptr or (yyvsp[-1].expr_t) != nullptr or (yyvsp[0].expr_t) != nullptr)) {
        delete (yyvsp[-3].select_stmt);
        delete (yyvsp[-2].order_by_expr_list_t);
        delete (yyvsp[-1].expr_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Result modifier(ORDER BY, LIMIT, OFFSET) is conflict with SEARCH expression.");
        YYERROR;
    }
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 4199 "parser.cpp"
    break;

  case 123: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1213 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4207 "parser.cpp"
    break;

  case 124: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1216 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4215 "parser.cpp"
    break;

  case 125: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1221 "parser.y"
                                                                                                {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_list_ = (yyvsp[-5].expr_array_t);
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-6].bool_value);
    (yyval.select_stmt)->table_ref_ = (yyvsp[-4].table_reference_t);
    (yyval.select_stmt)->search_expr_ = (yyvsp[-3].expr_t);
    (yyval.select_stmt)->where_expr_ = (yyvsp[-2].expr_t);
    (yyval.select_stmt)->group_by_list_ = (yyvsp[-1].expr_array_t);
    (yyval.select_stmt)->having_expr_ = (yyvsp[0].expr_t);

    if((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
}
#line 4235 "parser.cpp"
    break;

  case 126: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1237 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4243 "parser.cpp"
    break;

  case 127: /* order_by_clause: %empty  */
#line 1240 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4251 "parser.cpp"
    break;

  case 128: /* order_by_expr_list: order_by_expr  */
#line 1244 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4260 "parser.cpp"
    break;

  case 129: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1248 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4269 "parser.cpp"
    break;

  case 130: /* order_by_expr: expr order_by_type  */
#line 1253 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4279 "parser.cpp"
    break;

  case 131: /* order_by_type: ASC  */
#line 1259 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4287 "parser.cpp"
    break;

  case 132: /* order_by_type: DESC  */
#line 1262 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4295 "parser.cpp"
    break;

  case 133: /* order_by_type: %empty  */
#line 1265 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4303 "parser.cpp"
    break;

  case 134: /* limit_expr: LIMIT expr  */
#line 1269 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4311 "parser.cpp"
    break;

  case 135: /* limit_expr: %empty  */
#line 1273 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4317 "parser.cpp"
    break;

  case 136: /* offset_expr: OFFSET expr  */
#line 1275 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4325 "parser.cpp"
    break;

  case 137: /* offset_expr: %empty  */
#line 1279 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4331 "parser.cpp"
    break;

  case 138: /* distinct: DISTINCT  */
#line 1281 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4339 "parser.cpp"
    break;

  case 139: /* distinct: %empty  */
#line 1284 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4347 "parser.cpp"
    break;

  case 140: /* from_clause: FROM table_reference  */
#line 1288 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4355 "parser.cpp"
    break;

  case 141: /* from_clause: %empty  */
#line 1291 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4363 "parser.cpp"
    break;

  case 142: /* search_clause: SEARCH sub_search_array  */
#line 1295 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4373 "parser.cpp"
    break;

  case 143: /* search_clause: %empty  */
#line 1300 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4381 "parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1304 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4389 "parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1307 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4397 "parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1311 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4405 "parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1314 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4413 "parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1318 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4421 "parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1321 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4429 "parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1325 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4437 "parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1328 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4445 "parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1331 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4453 "parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1334 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4461 "parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1342 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4469 "parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1345 "parser.y"
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
#line 4487 "parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1362 "parser.y"
                                              {
    infinity::TableReference* table_ref = new infinity::TableReference();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        table_ref->db_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    table_ref->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    table_ref->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = table_ref;
}
#line 4505 "parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1376 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4516 "parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1385 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4526 "parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1390 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4538 "parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1399 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4548 "parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1404 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4558 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1409 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4569 "parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1415 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4577 "parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1422 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4585 "parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1425 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4593 "parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1429 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4602 "parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1432 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4611 "parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1437 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4623 "parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1449 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4635 "parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1456 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4648 "parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1470 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4656 "parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1473 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4664 "parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1476 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4672 "parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1479 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4680 "parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1482 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4688 "parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1485 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4696 "parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1488 "parser.y"
                {
}
#line 4703 "parser.cpp"
    break;

  case 180: /* show_statement: SHOW DATABASES  */
#line 1494 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4712 "parser.cpp"
    break;

  case 181: /* show_statement: SHOW TABLES  */
#line 1498 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4721 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW VIEWS  */
#line 1502 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4730 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW CONFIGS  */
#line 1506 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4739 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW PROFILES  */
#line 1510 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4748 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW SESSION STATUS  */
#line 1514 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4757 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW GLOBAL STATUS  */
#line 1518 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4766 "parser.cpp"
    break;

  case 187: /* show_statement: SHOW VAR IDENTIFIER  */
#line 1522 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kVar;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4778 "parser.cpp"
    break;

  case 188: /* show_statement: DESCRIBE table_name  */
#line 1529 "parser.y"
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
#line 4794 "parser.cpp"
    break;

  case 189: /* show_statement: DESCRIBE table_name SEGMENTS  */
#line 1540 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);
}
#line 4810 "parser.cpp"
    break;

  case 190: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE  */
#line 1551 "parser.y"
                                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
    delete (yyvsp[-2].table_name_t);
}
#line 4827 "parser.cpp"
    break;

  case 191: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1563 "parser.y"
                                                          {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
    if((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-4].table_name_t)->schema_name_ptr_;
        free((yyvsp[-4].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-4].table_name_t)->table_name_ptr_;
    free((yyvsp[-4].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
    (yyval.show_stmt)->block_id_ = (yyvsp[0].long_value);
    delete (yyvsp[-4].table_name_t);
}
#line 4845 "parser.cpp"
    break;

  case 192: /* show_statement: DESCRIBE INDEX table_name  */
#line 1576 "parser.y"
                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexes;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4861 "parser.cpp"
    break;

  case 193: /* flush_statement: FLUSH DATA  */
#line 1591 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4870 "parser.cpp"
    break;

  case 194: /* flush_statement: FLUSH LOG  */
#line 1595 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4879 "parser.cpp"
    break;

  case 195: /* flush_statement: FLUSH BUFFER  */
#line 1599 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4888 "parser.cpp"
    break;

  case 196: /* optimize_statement: OPTIMIZE table_name  */
#line 1607 "parser.y"
                                        {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    (yyval.optimize_stmt)->type_ = infinity::OptimizeType::kIRS;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4904 "parser.cpp"
    break;

  case 197: /* command_statement: USE IDENTIFIER  */
#line 1622 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4915 "parser.cpp"
    break;

  case 198: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1628 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4925 "parser.cpp"
    break;

  case 199: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1633 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4936 "parser.cpp"
    break;

  case 200: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1639 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4947 "parser.cpp"
    break;

  case 201: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1645 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4960 "parser.cpp"
    break;

  case 202: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1653 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4971 "parser.cpp"
    break;

  case 203: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1659 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4982 "parser.cpp"
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1665 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4993 "parser.cpp"
    break;

  case 205: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1671 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5004 "parser.cpp"
    break;

  case 206: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1677 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5017 "parser.cpp"
    break;

  case 207: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1685 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5028 "parser.cpp"
    break;

  case 208: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1691 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5039 "parser.cpp"
    break;

  case 209: /* command_statement: COMPACT TABLE table_name  */
#line 1697 "parser.y"
                           {
    (yyval.command_stmt) = new infinity::CommandStatement();
    if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.command_stmt)->command_info_ = std::make_unique<infinity::CompactTable>(std::string((yyvsp[0].table_name_t)->schema_name_ptr_), std::string((yyvsp[0].table_name_t)->table_name_ptr_));
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
        free((yyvsp[0].table_name_t)->table_name_ptr_);
    } else {
        (yyval.command_stmt)->command_info_ = std::make_unique<infinity::CompactTable>(std::string((yyvsp[0].table_name_t)->table_name_ptr_));
        free((yyvsp[0].table_name_t)->table_name_ptr_);
    } delete (yyvsp[0].table_name_t);
}
#line 5055 "parser.cpp"
    break;

  case 210: /* expr_array: expr_alias  */
#line 1713 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5064 "parser.cpp"
    break;

  case 211: /* expr_array: expr_array ',' expr_alias  */
#line 1717 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5073 "parser.cpp"
    break;

  case 212: /* expr_array_list: '(' expr_array ')'  */
#line 1722 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5082 "parser.cpp"
    break;

  case 213: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1726 "parser.y"
                                         {
    if(!(yyvsp[-4].expr_array_list_t)->empty() && (yyvsp[-4].expr_array_list_t)->back()->size() != (yyvsp[-1].expr_array_t)->size()) {
        yyerror(&yyloc, scanner, result, "The expr_array in list shall have the same size.");
        for (auto arr_ptr : *(yyvsp[-4].expr_array_list_t)) {
            for (auto ptr : *arr_ptr) {
                delete ptr;
            }
            delete (arr_ptr);
        }
        delete (yyvsp[-4].expr_array_list_t);
        YYERROR;
    }
    (yyvsp[-4].expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
    (yyval.expr_array_list_t) = (yyvsp[-4].expr_array_list_t);
}
#line 5102 "parser.cpp"
    break;

  case 214: /* expr_alias: expr AS IDENTIFIER  */
#line 1753 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5113 "parser.cpp"
    break;

  case 215: /* expr_alias: expr  */
#line 1759 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5121 "parser.cpp"
    break;

  case 221: /* operand: '(' expr ')'  */
#line 1769 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5129 "parser.cpp"
    break;

  case 222: /* operand: '(' select_without_paren ')'  */
#line 1772 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5140 "parser.cpp"
    break;

  case 223: /* operand: constant_expr  */
#line 1778 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5148 "parser.cpp"
    break;

  case 232: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1790 "parser.y"
                                                                                                      {
    infinity::KnnExpr* knn_expr = new infinity::KnnExpr();
    (yyval.expr_t) = knn_expr;

    // KNN search column
    knn_expr->column_expr_ = (yyvsp[-10].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    if(strcmp((yyvsp[-4].str_value), "l2") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-4].str_value), "ip") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-4].str_value), "cosine") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-4].str_value), "hamming") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
    } else {
        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
            delete param_ptr;
        }
        delete (yyvsp[0].with_index_param_list_t);
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn distance type");
        YYERROR;
    }

    // KNN data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    if(strcmp((yyvsp[-6].str_value), "float") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-8].const_expr_t)->double_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-8].const_expr_t)->long_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "tinyint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_]{};

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((char*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "smallint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        knn_expr->embedding_data_ptr_ = new short int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((short int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "integer") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        knn_expr->embedding_data_ptr_ = new int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bigint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        knn_expr->embedding_data_ptr_ = new long[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->long_array_.data(), knn_expr->dimension_ * sizeof(long));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bit") == 0 and knn_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        if(knn_expr->dimension_ % 8 == 0) {
            knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = knn_expr->dimension_ / 8;
            knn_expr->embedding_data_ptr_ = new char[embedding_size]{};

            for(long i = 0; i < embedding_size; ++ i) {
                char embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        char temp = embedding_unit << 1;
                        temp &= 1;
                        embedding_unit = temp;
                    } else if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        embedding_unit <<= 0;
                    } else {
                        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
                            delete param_ptr;
                        }
                        delete (yyvsp[0].with_index_param_list_t);
                        free((yyvsp[-6].str_value));
                        free((yyvsp[-4].str_value));
                        delete (yyvsp[-8].const_expr_t);
                        delete (yyval.expr_t);
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                }
                ((char*)knn_expr->embedding_data_ptr_)[i] = embedding_unit;
            }
            free((yyvsp[-6].str_value));
            free((yyvsp[-4].str_value));
            delete (yyvsp[-8].const_expr_t);
        } else {
            for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
                delete param_ptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-6].str_value));
            free((yyvsp[-4].str_value));
            delete (yyvsp[-8].const_expr_t);
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "KNN data type is bit which length should be aligned with 8");
            YYERROR;
        }

    } else if(strcmp((yyvsp[-6].str_value), "double") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        knn_expr->embedding_data_ptr_ = new double[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->double_array_.data(), knn_expr->dimension_ * sizeof(double));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else {
        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
            delete param_ptr;
        }
        delete (yyvsp[0].with_index_param_list_t);
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn data type");
        YYERROR;
    }
    knn_expr->topn_ = (yyvsp[-2].long_value);
    knn_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
}
#line 5319 "parser.cpp"
    break;

  case 233: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1957 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5332 "parser.cpp"
    break;

  case 234: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1965 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-5].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5347 "parser.cpp"
    break;

  case 235: /* query_expr: QUERY '(' STRING ')'  */
#line 1976 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5358 "parser.cpp"
    break;

  case 236: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1982 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5371 "parser.cpp"
    break;

  case 237: /* fusion_expr: FUSION '(' STRING ')'  */
#line 1991 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5382 "parser.cpp"
    break;

  case 238: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1997 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5395 "parser.cpp"
    break;

  case 239: /* sub_search_array: knn_expr  */
#line 2007 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5404 "parser.cpp"
    break;

  case 240: /* sub_search_array: match_expr  */
#line 2011 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5413 "parser.cpp"
    break;

  case 241: /* sub_search_array: query_expr  */
#line 2015 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5422 "parser.cpp"
    break;

  case 242: /* sub_search_array: fusion_expr  */
#line 2019 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5431 "parser.cpp"
    break;

  case 243: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 2023 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5440 "parser.cpp"
    break;

  case 244: /* sub_search_array: sub_search_array ',' match_expr  */
#line 2027 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5449 "parser.cpp"
    break;

  case 245: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2031 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5458 "parser.cpp"
    break;

  case 246: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2035 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5467 "parser.cpp"
    break;

  case 247: /* function_expr: IDENTIFIER '(' ')'  */
#line 2040 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5480 "parser.cpp"
    break;

  case 248: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2048 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5493 "parser.cpp"
    break;

  case 249: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2056 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5507 "parser.cpp"
    break;

  case 250: /* function_expr: operand IS NOT NULLABLE  */
#line 2065 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5519 "parser.cpp"
    break;

  case 251: /* function_expr: operand IS NULLABLE  */
#line 2072 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5531 "parser.cpp"
    break;

  case 252: /* function_expr: NOT operand  */
#line 2079 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5543 "parser.cpp"
    break;

  case 253: /* function_expr: '-' operand  */
#line 2086 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5555 "parser.cpp"
    break;

  case 254: /* function_expr: '+' operand  */
#line 2093 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5567 "parser.cpp"
    break;

  case 255: /* function_expr: operand '-' operand  */
#line 2100 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5580 "parser.cpp"
    break;

  case 256: /* function_expr: operand '+' operand  */
#line 2108 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5593 "parser.cpp"
    break;

  case 257: /* function_expr: operand '*' operand  */
#line 2116 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5606 "parser.cpp"
    break;

  case 258: /* function_expr: operand '/' operand  */
#line 2124 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5619 "parser.cpp"
    break;

  case 259: /* function_expr: operand '%' operand  */
#line 2132 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5632 "parser.cpp"
    break;

  case 260: /* function_expr: operand '=' operand  */
#line 2140 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5645 "parser.cpp"
    break;

  case 261: /* function_expr: operand EQUAL operand  */
#line 2148 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5658 "parser.cpp"
    break;

  case 262: /* function_expr: operand NOT_EQ operand  */
#line 2156 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5671 "parser.cpp"
    break;

  case 263: /* function_expr: operand '<' operand  */
#line 2164 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5684 "parser.cpp"
    break;

  case 264: /* function_expr: operand '>' operand  */
#line 2172 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5697 "parser.cpp"
    break;

  case 265: /* function_expr: operand LESS_EQ operand  */
#line 2180 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5710 "parser.cpp"
    break;

  case 266: /* function_expr: operand GREATER_EQ operand  */
#line 2188 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5723 "parser.cpp"
    break;

  case 267: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2196 "parser.y"
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
        delete func_expr;
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    free((yyvsp[-3].str_value));
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5758 "parser.cpp"
    break;

  case 268: /* function_expr: operand LIKE operand  */
#line 2226 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5771 "parser.cpp"
    break;

  case 269: /* function_expr: operand NOT LIKE operand  */
#line 2234 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5784 "parser.cpp"
    break;

  case 270: /* conjunction_expr: expr AND expr  */
#line 2243 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5797 "parser.cpp"
    break;

  case 271: /* conjunction_expr: expr OR expr  */
#line 2251 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5810 "parser.cpp"
    break;

  case 272: /* between_expr: operand BETWEEN operand AND operand  */
#line 2260 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5822 "parser.cpp"
    break;

  case 273: /* in_expr: operand IN '(' expr_array ')'  */
#line 2268 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5833 "parser.cpp"
    break;

  case 274: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2274 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5844 "parser.cpp"
    break;

  case 275: /* case_expr: CASE expr case_check_array END  */
#line 2281 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5855 "parser.cpp"
    break;

  case 276: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2287 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5867 "parser.cpp"
    break;

  case 277: /* case_expr: CASE case_check_array END  */
#line 2294 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5877 "parser.cpp"
    break;

  case 278: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2299 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5888 "parser.cpp"
    break;

  case 279: /* case_check_array: WHEN expr THEN expr  */
#line 2306 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5900 "parser.cpp"
    break;

  case 280: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2313 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5912 "parser.cpp"
    break;

  case 281: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2321 "parser.y"
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($5.width);
//            break;
//        }
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
#line 5940 "parser.cpp"
    break;

  case 282: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2345 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5951 "parser.cpp"
    break;

  case 283: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2351 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5962 "parser.cpp"
    break;

  case 284: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2357 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5974 "parser.cpp"
    break;

  case 285: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2364 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5986 "parser.cpp"
    break;

  case 286: /* column_expr: IDENTIFIER  */
#line 2372 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5998 "parser.cpp"
    break;

  case 287: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2379 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6010 "parser.cpp"
    break;

  case 288: /* column_expr: '*'  */
#line 2386 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6020 "parser.cpp"
    break;

  case 289: /* column_expr: column_expr '.' '*'  */
#line 2391 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6034 "parser.cpp"
    break;

  case 290: /* constant_expr: STRING  */
#line 2401 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6044 "parser.cpp"
    break;

  case 291: /* constant_expr: TRUE  */
#line 2406 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6054 "parser.cpp"
    break;

  case 292: /* constant_expr: FALSE  */
#line 2411 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6064 "parser.cpp"
    break;

  case 293: /* constant_expr: DOUBLE_VALUE  */
#line 2416 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6074 "parser.cpp"
    break;

  case 294: /* constant_expr: LONG_VALUE  */
#line 2421 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6084 "parser.cpp"
    break;

  case 295: /* constant_expr: DATE STRING  */
#line 2426 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6094 "parser.cpp"
    break;

  case 296: /* constant_expr: TIME STRING  */
#line 2431 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6104 "parser.cpp"
    break;

  case 297: /* constant_expr: DATETIME STRING  */
#line 2436 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6114 "parser.cpp"
    break;

  case 298: /* constant_expr: TIMESTAMP STRING  */
#line 2441 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6124 "parser.cpp"
    break;

  case 299: /* constant_expr: INTERVAL interval_expr  */
#line 2446 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6132 "parser.cpp"
    break;

  case 300: /* constant_expr: interval_expr  */
#line 2449 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6140 "parser.cpp"
    break;

  case 301: /* constant_expr: long_array_expr  */
#line 2452 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6148 "parser.cpp"
    break;

  case 302: /* constant_expr: double_array_expr  */
#line 2455 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6156 "parser.cpp"
    break;

  case 303: /* array_expr: long_array_expr  */
#line 2459 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6164 "parser.cpp"
    break;

  case 304: /* array_expr: double_array_expr  */
#line 2462 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6172 "parser.cpp"
    break;

  case 305: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2466 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6180 "parser.cpp"
    break;

  case 306: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2470 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6190 "parser.cpp"
    break;

  case 307: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2475 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6199 "parser.cpp"
    break;

  case 308: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2480 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6207 "parser.cpp"
    break;

  case 309: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2484 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6217 "parser.cpp"
    break;

  case 310: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2489 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6226 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE SECONDS  */
#line 2494 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6237 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE SECOND  */
#line 2500 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6248 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE MINUTES  */
#line 2506 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6259 "parser.cpp"
    break;

  case 314: /* interval_expr: LONG_VALUE MINUTE  */
#line 2512 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6270 "parser.cpp"
    break;

  case 315: /* interval_expr: LONG_VALUE HOURS  */
#line 2518 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6281 "parser.cpp"
    break;

  case 316: /* interval_expr: LONG_VALUE HOUR  */
#line 2524 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6292 "parser.cpp"
    break;

  case 317: /* interval_expr: LONG_VALUE DAYS  */
#line 2530 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6303 "parser.cpp"
    break;

  case 318: /* interval_expr: LONG_VALUE DAY  */
#line 2536 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6314 "parser.cpp"
    break;

  case 319: /* interval_expr: LONG_VALUE MONTHS  */
#line 2542 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6325 "parser.cpp"
    break;

  case 320: /* interval_expr: LONG_VALUE MONTH  */
#line 2548 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6336 "parser.cpp"
    break;

  case 321: /* interval_expr: LONG_VALUE YEARS  */
#line 2554 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6347 "parser.cpp"
    break;

  case 322: /* interval_expr: LONG_VALUE YEAR  */
#line 2560 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6358 "parser.cpp"
    break;

  case 323: /* copy_option_list: copy_option  */
#line 2571 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6367 "parser.cpp"
    break;

  case 324: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2575 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6376 "parser.cpp"
    break;

  case 325: /* copy_option: FORMAT IDENTIFIER  */
#line 2580 "parser.y"
                                {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    if (strcasecmp((yyvsp[0].str_value), "csv") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kCSV;
        free((yyvsp[0].str_value));
    } else if (strcasecmp((yyvsp[0].str_value), "json") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
        free((yyvsp[0].str_value));
    } else if (strcasecmp((yyvsp[0].str_value), "jsonl") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSONL;
        free((yyvsp[0].str_value));
    } else if (strcasecmp((yyvsp[0].str_value), "fvecs") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kFVECS;
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 6403 "parser.cpp"
    break;

  case 326: /* copy_option: DELIMITER STRING  */
#line 2602 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kDelimiter;
    if(strlen((yyvsp[0].str_value)) > 1 && (yyvsp[0].str_value)[0] == '\\') {
        if((yyvsp[0].str_value)[1] == 't') (yyval.copy_option_t)->delimiter_ = '\t';
    }else {
        (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    }
    free((yyvsp[0].str_value));
}
#line 6418 "parser.cpp"
    break;

  case 327: /* copy_option: HEADER  */
#line 2612 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6428 "parser.cpp"
    break;

  case 328: /* file_path: STRING  */
#line 2618 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6436 "parser.cpp"
    break;

  case 329: /* if_exists: IF EXISTS  */
#line 2622 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6442 "parser.cpp"
    break;

  case 330: /* if_exists: %empty  */
#line 2623 "parser.y"
  { (yyval.bool_value) = false; }
#line 6448 "parser.cpp"
    break;

  case 331: /* if_not_exists: IF NOT EXISTS  */
#line 2625 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6454 "parser.cpp"
    break;

  case 332: /* if_not_exists: %empty  */
#line 2626 "parser.y"
  { (yyval.bool_value) = false; }
#line 6460 "parser.cpp"
    break;

  case 335: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2641 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6473 "parser.cpp"
    break;

  case 336: /* if_not_exists_info: %empty  */
#line 2649 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6481 "parser.cpp"
    break;

  case 337: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2653 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6489 "parser.cpp"
    break;

  case 338: /* with_index_param_list: %empty  */
#line 2656 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6497 "parser.cpp"
    break;

  case 339: /* index_param_list: index_param  */
#line 2660 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6506 "parser.cpp"
    break;

  case 340: /* index_param_list: index_param_list ',' index_param  */
#line 2664 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6515 "parser.cpp"
    break;

  case 341: /* index_param: IDENTIFIER  */
#line 2669 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6525 "parser.cpp"
    break;

  case 342: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2674 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6538 "parser.cpp"
    break;

  case 343: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2682 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6550 "parser.cpp"
    break;

  case 344: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2689 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6562 "parser.cpp"
    break;

  case 345: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2700 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
    } else {
        free((yyvsp[-1].str_value));
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
        yyerror(&yyloc, scanner, result, "Unknown index type");
        YYERROR;
    }
    free((yyvsp[-1].str_value));

    size_t index_count = (yyvsp[-4].identifier_array_t)->size();
    if(index_count == 0) {
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
    }
    (yyval.index_info_list_t) = new std::vector<infinity::IndexInfo*>();
    (yyval.index_info_list_t)->reserve(index_count);

    infinity::IndexInfo* index_info = new infinity::IndexInfo();
    index_info->index_type_ = index_type;
    index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[0];
    index_info->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    (yyval.index_info_list_t)->emplace_back(index_info);

    for(size_t idx = 1; idx < index_count; ++ idx) {
        infinity::IndexInfo* index_info = new infinity::IndexInfo();
        index_info->index_type_ = index_type;
        index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[idx];

        size_t param_count = (yyvsp[0].with_index_param_list_t)->size();
        index_info->index_param_list_ = new std::vector<infinity::InitParameter*>();
        index_info->index_param_list_->resize(param_count);
        for(size_t param_idx = 0; param_idx < param_count; ++ param_idx) {
            (*(index_info->index_param_list_))[param_idx] = new infinity::InitParameter();
            *(*(index_info->index_param_list_))[param_idx] = *(*(yyvsp[0].with_index_param_list_t))[param_idx];
        }
        (yyval.index_info_list_t)->emplace_back(index_info);
    }
    delete (yyvsp[-4].identifier_array_t);
}
#line 6615 "parser.cpp"
    break;

  case 346: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2748 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
    } else {
        free((yyvsp[-1].str_value));
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
        yyerror(&yyloc, scanner, result, "Unknown index type");
        YYERROR;
    }
    free((yyvsp[-1].str_value));

    size_t index_count = (yyvsp[-4].identifier_array_t)->size();
    if(index_count == 0) {
        delete (yyvsp[-6].index_info_list_t);
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
    }
    (yyval.index_info_list_t) = (yyvsp[-6].index_info_list_t);
    (yyval.index_info_list_t)->reserve((yyval.index_info_list_t)->size() + index_count);

    infinity::IndexInfo* index_info = new infinity::IndexInfo();
    index_info->index_type_ = index_type;
    index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[0];
    index_info->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    (yyval.index_info_list_t)->emplace_back(index_info);

    for(size_t idx = 1; idx < index_count; ++ idx) {
        infinity::IndexInfo* index_info = new infinity::IndexInfo();
        index_info->index_type_ = index_type;
        index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[idx];

        size_t param_count = (yyvsp[0].with_index_param_list_t)->size();
        index_info->index_param_list_ = new std::vector<infinity::InitParameter*>();
        index_info->index_param_list_->resize(param_count);
        for(size_t param_idx = 0; param_idx < param_count; ++ param_idx) {
            (*(index_info->index_param_list_))[param_idx] = new infinity::InitParameter();
            *(*(index_info->index_param_list_))[param_idx] = *(*(yyvsp[0].with_index_param_list_t))[param_idx];
        }
        (yyval.index_info_list_t)->emplace_back(index_info);
    }
    delete (yyvsp[-4].identifier_array_t);
}
#line 6669 "parser.cpp"
    break;

  case 347: /* index_info_list: '(' identifier_array ')'  */
#line 2797 "parser.y"
                           {
    infinity::IndexType index_type = infinity::IndexType::kSecondary;
    size_t index_count = (yyvsp[-1].identifier_array_t)->size();
    (yyval.index_info_list_t) = new std::vector<infinity::IndexInfo*>();
    (yyval.index_info_list_t)->reserve(index_count);
    for(size_t idx = 0; idx < index_count; ++ idx) {
        infinity::IndexInfo* index_info = new infinity::IndexInfo();
        index_info->index_type_ = index_type;
        index_info->column_name_ = (*(yyvsp[-1].identifier_array_t))[idx];
        (yyval.index_info_list_t)->emplace_back(index_info);
    }
    delete (yyvsp[-1].identifier_array_t);
}
#line 6687 "parser.cpp"
    break;


#line 6691 "parser.cpp"

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

#line 2811 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

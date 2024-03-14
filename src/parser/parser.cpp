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
  YYSYMBOL_PROPERTIES = 29,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 30,                  /* DATABASE  */
  YYSYMBOL_TABLE = 31,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 32,                /* COLLECTION  */
  YYSYMBOL_TABLES = 33,                    /* TABLES  */
  YYSYMBOL_INTO = 34,                      /* INTO  */
  YYSYMBOL_VALUES = 35,                    /* VALUES  */
  YYSYMBOL_AST = 36,                       /* AST  */
  YYSYMBOL_PIPELINE = 37,                  /* PIPELINE  */
  YYSYMBOL_RAW = 38,                       /* RAW  */
  YYSYMBOL_LOGICAL = 39,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 40,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 41,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 42,                      /* VIEW  */
  YYSYMBOL_INDEX = 43,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 44,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 45,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 46,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 47,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 48,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 49,                     /* BLOCK  */
  YYSYMBOL_COLUMNS = 50,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 51,                   /* INDEXES  */
  YYSYMBOL_GROUP = 52,                     /* GROUP  */
  YYSYMBOL_BY = 53,                        /* BY  */
  YYSYMBOL_HAVING = 54,                    /* HAVING  */
  YYSYMBOL_AS = 55,                        /* AS  */
  YYSYMBOL_NATURAL = 56,                   /* NATURAL  */
  YYSYMBOL_JOIN = 57,                      /* JOIN  */
  YYSYMBOL_LEFT = 58,                      /* LEFT  */
  YYSYMBOL_RIGHT = 59,                     /* RIGHT  */
  YYSYMBOL_OUTER = 60,                     /* OUTER  */
  YYSYMBOL_FULL = 61,                      /* FULL  */
  YYSYMBOL_ON = 62,                        /* ON  */
  YYSYMBOL_INNER = 63,                     /* INNER  */
  YYSYMBOL_CROSS = 64,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 65,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 66,                     /* WHERE  */
  YYSYMBOL_ORDER = 67,                     /* ORDER  */
  YYSYMBOL_LIMIT = 68,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 69,                    /* OFFSET  */
  YYSYMBOL_ASC = 70,                       /* ASC  */
  YYSYMBOL_DESC = 71,                      /* DESC  */
  YYSYMBOL_IF = 72,                        /* IF  */
  YYSYMBOL_NOT = 73,                       /* NOT  */
  YYSYMBOL_EXISTS = 74,                    /* EXISTS  */
  YYSYMBOL_IN = 75,                        /* IN  */
  YYSYMBOL_FROM = 76,                      /* FROM  */
  YYSYMBOL_TO = 77,                        /* TO  */
  YYSYMBOL_WITH = 78,                      /* WITH  */
  YYSYMBOL_DELIMITER = 79,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 80,                    /* FORMAT  */
  YYSYMBOL_HEADER = 81,                    /* HEADER  */
  YYSYMBOL_CAST = 82,                      /* CAST  */
  YYSYMBOL_END = 83,                       /* END  */
  YYSYMBOL_CASE = 84,                      /* CASE  */
  YYSYMBOL_ELSE = 85,                      /* ELSE  */
  YYSYMBOL_THEN = 86,                      /* THEN  */
  YYSYMBOL_WHEN = 87,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 88,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 89,                   /* INTEGER  */
  YYSYMBOL_INT = 90,                       /* INT  */
  YYSYMBOL_TINYINT = 91,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 92,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 93,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 94,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 95,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 96,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 97,                    /* DOUBLE  */
  YYSYMBOL_REAL = 98,                      /* REAL  */
  YYSYMBOL_DECIMAL = 99,                   /* DECIMAL  */
  YYSYMBOL_DATE = 100,                     /* DATE  */
  YYSYMBOL_TIME = 101,                     /* TIME  */
  YYSYMBOL_DATETIME = 102,                 /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 103,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 104,                     /* UUID  */
  YYSYMBOL_POINT = 105,                    /* POINT  */
  YYSYMBOL_LINE = 106,                     /* LINE  */
  YYSYMBOL_LSEG = 107,                     /* LSEG  */
  YYSYMBOL_BOX = 108,                      /* BOX  */
  YYSYMBOL_PATH = 109,                     /* PATH  */
  YYSYMBOL_POLYGON = 110,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 111,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 112,                     /* BLOB  */
  YYSYMBOL_BITMAP = 113,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 114,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 115,                   /* VECTOR  */
  YYSYMBOL_BIT = 116,                      /* BIT  */
  YYSYMBOL_PRIMARY = 117,                  /* PRIMARY  */
  YYSYMBOL_KEY = 118,                      /* KEY  */
  YYSYMBOL_UNIQUE = 119,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 120,                 /* NULLABLE  */
  YYSYMBOL_IS = 121,                       /* IS  */
  YYSYMBOL_TRUE = 122,                     /* TRUE  */
  YYSYMBOL_FALSE = 123,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 124,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 125,                   /* SECOND  */
  YYSYMBOL_SECONDS = 126,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 127,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 128,                  /* MINUTES  */
  YYSYMBOL_HOUR = 129,                     /* HOUR  */
  YYSYMBOL_HOURS = 130,                    /* HOURS  */
  YYSYMBOL_DAY = 131,                      /* DAY  */
  YYSYMBOL_DAYS = 132,                     /* DAYS  */
  YYSYMBOL_MONTH = 133,                    /* MONTH  */
  YYSYMBOL_MONTHS = 134,                   /* MONTHS  */
  YYSYMBOL_YEAR = 135,                     /* YEAR  */
  YYSYMBOL_YEARS = 136,                    /* YEARS  */
  YYSYMBOL_EQUAL = 137,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 138,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 139,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 140,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 141,                  /* BETWEEN  */
  YYSYMBOL_AND = 142,                      /* AND  */
  YYSYMBOL_OR = 143,                       /* OR  */
  YYSYMBOL_EXTRACT = 144,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 145,                     /* LIKE  */
  YYSYMBOL_DATA = 146,                     /* DATA  */
  YYSYMBOL_LOG = 147,                      /* LOG  */
  YYSYMBOL_BUFFER = 148,                   /* BUFFER  */
  YYSYMBOL_KNN = 149,                      /* KNN  */
  YYSYMBOL_USING = 150,                    /* USING  */
  YYSYMBOL_SESSION = 151,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 152,                   /* GLOBAL  */
  YYSYMBOL_OFF = 153,                      /* OFF  */
  YYSYMBOL_EXPORT = 154,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 155,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 156,                  /* CONFIGS  */
  YYSYMBOL_PROFILES = 157,                 /* PROFILES  */
  YYSYMBOL_STATUS = 158,                   /* STATUS  */
  YYSYMBOL_VAR = 159,                      /* VAR  */
  YYSYMBOL_SEARCH = 160,                   /* SEARCH  */
  YYSYMBOL_MATCH = 161,                    /* MATCH  */
  YYSYMBOL_QUERY = 162,                    /* QUERY  */
  YYSYMBOL_FUSION = 163,                   /* FUSION  */
  YYSYMBOL_NUMBER = 164,                   /* NUMBER  */
  YYSYMBOL_165_ = 165,                     /* '='  */
  YYSYMBOL_166_ = 166,                     /* '<'  */
  YYSYMBOL_167_ = 167,                     /* '>'  */
  YYSYMBOL_168_ = 168,                     /* '+'  */
  YYSYMBOL_169_ = 169,                     /* '-'  */
  YYSYMBOL_170_ = 170,                     /* '*'  */
  YYSYMBOL_171_ = 171,                     /* '/'  */
  YYSYMBOL_172_ = 172,                     /* '%'  */
  YYSYMBOL_173_ = 173,                     /* '['  */
  YYSYMBOL_174_ = 174,                     /* ']'  */
  YYSYMBOL_175_ = 175,                     /* '('  */
  YYSYMBOL_176_ = 176,                     /* ')'  */
  YYSYMBOL_177_ = 177,                     /* '.'  */
  YYSYMBOL_178_ = 178,                     /* ';'  */
  YYSYMBOL_179_ = 179,                     /* ','  */
  YYSYMBOL_YYACCEPT = 180,                 /* $accept  */
  YYSYMBOL_input_pattern = 181,            /* input_pattern  */
  YYSYMBOL_statement_list = 182,           /* statement_list  */
  YYSYMBOL_statement = 183,                /* statement  */
  YYSYMBOL_explainable_statement = 184,    /* explainable_statement  */
  YYSYMBOL_create_statement = 185,         /* create_statement  */
  YYSYMBOL_table_element_array = 186,      /* table_element_array  */
  YYSYMBOL_table_element = 187,            /* table_element  */
  YYSYMBOL_table_column = 188,             /* table_column  */
  YYSYMBOL_column_type = 189,              /* column_type  */
  YYSYMBOL_column_constraints = 190,       /* column_constraints  */
  YYSYMBOL_column_constraint = 191,        /* column_constraint  */
  YYSYMBOL_table_constraint = 192,         /* table_constraint  */
  YYSYMBOL_identifier_array = 193,         /* identifier_array  */
  YYSYMBOL_delete_statement = 194,         /* delete_statement  */
  YYSYMBOL_insert_statement = 195,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 196, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 197,        /* explain_statement  */
  YYSYMBOL_explain_type = 198,             /* explain_type  */
  YYSYMBOL_update_statement = 199,         /* update_statement  */
  YYSYMBOL_update_expr_array = 200,        /* update_expr_array  */
  YYSYMBOL_update_expr = 201,              /* update_expr  */
  YYSYMBOL_drop_statement = 202,           /* drop_statement  */
  YYSYMBOL_copy_statement = 203,           /* copy_statement  */
  YYSYMBOL_select_statement = 204,         /* select_statement  */
  YYSYMBOL_select_with_paren = 205,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 206,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 207, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 208, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 209, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 210,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 211,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 212,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 213,            /* order_by_type  */
  YYSYMBOL_limit_expr = 214,               /* limit_expr  */
  YYSYMBOL_offset_expr = 215,              /* offset_expr  */
  YYSYMBOL_distinct = 216,                 /* distinct  */
  YYSYMBOL_from_clause = 217,              /* from_clause  */
  YYSYMBOL_search_clause = 218,            /* search_clause  */
  YYSYMBOL_where_clause = 219,             /* where_clause  */
  YYSYMBOL_having_clause = 220,            /* having_clause  */
  YYSYMBOL_group_by_clause = 221,          /* group_by_clause  */
  YYSYMBOL_set_operator = 222,             /* set_operator  */
  YYSYMBOL_table_reference = 223,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 224,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 225,     /* table_reference_name  */
  YYSYMBOL_table_name = 226,               /* table_name  */
  YYSYMBOL_table_alias = 227,              /* table_alias  */
  YYSYMBOL_with_clause = 228,              /* with_clause  */
  YYSYMBOL_with_expr_list = 229,           /* with_expr_list  */
  YYSYMBOL_with_expr = 230,                /* with_expr  */
  YYSYMBOL_join_clause = 231,              /* join_clause  */
  YYSYMBOL_join_type = 232,                /* join_type  */
  YYSYMBOL_show_statement = 233,           /* show_statement  */
  YYSYMBOL_flush_statement = 234,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 235,       /* optimize_statement  */
  YYSYMBOL_command_statement = 236,        /* command_statement  */
  YYSYMBOL_expr_array = 237,               /* expr_array  */
  YYSYMBOL_expr_array_list = 238,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 239,               /* expr_alias  */
  YYSYMBOL_expr = 240,                     /* expr  */
  YYSYMBOL_operand = 241,                  /* operand  */
  YYSYMBOL_knn_expr = 242,                 /* knn_expr  */
  YYSYMBOL_match_expr = 243,               /* match_expr  */
  YYSYMBOL_query_expr = 244,               /* query_expr  */
  YYSYMBOL_fusion_expr = 245,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 246,         /* sub_search_array  */
  YYSYMBOL_function_expr = 247,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 248,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 249,             /* between_expr  */
  YYSYMBOL_in_expr = 250,                  /* in_expr  */
  YYSYMBOL_case_expr = 251,                /* case_expr  */
  YYSYMBOL_case_check_array = 252,         /* case_check_array  */
  YYSYMBOL_cast_expr = 253,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 254,            /* subquery_expr  */
  YYSYMBOL_column_expr = 255,              /* column_expr  */
  YYSYMBOL_constant_expr = 256,            /* constant_expr  */
  YYSYMBOL_array_expr = 257,               /* array_expr  */
  YYSYMBOL_long_array_expr = 258,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 259, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 260,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 261, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 262,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 263,         /* copy_option_list  */
  YYSYMBOL_copy_option = 264,              /* copy_option  */
  YYSYMBOL_file_path = 265,                /* file_path  */
  YYSYMBOL_if_exists = 266,                /* if_exists  */
  YYSYMBOL_if_not_exists = 267,            /* if_not_exists  */
  YYSYMBOL_semicolon = 268,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 269,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 270,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 271, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 272,         /* index_param_list  */
  YYSYMBOL_index_param = 273,              /* index_param  */
  YYSYMBOL_index_info_list = 274           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 95 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 403 "parser.cpp"

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
#define YYFINAL  82
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   873

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  180
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  95
/* YYNRULES -- Number of rules.  */
#define YYNRULES  352
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  685

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   419


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
       2,     2,     2,     2,     2,     2,     2,   172,     2,     2,
     175,   176,   170,   168,   179,   169,   177,   171,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   178,
     166,   165,   167,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   173,     2,   174,     2,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   473,   473,   477,   483,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   520,   537,
     553,   582,   598,   616,   645,   649,   655,   658,   664,   699,
     733,   734,   735,   736,   737,   738,   739,   740,   741,   742,
     743,   744,   745,   746,   747,   748,   749,   750,   751,   754,
     756,   757,   758,   759,   762,   763,   764,   765,   766,   767,
     768,   769,   770,   771,   772,   773,   774,   775,   776,   777,
     796,   800,   810,   813,   816,   819,   823,   828,   835,   841,
     851,   867,   901,   914,   917,   924,   930,   933,   936,   939,
     942,   945,   948,   951,   958,   971,   975,   980,   993,  1006,
    1021,  1036,  1051,  1074,  1115,  1160,  1163,  1166,  1175,  1185,
    1188,  1192,  1197,  1219,  1222,  1227,  1243,  1246,  1250,  1254,
    1259,  1265,  1268,  1271,  1275,  1279,  1281,  1285,  1287,  1290,
    1294,  1297,  1301,  1306,  1310,  1313,  1317,  1320,  1324,  1327,
    1331,  1334,  1337,  1340,  1348,  1351,  1366,  1366,  1368,  1382,
    1391,  1396,  1405,  1410,  1415,  1421,  1428,  1431,  1435,  1438,
    1443,  1455,  1462,  1476,  1479,  1482,  1485,  1488,  1491,  1494,
    1500,  1504,  1508,  1512,  1516,  1520,  1524,  1528,  1535,  1541,
    1552,  1563,  1574,  1586,  1599,  1610,  1628,  1632,  1636,  1644,
    1659,  1665,  1670,  1676,  1682,  1690,  1696,  1702,  1708,  1714,
    1722,  1728,  1734,  1750,  1754,  1759,  1763,  1790,  1796,  1800,
    1801,  1802,  1803,  1804,  1806,  1809,  1815,  1818,  1819,  1820,
    1821,  1822,  1823,  1824,  1825,  1827,  1994,  2002,  2013,  2019,
    2028,  2034,  2044,  2048,  2052,  2056,  2060,  2064,  2068,  2072,
    2077,  2085,  2093,  2102,  2109,  2116,  2123,  2130,  2137,  2145,
    2153,  2161,  2169,  2177,  2185,  2193,  2201,  2209,  2217,  2225,
    2233,  2263,  2271,  2280,  2288,  2297,  2305,  2311,  2318,  2324,
    2331,  2336,  2343,  2350,  2358,  2382,  2388,  2394,  2401,  2409,
    2416,  2423,  2428,  2438,  2443,  2448,  2453,  2458,  2463,  2468,
    2473,  2478,  2483,  2486,  2489,  2492,  2496,  2499,  2503,  2507,
    2512,  2517,  2521,  2526,  2531,  2537,  2543,  2549,  2555,  2561,
    2567,  2573,  2579,  2585,  2591,  2597,  2608,  2612,  2617,  2639,
    2649,  2655,  2659,  2660,  2662,  2663,  2665,  2666,  2678,  2686,
    2690,  2693,  2697,  2700,  2704,  2708,  2713,  2718,  2726,  2733,
    2744,  2792,  2841
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
  "FLUSH", "USE", "OPTIMIZE", "PROPERTIES", "DATABASE", "TABLE",
  "COLLECTION", "TABLES", "INTO", "VALUES", "AST", "PIPELINE", "RAW",
  "LOGICAL", "PHYSICAL", "FRAGMENT", "VIEW", "INDEX", "ANALYZE", "VIEWS",
  "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK", "COLUMNS", "INDEXES",
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM",
  "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID", "POINT",
  "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE", "NULLABLE",
  "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "KNN", "USING",
  "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS", "PROFILES",
  "STATUS", "VAR", "SEARCH", "MATCH", "QUERY", "FUSION", "NUMBER", "'='",
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
  "search_clause", "where_clause", "having_clause", "group_by_clause",
  "set_operator", "table_reference", "table_reference_unit",
  "table_reference_name", "table_name", "table_alias", "with_clause",
  "with_expr_list", "with_expr", "join_clause", "join_type",
  "show_statement", "flush_statement", "optimize_statement",
  "command_statement", "expr_array", "expr_array_list", "expr_alias",
  "expr", "operand", "knn_expr", "match_expr", "query_expr", "fusion_expr",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "array_expr", "long_array_expr",
  "unclosed_long_array_expr", "double_array_expr",
  "unclosed_double_array_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon",
  "if_not_exists_info", "with_index_param_list",
  "optional_table_properties_list", "index_param_list", "index_param",
  "index_info_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-600)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-340)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     542,   199,     2,   216,    52,    57,    52,    62,   537,    60,
      50,    10,    82,    52,   138,    38,   -54,   232,    56,  -600,
    -600,  -600,  -600,  -600,  -600,  -600,  -600,   113,  -600,  -600,
     219,  -600,  -600,  -600,  -600,   167,   167,   167,   167,   -23,
      52,   177,   177,   177,   177,   177,    59,   247,    52,    18,
     270,   278,  -600,  -600,  -600,  -600,  -600,  -600,  -600,   595,
     282,    52,  -600,  -600,  -600,   129,   131,  -600,  -600,   300,
      52,  -600,  -600,  -600,  -600,  -600,   254,    83,  -600,   304,
     160,   166,  -600,   162,  -600,   316,  -600,  -600,     6,   285,
    -600,   288,   289,   365,    52,    52,    52,   366,   299,   195,
     301,   373,    52,    52,    52,   389,   391,   392,   336,   403,
     403,    15,    42,  -600,  -600,  -600,  -600,  -600,  -600,  -600,
     113,  -600,  -600,  -600,  -600,  -600,    13,  -600,  -600,  -600,
    -600,   233,   138,   403,  -600,  -600,  -600,  -600,     6,  -600,
    -600,  -600,   423,   369,   372,   371,  -600,   -48,  -600,   195,
    -600,    52,   443,     0,  -600,  -600,  -600,  -600,  -600,   390,
    -600,   291,   -44,  -600,   423,  -600,  -600,   379,   381,  -600,
    -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,   457,
     456,  -600,  -600,  -600,   219,  -600,  -600,   290,   292,   294,
    -600,  -600,   718,   515,   298,   305,   309,   463,   472,   477,
     479,  -600,  -600,   480,   312,   313,   314,   317,   318,   530,
     530,  -600,   238,   341,   -66,  -600,   -26,   573,  -600,  -600,
    -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,   321,
    -600,  -600,  -117,  -600,  -104,  -600,   423,   423,   422,  -600,
     -54,     9,   439,   320,  -600,    39,   324,  -600,    52,   423,
     392,  -600,   159,   325,   326,  -600,   459,   330,  -600,  -600,
     204,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,
    -600,  -600,  -600,   530,   337,   642,   435,   423,   423,   -71,
      78,  -600,  -600,  -600,  -600,   718,  -600,   511,   423,   513,
     518,   523,   124,   124,  -600,  -600,   352,   -84,     5,   423,
     370,   526,   423,   423,   -57,   356,   -19,   530,   530,   530,
     530,   530,   530,   530,   530,   530,   530,   530,   530,   530,
     530,    12,  -600,   531,  -600,   527,   359,  -600,   -33,   159,
     423,  -600,   113,   727,   421,   367,    49,  -600,  -600,  -600,
     -54,   443,   368,  -600,   538,   423,   361,  -600,   159,  -600,
     119,   119,   544,  -600,   423,  -600,    96,   435,   399,   383,
     -24,   -60,   109,  -600,   423,   423,   484,  -100,   382,   103,
     104,  -600,  -600,   -54,   384,   207,  -600,    14,  -600,  -600,
     -96,   336,  -600,  -600,   406,   387,   530,   341,   444,  -600,
     627,   627,   218,   218,   585,   627,   627,   218,   218,   124,
     124,  -600,  -600,  -600,  -600,  -600,  -600,  -600,   423,  -600,
    -600,  -600,   159,  -600,  -600,  -600,  -600,  -600,  -600,  -600,
    -600,  -600,  -600,  -600,   396,  -600,  -600,  -600,  -600,  -600,
    -600,  -600,  -600,  -600,  -600,   404,   405,   118,   407,   443,
     536,     9,   113,   132,   443,  -600,   154,   408,   583,   587,
    -600,   156,  -600,   164,  -600,   173,  -600,   418,  -600,   727,
     423,  -600,   423,   -43,    17,   530,   427,   591,  -600,   597,
    -600,   606,   -12,     5,   556,  -600,  -600,  -600,  -600,  -600,
    -600,   567,  -600,   622,  -600,  -600,  -600,  -600,  -600,   447,
     575,   341,   627,   452,   175,  -600,   530,  -600,   623,   308,
     358,   514,   517,  -600,  -600,   118,  -600,   443,   180,   461,
    -600,  -600,   490,   184,  -600,   423,  -600,  -600,  -600,   119,
    -600,  -600,  -600,   465,   159,   -31,  -600,   423,   153,   464,
    -600,  -600,   202,   466,   468,    14,   207,     5,     5,   470,
     -96,   594,   596,   473,   227,  -600,  -600,   642,   237,   476,
     478,   481,   482,   483,   486,   487,   488,   489,   491,   492,
     493,   496,   501,   502,   503,  -600,  -600,  -600,   241,  -600,
     648,   653,   519,   242,  -600,  -600,  -600,   159,  -600,   659,
    -600,   682,  -600,  -600,  -600,  -600,   625,   443,  -600,  -600,
    -600,  -600,   423,   423,  -600,  -600,  -600,  -600,   683,   689,
     691,   695,   696,   698,   701,   702,   703,   710,   713,   714,
     715,   720,   722,   723,   725,  -600,   568,   258,  -600,   654,
     731,  -600,   557,   559,   423,   259,   558,   159,   570,   571,
     582,   584,   586,   592,   593,   598,   599,   600,   601,   602,
     607,   608,   609,   610,   612,   217,  -600,   648,   614,  -600,
     654,   755,  -600,   159,  -600,  -600,  -600,  -600,  -600,  -600,
    -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,  -600,
    -600,  -600,  -600,  -600,  -600,  -600,   648,  -600,   611,   260,
     765,  -600,   615,   654,  -600
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,     0,     0,     0,     0,     0,   167,     0,   337,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   116,   115,
       0,     8,    14,    15,    16,   335,   335,   335,   335,   335,
       0,   333,   333,   333,   333,   333,   160,     0,     0,     0,
       0,     0,    97,   101,    98,    99,   100,   102,    96,   167,
       0,     0,   181,   182,   180,     0,     0,   183,   184,     0,
       0,   196,   197,   198,   200,   199,     0,   166,   168,     0,
       0,     0,     1,   167,     2,   150,   152,   153,     0,   139,
     121,   127,     0,     0,     0,     0,     0,     0,     0,    94,
       0,     0,     0,     0,     0,     0,     0,     0,   145,     0,
       0,     0,     0,    95,    17,    22,    24,    23,    18,    19,
      21,    20,    25,    26,    27,   188,   189,   185,   186,   187,
     212,     0,     0,     0,   120,   119,     4,   151,     0,   117,
     118,   138,     0,     0,   135,     0,    28,     0,    29,    94,
     338,     0,     0,   167,   332,   108,   110,   109,   111,     0,
     161,     0,   145,   105,     0,    90,   331,     0,     0,   204,
     206,   205,   202,   203,   209,   211,   210,   207,   208,     0,
       0,   191,   190,   194,     0,   169,   201,     0,     0,   289,
     293,   296,   297,     0,     0,     0,     0,     0,     0,     0,
       0,   294,   295,     0,     0,     0,     0,     0,     0,     0,
       0,   291,     0,   167,   141,   213,   218,   219,   231,   232,
     233,   234,   228,   223,   222,   221,   229,   230,   220,   227,
     226,   304,     0,   305,     0,   303,     0,     0,   137,   334,
     167,     0,     0,     0,    88,     0,     0,    92,     0,     0,
       0,   104,   144,     0,     0,   195,   192,     0,   124,   123,
       0,   315,   314,   317,   316,   319,   318,   321,   320,   323,
     322,   325,   324,     0,     0,   255,   167,     0,     0,     0,
       0,   298,   299,   300,   301,     0,   302,     0,     0,     0,
       0,     0,   257,   256,   312,   309,     0,     0,     0,     0,
     143,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   308,     0,   311,     0,   126,   128,   133,   134,
       0,   122,    31,     0,     0,     0,     0,    34,    36,    37,
     167,     0,    33,    93,     0,     0,    91,   112,   107,   106,
       0,     0,     0,   170,     0,   250,     0,   167,     0,     0,
       0,     0,     0,   280,     0,     0,     0,     0,     0,     0,
       0,   225,   224,   167,   140,   154,   156,   165,   157,   214,
       0,   145,   217,   273,   274,     0,     0,   167,     0,   254,
     264,   265,   268,   269,     0,   271,   263,   266,   267,   259,
     258,   260,   261,   262,   290,   292,   310,   313,     0,   131,
     132,   130,   136,    40,    43,    44,    41,    42,    45,    46,
      60,    47,    49,    48,    63,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     0,     0,    38,     0,     0,
     343,     0,    32,     0,     0,    89,     0,     0,     0,     0,
     330,     0,   326,     0,   193,     0,   251,     0,   285,     0,
       0,   278,     0,     0,     0,     0,     0,     0,   238,     0,
     240,     0,     0,     0,     0,   174,   175,   176,   177,   173,
     178,     0,   163,     0,   158,   242,   243,   244,   245,   142,
     149,   167,   272,     0,     0,   253,     0,   129,     0,     0,
       0,     0,     0,    83,    84,    39,    80,     0,     0,     0,
      30,    35,   352,     0,   215,     0,   329,   328,   114,     0,
     113,   252,   286,     0,   282,     0,   281,     0,     0,     0,
     306,   307,     0,     0,     0,   165,   155,     0,     0,   162,
       0,     0,   147,     0,     0,   287,   276,   275,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    85,    82,    81,     0,    87,
       0,     0,     0,     0,   327,   284,   279,   283,   270,     0,
     236,     0,   239,   241,   159,   171,     0,     0,   246,   247,
     248,   249,     0,     0,   125,   288,   277,    62,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    86,   346,     0,   344,   341,
       0,   216,     0,     0,     0,     0,   148,   146,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   342,     0,     0,   350,
     341,     0,   237,   172,   164,    61,    67,    68,    65,    66,
      69,    70,    71,    64,    75,    76,    73,    74,    77,    78,
      79,    72,   347,   349,   348,   345,     0,   351,     0,     0,
       0,   340,     0,   341,   235
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -600,  -600,  -600,   678,  -600,   741,  -600,   332,  -600,   342,
    -600,   297,  -600,  -337,   744,   745,   656,  -600,  -600,   747,
    -600,   589,   777,   778,   -56,   824,   -16,   671,   719,   -58,
    -600,  -600,   448,  -600,  -600,  -600,  -600,  -600,  -600,  -157,
    -600,  -600,  -600,  -600,   385,  -282,    28,   327,  -600,  -600,
     728,  -600,  -600,   800,   802,   804,   805,  -258,  -600,   566,
    -163,  -165,  -374,  -357,  -355,  -353,  -600,  -600,  -600,  -600,
    -600,  -600,   588,  -600,  -600,  -600,  -600,  -600,   400,  -600,
     402,  -600,   666,   520,   351,   -13,   161,   215,  -600,  -600,
    -599,  -600,   196,   226,  -600
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   113,    20,   336,   337,   338,   437,
     505,   506,   339,   245,    21,    22,   153,    23,    59,    24,
     162,   163,    25,    26,    27,    28,    29,    90,   139,    91,
     144,   326,   327,   411,   238,   331,   142,   300,   381,   165,
     594,   542,    88,   374,   375,   376,   377,   484,    30,    77,
      78,   378,   481,    31,    32,    33,    34,   214,   346,   215,
     216,   217,   218,   219,   220,   221,   489,   222,   223,   224,
     225,   226,   280,   227,   228,   229,   230,   529,   231,   232,
     233,   234,   235,   451,   452,   167,   101,    93,    84,    98,
     649,   510,   617,   618,   342
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      81,   252,   356,   120,   443,   251,   485,   240,    46,    85,
     298,    86,   333,    87,    89,   404,   278,   482,   385,   169,
     170,   171,   164,   486,    14,   487,   460,   488,   275,   301,
     140,   459,    47,   279,    49,   246,    40,   409,   410,  -339,
     526,    75,   302,   303,   292,   293,   174,   175,   176,    92,
     297,   677,   576,   205,   388,    46,   179,   322,   302,   303,
     180,   181,   323,   182,   183,   206,   207,   208,    99,   483,
     324,   302,   303,   328,   329,   325,   108,   172,    14,   466,
     188,    70,   302,   303,   684,    74,   348,   446,   386,   126,
      60,    61,   372,    62,   109,   110,   455,   168,   130,   302,
     303,   389,   508,   527,   177,    63,    64,   513,   275,   302,
     303,   302,   303,   299,   360,   361,   302,   303,   302,   303,
     186,    16,   147,   148,   149,   367,   334,   241,   335,   494,
     156,   157,   158,    48,    85,   250,    86,   247,    87,   383,
     384,    76,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,    71,    72,    73,   302,
     303,   363,  -336,   364,   535,   365,   588,   412,   173,     1,
     568,     2,     3,     4,     5,     6,     7,     8,     9,   243,
     373,   138,   405,   589,   332,   590,    10,   591,    11,    12,
      13,   501,   461,    79,   462,   178,   365,   296,   448,   449,
     450,   463,   464,   102,   103,   104,   105,   189,   190,   191,
     192,    65,    66,    50,    51,   343,    67,    68,   344,    69,
     672,   492,   673,   674,   490,   440,   358,    89,   441,    35,
      36,    37,    82,   544,    83,   502,   106,   503,   504,    92,
      14,    38,    39,   294,   295,   328,    41,    42,    43,   100,
     625,    94,    95,    96,    97,   585,   586,   573,    44,    45,
     359,   107,   132,   474,  -179,   475,   476,   477,   478,   354,
     479,   480,   456,   111,   306,   299,   347,   193,   194,   468,
     470,   112,   469,   471,   442,   125,   195,   127,   196,   128,
     307,   308,   309,   310,   318,   319,   320,   524,   312,   525,
     528,   302,   303,   129,   197,   198,   199,   200,   512,   131,
     133,   344,   189,   190,   191,   192,    15,   472,   313,   314,
     315,   316,   317,   318,   319,   320,   201,   202,   203,   578,
     514,   547,   518,   299,   626,   519,   134,    16,   137,   306,
     520,   457,   135,   519,   189,   190,   191,   192,   204,   521,
     141,   546,   299,   205,   299,   143,   569,  -340,  -340,   344,
     572,   151,   145,   344,   577,   206,   207,   208,   146,   150,
     152,   493,   209,   210,   211,   154,   155,   212,   580,   213,
     355,   581,   193,   194,  -340,  -340,   316,   317,   318,   319,
     320,   195,   159,   196,   160,   161,   278,   549,   550,   551,
     552,   553,   164,   596,   554,   555,   299,   166,   184,   197,
     198,   199,   200,   597,   193,   194,   598,   615,   621,    14,
     344,   299,   236,   195,   556,   196,   189,   190,   191,   192,
     627,   201,   202,   203,   646,   654,   681,   647,   344,   647,
     237,   197,   198,   199,   200,   239,   244,   557,   558,   559,
     560,   561,   248,   204,   562,   563,   249,   253,   205,   254,
     255,   653,   256,   201,   202,   203,   258,   281,   259,   260,
     206,   207,   208,   276,   564,   543,   282,   209,   210,   211,
     277,   283,   212,   284,   213,   204,   285,   287,   288,   289,
     205,   330,   290,   291,   340,   341,   193,   194,   321,   345,
     350,   351,   206,   207,   208,   195,   353,   196,   352,   209,
     210,   211,   357,    14,   212,   366,   213,   368,   189,   190,
     191,   192,   369,   197,   198,   199,   200,   370,   371,   382,
     380,   387,   407,   189,   190,   191,   192,   406,   408,   438,
     447,   445,   439,   444,   386,   201,   202,   203,   302,     1,
     454,     2,     3,     4,     5,     6,     7,     8,     9,   458,
     465,   467,   491,   473,   495,   509,    10,   204,    11,    12,
      13,   498,   205,    52,    53,    54,    55,    56,    57,   499,
     500,    58,   507,   515,   206,   207,   208,   516,   273,   274,
     517,   209,   210,   211,   522,   532,   212,   195,   213,   196,
     212,   533,     1,   273,     2,     3,     4,     5,     6,     7,
     534,     9,   195,   537,   196,   197,   198,   199,   200,    10,
      14,    11,    12,    13,   538,   539,   540,   541,   545,   548,
     197,   198,   199,   200,   565,   566,   570,   201,   202,   203,
     571,   575,   582,   579,   583,   587,   304,   592,   305,   595,
     593,   616,   201,   202,   203,   599,   619,   600,   358,   204,
     601,   602,   603,   622,   205,   604,   605,   606,   607,   620,
     608,   609,   610,    14,   204,   611,   206,   207,   208,   205,
     612,   613,   614,   209,   210,   211,   623,   624,   212,   628,
     213,   206,   207,   208,   306,   629,    15,   630,   209,   210,
     211,   631,   632,   212,   633,   213,   306,   634,   635,   636,
     307,   308,   309,   310,   311,   358,   637,    16,   312,   638,
     639,   640,   307,   308,   309,   310,   641,   496,   642,   643,
     312,   644,   648,   645,   650,   652,   651,   299,   313,   314,
     315,   316,   317,   318,   319,   320,   655,   656,   306,    15,
     313,   314,   315,   316,   317,   318,   319,   320,   657,   678,
     658,   136,   659,   306,  -340,  -340,   309,   310,   660,   661,
      16,   682,  -340,   511,   662,   663,   664,   665,   666,   307,
     308,   309,   310,   667,   668,   669,   670,   312,   671,   676,
     680,   683,  -340,   314,   315,   316,   317,   318,   319,   320,
     114,   523,   567,   115,   116,   242,   117,   313,   314,   315,
     316,   317,   318,   319,   320,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,   430,   431,   432,   433,   118,   119,   434,   349,
      80,   435,   436,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   257,   497,   187,   536,   121,
     185,   122,   584,   123,   124,   379,   530,   362,   531,   286,
     574,   453,   679,   675
};

static const yytype_int16 yycheck[] =
{
      16,   164,   260,    59,   341,   162,   380,    55,     3,    21,
      76,    23,     3,    25,     8,     3,    87,     3,    75,     4,
       5,     6,    66,   380,    78,   380,    86,   380,   193,    55,
      88,    55,     4,   196,     6,    35,    34,    70,    71,    62,
      83,    13,   142,   143,   209,   210,     4,     5,     6,    72,
     213,   650,    83,   149,    73,     3,    43,   174,   142,   143,
      47,    48,   179,    50,    51,   161,   162,   163,    40,    55,
     174,   142,   143,   236,   237,   179,    48,    62,    78,   179,
     138,    31,   142,   143,   683,     3,   249,   345,   145,    61,
      30,    31,   176,    33,    76,    77,   354,   110,    70,   142,
     143,   120,   439,    86,    62,    45,    46,   444,   273,   142,
     143,   142,   143,   179,   277,   278,   142,   143,   142,   143,
     133,   175,    94,    95,    96,   288,   117,   175,   119,   387,
     102,   103,   104,    76,    21,   179,    23,   153,    25,   302,
     303,     3,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   146,   147,   148,   142,
     143,    83,     0,    85,   176,    87,   540,   330,   153,     7,
     507,     9,    10,    11,    12,    13,    14,    15,    16,   151,
     175,   175,   170,   540,   240,   540,    24,   540,    26,    27,
      28,    73,    83,   155,    85,   153,    87,   213,    79,    80,
      81,   364,   365,    42,    43,    44,    45,     3,     4,     5,
       6,   151,   152,   151,   152,   176,   156,   157,   179,   159,
       3,   386,     5,     6,   381,   176,    73,     8,   179,    30,
      31,    32,     0,   491,   178,   117,   177,   119,   120,    72,
      78,    42,    43,     5,     6,   408,    30,    31,    32,    72,
     587,    36,    37,    38,    39,   537,   538,   515,    42,    43,
     276,    14,   179,    56,    57,    58,    59,    60,    61,    65,
      63,    64,   176,     3,   121,   179,   248,    73,    74,   176,
     176,     3,   179,   179,   340,     3,    82,   158,    84,   158,
     137,   138,   139,   140,   170,   171,   172,   460,   145,   462,
     465,   142,   143,     3,   100,   101,   102,   103,   176,    55,
       6,   179,     3,     4,     5,     6,   154,   373,   165,   166,
     167,   168,   169,   170,   171,   172,   122,   123,   124,   176,
     176,   496,   176,   179,   592,   179,   176,   175,    22,   121,
     176,   357,   176,   179,     3,     4,     5,     6,   144,   176,
      65,   176,   179,   149,   179,    67,   176,   139,   140,   179,
     176,    62,    73,   179,   527,   161,   162,   163,     3,     3,
     175,   387,   168,   169,   170,    74,     3,   173,   176,   175,
     176,   179,    73,    74,   166,   167,   168,   169,   170,   171,
     172,    82,     3,    84,     3,     3,    87,    89,    90,    91,
      92,    93,    66,   176,    96,    97,   179,     4,   175,   100,
     101,   102,   103,   176,    73,    74,   179,   176,   176,    78,
     179,   179,    53,    82,   116,    84,     3,     4,     5,     6,
     593,   122,   123,   124,   176,   176,   176,   179,   179,   179,
      68,   100,   101,   102,   103,    74,     3,    89,    90,    91,
      92,    93,    62,   144,    96,    97,   165,    78,   149,    78,
       3,   624,     6,   122,   123,   124,   176,     4,   176,   175,
     161,   162,   163,   175,   116,   491,     4,   168,   169,   170,
     175,     4,   173,     4,   175,   144,     6,   175,   175,   175,
     149,    69,   175,   175,    55,   175,    73,    74,   177,   175,
     175,   175,   161,   162,   163,    82,   176,    84,    49,   168,
     169,   170,   175,    78,   173,     4,   175,     4,     3,     4,
       5,     6,     4,   100,   101,   102,   103,     4,   176,     3,
     160,   175,     5,     3,     4,     5,     6,     6,   179,   118,
     179,     3,   175,   175,   145,   122,   123,   124,   142,     7,
       6,     9,    10,    11,    12,    13,    14,    15,    16,   176,
      76,   179,   175,   179,   120,    29,    24,   144,    26,    27,
      28,   175,   149,    36,    37,    38,    39,    40,    41,   175,
     175,    44,   175,   175,   161,   162,   163,     4,    73,    74,
       3,   168,   169,   170,   176,     4,   173,    82,   175,    84,
     173,     4,     7,    73,     9,    10,    11,    12,    13,    14,
       4,    16,    82,    57,    84,   100,   101,   102,   103,    24,
      78,    26,    27,    28,    57,     3,   179,    52,   176,     6,
     100,   101,   102,   103,   120,   118,   175,   122,   123,   124,
     150,   176,   176,   179,   176,   175,    73,    53,    75,   176,
      54,     3,   122,   123,   124,   179,     3,   179,    73,   144,
     179,   179,   179,     4,   149,   179,   179,   179,   179,   150,
     179,   179,   179,    78,   144,   179,   161,   162,   163,   149,
     179,   179,   179,   168,   169,   170,     4,    62,   173,     6,
     175,   161,   162,   163,   121,     6,   154,     6,   168,   169,
     170,     6,     6,   173,     6,   175,   121,     6,     6,     6,
     137,   138,   139,   140,   141,    73,     6,   175,   145,     6,
       6,     6,   137,   138,   139,   140,     6,   142,     6,     6,
     145,     6,    78,   165,     3,   176,   179,   179,   165,   166,
     167,   168,   169,   170,   171,   172,   176,   176,   121,   154,
     165,   166,   167,   168,   169,   170,   171,   172,   176,     4,
     176,    83,   176,   121,   137,   138,   139,   140,   176,   176,
     175,     6,   145,   441,   176,   176,   176,   176,   176,   137,
     138,   139,   140,   176,   176,   176,   176,   145,   176,   175,
     179,   176,   165,   166,   167,   168,   169,   170,   171,   172,
      59,   459,   505,    59,    59,   149,    59,   165,   166,   167,
     168,   169,   170,   171,   172,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,    59,    59,   111,   250,
      16,   114,   115,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   184,   408,   138,   473,    59,
     132,    59,   535,    59,    59,   299,   466,   279,   466,   203,
     519,   351,   676,   647
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      24,    26,    27,    28,    78,   154,   175,   181,   182,   183,
     185,   194,   195,   197,   199,   202,   203,   204,   205,   206,
     228,   233,   234,   235,   236,    30,    31,    32,    42,    43,
      34,    30,    31,    32,    42,    43,     3,   226,    76,   226,
     151,   152,    36,    37,    38,    39,    40,    41,    44,   198,
      30,    31,    33,    45,    46,   151,   152,   156,   157,   159,
      31,   146,   147,   148,     3,   226,     3,   229,   230,   155,
     205,   206,     0,   178,   268,    21,    23,    25,   222,     8,
     207,   209,    72,   267,   267,   267,   267,   267,   269,   226,
      72,   266,   266,   266,   266,   266,   177,    14,   226,    76,
      77,     3,     3,   184,   185,   194,   195,   199,   202,   203,
     204,   233,   234,   235,   236,     3,   226,   158,   158,     3,
     226,    55,   179,     6,   176,   176,   183,    22,   175,   208,
     209,    65,   216,    67,   210,    73,     3,   226,   226,   226,
       3,    62,   175,   196,    74,     3,   226,   226,   226,     3,
       3,     3,   200,   201,    66,   219,     4,   265,   265,     4,
       5,     6,    62,   153,     4,     5,     6,    62,   153,    43,
      47,    48,    50,    51,   175,   230,   265,   208,   209,     3,
       4,     5,     6,    73,    74,    82,    84,   100,   101,   102,
     103,   122,   123,   124,   144,   149,   161,   162,   163,   168,
     169,   170,   173,   175,   237,   239,   240,   241,   242,   243,
     244,   245,   247,   248,   249,   250,   251,   253,   254,   255,
     256,   258,   259,   260,   261,   262,    53,    68,   214,    74,
      55,   175,   196,   226,     3,   193,    35,   206,    62,   165,
     179,   219,   240,    78,    78,     3,     6,   207,   176,   176,
     175,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,    73,    74,   241,   175,   175,    87,   240,
     252,     4,     4,     4,     4,     6,   262,   175,   175,   175,
     175,   175,   241,   241,     5,     6,   206,   240,    76,   179,
     217,    55,   142,   143,    73,    75,   121,   137,   138,   139,
     140,   141,   145,   165,   166,   167,   168,   169,   170,   171,
     172,   177,   174,   179,   174,   179,   211,   212,   240,   240,
      69,   215,   204,     3,   117,   119,   186,   187,   188,   192,
      55,   175,   274,   176,   179,   175,   238,   226,   240,   201,
     175,   175,    49,   176,    65,   176,   237,   175,    73,   206,
     240,   240,   252,    83,    85,    87,     4,   240,     4,     4,
       4,   176,   176,   175,   223,   224,   225,   226,   231,   239,
     160,   218,     3,   240,   240,    75,   145,   175,    73,   120,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,     3,   170,     6,     5,   179,    70,
      71,   213,   240,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   111,   114,   115,   189,   118,   175,
     176,   179,   204,   193,   175,     3,   237,   179,    79,    80,
      81,   263,   264,   263,     6,   237,   176,   206,   176,    55,
      86,    83,    85,   240,   240,    76,   179,   179,   176,   179,
     176,   179,   204,   179,    56,    58,    59,    60,    61,    63,
      64,   232,     3,    55,   227,   242,   243,   244,   245,   246,
     219,   175,   241,   206,   237,   120,   142,   212,   175,   175,
     175,    73,   117,   119,   120,   190,   191,   175,   193,    29,
     271,   187,   176,   193,   176,   175,     4,     3,   176,   179,
     176,   176,   176,   189,   240,   240,    83,    86,   241,   257,
     258,   260,     4,     4,     4,   176,   224,    57,    57,     3,
     179,    52,   221,   206,   237,   176,   176,   241,     6,    89,
      90,    91,    92,    93,    96,    97,   116,    89,    90,    91,
      92,    93,    96,    97,   116,   120,   118,   191,   193,   176,
     175,   150,   176,   237,   264,   176,    83,   240,   176,   179,
     176,   179,   176,   176,   227,   225,   225,   175,   242,   243,
     244,   245,    53,    54,   220,   176,   176,   176,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   176,     3,   272,   273,     3,
     150,   176,     4,     4,    62,   193,   237,   240,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   165,   176,   179,    78,   270,
       3,   179,   176,   240,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,     3,     5,     6,   273,   175,   270,     4,   272,
     179,   176,     6,   176,   270
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   180,   181,   182,   182,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   185,   185,
     185,   185,   185,   185,   186,   186,   187,   187,   188,   188,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     190,   190,   191,   191,   191,   191,   192,   192,   193,   193,
     194,   195,   195,   196,   196,   197,   198,   198,   198,   198,
     198,   198,   198,   198,   199,   200,   200,   201,   202,   202,
     202,   202,   202,   203,   203,   204,   204,   204,   204,   205,
     205,   206,   207,   208,   208,   209,   210,   210,   211,   211,
     212,   213,   213,   213,   214,   214,   215,   215,   216,   216,
     217,   217,   218,   218,   219,   219,   220,   220,   221,   221,
     222,   222,   222,   222,   223,   223,   224,   224,   225,   225,
     226,   226,   227,   227,   227,   227,   228,   228,   229,   229,
     230,   231,   231,   232,   232,   232,   232,   232,   232,   232,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   234,   234,   234,   235,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   237,   237,   238,   238,   239,   239,   240,
     240,   240,   240,   240,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   242,   243,   243,   244,   244,
     245,   245,   246,   246,   246,   246,   246,   246,   246,   246,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   248,   248,   249,   250,   250,   251,   251,
     251,   251,   252,   252,   253,   254,   254,   254,   254,   255,
     255,   255,   255,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   257,   257,   258,   259,
     259,   260,   261,   261,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   263,   263,   264,   264,
     264,   265,   266,   266,   267,   267,   268,   268,   269,   269,
     270,   270,   271,   271,   272,   272,   273,   273,   273,   273,
     274,   274,   274
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       8,     6,     7,     6,     1,     3,     1,     1,     2,     3,
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
       2,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       4,     4,     5,     7,     4,     5,     2,     2,     2,     2,
       2,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     3,     1,     3,     3,     5,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,    13,     6,     8,     4,     6,
       4,     6,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     4,     5,     4,     3,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       6,     3,     4,     3,     3,     5,     5,     6,     4,     6,
       3,     5,     4,     5,     6,     4,     5,     5,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     2,     2,
       3,     2,     2,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     2,     2,
       1,     1,     2,     0,     3,     0,     1,     0,     2,     0,
       4,     0,     4,     0,     1,     3,     1,     3,     3,     3,
       6,     7,     3
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
#line 2015 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2023 "parser.cpp"
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
#line 2037 "parser.cpp"
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
#line 2051 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 298 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2062 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2071 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2080 "parser.cpp"
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
#line 2094 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 262 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2105 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2115 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2125 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2135 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2145 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2155 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2165 "parser.cpp"
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
#line 2179 "parser.cpp"
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
#line 2193 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2203 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2211 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2219 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2228 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2236 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2244 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2252 "parser.cpp"
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
#line 2266 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2275 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2284 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2293 "parser.cpp"
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
#line 2306 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 322 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2315 "parser.cpp"
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
#line 2329 "parser.cpp"
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
#line 2343 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 338 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2353 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2362 "parser.cpp"
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
#line 2376 "parser.cpp"
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
#line 2393 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2401 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2409 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2417 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2425 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2433 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2441 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2449 "parser.cpp"
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
#line 2463 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2471 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2479 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2487 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2495 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2503 "parser.cpp"
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
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2524 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2532 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2540 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2548 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2556 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2564 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2572 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2580 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2596 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2604 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2615 "parser.cpp"
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
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
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
#line 2643 "parser.cpp"
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
#line 2657 "parser.cpp"
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

#line 2765 "parser.cpp"

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
#line 2980 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 477 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2991 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 483 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3002 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 490 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3008 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 491 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3014 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 492 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3020 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 493 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3026 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 494 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3032 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 495 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3038 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 496 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3044 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3050 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 498 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3056 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 499 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3062 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 500 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3068 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3074 "parser.cpp"
    break;

  case 17: /* explainable_statement: create_statement  */
#line 503 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3080 "parser.cpp"
    break;

  case 18: /* explainable_statement: drop_statement  */
#line 504 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3086 "parser.cpp"
    break;

  case 19: /* explainable_statement: copy_statement  */
#line 505 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3092 "parser.cpp"
    break;

  case 20: /* explainable_statement: show_statement  */
#line 506 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3098 "parser.cpp"
    break;

  case 21: /* explainable_statement: select_statement  */
#line 507 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3104 "parser.cpp"
    break;

  case 22: /* explainable_statement: delete_statement  */
#line 508 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3110 "parser.cpp"
    break;

  case 23: /* explainable_statement: update_statement  */
#line 509 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3116 "parser.cpp"
    break;

  case 24: /* explainable_statement: insert_statement  */
#line 510 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3122 "parser.cpp"
    break;

  case 25: /* explainable_statement: flush_statement  */
#line 511 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3128 "parser.cpp"
    break;

  case 26: /* explainable_statement: optimize_statement  */
#line 512 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3134 "parser.cpp"
    break;

  case 27: /* explainable_statement: command_statement  */
#line 513 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3140 "parser.cpp"
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
#line 3160 "parser.cpp"
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
#line 3178 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 553 "parser.y"
                                                                                                   {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-4].table_name_t)->schema_name_ptr_;
        free((yyvsp[-4].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-4].table_name_t)->table_name_ptr_;
    free((yyvsp[-4].table_name_t)->table_name_ptr_);
    delete (yyvsp[-4].table_name_t);

    for (infinity::TableElement*& element : *(yyvsp[-2].table_element_array_t)) {
        if(element->type_ == infinity::TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((infinity::ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((infinity::TableConstraint*)element);
        }
    }
    delete (yyvsp[-2].table_element_array_t);

    if ((yyvsp[0].with_index_param_list_t) != nullptr) {
        create_table_info->properties_ = std::move(*(yyvsp[0].with_index_param_list_t));
        delete (yyvsp[0].with_index_param_list_t);
    }

    (yyval.create_stmt)->create_info_ = create_table_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-5].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3211 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 582 "parser.y"
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
#line 3231 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 598 "parser.y"
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
#line 3252 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 616 "parser.y"
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
#line 3285 "parser.cpp"
    break;

  case 34: /* table_element_array: table_element  */
#line 645 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3294 "parser.cpp"
    break;

  case 35: /* table_element_array: table_element_array ',' table_element  */
#line 649 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3303 "parser.cpp"
    break;

  case 36: /* table_element: table_column  */
#line 655 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3311 "parser.cpp"
    break;

  case 37: /* table_element: table_constraint  */
#line 658 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3319 "parser.cpp"
    break;

  case 38: /* table_column: IDENTIFIER column_type  */
#line 664 "parser.y"
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
#line 3359 "parser.cpp"
    break;

  case 39: /* table_column: IDENTIFIER column_type column_constraints  */
#line 699 "parser.y"
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
#line 3396 "parser.cpp"
    break;

  case 40: /* column_type: BOOLEAN  */
#line 733 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3402 "parser.cpp"
    break;

  case 41: /* column_type: TINYINT  */
#line 734 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3408 "parser.cpp"
    break;

  case 42: /* column_type: SMALLINT  */
#line 735 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3414 "parser.cpp"
    break;

  case 43: /* column_type: INTEGER  */
#line 736 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3420 "parser.cpp"
    break;

  case 44: /* column_type: INT  */
#line 737 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3426 "parser.cpp"
    break;

  case 45: /* column_type: BIGINT  */
#line 738 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3432 "parser.cpp"
    break;

  case 46: /* column_type: HUGEINT  */
#line 739 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3438 "parser.cpp"
    break;

  case 47: /* column_type: FLOAT  */
#line 740 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3444 "parser.cpp"
    break;

  case 48: /* column_type: REAL  */
#line 741 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3450 "parser.cpp"
    break;

  case 49: /* column_type: DOUBLE  */
#line 742 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3456 "parser.cpp"
    break;

  case 50: /* column_type: DATE  */
#line 743 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3462 "parser.cpp"
    break;

  case 51: /* column_type: TIME  */
#line 744 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3468 "parser.cpp"
    break;

  case 52: /* column_type: DATETIME  */
#line 745 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3474 "parser.cpp"
    break;

  case 53: /* column_type: TIMESTAMP  */
#line 746 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3480 "parser.cpp"
    break;

  case 54: /* column_type: UUID  */
#line 747 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3486 "parser.cpp"
    break;

  case 55: /* column_type: POINT  */
#line 748 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3492 "parser.cpp"
    break;

  case 56: /* column_type: LINE  */
#line 749 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3498 "parser.cpp"
    break;

  case 57: /* column_type: LSEG  */
#line 750 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3504 "parser.cpp"
    break;

  case 58: /* column_type: BOX  */
#line 751 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3510 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 754 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3516 "parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 756 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3522 "parser.cpp"
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3528 "parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 758 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3534 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL  */
#line 759 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3540 "parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 762 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3546 "parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 763 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3552 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 764 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3558 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 765 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3564 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 766 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3570 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 767 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3576 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 768 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3582 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 769 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3588 "parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 770 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3594 "parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 771 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3600 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 772 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3606 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 773 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3612 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 774 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3618 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 775 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3624 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 776 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3630 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 777 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3636 "parser.cpp"
    break;

  case 80: /* column_constraints: column_constraint  */
#line 796 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3645 "parser.cpp"
    break;

  case 81: /* column_constraints: column_constraints column_constraint  */
#line 800 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3659 "parser.cpp"
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 810 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3667 "parser.cpp"
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 813 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3675 "parser.cpp"
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 816 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3683 "parser.cpp"
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 819 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3691 "parser.cpp"
    break;

  case 86: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 823 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3701 "parser.cpp"
    break;

  case 87: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 828 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3711 "parser.cpp"
    break;

  case 88: /* identifier_array: IDENTIFIER  */
#line 835 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3722 "parser.cpp"
    break;

  case 89: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 841 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3733 "parser.cpp"
    break;

  case 90: /* delete_statement: DELETE FROM table_name where_clause  */
#line 851 "parser.y"
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
#line 3750 "parser.cpp"
    break;

  case 91: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 867 "parser.y"
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
#line 3789 "parser.cpp"
    break;

  case 92: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 901 "parser.y"
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
#line 3806 "parser.cpp"
    break;

  case 93: /* optional_identifier_array: '(' identifier_array ')'  */
#line 914 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3814 "parser.cpp"
    break;

  case 94: /* optional_identifier_array: %empty  */
#line 917 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3822 "parser.cpp"
    break;

  case 95: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 924 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3832 "parser.cpp"
    break;

  case 96: /* explain_type: ANALYZE  */
#line 930 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3840 "parser.cpp"
    break;

  case 97: /* explain_type: AST  */
#line 933 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3848 "parser.cpp"
    break;

  case 98: /* explain_type: RAW  */
#line 936 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3856 "parser.cpp"
    break;

  case 99: /* explain_type: LOGICAL  */
#line 939 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3864 "parser.cpp"
    break;

  case 100: /* explain_type: PHYSICAL  */
#line 942 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3872 "parser.cpp"
    break;

  case 101: /* explain_type: PIPELINE  */
#line 945 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3880 "parser.cpp"
    break;

  case 102: /* explain_type: FRAGMENT  */
#line 948 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3888 "parser.cpp"
    break;

  case 103: /* explain_type: %empty  */
#line 951 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3896 "parser.cpp"
    break;

  case 104: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 958 "parser.y"
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
#line 3913 "parser.cpp"
    break;

  case 105: /* update_expr_array: update_expr  */
#line 971 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3922 "parser.cpp"
    break;

  case 106: /* update_expr_array: update_expr_array ',' update_expr  */
#line 975 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3931 "parser.cpp"
    break;

  case 107: /* update_expr: IDENTIFIER '=' expr  */
#line 980 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3943 "parser.cpp"
    break;

  case 108: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 993 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3959 "parser.cpp"
    break;

  case 109: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1006 "parser.y"
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
#line 3977 "parser.cpp"
    break;

  case 110: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1021 "parser.y"
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
#line 3995 "parser.cpp"
    break;

  case 111: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1036 "parser.y"
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
#line 4013 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1051 "parser.y"
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
#line 4036 "parser.cpp"
    break;

  case 113: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1074 "parser.y"
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
#line 4082 "parser.cpp"
    break;

  case 114: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1115 "parser.y"
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
#line 4128 "parser.cpp"
    break;

  case 115: /* select_statement: select_without_paren  */
#line 1160 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4136 "parser.cpp"
    break;

  case 116: /* select_statement: select_with_paren  */
#line 1163 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4144 "parser.cpp"
    break;

  case 117: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1166 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4158 "parser.cpp"
    break;

  case 118: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1175 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4172 "parser.cpp"
    break;

  case 119: /* select_with_paren: '(' select_without_paren ')'  */
#line 1185 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4180 "parser.cpp"
    break;

  case 120: /* select_with_paren: '(' select_with_paren ')'  */
#line 1188 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4188 "parser.cpp"
    break;

  case 121: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1192 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4197 "parser.cpp"
    break;

  case 122: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1197 "parser.y"
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
#line 4223 "parser.cpp"
    break;

  case 123: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1219 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4231 "parser.cpp"
    break;

  case 124: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1222 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4239 "parser.cpp"
    break;

  case 125: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1227 "parser.y"
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
#line 4259 "parser.cpp"
    break;

  case 126: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1243 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4267 "parser.cpp"
    break;

  case 127: /* order_by_clause: %empty  */
#line 1246 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4275 "parser.cpp"
    break;

  case 128: /* order_by_expr_list: order_by_expr  */
#line 1250 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4284 "parser.cpp"
    break;

  case 129: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1254 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4293 "parser.cpp"
    break;

  case 130: /* order_by_expr: expr order_by_type  */
#line 1259 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4303 "parser.cpp"
    break;

  case 131: /* order_by_type: ASC  */
#line 1265 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4311 "parser.cpp"
    break;

  case 132: /* order_by_type: DESC  */
#line 1268 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4319 "parser.cpp"
    break;

  case 133: /* order_by_type: %empty  */
#line 1271 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4327 "parser.cpp"
    break;

  case 134: /* limit_expr: LIMIT expr  */
#line 1275 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4335 "parser.cpp"
    break;

  case 135: /* limit_expr: %empty  */
#line 1279 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4341 "parser.cpp"
    break;

  case 136: /* offset_expr: OFFSET expr  */
#line 1281 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4349 "parser.cpp"
    break;

  case 137: /* offset_expr: %empty  */
#line 1285 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4355 "parser.cpp"
    break;

  case 138: /* distinct: DISTINCT  */
#line 1287 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4363 "parser.cpp"
    break;

  case 139: /* distinct: %empty  */
#line 1290 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4371 "parser.cpp"
    break;

  case 140: /* from_clause: FROM table_reference  */
#line 1294 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4379 "parser.cpp"
    break;

  case 141: /* from_clause: %empty  */
#line 1297 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4387 "parser.cpp"
    break;

  case 142: /* search_clause: SEARCH sub_search_array  */
#line 1301 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4397 "parser.cpp"
    break;

  case 143: /* search_clause: %empty  */
#line 1306 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4405 "parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1310 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4413 "parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1313 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4421 "parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1317 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4429 "parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1320 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4437 "parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1324 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4445 "parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1327 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4453 "parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1331 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4461 "parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1334 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4469 "parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1337 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4477 "parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1340 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4485 "parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1348 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4493 "parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1351 "parser.y"
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
#line 4511 "parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1368 "parser.y"
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
#line 4529 "parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1382 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4540 "parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1391 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4550 "parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1396 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4562 "parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1405 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4572 "parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1410 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4582 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1415 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4593 "parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1421 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4601 "parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1428 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4609 "parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1431 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4617 "parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1435 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4626 "parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1438 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4635 "parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1443 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4647 "parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1455 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4659 "parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1462 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4672 "parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1476 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4680 "parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1479 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4688 "parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1482 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4696 "parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1485 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4704 "parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1488 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4712 "parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1491 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4720 "parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1494 "parser.y"
                {
}
#line 4727 "parser.cpp"
    break;

  case 180: /* show_statement: SHOW DATABASES  */
#line 1500 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4736 "parser.cpp"
    break;

  case 181: /* show_statement: SHOW TABLES  */
#line 1504 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4745 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW VIEWS  */
#line 1508 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4754 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW CONFIGS  */
#line 1512 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4763 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW PROFILES  */
#line 1516 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4772 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW SESSION STATUS  */
#line 1520 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4781 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW GLOBAL STATUS  */
#line 1524 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4790 "parser.cpp"
    break;

  case 187: /* show_statement: SHOW VAR IDENTIFIER  */
#line 1528 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kVar;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4802 "parser.cpp"
    break;

  case 188: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1535 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4813 "parser.cpp"
    break;

  case 189: /* show_statement: SHOW TABLE table_name  */
#line 1541 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTable;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4829 "parser.cpp"
    break;

  case 190: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1552 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kColumns;
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);
}
#line 4845 "parser.cpp"
    break;

  case 191: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1563 "parser.y"
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
#line 4861 "parser.cpp"
    break;

  case 192: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1574 "parser.y"
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
#line 4878 "parser.cpp"
    break;

  case 193: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1586 "parser.y"
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
#line 4896 "parser.cpp"
    break;

  case 194: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1599 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexes;
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);
}
#line 4912 "parser.cpp"
    break;

  case 195: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1610 "parser.y"
                                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndex;
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);

    (yyval.show_stmt)->index_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4931 "parser.cpp"
    break;

  case 196: /* flush_statement: FLUSH DATA  */
#line 1628 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4940 "parser.cpp"
    break;

  case 197: /* flush_statement: FLUSH LOG  */
#line 1632 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4949 "parser.cpp"
    break;

  case 198: /* flush_statement: FLUSH BUFFER  */
#line 1636 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4958 "parser.cpp"
    break;

  case 199: /* optimize_statement: OPTIMIZE table_name  */
#line 1644 "parser.y"
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
#line 4974 "parser.cpp"
    break;

  case 200: /* command_statement: USE IDENTIFIER  */
#line 1659 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4985 "parser.cpp"
    break;

  case 201: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1665 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4995 "parser.cpp"
    break;

  case 202: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1670 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5006 "parser.cpp"
    break;

  case 203: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1676 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5017 "parser.cpp"
    break;

  case 204: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1682 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5030 "parser.cpp"
    break;

  case 205: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1690 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5041 "parser.cpp"
    break;

  case 206: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1696 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5052 "parser.cpp"
    break;

  case 207: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1702 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5063 "parser.cpp"
    break;

  case 208: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1708 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5074 "parser.cpp"
    break;

  case 209: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1714 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5087 "parser.cpp"
    break;

  case 210: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1722 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5098 "parser.cpp"
    break;

  case 211: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1728 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5109 "parser.cpp"
    break;

  case 212: /* command_statement: COMPACT TABLE table_name  */
#line 1734 "parser.y"
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
#line 5125 "parser.cpp"
    break;

  case 213: /* expr_array: expr_alias  */
#line 1750 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5134 "parser.cpp"
    break;

  case 214: /* expr_array: expr_array ',' expr_alias  */
#line 1754 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5143 "parser.cpp"
    break;

  case 215: /* expr_array_list: '(' expr_array ')'  */
#line 1759 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5152 "parser.cpp"
    break;

  case 216: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1763 "parser.y"
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
#line 5172 "parser.cpp"
    break;

  case 217: /* expr_alias: expr AS IDENTIFIER  */
#line 1790 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5183 "parser.cpp"
    break;

  case 218: /* expr_alias: expr  */
#line 1796 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5191 "parser.cpp"
    break;

  case 224: /* operand: '(' expr ')'  */
#line 1806 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5199 "parser.cpp"
    break;

  case 225: /* operand: '(' select_without_paren ')'  */
#line 1809 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5210 "parser.cpp"
    break;

  case 226: /* operand: constant_expr  */
#line 1815 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5218 "parser.cpp"
    break;

  case 235: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1827 "parser.y"
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
#line 5389 "parser.cpp"
    break;

  case 236: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1994 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5402 "parser.cpp"
    break;

  case 237: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 2002 "parser.y"
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
#line 5417 "parser.cpp"
    break;

  case 238: /* query_expr: QUERY '(' STRING ')'  */
#line 2013 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5428 "parser.cpp"
    break;

  case 239: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2019 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5441 "parser.cpp"
    break;

  case 240: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2028 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5452 "parser.cpp"
    break;

  case 241: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2034 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5465 "parser.cpp"
    break;

  case 242: /* sub_search_array: knn_expr  */
#line 2044 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5474 "parser.cpp"
    break;

  case 243: /* sub_search_array: match_expr  */
#line 2048 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5483 "parser.cpp"
    break;

  case 244: /* sub_search_array: query_expr  */
#line 2052 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5492 "parser.cpp"
    break;

  case 245: /* sub_search_array: fusion_expr  */
#line 2056 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5501 "parser.cpp"
    break;

  case 246: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 2060 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5510 "parser.cpp"
    break;

  case 247: /* sub_search_array: sub_search_array ',' match_expr  */
#line 2064 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5519 "parser.cpp"
    break;

  case 248: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2068 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5528 "parser.cpp"
    break;

  case 249: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2072 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5537 "parser.cpp"
    break;

  case 250: /* function_expr: IDENTIFIER '(' ')'  */
#line 2077 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5550 "parser.cpp"
    break;

  case 251: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2085 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5563 "parser.cpp"
    break;

  case 252: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2093 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5577 "parser.cpp"
    break;

  case 253: /* function_expr: operand IS NOT NULLABLE  */
#line 2102 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5589 "parser.cpp"
    break;

  case 254: /* function_expr: operand IS NULLABLE  */
#line 2109 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5601 "parser.cpp"
    break;

  case 255: /* function_expr: NOT operand  */
#line 2116 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5613 "parser.cpp"
    break;

  case 256: /* function_expr: '-' operand  */
#line 2123 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5625 "parser.cpp"
    break;

  case 257: /* function_expr: '+' operand  */
#line 2130 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5637 "parser.cpp"
    break;

  case 258: /* function_expr: operand '-' operand  */
#line 2137 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5650 "parser.cpp"
    break;

  case 259: /* function_expr: operand '+' operand  */
#line 2145 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5663 "parser.cpp"
    break;

  case 260: /* function_expr: operand '*' operand  */
#line 2153 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5676 "parser.cpp"
    break;

  case 261: /* function_expr: operand '/' operand  */
#line 2161 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5689 "parser.cpp"
    break;

  case 262: /* function_expr: operand '%' operand  */
#line 2169 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5702 "parser.cpp"
    break;

  case 263: /* function_expr: operand '=' operand  */
#line 2177 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5715 "parser.cpp"
    break;

  case 264: /* function_expr: operand EQUAL operand  */
#line 2185 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5728 "parser.cpp"
    break;

  case 265: /* function_expr: operand NOT_EQ operand  */
#line 2193 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5741 "parser.cpp"
    break;

  case 266: /* function_expr: operand '<' operand  */
#line 2201 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5754 "parser.cpp"
    break;

  case 267: /* function_expr: operand '>' operand  */
#line 2209 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5767 "parser.cpp"
    break;

  case 268: /* function_expr: operand LESS_EQ operand  */
#line 2217 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5780 "parser.cpp"
    break;

  case 269: /* function_expr: operand GREATER_EQ operand  */
#line 2225 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5793 "parser.cpp"
    break;

  case 270: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2233 "parser.y"
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
#line 5828 "parser.cpp"
    break;

  case 271: /* function_expr: operand LIKE operand  */
#line 2263 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5841 "parser.cpp"
    break;

  case 272: /* function_expr: operand NOT LIKE operand  */
#line 2271 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5854 "parser.cpp"
    break;

  case 273: /* conjunction_expr: expr AND expr  */
#line 2280 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5867 "parser.cpp"
    break;

  case 274: /* conjunction_expr: expr OR expr  */
#line 2288 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5880 "parser.cpp"
    break;

  case 275: /* between_expr: operand BETWEEN operand AND operand  */
#line 2297 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5892 "parser.cpp"
    break;

  case 276: /* in_expr: operand IN '(' expr_array ')'  */
#line 2305 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5903 "parser.cpp"
    break;

  case 277: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2311 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5914 "parser.cpp"
    break;

  case 278: /* case_expr: CASE expr case_check_array END  */
#line 2318 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5925 "parser.cpp"
    break;

  case 279: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2324 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5937 "parser.cpp"
    break;

  case 280: /* case_expr: CASE case_check_array END  */
#line 2331 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5947 "parser.cpp"
    break;

  case 281: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2336 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5958 "parser.cpp"
    break;

  case 282: /* case_check_array: WHEN expr THEN expr  */
#line 2343 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5970 "parser.cpp"
    break;

  case 283: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2350 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5982 "parser.cpp"
    break;

  case 284: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2358 "parser.y"
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
#line 6010 "parser.cpp"
    break;

  case 285: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2382 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6021 "parser.cpp"
    break;

  case 286: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2388 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6032 "parser.cpp"
    break;

  case 287: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2394 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6044 "parser.cpp"
    break;

  case 288: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2401 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6056 "parser.cpp"
    break;

  case 289: /* column_expr: IDENTIFIER  */
#line 2409 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6068 "parser.cpp"
    break;

  case 290: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2416 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6080 "parser.cpp"
    break;

  case 291: /* column_expr: '*'  */
#line 2423 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6090 "parser.cpp"
    break;

  case 292: /* column_expr: column_expr '.' '*'  */
#line 2428 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6104 "parser.cpp"
    break;

  case 293: /* constant_expr: STRING  */
#line 2438 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6114 "parser.cpp"
    break;

  case 294: /* constant_expr: TRUE  */
#line 2443 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6124 "parser.cpp"
    break;

  case 295: /* constant_expr: FALSE  */
#line 2448 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6134 "parser.cpp"
    break;

  case 296: /* constant_expr: DOUBLE_VALUE  */
#line 2453 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6144 "parser.cpp"
    break;

  case 297: /* constant_expr: LONG_VALUE  */
#line 2458 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6154 "parser.cpp"
    break;

  case 298: /* constant_expr: DATE STRING  */
#line 2463 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6164 "parser.cpp"
    break;

  case 299: /* constant_expr: TIME STRING  */
#line 2468 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6174 "parser.cpp"
    break;

  case 300: /* constant_expr: DATETIME STRING  */
#line 2473 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6184 "parser.cpp"
    break;

  case 301: /* constant_expr: TIMESTAMP STRING  */
#line 2478 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6194 "parser.cpp"
    break;

  case 302: /* constant_expr: INTERVAL interval_expr  */
#line 2483 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6202 "parser.cpp"
    break;

  case 303: /* constant_expr: interval_expr  */
#line 2486 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6210 "parser.cpp"
    break;

  case 304: /* constant_expr: long_array_expr  */
#line 2489 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6218 "parser.cpp"
    break;

  case 305: /* constant_expr: double_array_expr  */
#line 2492 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6226 "parser.cpp"
    break;

  case 306: /* array_expr: long_array_expr  */
#line 2496 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6234 "parser.cpp"
    break;

  case 307: /* array_expr: double_array_expr  */
#line 2499 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6242 "parser.cpp"
    break;

  case 308: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2503 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6250 "parser.cpp"
    break;

  case 309: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2507 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6260 "parser.cpp"
    break;

  case 310: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2512 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6269 "parser.cpp"
    break;

  case 311: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2517 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6277 "parser.cpp"
    break;

  case 312: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2521 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6287 "parser.cpp"
    break;

  case 313: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2526 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6296 "parser.cpp"
    break;

  case 314: /* interval_expr: LONG_VALUE SECONDS  */
#line 2531 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6307 "parser.cpp"
    break;

  case 315: /* interval_expr: LONG_VALUE SECOND  */
#line 2537 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6318 "parser.cpp"
    break;

  case 316: /* interval_expr: LONG_VALUE MINUTES  */
#line 2543 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6329 "parser.cpp"
    break;

  case 317: /* interval_expr: LONG_VALUE MINUTE  */
#line 2549 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6340 "parser.cpp"
    break;

  case 318: /* interval_expr: LONG_VALUE HOURS  */
#line 2555 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6351 "parser.cpp"
    break;

  case 319: /* interval_expr: LONG_VALUE HOUR  */
#line 2561 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6362 "parser.cpp"
    break;

  case 320: /* interval_expr: LONG_VALUE DAYS  */
#line 2567 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6373 "parser.cpp"
    break;

  case 321: /* interval_expr: LONG_VALUE DAY  */
#line 2573 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6384 "parser.cpp"
    break;

  case 322: /* interval_expr: LONG_VALUE MONTHS  */
#line 2579 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6395 "parser.cpp"
    break;

  case 323: /* interval_expr: LONG_VALUE MONTH  */
#line 2585 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6406 "parser.cpp"
    break;

  case 324: /* interval_expr: LONG_VALUE YEARS  */
#line 2591 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6417 "parser.cpp"
    break;

  case 325: /* interval_expr: LONG_VALUE YEAR  */
#line 2597 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6428 "parser.cpp"
    break;

  case 326: /* copy_option_list: copy_option  */
#line 2608 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6437 "parser.cpp"
    break;

  case 327: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2612 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6446 "parser.cpp"
    break;

  case 328: /* copy_option: FORMAT IDENTIFIER  */
#line 2617 "parser.y"
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
#line 6473 "parser.cpp"
    break;

  case 329: /* copy_option: DELIMITER STRING  */
#line 2639 "parser.y"
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
#line 6488 "parser.cpp"
    break;

  case 330: /* copy_option: HEADER  */
#line 2649 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6498 "parser.cpp"
    break;

  case 331: /* file_path: STRING  */
#line 2655 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6506 "parser.cpp"
    break;

  case 332: /* if_exists: IF EXISTS  */
#line 2659 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6512 "parser.cpp"
    break;

  case 333: /* if_exists: %empty  */
#line 2660 "parser.y"
  { (yyval.bool_value) = false; }
#line 6518 "parser.cpp"
    break;

  case 334: /* if_not_exists: IF NOT EXISTS  */
#line 2662 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6524 "parser.cpp"
    break;

  case 335: /* if_not_exists: %empty  */
#line 2663 "parser.y"
  { (yyval.bool_value) = false; }
#line 6530 "parser.cpp"
    break;

  case 338: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2678 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6543 "parser.cpp"
    break;

  case 339: /* if_not_exists_info: %empty  */
#line 2686 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6551 "parser.cpp"
    break;

  case 340: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2690 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6559 "parser.cpp"
    break;

  case 341: /* with_index_param_list: %empty  */
#line 2693 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6567 "parser.cpp"
    break;

  case 342: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2697 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 6575 "parser.cpp"
    break;

  case 343: /* optional_table_properties_list: %empty  */
#line 2700 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 6583 "parser.cpp"
    break;

  case 344: /* index_param_list: index_param  */
#line 2704 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6592 "parser.cpp"
    break;

  case 345: /* index_param_list: index_param_list ',' index_param  */
#line 2708 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6601 "parser.cpp"
    break;

  case 346: /* index_param: IDENTIFIER  */
#line 2713 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6611 "parser.cpp"
    break;

  case 347: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2718 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6624 "parser.cpp"
    break;

  case 348: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2726 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6636 "parser.cpp"
    break;

  case 349: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2733 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6648 "parser.cpp"
    break;

  case 350: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2744 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
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
#line 6701 "parser.cpp"
    break;

  case 351: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2792 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
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
#line 6755 "parser.cpp"
    break;

  case 352: /* index_info_list: '(' identifier_array ')'  */
#line 2841 "parser.y"
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
#line 6773 "parser.cpp"
    break;


#line 6777 "parser.cpp"

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

#line 2855 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}
